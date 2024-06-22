#include "VIInstanceMesh.h"
#include "Mesh.h"

CVIInstanceMesh::CVIInstanceMesh(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :CVIInstancing(_pDevice, _pContext)
{
}

CVIInstanceMesh::CVIInstanceMesh(const shared_ptr<CVIInstanceMesh> _rhs)
    :CVIInstancing(_rhs)
{
}

HRESULT CVIInstanceMesh::Initialize(shared_ptr<class CMesh> _pMesh, vector<_float4x4>* _pMatrixVec)
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    /*인스턴스 갯수만큼 매트릭스가 저장*/
    m_iNumInstance = (UINT)_pMatrixVec->size();

    memcpy(m_szName, _pMesh->GetMeshName(), sizeof(_char) * MAX_PATH);
    m_iMaterialIndex = _pMesh->GetMaterialIndex();

    m_iFaceNum = _pMesh->GetFaceNum();

    m_iIndexCountPerInstance = _pMesh->GetFaceNum() * 3;
    m_iNumVertexBuffers = 2;
    m_iNumVertices = _pMesh->GetVerticesNum();
    m_iIndexStride = { sizeof(_ulong) };
    m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region INSTANCE_BUFFER

    m_iInstanceStride = sizeof(VTXVERTEXINSTANCE);

    /* 정점버퍼를 만든다.*/
    m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
    m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_BufferDesc.StructureByteStride = m_iInstanceStride;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_BufferDesc.MiscFlags = 0;

    vector<VTXVERTEXINSTANCE> instances;
    instances.resize(m_iNumInstance);


    for (_uint i = 0; i < m_iNumInstance; ++i)
    {
        _float3 vRight = (*_pMatrixVec)[i].Right();
        _float3 vUp = (*_pMatrixVec)[i].Up();
        _float3 vLook = (*_pMatrixVec)[i].Forward();
        _float3 vPos = (*_pMatrixVec)[i].Translation();

        instances[i].vRight = _float4(vRight.x, vRight.y, vRight.z, 0.f);
        instances[i].vUp = _float4(vUp.x, vUp.y, vUp.z, 0.f);
        instances[i].vLook = _float4(vLook.x, vLook.y, vLook.z, 0.f);
        instances[i].vTranslation = _float4(vPos.x, vPos.y, vPos.z, 1.f);
    }

    m_InitialData.pSysMem = instances.data();

    if (FAILED(__super::CreateBuffer(&m_pVBInstance)))
        return E_FAIL;


#pragma endregion


#pragma region VERTEX_BUFFER

    m_iVertexStride = sizeof(VTXMESH);

    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;

    vector<VTXMESH> vertices;
    vertices.resize(m_iNumVertices);

    vector<VTXMESH> m_Elements= *(_pMesh->GetElements());

    for (size_t i = 0; i < m_Elements.size(); ++i) {
        vertices[i] = m_Elements[i];
    }

    m_InitialData.pSysMem = vertices.data();

    if (FAILED(__super::CreateBuffer(&m_pVB)))
        return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER

    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;

    vector<_ulong> indices;
    indices.resize(m_iNumIndices, 0);

    vector<_uint> pindices = *(_pMesh->GetIndices());
    
    _uint		iNumFaces = { 0 };

    for (_uint i = 0; i < m_iNumInstance; i++)
    {
        for (_uint i = 0; i < pindices.size(); i++)
        {
            indices[iNumFaces++] = pindices[i];
        }
    }


    //for (size_t i = 0; i < pindices.size(); ++i) {
    //    indices[i] = pindices[i];
    //}

    m_InitialData.pSysMem = indices.data();

    if (FAILED(__super::CreateBuffer(&m_pIB)))
        return E_FAIL;

#pragma endregion


    return S_OK;
}

HRESULT CVIInstanceMesh::InitializeClone(_uint _iNumInstance, void* _pArg)
{
    if (FAILED(__super::InitializeClone(_pArg)))
        return E_FAIL;


    return S_OK;
}

HRESULT CVIInstanceMesh::DiscardAndFill(_float4x4* _WorldMatrixArray, UINT _iArrayNum)
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);

	VTXVERTEXINSTANCE* vertices = reinterpret_cast<VTXVERTEXINSTANCE*>(subResource.pData);

    memcpy_s(vertices, sizeof(_float4x4) * _iArrayNum, _WorldMatrixArray, sizeof(_float4x4) * _iArrayNum);

    m_pContext->Unmap(m_pVBInstance.Get(), 0);

    m_iNumInstance = _iArrayNum;

    return S_OK;
}

shared_ptr<CVIInstanceMesh> CVIInstanceMesh::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, shared_ptr<class CMesh> _pModel, vector<_float4x4>* _pMatrixVec)
{
    shared_ptr<CVIInstanceMesh> pInstance = make_shared<CVIInstanceMesh>(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pModel, _pMatrixVec)))
        MSG_BOX("Failed to Create : CVIInstanceMesh");
    
    return pInstance;
}

shared_ptr<CComponent> CVIInstanceMesh::Clone(_uint _iNumInstance, void* _pArg)
{
    return shared_ptr<CComponent>();
}
