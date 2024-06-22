#include "Bounding_AABB.h"


CBounding_AABB::CBounding_AABB(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	: CBounding(_pDevice, _pContext)
{
}

shared_ptr<CBounding_AABB> CBounding_AABB::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, void* _pArg)
{
	shared_ptr<CBounding_AABB> pInstance = make_shared<CBounding_AABB>(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CBounding_AABB::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CBounding_AABB::Initialize(void * pArg)
{
	/* 초기상태는 로컬상태 */
	AABBDesc* pDesc = (AABBDesc*)pArg;

	m_pOriginalAABB = make_shared<BoundingBox>(pDesc->vCenter, pDesc->vExtents);
	m_pAABB = make_shared<BoundingBox>(*m_pOriginalAABB);

	return S_OK;
}

void CBounding_AABB::Tick(_float4x4& _WorldMatrix)
{
	_float4x4 TransformMatrix = _WorldMatrix;

	TransformMatrix.Right(_float3(1.0f, 0.0f, 0.0f) * XMVector3Length(TransformMatrix.Right()).m128_f32[0]);
	TransformMatrix.Up(_float3(0.0f, 1.0f, 0.0f) * XMVector3Length(TransformMatrix.Up()).m128_f32[0]);
	TransformMatrix.Forward(_float3(0.0f, 0.0f, 1.0f) * XMVector3Length(TransformMatrix.Forward()).m128_f32[0]);

	m_pOriginalAABB->Transform(*m_pAABB, TransformMatrix);
}

HRESULT CBounding_AABB::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> _pBatch)
{
	DX::Draw(_pBatch.get(), *m_pAABB, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

bool CBounding_AABB::Intersect(CCollider::ECOL_TYPE _eType, shared_ptr<CBounding> _pBounding)
{
	m_isColl = false;

	void* pTargetBounding = _pBounding->GetBounding();

	switch (_eType)
	{
	case CCollider::AABB_TYPE:
		m_isColl = m_pAABB->Intersects(*(BoundingBox*)pTargetBounding);
		break;
	case CCollider::OBB_TYPE:
		m_isColl = m_pAABB->Intersects(*(BoundingOrientedBox*)pTargetBounding);
		break;
	case CCollider::SPHERE_TYPE:
		m_isColl = m_pAABB->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}

	return m_isColl;
}

_float3 CBounding_AABB::Get_Center()
{
	return 	m_pAABB->Center;
}

void CBounding_AABB::Set_Center(_float3 vCenter)
{
	m_pOriginalAABB->Center = vCenter;

}
