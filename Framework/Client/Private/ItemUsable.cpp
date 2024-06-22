#include "ItemUsable.h"

CItemUsable::CItemUsable()
{

}

CItemUsable::~CItemUsable()
{

}

HRESULT CItemUsable::Initialize(ItemDesc& _itemDesc)
{
    if (FAILED(Super::Initialize()))
        return E_FAIL;

    m_ItemDesc = _itemDesc;

    return S_OK;
}

void CItemUsable::PriorityTick(_float _deltaTime)
{

}

void CItemUsable::Tick(_float _deltaTime)
{

}

void CItemUsable::LateTick(_float _deltaTime)
{

}

HRESULT CItemUsable::Render()
{
	
    return S_OK;
}

shared_ptr<CItemUsable> CItemUsable::Create(ItemDesc& _itemDesc)
{
    shared_ptr<CItemUsable> pInstance = make_shared<CItemUsable>();

    if (FAILED(pInstance->Initialize(_itemDesc)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CItemUsable::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
