#include "ItemMgr.h"
#include "ItemBase.h"
#include "UISlot.h"
#include "UIMgr.h"
#include "Player.h"
#include "ItemUsable.h"
#include "UICoolTime.h"
#include "UIInstanceItemSlot.h"
#include "UIInstanceQuickSlot.h"
#include "ItemEquip.h"
#include "ItemMisc.h"

#include "GameInstance.h"
#include "VIInstancePoint.h"

IMPLEMENT_SINGLETON(Client::CItemMgr)

CItemMgr::CItemMgr()
{

}

CItemMgr::~CItemMgr()
{

}

HRESULT CItemMgr::Initialize()
{
	//BindInventorySlot();

	//BindQuickSlot();

	BindSkillSlot();

	BindSkillCoolTime();

	//PrepareUsableItem();

	//PrepareMiscItem();

	//CreateEquipItem(SET_PLATE_0);

	return S_OK;
}

shared_ptr<CUISlot> CItemMgr::GetSlotByIndex(ITEM_TYPE eItemType, _uint _iSlotIndex)
{
	shared_ptr<CUISlot> pSlot = m_InventorySlots[eItemType][_iSlotIndex];
	if (pSlot == nullptr)
		return nullptr;

	return pSlot;
}

vector<shared_ptr<CItemBase>>* CItemMgr::GetItemSlots(ITEM_TYPE eItemType)
{
	switch (eItemType)
	{
	case ITEM_EQUIP:
	{
		shared_ptr<CUIInstanceItemSlot> pUISlot = dynamic_pointer_cast<CUIInstanceItemSlot>(UIMGR->FindUI("InvEquipSlot0"));

		vector<shared_ptr<CItemBase>>* pItemSlots = pUISlot->GetItemSlotsPtr();

		return pItemSlots;
	}
	break;
	case ITEM_USABLE:
	{
		shared_ptr<CUIInstanceItemSlot> pUISlot = dynamic_pointer_cast<CUIInstanceItemSlot>(UIMGR->FindUI("UsableSlot0"));
		
		vector<shared_ptr<CItemBase>>* pItemSlots = pUISlot->GetItemSlotsPtr();

		return pItemSlots;
	}
	break;
	case ITEM_MISC:
	{
		shared_ptr<CUIInstanceItemSlot> pUISlot = dynamic_pointer_cast<CUIInstanceItemSlot>(UIMGR->FindUI("MiscSlot0"));
		
		vector<shared_ptr<CItemBase>>* pItemSlots = pUISlot->GetItemSlotsPtr();

		return pItemSlots;
	}
	break;
	}

	return nullptr;
}

vector<shared_ptr<CItemBase>>* CItemMgr::GetItemQuickSlots()
{
	shared_ptr<CUIInstanceQuickSlot> pUISlot = dynamic_pointer_cast<CUIInstanceQuickSlot>(UIMGR->FindUI("QuickFixedSlot0"));

	vector<shared_ptr<CItemBase>>* pItemQuickSlots = pUISlot->GetItemQuickSlots();

	return pItemQuickSlots;
}

shared_ptr<CUISlot> CItemMgr::GetQuickSlotByIndex(_uint _iSlotIndex)
{
	shared_ptr<CUISlot> pSlot = m_QuickSlots[_iSlotIndex];
	if (pSlot == nullptr)
		return nullptr;

	return pSlot;
}

void CItemMgr::GetCurrentSkillCoolTime(const string& _strCoolTimeTag, _float _fCurCoolTime)
{
	if (_strCoolTimeTag == "QuickCombatSlot0CoolTime")
		m_SkillCoolTimes[0]->SetCurCoolTime(_fCurCoolTime);

	if (_strCoolTimeTag == "QuickMainSlot0CoolTime")
	{
		m_SkillCoolTimes[1]->SetCurCoolTime(_fCurCoolTime);
	}

	else if (_strCoolTimeTag == "QuickMainSlot1CoolTime")
	{
		m_SkillCoolTimes[2]->SetCurCoolTime(_fCurCoolTime);
	}

	else if (_strCoolTimeTag == "QuickMainSlot2CoolTime")
	{
		m_SkillCoolTimes[3]->SetCurCoolTime(_fCurCoolTime);
	}

	else if (_strCoolTimeTag == "QuickMainSlot3CoolTime")
	{
		m_SkillCoolTimes[4]->SetCurCoolTime(_fCurCoolTime);
	}

	else if (_strCoolTimeTag == "QuickMainSlot4CoolTime")
	{
		m_SkillCoolTimes[5]->SetCurCoolTime(_fCurCoolTime);
	}
}

void CItemMgr::GetSkillCoolTime(const string& _strCoolTimeTag, _float _fMaxCoolTime)
{
	if (_strCoolTimeTag == "QuickCombatSlot0CoolTime")
		m_SkillCoolTimes[0]->SetMaxCoolTime(_fMaxCoolTime);

	if (_strCoolTimeTag == "QuickMainSlot0CoolTime")
	{
		m_SkillCoolTimes[1]->SetMaxCoolTime(_fMaxCoolTime);
	}

	else if (_strCoolTimeTag == "QuickMainSlot1CoolTime")
	{
		m_SkillCoolTimes[2]->SetMaxCoolTime(_fMaxCoolTime);
	}

	else if (_strCoolTimeTag == "QuickMainSlot2CoolTime")
	{
		m_SkillCoolTimes[3]->SetMaxCoolTime(_fMaxCoolTime);
	}

	else if (_strCoolTimeTag == "QuickMainSlot3CoolTime")
	{
		m_SkillCoolTimes[4]->SetMaxCoolTime(_fMaxCoolTime);
	}

	else if (_strCoolTimeTag == "QuickMainSlot4CoolTime")
	{
		m_SkillCoolTimes[5]->SetMaxCoolTime(_fMaxCoolTime);
	}
}

void CItemMgr::BindPlayerEquipSlot(vector<shared_ptr<CEquipment>>* _pPlayerEquipVec)
{
	if (_pPlayerEquipVec != nullptr)
		m_pPlayerEquipVec = _pPlayerEquipVec;
}

void CItemMgr::BindInventorySlot()
{
	/* 일단 하드 코딩 */
	m_InventorySlots.resize(ITEM_END); // Equip, Usable, Misc

#pragma region EquipTab LegacyCode
	/*shared_ptr<CUIBase> pEquipTab = UIMGR->FindUI("InventoryTabPanel0");
	if (pEquipTab == nullptr)
		return;

	unordered_map<string, weak_ptr<CUIBase>> equipSlotList = pEquipTab->GetChildrenList();
	_uint equipListSize = static_cast<_uint>(equipSlotList.size()) - 1;
	m_InventorySlots[ITEM_EQUIP].resize(equipListSize);

	for (auto& slotPair : equipSlotList)
	{
		if (!slotPair.second.expired() && slotPair.first != "InvEquipTabPanelIcon")
		{
			string uiTag = slotPair.second.lock()->GetUITag();
			stringstream ss;

			for (char c : uiTag)
			{
				if (std::isdigit(c))
					ss << c;
			}

			int foundNum{ 0 };
			ss >> foundNum;

			shared_ptr<CUIBase> pSlot;

			unordered_map<string, weak_ptr<CUIBase>> equipSlot = slotPair.second.lock()->GetChildrenList();
			for (auto& pPair : equipSlot)
				pSlot = pPair.second.lock();

			shared_ptr<CUISlot> uiSlot = dynamic_pointer_cast<CUISlot>(pSlot);
			if (uiSlot == nullptr)
				return;

			m_InventorySlots[ITEM_EQUIP][foundNum] = uiSlot;
		}
	}*/
#pragma endregion

#pragma region EquipTab

	


#pragma endregion

#pragma region UsableTab
	shared_ptr<CUIBase> pUsableTab = UIMGR->FindUI("InventoryTabPanel_Usable");
	if (pUsableTab == nullptr)
		return;

	unordered_map<string, weak_ptr<CUIBase>> usableSlotList = pUsableTab->GetChildrenList();
	_uint iUsableSlotSize = static_cast<_uint>(usableSlotList.size()) - 1;
	m_InventorySlots[ITEM_USABLE].resize(iUsableSlotSize);

	for (auto& slotPair : usableSlotList)
	{
		if (!slotPair.second.expired() && slotPair.first != "InvUsableTabPanelIcon")
		{
			string uiTag = slotPair.second.lock()->GetUITag();
			stringstream ss;

			for (char c : uiTag)
			{
				if (std::isdigit(c))
					ss << c;
			}

			int foundNum{ 0 };
			ss >> foundNum;

			shared_ptr<CUIBase> pSlot;

			unordered_map<string, weak_ptr<CUIBase>> equipSlot = slotPair.second.lock()->GetChildrenList();
			for (auto& pPair : equipSlot)
				pSlot = pPair.second.lock();

			shared_ptr<CUISlot> uiSlot = dynamic_pointer_cast<CUISlot>(pSlot);
			if (uiSlot == nullptr)
				return;

			m_InventorySlots[ITEM_USABLE][foundNum] = uiSlot;
		}
	}
#pragma endregion

#pragma region MiscTab
	shared_ptr<CUIBase> pMiscTab = UIMGR->FindUI("InventoryTabPanel_Misc");
	if (pMiscTab == nullptr)
		return;

	unordered_map<string, weak_ptr<CUIBase>> miscSlotList = pMiscTab->GetChildrenList();
	_uint iMiscSlotSize = static_cast<_uint>(miscSlotList.size()) - 1;
	m_InventorySlots[ITEM_MISC].resize(iMiscSlotSize);

	for (auto& slotPair : miscSlotList)
	{
		if (!slotPair.second.expired() && slotPair.first != "InvMiscTabPanelIcon")
		{
			string uiTag = slotPair.second.lock()->GetUITag();
			stringstream ss;

			for (char c : uiTag)
			{
				if (std::isdigit(c))
					ss << c;
			}

			int foundNum{ 0 };
			ss >> foundNum;

			shared_ptr<CUIBase> pSlot;

			unordered_map<string, weak_ptr<CUIBase>> equipSlot = slotPair.second.lock()->GetChildrenList();
			for (auto& pPair : equipSlot)
				pSlot = pPair.second.lock();

			shared_ptr<CUISlot> uiSlot = dynamic_pointer_cast<CUISlot>(pSlot);
			if (uiSlot == nullptr)
				return;

			m_InventorySlots[ITEM_MISC][foundNum] = uiSlot;
		}
	}
#pragma endregion
	
}

void CItemMgr::BindPlayer(shared_ptr<CPlayer> _pPlayer)
{
	if (_pPlayer != nullptr)
		m_pPlayer = _pPlayer;
}

void CItemMgr::BindQuickSlot()
{
	shared_ptr<CUIBase> quickFixedBG = UIMGR->FindUI("QuickSlotSubBG_C2");
	if (quickFixedBG == nullptr)
		return;

	unordered_map<string, weak_ptr<CUIBase>> quickFixedSlotList = quickFixedBG->GetChildrenList();
	_uint quickFixedSlotSize = static_cast<_uint>(quickFixedSlotList.size()) - 1;
	m_QuickSlots.resize(quickFixedSlotSize);

	for (auto& fixedSlot : quickFixedSlotList)
	{
		if (!fixedSlot.second.expired() && fixedSlot.first != "QuickSlotSubBG_R2")
		{
			string uiTag = fixedSlot.second.lock()->GetUITag();
			stringstream ss;

			for (char c : uiTag)
			{
				if (std::isdigit(c))
					ss << c;
			}

			int foundNum{ 0 };
			ss >> foundNum;

			shared_ptr<CUIBase> pSlot{ nullptr };
			unordered_map<string, weak_ptr<CUIBase>> quickSlot = fixedSlot.second.lock()->GetChildrenList();
			for (auto& pPair : quickSlot)
				pSlot = pPair.second.lock();

			shared_ptr<CUISlot> uiSlot = dynamic_pointer_cast<CUISlot>(pSlot);
			if (uiSlot == nullptr)
				return;

			m_QuickSlots[foundNum] = uiSlot;
		}
	}
}

void CItemMgr::BindSkillSlot()
{
	shared_ptr<CUIBase> quickMainSlotBG = UIMGR->FindUI("QuickSlotBackGroundR");
	if (quickMainSlotBG == nullptr)
		return;

	shared_ptr<CUIBase> quickCombatSlotBG = UIMGR->FindUI("QuickSlotSubBG_C");
	if (quickCombatSlotBG == nullptr)
		return;

	unordered_map<string, weak_ptr<CUIBase>> quickMainSlotList = quickMainSlotBG->GetChildrenList();
	_uint quickMainSlotSize = static_cast<_uint>(quickMainSlotList.size()) - 1;
	m_SkillSlots.resize(quickMainSlotSize);

	unordered_map<string, weak_ptr<CUIBase>> quickCombatSlotList = quickMainSlotBG->GetChildrenList();

	for (auto& combatSlot : quickCombatSlotList)
	{
		if (combatSlot.second.lock()->GetUITag() == "QuickCombatSlot0")
		{
			m_SkillSlots[0] = dynamic_pointer_cast<CUISlot>(combatSlot.second.lock());
		}
	}

	for (auto& mainSlot : quickMainSlotList)
	{
		if (!mainSlot.second.expired() && mainSlot.first != "QuickSlotSubBG_R")
		{
			string uiTag = mainSlot.second.lock()->GetUITag();
			stringstream ss;

			for (char c : uiTag)
			{
				if (std::isdigit(c))
					ss << c;
			}

			int foundNum{ 0 };
			ss >> foundNum;

			shared_ptr<CUISlot> uiSlot = dynamic_pointer_cast<CUISlot>(mainSlot.second.lock());
			if (uiSlot == nullptr)
				return;

			m_SkillSlots[foundNum + 1] = uiSlot;
		}
	}
}

void CItemMgr::BindSkillCoolTime()
{
	m_SkillCoolTimes.resize(6);

	{
		shared_ptr<CUICoolTime> pFoundUI = dynamic_pointer_cast<CUICoolTime>(UIMGR->FindUI("QuickCombatSlot0CoolTime"));
		if (pFoundUI == nullptr)
			return;
		
		//m_SkillCoolTimes.emplace_back(pFoundUI);
		m_SkillCoolTimes[0] = pFoundUI;
	}

	{
		shared_ptr<CUICoolTime> pFoundUI = dynamic_pointer_cast<CUICoolTime>(UIMGR->FindUI("QuickMainSlot0CoolTime"));
		if (pFoundUI == nullptr)
			return;

		//m_SkillCoolTimes.emplace_back(pFoundUI);
		m_SkillCoolTimes[1] = pFoundUI;
	}

	{
		shared_ptr<CUICoolTime> pFoundUI = dynamic_pointer_cast<CUICoolTime>(UIMGR->FindUI("QuickMainSlot1CoolTime"));
		if (pFoundUI == nullptr)
			return;

		m_SkillCoolTimes[2] = pFoundUI;
	}

	{
		shared_ptr<CUICoolTime> pFoundUI = dynamic_pointer_cast<CUICoolTime>(UIMGR->FindUI("QuickMainSlot2CoolTime"));
		if (pFoundUI == nullptr)
			return;

		m_SkillCoolTimes[3] = pFoundUI;
	}

	{
		shared_ptr<CUICoolTime> pFoundUI = dynamic_pointer_cast<CUICoolTime>(UIMGR->FindUI("QuickMainSlot3CoolTime"));
		if (pFoundUI == nullptr)
			return;

		m_SkillCoolTimes[4] = pFoundUI;
	}

	{
		shared_ptr<CUICoolTime> pFoundUI = dynamic_pointer_cast<CUICoolTime>(UIMGR->FindUI("QuickMainSlot4CoolTime"));
		if (pFoundUI == nullptr)
			return;

		m_SkillCoolTimes[5] = pFoundUI;
	}
}

void CItemMgr::BindCurrentWeaponIndex(_uint _iWeaponIndex)
{
	m_iWeaponIndex = _iWeaponIndex;
}

HRESULT CItemMgr::CreateEquipItem(CRAFT_SET_TAG _eCraftTag)
{
	switch (_eCraftTag)
	{
	case SET_PLATE_1:
	{
		/* 필요 없음(?) */
		/*shared_ptr<CItemBase> pRequiredItem0 = FindItem(ITEM_MISC, "Craft Item0");
		shared_ptr<CItemBase> pRequiredItem1 = FindItem(ITEM_MISC, "Craft Item1");
		shared_ptr<CItemBase> pRequiredItem2 = FindItem(ITEM_MISC, "Craft Item2");
		shared_ptr<CItemBase> pRequiredItem3 = FindItem(ITEM_MISC, "Craft Item3");
		shared_ptr<CItemBase> pRequiredItem4 = FindItem(ITEM_MISC, "Craft Item4");

		if (pRequiredItem0 && pRequiredItem1 && pRequiredItem2 && pRequiredItem3 && pRequiredItem4)
		{*/
			{ /* Helmet */
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "지옥 인도자의 면갑";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_HELMET;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_set_PL_M_HM_00019";
				itemDesc.strModelTag = "Helmet_Plate_1";
				itemDesc.iValue = 5;
				shared_ptr<CItemEquip> Parts_Helmet = CItemEquip::Create(itemDesc);
				if (Parts_Helmet == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Parts_Helmet);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "지옥 인도자의 갑옷";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_UPPER;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_set_PL_M_TS_00019";
				itemDesc.strModelTag = "Upper_Plate_1";
				itemDesc.iValue = 10;
				shared_ptr<CItemEquip> Parts_Upper0 = CItemEquip::Create(itemDesc);
				if (Parts_Upper0 == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Parts_Upper0);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "지옥 인도자의 각반";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_set_PL_M_PT_00019";
				itemDesc.strModelTag = "Lower_Plate_1";
				itemDesc.iValue = 8;
				shared_ptr<CItemEquip> Parts_Lower = CItemEquip::Create(itemDesc);
				if (Parts_Lower == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Parts_Lower);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "지옥 인도자의 망토";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_CAPE;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_M_CA_00019";
				itemDesc.strModelTag = "Cape_Plate_1";
				itemDesc.iValue = 3;
				shared_ptr<CItemEquip> Parts_Lower = CItemEquip::Create(itemDesc);
				if (Parts_Lower == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Parts_Lower);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "지옥 인도자의 장갑";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_set_PL_M_GL_00019";
				itemDesc.strModelTag = "Glove_Plate_1";
				itemDesc.iValue = 3;
				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "지옥 인도자의 장화";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_set_PL_M_BT_00019";
				itemDesc.strModelTag = "Boots_Plate_1";
				itemDesc.iValue = 5;
				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}
		//}
	}
	break;

	case SET_PLATE_0:
	{
		/*shared_ptr<CItemBase> pRequiredItem0 = FindItem(ITEM_MISC, "Craft Item0");
		shared_ptr<CItemBase> pRequiredItem1 = FindItem(ITEM_MISC, "Craft Item1");
		shared_ptr<CItemBase> pRequiredItem2 = FindItem(ITEM_MISC, "Craft Item2");
		shared_ptr<CItemBase> pRequiredItem3 = FindItem(ITEM_MISC, "Craft Item3");
		shared_ptr<CItemBase> pRequiredItem4 = FindItem(ITEM_MISC, "Craft Item4");

		if (pRequiredItem0 && pRequiredItem1 && pRequiredItem2 && pRequiredItem3 && pRequiredItem4)
		{*/
		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "특급 저항군 맹금두건";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_HELMET;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_LE_M_HM_05002";
			itemDesc.strModelTag = "Helmet_Leather_2";
			itemDesc.iValue = 2;

			shared_ptr<CItemEquip> part = CItemEquip::Create(itemDesc);
			if (part == nullptr)
				return E_FAIL;

			ITEMMGR->AddItemToInventory(part);
		}

			{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "특급 저항군 맹금갑옷";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_UPPER;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_LE_M_TS_05002";
			itemDesc.strModelTag = "Upper_Leather_2";
			itemDesc.iValue = 5;

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "특급 저항군 맹금바지";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_Set_LE_M_PT_05002";
				itemDesc.strModelTag = "Lower_Leather_2";
				itemDesc.iValue = 3;

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "특급 저항군 맹금장갑";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_Set_LE_M_GL_05002";
				itemDesc.strModelTag = "Glove_Leather_2";
				itemDesc.iValue = 1;

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "특급 저항군 맹금장화";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_Set_LE_M_BT_05002";
				itemDesc.strModelTag = "Boots_Leather_2";
				itemDesc.iValue = 2;

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}
		//}
	}
	break;
	case SET_PLATE_2:
	{
		/*shared_ptr<CItemBase> pRequiredItem0 = FindItem(ITEM_MISC, "Craft Item0");
		shared_ptr<CItemBase> pRequiredItem1 = FindItem(ITEM_MISC, "Craft Item1");
		shared_ptr<CItemBase> pRequiredItem2 = FindItem(ITEM_MISC, "Craft Item2");
		shared_ptr<CItemBase> pRequiredItem3 = FindItem(ITEM_MISC, "Craft Item3");
		shared_ptr<CItemBase> pRequiredItem4 = FindItem(ITEM_MISC, "Craft Item4");

		if (pRequiredItem0 && pRequiredItem1 && pRequiredItem2 && pRequiredItem3 && pRequiredItem4)
		{*/
			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "Helmet_Plate_3";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_HELMET;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_Set_PL_M_HM_00016";
				itemDesc.strModelTag = "Helmet_Plate_3";

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "Upper_Plate_3";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_UPPER;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_Set_PL_M_TS_00016";
				itemDesc.strModelTag = "Upper_Plate_3";

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "Lower_Plate_3";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_Set_PL_M_PT_00016";
				itemDesc.strModelTag = "Lower_Plate_3";

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "Glove_Plate_3";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_Set_PL_M_GL_00018";
				itemDesc.strModelTag = "Glove_Plate_3";

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "Boots_Plate_3";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_Set_PL_M_BT_00016";
				itemDesc.strModelTag = "Boots_Plate_3";

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}
		//}
	}
	break;
	case SET_PLATE_3:
	{
		/*shared_ptr<CItemBase> pRequiredItem0 = FindItem(ITEM_MISC, "Craft Item0");
		shared_ptr<CItemBase> pRequiredItem1 = FindItem(ITEM_MISC, "Craft Item1");
		shared_ptr<CItemBase> pRequiredItem2 = FindItem(ITEM_MISC, "Craft Item2");
		shared_ptr<CItemBase> pRequiredItem3 = FindItem(ITEM_MISC, "Craft Item3");
		shared_ptr<CItemBase> pRequiredItem4 = FindItem(ITEM_MISC, "Craft Item4");

		if (pRequiredItem0 && pRequiredItem1 && pRequiredItem2 && pRequiredItem3 && pRequiredItem4)
		{*/
			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "Helmet_Fabric_1";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_HELMET;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_Set_FA_M_HM_00014";
				itemDesc.strModelTag = "Helmet_Fabric_1";

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "Upper_Fabric_1";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_UPPER;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_Set_FA_M_TS_00014";
				itemDesc.strModelTag = "Upper_Fabric_1";

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "Lower_Fabric_1";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_Set_FA_M_PT_00014";
				itemDesc.strModelTag = "Lower_Fabric_1";

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "Glove_Fabric_1";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_Set_FA_M_GL_00014";
				itemDesc.strModelTag = "Glove_Fabric_1";

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;

				ITEMMGR->AddItemToInventory(Part);
			}

			{
				CItemBase::ItemDesc itemDesc{};
				itemDesc.strItemTag = "Boots_Fabric_1";
				itemDesc.eItemType = ITEM_EQUIP;
				itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
				itemDesc.numItem = 1;
				itemDesc.strTexTag = "P_Set_FA_M_BT_00014";
				itemDesc.strModelTag = "Boots_Fabric_1";

				shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
				if (Part == nullptr)
					return E_FAIL;
				
				ITEMMGR->AddItemToInventory(Part);
			}
		//}
	}
	break;
	}

	return S_OK;
}

shared_ptr<CItemBase> CItemMgr::FindItem(ITEM_TYPE _eItemType, string _strItemTag)
{
	vector<shared_ptr<CItemBase>>* pSlots;
	pSlots = GetItemSlots(_eItemType);
	if (pSlots == nullptr)
		return nullptr;

	string strItemTag = _strItemTag;

	auto it = find_if((*pSlots).begin(), (*pSlots).end(), [&strItemTag](shared_ptr<CItemBase> pItem) {
		if (pItem)
		{
			if (pItem->GetItemTag() == strItemTag)
				return true;

			return false;
		}
		
		return false;
		});

	if (it == (*pSlots).end())
		return nullptr;

	return *it;
}

void CItemMgr::AddItemToInventory(shared_ptr<CItemBase> _pItem)
{
	if (_pItem != nullptr)
	{
		switch (_pItem->GetItemType())
		{
		case ITEM_EQUIP:
		{
			/*for (auto& pItemSlot : m_InventorySlots[ITEM_EQUIP])
			{
				if (pItemSlot != nullptr && pItemSlot->IsEmpty())
				{
					pItemSlot->AddItemToSlot(_pItem);

					break;
				}
			}*/
			shared_ptr<CUIInstanceItemSlot> pUISlot = dynamic_pointer_cast<CUIInstanceItemSlot>(UIMGR->FindUI("InvEquipSlot0"));
			if (pUISlot == nullptr)
				return;

			pUISlot->AddItemToSlot(_pItem);
		}
		break;
		case ITEM_USABLE:
		{
			/*for (auto& pItemSlot : m_InventorySlots[ITEM_USABLE])
			{
				if (pItemSlot != nullptr && pItemSlot->IsEmpty())
				{
					pItemSlot->AddItemToSlot(_pItem);

					break;
				}
			}*/
			shared_ptr<CUIInstanceItemSlot> pUISlot = dynamic_pointer_cast<CUIInstanceItemSlot>(UIMGR->FindUI("UsableSlot0"));
			if (pUISlot == nullptr)
				return;

			pUISlot->AddItemToSlot(_pItem);
		}
		break;
		case ITEM_MISC:
		{
			/*for (auto& pItemSlot : m_InventorySlots[ITEM_MISC])
			{
				if (pItemSlot != nullptr && pItemSlot->IsEmpty())
				{
					pItemSlot->AddItemToSlot(_pItem);

					break;
				}
			}*/
			shared_ptr<CUIInstanceItemSlot> pUISlot = dynamic_pointer_cast<CUIInstanceItemSlot>(UIMGR->FindUI("MiscSlot0"));
			if (pUISlot == nullptr)
				return;

			if (_pItem)
			{
				if (_pItem->GetItemTag() == "석궁" || _pItem->GetItemTag() == "지팡이" || _pItem->GetItemTag() == "반지")
					break;
			}

			pUISlot->AddItemToSlot(_pItem);
		}
		break;
		}
	}
}

void CItemMgr::AddItemByIndex(_uint _iIndex, shared_ptr<CItemBase> _pItem)
{
	if (_pItem != nullptr)
	{
		switch (_pItem->GetItemType())
		{
		case ITEM_EQUIP:
		{
			/*shared_ptr<CUISlot> pClickedSlot = m_InventorySlots[ITEM_EQUIP][_iIndex];
			if (pClickedSlot != nullptr)
				pClickedSlot->AddItemToSlot(_pItem);*/

			shared_ptr<CUIInstanceItemSlot> pUISlot = dynamic_pointer_cast<CUIInstanceItemSlot>(UIMGR->FindUI("InvEquipSlot0"));
			if (pUISlot == nullptr)
				return;

			pUISlot->AddItemToSlotByIndex(_iIndex, _pItem);
		}
		break;
		case ITEM_USABLE:
		{
			/*shared_ptr<CUISlot> pClickedSlot = m_InventorySlots[ITEM_USABLE][_iIndex];
			if (pClickedSlot != nullptr)
				pClickedSlot->AddItemToSlot(_pItem);*/

			shared_ptr<CUIInstanceItemSlot> pUISlot = dynamic_pointer_cast<CUIInstanceItemSlot>(UIMGR->FindUI("UsableSlot0"));
			if (pUISlot == nullptr)
				return;

			pUISlot->AddItemToSlotByIndex(_iIndex, _pItem);
		}
		break;
		case ITEM_MISC:
		{
			/*shared_ptr<CUISlot> pClickedSlot = m_InventorySlots[ITEM_MISC][_iIndex];
			if (pClickedSlot != nullptr)
				pClickedSlot->AddItemToSlot(_pItem);*/

			shared_ptr<CUIInstanceItemSlot> pUISlot = dynamic_pointer_cast<CUIInstanceItemSlot>(UIMGR->FindUI("MiscSlot0"));
			if (pUISlot == nullptr)
				return;

			pUISlot->AddItemToSlotByIndex(_iIndex, _pItem);
		}
		break;
		}
	}
}

void CItemMgr::AddItemToQuickSlot(shared_ptr<CItemBase> _pItem)
{
	if (_pItem != nullptr)
	{
		for (auto& pQuickSlot : m_QuickSlots)
		{
			if (pQuickSlot != nullptr && pQuickSlot->IsEmpty())
			{
				pQuickSlot->AddItemToSlot(_pItem);

				break;
			}
		}
	}
}

void CItemMgr::AddItemToQuickSlotByIndex(_uint _iIndex, shared_ptr<CItemBase> _pItem)
{
	if (_pItem != nullptr)
	{
		shared_ptr<CUISlot> pClickedSlot = m_QuickSlots[_iIndex];
		if (pClickedSlot != nullptr)
			pClickedSlot->AddItemToSlot(_pItem);
	}
}

void CItemMgr::DeleteCertainItem(ITEM_TYPE _eItemType, string _strItemTag)
{
	vector<shared_ptr<CItemBase>>* pSlots = GetItemSlots(_eItemType);

	for (auto& pItem : *pSlots)
	{
		if (pItem != nullptr)
		{
			if (pItem->GetItemTag() == _strItemTag)
			{
				pItem.reset();
				pItem = nullptr;
			}
		}
	}
}

_uint CItemMgr::GetItemCount(ITEM_TYPE _eItemType, string _strItemTag)
{
	vector<shared_ptr<CItemBase>>* pSlots = GetItemSlots(_eItemType);

	_uint iItemCount{ 0 };

	for (auto& pItem : *pSlots)
	{
		if (pItem != nullptr)
		{
			if (pItem->GetItemTag() == _strItemTag)
				iItemCount += pItem->GetNumItem();
		}
	}

	return iItemCount;
}

_bool CItemMgr::DecreaseItemFromInventory(ITEM_TYPE _eItemType, string _strItemTag, _uint _numItem)
{
	vector<shared_ptr<CItemBase>>* pSlots = GetItemSlots(_eItemType);

	_uint iItemCount = GetItemCount(_eItemType, _strItemTag);

	_int iNumAfterCraft = iItemCount - _numItem;

	if (iNumAfterCraft < 0)
		return false;

	if (iNumAfterCraft == 0)
	{
		DeleteCertainItem(_eItemType, _strItemTag);

		return true;
	}

	if (iNumAfterCraft > 0)
	{
		_int iDecreaseAmount{ 0 };

		for (auto& pItem : *pSlots)
		{
			if (pItem == nullptr)
				continue;

			if (pItem->GetItemTag() == _strItemTag)
			{
				/*if (iDecreaseAmount < 0)
				{
					iDecreaseAmount = pItem->DecreaseNumItem(-iDecreaseAmount);

					if (iDecreaseAmount == 1)
						return true;

					else if (iDecreaseAmount == 0)
					{
						pItem.reset();
						pItem = nullptr;

						return true;
					}

					else if (iDecreaseAmount < 0)
					{
						pItem.reset();
						pItem = nullptr;
					}
				}

				else if (iDecreaseAmount == 0)
				{*/
					iDecreaseAmount = pItem->DecreaseNumItem(_numItem);

					if (iDecreaseAmount == 1)
						return true;

					else if (iDecreaseAmount == 0)
					{
						pItem.reset();
						pItem = nullptr;

						return true;
					}

					else if (iDecreaseAmount < 0)
					{
						//pItem.reset();
						//pItem = nullptr;
					}
				//}
			}
		}
	}

	return false;
}

_bool CItemMgr::CanDecreaseItemFromInventory(ITEM_TYPE _eItemType, string _strItemTag, _uint _numItem)
{
	vector<shared_ptr<CItemBase>>* pSlots = GetItemSlots(_eItemType);

	_uint iItemCount = GetItemCount(_eItemType, _strItemTag);

	_int iNumAfterCreate = iItemCount - _numItem;

	if (iNumAfterCreate < 0)
		return false;

	if (iNumAfterCreate > 0)
		return true;

	if (iNumAfterCreate == 0)
		return true;

	return false;
}

_bool CItemMgr::IncreaseMoney(_uint iMoney)
{
	m_iMoney += iMoney;

	return true;
}

_bool CItemMgr::DecreaseMoney(_uint iMoney)
{
	_int iAfter = m_iMoney - iMoney;
	if (iAfter < 0)
		return false;

	m_iMoney = iAfter;

	return true;
}

void CItemMgr::EquipItem(ITEM_EQUIP_TYPE _eItemEquipType, const string& _strModelKey)
{
	if (!m_pPlayer.expired())
	{
		ResetItemEquippedState(_eItemEquipType);
		m_pPlayer.lock()->ChangeEquipment(_eItemEquipType, _strModelKey);
	}
}

void CItemMgr::TakeOffItem(ITEM_EQUIP_TYPE _eItemEquipType)
{
	if (!m_pPlayer.expired() && (_eItemEquipType == ITEM_EQUIP_HELMET || _eItemEquipType == ITEM_EQUIP_CAPE))
	{
		ResetItemEquippedState(_eItemEquipType);
		m_pPlayer.lock()->NoneEquipment(_eItemEquipType);
	}
}

void CItemMgr::ResetItemEquippedState(ITEM_EQUIP_TYPE _eItemEquipType)
{
	/*for (auto& pUISlot : m_InventorySlots[ITEM_EQUIP])
	{
		if (pUISlot && pUISlot->GetSlotItem())
		{
			if (pUISlot->GetSlotItem()->GetItemEquipType() == _eItemEquipType)
				pUISlot->GetSlotItem()->SetIsEquipped(false);
		}
	}*/

	shared_ptr<CUIInstanceItemSlot> pUISlot = dynamic_pointer_cast<CUIInstanceItemSlot>(UIMGR->FindUI("InvEquipSlot0"));
	if (pUISlot == nullptr)
		return;

	vector<shared_ptr<CItemBase>> pItemSlots = pUISlot->GetItemSlots();
	for (auto& pSlot : pItemSlots)
	{
		if (pSlot)
		{
			if (pSlot->GetItemEquipType() == _eItemEquipType)
				pSlot->SetIsEquipped(false);
		}
	}
}

void CItemMgr::UseHPPotion(_float _fValue)
{
	if (!m_pPlayer.expired())
	{
		m_pPlayer.lock()->SetPlayerHp(_fValue);
	}
}

void CItemMgr::PrepareUsableItem()
{
	CItemBase::ItemDesc itemDesc{};
	itemDesc.strItemTag = "대형 회복 물약";
	itemDesc.eItemType = ITEM_USABLE;
	itemDesc.eItemUsableType = ITEM_USABLE_HP;
	itemDesc.strTexTag = "I_Health_Potion_3";
	itemDesc.strModelTag = "";
	itemDesc.numItem = 10;
	itemDesc.iValue = 1000;
	itemDesc.iPrice = 200;
	itemDesc.wstrItemDescription = L"";
	
	shared_ptr<CItemUsable> hpPotion = CItemUsable::Create(itemDesc);
	if (hpPotion == nullptr)
		return;

	AddItemToInventory(hpPotion);
}

void CItemMgr::PrepareMiscItem()
{
	{
		CItemBase::ItemDesc itemDesc{};
		itemDesc.strItemTag = "Craft Item0";
		itemDesc.eItemType = ITEM_MISC;
		itemDesc.strTexTag = "I_Enchant_Armor_KAA_001";
		itemDesc.strModelTag = "";
		itemDesc.numItem = 1;

		shared_ptr<CItemMisc> pItemMisc = CItemMisc::Create(itemDesc);
		if (pItemMisc == nullptr)
			return;

		AddItemToInventory(pItemMisc);
	}

	{
		CItemBase::ItemDesc itemDesc{};
		itemDesc.strItemTag = "Craft Item1";
		itemDesc.eItemType = ITEM_MISC;
		itemDesc.strTexTag = "I_AmmoMaterial_4";
		itemDesc.strModelTag = "";
		itemDesc.numItem = 1;

		shared_ptr<CItemMisc> pItemMisc = CItemMisc::Create(itemDesc);
		if (pItemMisc == nullptr)
			return;

		AddItemToInventory(pItemMisc);
	}

	{
		CItemBase::ItemDesc itemDesc{};
		itemDesc.strItemTag = "Craft Item2";
		itemDesc.eItemType = ITEM_MISC;
		itemDesc.strTexTag = "I_Basilisk_001";
		itemDesc.strModelTag = "";
		itemDesc.numItem = 1;

		shared_ptr<CItemMisc> pItemMisc = CItemMisc::Create(itemDesc);
		if (pItemMisc == nullptr)
			return;

		AddItemToInventory(pItemMisc);
	}

	{
		CItemBase::ItemDesc itemDesc{};
		itemDesc.strItemTag = "Craft Item3";
		itemDesc.eItemType = ITEM_MISC;
		itemDesc.strTexTag = "I_Claw_001";
		itemDesc.strModelTag = "";
		itemDesc.numItem = 1;

		shared_ptr<CItemMisc> pItemMisc = CItemMisc::Create(itemDesc);
		if (pItemMisc == nullptr)
			return;

		AddItemToInventory(pItemMisc);
	}

	{
		CItemBase::ItemDesc itemDesc{};
		itemDesc.strItemTag = "Craft Item4";
		itemDesc.eItemType = ITEM_MISC;
		itemDesc.strTexTag = "I_Core_002";
		itemDesc.strModelTag = "";
		itemDesc.numItem = 1;

		shared_ptr<CItemMisc> pItemMisc = CItemMisc::Create(itemDesc);
		if (pItemMisc == nullptr)
			return;

		AddItemToInventory(pItemMisc);
	}
}

void CItemMgr::PrepareCraftItem()
{
	{
		CItemBase::ItemDesc itemDesc0{};
		itemDesc0.strItemTag = "방어구 제작석";
		itemDesc0.eItemType = ITEM_MISC;
		itemDesc0.strTexTag = "I_Enchant_Armor_KAA_001";
		itemDesc0.strModelTag = "";
		itemDesc0.numItem = 4;

		shared_ptr<CItemMisc> pItemMisc0 = CItemMisc::Create(itemDesc0);

		ITEMMGR->AddItemToInventory(pItemMisc0);
	}

	{
		CItemBase::ItemDesc itemDesc1{};
		itemDesc1.strItemTag = "날카로운 발톱";
		itemDesc1.eItemType = ITEM_MISC;
		itemDesc1.strTexTag = "I_Claw_001";
		itemDesc1.strModelTag = "";
		itemDesc1.numItem = 2;

		shared_ptr<CItemMisc> pItemMisc1 = CItemMisc::Create(itemDesc1);

		ITEMMGR->AddItemToInventory(pItemMisc1);
	}


	{
		CItemBase::ItemDesc itemDesc{};
		itemDesc.strItemTag = "단단한 부리";
		itemDesc.eItemType = ITEM_MISC;
		itemDesc.eItemUsableType = ITEM_USABLE_END;
		itemDesc.strTexTag = "I_Basilisk_001";
		itemDesc.iValue = 50;
		itemDesc.numItem = 1;
		itemDesc.iPrice = 50;

		shared_ptr<CItemMisc> pItemMisc2 = CItemMisc::Create(itemDesc);

		ITEMMGR->AddItemToInventory(pItemMisc2);
	}

	{
		CItemBase::ItemDesc itemDesc{};
		itemDesc.strItemTag = "마력석 조각";
		itemDesc.eItemType = ITEM_MISC;
		itemDesc.eItemUsableType = ITEM_USABLE_END;
		itemDesc.strTexTag = "I_AmmoMaterial_4";
		itemDesc.iValue = 300;
		itemDesc.numItem = 1;
		itemDesc.iPrice = 300;

		shared_ptr<CItemMisc> pItemMisc3 = CItemMisc::Create(itemDesc);

		ITEMMGR->AddItemToInventory(pItemMisc3);
	}

	{
		CItemBase::ItemDesc itemDesc{};
		itemDesc.strItemTag = "온전한 마력석";
		itemDesc.eItemType = ITEM_MISC;
		itemDesc.eItemUsableType = ITEM_USABLE_END;
		itemDesc.strTexTag = "I_Core_002";
		itemDesc.iValue = 500;
		itemDesc.numItem = 1;
		itemDesc.iPrice = 500;

		shared_ptr<CItemMisc> pItemMisc4 = CItemMisc::Create(itemDesc);

		ITEMMGR->AddItemToInventory(pItemMisc4);
	}
}
