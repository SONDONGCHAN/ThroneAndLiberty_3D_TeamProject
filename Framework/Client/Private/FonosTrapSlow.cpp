#include "FonosTrapSlow.h"

CFonosTrapSlow::CFonosTrapSlow()
{
}

CFonosTrapSlow::~CFonosTrapSlow()
{
}

shared_ptr<CFonosTrapSlow> CFonosTrapSlow::Create(TRAP_TYPE _eTrapType, _float3 _vPos, _float3 _vLook)
{
	shared_ptr<CFonosTrapSlow> pInstance = make_shared<CFonosTrapSlow>();

	if (FAILED(pInstance->Initialize(_eTrapType, _vPos, _vLook)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CFonosTrapSlow::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CFonosTrapSlow::Initialize(TRAP_TYPE _eTrapType, _float3 _vPos, _float3 _vLook)
{
	GeometryBox BoxDesc;
	BoxDesc.vSize = _float3(1.7f, 0.3f, 1.7f);

	BoxDesc.strShapeTag = "Fonos_Trap_Slow";

	BoxDesc.vOffSetPosition = _float3(0.f, 0.15f, 0.f);
	BoxDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);

	__super::Initialize(_eTrapType, &BoxDesc, 0.5f, _vPos, _vLook);

	m_pModelCom = GAMEINSTANCE->GetModel("SpiderWeb_Floor1");

	return S_OK;
}

void CFonosTrapSlow::PriorityTick(_float _fTimeDelta)
{
}

void CFonosTrapSlow::Tick(_float _fTimeDelta)
{
}

void CFonosTrapSlow::LateTick(_float _fTimeDelta)
{
	__super::LateTick(_fTimeDelta);
}

HRESULT CFonosTrapSlow::Render()
{
	return __super::Render();
}

void CFonosTrapSlow::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.eColFlag & COL_AMITOY)
	{
		int a = 3;
		/*m_isAttackedbyAmitoy = true;
		m_AttakedPos = _ColData.pGameObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);*/
	}
}

void CFonosTrapSlow::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CFonosTrapSlow::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}
