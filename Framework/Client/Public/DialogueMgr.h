#pragma once

#include "Client_Defines.h"

BEGIN(Engine)

class CGameObject;

END

BEGIN(Client)

class CUIInstanceDialogue;
class CPolymorph;
class CMonster;
class CNPC;

class CDialogueMgr
{
	DECLARE_SINGLETON(Client::CDialogueMgr)

public:
	CDialogueMgr();
	~CDialogueMgr();

public:
	HRESULT Initialize();
	void Tick(_float _fDeltaTime);

public: /* Get & Set Function */
	void SetCurPolymorph(shared_ptr<CPolymorph> _player) { m_pPoly = _player; }

	_bool IsDialoguePlaying() { return m_bIsPlaying; }

	_bool SetCurDialogue(const wstring& _strNPC);
	_bool SetCurDefaultDialogue(const wstring& _strNPC);

	wstring GetCurDialogueKey() { return m_strCurActivatedDialogueTag; }

	DIALOGUE_STATE GetDialogueState() { return m_eDialogueState; }

public:
	wstring FindDialogue(const wstring& _strNPC);

	/* 매 레벨마다 Clear 해주고 레벨에 있는 NPC를 넣어준다 */
	shared_ptr<CNPC> FindNPC(const wstring& _strNPCName);
	void AddNPC(const wstring& _strNPCName, shared_ptr<CNPC> _pNPC);
	void ClearNPC();

public:
	void InitDialogue();

public:
	void UpdateDialogue(const wstring& _strNPC, const wstring& _strContent, const wstring& _strInteraction, const string& _strNPCFont = "Font_SuiteL12", const string& _strContentFont = "Font_SuiteL10", const string& _strInteractionFont = "Font_SuiteL10", _float4 _vNPCColor = _float4(1.f, 1.f, 1.f, 1.f), _float4 _vContentColor = _float4(1.f, 1.f, 1.f, 1.f), _float4 _vInteractionColor = _float4(1.f, 1.f, 1.f, 1.f));

public:
	void AddDialogue(const wstring& _strNpc, list<pair<wstring, wstring>> _strContent);
	void DeleteDialogue(const wstring& _strCurNpc);

public:
	HRESULT PreLoadJsonFiles(const wstring& _strPath);
	HRESULT LoadJsonFile(Json::Value& root, const string& _strTag);

private:
	HRESULT BindDialogueUI();

private:
	weak_ptr<CUIInstanceDialogue> m_pDialogueUI;
	weak_ptr<CPolymorph> m_pPoly;

private:
	/* Store All Dialogues */
	unordered_map<wstring, list<pair<wstring, wstring>>> m_Dialogues;
	unordered_map<wstring, list<string>> m_DialogueSounds;
	unordered_map<wstring, list<pair<wstring, wstring>>> m_defaultDialogues;
	unordered_map<wstring, list<pair<wstring, wstring>>> m_storeDefaultDialogues;

private:
	/* Currently Activated Dialogue */
	/* It will be kept playing until it's empty */
	list<pair<wstring, wstring>> m_CurActivatedDialogue;
	list<string> m_CurActivatedDialogueSound;
	wstring m_strCurActivatedDialogueTag; // Container's Key
	wstring m_strCurActivatedDialogueNPCName; // NPC's Name

private:
	/* Check if Dialogue is still playing */
	_bool m_bIsPlaying{ false };

private:
	/* Check if it is Default Dialogue */
	_bool m_bIsDefault{ false };

private:
	unordered_map<wstring, weak_ptr<CNPC>> m_pNPCs;

private:
	DIALOGUE_STATE m_eDialogueState{ DIALOGUE_END };

};

END
