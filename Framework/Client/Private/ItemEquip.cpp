#include "ItemEquip.h"

CItemEquip::CItemEquip()
{

}

CItemEquip::~CItemEquip()
{

}

HRESULT CItemEquip::Initialize(ItemDesc& _itemDesc)
{
    if (FAILED(Super::Initialize()))
        return E_FAIL;
        
    m_ItemDesc = _itemDesc;

	return S_OK;
}

void CItemEquip::PriorityTick(_float _deltaTime)
{

}

void CItemEquip::Tick(_float _deltaTime)
{

}

void CItemEquip::LateTick(_float _deltaTime)
{

}

HRESULT CItemEquip::Render()
{
	
    return S_OK;
}

shared_ptr<CItemEquip> CItemEquip::Create(ItemDesc& _itemDesc)
{
    shared_ptr<CItemEquip> pInstance = make_shared<CItemEquip>();

    if (FAILED(pInstance->Initialize(_itemDesc)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CItemEquip::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
