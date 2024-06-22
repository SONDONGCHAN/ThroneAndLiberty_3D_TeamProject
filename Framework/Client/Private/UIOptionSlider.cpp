#include "UIOptionSlider.h"
#include "UIOptionBG.h"
#include "UIOptionTab.h"
#include "UIOptionHover.h"
#include "UIOptionCheckBox.h"
#include "UIMgr.h"

#include "VIInstanceUI.h"

CUIOptionSlider::CUIOptionSlider()
{

}

CUIOptionSlider::~CUIOptionSlider()
{

}

HRESULT CUIOptionSlider::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    if (FAILED(Super::Initialize()))
        return E_FAIL;

    m_vUIPos = _vUIPos;
    m_vUISize = _vUISize;

    m_UIDesc = _UIDesc;

    if (FAILED(AddComponent(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _InstanceDesc, _vUIPos, _vUISize)))
        return E_FAIL;

    m_fX = _InstanceDesc.vCenter.x;
    m_fY = _InstanceDesc.vCenter.y;
    m_fSizeX = _InstanceDesc.vSize.x;
    m_fSizeY = _InstanceDesc.fSizeY;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_UIINSTANCE;
    m_iShaderPass = m_UIDesc.iShaderPassIdx;

    m_vSlotUVRatio.resize(m_vUIPos.size());
    m_fSliderRatios.resize(3);
    m_bIsSelected.resize(3);

    InitUpdateTexture();

    m_fSliderMin = m_vUIPos[0].x - (m_vUISize[0].x / 2.f);
    m_fSliderMax = m_vUIPos[0].x + (m_vUISize[0].x / 2.f);

    return S_OK;
}

void CUIOptionSlider::PriorityTick(_float _fTimeDelta)
{

}

void CUIOptionSlider::Tick(_float _fTimeDelta)
{
    if (m_isLoadingDone)
    {
        KeyInput();

        /* 부모를 따라다니게끔 */
        if (!m_pUIParent.expired() && (m_pUIParent.lock()->GETPOS != m_vPrevParentPos) && (m_vPrevParentPos != _float3(FLT_MAX, FLT_MAX, FLT_MAX)))
        {
            _float3 moveDir = m_pUIParent.lock()->GETPOS - m_vPrevParentPos;
            moveDir.Normalize();
            _float moveDist = SimpleMath::Vector3::Distance(m_pUIParent.lock()->GETPOS, m_vPrevParentPos);

            _float3 newPos = _float3(m_fX, m_fY, 0.f) + (moveDir * moveDist);
            SetUIPos(newPos.x, newPos.y);

            for (auto& texPos : m_vUIPos)
            {
                _float3 newTexPos = _float3(texPos.x, texPos.y, 0.f) + (moveDir * moveDist);
                texPos = _float2(newTexPos.x, newTexPos.y);
            }

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
                    childPair.second.lock()->Tick(_fTimeDelta);
            }
        }

        if (!m_pUIParent.expired())
            m_vPrevParentPos = m_pUIParent.lock()->GETPOS;
    }
}

void CUIOptionSlider::LateTick(_float _fTimeDelta)
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
                    childPair.second.lock()->LateTick(_fTimeDelta);
            }
        }

        UpdateTexture();
    }
}

HRESULT CUIOptionSlider::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_iInstanceRenderState", &m_iInstanceRenderState, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_vColor", &m_vColorAlpha, sizeof(_float4))))
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

    if (FAILED(m_pInstanceBuffer->BindBuffers()))
        return E_FAIL;

    if (FAILED(m_pInstanceBuffer->Render()))
        return E_FAIL;

    if (FAILED(RenderFont()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUIOptionSlider::RenderFont()
{
    return S_OK;
}

void CUIOptionSlider::SetOtherNonRender(_int _iIndex)
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    if (_iIndex == -1)
    {
        for (_uint i = 0; i < iNumSlots; ++i)
            vertices[i].iSlotUV = 0;
    }

    else
    {
        for (_uint i = 0; i < iNumSlots; ++i)
        {
            if (i == _iIndex || i == _iIndex + 1)
                continue;

            vertices[i].iSlotUV = 1;
        }
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUIOptionSlider::InitUpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    for (_uint i = 0; i < iNumSlots; ++i)
    {
        vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
        vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
        vertices[i].iSlotUV = 0;
    }

    vertices[0].iTexIndex = 0;
    vertices[2].iTexIndex = 0;
    vertices[4].iTexIndex = 0;

    vertices[1].iTexIndex = 1;
    vertices[3].iTexIndex = 1;
    vertices[5].iTexIndex = 1;

    vertices[6].iTexIndex = 3;
    vertices[8].iTexIndex = 3;
    vertices[10].iTexIndex = 3;

    vertices[7].iTexIndex = 4;
    vertices[9].iTexIndex = 4;
    vertices[11].iTexIndex = 4;

    // Default Value
    // 201.342
    // 197.682
    // 207.32
    vertices[1].vTranslation.x = 201.342f;
    m_vUIPos[1].x = vertices[1].vTranslation.x;
    vertices[3].vTranslation.x = 197.682f;
    m_vUIPos[3].x = vertices[3].vTranslation.x;
    vertices[5].vTranslation.x = 207.32f;
    m_vUIPos[5].x = vertices[5].vTranslation.x;

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUIOptionSlider::UpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    _int iNumMouse = IsInstanceMouseOn();

    if (IsInstanceMouseOn(1))
        vertices[1].iTexIndex = 2;
    else
        vertices[1].iTexIndex = 1;

    if (IsInstanceMouseOn(3))
        vertices[3].iTexIndex = 2;
    else
        vertices[3].iTexIndex = 1;

    if (IsInstanceMouseOn(5))
        vertices[5].iTexIndex = 2;
    else
        vertices[5].iTexIndex = 1;

    for (_uint i = 0; i < iNumSlots; ++i)
    {
        if (i == iNumMouse)
        {
            if (i == 6 || i == 8 || i == 10)
                vertices[i].iTexIndex = 5;

            if (i == 7 || i == 9 || i == 11)
                vertices[i].iTexIndex = 6;
        }
        else
        {
            if (i == 6 || i == 8 || i == 10)
                vertices[i].iTexIndex = 3;

            if (i == 7 || i == 9 || i == 11)
                vertices[i].iTexIndex = 4;
        }
    }

    if (GAMEINSTANCE->MouseDown(DIM_LB) && iNumMouse != -1)
    {
        if (iNumMouse == 6)
        {
            vertices[1].vTranslation.x -= 10.f;
            if (vertices[1].vTranslation.x <= m_fSliderMin)
                vertices[1].vTranslation.x = m_fSliderMin;

            m_vUIPos[1].x = vertices[1].vTranslation.x;
        }

        else if (iNumMouse == 8)
        {
            vertices[3].vTranslation.x -= 10.f;
            if (vertices[3].vTranslation.x <= m_fSliderMin)
                vertices[3].vTranslation.x = m_fSliderMin;

            m_vUIPos[3].x = vertices[3].vTranslation.x;
        }

        else if (iNumMouse == 10)
        {
            vertices[5].vTranslation.x -= 10.f;
            if (vertices[5].vTranslation.x <= m_fSliderMin)
                vertices[5].vTranslation.x = m_fSliderMin;

            m_vUIPos[5].x = vertices[5].vTranslation.x;
        }

        if (iNumMouse == 7)
        {
            vertices[1].vTranslation.x += 10.f;
            if (vertices[1].vTranslation.x >= m_fSliderMax)
                vertices[1].vTranslation.x = m_fSliderMax;

            m_vUIPos[1].x = vertices[1].vTranslation.x;
        }

        else if (iNumMouse == 9)
        {
            vertices[3].vTranslation.x += 10.f;
            if (vertices[3].vTranslation.x >= m_fSliderMax)
                vertices[3].vTranslation.x = m_fSliderMax;

            m_vUIPos[3].x = vertices[3].vTranslation.x;
        } 

        else if (iNumMouse == 11)
        {
            vertices[5].vTranslation.x += 10.f;
            if (vertices[5].vTranslation.x >= m_fSliderMax)
                vertices[5].vTranslation.x = m_fSliderMax;

            m_vUIPos[5].x = vertices[5].vTranslation.x;
        }
    }

    if (GAMEINSTANCE->MouseDown(DIM_LB))
    {
        if (IsInstanceMouseOn(1))
            m_bIsSelected[0] = true;

        else if (IsInstanceMouseOn(3))
            m_bIsSelected[1] = true;

        else if (IsInstanceMouseOn(5))
            m_bIsSelected[2] = true;
    }

    if (GAMEINSTANCE->MousePressing(DIM_LB) && IsInstanceMouseOn() != -1)
    {
        POINT mousePt;
        ::GetCursorPos(&mousePt);
        ::ScreenToClient(g_hWnd, &mousePt);

        _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
        _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

        if (m_bIsSelected[0])
        {
            vertices[1].vTranslation.x = mousePosX;
            if (vertices[1].vTranslation.x <= m_fSliderMin)
                vertices[1].vTranslation.x = m_fSliderMin;

            if (vertices[1].vTranslation.x >= m_fSliderMax)
                vertices[1].vTranslation.x = m_fSliderMax;

            m_vUIPos[1].x = vertices[1].vTranslation.x;

            weak_ptr<CUIOptionBG> pABGs = dynamic_pointer_cast<CUIOptionBG>(UIMGR->FindUI("OptionABGs"));
            if (!pABGs.expired())
                pABGs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            pABGs.lock()->SetIsFontRender(false);

            weak_ptr<CUIOptionBG> pDBGs = dynamic_pointer_cast<CUIOptionBG>(UIMGR->FindUI("OptionDBGs"));
            if (!pDBGs.expired())
                pDBGs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            pDBGs.lock()->SetIsFontRender(false);
            
            weak_ptr<CUIOptionTab> pBTabs = dynamic_pointer_cast<CUIOptionTab>(UIMGR->FindUI("OptionBTabs"));
            if (!pBTabs.expired())
                pBTabs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            pBTabs.lock()->SetIsFontRender(false);
            
            weak_ptr<CUIOptionTab> pCTabs = dynamic_pointer_cast<CUIOptionTab>(UIMGR->FindUI("OptionCTabs"));
            if (!pCTabs.expired())
                pCTabs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            pCTabs.lock()->SetIsFontRender(false);
            
            weak_ptr<CUIOptionHover> pEHover = dynamic_pointer_cast<CUIOptionHover>(UIMGR->FindUI("OptionEHovers"));
            if (!pEHover.expired())
                pEHover.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));
            
            SetOtherNonRender(0);
            
            weak_ptr<CUIOptionCheckBox> pGCheckBox = dynamic_pointer_cast<CUIOptionCheckBox>(UIMGR->FindUI("OptionGCheckButtons"));
            if (!pGCheckBox.expired())
                pGCheckBox.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));
        }

        else if (m_bIsSelected[1])
        {
            vertices[3].vTranslation.x = mousePosX;
            if (vertices[3].vTranslation.x <= m_fSliderMin)
                vertices[3].vTranslation.x = m_fSliderMin;

            if (vertices[3].vTranslation.x >= m_fSliderMax)
                vertices[3].vTranslation.x = m_fSliderMax;

            m_vUIPos[3].x = vertices[3].vTranslation.x;

            weak_ptr<CUIOptionBG> pABGs = dynamic_pointer_cast<CUIOptionBG>(UIMGR->FindUI("OptionABGs"));
            if (!pABGs.expired())
                pABGs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            pABGs.lock()->SetIsFontRender(false);

            weak_ptr<CUIOptionBG> pDBGs = dynamic_pointer_cast<CUIOptionBG>(UIMGR->FindUI("OptionDBGs"));
            if (!pDBGs.expired())
                pDBGs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            pDBGs.lock()->SetIsFontRender(false);

            weak_ptr<CUIOptionTab> pBTabs = dynamic_pointer_cast<CUIOptionTab>(UIMGR->FindUI("OptionBTabs"));
            if (!pBTabs.expired())
                pBTabs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            pBTabs.lock()->SetIsFontRender(false);

            weak_ptr<CUIOptionTab> pCTabs = dynamic_pointer_cast<CUIOptionTab>(UIMGR->FindUI("OptionCTabs"));
            if (!pCTabs.expired())
                pCTabs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            pCTabs.lock()->SetIsFontRender(false);

            weak_ptr<CUIOptionHover> pEHover = dynamic_pointer_cast<CUIOptionHover>(UIMGR->FindUI("OptionEHovers"));
            if (!pEHover.expired())
                pEHover.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            SetOtherNonRender(2);

            weak_ptr<CUIOptionCheckBox> pGCheckBox = dynamic_pointer_cast<CUIOptionCheckBox>(UIMGR->FindUI("OptionGCheckButtons"));
            if (!pGCheckBox.expired())
                pGCheckBox.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));
        }

        else if (m_bIsSelected[2])
        {
            vertices[5].vTranslation.x = mousePosX;
            if (vertices[5].vTranslation.x <= m_fSliderMin)
                vertices[5].vTranslation.x = m_fSliderMin;

            if (vertices[5].vTranslation.x >= m_fSliderMax)
                vertices[5].vTranslation.x = m_fSliderMax;

            m_vUIPos[5].x = vertices[5].vTranslation.x;

            weak_ptr<CUIOptionBG> pABGs = dynamic_pointer_cast<CUIOptionBG>(UIMGR->FindUI("OptionABGs"));
            if (!pABGs.expired())
                pABGs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            pABGs.lock()->SetIsFontRender(false);

            weak_ptr<CUIOptionBG> pDBGs = dynamic_pointer_cast<CUIOptionBG>(UIMGR->FindUI("OptionDBGs"));
            if (!pDBGs.expired())
                pDBGs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            pDBGs.lock()->SetIsFontRender(false);

            weak_ptr<CUIOptionTab> pBTabs = dynamic_pointer_cast<CUIOptionTab>(UIMGR->FindUI("OptionBTabs"));
            if (!pBTabs.expired())
                pBTabs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            pBTabs.lock()->SetIsFontRender(false);

            weak_ptr<CUIOptionTab> pCTabs = dynamic_pointer_cast<CUIOptionTab>(UIMGR->FindUI("OptionCTabs"));
            if (!pCTabs.expired())
                pCTabs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            pCTabs.lock()->SetIsFontRender(false);

            weak_ptr<CUIOptionHover> pEHover = dynamic_pointer_cast<CUIOptionHover>(UIMGR->FindUI("OptionEHovers"));
            if (!pEHover.expired())
                pEHover.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));

            SetOtherNonRender(4);

            weak_ptr<CUIOptionCheckBox> pGCheckBox = dynamic_pointer_cast<CUIOptionCheckBox>(UIMGR->FindUI("OptionGCheckButtons"));
            if (!pGCheckBox.expired())
                pGCheckBox.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 0.f));
        }

        m_fSliderRatios[0] = (vertices[1].vTranslation.x - m_fSliderMin) / (m_fSliderMax - m_fSliderMin);
        m_fSliderRatios[1] = (vertices[3].vTranslation.x - m_fSliderMin) / (m_fSliderMax - m_fSliderMin);
        m_fSliderRatios[2] = (vertices[5].vTranslation.x - m_fSliderMin) / (m_fSliderMax - m_fSliderMin);

        GAMEINSTANCE->SetToneMappingProperties(m_fSliderRatios[0], m_fSliderRatios[1], m_fSliderRatios[2]);
    }

    if (GAMEINSTANCE->MouseUp(DIM_LB))
    {
        for (_uint i = 0; i < static_cast<_uint>(m_bIsSelected.size()); ++i)
            m_bIsSelected[i] = false;

        weak_ptr<CUIOptionBG> pABGs = dynamic_pointer_cast<CUIOptionBG>(UIMGR->FindUI("OptionABGs"));
        if (!pABGs.expired())
            pABGs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 1.f));

        pABGs.lock()->SetIsFontRender(true);

        weak_ptr<CUIOptionBG> pDBGs = dynamic_pointer_cast<CUIOptionBG>(UIMGR->FindUI("OptionDBGs"));
        if (!pDBGs.expired())
            pDBGs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 1.f));

        pDBGs.lock()->SetIsFontRender(true);

        weak_ptr<CUIOptionTab> pBTabs = dynamic_pointer_cast<CUIOptionTab>(UIMGR->FindUI("OptionBTabs"));
        if (!pBTabs.expired())
            pBTabs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 1.f));

        pBTabs.lock()->SetIsFontRender(true);

        weak_ptr<CUIOptionTab> pCTabs = dynamic_pointer_cast<CUIOptionTab>(UIMGR->FindUI("OptionCTabs"));
        if (!pCTabs.expired())
            pCTabs.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 1.f));

        pCTabs.lock()->SetIsFontRender(true);

        weak_ptr<CUIOptionHover> pEHover = dynamic_pointer_cast<CUIOptionHover>(UIMGR->FindUI("OptionEHovers"));
        if (!pEHover.expired())
            pEHover.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 1.f));

        SetOtherNonRender(-1);

        weak_ptr<CUIOptionCheckBox> pGCheckBox = dynamic_pointer_cast<CUIOptionCheckBox>(UIMGR->FindUI("OptionGCheckButtons"));
        if (!pGCheckBox.expired())
            pGCheckBox.lock()->SetColorAlpha(_float4(1.f, 1.f, 1.f, 1.f));
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

HRESULT CUIOptionSlider::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    m_pInstanceBuffer = CVIInstanceUI::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), _InstanceDesc, _vUIPos);
    if (m_pInstanceBuffer == nullptr)
        return E_FAIL;

    m_strTexKeys = _strTextureTags;

    if (m_UIDesc.isMasked)
        m_strMaskKey = _strMaskTextureTag;

    if (m_UIDesc.isNoised)
        m_strNoiseKey = _strNoiseTextureTag;

    return S_OK;
}

shared_ptr<CUIOptionSlider> CUIOptionSlider::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUIOptionSlider> pInstance = make_shared<CUIOptionSlider>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIOptionSlider::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
