#include "UIOptionBG.h"

#include "VIInstanceUI.h"

CUIOptionBG::CUIOptionBG()
{

}

CUIOptionBG::~CUIOptionBG()
{

}

HRESULT CUIOptionBG::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

void CUIOptionBG::PriorityTick(_float _fTimeDelta)
{

}

void CUIOptionBG::Tick(_float _fTimeDelta)
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

void CUIOptionBG::LateTick(_float _fTimeDelta)
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
            if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_WORLDUI, shared_from_this())))
                return;
        }

        else
        {
            if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, shared_from_this())))
                return;
        }

        if (!m_pUIChildren.empty())
        {
            for (auto& childPair : m_pUIChildren)
            {
                if (childPair.second.lock() != nullptr)
                    childPair.second.lock()->LateTick(_fTimeDelta);
            }
        }

        UpdateTexture();
    }
}

HRESULT CUIOptionBG::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_iInstanceRenderState", &m_iInstanceRenderState, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_vColor", &m_vColorAlpha, sizeof(_float4))))
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

    if (m_bIsFontRender)
    {
        if (FAILED(RenderFont()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CUIOptionBG::RenderFont()
{
    if (m_UIDesc.strTag == "OptionABGs")
    {
        wstring strSlot8 = L"환경설정";
        _float fSlotoffset8 = static_cast<_float>(strSlot8.size()) * 7.f;
        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", strSlot8, _float2((m_vUIPos[1].x - fSlotoffset8) + (g_iWinSizeX * 0.5f), (-m_vUIPos[1].y - 50.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, m_vFontColor, 0.f, _float2(0.f, 0.f), 1.f)))
            return E_FAIL;
    }

    if (m_UIDesc.strTag == "OptionDBGs")
    {
        wstring strSlot0 = L"High Dynamic Range (HDR)";
        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", strSlot0, _float2((m_vUIPos[1].x + 5.f - (m_vUISize[1].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[1].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, m_vFontColor, 0.f, _float2(0.f, 0.f), 1.f)))
            return E_FAIL;

        wstring strSlot1 = L"Physical Based Rendering (PBR)";
        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", strSlot1, _float2((m_vUIPos[2].x + 5.f - (m_vUISize[2].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[2].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, m_vFontColor, 0.f, _float2(0.f, 0.f), 1.f)))
            return E_FAIL;

        wstring strSlot2 = L"Ambient Occlusion (AO)";
        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", strSlot2, _float2((m_vUIPos[3].x + 5.f - (m_vUISize[3].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[3].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, m_vFontColor, 0.f, _float2(0.f, 0.f), 1.f)))
            return E_FAIL;

        wstring strSlot3 = L"Cascade Shadow Map";
        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", strSlot3, _float2((m_vUIPos[4].x + 5.f - (m_vUISize[4].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[4].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, m_vFontColor, 0.f, _float2(0.f, 0.f), 1.f)))
            return E_FAIL;

        wstring strSlot4 = L"Depth of Field (DOF)";
        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", strSlot4, _float2((m_vUIPos[5].x + 5.f - (m_vUISize[5].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[5].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, m_vFontColor, 0.f, _float2(0.f, 0.f), 1.f)))
            return E_FAIL;

        wstring strSlot5 = L"Tone Mapping : Saturate";
        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", strSlot5, _float2((m_vUIPos[6].x + 5.f - (m_vUISize[6].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[6].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, m_vFontColor, 0.f, _float2(0.f, 0.f), 1.f)))
            return E_FAIL;

        wstring strSlot6 = L"Tone Mapping : Constrast";
        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", strSlot6, _float2((m_vUIPos[7].x + 5.f - (m_vUISize[7].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[7].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, m_vFontColor, 0.f, _float2(0.f, 0.f), 1.f)))
            return E_FAIL;

        wstring strSlot7 = L"Tone Mapping : MiddleGray";
        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", strSlot7, _float2((m_vUIPos[8].x + 5.f - (m_vUISize[8].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[8].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, m_vFontColor, 0.f, _float2(0.f, 0.f), 1.f)))
            return E_FAIL;
    }

    return S_OK;
}

void CUIOptionBG::SetColorAlpha(_float4 _vColorAlpha)
{
    m_vColorAlpha = _vColorAlpha;
}

void CUIOptionBG::SetFontColorAlpha(_float4 _fColorAlpha)
{
    m_vFontColor = _fColorAlpha;
}

void CUIOptionBG::InitUpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    for (_uint i = 0; i < iNumSlots; ++i)
    {
        if (m_UIDesc.strTag == "OptionABGs")
        {
            vertices[i].iTexIndex = i;
            if (i == 3)
                vertices[i].vColor = _float4(1.f, 1.f, 1.f, 0.5f);

        }
        if (m_UIDesc.strTag == "OptionDBGs")
        {
            if (i == 0)
                vertices[i].iTexIndex = 0;

            else if (i > 0 && i <= 8)
                vertices[i].iTexIndex = 1;

            else if (i > 8)
                vertices[i].iTexIndex = 2;
        }
        vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
        vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
        vertices[i].iSlotUV = 0;
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUIOptionBG::UpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    _int iNumMouse = IsInstanceMouseOn();



    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

HRESULT CUIOptionBG::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

shared_ptr<CUIOptionBG> CUIOptionBG::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUIOptionBG> pInstance = make_shared<CUIOptionBG>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIOptionBG::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
