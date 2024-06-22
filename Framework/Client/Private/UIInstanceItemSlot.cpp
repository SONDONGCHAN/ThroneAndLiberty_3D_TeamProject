#include "UIInstanceItemSlot.h"
#include "UIInstanceItemIcon.h"
#include "UIInstanceItemHover.h"
#include "UIScroll.h"
#include "UIMgr.h"
#include "ItemMgr.h"
#include "ItemBase.h"
#include "UISlot.h"
#include "UIItemToolTip.h"
#include "UIPanel.h"

#include "VIInstanceUI.h"

CUIInstanceItemSlot::CUIInstanceItemSlot()
{

}

CUIInstanceItemSlot::~CUIInstanceItemSlot()
{

}

HRESULT CUIInstanceItemSlot::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

    m_pItems.resize(m_vUIPos.size());

    return S_OK;
}

void CUIInstanceItemSlot::PriorityTick(_float _fTimeDelta)
{

}

void CUIInstanceItemSlot::Tick(_float _fTimeDelta)
{
    KeyInput();

    switch(m_eUIItemType)
    {
    case UI_ITEM_EQUIP:
    {
        if (UIMGR->GetTabIndex() == 0 && m_UIDesc.strTag == "InvEquipSlot0")
        {
            SlotKeyInput();
        }
    }
    break;
    case UI_ITEM_USABLE:
    {
        if (UIMGR->GetTabIndex() == 1 && m_UIDesc.strTag == "UsableSlot0")
        {
            SlotKeyInput();
        }
    }
    break;
    case UI_ITEM_MISC:
    {
        if (UIMGR->GetTabIndex() == 2 && m_UIDesc.strTag == "MiscSlot0")
        {
            SlotKeyInput();
        }
    }
    break;
    }

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

void CUIInstanceItemSlot::LateTick(_float _fTimeDelta)
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
            {
                if (childPair.second.lock()->GetUIType() == UI_INSTANCEITEMICON)
                {
                    if (childPair.second.lock()->GetUITag() == "EquipSlotIcon0")
                        dynamic_pointer_cast<CUIInstanceItemIcon>(childPair.second.lock())->IsRender(m_fVisibleTop, m_fVisibleBottom);

                    childPair.second.lock()->LateTick(_fTimeDelta);
                }

                else if (childPair.second.lock()->GetUIType() == UI_INSTANCEITEMHOVER)
                {
                    //dynamic_pointer_cast<CUIInstanceItemHover>(childPair.second.lock())->IsRender(m_fVisibleTop, m_fVisibleBottom);

                    childPair.second.lock()->LateTick(_fTimeDelta);
                }

                else
                    childPair.second.lock()->LateTick(_fTimeDelta);
            }
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

    if (m_UIDesc.strTag != "InvEquipSlot0")
        IsRender();
}

HRESULT CUIInstanceItemSlot::Render()
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
        /*if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture0", m_strTexKeys[m_iTexIndex])))
            return E_FAIL;*/

        _uint iNumItems = static_cast<_uint>(m_pItems.size());
        for (_uint i = 0; i < iNumItems; ++i)
        {
            if (m_pItems[i] == nullptr)
                continue;

            string shaderKey = "g_DiffuseTexture" + to_string(i);

            if (FAILED(GAMEINSTANCE->BindSRV(shaderKey.c_str(), m_pItems[i]->GetItemTexTag())))
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

    if (UIMGR->GetTabIndex() == 0 && m_UIDesc.strTag == "InvEquipSlot0")
    {
        RenderFont();
    }

    if (UIMGR->GetTabIndex() == 1 && m_UIDesc.strTag == "UsableSlot0")
    {
        RenderFont();
    }

    if (UIMGR->GetTabIndex() == 2 && m_UIDesc.strTag == "MiscSlot0")
    {
        RenderFont();
    }

    return S_OK;
}

HRESULT CUIInstanceItemSlot::RenderFont()
{
    _uint iNumSlots = static_cast<_uint>(m_pItems.size());
    for (_uint i = 0; i < iNumSlots; ++i)
    {
        if (m_pItems[i])
        {
            if (m_pItems[i]->GetNumItem() == 1)
                continue;

            wstring strNumItem = to_wstring(m_pItems[i]->GetNumItem());
            _uint istrOffset = static_cast<_uint>(strNumItem.size()) * 8.f;

            if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", strNumItem, _float2((m_vUIPos[i].x + 15.f - istrOffset) + (g_iWinSizeX * 0.5f), (-m_vUIPos[i].y + 2.f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 1.f)))
                return E_FAIL;
        }
    }

    return S_OK;
}

void CUIInstanceItemSlot::IsRender(_float _fVisibleYTop, _float _fVisibleYBottom)
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

    // 얘를 어케 처리해야 할까...
    _uint numUIPos = static_cast<_uint>(m_vUIPos.size());
    for (_uint i = 0; i < numUIPos; ++i)
    {
        if (m_pItems[i] != nullptr)
        {
            vertices[i].iTexIndex = i;
        }

        if ((m_vUIPos[i].y - (m_fSizeY * 0.5f)) > _fVisibleYBottom && (m_vUIPos[i].y + (m_fSizeY * 0.5f)) < _fVisibleYTop)
        {
            vertices[i].iSlotUV = 2;

            /* 비어 있으면 안보이게 처리 */
            if (m_pItems[i] == nullptr)
                vertices[i].iSlotUV = 3;

            continue;
        }

        if ((m_vUIPos[i].y + (m_fSizeY * 0.5f)) > _fVisibleYTop && (m_vUIPos[i].y - (m_fSizeY * 0.5f)) < _fVisibleYTop)
        {
            m_fYRatio = 1.f - ((_fVisibleYTop - (m_vUIPos[i].y - (m_fSizeY * 0.5f))) / m_fSizeY);

            vertices[i].iSlotUV = 1;
            m_vSlotUVRatio[i].y = m_fYRatio;
            vertices[i].vUVRatio = _float2(m_vSlotUVRatio[i].x, m_vSlotUVRatio[i].y);

            /* 비어 있으면 안보이게 처리 */
            if (m_pItems[i] == nullptr)
                vertices[i].iSlotUV = 3;

            continue;
        }

        if ((m_vUIPos[i].y - (m_fSizeY * 0.5f)) < _fVisibleYBottom && (m_vUIPos[i].y + (m_fSizeY * 0.5f)) > _fVisibleYBottom)
        {
            m_fYRatio = ((m_vUIPos[i].y + (m_fSizeY * 0.5f)) - _fVisibleYBottom) / m_fSizeY;

            vertices[i].iSlotUV = 0;
            m_vSlotUVRatio[i].y = m_fYRatio;
            vertices[i].vUVRatio = _float2(m_vSlotUVRatio[i].x, m_vSlotUVRatio[i].y);

            /* 비어 있으면 안보이게 처리 */
            if (m_pItems[i] == nullptr)
                vertices[i].iSlotUV = 3;

            continue;
        }

        else
        {
            vertices[i].iSlotUV = 3;

            /* 비어 있으면 안보이게 처리 */
            if (m_pItems[i] == nullptr)
                vertices[i].iSlotUV = 3;

            continue;
        }
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

_bool CUIInstanceItemSlot::IsEmpty(_uint _iItemIndex)
{
    if (_iItemIndex >= static_cast<_uint>(m_pItems.size()))
        return false;

    if (m_pItems[_iItemIndex] == nullptr)
        return true;

    return false;
}

void CUIInstanceItemSlot::AddItemToSlot(shared_ptr<CItemBase> _pItem)
{
    if (_pItem == nullptr)
        return;

    /* If Itemtype is Usable or Misc, it should be stacked */
    /* Need to Modify Code */
    if (_pItem->GetItemType() == ITEM_EQUIP)
    {
        _uint iNumItems = static_cast<_uint>(m_pItems.size());
        for (_uint i = 0; i < iNumItems; ++i)
        {
            if (m_pItems[i] == nullptr)
            {
                m_pItems[i] = _pItem;

                break;
            }
        }
    }

    else
    {
        _uint iNumItems = static_cast<_uint>(m_pItems.size());
        for (_uint i = 0; i < iNumItems; ++i)
        {
            if (m_pItems[i] != nullptr)
            {
                if (m_pItems[i]->GetItemTag() == _pItem->GetItemTag())
                {
                    _int numGap = m_pItems[i]->IncreaseNumItem(_pItem->GetNumItem());
                    if (numGap == 0)
                    {
                        _pItem.reset();
                        _pItem = nullptr;

                        break;
                    }

                    else
                    {
                        _pItem->SetNumItem(numGap);

                        continue;
                    }
                }
            }

            else if (m_pItems[i] == nullptr)
            {
                m_pItems[i] = _pItem;

                break;
            }
        }
    }
}

void CUIInstanceItemSlot::AddItemToSlotByIndex(_uint _iSlotIndex, shared_ptr<CItemBase> _pItem)
{
    if (_iSlotIndex >= static_cast<_uint>(m_pItems.size()))
        return;

    if (_pItem == nullptr)
        return;

    m_pItems[_iSlotIndex] = _pItem;
}

void CUIInstanceItemSlot::RemoveItemFromSlot(_uint _iItemIndex)
{
    _uint iNumItems = static_cast<_uint>(m_pItems.size());
    if (_iItemIndex >= iNumItems)
        return;

    m_pItems[_iItemIndex].reset();
    m_pItems[_iItemIndex] = nullptr;
}

void CUIInstanceItemSlot::IsRender()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    // 얘를 어케 처리해야 할까...
    _uint numUIPos = static_cast<_uint>(m_vUIPos.size());
    for (_uint i = 0; i < numUIPos; ++i)
    {
        if (m_pItems[i] != nullptr)
            vertices[i].iTexIndex = i;

        vertices[i].iSlotUV = 2;

        if (m_pItems[i] == nullptr)
            vertices[i].iSlotUV = 3;
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUIInstanceItemSlot::SlotKeyInput()
{
    _int iSlotNum = IsInstanceMouseOn();

    if (iSlotNum != -1)
    {
        shared_ptr<CUIItemToolTip> pUI = dynamic_pointer_cast<CUIItemToolTip>(UIMGR->FindUI("ToolTipInstance"));
        if (pUI != nullptr && (m_pItems[iSlotNum] != nullptr))
        {
            weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("ToolTipABGs"));
            if (!pUIPanel.expired())
                pUIPanel.lock()->SetUIPos(m_vUIPos[iSlotNum].x - 80.f, m_vUIPos[iSlotNum].y - 130.f);
            pUI->SetDisplayItem(m_pItems[iSlotNum]);
            UIMGR->ActivateItemToolTip();
            // 한번만 재생
            if (!m_bIsSoundOnce)
            {
                GAMEINSTANCE->PlaySoundW("UI_MouseOver_01_B", 1.f);
                m_bIsSoundOnce = true;
            }
        }

        if (pUI != nullptr && m_pItems[iSlotNum] == nullptr)
        {
            if (!m_bIsSoundOnce)
            {
                GAMEINSTANCE->PlaySoundW("UI_MouseOver_01_B", 1.f);
                m_bIsSoundOnce = true;
            }
        }
    }
    else
    {
        shared_ptr<CUIItemToolTip> pUI = dynamic_pointer_cast<CUIItemToolTip>(UIMGR->FindUI("ToolTipInstance"));
        if (pUI != nullptr)
        {
            pUI->SetDisplayItem(nullptr);
            UIMGR->DeactivateItemToolTip();
            m_bIsSoundOnce = false;
        }
    }

    if (GAMEINSTANCE->MousePressing(DIM_LB) && (iSlotNum != -1))
    {
        shared_ptr<CUISlot> mouseSlot = dynamic_pointer_cast<CUISlot>(UIMGR->FindUI("MouseSlot"));
        if (mouseSlot == nullptr)
            return;

        if (mouseSlot->IsEmpty() && !IsEmpty(iSlotNum))
        {
            mouseSlot->AddItemToSlot(m_pItems[iSlotNum]);
            mouseSlot->SetPrevSlotIndex(iSlotNum);
            mouseSlot->SetPrevSlotType(m_UIDesc.eUISlotType);
            mouseSlot->SetIsEmpty(false);
        }
    }

    if (GAMEINSTANCE->MouseUp(DIM_LB) && (iSlotNum != -1))
    {
        shared_ptr<CUISlot> mouseSlot = dynamic_pointer_cast<CUISlot>(UIMGR->FindUI("MouseSlot"));
        if (mouseSlot == nullptr)
            return;

        if (!mouseSlot->IsEmpty() && m_UIDesc.eUISlotType == SLOT_INVEN)
        {
            GAMEINSTANCE->PlaySoundW("UI_MouseAfterDrop_01_B", 1.f);

            if (IsEmpty(iSlotNum)) // Slot that about to drop Item is Empty
            {
                _uint iPrevSlotIndex = mouseSlot->GetPrevSlotIndex();
                UI_SLOT_TYPE ePrevSlotType = mouseSlot->GetPrevSlotType();
                shared_ptr<CItemBase> pItem = mouseSlot->GetSlotItem();

                vector<shared_ptr<CItemBase>>* pPrevSlots{ nullptr };
                if (ePrevSlotType == SLOT_INVEN)
                {
                    pPrevSlots = ITEMMGR->GetItemSlots(pItem->GetItemType());
                    AddItemToSlotByIndex(iSlotNum, pItem);
                }

                else if (ePrevSlotType == SLOT_QUICK)
                {
                    /* 추후 수정 */
                    pPrevSlots = ITEMMGR->GetItemQuickSlots();
                    AddItemToSlotByIndex(iSlotNum, pItem);
                }

                mouseSlot->RemoveItemFromSlot();
                mouseSlot->SetIsEmpty(true);
                //pPrevSlots = ITEMMGR->GetItemSlots(pItem->GetItemType());
                (*pPrevSlots)[iPrevSlotIndex].reset();
                (*pPrevSlots)[iPrevSlotIndex] = nullptr;
            }

            else // Slot that about to drop Item is Not Empty, Swap Item
            {
                _uint iPrevSlotIndex = mouseSlot->GetPrevSlotIndex();
                UI_SLOT_TYPE ePrevSlotType = mouseSlot->GetPrevSlotType();
                shared_ptr<CItemBase> pInItem = mouseSlot->GetSlotItem();
                shared_ptr<CItemBase> pOutItem = m_pItems[iSlotNum];

                vector<shared_ptr<CItemBase>>* pPrevSlots;
                if (ePrevSlotType == SLOT_INVEN)
                {
                    pPrevSlots = ITEMMGR->GetItemSlots(pInItem->GetItemType());
                    AddItemToSlotByIndex(iSlotNum, pInItem);
                    mouseSlot->RemoveItemFromSlot();
                    mouseSlot->SetIsEmpty(true);
                    ITEMMGR->AddItemByIndex(iPrevSlotIndex, pOutItem);
                }

                else if (ePrevSlotType == SLOT_QUICK)
                {
                    /* 추후 추가 */
                    pPrevSlots = ITEMMGR->GetItemQuickSlots();
                    mouseSlot->RemoveItemFromSlot();
                    mouseSlot->SetIsEmpty(true);
                    (*pPrevSlots)[iPrevSlotIndex].reset();
                    (*pPrevSlots)[iPrevSlotIndex] = nullptr;
                }
            }
        }
    }

    if (GAMEINSTANCE->MouseDown(DIM_RB) && (iSlotNum != -1))
    {
        if (!IsEmpty(iSlotNum) && m_pItems[iSlotNum])
        {
            if (m_pItems[iSlotNum]->GetItemType() == ITEM_EQUIP)
            {
                if (!m_pItems[iSlotNum]->IsEquipped())
                {
                    ITEMMGR->EquipItem(m_pItems[iSlotNum]->GetItemEquipType(), m_pItems[iSlotNum]->GetItemModelTag());
                    m_pItems[iSlotNum]->SetIsEquipped(true);
                }

                else
                {
                    if (m_pItems[iSlotNum]->GetItemEquipType() == ITEM_EQUIP_HELMET || m_pItems[iSlotNum]->GetItemEquipType() == ITEM_EQUIP_CAPE)
                    {
                        ITEMMGR->TakeOffItem(m_pItems[iSlotNum]->GetItemEquipType());
                        m_pItems[iSlotNum]->SetIsEquipped(false);
                    }
                }
            }

            else if (m_pItems[iSlotNum]->GetItemType() == ITEM_USABLE)
            {
                switch (m_pItems[iSlotNum]->GetItemUsableType())
                {
                case ITEM_USABLE_HP:
                {
                    if (m_pItems[iSlotNum]->DecreaseNumItem(1) == 1)
                    {
                        ITEMMGR->UseHPPotion(static_cast<_float>(m_pItems[iSlotNum]->GetItemValue()));
                    }
                    else if (m_pItems[iSlotNum]->DecreaseNumItem(1) <= 0)
                    {
                        ITEMMGR->UseHPPotion(static_cast<_float>(m_pItems[iSlotNum]->GetItemValue()));
                        RemoveItemFromSlot(iSlotNum);
                    }
                }
                break;
                case ITEM_USABLE_MP:
                {

                }
                break;
                }
            }
        }
    }
}

HRESULT CUIInstanceItemSlot::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

shared_ptr<CUIInstanceItemSlot> CUIInstanceItemSlot::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUIInstanceItemSlot> pInstance = make_shared<CUIInstanceItemSlot>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIInstanceItemSlot::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
