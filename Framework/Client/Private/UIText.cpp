#include "UIText.h"
#include "UIBackGround.h"
#include "UIMgr.h"

#include "GameInstance.h"
#include "VIInstancePoint.h"

CUIText::CUIText()
{

}

CUIText::~CUIText()
{

}

HRESULT CUIText::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, shared_ptr<CCustomFont> _pCustomFont, wstring _strScript)
{
    if (FAILED(Super::Initialize()))
        return E_FAIL;

    m_UIDesc = _UIDesc;

    if (FAILED(AddComponent(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _InstanceDesc)))
        return E_FAIL;

    m_fX = _InstanceDesc.vCenter.x;
    m_fY = _InstanceDesc.vCenter.y;
    m_fSizeX = _InstanceDesc.vSize.x;
    m_fSizeY = _InstanceDesc.fSizeY;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_UI;
    m_iShaderPass = m_UIDesc.iShaderPassIdx;

    m_Font = _pCustomFont;
    m_strScript = _strScript;

    if (!m_Font.expired())
        m_Rect = m_Font.lock()->GetFont()->MeasureDrawBounds(m_strScript.c_str(), _float2(0.f, 0.f));

    //if (FAILED(PrepareCoverUI()))
    //    return E_FAIL;

    return S_OK;
}

void CUIText::PriorityTick(_float _fDeltaTime)
{

}

void CUIText::Tick(_float _fDeltaTime)
{
    KeyInput();

    if (m_strTexKeys.size() > 1 && IsMouseOn())
        m_iTexIndex = 1;
    else
        m_iTexIndex = 0;

    if (!m_Font.expired())
        m_Rect = m_Font.lock()->GetFont()->MeasureDrawBounds(m_strScript.c_str(), _float2(0.f, 0.f));

    if (m_Rect != m_PrevRect)
    {
        _float textSizeX = static_cast<_float>(m_Rect.right - m_Rect.left) + 5.f;
        _float textSizeY = static_cast<_float>(m_Rect.bottom - m_Rect.top);

        SetBufferSize(textSizeX, textSizeY);
    }

    /* 부모를 따라다니게끔 */
    if (!m_pUIParent.expired() && (m_pUIParent.lock()->GETPOS != m_vPrevParentPos))
    {
        _float3 moveDir = m_pUIParent.lock()->GETPOS - m_vPrevParentPos;
        moveDir.Normalize();
        _float moveDist = SimpleMath::Vector3::Distance(m_pUIParent.lock()->GETPOS, m_vPrevParentPos);

        _float3 newPos = _float3(m_fX, m_fY, 0.f) + (moveDir * moveDist);
        SetUIPos(newPos.x, newPos.y);

        m_BindWorldMat = m_pTransformCom->GetWorldMatrix();
    }
    else
        m_BindWorldMat = m_pTransformCom->GetWorldMatrix();

    if (!m_pUIChildren.empty())
    {
        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.second.lock() != nullptr)
                childPair.second.lock()->Tick(_fDeltaTime);
        }
    }

    if (!m_pUIParent.expired())
        m_vPrevParentPos = m_pUIParent.lock()->GETPOS;

    m_PrevRect = m_Rect;
}

void CUIText::LateTick(_float _fDeltaTime)
{
    if (m_isOnce && m_isWorldUI && !m_pUIChildren.empty())
    {
        m_isOnce = false;

        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.second.lock() != nullptr)
                childPair.second.lock()->SetIsWorldUI(true);
        }

        if (m_iShaderPass == 0)
            m_iShaderPass = 14;
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
                childPair.second.lock()->LateTick(_fDeltaTime);
        }
    }
}

HRESULT CUIText::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    switch (m_UIDesc.eUITexType)
    {
    case UI_TEX_D:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_UIDesc.eUITexType, sizeof(UI_TEX_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;
    }
    break;
    case UI_TEX_DM:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_UIDesc.eUITexType, sizeof(UI_TEX_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;

        if (m_UIDesc.isMasked)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                return E_FAIL;
        }

    }
    break;
    case UI_TEX_M:
    {
        if (m_UIDesc.isMasked)
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_UIDesc.eUITexType, sizeof(UI_TEX_TYPE))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                return E_FAIL;
        }
    }
    break;
    case UI_TEX_NONE:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_UIDesc.eUITexType, sizeof(UI_TEX_TYPE))))
            return E_FAIL;
    }
    break;
    case UI_TEX_DMN:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_UIDesc.eUITexType, sizeof(UI_TEX_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;

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
    }
    break;
    case UI_TEX_DN:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_UIDesc.eUITexType, sizeof(UI_TEX_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;

        if (m_UIDesc.isNoised)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                return E_FAIL;
        }
    }
    break;
    case UI_TEX_N:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_UIDesc.eUITexType, sizeof(UI_TEX_TYPE))))
            return E_FAIL;

        if (m_UIDesc.isNoised)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                return E_FAIL;
        }
    }
    break;
    case UI_TEX_MN:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_UIDesc.eUITexType, sizeof(UI_TEX_TYPE))))
            return E_FAIL;

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
    }
    break;
    }

    if (FAILED(GAMEINSTANCE->BeginShader()))
        return E_FAIL;

    if (FAILED(m_pBuffer->BindBuffers()))
        return E_FAIL;

    if (FAILED(m_pBuffer->Render()))
        return E_FAIL;

    //if (FAILED(GAMEINSTANCE->RenderFont("FontAsiaG", m_strScript, _float2((m_fX - (m_fSizeX)) - (g_iWinSizeX * 0.5f), (-m_fY - (m_fSizeY)) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_CENTER, _float4(1.f, 1.f, 0.f, 1.f))))
    //    return E_FAIL;

    //_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
    //_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

    if (m_UIDesc.strTag == "NPCName")
    {
        _float fNameOffset = static_cast<_float>(m_strScript.size()) * 1.5f;

        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", m_strScript, _float2((m_fX - (m_fSizeX + fNameOffset)) + (g_iWinSizeX * 0.5f), (-m_fY - (m_fSizeY)) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_CENTER, _float4(0.79f, 0.65f, 0.28f, 1.f))))
            return E_FAIL;
    }
    else
    {
        if (FAILED(GAMEINSTANCE->RenderFont("Font_AdenL", m_strScript, _float2((m_fX - (m_fSizeX)) + (g_iWinSizeX * 0.5f), (-m_fY - (m_fSizeY)) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_CENTER, _float4(0.79f, 0.65f, 0.28f, 1.f))))
            return E_FAIL;
    }
    

    //if (FAILED(GAMEINSTANCE->RenderFont("Font_AsiaG", m_strScript, _float2(m_fX, m_fY), CCustomFont::FA_CENTER, _float4(1.f, 1.f, 0.f, 1.f))))
    //    return E_FAIL;

    return S_OK;
}

void CUIText::SwapText(const wstring& _strScript)
{
    m_strScript = _strScript;
}

HRESULT CUIText::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    shared_ptr<CVIInstancePoint> pBufferCom = dynamic_pointer_cast<CVIInstancePoint>(GAMEINSTANCE->GetBuffer("Buffer_InstancePoint"));
    if (pBufferCom == nullptr)
        return E_FAIL;

    m_pBuffer = dynamic_pointer_cast<CVIInstancePoint>(pBufferCom->Clone(1, &_InstanceDesc));
    if (m_pBuffer == nullptr)
        return E_FAIL;

    m_strTexKeys = _strTextureTags;

    if (m_UIDesc.isMasked)
        m_strMaskKey = _strMaskTextureTag;

    if (m_UIDesc.isNoised)
        m_strNoiseKey = _strNoiseTextureTag;

    return S_OK;
}

HRESULT CUIText::PrepareCoverUI()
{
    CUIBase::UIDesc uiDesc{};
    CVIInstancing::InstanceDesc instanceDesc{};

    uiDesc.strTag = "TextLCover";
    uiDesc.eUILayer = UI_POPUP_MOVABLE;
    uiDesc.eUIType = UI_HL;
    uiDesc.eUITexType = UI_TEX_D;
    uiDesc.eUISlotType = SLOT_END;
    uiDesc.isMasked = false;
    uiDesc.isNoised = false;
    uiDesc.fDiscardAlpha = 0.1f;
    uiDesc.fZOrder = 0.2f;
    uiDesc.iShaderPassIdx = 13;

    instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
    instanceDesc.vSize = _float2(64.f, 64.f);
    instanceDesc.fSizeY = m_fSizeY;

    vector<string> ltexs;
    ltexs.emplace_back("Tex_Nameplate_NPC_NameSelectC_Hostile");
    shared_ptr<CUIBackGround> leftCover = CUIBackGround::Create(ltexs, "", "", uiDesc, instanceDesc);
    if (leftCover == nullptr)
        return E_FAIL;

    //UIMGR->AddUI("TextLCover", leftCover);
    m_pCovers.emplace_back(leftCover);

    leftCover->SetUIPos(m_fX - (m_fSizeX / 2.f), m_fY);

    AddUIChild("TextLCover", leftCover);
    leftCover->AddUIParent(static_pointer_cast<CUIText>(shared_from_this()));

    uiDesc.strTag = "TextRCover";
    uiDesc.eUILayer = UI_POPUP_MOVABLE;
    uiDesc.eUIType = UI_HR;
    uiDesc.eUITexType = UI_TEX_D;
    uiDesc.eUISlotType = SLOT_END;
    uiDesc.isMasked = false;
    uiDesc.isNoised = false;
    uiDesc.fDiscardAlpha = 0.1f;
    uiDesc.fZOrder = 0.2f;
    uiDesc.iShaderPassIdx = 13;

    instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
    instanceDesc.vSize = _float2(64.f, 64.f);
    instanceDesc.fSizeY = m_fSizeY;

    vector<string> rtexs;
    rtexs.emplace_back("Tex_Nameplate_NPC_NameSelectC_Hostile");
    shared_ptr<CUIBackGround> rightCover = CUIBackGround::Create(rtexs, "", "", uiDesc, instanceDesc);
    if (rightCover == nullptr)
        return E_FAIL;

    //UIMGR->AddUI("TextRCover", rightCover);
    m_pCovers.emplace_back(rightCover);

    rightCover->SetUIPos(m_fX + (m_fSizeX / 2.f), m_fY);

    AddUIChild("TextRCover", rightCover);
    rightCover->AddUIParent(static_pointer_cast<CUIText>(shared_from_this()));

    return S_OK;
}

shared_ptr<CUIText> CUIText::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, shared_ptr<CCustomFont> _pCustomFont, wstring _strScript)
{
    shared_ptr<CUIText> pInstance = make_shared<CUIText>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _pCustomFont, _strScript)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIText::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
