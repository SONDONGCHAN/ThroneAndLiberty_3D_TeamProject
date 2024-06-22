#include "pch.h"
#include "ToolUIItemTab.h"
#include "ToolUIMgr.h"

#include "GameInstance.h"
#include "VIInstancePoint.h"

CToolUIItemTab::CToolUIItemTab()
{

}

CToolUIItemTab::~CToolUIItemTab()
{

}

HRESULT CToolUIItemTab::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
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

    if (_initPos == _float2(0.f, 0.f))
    {
        m_fX = _InstanceDesc.vCenter.x;
        m_fY = _InstanceDesc.vCenter.y;
    }

    else
    {
        m_fX = _initPos.x;
        m_fY = _initPos.y;
    }

    m_fSizeX = _InstanceDesc.vSize.x;
    m_fSizeY = _InstanceDesc.fSizeY;

    /* Shader Info */
    m_eShaderType = ESHADER_TYPE::ST_UI;
    m_iShaderPass = 11;
    m_ToolUIDesc.iShaderPassIdx = 11;

    /* Test */
    if (m_ToolUIDesc.m_strTag == "EquipTab")
        m_isTabActivated = true;

    return S_OK;
}

void CToolUIItemTab::PriorityTick(_float _fDeltaTime)
{

}

void CToolUIItemTab::Tick(_float _fDeltaTime)
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
        if (m_strTexKeys.size() > 1 && IsMouseOn())
            m_iTexIndex = 2;
        else
            m_iTexIndex = 3;
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

void CToolUIItemTab::LateTick(_float _fDeltaTime)
{
    if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, shared_from_this())))
        return;

    if (!m_pUIChildren.empty())
    {
        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.second.lock() != nullptr)
            {
                if (childPair.second.lock()->GetUIType() == TOOL_UI_SCROLLBG)
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

HRESULT CToolUIItemTab::Render()
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

    return S_OK;
}

void CToolUIItemTab::DeactivateOtherTabs()
{
    if (m_otherTabs.empty())
    {
        unordered_map<string, shared_ptr<CToolUIBase>>* allUIs = UIMGR->GetAllUIs();
        for (auto& uiPair : *allUIs)
        {
            if (uiPair.second->GetUIType() == TOOL_UI_ITEMTAB && uiPair.second->GetUITag() != m_ToolUIDesc.m_strTag)
            {
                shared_ptr<CToolUIItemTab> itemTab = dynamic_pointer_cast<CToolUIItemTab>(uiPair.second);
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

void CToolUIItemTab::ActivateTab()
{
    m_isTabActivated = true;
}

void CToolUIItemTab::DeactivateTab()
{
    m_isTabActivated = false;
}

void CToolUIItemTab::ItemTabKeyInput()
{
    if (GAMEINSTANCE->MouseDown(DIM_LB) && IsMouseOn())
    {
        DeactivateOtherTabs();
        ActivateTab();
    }
}

HRESULT CToolUIItemTab::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
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

shared_ptr<CToolUIItemTab> CToolUIItemTab::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
{
    shared_ptr<CToolUIItemTab> pInstance = make_shared<CToolUIItemTab>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc, _initPos)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CToolUIItemTab::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
