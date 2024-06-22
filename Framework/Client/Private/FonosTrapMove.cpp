#include "FonosTrapMove.h"
#include "RigidBody.h"
#include "FonosTrap.h"

CFonosTrapMove::CFonosTrapMove()
{
}

CFonosTrapMove::~CFonosTrapMove()
{
}

shared_ptr<CFonosTrapMove> CFonosTrapMove::Create(TRAP_TYPE _eTrapType, _float3 _vPos, _float3 _vLook)
{
	shared_ptr<CFonosTrapMove> pInstance = make_shared<CFonosTrapMove>();

	if (FAILED(pInstance->Initialize(_eTrapType, _vPos, _vLook)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CFonosTrapMove::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CFonosTrapMove::Initialize(TRAP_TYPE _eTrapType, _float3 _vPos, _float3 _vLook)
{
	GeometryBox BoxDesc;
	BoxDesc.vSize = _float3(0.5f, 0.5f, 0.5f);

	BoxDesc.strShapeTag = "Fonos_Trap_Move";

	BoxDesc.vOffSetPosition = _float3(0.f, 0.4f, 0.f);
	BoxDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);

	__super::Initialize(_eTrapType, &BoxDesc, 0.5f, _vPos, _vLook);

	EFFECTMGR->PlayEffect("MiniGame_ElecBall", shared_from_this());

	return S_OK;
}

void CFonosTrapMove::PriorityTick(_float _fTimeDelta)
{
}

void CFonosTrapMove::Tick(_float _fTimeDelta)
{
	_float3 vDir;
	_float3 vDeltaPos;

	switch (m_eMoveType)
	{
	case MOVE_UD:
		// Move Up
		if (m_bMoveDir)
			vDir = { 0.f, 1.f, 0.f };
		// Move Down
		else
			vDir = { 0.f, -1.f, 0.f };
		break;
	case MOVE_LR:
		// Move Left
		if (m_bMoveDir)
		{
			vDir = -m_pTransformCom->GetState(CTransform::STATE_RIGHT);
			vDir.Normalize();
		}
		// Move Right
		else
		{
			vDir = m_pTransformCom->GetState(CTransform::STATE_RIGHT);
			vDir.Normalize();
		}
		break;
	}

	vDeltaPos = vDir * m_fMoveSpeed * _fTimeDelta;

	if (m_fMaxMoveAmount <= m_fMoveAmount + vDeltaPos.Length())
	{
		vDeltaPos = vDir * (m_fMaxMoveAmount - m_fMoveAmount);
		m_bMoveDir = !m_bMoveDir;
		m_fMoveAmount = 0.f;
	}

	else
		m_fMoveAmount += vDeltaPos.Length();

	m_pTransformCom->SetState(CTransform::STATE_POSITION, m_pTransformCom->GetState(CTransform::STATE_POSITION) + vDeltaPos);

	m_pTransformCom->Tick(_fTimeDelta);
}

void CFonosTrapMove::LateTick(_float _fTimeDelta)
{
	m_pTransformCom->LateTick(_fTimeDelta);

#ifdef _DEBUG
	if (m_pRigidBody)
		GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
}

HRESULT CFonosTrapMove::Render()
{
	return S_OK;
}

void CFonosTrapMove::SetMoveType(MOVE_TRAP_TYPE _eMoveType, _bool _bStartDir, _float _fSpeed)
{
	m_eMoveType = _eMoveType;
	m_bMoveDir = _bStartDir;
	m_fMoveSpeed = _fSpeed;
}

void CFonosTrapMove::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.eColFlag & COL_AMITOY)
	{
		int a = 3;
		/*m_isAttackedbyAmitoy = true;
		m_AttakedPos = _ColData.pGameObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);*/
	}
}

void CFonosTrapMove::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CFonosTrapMove::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}
