#include "pch.h"
#include "ToolUIBG.h"
#include "ToolUIScroll.h"
#include "ToolUISlot.h"
#include "ToolUIItemBackGround.h"
#include "ToolUIPanel.h"
#include "ToolUIInstanceItemBG.h"

#include "GameInstance.h"
#include "VIInstancePoint.h"

CToolUIBG::CToolUIBG()
{

}

CToolUIBG::~CToolUIBG()
{

}

HRESULT CToolUIBG::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float _fVisibleSizeY, _float2 _initPos)
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
    m_vOriginPos = _float2(m_fX, m_fY);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_UI;
    m_iShaderPass = 3;
    m_ToolUIDesc.iShaderPassIdx = 3;

    /* Initial Data Setting */
    /* Inventory BackGround의 정보를 가져와 세팅할 필요가 있다 */
    //m_fVisibleYSize = (m_fSizeY * 2.f) / 3.f;
    m_fOriginVisibleYSize = _fVisibleSizeY;
    m_fVisibleYSize = (m_fOriginVisibleYSize * 2.f) / 3.f;
    
    return S_OK;
}

void CToolUIBG::PriorityTick(_float _fDeltaTime)
{
   
}

void CToolUIBG::Tick(_float _fDeltaTime)
{
    m_pBuffer->TickUIScaling(_fDeltaTime);

    KeyInput();

    BGKeyInput();

    UpdateVisibleRange();

    m_BindWorldMat = m_pTransformCom->GetWorldMatrix();

    if (!m_pUIChildren.empty())
    {
        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.second.lock() != nullptr/* && childPair.second.lock()->GetUIType() != TOOL_UI_SCROLL*/)
                childPair.second.lock()->Tick(_fDeltaTime);
        }
    }

    if (!m_pUIParent.expired())
        m_vPrevParentPos = m_pUIParent.lock()->GETPOS;
}

void CToolUIBG::LateTick(_float _fDeltaTime)
{
    if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, shared_from_this())))
        return;

    if (!m_pUIChildren.empty())
    {
        for (auto& childPair : m_pUIChildren)
        {
           /* if (childPair.second.lock()->GetUIType() == TOOL_UI_SLOT && childPair.second.lock()->GetSlotType() == TOOL_SLOT_INVEN)
            {
                if (dynamic_pointer_cast<CToolUISlot>(childPair.second.lock())->IsRender(m_vVisibleCenter.y + (m_fVisibleYSize / 2.f),
                    m_vVisibleCenter.y - (m_fVisibleYSize / 2.f)))
                    childPair.second.lock()->LateTick(_fDeltaTime);
            }*/

            if (childPair.second.lock()->GetUIType() == TOOL_UI_ITEMBG)
            {
                if (dynamic_pointer_cast<CToolUIItemBackGround>(childPair.second.lock())->IsRender(m_vVisibleCenter.y + (m_fVisibleYSize / 2.f),
                    m_vVisibleCenter.y - (m_fVisibleYSize / 2.f)))
                    childPair.second.lock()->LateTick(_fDeltaTime);
            }
            else if (childPair.second.lock()->GetUIType() == TOOL_UI_PANEL)
            {
                shared_ptr<CToolUIPanel> childPanel = dynamic_pointer_cast<CToolUIPanel>(childPair.second.lock());
                if (childPanel != nullptr)
                {
                    childPanel->SetVisibleRange(m_vVisibleCenter.y + (m_fVisibleYSize / 2.f), m_vVisibleCenter.y - (m_fVisibleYSize / 2.f));
                    childPanel->LateTick(_fDeltaTime);
                }
            }
            else if (childPair.second.lock()->GetUIType() == TOOL_UI_INSTANCEITEMBG)
            {
                dynamic_pointer_cast<CToolUIInstanceItemBG>(childPair.second.lock())->IsRender(m_vVisibleCenter.y + (m_fVisibleYSize / 2.f), m_vVisibleCenter.y - (m_fVisibleYSize / 2.f));
                childPair.second.lock()->LateTick(_fDeltaTime);
            }
            else
                childPair.second.lock()->LateTick(_fDeltaTime);
            
            //if (childPair.second.lock()->GetUIType() == TOOL_UI_SLOT && childPair.second.lock()->GetSlotType() == TOOL_SLOT_INVEN)
            //{
            //    if (dynamic_pointer_cast<CToolUISlot>(childPair.second.lock())->IsRender(m_vVisibleCenter.y + (m_fVisibleYSize / 2.f),
            //        m_vVisibleCenter.y - (m_fVisibleYSize / 2.f)))
            //        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, childPair.second.lock());
            //}
        }
    }
}

HRESULT CToolUIBG::Render()
{
    if (!m_pUIParent.expired())
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_ToolUIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fVisiblePosY", &m_fVisibleUVPosY, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fVisibleSizeY", &m_fVisibleUVSizeY, sizeof(_float))))
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
    else
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, 11)))
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
    }
    
    return S_OK;
}

void CToolUIBG::SetVisibleRange()
{
    m_vVisibleCenter = _float2(m_fX, (m_fY + (m_fSizeY / 2.f)) - (m_fVisibleYSize / 2.f));

    //m_fVisibleUVPosY = (m_fVisibleYSize / 2.f) / m_fSizeY;
    m_fVisibleUVPosY = ((m_fY + (m_fSizeY / 2.f)) - ((m_vVisibleCenter.y)) / m_fSizeY);
    m_fVisibleUVSizeY = m_fVisibleYSize / m_fSizeY;
}

void CToolUIBG::SetVisibleSizeY(_float _visibleSizeY)
{
    /* 테스트 필요 */
    m_fVisibleYSize = _visibleSizeY;
    m_vVisibleCenter = _float2(m_fX, (m_fY + (m_fSizeY / 2.f)) - (m_fVisibleYSize / 2.f));

    m_fVisibleUVPosY = ((m_fY + (m_fSizeY / 2.f)) - (m_vVisibleCenter.y)) / m_fSizeY;
    m_fVisibleUVSizeY = m_fVisibleYSize / m_fSizeY;
}

void CToolUIBG::SetBGInitPos(_float _fPosX, _float _fPosY)
{
    m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(_fPosX, _fPosY, 0.f));
    m_fX = _fPosX;
    m_fY = _fPosY;

    m_vVisibleCenter = _float2(m_fX, (m_fY + (m_fSizeY / 2.f)) - (m_fVisibleYSize / 2.f));
}

void CToolUIBG::UpdateVisibleRange()
{
    if (!m_pUIParent.expired())
    {
        if (/*m_pUIParent.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) != m_vPrevParentPos &&*/ m_vPrevParentPos != _float3(FLT_MAX, FLT_MAX, FLT_MAX))
        {
            _float2 movedDir = _float2(m_pUIParent.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) - _float2(m_vPrevParentPos.x, m_vPrevParentPos.y));
            movedDir.Normalize();
            m_fDifDistFromOrigin = SimpleMath::Vector2::Distance(_float2(m_pUIParent.lock()->GetTransform()->GetState(CTransform::STATE_POSITION)), _float2(m_vPrevParentPos.x, m_vPrevParentPos.y));

            m_vVisibleCenter += movedDir * m_fDifDistFromOrigin;
            _float2 newPos = _float2(m_fX, m_fY) + (movedDir * m_fDifDistFromOrigin);
            SetUIPos(newPos.x, newPos.y);

            m_fVisibleUVPosY = ((m_fY + (m_fSizeY / 2.f)) - (m_vVisibleCenter.y)) / m_fSizeY;

            return;
        }

        //m_fVisibleUVPosY = ((m_fY + (m_fSizeY / 2.f)) - (m_vVisibleCenter.y)) / m_fSizeY;
    }

    m_fVisibleUVPosY = ((m_fY + (m_fSizeY / 2.f)) - (m_vVisibleCenter.y)) / m_fSizeY;
    /* Origin */
}

void CToolUIBG::BGKeyInput()
{

}

HRESULT CToolUIBG::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
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

shared_ptr<CToolUIBG> CToolUIBG::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float _fVisibleSizeY, _float2 _initPos)
{
    shared_ptr<CToolUIBG> pInstance = make_shared<CToolUIBG>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc, _fVisibleSizeY, _initPos)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CToolUIBG::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
