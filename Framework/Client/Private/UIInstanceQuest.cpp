#include "UIInstanceQuest.h"
#include "UIMgr.h"
#include "UIPanel.h"

#include "GameInstance.h"
#include "VIInstanceUI.h"

CUIInstanceQuest::CUIInstanceQuest()
{

}

CUIInstanceQuest::~CUIInstanceQuest()
{

}

HRESULT CUIInstanceQuest::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

    {
        D3D11_MAPPED_SUBRESOURCE subResource{};

        ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

        GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

        VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

        _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

        for (_uint i = 0; i < iNumSlots; ++i)
        {
            vertices[i].iTexIndex = i;
            vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
            vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            vertices[i].iSlotUV = 1;
        }

        vertices[6].iSlotUV = 0;
        vertices[9].iSlotUV = 0;
        vertices[10].iSlotUV = 0;
        vertices[14].iSlotUV = 0;
        vertices[15].iSlotUV = 0;
        vertices[17].iSlotUV = 0;
        vertices[18].iSlotUV = 0;

        GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
    }

    return S_OK;
}

void CUIInstanceQuest::PriorityTick(_float _fTimeDelta)
{

}

void CUIInstanceQuest::Tick(_float _fTimeDelta)
{
    KeyInput();
    QuestKeyInput();

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

void CUIInstanceQuest::LateTick(_float _fTimeDelta)
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

    if (!m_strTexKeys.empty())
    {
        m_iInstanceRenderState = 0;
    }

    if (m_UIDesc.isMasked)
    {
        m_iInstanceRenderState = 1;
    }

    if (m_UIDesc.isNoised)
    {
        m_iInstanceRenderState = 2;
    }

    if (m_strTexKeys.empty() && !m_UIDesc.isMasked && !m_UIDesc.isNoised)
    {
        m_iInstanceRenderState = 3;
    }

    UpdateTexture();
}

HRESULT CUIInstanceQuest::Render()
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

    if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", m_strPanelText, _float2((m_vUIPos[6].x - (m_vUISize[6].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[6].y - (m_vUISize[6].y * 0.5f)) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.65f, 0.52f, 0.42f, 1.f), 0.f, _float2(0.f, 0.f), 1.f)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", m_strMainQuestTitle, _float2((m_vUIPos[9].x - (m_vUISize[9].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[9].y - (m_vUISize[9].y * 0.5f)) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.79f, 0.65f, 0.28f, 1.f), 0.f, _float2(0.f, 0.f), 1.f)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", m_strMainQuestContent, _float2((m_vUIPos[10].x - (m_vUISize[10].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[10].y - (m_vUISize[10].y * 0.5f)) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 1.f)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", m_strSubQuestTitle0, _float2((m_vUIPos[14].x - (m_vUISize[14].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[14].y - (m_vUISize[14].y * 0.5f)) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.79f, 0.65f, 0.28f, 1.f), 0.f, _float2(0.f, 0.f), 1.f)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", m_strSubQuestContent0, _float2((m_vUIPos[15].x - (m_vUISize[15].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[15].y - (m_vUISize[15].y * 0.5f)) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 1.f)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", m_strSubQuestTitle1, _float2((m_vUIPos[17].x - (m_vUISize[17].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[17].y - (m_vUISize[17].y * 0.5f)) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.79f, 0.65f, 0.28f, 1.f), 0.f, _float2(0.f, 0.f), 1.f)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", m_strSubQuestContent1, _float2((m_vUIPos[18].x - (m_vUISize[18].x * 0.5f)) + (g_iWinSizeX * 0.5f), (-m_vUIPos[18].y - (m_vUISize[18].y * 0.5f)) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 1.f)))
        return E_FAIL;

    return S_OK;
}

void CUIInstanceQuest::UpdateQuestContent(QUEST_TYPE _questType, const wstring& _questTitle, const wstring& _questContent, const string& _strFont, _float4 _vTitleFontColor, _float4 _vContentFontColor)
{
    wstring questTitle = _questTitle;
    wstring questContent = _questContent;

    if (_questTitle.size() > 20)
    {
        if (_questTitle.size() > 20)
            questTitle.erase(20);
    }

    if (_questContent.size() > 24)
    {
        wchar_t ch = L'\n';
        questContent = questContent.insert(24, 1, ch);

        if (questContent.size() > 48)
            questContent = questContent.insert(48, 1, ch);

        if (questContent.size() > 72)
            questContent.erase(72);
    }

    switch(_questType)
    {
    case QUEST_MAIN:
    {
        m_strMainQuestFont = _strFont;
        m_vMainTitleFontColor = _vTitleFontColor;
        m_vMainContentFontColor = _vContentFontColor;
        m_strMainQuestTitle = questTitle;
        m_strMainQuestContent = questContent;
    }
    break;
    case QUEST_SUB0:
    {
        /*if (m_strSubQuestTitle0 != L"" && m_strSubQuestTitle1 == L"")
        {
            m_strSubQuestFont1 = _strFont;
            m_vSubTitleFontColor1 = _vTitleFontColor;
            m_vSubContentFontColor1 = _vContentFontColor;
            m_strSubQuestTitle1 = questTitle;
            m_strSubQuestContent1 = questContent;
        }

        else if (m_strSubQuestTitle0 == L"" && m_strSubQuestTitle1 != L"")
        {
            m_strSubQuestFont0 = _strFont;
            m_vSubTitleFontColor0 = _vTitleFontColor;
            m_vSubContentFontColor0 = _vContentFontColor;
            m_strSubQuestTitle0 = questTitle;
            m_strSubQuestContent0 = questContent;
        }*/
        m_strSubQuestFont0 = _strFont;
        m_vSubTitleFontColor0 = _vTitleFontColor;
        m_vSubContentFontColor0 = _vContentFontColor;
        m_strSubQuestTitle0 = questTitle;
        m_strSubQuestContent0 = questContent;
    }
    break;
    case QUEST_SUB1:
    {
        /*if (m_strSubQuestTitle0 != L"" && m_strSubQuestTitle1 == L"")
        {
            m_strSubQuestFont1 = _strFont;
            m_vSubTitleFontColor1 = _vTitleFontColor;
            m_vSubContentFontColor1 = _vContentFontColor;
            m_strSubQuestTitle1 = questTitle;
            m_strSubQuestContent1 = questContent;
        }

        else if (m_strSubQuestTitle0 == L"" && m_strSubQuestTitle1 != L"")
        {
            m_strSubQuestFont0 = _strFont;
            m_vSubTitleFontColor0 = _vTitleFontColor;
            m_vSubContentFontColor0 = _vContentFontColor;
            m_strSubQuestTitle0 = questTitle;
            m_strSubQuestContent0 = questContent;
        }*/
        m_strSubQuestFont1 = _strFont;
        m_vSubTitleFontColor1 = _vTitleFontColor;
        m_vSubContentFontColor1 = _vContentFontColor;
        m_strSubQuestTitle1 = questTitle;
        m_strSubQuestContent1 = questContent;
    }
    break;
    }
}

void CUIInstanceQuest::UpdateQuestContentNonAuto(QUEST_TYPE _questType, const wstring& _questTitle, const wstring& _questContent, const string& _strFont, _float4 _vTitleFontColor, _float4 _vContentFontColor)
{
    wstring questTitle = _questTitle;
    wstring questContent = _questContent;

    switch (_questType)
    {
    case QUEST_MAIN:
    {
        m_strMainQuestFont = _strFont;
        m_vMainTitleFontColor = _vTitleFontColor;
        m_vMainContentFontColor = _vContentFontColor;
        m_strMainQuestTitle = questTitle;
        m_strMainQuestContent = questContent;
    }
    break;
    case QUEST_SUB0:
    {
        m_strSubQuestFont0 = _strFont;
        m_vSubTitleFontColor0 = _vTitleFontColor;
        m_vSubContentFontColor0 = _vContentFontColor;
        m_strSubQuestTitle0 = questTitle;
        m_strSubQuestContent0 = questContent;
    }
    break;
    case QUEST_SUB1:
    {
        m_strSubQuestFont1 = _strFont;
        m_vSubTitleFontColor1 = _vTitleFontColor;
        m_vSubContentFontColor1 = _vContentFontColor;
        m_strSubQuestTitle1 = questTitle;
        m_strSubQuestContent1 = questContent;
    }
    break;
    }
}

void CUIInstanceQuest::AppendQuestContent(QUEST_TYPE _questType, const wstring& _questContent)
{
    switch (_questType)
    {
    case QUEST_MAIN:
    {
        wstring questContent = _questContent;
        if (questContent.size() > 24)
        {
            wchar_t ch = L'\n';
            questContent = questContent.insert(24, 1, ch);

            if (questContent.size() > 48)
                questContent = questContent.insert(48, 1, ch);

            if (questContent.size() > 72)
                questContent.erase(72);
        }
        m_strMainQuestContent = questContent;
    }
    break;
    case QUEST_SUB0:
    {
        wstring questContent = _questContent;
        if (questContent.size() > 22)
        {
            wchar_t ch = L'\n';
            questContent = questContent.insert(22, 1, ch);

            if (questContent.size() > 44)
                questContent = questContent.insert(44, 1, ch);

            if (questContent.size() > 66)
                questContent.erase(66);
        }
        m_strSubQuestContent0 = questContent;
    }
    break;
    case QUEST_SUB1:
    {
        wstring questContent = _questContent;
        if (questContent.size() > 24)
        {
            wchar_t ch = L'\n';
            questContent = questContent.insert(24, 1, ch);

            if (questContent.size() > 48)
                questContent = questContent.insert(48, 1, ch);

            if (questContent.size() > 72)
                questContent.erase(72);
        }
        m_strSubQuestContent1 = questContent;
    }
    break;
    }
}

void CUIInstanceQuest::UpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    if (m_strMainQuestTitle == L"")
    {
        vertices[7].iSlotUV = 0;
        vertices[8].iSlotUV = 0;
    }
    else
    {
        vertices[7].iSlotUV = 1;
        vertices[8].iSlotUV = 1;
    }

    if (m_strSubQuestTitle0 == L"")
    {
        vertices[12].iSlotUV = 0;
        vertices[13].iSlotUV = 0;
    }
    else
    {
        vertices[12].iSlotUV = 1;
        vertices[13].iSlotUV = 1;
    }

    if (m_strSubQuestTitle1 == L"")
    {
        vertices[11].iSlotUV = 0;
        vertices[16].iSlotUV = 0;
    }
    else
    {
        vertices[11].iSlotUV = 1;
        vertices[16].iSlotUV = 1;
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUIInstanceQuest::QuestKeyInput()
{
    _int iNumPos = IsInstanceMouseOn();

    if (GAMEINSTANCE->MouseDown(DIM_LB) && iNumPos != -1 && iNumPos == 7)
    {
        weak_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("TradeAllPanel"));
        if (!pUI.expired())
        {
            if (!pUI.lock()->IsActivated())
            {
                if (UIMGR->IsQuestNPCActivated(QUEST_MAIN))
                    UIMGR->SetIsQuestNPCActivated(QUEST_MAIN, false);
                else
                    UIMGR->SetIsQuestNPCActivated(QUEST_MAIN, true);
            }
        }
    }

    if (GAMEINSTANCE->MouseDown(DIM_LB) && iNumPos != -1 && iNumPos == 12)
    {
        weak_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("TradeAllPanel"));
        if (!pUI.expired())
        {
            if (!pUI.lock()->IsActivated())
            {
                if (UIMGR->IsQuestNPCActivated(QUEST_SUB0))
                    UIMGR->SetIsQuestNPCActivated(QUEST_SUB0, false);
                else
                    UIMGR->SetIsQuestNPCActivated(QUEST_SUB0, true);
            }
        }
    }

    if (GAMEINSTANCE->MouseDown(DIM_LB) && iNumPos != -1 && iNumPos == 16)
    {
        weak_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("TradeAllPanel"));
        if (!pUI.expired())
        {
            if (!pUI.lock()->IsActivated())
            {
                if (UIMGR->IsQuestNPCActivated(QUEST_SUB1))
                    UIMGR->SetIsQuestNPCActivated(QUEST_SUB1, false);
                else
                    UIMGR->SetIsQuestNPCActivated(QUEST_SUB1, true);
            }
        }
    }
}

HRESULT CUIInstanceQuest::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

shared_ptr<CUIInstanceQuest> CUIInstanceQuest::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUIInstanceQuest> pInstance = make_shared<CUIInstanceQuest>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIInstanceQuest::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
