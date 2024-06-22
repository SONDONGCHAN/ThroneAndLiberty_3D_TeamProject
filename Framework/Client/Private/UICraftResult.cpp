#include "UICraftResult.h"
#include "UIMgr.h"

#include "VIInstanceUI.h"
#include "GameInstance.h"

CUICraftResult::CUICraftResult()
{

}

CUICraftResult::~CUICraftResult()
{

}

HRESULT CUICraftResult::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

    InitUpdateTexture();

    return S_OK;
}

void CUICraftResult::PriorityTick(_float _fTimeDelta)
{

}

void CUICraftResult::Tick(_float _fTimeDelta)
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

void CUICraftResult::LateTick(_float _fTimeDelta)
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
            if (m_isRender)
            {
                if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_WORLDUI, shared_from_this())))
                    return;
            }

        }

        else
        {
            if (m_isRender)
            {
                if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, shared_from_this())))
                    return;
            }
        }

        if (!m_pUIChildren.empty())
        {
            for (auto& childPair : m_pUIChildren)
            {
                if (childPair.second.lock() != nullptr)
                    childPair.second.lock()->LateTick(_fTimeDelta);
            }
        }

        //UpdateTexture();

        m_fRatio = m_fCur / m_fMax;
    }
}

HRESULT CUICraftResult::Render()
{
    if (m_isRender)
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iInstanceRenderState", &m_iInstanceRenderState, sizeof(_uint))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fCoolTimeRatio", &m_fRatio, sizeof(_float))))
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
    }

    return S_OK;
}

void CUICraftResult::ResetRatio()
{
    m_fCur = 0.f;
    m_fRatio = 0.f;
    m_bIsScriptEnabled = false;

    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    vertices[0].iSlotUV = 1;
    vertices[1].iSlotUV = 0;
    vertices[2].iSlotUV = 2;

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUICraftResult::RenderScript()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    vertices[0].iSlotUV = 0;
    vertices[1].iSlotUV = 1;
    vertices[2].iSlotUV = 1;

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);

    m_bIsScriptEnabled = true;
}

HRESULT CUICraftResult::RenderFont()
{
    wstring displaySuccess;

    switch (m_iSuccessType)
    {
    case -1:
    {
        displaySuccess = L"방어구 제작에 실패하셨습니다!";
    }
    break;
    case 0:
    {
        displaySuccess = L"방어구 제작에 성공하셨습니다!";
    }
    break;
    case 1:
    {
        displaySuccess = L"솔란트가 부족합니다!";
    }
    break;
    case 2:
    {
        displaySuccess = L"제작 재료가 부족합니다!";
    }
    break;
    }

    _float iDisplaySuccessOffset = static_cast<_float>(displaySuccess.size()) * 7.f;

    if (m_bIsScriptEnabled)
    {
        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", displaySuccess, _float2((m_vUIPos[0].x - iDisplaySuccessOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[0].y - 7.5f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.79f, 0.65f, 0.28f, 1.f), 0.f, _float2(0.f, 0.f), 1.f)))
            return E_FAIL;
    }

    return S_OK;
}

void CUICraftResult::InitUpdateTexture()
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

    vertices[0].iSlotUV = 1;

    vertices[1].iTexIndex = 1;
    vertices[1].vColor = _float4(1.f, 1.f, 1.f, 1.f);
    vertices[1].iSlotUV = 0;
    
    vertices[2].iTexIndex = 2;
    vertices[2].iSlotUV = 2;

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUICraftResult::UpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    _int iNumMouse = IsInstanceMouseOn();



    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

HRESULT CUICraftResult::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

shared_ptr<CUICraftResult> CUICraftResult::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUICraftResult> pInstance = make_shared<CUICraftResult>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUICraftResult::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
