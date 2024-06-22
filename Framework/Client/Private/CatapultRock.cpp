#include "CatapultRock.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "CameraMgr.h"
#include "Catapult.h"


CCatapultRock::CCatapultRock()
{
}

HRESULT CCatapultRock::Initialize(shared_ptr<CatapultRock_Desc> _pCatapultRockDesc, _float3 _vPosition)
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 0.f;
	pArg->fRotationPerSec = XMConvertToRadians(90.0f);

	__super::Initialize(pArg.get());

	m_pModel = GAMEINSTANCE->GetModel("CatapultRock");
	AddComponent(L"CatapultRock_Model", m_pModel);
	AddRigidBody("CatapultRock");

	m_MyDesc.pParentTransform	= _pCatapultRockDesc->pParentTransform;
	m_MyDesc.pSocketMatrix		= _pCatapultRockDesc->pSocketMatrix;
	m_MyDesc.pCatapult			= _pCatapultRockDesc->pCatapult;

	
	m_OffsetMatrix = SimpleMath::Matrix::CreateTranslation(_float3(0.f, 2.2f, 0.f));

	m_eShaderType = ST_NONANIM;
	m_iShaderPass = 0;

	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);
	m_pTransformCom->SetScaling(0.6f, 0.6f, 0.6f);

	m_pTransformCom->Tick(0.f);

	m_pCamera = CCameraMgr::GetInstance()->FindCamera("StaticCamera");

	m_pRigidBody->SetMass(10.f);


	return S_OK;
}

void CCatapultRock::PriorityTick(_float _fTimeDelta)
{

}

void CCatapultRock::Tick(_float _fTimeDelta)
{
	if (m_IsHit)
	{
		m_fAfterHitTime -= _fTimeDelta;

		if (m_fAfterHitTime <= 0.f)
		{
			m_MyDesc.pCatapult.lock()->OnCatapultCamera();
			m_pRigidBody->DisableCollision("CatapultRock_Body");
			m_pRigidBody->SimulationOff();
			shared_from_this()->SetEnable(false);
		}
	}

	if(m_IsOn && (!m_IsUsed))
	{ 
		XMStoreFloat4x4(&m_WorldMatrix, (*m_MyDesc.pSocketMatrix) * ((m_MyDesc.pParentTransform)->GetWorldMatrix()) * m_OffsetMatrix );
		_float3 scale;
		_quaternion quat;
		_float3 pos;
		m_WorldMatrix.Decompose(scale, quat, pos);
		m_pRigidBody->SetGlobalPos(m_WorldMatrix.Translation(), quat);
	}
	else if(m_IsUsed)
	{

	}

	//m_pTransformCom->Tick(_fTimeDelta);
}

void CCatapultRock::LateTick(_float _fTimeDelta)
{
	if (m_IsOn && (!m_IsUsed))
		m_pTransformCom->LateTick(_fTimeDelta);

	else if ((m_IsOn && m_IsUsed) || (!m_IsOn && !m_IsUsed))
	{
		m_pTransformCom->LateTickFragment(_fTimeDelta);

		if (m_IsCamOn)
		{
			_float3 vMyPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
			//_float3 vMyLeft = m_vShootDir.Cross(_float3(0.f, 1.f, 0.f));
			//vMyLeft.Normalize();

			_float3 vMyAt = vMyPos;
			_float3 vMyEye = vMyPos - (m_vShootDir * 10.f) + _float3( 0.f, 2.f, 0.f);

			m_pCamera.lock()->SetEyeAt(vMyEye, vMyAt);

			if(!m_IsHit)
				GAMEINSTANCE->SetZoomBlur(0.02f, 0.3f, m_pTransformCom->GetState(CTransform::STATE_POSITION));
		}
	}

	if (m_bDslv)
		UpdateDslv(_fTimeDelta);

	if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 3.0f))
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
		GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());
	}

#ifdef _DEBUG
	if (m_pRigidBody)
		GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG

}

HRESULT CCatapultRock::Render()
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

	_uint iNumMeshes = m_pModel->GetNumMeshes();
	
	for (_uint i = 0; i < iNumMeshes; i++) 
	{
		GAMEINSTANCE->BeginNonAnimModel(m_pModel, i);
	}

	_bool bDslvFalse = false;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &bDslvFalse, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCatapultRock::RenderShadow()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 1);
	GAMEINSTANCE->BindLightProjMatrix();
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
	
	_uint iNumMeshes = m_pModel->GetNumMeshes();
	
	for (size_t i = 0; i < iNumMeshes; i++)
		GAMEINSTANCE->BeginNonAnimModel(m_pModel, (_uint)i);
	
	return S_OK;
}

void CCatapultRock::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (m_IsOn && m_IsUsed && (!m_IsHit))
	{
		if (_ColData.eColFlag == COL_STATIC || _ColData.eColFlag == COL_INTERACTIONOBJ)
		{
			m_IsHit = true;
			ROT_DESC RotDesc = CCameraMgr::GetInstance()->RotSetting({ 0.06f, 0.06f, 0.0f }, { 4.0f,7.0f,0.0f }, { 0.0f, 0.1f });
			CCameraMgr::GetInstance()->CameraShake(SHAKE_ROT, 0.3f, &RotDesc);
			GAMEINSTANCE->PlaySoundW("Catapult_Crash_1", 0.5f);
			GAMEINSTANCE->StopSound("Catapult_Wind");
			//GAMEINSTANCE->SetMotionBlur(0.3f, 0.04f);
		}
	}
	//else if (m_IsOn && m_IsUsed && m_IsHit)
	//{
	//	if (_ColData.eColFlag == COL_STATIC || _ColData.eColFlag == COL_INTERACTIONOBJ)
	//	{
	//		m_IsHit = true;
	//		ROT_DESC RotDesc = CCameraMgr::GetInstance()->RotSetting({ 0.03f, 0.03f, 0.0f }, { 3.0f,5.0f,0.0f }, { 0.0f, 0.05f });
	//		CCameraMgr::GetInstance()->CameraShake(SHAKE_ROT, 0.1f, &RotDesc);
	//		//GAMEINSTANCE->SetMotionBlur(0.3f, 0.04f);
	//	}
	//}
}

void CCatapultRock::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CCatapultRock::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CCatapultRock::SetOnCatapultBarrel()
{
	if (!m_IsOn)
	{
		m_IsOn = true;

		m_pRigidBody->SetLinearVelocity({0.f,0.f,0.f});
		m_pRigidBody->SetAngularVelocity({0.f,0.f,0.f});

		XMStoreFloat4x4(&m_WorldMatrix,(*m_MyDesc.pSocketMatrix) * ((m_MyDesc.pParentTransform)->GetWorldMatrix()) * m_OffsetMatrix);
		_float3 scale;
		_quaternion quat;
		_float3 pos;
		m_WorldMatrix.Decompose(scale, quat, pos);
		m_pRigidBody->SetGlobalPos(m_WorldMatrix.Translation(), quat);

		m_pRigidBody->GravityOff();

		SetDissolve(true, false, 0.8f);
		SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
	}
}

void CCatapultRock::ShootRock(_float3 _vDir, _float _fForce)
{
	m_IsUsed = true;
	m_pRigidBody->SetAngularVelocity({ 0.f,0.f,3.f });
	m_pRigidBody->AddForce(ToPxVec3(XMVector3Normalize(_vDir) * _fForce), PxForceMode::eVELOCITY_CHANGE);
	m_pRigidBody->GravityOn();

	m_vShootDir = _vDir;
	m_vShootDir.y -= 1.f;
}

void CCatapultRock::StartDissolve()
{
	SetDissolve(true, true, 0.6f);
	SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
}

void CCatapultRock::AddRigidBody(string _strModelKey)
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	//==º»Ã¼==
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = true;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::NONE;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 1.0f;
	RigidBodyDesc.fRestitution = 0.1f;

	GeometrySphere SphereDesc;
	SphereDesc.fRadius = 2.f;
	SphereDesc.strShapeTag = "CatapultRock_Body";
	RigidBodyDesc.pGeometry = &SphereDesc;

	RigidBodyDesc.eThisColFlag = COL_INTERACTIONOBJ;
	RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER | COL_INTERACTIONOBJ;
	m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

	m_pTransformCom->SetRigidBody(m_pRigidBody);
}

shared_ptr<CCatapultRock> CCatapultRock::Create(shared_ptr<CatapultRock_Desc> _pCatapultRockDesc, _float3 _vPosition)
{
	shared_ptr<CCatapultRock> pCatapultRock = make_shared<CCatapultRock>();

	if (FAILED(pCatapultRock->Initialize(_pCatapultRockDesc,_vPosition)))
		MSG_BOX("Failed to Create : CCatapultRock");

	return pCatapultRock;
}
