#include "Wolf.h"
#include "Polymorph.h"
#include "Model.h"
#include "RigidBody.h"
#include "EffectMgr.h"
#include "CameraMgr.h"
#include "CameraSpherical.h"
#include "CharacterMgr.h"
#include "QuestMgr.h"

CWolf::CWolf()
{
}

CWolf::~CWolf()
{
}

HRESULT CWolf::Initialize()
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 8.f;
	pArg->fRotationPerSec = XMConvertToRadians(90.0f);
	__super::Initialize(pArg.get());

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_ANIM;
	m_iShaderPass = 2;

	m_pModelCom = GAMEINSTANCE->GetModel("Wolf_2");
	AddComponent(L"Com_Model", m_pModelCom);
	ChangeAnim(ANIMAL_WOLF_IDLE, 0.1f, true);

	//m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3{ 9.f, 5.f, 10.f });
	AddRigidBody("Wolf_2");

	m_pCamera = dynamic_pointer_cast<CCameraSpherical>(CCameraMgr::GetInstance()->FindCamera("PlayerCamera"));

	m_pRigidBody->SimulationOff();

	MakeAnimationSet();

	m_pRigidBody->SimulationOff();

	return S_OK;
}

void CWolf::PriorityTick(_float _fTimeDelta)
{
	PolymorphControl(_fTimeDelta);

	//if (GAMEINSTANCE->KeyDown(DIK_COMMA))
	//	m_iTestKeyInput++;
}

void CWolf::Tick(_float _fTimeDelta)
{
	if (!m_isChanged)
	{
		GAMEINSTANCE->PlaySoundW ("Player_Poly_to_Wolf", 0.4f);
		SetDissolve(true, false, 0.8f);
		SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
		m_isChanged = true;
		m_isMoveState = false;
	}

	CommonTick(_fTimeDelta);

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

void CWolf::LateTick(_float _fTimeDelta)
{
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

HRESULT CWolf::Render()
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

void CWolf::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.eColFlag == COL_STATIC)
	{
		if (((_ColData.szShapeTag == "HeightField") || (_ColData.szShapeTag == "Static_Collider") || (_ColData.szShapeTag == "Pillar_Body")) && (_szMyShapeTag == "Land_Check") && m_isJumpState && (!m_isJumpMinActive))
		{
			CancleAnimationSet();

			if(m_iJumpFlag == 0)
				ChangeAnim(ANIMAL_WOLF_JUMP_END, 0.05f, false, 0.1f);
			else if(m_iJumpFlag ==1 || m_iJumpFlag == 2)
				ChangeAnim(ANIMAL_WOLF_JUMP_END_RUN, 0.05f, false, 0.25f);

			m_isJumpState = false;
			m_isLanding = true;
			m_iJumpFlag = -1;
			m_pModelCom->PlayAnimation(0.f);
		}

		return;
	}
}

void CWolf::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.eColFlag == COL_STATIC)
	{
		if (((_ColData.szShapeTag == "HeightField") || (_ColData.szShapeTag == "Static_Collider") || (_ColData.szShapeTag == "Pillar_Body")) && (_szMyShapeTag == "Jump_Check"))
		{
			m_isLanding = true;
		}
	
		else if (((_ColData.szShapeTag == "HeightField") || (_ColData.szShapeTag == "Static_Collider") || (_ColData.szShapeTag == "Pillar_Body")) && (_szMyShapeTag == "Land_Check"))
		{
			m_isLanding = true;
		}
	}
}

void CWolf::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CWolf::EnableCollision(const char* _strShapeTag)
{
	__super::EnableCollision(_strShapeTag);
}

void CWolf::DisableCollision(const char* _strShapeTag)
{
	__super::DisableCollision(_strShapeTag);
}

void CWolf::PlayEffect(string _strGroupID)
{
	EFFECTMGR->PlayEffect(_strGroupID, shared_from_this());
}

HRESULT CWolf::RenderShadow()
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

void CWolf::LoopStateFSM(_float _fTimeDelta)
{
	if (PlayAnimationSet())
		return;

	if (EmotionControl())
		return;

	if (MoveControl(_fTimeDelta))
	{
		if (m_iJumpFlag != -1)
		{
			m_isMoveState = false;
			EFFECTMGR->StopEffect(m_strStepEffectKey, 0);
			return;
		}
	
		if (!m_isMoveState)
		{
			m_isMoveState = true;
			EFFECTMGR->StopEffect(m_strStepEffectKey, 0);

			m_iStepEffectIdx = EFFECTMGR->PlayEffect(m_strStepEffectKey, shared_from_this());
		}

		return;
	}
	else
	{
		if (m_iJumpFlag != -1)
		{
			m_isMoveState = false;
			EFFECTMGR->StopEffect(m_strStepEffectKey, 0);
			return;
		}

		if (m_isMoveState)
		{
			m_isMoveState = false;
			EFFECTMGR->StopEffect(m_strStepEffectKey, 0);
		}

		return;
	}

}

void CWolf::ActiveStateFSM(_float _fTimeDelta)
{
	if (m_pModelCom->GetCanCancelAnimation())
	{
		if (PlayAnimationSet())
			return;
	}
	if (m_pModelCom->GetIsFinishedAnimation())
	{
		if (m_isJumpState)
			return;

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

_bool CWolf::MoveControl(_float _fTimeDelta, _bool _bMove)
{
	_bool isMove	= false;
	_bool isSprint	= false;
	_bool isJump	= false;

	if (GAMEINSTANCE->KeyPressing(DIK_W) || GAMEINSTANCE->KeyPressing(DIK_A) || GAMEINSTANCE->KeyPressing(DIK_S) || GAMEINSTANCE->KeyPressing(DIK_D))
	{
		isMove = true;
		
		if (!_bMove)
		 	return isMove;

		if (GAMEINSTANCE->KeyDown(DIK_E))
			isJump = true;
		
		_float fDgree = 0.f;
		
		if (GAMEINSTANCE->KeyPressing(DIK_W) && GAMEINSTANCE->KeyPressing(DIK_A))
			fDgree = 315.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_W) && GAMEINSTANCE->KeyPressing(DIK_D))
			fDgree = 45.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_S) && GAMEINSTANCE->KeyPressing(DIK_A))
			fDgree = 225.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_S) && GAMEINSTANCE->KeyPressing(DIK_D))
			fDgree = 135.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_W))
			fDgree = 0.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_A))
			fDgree = 270.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_S))
			fDgree = 180.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_D))
			fDgree = 90.f;

		if (GAMEINSTANCE->KeyPressing(DIK_SPACE))
			isSprint = true;

		SetDirectionFromCamera(fDgree);

		if (isJump)
		{
			m_pTransformCom->Jump(420.0f);
			m_isJumpState = true;
			m_isJumpMinActive = true;
			TriggerAnimationSet("Jump_Wolf", 1);
			if (isSprint)
				m_iJumpFlag = 2;
			else 
				m_iJumpFlag = 1;
		}
		else if (isSprint)
		{
			GAMEINSTANCE->SetZoomBlur(0.02f, 0.2f, m_pTransformCom->GetState(CTransform::STATE_POSITION));
			ChangeAnim(ANIMAL_WOLF_RUN_FAST, 0.05f, true);
			m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 11.f);
		}
		else
		{
			ChangeAnim(ANIMAL_WOLF_RUN_NORMAL, 0.05f, true);
			m_pTransformCom->GoStraight(_fTimeDelta);
		}

		if (m_isSprintState != isSprint)
		{
			if (isSprint)
			{
				EFFECTMGR->StopEffect("Wolf_Leave", 0);
				EFFECTMGR->StopEffect("Sprint", 0);

				EFFECTMGR->PlayEffect("Sprint", shared_from_this());
				
				switch (GAMEINSTANCE->GetCurrentLevel())
				{
				case LEVEL_VILLAGE:
				case LEVEL_FONOS:
					EFFECTMGR->PlayEffect("Wolf_Leave", shared_from_this());
					break;
				}
			}

			else
			{
				EFFECTMGR->StopEffect("Wolf_Leave", 0);
				EFFECTMGR->StopEffect("Sprint", 0);
			}

			m_isSprintState = isSprint;
		}

	}
	else
	{
		if (!_bMove)
			return isMove;

		if (m_isSprintState == true)
		{
			m_isSprintState = false;
			EFFECTMGR->StopEffect("Wolf_Leave", 0);
			EFFECTMGR->StopEffect("Sprint", 0);
		}

		if (GAMEINSTANCE->KeyDown(DIK_E))
			isJump = true;

		if (isJump)
		{
			m_pTransformCom->Jump(420.0f);
			m_isJumpState = true;
			m_isJumpMinActive = true;
			TriggerAnimationSet("Jump_Wolf", 2);
			m_iJumpFlag = 0;
		}
		else 
			ChangeAnim(ANIMAL_WOLF_IDLE, 0.05f, true);
	}

	return isMove;
}

void CWolf::LandingControl()
{
	if ((!m_isJumpState) && (!m_isLanding) && (m_pModelCom->GetCanCancelAnimation()))
	{
		CancleAnimationSet();

		ChangeAnim(ANIMAL_WOLF_JUMP_LOOP, 0.1f, true);

		m_isJumpState = true;
		m_iJumpFlag = 1;
		m_eCurrentBaseState = STATE_ACTIVE;
		m_pModelCom->PlayAnimation(0.f);
	}
}

_bool CWolf::EmotionControl()
{
	if (GAMEINSTANCE->KeyDown(DIK_1))
	{
		ChangeAnim(ANIMAL_WOLF_EMOTION_1, 0.1f, false);
		return true;
	}
	else if (GAMEINSTANCE->KeyDown(DIK_2))
	{
		ChangeAnim(ANIMAL_WOLF_EMOTION_2, 0.1f, false);
		return true;
	}
	return false;
}

void CWolf::CommonTick(_float _fTimeDelta)
{
	LandingControl();

	if (m_isJumpMinActive)
	{
		m_fJumpMinActiveTime += _fTimeDelta;

		if (m_fJumpMinActiveTime >= 0.1f)
		{
			m_isJumpMinActive = false;
			m_fJumpMinActiveTime = 0.f;
		}
	}

	if (m_iJumpFlag == 1)
	{
		m_pTransformCom->GoStraight(_fTimeDelta);
	}
	else if (m_iJumpFlag == 2)
	{
		GAMEINSTANCE->SetZoomBlur(0.02f, 0.2f, m_pTransformCom->GetState(CTransform::STATE_POSITION));
		m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 11.f);
	}
	if (ANIMAL_WOLF_JUMP_END_RUN == m_eCurrentAnimation)
	{
		m_pTransformCom->GoStraight(_fTimeDelta);
	}
	m_isLanding = false;
}

void CWolf::ChangeAnim(_uint _iAnimIndex, _float _fChangingDelay, _bool _isLoop, _float _fCancleRatio)
{
	if (!_isLoop)
		m_eCurrentBaseState = STATE_ACTIVE;

	m_pModelCom->ChangeAnim(_iAnimIndex, _fChangingDelay, _isLoop, _fCancleRatio);
	m_eCurrentAnimation = (WOLF_ANIMATION)_iAnimIndex;
}

_float3 CWolf::SetDirectionFromCamera(_float _fDgree, _bool _isSoft)
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

_bool CWolf::PlayAnimationSet()
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

void CWolf::CancleAnimationSet()
{
	m_iSetTrigger = 0;
	m_iAnimSetCount = 0;
	m_vecAnimationSet.clear();
}

void CWolf::TriggerAnimationSet(string _strSetName, _uint _SetFlag)
{
	m_iSetTrigger = _SetFlag;
	m_vecAnimationSet = m_AnimationSets.find(_strSetName)->second;
	ChangeAnim(get<0>(m_vecAnimationSet[0]), get<1>(m_vecAnimationSet[0]), false, get<2>(m_vecAnimationSet[0]));
}

void CWolf::PolymorphControl(_float _fTimeDelta)
{
	if (GAMEINSTANCE->KeyDown(DIK_LSHIFT) && m_isChanged && (!m_bChanging))
	{
		SetDissolve(true, true, 0.45f);
		SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
		m_bChanging = true;
		return;
	}
	if (true == m_bChanging)
	{
		m_fChangeingTime += _fTimeDelta;

		if (0.5f <= m_fChangeingTime)
		{
			m_fChangeingTime = 0.f;
			m_isChanged = false;
			m_bChanging = false;
			SetDissolve(false, false, 0.0f);

			if (m_isJumpState)
			{
				CHARACTERMGR->ChangePolymorph("Form_Eagle");
			}
			else
			{
				CHARACTERMGR->ChangePolymorph("Form_Human");
			}
		}
	}
	//////////////////////////////////
}

HRESULT CWolf::AddRigidBody(string _strModelKey)
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	//==본체==
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = true;
	//RigidBodyDesc.isGravity = false;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL/*| CRigidBody::LOCK_POS_X | CRigidBody::LOCK_POS_Z*/;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestitution = 0.0f;

	GeometryCapsule CapsuleDesc;
	//CapsuleDesc.fHeight = 1.f;
	//CapsuleDesc.fRadius = 0.25f;
	//CapsuleDesc.vOffSetPosition = { 0.0f, 0.25f, 0.0f };
	//CapsuleDesc.vOffSetRotation = { 0.0f, 90.0f, 0.0f };
	CapsuleDesc.fHeight = 0.2f;
	CapsuleDesc.fRadius = 0.25f;
	CapsuleDesc.vOffSetPosition = { 0.0f, 0.35f, 0.0f };
	CapsuleDesc.vOffSetRotation = { 0.0f, 0.0f, 90.0f };
	CapsuleDesc.strShapeTag = "Wolf_Body";
	RigidBodyDesc.pGeometry = &CapsuleDesc;

	RigidBodyDesc.eThisColFlag = COL_PLAYER;
	RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_TRIGGER | COL_INTERACTIONOBJ;
	m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

	m_pTransformCom->SetRigidBody(m_pRigidBody);

	//==피직스 낙하판정 체크용==
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.isInitCol = true;

	GeometryBox BoxDesc;
	BoxDesc.vSize = _float3{ 1.2f, 0.8f, 1.2f };
	BoxDesc.vOffSetPosition = { 0.0f, 0.05f, 0.0f };
	BoxDesc.strShapeTag = "Jump_Check";
	RigidBodyDesc.pGeometry = &BoxDesc;

	RigidBodyDesc.eThisColFlag = COL_PLAYER;
	RigidBodyDesc.eTargetColFlag = COL_STATIC;
	m_pRigidBody->CreateShape(&RigidBodyDesc);

	//==피직스 착지판정 체크용==
	BoxDesc.vSize = _float3{ 0.05f, 0.4f, 0.05f };
	BoxDesc.vOffSetPosition = { 0.f, 0.f, 0.0f };
	BoxDesc.strShapeTag = "Land_Check";
	RigidBodyDesc.pGeometry = &BoxDesc;

	RigidBodyDesc.eThisColFlag = COL_PLAYER;
	RigidBodyDesc.eTargetColFlag = COL_STATIC;
	m_pRigidBody->CreateShape(&RigidBodyDesc);

	return S_OK;
}

HRESULT CWolf::MakeAnimationSet()
{
	// NormalJump_=========================================================================
	vector< tuple<WOLF_ANIMATION, _float, _float> > vecJumpWolf;

	tuple<WOLF_ANIMATION, _float, _float> tpJumpWolfStart(ANIMAL_WOLF_JUMP_START, 0.05f, 1.f);
	vecJumpWolf.push_back(tpJumpWolfStart);
	tuple<WOLF_ANIMATION, _float, _float> tpJumpWolfLoop(ANIMAL_WOLF_JUMP_LOOP, 0.f, 1.f);
	vecJumpWolf.push_back(tpJumpWolfLoop);

	m_AnimationSets.emplace("Jump_Wolf", vecJumpWolf);
	// =================================================================================

	return S_OK;
}

shared_ptr<CWolf> CWolf::Create()
{
	shared_ptr<CWolf> pInstance = make_shared<CWolf>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CWolf");

	return pInstance;
}