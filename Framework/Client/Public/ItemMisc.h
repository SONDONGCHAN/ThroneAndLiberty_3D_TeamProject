#pragma once

#include "ItemBase.h"

BEGIN(Client)

class CItemMisc : public CItemBase
{
	using Super = CItemBase;

public:
	CItemMisc();
	virtual ~CItemMisc();

public:
	virtual HRESULT Initialize(ItemDesc& _itemDesc);
	virtual void PriorityTick(_float _deltaTime) override;
	virtual void Tick(_float _deltaTime) override;
	virtual void LateTick(_float _deltaTime) override;
	virtual HRESULT Render() override;

public:
	static shared_ptr<CItemMisc> Create(ItemDesc& _itemDesc);

};

END
