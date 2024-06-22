#include "UISlot.h"
#include "UIMgr.h"
#include "UIIcon.h"
#include "UIItemHover.h"
#include "ItemBase.h"
#include "ItemMgr.h"

#include "GameInstance.h"
#include "VIInstancePoint.h"

CUISlot::CUISlot()
{

}

CUISlot::~CUISlot()
{

}

HRESULT CUISlot::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
{
    if (FAILED(Super::Initialize()))
        return E_FAIL;

    m_UIDesc = _ToolUIDesc;

    if (FAILED(AddComponent(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _InstanceDesc)))
        return E_FAIL;

    //if (!_strTextureTags.empty())
    //    m_SlotDesc.strImageTag = _strTextureTags[0];

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
    m_iShaderPass = m_UIDesc.iShaderPassIdx;

    if (m_UIDesc.eUISlotType == SLOT_INVEN || m_UIDesc.eUISlotType == SLOT_QUICK)
    {
        string uiTag = m_UIDesc.strTag;
        stringstream ss;

        for (char c : uiTag)
        {
            if (std::isdigit(c))
                ss << c;
        }

        int foundNum{ 0 };
        ss >> foundNum;

        m_iSlotIndex = foundNum;
    }

    return S_OK;
}

void CUISlot::PriorityTick(_float _fDeltaTime)
{

}

void CUISlot::Tick(_float _fDeltaTime)
{
    if (m_UIDesc.eUISlotType == SLOT_MOUSE)
        FollowMouse();

    KeyInput();

    if (m_UIDesc.eUISlotType == SLOT_MOUSE)
    {
        SlotKeyInput();
    }
    else if (m_UIDesc.eUISlotType == SLOT_QUICK)
    {
        SlotKeyInput();

        if (m_UIDesc.strTag.find("QuickCombatSlot0") != std::string::npos || m_UIDesc.strTag.find("QuickMainSlot0") != std::string::npos ||
            m_UIDesc.strTag == "QuickMainSlot1" || m_UIDesc.strTag.find("QuickMainSlot2") != std::string::npos ||
            m_UIDesc.strTag.find("QuickMainSlot3") != std::string::npos || m_UIDesc.strTag.find("QuickMainSlot4") != std::string::npos)
        {
            _uint iCurWeaponIndex = ITEMMGR->GetCurrentWeaponIndex();

            switch (iCurWeaponIndex)
            {
            case 0:
            {
                m_iTexIndex = 1;
            }
            break;
            case 1:
            {

            }
            break;
            case 2:
            {

            }
            break;
            }
        }
    }
    else
    {
        switch (m_eUIItemType)
        {
        case UI_ITEM_EQUIP:
        {
            if (UIMGR->GetTabIndex() == 0)
            {
                SlotKeyInput();
            }
        }
        break;
        case UI_ITEM_USABLE:
        {
            if (UIMGR->GetTabIndex() == 1)
            {
                SlotKeyInput();
            }
        }
        break;
        case UI_ITEM_MISC:
        {
            if (UIMGR->GetTabIndex() == 2)
            {
                SlotKeyInput();
            }
        }
        break;
        }
    }

    /*if (m_strTexKeys.size() > 1 && IsMouseOn() && m_UIDesc.eUISlotType == SLOT_INVEN)
        m_iTexIndex = 1;
    else
        m_iTexIndex = 0;*/

    /*if (m_strTexKeys.size() > 1 && IsMouseOn())
        m_iTexIndex = 1;
    else
        m_iTexIndex = 0;*/

    if ((m_UIDesc.eUISlotType == SLOT_ABNORMAL) && IsMouseOn())
        m_fExpandUV = 0.05f;
    else
        m_fExpandUV = 0.f;

    if (m_UIDesc.eUISlotType == SLOT_INVEN)
    {
        if (m_pUIParent.expired())
            m_iSlotUV = 4;
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
}

void CUISlot::LateTick(_float _fDeltaTime)
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

    if (m_isRender)
    {
        if (!m_pUIChildren.empty())
        {
            for (auto& childPair : m_pUIChildren)
            {
                if (childPair.second.lock() != nullptr)
                {
                    if (childPair.second.lock()->GetUIType() == UI_ITEMHOVER)
                    {
                        if (dynamic_pointer_cast<CUIItemHover>(childPair.second.lock())->IsRender(m_fVisibleTop, m_fVisibleBottom))
                            childPair.second.lock()->LateTick(_fDeltaTime);
                    }

                    else
                        childPair.second.lock()->LateTick(_fDeltaTime);
                }
            }
        }
    }

    /* 비어 있으면 안보이게 처리 */
    if (m_UIDesc.eUISlotType == SLOT_INVEN)
    {
        if (m_isEmpty)
            m_iSlotUV = 3;
    }
}

HRESULT CUISlot::Render()
{
#pragma region Legacy Code
    /*if (m_UIDesc.eUISlotType != SLOT_MOUSE && m_isEmpty == true)
    {
        if (m_UIDesc.eUISlotType == SLOT_INVEN)
        {
            if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_UIDesc.eUISlotType, sizeof(UI_SLOT_TYPE))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotUV", &m_iSlotUV, sizeof(_uint))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_fRatio", &m_fYRatio, sizeof(_float))))
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
    }

    if (m_UIDesc.eUISlotType != SLOT_MOUSE && m_isEmpty == false)
    {
        if (m_UIDesc.eUISlotType == SLOT_INVEN)
        {
            if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_UIDesc.eUISlotType, sizeof(UI_SLOT_TYPE))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotUV", &m_iSlotUV, sizeof(_uint))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_fRatio", &m_fYRatio, sizeof(_float))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_SlotDesc.strImageTag)))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BeginShader()))
                return E_FAIL;

            if (FAILED(m_pBuffer->BindBuffers()))
                return E_FAIL;

            if (FAILED(m_pBuffer->Render()))
                return E_FAIL;

            return S_OK;
        }
    }

    if (m_UIDesc.eUISlotType == SLOT_MOUSE)
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, 0)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_UIDesc.eUISlotType, sizeof(UI_SLOT_TYPE))))
            return E_FAIL;

        if (!m_isEmpty)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_SlotDesc.strImageTag)))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BeginShader()))
                return E_FAIL;

            if (FAILED(m_pBuffer->BindBuffers()))
                return E_FAIL;

            if (FAILED(m_pBuffer->Render()))
                return E_FAIL;
        }

        return S_OK;
    }

    if (m_UIDesc.eUISlotType == SLOT_ABNORMAL)
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_UIDesc.eUISlotType, sizeof(UI_SLOT_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fExpandUV", &m_fExpandUV, sizeof(_float))))
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

    if (m_UIDesc.eUISlotType == SLOT_QUICK)
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_UIDesc.eUISlotType, sizeof(UI_SLOT_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fExpandUV", &m_fExpandUV, sizeof(_float))))
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
    }*/
#pragma endregion

if (m_UIDesc.eUISlotType != SLOT_MOUSE && m_isEmpty == true)
{
    if (m_UIDesc.eUISlotType == SLOT_INVEN)
    {
        if (m_iSlotUV == 3)
            return S_OK;

        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_UIDesc.eUISlotType, sizeof(UI_SLOT_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotUV", &m_iSlotUV, sizeof(_uint))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fRatio", &m_fYRatio, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_UIDesc.eUITexType, sizeof(UI_TEX_TYPE))))
            return E_FAIL;

        if (!m_strTexKeys.empty())
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
                return E_FAIL;
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

        if (FAILED(m_pBuffer->BindBuffers()))
            return E_FAIL;

        if (FAILED(m_pBuffer->Render()))
            return E_FAIL;

        return S_OK;
    }
}

if (m_UIDesc.eUISlotType != SLOT_MOUSE && m_isEmpty == false)
{
    if (m_UIDesc.eUISlotType == SLOT_INVEN)
    {
        if (m_iSlotUV == 3)
            return S_OK;

        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_UIDesc.eUISlotType, sizeof(UI_SLOT_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotUV", &m_iSlotUV, sizeof(_uint))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fRatio", &m_fYRatio, sizeof(_float))))
            return E_FAIL;

        if (m_pItem)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_pItem->GetItemTexTag())))
                return E_FAIL;
        }

        if (FAILED(GAMEINSTANCE->BeginShader()))
            return E_FAIL;

        if (FAILED(m_pBuffer->BindBuffers()))
            return E_FAIL;

        if (FAILED(m_pBuffer->Render()))
            return E_FAIL;

        return S_OK;
    }
}

if (m_UIDesc.eUISlotType == SLOT_MOUSE)
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, 0)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_UIDesc.eUISlotType, sizeof(UI_SLOT_TYPE))))
        return E_FAIL;

    if (!m_isEmpty)
    {
        if (m_pItem)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_pItem->GetItemTexTag())))
                return E_FAIL;
        }

        if (FAILED(GAMEINSTANCE->BeginShader()))
            return E_FAIL;

        if (FAILED(m_pBuffer->BindBuffers()))
            return E_FAIL;

        if (FAILED(m_pBuffer->Render()))
            return E_FAIL;
    }

    else
    {
        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[0])))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BeginShader()))
            return E_FAIL;

        if (FAILED(m_pBuffer->BindBuffers()))
            return E_FAIL;

        if (FAILED(m_pBuffer->Render()))
            return E_FAIL;
    }

    return S_OK;
}

if (m_UIDesc.eUISlotType == SLOT_ABNORMAL)
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_UIDesc.eUISlotType, sizeof(UI_SLOT_TYPE))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fExpandUV", &m_fExpandUV, sizeof(_float))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_UIDesc.eUITexType, sizeof(UI_TEX_TYPE))))
        return E_FAIL;

    /* If Slot is not empty(In this case, it is not Item but skill), it should Bind other textures */
    if (!m_strTexKeys.empty())
    {
        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;
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

    if (FAILED(m_pBuffer->BindBuffers()))
        return E_FAIL;

    if (FAILED(m_pBuffer->Render()))
        return E_FAIL;

    return S_OK;
}

if (m_UIDesc.eUISlotType == SLOT_QUICK)
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_UIDesc.eUISlotType, sizeof(UI_SLOT_TYPE))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fExpandUV", &m_fExpandUV, sizeof(_float))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_UIDesc.eUITexType, sizeof(UI_TEX_TYPE))))
        return E_FAIL;

    if (m_isEmpty)
    {
        /* If Slot is not empty(In this case, it is not Item but skill), it should Bind other textures */
        if (!m_strTexKeys.empty())
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
                return E_FAIL;
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
    }
    else
    {
        if (m_pItem)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_pItem->GetItemTexTag())))
                return E_FAIL;
        }
    }

    if (FAILED(GAMEINSTANCE->BeginShader()))
        return E_FAIL;

    if (FAILED(m_pBuffer->BindBuffers()))
        return E_FAIL;

    if (FAILED(m_pBuffer->Render()))
        return E_FAIL;

    return S_OK;
}

    return S_OK;
}

bool CUISlot::IsRender(_float _fVisibleYTop, _float _fVisibleYBottom)
{
    m_fVisibleTop = _fVisibleYTop;
    m_fVisibleBottom = _fVisibleYBottom;

    if ((m_fY - (m_fSizeY * 0.5f)) > _fVisibleYBottom && (m_fY + (m_fSizeY * 0.5f)) < _fVisibleYTop)
    {
        m_iSlotUV = 2;

        return true;
    }

    if ((m_fY + (m_fSizeY * 0.5f)) > _fVisibleYTop && (m_fY - (m_fSizeY * 0.5f)) < _fVisibleYTop)
    {
        m_fYRatio = 1.f - ((_fVisibleYTop - (m_fY - (m_fSizeY * 0.5f))) / m_fSizeY);

        m_iSlotUV = 1;

        return true;
    }

    if ((m_fY - (m_fSizeY * 0.5f)) < _fVisibleYBottom && (m_fY + (m_fSizeY * 0.5f)) > _fVisibleYBottom)
    {
        m_fYRatio = ((m_fY + (m_fSizeY * 0.5f)) - _fVisibleYBottom) / m_fSizeY;

        m_iSlotUV = 0;

        return true;
    }

    else
    {
        m_iSlotUV = 3;

        return false;
    }

    return false;
}

void CUISlot::AddItemToSlot(shared_ptr<CItemBase> _pItem)
{
    if (_pItem == nullptr)
        return;

    m_pItem = _pItem;
    m_isEmpty = false;
}

void CUISlot::RemoveItemFromSlot()
{
    if (m_isEmpty == false && m_pItem)
    {
        m_pItem.reset();
        m_pItem = nullptr;
        m_isEmpty = true;
    }
}

void CUISlot::FollowMouse()
{
    POINT mousePt;
    ::GetCursorPos(&mousePt);
    ::ScreenToClient(g_hWnd, &mousePt);

    _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
    _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

    SetUIPos(mousePosX, mousePosY);
}

void CUISlot::SlotKeyInput()
{
    if (GAMEINSTANCE->MousePressing(DIM_LB) && IsMouseOn() && m_UIDesc.eUISlotType != SLOT_MOUSE)
    {
        shared_ptr<CUISlot> mouseSlot = dynamic_pointer_cast<CUISlot>(UIMGR->FindUI("MouseSlot"));
        if (mouseSlot == nullptr)
            return;

        if (mouseSlot->IsEmpty() && !m_isEmpty)
        {
            mouseSlot->AddItemToSlot(GetSlotItem());
            mouseSlot->SetSlotTag(GetUITag());
            mouseSlot->SetPrevSlotIndex(m_iSlotIndex);
            mouseSlot->SetPrevSlotType(m_UIDesc.eUISlotType);
            mouseSlot->SetIsEmpty(false);
        }
    }

    if (GAMEINSTANCE->MouseUp(DIM_LB) && m_UIDesc.eUISlotType != SLOT_MOUSE && IsMouseOn())
    {
        shared_ptr<CUISlot> mouseSlot = dynamic_pointer_cast<CUISlot>(UIMGR->FindUI("MouseSlot"));
        if (mouseSlot == nullptr)
            return;

        if (!mouseSlot->IsEmpty() && m_UIDesc.eUISlotType == SLOT_INVEN)
        {
            if (m_isEmpty == true) // Slot that about to drop Item is Empty
            {
                _uint iPrevSlotIndex = mouseSlot->GetPrevSlotIndex();
                UI_SLOT_TYPE ePrevSlotType = mouseSlot->GetPrevSlotType();
                shared_ptr<CItemBase> pItem = mouseSlot->GetSlotItem();

                shared_ptr<CUISlot> pPrevSlot{ nullptr };
                if (ePrevSlotType == SLOT_INVEN)
                {
                    pPrevSlot = ITEMMGR->GetSlotByIndex(pItem->GetItemType(), iPrevSlotIndex);
                    AddItemToSlot(pItem);
                    m_isEmpty = false;
                }
                else if (ePrevSlotType == SLOT_QUICK)
                {
                    pPrevSlot = ITEMMGR->GetQuickSlotByIndex(iPrevSlotIndex);
                }

                mouseSlot->RemoveItemFromSlot();
                mouseSlot->SetIsEmpty(true);
                pPrevSlot->RemoveItemFromSlot();
                pPrevSlot->SetIsEmpty(true);
            }

            else // Slot that about to drop Item is Not Empty, Swap Item
            {
                _uint iPrevSlotIndex = mouseSlot->GetPrevSlotIndex();
                UI_SLOT_TYPE ePrevSlotType = mouseSlot->GetPrevSlotType();
                shared_ptr<CItemBase> pInItem = mouseSlot->GetSlotItem();
                shared_ptr<CItemBase> pOutItem = m_pItem;

                shared_ptr<CUISlot> pPrevSlot{ nullptr };
                if (ePrevSlotType == SLOT_INVEN)
                {
                    pPrevSlot = ITEMMGR->GetSlotByIndex(pInItem->GetItemType(), iPrevSlotIndex);
                    AddItemToSlot(pInItem);
                    m_isEmpty = false;
                    mouseSlot->RemoveItemFromSlot();
                    mouseSlot->SetIsEmpty(true);
                    ITEMMGR->AddItemByIndex(iPrevSlotIndex, pOutItem);
                }

                else if (ePrevSlotType == SLOT_QUICK)
                {
                    pPrevSlot = ITEMMGR->GetQuickSlotByIndex(iPrevSlotIndex);
                    mouseSlot->RemoveItemFromSlot();
                    mouseSlot->SetIsEmpty(true);
                    pPrevSlot->RemoveItemFromSlot();
                    pPrevSlot->SetIsEmpty(true);
                }
            }
        }

        else if (!mouseSlot->IsEmpty() && m_UIDesc.eUISlotType == SLOT_QUICK)
        {
            if (m_isEmpty == true) // Slot that about to drop Item is Empty
            {
                _uint iPrevSlotIndex = mouseSlot->GetPrevSlotIndex();
                UI_SLOT_TYPE ePrevSlotType = mouseSlot->GetPrevSlotType();
                shared_ptr<CItemBase> pItem = mouseSlot->GetSlotItem();

                shared_ptr<CUISlot> pPrevSlot{ nullptr };
                if (ePrevSlotType == SLOT_INVEN)
                {
                    pPrevSlot = ITEMMGR->GetSlotByIndex(pItem->GetItemType(), iPrevSlotIndex);
                    AddItemToSlot(pItem);
                    m_isEmpty = false;
                }
                else if (ePrevSlotType == SLOT_QUICK)
                {
                    pPrevSlot = ITEMMGR->GetQuickSlotByIndex(iPrevSlotIndex);
                }
                
                AddItemToSlot(pItem);
                m_isEmpty = false;
                mouseSlot->RemoveItemFromSlot();
                mouseSlot->SetIsEmpty(true);
                if (pPrevSlot->GetSlotType() == SLOT_QUICK)
                {
                    pPrevSlot->RemoveItemFromSlot();
                    pPrevSlot->SetIsEmpty(true);
                }
            }

            else // Slot that about to drop Item is Not Empty
            {
                _uint iPrevSlotIndex = mouseSlot->GetPrevSlotIndex();
                UI_SLOT_TYPE ePrevSlotType = mouseSlot->GetPrevSlotType();
                shared_ptr<CItemBase> pInItem = mouseSlot->GetSlotItem();
                shared_ptr<CItemBase> pOutItem = m_pItem;

                shared_ptr<CUISlot> pPrevSlot{ nullptr };
                if (ePrevSlotType == SLOT_INVEN)
                {
                    pPrevSlot = ITEMMGR->GetSlotByIndex(pInItem->GetItemType(), iPrevSlotIndex);
                }
                else if (ePrevSlotType == SLOT_QUICK)
                {
                    pPrevSlot = ITEMMGR->GetQuickSlotByIndex(iPrevSlotIndex);
                }

                RemoveItemFromSlot();
                AddItemToSlot(pInItem);
                mouseSlot->RemoveItemFromSlot();
                mouseSlot->SetIsEmpty(true);
                if (pPrevSlot->GetSlotType() == SLOT_QUICK)
                {
                    pPrevSlot->RemoveItemFromSlot();
                    ITEMMGR->AddItemToQuickSlotByIndex(iPrevSlotIndex, pOutItem);
                }
            }
        }
    }

    if (GAMEINSTANCE->MouseUp(DIM_LB) && m_UIDesc.eUISlotType == SLOT_MOUSE)
    {
        shared_ptr<CUISlot> mouseSlot = dynamic_pointer_cast<CUISlot>(UIMGR->FindUI("MouseSlot"));
        if (mouseSlot == nullptr)
            return;

        if (!mouseSlot->IsEmpty())
        {
            _uint iPrevSlotIndex = mouseSlot->GetPrevSlotIndex();
            UI_SLOT_TYPE ePrevSlotType = mouseSlot->GetPrevSlotType();
            //shared_ptr<CUISlot> pPrevSlot{ nullptr };
            vector<shared_ptr<CItemBase>>* pPrevSlots{ nullptr };
            if (ePrevSlotType == SLOT_QUICK)
            {
                /*pPrevSlot = ITEMMGR->GetQuickSlotByIndex(iPrevSlotIndex);
                pPrevSlot->RemoveItemFromSlot();*/
                pPrevSlots = ITEMMGR->GetItemQuickSlots();
                (*pPrevSlots)[iPrevSlotIndex].reset();
                (*pPrevSlots)[iPrevSlotIndex] = nullptr;
            }

            mouseSlot->RemoveItemFromSlot();
            mouseSlot->SetIsEmpty(true);
        }
    }

    if (GAMEINSTANCE->MouseDown(DIM_RB) && m_UIDesc.eUISlotType != SLOT_MOUSE && IsMouseOn())
    {
        if (!m_isEmpty && m_pItem)
        {
            if (m_pItem->GetItemType() == ITEM_EQUIP)
            {
                if (!m_pItem->IsEquipped())
                {
                    ITEMMGR->EquipItem(m_pItem->GetItemEquipType(), m_pItem->GetItemModelTag());
                    m_pItem->SetIsEquipped(true);
                }

                else
                {
                    if (m_pItem->GetItemEquipType() == ITEM_EQUIP_HELMET || m_pItem->GetItemEquipType() == ITEM_EQUIP_CAPE)
                    {
                        ITEMMGR->TakeOffItem(m_pItem->GetItemEquipType());
                        m_pItem->SetIsEquipped(false);
                    }
                }
            }
            
            else if (m_pItem->GetItemType() == ITEM_USABLE)
            {
                switch (m_pItem->GetItemUsableType())
                {
                case ITEM_USABLE_HP:
                {
                    if (m_pItem->DecreaseNumItem(1) == 1)
                    {
                        ITEMMGR->UseHPPotion(static_cast<_float>(m_pItem->GetItemValue()));
                    }
                    else if (m_pItem->DecreaseNumItem(1) <= 0)
                    {
                        ITEMMGR->UseHPPotion(static_cast<_float>(m_pItem->GetItemValue()));
                        RemoveItemFromSlot();
                        m_isEmpty = true;
                    }
                }
                break;
                case ITEM_USABLE_MP:
                {

                }
                break;
                }
            }
        }
    }

    if (GAMEINSTANCE->KeyDown(DIK_F1) && m_UIDesc.eUISlotType == SLOT_QUICK)
    {
        if (m_UIDesc.strTag == "QuickFixedSlot0" && !m_isEmpty && m_pItem)
        {
            switch (m_pItem->GetItemUsableType())
            {
            case ITEM_USABLE_HP:
            {
                if (m_pItem->DecreaseNumItem(1) == 1)
                {
                    ITEMMGR->UseHPPotion(static_cast<_float>(m_pItem->GetItemValue()));
                }
                else if (m_pItem->DecreaseNumItem(1) <= 0)
                {
                    ITEMMGR->UseHPPotion(static_cast<_float>(m_pItem->GetItemValue()));
                    RemoveItemFromSlot();
                    m_isEmpty = true;
                }
            }
            break;
            case ITEM_USABLE_MP:
            {

            }
            break;
            }
        }
    }
}

HRESULT CUISlot::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    if (m_UIDesc.eUISlotType != SLOT_MOUSE)
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
    }

    else
    {
        shared_ptr<CVIInstancePoint> pBufferCom = dynamic_pointer_cast<CVIInstancePoint>(GAMEINSTANCE->GetBuffer("Buffer_InstancePoint"));
        if (pBufferCom == nullptr)
            return E_FAIL;

        m_pBuffer = dynamic_pointer_cast<CVIInstancePoint>(pBufferCom->Clone(1, &_InstanceDesc));
        if (m_pBuffer == nullptr)
            return E_FAIL;

        m_strTexKeys = _strTextureTags;
    }

    return S_OK;
}

shared_ptr<CUISlot> CUISlot::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
{
    shared_ptr<CUISlot> pInstance = make_shared<CUISlot>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc, _initPos)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUISlot::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
