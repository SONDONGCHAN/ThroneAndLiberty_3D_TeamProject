#include "UICover.h"

#include "VIInstancePoint.h"

CUICover::CUICover()
{

}

CUICover::~CUICover()
{

}

HRESULT CUICover::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
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

void CUICover::PriorityTick(_float _fDeltaTime)
{

}

void CUICover::Tick(_float _fDeltaTime)
{
    KeyInput();

    if (m_strTexKeys.size() > 1 && IsMouseOn())
        m_iTexIndex = 1;
    else
        m_iTexIndex = 0;

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

void CUICover::LateTick(_float _fDeltaTime)
{
    if (m_isOnce && m_isWorldUI && !m_pUIChildren.empty())
    {
        m_isOnce = false;

        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.second.lock() != nullptr)
                childPair.second.lock()->SetIsWorldUI(true);
        }

        if (m_iShaderPass == 0)
            m_iShaderPass = 14;
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
                childPair.second.lock()->LateTick(_fDeltaTime);
        }
    }
}

HRESULT CUICover::Render()
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

HRESULT CUICover::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
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

shared_ptr<CUICover> CUICover::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos)
{
    shared_ptr<CUICover> pInstance = make_shared<CUICover>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _initPos)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUICover::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
