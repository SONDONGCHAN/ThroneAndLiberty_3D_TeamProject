#include "pch.h"
#include "TrapDoor.h"
#include "Model.h"
#include "RigidBody.h"

CTrapDoor::CTrapDoor()
{
}

shared_ptr<CTrapDoor> CTrapDoor::Create(const _float3& _vPosition)
{
	shared_ptr<CTrapDoor> pInstance = make_shared<CTrapDoor>();

	if (FAILED(pInstance->Initialize(_vPosition)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CTrapDoor::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CTrapDoor::Initialize(const _float3& _vPosition)
{
	if (FAILED(CTrap::Initialize()))
		return E_FAIL;
	m_pTransformCom->LookAtDir({ 1.0f,0.0f,0.0f });
	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);

	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	m_pModelCom = pGameInstance->GetModel("BG_OneDoor_40_01");

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestitution = 0.0f;

	GeometryBox BoxDesc{};
	BoxDesc.strShapeTag = "StoneTrap";
	BoxDesc.vSize = { 7.0f,8.0f,3.0f };
	BoxDesc.vOffSetPosition = { 0.0f,4.0f,0.0f };

	RigidBodyDesc.pGeometry = &BoxDesc;

	RigidBodyDesc.eThisColFlag = COL_STATIC;
	RigidBodyDesc.eTargetColFlag = COL_PLAYER;
	m_pRigidBody = pGameInstance->CloneRigidBody(&RigidBodyDesc);

	m_pTransformCom->SetRigidBody(m_pRigidBody);

    return S_OK;
}

void CTrapDoor::PriorityTick(_float _fTimeDelta)
{
}

void CTrapDoor::Tick(_float _fTimeDelta)
{
	if (m_isDoorAction)
	{
		m_fTimeAcc += _fTimeDelta;
		if (m_isClosing)
		{
			m_pTransformCom->SetPositionParam(1, LinearIp(6.5f, -0.5f, m_fTimeAcc));
		}
		else
		{
			m_pTransformCom->SetPositionParam(1, LinearIp(-0.5f, 6.5f, m_fTimeAcc));
		}
		if (m_fTimeAcc >= 1.0f)
		{
			memset(&m_fTimeAcc, 0, sizeof(float));
			m_isDoorAction = false;
		}
		m_pTransformCom->Tick(_fTimeDelta);
	}
}

void CTrapDoor::LateTick(_float _fTimeDelta)
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	// frustrum
	if (pGameInstance->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 6.0f))
	{
		pGameInstance->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody)
			pGameInstance->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CTrapDoor::Render()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	pGameInstance->ReadyShader(m_eShaderType, m_iShaderPass);
	pGameInstance->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	UINT iNumMeshes = m_pModelCom->GetNumMeshes();

	for (UINT i = 0; i < iNumMeshes; i++) {

		pGameInstance->BeginNonAnimModel(m_pModelCom, i);
	}

	return S_OK;
}

void CTrapDoor::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CTrapDoor::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CTrapDoor::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CTrapDoor::DoorClose()
{
	m_isDoorAction = true;
	m_isClosing = true;
}

void CTrapDoor::DoorOpen()
{
	m_isDoorAction = true;
	m_isClosing = false;
}
