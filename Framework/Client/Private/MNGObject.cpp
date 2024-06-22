#include "MNGObject.h"
#include "Model.h"
#include "pch.h"
#include "RigidBody.h"

CMNGObject::CMNGObject()
{
}

CMNGObject::~CMNGObject()
{
}

shared_ptr<CMNGObject> CMNGObject::Create(const string& _strModelTag, const _float4x4& _WorldMatrix, Geometry* _pGeometry, float _fRadius, bool _isTrap)
{
	shared_ptr< CMNGObject> pInstance = make_shared<CMNGObject>();

	if (FAILED(pInstance->Initialize(_strModelTag, _WorldMatrix, _pGeometry, _fRadius, _isTrap)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CMNGObject::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CMNGObject::Initialize(const string& _strModelTag, const _float4x4& _WorldMatrix, Geometry* _pGeometry, float _fRadius, bool _isTrap)
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	m_pTransformCom = CTransform::Create(pGameInstance->GetDeviceInfo(), pGameInstance->GetDeviceContextInfo());
	m_pTransformCom->SetWorldMatrix(_WorldMatrix);

	m_pModelCom = pGameInstance->GetModel(_strModelTag);

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_NONANIM;
	m_iShaderPass = 0;

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = true;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::NONE;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 1.0f;
	RigidBodyDesc.fRestitution = 0.3f;

	RigidBodyDesc.pGeometry = _pGeometry;

	RigidBodyDesc.eThisColFlag = COL_STATIC;
	RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_AMITOY | COL_AMITOY_ATTACK | COL_TRAP;
	m_pRigidBody = pGameInstance->CloneRigidBody(&RigidBodyDesc);

	m_pTransformCom->SetRigidBody(m_pRigidBody);

	m_fRadius = _fRadius;
	m_isTrap = _isTrap;

	if (m_isTrap)
	{
		GeometrySphere BoomDesc{};
		BoomDesc.fRadius = 3.0f;
		BoomDesc.strShapeTag = "BoomShape";
		BoomDesc.vOffSetPosition = { 0.0f,0.5f,0.0f };
		RigidBodyDesc.pGeometry = &BoomDesc;
		RigidBodyDesc.isTrigger = true;
		RigidBodyDesc.isInitCol = false;
		RigidBodyDesc.eThisColFlag = COL_TRAP;
		RigidBodyDesc.eTargetColFlag = COL_AMITOY | COL_STATIC;

		m_pRigidBody->CreateShape(&RigidBodyDesc);
	}

	m_pRigidBody->SimulationOff();
	m_IsActive = false;

	return S_OK;
}

void CMNGObject::PriorityTick(_float _fTimeDelta)
{
}

void CMNGObject::Tick(_float _fTimeDelta)
{
	if (m_isGrabed && GAMEINSTANCE->MouseDown(DIM_RB))
	{
		m_isGrabed = false;
		m_pRigidBody->GravityOn();
		m_pRigidBody->WakeUp();
	}

	if (m_isGravityChange)
	{
		m_pRigidBody->GravityOff();
		m_isGravityChange = false;
	}
	if (m_isGrabed && m_pAmitoy.lock())
	{
		shared_ptr<CTransform> AmitoyTransform = m_pAmitoy.lock()->GetTransform();
		m_pRigidBody->SetGlobalPos(_float3(AmitoyTransform->GetState(CTransform::STATE_POSITION) + AmitoyTransform->GetState(CTransform::STATE_LOOK) + _float3{ 0.0f, 1.1f, 0.0f }), AmitoyTransform->GetQuat());
		m_pRigidBody->SetLinearVelocity({ 0.0f,0.0f,0.0f });
		m_pRigidBody->SetAngularVelocity({ 0.0f,0.0f,0.0f });
	}
}

void CMNGObject::LateTick(_float _fTimeDelta)
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	if (m_bDslv)
		UpdateDslv(_fTimeDelta);

	if (m_isDissolving)
	{
		if (m_isTrap)
		{
			m_pRigidBody->DisableCollision("BoomShape");
			m_isTrap = false;
		}
		if(!m_bDslv)
		{
			m_IsActive = false;
			m_pRigidBody->SimulationOff();
			return;
		}
	}

	if (m_isAttackedbyAmitoy)
	{
		m_pRigidBody->AddForce(ToPxVec3(XMVector3Normalize(m_pTransformCom->GetState(CTransform::STATE_POSITION) - m_AttakedPos + _float3{ 0.0f, 1.3f, 0.0f })) * 16.0f, PxForceMode::eVELOCITY_CHANGE);
		m_isAttackedbyAmitoy = false;
		m_iAttackedCnt++;
		if (m_iAttackedCnt == 4)
		{
			SetDissolve(true, true, 1.0f);
			SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
			m_isDissolving = true;
		}
		if (m_isTrap)
		{
			SetDissolve(true, true, 1.0f);
			SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
			m_isDissolving = true;
			m_pRigidBody->EnableCollision("BoomShape");
			pGameInstance->PlaySoundW("Explode", 0.6f);
		}
	}

	if (m_AttackedbyBomb)
	{
		m_pRigidBody->AddForce(ToPxVec3(XMVector3Normalize(m_pTransformCom->GetState(CTransform::STATE_POSITION) - m_AttackedBombPos) * 17.0f), PxForceMode::eVELOCITY_CHANGE);
		m_AttackedbyBomb = false;
		if (m_iAttackedCnt == 4)
		{
			SetDissolve(true, true, 1.0f);
			SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
			m_isDissolving = true;
		}
		if (m_isTrap)
		{
			SetDissolve(true, true, 1.0f);
			SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
			m_isDissolving = true;
			m_pRigidBody->EnableCollision("BoomShape");
			pGameInstance->PlaySoundW("Explode", 0.6f);
		}
	}

	m_pTransformCom->LateTickFragment(_fTimeDelta);

	if (m_isGrabed)
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_EDGE, shared_from_this());

	// frustrum
	if (pGameInstance->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), m_fRadius))
	{
		pGameInstance->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody)
			pGameInstance->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CMNGObject::Render()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	pGameInstance->ReadyShader(m_eShaderType, m_iShaderPass);
	pGameInstance->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	if (m_bDslv)
	{
		if (FAILED(pGameInstance->BindRawValue("g_bDslv", &m_bDslv, sizeof(_bool))))
			return E_FAIL;

		_float g_fDslvValue = m_fDslvTime / m_fMaxDslvTime;


		if (FAILED(pGameInstance->BindRawValue("g_fDslvValue", &g_fDslvValue, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pGameInstance->BindRawValue("g_fDslvThick", &m_fDslvThick, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pGameInstance->BindRawValue("g_vDslvColor", &m_vDslvColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pGameInstance->BindSRV("g_DslvTexture", m_strDslvTexKey)))
			return E_FAIL;
	}

	UINT iNumMeshes = m_pModelCom->GetNumMeshes();

	UsingRimLight({ 0.9f, 0.9f, 0.9f }, 3.0f);

	for (UINT i = 0; i < iNumMeshes; i++) {

		pGameInstance->BeginNonAnimModel(m_pModelCom, i);
	}

	/*RimLight Off*/
	bool bRimOff = false;
	pGameInstance->BindRawValue("g_IsUsingRim", &bRimOff, sizeof(bool));

	if (m_bDslv)
	{
		bool bDslvFalse = false;
		if (FAILED(pGameInstance->BindRawValue("g_bDslv", &bDslvFalse, sizeof(bool))))
			return E_FAIL;
	}

	return S_OK;
}

void CMNGObject::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.eColFlag & COL_AMITOY_ATTACK)
	{
		m_isAttackedbyAmitoy = true;
		m_AttakedPos = _ColData.pGameObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
	}

	if (_ColData.eColFlag & COL_TRAP)
	{
		m_AttackedbyBomb = true;
		m_AttackedBombPos = _ColData.pGameObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
	}

	if (0 == _ColData.szShapeTag.compare("AmitoyHands"))
	{
		m_isGravityChange = true;
		m_pAmitoy = _ColData.pGameObject;
		m_isGrabed = true;
	}

}

void CMNGObject::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CMNGObject::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CMNGObject::SpawnObject()
{
	m_IsActive = true;
	m_pRigidBody->SimulationOn();
	m_pRigidBody->WakeUp();
	SetDissolve(true, false, 2.0f);
	SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
}

void CMNGObject::DespawnObject()
{
	SetDissolve(true, true, 2.0f);
	SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
	m_isDissolving = true;
}

HRESULT CMNGObject::RenderEdgeDepth()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
	// 외곽선을 위한 깊이 그리기
	pGameInstance->ReadyShader(m_eShaderType, 2);
	pGameInstance->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		pGameInstance->BeginNonAnimModel(m_pModelCom, i);
	}

	return S_OK;
}

HRESULT CMNGObject::RenderEdge() 
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
	// 외곽선을 위한 노말 늘린 포지션 그리기
	pGameInstance->ReadyShader(m_eShaderType, 7);
	pGameInstance->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		pGameInstance->BeginNonAnimModel(m_pModelCom, i);
	}

	return S_OK;
}
