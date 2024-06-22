#include "UINormalText.h"
#include "UIMgr.h"

#include "GameInstance.h"
#include "VIInstancePoint.h"

CUINormalText::CUINormalText()
{

}

CUINormalText::~CUINormalText()
{

}

HRESULT CUINormalText::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, shared_ptr<CCustomFont> _pCustomFont, wstring _strScript)
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
    m_eShaderType = ESHADER_TYPE::ST_UIINSTANCE;
    m_iShaderPass = m_UIDesc.iShaderPassIdx;

    m_Font = _pCustomFont;
    m_strScript = _strScript;

    return S_OK;
}

void CUINormalText::PriorityTick(_float _fDeltaTime)
{

}

void CUINormalText::Tick(_float _fDeltaTime)
{
    KeyInput();

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

void CUINormalText::LateTick(_float _fDeltaTime)
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

    if (!m_pUIParent.expired())
        m_iInstanceRenderState = 4;
}

HRESULT CUINormalText::Render()
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

    if (FAILED(m_pBuffer->BindBuffers()))
        return E_FAIL;

    if (FAILED(m_pBuffer->Render()))
        return E_FAIL;
    
    if (FAILED(GAMEINSTANCE->RenderFont("Font_AsiaG", m_strScript, _float2((m_fX - (m_fSizeX)) + (g_iWinSizeX * 0.5f), (-m_fY - (m_fSizeY)) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_LEFT, _float4(255.f / 255.f, 247.f / 255.f, 153.f / 255.f, 1.f))))
        return E_FAIL;

    return S_OK;
}

void CUINormalText::SwapText(const wstring& _strScript)
{
    m_strScript = _strScript;
}

HRESULT CUINormalText::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
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

shared_ptr<CUINormalText> CUINormalText::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, shared_ptr<CCustomFont> _pCustomFont, wstring _strScript)
{
    shared_ptr<CUINormalText> pInstance = make_shared<CUINormalText>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _pCustomFont, _strScript)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUINormalText::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
