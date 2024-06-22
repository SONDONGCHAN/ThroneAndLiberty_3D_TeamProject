#include "Bounding_Sphere.h"


CBounding_Sphere::CBounding_Sphere(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	: CBounding(_pDevice, _pContext)
{
}

shared_ptr<CBounding_Sphere> CBounding_Sphere::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, void* _pArg)
{
	shared_ptr<CBounding_Sphere> pInstance = make_shared<CBounding_Sphere>(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CBounding_Sphere::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CBounding_Sphere::Initialize(void * _pArg)
{
	/* 초기상태는 로컬상태 */
	SphereDesc*	pSphereDesc = (SphereDesc*)_pArg;

	m_pOriginalSphere = make_shared<BoundingSphere>(pSphereDesc->vCenter, pSphereDesc->fRadius);
	m_pSphere = make_shared<BoundingSphere>(*m_pOriginalSphere);
	
	return S_OK;
}

void CBounding_Sphere::Tick(_float4x4& _WorldMatrix)
{
	m_pOriginalSphere->Transform(*m_pSphere, _WorldMatrix);
}

HRESULT CBounding_Sphere::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> _pBatch)
{
	DX::Draw(_pBatch.get(), *m_pSphere, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

bool CBounding_Sphere::Intersect(CCollider::ECOL_TYPE _eType, shared_ptr<CBounding> _pBounding)
{
	m_isColl = false;

	void* pTargetBounding = _pBounding->GetBounding();

	switch (_eType)
	{
	case CCollider::AABB_TYPE:
		m_isColl = m_pSphere->Intersects(*(BoundingBox*)(pTargetBounding));
		break;
	case CCollider::OBB_TYPE:
		m_isColl = m_pSphere->Intersects(*(BoundingOrientedBox*)pTargetBounding);
		break;
	case CCollider::SPHERE_TYPE:
		m_isColl = m_pSphere->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}

	return m_isColl;
}

_float CBounding_Sphere::Get_Radius()
{
	return 	m_pSphere->Radius;
}

_float3 CBounding_Sphere::Get_Center()
{
	return 	m_pSphere->Center;
}

void CBounding_Sphere::Set_Radius(_float fRadius)
{
	m_pOriginalSphere->Radius = fRadius;
}

void CBounding_Sphere::Set_Center(_float3 vCenter)
{
	m_pOriginalSphere->Center = vCenter;
}
