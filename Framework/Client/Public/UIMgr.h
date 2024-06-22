#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CUIBase;
class CPlayer;
class CNPC;

class CUIMgr
{
	DECLARE_SINGLETON(Client::CUIMgr)

public:
	CUIMgr();
	~CUIMgr();

public:
	HRESULT Initialize();
	void Tick(_float _fDeltaTime);

public: /* Get & Set Function */
	void SetHUD(const string& _strUITag);

	void SetUISize(_float _fSizeX, _float _fSizeY);

	shared_ptr<CUIBase> GetLatestActivatedUI() { return m_pLatestActivatedUI.lock(); }

	/* 수정 필요 */
	function<void(void)> GetBindFunc(const string& _funcTag);

	/* 주의 */
	unordered_map<string, shared_ptr<CUIBase>>* GetAllUIs() { return &m_UIs; }
	vector<weak_ptr<CUIBase>>* GetAllActivatedUIs() { return &m_UIVec; }

	_int GetPlayerCurAnimation();
	_bool IsCrossBowBuffed();

public:
	void SetLockOnTarget(shared_ptr<CGameObject> _lockOnTarget);
	void SetLockOnIcon(_bool _isLockedOn);

	_uint GetTabIndex() { return m_iTabIndex; }
	void SetTabIndex(_uint _iTabIndex) { m_iTabIndex = _iTabIndex; }

	void BindPlayer(shared_ptr<CPlayer> _pPlayer);

	void SetCurNPC(shared_ptr<CNPC> _pNPC);

	void SetCurQuestNPC(QUEST_TYPE _eQuestType, shared_ptr<CNPC> _pNPC);

	_bool IsQuestNPCActivated(QUEST_TYPE _eQuestType) { return m_bIsQuestNPCActivated[_eQuestType]; }
	void SetIsQuestNPCActivated(QUEST_TYPE _eQuestType, _bool _bIsQuestNPCActivated) { m_bIsQuestNPCActivated[_eQuestType] = _bIsQuestNPCActivated; }

	/* Speech Ballon */
	void SetSpeechBallonContent(const wstring& _strNPCName, const wstring& _strContent);

	void SetIsCameraDirecting(_bool _isCameraDirecting) { m_bIsCameraDirecting = _isCameraDirecting; }

	void SetInteractionPopUpContent(const wstring& _strContent);

	_bool GetIsStartSignal();
	
	void SetMapTitle(_uint _iLevel);

	_bool IsCameraDirecting() { return m_bIsCameraDirecting; }

	_bool IsHUDOn() { return m_bIsHUDOn; }

public:
	HRESULT AddUI(const string& _strUITag, shared_ptr<CUIBase> _pUI);
	HRESULT AddUIAfterSwapTag(const string& _strUITag, shared_ptr<CUIBase> _pUI);

	shared_ptr<CUIBase> FindUI(const string& _strUITag);

	HRESULT AddUIChild(const string& _strUITag, const string& _strChildTag);
	HRESULT AddUIParent(const string& _strUITag, const string& _strParentTag);
	HRESULT AddUIChild(const string& _strChildUITag);
	HRESULT AddUIParent(const string& _strParentTag);

	HRESULT RemoveUIChild(const string& _strUITag, const string& _strChildUITag);

public:
	void ActivateUI(const string& _strUITag);
	void DeactivateUI(const string& _strUITag);

	void SelectUI(const string& _strUITag);

	void DeleteSelectedUI(const string& _strUITag);

	void ActivateCrossHair();
	void DeactivateCrossHair();

	void ActivateInteractionUI();
	void DeactivateInteractionUI();

	void ActivateItemToolTip();
	void DeactivateItemToolTip();

	void ActivateSpeechBallon();
	void DeactivateSpeechBallon();

	void ActivateTargetIndicator();
	void DeactivateTargetIndicator();

	void ActivateNPCIndicator(QUEST_TYPE _eQuestType);
	void DeactivateNPCIndicator(QUEST_TYPE _eQuestType);

	void ActivateInteractionStatue();
	void DeactivateInteractionStatue();

	void ActivateOption();
	void DeactivateOption();

	void ActivateStarForce();
	void DeactivateStarForce();

	void ActivateCraft();
	void DeactivateCraft();

	void ActivateCraftResult();
	void DeactivateCraftResult();

	void ActivateFailedCraftResult();
	void DeactivateFailedCraftResult();

	void ActivateLockOnTarget();
	void DeactivateLockOnTarget();

	void ActivateTradeItemToolTip();
	void DeactivateTradeItemToolTip();

	void ActivateLoading(_uint _iTexIndex);
	void DeactivateLoading();

	void ActivateCatapultUI();
	void DeactivateCatapultUI();

	void ActivateCatapultTutorial();
	void DeactivateCatapultTutorial();

	void ActivateInteractionPopUp();
	void DeactivateInteractionPopUp();

	void ActivateCountDown();
	void UpdateCountDown(_float _fRemainTime);
	void UpdateNumFound(_int _iCurFound, _int _iMaxFound);
	void DeactivateCountDown();

	void ActivateMapTitle();
	void DeactivateMapTitle();

	void ActivateMouseCursor(_float2 _vPos);
	void DeactivateMouseCursor();

	void RenderOnMouseCursor();
	void RenderOffMouseCursor();

	void ActivateCraftResultEffect();
	void DeactivateCraftResultEffect();

	void ActivateQuestPanelEffect(QUEST_TYPE _eQuestType);
	void DeactivateQuestPanelEffect(QUEST_TYPE _eQuestType);

	void ActivateQuestCompleteSlotEffect();
	void DeactivateQuestCompleteSlotEffect();

	void ActivateQuestCompleteEffect();
	void DeactivateQuestCompleteEffect();

	void ActivateTradeSlotBGEffect(_float2 _vPos);
	void DeactivateTradeSlotBGEffect();

	void ActivateCoolTimeEffect(_float2 _vPos);
	void DeactivateCoolTimeEffect();

	void ActivateQuestIconEffect(QUEST_TYPE _questType);
	void DeactivateQuestIconEffect(QUEST_TYPE _questType);

	void ActivateQuestCompleteIconEffect();
	void DeactivateQuestCompleteIconEffect();

	void ActivateCertainNPCIndicator(_float2 _vPos);
	void DeactivateCertainNPCIndicator();

	void ActivateHUD(); // HUD의 Rendering을 On
	void DeactivateHUD(); // HUD의 Rendering을 Off

	void UITickOn();
	void UITickOff();

public:
	void FollowLockOnTarget();
	void UpdateParryingStack();
	void UpdateSpeechBallonPos();
	void UpdateTargetIndicator();
	void UpdateMainNPCIndicator();
	void UpdateSub0NPCIndicator();
	void UpdateSub1NPCIndicator();
	void UpdateCatapultUI(_float _fRatio);

public:
	void SortUIZOrder();
	void ResetUISelect();

public:
	void PrepareUVs(vector<_float2>& _vec, _float _fX, _float _fY);

public:
	void ResetLoadList();
	HRESULT PreLoadUIs(const wstring& _strUIPath);
	HRESULT PreLoadUIsJASON(const wstring& _strUIPath);
	HRESULT LoadCertainUI(const string& _strUIJsonTag);

	HRESULT TestInstancing();
	HRESULT PrepareCrossHair();
	HRESULT PrepareAdenAmount();
	HRESULT PrepareCountDown();
	HRESULT PrepareMapTitle();
	HRESULT PrepareSpriteEffects();

private:
	void KeyInput();
	void SortChildUIs(shared_ptr<CUIBase> _parentItemBG, shared_ptr<CUIBase> _childItemBG);

private:
	HRESULT PrepareBindFuncs();
	HRESULT PrepareMouseSlot();
	HRESULT LoadUI(std::ifstream& _if);
	HRESULT LoadUIJSON(Json::Value& root, const string& _strUITag);

public:
	static shared_ptr<CUIMgr> Create();

private:
	unordered_map<string, shared_ptr<CUIBase>> m_UIs; // 모든 UI를 모아서 넣은 컨테이너 -> 찾기 등 쉽게 쓰려고 만듦 -> 지워지면 안됌
	vector<weak_ptr<CUIBase>> m_UIVec; // 활성화된 UI들만 모아넣은 컨테이너 -> 활성화 된 UI들만 따로 관리하기 위해 만듦 -> 중간에 지워야함
	//list<weak_ptr<ToolUIBase>> _uiList; // 활성화된 UI들만 모아넣은 컨테이너 -> 활성화 된 UI들만 따로 관리하기 위해 만듦 -> 중간에 지워야함

	shared_ptr<CUIBase> m_pHUD{ nullptr };
	weak_ptr<CUIBase> m_pLatestActivatedUI;

private: /* Move UI */
	_float3 m_vDir{};
	_float m_fDist{ 0.f };

private: /* Button */
	unordered_map<string, function<void(void)>> m_bindFuncs;

private: /* Activate MouseSlot */
	_bool m_isMouseSlotActivated{ false };

private: /* For UI PreLoad */
	unordered_map<string, vector<string>> m_uiChildrenTags;
	vector<string> m_strLoadUIList;

private:
	_float m_fTimeAcc{ 0.f };

private: /* Lock On */
	weak_ptr<CGameObject> m_pLockOnTarget;
	_bool m_bIsLockOnRender{ true };

private:
	_uint m_iTabIndex{ 0 }; // if 0 Equip, if 1 Usuable, if 2 Misc

private:
	weak_ptr<CPlayer> m_pPlayer;

private: /* NPC */
	weak_ptr<CNPC> m_pNPC;
	vector<weak_ptr<CNPC>> m_pQuestNPCs; // For Indicator
	vector<_bool> m_bIsQuestNPCActivated;

private:
	_bool m_bIsCatapultActivated{ false };
	_bool m_bIsCameraDirecting{ false };

private:
	_bool m_bIsLoadingDone{ true };

private:
	_bool m_bIsHUDOn{ true };

};

END
