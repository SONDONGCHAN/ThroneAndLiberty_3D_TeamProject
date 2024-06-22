#include "Eagle.h"
#include "Polymorph.h"
#include "Model.h"
#include "RigidBody.h"
#include "EffectMgr.h"
#include "CameraMgr.h"
#include "CameraSpherical.h"
#include "CharacterMgr.h"
#include "QuestMgr.h"

CEagle::CEagle()
{
}

CEagle::~CEagle()
{
}

HRESULT CEagle::Initialize()
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 11.f;
	pArg->fRotationPerSec = XMConvertToRadians(60.0f);
	__super::Initialize(pArg.get());

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_ANIM;
	m_iShaderPass = 2;

	m_pModelCom = GAMEINSTANCE->GetModel("Eagle");
	AddComponent(L"Com_Model", m_pModelCom);
	AddRigidBody("Eagle");

	m_pCamera = dynamic_pointer_cast<CCameraSpherical>(CCameraMgr::GetInstance()->FindCamera("PlayerCamera"));

	m_pRigidBody->SimulationOff();

	MakeAnimationSet();

	m_pRigidBody->SimulationOff();

	return S_OK;
}

void CEagle::PriorityTick(_float _fTimeDelta)
{
	GravityControl(_fTimeDelta);

	PolymorphControl(_fTimeDelta);

	m_isPrevSprint = m_isSprint;
	m_isPrevDive = m_isDive;
}

void CEagle::Tick(_float _fTimeDelta)
{
	m_pRigidBody->SetLinearVelocity({ 0.f,0.f,0.f });

	if (!m_isChanged)
	{
		GAMEINSTANCE->PlaySoundL("Player_Eagle_Wind", 1.f);
		m_isDive = false;
		GAMEINSTANCE->PlaySoundW("Player_Eagle_Loar", 0.5f);
		ChangeAnim(ANIMAL_EAGLE_TRANS_START, 0.05f);
		m_pModelCom->PlayAnimation(0);
		SetDissolve(true, false, 2.f);
		SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
		m_isChanged = true;
		m_iStepEffectIdx = EFFECTMGR->PlayEffect(m_strStepEffectKey, shared_from_this());
	}
	CommonTick(_fTimeDelta);

	if (GAMEINSTANCE->KeyPressing(DIK_LCONTROL))
	{
		if (!m_isDive)
		{
			m_isSprint = false;
			ChangeAnim(ANIMAL_EAGLE_GLIDING_NORMAL_TO_DIVE, 0.1f);
			m_pModelCom->PlayAnimation(0);
			m_isDive = true;
			m_iDirection = 1;
			return;
		}

	}

	switch (m_eCurrentBaseState)
	{
	case STATE_LOOP:
		LoopStateFSM(_fTimeDelta);
		break;

	case STATE_ACTIVE:
		ActiveStateFSM(_fTimeDelta);
		break;

	default:
		break;
	}

	m_pModelCom->PlayAnimation(_fTimeDelta);
	m_pTransformCom->Tick(_fTimeDelta);

	UpdateLightDesc();
}

void CEagle::LateTick(_float _fTimeDelta)
{
	EffectControl();

	if (m_bDslv)
		UpdateDslv(_fTimeDelta);

	m_pTransformCom->LateTick(_fTimeDelta);
	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
	GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());
	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_EDGE, shared_from_this());

#ifdef _DEBUG
	if (m_pRigidBody)
		GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG

}

HRESULT CEagle::Render()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	if (m_bDslv)
	{
		if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &m_bDslv, sizeof(_bool))))
			return E_FAIL;

		_float g_fDslvValue = m_fDslvTime / m_fMaxDslvTime;


		if (FAILED(GAMEINSTANCE->BindRawValue("g_fDslvValue", &g_fDslvValue, sizeof(_float))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_fDslvThick", &m_fDslvThick, sizeof(_float))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_vDslvColor", &m_vDslvColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindSRV("g_DslvTexture", m_strDslvTexKey)))
			return E_FAIL;
	}

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		GAMEINSTANCE->BeginAnimModel(m_pModelCom, i);
	}

	_bool bDslvFalse = false;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &bDslvFalse, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

void CEagle::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.eColFlag == COL_STATIC)
	{
		if (((_ColData.szShapeTag == "HeightField") || (_ColData.szShapeTag == "Static_Collider") || (_ColData.szShapeTag == "Pillar_Body")) && (_szMyShapeTag == "Land_Check"))
		{
			CancleAnimationSet();

			if (m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_DIVE)
				m_iLanded = 2;
			else
				m_iLanded = 1;
		}

		return;
	}
}

void CEagle::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{

}

void CEagle::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CEagle::EnableCollision(const char* _strShapeTag)
{
	__super::EnableCollision(_strShapeTag);
}

void CEagle::DisableCollision(const char* _strShapeTag)
{
	__super::DisableCollision(_strShapeTag);
}

void CEagle::PlayEffect(string _strGroupID)
{
	EFFECTMGR->PlayEffect(_strGroupID, shared_from_this());
}

HRESULT CEagle::RenderShadow()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 3);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
	
	GAMEINSTANCE->BindLightProjMatrix();
	
	_uint iNumMeshes = m_pModelCom->GetNumMeshes();
	
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		GAMEINSTANCE->BeginAnimModel(m_pModelCom, i);
	}
	
	return S_OK;
}

void CEagle::LoopStateFSM(_float _fTimeDelta)
{
	if (PlayAnimationSet())
		return;

	if (DiveControl(_fTimeDelta))
		return;

	if (EmotionControl())
		return;

	if (DirectionControl(_fTimeDelta))
		return;
	
	if (SprintControl(_fTimeDelta))
		return;

	if (MoveControl(_fTimeDelta))
		return;

}

void CEagle::ActiveStateFSM(_float _fTimeDelta)
{
	if (m_pModelCom->GetCanCancelAnimation())
	{
		if (PlayAnimationSet())
			return;
	}
	if (m_pModelCom->GetIsFinishedAnimation())
	{
		m_eCurrentBaseState = STATE_LOOP;
		LoopStateFSM(_fTimeDelta);
		return;
	}
	if (m_pModelCom->GetCanCancelAnimation() && MoveControl(_fTimeDelta, false) )
	{
		m_eCurrentBaseState = STATE_LOOP;
		return;
	}
}

_bool CEagle::MoveControl(_float _fTimeDelta, _bool _bMove)
{
	if(!_bMove)
		return true;

	_bool isAscend	= false;
	 
	if (GAMEINSTANCE->KeyDown(DIK_E))
		isAscend = true;

	if (GAMEINSTANCE->KeyPressing(DIK_A) || GAMEINSTANCE->KeyPressing(DIK_D))
	{
		if (GAMEINSTANCE->KeyPressing(DIK_A))
		{
			m_iDirection = 0;
			m_pTransformCom->Turn(_float3(0.f, -1.f, 0.f), _fTimeDelta);

			if (isAscend)
			{
				m_fCurrentGravity = m_fUpGravity;

				if (m_isSprint)
					ChangeAnim(ANIMAL_EAGLE_GLIDING_FAST_L_UP, 0.05f);
				else
					ChangeAnim(ANIMAL_EAGLE_GLIDING_NORMAL_L_UP, 0.05f);
			}
			else
			{
				if (m_isSprint)
					ChangeAnim(ANIMAL_EAGLE_GLIDING_FAST_L, 0.05f, true);
				else
					ChangeAnim(ANIMAL_EAGLE_GLIDING_NORMAL_L, 0.05f, true);
			}
		}
		else if (GAMEINSTANCE->KeyPressing(DIK_D))
		{
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), _fTimeDelta);
			m_iDirection = 2;

			if (isAscend)
			{
				m_fCurrentGravity = m_fUpGravity;

				if (m_isSprint)
					ChangeAnim(ANIMAL_EAGLE_GLIDING_FAST_R_UP, 0.05f);
				else
					ChangeAnim(ANIMAL_EAGLE_GLIDING_NORMAL_R_UP, 0.05f);
			}
			else
			{
				if (m_isSprint)
					ChangeAnim(ANIMAL_EAGLE_GLIDING_FAST_R, 0.05f, true);
				else
					ChangeAnim(ANIMAL_EAGLE_GLIDING_NORMAL_R, 0.05f, true);
			}
		}
	}
	
	else
	{
		m_iDirection = 1;

		if (isAscend)
		{
			m_fCurrentGravity = m_fUpGravity;

			if (m_isSprint)
				ChangeAnim(ANIMAL_EAGLE_GLIDING_FAST_UP, 0.05f);
			else
				ChangeAnim(ANIMAL_EAGLE_GLIDING_NORMAL_UP, 0.05f);
		}
		else
		{
			if (m_isSprint)
				ChangeAnim(ANIMAL_EAGLE_GLIDING_FAST, 0.05f, true);
			else
				ChangeAnim(ANIMAL_EAGLE_GLIDING_NORMAL, 0.05f, true);
		}

	}

	return true;
}

_bool CEagle::EmotionControl()
{
	if (GAMEINSTANCE->KeyDown(DIK_1))
	{
		ChangeAnim(ANIMAL_EAGLE_EMOTION_1, 0.1f, false);
		return true;
	}
	return false;
}

_bool CEagle::SprintControl(_float _fTimeDelta)
{
	if (GAMEINSTANCE->KeyPressing(DIK_SPACE))
	{
		if ((!(m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_NORMAL_L_START ||
			   m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_NORMAL_L ||
			   m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_NORMAL_L_UP ||
			   m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_NORMAL_R_START ||
			   m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_NORMAL_R ||
			   m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_NORMAL_R_UP ||
			   m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_FAST_L_START ||
			   m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_FAST_L ||
			   m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_FAST_L_UP ||
			   m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_FAST_R_START ||
			   m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_FAST_R ||
			   m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_FAST_R_UP) ) 
			&& (!m_isSprint) )
		{
			ChangeAnim(ANIMAL_EAGLE_GLIDING_NORMAL_TO_FAST, 0.1f);
			m_fDiveFrontSpeed = 15.f;
			m_isSprint = true;
			return true;
		}
	}
	else 
	{
		if (m_isSprint)
		{
			ChangeAnim(ANIMAL_EAGLE_GLIDING_FAST_TO_NORMAL, 0.05f);
			m_fDiveFrontSpeed = 11.f;
			m_isSprint = false;
			m_iDirection = 1 ;
			return true;
		}
	}
	return false;
}

_bool CEagle::DiveControl(_float _fTimeDelta)
{
	if (GAMEINSTANCE->KeyPressing(DIK_LCONTROL))
	{
		if (m_isDive)
		{
			m_pTransformCom->GoDDownFreeSpeed(_fTimeDelta, 8.f);
			GAMEINSTANCE->SetZoomBlur(0.02f, 0.3f, m_pTransformCom->GetState(CTransform::STATE_POSITION));
			ChangeAnim(ANIMAL_EAGLE_GLIDING_DIVE, 0.05f, true);
			return true;
		}
	}
	else
	{
		if (m_isDive)
		{
			ChangeAnim(ANIMAL_EAGLE_GLIDING_DIVE_TO_NORMAL, 0.05f);
			m_isDive = false;
			return true;
		}
	}
	return false;
}

_bool CEagle::DirectionControl(_float _fTimeDelta)
{
	if (GAMEINSTANCE->KeyPressing(DIK_A))
	{
		if (m_iDirection != 0)
		{
			m_iDirection = 0;

			if (m_isSprint)
				ChangeAnim(ANIMAL_EAGLE_GLIDING_FAST_L_START, 0.05f);
			else
				ChangeAnim(ANIMAL_EAGLE_GLIDING_NORMAL_L_START, 0.05f);

			return true;

		}
		//m_pModelCom->PlayAnimation(0);
	}
	else if (GAMEINSTANCE->KeyPressing(DIK_D))
	{
		if (m_iDirection != 2)
		{
			m_iDirection = 2;

			if (m_isSprint)
				ChangeAnim(ANIMAL_EAGLE_GLIDING_FAST_R_START, 0.05f);
			else
				ChangeAnim(ANIMAL_EAGLE_GLIDING_NORMAL_R_START, 0.05f);

			return true;

		}
		//m_pModelCom->PlayAnimation(0);

	}
	else
	{
		if (m_iDirection != 1)
		{
			m_iDirection = 1;
			ChangeAnim(ANIMAL_EAGLE_TRANS_START, 0.1f);
			return true;

		}
	}

	return false;
}

void CEagle::GravityControl(_float _fTimeDelta)
{
	m_fCurrentGravity += (m_fGravityAccel * _fTimeDelta);
	
	if (m_fCurrentGravity >= 1.2f)
		m_fCurrentGravity = 1.2f;

	m_pTransformCom->GoDDownFreeSpeed(_fTimeDelta, m_fCurrentGravity);
}

void CEagle::EffectControl()
{
	if (m_isPrevSprint != m_isSprint)
	{
		if (m_isSprint)
		{
			EFFECTMGR->PlayEffect("Sprint", shared_from_this());
			EFFECTMGR->PlayEffect("Eagle_Feather", shared_from_this());
		}

		else
		{
			EFFECTMGR->StopEffect("Sprint", 0);
			EFFECTMGR->StopEffect("Eagle_Feather", 0);
		}
	}

	if (m_isPrevDive != m_isDive)
	{
		if (m_isDive)
		{
			EFFECTMGR->PlayEffect("Eagle_Speed_Dive", shared_from_this());
			EFFECTMGR->PlayEffect("Eagle_Feather_Dive", shared_from_this());
			EFFECTMGR->PlayEffect("Eagle_Leave_Dive", shared_from_this());
		}

		else
		{
			EFFECTMGR->StopEffect("Eagle_Speed_Dive", 0);
			EFFECTMGR->StopEffect("Eagle_Feather_Dive", 0);
			EFFECTMGR->StopEffect("Eagle_Leave_Dive", 0);
		}
	}
}

void CEagle::CommonTick(_float _fTimeDelta)
{
	if (!m_isDive)
	{
		if (m_isSprint)
		{
			if (m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_DIVE_TO_NORMAL)
			{
				m_fDiveFrontSpeed = GAMEINSTANCE->Lerp(m_fDiveFrontSpeed, 15.f, 0.05f);
				m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, m_fDiveFrontSpeed);
			}
			else
			{
				m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 15.f);
				GAMEINSTANCE->SetZoomBlur(0.02f, 0.3f, m_pTransformCom->GetState(CTransform::STATE_POSITION));
			}
		}
		else
		{
			if (m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_DIVE_TO_NORMAL)
			{
				m_fDiveFrontSpeed = GAMEINSTANCE->Lerp(m_fDiveFrontSpeed, 11.f, 0.05f);
				m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, m_fDiveFrontSpeed);
			}
			else
			{
				m_pTransformCom->GoStraight(_fTimeDelta);
				GAMEINSTANCE->SetZoomBlur(0.02f, 0.15f, m_pTransformCom->GetState(CTransform::STATE_POSITION));
			}
		}

		if (m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_NORMAL_L_UP ||
			m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_FAST_L_UP ||
			m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_NORMAL_L_START ||
			m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_FAST_L_START)
			m_pTransformCom->Turn(_float3(0.f, -1.f, 0.f), _fTimeDelta);

		else if (m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_NORMAL_R_UP ||
			m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_FAST_R_UP || 
			m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_NORMAL_R_START ||
			m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_FAST_R_START)
			m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), _fTimeDelta);
	}
	else
	{
		if (m_eCurrentAnimation == ANIMAL_EAGLE_GLIDING_NORMAL_TO_DIVE)
		{
			m_fDiveFrontSpeed = GAMEINSTANCE->Lerp(m_fDiveFrontSpeed, 0.f, 0.05f);
			m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, m_fDiveFrontSpeed);
		}
	
	}

}

void CEagle::ChangeAnim(_uint _iAnimIndex, _float _fChangingDelay, _bool _isLoop, _float _fCancleRatio)
{
	if (!_isLoop)
		m_eCurrentBaseState = STATE_ACTIVE;

	m_pModelCom->ChangeAnim(_iAnimIndex, _fChangingDelay, _isLoop, _fCancleRatio);
	m_eCurrentAnimation = (EAGLE_ANIMATION)_iAnimIndex;
}

_float3 CEagle::SetDirectionFromCamera(_float _fDgree, _bool _isSoft)
{
	_float3	CamDir = GAMEINSTANCE->GetCamLook();
	CamDir.y = 0.f;
	CamDir.Normalize();
	
	_float3 ResultDir;
	SimpleMath::Vector3::TransformNormal(CamDir, SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(_fDgree)), ResultDir);
	
	if (!_isSoft)
	{
		m_pTransformCom->LookAtDir(ResultDir);
		return ResultDir;
	}
	
	_float fRatio = { 1.f };
	if (0 > XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pTransformCom->GetState(CTransform::STATE_LOOK)), ResultDir)))
		fRatio = 0.499f;
	else
		fRatio = 0.1f;
	
	ResultDir = SimpleMath::Vector3::Lerp(XMVector3Normalize(m_pTransformCom->GetState(CTransform::STATE_LOOK)), ResultDir, fRatio);
	ResultDir.Normalize();
	
	m_pTransformCom->LookAtDir(ResultDir);
	
	return ResultDir;
}     

_bool CEagle::PlayAnimationSet()
{
	if (m_iSetTrigger != 0)
	{
		m_iAnimSetCount++;

		if (m_iAnimSetCount > m_vecAnimationSet.size() - 1)
		{
			m_iSetTrigger = 0;
			m_iAnimSetCount = 0;
			m_vecAnimationSet.clear();

			return false;
		}
		if ((m_iSetTrigger == 2) && (m_iAnimSetCount == (m_vecAnimationSet.size() - 1)))
			ChangeAnim(get<0>(m_vecAnimationSet[m_iAnimSetCount]), get<1>(m_vecAnimationSet[m_iAnimSetCount]), true, get<2>(m_vecAnimationSet[m_iAnimSetCount]));
		else
			ChangeAnim(get<0>(m_vecAnimationSet[m_iAnimSetCount]), get<1>(m_vecAnimationSet[m_iAnimSetCount]), false, get<2>(m_vecAnimationSet[m_iAnimSetCount]));

		return true;
	}
	return false;
}

void CEagle::CancleAnimationSet()
{
	m_iSetTrigger = 0;
	m_iAnimSetCount = 0;
	m_vecAnimationSet.clear();
}

void CEagle::TriggerAnimationSet(string _strSetName, _uint _SetFlag)
{
	m_iSetTrigger = _SetFlag;
	m_vecAnimationSet = m_AnimationSets.find(_strSetName)->second;
	ChangeAnim(get<0>(m_vecAnimationSet[0]), get<1>(m_vecAnimationSet[0]), false, get<2>(m_vecAnimationSet[0]));
}

void CEagle::PolymorphControl(_float _fTimeDelta)
{
	// Landing
	if (m_iLanded == 1 && (!m_bChanging))
	{
		ChangeAnim(ANIMAL_EAGLE_TRANS_LAND_END, 0.1f, false);
		//TriggerAnimationSet("Land_Normal",1);		
		SetDissolve(true, true, 0.45f);
		SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
		m_bChanging = true;
		EFFECTMGR->StopEffect(m_strStepEffectKey, m_iStepEffectIdx);
	}
	else if (m_iLanded == 2 && (!m_bChanging))
	{
		ChangeAnim(ANIMAL_EAGLE_TRANS_LAND_DIVE, 0.05f, false);
		m_pModelCom->PlayAnimation(0);
		SetDissolve(true, true, 0.45f);
		SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
		m_bChanging = true;
		EFFECTMGR->StopEffect(m_strStepEffectKey, m_iStepEffectIdx);
	}

	// Change to Human
	if (GAMEINSTANCE->KeyDown(DIK_LSHIFT) && m_isChanged && (!m_bChanging))
	{
		SetDissolve(true, true, 0.45f);
		SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
		m_bChanging = true;
		EFFECTMGR->StopEffect(m_strStepEffectKey, m_iStepEffectIdx);
		return;
	}
	if (true == m_bChanging)
	{
		m_fChangeingTime += _fTimeDelta;

		if (0.5f <= m_fChangeingTime)
		{
			m_iLanded = 0;
			m_fChangeingTime = 0.f;
			m_isChanged = false;
			m_bChanging = false;
			SetDissolve(false, false, 0.0f);
			CHARACTERMGR->ChangePolymorph("Form_Human");		
			GAMEINSTANCE->StopSound("Player_Eagle_Wind");
		}
	}
}

HRESULT CEagle::AddRigidBody(string _strModelKey)
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	//==본체==
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL/*| CRigidBody::LOCK_POS_X | CRigidBody::LOCK_POS_Z*/;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestitution = 0.0f;

	GeometryBox BoxDesc;
	//CapsuleDesc.fHeight = 1.f;
	//CapsuleDesc.fRadius = 0.25f;
	//CapsuleDesc.vOffSetPosition = { 0.0f, 0.25f, 0.0f };
	//CapsuleDesc.vOffSetRotation = { 0.0f, 90.0f, 0.0f };
	BoxDesc.vSize = _float3{ 1.f, 1.f, 2.f };
	BoxDesc.vOffSetPosition = { 0.0f, 0.5f, 0.0f };
	BoxDesc.strShapeTag = "Eagle_Body";
	RigidBodyDesc.pGeometry = &BoxDesc;

	RigidBodyDesc.eThisColFlag = COL_PLAYER;
	RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_TRIGGER | COL_INTERACTIONOBJ;
	m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

	m_pTransformCom->SetRigidBody(m_pRigidBody);

	//==피직스 착지판정 체크용==
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.isInitCol = true;
	BoxDesc.vSize = _float3{ 0.05f, 0.6f, 0.05f };
	BoxDesc.vOffSetPosition = { 0.f, 0.f, 0.0f };
	BoxDesc.strShapeTag = "Land_Check";
	RigidBodyDesc.pGeometry = &BoxDesc;

	RigidBodyDesc.eThisColFlag = COL_PLAYER;
	RigidBodyDesc.eTargetColFlag = COL_STATIC;
	m_pRigidBody->CreateShape(&RigidBodyDesc);

	return S_OK;
}

HRESULT CEagle::MakeAnimationSet()
{
	// Normal_Land=========================================================================
	vector< tuple<EAGLE_ANIMATION, _float, _float> > vecLandEagleNormal;

	tuple<EAGLE_ANIMATION, _float, _float> tpLandEagelStart(ANIMAL_EAGLE_TRANS_LAND_START, 0.05f, 1.f);
	vecLandEagleNormal.push_back(tpLandEagelStart);
	tuple<EAGLE_ANIMATION, _float, _float> tpLandEagelEnd(ANIMAL_EAGLE_TRANS_LAND_END, 0.05f, 1.f);
	vecLandEagleNormal.push_back(tpLandEagelEnd);

	m_AnimationSets.emplace("Land_Normal", vecLandEagleNormal);
	// =================================================================================

	return S_OK;
}

shared_ptr<CEagle> CEagle::Create()
{
	shared_ptr<CEagle> pInstance = make_shared<CEagle>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CEagle");

	return pInstance;
}