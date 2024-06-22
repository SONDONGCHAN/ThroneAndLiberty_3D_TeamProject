#include "VIInstanceUI.h"

CVIInstanceUI::CVIInstanceUI(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	: Super(_pDevice, _pContext)
{

}

CVIInstanceUI::CVIInstanceUI(const shared_ptr<CVIInstancing> _rhs)
	: Super(_rhs)
{

}

CVIInstanceUI::~CVIInstanceUI()
{

}

HRESULT CVIInstanceUI::Initialize(CVIInstancing::InstanceDesc& _instanceUI, vector<_float2>& _uiPos)
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	m_InstanceData.vCenter = _instanceUI.vCenter;
	m_InstanceData.vPivot = _instanceUI.vPivot;
	m_InstanceData.vSize = _instanceUI.vSize;
	m_InstanceData.vColor = _instanceUI.vColor;
	m_InstanceData.vLifeTime = _instanceUI.vLifeTime;
	m_InstanceData.isLoop = _instanceUI.isLoop;
	m_InstanceData.vSpeed = _instanceUI.vSpeed;
	m_InstanceData.fDuration = _instanceUI.fDuration;
	m_InstanceData.fSizeY = _instanceUI.fSizeY;
	//strcpy_s(m_InstanceUIData.strTexTag, sizeof(m_InstanceUIData.strTexTag), _instanceUI.strTexTag);

	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;

	m_iNumInstance = static_cast<_uint>(_uiPos.size());
	m_iIndexCountPerInstance = 1;
	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region Instance Buffer

	m_iInstanceStride = sizeof(VTXUI);

	/* 정점버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	vector<VTXUI> instances;
	instances.resize(m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		instances[i].vRight = _float4(_instanceUI.vSize.x, 0.f, 0.f, 0.f);
		instances[i].vUp = _float4(0.f, _instanceUI.fSizeY, 0.f, 0.f);
		instances[i].vLook = _float4(0.f, 0.f, _instanceUI.vSize.x, 0.f);
		instances[i].vTranslation = _float4(_uiPos[i].x, _uiPos[i].y, 0.f, 1.f);

		instances[i].vColor = _instanceUI.vColor;

		instances[i].iSlotUV = 2;
		instances[i].vUVRatio = _float2(0.f, 0.f);
		instances[i].iTexIndex = 0;
	}

	m_InitialData.pSysMem = instances.data();

	if (FAILED(Super::CreateBuffer(&m_pVBInstance)))
		return E_FAIL;

#pragma endregion

#pragma region Vertex Buffer

	m_iVertexStride = sizeof(VTXPOINT);
	
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	vector<VTXPOINT> vertices;
	vertices.resize(m_iNumVertices);

	vertices[0].vPosition = _float3(0.f, 0.f, 0.f);
	vertices[0].vPSize = _float2(1.f, 1.f);

	m_InitialData.pSysMem = vertices.data();

	if (FAILED(Super::CreateBuffer(&m_pVB)))
		return E_FAIL;

#pragma endregion

#pragma region Index Buffer

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	vector<_ushort> indices;
	indices.resize(m_iNumIndices, 0);

	m_InitialData.pSysMem = indices.data();

	if (FAILED(Super::CreateBuffer(&m_pIB)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CVIInstanceUI::InitializeClone(_uint _iNumInstance, void* _pArg)
{
	if (FAILED(Super::InitializeClone(_pArg)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CVIInstanceUI> CVIInstanceUI::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, CVIInstancing::InstanceDesc& _instanceUI, vector<_float2>& _uiPos)
{
	shared_ptr<CVIInstanceUI> pInstance = make_shared<CVIInstanceUI>(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_instanceUI, _uiPos)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CVIInstanceUI::Create", MB_OK);

		pInstance.reset();

		return nullptr;
	}

	return pInstance;
}

shared_ptr<CComponent> CVIInstanceUI::Clone(_uint _iNumInstance, void* _pArg)
{
	return shared_ptr<CComponent>();
}
