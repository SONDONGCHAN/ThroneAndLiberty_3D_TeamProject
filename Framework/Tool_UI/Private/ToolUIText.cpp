#include "pch.h"
#include "ToolUIText.h"
#include "ToolUIBackGround.h"
#include "ToolUIMgr.h"

#include "GameInstance.h"
#include "VIInstancePoint.h"

CToolUIText::CToolUIText()
{

}

CToolUIText::~CToolUIText()
{

}

HRESULT CToolUIText::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, shared_ptr<CCustomFont> _pCustomFont, wstring _strScript)
{
    CTransform::TRANSFORM_DESC pGameObjectDesc;

    pGameObjectDesc.fSpeedPerSec = 10.f;
    pGameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(Super::Initialize(&pGameObjectDesc)))
        return E_FAIL;

    m_ToolUIDesc = _ToolUIDesc;
    m_InstanceDesc = _InstanceDesc;

    if (FAILED(AddComponent(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _InstanceDesc)))
        return E_FAIL;

    m_fX = _InstanceDesc.vCenter.x;
    m_fY = _InstanceDesc.vCenter.y;
    m_fSizeX = _InstanceDesc.vSize.x;
    m_fSizeY = _InstanceDesc.fSizeY;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_UI;
    m_iShaderPass = 0;
    m_ToolUIDesc.iShaderPassIdx = 0;

    m_Font = _pCustomFont;
    //m_strScript = GAMEINSTANCE->StringToWString(_strScript);
    m_strScript = _strScript;

    if (!m_Font.expired())
        m_Rect = m_Font.lock()->GetFont()->MeasureDrawBounds(m_strScript.c_str(), _float2(0.f, 0.f));

    //if (FAILED(PrepareCoverUI()))
    //    return E_FAIL;

    return S_OK;
}

void CToolUIText::PriorityTick(_float _fDeltaTime)
{

}

void CToolUIText::Tick(_float _fDeltaTime)
{
    KeyInput();

    if (!m_Font.expired())
        m_Rect = m_Font.lock()->GetFont()->MeasureDrawBounds(m_strScript.c_str(), _float2(0.f, 0.f));

    if (m_Rect != m_PrevRect)
    {
        _float textSizeX = (m_Rect.right - m_Rect.left) + 5.f;
        _float textSizeY = (m_Rect.bottom - m_Rect.top);

        SetBufferSize(textSizeX, textSizeY);
    }

    /* 부모를 따라다니게끔 */
    if (!m_pUIParent.expired() && (m_pUIParent.lock()->GETPOS != m_vPrevParentPos) && (m_vPrevParentPos != _float3(FLT_MAX, FLT_MAX, FLT_MAX)))
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

void CToolUIText::LateTick(_float _fDeltaTime)
{
    if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, shared_from_this())))
        return;

    if (!m_pUIChildren.empty())
    {
        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.second.lock() != nullptr)
                childPair.second.lock()->LateTick(_fDeltaTime);
        }
    }
}

HRESULT CToolUIText::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_ToolUIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    switch (m_ToolUIDesc.m_eUITexType)
    {
    case TOOL_UI_TEX_D:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;
    }
    break;
    case TOOL_UI_TEX_DM:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;

        if (m_ToolUIDesc.m_isMasked)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                return E_FAIL;
        }

    }
    break;
    case TOOL_UI_TEX_M:
    {
        if (m_ToolUIDesc.m_isMasked)
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                return E_FAIL;
        }
    }
    break;
    case TOOL_UI_TEX_NONE:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;
    }
    break;
    case TOOL_UI_TEX_DMN:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;

        if (m_ToolUIDesc.m_isMasked)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                return E_FAIL;
        }

        if (m_ToolUIDesc.isNoised)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                return E_FAIL;
        }
    }
    break;
    case TOOL_UI_TEX_DN:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;

        if (m_ToolUIDesc.isNoised)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                return E_FAIL;
        }
    }
    break;
    case TOOL_UI_TEX_N:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;

        if (m_ToolUIDesc.isNoised)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                return E_FAIL;
        }
    }
    break;
    case TOOL_UI_TEX_MN:
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_ToolUIDesc.m_eUITexType, sizeof(ETOOL_UI_TEX_TYPE))))
            return E_FAIL;

        if (m_ToolUIDesc.m_isMasked)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                return E_FAIL;
        }

        if (m_ToolUIDesc.isNoised)
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

    if (FAILED(GAMEINSTANCE->RenderFont("Font_Nanum", m_strScript, _float2((m_fX - (m_fSizeX)) + (g_iWinSizeX * 0.5f), (-m_fY - (m_fSizeY)) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_RIGHT, m_InstanceDesc.vColor)))
        return E_FAIL;

    return S_OK;
}

void CToolUIText::SwapText(const string& _strScript)
{
    wstring swapText = GAMEINSTANCE->StringToWString(_strScript);

    m_strScript = swapText;
}

HRESULT CToolUIText::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    shared_ptr<CVIInstancePoint> pBufferCom = dynamic_pointer_cast<CVIInstancePoint>(GAMEINSTANCE->GetBuffer("Buffer_InstancePoint"));
    if (pBufferCom == nullptr)
        return E_FAIL;

    m_pBuffer = dynamic_pointer_cast<CVIInstancePoint>(pBufferCom->Clone(1, &_InstanceDesc));
    if (m_pBuffer == nullptr)
        return E_FAIL;

    m_strTexKeys = _strTextureTags;

    if (m_ToolUIDesc.m_isMasked)
        m_strMaskKey = _strMaskTextureTag;

    if (m_ToolUIDesc.isNoised)
        m_strNoiseKey = _strNoiseTextureTag;

    return S_OK;
}

HRESULT CToolUIText::PrepareCoverUI()
{
    CToolUIBase::ToolUIDesc uiDesc{};
    CVIInstancing::InstanceDesc instanceDesc{};

    uiDesc.m_strTag = "TextLCover";
    uiDesc.m_eUILayer = TOOL_UI_POPUP_MOVABLE;
    uiDesc.m_eUIType = TOOL_UI_HL;
    uiDesc.m_eUITexType = TOOL_UI_TEX_D;
    uiDesc.m_eUISlotType = TOOL_SLOT_END;
    uiDesc.m_isMasked = false;
    uiDesc.isNoised = false;
    uiDesc.fDiscardAlpha = 0.1f;
    uiDesc.fZOrder = 1.2f;

    instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
    instanceDesc.vSize = _float2(64.f, 64.f);
    instanceDesc.fSizeY = m_fSizeY;

    vector<string> ltexs;
    ltexs.emplace_back("Tex_Nameplate_NPC_NameSelectC_Hostile");
    shared_ptr<CToolUIBackGround> leftCover = CToolUIBackGround::Create(ltexs, "", "", uiDesc, instanceDesc);
    if (leftCover == nullptr)
        return E_FAIL;

    UIMGR->AddUI("TextLCover", leftCover);

    leftCover->SetUIPos(m_fX - (m_fSizeX / 2.f), m_fY);

    AddUIChild("TextLCover", leftCover);
    leftCover->AddUIParent(static_pointer_cast<CToolUIText>(shared_from_this()));

    uiDesc.m_strTag = "TextRCover";
    uiDesc.m_eUILayer = TOOL_UI_POPUP_MOVABLE;
    uiDesc.m_eUIType = TOOL_UI_HR;
    uiDesc.m_eUITexType = TOOL_UI_TEX_D;
    uiDesc.m_eUISlotType = TOOL_SLOT_END;
    uiDesc.m_isMasked = false;
    uiDesc.isNoised = false;
    uiDesc.fDiscardAlpha = 0.1f;
    uiDesc.fZOrder = 1.2f;

    instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
    instanceDesc.vSize = _float2(64.f, 64.f);
    instanceDesc.fSizeY = m_fSizeY;

    vector<string> rtexs;
    rtexs.emplace_back("Tex_Nameplate_NPC_NameSelectC_Hostile");
    shared_ptr<CToolUIBackGround> rightCover = CToolUIBackGround::Create(rtexs, "", "", uiDesc, instanceDesc);
    if (rightCover == nullptr)
        return E_FAIL;

    UIMGR->AddUI("TextRCover", rightCover);

    rightCover->SetUIPos(m_fX + (m_fSizeX / 2.f), m_fY);

    AddUIChild("TextRCover", rightCover);
    rightCover->AddUIParent(static_pointer_cast<CToolUIText>(shared_from_this()));

    return S_OK;
}

shared_ptr<CToolUIText> CToolUIText::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, shared_ptr<CCustomFont> _pCustomFont, wstring _strScript)
{
    shared_ptr<CToolUIText> pInstance = make_shared<CToolUIText>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc, _pCustomFont, _strScript)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CToolUIText::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
