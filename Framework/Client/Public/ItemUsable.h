#pragma once

#include "ItemBase.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CItemUsable : public CItemBase
{
	using Super = CItemBase;

public:
	CItemUsable();
	virtual ~CItemUsable();

public:
	virtual HRESULT Initialize(ItemDesc& _itemDesc);
	virtual void PriorityTick(_float _deltaTime) override;
	virtual void Tick(_float _deltaTime) override;
	virtual void LateTick(_float _deltaTime) override;
	virtual HRESULT Render() override;

public:
	static shared_ptr<CItemUsable> Create(ItemDesc& _itemDesc);

};

END
