#include "UIBackGround.h"
#include "UIItemTab.h"

#include "VIInstancePoint.h"

CUIBackGround::CUIBackGround()
{

}

CUIBackGround::~CUIBackGround()
{

}

HRESULT CUIBackGround::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc)
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
    m_eShaderType = ESHADER_TYPE::ST_UI;
    m_iShaderPass = m_UIDesc.iShaderPassIdx;

    if (m_UIDesc.eUIType == UI_HL)
        m_isRight = 0;

    else if (m_UIDesc.eUIType == UI_HR)
        m_isRight = 1;

    if (m_UIDesc.strTag == "InventoryPanel")
        m_isPickable = true;

    if (m_UIDesc.strTag == "LoadingBG1")
    {
        m_strLoading.emplace_back(L"Loading .    ");
        m_strLoading.emplace_back(L"Loading . .  ");
        m_strLoading.emplace_back(L"Loading . . .");
    }

    return S_OK;
}

void CUIBackGround::PriorityTick(_float _fDeltaTime)
{

}

void CUIBackGround::Tick(_float _fDeltaTime)
{
    if (m_isLoadingDone)
    {
        KeyInput();

        m_fIndexTime += _fDeltaTime;
        if (m_fIndexTime >= 0.5f)
        {
            m_iStrIndex++;
            if (m_iStrIndex >= static_cast<_uint>(m_strLoading.size()))
                m_iStrIndex = 0;

            m_fIndexTime = 0.f;
        }

        if (m_UIDesc.strTag == "PCInfoIconBG" || m_UIDesc.strTag == "LoadingBG0")
        {
            
        }

        else
        {
            if (m_strTexKeys.size() > 1 && IsMouseOn())
                m_iTexIndex = 1;
            else
                m_iTexIndex = 0;
        }

        if (m_UIDesc.strTag == "LoadingBG1")
        {
            m_fTimeAcc += _fDeltaTime * -45.f;
            RotateUI(0.f, 0.f, m_fTimeAcc);
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

                if (childPair.second.lock()->GetUIType() == UI_ITEMTAB)
                {
                    weak_ptr<CUIItemTab> childTab = dynamic_pointer_cast<CUIItemTab>(childPair.second.lock());
                    if (!childTab.expired())
                    {
                        if (childTab.lock()->IsTabActivated())
                            m_iTexIndex = 1;
                    }
                }
            }
        }

        if (!m_pUIParent.expired())
            m_vPrevParentPos = m_pUIParent.lock()->GETPOS;
    }
}

void CUIBackGround::LateTick(_float _fDeltaTime)
{
    if (m_isLoadingDone)
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

            else if (m_iShaderPass == 11)
                m_iShaderPass = 16;
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
}

HRESULT CUIBackGround::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    if (m_UIDesc.eUIType == UI_HL || m_UIDesc.eUIType == UI_HR)
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_iHF", &m_isRight, sizeof(_uint))))
            return E_FAIL;
    }

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

    if (FAILED(RenderFont()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUIBackGround::RenderFont()
{
    if (m_UIDesc.strTag == "LoadingBG1")
    {
        _float fContentOffset = static_cast<_float>(m_strLoading[m_iStrIndex].size()) * 16.f;

        if (FAILED(GAMEINSTANCE->RenderFont("Font_DNFB16", m_strLoading[m_iStrIndex], _float2((m_fX - fContentOffset) + (g_iWinSizeX * 0.5f), (-m_fY - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.65f, 0.52f, 0.42f, 1.f), 0.f, _float2(0.f, 0.f), 1.f)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CUIBackGround::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
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

shared_ptr<CUIBackGround> CUIBackGround::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    shared_ptr<CUIBackGround> pInstance = make_shared<CUIBackGround>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIBackGround::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
