#include "Mesh.h"
#include "Bone.h"

#include <iostream>
#include <fstream>

CMesh::CMesh(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :CVIBuffer(_pDevice, _pContext)
{
}

HRESULT CMesh::Initialize(CModel::TYPE _eType, ifstream& _ifs, _float4x4 PivotMatrix)
{
	_ifs.read(m_szName, sizeof(char) * MAX_PATH);
	_ifs.read((char*)&m_iMaterialIndex, sizeof(_uint));
	_ifs.read((char*)&m_iNumVertices, sizeof(_uint));
	_ifs.read((char*)&m_iNumIndices, sizeof(_uint));

	m_iNumVertexBuffers = 1;

	m_iIndexStride = 4;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/////////////Vertex Buffer///////////////
	HRESULT hr = _eType == CModel::TYPE_NONANIM ? ReadyVertexBufferNonAnim(_ifs, PivotMatrix) : ReadyVertexBufferAnim(_ifs);

	if (FAILED(hr))
		return E_FAIL;

	/////////////Index Buffer///////////////
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint iNumIndices = { 0 };

	_uint iFaceNum;
	_ifs.read((char*)&iFaceNum, sizeof(_uint));


	_uint iIndex1, iIndex2, iIndex3;

	m_vIndicesNum.reserve(m_iNumIndices);
	m_iFaceNum = iFaceNum;

	for (size_t i = 0; i < iFaceNum; i++) {

		_ifs.read((char*)&iIndex1, sizeof(_uint));
		_ifs.read((char*)&iIndex2, sizeof(_uint));
		_ifs.read((char*)&iIndex3, sizeof(_uint));

		pIndices[iNumIndices++] = iIndex1;
		pIndices[iNumIndices++] = iIndex2;
		pIndices[iNumIndices++] = iIndex3;

		m_vIndicesNum.push_back(iIndex1);
		m_vIndicesNum.push_back(iIndex2);
		m_vIndicesNum.push_back(iIndex3);

	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	return S_OK;
}

void CMesh::SetUpBoneMatrices(_float4x4* _pBoneMatirces, vector<shared_ptr<class CBone>>& _Bones)
{
	_float4x4 CombinedMat;

	for (size_t i = 0; i < m_iNumBones; i++) {
		CombinedMat = _Bones[m_BoneIndices[i]]->GetCombiendTransformationMatrix();
		XMStoreFloat4x4(&_pBoneMatirces[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(&CombinedMat));
	}
}

HRESULT CMesh::ReadyVertexBufferNonAnim(ifstream& _ifs, _float4x4 PivotMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_verticesPos.reserve(m_iNumVertices);
	m_meshElements.reserve(m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++) {

		_ifs.read((char*)&pVertices[i].vPosition, sizeof(_float3));
		_ifs.read((char*)&pVertices[i].vNormal, sizeof(_float3));
		_ifs.read((char*)&pVertices[i].vTexcoord, sizeof(_float2));
		_ifs.read((char*)&pVertices[i].vTangent, sizeof(_float3));

		m_verticesPos.push_back(pVertices[i].vPosition);
		m_meshElements.push_back(pVertices[i]);
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	delete[] pVertices;

	return S_OK;
}

HRESULT CMesh::ReadyVertexBufferAnim(ifstream& _ifs)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++) {
		_ifs.read((char*)&pVertices[i].vPosition, sizeof(_float3));
		_ifs.read((char*)&pVertices[i].vNormal, sizeof(_float3));
		_ifs.read((char*)&pVertices[i].vTexcoord, sizeof(_float2));
		_ifs.read((char*)&pVertices[i].vTangent, sizeof(_float3));

		m_verticesPos.push_back(pVertices[i].vPosition);


	}

	//메시의 정점들의 상태에 영향을 주는 뼈들을 가져옴

	_ifs.read((char*)&m_iNumBones, sizeof(_uint));

	_float	a1{}, a2{}, a3{}, a4{},
			a5{}, a6{}, a7{}, a8{},
			a9{}, a10{}, a11{}, a12{},
			a13{}, a14{}, a15{}, a16{};


	for (size_t i = 0; i < m_iNumBones; i++) {

		_float4x4 OffsetMatrix = {};

		_ifs.read((char*)&a1, sizeof(_float));
		_ifs.read((char*)&a2, sizeof(_float));
		_ifs.read((char*)&a3, sizeof(_float));
		_ifs.read((char*)&a4, sizeof(_float));
		_ifs.read((char*)&a5, sizeof(_float));
		_ifs.read((char*)&a6, sizeof(_float));
		_ifs.read((char*)&a7, sizeof(_float));
		_ifs.read((char*)&a8, sizeof(_float));
		_ifs.read((char*)&a9, sizeof(_float));
		_ifs.read((char*)&a10, sizeof(_float));
		_ifs.read((char*)&a11, sizeof(_float));
		_ifs.read((char*)&a12, sizeof(_float));
		_ifs.read((char*)&a13, sizeof(_float));
		_ifs.read((char*)&a14, sizeof(_float));
		_ifs.read((char*)&a15, sizeof(_float));
		_ifs.read((char*)&a16, sizeof(_float));



		OffsetMatrix._11 = a1;
		OffsetMatrix._12 = a2;
		OffsetMatrix._13 = a3;
		OffsetMatrix._14 = a4;
		OffsetMatrix._21 = a5;
		OffsetMatrix._22 = a6;
		OffsetMatrix._23 = a7;
		OffsetMatrix._24 = a8;
		OffsetMatrix._31 = a9;
		OffsetMatrix._32 = a10;
		OffsetMatrix._33 = a11;
		OffsetMatrix._34 = a12;
		OffsetMatrix._41 = a13;
		OffsetMatrix._42 = a14;
		OffsetMatrix._43 = a15;
		OffsetMatrix._44 = a16;


		m_OffsetMatrices.push_back(OffsetMatrix);

		_int iIdxNum;
		_ifs.read((char*)&iIdxNum, sizeof(_int));

		m_BoneIndices.push_back(iIdxNum);

	}

	for (size_t i = 0; i < m_iNumVertices; i++) {
		_ifs.read((char*)&pVertices[i], sizeof(VTXANIMMESH));
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


	return S_OK;
}

shared_ptr<CMesh> CMesh::Create(CModel::TYPE _eType, wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, ifstream& _ifs, _float4x4 PivotMatrix)
{
	shared_ptr<CMesh> pInstacne = make_shared<CMesh>(_pDevice, _pContext);

	if (FAILED(pInstacne->Initialize(_eType, _ifs, PivotMatrix)))
		MSG_BOX("Failed to Create : CMesh");

    return pInstacne;
}
