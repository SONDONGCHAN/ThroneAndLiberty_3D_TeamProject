#include "pch.h"
#include "TrapElectric.h"
#include "RigidBody.h"
#include "EffectMgr.h"

CTrapElectric::CTrapElectric()
{
}

shared_ptr<CTrapElectric> CTrapElectric::Create()
{
	shared_ptr<CTrapElectric> pInstance = make_shared<CTrapElectric>();

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(nullptr, L"Initialize Failed", L"", MB_OK);
	}

	return pInstance;
}

HRESULT CTrapElectric::Initialize()
{
	if (FAILED(CTrap::Initialize()))
		return E_FAIL;

	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::NONE;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 0.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestitution = 0.0f;

	GeometryCapsule CapsulDesc{};
	CapsulDesc.strShapeTag = "ElectricTrap";
	CapsulDesc.fHeight = 72.0f;
	CapsulDesc.fRadius = 0.4f;
	
	RigidBodyDesc.pGeometry = &CapsulDesc;

	RigidBodyDesc.eThisColFlag = COL_TRAP;
	RigidBodyDesc.eTargetColFlag = COL_PLAYER;
	m_pRigidBody = pGameInstance->CloneRigidBody(&RigidBodyDesc);

	m_pTransformCom->SetRigidBody(m_pRigidBody);

	m_pRigidBody->SimulationOff();

	m_IsActive = false;

	return S_OK;
}

void CTrapElectric::PriorityTick(_float _fTimeDelta)
{
}

void CTrapElectric::Tick(_float _fTimeDelta)
{
	m_fTimeAcc += _fTimeDelta;
	if (m_isStart)
	{
		switch (m_eMode)
		{
		case Client::CTrapElectric::ELEC_GO:
		{
			m_pTransformCom->GoStraight(_fTimeDelta);
		}
		break;
		case Client::CTrapElectric::ELEC_SPIN:
		{
			m_pTransformCom->RotationPlus({ 0.0f,1.0f,0.0f }, m_fSpeed * _fTimeDelta);
		}
		break;
		}
	}
	else
	{
		if (m_fTimeAcc >= 2.0f)
		{
			m_isStart = true;
			m_pRigidBody->SimulationOn();
		}
	}

	m_pTransformCom->Tick(_fTimeDelta);
}

void CTrapElectric::LateTick(_float _fTimeDelta)
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	pGameInstance->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

#ifdef _DEBUG
	if (m_pRigidBody)
		pGameInstance->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG

	switch (m_eMode)
	{
	case Client::CTrapElectric::ELEC_GO:
	{
		_float3 vPosition{ m_pTransformCom->GetState(CTransform::STATE_POSITION) };
		if (vPosition.x <= 47.4f || vPosition.x >= 107.2f)
		{
			TrapStop();
			return;
		}
		else if (vPosition.z <= 85.2f || vPosition.z >= 122.3f)
		{
			TrapStop();
			return;
		}
	}
	break;
	case Client::CTrapElectric::ELEC_SPIN:
	{
		if (m_fTimeAcc >= 32.0f)
		{
			TrapStop();
			return;
		}
	}
	break;
	}
}

HRESULT CTrapElectric::Render()
{
	return S_OK;
}

void CTrapElectric::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CTrapElectric::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CTrapElectric::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CTrapElectric::SettingElectric(EELECTRICMODE _eMode, const _float4x4& _WorldMatrix, float _fSpeed)
{
	m_eMode = _eMode;
	m_IsActive = true;
	m_pTransformCom->SetWorldMatrix(_WorldMatrix);
	m_pTransformCom->SetSpeed(_fSpeed);
	m_pRigidBody->SetGlobalPos(_WorldMatrix.Translation(), m_pTransformCom->GetQuat());
	memcpy_s(&m_fSpeed, sizeof(float), &_fSpeed, sizeof(float));
	m_isStart = false;
	m_fTimeAcc = 0.0f;
	m_pRigidBody->SimulationOff();
	EFFECTMGR->PlayEffect("Trap_Razor", shared_from_this());
}

void CTrapElectric::TrapStop()
{
	m_IsActive = false;
	m_isStart = false;
	m_fTimeAcc = 0.0f;
	m_pRigidBody->SimulationOff();
}
