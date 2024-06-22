#include "UIOptionCheckBox.h"

#include "VIInstanceUI.h"

CUIOptionCheckBox::CUIOptionCheckBox()
{

}

CUIOptionCheckBox::~CUIOptionCheckBox()
{

}

HRESULT CUIOptionCheckBox::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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
    m_bIsChecked.resize(m_vUIPos.size());
    for (_uint i = 0; i < static_cast<_uint>(m_bIsChecked.size()); ++i)
        m_bIsChecked[i] = true;

    InitUpdateTexture();

    return S_OK;
}

void CUIOptionCheckBox::PriorityTick(_float _fTimeDelta)
{

}

void CUIOptionCheckBox::Tick(_float _fTimeDelta)
{
    if (m_isLoadingDone)
    {
        KeyInput();
        OptionKeyInput();

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

void CUIOptionCheckBox::LateTick(_float _fTimeDelta)
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

HRESULT CUIOptionCheckBox::Render()
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

HRESULT CUIOptionCheckBox::RenderFont()
{
    return S_OK;
}

void CUIOptionCheckBox::OptionKeyInput()
{
    _int iNumMouse = IsInstanceMouseOn();

    if (GAMEINSTANCE->MouseDown(DIM_LB) && iNumMouse != -1)
    {
        if (m_bIsChecked[iNumMouse])
            m_bIsChecked[iNumMouse] = false;

        else
            m_bIsChecked[iNumMouse] = true;

        if (iNumMouse == 0)
            GAMEINSTANCE->SwitchingHDROption(m_bIsChecked[iNumMouse]); // HDR

        if (iNumMouse == 1)
            GAMEINSTANCE->SwitchingPBROption(m_bIsChecked[iNumMouse]); // PBR

        if (iNumMouse == 2)
            GAMEINSTANCE->SwitchingAOOptions(m_bIsChecked[iNumMouse]); // AO

        if (iNumMouse == 3)
            GAMEINSTANCE->SwitchingShadowOption(m_bIsChecked[iNumMouse]); // Shadow

        if (iNumMouse == 4)
            GAMEINSTANCE->SwitchingDOF(m_bIsChecked[iNumMouse]); // DOF
    }
}

void CUIOptionCheckBox::InitUpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    for (_uint i = 0; i < iNumSlots; ++i)
    {
        vertices[i].iTexIndex = 0;
        vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
        vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
        vertices[i].iSlotUV = 0;
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUIOptionCheckBox::UpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    _int iNumMouse = IsInstanceMouseOn();

    for (_uint i = 0; i < iNumSlots; ++i)
    {
        if (i == iNumMouse)
        {
            if (m_bIsChecked[i])
                vertices[i].iTexIndex = 3;
            else
                vertices[i].iTexIndex = 1;
        }
        else
        {
            if (m_bIsChecked[i])
                vertices[i].iTexIndex = 2;
            else
                vertices[i].iTexIndex = 0;
        }
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

HRESULT CUIOptionCheckBox::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

shared_ptr<CUIOptionCheckBox> CUIOptionCheckBox::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUIOptionCheckBox> pInstance = make_shared<CUIOptionCheckBox>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIOptionCheckBox::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
