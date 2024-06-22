#include "UICraftSlot.h"
#include "UICraftTab.h"
#include "UIMgr.h"
#include "ItemMgr.h"
#include "ItemEquip.h"
#include "ItemMisc.h"

#include "GameInstance.h"
#include "VIInstanceUI.h"

CUICraftSlot::CUICraftSlot()
{
}

CUICraftSlot::~CUICraftSlot()
{
}

HRESULT CUICraftSlot::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

    m_ItemDescs.resize(m_vUIPos.size() * 4);

    if (FAILED(PrepareItemDescs()))
        return E_FAIL;

    return S_OK;
}

void CUICraftSlot::PriorityTick(_float _fTimeDelta)
{

}

void CUICraftSlot::Tick(_float _fTimeDelta)
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

void CUICraftSlot::LateTick(_float _fTimeDelta)
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

HRESULT CUICraftSlot::Render()
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

    if (FAILED(m_pInstanceBuffer->BindBuffers()))
        return E_FAIL;

    if (FAILED(m_pInstanceBuffer->Render()))
        return E_FAIL;

    RenderFonts();

    return S_OK;
}

void CUICraftSlot::RenderFonts()
{
    /*{
        string displayItemTag = m_ItemDescs[0].strItemTag;
        wchar_t wDisplayItemTag[MAX_PATH] = L"";

        MultiByteToWideChar(CP_UTF8, 0, displayItemTag.c_str(), static_cast<_int>(displayItemTag.size()), wDisplayItemTag, MAX_PATH);

        if (FAILED(GAMEINSTANCE->RenderFont("Font_AdenL10", wDisplayItemTag, _float2((m_vUIPos[0].x + 40.f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[0].y - 10.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_END, _float4(255.f / 255.f, 247.f / 255.f, 153.f / 255.f, 1.f))))
            return;
    }

    {
        string displayItemTag = m_ItemDescs[1].strItemTag;
        wchar_t wDisplayItemTag[MAX_PATH] = L"";

        MultiByteToWideChar(CP_UTF8, 0, displayItemTag.c_str(), static_cast<_int>(displayItemTag.size()), wDisplayItemTag, MAX_PATH);

        if (FAILED(GAMEINSTANCE->RenderFont("Font_AdenL10", wDisplayItemTag, _float2((m_vUIPos[1].x + 40.f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[1].y - 10.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_END, _float4(255.f / 255.f, 247.f / 255.f, 153.f / 255.f, 1.f))))
            return;
    }

    {
        string displayItemTag = m_ItemDescs[2].strItemTag;
        wchar_t wDisplayItemTag[MAX_PATH] = L"";

        MultiByteToWideChar(CP_UTF8, 0, displayItemTag.c_str(), static_cast<_int>(displayItemTag.size()), wDisplayItemTag, MAX_PATH);

        if (FAILED(GAMEINSTANCE->RenderFont("Font_AdenL10", wDisplayItemTag, _float2((m_vUIPos[2].x + 40.f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[2].y - 10.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_END, _float4(255.f / 255.f, 247.f / 255.f, 153.f / 255.f, 1.f))))
            return;
    }

    {
        string displayItemTag = m_ItemDescs[3].strItemTag;
        wchar_t wDisplayItemTag[MAX_PATH] = L"";

        MultiByteToWideChar(CP_UTF8, 0, displayItemTag.c_str(), static_cast<_int>(displayItemTag.size()), wDisplayItemTag, MAX_PATH);

        if (FAILED(GAMEINSTANCE->RenderFont("Font_AdenL10", wDisplayItemTag, _float2((m_vUIPos[3].x + 40.f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[3].y - 10.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_END, _float4(255.f / 255.f, 247.f / 255.f, 153.f / 255.f, 1.f))))
            return;
    }

    {
        string displayItemTag = m_ItemDescs[4].strItemTag;
        wchar_t wDisplayItemTag[MAX_PATH] = L"";

        MultiByteToWideChar(CP_UTF8, 0, displayItemTag.c_str(), static_cast<_int>(displayItemTag.size()), wDisplayItemTag, MAX_PATH);

        if (FAILED(GAMEINSTANCE->RenderFont("Font_AdenL10", wDisplayItemTag, _float2((m_vUIPos[4].x + 40.f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[4].y - 10.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_END, _float4(255.f / 255.f, 247.f / 255.f, 153.f / 255.f, 1.f))))
            return;
    }

    {
        string displayItemTag = m_ItemDescs[5].strItemTag;
        wchar_t wDisplayItemTag[MAX_PATH] = L"";

        MultiByteToWideChar(CP_UTF8, 0, displayItemTag.c_str(), static_cast<_int>(displayItemTag.size()), wDisplayItemTag, MAX_PATH);

        if (FAILED(GAMEINSTANCE->RenderFont("Font_AdenL10", wDisplayItemTag, _float2((m_vUIPos[5].x + 40.f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[5].y - 10.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_END, _float4(255.f / 255.f, 247.f / 255.f, 153.f / 255.f, 1.f))))
            return;
    }

    {
        string displayItemTag = m_ItemDescs[7].strItemTag;
        wchar_t wDisplayItemTag[MAX_PATH] = L"";

        MultiByteToWideChar(CP_UTF8, 0, displayItemTag.c_str(), static_cast<_int>(displayItemTag.size()), wDisplayItemTag, MAX_PATH);

        if (FAILED(GAMEINSTANCE->RenderFont("Font_AdenL10", wDisplayItemTag, _float2((m_vUIPos[7].x + 40.f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[7].y - 10.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_END, _float4(255.f / 255.f, 247.f / 255.f, 153.f / 255.f, 1.f))))
            return;
    }*/

    weak_ptr<CUICraftTab> pCraftTab = dynamic_pointer_cast<CUICraftTab>(UIMGR->FindUI("CraftFTabs"));
    _uint iCurTabIdx = pCraftTab.lock()->GetCurTabIndex();

    for (_uint i = 0; i < 12; ++i)
    {
        if (i == 6)
            continue;

        string displayItemTag = m_ItemDescs[i + (12 * (iCurTabIdx))].strItemTag;
        if (displayItemTag != "")
        {
            wchar_t wDisplayItemTag[MAX_PATH] = L"";

            MultiByteToWideChar(CP_ACP, 0, displayItemTag.c_str(), static_cast<_int>(displayItemTag.size()), wDisplayItemTag, MAX_PATH);

            if (i > 6)
            {
                if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", wDisplayItemTag, _float2((m_vUIPos[i].x + 40.f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[i].y - 20.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f))))
                    return;

                wstring numNeed = to_wstring(m_ItemDescs[i + (12 * (iCurTabIdx))].numItem);
                wstring dash = L" / ";

                string strItemTag = m_ItemDescs[i + (12 * (iCurTabIdx))].strItemTag;
                vector<shared_ptr<CItemBase>>* pMiscSlots = ITEMMGR->GetItemSlots(ITEM_MISC);
                auto it = find_if((*pMiscSlots).begin(), (*pMiscSlots).end(), [&strItemTag](shared_ptr<CItemBase> pItem) {
                    if (pItem != nullptr)
                    {
                        if (pItem->GetItemTag() == strItemTag)
                            return true;
                    }

                    return false;
                    });

                wstring curNum;
                if (it == (*pMiscSlots).end())
                    curNum = L"0";

                else
                    curNum = to_wstring((*it)->GetNumItem());

                if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", curNum + dash + numNeed, _float2((m_vUIPos[i].x + 40.f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[i].y) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f))))
                    return;
            }

            else
            {
                if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", wDisplayItemTag, _float2((m_vUIPos[i].x + 40.f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[i].y - 10.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f))))
                    return;
            }
        }
    }
}

void CUICraftSlot::InitUpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    for (_uint i = 0; i < iNumSlots; ++i)
    {
        vertices[i].iTexIndex = 0;
        vertices[i].iSlotUV = 0;
        vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
        vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUICraftSlot::UpdateTexture()
{
    weak_ptr<CUICraftTab> pCraftTab = dynamic_pointer_cast<CUICraftTab>(UIMGR->FindUI("CraftFTabs"));

    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    _int iNumMouse = IsInstanceMouseOn();

    _uint iTabIndex = pCraftTab.lock()->GetCurTabIndex();

    for (_uint i = 0; i < iNumSlots; ++i)
    {
        //if (i == 6)
        //    continue;

        switch (iTabIndex)
        {
        case 0:
        {
            vertices[i].iTexIndex = i;
            if (i == 0)
                vertices[0].iSlotUV = 0;
            else if (i == 3)
                vertices[3].iSlotUV = 1;
        }
        break;
        case 1:
        {
            vertices[i].iTexIndex = i + 12;
            if (i == 0)
                vertices[0].iSlotUV = 0;
            else if (i == 3)
                vertices[3].iSlotUV = 0;
        }
        break;
        case 2:
        {
            vertices[i].iTexIndex = i + 24;
            if (i == 0)
                vertices[0].iSlotUV = 0;
            else if (i == 3)
                vertices[3].iSlotUV = 1;
        }
        break;
        case 3:
        {
            vertices[i].iTexIndex = i + 36;

            if (i == 0)
                vertices[0].iSlotUV = 0;
            else if (i == 3)
                vertices[3].iSlotUV = 1;
        }
        break;
        }
    }

    if (GAMEINSTANCE->MouseDown(DIM_LB) && iNumMouse != -1)
        vertices[6].iTexIndex = vertices[iNumMouse].iTexIndex;

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

HRESULT CUICraftSlot::PrepareItemDescs()
{
    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "특급 저항군 맹금두건";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_HELMET;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_LE_M_HM_05002";
        itemDesc.strModelTag = "Helmet_Leather_2";
        itemDesc.iValue = 2;

        m_ItemDescs[0] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "특급 저항군 맹금갑옷";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_UPPER;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_LE_M_TS_05002";
        itemDesc.strModelTag = "Upper_Leather_2";
        itemDesc.iValue = 5;

        m_ItemDescs[1] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "특급 저항군 맹금바지";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_LE_M_PT_05002";
        itemDesc.strModelTag = "Lower_Leather_2";
        itemDesc.iValue = 3;

        m_ItemDescs[2] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};

        m_ItemDescs[3] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "특급 저항군 맹금장갑";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_LE_M_GL_05002";
        itemDesc.strModelTag = "Glove_Leather_2";
        itemDesc.iValue = 1;

        m_ItemDescs[4] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "특급 저항군 맹금장화";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_LE_M_BT_05002";
        itemDesc.strModelTag = "Boots_Leather_2";
        itemDesc.iValue = 2;

        m_ItemDescs[5] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};

        m_ItemDescs[6] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "방어구 제작석";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Enchant_Armor_KAA_001";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[7] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "마력석 조각";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_AmmoMaterial_4";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[8] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "온전한 마력석";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Core_002";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[9] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "단단한 부리";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Basilisk_001";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[10] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "날카로운 발톱";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Claw_001";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[11] = itemDesc;
    }

    { /* Helmet */
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "지옥 인도자의 면갑";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_HELMET;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_set_PL_M_HM_00019";
        itemDesc.strModelTag = "Helmet_Plate_1";
        itemDesc.iValue = 5;

        m_ItemDescs[12] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "지옥 인도자의 갑옷";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_UPPER;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_set_PL_M_TS_00019";
        itemDesc.strModelTag = "Upper_Plate_1";
        itemDesc.iValue = 10;

        m_ItemDescs[13] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "지옥 인도자의 각반";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_set_PL_M_PT_00019";
        itemDesc.strModelTag = "Lower_Plate_1";
        itemDesc.iValue = 8;

        m_ItemDescs[14] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "지옥 인도자의 망토";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_CAPE;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_M_CA_00019";
        itemDesc.strModelTag = "Cape_Plate_1";
        itemDesc.iValue = 3;

        m_ItemDescs[15] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "지옥 인도자의 장갑";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_set_PL_M_GL_00019";
        itemDesc.strModelTag = "Glove_Plate_1";
        itemDesc.iValue = 3;

        m_ItemDescs[16] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "지옥 인도자의 장화";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_set_PL_M_BT_00019";
        itemDesc.strModelTag = "Boots_Plate_1";
        itemDesc.iValue = 5;

        m_ItemDescs[17] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};

        m_ItemDescs[18] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "방어구 제작석";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Enchant_Armor_KAA_001";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[19] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "마력석 조각";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_AmmoMaterial_4";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[20] = itemDesc;
    }
    
    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "온전한 마력석";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Core_002";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[21] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "마력 가루";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_material_blueprint_sub_037";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[22] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "희귀한 마린드";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_SkillGrowth_Polymorph_Sapphire_004";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[23] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Helmet_Plate_3";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_HELMET;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_PL_M_HM_00016";
        itemDesc.strModelTag = "Helmet_Plate_3";

        m_ItemDescs[24] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Upper_Plate_3";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_UPPER;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_PL_M_TS_00016";
        itemDesc.strModelTag = "Upper_Plate_3";

        m_ItemDescs[25] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Lower_Plate_3";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_PL_M_PT_00016";
        itemDesc.strModelTag = "Lower_Plate_3";

        m_ItemDescs[26] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};

        m_ItemDescs[27] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Glove_Plate_3";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_PL_M_GL_00018";
        itemDesc.strModelTag = "Glove_Plate_3";

        m_ItemDescs[28] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Boots_Plate_3";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_PL_M_BT_00016";
        itemDesc.strModelTag = "Boots_Plate_3";

        m_ItemDescs[29] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};

        m_ItemDescs[30] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Craft Item0";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Enchant_Armor_KAA_001";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[31] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Craft Item1";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_AmmoMaterial_4";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[32] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Craft Item2";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Basilisk_001";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[33] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Craft Item3";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Claw_001";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[34] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Craft Item4";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Core_002";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[35] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Helmet_Fabric_1";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_HELMET;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_FA_M_HM_00014";
        itemDesc.strModelTag = "Helmet_Fabric_1";

        m_ItemDescs[36] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc2{};
        itemDesc2.strItemTag = "Upper_Fabric_1";
        itemDesc2.eItemType = ITEM_EQUIP;
        itemDesc2.eItemEquipType = ITEM_EQUIP_UPPER;
        itemDesc2.numItem = 1;
        itemDesc2.strTexTag = "P_Set_FA_M_TS_00014";
        itemDesc2.strModelTag = "Upper_Fabric_1";

        m_ItemDescs[37] = itemDesc2;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Lower_Fabric_1";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_FA_M_PT_00014";
        itemDesc.strModelTag = "Lower_Fabric_1";

        m_ItemDescs[38] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};

        m_ItemDescs[39] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Glove_Fabric_1";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_FA_M_GL_00014";
        itemDesc.strModelTag = "Glove_Fabric_1";

        m_ItemDescs[40] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Boots_Fabric_1";
        itemDesc.eItemType = ITEM_EQUIP;
        itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
        itemDesc.numItem = 1;
        itemDesc.strTexTag = "P_Set_FA_M_BT_00014";
        itemDesc.strModelTag = "Boots_Fabric_1";

        m_ItemDescs[41] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};

        m_ItemDescs[42] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Craft Item0";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Enchant_Armor_KAA_001";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[43] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Craft Item1";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_AmmoMaterial_4";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[44] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Craft Item2";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Basilisk_001";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[45] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Craft Item3";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Claw_001";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[46] = itemDesc;
    }

    {
        CItemBase::ItemDesc itemDesc{};
        itemDesc.strItemTag = "Craft Item4";
        itemDesc.eItemType = ITEM_MISC;
        itemDesc.strTexTag = "I_Core_002";
        itemDesc.strModelTag = "";
        itemDesc.numItem = 1;

        m_ItemDescs[47] = itemDesc;
    }

    return S_OK;
}

HRESULT CUICraftSlot::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

shared_ptr<CUICraftSlot> CUICraftSlot::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUICraftSlot> pInstance = make_shared<CUICraftSlot>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUICraftSlot::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
