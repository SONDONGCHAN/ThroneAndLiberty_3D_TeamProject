#include "pch.h"
#include "ToolUIPanel.h"
#include "ToolUIItemBackGround.h"
#include "ToolUIItemIcon.h"
#include "ToolUIInstanceItemBG.h"

#include "GameInstance.h"
#include "Texture.h"
#include "VIInstancePoint.h"

CToolUIPanel::CToolUIPanel()
{
}

CToolUIPanel::~CToolUIPanel()
{
}

HRESULT CToolUIPanel::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc)
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
    m_iShaderPass = 7;
    m_ToolUIDesc.iShaderPassIdx = 7;

    return S_OK;
}

void CToolUIPanel::PriorityTick(_float _fDeltaTime)
{

}

void CToolUIPanel::Tick(_float _fDeltaTime)
{
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

    if (m_strTexKeys.size() > 1 && IsMouseOn())
        m_iTexIndex = 1;
    else
        m_iTexIndex = 0;

    if (m_pUIChildren.empty())
        m_iRenderState = 0;
    else
        m_iRenderState = 1;

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

void CToolUIPanel::LateTick(_float _fDeltaTime)
{
    if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, shared_from_this())))
        return;

    if (!m_pUIChildren.empty())
    {
        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.second.lock() != nullptr)
            {
                if (childPair.second.lock()->GetUIType() == TOOL_UI_ITEMBG)
                {
                    if (dynamic_pointer_cast<CToolUIItemBackGround>(childPair.second.lock())->IsRender(m_fVisibleTop, m_fVisibleBottom))
                        childPair.second.lock()->LateTick(_fDeltaTime);
                }

                else if (childPair.second.lock()->GetUIType() == TOOL_UI_ITEMICON)
                {
                    if (dynamic_pointer_cast<CToolUIItemIcon>(childPair.second.lock())->IsRender(m_fVisibleTop, m_fVisibleBottom))
                        childPair.second.lock()->LateTick(_fDeltaTime);
                }

                else if (childPair.second.lock()->GetUIType() == TOOL_UI_INSTANCEITEMBG)
                {
                    dynamic_pointer_cast<CToolUIInstanceItemBG>(childPair.second.lock())->IsRender(m_fVisibleTop, m_fVisibleBottom);
                    childPair.second.lock()->LateTick(_fDeltaTime);
                }

                else
                    childPair.second.lock()->LateTick(_fDeltaTime);
            }
        }
    }
}

HRESULT CToolUIPanel::Render()
{
    /* 자식이 하나라도 있을 경우에는 텍스쳐를 렌더링을 하지 않는다 */
    /* 자식이 있을 때 렌더링을 하는 경우는 툴에서 위치 잡기를 용이하게 하기 위해서 */
    if (!m_pUIChildren.empty())
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_ToolUIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iRenderState", &m_iRenderState, sizeof(_uint))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BeginShader()))
            return E_FAIL;

        if (FAILED(m_pBuffer->BindBuffers()))
            return E_FAIL;

        if (FAILED(m_pBuffer->Render()))
            return E_FAIL;
    }

    else
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_ToolUIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iRenderState", &m_iRenderState, sizeof(_uint))))
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
    }
    
    return S_OK;
}

void CToolUIPanel::SetVisibleRange(_float _fVisibleTop, _float _fVisibleBottom)
{
    m_fVisibleTop = _fVisibleTop;
    m_fVisibleBottom = _fVisibleBottom;
}

HRESULT CToolUIPanel::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
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

shared_ptr<CToolUIPanel> CToolUIPanel::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    shared_ptr<CToolUIPanel> pInstance = make_shared<CToolUIPanel>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CToolUIPanel::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
