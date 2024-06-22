#include "UICraftBG.h"
#include "ItemMgr.h"
#include "UIMgr.h"
#include "UICraftTab.h"
#include "UICraftSlot.h"
#include "UIStarForce.h"
#include "UIPanel.h"
#include "UICraftResult.h"
#include "GameMgr.h"

#include "GameInstance.h"
#include "VIInstanceUI.h"

CUICraftBG::CUICraftBG()
{

}

CUICraftBG::~CUICraftBG()
{

}

HRESULT CUICraftBG::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

    wstring strSetName0 = L"±×¸²ÀÚ °©¿Ê ¼¼Æ®";
    wstring strSetName1 = L"È²±Ý °©¿Ê ¼¼Æ®";
    wstring strSetName2 = L"±×³É °©¿Ê ¼¼Æ®";
    wstring strSetName3 = L"ÃµÂÉ°¡¸® ¼¼Æ®";

    m_strItemSetNames.emplace_back(strSetName0);
    m_strItemSetNames.emplace_back(strSetName1);
    m_strItemSetNames.emplace_back(strSetName2);
    m_strItemSetNames.emplace_back(strSetName3);

    m_iItemSetPrices.emplace_back(500);
    m_iItemSetPrices.emplace_back(1000);
    m_iItemSetPrices.emplace_back(1500);
    m_iItemSetPrices.emplace_back(2000);

    return S_OK;
}

void CUICraftBG::PriorityTick(_float _fTimeDelta)
{

}

void CUICraftBG::Tick(_float _fTimeDelta)
{
    KeyInput();

    /* ºÎ¸ð¸¦ µû¶ó´Ù´Ï°Ô²û */
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

void CUICraftBG::LateTick(_float _fTimeDelta)
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

HRESULT CUICraftBG::Render()
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

HRESULT CUICraftBG::RenderFont()
{
    if (m_UIDesc.strTag == "CraftBBGs")
    {
        if (FAILED(GAMEINSTANCE->RenderFont("Font_DNFB14", L"Á¦ÀÛ", _float2((m_vUIPos[17].x - 34.5f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[17].y - 22.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_CENTER, _float4(0.65f, 0.52f, 0.42f, 1.f))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->RenderFont("Font_DNFB14", L"ÀÏ°ý Á¦ÀÛ", _float2((m_vUIPos[18].x - 74.5f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[18].y - 22.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_CENTER, _float4(0.65f, 0.52f, 0.42f, 1.f))))
            return E_FAIL;

        weak_ptr<CUICraftTab> pCraftTab = dynamic_pointer_cast<CUICraftTab>(UIMGR->FindUI("CraftFTabs"));
        _uint iCurTabIdx = pCraftTab.lock()->GetCurTabIndex();

        if (FAILED(GAMEINSTANCE->RenderFont("Font_DNFB16", m_strItemSetNames[iCurTabIdx], _float2((m_vUIPos[8].x - 140.f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[8].y + 55.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_END, _float4(0.65f, 0.52f, 0.42f, 1.f))))
            return E_FAIL;

        wstring strAmount = to_wstring(m_iItemSetPrices[iCurTabIdx]);
        _uint iAmountOffset = static_cast<_uint>(strAmount.size()) * 4.f;
        _uint iInsertNum{ 0 };

        auto rit = strAmount.rbegin();
        wchar_t ch = L',';
        for (rit; rit != strAmount.rend(); ++rit)
        {
            if (iInsertNum % 3 == 0 && iInsertNum != 0)
            {
                auto it = rit.base();
                strAmount.insert(it, ch);
            }
            iInsertNum++;
        }

        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", strAmount, _float2((m_vUIPos[16].x - iAmountOffset) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[16].y - 8.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_END, _float4(255.f / 255.f, 247.f / 255.f, 153.f / 255.f, 1.f))))
            return E_FAIL;
    }

    else if (m_UIDesc.strTag == "CraftCBGs")
    {
        wstring strAmount = to_wstring(ITEMMGR->GetMoney());
        _uint iAmountOffset = static_cast<_uint>(strAmount.size());
        _uint iInsertNum{ 0 };

        auto rit = strAmount.rbegin();
        wchar_t ch = L',';
        for (rit; rit != strAmount.rend(); ++rit)
        {
            if (iInsertNum % 3 == 0 && iInsertNum != 0)
            {
                auto it = rit.base();
                strAmount.insert(it, ch);
            }
            iInsertNum++;
        }

        if (FAILED(GAMEINSTANCE->RenderFont("Font_SuiteL10", strAmount, _float2((m_vUIPos[14].x + 15.f) + (g_iWinSizeX * 0.5f), (-m_vUIPos[14].y - 7.5f) + (g_iWinSizeY * 0.5f)), CCustomFont::FA_END, _float4(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 1.f)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->RenderFont("Font_DNFB12", L"Á¦ÀÛ Àç·á", _float2((m_vUIPos[12].x + 10.f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[12].y - 18.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_CENTER, _float4(0.65f, 0.52f, 0.42f, 1.f))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->RenderFont("Font_DNFB12", L"Á¦ÀÛ ºñ¿ë", _float2((m_vUIPos[13].x - 128.f) + (g_iWinSizeX * 0.5f), ((-m_vUIPos[13].y - 18.f) + (g_iWinSizeY * 0.5f))), CCustomFont::FA_CENTER, _float4(0.65f, 0.52f, 0.42f, 1.f))))
            return E_FAIL;
    }

    return S_OK;
}

void CUICraftBG::SetCraftBGScaling(_uint _slotIndex)
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    if (m_UIDesc.strTag == "CraftBBGs")
    {
        if (_slotIndex == 17 || _slotIndex == 18)
        {
            vertices[_slotIndex].vRight = _float4(vertices[_slotIndex].vRight.x * 0.9f, 0.f, 0.f, 0.f);
            vertices[_slotIndex].vUp = _float4(0.f, vertices[_slotIndex].vUp.y * 0.9f, 0.f, 0.f);

            m_fScalingTime = 0.f;
        }
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUICraftBG::InitUpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    if (m_UIDesc.strTag == "CraftABGs")
    {
        for (_uint i = 0; i < iNumSlots; ++i)
        {
            vertices[i].iTexIndex = i;
            vertices[i].iSlotUV = 0;
            vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
            vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
        }
    }

    else if (m_UIDesc.strTag == "CraftBBGs")
    {
        for (_uint i = 0; i < iNumSlots; ++i)
        {
            if (i <= 5)
            {
                vertices[i].iTexIndex = 0;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }

            else if (i == 6)
            {
                vertices[i].iTexIndex = 1;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }

            else if (i == 7)
            {
                vertices[i].iTexIndex = 2;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }

            else if (i == 8)
            {
                vertices[i].iTexIndex = 3;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }

            else if (i == 9)
            {
                vertices[i].iTexIndex = 4;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }

            else if (i > 9 && i <= 14)
            {
                vertices[i].iTexIndex = 5;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }

            else if (i == 15)
            {
                vertices[i].iTexIndex = 6;
                vertices[i].iSlotUV = 1;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }

            else if (i > 15 && i <= 16)
            {
                vertices[i].iTexIndex = 6;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }

            else if (i > 16 && i <= 18)
            {
                vertices[i].iTexIndex = 7;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }

            else if (i == 19)
            {
                vertices[i].iTexIndex = 8;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }
        }
    }

    else if (m_UIDesc.strTag == "CraftCBGs")
    {
        for (_uint i = 0; i < iNumSlots; ++i)
        {
            if (i == 7) // ¿À¸¥ÂÊ ½½·Ô
            {
                vertices[i].iTexIndex = 4;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);

                continue;
            }

            if (i <= 11)
            {
                vertices[i].iTexIndex = 0;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }

            else if (i == 12)
            {
                vertices[i].iTexIndex = 1;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }

            else if (i > 12 && i <= 14)
            {
                vertices[i].iTexIndex = 2;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }

            else if (i == 15)
            {
                vertices[i].iTexIndex = 3;
                vertices[i].iSlotUV = 0;
                vertices[i].vRight = _float4(m_vUISize[i].x, 0.f, 0.f, 0.f);
                vertices[i].vUp = _float4(0.f, m_vUISize[i].y, 0.f, 0.f);
            }
        }
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

void CUICraftBG::UpdateTexture()
{
    D3D11_MAPPED_SUBRESOURCE subResource{};

    ComPtr<ID3D11Buffer> instanceBuffer = m_pInstanceBuffer->GetInstanceBuffer();

    GAMEINSTANCE->GetDeviceContextInfo()->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

    VTXUI* vertices = reinterpret_cast<VTXUI*>(subResource.pData);

    _uint iNumSlots = static_cast<_uint>(m_vUIPos.size());

    _int iNumMouse = IsInstanceMouseOn();

    if (m_UIDesc.strTag == "CraftBBGs")
    {
        vertices[17].vRight = SimpleMath::Vector4::Lerp(vertices[17].vRight, _float4(m_vUISize[17].x, 0.f, 0.f, 0.f), m_fScalingTime);
        vertices[17].vUp = SimpleMath::Vector4::Lerp(vertices[17].vUp, _float4(0.f, m_vUISize[17].y, 0.f, 0.f), m_fScalingTime);

        if (vertices[17].vRight.x >= m_vUISize[17].x && vertices[17].vUp.y >= m_vUISize[17].y)
        {
            vertices[17].vRight = _float4(m_vUISize[17].x, 0.f, 0.f, 0.f);
            vertices[17].vUp = _float4(0.f, m_vUISize[17].y, 0.f, 0.f);
        }

        vertices[18].vRight = SimpleMath::Vector4::Lerp(vertices[18].vRight, _float4(m_vUISize[18].x, 0.f, 0.f, 0.f), m_fScalingTime);
        vertices[18].vUp = SimpleMath::Vector4::Lerp(vertices[18].vUp, _float4(0.f, m_vUISize[18].y, 0.f, 0.f), m_fScalingTime);

        if (vertices[18].vRight.x >= m_vUISize[18].x && vertices[18].vUp.y >= m_vUISize[18].y)
        {
            vertices[18].vRight = _float4(m_vUISize[18].x, 0.f, 0.f, 0.f);
            vertices[18].vUp = _float4(0.f, m_vUISize[18].y, 0.f, 0.f);
        }

        weak_ptr<CUICraftTab> pCraftTab = dynamic_pointer_cast<CUICraftTab>(UIMGR->FindUI("CraftFTabs"));
        _uint iTabIndex = pCraftTab.lock()->GetCurTabIndex();

        if (GAMEINSTANCE->MouseDown(DIM_LB) && iNumMouse == 17)
        {
            SetCraftBGScaling(17);
        }

        if (GAMEINSTANCE->MouseDown(DIM_LB) && iNumMouse == 18)
        {
            SetCraftBGScaling(18);

            weak_ptr<CUIBase> pStarForcePanel = UIMGR->FindUI("StarForceBGPanel");
            if (pStarForcePanel.expired())
                return;
            weak_ptr<CUIStarForce> pStarForce = dynamic_pointer_cast<CUIStarForce>(UIMGR->FindUI("StarForceBGs"));
            if (pStarForce.expired())
                return;

            switch (iTabIndex)
            {
            case 0:
            {
                if (ITEMMGR->DecreaseMoney(m_iItemSetPrices[iTabIndex]))
                    m_bIsAvailableToBuy = true;
                else
                    m_bIsAvailableToBuy = false;
            }
            break;
            case 1:
            {
                if (ITEMMGR->DecreaseMoney(m_iItemSetPrices[iTabIndex]))
                    m_bIsAvailableToBuy = true;
                else
                    m_bIsAvailableToBuy = false;
            }
            break;
            case 2:
            {
                if (ITEMMGR->DecreaseMoney(m_iItemSetPrices[iTabIndex]))
                    m_bIsAvailableToBuy = true;
                else
                    m_bIsAvailableToBuy = false;
            }
            break;
            case 3:
            {
                if (ITEMMGR->DecreaseMoney(m_iItemSetPrices[iTabIndex]))
                    m_bIsAvailableToBuy = true;
                else
                    m_bIsAvailableToBuy = false;
            }
            break;
            }

            if (m_bIsAvailableToBuy)
            {
                weak_ptr<CUICraftTab> pCraftTab = dynamic_pointer_cast<CUICraftTab>(UIMGR->FindUI("CraftFTabs"));
                _uint iTabIndex = pCraftTab.lock()->GetCurTabIndex();

                weak_ptr<CUICraftSlot> pCraftSlot = dynamic_pointer_cast<CUICraftSlot>(UIMGR->FindUI("CraftDSlots"));
                if (pCraftSlot.expired())
                    return;

                CItemBase::ItemDesc pItemDesc0 = pCraftSlot.lock()->GetItemDesc(7 + 12 * (iTabIndex));
                CItemBase::ItemDesc pItemDesc1 = pCraftSlot.lock()->GetItemDesc(8 + 12 * (iTabIndex));
                CItemBase::ItemDesc pItemDesc2 = pCraftSlot.lock()->GetItemDesc(9 + 12 * (iTabIndex));
                CItemBase::ItemDesc pItemDesc3 = pCraftSlot.lock()->GetItemDesc(10 + 12 * (iTabIndex));
                CItemBase::ItemDesc pItemDesc4 = pCraftSlot.lock()->GetItemDesc(11 + 12 * (iTabIndex));

                if (ITEMMGR->CanDecreaseItemFromInventory(ITEM_MISC, pItemDesc0.strItemTag, pItemDesc0.numItem) &&
                    ITEMMGR->CanDecreaseItemFromInventory(ITEM_MISC, pItemDesc1.strItemTag, pItemDesc1.numItem) &&
                    ITEMMGR->CanDecreaseItemFromInventory(ITEM_MISC, pItemDesc2.strItemTag, pItemDesc2.numItem) &&
                    ITEMMGR->CanDecreaseItemFromInventory(ITEM_MISC, pItemDesc3.strItemTag, pItemDesc3.numItem) &&
                    ITEMMGR->CanDecreaseItemFromInventory(ITEM_MISC, pItemDesc4.strItemTag, pItemDesc4.numItem))
                {
                    pStarForcePanel.lock()->SetIsRender(true);
                    pStarForce.lock()->ResetStarForce();
                    UIMGR->ActivateStarForce();
                }
                else
                {
                    ITEMMGR->IncreaseMoney(m_iItemSetPrices[iTabIndex]);
                    UIMGR->ActivateFailedCraftResult();
                    weak_ptr<CUICraftResult> pCraftResult = dynamic_pointer_cast<CUICraftResult>(UIMGR->FindUI("CraftResultInstance"));
                    if (!pCraftResult.expired())
                        pCraftResult.lock()->SetSuccessType(2);
                }
            }

            else
            {
                UIMGR->ActivateFailedCraftResult();
                weak_ptr<CUICraftResult> pCraftResult = dynamic_pointer_cast<CUICraftResult>(UIMGR->FindUI("CraftResultInstance"));
                if (!pCraftResult.expired())
                    pCraftResult.lock()->SetSuccessType(1);
            }

            /*weak_ptr<CUICraftSlot> pCraftSlot = dynamic_pointer_cast<CUICraftSlot>(UIMGR->FindUI("CraftDSlots"));
            if (pCraftSlot.expired())
                return;

            CItemBase::ItemDesc pItemDesc0 = pCraftSlot.lock()->GetItemDesc(7 + 12 * (iTabIndex));
            CItemBase::ItemDesc pItemDesc1 = pCraftSlot.lock()->GetItemDesc(8 + 12 * (iTabIndex));
            CItemBase::ItemDesc pItemDesc2 = pCraftSlot.lock()->GetItemDesc(9 + 12 * (iTabIndex));
            CItemBase::ItemDesc pItemDesc3 = pCraftSlot.lock()->GetItemDesc(10 + 12 * (iTabIndex));
            CItemBase::ItemDesc pItemDesc4 = pCraftSlot.lock()->GetItemDesc(11 + 12 * (iTabIndex));

            if (ITEMMGR->DecreaseItemFromInventory(ITEM_MISC, pItemDesc0.strItemTag, pItemDesc0.numItem) &&
                ITEMMGR->DecreaseItemFromInventory(ITEM_MISC, pItemDesc1.strItemTag, pItemDesc1.numItem) &&
                ITEMMGR->DecreaseItemFromInventory(ITEM_MISC, pItemDesc2.strItemTag, pItemDesc2.numItem) &&
                ITEMMGR->DecreaseItemFromInventory(ITEM_MISC, pItemDesc3.strItemTag, pItemDesc3.numItem) &&
                ITEMMGR->DecreaseItemFromInventory(ITEM_MISC, pItemDesc4.strItemTag, pItemDesc4.numItem))
            {
                ITEMMGR->CreateEquipItem(static_cast<CRAFT_SET_TAG>(iTabIndex));
            }*/
        }
    }

    if (m_UIDesc.strTag == "CraftCBGs")
    {
        if (GAMEINSTANCE->MouseDown(DIM_LB) && iNumMouse == 15)
        {
            UIMGR->DeactivateUI("CraftAllPanel");
            UIMGR->ActivateHUD();
            CGameMgr::GetInstance()->ChangeToNormalMouse();
        }
    }

    GAMEINSTANCE->GetDeviceContextInfo()->Unmap(instanceBuffer.Get(), 0);
}

HRESULT CUICraftBG::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
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

shared_ptr<CUICraftBG> CUICraftBG::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize)
{
    shared_ptr<CUICraftBG> pInstance = make_shared<CUICraftBG>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc, _vUIPos, _vUISize)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUICraftBG::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
