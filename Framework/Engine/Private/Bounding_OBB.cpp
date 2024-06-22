#include "Bounding_OBB.h"


CBounding_OBB::CBounding_OBB(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext )
	: CBounding(_pDevice, _pContext)
{
}

shared_ptr<CBounding_OBB> CBounding_OBB::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, void* _pArg)
{
	shared_ptr<CBounding_OBB> pInstance = make_shared<CBounding_OBB>(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CBounding_OBB::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CBounding_OBB::Initialize(void * pArg)
{
	/* 초기상태는 로컬상태 */
	OBBDesc* pDesc = (OBBDesc*)pArg;

	_float4 vRotation;
	XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYaw(pDesc->vRadians.x, pDesc->vRadians.y, pDesc->vRadians.z));

	m_pOriginalOBB = make_shared<BoundingOrientedBox>(pDesc->vCenter, pDesc->vExtents, vRotation);
	m_pOBB = make_shared<BoundingOrientedBox>(*m_pOriginalOBB);

	return S_OK;
}

void CBounding_OBB::Tick(_float4x4& _WorldMatrix)
{
	m_pOriginalOBB->Transform(*m_pOBB, _WorldMatrix);
}

HRESULT CBounding_OBB::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> _pBatch)
{
	DX::Draw(_pBatch.get(), *m_pOBB, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

bool CBounding_OBB::Intersect(CCollider::ECOL_TYPE _eType, shared_ptr<CBounding> _pBounding)
{
	m_isColl = false;

	void* pTargetBounding = _pBounding->GetBounding();

	switch (_eType)
	{
	case CCollider::AABB_TYPE:
		m_isColl = m_pOBB->Intersects(*(BoundingBox*)pTargetBounding);
		break;
	case CCollider::OBB_TYPE:
		m_isColl = m_pOBB->Intersects(*(BoundingOrientedBox*)pTargetBounding);
		break;
	case CCollider::SPHERE_TYPE:
		m_isColl = m_pOBB->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}

	return m_isColl;
}

_float3 CBounding_OBB::Get_Extents()
{
	return 	m_pOBB->Extents;
}

_float3 CBounding_OBB::Get_Center()
{
	return 	m_pOBB->Center;
}

void CBounding_OBB::Set_Extents(_float3 vExtents)
{
	m_pOriginalOBB->Extents = vExtents;
}

void CBounding_OBB::Set_Center(_float3 vCenter)
{
	m_pOriginalOBB->Center = vCenter;
}
