#include "VIRect.h"

CVIRect::CVIRect(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :CVIBuffer(_pDevice, _pContext)
{
}

CVIRect::CVIRect(const shared_ptr<CVIRect> _rhs)
    :CVIBuffer(_rhs)
{
}

HRESULT CVIRect::Initialize()
{
    m_iNumVertexBuffers = 1;
    m_iVertexStride = sizeof(VTXPOSTEX);
    m_iNumVertices = 4;

   
    m_iIndexStride = 2;
    m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    m_iNumIndices = 6;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    /****** Vertex Buffer ******/

    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;

    m_BufferDesc.StructureByteStride = m_iVertexStride;


    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

    pVertices[0].vPositon = _float3(-0.5f, 0.5f, 0.f);
    pVertices[0].vTexcoord = _float2(0.f, 0.f);

    pVertices[1].vPositon = _float3(0.5f, 0.5f, 0.f);
    pVertices[1].vTexcoord = _float2(1.f, 0.f);

    pVertices[2].vPositon = _float3(0.5f, -0.5f, 0.f);
    pVertices[2].vTexcoord = _float2(1.f, 1.f);

    pVertices[3].vPositon = _float3(-0.5f, -0.5f, 0.f);
    pVertices[3].vTexcoord = _float2(0.f, 1.f);

    m_InitialData.pSysMem = pVertices;


    if (FAILED(__super::CreateBuffer(&m_pVB)))
        return E_FAIL;

 
    delete[] pVertices;
    pVertices = nullptr;


    /****** Index Buffer ******/

    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;

    _ushort* pIndices = new _ushort[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;

    pIndices[3] = 0;
    pIndices[4] = 2;
    pIndices[5] = 3;

    m_InitialData.pSysMem = pIndices;

    if (FAILED(__super::CreateBuffer(&m_pIB)))
        return E_FAIL;

    delete[] pIndices;
    pIndices = nullptr;

    return S_OK;
}

shared_ptr<CVIRect> CVIRect::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
{
    shared_ptr<CVIRect> pInstance = make_shared<CVIRect>(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CVIRect");

    return pInstance;
}
