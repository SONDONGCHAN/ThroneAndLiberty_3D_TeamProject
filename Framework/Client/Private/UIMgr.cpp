#include "UIMgr.h"
#include "UIBar.h"
#include "UICoolTime.h"
#include "UIIcon.h"
#include "UISlider.h"
#include "UIButton.h"
#include "UISlot.h"
#include "UIScroll.h"
#include "UIScrollBG.h"
#include "UIUV.h"
#include "UIPanel.h"
#include "UIBackGround.h"
#include "UIItemBackGround.h"
#include "UIHover.h"
#include "UICover.h"
#include "UIItemBackGround.h"
#include "UIItemHover.h"
#include "UIItemIcon.h"
#include "UIItemTab.h"
#include "UILockOn.h"
#include "UIInstanceItemBG.h"
#include "UIInstanceItemSlot.h"
#include "UIInstanceItemIcon.h"
#include "UIInstanceItemCoolTime.h"
#include "UIInstanceItemHover.h"
#include "UIInstanceQuickBG.h"
#include "UIInstanceQuickSlot.h"
#include "UIInstanceParring.h"
#include "UIInstanceAbnormalSlot.h"
#include "UIInstanceWorldHP.h"
#include "UIInstanceBossHP.h"
#include "UINormalText.h"
#include "UIInstanceBossAbnormal.h"
#include "UIInstanceQuest.h"
#include "UIInstanceIndicator.h"
#include "UIInstanceInteraction.h"
#include "UICrossHair.h"
#include "UIInstanceDialogue.h"
#include "UICraftBG.h"
#include "UICraftHover.h"
#include "UICraftSlot.h"
#include "UICraftTab.h"
#include "UIInstanceWorldNPC.h"
#include "UITradeBG.h"
#include "UITradeSlot.h"
#include "UITradeTab.h"
#include "UITradeHover.h"
#include "UIQuestComplete.h"
#include "UIItemToolTip.h"
#include "UISpeechBallon.h"
#include "UITargetIndicator.h"
#include "UIInteractionStatue.h"
#include "UIStarForce.h"
#include "UIOptionBG.h"
#include "UIOptionTab.h"
#include "UIOptionHover.h"
#include "UIOptionSlider.h"
#include "UIOptionCheckBox.h"
#include "UIAmountAden.h"
#include "UICraftResult.h"
#include "UITutorial.h"
#include "UIInteractionPopUp.h"
#include "UICountDown.h"
#include "UIMapTitle.h"
#include "UISpriteEffect.h"
#include "UIUVBar.h"
#include "Monster.h"
#include "Player.h"
#include "CameraMgr.h"
#include "Midget_Berserker.h"
#include "Midget_Berserker_Shadow.h"
#include "Orc_DarkCommander.h"
#include "Orc_DarkCommander_Shadow.h"
#include "RedKnight.h"
#include "RedKnight_Shadow.h"
#include "NPC.h"
#include "Polymorph.h"
#include "CharacterMgr.h"
#include "ItemMgr.h"
#include "Catapult.h"

#include "GameInstance.h"
#include "Transform.h"
#include "VIInstancing.h"
#include "GameMgr.h"


#include "fstream"
#include "filesystem"

IMPLEMENT_SINGLETON(Client::CUIMgr)

CUIMgr::CUIMgr()
{
}

CUIMgr::~CUIMgr()
{
	m_pHUD.reset();

	for (auto& ui : m_UIs)
	{
		if (ui.second != nullptr)
			ui.second.reset();
	}

	m_UIs.clear();
}

HRESULT CUIMgr::Initialize()
{
	if (FAILED(PrepareBindFuncs()))
		return E_FAIL;

	if (FAILED(PrepareMouseSlot()))
		return E_FAIL;

	if (FAILED(PrepareCrossHair()))
		return E_FAIL;

	if (FAILED(PrepareAdenAmount()))
		return E_FAIL;

	if (FAILED(PrepareCountDown()))
		return E_FAIL;

	if (FAILED(PrepareMapTitle()))
		return E_FAIL;

	if (FAILED(PrepareSpriteEffects()))
		return E_FAIL;

	DeactivateMouseCursor();
	RenderOffMouseCursor();
	DeactivateCraftResultEffect();
	DeactivateInteractionPopUp();
	weak_ptr<CUIPanel> pQuestComplete = dynamic_pointer_cast<CUIPanel>(FindUI("QuestCompleteBGs"));
	if (!pQuestComplete.expired())
		pQuestComplete.lock()->SetIsRender(false);

	m_pQuestNPCs.resize(static_cast<_uint>(QUEST_END));
	m_bIsQuestNPCActivated.resize(static_cast<_uint>(QUEST_END));

	return S_OK;
}

void CUIMgr::Tick(_float _fDeltaTime)
{
	if (m_bIsLoadingDone)
	{
		KeyInput();

		if (m_pHUD != nullptr)
			m_pHUD->Tick(_fDeltaTime);

		FollowLockOnTarget();
		UpdateParryingStack();
		UpdateSpeechBallonPos();

		DeactivateNPCIndicator(QUEST_MAIN);
		DeactivateNPCIndicator(QUEST_SUB0);
		DeactivateNPCIndicator(QUEST_SUB1);
	}
	
	if (m_bIsLoadingDone)
	{
		if (!m_pQuestNPCs.empty())
		{
			if (!m_pQuestNPCs[QUEST_MAIN].expired() && m_bIsQuestNPCActivated[QUEST_MAIN])
				UpdateMainNPCIndicator();

			if (!m_pQuestNPCs[QUEST_SUB0].expired() && m_bIsQuestNPCActivated[QUEST_SUB0])
				UpdateSub0NPCIndicator();

			if (!m_pQuestNPCs[QUEST_SUB1].expired() && m_bIsQuestNPCActivated[QUEST_SUB1])
				UpdateSub1NPCIndicator();
		}
	}

	for (auto& pUI : m_UIVec)
	{
		if (!pUI.expired())
		{
			if (pUI.lock()->IsLoadingDone())
				pUI.lock()->Tick(_fDeltaTime);
		}
	}

	if (m_bIsLoadingDone)
	{
		SortUIZOrder();

		if (m_pHUD != nullptr)
			m_pHUD->LateTick(_fDeltaTime);
	}
	

	for (auto& pUI : m_UIVec)
	{
		if (!pUI.expired())
		{
			if (pUI.lock()->IsLoadingDone())
				pUI.lock()->LateTick(_fDeltaTime);
		}
	}

	if (m_bIsLoadingDone)
	{
		DeactivateInteractionUI();

		weak_ptr<CUIBackGround> pUI = dynamic_pointer_cast<CUIBackGround>(FindUI("InventoryPanel"));
		if (!pUI.expired())
		{
			if (!pUI.lock()->IsActivated())
				DeactivateItemToolTip();
		}

		weak_ptr<CUIPanel> pTrade = dynamic_pointer_cast<CUIPanel>(FindUI("TradeAllPanel"));
		if (!pTrade.expired())
		{
			if (!pTrade.lock()->IsActivated())
				DeactivateTradeItemToolTip();
		}
	}
	
	/*m_fTimeAcc += _fDeltaTime;
	if (m_fTimeAcc >= 3.f)
	{
		POINT mousePt;
		::GetCursorPos(&mousePt);
		::ScreenToClient(g_hWnd, &mousePt);

		_float mousePosX = static_cast<_float>(mousePt.x) - (g_iWinSizeX - 1.f) * 0.5f;
		_float mousePosY = static_cast<_float>(-mousePt.y) + (g_iWinSizeY + 1.f) * 0.5f;

		console("UIMgr Mouse X : %f, Mouse Y : %f \n", mousePosX, mousePosY);

		m_fTimeAcc = 0.f;
	}*/

	/* 활성화 된 UI들 중 겹칠 경우 ZOrder를 처리하는 함수 필요 */
	/* 필요...한가? */
}

void CUIMgr::SetHUD(const string& _strUITag)
{
	shared_ptr<CUIBase> pUIHUD = FindUI(_strUITag);

	if (pUIHUD == nullptr)
		return;

	m_pHUD = pUIHUD;
}

void CUIMgr::SetUISize(_float _fSizeX, _float _fSizeY)
{
	if (m_pLatestActivatedUI.lock() != nullptr && m_pLatestActivatedUI.lock() != m_pHUD)
		m_pLatestActivatedUI.lock()->SetBufferSize(_fSizeX, _fSizeY);
}

function<void(void)> CUIMgr::GetBindFunc(const string& _funcTag)
{
	return m_bindFuncs[_funcTag];
}

_int CUIMgr::GetPlayerCurAnimation()
{
	if (!m_pPlayer.expired())
	{
		return static_cast<_int>(m_pPlayer.lock()->GetCurrentAnimation());
	}

	return -1;
}

_bool CUIMgr::IsCrossBowBuffed()
{
	if (!m_pPlayer.expired())
		return m_pPlayer.lock()->IsCrossBowBuffed();

	return false;
}

void CUIMgr::SetLockOnTarget(shared_ptr<CGameObject> _lockOnTarget)
{
	m_pLockOnTarget = _lockOnTarget;
}

void CUIMgr::SetLockOnIcon(_bool _isLockedOn)
{
	shared_ptr<CUIBase> lockOnIcon = FindUI("LockOnIcon");
	if (lockOnIcon == nullptr)
		return;

	if (_isLockedOn)
		lockOnIcon->SetTexIndex(1);
	else
		lockOnIcon->SetTexIndex(0);
}

void CUIMgr::BindPlayer(shared_ptr<CPlayer> _pPlayer)
{
	m_pPlayer = _pPlayer;
}

void CUIMgr::SetCurNPC(shared_ptr<CNPC> _pNPC)
{
	m_pNPC = _pNPC;
}

void CUIMgr::SetCurQuestNPC(QUEST_TYPE _eQuestType, shared_ptr<CNPC> _pNPC)
{
	m_pQuestNPCs[_eQuestType] = _pNPC;
}

void CUIMgr::SetSpeechBallonContent(const wstring& _strNPCName, const wstring& _strContent)
{
	shared_ptr<CUISpeechBallon> pUI = static_pointer_cast<CUISpeechBallon>(FindUI("SpeechBallonInstance"));
	if (pUI != nullptr)
		pUI->UpdateSpeechBallon(_strNPCName, _strContent);
}

void CUIMgr::SetInteractionPopUpContent(const wstring& _strContent)
{
	shared_ptr<CUIInteractionPopUp> pUI = dynamic_pointer_cast<CUIInteractionPopUp>(FindUI("InteractionPopUpInstance"));
	if (pUI == nullptr)
		return;

	pUI->SetDisplayContent(_strContent);
}

_bool CUIMgr::GetIsStartSignal()
{
	shared_ptr<CUICountDown> pUI = dynamic_pointer_cast<CUICountDown>(FindUI("CountDown"));
	if (pUI == nullptr)
		return false;

	return pUI->GetStartSignal();
}

void CUIMgr::SetMapTitle(_uint _iLevel)
{
	shared_ptr<CUIMapTitle> pUI = dynamic_pointer_cast<CUIMapTitle>(FindUI("MapTitle"));
	if (pUI == nullptr)
		return;

	switch (_iLevel)
	{
	case LEVEL_VILLAGE:
	{
		pUI->SetDisplayContent(L"케슬러 마을");
	}
	break;
	case LEVEL_FONOS:
	{
		pUI->SetDisplayContent(L"포노스 분지");
	}
	break;
	case LEVEL_DUNGEON_MAIN:
	{
		pUI->SetDisplayContent(L"던전 상층");
	}
	break;
	case LEVEL_DUNGEON_FINAL:
	{
		pUI->SetDisplayContent(L"던전 하층");
	}
	break;
	case 10:
	{
		pUI->SetDisplayContent(L"흰곰자리 들판");
	}
	break;
	}
}

HRESULT CUIMgr::AddUI(const string& _strUITag, shared_ptr<CUIBase> _pUI)
{
	shared_ptr<CUIBase> foundUI = FindUI(_strUITag);
	if (foundUI != nullptr)
		return E_FAIL;

	m_UIs.emplace(_strUITag, _pUI);

	return S_OK;
}

HRESULT CUIMgr::AddUIAfterSwapTag(const string& _strUITag, shared_ptr<CUIBase> _pUI)
{
	if (_pUI == nullptr)
		return E_FAIL;

	m_UIs.emplace(_strUITag, _pUI);

	return S_OK;
}

shared_ptr<CUIBase> CUIMgr::FindUI(const string& _strUITag)
{
	auto it = m_UIs.find(_strUITag);
	if (it == m_UIs.end())
		return nullptr;

	return it->second;
}

HRESULT CUIMgr::AddUIChild(const string& _strUITag, const string& _strChildTag)
{
	shared_ptr<CUIBase> foundUI = FindUI(_strUITag);
	if (foundUI == nullptr)
		return E_FAIL;

	shared_ptr<CUIBase> foundChildUI = FindUI(_strChildTag);
	if (foundChildUI == nullptr)
		return E_FAIL;

	if (foundUI->GetUIType() == UI_SCROLLBG && foundChildUI->GetUIType() == UI_SCROLL)
	{
		shared_ptr<CUIScroll> foundScroll = dynamic_pointer_cast<CUIScroll>(foundChildUI);
		shared_ptr<CUIScrollBG> pUIBG = dynamic_pointer_cast<CUIScrollBG>(foundUI);
		pUIBG->SetScrollUI(foundScroll);
		foundScroll->SetUIBG(pUIBG);
		foundScroll->SetVisibleCenterAndSizeY(foundUI->GetUIPos(), foundUI->GetUISize().y);
	}

	if (foundUI->GetUIType() == UI_PANEL && foundChildUI->GetUIType() == UI_ITEMBG)
	{
		shared_ptr<CUIPanel> foundParentPanel = dynamic_pointer_cast<CUIPanel>(foundUI);
		shared_ptr<CUIItemBackGround> foundChildItemBG = dynamic_pointer_cast<CUIItemBackGround>(foundChildUI);
		SortChildUIs(foundParentPanel, foundChildItemBG);
	}

	if (foundChildUI->GetUIType() == UI_SCROLLBG)
	{
		shared_ptr<CUIScrollBG> pUIBG = dynamic_pointer_cast<CUIScrollBG>(foundChildUI);
		pUIBG->SetVisibleRange();
	}

	foundUI->AddUIChild(foundChildUI->GetUITag(), foundChildUI);
	foundChildUI->AddUIParent(foundUI);

	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&foundChildUI](weak_ptr<CUIBase> pUI) {
		if (!pUI.expired() && (pUI.lock()->GetUITag() == foundChildUI->GetUITag()))
			return true;

		return false;
		});

	if (it != m_UIVec.end())
	{
		it->reset();
		m_UIVec.erase(it);
	}

	return S_OK;
}

HRESULT CUIMgr::AddUIParent(const string& _strUITag, const string& _strParentTag)
{
	shared_ptr<CUIBase> foundUI = FindUI(_strUITag);
	if (foundUI == nullptr)
		return E_FAIL;

	shared_ptr<CUIBase> foundParentUI = FindUI(_strParentTag);
	if (foundParentUI == nullptr)
		return E_FAIL;

	foundUI->AddUIParent(foundParentUI); // 부모UI는 하나밖에 존재할 수 없다

	return S_OK;
}

HRESULT CUIMgr::AddUIChild(const string& _strChildUITag)
{
	shared_ptr<CUIBase> foundChildUI = FindUI(_strChildUITag);
	if (foundChildUI == nullptr)
		return E_FAIL;

	if (m_pLatestActivatedUI.lock()->GetUIType() == UI_SCROLLBG && foundChildUI->GetUIType() == UI_SCROLL)
	{
		shared_ptr<CUIScroll> foundScroll = dynamic_pointer_cast<CUIScroll>(foundChildUI);
		shared_ptr<CUIScrollBG> pUIBG = dynamic_pointer_cast<CUIScrollBG>(m_pLatestActivatedUI.lock());
		pUIBG->SetScrollUI(foundScroll);
		foundScroll->SetUIBG(pUIBG);
		foundScroll->SetVisibleCenterAndSizeY(m_pLatestActivatedUI.lock()->GetUIPos(), m_pLatestActivatedUI.lock()->GetUISize().y);
	}

	if (m_pLatestActivatedUI.lock()->GetUIType() == UI_PANEL && foundChildUI->GetUIType() == UI_ITEMBG)
	{
		shared_ptr<CUIPanel> foundParentPanel = dynamic_pointer_cast<CUIPanel>(m_pLatestActivatedUI.lock());
		shared_ptr<CUIItemBackGround> foundChildItemBG = dynamic_pointer_cast<CUIItemBackGround>(foundChildUI);
		SortChildUIs(foundParentPanel, foundChildItemBG);
	}

	if (foundChildUI->GetUIType() == UI_SCROLLBG)
	{
		shared_ptr<CUIScrollBG> pUIBG = dynamic_pointer_cast<CUIScrollBG>(foundChildUI);
		pUIBG->SetVisibleRange();
	}

	m_pLatestActivatedUI.lock()->AddUIChild(foundChildUI->GetUITag(), foundChildUI);
	foundChildUI->AddUIParent(m_pLatestActivatedUI.lock());

	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&foundChildUI](weak_ptr<CUIBase> pUI) {
		if (!pUI.expired() && (pUI.lock()->GetUITag() == foundChildUI->GetUITag()))
			return true;

		return false;
		});

	if (it != m_UIVec.end())
	{
		it->reset();
		m_UIVec.erase(it);
	}

	return S_OK;
}

HRESULT CUIMgr::AddUIParent(const string& _strParentTag)
{
	shared_ptr<CUIBase> foundParentUI = FindUI(_strParentTag);
	if (foundParentUI == nullptr)
		return E_FAIL;

	m_pLatestActivatedUI.lock()->AddUIParent(foundParentUI);
	foundParentUI->AddUIChild(m_pLatestActivatedUI.lock()->GetUITag(), m_pLatestActivatedUI.lock());
	weak_ptr<CUIBase> tempPtr = m_pLatestActivatedUI;
	//m_pLatestActivatedUI = foundParentUI;
	ActivateUI(foundParentUI->GetUITag());

	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&tempPtr](weak_ptr<CUIBase> pUI) {
		if (!pUI.expired() && (pUI.lock()->GetUITag() == tempPtr.lock()->GetUITag()))
			return true;

		return false;
		});

	if (it != m_UIVec.end())
	{
		it->reset();
		m_UIVec.erase(it);
	}

	tempPtr.reset();

	return S_OK;
}

HRESULT CUIMgr::RemoveUIChild(const string& _strUITag, const string& _strChildUITag)
{
	shared_ptr<CUIBase> pFoundUI = FindUI(_strUITag);
	if (pFoundUI == nullptr)
		return E_FAIL;

	shared_ptr<CUIBase> pFoundChildUI = FindUI(_strChildUITag);
	if (pFoundChildUI == nullptr)
		return E_FAIL;

	pFoundUI->RemoveUIChild(_strChildUITag);
	pFoundChildUI->RemoveUIParent();

	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&pFoundChildUI](weak_ptr<CUIBase> pUI) {
		if (!pUI.expired() && (pUI.lock()->GetUITag() == pFoundChildUI->GetUITag()))
			return true;

		return false;
		});

	if (it == m_UIVec.end())
		ActivateUI(pFoundChildUI->GetUITag());

	return S_OK;
}

void CUIMgr::ActivateUI(const string& _strUITag)
{
	shared_ptr<CUIBase> foundUI = FindUI(_strUITag);
	if (foundUI == nullptr)
		return;

	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&foundUI](weak_ptr<CUIBase> pUI) {
		if (foundUI == pUI.lock())
			return true;

		return false;
		});

	if (it == m_UIVec.end())
		m_UIVec.emplace_back(foundUI);

	/* 여기에서 ZOrder Sorting을 한 번 해줄 필요가 있음 */
	SortUIZOrder();

	foundUI->ActivateUI();

	m_pLatestActivatedUI = foundUI;
}

void CUIMgr::DeactivateUI(const string& _strUITag)
{
	if (m_UIVec.empty())
		return;

	shared_ptr<CUIBase> foundUI = FindUI(_strUITag);
	if (foundUI == nullptr)
		return;

	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&foundUI](weak_ptr<CUIBase> pUIBase) {
		if (!pUIBase.expired())
		{
			if (foundUI == pUIBase.lock())
				return true;

			return false;
		}

		return false;
		});

	if (it == m_UIVec.end())
		return;

	foundUI->DeactivateUI();

	if (it != m_UIVec.end())
	{
		if (!it->expired())
			m_UIVec.erase(it);
	}

	/* 여기에서 ZOrder Sorting을 한 번 해줄 필요가 있음 */
	SortUIZOrder();

	if (m_UIVec.size() >= 1)
		m_pLatestActivatedUI = m_UIVec.back();
	else
		m_pLatestActivatedUI.reset();
}

void CUIMgr::SelectUI(const string& _strUITag)
{
	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&_strUITag](weak_ptr<CUIBase> pUI) {
		if (!pUI.expired())
		{
			if (pUI.lock()->GetUITag() == _strUITag)
				return true;

			return false;
		}

		return false;
		});

	if (it != m_UIVec.end())
	{
		m_pLatestActivatedUI = it->lock();

		return;
	}

	ActivateUI(_strUITag);
}

void CUIMgr::DeleteSelectedUI(const string& _strUITag)
{
	DeactivateUI(_strUITag);

	auto it = m_UIs.find(_strUITag);
	if (it == m_UIs.end())
		return;

	if (!it->second->GetChildrenList().empty())
	{
		unordered_map<string, weak_ptr<CUIBase>> childrenList = it->second->GetChildrenList();
		for (auto& childPair : childrenList)
		{
			DeleteSelectedUI(childPair.first);
		}
	}

	it->second.reset();

	m_UIs.erase(it);
}

void CUIMgr::ActivateCrossHair()
{
	shared_ptr<CUIBase> pUI = FindUI("CrossHair");
	if (pUI == nullptr)
		return;

	ActivateUI("CrossHair");

	pUI->SetIsRender(true);
}

void CUIMgr::DeactivateCrossHair()
{
	shared_ptr<CUIBase> pUI = FindUI("CrossHair");
	if (pUI == nullptr)
		return;

	DeactivateUI("CrossHair");

	pUI->SetIsRender(false);
}

void CUIMgr::ActivateInteractionUI()
{
	shared_ptr<CUIBase> pUI = FindUI("InteractionPanel");
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(true);
}

void CUIMgr::DeactivateInteractionUI()
{
	shared_ptr<CUIBase> pUI = FindUI("InteractionPanel");
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(false);
}

void CUIMgr::ActivateItemToolTip()
{
	shared_ptr<CUIBase> pUI = FindUI("ToolTipABGs");
	if (pUI == nullptr)
		return;

	ActivateUI("ToolTipABGs");
}

void CUIMgr::DeactivateItemToolTip()
{
	shared_ptr<CUIBase> pUI = FindUI("ToolTipABGs");
	if (pUI == nullptr)
		return;

	shared_ptr<CUIItemToolTip> pItemToolTip = dynamic_pointer_cast<CUIItemToolTip>(FindUI("ToolTipInstance"));
	if (pItemToolTip == nullptr)
		return;

	DeactivateUI("ToolTipABGs");
}

void CUIMgr::ActivateSpeechBallon()
{
	shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("SpeechBallonBGs"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(true);

	ActivateUI("SpeechBallonBGs");
}

void CUIMgr::DeactivateSpeechBallon()
{
	shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("SpeechBallonBGs"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(false);

	DeactivateUI("SpeechBallonBGs");
}

void CUIMgr::ActivateTargetIndicator()
{
	shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("TargetIndicatorPanel"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(true);
}

void CUIMgr::DeactivateTargetIndicator()
{
	shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("TargetIndicatorPanel"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(false);
}

void CUIMgr::ActivateNPCIndicator(QUEST_TYPE _eQuestType)
{
	switch (_eQuestType)
	{
	case QUEST_MAIN:
	{
		shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("TargetIndicatorPanel_0"));
		if (pUI == nullptr)
			return;

		pUI->SetIsRender(true);
	}
	break;
	case QUEST_SUB0:
	{
		shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("TargetIndicatorPanel_1"));
		if (pUI == nullptr)
			return;

		pUI->SetIsRender(true);
	}
	break;
	case QUEST_SUB1:
	{
		shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("TargetIndicatorPanel_2"));
		if (pUI == nullptr)
			return;

		pUI->SetIsRender(true);
	}
	break;
	}
}

void CUIMgr::DeactivateNPCIndicator(QUEST_TYPE _eQuestType)
{
	switch (_eQuestType)
	{
	case QUEST_MAIN:
	{
		shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("TargetIndicatorPanel_0"));
		if (pUI == nullptr)
			return;

		pUI->SetIsRender(false);

		weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("MainQuestIndicatorPanel"));
		if (pUIPanel.expired())
			return;

		pUIPanel.lock()->SetIsRender(false);
	}
	break;
	case QUEST_SUB0:
	{
		shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("TargetIndicatorPanel_1"));
		if (pUI == nullptr)
			return;

		pUI->SetIsRender(false);

		weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("SubQuestIndicatorPanel0"));
		if (pUIPanel.expired())
			return;

		pUIPanel.lock()->SetIsRender(false);
	}
	break;
	case QUEST_SUB1:
	{
		shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("TargetIndicatorPanel_2"));
		if (pUI == nullptr)
			return;

		pUI->SetIsRender(false);

		weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("SubQuestIndicatorPanel1"));
		if (pUIPanel.expired())
			return;

		pUIPanel.lock()->SetIsRender(false);
	}
	break;
	}
}

void CUIMgr::ActivateInteractionStatue()
{
	shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("RotationArrowPanel"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(true);
}

void CUIMgr::DeactivateInteractionStatue()
{
	shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("RotationArrowPanel"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(false);
}

void CUIMgr::ActivateOption()
{
	shared_ptr<CUIBase> foundUI = FindUI("OptionAllPanel");
	if (foundUI != nullptr)
	{
		if (foundUI->IsActivated() == false)
		{
			ActivateUI("OptionAllPanel");
			DeactivateHUD();
			CGameMgr::GetInstance()->ChangeToFreeMouse(0);
		}
	}
}

void CUIMgr::DeactivateOption()
{
	shared_ptr<CUIBase> foundUI = FindUI("OptionAllPanel");
	if (foundUI != nullptr)
	{
		if (foundUI->IsActivated() != false)
		{
			DeactivateUI("OptionAllPanel");
			ActivateHUD();
			CGameMgr::GetInstance()->ChangeToNormalMouse();
		}
	}
}

void CUIMgr::ActivateStarForce()
{
	shared_ptr<CUIBase> foundUI = FindUI("StarForceBGPanel");
	if (foundUI != nullptr)
	{
		ActivateUI("StarForceBGPanel");
		//DeactivateHUD();
		//if (!m_pPlayer.expired())
		//	m_pPlayer.lock()->SetInputState(true);
		//CGameMgr::GetInstance()->ChangeToFreeMouse();
	}
}

void CUIMgr::DeactivateStarForce()
{
	shared_ptr<CUIBase> foundUI = FindUI("StarForceBGPanel");
	if (foundUI != nullptr)
	{
		weak_ptr<CUIStarForce> pUI = dynamic_pointer_cast<CUIStarForce>(FindUI("StarForceBGs"));
		if (!pUI.expired())
			pUI.lock()->SetIsPressed(false);
		DeactivateUI("StarForceBGPanel");
		//ActivateHUD();
		//if (!m_pPlayer.expired())
		//	m_pPlayer.lock()->SetInputState(false);
		//CGameMgr::GetInstance()->ChangeToNormalMouse();
	}
}

void CUIMgr::ActivateCraft()
{
	shared_ptr<CUIBase> foundUI = FindUI("CraftAllPanel");
	if (foundUI != nullptr)
	{
		ActivateUI("CraftAllPanel");
		DeactivateHUD();
		CGameMgr::GetInstance()->ChangeToFreeMouse(0);
	}
}

void CUIMgr::DeactivateCraft()
{
	shared_ptr<CUIBase> foundUI = FindUI("CraftAllPanel");
	if (foundUI != nullptr)
	{
		DeactivateUI("CraftAllPanel");
		ActivateHUD();

		CGameMgr::GetInstance()->ChangeToNormalMouse();
	}
}

void CUIMgr::ActivateCraftResult()
{
	shared_ptr<CUIPanel> foundUI = dynamic_pointer_cast<CUIPanel>(FindUI("CraftResultPanel"));
	if (foundUI != nullptr)
	{
		foundUI->SetIsRender(true);
		foundUI->SetIsFailed(false);

		ActivateUI("CraftResultPanel");
	}
}

void CUIMgr::DeactivateCraftResult()
{
	shared_ptr<CUIPanel> foundUI = dynamic_pointer_cast<CUIPanel>(FindUI("CraftResultPanel"));
	if (foundUI != nullptr)
		foundUI->SetIsRender(false);

	DeactivateUI("CraftResultPanel");
}

void CUIMgr::ActivateFailedCraftResult()
{
	shared_ptr<CUIPanel> foundUI = dynamic_pointer_cast<CUIPanel>(FindUI("CraftResultPanel"));
	if (foundUI != nullptr)
	{
		foundUI->SetIsRender(true);
		foundUI->SetIsFailed(true);
	}
}

void CUIMgr::DeactivateFailedCraftResult()
{
	shared_ptr<CUIPanel> foundUI = dynamic_pointer_cast<CUIPanel>(FindUI("CraftResultPanel"));
	if (foundUI != nullptr)
		foundUI->SetIsRender(false);
}

void CUIMgr::ActivateLockOnTarget()
{
	shared_ptr<CUIPanel> foundUI = dynamic_pointer_cast<CUIPanel>(FindUI("LockOnPanel"));
	if (foundUI != nullptr)
	{
		foundUI->SetIsRender(true);
		m_bIsLockOnRender = true;
	}
}

void CUIMgr::DeactivateLockOnTarget()
{
	shared_ptr<CUIPanel> foundUI = dynamic_pointer_cast<CUIPanel>(FindUI("LockOnPanel"));
	if (foundUI != nullptr)
	{
		foundUI->SetIsRender(false);
		m_bIsLockOnRender = false;
	}
}

void CUIMgr::ActivateTradeItemToolTip()
{
	shared_ptr<CUIBase> pUI = FindUI("ToolTipABGs_0");
	if (pUI == nullptr)
		return;

	ActivateUI("ToolTipABGs_0");
}

void CUIMgr::DeactivateTradeItemToolTip()
{
	shared_ptr<CUIBase> pUI = FindUI("ToolTipABGs_0");
	if (pUI == nullptr)
		return;

	DeactivateUI("ToolTipABGs_0");
}

void CUIMgr::ActivateLoading(_uint _iTexIndex)
{
	shared_ptr<CUIBase> pUI = FindUI("LoadingBGs");
	if (pUI == nullptr)
		return;

	shared_ptr<CUIBackGround> pBG = dynamic_pointer_cast<CUIBackGround>(FindUI("LoadingBG0"));
	if (pBG == nullptr)
		return;

	if (_iTexIndex == 2)
	{
		pBG->SetTexIndex(3);
	}

	else if (_iTexIndex == 3)
	{
		pBG->SetTexIndex(4);
	}

	else if (_iTexIndex == 4)
	{
		pBG->SetTexIndex(1);
	}

	else if (_iTexIndex == 5)
	{
		pBG->SetTexIndex(2);
	}

	pUI->SetIsRender(true);

	ActivateUI("LoadingBGs");
}

void CUIMgr::DeactivateLoading()
{
	shared_ptr<CUIBase> pUI = FindUI("LoadingBGs");
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(false);

	DeactivateUI("LoadingBGs");
}

void CUIMgr::ActivateCatapultUI()
{
	shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("InstanceParringPanel_0"));
	if (pUI == nullptr)
		return;
	
	m_bIsCatapultActivated = true;

	pUI->SetIsRender(true);

	ActivateUI("InstanceParringPanel_0");
}

void CUIMgr::DeactivateCatapultUI()
{
	shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("InstanceParringPanel_0"));
	if (pUI == nullptr)
		return;

	m_bIsCatapultActivated = false;

	pUI->SetIsRender(false);

	DeactivateUI("InstanceParringPanel_0");
}

void CUIMgr::ActivateCatapultTutorial()
{
	shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("TutorialPanel"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(true);

	ActivateUI("TutorialPanel");
}

void CUIMgr::DeactivateCatapultTutorial()
{
	shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("TutorialPanel"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(false);

	DeactivateUI("TutorialPanel");
}

void CUIMgr::ActivateInteractionPopUp()
{
	shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("InteractionPopUpPanel"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(true);

	ActivateUI("InteractionPopUpPanel");
	GAMEINSTANCE->PlaySoundW("Interation_Success02", 0.6f);
}

void CUIMgr::DeactivateInteractionPopUp()
{
	shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(FindUI("InteractionPopUpPanel"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(false);

	DeactivateUI("InteractionPopUpPanel");
}

void CUIMgr::ActivateCountDown()
{
	shared_ptr<CUICountDown> pUI = dynamic_pointer_cast<CUICountDown>(FindUI("CountDown"));
	if (pUI == nullptr)
		return;

	pUI->ResetStartSignal();
	pUI->SetIsRender(true);

	ActivateUI("CountDown");
}

void CUIMgr::UpdateCountDown(_float _fRemainTime)
{
	shared_ptr<CUICountDown> pUI = dynamic_pointer_cast<CUICountDown>(FindUI("CountDown"));
	if (pUI == nullptr)
		return;

	pUI->UpdateRemainTime(_fRemainTime);
}

void CUIMgr::UpdateNumFound(_int _iCurFound, _int _iMaxFound)
{
	shared_ptr<CUICountDown> pUI = dynamic_pointer_cast<CUICountDown>(FindUI("CountDown"));
	if (pUI == nullptr)
		return;

	pUI->UpdateNumFound(_iCurFound, _iMaxFound);
}

void CUIMgr::DeactivateCountDown()
{
	shared_ptr<CUICountDown> pUI = dynamic_pointer_cast<CUICountDown>(FindUI("CountDown"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(false);

	DeactivateUI("CountDown");
}

void CUIMgr::ActivateMapTitle()
{
	shared_ptr<CUIMapTitle> pUI = dynamic_pointer_cast<CUIMapTitle>(FindUI("MapTitle"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(true);

	ActivateUI("MapTitle");
}

void CUIMgr::DeactivateMapTitle()
{
	shared_ptr<CUIMapTitle> pUI = dynamic_pointer_cast<CUIMapTitle>(FindUI("MapTitle"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(false);

	DeactivateUI("MapTitle");
}

void CUIMgr::ActivateMouseCursor(_float2 _vPos)
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("CursorDust"));
	if (pUI == nullptr)
		return;

	pUI->ResetTimer();
	pUI->SetIsLoop(false);
	pUI->SetFollowPos(_vPos);

	ActivateUI("CursorDust");
}

void CUIMgr::DeactivateMouseCursor()
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("CursorDust"));
	if (pUI == nullptr)
		return;

	DeactivateUI("CursorDust");
}

void CUIMgr::RenderOnMouseCursor()
{
	shared_ptr<CUISlot> pUI = dynamic_pointer_cast<CUISlot>(FindUI("MouseSlot"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(true);

	ActivateUI("MouseSlot");
}

void CUIMgr::RenderOffMouseCursor()
{
	shared_ptr<CUISlot> pUI = dynamic_pointer_cast<CUISlot>(FindUI("MouseSlot"));
	if (pUI == nullptr)
		return;

	pUI->SetIsRender(false);

	DeactivateUI("MouseSlot");
}

void CUIMgr::ActivateCraftResultEffect()
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("CraftResultEffect"));
	if (pUI == nullptr)
		return;

	if (!pUI->IsPlaying())
	{
		pUI->ResetTimer();
		pUI->SetIsLoop(false);
		pUI->SetFollowPos({ 10.f, 10.f });

		ActivateUI("CraftResultEffect");

		shared_ptr<CUICraftResult> pCraftResult = dynamic_pointer_cast<CUICraftResult>(FindUI("CraftResultInstance"));
		if (pCraftResult)
		{
			GAMEINSTANCE->StopSound("UI_Text_upgrade_Ready_01_A");

			if (pCraftResult->GetSuccessType() == 0)
				GAMEINSTANCE->PlaySoundW("UI_Text_upgrade_Success_01_A", 1.f);
			else
				GAMEINSTANCE->PlaySoundW("UI_Text_upgrade_Fail_01_D", 1.f);
		}
	}
}

void CUIMgr::DeactivateCraftResultEffect()
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("CraftResultEffect"));
	if (pUI == nullptr)
		return;

	DeactivateUI("CraftResultEffect");
}

void CUIMgr::ActivateQuestPanelEffect(QUEST_TYPE _eQuestType)
{
	shared_ptr<CUIPanel> pPanel = dynamic_pointer_cast<CUIPanel>(FindUI("DialoguePanel"));
	if (pPanel == nullptr)
		return;

	switch (_eQuestType)
	{
	case QUEST_MAIN:
	{
		shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("MainQuestPanelEffect"));
		if (pUI == nullptr)
			return;

		if (!pUI->IsPlaying() && !pPanel->IsRender())
		{
			pUI->ResetTimer();
			pUI->SetIsLoop(true);
			pUI->SetFollowPos({ 512.f, 35.f });

			ActivateUI("MainQuestPanelEffect");
		}
	}
	break;
	case QUEST_SUB0:
	{
		shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest0PanelEffect"));
		if (pUI == nullptr)
			return;

		if (!pUI->IsPlaying() && !pPanel->IsRender())
		{
			pUI->ResetTimer();
			pUI->SetIsLoop(true);
			pUI->SetFollowPos({ 512.f, -59.f });

			ActivateUI("SubQuest0PanelEffect");
		}
	}
	break;
	case QUEST_SUB1:
	{
		shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest1PanelEffect"));
		if (pUI == nullptr)
			return;

		if (!pUI->IsPlaying() && !pPanel->IsRender())
		{
			pUI->ResetTimer();
			pUI->SetIsLoop(true);
			pUI->SetFollowPos({ 512.f, -153.f });

			ActivateUI("SubQuest1PanelEffect");
		}
	}
	break;
	}
	
}

void CUIMgr::DeactivateQuestPanelEffect(QUEST_TYPE _eQuestType)
{
	switch (_eQuestType)
	{
	case QUEST_MAIN:
	{
		shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("MainQuestPanelEffect"));
		if (pUI == nullptr)
			return;

		pUI->SetIsPlaying(false);

		DeactivateUI("MainQuestPanelEffect");
	}
	break;
	case QUEST_SUB0:
	{
		shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest0PanelEffect"));
		if (pUI == nullptr)
			return;

		pUI->SetIsPlaying(false);

		DeactivateUI("SubQuest0PanelEffect");
	}
	break;
	case QUEST_SUB1:
	{
		shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest1PanelEffect"));
		if (pUI == nullptr)
			return;

		pUI->SetIsPlaying(false);

		DeactivateUI("SubQuest1PanelEffect");
	}
	break;
	}
	
}

void CUIMgr::ActivateQuestCompleteSlotEffect()
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("QuestCompleteSlotEffect"));
	if (pUI == nullptr)
		return;

	if (!pUI->IsPlaying())
	{
		pUI->ResetTimer();
		pUI->SetIsLoop(true);
		//pUI->SetFollowPos({ 512.f, -153.f });

		ActivateUI("QuestCompleteSlotEffect");
	}
}

void CUIMgr::DeactivateQuestCompleteSlotEffect()
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("QuestCompleteSlotEffect"));
	if (pUI == nullptr)
		return;

	pUI->SetIsPlaying(false);

	DeactivateUI("QuestCompleteSlotEffect");
}

void CUIMgr::ActivateQuestCompleteEffect()
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("QuestCompleteEffect"));
	if (pUI == nullptr)
		return;

	if (!pUI->IsPlaying())
	{
		pUI->ResetTimer();
		pUI->SetIsLoop(false);
		//pUI->SetFollowPos({ 512.f, -153.f });

		ActivateUI("QuestCompleteEffect");
	}
}

void CUIMgr::DeactivateQuestCompleteEffect()
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("QuestCompleteEffect"));
	if (pUI == nullptr)
		return;

	pUI->SetIsPlaying(false);

	DeactivateUI("QuestCompleteEffect");
}

void CUIMgr::ActivateTradeSlotBGEffect(_float2 _vPos)
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("TradeSlotBGEffect"));
	if (pUI == nullptr)
		return;

	shared_ptr<CUITradeBG> pUISlotBG = dynamic_pointer_cast<CUITradeBG>(FindUI("TradeBSlotBGs"));
	if (pUISlotBG == nullptr)
		return;

	if (!pUI->IsPlaying())
	{
		pUI->ResetTimer();
		pUI->SetIsLoop(false);
		pUI->SetFollowPos({ _vPos });

		ActivateUI("TradeSlotBGEffect");
	}
}

void CUIMgr::DeactivateTradeSlotBGEffect()
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("TradeSlotBGEffect"));
	if (pUI == nullptr)
		return;

	pUI->SetIsPlaying(false);

	DeactivateUI("TradeSlotBGEffect");
}

void CUIMgr::ActivateCoolTimeEffect(_float2 _vPos)
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("CoolTimeEffect"));
	if (pUI == nullptr)
		return;

	if (!pUI->IsPlaying())
	{
		pUI->ResetTimer();
		pUI->SetIsLoop(false);
		pUI->SetFollowPos({ _vPos });

		ActivateUI("CoolTimeEffect");
	}
}

void CUIMgr::DeactivateCoolTimeEffect()
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("CoolTimeEffect"));
	if (pUI == nullptr)
		return;

	pUI->SetIsPlaying(false);

	DeactivateUI("CoolTimeEffect");
}

void CUIMgr::ActivateQuestIconEffect(QUEST_TYPE _questType)
{
	shared_ptr<CUIPanel> pPanel = dynamic_pointer_cast<CUIPanel>(FindUI("DialoguePanel"));
	if (pPanel == nullptr)
		return;

	switch (_questType)
	{
	case QUEST_MAIN:
	{
		shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("MainQuestIconEffect"));
		if (pUI == nullptr)
			return;

		if (!pUI->IsPlaying() && !pPanel->IsRender())
		{
			pUI->ResetTimer();
			pUI->SetIsLoop(true);
			pUI->SetFollowPos({ 404.f, 78.f });

			ActivateUI("MainQuestIconEffect");
		}
	}
	break;
	case QUEST_SUB0:
	{
		shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest0IconEffect"));
		if (pUI == nullptr)
			return;

		if (!pUI->IsPlaying() && !pPanel->IsRender())
		{
			pUI->ResetTimer();
			pUI->SetIsLoop(true);
			pUI->SetFollowPos({ 404.f, -17.f });

			ActivateUI("SubQuest0IconEffect");
		}
	}
	break;
	case QUEST_SUB1:
	{
		shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest1IconEffect"));
		if (pUI == nullptr)
			return;

		if (!pUI->IsPlaying() && !pPanel->IsRender())
		{
			pUI->ResetTimer();
			pUI->SetIsLoop(true);
			pUI->SetFollowPos({ 404.f, -111.f });

			ActivateUI("SubQuest1IconEffect");
		}
	}
	break;
	}
}

void CUIMgr::DeactivateQuestIconEffect(QUEST_TYPE _questType)
{
	switch (_questType)
	{
	case QUEST_MAIN:
	{
		shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("MainQuestIconEffect"));
		if (pUI == nullptr)
			return;

		pUI->SetIsPlaying(false);

		DeactivateUI("MainQuestIconEffect");
	}
	break;
	case QUEST_SUB0:
	{
		shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest0IconEffect"));
		if (pUI == nullptr)
			return;

		pUI->SetIsPlaying(false);

		DeactivateUI("SubQuest0IconEffect");
	}
	break;
	case QUEST_SUB1:
	{
		shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest1IconEffect"));
		if (pUI == nullptr)
			return;

		pUI->SetIsPlaying(false);

		DeactivateUI("SubQuest1IconEffect");
	}
	break;
	}
}

void CUIMgr::ActivateQuestCompleteIconEffect()
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("QuestCompleteIconEffect"));
	if (pUI == nullptr)
		return;

	if (!pUI->IsPlaying())
	{
		pUI->ResetTimer();
		pUI->SetIsLoop(true);
		pUI->SetFollowPos({ 0.f, 202.5 });

		ActivateUI("QuestCompleteIconEffect");
	}
}

void CUIMgr::DeactivateQuestCompleteIconEffect()
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("QuestCompleteIconEffect"));
	if (pUI == nullptr)
		return;

	pUI->SetIsPlaying(false);

	DeactivateUI("QuestCompleteIconEffect");
}

void CUIMgr::ActivateCertainNPCIndicator(_float2 _vPos)
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("CertainNPCIndicator"));
	if (pUI == nullptr)
		return;

	if (!pUI->IsPlaying())
	{
		pUI->ResetTimer();
		pUI->SetIsLoop(true);
		//pUI->SetFollowPos({ _vPos });

		ActivateUI("CertainNPCIndicator");
	}
}

void CUIMgr::DeactivateCertainNPCIndicator()
{
	shared_ptr<CUISpriteEffect> pUI = dynamic_pointer_cast<CUISpriteEffect>(FindUI("CertainNPCIndicator"));
	if (pUI == nullptr)
		return;

	pUI->SetIsPlaying(false);

	DeactivateUI("CertainNPCIndicator");
}

void CUIMgr::ActivateHUD()
{
	m_bIsHUDOn = true;

	shared_ptr<CUIBase> pUIPCInfo = FindUI("PCInfo");
	if (pUIPCInfo == nullptr)
		return;

	pUIPCInfo->SetIsRender(true);

	shared_ptr<CUIBase> pUIQuick = FindUI("QuickSlotPanel");
	if (pUIQuick == nullptr)
		return;

	pUIQuick->SetIsRender(true);

	shared_ptr<CUIBase> pUIQuest = FindUI("QuestPanel");
	if (pUIQuest == nullptr)
		return;

	pUIQuest->SetIsRender(true);

	shared_ptr<CUIAmountAden> pUIAdenAmount = dynamic_pointer_cast<CUIAmountAden>(FindUI("AdenAmount"));
	if (pUIAdenAmount == nullptr)
		return;

	pUIAdenAmount->SetIsRender(true);

	ActivateInteractionUI();

	shared_ptr<CUISpriteEffect> pUISprite1 = dynamic_pointer_cast<CUISpriteEffect>(FindUI("MainQuestPanelEffect"));
	if (pUISprite1 == nullptr)
		return;

	pUISprite1->SetIsRender(true);

	shared_ptr<CUISpriteEffect> pUISprite2 = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest0PanelEffect"));
	if (pUISprite2 == nullptr)
		return;

	pUISprite2->SetIsRender(true);

	shared_ptr<CUISpriteEffect> pUISprite3 = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest1PanelEffect"));
	if (pUISprite3 == nullptr)
		return;

	pUISprite3->SetIsRender(true);

	shared_ptr<CUISpriteEffect> pUISprite4 = dynamic_pointer_cast<CUISpriteEffect>(FindUI("MainQuestIconEffect"));
	if (pUISprite4 == nullptr)
		return;

	pUISprite4->SetIsRender(true);

	shared_ptr<CUISpriteEffect> pUISprite5 = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest0IconEffect"));
	if (pUISprite5 == nullptr)
		return;

	pUISprite5->SetIsRender(true);

	shared_ptr<CUISpriteEffect> pUISprite6 = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest1IconEffect"));
	if (pUISprite6 == nullptr)
		return;

	pUISprite6->SetIsRender(true);
}

void CUIMgr::DeactivateHUD()
{
	m_bIsHUDOn = false;

	shared_ptr<CUIBase> pUIPCInfo = FindUI("PCInfo");
	if (pUIPCInfo == nullptr)
		return;

	pUIPCInfo->SetIsRender(false);

	shared_ptr<CUIBase> pUIQuick = FindUI("QuickSlotPanel");
	if (pUIQuick == nullptr)
		return;

	pUIQuick->SetIsRender(false);

	shared_ptr<CUIBase> pUIQuest = FindUI("QuestPanel");
	if (pUIQuest == nullptr)
		return;

	pUIQuest->SetIsRender(false);

	shared_ptr<CUIAmountAden> pUIAdenAmount = dynamic_pointer_cast<CUIAmountAden>(FindUI("AdenAmount"));
	if (pUIAdenAmount == nullptr)
		return;

	pUIAdenAmount->SetIsRender(false);

	DeactivateInteractionUI();

	shared_ptr<CUISpriteEffect> pUISprite1 = dynamic_pointer_cast<CUISpriteEffect>(FindUI("MainQuestPanelEffect"));
	if (pUISprite1 == nullptr)
		return;

	pUISprite1->SetIsRender(false);

	shared_ptr<CUISpriteEffect> pUISprite2 = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest0PanelEffect"));
	if (pUISprite2 == nullptr)
		return;

	pUISprite2->SetIsRender(false);

	shared_ptr<CUISpriteEffect> pUISprite3 = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest1PanelEffect"));
	if (pUISprite3 == nullptr)
		return;

	pUISprite3->SetIsRender(false);

	shared_ptr<CUISpriteEffect> pUISprite4 = dynamic_pointer_cast<CUISpriteEffect>(FindUI("MainQuestIconEffect"));
	if (pUISprite4 == nullptr)
		return;

	pUISprite4->SetIsRender(false);

	shared_ptr<CUISpriteEffect> pUISprite5 = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest0IconEffect"));
	if (pUISprite5 == nullptr)
		return;

	pUISprite5->SetIsRender(false);

	shared_ptr<CUISpriteEffect> pUISprite6 = dynamic_pointer_cast<CUISpriteEffect>(FindUI("SubQuest1IconEffect"));
	if (pUISprite6 == nullptr)
		return;

	pUISprite6->SetIsRender(false);
}

void CUIMgr::UITickOn()
{
	m_bIsLoadingDone = true;

	for (auto& pUI : m_UIs)
	{
		if (pUI.second)
		{
			pUI.second->SetIsLoadingDone(true);
		}
	}

	/*for (auto& pUI : m_UIVec)
	{
		if (!pUI.expired())
			pUI.lock()->SetIsLoadingDone(true);
	}*/
}

void CUIMgr::UITickOff()
{
	m_bIsLoadingDone = false;

	for (auto& pUI : m_UIs)
	{
		if (pUI.second)
		{
			if (pUI.second->GetUITag() == "LoadingBGs" || pUI.second->GetUITag() == "LoadingBG0" ||
				pUI.second->GetUITag() == "LoadingBG1")
				continue;

			pUI.second->SetIsLoadingDone(false);
		}
	}

	/*for (auto& pUI : m_UIVec)
	{
		if (!pUI.expired())
		{
			if (pUI.lock()->GetUITag() == "LoadingBGs" || pUI.lock()->GetUITag() == "LoadingBG0" ||
				pUI.lock()->GetUITag() == "LoadingBG1")
				continue;

			pUI.lock()->SetIsLoadingDone(false);
		}
	}*/
}

void CUIMgr::FollowLockOnTarget()
{
	if (!m_pLockOnTarget.expired() && !m_bIsCameraDirecting)
	{
		shared_ptr<CUIBase> lockOnPanel = FindUI("LockOnPanel");
		if (lockOnPanel == nullptr)
			return;

		shared_ptr<CMonster> targetMonster = dynamic_pointer_cast<CMonster>(m_pLockOnTarget.lock());
		_float3 playerPos = m_pPlayer.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float3 monsterPos = targetMonster->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float dist = SimpleMath::Vector3::Distance(playerPos, monsterPos);

		lockOnPanel->SetIsRender(true);

		_float4x4 followWorldMat{};
		_float3 followPos{};

		if (targetMonster->GetModelName() == "Puppet")
		{
			followWorldMat = targetMonster->GetTransform()->GetWorldMatrix();
			followPos = followWorldMat.Translation() + _float3(0.f, 1.5f, 0.f);
		}

		else
		{
			followWorldMat = targetMonster->GetCombinedBoneMat("Bip001") * targetMonster->GetTransform()->GetWorldMatrix();
			followPos = followWorldMat.Translation();
		}
		
		followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
		followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

		/*if (dist > 20.f)
		{
			UpdateTargetIndicator();
		}
		else
		{
			DeactivateTargetIndicator();
		}*/

		if (followPos.z <= 0.f || followPos.z > 1.f)
		{
			//followPos.x = 100000.f;
			//followPos.y = 100000.f;

			lockOnPanel->SetIsRender(false);

			//UpdateTargetIndicator();
		}

		_float posX = (followPos.x + 1.f) * 0.5f;
		_float posY = (followPos.y - 1.f) * -0.5f;

		posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
		posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

		/* LockOnUI 찾아서 Pos Set */
		lockOnPanel->SetUIPos(posX, posY);
	}
	else
	{
		shared_ptr<CUIBase> lockOnPanel = FindUI("LockOnPanel");
		if (lockOnPanel == nullptr)
			return;

		lockOnPanel->SetIsRender(false);

		DeactivateTargetIndicator();
	}
}

void CUIMgr::UpdateParryingStack()
{
	if (!m_pLockOnTarget.expired() && !m_bIsCameraDirecting)
	{
		shared_ptr<CUIInstanceBossAbnormal> pBossAbnormal = dynamic_pointer_cast<CUIInstanceBossAbnormal>(FindUI("BossAbnormalSlot0"));
		pBossAbnormal->ResetStates();

		if (dynamic_pointer_cast<CMonster>(m_pLockOnTarget.lock())->GetModelName() == "M_L1Midget_Berserker")
		{
			weak_ptr<CMidget_Berserker> pTarget = dynamic_pointer_cast<CMidget_Berserker>(m_pLockOnTarget.lock());
			shared_ptr<CUIInstanceParring> pParryingStack = dynamic_pointer_cast<CUIInstanceParring>(FindUI("InstanceParring"));
			shared_ptr<CUIInstanceBossHP> pBossHP = dynamic_pointer_cast<CUIInstanceBossHP>(FindUI("BossHP"));
			shared_ptr<CUINormalText> pBossText = dynamic_pointer_cast<CUINormalText>(FindUI("BossHPText"));
			if (!pTarget.expired() && pParryingStack)
			{
				pParryingStack->SetParringStack(pTarget.lock()->GetCurParryingStack(), pTarget.lock()->GetMaxParryingStack());
				shared_ptr<CUIBase> pParryingStackPanel = FindUI("InstanceParringPanel");
				pParryingStackPanel->SetIsRender(true);

				pBossHP->SetRatio(pTarget.lock()->GetHPRatio());
				pBossHP->SetBossIconType(0);
				pBossText->SwapText(pTarget.lock()->GetDisplayName());
				shared_ptr<CUIBase> pBossHPPanel = FindUI("BossHPPanel");
				pBossHPPanel->SetIsRender(true);
				pBossAbnormal->SetGobState(pTarget.lock()->GetBossActionState());
			}
		}

		else if (dynamic_pointer_cast<CMonster>(m_pLockOnTarget.lock())->GetModelName() == "M_L1Midget_Berserker_Shadow")
		{
			weak_ptr<CMidget_Berserker_Shadow> pTarget = dynamic_pointer_cast<CMidget_Berserker_Shadow>(m_pLockOnTarget.lock());
			shared_ptr<CUIInstanceParring> pParryingStack = dynamic_pointer_cast<CUIInstanceParring>(FindUI("InstanceParring"));
			shared_ptr<CUIInstanceBossHP> pBossHP = dynamic_pointer_cast<CUIInstanceBossHP>(FindUI("BossHP"));
			shared_ptr<CUINormalText> pBossText = dynamic_pointer_cast<CUINormalText>(FindUI("BossHPText"));
			if (!pTarget.expired() && pParryingStack)
			{
				pParryingStack->SetParringStack(pTarget.lock()->GetCurParryingStack(), pTarget.lock()->GetMaxParryingStack());
				shared_ptr<CUIBase> pParryingStackPanel = FindUI("InstanceParringPanel");
				pParryingStackPanel->SetIsRender(true);

				pBossHP->SetRatio(pTarget.lock()->GetHPRatio());
				pBossHP->SetBossIconType(0);
				pBossText->SwapText(pTarget.lock()->GetDisplayName());
				shared_ptr<CUIBase> pBossHPPanel = FindUI("BossHPPanel");
				pBossHPPanel->SetIsRender(true);
				pBossAbnormal->SetGobState(pTarget.lock()->GetBossActionState());
			}
		}

		// M_Orc_LowDarkCommander
		else if (dynamic_pointer_cast<CMonster>(m_pLockOnTarget.lock())->GetModelName() == "M_Orc_LowDarkCommander")
		{
			weak_ptr<COrc_DarkCommander> pTarget = dynamic_pointer_cast<COrc_DarkCommander>(m_pLockOnTarget.lock());
			shared_ptr<CUIInstanceParring> pParryingStack = dynamic_pointer_cast<CUIInstanceParring>(FindUI("InstanceParring"));
			shared_ptr<CUIInstanceBossHP> pBossHP = dynamic_pointer_cast<CUIInstanceBossHP>(FindUI("BossHP"));
			shared_ptr<CUINormalText> pBossText = dynamic_pointer_cast<CUINormalText>(FindUI("BossHPText"));
			if (!pTarget.expired() && pParryingStack)
			{
				pParryingStack->SetParringStack(pTarget.lock()->GetCurParryingStack(), pTarget.lock()->GetMaxParryingStack());
				shared_ptr<CUIBase> pParryingStackPanel = FindUI("InstanceParringPanel");
				pParryingStackPanel->SetIsRender(true);

				pBossHP->SetRatio(pTarget.lock()->GetHPRatio());
				pBossHP->SetBossIconType(1);
				pBossText->SwapText(pTarget.lock()->GetDisplayName());
				shared_ptr<CUIBase> pBossHPPanel = FindUI("BossHPPanel");
				pBossHPPanel->SetIsRender(true);
				pBossAbnormal->SetOrcState(pTarget.lock()->GetBossActionState());
			}
		}

		else if (dynamic_pointer_cast<CMonster>(m_pLockOnTarget.lock())->GetModelName() == "M_Orc_LowDarkCommander_Shadow")
		{
			weak_ptr<COrc_DarkCommander_Shadow> pTarget = dynamic_pointer_cast<COrc_DarkCommander_Shadow>(m_pLockOnTarget.lock());
			shared_ptr<CUIInstanceParring> pParryingStack = dynamic_pointer_cast<CUIInstanceParring>(FindUI("InstanceParring"));
			shared_ptr<CUIInstanceBossHP> pBossHP = dynamic_pointer_cast<CUIInstanceBossHP>(FindUI("BossHP"));
			shared_ptr<CUINormalText> pBossText = dynamic_pointer_cast<CUINormalText>(FindUI("BossHPText"));
			if (!pTarget.expired() && pParryingStack)
			{
				pParryingStack->SetParringStack(pTarget.lock()->GetCurParryingStack(), pTarget.lock()->GetMaxParryingStack());
				shared_ptr<CUIBase> pParryingStackPanel = FindUI("InstanceParringPanel");
				pParryingStackPanel->SetIsRender(true);

				pBossHP->SetRatio(pTarget.lock()->GetHPRatio());
				pBossHP->SetBossIconType(1);
				pBossText->SwapText(pTarget.lock()->GetDisplayName());
				shared_ptr<CUIBase> pBossHPPanel = FindUI("BossHPPanel");
				pBossHPPanel->SetIsRender(true);
				pBossAbnormal->SetOrcState(pTarget.lock()->GetBossActionState());
			}
		}

		// M_UndeadArmy_Knight
		else if (dynamic_pointer_cast<CMonster>(m_pLockOnTarget.lock())->GetModelName() == "M_UndeadArmy_Knight")
		{
			weak_ptr<CRedKnight> pTarget = dynamic_pointer_cast<CRedKnight>(m_pLockOnTarget.lock());
			shared_ptr<CUIInstanceParring> pParryingStack = dynamic_pointer_cast<CUIInstanceParring>(FindUI("InstanceParring"));
			shared_ptr<CUIInstanceBossHP> pBossHP = dynamic_pointer_cast<CUIInstanceBossHP>(FindUI("BossHP"));
			shared_ptr<CUINormalText> pBossText = dynamic_pointer_cast<CUINormalText>(FindUI("BossHPText"));
			if (!pTarget.expired() && pParryingStack)
			{
				pParryingStack->SetParringStack(pTarget.lock()->GetCurParryingStack(), pTarget.lock()->GetMaxParryingStack());
				shared_ptr<CUIBase> pParryingStackPanel = FindUI("InstanceParringPanel");
				pParryingStackPanel->SetIsRender(true);

				pBossHP->SetRatio(pTarget.lock()->GetHPRatio());
				pBossHP->SetBossIconType(2);
				pBossText->SwapText(pTarget.lock()->GetDisplayName());
				shared_ptr<CUIBase> pBossHPPanel = FindUI("BossHPPanel");
				pBossHPPanel->SetIsRender(true);
				pBossAbnormal->SetFinalBossState(pTarget.lock()->GetBossActionState());
			}
		}
	}
	else
	{
		shared_ptr<CUIBase> pParryingStackPanel = FindUI("InstanceParringPanel");
		if (pParryingStackPanel != nullptr)
			pParryingStackPanel->SetIsRender(false);

		shared_ptr<CUIBase> pBossHPPanel = FindUI("BossHPPanel");
		if (pBossHPPanel != nullptr)
			pBossHPPanel->SetIsRender(false);
	}
}

void CUIMgr::UpdateSpeechBallonPos()
{
	if (!m_pNPC.expired())
	{ 
		shared_ptr<CUIPanel> speechBallonPanel = dynamic_pointer_cast<CUIPanel>(FindUI("SpeechBallonBGs"));
		/* 수정 필요 */
		if (speechBallonPanel == nullptr)
			return;

		shared_ptr<CNPC> pNPC = dynamic_pointer_cast<CNPC>(m_pNPC.lock());
		_float3 playerPos = m_pPlayer.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float3 npcPos = pNPC->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float dist = SimpleMath::Vector3::Distance(playerPos, npcPos);

		speechBallonPanel->SetNonResetRender(true);

		_float4x4 followWorldMat{};
		_float3 followPos{};

		followWorldMat = pNPC->GetTransform()->GetWorldMatrix();
		followPos = followWorldMat.Translation() + _float3(0.f, pNPC->GetCombinedBoneMat("Bip001").Translation().y + 1.5f, 0.f);

		followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
		followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

		if (followPos.z < 0.f || followPos.z > 1.f)
		{
			followPos.x = 100000.f;
			followPos.y = 100000.f;

			speechBallonPanel->SetIsRender(false);
		}

		_float posX = (followPos.x + 1.f) * 0.5f;
		_float posY = (followPos.y - 1.f) * -0.5f;

		posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
		posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

		speechBallonPanel->SetUIPos(posX, posY);
	}
}

void CUIMgr::UpdateTargetIndicator()
{
	if (!m_pLockOnTarget.expired() && !m_bIsCameraDirecting)
	{
		shared_ptr<CUIBase> indicatorPanel = FindUI("TargetIndicatorPanel");
		if (indicatorPanel == nullptr)
			return;

		shared_ptr<CMonster> targetMonster = dynamic_pointer_cast<CMonster>(m_pLockOnTarget.lock());
		//_float3 playerPos = m_pPlayer.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float3 playerPos = CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float3 monsterPos = targetMonster->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float dist = SimpleMath::Vector3::Distance(playerPos, monsterPos);

		indicatorPanel->SetIsRender(true);

		_float4x4 followWorldMat{};
		_float3 followPos{};
		_float3 standPos{};

		if (targetMonster->GetModelName() == "Puppet")
		{
			followWorldMat = targetMonster->GetTransform()->GetWorldMatrix();
			followPos = followWorldMat.Translation() + _float3(0.f, 1.5f, 0.f);
			standPos = playerPos;
		}

		else
		{
			followWorldMat = targetMonster->GetCombinedBoneMat("Bip001") * targetMonster->GetTransform()->GetWorldMatrix();
			followPos = followWorldMat.Translation();
			standPos = playerPos;
		}

		followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
		followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));
		standPos = SimpleMath::Vector3::Transform(standPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
		standPos = SimpleMath::Vector3::Transform(standPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

		_float posX = (followPos.x + 1.f) * 0.5f;
		_float posY = (followPos.y - 1.f) * -0.5f;

		_float sPosX = (standPos.x + 1.f) * 0.5f;
		_float sPosY = (standPos.y - 1.f) * -0.5f;

		posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
		posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

		sPosX = g_iWinSizeX * sPosX - (g_iWinSizeX * 0.5f);
		sPosY = g_iWinSizeY * -sPosY + (g_iWinSizeY * 0.5f);

		/* LockOnUI 찾아서 Pos Set */
		_float2 targetPos = _float2(posX, posY);
		_float2 standVector = _float2(sPosX, sPosY);
		_float rotAngle = atan2(targetPos.y - standVector.y, targetPos.x - standVector.x);
		if (rotAngle > 3.1415927f)
			rotAngle -= 3.1415927f * 2.f;
		else if (rotAngle <= -3.1415927f)
			rotAngle += 3.1415927f * 2.f;

		rotAngle = rotAngle * 180.f / 3.1415927f;
		/*if (rotAngle > 90)
			rotAngle = rotAngle - 90.f;
		else
			rotAngle = rotAngle + 90.f;*/

		rotAngle = rotAngle - 90.f;

		targetPos.Normalize();
		standVector.Normalize();

		if (followPos.z < 0.f || followPos.z > 1.f)
		{
			rotAngle += 180.f;
			targetPos *= -1.f;
		}

		targetPos = targetPos * 100.f;

		shared_ptr<CUITargetIndicator> targetUI = dynamic_pointer_cast<CUITargetIndicator>(FindUI("TargetIndicator"));
		targetUI->SetTargetInfo(targetMonster->GetDisplayName());
		targetUI->SetTargetDist(to_wstring(dist));
		targetUI->SetUIPos(targetPos.x, targetPos.y);
		targetUI->RotateUI(0.f, 0.f, rotAngle);
	}
	else
	{
		shared_ptr<CUIBase> indicatorPanel = FindUI("TargetIndicatorPanel");
		if (indicatorPanel == nullptr)
			return;

		shared_ptr<CUITargetIndicator> targetUI = dynamic_pointer_cast<CUITargetIndicator>(FindUI("TargetIndicator"));
		if (targetUI == nullptr)
			return;

		targetUI->SetTargetInfo(L"");

		indicatorPanel->SetIsRender(false);
	}
}

void CUIMgr::UpdateMainNPCIndicator()
{
	if (!m_pQuestNPCs[QUEST_MAIN].expired() && !m_bIsCameraDirecting)
	{
		shared_ptr<CUIBase> indicatorPanel = FindUI("TargetIndicatorPanel_0");
		if (indicatorPanel == nullptr)
			return;

		shared_ptr<CNPC> targetMonster = dynamic_pointer_cast<CNPC>(m_pQuestNPCs[QUEST_MAIN].lock());
		//_float3 playerPos = m_pPlayer.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float3 playerPos = CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float3 monsterPos = targetMonster->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float dist = SimpleMath::Vector3::Distance(playerPos, monsterPos);

		_float4x4 followWorldMat{};
		_float3 followPos{};
		_float3 standPos{};

		followWorldMat = targetMonster->GetCombinedBoneMat("Bip001") * targetMonster->GetTransform()->GetWorldMatrix();
		followPos = followWorldMat.Translation();
		standPos = playerPos;

		followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
		followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));
		standPos = SimpleMath::Vector3::Transform(standPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
		standPos = SimpleMath::Vector3::Transform(standPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

		_float posX = (followPos.x + 1.f) * 0.5f;
		_float posY = (followPos.y - 1.f) * -0.5f;

		_float sPosX = (standPos.x + 1.f) * 0.5f;
		_float sPosY = (standPos.y - 1.f) * -0.5f;

		posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
		posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

		sPosX = g_iWinSizeX * sPosX - (g_iWinSizeX * 0.5f);
		sPosY = g_iWinSizeY * -sPosY + (g_iWinSizeY * 0.5f);

		/* LockOnUI 찾아서 Pos Set */
		_float2 targetPos = _float2(posX, posY);
		_float2 npcPos = _float2(posX, posY);
		_float2 standVector = _float2(sPosX, sPosY);
		_float rotAngle = atan2(targetPos.y - standVector.y, targetPos.x - standVector.x);
		if (rotAngle > 3.1415927f)
			rotAngle -= 3.1415927f * 2.f;
		else if (rotAngle <= -3.1415927f)
			rotAngle += 3.1415927f * 2.f;

		rotAngle = rotAngle * 180.f / 3.1415927f;
		/*if (rotAngle > 90)
			rotAngle = rotAngle - 90.f;
		else
			rotAngle = rotAngle + 90.f;*/

		rotAngle = rotAngle - 90.f;

		targetPos.Normalize();
		standVector.Normalize();

		if (followPos.z < 0.f || followPos.z > 1.f)
		{
			rotAngle += 180.f;
			targetPos *= -1.f;
		}

		targetPos = targetPos * 100.f;

		shared_ptr<CUITargetIndicator> targetUI = dynamic_pointer_cast<CUITargetIndicator>(FindUI("TargetIndicator_0"));

		targetUI->SetTargetInfo(targetMonster->GetDisplayName());
		wstringstream ss;
		ss << fixed << std::setprecision(2) << dist;
		wstring strDist = ss.str() + L" m";
		targetUI->SetTargetDist(strDist);

		if (dist < 10.f)
		{
			indicatorPanel->SetIsRender(false);

			weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("MainQuestIndicatorPanel"));
			weak_ptr<CUIInstanceIndicator> pUI = dynamic_pointer_cast<CUIInstanceIndicator>(UIMGR->FindUI("MainQuestIndicatorUV"));
			if (!pUI.expired())
			{
				pUIPanel.lock()->SetIsRender(true);
				pUI.lock()->UpdateDistance(dist);
				pUIPanel.lock()->SetUIPos(npcPos.x, npcPos.y);
			}
		}

		else
		{
			weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("MainQuestIndicatorPanel"));
			pUIPanel.lock()->SetIsRender(false);

			indicatorPanel->SetIsRender(true);
			targetUI->SetUIPos(targetPos.x, targetPos.y);
			targetUI->RotateUI(0.f, 0.f, rotAngle);
		}
	}
	else
	{
		shared_ptr<CUIBase> indicatorPanel = FindUI("TargetIndicatorPanel_0");
		if (indicatorPanel == nullptr)
			return;

		weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("MainQuestIndicatorPanel"));
		if (pUIPanel.expired())
			return;

		pUIPanel.lock()->SetIsRender(false);
		indicatorPanel->SetIsRender(false);
	}
}

void CUIMgr::UpdateSub0NPCIndicator()
{
	if (!m_pQuestNPCs[QUEST_SUB0].expired() && !m_bIsCameraDirecting)
	{
		shared_ptr<CUIBase> indicatorPanel = FindUI("TargetIndicatorPanel_1");
		if (indicatorPanel == nullptr)
			return;

		shared_ptr<CNPC> targetMonster = dynamic_pointer_cast<CNPC>(m_pQuestNPCs[QUEST_SUB0].lock());
		//_float3 playerPos = m_pPlayer.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float3 playerPos = CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float3 monsterPos = targetMonster->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float dist = SimpleMath::Vector3::Distance(playerPos, monsterPos);

		_float4x4 followWorldMat{};
		_float3 followPos{};
		_float3 standPos{};

		followWorldMat = targetMonster->GetCombinedBoneMat("Bip001") * targetMonster->GetTransform()->GetWorldMatrix();
		followPos = followWorldMat.Translation();
		standPos = playerPos;

		followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
		followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));
		standPos = SimpleMath::Vector3::Transform(standPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
		standPos = SimpleMath::Vector3::Transform(standPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

		_float posX = (followPos.x + 1.f) * 0.5f;
		_float posY = (followPos.y - 1.f) * -0.5f;

		_float sPosX = (standPos.x + 1.f) * 0.5f;
		_float sPosY = (standPos.y - 1.f) * -0.5f;

		posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
		posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

		sPosX = g_iWinSizeX * sPosX - (g_iWinSizeX * 0.5f);
		sPosY = g_iWinSizeY * -sPosY + (g_iWinSizeY * 0.5f);

		/* LockOnUI 찾아서 Pos Set */
		_float2 targetPos = _float2(posX, posY);
		_float2 npcPos = _float2(posX, posY);
		_float2 standVector = _float2(sPosX, sPosY);
		_float rotAngle = atan2(targetPos.y - standVector.y, targetPos.x - standVector.x);
		if (rotAngle > 3.1415927f)
			rotAngle -= 3.1415927f * 2.f;
		else if (rotAngle <= -3.1415927f)
			rotAngle += 3.1415927f * 2.f;

		rotAngle = rotAngle * 180.f / 3.1415927f;
		/*if (rotAngle > 90)
			rotAngle = rotAngle - 90.f;
		else
			rotAngle = rotAngle + 90.f;*/

		rotAngle = rotAngle - 90.f;

		targetPos.Normalize();
		standVector.Normalize();

		if (followPos.z < 0.f || followPos.z > 1.f)
		{
			rotAngle += 180.f;
			targetPos *= -1.f;
		}

		targetPos = targetPos * 100.f;

		shared_ptr<CUITargetIndicator> targetUI = dynamic_pointer_cast<CUITargetIndicator>(FindUI("TargetIndicator_1"));

		targetUI->SetTargetInfo(targetMonster->GetDisplayName());
		wstringstream ss;
		ss << fixed << std::setprecision(2) << dist;
		wstring strDist = ss.str() + L" m";
		targetUI->SetTargetDist(strDist);

		if (dist < 10.f)
		{
			indicatorPanel->SetIsRender(false);

			weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("SubQuestIndicatorPanel0"));
			weak_ptr<CUIInstanceIndicator> pUI = dynamic_pointer_cast<CUIInstanceIndicator>(UIMGR->FindUI("SubQuestIndicatorUV0"));
			if (!pUI.expired())
			{
				pUIPanel.lock()->SetIsRender(true);
				pUI.lock()->UpdateDistance(dist);
				pUIPanel.lock()->SetUIPos(npcPos.x, npcPos.y);
			}
		}

		else
		{
			weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("SubQuestIndicatorPanel0"));
			pUIPanel.lock()->SetIsRender(false);

			indicatorPanel->SetIsRender(true);
			targetUI->SetUIPos(targetPos.x, targetPos.y);
			targetUI->RotateUI(0.f, 0.f, rotAngle);
		}
	}
	else
	{
		shared_ptr<CUIBase> indicatorPanel = FindUI("TargetIndicatorPanel_1");
		if (indicatorPanel == nullptr)
			return;

		weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("SubQuestIndicatorPanel0"));
		if (pUIPanel.expired())
			return;

		pUIPanel.lock()->SetIsRender(false);
		indicatorPanel->SetIsRender(false);
	}
}

void CUIMgr::UpdateSub1NPCIndicator()
{
	if (!m_pQuestNPCs[QUEST_SUB1].expired() && !m_bIsCameraDirecting)
	{
		shared_ptr<CUIBase> indicatorPanel = FindUI("TargetIndicatorPanel_2");
		if (indicatorPanel == nullptr)
			return;

		shared_ptr<CNPC> targetMonster = dynamic_pointer_cast<CNPC>(m_pQuestNPCs[QUEST_SUB1].lock());
		//_float3 playerPos = m_pPlayer.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float3 playerPos = CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float3 monsterPos = targetMonster->GetTransform()->GetState(CTransform::STATE_POSITION);
		_float dist = SimpleMath::Vector3::Distance(playerPos, monsterPos);

		_float4x4 followWorldMat{};
		_float3 followPos{};
		_float3 standPos{};

		followWorldMat = targetMonster->GetCombinedBoneMat("Bip001") * targetMonster->GetTransform()->GetWorldMatrix();
		followPos = followWorldMat.Translation();
		standPos = playerPos;

		followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
		followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));
		standPos = SimpleMath::Vector3::Transform(standPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
		standPos = SimpleMath::Vector3::Transform(standPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

		_float posX = (followPos.x + 1.f) * 0.5f;
		_float posY = (followPos.y - 1.f) * -0.5f;

		_float sPosX = (standPos.x + 1.f) * 0.5f;
		_float sPosY = (standPos.y - 1.f) * -0.5f;

		posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
		posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

		sPosX = g_iWinSizeX * sPosX - (g_iWinSizeX * 0.5f);
		sPosY = g_iWinSizeY * -sPosY + (g_iWinSizeY * 0.5f);

		/* LockOnUI 찾아서 Pos Set */
		_float2 targetPos = _float2(posX, posY);
		_float2 npcPos = _float2(posX, posY);
		_float2 standVector = _float2(sPosX, sPosY);
		_float rotAngle = atan2(targetPos.y - standVector.y, targetPos.x - standVector.x);
		if (rotAngle > 3.1415927f)
			rotAngle -= 3.1415927f * 2.f;
		else if (rotAngle <= -3.1415927f)
			rotAngle += 3.1415927f * 2.f;

		rotAngle = rotAngle * 180.f / 3.1415927f;
		/*if (rotAngle > 90)
			rotAngle = rotAngle - 90.f;
		else
			rotAngle = rotAngle + 90.f;*/

		rotAngle = rotAngle - 90.f;

		targetPos.Normalize();
		standVector.Normalize();

		if (followPos.z < 0.f || followPos.z > 1.f)
		{
			rotAngle += 180.f;
			targetPos *= -1.f;
		}

		targetPos = targetPos * 100.f;

		shared_ptr<CUITargetIndicator> targetUI = dynamic_pointer_cast<CUITargetIndicator>(FindUI("TargetIndicator_2"));

		targetUI->SetTargetInfo(targetMonster->GetDisplayName());
		wstringstream ss;
		ss << fixed << std::setprecision(2) << dist;
		wstring strDist = ss.str() + L" m";
		targetUI->SetTargetDist(strDist);

		if (dist < 10.f)
		{
			indicatorPanel->SetIsRender(false);

			weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("SubQuestIndicatorPanel1"));
			weak_ptr<CUIInstanceIndicator> pUI = dynamic_pointer_cast<CUIInstanceIndicator>(UIMGR->FindUI("SubQuestIndicatorUV1"));
			if (!pUI.expired())
			{
				pUIPanel.lock()->SetIsRender(true);
				pUI.lock()->UpdateDistance(dist);
				pUIPanel.lock()->SetUIPos(npcPos.x, npcPos.y);
			}
		}

		else
		{
			weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("SubQuestIndicatorPanel1"));
			pUIPanel.lock()->SetIsRender(false);

			indicatorPanel->SetIsRender(true);
			targetUI->SetUIPos(targetPos.x, targetPos.y);
			targetUI->RotateUI(0.f, 0.f, rotAngle);
		}
	}
	else
	{
		shared_ptr<CUIBase> indicatorPanel = FindUI("TargetIndicatorPanel_2");
		if (indicatorPanel == nullptr)
			return;

		weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("SubQuestIndicatorPanel1"));
		if (pUIPanel.expired())
			return;

		pUIPanel.lock()->SetIsRender(false);
		indicatorPanel->SetIsRender(false);
	}
}

void CUIMgr::UpdateCatapultUI(_float _fRatio)
{
	if (m_bIsCatapultActivated)
	{
		shared_ptr<CUIInstanceParring> pUI = dynamic_pointer_cast<CUIInstanceParring>(FindUI("InstanceParring_0"));
		if (pUI)
			pUI->SetCatapultRatio(_fRatio);
	}
}

void CUIMgr::SortUIZOrder()
{
	if (static_cast<_uint>(m_UIVec.size()) > 1)
	{
		::sort(m_UIVec.begin(), m_UIVec.end(), [](weak_ptr<CUIBase> first, weak_ptr<CUIBase> second) {
			if (first.lock()->IsSelected())
			{
				if (first.lock()->GetSelectedZOrder() > second.lock()->GetZOrder())
					return true;
				else
					return false;
			}

			else if (second.lock()->IsSelected())
			{
				if (first.lock()->GetZOrder() > second.lock()->GetSelectedZOrder())
					return true;
				else
					return false;
			}

			else
			{
				if (first.lock()->GetZOrder() > second.lock()->GetZOrder())
					return true;
				else
					return false;
			}
			});
	}
}

void CUIMgr::ResetUISelect()
{
	for (auto& ui : m_UIVec)
	{
		if (ui.lock() != nullptr)
		{
			ui.lock()->SetIsSelected(false);
		}
	}
}

void CUIMgr::PrepareUVs(vector<_float2>& _vec, _float _fX, _float _fY)
{
	for (_uint i = 0; i < static_cast<_uint>(_fY); ++i)
	{
		for (_uint j = 0; j < static_cast<_uint>(_fX); ++j)
		{
			_vec.emplace_back(_float4(static_cast<_float>(j) / _fX, static_cast<_float>(i) / _fY, static_cast<_float>(j + 1) / _fX, static_cast<_float>(i + 1) / _fY));
		}
	}
}

void CUIMgr::ResetLoadList()
{
	m_strLoadUIList.clear();
	m_strLoadUIList.shrink_to_fit();
	m_uiChildrenTags.clear();
}

HRESULT CUIMgr::PreLoadUIs(const wstring& _strUIPath)
{
	for (const filesystem::directory_entry& entry : filesystem::recursive_directory_iterator(_strUIPath))
	{
		const filesystem::path& p = entry.path();

		if (filesystem::is_regular_file(p))
		{
			if (p.extension() == ".data")
			{
				string strFilePath = p.string();

				string strFileName = p.filename().stem().string();

				std::ifstream inFile(strFilePath, std::ios::binary);

				if (inFile.is_open())
				{
					ResetLoadList();

					if (FAILED(LoadUI(inFile)))
						return E_FAIL;
				}
			}
		}
	}

	return S_OK;
}

HRESULT CUIMgr::PreLoadUIsJASON(const wstring& _strUIPath)
{
	for (const filesystem::directory_entry& entry : filesystem::recursive_directory_iterator(_strUIPath))
	{
		const filesystem::path& p = entry.path();

		if (filesystem::is_regular_file(p))
		{
			if (p.extension() == ".json")
			{
				string strFilePath = p.string();
				wstring wstrFilePath = p.wstring();

				string strFileName = p.filename().stem().string();

				if (strFileName == "InstanceMonsterHPPanel" || strFileName == "InstanceNPCPanel"/* || strFileName == "InstanceParringPanel"*/)
					continue;

				Json::Value root = GAMEINSTANCE->ReadJson(wstrFilePath);

				ResetLoadList();

				if (FAILED(LoadUIJSON(root, strFileName)))
					return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CUIMgr::LoadCertainUI(const string& _strUIJsonTag)
{
	// TEXT("../Bin/Resources/Level_Static/Textures/UI/UIData/")
	string convertString = _strUIJsonTag + ".json";
	wstring wJsonTag;
	wJsonTag = GAMEINSTANCE->StringToWString(convertString);
	wstring finalPath = TEXT("../Bin/Resources/Level_Static/Textures/UI/UIData/") + wJsonTag;

	Json::Value root = GAMEINSTANCE->ReadJson(finalPath);

	ResetLoadList();

	if (FAILED(LoadUIJSON(root, _strUIJsonTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIMgr::TestInstancing()
{
	//InstanceUI instanceUI{};
	//instanceUI.vCenter = _float3(0.f, 0.f, 0.f);
	//instanceUI.vSize = _float2(100.f, 100.f);
	//instanceUI.vColor = _float4(1.f, 1.f, 1.f, 1.f);

	//CUIBase::UIDesc uiDesc{};
	//uiDesc.strTag = "Test";
	//uiDesc.iShaderPassIdx = 0;
	//uiDesc.eUILayer = UI_LAYER_HUD;
	//uiDesc.eUIType = UI_BACKGROUND;
	//uiDesc.eUITexType = UI_TEX_D;
	//uiDesc.eUISlotType = SLOT_END;
	//uiDesc.fDiscardAlpha = 0.1f;
	//uiDesc.fZOrder = 0.02f;
	//uiDesc.isMasked = false;
	//uiDesc.isNoised = false;

	//vector<_float2> uiPos;
	//uiPos.emplace_back(_float2(0.f, 0.f));
	//uiPos.emplace_back(_float2(100.f, 0.f));
	//uiPos.emplace_back(_float2(200.f, 0.f));

	//// Tex_Common_IconCheck_01_Small
	//vector<string> texs;
	//texs.emplace_back("Tex_Common_IconCheck_01_Small");
	//shared_ptr<CUIInstanceBG> instanceBG = CUIInstanceBG::Create(texs, "", "", uiDesc, instanceUI, uiPos);
	//if (instanceBG == nullptr)
	//	return E_FAIL;

	//if (FAILED(AddUI("Test", instanceBG)))
	//	return E_FAIL;

	//ActivateUI("Test");

	//return S_OK;

	return S_OK;
}

HRESULT CUIMgr::PrepareCrossHair()
{
	CVIInstancing::InstanceDesc instanceDesc{};
	instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	instanceDesc.vSize = _float2(10.f, 10.f);
	instanceDesc.fSizeY = 10.f;
	instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

	CUIBase::UIDesc uiDesc{};
	uiDesc.strTag = "CrossHair";
	uiDesc.eUIType = UI_CROSSHAIR;
	uiDesc.eUITexType = UI_TEX_D;
	uiDesc.eUISlotType = SLOT_END;
	uiDesc.isMasked = false;
	uiDesc.isNoised = false;
	uiDesc.eUILayer = UI_LAYER_HUD;
	uiDesc.fZOrder = 0.1f;
	uiDesc.fDiscardAlpha = 0.1f;
	uiDesc.iShaderPassIdx = 0;

	vector<string> tex;
	tex.emplace_back("HUD_Console_CrossHair_Dot");

	shared_ptr<CUICrossHair> pUI = CUICrossHair::Create(tex, "", "", uiDesc, instanceDesc);
	if (pUI == nullptr)
		return E_FAIL;

	AddUI("CrossHair", pUI);

	return S_OK;
}

HRESULT CUIMgr::PrepareAdenAmount()
{
	CVIInstancing::InstanceDesc instanceDesc{};
	instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 0.7f);
	/*instanceDesc.vSize = _float2(137.f, 137.f);
	instanceDesc.fSizeY = 24.f;*/
	instanceDesc.vSize = _float2(24.f, 24.f);
	instanceDesc.fSizeY = 24.f;
	instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

	CUIBase::UIDesc uiDesc{};
	uiDesc.strTag = "AdenAmount";
	uiDesc.eUIType = UI_ADENAMOUNT;
	uiDesc.eUITexType = UI_TEX_D;
	uiDesc.eUISlotType = SLOT_END;
	uiDesc.isMasked = false;
	uiDesc.isNoised = false;
	uiDesc.eUILayer = UI_LAYER_HUD;
	uiDesc.fZOrder = 0.91f;
	uiDesc.fDiscardAlpha = 0.1f;
	uiDesc.iShaderPassIdx = 0;

	vector<string> tex;
	//tex.emplace_back("Tex_Common_Amount_Box_003_Aden");
	tex.emplace_back("ICO_Adena");

	shared_ptr<CUIAmountAden> pUI = CUIAmountAden::Create(tex, "", "", uiDesc, instanceDesc);
	if (pUI == nullptr)
		return E_FAIL;

	pUI->SetUIPos(-530.f, 318.f);

	AddUI("AdenAmount", pUI);

	ActivateUI("AdenAmount");

	return S_OK;
}

HRESULT CUIMgr::PrepareCountDown()
{
	CVIInstancing::InstanceDesc instanceDesc{};
	instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 0.7f);
	instanceDesc.vSize = _float2(300.f, 300.f);
	instanceDesc.fSizeY = 82.f;
	instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

	CUIBase::UIDesc uiDesc{};
	uiDesc.strTag = "CountDown";
	uiDesc.eUIType = UI_COUNTDOWN;
	uiDesc.eUITexType = UI_TEX_D;
	uiDesc.eUISlotType = SLOT_END;
	uiDesc.isMasked = false;
	uiDesc.isNoised = false;
	uiDesc.eUILayer = UI_LAYER_HUD;
	uiDesc.fZOrder = 0.15f;
	uiDesc.fDiscardAlpha = 0.0f;
	uiDesc.iShaderPassIdx = 18;

	vector<string> tex;
	tex.emplace_back("BP_MapIconDetail_Tooltip_BG");

	vector<_float2> uiPos;
	uiPos.emplace_back(_float2(0.f, 280.f));

	vector<_float2> uiSize;
	uiSize.emplace_back(_float2(300.f, 82.f));

	shared_ptr<CUICountDown> pUI = CUICountDown::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize);
	if (pUI == nullptr)
		return E_FAIL;

	//pUI->SetUIPos();

	AddUI("CountDown", pUI);

	//ActivateUI("CountDown");

	return S_OK;
}

HRESULT CUIMgr::PrepareMapTitle()
{
	CVIInstancing::InstanceDesc instanceDesc{};
	instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 0.0f);
	instanceDesc.vSize = _float2(479.f, 479.f);
	instanceDesc.fSizeY = 288.f;
	instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

	CUIBase::UIDesc uiDesc{};
	uiDesc.strTag = "MapTitle";
	uiDesc.eUIType = UI_MAPTITLE;
	uiDesc.eUITexType = UI_TEX_D;
	uiDesc.eUISlotType = SLOT_END;
	uiDesc.isMasked = false;
	uiDesc.isNoised = false;
	uiDesc.eUILayer = UI_LAYER_HUD;
	uiDesc.fZOrder = 0.15f;
	uiDesc.fDiscardAlpha = 0.0f;
	uiDesc.iShaderPassIdx = 18;

	vector<string> tex;
	tex.emplace_back("Tex_Map_Title_BG");

	vector<_float2> uiPos;
	uiPos.emplace_back(_float2(0.f, 200.f));

	vector<_float2> uiSize;
	uiSize.emplace_back(_float2(479.f, 288.f));

	shared_ptr<CUIMapTitle> pUI = CUIMapTitle::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize);
	if (pUI == nullptr)
		return E_FAIL;

	//pUI->SetUIPos();

	AddUI("MapTitle", pUI);

	ActivateUI("MapTitle");

	return S_OK;
}

HRESULT CUIMgr::PrepareSpriteEffects()
{
	{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 0.8f);
		instanceDesc.vSize = _float2(40.f, 40.f);
		instanceDesc.fSizeY = 40.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "CursorDust";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.11f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("FluidCircleBoomFX_spritesheet");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 0.f));

		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(40.f, 40.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(6.f, 7.f));
		if (pUI == nullptr)
			return E_FAIL;

		//pUI->SetUIPos();

		AddUI("CursorDust", pUI);

		ActivateUI("CursorDust");
	}

	{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(255.f / 255.f, 196.f / 255.f, 105.f / 255.f, 0.8f);
		instanceDesc.vSize = _float2(360.f, 360.f);
		instanceDesc.fSizeY = 360.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "CraftResultEffect";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.11f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("seassonPassLevelUpBoomFX_01_spriteSheet");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 10.f));

		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(360.f, 360.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(6.f, 7.f));
		if (pUI == nullptr)
			return E_FAIL;

		//pUI->SetUIPos();

		AddUI("CraftResultEffect", pUI);

		ActivateUI("CraftResultEffect");
	}

	{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(255.f / 255.f, 196.f / 255.f, 105.f / 255.f, 0.8f);
		instanceDesc.vSize = _float2(240.f, 240.f);
		instanceDesc.fSizeY = 98.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "MainQuestPanelEffect";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.11f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("rectangleRotationFX_spritesheet");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 10.f));

		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(240.f, 98.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(5.f, 6.f));
		if (pUI == nullptr)
			return E_FAIL;

		//pUI->SetUIPos();

		//pUI->SetIsLoop(true);
		//pUI->SetIsPlaying(true);
		
		AddUI("MainQuestPanelEffect", pUI);

		//ActivateUI("QuestPanelEffect");
	}

	{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(255.f / 255.f, 196.f / 255.f, 105.f / 255.f, 0.8f);
		instanceDesc.vSize = _float2(240.f, 240.f);
		instanceDesc.fSizeY = 98.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "SubQuest0PanelEffect";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.11f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("rectangleRotationFX_spritesheet");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 10.f));

		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(240.f, 98.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(5.f, 6.f));
		if (pUI == nullptr)
			return E_FAIL;

		//pUI->SetUIPos();

		//pUI->SetIsLoop(true);
		//pUI->SetIsPlaying(true);

		AddUI("SubQuest0PanelEffect", pUI);

		//ActivateUI("QuestPanelEffect");
	}

	{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(255.f / 255.f, 196.f / 255.f, 105.f / 255.f, 0.8f);
		instanceDesc.vSize = _float2(240.f, 240.f);
		instanceDesc.fSizeY = 98.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "SubQuest1PanelEffect";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.11f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("rectangleRotationFX_spritesheet");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 10.f));

		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(240.f, 98.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(5.f, 6.f));
		if (pUI == nullptr)
			return E_FAIL;

		//pUI->SetUIPos();

		//pUI->SetIsLoop(true);
		//pUI->SetIsPlaying(true);

		AddUI("SubQuest1PanelEffect", pUI);

		//ActivateUI("QuestPanelEffect");
	}

	{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(255.f / 255.f, 196.f / 255.f, 105.f / 255.f, 0.8f);
		instanceDesc.vSize = _float2(120.f, 120.f);
		instanceDesc.fSizeY = 120.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "QuestCompleteSlotEffect";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.11f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("HotDealFX_spritesheet");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 65.f));

		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(120.f, 120.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(6.f, 6.f));
		if (pUI == nullptr)
			return E_FAIL;

		//pUI->SetUIPos();

		//pUI->SetIsLoop(true);
		//pUI->SetIsPlaying(true);

		AddUI("QuestCompleteSlotEffect", pUI);

		//ActivateUI("QuestPanelEffect");
	}

	{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(255.f / 255.f, 196.f / 255.f, 105.f / 255.f, 0.8f);
		instanceDesc.vSize = _float2(425.f, 425.f);
		instanceDesc.fSizeY = 150.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "QuestCompleteEffect";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.11f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("particleFX");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 112.5f));

		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(425.f, 150.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(6.f, 6.f));
		if (pUI == nullptr)
			return E_FAIL;

		//pUI->SetUIPos();

		//pUI->SetIsLoop(true);
		//pUI->SetIsPlaying(true);

		AddUI("QuestCompleteEffect", pUI);

		//ActivateUI("QuestPanelEffect");
	}

	{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 0.4f);
		instanceDesc.vSize = _float2(340.f, 340.f);
		instanceDesc.fSizeY = 90.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "TradeSlotBGEffect";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.11f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("inventorySlotOpenFX_01_SpirteSheet");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 0.f));

		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(340.f, 90.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(5.f, 6.f));
		if (pUI == nullptr)
			return E_FAIL;

		//pUI->SetUIPos();

		//pUI->SetIsLoop(true);
		//pUI->SetIsPlaying(true);

		AddUI("TradeSlotBGEffect", pUI);

		//ActivateUI("QuestPanelEffect");
	}

	{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
		instanceDesc.vSize = _float2(38.f, 38.f);
		instanceDesc.fSizeY = 38.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "CoolTimeEffect";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.08f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("particularBoomFX_02_spriteSheet");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 0.f));
		
		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(38.f, 38.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(5.f, 8.f));
		if (pUI == nullptr)
			return E_FAIL;

		//pUI->SetUIPos();

		//pUI->SetIsLoop(true);
		//pUI->SetIsPlaying(true);

		AddUI("CoolTimeEffect", pUI);

		//ActivateUI("QuestPanelEffect");
	}

	{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
		instanceDesc.vSize = _float2(68.f, 68.f);
		instanceDesc.fSizeY = 68.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "MainQuestIconEffect";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.08f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("vow_StoneFX_sprtiesheet");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 0.f));

		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(68.f, 68.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(6.f, 7.f));
		if (pUI == nullptr)
			return E_FAIL;

		AddUI("MainQuestIconEffect", pUI);
	}

	{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
		instanceDesc.vSize = _float2(68.f, 68.f);
		instanceDesc.fSizeY = 68.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "SubQuest0IconEffect";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.08f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("vow_StoneFX_sprtiesheet");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 0.f));

		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(68.f, 68.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(6.f, 7.f));
		if (pUI == nullptr)
			return E_FAIL;

		AddUI("SubQuest0IconEffect", pUI);
	}

	{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
		instanceDesc.vSize = _float2(68.f, 68.f);
		instanceDesc.fSizeY = 68.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "SubQuest1IconEffect";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.08f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("vow_StoneFX_sprtiesheet");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 0.f));

		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(68.f, 68.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(6.f, 7.f));
		if (pUI == nullptr)
			return E_FAIL;

		AddUI("SubQuest1IconEffect", pUI);
	}

	{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
		instanceDesc.vSize = _float2(80.f, 80.f);
		instanceDesc.fSizeY = 80.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "QuestCompleteIconEffect";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.08f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("RewardFX_SpriteSheet");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 0.f));

		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(80.f, 80.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(6.f, 6.f));
		if (pUI == nullptr)
			return E_FAIL;

		AddUI("QuestCompleteIconEffect", pUI);
	}

	/*{
		CVIInstancing::InstanceDesc instanceDesc{};
		instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
		instanceDesc.vSize = _float2(40.f, 40.f);
		instanceDesc.fSizeY = 80.f;
		instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

		CUIBase::UIDesc uiDesc{};
		uiDesc.strTag = "CertainNPCIndicator";
		uiDesc.eUIType = UI_SPRITEEFFECT;
		uiDesc.eUITexType = UI_TEX_D;
		uiDesc.eUISlotType = SLOT_END;
		uiDesc.isMasked = false;
		uiDesc.isNoised = false;
		uiDesc.eUILayer = UI_LAYER_HUD;
		uiDesc.fZOrder = 0.08f;
		uiDesc.fDiscardAlpha = 0.08f;
		uiDesc.iShaderPassIdx = 20;

		vector<string> tex;
		tex.emplace_back("ArrowMotion_spritesheet");

		vector<_float2> uiPos;
		uiPos.emplace_back(_float2(0.f, 0.f));

		vector<_float2> uiSize;
		uiSize.emplace_back(_float2(40.f, 80.f));

		shared_ptr<CUISpriteEffect> pUI = CUISpriteEffect::Create(tex, "", "", uiDesc, instanceDesc, uiPos, uiSize, _float2(5.f, 6.f));
		if (pUI == nullptr)
			return E_FAIL;

		AddUI("CertainNPCIndicator", pUI);
	}*/

	return S_OK;
}

void CUIMgr::KeyInput()
{
	if (GAMEINSTANCE->KeyDown(DIK_I)) // 수정 예정
	{
		shared_ptr<CUIBase> foundUI = FindUI("InventoryPanel");
		if (foundUI != nullptr)
		{
			if (foundUI->IsActivated() == false) // 켤 때
			{
				ActivateUI("InventoryPanel");		
				CGameMgr::GetInstance()->ChangeToFreeMouse(0);
			}
			else // 끌때
			{
				DeactivateUI("InventoryPanel");
				CGameMgr::GetInstance()->ChangeToNormalMouse();
			}
		}
	}

	if (GAMEINSTANCE->KeyDown(DIK_K))
		GAMEINSTANCE->SetToneMappingProperties(0.58f, 0.601f, 0.511f);

	//if (GAMEINSTANCE->KeyDown(DIK_K)) // 수정 예정
	//{
	//	shared_ptr<CUIBase> foundUI = FindUI("CraftAllPanel");
	//	if (foundUI != nullptr)
	//	{
	//		if (foundUI->IsActivated() == false)
	//		{
	//			ActivateUI("CraftAllPanel");
	//			DeactivateHUD();
	//			CGameMgr::GetInstance()->ChangeToFreeMouse(0);
	//		}

	//		else
	//		{
	//			DeactivateUI("CraftAllPanel");
	//			ActivateHUD();
	//			CGameMgr::GetInstance()->ChangeToNormalMouse();
	//		}
	//	}
	//}

	//if (GAMEINSTANCE->KeyDown(DIK_L)) // 수정 예정
	//{
	//	shared_ptr<CUIBase> foundUI = FindUI("TradeAllPanel");
	//	if (foundUI != nullptr)
	//	{
	//		if (foundUI->IsActivated() == false)
	//		{
	//			ActivateUI("TradeAllPanel");
	//			DeactivateHUD();
	//			CGameMgr::GetInstance()->ChangeToFreeMouse(0);
	//		}

	//		else
	//		{
	//			DeactivateUI("TradeAllPanel");
	//			ActivateHUD();
	//			CGameMgr::GetInstance()->ChangeToNormalMouse();
	//		}
	//	}
	//}

	if (GAMEINSTANCE->KeyDown(DIK_U))
	{
		shared_ptr<CUIBase> foundUI = FindUI("OptionAllPanel");
		if (foundUI != nullptr)
		{
			if (foundUI->IsActivated() == false)
			{
				ActivateUI("OptionAllPanel");
				DeactivateHUD();
				CGameMgr::GetInstance()->ChangeToFreeMouse(0);
			}

			else
			{
				DeactivateUI("OptionAllPanel");
				ActivateHUD();
				CGameMgr::GetInstance()->ChangeToNormalMouse();
			}
		}
	}

	if (GAMEINSTANCE->KeyDown(DIK_Y))
	{
		ITEMMGR->IncreaseMoney(2000);
	}

	if (GAMEINSTANCE->KeyDown(DIK_LSHIFT))
	{
		shared_ptr<CUIBase> foundUI = FindUI("MouseSlot");
		if (foundUI != nullptr)
		{
			if (m_isMouseSlotActivated == false)
			{
				//ActivateUI("MouseSlot");
				m_isMouseSlotActivated = true;
				foundUI->SetIsSelected(true);
			}
			else
			{
				//DeactivateUI("MouseSlot");
				m_isMouseSlotActivated = false;
				foundUI->SetIsSelected(false);
			}
		}
	}

	if (GAMEINSTANCE->MouseDown(DIM_LB))
	{
		if (CGameMgr::GetInstance()->GetMouseState() == 1)
		{
			POINT mousePt;
			::GetCursorPos(&mousePt);
			::ScreenToClient(g_hWnd, &mousePt);

			_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
			_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

			ActivateMouseCursor({ mousePosX, mousePosY });

			GAMEINSTANCE->PlaySoundW("UI_MouseClick_01_B", 0.5f);
		}
	}

	if (m_isMouseSlotActivated == false)
	{
		if (GAMEINSTANCE->MouseDown(DIM_LB))
		{
			auto it = m_UIVec.rbegin();
			for (it; it != m_UIVec.rend(); ++it)
			{
				/*if (it->lock()->IsMouseOn() && !it->lock()->IsChild() && !((it->lock()->GetUITag() == "QuickSlotPanel") || (it->lock()->GetUITag() == "PCInfo")))*/
				if (it->lock()->IsMouseOn() && !it->lock()->IsChild() && it->lock()->IsPickable())
				{
					ResetUISelect();

					if (it->lock()->GetUIType() == UI_SLOT && dynamic_pointer_cast<CUISlot>(it->lock())->GetSlotType() == SLOT_MOUSE)
						continue;

					if (it->lock()->GetUITag() == "InteractionPanel")
						continue;

					it->lock()->SetIsSelected(true);
					m_pLatestActivatedUI = it->lock();

					POINT mousePt;
					::GetCursorPos(&mousePt);
					::ScreenToClient(g_hWnd, &mousePt);

					_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
					_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

					m_vDir = _float3(mousePosX, mousePosY, 0.f) - m_pLatestActivatedUI.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
					m_fDist = m_vDir.Length();
					m_vDir.Normalize();

					return;
				}
			}
		}

		if (GAMEINSTANCE->MousePressing(DIM_LB) && !m_pLatestActivatedUI.expired())
		{
			if (m_pLatestActivatedUI.lock()->IsSelected() && m_pLatestActivatedUI.lock()->IsActivated() && m_pLatestActivatedUI.lock()->IsPickable())
			{
				if (m_pLatestActivatedUI.lock()->GetUIType() == UI_SLOT && dynamic_pointer_cast<CUISlot>(m_pLatestActivatedUI.lock())->GetSlotType() == SLOT_MOUSE)
				{

				}

				else
				{
					POINT mousePt;
					::GetCursorPos(&mousePt);
					::ScreenToClient(g_hWnd, &mousePt);

					_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
					_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

					_float3 dir = _float3(mousePosX, mousePosY, 0.f) - m_pLatestActivatedUI.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
					_float dist = dir.Length();
					dir.Normalize();

					if (dist != m_fDist)
					{
						_float3 moveDir = m_vDir * m_fDist;

						_float3 newPos = _float3(mousePosX, mousePosY, 0.f) - moveDir;

						m_pLatestActivatedUI.lock()->SetUIPos(newPos.x, newPos.y);
					}
				}
			}
		}

		if (GAMEINSTANCE->MouseUp(DIM_LB) && !m_pLatestActivatedUI.expired())
		{
			m_pLatestActivatedUI.lock()->SetIsSelected(false);
		}
	}
}

void CUIMgr::SortChildUIs(shared_ptr<CUIBase> _parentItemBG, shared_ptr<CUIBase> _childItemBG)
{
	_float upperPadding = 30.f;
	_float parentSizeX = _parentItemBG->GetUISize().x;
	_float parentSizeY = _parentItemBG->GetUISize().y;
	_float parentTop = _parentItemBG->GetUIPos().y + (parentSizeY / 2.f);

	_float parentStartTop = parentTop - upperPadding;
	_float parentBottom = _parentItemBG->GetUIPos().y - (parentSizeY / 2.f);
	_float parentLeft = _parentItemBG->GetUIPos().x - (parentSizeX / 2.f);
	_float parentRight = _parentItemBG->GetUIPos().x + (parentSizeX / 2.f);

	_float intervalSize = (parentSizeX / 6.f) * 1.2f;

	vector<_float2> uiPos;

	_uint maxNum = 50;
	for (_uint i = 0; i < maxNum; ++i)
	{
		if (i == 0)
		{
			uiPos.emplace_back(_float2(parentLeft + (intervalSize / 2.f), parentStartTop - (intervalSize / 2.f)));
		}

		else if (i < 5 && i > 0)
		{
			uiPos.emplace_back(_float2(uiPos[i - 1].x + intervalSize, parentStartTop - (intervalSize / 2.f)));
		}

		else
		{
			uiPos.emplace_back(_float2(uiPos[i - 5].x, uiPos[i - 5].y - intervalSize));
		}
	}

	for (_uint i = 0; i < maxNum; ++i)
	{
		string childUITag = _childItemBG->GetUITag();
		stringstream ss;

		for (char c : childUITag)
		{
			if (std::isdigit(c))
				ss << c;
		}

		int foundNum{ 0 };
		ss >> foundNum;

		if (foundNum == i)
		{
			_childItemBG->SetUIPos(uiPos[i].x, uiPos[i].y);
		}
	}
}

HRESULT CUIMgr::PrepareBindFuncs()
{
	m_bindFuncs.emplace("Deactivate Inventory", [&](void) -> void {
		shared_ptr<CUIBase> foundUI = FindUI("InventoryPanel");
		if (foundUI == nullptr)
			return;

		if (foundUI->IsActivated())
			DeactivateUI(foundUI->GetUITag());

		CGameMgr::GetInstance()->ChangeToNormalMouse();
		});

	return S_OK;
}

HRESULT CUIMgr::PrepareMouseSlot()
{
	CVIInstancing::InstanceDesc instanceDesc;
	instanceDesc.vSize = _float2(25.f, 25.f);
	instanceDesc.fSizeY = 33.f;
	instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	instanceDesc.vCenter = _float3(7.f, -9.f, 0.f);

	CUIBase::UIDesc uiDesc{};
	uiDesc.strTag = "MouseSlot";
	uiDesc.eUILayer = UI_POPUP_MOVABLE;
	uiDesc.eUIType = UI_SLOT;
	uiDesc.eUITexType = UI_TEX_D;
	uiDesc.eUISlotType = SLOT_MOUSE;
	uiDesc.fDiscardAlpha = 0.1f;
	uiDesc.fZOrder = 0.f;
	uiDesc.iShaderPassIdx = 4;
	uiDesc.isMasked = false;
	uiDesc.isNoised = false;

	vector<string> mouseSlotTexTags;
	mouseSlotTexTags.emplace_back("UI_Cursor_Base");
	shared_ptr<CUISlot> pMouseSlot = CUISlot::Create(mouseSlotTexTags, "", "", uiDesc, instanceDesc);
	if (pMouseSlot == nullptr)
		return E_FAIL;

	if (FAILED(AddUI(pMouseSlot->GetUITag(), pMouseSlot)))
		return E_FAIL;

	ActivateUI(pMouseSlot->GetUITag());

	return S_OK;
}

HRESULT CUIMgr::LoadUI(std::ifstream& _if)
{
	CUIBase::UIDesc uiDesc{};
	CVIInstancing::InstanceDesc instanceDesc;
	_uint iNumTexKeys;
	vector<string> strKeys;
	string strMaskKey;
	string strNoiseKey;
	_float2 uiPos{};
	_float2 uiSize{};
	vector<_float4> atlasUVs;
	string strUIParentTag;
	vector<string> strUIChildTags;

	char cStrTag[MAX_PATH] = "";
	_if.read(reinterpret_cast<char*>(cStrTag), MAX_PATH);
	uiDesc.strTag = cStrTag;

	_if.read(reinterpret_cast<char*>(&uiDesc.eUILayer), sizeof(UI_LAYER));
	_if.read(reinterpret_cast<char*>(&uiDesc.eUIType), sizeof(UI_TYPE));
	_if.read(reinterpret_cast<char*>(&uiDesc.eUITexType), sizeof(UI_TEX_TYPE));
	_if.read(reinterpret_cast<char*>(&uiDesc.eUISlotType), sizeof(UI_SLOT_TYPE));
	_if.read(reinterpret_cast<char*>(&uiDesc.isMasked), sizeof(_bool));
	_if.read(reinterpret_cast<char*>(&uiDesc.isNoised), sizeof(_bool));
	_if.read(reinterpret_cast<char*>(&uiDesc.iShaderPassIdx), sizeof(_uint));

	_if.read(reinterpret_cast<char*>(&instanceDesc), sizeof(CVIInstancing::InstanceDesc));

	_if.read(reinterpret_cast<char*>(&uiDesc.fDiscardAlpha), sizeof(_float));

	/* Texture */
	{
		_if.read(reinterpret_cast<char*>(&iNumTexKeys), sizeof(_uint));
		for (_uint i = 0; i < iNumTexKeys; ++i)
		{
			string strTex;
			char cStrTex[MAX_PATH] = "";
			_if.read(reinterpret_cast<char*>(cStrTex), MAX_PATH);
			strTex = cStrTex;

			strKeys.emplace_back(strTex);
		}
	}
	/* Mask Texture */
	if (uiDesc.isMasked)
	{
		char cStrMaskKey[MAX_PATH] = "";
		_if.read(reinterpret_cast<char*>(cStrMaskKey), MAX_PATH);
		strMaskKey = cStrMaskKey;
	}
	/* Noise Texture */
	if (uiDesc.isNoised)
	{
		char cStrNoiseKey[MAX_PATH] = "";
		_if.read(reinterpret_cast<char*>(cStrNoiseKey), MAX_PATH);
		strNoiseKey = cStrNoiseKey;
	}
	/* Atlas UVs */
	if (uiDesc.eUIType == UI_UV)
	{
		_uint iNumUVs{ 0 };
		_if.read(reinterpret_cast<char*>(&iNumUVs), sizeof(_uint));

		for (_uint i = 0; i < iNumUVs; ++i)
		{
			_float4 atlasUV{};
			_if.read(reinterpret_cast<char*>(&atlasUV), sizeof(_float4));
			atlasUVs.emplace_back(atlasUV);
		}
	}
	/* Pos */
	_if.read(reinterpret_cast<char*>(&uiPos), sizeof(_float2));
	/* Size */
	_if.read(reinterpret_cast<char*>(&uiSize), sizeof(_float2));
	/* ZOrder */
	_if.read(reinterpret_cast<char*>(&uiDesc.fZOrder), sizeof(_float));

	switch (uiDesc.eUIType)
	{
	case UI_HUD:
	{

	}
	break;
	case UI_BAR:
	{
		shared_ptr<CUIBar> pUI = CUIBar::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_ICON:
	{
		shared_ptr<CUIIcon> pUI = CUIIcon::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_SLIDER:
	{
		shared_ptr<CUISlider> pUI = CUISlider::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_BUTTON:
	{
		//shared_ptr<CToolUIButton> pUI = CToolUIButton::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

	}
	break;
	case UI_SCROLLBG:
	{
		shared_ptr<CUIScrollBG> pUI = CUIScrollBG::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, 0.f);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetBGInitPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_SLOT:
	{
		shared_ptr<CUISlot> pUI = CUISlot::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_SCROLL:
	{
		shared_ptr<CUIScroll> pUI = CUIScroll::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_COOLTIME:
	{
		shared_ptr<CUICoolTime> pUI = CUICoolTime::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_UV:
	{

	}
	break;
	case UI_PANEL:
	{
		shared_ptr<CUIPanel> pUI = CUIPanel::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_BACKGROUND:
	{
		shared_ptr<CUIBackGround> pUI = CUIBackGround::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	}

	{
		_uint iNumChildren{ 0 };
		_if.read(reinterpret_cast<char*>(&iNumChildren), sizeof(_uint));

		if (iNumChildren <= 0)
		{
			m_uiChildrenTags.emplace(uiDesc.strTag, strUIChildTags);

			return E_FAIL;
		}

		for (_uint i = 0; i < iNumChildren; ++i)
		{
			char cStrChildTag[MAX_PATH] = "";
			string strChildTag;
			_if.read(reinterpret_cast<char*>(cStrChildTag), MAX_PATH);
			strChildTag = cStrChildTag;

			strUIChildTags.emplace_back(strChildTag);
		}

		m_uiChildrenTags.emplace(uiDesc.strTag, strUIChildTags);

		for (_uint i = 0; i < iNumChildren; ++i)
		{
			LoadUI(_if);
		}
	}

	for (auto& loadUI : m_strLoadUIList)
	{
		auto it = m_uiChildrenTags.find(loadUI);
		if (it != m_uiChildrenTags.end())
		{
			vector<string> childrenTags = it->second;
			if (!childrenTags.empty())
			{
				for (auto& childTag : childrenTags)
				{
					AddUIChild(it->first, childTag);
				}
			}
		}
	}

	return S_OK;
}

HRESULT CUIMgr::LoadUIJSON(Json::Value& root, const string& _strUITag)
{
	Json::Value subRoot = root[_strUITag];

	CUIBase::UIDesc uiDesc{};
	UI_ITEM_TYPE eUIItemType{ UI_ITEM_END };
	_float fVisibleSizeY{ 0.f };
	CVIInstancing::InstanceDesc instanceDesc{};
	_uint iNumTexKeys{ 0 };
	vector<string> strKeys;
	string strMaskKey;
	string strNoiseKey;
	_float2 uiPos{};
	_float2 uiSize{};
	vector<_float4> atlasUVs;
	string strUIParentTag;
	vector<string> strUIChildTags;
	vector<_float2> instanceUIPos;
	vector<_float2> instanceUISize;

	/* UIDesc */
	Json::Value UIDesc = subRoot["UIDesc"];

	uiDesc.strTag = UIDesc["UITag"].asString();
	uiDesc.eUILayer = static_cast<UI_LAYER>(UIDesc["UILayer"].asUInt());
	uiDesc.eUIType = static_cast<UI_TYPE>(UIDesc["UIType"].asUInt());
	uiDesc.eUITexType = static_cast<UI_TEX_TYPE>(UIDesc["UITexType"].asUInt());
	uiDesc.eUISlotType = static_cast<UI_SLOT_TYPE>(UIDesc["UISlotType"].asUInt());
	uiDesc.isMasked = UIDesc["UIIsMasked"].asBool();
	uiDesc.isNoised = UIDesc["UIIsNoised"].asBool();
	uiDesc.iShaderPassIdx = UIDesc["UIPassIndex"].asUInt();
	uiDesc.fZOrder = UIDesc["UIZOrder"].asFloat();
	uiDesc.fDiscardAlpha = UIDesc["UIDiscardAlpha"].asFloat();

	if (uiDesc.eUISlotType == SLOT_INVEN)
		eUIItemType = static_cast<UI_ITEM_TYPE>(UIDesc["UIItemType"].asUInt());

	if (uiDesc.eUIType == UI_SCROLLBG)
		fVisibleSizeY = UIDesc["UIVisibleSizeY"].asFloat();

	/* Instance Desc */
	Json::Value InstanceDesc = subRoot["InstanceDesc"];
	{
		const Json::Value Pivot = InstanceDesc["Pivot"];
		if (Pivot.isArray())
			instanceDesc.vPivot = _float3(Pivot[0].asFloat(), Pivot[1].asFloat(), Pivot[2].asFloat());

		const Json::Value Center = InstanceDesc["Center"];
		if (Center.isArray())
			instanceDesc.vCenter = _float3(Center[0].asFloat(), Center[1].asFloat(), Center[2].asFloat());

		const Json::Value Range = InstanceDesc["Range"];
		if (Range.isArray())
			instanceDesc.vRange = _float3(Range[0].asFloat(), Range[1].asFloat(), Range[2].asFloat());

		const Json::Value Size = InstanceDesc["Size"];
		if (Size.isArray())
			instanceDesc.vSize = _float2(Size[0].asFloat(), Size[1].asFloat());

		const Json::Value Speed = InstanceDesc["Speed"];
		if (Speed.isArray())
			instanceDesc.vSpeed = _float2(Speed[0].asFloat(), Speed[1].asFloat());

		const Json::Value Time = InstanceDesc["LifeTime"];
		if (Time.isArray())
			instanceDesc.vLifeTime = _float2(Time[0].asFloat(), Time[1].asFloat());

		const Json::Value Color = InstanceDesc["Color"];
		if (Color.isArray())
			instanceDesc.vColor = _float4(Color[0].asFloat(), Color[1].asFloat(), Color[2].asFloat(), Color[3].asFloat());

		instanceDesc.fSizeY = InstanceDesc["SizeY"].asFloat();
		instanceDesc.fDuration = InstanceDesc["Duration"].asFloat();
		instanceDesc.isLoop = InstanceDesc["IsLoop"].asBool();
	}

	/* Textures */
	Json::Value Textures = subRoot["Textures"];
	{
		Json::Value Diffuse = Textures["Diffuse"];
		if (Diffuse.isArray())
		{
			for (const auto& strKey : Diffuse)
				strKeys.emplace_back(strKey.asString());
		}

		if (uiDesc.isMasked == true)
			strMaskKey = Textures["Mask"].asString();

		if (uiDesc.isNoised == true)
			strNoiseKey = Textures["Noise"].asString();
	}

	/* AtalsUVs */
	if (uiDesc.eUIType == UI_UV)
	{
		Json::Value AtlasUVs = subRoot["AtlasUVs"];
		if (AtlasUVs.isArray())
		{
			for (auto& atlasValue : AtlasUVs)
			{
				if (atlasValue.isArray())
				{
					_float4 UVs = _float4(atlasValue[0].asFloat(), atlasValue[1].asFloat(), atlasValue[2].asFloat(), atlasValue[3].asFloat());
					atlasUVs.emplace_back(UVs);
				}
			}
		}
	}

	/* SRT */
	Json::Value SRT = subRoot["SRT"];
	{
		Json::Value Pos = SRT["Pos"];
		if (Pos.isArray())
			uiPos = _float2(Pos[0].asFloat(), Pos[1].asFloat());

		Json::Value Scale = SRT["Scale"];
		if (Scale.isArray())
			uiSize = _float2(Scale[0].asFloat(), Scale[1].asFloat());
	}

	/* InstanceUIValues */
	if (uiDesc.eUIType == UI_INSTANCEITEMBG || uiDesc.eUIType == UI_INSTANCEITEMSLOT || uiDesc.eUIType == UI_INSTANCEITEMICON ||
		uiDesc.eUIType == UI_INSTANCEITEMCOOLTIME || uiDesc.eUIType == UI_INSTANCEITEMHOVER || uiDesc.eUIType == UI_INSTANCEQUICKBG ||
		uiDesc.eUIType == UI_INSTANCEQUICKMAINSLOT || uiDesc.eUIType == UI_INSTANCEQUICKITEMSLOT || uiDesc.eUIType == UI_INSTANCEQUICKCOMBATSLOT ||
		uiDesc.eUIType == UI_INSTANCEQUICKFRAME || uiDesc.eUIType == UI_INSTANCEPARRING || uiDesc.eUIType == UI_INSTANCEABNORMALSLOT ||
		uiDesc.eUIType == UI_INSTANCEWORLDHP || uiDesc.eUIType == UI_INSTANCEBOSSHP || uiDesc.eUIType == UI_INSTANCEBOSSABNORMAL ||
		uiDesc.eUIType == UI_INSTANCEQUEST || uiDesc.eUIType == UI_INSTANCEQUESTINDICATOR || uiDesc.eUIType == UI_INSTANCEINTERACTION ||
		uiDesc.eUIType == UI_INSTANCEDIALOGUE || uiDesc.eUIType == UI_INSTANCECRAFTBG || uiDesc.eUIType == UI_INSTANCECRAFTHOVER ||
		uiDesc.eUIType == UI_INSTANCECRAFTTAB || uiDesc.eUIType == UI_INSTANCECRAFTSLOT || uiDesc.eUIType == UI_INSTANCEWORLDNPC ||
		uiDesc.eUIType == UI_INSTANCETRADEBG || uiDesc.eUIType == UI_INSTANCETRADESLOT || uiDesc.eUIType == UI_INSTANCETRADETAB ||
		uiDesc.eUIType == UI_INSTANCETRADEHOVER || uiDesc.eUIType == UI_QUESTCOMPLETE || uiDesc.eUIType == UI_ITEMTOOLTIP ||
		uiDesc.eUIType == UI_SPEECHBALLON || uiDesc.eUIType == UI_STARFORCE || uiDesc.eUIType == UI_OPTIONBG ||
		uiDesc.eUIType == UI_OPTIONTAB || uiDesc.eUIType == UI_OPTIONHOVER || uiDesc.eUIType == UI_OPTIONSLIDER ||
		uiDesc.eUIType == UI_OPTIONCHECKBOX || uiDesc.eUIType == UI_CRAFTRESULT || uiDesc.eUIType == UI_TUTORIAL ||
		uiDesc.eUIType == UI_INTERACTIONPOPUP)
	{
		Json::Value InstanceUIValues = subRoot["InstanceUIValues"];

		_uint iNumValues{ 0 };
		for (Json::ValueIterator it = InstanceUIValues.begin(); it != InstanceUIValues.end(); ++it)
		{
			string strKey = "InstanceValue" + to_string(iNumValues);

			Json::Value InstanceValue = InstanceUIValues[strKey];
			{
				_float2 instanceValuePos{};
				_float2 instanceValueSize{};

				Json::Value InstancePos = InstanceValue["InstancePos"];
				if (InstancePos.isArray())
					instanceValuePos = _float2(InstancePos[0].asFloat(), InstancePos[1].asFloat());

				Json::Value InstanceSize = InstanceValue["InstanceSize"];
				if (InstanceSize.isArray())
					instanceValueSize = _float2(InstanceSize[0].asFloat(), InstanceSize[1].asFloat());

				instanceUIPos.emplace_back(instanceValuePos);
				instanceUISize.emplace_back(instanceValueSize);

				iNumValues++;
			}
		}
	}

	/* Create UI */
	switch (uiDesc.eUIType)
	{
	case UI_HUD:
	{

	}
	break;
	case UI_BAR:
	{
		shared_ptr<CUIBar> pUI = CUIBar::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_ICON:
	{
		shared_ptr<CUIIcon> pUI = CUIIcon::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_SLIDER:
	{
		shared_ptr<CUISlider> pUI = CUISlider::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_BUTTON:
	{
		shared_ptr<CUIButton> pUI = CUIButton::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_SCROLLBG:
	{
		shared_ptr<CUIScrollBG> pUI = CUIScrollBG::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, fVisibleSizeY);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		//pUI->SetUIPos(uiPos.x, uiPos.y);
		pUI->SetBGInitPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_SLOT:
	{
		shared_ptr<CUISlot> pUI = CUISlot::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
		pUI->SetItemType(eUIItemType);
	}
	break;
	case UI_SCROLL:
	{
		shared_ptr<CUIScroll> pUI = CUIScroll::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_COOLTIME:
	{
		shared_ptr<CUICoolTime> pUI = CUICoolTime::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_UV:
	{

	}
	break;
	case UI_PANEL:
	{
		shared_ptr<CUIPanel> pUI = CUIPanel::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_BACKGROUND:
	{
		shared_ptr<CUIBackGround> pUI = CUIBackGround::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_BACKGROUND_R:
	{
		shared_ptr<CUIBackGround> pUI = CUIBackGround::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_HOVER:
	{
		shared_ptr<CUIHover> pUI = CUIHover::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_COVER:
	{
		shared_ptr<CUICover> pUI = CUICover::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_ITEMBG:
	{
		shared_ptr<CUIItemBackGround> pUI = CUIItemBackGround::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_ITEMICON:
	{
		shared_ptr<CUIItemIcon> pUI = CUIItemIcon::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_ITEMHOVER:
	{
		shared_ptr<CUIItemHover> pUI = CUIItemHover::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_ITEMTAB:
	{
		shared_ptr<CUIItemTab> pUI = CUIItemTab::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_COVER_RV:
	{
		shared_ptr<CUICover> pUI = CUICover::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_BACKGROUND_NT:
	{
		shared_ptr<CUIBackGround> pUI = CUIBackGround::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_LOCKON:
	{
		shared_ptr<CUILockOn> pUI = CUILockOn::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_HL:
	{
		shared_ptr<CUIBackGround> pUI = CUIBackGround::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_HR:
	{
		shared_ptr<CUIBackGround> pUI = CUIBackGround::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEITEMBG:
	{
		shared_ptr<CUIInstanceItemBG> pUI = CUIInstanceItemBG::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEITEMSLOT:
	{
		shared_ptr<CUIInstanceItemSlot> pUI = CUIInstanceItemSlot::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
		pUI->SetItemType(eUIItemType);
	}
	break;
	case UI_INSTANCEITEMICON:
	{
		shared_ptr<CUIInstanceItemIcon> pUI = CUIInstanceItemIcon::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEITEMCOOLTIME:
	{
		shared_ptr<CUIInstanceItemCoolTime> pUI = CUIInstanceItemCoolTime::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEITEMHOVER:
	{
		shared_ptr<CUIInstanceItemHover> pUI = CUIInstanceItemHover::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEQUICKBG:
	{
		shared_ptr<CUIInstanceQuickBG> pUI = CUIInstanceQuickBG::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEQUICKMAINSLOT:
	{
		shared_ptr<CUIInstanceQuickSlot> pUI = CUIInstanceQuickSlot::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEQUICKITEMSLOT:
	{
		shared_ptr<CUIInstanceQuickSlot> pUI = CUIInstanceQuickSlot::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEQUICKCOMBATSLOT:
	{
		shared_ptr<CUIInstanceQuickSlot> pUI = CUIInstanceQuickSlot::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEQUICKFRAME:
	{
		shared_ptr<CUIInstanceQuickBG> pUI = CUIInstanceQuickBG::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEPARRING:
	{
		shared_ptr<CUIInstanceParring> pUI = CUIInstanceParring::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEABNORMALSLOT:
	{
		shared_ptr<CUIInstanceAbnormalSlot> pUI = CUIInstanceAbnormalSlot::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEWORLDHP:
	{
		shared_ptr<CUIInstanceWorldHP> pUI = CUIInstanceWorldHP::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEBOSSHP:
	{
		shared_ptr<CUIInstanceBossHP> pUI = CUIInstanceBossHP::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_NORMALTEXT:
	{
		weak_ptr<CCustomFont> pFont = GAMEINSTANCE->FindFont("Font_AdenL");
		if (!pFont.expired())
		{
			wstring strScript = L"적기사";

			shared_ptr<CUINormalText> pUI = CUINormalText::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, pFont.lock(), strScript);
			if (FAILED(AddUI(pUI->GetUITag(), pUI)))
				return E_FAIL;

			pUI->SetUIPos(uiPos.x, uiPos.y);
			m_strLoadUIList.emplace_back(pUI->GetUITag());
			ActivateUI(pUI->GetUITag());
		}
	}
	break;
	case UI_INSTANCEBOSSABNORMAL:
	{
		shared_ptr<CUIInstanceBossAbnormal> pUI = CUIInstanceBossAbnormal::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEQUEST:
	{
		shared_ptr<CUIInstanceQuest> pUI = CUIInstanceQuest::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		//pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());

		// Test
		//pUI->UpdateQuestContent(QUEST_MAIN, L"아무거나 테스트 중", L"정말 아무거나 테스트 중인데 뭔가 되는 지 안되는 지 알 수가 없다 이것도 되나?");
	}
	break;
	case UI_INSTANCEQUESTINDICATOR:
	{
		shared_ptr<CUIInstanceIndicator> pUI = CUIInstanceIndicator::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEINTERACTION:
	{
		shared_ptr<CUIInstanceInteraction> pUI = CUIInstanceInteraction::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEDIALOGUE:
	{
		shared_ptr<CUIInstanceDialogue> pUI = CUIInstanceDialogue::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCECRAFTBG:
	{
		shared_ptr<CUICraftBG> pUI = CUICraftBG::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCECRAFTHOVER:
	{
		shared_ptr<CUICraftHover> pUI = CUICraftHover::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCECRAFTSLOT:
	{
		shared_ptr<CUICraftSlot> pUI = CUICraftSlot::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCECRAFTTAB:
	{
		shared_ptr<CUICraftTab> pUI = CUICraftTab::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCEWORLDNPC:
	{
		shared_ptr<CUIInstanceWorldNPC> pUI = CUIInstanceWorldNPC::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCETRADEBG:
	{
		shared_ptr<CUITradeBG> pUI = CUITradeBG::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCETRADESLOT:
	{
		shared_ptr<CUITradeSlot> pUI = CUITradeSlot::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCETRADETAB:
	{
		shared_ptr<CUITradeTab> pUI = CUITradeTab::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INSTANCETRADEHOVER:
	{
		shared_ptr<CUITradeHover> pUI = CUITradeHover::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_QUESTCOMPLETE:
	{
		shared_ptr<CUIQuestComplete> pUI = CUIQuestComplete::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_ITEMTOOLTIP:
	{
		shared_ptr<CUIItemToolTip> pUI = CUIItemToolTip::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_SPEECHBALLON:
	{
		shared_ptr<CUISpeechBallon> pUI = CUISpeechBallon::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_TARGETINDICATOR:
	{
		shared_ptr<CUITargetIndicator> pUI = CUITargetIndicator::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INTERACTIONSTATUE:
	{
		shared_ptr<CUIInteractionStatue> pUI = CUIInteractionStatue::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_STARFORCE:
	{
		shared_ptr<CUIStarForce> pUI = CUIStarForce::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_OPTIONBG:
	{
		shared_ptr<CUIOptionBG> pUI = CUIOptionBG::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_OPTIONTAB:
	{
		shared_ptr<CUIOptionTab> pUI = CUIOptionTab::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_OPTIONHOVER:
	{
		shared_ptr<CUIOptionHover> pUI = CUIOptionHover::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_OPTIONSLIDER:
	{
		shared_ptr<CUIOptionSlider> pUI = CUIOptionSlider::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_OPTIONCHECKBOX:
	{
		shared_ptr<CUIOptionCheckBox> pUI = CUIOptionCheckBox::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_CRAFTRESULT:
	{
		shared_ptr<CUICraftResult> pUI = CUICraftResult::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_TUTORIAL:
	{
		shared_ptr<CUITutorial> pUI = CUITutorial::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_INTERACTIONPOPUP:
	{
		shared_ptr<CUIInteractionPopUp> pUI = CUIInteractionPopUp::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc, instanceUIPos, instanceUISize);
		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	case UI_UVBAR:
	{
		shared_ptr<CUIUVBar> pUI = CUIUVBar::Create(strKeys, strMaskKey, strNoiseKey, uiDesc, instanceDesc);

		if (FAILED(AddUI(pUI->GetUITag(), pUI)))
			return E_FAIL;

		pUI->PrepareUVs(3.f, 40.f);

		pUI->SetUIPos(uiPos.x, uiPos.y);
		m_strLoadUIList.emplace_back(pUI->GetUITag());
		ActivateUI(pUI->GetUITag());
	}
	break;
	}

	/* Children */
	Json::Value Children = subRoot["Children"];

	_uint iNumChildren = Children["NumChildren"].asUInt();
	if (iNumChildren <= 0)
	{
		m_uiChildrenTags.emplace(uiDesc.strTag, strUIChildTags);

		return S_OK;
	}

	Json::Value ChildrenTag = Children["ChildrenTag"];
	if (ChildrenTag.isArray())
	{
		for (const auto& strTag : ChildrenTag)
			strUIChildTags.emplace_back(strTag.asString());
	}

	m_uiChildrenTags.emplace(uiDesc.strTag, strUIChildTags);

	for (auto& childTag : strUIChildTags)
		LoadUIJSON(root, childTag);

	for (auto& loadUI : m_strLoadUIList)
	{
		auto it = m_uiChildrenTags.find(loadUI);
		if (it != m_uiChildrenTags.end())
		{
			vector<string> childrenTags = it->second;
			if (!childrenTags.empty())
			{
				for (auto& childTag : childrenTags)
				{
					AddUIChild(it->first, childTag);
				}
			}
		}
	}

	return S_OK;
}

shared_ptr<CUIMgr> CUIMgr::Create()
{
	shared_ptr<CUIMgr> pInstance = make_shared<CUIMgr>();

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CUIMgr::Create", MB_OK);

		pInstance.reset();

		return nullptr;
	}

	return pInstance;
}
