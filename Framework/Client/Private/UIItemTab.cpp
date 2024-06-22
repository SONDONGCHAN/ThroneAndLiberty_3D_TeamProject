#include "UIItemTab.h"
#include "UIMgr.h"

#include "GameInstance.h"
#include "VIInstancePoint.h"

CUIItemTab::CUIItemTab()
{

}

CUIItemTab::~CUIItemTab()
{

}

HRESULT CUIItemTab::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
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

    /* Test */
    if (m_UIDesc.strTag == "EquipTab")
        m_isTabActivated = true;

    return S_OK;
}

void CUIItemTab::PriorityTick(_float _fDeltaTime)
{

}

void CUIItemTab::Tick(_float _fDeltaTime)
{
    KeyInput();
    ItemTabKeyInput();

    if (!m_isTabActivated)
    {
        if (m_strTexKeys.size() > 1 && IsMouseOn())
            m_iTexIndex = 1;
        else
            m_iTexIndex = 0;
    }

    else
    {
        if (GetUITag() == "EquipTab")
            UIMGR->SetTabIndex(0);
        else if (GetUITag() == "UsableTab")
            UIMGR->SetTabIndex(1);
        else if (GetUITag() == "MiscTab")
            UIMGR->SetTabIndex(2);

        if (m_strTexKeys.size() > 1 && IsMouseOn())
            m_iTexIndex = 2;
        else
            m_iTexIndex = 3;
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
            {
                /*if (childPair.second.lock()->GetUIType() == TOOL_UI_SCROLLBG)
                {
                    if (m_isTabActivated)
                        childPair.second.lock()->Tick(_fDeltaTime);
                }
                else*/
                childPair.second.lock()->Tick(_fDeltaTime);
            }
        }
    }

    if (!m_pUIParent.expired())
        m_vPrevParentPos = m_pUIParent.lock()->GETPOS;
}

void CUIItemTab::LateTick(_float _fDeltaTime)
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
            {
                if (childPair.second.lock()->GetUIType() == UI_SCROLLBG)
                {
                    if (m_isTabActivated)
                        childPair.second.lock()->LateTick(_fDeltaTime);
                }
                else
                    childPair.second.lock()->LateTick(_fDeltaTime);
            }
        }
    }
}

HRESULT CUIItemTab::Render()
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

    return S_OK;
}

void CUIItemTab::DeactivateOtherTabs()
{
    if (m_otherTabs.empty())
    {
        unordered_map<string, shared_ptr<CUIBase>>* allUIs = UIMGR->GetAllUIs();
        for (auto& uiPair : *allUIs)
        {
            if (uiPair.second->GetUIType() == UI_ITEMTAB && uiPair.second->GetUITag() != m_UIDesc.strTag)
            {
                shared_ptr<CUIItemTab> itemTab = dynamic_pointer_cast<CUIItemTab>(uiPair.second);
                if (itemTab != nullptr)
                    m_otherTabs.emplace_back(itemTab);
            }
        }

        for (auto& otherTab : m_otherTabs)
        {
            if (!otherTab.expired())
                otherTab.lock()->DeactivateTab();
        }
    }
    else
    {
        for (auto& otherTab : m_otherTabs)
        {
            if (!otherTab.expired())
                otherTab.lock()->DeactivateTab();
        }
    }
}

void CUIItemTab::ActivateTab()
{
    m_isTabActivated = true;
}

void CUIItemTab::DeactivateTab()
{
    m_isTabActivated = false;
}

void CUIItemTab::ItemTabKeyInput()
{
    if (GAMEINSTANCE->MouseDown(DIM_LB) && IsMouseOn())
    {
        DeactivateOtherTabs();
        ActivateTab();
    }
}

HRESULT CUIItemTab::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
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

shared_ptr<CUIItemTab> CUIItemTab::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
{
    shared_ptr<CUIItemTab> pInstance = make_shared<CUIItemTab>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _initPos)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIItemTab::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
