#include "RenderTarget.h"
#include "GameInstance.h"

CRenderTarget::CRenderTarget(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice(_pDevice),
    m_pContext(_pContext)
{
}

HRESULT CRenderTarget::Initialize(_uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
    m_vClearColor = _vClearColor;

    D3D11_TEXTURE2D_DESC TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = _iSizeX;
    TextureDesc.Height = _iSizeY;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = _ePixelFormat;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE |
        D3D11_BIND_UNORDERED_ACCESS;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 1;

    D3D11_SHADER_RESOURCE_VIEW_DESC tSrvDesc{};
    tSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    tSrvDesc.Texture2D.MostDetailedMip = 0;
    tSrvDesc.Texture2D.MipLevels = 1;

    D3D11_RENDER_TARGET_VIEW_DESC tRtvDesc{};
    tRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    tRtvDesc.Texture2D.MipSlice = 0;

    D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc{};
    tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D.Get(), &tRtvDesc, &m_pRTV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D.Get(), &tSrvDesc, &m_pSRV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pTexture2D.Get(), &tUAVDesc, &m_pUAV)))
        return E_FAIL;

    return S_OK;

}


HRESULT CRenderTarget::InitializeTex2DArr(_uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor, _uint _iArrSize)
{

    m_IsDepthTex = true;
    m_vClearColor = _vClearColor;

    D3D11_TEXTURE2D_DESC TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
    

     TextureDesc.Width = _iSizeX;
    TextureDesc.Height = _iSizeY;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = _iArrSize;
    TextureDesc.Format = _ePixelFormat;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
        return E_FAIL;

    D3D11_RENDER_TARGET_VIEW_DESC RTVdesc;
    RTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    RTVdesc.Format = _ePixelFormat;
    RTVdesc.Texture2DArray.ArraySize = _iArrSize;
    RTVdesc.Texture2DArray.FirstArraySlice = 0;
    RTVdesc.Texture2DArray.MipSlice = 0;

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D.Get(), &RTVdesc, &m_pRTV)))
        return E_FAIL;

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;

    SRVDesc.Format = _ePixelFormat;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    SRVDesc.Texture2DArray.ArraySize = _iArrSize;
    SRVDesc.Texture2DArray.FirstArraySlice = 0;
    SRVDesc.Texture2DArray.MipLevels = 1;
    SRVDesc.Texture2DArray.MostDetailedMip = 0;


    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D.Get(), &SRVDesc, &m_pSRV)))
        return E_FAIL;

    return S_OK;

}

HRESULT CRenderTarget::InitializeTex3D(_uint _iSizeX, _uint _iSizeY, _uint _iDepth, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
    m_vClearColor = _vClearColor;

    D3D11_TEXTURE3D_DESC TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE3D_DESC));

    TextureDesc.Width = _iSizeX;
    TextureDesc.Height = _iSizeY;
    TextureDesc.Depth = _iDepth;
    TextureDesc.Format = _ePixelFormat;

    TextureDesc.MipLevels = 1;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 1;

    D3D11_SHADER_RESOURCE_VIEW_DESC tSrvDesc{};
    tSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
    tSrvDesc.Texture2D.MostDetailedMip = 0;
    tSrvDesc.Texture2D.MipLevels = 1;

    D3D11_RENDER_TARGET_VIEW_DESC tRtvDesc{};
    tRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
    tRtvDesc.Texture2D.MipSlice = 0;

    D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc{};
    tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;

    if (FAILED(m_pDevice->CreateTexture3D(&TextureDesc, nullptr, &m_pTexture3D)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture3D.Get(), &tRtvDesc, &m_pRTV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture3D.Get(), &tSrvDesc, &m_pSRV)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pTexture3D.Get(), &tUAVDesc, &m_pUAV)))
        return E_FAIL;

    return S_OK;
}


HRESULT CRenderTarget::Clear()
{
    //if (!m_IsDepthTex) {
        m_pContext->ClearRenderTargetView(m_pRTV.Get(), (_float*)&m_vClearColor);
   // }
    return S_OK;
}

HRESULT CRenderTarget::BindSRV(const _char* _pConstantName)
{
    return CGameInstance::GetInstance()->BindSRVDirect(_pConstantName, m_pSRV);
}

HRESULT CRenderTarget::BindUAV(const _char* _pConstantName)
{
    return CGameInstance::GetInstance()->BindUAV(_pConstantName, m_pUAV);
}

HRESULT CRenderTarget::CreateSRV()
{

    D3D11_SHADER_RESOURCE_VIEW_DESC tSrvDesc{};
    tSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    tSrvDesc.Texture2D.MostDetailedMip = 0;
    tSrvDesc.Texture2D.MipLevels = 1;


    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D.Get(), &tSrvDesc, &m_pSRV)))
        return E_FAIL;

    return S_OK;
}

//#ifdef _DEBUG
HRESULT CRenderTarget::ReadyDebug(_float _fX, _float _fY, _float _fSizeX, _float _fSizeY, const wstring& _strTargetName)
{
    D3D11_VIEWPORT ViewPortDesc{};

    _uint iNumViewport = 1;
    m_pContext->RSGetViewports(&iNumViewport, &ViewPortDesc);

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    m_WorldMatrix._11 = _fSizeX;
    m_WorldMatrix._22 = _fSizeY;
    m_WorldMatrix._41 = _fX - ViewPortDesc.Width * 0.5f;
    m_WorldMatrix._42 = -_fY + ViewPortDesc.Height * 0.5f;

    //For Display Target Name
    m_vTargetNamePos = {
        _fX - (_fSizeX* 0.5f) + 10.f,
        _fY - (_fSizeY * 0.5f)
    };


    m_strTargetName = _strTargetName;
    m_strTargetName.erase(m_strTargetName.begin(), m_strTargetName.begin() + 7);

    return S_OK;
}

HRESULT CRenderTarget::Render()
{
    
    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(CGameInstance::GetInstance()->BindSRVDirect("g_Texture", m_pSRV)))
        return E_FAIL;

    CGameInstance::GetInstance()->BeginShader();


    GAMEINSTANCE->RenderFont("Font_DNFB14", m_strTargetName, m_vTargetNamePos, CCustomFont::FA_LEFT, { 1.f, 1.f, 0.f, 1.f });


    return S_OK;
}
//#endif

shared_ptr<CRenderTarget> CRenderTarget::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, _uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
    shared_ptr<CRenderTarget> pInstance = make_shared<CRenderTarget>(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_iSizeX, _iSizeY, _ePixelFormat, _vClearColor)))
        MSG_BOX("Failed to Create : CRenderTarget");

    return pInstance;
}

shared_ptr<CRenderTarget> CRenderTarget::CreateTex2DArr(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, _uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor, _uint _iArrNum)
{
    shared_ptr<CRenderTarget> pInstance = make_shared<CRenderTarget>(_pDevice, _pContext);

    if (FAILED(pInstance->InitializeTex2DArr(_iSizeX, _iSizeY, _ePixelFormat, _vClearColor, _iArrNum)))
        MSG_BOX("Failed to Create : CRenderTarget");

    return pInstance;
}


void CRenderTarget::Free()
{
}
