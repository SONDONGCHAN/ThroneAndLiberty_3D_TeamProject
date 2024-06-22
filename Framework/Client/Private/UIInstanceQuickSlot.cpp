#include "UIInstanceQuickSlot.h"
#include "ItemMgr.h"
#include "UISlot.h"
#include "UIMgr.h"
#include "ItemBase.h"

#include "VIInstanceUI.h"

CUIInstanceQuickSlot::CUIInstanceQuickSlot()
{

}

CUIInstanceQuickSlot::~CUIInstanceQuickSlot()
{

}

HRESULT CUIInstanceQuickSlot::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

    if (m_UIDesc.strTag == "QuickFixedSlot0")
        m_pItems.resize(m_vUIPos.size());

    return S_OK;
}

void CUIInstanceQuickSlot::PriorityTick(_float _fTimeDelta)
{

}

void CUIInstanceQuickSlot::Tick(_float _fTimeDelta)
{
    KeyInput();
    SlotKeyInput();

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

void CUIInstanceQuickSlot::LateTick(_float _fTimeDelta)
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

    m_fTimeAcc += _fTimeDelta;

    UpdateTexture();
}

HRESULT CUIInstanceQuickSlot::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_iInstanceRenderState", &m_iInstanceRenderState, sizeof(_uint))))
        return E_FAIL;

    if (m_UIDesc.strTag == "QuickFixedSlot0")
    {
        _uint iNumItems = static_cast<_uint>(m_pItems.size());
        for (_uint i = 0; i < iNumItems; ++i)
        {
            if (m_pItems[i] != nullptr)
            {
                string shaderKey = "g_DiffuseTexture" + to_string(i);

                if (FAILED(GAMEINSTANCE->BindSRV(shaderKey.c_str(), m_pItems[i]->GetItemTexTag())))
                    return E_FAIL;
            }
            else
            {
                string shaderKey = "g_DiffuseTexture" + to_string(i);

                if (FAILED(GAMEINSTANCE->BindSRV(shaderKey.c_str(), m_strTexKeys[0])))
                    return E_FAIL;
            }
        }
    }

    else
    {
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

HRESULT CUIInstanceQuickSlot::RenderFont()
{
    if (m_UIDesc.strTag == "QuickFixedSlot0")
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
    }

    return S_OK;
}

_bool CUIInstanceQuickSlot::IsEmpty(_uint _iSlotIndex)
{
    if (_iSlotIndex >= static_cast<_uint>(m_pItems.size()))
        return false;

    if (m_pItems[_iSlotIndex] == nullptr)
        return true;

    return false;
}

void CUIInstanceQuickSlot::SetQuickSlotScaling(_uint _slotIndex)
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    if (m_UIDesc.strTag == "QuickMainSlot0")
    {
        vertices[_slotIndex].vRight = _float4(vertices[_slotIndex].vRight.x * 0.8f, 0.f, 0.f, 0.f);
        vertices[_slotIndex].vUp = _float4(0.f, vertices[_slotIndex].vUp.y * 0.8f, 0.f, 0.f);

        m_fTimeAcc = 0.f;
    }

    if (m_UIDesc.strTag == "QuickCombatSlot0")
    {
        vertices[_slotIndex].vRight = _float4(vertices[_slotIndex].vRight.x * 0.8f, 0.f, 0.f, 0.f);
        vertices[_slotIndex].vUp = _float4(0.f, vertices[_slotIndex].vUp.y * 0.8f, 0.f, 0.f);

        m_fTimeAcc = 0.f;
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUIInstanceQuickSlot::AddItemToSlot(shared_ptr<CItemBase> _pItem)
{
    if (_pItem == nullptr)
        return;

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

void CUIInstanceQuickSlot::AddItemToSlotByIndex(_uint _iSlotIndex, shared_ptr<CItemBase> _pItem)
{
    if (_iSlotIndex >= static_cast<_uint>(m_pItems.size()))
        return;

    if (_pItem == nullptr)
        return;

    m_pItems[_iSlotIndex] = _pItem;
}

void CUIInstanceQuickSlot::RemoveItemFromSlot(_uint _iItemIndex)
{
    _uint iNumItems = static_cast<_uint>(m_pItems.size());
    if (_iItemIndex >= iNumItems)
        return;

    m_pItems[_iItemIndex].reset();
    m_pItems[_iItemIndex] = nullptr;
}

void CUIInstanceQuickSlot::UpdateTexture()
{
    m_iCurWeaponIndex = ITEMMGR->GetCurrentWeaponIndex();

    //if (m_iCurWeaponIndex != m_iPrevWeaponIndex)
    {
        D3D11_MAPPED_SUBRESOURCE subResource{};

        ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

        GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

        VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

        if (m_UIDesc.strTag == "QuickMainSlot0")
        {
            for (_uint i = 0; i < 5; ++i)
            {
                switch (m_iCurWeaponIndex)
                {
                case 0:
                {
                    vertices[i].iTexIndex = i + 1;
                }
                break;
                case 1:
                {
                    vertices[i].iTexIndex = i + 6;
                }
                break;
                case 2:
                {
                    vertices[i].iTexIndex = i + 11;
                }
                break;
                }

                vertices[i].vRight = SimpleMath::Vector4::Lerp(vertices[i].vRight, _float4(m_vUISize[i].x, 0.f, 0.f, 0.f), m_fTimeAcc);
                vertices[i].vUp = SimpleMath::Vector4::Lerp(vertices[i].vUp, _float4(0.f, m_vUISize[i].y, 0.f, 0.f), m_fTimeAcc);

                //vertices[i].vRight.x  = CosIp(vertices[i].vRight, m_vUISize[i].x, m_fTimeAcc);
                //vertices[i].vUp.y     = CosIp(m_fQuickSizeY, m_vUISize[i].y, m_fTimeAcc);

                if (vertices[i].vRight.x >= m_vUISize[i].x && vertices[i].vUp.y >= m_vUISize[i].y)
                {
                    vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                    vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
                }
            }
        }

        else if (m_UIDesc.strTag == "QuickFixedSlot0")
        {
            _uint iNumItems = static_cast<_uint>(m_vUIPos.size());
            for (_uint i = 0; i < iNumItems; ++i)
            {
                vertices[i].iTexIndex = i;
            }
        }

        else if (m_UIDesc.strTag == "QuickCombatSlot0")
        {
            switch (m_iCurWeaponIndex)
            {
            case 0:
            {
                vertices[0].iTexIndex = 1;
                vertices[1].iTexIndex = 4;
                vertices[2].iTexIndex = 7;
                vertices[3].iTexIndex = 8;
            }
            break;
            case 1:
            {
                vertices[0].iTexIndex = 2;
                vertices[1].iTexIndex = 5;
                vertices[2].iTexIndex = 7;
                vertices[3].iTexIndex = 8;
            }
            break;
            case 2:
            {
                vertices[0].iTexIndex = 3;
                vertices[1].iTexIndex = 6;
                vertices[2].iTexIndex = 7;
                vertices[3].iTexIndex = 8;
            }
            break;
            }

            for (_uint i = 0; i < 4; ++i)
            {
                vertices[i].vRight = SimpleMath::Vector4::Lerp(vertices[i].vRight, _float4(m_vUISize[i].x, 0.f, 0.f, 0.f), m_fTimeAcc);
                vertices[i].vUp = SimpleMath::Vector4::Lerp(vertices[i].vUp, _float4(0.f, m_vUISize[i].y, 0.f, 0.f), m_fTimeAcc);

                if (vertices[i].vRight.x >= m_vUISize[i].x && vertices[i].vUp.y >= m_vUISize[i].y)
                {
                    vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                    vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
                }
            }
        }

        GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
    }

    m_iPrevWeaponIndex = m_iCurWeaponIndex;
}

void CUIInstanceQuickSlot::SlotKeyInput()
{
    _int iSlotNum = IsInstanceMouseOn();

    if (m_UIDesc.strTag == "QuickFixedSlot0")
    {
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

            if (!mouseSlot->IsEmpty() && IsEmpty(iSlotNum)) // Slot that about to drop Item is Empty
            {
                _uint iPrevSlotIndex = mouseSlot->GetPrevSlotIndex();
                UI_SLOT_TYPE ePrevSlotType = mouseSlot->GetPrevSlotType();
                shared_ptr<CItemBase> pItem = mouseSlot->GetSlotItem();

                vector<shared_ptr<CItemBase>>* pPrevSlots{ nullptr };
                if (ePrevSlotType == SLOT_INVEN)
                    pPrevSlots = ITEMMGR->GetItemSlots(pItem->GetItemType());

                else if (ePrevSlotType == SLOT_QUICK)
                    pPrevSlots = ITEMMGR->GetItemQuickSlots();

                AddItemToSlotByIndex(iSlotNum, pItem);
                mouseSlot->RemoveItemFromSlot();
                mouseSlot->SetIsEmpty(true);
                if (ePrevSlotType == SLOT_QUICK)
                {
                    (*pPrevSlots)[iPrevSlotIndex].reset();
                    (*pPrevSlots)[iPrevSlotIndex] = nullptr;
                }
            }

            else if (!mouseSlot->IsEmpty() && !IsEmpty(iSlotNum)) //Slot that about to drop Item is Not Empty
            {
                _uint iPrevSlotIndex = mouseSlot->GetPrevSlotIndex();
                UI_SLOT_TYPE ePrevSlotType = mouseSlot->GetPrevSlotType();
                shared_ptr<CItemBase> pInItem = mouseSlot->GetSlotItem();
                shared_ptr<CItemBase> pOutItem = m_pItems[iSlotNum];

                vector<shared_ptr<CItemBase>>* pPrevSlots{ nullptr };
                if (ePrevSlotType == SLOT_INVEN)
                    pPrevSlots = ITEMMGR->GetItemSlots(pInItem->GetItemType());
                else if (ePrevSlotType == SLOT_QUICK)
                    pPrevSlots = ITEMMGR->GetItemQuickSlots();

                RemoveItemFromSlot(iSlotNum);
                AddItemToSlotByIndex(iSlotNum, pInItem);
                mouseSlot->RemoveItemFromSlot();
                mouseSlot->SetIsEmpty(true);
                if (ePrevSlotType == SLOT_QUICK)
                {
                    (*pPrevSlots)[iPrevSlotIndex].reset();
                    (*pPrevSlots)[iPrevSlotIndex] = nullptr;
                    (*pPrevSlots)[iPrevSlotIndex] = pOutItem;
                }
            }
        }

        if (GAMEINSTANCE->KeyDown(DIK_F1))
        {
            if (!IsEmpty(0))
            {
                switch (m_pItems[0]->GetItemUsableType())
                {
                case ITEM_USABLE_HP:
                {
                    if (m_pItems[0]->DecreaseNumItem(1) == 1)
                        ITEMMGR->UseHPPotion(static_cast<_float>(m_pItems[0]->GetItemValue()));
                    else if (m_pItems[0]->DecreaseNumItem(1) <= 0)
                    {
                        ITEMMGR->UseHPPotion(static_cast<_float>(m_pItems[0]->GetItemValue()));
                        shared_ptr<CItemBase> pItem = m_pItems[0];
                        vector<shared_ptr<CItemBase>>* pInvenSlots = ITEMMGR->GetItemSlots(m_pItems[0]->GetItemType());
                        auto it = find_if(pInvenSlots->begin(), pInvenSlots->end(), [&pItem](shared_ptr<CItemBase> _pItem) {
                            if (pItem == _pItem)
                                return true;

                            return false;
                            });

                        if (it != pInvenSlots->end())
                        {
                            it->reset();
                            *it = nullptr;
                        }
                        
                        RemoveItemFromSlot(0);
                    }

                    GAMEINSTANCE->PlaySoundW("UI_Potion_SFX_01", 1.f);
                }
                break;
                case ITEM_USABLE_MP:
                {

                }
                break;
                }
            }
        }

        if (GAMEINSTANCE->KeyDown(DIK_F2))
        {
            if (!IsEmpty(1))
            {
                switch (m_pItems[1]->GetItemUsableType())
                {
                case ITEM_USABLE_HP:
                {
                    if (m_pItems[1]->DecreaseNumItem(1) == 1)
                        ITEMMGR->UseHPPotion(static_cast<_float>(m_pItems[1]->GetItemValue()));
                    else if (m_pItems[1]->DecreaseNumItem(1) <= 0)
                    {
                        ITEMMGR->UseHPPotion(static_cast<_float>(m_pItems[1]->GetItemValue()));
                        shared_ptr<CItemBase> pItem = m_pItems[1];
                        vector<shared_ptr<CItemBase>>* pInvenSlots = ITEMMGR->GetItemSlots(m_pItems[1]->GetItemType());
                        auto it = find_if(pInvenSlots->begin(), pInvenSlots->end(), [&pItem](shared_ptr<CItemBase> _pItem) {
                            if (pItem == _pItem)
                                return true;

                            return false;
                            });

                        if (it != pInvenSlots->end())
                        {
                            it->reset();
                            *it = nullptr;
                        }

                        RemoveItemFromSlot(1);
                    }

                    GAMEINSTANCE->PlaySoundW("UI_Potion_SFX_01", 1.f);
                }
                break;
                case ITEM_USABLE_MP:
                {

                }
                break;
                }
            }
        }

        if (GAMEINSTANCE->KeyDown(DIK_F3))
        {
            if (!IsEmpty(2))
            {
                switch (m_pItems[2]->GetItemUsableType())
                {
                case ITEM_USABLE_HP:
                {
                    if (m_pItems[2]->DecreaseNumItem(1) == 1)
                        ITEMMGR->UseHPPotion(static_cast<_float>(m_pItems[2]->GetItemValue()));
                    else if (m_pItems[2]->DecreaseNumItem(1) <= 0)
                    {
                        ITEMMGR->UseHPPotion(static_cast<_float>(m_pItems[2]->GetItemValue()));
                        shared_ptr<CItemBase> pItem = m_pItems[2];
                        vector<shared_ptr<CItemBase>>* pInvenSlots = ITEMMGR->GetItemSlots(m_pItems[2]->GetItemType());
                        auto it = find_if(pInvenSlots->begin(), pInvenSlots->end(), [&pItem](shared_ptr<CItemBase> _pItem) {
                            if (pItem == _pItem)
                                return true;

                            return false;
                            });

                        if (it != pInvenSlots->end())
                        {
                            it->reset();
                            *it = nullptr;
                        }

                        RemoveItemFromSlot(2);
                    }

                    GAMEINSTANCE->PlaySoundW("UI_Potion_SFX_01", 1.f);
                }
                break;
                case ITEM_USABLE_MP:
                {

                }
                break;
                }
            }
        }

        if (GAMEINSTANCE->KeyDown(DIK_F4))
        {
            if (!IsEmpty(3))
            {
                switch (m_pItems[3]->GetItemUsableType())
                {
                case ITEM_USABLE_HP:
                {
                    if (m_pItems[3]->DecreaseNumItem(1) == 1)
                        ITEMMGR->UseHPPotion(static_cast<_float>(m_pItems[3]->GetItemValue()));
                    else if (m_pItems[3]->DecreaseNumItem(1) <= 0)
                    {
                        ITEMMGR->UseHPPotion(static_cast<_float>(m_pItems[3]->GetItemValue()));
                        shared_ptr<CItemBase> pItem = m_pItems[3];
                        vector<shared_ptr<CItemBase>>* pInvenSlots = ITEMMGR->GetItemSlots(m_pItems[3]->GetItemType());
                        auto it = find_if(pInvenSlots->begin(), pInvenSlots->end(), [&pItem](shared_ptr<CItemBase> _pItem) {
                            if (pItem == _pItem)
                                return true;

                            return false;
                            });

                        if (it != pInvenSlots->end())
                        {
                            it->reset();
                            *it = nullptr;
                        }

                        RemoveItemFromSlot(3);
                    }

                    GAMEINSTANCE->PlaySoundW("UI_Potion_SFX_01", 1.f);
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

HRESULT CUIInstanceQuickSlot::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

shared_ptr<CUIInstanceQuickSlot> CUIInstanceQuickSlot::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUIInstanceQuickSlot> pInstance = make_shared<CUIInstanceQuickSlot>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIInstanceQuickSlot::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
