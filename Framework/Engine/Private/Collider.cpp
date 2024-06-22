#include "Collider.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CCollider::CCollider(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CCollider::CCollider(const shared_ptr<CCollider> _pOrigin)
	: CComponent(_pOrigin), m_eType(_pOrigin->m_eType), m_pBounding(_pOrigin->m_pBounding)
	, m_pBatch(_pOrigin->m_pBatch)
	, m_pEffect(_pOrigin->m_pEffect)
	, m_pInputLayout(_pOrigin->m_pInputLayout)
{
}

shared_ptr<CCollider> CCollider::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, ECOL_TYPE _eType, void* _pArg)
{
	shared_ptr<CCollider> pInstance = make_shared<CCollider>(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_eType, _pArg)))
	{
		MSG_BOX("Failed to Created : CCollider");
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}


shared_ptr<CComponent> CCollider::Clone(void* _pArg)
{
	shared_ptr<CCollider> pClone = make_shared<CCollider>(static_pointer_cast<CCollider>(shared_from_this()));

	if (FAILED(pClone->InitializeClone(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		pClone.reset();
		return nullptr;
	}

	return pClone;
}

HRESULT CCollider::Initialize(ECOL_TYPE _eType, void* _pArg)
{
	m_eType = _eType;	

	m_pBatch = make_shared<PrimitiveBatch<VertexPositionColor>>(m_pContext.Get());
	m_pEffect = make_shared<BasicEffect>(m_pDevice.Get());

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = nullptr;
	size_t		iByteCodeLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

	switch (m_eType)
	{
	case SPHERE_TYPE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, _pArg);
		break;
	case AABB_TYPE:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, _pArg);
		break;
	case OBB_TYPE:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, _pArg);
		break;
	}

	return S_OK;
}

HRESULT CCollider::InitializeClone(void * pArg)
{
	switch (m_eType)
	{
	case SPHERE_TYPE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pArg);
		break;
	case AABB_TYPE:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pArg);
		break;
	case OBB_TYPE:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pArg);
		break;
	}

	return S_OK;
}

void CCollider::Tick(_float4x4 _WorldMatrix)
{
	_float4x4 MatRotX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(m_vRotation.x));
	_float4x4 MatRotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(m_vRotation.y));
	_float4x4 MatRotZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(m_vRotation.z));

	_float4x4 MatResult = MatRotX * MatRotY * MatRotZ * _WorldMatrix;

	m_pBounding->Tick(MatResult);
}

bool CCollider::Intersect(CCollider * pTargetCollider)
{
	return m_pBounding->Intersect(pTargetCollider->m_eType, pTargetCollider->m_pBounding);
}

void CCollider::Set_Center(_float3 vCenter)
{
	m_pBounding->Set_Center(vCenter);
}

void CCollider::Set_Extents(_float3 vExtents)
{
	static_pointer_cast<CBounding_OBB>(m_pBounding)->Set_Extents(vExtents);
}

void CCollider::Set_Radius(_float fRadius)
{
	static_pointer_cast<CBounding_Sphere>(m_pBounding)->Set_Radius(fRadius);
}

_float3 CCollider::Get_Center()
{
	return m_pBounding->Get_Center();
}

_float3 CCollider::Get_Extents()
{
	return static_pointer_cast<CBounding_OBB>(m_pBounding)->Get_Extents();
}

_float CCollider::Get_Radius()
{
	return static_pointer_cast<CBounding_Sphere>(m_pBounding)->Get_Radius();
}

void CCollider::Set_Rotation(_float3 vRotation)
{
	m_vRotation = vRotation;
}

HRESULT CCollider::Render()
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout.Get());

	m_pEffect->Apply(m_pContext.Get());

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch);

	m_pBatch->End();

	return S_OK;
}