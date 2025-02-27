#include "VIPhysX.h"

CVIPhysX::CVIPhysX(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :CVIBuffer(_pDevice, _pContext)
{
}

HRESULT CVIPhysX::Initialize(CVIPhysX::DESC* _Desc)
{
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXCUBE);
	m_iNumVertices = 8;

	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = 36;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	/* 정점버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	_float3 vCubeSize = _Desc->_vSize;

	VTXCUBE* pVertices = new VTXCUBE[m_iNumVertices]{};

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f) * vCubeSize;
	pVertices[0].vTexcoord = _float3(-0.5f, 0.5f, -0.5f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f) * vCubeSize;
	pVertices[1].vTexcoord = _float3(0.5f, 0.5f, -0.5f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f) * vCubeSize;
	pVertices[2].vTexcoord = _float3(0.5f, -0.5f, -0.5f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f) * vCubeSize;
	pVertices[3].vTexcoord = _float3(-0.5f, -0.5f, -0.5f);

	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f) * vCubeSize;
	pVertices[4].vTexcoord = _float3(-0.5f, 0.5f, 0.5f);

	pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f) * vCubeSize;
	pVertices[5].vTexcoord = _float3(0.5f, 0.5f, 0.5f);

	pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f) * vCubeSize;
	pVertices[6].vTexcoord = _float3(0.5f, -0.5f, 0.5f);

	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f) * vCubeSize;
	pVertices[7].vTexcoord = _float3(-0.5f, -0.5f, 0.5f);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i].vPosition = XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), XMMatrixRotationQuaternion(_Desc->vOffSetQuat)) + XMLoadFloat3(&_Desc->vOffSetPosition);
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices]{};

	/* +X */
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

	/* -X */
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;

	/* +Y */
	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

	/* -Y */
	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

	/* +Z */
	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;

	/* -Z */
	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

shared_ptr<CVIPhysX> CVIPhysX::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, CVIPhysX::DESC* _Desc)
{
    shared_ptr<CVIPhysX> pInstance = make_shared<CVIPhysX>(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_Desc)))
        MSG_BOX("Failed to Create : CVIPhysX");

    return pInstance;
} 
