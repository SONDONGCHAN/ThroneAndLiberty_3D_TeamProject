#include "VITerrain.h"

#include "Texture.h"

#include "QuadTree.h"

#include <fstream>

CVITerrain::CVITerrain(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

HRESULT CVITerrain::Initialize(ifstream& _In)
{
	// 1. 버텍스 X, Y 사이즈
	_In.read(reinterpret_cast<char*>(&m_iNumVerticesX), sizeof(_uint));
	_In.read(reinterpret_cast<char*>(&m_iNumVerticesZ), sizeof(_uint));

	// 2. 버텍스 정보
	_In.read(reinterpret_cast<char*>(&m_iNumVertices), sizeof(_uint));
	m_vecVertices.resize(m_iNumVertices);
	for (_uint iCntVtx = 0; iCntVtx < m_iNumVertices; iCntVtx++)
	{
		VTXNORTEX Vertex{};
		_In.read(reinterpret_cast<char*>(&Vertex), sizeof(VTXNORTEX));

		m_vecVertices[iCntVtx] = Vertex;
	}

	// 3. 인덱스 정보
	_In.read(reinterpret_cast<char*>(&m_iNumIndices), sizeof(_uint));
	m_vecIndices.resize(m_iNumIndices);
	for (_uint iCntIdx = 0; iCntIdx < m_iNumIndices; iCntIdx++)
	{
		_uint iIndex{};
		_In.read(reinterpret_cast<char*>(&iIndex), sizeof(_uint));

		m_vecIndices[iCntIdx] = iIndex;
	}

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXNORTEX);

	m_iIndexStride = 4;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_InitialData.pSysMem = m_vecVertices.data();

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_InitialData.pSysMem = m_vecIndices.data();

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	_uint iVerticesSize = static_cast<_uint>(m_vecVertices.size());

	m_verticesPos.resize(iVerticesSize);

	for (_uint i = 0; i < iVerticesSize; i++)
		m_verticesPos[i] = m_vecVertices[i].vPosition;

	//m_pQuadTree = CQuadTree::Create(static_pointer_cast<CVITerrain>(shared_from_this()),
	//	m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX,
	//	m_iNumVerticesX * m_iNumVerticesZ - 1,
	//	m_iNumVerticesX - 1,
	//	0);

	//m_pQuadTree->Make_Neighbors();

	return S_OK;
}

HRESULT CVITerrain::Initialize(array<_int, AXIS_END> _arrSize)
{
	// 문자열 변환

	m_iNumVerticesX = _arrSize[AXIS_X];
	m_iNumVerticesZ = _arrSize[AXIS_Z];

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iIndexStride = 4;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_vecVertices.resize(m_iNumVertices);
	fill(m_vecVertices.begin(), m_vecVertices.end(), VTXNORTEX{});

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;
			
			m_vecVertices[iIndex].vPosition = _float3(static_cast<_float>(j), static_cast<_float>(_arrSize[AXIS_Y]), static_cast<_float>(i));
			m_vecVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			m_vecVertices[iIndex].vTexcoord = _float2(static_cast<_float>(j) / (m_iNumVerticesX - 1.f), static_cast<_float>(i) / (m_iNumVerticesZ - 1.f));
		}
	}

	m_vecIndices.resize(m_iNumIndices);
	fill(m_vecIndices.begin(), m_vecIndices.end(), 0);

	_uint iNumIndices = { 0 };

	for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_float3 vSourDir, vDestDir, vNormal;

			m_vecIndices[iNumIndices++] = iIndices[0];
			m_vecIndices[iNumIndices++] = iIndices[1];
			m_vecIndices[iNumIndices++] = iIndices[2];

			vSourDir = m_vecVertices[iIndices[1]].vPosition - m_vecVertices[iIndices[0]].vPosition;
			vDestDir = m_vecVertices[iIndices[2]].vPosition - m_vecVertices[iIndices[1]].vPosition;
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			m_vecVertices[iIndices[0]].vNormal = XMVector3Normalize(m_vecVertices[iIndices[0]].vNormal + vNormal);
			m_vecVertices[iIndices[1]].vNormal = XMVector3Normalize(m_vecVertices[iIndices[1]].vNormal + vNormal);
			m_vecVertices[iIndices[2]].vNormal = XMVector3Normalize(m_vecVertices[iIndices[2]].vNormal + vNormal);

			m_vecIndices[iNumIndices++] = iIndices[0];
			m_vecIndices[iNumIndices++] = iIndices[2];
			m_vecIndices[iNumIndices++] = iIndices[3];

			vSourDir = m_vecVertices[iIndices[2]].vPosition - m_vecVertices[iIndices[0]].vPosition;
			vDestDir = m_vecVertices[iIndices[3]].vPosition - m_vecVertices[iIndices[2]].vPosition;
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			m_vecVertices[iIndices[0]].vNormal = XMVector3Normalize(m_vecVertices[iIndices[0]].vNormal + vNormal);
			m_vecVertices[iIndices[2]].vNormal = XMVector3Normalize(m_vecVertices[iIndices[2]].vNormal + vNormal);
			m_vecVertices[iIndices[3]].vNormal = XMVector3Normalize(m_vecVertices[iIndices[3]].vNormal + vNormal);
		}
	}

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_InitialData.pSysMem = m_vecVertices.data();

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_InitialData.pSysMem = m_vecIndices.data();

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	_uint iVerticesSize = static_cast<_uint>(m_vecVertices.size());

	m_verticesPos.resize(iVerticesSize);

	for (_uint i = 0; i < iVerticesSize; i++)
		m_verticesPos[i] = m_vecVertices[i].vPosition;

	//m_pQuadTree = CQuadTree::Create(static_pointer_cast<CVITerrain>(shared_from_this()),
	//	m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX,
	//	m_iNumVerticesX * m_iNumVerticesZ - 1,
	//	m_iNumVerticesX - 1,
	//	0);

	//m_pQuadTree->Make_Neighbors();

	return S_OK;
}

HRESULT CVITerrain::Initialize(vector<_float3>& _vecPos, _uint _iSize)
{
	// 문자열 변환

	m_iNumVerticesX = _iSize;
	m_iNumVerticesZ = _iSize;

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iIndexStride = 4;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_vecVertices.resize(m_iNumVertices);
	fill(m_vecVertices.begin(), m_vecVertices.end(), VTXNORTEX{});

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			m_vecVertices[iIndex].vPosition = _float3(static_cast<_float>(j), _vecPos[iIndex].y, static_cast<_float>(i));
			m_vecVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			m_vecVertices[iIndex].vTexcoord = _float2(static_cast<_float>(j) / (m_iNumVerticesX - 1.f), static_cast<_float>(i) / (m_iNumVerticesZ - 1.f));
		}
	}

	m_vecIndices.resize(m_iNumIndices);
	fill(m_vecIndices.begin(), m_vecIndices.end(), 0);

	_uint iNumIndices = { 0 };

	for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_float3 vSourDir, vDestDir, vNormal;

			m_vecIndices[iNumIndices++] = iIndices[0];
			m_vecIndices[iNumIndices++] = iIndices[1];
			m_vecIndices[iNumIndices++] = iIndices[2];

			vSourDir = m_vecVertices[iIndices[1]].vPosition - m_vecVertices[iIndices[0]].vPosition;
			vDestDir = m_vecVertices[iIndices[2]].vPosition - m_vecVertices[iIndices[1]].vPosition;
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			m_vecVertices[iIndices[0]].vNormal = XMVector3Normalize(m_vecVertices[iIndices[0]].vNormal + vNormal);
			m_vecVertices[iIndices[1]].vNormal = XMVector3Normalize(m_vecVertices[iIndices[1]].vNormal + vNormal);
			m_vecVertices[iIndices[2]].vNormal = XMVector3Normalize(m_vecVertices[iIndices[2]].vNormal + vNormal);

			m_vecIndices[iNumIndices++] = iIndices[0];
			m_vecIndices[iNumIndices++] = iIndices[2];
			m_vecIndices[iNumIndices++] = iIndices[3];

			vSourDir = m_vecVertices[iIndices[2]].vPosition - m_vecVertices[iIndices[0]].vPosition;
			vDestDir = m_vecVertices[iIndices[3]].vPosition - m_vecVertices[iIndices[2]].vPosition;
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			m_vecVertices[iIndices[0]].vNormal = XMVector3Normalize(m_vecVertices[iIndices[0]].vNormal + vNormal);
			m_vecVertices[iIndices[2]].vNormal = XMVector3Normalize(m_vecVertices[iIndices[2]].vNormal + vNormal);
			m_vecVertices[iIndices[3]].vNormal = XMVector3Normalize(m_vecVertices[iIndices[3]].vNormal + vNormal);
		}
	}

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_InitialData.pSysMem = m_vecVertices.data();

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_InitialData.pSysMem = m_vecIndices.data();

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	_uint iVerticesSize = static_cast<_uint>(m_vecVertices.size());

	m_verticesPos.resize(iVerticesSize);

	for (_uint i = 0; i < iVerticesSize; i++)
		m_verticesPos[i] = m_vecVertices[i].vPosition;

	//m_pQuadTree = CQuadTree::Create(static_pointer_cast<CVITerrain>(shared_from_this()),
	//	m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX,
	//	m_iNumVerticesX * m_iNumVerticesZ - 1,
	//	m_iNumVerticesX - 1,
	//	0);

	//m_pQuadTree->Make_Neighbors();

	return S_OK;
}

void CVITerrain::UpdateHight(vector<_uint>& _vecEditableIndices, _float3 _vPickedLocalPos, _float _fRadius, _float _fPower, _float _fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXNORTEX* vertices = reinterpret_cast<VTXNORTEX*>(SubResource.pData);

	for (const auto& iIndex : _vecEditableIndices)
	{
		_float fDistance = (vertices[iIndex].vPosition - _vPickedLocalPos).Length();

		if (fDistance <= _fRadius)
		{
			_float fAcc = (_fRadius - fDistance) / _fRadius;
			//fAcc = pow(fAcc, (1.f / _fPower));

			_float fWeight = _fPower * fAcc;

			m_verticesPos[iIndex].y
				= m_vecVertices[iIndex].vPosition.y
				= vertices[iIndex].vPosition.y += (fWeight * _fTimeDelta);
		}
	}

	

	m_pContext->Unmap(m_pVB.Get(), 0);
}

void CVITerrain::UpdateHightFlat(vector<_uint>& _vecEditableIndices, _float3 _vPickedLocalPos, _float _fRadius, _float _fPivotY)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXNORTEX* vertices = reinterpret_cast<VTXNORTEX*>(SubResource.pData);

	for (const auto& iIndex : _vecEditableIndices)
	{
		_float fDistance = (vertices[iIndex].vPosition - _vPickedLocalPos).Length();

		if (fDistance <= _fRadius)
		{
			m_verticesPos[iIndex].y
				= m_vecVertices[iIndex].vPosition.y
				= vertices[iIndex].vPosition.y = _fPivotY;
		}
	}



	m_pContext->Unmap(m_pVB.Get(), 0);
}

void CVITerrain::Search(vector<_uint>& _vecIndices, _float2& _vRayPos, _float2& _vRayDir, _float& _fRaySlope, _float& _fRayIntercept)
{
	m_pQuadTree->Search(_vecIndices, _vRayPos, _vRayDir, _fRaySlope, _fRayIntercept);
}

void CVITerrain::SaveBinary(ofstream& _Out)
{
	_Out.write(reinterpret_cast<const char*>(&m_iNumVerticesX), sizeof(_uint));
	_Out.write(reinterpret_cast<const char*>(&m_iNumVerticesZ), sizeof(_uint));

	_uint iNumVertices = static_cast<_uint>(m_vecVertices.size());
	_Out.write(reinterpret_cast<const char*>(&iNumVertices), sizeof(_uint));
	_Out.write(reinterpret_cast<const char*>(m_vecVertices.data()), sizeof(VTXNORTEX) * iNumVertices);

	_uint iNumIndices = static_cast<_uint>(m_vecIndices.size());
	_Out.write(reinterpret_cast<const char*>(&iNumIndices), sizeof(_uint));
	_Out.write(reinterpret_cast<const char*>(m_vecIndices.data()), sizeof(_uint) * iNumIndices);
}

shared_ptr<CVITerrain> CVITerrain::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext, ifstream& _In)
{
	shared_ptr<CVITerrain> pInstance = make_shared<CVITerrain>(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_In)))
	{
		MSG_BOX("Failed to Created : CVITerrain");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CVITerrain> CVITerrain::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext, array<_int, AXIS_END> _arrSize)
{
	shared_ptr<CVITerrain> pInstance = make_shared<CVITerrain>(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_arrSize)))
	{
		MSG_BOX("Failed to Created : CVITerrain");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CVITerrain> CVITerrain::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext, vector<_float3>& _vecPos, _uint _iSize)
{
	shared_ptr<CVITerrain> pInstance = make_shared<CVITerrain>(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_vecPos, _iSize)))
	{
		MSG_BOX("Failed to Created : CVITerrain");
		pInstance.reset();
	}

	return pInstance;
}
