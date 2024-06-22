#include "UIItemBackGround.h"
#include "UISlot.h"
#include "UIItemIcon.h"
#include "UIScrollBG.h"

#include "GameInstance.h"
#include "VIInstancePoint.h"

CUIItemBackGround::CUIItemBackGround()
{

}

CUIItemBackGround::~CUIItemBackGround()
{

}

HRESULT CUIItemBackGround::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
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

    return S_OK;
}

void CUIItemBackGround::PriorityTick(_float _fDeltaTime)
{

}

void CUIItemBackGround::Tick(_float _fDeltaTime)
{
    KeyInput();

    if (m_strTexKeys.size() > 1 && IsMouseOn())
        m_iTexIndex = 1;
    else
        m_iTexIndex = 0;

    if (m_pUIParent.expired())
        m_iSlotUV = 4;

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

void CUIItemBackGround::LateTick(_float _fDeltaTime)
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
                if (childPair.second.lock()->GetUIType() == UI_SLOT && childPair.second.lock()->GetSlotType() == SLOT_INVEN)
                {
                    if (dynamic_pointer_cast<CUISlot>(childPair.second.lock())->IsRender(m_fVisibleTop, m_fVisibleBottom))
                        childPair.second.lock()->LateTick(_fDeltaTime);
                }

                else if (childPair.second.lock()->GetUIType() == UI_ITEMICON)
                {
                    if (dynamic_pointer_cast<CUIItemIcon>(childPair.second.lock())->IsRender(m_fVisibleTop, m_fVisibleBottom))
                        childPair.second.lock()->LateTick(_fDeltaTime);
                }

                else
                    childPair.second.lock()->LateTick(_fDeltaTime);
            }
        }
    }
}

HRESULT CUIItemBackGround::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
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

bool CUIItemBackGround::IsRender(_float _fVisibleYTop, _float _fVisibleYBottom)
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

HRESULT CUIItemBackGround::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
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

shared_ptr<CUIItemBackGround> CUIItemBackGround::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
{
    shared_ptr<CUIItemBackGround> pInstance = make_shared<CUIItemBackGround>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _initPos)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIItemBackGround::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
