#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"
#include "GameObject.h"
#include "VIInstancing.h"

BEGIN(Engine)

class CGameObject;
class CVIInstancePoint;
class CUIItem;

END

BEGIN(Client)

class CEquipment;

class CItemBase abstract : public CGameObject
{
	using Super = CGameObject;

public:
	struct ItemDesc
	{
		string strItemTag{ "" };
		ITEM_TYPE eItemType{ ITEM_END };
		ITEM_EQUIP_TYPE eItemEquipType{ ITEM_EQUIP_END };
		ITEM_USABLE_TYPE eItemUsableType{ ITEM_USABLE_END };
		string strTexTag{ "" };
		string strModelTag{ "" };
		_uint numItem{ 0 };
		_uint iValue{ 0 };
		_uint iPrice{ 0 };
		wstring wstrItemDescription{L""}; 
	};

public:
	CItemBase();
	virtual ~CItemBase();

public:
	virtual HRESULT Initialize();
	virtual void PriorityTick(_float _deltaTime) override;
	virtual void Tick(_float _deltaTime) override;
	virtual void LateTick(_float _deltaTime) override;
	virtual HRESULT Render() override;

public: /* Get & Set Function */
	ItemDesc& GetItemDesc() { return m_ItemDesc; }
	void SetItemDesc(ItemDesc& _itemDesc) { m_ItemDesc = _itemDesc; }

	string GetItemTag() { return m_ItemDesc.strItemTag; }
	void SetItemTag(const string& _strItemTag) { m_ItemDesc.strItemTag = _strItemTag; }

	ITEM_TYPE GetItemType() { return m_ItemDesc.eItemType; }
	void SetItemType(ITEM_TYPE _eItemType) { m_ItemDesc.eItemType = _eItemType; }

	ITEM_EQUIP_TYPE GetItemEquipType() { return m_ItemDesc.eItemEquipType; }
	void SetItemEquipType(ITEM_EQUIP_TYPE _eItemEquipType) { m_ItemDesc.eItemEquipType = _eItemEquipType; }

	ITEM_USABLE_TYPE GetItemUsableType() { return m_ItemDesc.eItemUsableType; }
	void SetItemUsableType(ITEM_USABLE_TYPE _eItemUsableType) { m_ItemDesc.eItemUsableType = _eItemUsableType; }

	/* Texture */
	string GetItemTexTag() { return m_ItemDesc.strTexTag; }
	void SetItemTexTag(const string& _strItemTexTag) { m_ItemDesc.strTexTag = _strItemTexTag; }

	string GetItemModelTag() { return m_ItemDesc.strModelTag; }
	void SetItemModelTag(const string& _strItemModelTag) { m_ItemDesc.strModelTag = _strItemModelTag; }

	_uint GetNumItem() { return m_ItemDesc.numItem; }
	void SetNumItem(_uint _iNumItem) { m_ItemDesc.numItem = _iNumItem; }

	_bool IsEquipped() { return m_isEquipped; }
	void SetIsEquipped(_bool _isEquipped) { m_isEquipped = _isEquipped; }

	_uint GetItemValue() { return m_ItemDesc.iValue; }
	void SetItemValue(_uint _iValue) { m_ItemDesc.iValue = _iValue; }

	_uint GetItemPrice() { return m_ItemDesc.iPrice; }
	void SetItemPrice(_uint _iPrice) { m_ItemDesc.iPrice = _iPrice; }

	wstring GetItemDescription() { return m_ItemDesc.wstrItemDescription; }
	void SetItemDescription(wstring _wstrItemDescription) { m_ItemDesc.wstrItemDescription = _wstrItemDescription; }

public:
	_int IncreaseNumItem(_uint _iIncNum = 1); // return이 0이면 무난하게 성공, -1이면 실패, 이외의 수일경우 하고 남은 수
	_int DecreaseNumItem(_uint _iDecNum = 1); // return이 0이면 무난하게 성공, -1이면 삭제, 1일경우 실패

protected:
	ItemDesc m_ItemDesc{};

protected:
	string m_strEquipmentTag; // Bind with CEquipment using string?
	weak_ptr<CEquipment> m_pEquipment; // Bind with CEquipment using weak_ptr?

protected:
	_bool m_isEquipped{ false }; // Only for Equipment

};

END
