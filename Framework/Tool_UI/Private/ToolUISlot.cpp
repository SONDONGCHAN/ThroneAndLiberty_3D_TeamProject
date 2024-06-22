#include "pch.h"
#include "ToolUISlot.h"
#include "ToolUIMgr.h"
#include "ToolUIIcon.h"
#include "ToolUIItemHover.h"

#include "GameInstance.h"
#include "Texture.h"
#include "VIInstancePoint.h"

CToolUISlot::CToolUISlot()
{

}

CToolUISlot::~CToolUISlot()
{

}

HRESULT CToolUISlot::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
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

    if (!_strTextureTags.empty())
        m_SlotDesc.strImageTag = _strTextureTags[0];

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
    m_iShaderPass = 4;
    m_ToolUIDesc.iShaderPassIdx = 4;

    if (m_ToolUIDesc.m_eUISlotType == TOOL_SLOT_ABNORMAL)
    {
        m_iShaderPass = 8;
        m_ToolUIDesc.iShaderPassIdx = 8;
    }

    m_SlotDesc.fExpandUV = 0.0f;

    return S_OK;
}

void CToolUISlot::PriorityTick(_float _fDeltaTime)
{

}

void CToolUISlot::Tick(_float _fDeltaTime)
{
    if (m_ToolUIDesc.m_eUISlotType == TOOL_SLOT_MOUSE)
        FollowMouse();

    /*if (!m_pUIParent.expired() && (m_pUIParent.lock()->GETPOS != m_vPrevParentPos) && (m_vPrevParentPos != _float3(FLT_MAX, FLT_MAX, FLT_MAX)))
    {
        _float3 moveDir = m_pUIParent.lock()->GETPOS - m_vPrevParentPos;
        moveDir.Normalize();
        _float moveDist = SimpleMath::Vector3::Distance(m_pUIParent.lock()->GETPOS, m_vPrevParentPos);

        _float3 newPos = _float3(m_fX, m_fY, 0.f) + (moveDir * moveDist);
        SetUIPos(newPos.x, newPos.y);
    }*/
    KeyInput();

    SlotKeyInput();

    /*if (m_strTexKeys.size() > 1 && IsMouseOn() && m_ToolUIDesc.m_eUISlotType == TOOL_SLOT_INVEN)
        m_iTexIndex = 1;
    else
        m_iTexIndex = 0;*/

    if (m_strTexKeys.size() > 1 && IsMouseOn())
        m_iTexIndex = 1;
    else
        m_iTexIndex = 0;

    if ((m_ToolUIDesc.m_eUISlotType == TOOL_SLOT_ABNORMAL) && IsMouseOn())
        m_SlotDesc.fExpandUV = 0.05f;
    else
        m_SlotDesc.fExpandUV = 0.f;

    if (m_ToolUIDesc.m_eUISlotType == TOOL_SLOT_INVEN)
    {
        if (m_pUIParent.expired())
            m_iSlotUV = 4;
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
    {
        m_BindWorldMat = m_pTransformCom->GetWorldMatrix();
    }

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

void CToolUISlot::LateTick(_float _fDeltaTime)
{
    /*if (m_pUIParent.expired())
    {
        if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, shared_from_this())))
            return;
    }*/

    if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, shared_from_this())))
        return;

    if (!m_pUIChildren.empty())
    {
        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.second.lock()->GetUIType() == TOOL_UI_ITEMHOVER)
            {
                if (dynamic_pointer_cast<CToolUIItemHover>(childPair.second.lock())->IsRender(m_fVisibleTop, m_fVisibleBottom))
                    childPair.second.lock()->LateTick(_fDeltaTime);
            }
            else
                childPair.second.lock()->LateTick(_fDeltaTime);
        }
    }
}

HRESULT CToolUISlot::Render()
{
    if (m_ToolUIDesc.m_eUISlotType != TOOL_SLOT_MOUSE && m_SlotDesc.isFilled == false)
    {
        if (m_ToolUIDesc.m_eUISlotType == TOOL_SLOT_INVEN)
        {
            if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_ToolUIDesc.fDiscardAlpha, sizeof(_float))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_ToolUIDesc.m_eUISlotType, sizeof(ETOOL_SLOT_TYPE))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotUV", &m_iSlotUV, sizeof(_uint))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_fRatio", &m_fYRatio, sizeof(_float))))
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
    }

    if (m_ToolUIDesc.m_eUISlotType != TOOL_SLOT_MOUSE && m_SlotDesc.isFilled)
    {
        if (m_ToolUIDesc.m_eUISlotType == TOOL_SLOT_INVEN)
        {
            if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_ToolUIDesc.fDiscardAlpha, sizeof(_float))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_ToolUIDesc.m_eUISlotType, sizeof(ETOOL_SLOT_TYPE))))
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
    
    if (m_ToolUIDesc.m_eUISlotType == TOOL_SLOT_MOUSE)
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, 0)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_ToolUIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_ToolUIDesc.m_eUISlotType, sizeof(ETOOL_SLOT_TYPE))))
            return E_FAIL;

        if (m_SlotDesc.isFilled)
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

    if (m_ToolUIDesc.m_eUISlotType == TOOL_SLOT_ABNORMAL)
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_ToolUIDesc.m_eUISlotType, sizeof(ETOOL_SLOT_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_ToolUIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        /* 확대를 위한 뭔가가 필요 */
        if (FAILED(GAMEINSTANCE->BindRawValue("g_fExpandUV", &m_SlotDesc.fExpandUV, sizeof(_float))))
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

    if (m_ToolUIDesc.m_eUISlotType == TOOL_SLOT_QUICK)
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iSlotType", &m_ToolUIDesc.m_eUISlotType, sizeof(ETOOL_SLOT_TYPE))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_ToolUIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        /* 확대를 위한 뭔가가 필요 */
        if (FAILED(GAMEINSTANCE->BindRawValue("g_fExpandUV", &m_SlotDesc.fExpandUV, sizeof(_float))))
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

    return S_OK;
}

bool CToolUISlot::IsRender(_float _fVisibleYTop, _float _fVisibleYBottom)
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

    /*if ((m_BindWorldMat._42 - (m_fSizeY * 0.5f)) >= _fVisibleYBottom && (m_BindWorldMat._42 + (m_fSizeY * 0.5f)) <= _fVisibleYTop)
    {
        m_iSlotUV = 2;

        return true;
    }

    if ((m_BindWorldMat._42 + (m_fSizeY * 0.5f)) >= _fVisibleYTop && (m_BindWorldMat._42 - (m_fSizeY * 0.5f)) <= _fVisibleYTop)
    {
        m_fYRatio = 1.f - ((_fVisibleYTop - (m_BindWorldMat._42 - (m_fSizeY * 0.5f))) / m_fSizeY);

        m_iSlotUV = 1;

        return true;
    }

    if ((m_BindWorldMat._42 - (m_fSizeY * 0.5f)) <= _fVisibleYBottom && (m_BindWorldMat._42 + (m_fSizeY * 0.5f)) >= _fVisibleYBottom)
    {
        m_fYRatio = (((m_BindWorldMat._42 + (m_fSizeY * 0.5f)) - _fVisibleYBottom) / m_fSizeY);

        m_iSlotUV = 0;

        return true;
    }

    else
        return false;

    return false;*/
}

void CToolUISlot::FollowMouse()
{
    POINT mousePt;
    ::GetCursorPos(&mousePt);
    ::ScreenToClient(g_hWnd, &mousePt);

    _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
    _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

    SetUIPos(mousePosX, mousePosY);
}

void CToolUISlot::SlotKeyInput()
{
    if (GAMEINSTANCE->MousePressing(DIM_LB) && SlotIsMouseOn() && m_ToolUIDesc.m_eUISlotType != TOOL_SLOT_MOUSE)
    {
        shared_ptr<CToolUISlot> mouseSlot = dynamic_pointer_cast<CToolUISlot>(UIMGR->FindUI("MouseSlot"));
        if (mouseSlot == nullptr)
            return;

        if (!mouseSlot->IsFilled())
        {
            mouseSlot->SetImageTag(m_strTexKeys[m_iTexIndex]);
            mouseSlot->SetSlotTag(GetUITag());
            mouseSlot->SetIsFilled(true);
        }
    }

    if (GAMEINSTANCE->MouseUp(DIM_LB) && m_ToolUIDesc.m_eUISlotType != TOOL_SLOT_MOUSE && SlotIsMouseOn())
    {
        shared_ptr<CToolUISlot> mouseSlot = dynamic_pointer_cast<CToolUISlot>(UIMGR->FindUI("MouseSlot"));
        if (mouseSlot == nullptr)
            return;

        if (mouseSlot->IsFilled())
        {
            SetImageTag(mouseSlot->GetImageTag());
            SetIsFilled(true);
            mouseSlot->SetIsFilled(false);
        }
    }

    if (GAMEINSTANCE->MouseUp(DIM_LB) && m_ToolUIDesc.m_eUISlotType == TOOL_SLOT_MOUSE)
    {
        shared_ptr<CToolUISlot> mouseSlot = dynamic_pointer_cast<CToolUISlot>(UIMGR->FindUI("MouseSlot"));
        if (mouseSlot == nullptr)
            return;

        if (mouseSlot->IsFilled())
        {
            mouseSlot->SetIsFilled(false);
        }
    }

}

_bool CToolUISlot::SlotIsMouseOn()
{
    /*if (!m_pUIParent.expired())
    {
        POINT mousePt;
        ::GetCursorPos(&mousePt);
        ::ScreenToClient(g_hWnd, &mousePt);

        _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
        _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

        if ((mousePosX >= (m_BindWorldMat._31 - m_fSizeX * 0.5f)) && (mousePosX <= (m_BindWorldMat._31 + m_fSizeX * 0.5f)) &&
            (mousePosY >= (m_BindWorldMat._32 - m_fSizeY * 0.5f)) && (mousePosY <= (m_BindWorldMat._32 + m_fSizeY * 0.5f)))
            return true;

        return false;
    }

    return false;*/

    if (!m_pUIParent.expired())
    {
        POINT mousePt;
        ::GetCursorPos(&mousePt);
        ::ScreenToClient(g_hWnd, &mousePt);

        _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
        _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

        if ((mousePosX >= (m_fX - m_fSizeX * 0.5f)) && (mousePosX <= (m_fX + m_fSizeX * 0.5f)) &&
            (mousePosY >= (m_fY - m_fSizeY * 0.5f)) && (mousePosY <= (m_fY + m_fSizeY * 0.5f)))
            return true;

        return false;
    }

    return false;
}

HRESULT CToolUISlot::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    if (m_ToolUIDesc.m_eUISlotType != TOOL_SLOT_MOUSE)
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
    }
    
    else
    {
        shared_ptr<CVIInstancePoint> pBufferCom = dynamic_pointer_cast<CVIInstancePoint>(GAMEINSTANCE->GetBuffer("Buffer_InstancePoint"));
        if (pBufferCom == nullptr)
            return E_FAIL;

        m_pBuffer = dynamic_pointer_cast<CVIInstancePoint>(pBufferCom->Clone(1, &_InstanceDesc));
        if (m_pBuffer == nullptr)
            return E_FAIL;
    }

    return S_OK;
}

shared_ptr<CToolUISlot> CToolUISlot::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
{
    shared_ptr<CToolUISlot> pInstance = make_shared<CToolUISlot>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc, _initPos)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CToolUISlot::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
