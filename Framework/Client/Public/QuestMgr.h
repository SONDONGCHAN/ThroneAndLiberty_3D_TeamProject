#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CUIInstanceQuest;
class CPlayer;
class CMonster;
class CPolymorph;
class CNPC;

class CQuestMgr
{
	DECLARE_SINGLETON(Client::CQuestMgr)

public:
	CQuestMgr();
	~CQuestMgr();

public:
	HRESULT Initialize();
	void Tick(_float _fDeltaTime);

public: /* Get & Set Function */
	void CheckDeadMonster(shared_ptr<CGameObject> _pMonster);
	void CheckDialogNPC(shared_ptr<CGameObject> _pNPC);
	void ActiveEventTag(QUEST_TYPE _QuestType, string _strEventTag);

	void SetCurPolymorph(shared_ptr<CPolymorph> _player) { m_pPoly = _player; }

	shared_ptr<CNPC> GetNPC(const string& _strNPCKey);
	

	function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)>& GetCurBindMainQuest() { return m_curBindMainQuest; }
	function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)>& GetCurBindSubQuest0() { return m_curBindSubQuest0; }
	function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)>& GetCurBindSubQuest1() { return m_curBindSubQuest1; }

	_bool IsQuestBinded(const wstring& _strNPCKey);
	_bool IsMainQuestAvailable();
	_bool IsSubQuestAvailable();

public:
	void AddQuest(QUEST_TYPE _questType, const wstring& _strKey, function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)>& _questContent);
	void AddQuestReward(const wstring& _strKey, function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)>& _questContent);
	void AddNextQuest(const wstring& _strKey, function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)>& _questContent);
	void DeleteQuest(QUEST_TYPE _questType, wstring _strQuestTag);

	/* 매 레벨마다 Clear 해주고 레벨에 있는 NPC를 넣어준다 */
	void AddNPC(const string& _strNPCKey, shared_ptr<CNPC> _pNPC);
	void ClearNPCs();

public:
	wstring GetCurrentQuestKey(QUEST_TYPE _questType);

public:
	void UpdateQuestContent(QUEST_TYPE _questType, const wstring& _questTitle, const wstring& _questContent, const string& _strFont = "Font_AdenL10", _float4 _vTitleFontColor = _float4(0.f, 0.f, 0.f, 0.f), _float4 _vContentFontColor = _float4(0.f, 0.f, 0.f, 0.f));

public:
	void UpdateQuest(QUEST_TYPE _questType, const wstring _questKey);
	void UpdateHuntingQuest(QUEST_TYPE _questType, wstring _strQuestKey, string _strTargetKey, _uint _count = 0);
	void UpdateHuntingContent(QUEST_TYPE _questType, wstring _strQuestKey, const string& _strFont = "Font_AdenL10", _float4 _vTitleFontColor = _float4(0.f, 0.f, 0.f, 0.f), _float4 _vContentFontColor = _float4(0.f, 0.f, 0.f, 0.f));

public:
	void UpdateEventActiveQuest(QUEST_TYPE _questType, wstring _strQuestKey, _uint _count = 0);
	void UpdateEventContent(QUEST_TYPE _questType, wstring _strQuestKey, const string& _strFont = "Font_AdenL10", _float4 _vTitleFontColor = _float4(0.f, 0.f, 0.f, 0.f), _float4 _vContentFontColor = _float4(0.f, 0.f, 0.f, 0.f));

public:
	HRESULT PreLoadJsonFiles(const wstring& _strPath);
	HRESULT LoadJsonFile(Json::Value& root, const string& _strTag);

public:
	HRESULT PrepareNPCs();

private:
	HRESULT BindQuestUI();
	HRESULT TestBindQuest();

private:
	void QuestKeyInput();
	void AddFirstSetToInventory();
	void AddSecondSetToInventory();
	void AddCraftItem();

private:
	weak_ptr<CUIInstanceQuest> m_pQuestUI;
	weak_ptr<CPolymorph> m_pPoly;

private:
	/* Quest Objects */
	list<weak_ptr<CGameObject>> m_pMainObjects;
	list<weak_ptr<CGameObject>> m_pSubObjects0;
	list<weak_ptr<CGameObject>> m_pSubObjects1;

private:
	/* Quest Display Values */
	unordered_map<wstring, pair<wstring, wstring>> m_QuestContents;

private:
	/* Store All Quests */
	unordered_map<wstring, function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)>> m_bindMainQuests;
	unordered_map<wstring, function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)>> m_bindSubQuests;

private:
	/* Currently Activated Quests */
	function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> m_curBindMainQuest;
	function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> m_curBindSubQuest0;
	function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> m_curBindSubQuest1;

private:
	/* Reward After Quest */
	unordered_map<wstring, function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)>> m_QuestRewards;

private:
	/* Currently Activated Reward */
	function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> m_curMainQuestReward;
	function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> m_curSubQuest0Reward;
	function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> m_curSubQuest1Reward;

private:
	/* Next Quest if needed */
	unordered_map<wstring, function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)>> m_NextQuests;

private:
	/* Currently Activated Next Quest */
	function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> m_curMainNextQuest;
	function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> m_curSubNextQuest0;
	function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> m_curSubNextQuest1;

	/*String Event Params*/
private:
	string m_strCurrentMainEvent = "";
	string m_strCurrentSubEvent0 = "";
	string m_strCurrentSubEvent1 = "";


private:
	wstring m_strCurrentMain = TEXT("");
	wstring m_strCurrentSub0 = TEXT("");
	wstring m_strCurrentSub1 = TEXT("");


private:
	_uint m_iMainQuestIndex{ 0 };
	_uint m_iSubQuestIndex{ 0 };

private: /* HuntingMultipleQuest */
	//unordered_map<wstring, vector<string>> m_multipleTargetKeys;
	//unordered_map<string, pair<_uint, _uint>> m_multipleTargets;

	unordered_map<wstring, unordered_map<string, pair<string, pair<_uint, _uint>>>> m_multipleTargets;

private: /* EventActiveQuest */
	unordered_map<wstring, pair<_uint, _uint>> m_multipleEvents;

private: /* NPCs */
	unordered_map<string, shared_ptr<CNPC>> m_pNPCs;
	_bool m_bIsOnce{ true };

private: /* Quest Complete Check */
	_bool m_curMainCompleted{ false };
	_bool m_curSub0Completed{ false };
	_bool m_curSub1Completed{ false };

private: /* QuestContent */
	wstring m_curMainQuestContent = TEXT("");
	wstring m_curSubQuest0Content = TEXT("");
	wstring m_curSubQuest1Content = TEXT("");

private: /* QuestTitle */
	wstring m_MainQuestTitle{ TEXT("") };
	wstring m_SubQuest0Title{ TEXT("") };
	wstring m_SubQuest1Title{ TEXT("") };

private:
	_uint m_iNumPressed{ 0 };

};

END
