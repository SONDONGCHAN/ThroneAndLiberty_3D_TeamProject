#include "UICountDown.h"

#include "VIInstanceUI.h"
#include "GameInstance.h"

CUICountDown::CUICountDown()
{

}

CUICountDown::~CUICountDown()
{

}

HRESULT CUICountDown::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    if (FAILED(Super::Initialize()))
        return E_FAIL;

    m_vUIPos = _vUIPos;
    m_vUISize = _vUISize;

    m_UIDesc = _UIDesc;

    if (FAILED(AddComponent(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _InstanceDesc, _vUIPos, _vUISize)))
        return E_FAIL;

    m_fX = _InstanceDesc.vCenter.x;
    m_fY = _InstanceDesc.vCenter.y;
    m_fSizeX = _InstanceDesc.vSize.x;
    m_fSizeY = _InstanceDesc.fSizeY;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_UIINSTANCE;
    m_iShaderPass = m_UIDesc.iShaderPassIdx;

    m_vSlotUVRatio.resize(m_vUIPos.size());

    InitUpdateTexture();

    return S_OK;
}

void CUICountDown::PriorityTick(_float _fTimeDelta)
{

}

void CUICountDown::Tick(_float _fTimeDelta)
{
    if (m_isLoadingDone)
    {
        KeyInput();

        /* 부모를 따라다니게끔 */
        if (!m_pUIParent.expired() && (m_pUIParent.lock()->GETPOS != m_vPrevParentPos) && (m_vPrevParentPos != _float3(FLT_MAX, FLT_MAX, FLT_MAX)))
        {
            _float3 moveDir = m_pUIParent.lock()->GETPOS - m_vPrevParentPos;
            moveDir.Normalize();
            _float moveDist = SimpleMath::Vector3::Distance(m_pUIParent.lock()->GETPOS, m_vPrevParentPos);

            _float3 newPos = _float3(m_fX, m_fY, 0.f) + (moveDir * moveDist);
            SetUIPos(newPos.x, newPos.y);

            for (auto& texPos : m_vUIPos)
            {
                _float3 newTexPos = _float3(texPos.x, texPos.y, 0.f) + (moveDir * moveDist);
                texPos = _float2(newTexPos.x, newTexPos.y);
            }

            m_BindWorldMat = m_pTransformCom->GetWorldMatrix();
        }
        else
        {
            m_BindWorldMat = m_pTransformCom->GetWorldMatrix();
        }

        if (!m_pUIChildren.empty())
        {
            for (auto& childPair : m_pUIChildren)
            {
                if (childPair.second.lock() != nullptr)
                    childPair.second.lock()->Tick(_fTimeDelta);
            }
        }

        if (!m_pUIParent.expired())
            m_vPrevParentPos = m_pUIParent.lock()->GETPOS;
    }
}

void CUICountDown::LateTick(_float _fTimeDelta)
{
    if (m_isLoadingDone)
    {
        if (m_isOnce && m_isWorldUI && !m_pUIChildren.empty())
        {
            m_isOnce = false;

            for (auto& childPair : m_pUIChildren)
            {
                if (childPair.second.lock() != nullptr)
                    childPair.second.lock()->SetIsWorldUI(true);
            }
        }

        if (m_isWorldUI)
        {
            if (m_isRender)
            {
                if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_WORLDUI, shared_from_this())))
                    return;
            }

        }

        else
        {
            if (m_isRender)
            {
                if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, shared_from_this())))
                    return;
            }
        }

        if (!m_pUIChildren.empty())
        {
            for (auto& childPair : m_pUIChildren)
            {
                if (childPair.second.lock() != nullptr)
                    childPair.second.lock()->LateTick(_fTimeDelta);
            }
        }

        //UpdateTexture();
    }

    m_fTimeDelta = _fTimeDelta;
}

HRESULT CUICountDown::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_iInstanceRenderState", &m_iInstanceRenderState, sizeof(_uint))))
        return E_FAIL;

    if (!m_strTexKeys.empty())
    {
        _uint iNumTexKeys = static_cast<_uint>(m_strTexKeys.size());
        for (_uint i = 0; i < iNumTexKeys; ++i)
        {
            string shaderKey = "g_DiffuseTexture" + to_string(i);

            if (FAILED(GAMEINSTANCE->BindSRV(shaderKey.c_str(), m_strTexKeys[i])))
                return E_FAIL;
        }
    }

    if (m_UIDesc.isMasked)
    {
        if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
            return E_FAIL;
    }

    if (m_UIDesc.isNoised)
    {
        if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
            return E_FAIL;
    }

    if (FAILED(GAMEINSTANCE->BeginShader()))
        return E_FAIL;

    if (FAILED(m_pInstanceBuffer->BindBuffers()))
        return E_FAIL;

    if (FAILED(m_pInstanceBuffer->Render()))
        return E_FAIL;

    if (FAILED(RenderFont()))
        return E_FAIL;

    return S_OK;
}

void CUICountDown::RenderScript()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    vertices[0].iSlotUV = 0;
    vertices[1].iSlotUV = 1;
    vertices[2].iSlotUV = 1;

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);

    m_bIsScriptEnabled = true;
}

HRESULT CUICountDown::RenderFont()
{
    //wstring strDisplayContent = L"팝업 테스트";

    /*if (m_strDisplayContent == L"START")
    {
        m_bIsStartSignal = true;
    }*/

    if (!m_bIsStartSignal)
    {
        if (m_fTimeAcc >= 2.5f)
            m_strDisplayContent = to_wstring(3);

        if (m_fTimeAcc < 2.5f && m_fTimeAcc >= 1.5f)
            m_strDisplayContent = to_wstring(2);

        if (m_fTimeAcc < 1.5f && m_fTimeAcc >= 0.5f)
            m_strDisplayContent = to_wstring(1);

        if (m_fTimeAcc < 0.5f)
        {
            m_strDisplayContent = L"START";
        }

        if (m_fTimeAcc <= 0.f)
            m_bIsStartSignal = true;

        m_fTimeAcc -= m_fTimeDelta;
    }

    if (m_fCurRemainTime >= 0.f)
    {
        wstringstream ss;
        ss << std::fixed << std::setprecision(2) << m_fCurRemainTime;
        m_strDisplayContent = ss.str() + L' s';
    }

    if (m_fCurRemainTime < 0.f && m_iNumCurFound != -1)
    {
        wstring strCurFound = to_wstring(m_iNumCurFound);
        wstring strMaxFound = to_wstring(m_iNumMaxFound);

        m_strDisplayContent = strCurFound + L" / " + strMaxFound;
    }

    _float fContentOffset = static_cast<_float>(m_strDisplayContent.size()) * 10.f;
    
    if (FAILED(GAMEINSTANCE->RenderFont("Font_DNFB24", m_strDisplayContent, _float2((m_vUIPos[0].x - fContentOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[0].y - 17.5f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.79f, 0.65f, 0.28f, 0.2f), 0.f, _float2(0.f, 0.f), 1.f)))
        return E_FAIL;

    return S_OK;
}

void CUICountDown::ResetStartSignal()
{
    m_bIsStartSignal = false;
    m_fTimeAcc = 3.5f;
}

void CUICountDown::UpdateNumFound(_int _curFound, _int _maxFound)
{
    m_iNumCurFound = _curFound;
    m_iNumMaxFound = _maxFound;
}

void CUICountDown::InitUpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    //for (_uint i = 0; i < iNumSlots; ++i)
    //{
    //    vertices[i].iTexIndex = i;
    //    vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
    //    vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
    //    vertices[i].iSlotUV = 0;
    //}

    vertices[0].iTexIndex = 0;
    vertices[0].vRight = _float4(m_vUISize[0].x, 0.f, 0.f, 0.f);
    vertices[0].vUp = _float4(0.f, m_vUISize[0].y, 0.f, 0.f);
    vertices[0].iSlotUV = 0;

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUICountDown::UpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    _int iNumMouse = IsInstanceMouseOn();



    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

HRESULT CUICountDown::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    m_pInstanceBuffer = CVIInstanceUI::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), _InstanceDesc, _vUIPos);
    if (m_pInstanceBuffer == nullptr)
        return E_FAIL;

    m_strTexKeys = _strTextureTags;

    if (m_UIDesc.isMasked)
        m_strMaskKey = _strMaskTextureTag;

    if (m_UIDesc.isNoised)
        m_strNoiseKey = _strNoiseTextureTag;

    return S_OK;
}

shared_ptr<CUICountDown> CUICountDown::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUICountDown> pInstance = make_shared<CUICountDown>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUICountDown::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
