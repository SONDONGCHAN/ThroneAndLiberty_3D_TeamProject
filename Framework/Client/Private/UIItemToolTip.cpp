#include "UIItemToolTip.h"
#include "ItemEquip.h"
#include "ItemUsable.h"
#include "ItemMisc.h"

#include "VIInstanceUI.h"

CUIItemToolTip::CUIItemToolTip()
{

}

CUIItemToolTip::~CUIItemToolTip()
{

}

HRESULT CUIItemToolTip::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

void CUIItemToolTip::PriorityTick(_float _fTimeDelta)
{

}

void CUIItemToolTip::Tick(_float _fTimeDelta)
{
    if (!m_isLoadingDone)
        return;

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

void CUIItemToolTip::LateTick(_float _fTimeDelta)
{
    if (!m_isLoadingDone)
        return;

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

HRESULT CUIItemToolTip::Render()
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
            if (i == 3 && !m_pDisplayItem.expired())
            {
                string shaderKey = "g_DiffuseTexture" + to_string(i);

                if (FAILED(GAMEINSTANCE->BindSRV(shaderKey.c_str(), m_pDisplayItem.lock()->GetItemTexTag())))
                    return E_FAIL;

                continue;
            }

            if (i == 3 && m_pDisplayItemDesc.strItemTag != "")
            {
                string shaderKey = "g_DiffuseTexture" + to_string(i);

                if (FAILED(GAMEINSTANCE->BindSRV(shaderKey.c_str(), m_pDisplayItemDesc.strTexTag)))
                    return E_FAIL;

                continue;
            }

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

HRESULT CUIItemToolTip::RenderFont()
{
    /*_uint strSize = static_cast<_uint>(m_strQuestTitle.size());

    if (strSize > 7)
    {
        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", m_strQuestTitle, _float2((m_vUIPos[4].x - (38.f + (strSize - 7))) + (g_iWinSizeX * 0.5f), (-m_vUIPos[4].y - 120.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.79f, 0.65f, 0.28f, 1.f))))
            return E_FAIL;
    }

    else
    {
        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", m_strQuestTitle, _float2((m_vUIPos[4].x - 38.f) + (g_iWinSizeX * 0.5f), (-m_vUIPos[4].y - 120.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.79f, 0.65f, 0.28f, 1.f))))
            return E_FAIL;
    }

    if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", L"1500", _float2((m_vUIPos[6].x + 20.f) + (g_iWinSizeX * 0.5f), (-m_vUIPos[6].y - 120.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", L"1000", _float2((m_vUIPos[7].x + 20.f) + (g_iWinSizeX * 0.5f), (-m_vUIPos[7].y - 120.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", L"완료 보상", _float2((m_vUIPos[8].x - 25.f) + (g_iWinSizeX * 0.5f), (-m_vUIPos[8].y - 120.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;*/

    if (m_UIDesc.strTag == "ToolTipInstance")
    {
        if (!m_pDisplayItem.expired())
        {
            string strDisplayItemName = m_pDisplayItem.lock()->GetItemTag();

            wchar_t wstrDisplayItemName[MAX_PATH] = L"";

            MultiByteToWideChar(CP_ACP, 0, strDisplayItemName.c_str(), static_cast<_uint>(strDisplayItemName.size()), wstrDisplayItemName, MAX_PATH);

            wstring wDisplayName = wstrDisplayItemName;
            _float fDisplayNameOffset = static_cast<_uint>(wDisplayName.size()) * 6.f;

            if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", wDisplayName, _float2((m_vUIPos[4].x - fDisplayNameOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[4].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.79f, 0.65f, 0.28f, 1.f))))
                return E_FAIL;

            _uint iNumItem = m_pDisplayItem.lock()->GetNumItem();

            wstring strNumItem = to_wstring(iNumItem);

            _float fNumItemOffset = static_cast<_uint>(strNumItem.size()) * 4.f;

            if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", strNumItem, _float2((m_vUIPos[7].x + 30.f - fNumItemOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[7].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f))))
                return E_FAIL;

            ITEM_TYPE eItemType = m_pDisplayItem.lock()->GetItemType();
            _uint iValue = m_pDisplayItem.lock()->GetItemValue();

            wstring strItemType;
            wstring strItemValue;
            _float fValueOffset{ 0.f };

            switch (eItemType)
            {
            case ITEM_EQUIP:
            {
                strItemType = L"장비";
                strItemValue = L"방어력 : " + to_wstring(iValue);
            }
            break;
            case ITEM_USABLE:
            {
                strItemType = L"소모품";
                if (m_pDisplayItem.lock()->GetItemUsableType() == ITEM_USABLE_HP)
                {
                    strItemValue = L"생명력 회복 : " + to_wstring(iValue);
                }
                else if (m_pDisplayItem.lock()->GetItemUsableType() == ITEM_USABLE_MP)
                {
                    strItemValue = L"마나 회복 : " + to_wstring(iValue);
                }
            }
            break;
            case ITEM_MISC:
            {
                strItemType = L"재료";

                strItemValue = L"판매가 : " + to_wstring(iValue);
            }
            break;
            }

            fValueOffset = static_cast<_float>(strItemValue.size()) * 4.f;

            if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", strItemType, _float2((m_vUIPos[6].x - 35.f) + (g_iWinSizeX * 0.5f), (-m_vUIPos[6].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", strItemValue, _float2((m_vUIPos[5].x - fValueOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[5].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f))))
                return E_FAIL;
        }
    }

    if (m_UIDesc.strTag == "ToolTipInstance_0")
    {
        if (m_pDisplayItemDesc.strItemTag != "")
        {
            string strDisplayItemName = m_pDisplayItemDesc.strItemTag;

            wchar_t wstrDisplayItemName[MAX_PATH] = L"";

            MultiByteToWideChar(CP_ACP, 0, strDisplayItemName.c_str(), static_cast<_uint>(strDisplayItemName.size()), wstrDisplayItemName, MAX_PATH);

            wstring wDisplayName = wstrDisplayItemName;
            _float fDisplayNameOffset = static_cast<_uint>(wDisplayName.size()) * 6.f;

            if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", wDisplayName, _float2((m_vUIPos[4].x - fDisplayNameOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[4].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.79f, 0.65f, 0.28f, 1.f))))
                return E_FAIL;

            ITEM_TYPE eItemType = m_pDisplayItemDesc.eItemType;
            _uint iValue = m_pDisplayItemDesc.iValue;

            wstring strItemType;
            wstring strItemValue;
            _float fValueOffset{ 0.f };

            switch (eItemType)
            {
            case ITEM_EQUIP:
            {
                strItemType = L"장비";
                strItemValue = L"방어력 : " + to_wstring(iValue);
            }
            break;
            case ITEM_USABLE:
            {
                strItemType = L"소모품";
                if (m_pDisplayItemDesc.eItemUsableType == ITEM_USABLE_HP)
                {
                    strItemValue = L"생명력 회복 : " + to_wstring(iValue);
                }
                else if (m_pDisplayItemDesc.iValue == ITEM_USABLE_MP)
                {
                    strItemValue = L"마나 회복 : " + to_wstring(iValue);
                }
            }
            break;
            case ITEM_MISC:
            {
                strItemType = L"재료";

                strItemValue = L"판매가 : " + to_wstring(iValue);
            }
            break;
            }

            fValueOffset = static_cast<_float>(strItemValue.size()) * 4.f;

            if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", strItemType, _float2((m_vUIPos[6].x - 35.f) + (g_iWinSizeX * 0.5f), (-m_vUIPos[6].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f))))
                return E_FAIL;

            if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", strItemValue, _float2((m_vUIPos[5].x - fValueOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[5].y - 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f))))
                return E_FAIL;
        }
    }

    return S_OK;
}

void CUIItemToolTip::InitUpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    for (_uint i = 0; i < iNumSlots; ++i)
    {
        if (i > 4)
            vertices[i].iTexIndex = 5;

        else
            vertices[i].iTexIndex = i;

        vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
        vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
        vertices[i].iSlotUV = 0;

        if (i == 4)
            vertices[i].vColor = _float4(1.f, 1.f, 1.f, 0.4f);
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUIItemToolTip::UpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    _int iNumMouse = IsInstanceMouseOn();

    if (m_UIDesc.strTag == "ToolTipInstance_0")
    {
        if (m_pDisplayItemDesc.strItemTag == "")
            vertices[3].iSlotUV = 1;
        else
            vertices[3].iSlotUV = 0;
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

HRESULT CUIItemToolTip::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

shared_ptr<CUIItemToolTip> CUIItemToolTip::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUIItemToolTip> pInstance = make_shared<CUIItemToolTip>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIItemToolTip::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
