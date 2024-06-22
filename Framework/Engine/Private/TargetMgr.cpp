#include "TargetMgr.h"
#include "RenderTarget.h"
#include "Shader.h"

#include "GameInstance.h"

CTargetMgr::CTargetMgr(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice(_pDevice),
    m_pContext(_pContext)
{
}
//For. Pixel Picking
ComPtr<ID3D11Texture2D> CTargetMgr::GetTexture2DForTarget(const wstring& _strTargetTag)
{
    shared_ptr<CRenderTarget> pRenderTarget = FindRenderTarget(_strTargetTag);

    if (!pRenderTarget)
        return nullptr;

    return pRenderTarget->GetTexture2D();
}

HRESULT CTargetMgr::Initialize()
{
    return S_OK;
}

HRESULT CTargetMgr::AddRenderTarget(const wstring& _strTargetTag, _uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
    if (FindRenderTarget(_strTargetTag))
        return E_FAIL;

    shared_ptr<CRenderTarget> pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, _iSizeX, _iSizeY, _ePixelFormat, _vClearColor);

    if (!pRenderTarget)
        return E_FAIL;

    m_RenderTargets.emplace(_strTargetTag, pRenderTarget);

    return S_OK;
}

HRESULT CTargetMgr::AddRenderTargetTex2DArr(const wstring& _strTargetTag, _uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor, _uint _iArrSize)
{
    if (FindRenderTarget(_strTargetTag))
        return E_FAIL;

    shared_ptr<CRenderTarget> pRenderTarget = CRenderTarget::CreateTex2DArr(m_pDevice, m_pContext, _iSizeX, _iSizeY, _ePixelFormat, _vClearColor, _iArrSize);

    if (!pRenderTarget)
        return E_FAIL;

    m_RenderTargets.emplace(_strTargetTag, pRenderTarget);

    return S_OK;
}

HRESULT CTargetMgr::AddMRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
    shared_ptr<CRenderTarget> pRenderTarget = FindRenderTarget(strTargetTag);

    if (!pRenderTarget)
        return E_FAIL;

    list<shared_ptr<CRenderTarget>>* pMRTList = FindMRT(strMRTTag);

    if (!pMRTList) {

        list<shared_ptr<CRenderTarget>> MRTList;
        MRTList.push_back(pRenderTarget);

        m_MRTs.emplace(strMRTTag, MRTList);

    }
    else {

        pMRTList->push_back(pRenderTarget);
    }

    return S_OK;
}

HRESULT CTargetMgr::BeginMRT(const wstring& strMRTTag)
{
    ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{ nullptr };

    m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

    ID3D11UnorderedAccessView* ppNullUAV[] = { nullptr };
    m_pContext->CSSetUnorderedAccessViews(0, 1, ppNullUAV, nullptr);

    list<shared_ptr<CRenderTarget>>* pMRTList = FindMRT(strMRTTag);
    ID3D11RenderTargetView* pRTVs[8] = { nullptr };

    m_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

    _uint iNumRTVs = { 0 };
    
    for (auto& pRenderTarget : *pMRTList) {
        pRenderTarget->Clear();
        pRTVs[iNumRTVs++] = pRenderTarget->GetRTV().Get();
    }

    m_pContext->OMSetRenderTargets(iNumRTVs, pRTVs, m_pDepthStencilView.Get());
   

    return S_OK;
}

HRESULT CTargetMgr::BeginMRT(const wstring& strMRTTag, ID3D11DepthStencilView* _pDSV)
{
    ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{ nullptr };

    m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

    list<shared_ptr<CRenderTarget>>* pMRTList = FindMRT(strMRTTag);
    ID3D11RenderTargetView* pRTVs[8] = { nullptr };

    m_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

    _uint iNumRTVs = { 0 };

    for (auto& pRenderTarget : *pMRTList) {
        pRenderTarget->Clear();
        pRTVs[iNumRTVs++] = pRenderTarget->GetRTV().Get();
    }

    m_pContext->ClearDepthStencilView(_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    m_pContext->OMSetRenderTargets(iNumRTVs, pRTVs, _pDSV);
   // m_pContext->OMSetRenderTargets(0, NULL, _pDSV);
    return S_OK;
}

HRESULT CTargetMgr::EndMRT()
{
    m_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView.Get());
    m_pBackBufferView->Release();

    return S_OK;
}

HRESULT CTargetMgr::BindSRV(const wstring& _strTargetTag, const _char* _pConstantName)
{
    shared_ptr<CRenderTarget> pRenderTarget = FindRenderTarget(_strTargetTag);

    if (!pRenderTarget)
        return E_FAIL;

    return pRenderTarget->BindSRV(_pConstantName);
}

HRESULT CTargetMgr::BindUAV(const wstring& _strTargetTag, const _char* _pConstantName)
{
    shared_ptr<CRenderTarget> pRenderTarget = FindRenderTarget(_strTargetTag);

    if (!pRenderTarget)
        return E_FAIL;

    return pRenderTarget->BindUAV(_pConstantName);
}

HRESULT CTargetMgr::CreateTargetSRV(const wstring& _strTargetTag)
{
    shared_ptr<CRenderTarget> pRenderTarget = FindRenderTarget(_strTargetTag);

    if (!pRenderTarget)
        return E_FAIL;

    return pRenderTarget->CreateSRV();
}

HRESULT CTargetMgr::CreateCopyBuffer(_uint iSizeX, _uint iSizeY)
{
    ID3D11RenderTargetView* pBackBufferRTV = nullptr;
    m_pContext->OMGetRenderTargets(1, &pBackBufferRTV, nullptr);

    D3D11_RENDER_TARGET_VIEW_DESC BackBufferDesc;
    pBackBufferRTV->GetDesc(&BackBufferDesc);

    D3D11_TEXTURE2D_DESC BackBufferCopyDesc;
    ZeroMemory(&BackBufferCopyDesc, sizeof(D3D11_TEXTURE2D_DESC));

    BackBufferCopyDesc.Width = iSizeX;
    BackBufferCopyDesc.Height = iSizeY;
    BackBufferCopyDesc.MipLevels = 1;
    BackBufferCopyDesc.ArraySize = 1;
    BackBufferCopyDesc.Format = BackBufferDesc.Format;
    BackBufferCopyDesc.SampleDesc.Quality = 0;
    BackBufferCopyDesc.SampleDesc.Count = 1;
    BackBufferCopyDesc.Usage = D3D11_USAGE_DEFAULT;
    BackBufferCopyDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    BackBufferCopyDesc.CPUAccessFlags = 0;
    BackBufferCopyDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&BackBufferCopyDesc, nullptr, &m_pCopyRes)))
        return E_FAIL;

    pBackBufferRTV->Release();

    return S_OK;
}

HRESULT CTargetMgr::BindBackBufferSRV(const _char* _pConstantName)
{
    ID3D11RenderTargetView* pBackBufferRTV = nullptr;
    m_pContext->OMGetRenderTargets(1, &pBackBufferRTV, nullptr);

    ID3D11Resource* pBackBufferResource = nullptr;
    pBackBufferRTV->GetResource(&pBackBufferResource);

    m_pContext->CopyResource(m_pCopyRes.Get(), pBackBufferResource);

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pCopyRes.Get(), nullptr, &m_pBackBufferSRV)))
        return E_FAIL;

    pBackBufferRTV->Release();
    pBackBufferResource->Release();

    
    if (FAILED(CGameInstance::GetInstance()->BindSRVDirect(_pConstantName, m_pBackBufferSRV)))
        return E_FAIL;

    return S_OK;
}

//#ifdef _DEBUG
HRESULT CTargetMgr::ReadyDebug(const wstring& _strTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY)
{
    shared_ptr<CRenderTarget> pRenderTarget = FindRenderTarget(_strTargetTag);

    if (!pRenderTarget)
        return E_FAIL;

    return pRenderTarget->ReadyDebug(_fX, _fY, _fSizeX, _fSizeY, _strTargetTag);
}

HRESULT CTargetMgr::RenderMRT(const wstring& _strMRTTag)
{
    list<shared_ptr<CRenderTarget>>* pMRTList = FindMRT(_strMRTTag);

    for (auto& pRenderTarget : *pMRTList) {
        pRenderTarget->Render();
    }

    return S_OK;
}
//#endif

shared_ptr<class CRenderTarget> CTargetMgr::FindRenderTarget(const wstring& _strTargetTag)
{
    auto iter = m_RenderTargets.find(_strTargetTag);

    if (iter == m_RenderTargets.end())
        return nullptr;

    return iter->second;
}

list<shared_ptr<class CRenderTarget>>* CTargetMgr::FindMRT(const wstring& _strMRTTag)
{
    auto iter = m_MRTs.find(_strMRTTag);

    if (iter == m_MRTs.end())
        return nullptr;
    return &iter->second;
}

shared_ptr<CTargetMgr> CTargetMgr::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
{
    shared_ptr<CTargetMgr> pInstnce = make_shared<CTargetMgr>(_pDevice, _pContext);

    if (FAILED(pInstnce->Initialize()))
        MSG_BOX("Failed to Create : CTargetMgr");

    return pInstnce;
}
