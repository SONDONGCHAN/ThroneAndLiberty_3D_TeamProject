#include "VIInstancePoint.h"

CVIInstancePoint::CVIInstancePoint(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	: Super(_pDevice, _pContext)
{

}

CVIInstancePoint::CVIInstancePoint(const shared_ptr<CVIInstancing> _rhs)
	: Super(_rhs)
{

}

CVIInstancePoint::~CVIInstancePoint()
{

}

HRESULT CVIInstancePoint::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIInstancePoint::InitializeClone(_uint _iNumInstance, void* _pArg)
{
	if (FAILED(Super::InitializeClone(_pArg)))
		return E_FAIL;

	m_fScaleY = m_InstanceData.fSizeY;

	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;

	m_iNumInstance = _iNumInstance;
	m_iIndexCountPerInstance = 1;
	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER
	/* 정점버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	vector<VTXPOINT> vertices;
	vertices.resize(m_iNumVertices);

	vertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);
	vertices[0].vPSize = _float2(1.f, 1.f);

	m_InitialData.pSysMem = vertices.data();

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	vector<_ushort> indices;
	indices.resize(m_iNumIndices, 0);

	m_InitialData.pSysMem = indices.data();

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

#pragma endregion

#pragma region INSTANCE_BUFFER

	m_iInstanceStride = sizeof(VTXINSTANCE);

	/* 정점버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	vector<VTXINSTANCE> instances;
	instances.resize(m_iNumInstance);

	uniform_real_distribution<float>	WidthRange(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
	uniform_real_distribution<float>	HeightRange(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
	uniform_real_distribution<float>	DepthRange(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);
	uniform_real_distribution<float>	SizeRange(m_InstanceData.vSize.x, m_InstanceData.vSize.y);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_float		fSize = SizeRange(m_RandomNumber);

		instances[i].vRight = _float4(fSize, 0.f, 0.f, 0.f);

		if (m_fScaleY != 0.f)
			instances[i].vUp = _float4(0.f, m_fScaleY, 0.f, 0.f); /* 가로 세로 비율을 다르게 하고 싶을 때 */
		else
			instances[i].vUp = _float4(0.f, fSize, 0.f, 0.f);

		instances[i].vLook = _float4(0.f, 0.f, fSize, 0.f);
		instances[i].vTranslation = _float4(
			m_InstanceData.vCenter.x + WidthRange(m_RandomNumber),
			m_InstanceData.vCenter.y + HeightRange(m_RandomNumber),
			m_InstanceData.vCenter.z + DepthRange(m_RandomNumber),
			1.f);

		instances[i].vColor = m_InstanceData.vColor;
	}

	m_InitialData.pSysMem = instances.data();

	if (FAILED(Super::CreateBuffer(&m_pVBInstance)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

shared_ptr<CVIInstancePoint> CVIInstancePoint::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
{
	shared_ptr<CVIInstancePoint> pInstance = make_shared<CVIInstancePoint>(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CVIInstancePoint::Create", MB_OK);

		pInstance.reset();

		return nullptr;
	}

	return pInstance;
}

shared_ptr<CComponent> CVIInstancePoint::Clone(_uint _iNumInstance, void* _pArg)
{
	shared_ptr<CVIInstancePoint> pClone = make_shared<CVIInstancePoint>(static_pointer_cast<CVIInstancePoint>(shared_from_this()));

	if (FAILED(pClone->InitializeClone(_iNumInstance, _pArg)))
	{
		MessageBox(nullptr, L"InitializeClone Failed", L"CVIInstancePoint::Clone", MB_OK);

		pClone.reset();

		return nullptr;
	}

	return pClone;
}
