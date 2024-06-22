#include "UIInstanceItemHover.h"
#include "UIScroll.h"
#include "UIMgr.h"

#include "VIInstanceUI.h"

CUIInstanceItemHover::CUIInstanceItemHover()
{

}

CUIInstanceItemHover::~CUIInstanceItemHover()
{

}

HRESULT CUIInstanceItemHover::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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
    m_fSizeY = _InstanceDesc.vSize.y;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_UIINSTANCE;
    m_iShaderPass = m_UIDesc.iShaderPassIdx;

    m_vSlotUVRatio.resize(m_vUIPos.size());

    return S_OK;
}

void CUIInstanceItemHover::PriorityTick(_float _fTimeDelta)
{

}

void CUIInstanceItemHover::Tick(_float _fTimeDelta)
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

void CUIInstanceItemHover::LateTick(_float _fTimeDelta)
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

    if (m_pUIParent.expired())
    {
        m_iInstanceRenderState = 3;
    }

    if (m_UIDesc.strTag != "InvEquipSlot0Hover")
        IsRender();
}

HRESULT CUIInstanceItemHover::Render()
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
        if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture0", m_strTexKeys[m_iTexIndex])))
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

    if (FAILED(m_pInstanceBuffer->BindBuffers()))
        return E_FAIL;

    if (FAILED(m_pInstanceBuffer->Render()))
        return E_FAIL;

    return S_OK;
}

void CUIInstanceItemHover::IsRender(_float _fVisibleYTop, _float _fVisibleYBottom)
{
    m_fVisibleTop = _fVisibleYTop;
    m_fVisibleBottom = _fVisibleYBottom;

    // InventoryScroll
    /*shared_ptr<CUIScroll> scrollUI = dynamic_pointer_cast<CUIScroll>(UIMGR->FindUI("InventoryScroll"));
    if (scrollUI == nullptr)
        return;

    if (!scrollUI->IsScrolling())
        return;*/

    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _int iNumSlot = IsInstanceMouseOn();

    // 얘를 어케 처리해야 할까...
    _uint numUIPos = static_cast<_uint>(m_vUIPos.size());
    for (_uint i = 0; i < numUIPos; ++i)
    {
        if ((m_vUIPos[i].y - (m_fSizeY * 0.5f)) > _fVisibleYBottom && (m_vUIPos[i].y + (m_fSizeY * 0.5f)) < _fVisibleYTop)
        {
            vertices[i].iSlotUV = 2;

            if (iNumSlot == -1 || iNumSlot != i)
                vertices[i].iSlotUV = 3;

            continue;
        }

        if ((m_vUIPos[i].y + (m_fSizeY * 0.5f)) > _fVisibleYTop && (m_vUIPos[i].y - (m_fSizeY * 0.5f)) < _fVisibleYTop)
        {
            m_fYRatio = 1.f - ((_fVisibleYTop - (m_vUIPos[i].y - (m_fSizeY * 0.5f))) / m_fSizeY);

            vertices[i].iSlotUV = 1;
            m_vSlotUVRatio[i].y = m_fYRatio;
            vertices[i].vUVRatio = _float2(m_vSlotUVRatio[i].x, m_vSlotUVRatio[i].y);

            if (iNumSlot == -1 || iNumSlot != i)
                vertices[i].iSlotUV = 3;

            continue;
        }

        if ((m_vUIPos[i].y - (m_fSizeY * 0.5f)) < _fVisibleYBottom && (m_vUIPos[i].y + (m_fSizeY * 0.5f)) > _fVisibleYBottom)
        {
            m_fYRatio = ((m_vUIPos[i].y + (m_fSizeY * 0.5f)) - _fVisibleYBottom) / m_fSizeY;

            vertices[i].iSlotUV = 0;
            m_vSlotUVRatio[i].y = m_fYRatio;
            vertices[i].vUVRatio = _float2(m_vSlotUVRatio[i].x, m_vSlotUVRatio[i].y);

            if (iNumSlot == -1 || iNumSlot != i)
                vertices[i].iSlotUV = 3;

            continue;
        }

        else
        {
            vertices[i].iSlotUV = 3;

            continue;
        }
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUIInstanceItemHover::IsRender()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _int iNumSlot = IsInstanceMouseOn();

    // 얘를 어케 처리해야 할까...
    _uint numUIPos = static_cast<_uint>(m_vUIPos.size());
    for (_uint i = 0; i < numUIPos; ++i)
    {
        vertices[i].iSlotUV = 2;

        if (iNumSlot == -1 || iNumSlot != i)
            vertices[i].iSlotUV = 3;
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

HRESULT CUIInstanceItemHover::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

shared_ptr<CUIInstanceItemHover> CUIInstanceItemHover::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUIInstanceItemHover> pInstance = make_shared<CUIInstanceItemHover>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIInstanceItemHover::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
