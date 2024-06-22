#include "VIBuffer.h"

CVIBuffer::CVIBuffer(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :CComponent(_pDevice, _pContext)
{
}

CVIBuffer::CVIBuffer(const shared_ptr<CVIBuffer> _rhs)
    :CComponent(_rhs)
    , m_pVB(_rhs->m_pVB)
    , m_pIB(_rhs->m_pIB)
    , m_iVertexStride(_rhs->m_iVertexStride)
    , m_iNumVertices(_rhs->m_iNumVertices)
    , m_iIndexStride(_rhs->m_iIndexStride)
    , m_iNumIndices(_rhs->m_iNumIndices)
    , m_iNumVertexBuffers(_rhs->m_iNumVertexBuffers)
    , m_eIndexFormat(_rhs->m_eIndexFormat)
    , m_ePrimitiveTopology(_rhs->m_ePrimitiveTopology)
{
}

HRESULT CVIBuffer::Initialize()
{
    return S_OK;
}

HRESULT CVIBuffer::Render()
{
    m_pContext->DrawIndexed(m_iNumIndices, 0, 0);
    return S_OK;
}

HRESULT CVIBuffer::BindBuffers()
{
    wrl::ComPtr<ID3D11Buffer> pVertexBuffers[] = {
       m_pVB,
    };

    _uint iVertexStrides[] = {
        m_iVertexStride,
    };

    _uint iOffsets[] = {
        0,
    };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers->GetAddressOf(), iVertexStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);


    return S_OK;
}

HRESULT CVIBuffer::CreateBuffer(wrl::ComPtr<ID3D11Buffer>* ppBuffer)
{
    return m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitialData, ppBuffer->GetAddressOf());
}