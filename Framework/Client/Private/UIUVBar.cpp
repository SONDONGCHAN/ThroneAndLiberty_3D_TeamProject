#include "UIUVBar.h"

#include "VIInstancePoint.h"

CUIUVBar::CUIUVBar()
{

}

CUIUVBar::~CUIUVBar()
{

}

HRESULT CUIUVBar::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    if (FAILED(Super::Initialize()))
        return E_FAIL;

    m_UIDesc = _ToolUIDesc;

    if (FAILED(AddComponent(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _InstanceDesc)))
        return E_FAIL;

    m_fX = _InstanceDesc.vCenter.x;
    m_fY = _InstanceDesc.vCenter.y;
    m_fSizeX = _InstanceDesc.vSize.x;
    m_fSizeY = _InstanceDesc.fSizeY;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_UIINSTANCE;
    m_iShaderPass = m_UIDesc.iShaderPassIdx;

    if (m_UIDesc.strTag == "PCInfoGaugeMP")
    {
        m_fBindRatio = 0.f;
        m_fRatio = 0.f;
        m_fPrevHP = 0.f;
    }

    if (m_UIDesc.strTag == "PCInfoGaugeHP")
    {
        m_fCurHP = 1000.f;
        m_fPrevHP = 1000.f;
        m_fMaxHP = 1000.f;
    }

    return S_OK;
}

void CUIUVBar::PriorityTick(_float _fDeltaTime)
{

}

void CUIUVBar::Tick(_float _fDeltaTime)
{
    m_fTimeAcc += _fDeltaTime;
    if (m_fTimeAcc > 0.02f)
    {
        m_iUVIndex++;
        if (m_iUVIndex >= m_UVs.size())
            m_iUVIndex = 0;

        m_fTimeAcc = 0.f;
    }

    if (m_fCurHP != m_fPrevHP)
    {
        m_fRatioTime += _fDeltaTime;
        if (m_fRatioTime >= 1.f)
        {
            m_fRatioTime = 0.f;
            m_fPrevHP = m_fCurHP;
        }

        m_fRatioHP = LinearIp(m_fPrevHP, m_fCurHP, m_fRatioTime);
        m_fBindRatio = m_fRatioHP / m_fMaxHP;
        m_fRatio = m_fRatioHP / m_fMaxHP;
        if (m_fBindRatio >= 1.f)
            m_fBindRatio = 1.f;

        if (m_fBindRatio <= 0.f)
            m_fBindRatio = 0.f;
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

void CUIUVBar::LateTick(_float _fDeltaTime)
{
    if (m_isOnce && m_isWorldUI && !m_pUIChildren.empty())
    {
        m_isOnce = false;

        for (auto& childPair : m_pUIChildren)
        {
            if (childPair.second.lock() != nullptr)
                childPair.second.lock()->SetIsWorldUI(true);
        }

        if (m_iShaderPass == 2)
            m_iShaderPass = 15;
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

HRESULT CUIUVBar::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    _float2 uvTL = _float2(m_UVs[m_iUVIndex].x, m_UVs[m_iUVIndex].y);
    if (FAILED(GAMEINSTANCE->BindRawValue("g_vTexcoordTL", &uvTL, sizeof(_float2))))
        return E_FAIL;

    _float2 uvBR = _float2(m_UVs[m_iUVIndex].z, m_UVs[m_iUVIndex].w);
    if (FAILED(GAMEINSTANCE->BindRawValue("g_vTexcoordBR", &uvBR, sizeof(_float2))))
        return E_FAIL;

    //m_fBindRatio = m_UVs[m_iUVIndex].x + (m_UVs[m_iUVIndex].z - m_UVs[m_iUVIndex].x) * m_fBindRatio;
    m_fBindRatio = uvTL.x + (uvBR.x - uvTL.x) * m_fRatio;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fRatio", &m_fBindRatio, sizeof(_float))))
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

    return S_OK;
}

void CUIUVBar::SetBar(_float _fCurHP, _float _fMaxHP)
{
    m_fCurHP = _fCurHP;
    m_fMaxHP = _fMaxHP;
    //m_fRatioTime = 0.f;

    //m_bStart = true;
}

HRESULT CUIUVBar::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    shared_ptr<CVIInstancePoint> pBufferCom = dynamic_pointer_cast<CVIInstancePoint>(GAMEINSTANCE->GetBuffer("Buffer_InstancePoint"));
    if (pBufferCom == nullptr)
        return E_FAIL;

    m_pBuffer = dynamic_pointer_cast<CVIInstancePoint>(pBufferCom->Clone(1, &_InstanceDesc));
    if (m_pBuffer == nullptr)
        return E_FAIL;

    if (!_strTextureTags.empty())
        m_strTexKeys = _strTextureTags;

    if (m_UIDesc.isMasked)
        m_strMaskKey = _strMaskTextureTag;

    if (m_UIDesc.isNoised)
        m_strNoiseKey = _strNoiseTextureTag;

    return S_OK;
}

shared_ptr<CUIUVBar> CUIUVBar::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    shared_ptr<CUIUVBar> pInstance = make_shared<CUIUVBar>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIUVBar::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
