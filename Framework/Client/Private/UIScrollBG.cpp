#include "UIScrollBG.h"
#include "UISlot.h"
#include "UIPanel.h"
#include "UIItemBackGround.h"

#include "VIInstancePoint.h"

CUIScrollBG::CUIScrollBG()
{

}

CUIScrollBG::~CUIScrollBG()
{

}

HRESULT CUIScrollBG::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float _fVisibleSizeY, _float2 _initPos)
{
    if (FAILED(Super::Initialize()))
        return E_FAIL;

    m_UIDesc = _ToolUIDesc;

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
    m_iShaderPass = m_UIDesc.iShaderPassIdx;

    /* Initial Data Setting */
    /* Inventory BackGround의 정보를 가져와 세팅할 필요가 있다 */
    m_fOriginVisibleYSize = _fVisibleSizeY;
    m_fVisibleYSize = (m_fOriginVisibleYSize * 2.f) / 3.f;

    //m_vVisibleCenter = _float2(m_fX, (m_fY + (m_fSizeY / 2.f)) - (m_fVisibleYSize / 2.f));
    //m_fVisibleUVPosY = ((m_fY + (m_fSizeY / 2.f)) - (m_vVisibleCenter.y)) / m_fSizeY;
    //m_fVisibleUVSizeY = m_fVisibleYSize / m_fSizeY;

    return S_OK;
}

void CUIScrollBG::PriorityTick(_float _fDeltaTime)
{

}

void CUIScrollBG::Tick(_float _fDeltaTime)
{
    KeyInput();

    BGKeyInput();

    /* 부모를 따라다니게끔 */
    UpdateVisibleRange();

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

void CUIScrollBG::LateTick(_float _fDeltaTime)
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
            if (childPair.second.lock()->GetUIType() == UI_ITEMBG)
            {
                if (dynamic_pointer_cast<CUIItemBackGround>(childPair.second.lock())->IsRender(m_vVisibleCenter.y + (m_fVisibleYSize / 2.f),
                    m_vVisibleCenter.y - (m_fVisibleYSize / 2.f)))
                    childPair.second.lock()->LateTick(_fDeltaTime);
            }

            else if (childPair.second.lock()->GetUIType() == UI_PANEL)
            {
                shared_ptr<CUIPanel> childPanel = dynamic_pointer_cast<CUIPanel>(childPair.second.lock());
                if (childPanel != nullptr)
                {
                    childPanel->SetVisibleRange(m_vVisibleCenter.y + (m_fVisibleYSize / 2.f), m_vVisibleCenter.y - (m_fVisibleYSize / 2.f));
                    childPanel->LateTick(_fDeltaTime);
                }
            }

            else
                childPair.second.lock()->LateTick(_fDeltaTime);
        }
    }
}

HRESULT CUIScrollBG::Render()
{
    if (!m_pUIParent.expired())
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fVisiblePosY", &m_fVisibleUVPosY, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fVisibleSizeY", &m_fVisibleUVSizeY, sizeof(_float))))
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
    }
    
    else
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, 11)))
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
    }

    return S_OK;
}

void CUIScrollBG::SetVisibleRange()
{
    m_vVisibleCenter = _float2(m_fX, (m_fY + (m_fSizeY / 2.f)) - (m_fVisibleYSize / 2.f));

    //m_fVisibleUVPosY = (m_fVisibleYSize / 2.f) / m_fSizeY;
    m_fVisibleUVPosY = ((m_fY + (m_fSizeY / 2.f)) - ((m_vVisibleCenter.y)) / m_fSizeY);
    m_fVisibleUVSizeY = m_fVisibleYSize / m_fSizeY;
}

void CUIScrollBG::SetVisibleSizeY(_float _fVisibleSizeY)
{
    /* 테스트 필요 */
    m_fVisibleYSize = _fVisibleSizeY;
    m_vVisibleCenter = _float2(m_fX, (m_fY + (m_fSizeY / 2.f)) - (m_fVisibleYSize / 2.f));

    m_fVisibleUVPosY = ((m_fY + (m_fSizeY / 2.f)) - (m_vVisibleCenter.y)) / m_fSizeY;
    m_fVisibleUVSizeY = m_fVisibleYSize / m_fSizeY;
}

void CUIScrollBG::SetBGInitPos(_float _fPosX, _float _fPosY)
{
    m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(_fPosX, _fPosY, 0.f));
    m_fX = _fPosX;
    m_fY = _fPosY;

    m_vVisibleCenter = _float2(m_fX, (m_fY + (m_fSizeY / 2.f)) - (m_fVisibleYSize / 2.f));
}

void CUIScrollBG::UpdateVisibleRange()
{
    if (!m_pUIParent.expired())
    {
        if (m_pUIParent.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) != m_vPrevParentPos)
        {
            _float2 movedDir = _float2(m_pUIParent.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) - _float2(m_vPrevParentPos.x, m_vPrevParentPos.y));
            movedDir.Normalize();
            m_fDifDistFromOrigin = SimpleMath::Vector2::Distance(_float2(m_pUIParent.lock()->GetTransform()->GetState(CTransform::STATE_POSITION)), _float2(m_vPrevParentPos.x, m_vPrevParentPos.y));

            m_vVisibleCenter += movedDir * m_fDifDistFromOrigin;
            _float2 newPos = _float2(m_fX, m_fY) + (movedDir * m_fDifDistFromOrigin);
            SetUIPos(newPos.x, newPos.y);
        }
    }

    /* Origin */
    m_fVisibleUVPosY = ((m_fY + (m_fSizeY / 2.f)) - (m_vVisibleCenter.y)) / m_fSizeY;
}

void CUIScrollBG::BGKeyInput()
{

}

HRESULT CUIScrollBG::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
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

shared_ptr<CUIScrollBG> CUIScrollBG::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float _fVisibleSizeY, _float2 _initPos)
{
    shared_ptr<CUIScrollBG> pInstance = make_shared<CUIScrollBG>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc, _fVisibleSizeY, _initPos)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIScrollBG::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
