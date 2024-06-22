#include "DialogueMgr.h"
#include "UIInstanceDialogue.h"
#include "UIMgr.h"
#include "QuestMgr.h"
#include "Polymorph.h"
#include "NPC.h"
#include "GameMgr.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(Client::CDialogueMgr)

CDialogueMgr::CDialogueMgr()
{

}

CDialogueMgr::~CDialogueMgr()
{
	
}

HRESULT CDialogueMgr::Initialize()
{
	if (FAILED(BindDialogueUI()))
		return E_FAIL;

	if (FAILED(PreLoadJsonFiles(L"../../Client/Bin/Resources/Level_Static/Textures/UI/DialogueData/")))
		return E_FAIL;

	/* SetCurDialogue로 대화 시작 */
	//SetCurDialogue(L"노스");

	return S_OK;
}

void CDialogueMgr::Tick(_float _fDeltaTime)
{
	/* 현재 활성화 되어있는 다이얼로그가 비어있지 않고 계속 플레이 중이면 UI를 계속 업데이트 */
	/* 마우스를 누를 때만 다음 대화로 넘어간다 */
	if (!m_bIsPlaying)
	{
		shared_ptr<CUIBase> pDialoguePanel = UIMGR->FindUI("DialoguePanel");
		if (pDialoguePanel != nullptr)
			pDialoguePanel->SetIsRender(false);

		m_eDialogueState = DIALOGUE_DONE;
	}

	else if (m_bIsPlaying)
	{
		if (m_CurActivatedDialogue.empty())
		{
			/* 현재 활성화된 Dialogue가 비었을 때(끝났을 때) */
			/* 뭔가 해줘야 하나? */
			/* Dialogue UI를 꺼주고, 다른 HUD들을 켠다 */
			if (GAMEINSTANCE->MouseDown(DIM_LB) && (m_pDialogueUI.lock()->IsInstanceMouseOn() != -1))
			{
				m_CurActivatedDialogue.clear();
				m_CurActivatedDialogueSound.clear();

				if (!m_bIsDefault) // Default Dialogue가 아닐 때만 지워준다
					DeleteDialogue(m_strCurActivatedDialogueTag);
				m_bIsPlaying = false;

				m_eDialogueState = DIALOGUE_COMPLETE;

				/* 다이얼로그가 끝났을 때 퀘스트를 자동으로 등록 */
				//QUESTMGR->UpdateQuest(QUEST_MAIN, m_strCurActivatedDialogueTag);

				/* HUD가 다시 렌더링 되게끔 */
				UIMGR->ActivateHUD();
				CGameMgr::GetInstance()->ChangeToNormalMouse();
				GAMEINSTANCE->PlaySoundW("Interation_Success01", 1.f);
			}
		}

		else if (!m_CurActivatedDialogue.empty())
		{
			m_eDialogueState = DIALOGUE_PROCESSING;

			shared_ptr<CNPC> pNPC = FindNPC(m_strCurActivatedDialogueNPCName);
			if (pNPC != nullptr)
				pNPC->SetIsDialoguePressed(false);

			if (GAMEINSTANCE->MouseDown(DIM_LB) && (m_pDialogueUI.lock()->IsInstanceMouseOn() != -1))
			{
				pair<wstring, wstring> pair = m_CurActivatedDialogue.front();
				string soundKey = m_CurActivatedDialogueSound.front();

				UpdateDialogue(m_strCurActivatedDialogueNPCName, pair.first, pair.second);

				/* Notify to NPC */
				shared_ptr<CNPC> pNPC = FindNPC(m_strCurActivatedDialogueNPCName);
				if (pNPC != nullptr)
					pNPC->SetIsDialoguePressed(true);

				if (soundKey != "")
					GAMEINSTANCE->PlaySoundW(soundKey, 4.f);

				m_CurActivatedDialogue.pop_front();
				m_CurActivatedDialogueSound.pop_front();
			}
		}

		//if (GAMEINSTANCE->MouseDown(DIM_LB) && (m_pDialogueUI.lock()->IsInstanceMouseOn() != -1))
		//{
		//	pair<wstring, wstring> pair = m_CurActivatedDialogue.front();

		//	UpdateDialogue(m_strCurActivatedDialogueNPCName, pair.first, pair.second);

		//	m_CurActivatedDialogue.pop_front();

		//	if (m_CurActivatedDialogue.empty() && GAMEINSTANCE->MouseDown(DIM_LB) && (m_pDialogueUI.lock()->IsInstanceMouseOn() != -1))
		//	{
		//		/* 현재 활성화된 Dialogue가 비었을 때(끝났을 때) */
		//		/* 뭔가 해줘야 하나? */
		//		/* Dialogue UI를 꺼주고, 다른 HUD들을 켠다 */
		//		shared_ptr<CUIBase> pDialoguePanel = UIMGR->FindUI("DialoguePanel");
		//		pDialoguePanel->SetIsRender(false);

		//		m_CurActivatedDialogue.clear();
		//		DeleteDialogue(m_strCurActivatedDialogueTag);
		//		m_bIsPlaying = false;
		//	}
		//}
	}
}

_bool CDialogueMgr::SetCurDialogue(const wstring& _strNPC)
{
	wstring strDialogueKey = FindDialogue(_strNPC);
	if (strDialogueKey == L"")
		return false;

	/* 예외처리 필요 */
	//function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&)> 
	// 여기가 문제네?
	/*if (!QUESTMGR->IsQuestBinded(strDialogueKey) && !(QUESTMGR->IsMainQuestAvailable() || QUESTMGR->IsSubQuestAvailable()))
		return false;*/

	wstring strNPCName;
	for (wchar_t ch : strDialogueKey)
	{
		if (!std::isdigit(ch))
			strNPCName.push_back(ch);
	}

	m_strCurActivatedDialogueNPCName = strNPCName; // 이름만 넣기 위함
	m_strCurActivatedDialogueTag = strDialogueKey;

	m_CurActivatedDialogue = m_Dialogues[strDialogueKey];
	m_CurActivatedDialogueSound = m_DialogueSounds[strDialogueKey];

	m_bIsPlaying = true;

	shared_ptr<CUIBase> pDialoguePanel = UIMGR->FindUI("DialoguePanel");
	if (pDialoguePanel != nullptr)
		pDialoguePanel->SetIsRender(true);
	
	InitDialogue();

	UIMGR->DeactivateHUD();
	CGameMgr::GetInstance()->ChangeToFreeMouse(1);

	return true;
}

_bool CDialogueMgr::SetCurDefaultDialogue(const wstring& _strNPC)
{
	wstring defaultDialogueKey{ L"" };
	defaultDialogueKey = _strNPC + L"Default";

	auto it = m_Dialogues.find(defaultDialogueKey);
	if (it == m_Dialogues.end())
		return false;

	m_strCurActivatedDialogueNPCName = _strNPC;
	m_strCurActivatedDialogueTag = defaultDialogueKey;

	m_CurActivatedDialogue = m_Dialogues[defaultDialogueKey];
	m_CurActivatedDialogueSound = m_DialogueSounds[defaultDialogueKey];

	m_bIsPlaying = true;
	m_bIsDefault = true;

	shared_ptr<CUIBase> pDialoguePanel = UIMGR->FindUI("DialoguePanel");
	if (pDialoguePanel != nullptr)
		pDialoguePanel->SetIsRender(true);

	InitDialogue();

	UIMGR->DeactivateHUD();
	CGameMgr::GetInstance()->ChangeToFreeMouse(1);

	return true;
}

wstring CDialogueMgr::FindDialogue(const wstring& _strNPC)
{
	/*_uint iNumDialogue = static_cast<_uint>(m_Dialogues.size());

	for (_uint i = 0; i < iNumDialogue; ++i)
	{
		wstring strKey = _strNPC + to_wstring(i);

		auto it = m_Dialogues.find(strKey);
		if (it != m_Dialogues.end())
			return strKey;
	}*/

	auto it = m_Dialogues.find(_strNPC);
	if (it != m_Dialogues.end())
		return _strNPC;

	return L"";
}

shared_ptr<CNPC> CDialogueMgr::FindNPC(const wstring& _strNPCName)
{
	auto it = m_pNPCs.find(_strNPCName);
	if (it == m_pNPCs.end())
		return nullptr;

	return it->second.lock();
}

void CDialogueMgr::AddNPC(const wstring& _strNPCName, shared_ptr<CNPC> _pNPC)
{
	auto it = m_pNPCs.find(_strNPCName);
	if (it != m_pNPCs.end())
		return;

	m_pNPCs.emplace(_strNPCName, _pNPC);
}

void CDialogueMgr::ClearNPC()
{
	m_pNPCs.clear();
}

void CDialogueMgr::InitDialogue()
{
	if (!m_CurActivatedDialogue.empty())
	{
		pair<wstring, wstring> pair = m_CurActivatedDialogue.front();
		string soundKey = m_CurActivatedDialogueSound.front();

		UpdateDialogue(m_strCurActivatedDialogueNPCName, pair.first, pair.second);

		if (soundKey != "")
			GAMEINSTANCE->PlaySoundW(soundKey, 6.f);

		m_CurActivatedDialogue.pop_front();
		m_CurActivatedDialogueSound.pop_front();

		m_eDialogueState = DIALOGUE_START;
	}
}

void CDialogueMgr::UpdateDialogue(const wstring& _strNPC, const wstring& _strContent, const wstring& _strInteraction, const string& _strNPCFont, const string& _strContentFont, const string& _strInteractionFont, _float4 _vNPCColor, _float4 _vContentColor, _float4 _vInteractionColor)
{
	if (!m_pDialogueUI.expired())
		m_pDialogueUI.lock()->UpdateDialogue(_strNPC, _strContent, _strInteraction, _strNPCFont, _strContentFont, _strInteractionFont, _vNPCColor, _vContentColor, _vInteractionColor);
}

void CDialogueMgr::AddDialogue(const wstring& _strNPCKey, list<pair<wstring, wstring>> _strContent)
{
	auto it = m_Dialogues.find(_strNPCKey);
	if (it != m_Dialogues.end())
		return;

	m_Dialogues.emplace(_strNPCKey, _strContent);
}

void CDialogueMgr::DeleteDialogue(const wstring& _strCurNpcKey)
{
	auto it = m_Dialogues.find(_strCurNpcKey);
	if (it == m_Dialogues.end())
		return;

	m_Dialogues.erase(_strCurNpcKey);

	auto soundIt = m_DialogueSounds.find(_strCurNpcKey);
	if (soundIt == m_DialogueSounds.end())
		return;

	m_DialogueSounds.erase(_strCurNpcKey);
}

HRESULT CDialogueMgr::PreLoadJsonFiles(const wstring& _strPath)
{
	for (const filesystem::directory_entry& entry : filesystem::recursive_directory_iterator(_strPath))
	{
		const filesystem::path& p = entry.path();

		if (filesystem::is_regular_file(p))
		{
			if (p.extension() == ".json")
			{
				string strFilePath = p.string();
				wstring wstrFilePath = p.wstring();

				string strFileName = p.filename().stem().string();

				Json::Value root = GAMEINSTANCE->ReadJson(wstrFilePath);

				if (FAILED(LoadJsonFile(root, strFileName)))
					return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CDialogueMgr::LoadJsonFile(Json::Value& root, const string& _strTag)
{
	Json::Value subRoot = root[_strTag];

	list<pair<wstring, wstring>> dialogues;
	list<string> dialogueSounds;

	string strDialogueKey = subRoot["NPC Key"].asString();
	_tchar wNPCKey[MAX_PATH] = TEXT("");
	MultiByteToWideChar(CP_UTF8, 0, strDialogueKey.c_str(), static_cast<_uint>(strDialogueKey.size()), wNPCKey, MAX_PATH);

	Json::Value contents = subRoot["Dialogue Contents"];
	if (contents.isArray())
	{
		for (auto& content : contents)
		{
			if (content.isArray())
			{
				string dialogueContent = content[0].asString();
				string interactionTag = content[1].asString();
				string dialogueSoundKey = content[2].asString();

				_tchar wDialogueContent[MAX_PATH] = TEXT("");
				_tchar wInteractionTag[MAX_PATH] = TEXT("");

				MultiByteToWideChar(CP_UTF8, 0, dialogueContent.c_str(), static_cast<_int>(dialogueContent.size()), wDialogueContent, MAX_PATH);
				MultiByteToWideChar(CP_UTF8, 0, interactionTag.c_str(), static_cast<_int>(interactionTag.size()), wInteractionTag, MAX_PATH);

				pair<wstring, wstring> dialoguePair = make_pair(wDialogueContent, wInteractionTag);
				dialogues.emplace_back(dialoguePair);
				dialogueSounds.emplace_back(dialogueSoundKey);
			}
		}
	}

	m_Dialogues.emplace(wNPCKey, dialogues);
	m_DialogueSounds.emplace(wNPCKey, dialogueSounds);

	return S_OK;
}

HRESULT CDialogueMgr::BindDialogueUI()
{
	shared_ptr<CUIInstanceDialogue> pUI = dynamic_pointer_cast<CUIInstanceDialogue>(UIMGR->FindUI("Dialogue"));
	if (pUI == nullptr)
		return E_FAIL;

	m_pDialogueUI = pUI;

	return S_OK;
}
