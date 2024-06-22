#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CUIBase;
class CUISlot;
class CEquipment;
class CItemBase;
class CPlayer;
class CUICoolTime;

class CItemMgr
{
	DECLARE_SINGLETON(Client::CItemMgr)

public:
	CItemMgr();
	~CItemMgr();

public:
	HRESULT Initialize();

public: /* Get & Set Function */
	shared_ptr<CUISlot> GetSlotByIndex(ITEM_TYPE eItemType, _uint _iSlotIndex);
	vector<shared_ptr<CItemBase>>* GetItemSlots(ITEM_TYPE eItemType);
	vector<shared_ptr<CItemBase>>* GetItemQuickSlots();
	shared_ptr<CUISlot> GetQuickSlotByIndex(_uint _iSlotIndex);

	_uint GetCurrentWeaponIndex() { return m_iWeaponIndex; }

	void GetCurrentSkillCoolTime(const string& _strCoolTimeTag, _float _fCurCoolTime);
	void GetSkillCoolTime(const string& _strCoolTimeTag, _float _fMaxCoolTime);

	_uint GetMoney() { return m_iMoney; }

public:
	void BindPlayerEquipSlot(vector<shared_ptr<CEquipment>>* _pPlayerEquipVec);
	void BindInventorySlot();
	void BindPlayer(shared_ptr<CPlayer> _pPlayer);
	void BindQuickSlot();
	void BindSkillSlot();
	void BindSkillCoolTime();

	void BindCurrentWeaponIndex(_uint _iWeaponIndex);

public:
	HRESULT CreateEquipItem(CRAFT_SET_TAG _eCraftTag);
	shared_ptr<CItemBase> FindItem(ITEM_TYPE _eItemType, string _strItemTag);

public:
	void AddItemToInventory(shared_ptr<CItemBase> _pItem);
	void AddItemByIndex(_uint _iIndex, shared_ptr<CItemBase> _pItem);

	void AddItemToQuickSlot(shared_ptr<CItemBase> _pItem);
	void AddItemToQuickSlotByIndex(_uint _iIndex, shared_ptr<CItemBase> _pItem);

public:
	void DeleteCertainItem(ITEM_TYPE _eItemType, string _strItemTag);

public:
	_uint GetItemCount(ITEM_TYPE _eItemType, string _strItemTag);
	_bool DecreaseItemFromInventory(ITEM_TYPE _eItemType, string _strItemTag, _uint _numItem = 1);
	_bool CanDecreaseItemFromInventory(ITEM_TYPE _eItemType, string _strItemTag, _uint _numItem = 1);

public:
	_bool IncreaseMoney(_uint iMoney = 0);
	_bool DecreaseMoney(_uint iMoney = 0);

public: /* Equipment */
	void EquipItem(ITEM_EQUIP_TYPE _eItemEquipType, const string& _strModelKey);
	void TakeOffItem(ITEM_EQUIP_TYPE _eItemEquipType);
	void ResetItemEquippedState(ITEM_EQUIP_TYPE _eItemEquipType);

public: /* Usable */
	void UseHPPotion(_float _fValue);

public:
	void PrepareCraftItem();

private: /* Temp */
	void PrepareUsableItem();
	void PrepareMiscItem();

private:
	vector<shared_ptr<CEquipment>>* m_pPlayerEquipVec{ nullptr }; // ÇÃ·¹ÀÌ¾îÀÇ Àåºñ ½½·Ô
	
	vector<vector<shared_ptr<CUISlot>>> m_InventorySlots; // Slots of Inventory(Equip, Usable, Misc)

	vector<shared_ptr<CUISlot>> m_QuickSlots; // Äü½½·ÔÀÇ ½½·Ô
	/* 0 - ÆÐ¸µ, 1 ~ ÇÃ·¹ÀÌ¾î ½ºÅ³ */
	vector<shared_ptr<CUISlot>> m_SkillSlots; // Äü½½·ÔÀÇ ½ºÅ³ ½½·Ô
	vector<shared_ptr<CUICoolTime>> m_SkillCoolTimes;// ½ºÅ³ ÄðÅ¸ÀÓ

private:
	weak_ptr<CPlayer> m_pPlayer;

private:
	_uint m_iWeaponIndex{ 0 };

private:
	_uint m_iMoney{ 0 };

};

END
