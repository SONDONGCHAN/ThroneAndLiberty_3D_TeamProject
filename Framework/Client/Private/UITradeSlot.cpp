#include "UITradeSlot.h"

#include "GameInstance.h"
#include "VIInstanceUI.h"

CUITradeSlot::CUITradeSlot()
{

}

CUITradeSlot::~CUITradeSlot()
{

}

HRESULT CUITradeSlot::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

    m_SlotDescs.resize(32);
    m_bIsSelecteds.resize(32);

    if (FAILED(PrepareSlots()))
        return E_FAIL;

    return S_OK;
}

void CUITradeSlot::PriorityTick(_float _fTimeDelta)
{

}

void CUITradeSlot::Tick(_float _fTimeDelta)
{
    if (!m_isLoadingDone)
        return;

    KeyInput();
    TradeKeyInput();

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

void CUITradeSlot::LateTick(_float _fTimeDelta)
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

HRESULT CUITradeSlot::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_iInstanceRenderState", &m_iInstanceRenderState, sizeof(_uint))))
        return E_FAIL;

    /*if (!m_strTexKeys.empty())
    {
        _uint iNumTexKeys = static_cast<_uint>(m_strTexKeys.size());
        for (_uint i = 0; i < iNumTexKeys; ++i)
        {
            string shaderKey = "g_DiffuseTexture" + to_string(i);

            if (FAILED(GAMEINSTANCE->BindSRV(shaderKey.c_str(), m_strTexKeys[i])))
                return E_FAIL;
        }
    }*/

    if (!m_SlotDescs.empty())
    {
        _uint iNumTexKeys = static_cast<_uint>(m_SlotDescs.size());
        for (_uint i = 0; i < iNumTexKeys; ++i)
        {
            string shaderKey = "g_DiffuseTexture" + to_string(i);

            if (m_SlotDescs[i].strItemTag != "")
            {
                if (FAILED(GAMEINSTANCE->BindSRV(shaderKey.c_str(), m_SlotDescs[i].strTexTag)))
                    return E_FAIL;
            }
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

HRESULT CUITradeSlot::RenderFont()
{
    for (_uint i = 0; i < 11; ++i)
    {
        if (m_SlotDescs[i].strItemTag != "")
        {
            string displayTag = m_SlotDescs[i].strItemTag;

            wchar_t wDisplayItemTag[MAX_PATH] = L"";

            MultiByteToWideChar(CP_ACP, 0, displayTag.c_str(), static_cast<_int>(displayTag.size()), wDisplayItemTag, MAX_PATH);

            if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", wDisplayItemTag, _float2((m_vUIPos[i].x + 40.f) + (g_iWinSizeX * 0.5f), (-m_vUIPos[i].y - 7.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f))))
                return E_FAIL;
        }
    }

    for (_uint i = 12; i < 31; ++i)
    {
        if (m_SlotDescs[i].strItemTag != "")
        {
            wstring strNumItem = to_wstring(m_SlotDescs[i].numItem);
            _float istrOffset = static_cast<_float>(strNumItem.size()) * 9.f;

            if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", strNumItem, _float2((m_vUIPos[i].x + 22.5f - istrOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[i].y + 10.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 1.f)))
                return E_FAIL;
        }
    }

	return S_OK;
}

CItemBase::ItemDesc& CUITradeSlot::GetItemDesc(_uint _iSlotIndex)
{
    return m_SlotDescs[_iSlotIndex];
}

_bool CUITradeSlot::AddItemDescToSlot(CItemBase::ItemDesc& _itemDesc)
{
    if (_itemDesc.strItemTag == "")
        return false;

    for (_uint i = 12; i < static_cast<_uint>(m_SlotDescs.size()); ++i)
    {
        if (m_SlotDescs[i].strItemTag == _itemDesc.strItemTag)
        {
            m_SlotDescs[i].numItem += _itemDesc.numItem;

            break;
        }

        else if (m_SlotDescs[i].strItemTag == "")
        {
            m_SlotDescs[i] = _itemDesc;

            break;
        }
    }

    return true;
}

_bool CUITradeSlot::DeleteItemDescFromSlot(_uint _iSlotIndex)
{
    if (_iSlotIndex >= static_cast<_uint>(m_SlotDescs.size()))
        return false;

    CItemBase::ItemDesc itemDesc{};
    itemDesc.strItemTag = "";
    itemDesc.eItemType = ITEM_END;
    itemDesc.eItemUsableType = ITEM_USABLE_END;
    itemDesc.strTexTag = "BG_ItemGrade_01";
    itemDesc.iValue = 0;

    m_SlotDescs[_iSlotIndex] = itemDesc;

    return true;
}

_bool CUITradeSlot::IncreaseNumItem()
{
    if (m_iCurSelectedIdx == -1)
        return false;

    if (m_SlotDescs[m_iCurSelectedIdx].strItemTag == "")
        return false;

    if (!m_bIsSelecteds[m_iCurSelectedIdx])
        return false;

    m_SlotDescs[m_iCurSelectedIdx].numItem += 1;

    return true;
}

_bool CUITradeSlot::DecreaseNumItem()
{
    if (m_iCurSelectedIdx == -1)
        return false;

    if (m_SlotDescs[m_iCurSelectedIdx].strItemTag == "")
        return false;

    if (!m_bIsSelecteds[m_iCurSelectedIdx])
        return false;

    _uint iNumItem = m_SlotDescs[m_iCurSelectedIdx].numItem - 1;

    if (iNumItem > 0)
    {
        m_SlotDescs[m_iCurSelectedIdx].numItem -= 1;

        return true;
    }

    m_SlotDescs[m_iCurSelectedIdx].numItem = 1;

    return true;
}

_bool CUITradeSlot::ClearBuySlots()
{
    for (_uint i = 12; i < static_cast<_uint>(m_SlotDescs.size()); ++i)
    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "";
        itemDesc.eItemType = ITEM_END;
        itemDesc.eItemUsableType = ITEM_USABLE_END;
        itemDesc.strTexTag = "BG_ItemGrade_01";
        itemDesc.iValue = 0;

        m_SlotDescs[i] = itemDesc;
    }

    m_buySlotDescs.clear();

    return true;
}

vector<CItemBase::ItemDesc>& CUITradeSlot::GetBuySlotDescs()
{
    m_buySlotDescs.clear();

    _uint iNumSlots = static_cast<_uint>(m_SlotDescs.size());

    for (_uint i = 12; i < iNumSlots; ++i)
    {
        if (m_SlotDescs[i].strItemTag != "")
            m_buySlotDescs.emplace_back(m_SlotDescs[i]);
    }

    return m_buySlotDescs;
}

void CUITradeSlot::TradeKeyInput()
{
    if (m_UIDesc.strTag == "TradeCSlots")
    {
        _int iNumMouse = IsInstanceMouseOn();

        if (GAMEINSTANCE->MouseDown(DIM_LB) && iNumMouse != -1)
        {
            if (iNumMouse > 11 && m_bIsSelecteds[iNumMouse])
            {
                //DeleteItemDescFromSlot(iNumMouse);
                m_bIsSelecteds[iNumMouse] = false;
                m_iCurSelectedIdx = -1;
            }

            if (iNumMouse > 11 && !m_bIsSelecteds[iNumMouse])
            {
                for (_uint i = 12; i < static_cast<_uint>(m_bIsSelecteds.size()); ++i)
                    m_bIsSelecteds[i] = false;

                m_bIsSelecteds[iNumMouse] = true;
                m_iCurSelectedIdx = iNumMouse;
            }
        }
    }
}

void CUITradeSlot::InitUpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    for (_uint i = 0; i < iNumSlots; ++i)
    {
        vertices[i].iTexIndex = 0;
        vertices[i].iSlotUV = 1;

        vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
        vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUITradeSlot::UpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    _int iNumMouse = IsInstanceMouseOn();

    for (_uint i = 0; i < iNumSlots; ++i)
    {
        if (m_SlotDescs[i].strItemTag != "")
        {
            vertices[i].iSlotUV = 0;
            vertices[i].iTexIndex = i;
        }

        else
        {
            vertices[i].iSlotUV = 1;
        }
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

HRESULT CUITradeSlot::PrepareSlots()
{
    for (_uint i = 0; i < static_cast<_uint>(m_SlotDescs.size()); ++i)
    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "";
        itemDesc.eItemType = ITEM_END;
        itemDesc.eItemUsableType = ITEM_USABLE_END;
        itemDesc.strTexTag = "BG_ItemGrade_01";
        itemDesc.iValue = 0;
        itemDesc.numItem = 1;
        

        m_SlotDescs[i] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "소형 회복 물약";
        itemDesc.eItemType = ITEM_USABLE;
        itemDesc.eItemUsableType = ITEM_USABLE_HP;
        itemDesc.strTexTag = "I_Health_Potion_1";
        itemDesc.iValue = 100;
        itemDesc.numItem = 1;
        itemDesc.iPrice = 50;
        itemDesc.wstrItemDescription = TEXT("체력을 조금 회복시켜주는 물약");
        
        m_SlotDescs[0] = itemDesc;
    }
    
    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "중형 회복 물약";
        itemDesc.eItemType = ITEM_USABLE;
        itemDesc.eItemUsableType = ITEM_USABLE_HP;
        itemDesc.strTexTag = "I_Health_Potion_2";
        itemDesc.iValue = 500;
        itemDesc.numItem = 1;
        itemDesc.iPrice = 100;
        itemDesc.wstrItemDescription = TEXT("체력을 적당히 회복시켜주는 물약");

        m_SlotDescs[1] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "대형 회복 물약";
        itemDesc.eItemType = ITEM_USABLE;
        itemDesc.eItemUsableType = ITEM_USABLE_HP;
        itemDesc.strTexTag = "I_Health_Potion_3";
        itemDesc.iValue = 1000;
        itemDesc.numItem = 1;
        itemDesc.iPrice = 200;
        itemDesc.wstrItemDescription = TEXT("체력을 많이 회복시켜주는 물약");

        m_SlotDescs[2] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "대형 마나 물약";
        itemDesc.eItemType = ITEM_USABLE;
        itemDesc.eItemUsableType = ITEM_USABLE_MP;
        itemDesc.strTexTag = "I_Mana_Regen_Potion_3";
        itemDesc.iValue = 100;
        itemDesc.numItem = 1;
        itemDesc.iPrice = 1000;

        m_SlotDescs[3] = itemDesc;
    }

    //{
    //    CItemBase::ItemDesc itemDesc{};
    //    itemDesc.strItemTag = "방어구 제작석"; // 퀘스트 보상으로 뺀다?
    //    itemDesc.eItemType = ITEM_MISC;
    //    itemDesc.eItemUsableType = ITEM_USABLE_END;
    //    itemDesc.strTexTag = "I_Enchant_Armor_KAA_001";
    //    itemDesc.iValue = 300;
    //    itemDesc.numItem = 1;
    //    itemDesc.iPrice = 300;

    //    m_SlotDescs[4] = itemDesc;
    //}

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "단단한 부리";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.eItemUsableType = ITEM_USABLE_END;
        itemDesc.strTexTag = "I_Basilisk_001";
        itemDesc.iValue = 50;
        itemDesc.numItem = 1;
        itemDesc.iPrice = 50;

        m_SlotDescs[4] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "마력 가루";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.eItemUsableType = ITEM_USABLE_END;
        itemDesc.strTexTag = "I_material_blueprint_sub_037";
        itemDesc.iValue = 100;
        itemDesc.numItem = 1;
        itemDesc.iPrice = 100;

        m_SlotDescs[5] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "마력석 조각";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.eItemUsableType = ITEM_USABLE_END;
        itemDesc.strTexTag = "I_AmmoMaterial_4";
        itemDesc.iValue = 300;
        itemDesc.numItem = 1;
        itemDesc.iPrice = 300;

        m_SlotDescs[6] = itemDesc;
    }

    //{
    //    CItemBase::ItemDesc itemDesc{};
    //    itemDesc.strItemTag = "날카로운 발톱"; // 퀘스트 아이템으로 뺀다?
    //    itemDesc.eItemType = ITEM_MISC;
    //    itemDesc.eItemUsableType = ITEM_USABLE_END;
    //    itemDesc.strTexTag = "I_Claw_001";
    //    itemDesc.iValue = 100;
    //    itemDesc.numItem = 1;

    //    m_SlotDescs[7] = itemDesc;
    //}

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "온전한 마력석";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.eItemUsableType = ITEM_USABLE_END;
        itemDesc.strTexTag = "I_Core_002";
        itemDesc.iValue = 500;
        itemDesc.numItem = 1;
        itemDesc.iPrice = 500;

        m_SlotDescs[7] = itemDesc;
    }

    return S_OK;
}

HRESULT CUITradeSlot::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

shared_ptr<CUITradeSlot> CUITradeSlot::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUITradeSlot> pInstance = make_shared<CUITradeSlot>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUITradeSlot::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
