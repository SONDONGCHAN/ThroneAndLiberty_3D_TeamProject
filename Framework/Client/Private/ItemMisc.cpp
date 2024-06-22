#include "ItemMisc.h"

CItemMisc::CItemMisc()
{
}

CItemMisc::~CItemMisc()
{
}

HRESULT CItemMisc::Initialize(ItemDesc& _itemDesc)
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	m_ItemDesc = _itemDesc;

	return S_OK;
}

void CItemMisc::PriorityTick(_float _deltaTime)
{
}

void CItemMisc::Tick(_float _deltaTime)
{
}

void CItemMisc::LateTick(_float _deltaTime)
{
}

HRESULT CItemMisc::Render()
{
	return S_OK;
}

shared_ptr<CItemMisc> CItemMisc::Create(ItemDesc& _itemDesc)
{
    shared_ptr<CItemMisc> pInstance = make_shared<CItemMisc>();

    if (FAILED(pInstance->Initialize(_itemDesc)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CItemMisc::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
