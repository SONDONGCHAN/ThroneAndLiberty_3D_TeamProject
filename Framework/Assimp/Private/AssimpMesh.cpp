#include "AssimpMesh.h"
#include <fstream>

CAssimpMesh::CAssimpMesh(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	:CVIBuffer(_pDevice, _pContext)
{
}

CAssimpMesh::~CAssimpMesh()
{
}

HRESULT CAssimpMesh::Initialize(_uint _iAnimFlag, const aiMesh* pAIMesh, shared_ptr<CAssimpModel> _pModel, _float4x4 PivotMatrix)
{
	m_pAIMesh = pAIMesh;
	m_pModel = _pModel;

	strcpy_s(m_szName, pAIMesh->mName.data);

	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;

	m_iIndexStride = 4;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	//mNumFaces : 면의 갯수 
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/////////////Vertex Buffer///////////////
	HRESULT hr = _iAnimFlag == 1 ? ReadyVertexBufferNonAnim(pAIMesh, PivotMatrix) : ReadyVertexBufferAnim(pAIMesh, _pModel);

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

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++) {

		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];

	}

	m_InitialData.pSysMem = pIndices;

	//if (FAILED(__super::CreateBuffer(&m_pIB)))
	//	return E_FAIL;

	delete[] pIndices;

	return S_OK;
}

void CAssimpMesh::SetUpBoneMatrices(_float4x4* _pBoneMatirces, vector<shared_ptr<class CBone>>& _Bones)
{
}

HRESULT CAssimpMesh::ReadyVertexBufferNonAnim(const aiMesh* _pAIMesh, _float4x4 PivotMatrix)
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

	for (size_t i = 0; i < m_iNumVertices; i++) {

		memcpy(&pVertices[i].vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));

	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


	return S_OK;
}

HRESULT CAssimpMesh::ReadyVertexBufferAnim(const aiMesh* _pAIMesh, shared_ptr<CAssimpModel> _pModel)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++) {

		memcpy(&m_pVertices[i].vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&m_pVertices[i].vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&m_pVertices[i].vTexcoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&m_pVertices[i].vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));

	}

	//메시의 정점들의 상태에 영향을 주는 뼈들을 가져옴

	m_iNumBones = _pAIMesh->mNumBones;


	for (size_t i = 0; i < m_iNumBones; i++) {

		aiBone* pAIBone = _pAIMesh->mBones[i];
		_float4x4 OffsetMatrix = {};

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);
		m_BoneIndices.push_back(_pModel->GetBoneIndex(pAIBone->mName.data));

		//최대 4개까지 영향을 받을 수 있음 
		//Weight는 0이 될 수 없기 때문에 weights가 0일때 값이 채워지지 않음을 알 수 있다
		//한번만 채워주고 빠져나가야하기 때문에 else if문으로 만들어준다
		for (size_t j = 0; j < pAIBone->mNumWeights; j++) {
			if (0.f == m_pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x) {

				m_pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = (_uint)i;
				m_pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;

			}
			else if (0.f == m_pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y) {

				m_pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = (_uint)i;
				m_pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;

			}
			else if (0.f == m_pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z) {

				m_pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = (_uint)i;
				m_pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;

			}
			else if (0.f == m_pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w) {

				m_pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = (_uint)i;
				m_pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;

			}
		}
	}

	//본이 없는 sword, shadow 같은 메쉬용
	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;
		m_BoneIndices.push_back(_pModel->GetBoneIndex(m_szName));

		_float4x4 OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	m_InitialData.pSysMem = m_pVertices;

	//if (FAILED(__super::CreateBuffer(&m_pVB)))
	//	return E_FAIL;

	//Safe_Delete_Array(pVertices);

	return S_OK;
}

void CAssimpMesh::ParsingMeshData(HANDLE _handle)
{
	_ulong dwByte = { 0 };

	size_t NameSize = sizeof(m_szName);

	//다음에 읽을 이름 Char 배열의 크기
	WriteFile(_handle, m_szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);

	WriteFile(_handle, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

	WriteFile(_handle, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	//m_iNumIndices자체를 저장하기 
	WriteFile(_handle, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);


	for (size_t i = 0; i < m_iNumVertices; i++) {

		WriteFile(_handle, &m_pAIMesh->mVertices[i], sizeof(_float3), &dwByte, nullptr);
		WriteFile(_handle, &m_pAIMesh->mNormals[i], sizeof(_float3), &dwByte, nullptr);
		WriteFile(_handle, &m_pAIMesh->mTextureCoords[0][i], sizeof(_float2), &dwByte, nullptr);
		WriteFile(_handle, &m_pAIMesh->mTangents[i], sizeof(_float3), &dwByte, nullptr);
	}

	_uint mFace = m_pAIMesh->mNumFaces;
	WriteFile(_handle, &mFace, sizeof(_uint), &dwByte, nullptr);


	for (size_t i = 0; i < mFace; i++) {

		_uint ind1 = m_pAIMesh->mFaces[i].mIndices[0];
		_uint ind2 = m_pAIMesh->mFaces[i].mIndices[1];
		_uint ind3 = m_pAIMesh->mFaces[i].mIndices[2];

		WriteFile(_handle, &ind1, sizeof(_uint), &dwByte, nullptr);
		WriteFile(_handle, &ind2, sizeof(_uint), &dwByte, nullptr);
		WriteFile(_handle, &ind3, sizeof(_uint), &dwByte, nullptr);


	}
}

void CAssimpMesh::ParsingAnimMeshDatat(ofstream& out)
{
	_ulong dwByte = { 0 };

	size_t NameSize = sizeof(m_szName);


	out.write((char*)m_szName, sizeof(char) * MAX_PATH);
	out.write((char*)&m_iMaterialIndex, sizeof(_uint));
	out.write((char*)&m_iNumVertices, sizeof(_uint));
	out.write((char*)&m_iNumIndices, sizeof(_uint));


	for (size_t i = 0; i < m_iNumVertices; i++) {

		out.write((char*)&m_pAIMesh->mVertices[i], sizeof(_float3));
		out.write((char*)&m_pAIMesh->mNormals[i], sizeof(_float3));
		out.write((char*)&m_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		out.write((char*)&m_pAIMesh->mTangents[i], sizeof(_float3));

	}

	out.write((char*)&m_iNumBones, sizeof(_uint));

	for (size_t i = 0; i < m_iNumBones; i++) {
		_int idxNum = m_pModel->GetBoneIndex(m_pAIMesh->mBones[i]->mName.data);

		out.write((char*)&m_OffsetMatrices[i], sizeof(_float4x4));
		out.write((char*)&idxNum, sizeof(_int));

	}

	for (size_t i = 0; i < m_iNumVertices; ++i) {

		out.write((char*)&m_pVertices[i], sizeof(VTXANIMMESH));

	}

	_uint mFace = m_pAIMesh->mNumFaces;

	out.write((char*)&mFace, sizeof(_uint));


	for (size_t i = 0; i < mFace; i++) {

		_uint ind1 = m_pAIMesh->mFaces[i].mIndices[0];
		_uint ind2 = m_pAIMesh->mFaces[i].mIndices[1];
		_uint ind3 = m_pAIMesh->mFaces[i].mIndices[2];


		out.write((char*)&ind1, sizeof(_uint));
		out.write((char*)&ind2, sizeof(_uint));
		out.write((char*)&ind3, sizeof(_uint));


	}
}

shared_ptr<CAssimpMesh> CAssimpMesh::Create(_uint _iAnimFlag, wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, const aiMesh* pAIMesh, shared_ptr<CAssimpModel> _pModel, _float4x4 PivotMatrix)
{
	shared_ptr<CAssimpMesh> pInstance = make_shared<CAssimpMesh>(nullptr, nullptr);

	pInstance->Initialize(_iAnimFlag, pAIMesh, _pModel, PivotMatrix);

    return pInstance;
}
