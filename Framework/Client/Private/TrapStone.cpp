#include "pch.h"
#include "TrapStone.h"
#include "Model.h"
#include "RigidBody.h"
#include "EffectMgr.h"

CTrapStone::CTrapStone()
{
}

shared_ptr<CTrapStone> CTrapStone::Create()
{
	shared_ptr<CTrapStone> pInstance = make_shared<CTrapStone>();

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CTrapStone::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CTrapStone::Initialize()
{
	if (FAILED(CTrap::Initialize()))
		return E_FAIL;

	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	m_pModelCom = pGameInstance->GetModel("TrapStone");

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 0.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestitution = 0.0f;

	GeometrySphere SphereDesc{};
	SphereDesc.strShapeTag = "StoneTrap";
	SphereDesc.fRadius = 1.5f;

	RigidBodyDesc.pGeometry = &SphereDesc;

	RigidBodyDesc.eThisColFlag = COL_TRAP;
	RigidBodyDesc.eTargetColFlag = COL_PLAYER;
	m_pRigidBody = pGameInstance->CloneRigidBody(&RigidBodyDesc);

	m_pTransformCom->SetRigidBody(m_pRigidBody);

	m_pRigidBody->SimulationOff();

	m_IsActive = false;

	return S_OK;
}

void CTrapStone::PriorityTick(_float _fTimeDelta)
{
}

void CTrapStone::Tick(_float _fTimeDelta)
{
	if (m_isStart)
	{
		_float3 vPosition = m_pTransformCom->GetState(CTransform::STATE_POSITION);

		vPosition.y -= m_fSpeed * _fTimeDelta;
		if (vPosition.y <= 0.0f)
		{
			m_IsActive = false;
			m_pRigidBody->SimulationOff();
			GAMEINSTANCE->PlaySoundW("StoneDrop", 0.6f);
			return;
		}
		else
		{
			m_pTransformCom->SetState(CTransform::STATE_POSITION, vPosition);
		}
	}
	else
	{
		m_fTimeAcc += _fTimeDelta;
		if (m_fTimeAcc >= 0.5f)
		{
			EFFECTMGR->PlayEffect("Trap_Stone2", shared_from_this());
			m_isStart = true;
			m_pRigidBody->SimulationOn();
			int iSound = GAMEINSTANCE->PickRandomInt(0, 2);
			switch (iSound)
			{
			case 0:
			{
				GAMEINSTANCE->PlaySoundW("StoneDropping1", 0.4f);
			}
			break;
			case 1:
			{
				GAMEINSTANCE->PlaySoundW("StoneDropping2", 0.4f);
			}
			break;
			}
		}
	}

	m_pTransformCom->Tick(_fTimeDelta);
}

void CTrapStone::LateTick(_float _fTimeDelta)
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	// frustrum
	if (pGameInstance->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 3.0f))
	{
		pGameInstance->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody)
			pGameInstance->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CTrapStone::Render()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	pGameInstance->ReadyShader(m_eShaderType, m_iShaderPass);
	pGameInstance->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());


	return S_OK;
}

void CTrapStone::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CTrapStone::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CTrapStone::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CTrapStone::SettingStone(const _float3& _vPosition, float _fSpeed)
{
	m_IsActive = true;
	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);
	m_isStart = false;
	memcpy_s(&m_fSpeed, sizeof(float), &_fSpeed, sizeof(float));
	memset(&m_fTimeAcc, 0, sizeof(float));
	EFFECTMGR->PlayEffect("Trap_Stone_Target2", { _vPosition.x,0.1f,_vPosition.z }, false, 3.f);
}
