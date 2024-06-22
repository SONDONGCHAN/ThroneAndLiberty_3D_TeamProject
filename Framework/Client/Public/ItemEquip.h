#pragma once

#include "ItemBase.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CItemEquip final : public CItemBase
{
	using Super = CItemBase;

public:
	CItemEquip();
	virtual ~CItemEquip();

public:
	virtual HRESULT Initialize(ItemDesc& _itemDesc);
	virtual void PriorityTick(_float _deltaTime) override;
	virtual void Tick(_float _deltaTime) override;
	virtual void LateTick(_float _deltaTime) override;
	virtual HRESULT Render() override;

public:
	static shared_ptr<CItemEquip> Create(ItemDesc& _itemDesc);

};

END
