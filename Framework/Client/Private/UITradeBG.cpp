#include "UITradeBG.h"
#include "UITradeSlot.h"
#include "UIMgr.h"
#include "ItemMgr.h"
#include "ItemEquip.h"
#include "ItemUsable.h"
#include "ItemMisc.h"
#include "UIItemToolTip.h"
#include "UIPanel.h"
#include "GameMgr.h"

#include "VIInstanceUI.h"

CUITradeBG::CUITradeBG()
{

}

CUITradeBG::~CUITradeBG()
{

}

HRESULT CUITradeBG::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

void CUITradeBG::PriorityTick(_float _fTimeDelta)
{

}

void CUITradeBG::Tick(_float _fTimeDelta)
{
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

void CUITradeBG::LateTick(_float _fTimeDelta)
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

    m_fScalingTime += _fTimeDelta;

    UpdateTexture();
}

HRESULT CUITradeBG::Render()
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

    if (FAILED(RenderFont()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUITradeBG::RenderFont()
{
    if (m_UIDesc.strTag == "TradeABottomBGs")
    {
        wstring strTab = L"잡화상점";
        _float iStrOffset = static_cast<_float>(strTab.size()) * 10.f;

        if (FAILED(GAMEINSTANCE->RenderFont("Font_DNFB16", strTab, _float2((m_vUIPos[3].x - iStrOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[3].y - 14.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.65f, 0.52f, 0.42f, 1.f))))
            return E_FAIL;
    }

    if (m_UIDesc.strTag == "TradeDDecoBGs")
    {
        shared_ptr<CUITradeSlot> pUI = dynamic_pointer_cast<CUITradeSlot>(UIMGR->FindUI("TradeCSlots"));
        if (pUI == nullptr)
            return E_FAIL;

        _int iCurSelectedIdx = pUI->GetCurSelectedIdx();
        if (iCurSelectedIdx != -1)
        {
            CItemBase::ItemDesc itemDesc = pUI->GetItemDesc(iCurSelectedIdx);

            wstring strAmount = to_wstring(itemDesc.numItem);

            _uint iAmountOffset = static_cast<_uint>(strAmount.size() * 4.f);

            if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", strAmount, _float2((m_vUIPos[6].x - iAmountOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[6].y - 12.5f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(255.f / 255.f, 247.f / 255.f, 153.f / 255.f, 1.f))))
                return E_FAIL;
        }
        vector<CItemBase::ItemDesc> vecItemDescs = pUI->GetBuySlotDescs();
        if (!vecItemDescs.empty())
        {
            _uint iAllBuyAmounts{ 0 };
            for (auto& pItemDesc : vecItemDescs)
                iAllBuyAmounts += pItemDesc.numItem * pItemDesc.iValue;

            wstring strBuyAmount;
            strBuyAmount = to_wstring(iAllBuyAmounts);

            auto rit = strBuyAmount.rbegin();
            wchar_t ch = L',';

            _uint iInsertNum{ 0 };
            for (rit; rit != strBuyAmount.rend(); ++rit)
            {
                if (iInsertNum % 3 == 0 && iInsertNum != 0)
                {
                    auto it = rit.base();
                    strBuyAmount.insert(it, ch);
                }
                iInsertNum++;
            }

            _float iBuyAmountOffset = static_cast<_float>(strBuyAmount.size()) * 3.f;

            if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", strBuyAmount, _float2((m_vUIPos[19].x - iBuyAmountOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[19].y - 7.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.65f, 0.52f, 0.42f, 1.f))))
                return E_FAIL;
        }

        if (iCurSelectedIdx == -1 && vecItemDescs.empty())
        {
            wstring strBuyAmount;
            strBuyAmount = L"0";

            _float iBuyAmountOffset = static_cast<_float>(strBuyAmount.size()) * 3.f;

            if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", strBuyAmount, _float2((m_vUIPos[19].x - iBuyAmountOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[19].y - 7.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.65f, 0.52f, 0.42f, 1.f))))
                return E_FAIL;
        }

        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", L"판매 물품 목록", _float2((m_vUIPos[1].x + 18.5f) + (g_iWinSizeX * 0.5f), (-m_vUIPos[1].y - 12.5f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.65f, 0.52f, 0.42f, 1.f))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", L"구매 목록", _float2((m_vUIPos[2].x + 18.5f) + (g_iWinSizeX * 0.5f), (-m_vUIPos[2].y - 12.5f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.65f, 0.52f, 0.42f, 1.f))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL12", L"총 구매액", _float2((m_vUIPos[4].x + 18.5f) + (g_iWinSizeX * 0.5f), (-m_vUIPos[4].y - 12.5f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.65f, 0.52f, 0.42f, 1.f))))
            return E_FAIL;

        /*if (FAILED(GAMEINSTANCE->RenderFont("Font_DNFB14", L"구매", _float2((m_vUIPos[7].x - 20.f) + (g_iWinSizeX * 0.5f), (-m_vUIPos[7].y - 12.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.65f, 0.52f, 0.42f, 1.f))))
            return E_FAIL;*/

        if (FAILED(GAMEINSTANCE->RenderFont("Font_DNFB14", L"구매", _float2((m_vUIPos[7].x - 36.f) + (g_iWinSizeX * 0.5f), (-m_vUIPos[7].y - 22.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_CENTER, _float4(0.65f, 0.52f, 0.42f, 1.f))))
            return E_FAIL;

        wstring strAdenAmount = to_wstring(ITEMMGR->GetMoney());
        _float iAmountOffset = static_cast<_float>(strAdenAmount.size()) * 3.f;
        auto rit = strAdenAmount.rbegin();
        wchar_t ch = L',';

        _uint iInsertNum{ 0 };
        for (rit; rit != strAdenAmount.rend(); ++rit)
        {
            if (iInsertNum % 3 == 0 && iInsertNum != 0)
            {
                auto it = rit.base();
                strAdenAmount.insert(it, ch);
            }
            iInsertNum++;
        }

        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", strAdenAmount, _float2((m_vUIPos[18].x - iAmountOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[18].y - 7.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(0.65f, 0.52f, 0.42f, 1.f))))
            return E_FAIL;
    }

	return S_OK;
}

void CUITradeBG::SetTradeBGScaling(_uint _slotIndex)
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    if (m_UIDesc.strTag == "TradeBSlotBGs")
    {
        vertices[_slotIndex].vRight = _float4(vertices[_slotIndex].vRight.x * 0.9f, 0.f, 0.f, 0.f);
        vertices[_slotIndex].vUp = _float4(0.f, vertices[_slotIndex].vUp.y * 0.9f, 0.f, 0.f);

        m_fScalingTime = 0.f;
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUITradeBG::TradeKeyInput()
{
    if (m_UIDesc.strTag == "TradeBSlotBGs")
    {
        _int iNumMouse = IsInstanceMouseOn();

        if (iNumMouse != -1)
        {
            shared_ptr<CUIItemToolTip> pUI = dynamic_pointer_cast<CUIItemToolTip>(UIMGR->FindUI("ToolTipInstance_0"));
            shared_ptr<CUITradeSlot> pUISlot = dynamic_pointer_cast<CUITradeSlot>(UIMGR->FindUI("TradeCSlots"));

            if (pUI != nullptr)
            {
                if (iNumMouse < 12)
                {
                    CItemBase::ItemDesc itemDesc = pUISlot->GetItemDesc(iNumMouse);
                    if (itemDesc.strItemTag == "")
                    {
                        CItemBase::ItemDesc itemDesc;
                        itemDesc.strItemTag = "";
                        itemDesc.eItemType = ITEM_END;
                        itemDesc.eItemUsableType = ITEM_USABLE_END;
                        itemDesc.strTexTag = "";
                        itemDesc.iValue = 0;

                        pUI->SetDisplayDesc(itemDesc);
                        UIMGR->DeactivateTradeItemToolTip();
                        if (!m_bIsSoundOnce)
                        {
                            GAMEINSTANCE->PlaySoundW("UI_MouseOver_01_B", 2.f);
                            m_bIsSoundOnce = true;
                        }
                    }

                    else
                    {
                        weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("ToolTipABGs_0"));
                        if (!pUIPanel.expired())
                            pUIPanel.lock()->SetUIPos(m_vUIPos[iNumMouse].x - 60.f, m_vUIPos[iNumMouse].y - 130.f);
                        pUI->SetDisplayDesc(itemDesc);
                        UIMGR->ActivateTradeItemToolTip();
                        if (!m_bIsSoundOnce)
                        {
                            GAMEINSTANCE->PlaySoundW("UI_MouseOver_01_B", 2.f);
                            m_bIsSoundOnce = true;
                        }
                    }
                }
                else
                {
                    shared_ptr<CUIItemToolTip> pUI = dynamic_pointer_cast<CUIItemToolTip>(UIMGR->FindUI("ToolTipInstance_0"));
                    if (pUI != nullptr)
                    {
                        CItemBase::ItemDesc itemDesc;
                        itemDesc.strItemTag = "";
                        itemDesc.eItemType = ITEM_END;
                        itemDesc.eItemUsableType = ITEM_USABLE_END;
                        itemDesc.strTexTag = "";
                        itemDesc.iValue = 0;

                        pUI->SetDisplayDesc(itemDesc);
                        UIMGR->DeactivateTradeItemToolTip();
                        if (!m_bIsSoundOnce)
                        {
                            GAMEINSTANCE->PlaySoundW("UI_MouseOver_01_B", 2.f);
                            m_bIsSoundOnce = true;
                        }
                    }
                }
            }
        }
        else
        {
            shared_ptr<CUIItemToolTip> pUI = dynamic_pointer_cast<CUIItemToolTip>(UIMGR->FindUI("ToolTipInstance_0"));
            if (pUI != nullptr)
            {
                CItemBase::ItemDesc itemDesc;
                itemDesc.strItemTag = "";
                itemDesc.eItemType = ITEM_END;
                itemDesc.eItemUsableType = ITEM_USABLE_END;
                itemDesc.strTexTag = "";
                itemDesc.iValue = 0;

                pUI->SetDisplayDesc(itemDesc);
                UIMGR->DeactivateTradeItemToolTip();
                m_bIsSoundOnce = false;
            }
        }

        if (GAMEINSTANCE->MouseDown(DIM_LB))
        {
            if (iNumMouse >= 0 && iNumMouse < 12)
            {
                shared_ptr<CUITradeSlot> pUI = dynamic_pointer_cast<CUITradeSlot>(UIMGR->FindUI("TradeCSlots"));
                if (pUI == nullptr)
                    return;

                pUI->AddItemDescToSlot(pUI->GetItemDesc(iNumMouse));
                GAMEINSTANCE->PlaySoundW("UI_Slide_01_K", 0.7f);
                SetTradeBGScaling(iNumMouse);
                UIMGR->ActivateTradeSlotBGEffect({ m_vUIPos[iNumMouse].x, m_vUIPos[iNumMouse].y });
            }
        }
    }

    else if (m_UIDesc.strTag == "TradeDDecoBGs")
    {
        _int iNumMouse = IsInstanceMouseOn();

        if (GAMEINSTANCE->MouseDown(DIM_LB) && iNumMouse == 20)
        {
            UIMGR->DeactivateUI("TradeAllPanel");
            UIMGR->ActivateHUD();
            CGameMgr::GetInstance()->ChangeToNormalMouse();
        }

        if (GAMEINSTANCE->MouseDown(DIM_LB) && iNumMouse != -1 && iNumMouse == 7)
        {
            shared_ptr<CUITradeSlot> pUI = dynamic_pointer_cast<CUITradeSlot>(UIMGR->FindUI("TradeCSlots"));
            if (pUI == nullptr)
                return;

            /*_int curIdx = pUI->GetCurSelectedIdx();
            if (curIdx != -1)
            {
                CItemBase::ItemDesc itemDesc = pUI->GetItemDesc(curIdx);

                _uint iAllAmount = itemDesc.numItem * itemDesc.iValue;
                if (ITEMMGR->DecreaseMoney(iAllAmount))
                {
                    shared_ptr<CItemBase> pItem{ nullptr };
                    switch (itemDesc.eItemType)
                    {
                    case ITEM_EQUIP:
                    {
                        pItem = CItemEquip::Create(itemDesc);
                        if (pItem == nullptr)
                            return;
                    }
                    break;
                    case ITEM_USABLE:
                    {
                        pItem = CItemUsable::Create(itemDesc);
                    }
                    break;
                    case ITEM_MISC:
                    {
                        pItem = CItemMisc::Create(itemDesc);
                    }
                    break;
                    }

                    ITEMMGR->AddItemToInventory(pItem);
                }
            }*/

            m_iBuyAllAmounts = 0;
            vector<CItemBase::ItemDesc> vecBuySlotDescs = pUI->GetBuySlotDescs();
            if (!vecBuySlotDescs.empty())
            {
                for (auto& pItemDesc : vecBuySlotDescs)
                {
                    if (pItemDesc.strItemTag != "")
                        m_iBuyAllAmounts += pItemDesc.numItem * pItemDesc.iValue;
                }

                if (ITEMMGR->DecreaseMoney(m_iBuyAllAmounts))
                {
                    for (auto& pItemDesc : vecBuySlotDescs)
                    {
                        shared_ptr<CItemBase> pItem{ nullptr };
                        switch (pItemDesc.eItemType)
                        {
                        case ITEM_EQUIP:
                        {
                            pItem = CItemEquip::Create(pItemDesc);
                            if (pItem == nullptr)
                                continue;
                        }
                        break;
                        case ITEM_USABLE:
                        {
                            pItem = CItemUsable::Create(pItemDesc);
                            if (pItem == nullptr)
                                continue;
                        }
                        break;
                        case ITEM_MISC:
                        {
                            pItem = CItemMisc::Create(pItemDesc);
                            if (pItem == nullptr)
                                continue;
                        }
                        break;
                        }

                        ITEMMGR->AddItemToInventory(pItem);
                    }

                    GAMEINSTANCE->PlaySoundW("UI_Text_Exchange_01_A", 0.6f);

                    pUI->ClearBuySlots();
                }
            }
        }

        if (GAMEINSTANCE->MouseDown(DIM_LB) && iNumMouse != -1 && iNumMouse == 3)
        {
            shared_ptr<CUITradeSlot> pUI = dynamic_pointer_cast<CUITradeSlot>(UIMGR->FindUI("TradeCSlots"));
            if (pUI == nullptr)
                return;

            pUI->ClearBuySlots();
        }
    }
}

void CUITradeBG::InitUpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    if (m_UIDesc.strTag == "TradeABottomBGs")
    {
        for (_uint i = 0; i < iNumSlots; ++i)
        {
            vertices[i].iTexIndex = i;
            vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
            vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            vertices[i].iSlotUV = 0;
        }
    }

    else if (m_UIDesc.strTag == "TradeBSlotBGs")
    {
        for (_uint i = 0; i < iNumSlots; ++i)
        {
            if (i < 12)
                vertices[i].iTexIndex = 0;
            else
                vertices[i].iTexIndex = 1;

            vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
            vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            vertices[i].iSlotUV = 0;
        }
    }

    else if (m_UIDesc.strTag == "TradeDDecoBGs")
    {
        for (_uint i = 0; i < iNumSlots; ++i)
        {
            if (i == 0)
                vertices[i].iTexIndex = 0;

            if (i > 0 && i <= 2)
                vertices[i].iTexIndex = 1;

            if (i == 4)
                vertices[i].iTexIndex = 1;

            if (i == 3)
                vertices[i].iTexIndex = 2;

            if (i == 5)
                vertices[i].iTexIndex = 4;

            if (i == 6)
                vertices[i].iTexIndex = 5;

            if (i == 7)
                vertices[i].iTexIndex = 6;

            if (i > 7 && i <= 17)
            {
                vertices[i].iTexIndex = 7;
                vertices[i].vColor = _float4(1.f, 1.f, 1.f, 0.5f);
            }

            if (i > 17 && i <= 19)
                vertices[i].iTexIndex = 8;

            if (i == 20)
                vertices[i].iTexIndex = 9;

            if (i == 21)
                vertices[i].iTexIndex = 11;

            vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
            vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            vertices[i].iSlotUV = 0;
        }
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUITradeBG::UpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    _int iNumMouse = IsInstanceMouseOn();

    if (m_UIDesc.strTag == "TradeBSlotBGs")
    {
        for (_uint i = 0; i < iNumSlots; ++i)
        {
            if (i <= 3)
                vertices[i].iTexIndex = 0;
            else if (i > 3 && i <= 11)
                vertices[i].iTexIndex = 1;
            else
                vertices[i].iTexIndex = 2;

            vertices[i].vRight = SimpleMath::Vector4::Lerp(vertices[i].vRight, _float4(m_vUISize[i].x, 0.f, 0.f, 0.f), m_fScalingTime);
            vertices[i].vUp = SimpleMath::Vector4::Lerp(vertices[i].vUp, _float4(0.f, m_vUISize[i].y, 0.f, 0.f), m_fScalingTime);

            if (vertices[i].vRight.x >= m_vUISize[i].x && vertices[i].vUp.y >= m_vUISize[i].y)
            {
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }
        }
    }

    else if (m_UIDesc.strTag == "TradeDDecoBGs")
    {
        for (_uint i = 0; i < iNumSlots; ++i)
        {
            if (i == iNumMouse)
            {
                if (i == 20)
                    vertices[i].iTexIndex = 10;
                else if (i == 3)
                    vertices[i].iTexIndex = 3;
            }

            else
            {
                if (i == 20)
                    vertices[i].iTexIndex = 9;

                else if (i == 3)
                    vertices[i].iTexIndex = 2;
            }
        }
    }
    
    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

HRESULT CUITradeBG::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

shared_ptr<CUITradeBG> CUITradeBG::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUITradeBG> pInstance = make_shared<CUITradeBG>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUITradeBG::Create", MB_OK);

        pInstance.reset(); 

        return nullptr;
    }

    return pInstance;
}
