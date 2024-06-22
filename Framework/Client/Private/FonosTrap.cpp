#include "pch.h"
#include "Model.h"
#include "RigidBody.h"
#include "FonosTrap.h"

CFonosTrap::CFonosTrap()
{
}

CFonosTrap::~CFonosTrap()
{
}

HRESULT CFonosTrap::Initialize(TRAP_TYPE _eTrapType, Geometry* _pGeometry, float _fRadius, _float3 _vPos, _float3 _vLook)
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	m_pTransformCom = CTransform::Create(pGameInstance->GetDeviceInfo(), pGameInstance->GetDeviceContextInfo());

	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPos);
	m_pTransformCom->LookAtForLandObject(_vLook);

	m_eTrapType = _eTrapType;

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_NONANIM;
	m_iShaderPass = 0;

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::NONE;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestitution = 0.0f;

	RigidBodyDesc.pGeometry = _pGeometry;

	RigidBodyDesc.eThisColFlag = COL_TRAP;
	RigidBodyDesc.eTargetColFlag = COL_AMITOY;
	m_pRigidBody = pGameInstance->CloneRigidBody(&RigidBodyDesc);

	m_pTransformCom->SetRigidBody(m_pRigidBody);

	m_fRadius = _fRadius;

	/*if (m_isTrap)
	{
		GeometrySphere BoomDesc{};
		BoomDesc.fRadius = 3.0f;
		BoomDesc.strShapeTag = "BoomShape";
		BoomDesc.vOffSetPosition = { 0.0f,0.5f,0.0f };
		RigidBodyDesc.pGeometry = &BoomDesc;
		RigidBodyDesc.isTrigger = true;
		RigidBodyDesc.isInitCol = false;
		RigidBodyDesc.eThisColFlag = COL_TRAP;
		RigidBodyDesc.eTargetColFlag = COL_AMITOY;

		m_pRigidBody->CreateShape(&RigidBodyDesc);
	}*/


	return S_OK;
}

void CFonosTrap::PriorityTick(_float _fTimeDelta)
{
}

void CFonosTrap::Tick(_float _fTimeDelta)
{
}

void CFonosTrap::LateTick(_float _fTimeDelta)
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	if (m_bDslv)
		UpdateDslv(_fTimeDelta);

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

HRESULT CFonosTrap::Render()
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

	//UsingRimLight({ 0.9f, 0.9f, 0.9f }, 3.0f);

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

void CFonosTrap::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CFonosTrap::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CFonosTrap::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}
