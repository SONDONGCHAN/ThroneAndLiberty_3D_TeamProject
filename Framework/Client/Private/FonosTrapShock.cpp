#include "FonosTrapShock.h"

CFonosTrapShock::CFonosTrapShock()
{
}

CFonosTrapShock::~CFonosTrapShock()
{
}

shared_ptr<CFonosTrapShock> CFonosTrapShock::Create(TRAP_TYPE _eTrapType, _float3 _vPos, _float3 _vLook)
{
	shared_ptr<CFonosTrapShock> pInstance = make_shared<CFonosTrapShock>();

	if (FAILED(pInstance->Initialize(_eTrapType, _vPos, _vLook)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CFonosTrapShock::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CFonosTrapShock::Initialize(TRAP_TYPE _eTrapType, _float3 _vPos, _float3 _vLook)
{
	GeometryBox BoxDesc;
	BoxDesc.vSize = _float3(0.3f, 2.f, 4.0f);

	BoxDesc.strShapeTag = "Fonos_Trap_Shock";

	BoxDesc.vOffSetPosition = _float3(0.f, 1.f, 0.f);
	BoxDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);

	__super::Initialize(_eTrapType, &BoxDesc, 0.5f, _vPos, _vLook);

	m_pModelCom = GAMEINSTANCE->GetModel("WoodFence_11_04");

	EFFECTMGR->PlayEffect("Fonos_MiniGame_Fence", shared_from_this());

	return S_OK;
}

void CFonosTrapShock::PriorityTick(_float _fTimeDelta)
{
}

void CFonosTrapShock::Tick(_float _fTimeDelta)
{
}

void CFonosTrapShock::LateTick(_float _fTimeDelta)
{
	__super::LateTick(_fTimeDelta);
}

HRESULT CFonosTrapShock::Render()
{
	return __super::Render();
}

void CFonosTrapShock::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.eColFlag & COL_AMITOY)
	{
		int a = 3;
		/*m_isAttackedbyAmitoy = true;
		m_AttakedPos = _ColData.pGameObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);*/
	}
}

void CFonosTrapShock::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CFonosTrapShock::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}
