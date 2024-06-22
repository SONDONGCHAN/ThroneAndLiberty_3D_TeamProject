#include "QuestMgr.h"
#include "UIMgr.h"
#include "UIInstanceQuest.h"
#include "Monster.h"
#include "Player.h"
#include "Polymorph.h"
#include "UIInstanceIndicator.h"
#include "CharacterMgr.h"
#include "UIPanel.h"
#include "ItemMgr.h"
#include "ItemMisc.h"
#include "ItemEquip.h"
#include "ItemUsable.h"
#include "UIQuestComplete.h"
#include "UIQuestComplete.h"
#include "GameMgr.h"
#include "UIBackGround.h"

#include "NPC_Henry.h"
#include "NPC_Lottie.h"
#include "NPC_JaniceCarter.h"
#include "NPC_Lukas.h"
#include "NPC_Davinci.h"
#include "NPC_Robert.h"
#include "NPC_LeonardA.h"
#include "NPC_RuffaloRice.h"
#include "NPC_Elleia.h"
#include "NPC_David.h"
#include "NPC_RobertGhost.h"
#include "NPC_Ashien.h"
#include "NPC_MonsterDavid.h"
#include "NPC_Ashien.h"

#include "GameInstance.h"
#include <fstream>


IMPLEMENT_SINGLETON(Client::CQuestMgr)

CQuestMgr::CQuestMgr()
{

}

CQuestMgr::~CQuestMgr()
{

}

HRESULT CQuestMgr::Initialize()
{
	//m_QuestContents.resize(QUEST_END);

	if (FAILED(BindQuestUI()))
		return E_FAIL;

	if (FAILED(TestBindQuest()))
		return E_FAIL;

	if (FAILED(PreLoadJsonFiles(TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/QuestData/"))))
		return E_FAIL;

	/*if (FAILED(PrepareNPCs()))
		return E_FAIL;*/

	UpdateQuest(QUEST_MAIN, L"메인0");
	//UpdateQuest(QUEST_SUB0, L"마을서브1");
	//UpdateQuest(QUEST_SUB1, L"마을서브2-1");


	//UpdateQuest(QUEST_MAIN, L"메인2");

	//m_pPoly = CCharacterMgr::GetInstance()->GetCurPolymorph();

	return S_OK;
}

void CQuestMgr::Tick(_float _fDeltaTime)
{
	QuestKeyInput();

 	if (m_curBindMainQuest != nullptr)
	{
		if (m_curBindMainQuest(CHARACTERMGR->GetCurPolymorph().lock(), m_pMainObjects, m_strCurrentMainEvent))
		{
			UpdateQuestContent(QUEST_MAIN, m_strCurrentMain, L"퀘스트가 완료되었습니다.");

			UIMGR->ActivateQuestPanelEffect(QUEST_MAIN);
			UIMGR->ActivateQuestIconEffect(QUEST_MAIN);
			UIMGR->SetCurQuestNPC(QUEST_MAIN, nullptr);

			if (m_curMainQuestReward == nullptr && m_curMainNextQuest == nullptr)
			{
				if (m_pQuestUI.lock()->IsInstanceMouseOn() == 7 && GAMEINSTANCE->MouseDown(DIM_LB))
				{
					m_pMainObjects.clear();
					m_curBindMainQuest = nullptr;

					shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("QuestCompleteBGs"));
					shared_ptr<CUIQuestComplete> pQuestComplete = dynamic_pointer_cast<CUIQuestComplete>(UIMGR->FindUI("QuestCompleteInstance"));
					pQuestComplete->SetQuestTitle(m_MainQuestTitle);
					pUI->SetIsRender(true);
					UIMGR->DeactivateQuestPanelEffect(QUEST_MAIN);
					UIMGR->DeactivateQuestIconEffect(QUEST_MAIN);
					CGameMgr::GetInstance()->ChangeToNormalMouse();
				}
			}

			if (m_pQuestUI.lock()->IsInstanceMouseOn() == 7 && GAMEINSTANCE->MouseDown(DIM_LB))
			{
				if (m_curMainQuestReward != nullptr)
				{
					if (m_curMainQuestReward(CHARACTERMGR->GetCurPolymorph().lock(), m_pMainObjects, m_strCurrentMainEvent))
					{
						m_pMainObjects.clear();
						m_curMainQuestReward = nullptr;
						m_curBindMainQuest = nullptr;
					}
				}

				if (m_curMainNextQuest != nullptr)
				{
					if (m_curMainNextQuest(CHARACTERMGR->GetCurPolymorph().lock(), m_pMainObjects, m_strCurrentMainEvent))
					{
						m_pMainObjects.clear();
					}
				}

				shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("QuestCompleteBGs"));
				shared_ptr<CUIQuestComplete> pQuestComplete = dynamic_pointer_cast<CUIQuestComplete>(UIMGR->FindUI("QuestCompleteInstance"));
				pQuestComplete->SetQuestTitle(m_MainQuestTitle);
				pUI->SetIsRender(true);
				UIMGR->DeactivateQuestPanelEffect(QUEST_MAIN);
				UIMGR->DeactivateQuestIconEffect(QUEST_MAIN);
				CGameMgr::GetInstance()->ChangeToNormalMouse();
			}
		}
		else
		{
			UpdateHuntingContent(QUEST_MAIN, m_strCurrentMain);
			UpdateEventContent(QUEST_MAIN, m_strCurrentMain);
			m_MainQuestTitle = m_strCurrentMain;
		}
	}

	else
	{
		UpdateQuestContent(QUEST_MAIN, L"", L"");
	}

	if (m_curBindSubQuest0 != nullptr)
	{
		if (m_curBindSubQuest0(CHARACTERMGR->GetCurPolymorph().lock(), m_pSubObjects0, m_strCurrentSubEvent0))
		{
			//if (m_curSub0Completed == true)
			//{
			//	m_curBindSubQuest0 = nullptr;
			//	m_curSubQuest0Reward = nullptr;
			//	m_curSub0Completed = false;
			//}

			UpdateQuestContent(QUEST_SUB0, m_strCurrentSub0, L"퀘스트가 완료되었습니다.");

			UIMGR->ActivateQuestPanelEffect(QUEST_SUB0);
			UIMGR->ActivateQuestIconEffect(QUEST_SUB0);
			UIMGR->SetCurQuestNPC(QUEST_SUB0, nullptr);

			if (m_curSubQuest0Reward == nullptr && m_curSubNextQuest0 == nullptr)
			{
				if (m_pQuestUI.lock()->IsInstanceMouseOn() == 12 && GAMEINSTANCE->MouseDown(DIM_LB))
				{
					m_pSubObjects0.clear();
					m_curBindSubQuest0 = nullptr;

					shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("QuestCompleteBGs"));
					shared_ptr<CUIQuestComplete> pQuestComplete = dynamic_pointer_cast<CUIQuestComplete>(UIMGR->FindUI("QuestCompleteInstance"));
					pQuestComplete->SetQuestTitle(m_SubQuest0Title);
					pUI->SetIsRender(true);
					UIMGR->DeactivateQuestPanelEffect(QUEST_SUB0);
					UIMGR->DeactivateQuestIconEffect(QUEST_SUB0);
					CGameMgr::GetInstance()->ChangeToNormalMouse();

					//m_curSub0Completed = true;
				}
			}

			if (m_pQuestUI.lock()->IsInstanceMouseOn() == 12 && GAMEINSTANCE->MouseDown(DIM_LB))
			{
				if (m_curSubQuest0Reward != nullptr)
				{
					if (m_curSubQuest0Reward(CHARACTERMGR->GetCurPolymorph().lock(), m_pSubObjects0, m_strCurrentSubEvent0))
					{
						m_pSubObjects0.clear();
						m_curSubQuest0Reward = nullptr;
						m_curBindSubQuest0 = nullptr;

						//m_curSub0Completed = true;
					}
				}

				if (m_curSubNextQuest0 != nullptr)
				{
					if (m_curSubNextQuest0(CHARACTERMGR->GetCurPolymorph().lock(), m_pSubObjects0, m_strCurrentSubEvent0))
					{
						m_pSubObjects0.clear();
					}
				}
				else
				{
					//m_curSub0Completed = true;
					m_curBindSubQuest0 = nullptr;
					UpdateQuestContent(QUEST_SUB0, L"", L"");
				}

				shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("QuestCompleteBGs"));
				shared_ptr<CUIQuestComplete> pQuestComplete = dynamic_pointer_cast<CUIQuestComplete>(UIMGR->FindUI("QuestCompleteInstance"));
				pQuestComplete->SetQuestTitle(m_SubQuest0Title);
				pUI->SetIsRender(true);
				UIMGR->DeactivateQuestPanelEffect(QUEST_SUB0);
				UIMGR->DeactivateQuestIconEffect(QUEST_SUB0);
				CGameMgr::GetInstance()->ChangeToNormalMouse();
			}
		}

		else
		{
			UpdateHuntingContent(QUEST_SUB0, m_strCurrentSub0);
			UpdateEventContent(QUEST_SUB0, m_strCurrentSub0);
			m_SubQuest0Title = m_strCurrentSub0;
		}
	}

	else
	{
		UpdateQuestContent(QUEST_SUB0, L"", L"");
	}

	if (m_curBindSubQuest1 != nullptr)
	{
		if (m_curBindSubQuest1(CHARACTERMGR->GetCurPolymorph().lock(), m_pSubObjects1, m_strCurrentSubEvent1))
		{
			/*if (m_curSub1Completed == true)
			{
				m_curBindSubQuest1 = nullptr;
				m_curSubQuest1Reward = nullptr;
				m_curSub1Completed = false;
			}*/

			UpdateQuestContent(QUEST_SUB1, m_strCurrentSub1, L"퀘스트가 완료되었습니다.");

			UIMGR->ActivateQuestPanelEffect(QUEST_SUB1);
			UIMGR->ActivateQuestIconEffect(QUEST_SUB1);
			UIMGR->SetCurQuestNPC(QUEST_SUB1, nullptr);

			if (m_curSubQuest1Reward == nullptr && m_curSubNextQuest1 == nullptr)
			{
				if (m_pQuestUI.lock()->IsInstanceMouseOn() == 16 && GAMEINSTANCE->MouseDown(DIM_LB))
				{
					m_pSubObjects1.clear();
					m_curBindSubQuest1 = nullptr;

					shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("QuestCompleteBGs"));
					shared_ptr<CUIQuestComplete> pQuestComplete = dynamic_pointer_cast<CUIQuestComplete>(UIMGR->FindUI("QuestCompleteInstance"));
					pQuestComplete->SetQuestTitle(m_SubQuest1Title);
					pUI->SetIsRender(true);
					UIMGR->DeactivateQuestPanelEffect(QUEST_SUB1);
					UIMGR->DeactivateQuestIconEffect(QUEST_SUB1);
					CGameMgr::GetInstance()->ChangeToNormalMouse();

					//m_curSub1Completed = true;
				}
			}

			if (m_pQuestUI.lock()->IsInstanceMouseOn() == 16 && GAMEINSTANCE->MouseDown(DIM_LB))
			{
				if (m_curSubQuest1Reward != nullptr)
				{
					if (m_curSubQuest1Reward(CHARACTERMGR->GetCurPolymorph().lock(), m_pSubObjects1, m_strCurrentSubEvent1))
					{
						m_pSubObjects1.clear();
						m_curSubQuest1Reward = nullptr;
						m_curBindSubQuest1 = nullptr;

						//m_curSub1Completed = true;
					}
				}

				if (m_curSubNextQuest1 != nullptr)
				{
					if (m_curSubNextQuest1(CHARACTERMGR->GetCurPolymorph().lock(), m_pSubObjects1, m_strCurrentSubEvent1))
					{
						m_pSubObjects1.clear();
					}
				}
				else
				{
					//m_curSub1Completed = true;
					m_curBindSubQuest1 = nullptr;
					UpdateQuestContent(QUEST_SUB1, L"", L"");
				}

				shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("QuestCompleteBGs"));
				shared_ptr<CUIQuestComplete> pQuestComplete = dynamic_pointer_cast<CUIQuestComplete>(UIMGR->FindUI("QuestCompleteInstance"));
				pQuestComplete->SetQuestTitle(m_SubQuest1Title);
				pUI->SetIsRender(true);
				UIMGR->DeactivateQuestPanelEffect(QUEST_SUB1);
				UIMGR->DeactivateQuestIconEffect(QUEST_SUB1);
				CGameMgr::GetInstance()->ChangeToNormalMouse();

			}
		}

		else
		{
			UpdateHuntingContent(QUEST_SUB1, m_strCurrentSub1);
			UpdateEventContent(QUEST_SUB1, m_strCurrentSub1);
			m_SubQuest1Title = m_strCurrentSub1;
			if (m_strCurrentSub1 == L"루카스 구출" || m_strCurrentSub1 == L"풋내기 모험가")
				UIMGR->SetCurQuestNPC(QUEST_SUB1, nullptr);
		}
	}

	else
	{
		UpdateQuestContent(QUEST_SUB1, L"", L"");
	}

	m_strCurrentMainEvent = "";
	m_strCurrentSubEvent0 = "";
	m_strCurrentSubEvent1 = "";
}

void CQuestMgr::CheckDeadMonster(shared_ptr<CGameObject> _pMonster)
{
	if (_pMonster && m_curBindMainQuest)
		m_pMainObjects.emplace_back(_pMonster);

	if (_pMonster && m_curBindSubQuest0)
		m_pSubObjects0.emplace_back(_pMonster);

	if (_pMonster && m_curBindSubQuest1)
		m_pSubObjects1.emplace_back(_pMonster);
}

void CQuestMgr::CheckDialogNPC(shared_ptr<CGameObject> _pNPC)
{
	if (_pNPC && m_curBindMainQuest)
		m_pMainObjects.emplace_back(_pNPC);

	if (_pNPC && m_curBindSubQuest0)
		m_pSubObjects0.emplace_back(_pNPC);

	if (_pNPC && m_curBindSubQuest1)
		m_pSubObjects1.emplace_back(_pNPC);
}

void CQuestMgr::ActiveEventTag(QUEST_TYPE _QuestType, string _strEventTag)
{
	if (QUEST_TYPE::QUEST_MAIN == _QuestType)
	{
		m_strCurrentMainEvent = _strEventTag;
	}
	else if (QUEST_TYPE::QUEST_SUB0 == _QuestType)
	{
		m_strCurrentSubEvent0 = _strEventTag;
	}
	else if (QUEST_TYPE::QUEST_SUB1 == _QuestType)
	{
		m_strCurrentSubEvent1 = _strEventTag;
	}
}

shared_ptr<CNPC> CQuestMgr::GetNPC(const string& _strNPCKey)
{
	auto it = m_pNPCs.find(_strNPCKey);
	if (it == m_pNPCs.end())
		return nullptr;

	return m_pNPCs[_strNPCKey];
}

void CQuestMgr::AddQuest(QUEST_TYPE _questType, const wstring& _strKey, function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)>& _questContent)
{
	if (_questContent == nullptr)
		return;

	switch (_questType)
	{
	case QUEST_MAIN:
	{
		m_bindMainQuests.emplace(_strKey, _questContent);
	}
	break;
	case QUEST_SUB0:
	{
		m_bindSubQuests.emplace(_strKey, _questContent);
	}
	break;
	case QUEST_SUB1:
	{
		m_bindSubQuests.emplace(_strKey, _questContent);
	}
	break;
	}
}

void CQuestMgr::AddQuestReward(const wstring& _strKey, function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)>& _questContent)
{
	if (_questContent != nullptr)
		m_QuestRewards.emplace(_strKey, _questContent);
}

void CQuestMgr::AddNextQuest(const wstring& _strKey, function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)>& _questContent)
{
	if (_questContent != nullptr)
		m_NextQuests.emplace(_strKey, _questContent);
}

void CQuestMgr::DeleteQuest(QUEST_TYPE _questType, wstring _strQuestTag)
{
	switch (_questType)
	{
	case QUEST_MAIN:
	{
		auto questIt = m_bindMainQuests.find(_strQuestTag);
		if (questIt != m_bindMainQuests.end())
			m_bindMainQuests.erase(questIt);

		auto displayIt = m_QuestContents.find(_strQuestTag);
		if (displayIt != m_QuestContents.end())
			m_QuestContents.erase(displayIt);

		auto rewardIt = m_QuestRewards.find(_strQuestTag);
		if (rewardIt != m_QuestRewards.end())
			m_QuestRewards.erase(rewardIt);

		auto nextQuestIt = m_NextQuests.find(_strQuestTag);
		if (nextQuestIt != m_NextQuests.end())
			m_NextQuests.erase(nextQuestIt);
	}
	break;
	case QUEST_SUB0:
	{
		auto questIt = m_bindSubQuests.find(_strQuestTag);
		if (questIt != m_bindSubQuests.end())
			m_bindSubQuests.erase(questIt);

		auto displayIt = m_QuestContents.find(_strQuestTag);
		if (displayIt != m_QuestContents.end())
			m_QuestContents.erase(displayIt);

		auto rewardIt = m_QuestRewards.find(_strQuestTag);
		if (rewardIt != m_QuestRewards.end())
			m_QuestRewards.erase(rewardIt);

		auto nextQuestIt = m_NextQuests.find(_strQuestTag);
		if (nextQuestIt != m_NextQuests.end())
			m_NextQuests.erase(nextQuestIt);
	}
	break;
	case QUEST_SUB1:
	{
		auto questIt = m_bindSubQuests.find(_strQuestTag);
		if (questIt != m_bindSubQuests.end())
			m_bindSubQuests.erase(questIt);

		auto displayIt = m_QuestContents.find(_strQuestTag);
		if (displayIt != m_QuestContents.end())
			m_QuestContents.erase(displayIt);

		auto rewardIt = m_QuestRewards.find(_strQuestTag);
		if (rewardIt != m_QuestRewards.end())
			m_QuestRewards.erase(rewardIt);

		auto nextQuestIt = m_NextQuests.find(_strQuestTag);
		if (nextQuestIt != m_NextQuests.end())
			m_NextQuests.erase(nextQuestIt);
	}
	break;
	}
}

void CQuestMgr::AddNPC(const string& _strNPCKey, shared_ptr<CNPC> _pNPC)
{
	auto it = m_pNPCs.find(_strNPCKey);
	if (it != m_pNPCs.end())
		return;

	m_pNPCs.emplace(_strNPCKey, _pNPC);
}

void CQuestMgr::ClearNPCs()
{
	m_pNPCs.clear();
}

wstring CQuestMgr::GetCurrentQuestKey(QUEST_TYPE _questType)
{

	if (QUEST_TYPE::QUEST_MAIN == _questType) {

		return m_strCurrentMain;
	}
	else if (QUEST_TYPE::QUEST_SUB0 == _questType) {
		return m_strCurrentSub0;
	}
	else if (QUEST_TYPE::QUEST_SUB1 == _questType) {

		return m_strCurrentSub1;
	}

	return wstring();
}

_bool CQuestMgr::IsQuestBinded(const wstring& _strNPCKey)
{
	auto it = m_QuestContents.find(_strNPCKey);
	if (it == m_QuestContents.end())
		return false;

	return true;
}

_bool CQuestMgr::IsMainQuestAvailable()
{
	if (m_curBindMainQuest == nullptr)
		return true;

	return false;
}

_bool CQuestMgr::IsSubQuestAvailable()
{
	if (m_curBindSubQuest0 == nullptr || m_curBindSubQuest1 == nullptr)
		return true;

	return false;
}

void CQuestMgr::UpdateQuestContent(QUEST_TYPE _questType, const wstring& _questTitle, const wstring& _questContent, const string& _strFont, _float4 _vTitleFontColor, _float4 _vContentFontColor)
{
	if (!m_pQuestUI.expired())
	{
		m_pQuestUI.lock()->UpdateQuestContent(_questType, _questTitle, _questContent, _strFont, _vTitleFontColor, _vContentFontColor);
		switch (_questType)
		{
		case QUEST_MAIN:
		{
			m_curMainQuestContent = _questContent;
		}
		break;
		case QUEST_SUB0:
		{
			m_curSubQuest0Content = _questContent;
		}
		break;
		case QUEST_SUB1:
		{
			m_curSubQuest1Content = _questContent;
		}
		break;
		}
	}
}

HRESULT CQuestMgr::PreLoadJsonFiles(const wstring& _strPath)
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

HRESULT CQuestMgr::LoadJsonFile(Json::Value& root, const string& _strTag)
{
	/*Json::Value subRoot = root[_strTag];

	if (_strTag == "MainQuests")
	{
		_uint iNumCount{ 0 };
		for (Json::ValueIterator it = subRoot.begin(); it != subRoot.end(); ++it)
		{
			string strKey = "Main Quest" + to_string(iNumCount);
			Json::Value MainQuest = subRoot[strKey];
			string questTitle = MainQuest["QuestTitle"].asString();
			string questContent = MainQuest["QuestContent"].asString();

			_tchar wQuestKey[MAX_PATH] = TEXT("");
			_tchar wQuestTitle[MAX_PATH] = TEXT("");
			_tchar wQuestContent[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_UTF8, 0, strKey.c_str(), static_cast<_int>(strKey.size()), wQuestKey, MAX_PATH);
			MultiByteToWideChar(CP_UTF8, 0, questTitle.c_str(), static_cast<_int>(questTitle.size()), wQuestTitle, MAX_PATH);
			MultiByteToWideChar(CP_UTF8, 0, questContent.c_str(), static_cast<_int>(questContent.size()), wQuestContent, MAX_PATH);

			pair<wstring, wstring> quest = make_pair(wQuestTitle, wQuestContent);

			m_QuestContents.emplace(wQuestKey, quest);

			iNumCount++;
		}
	}

	else if (_strTag == "SubQuests")
	{
		_uint iNumCount{ 0 };
		for (Json::ValueIterator it = subRoot.begin(); it != subRoot.end(); ++it)
		{
			string strKey = "Sub Quest" + to_string(iNumCount);
			Json::Value MainQuest = subRoot[strKey];
			string questTitle = MainQuest["QuestTitle"].asString();
			string questContent = MainQuest["QuestContent"].asString();

			_tchar wQuestKey[MAX_PATH] = TEXT("");
			_tchar wQuestTitle[MAX_PATH] = TEXT("");
			_tchar wQuestContent[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_UTF8, 0, strKey.c_str(), static_cast<_int>(strKey.size()), wQuestKey, MAX_PATH);
			MultiByteToWideChar(CP_UTF8, 0, questTitle.c_str(), static_cast<_int>(questTitle.size()), wQuestTitle, MAX_PATH);
			MultiByteToWideChar(CP_UTF8, 0, questContent.c_str(), static_cast<_int>(questContent.size()), wQuestContent, MAX_PATH);

			pair<wstring, wstring> quest = make_pair(wQuestTitle, wQuestContent);

			m_QuestContents.emplace(wQuestKey, quest);

			iNumCount++;
		}
	}*/

	Json::Value subRoot = root[_strTag];

	Json::Value MainQuest = subRoot["Main Quests"];
	if (MainQuest.isArray())
	{
		for (auto& quest : MainQuest)
		{
			QUEST_TYPE questType = static_cast<QUEST_TYPE>(quest["Quest Type"].asUInt());
			string strQuestKey = quest["Quest Key"].asString();
			string strQuestTitle = quest["Quest Title"].asString();
			string strQuestContent = quest["Quest Content"].asString();

			_tchar wQuestKey[MAX_PATH] = TEXT("");
			_tchar wQuestTitle[MAX_PATH] = TEXT("");
			_tchar wQuestContent[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_UTF8, 0, strQuestKey.c_str(), static_cast<_int>(strQuestKey.size()), wQuestKey, MAX_PATH);
			MultiByteToWideChar(CP_UTF8, 0, strQuestTitle.c_str(), static_cast<_int>(strQuestTitle.size()), wQuestTitle, MAX_PATH);
			MultiByteToWideChar(CP_UTF8, 0, strQuestContent.c_str(), static_cast<_int>(strQuestContent.size()), wQuestContent, MAX_PATH);

			pair<wstring, wstring> quest = make_pair(wQuestTitle, wQuestContent);

			m_QuestContents.emplace(wQuestKey, quest);
		}
	}

	return S_OK;
}

void CQuestMgr::UpdateQuest(QUEST_TYPE _questType, const wstring _questKey)
{
#pragma region Legacy Code
	/*switch (_questType)
	{
	case QUEST_MAIN:
	{
		if (!m_bindMainQuests.empty())
		{
			wstring mainQuestTag = L"Main Quest";

			wstringstream ss;
			ss << mainQuestTag << m_iMainQuestIndex;

			wstring finalTag = ss.str();

			m_curBindMainQuest = m_bindMainQuests[finalTag];
			UpdateQuestContent(QUEST_MAIN, m_QuestContents[QUEST_MAIN][finalTag].first, m_QuestContents[QUEST_MAIN][finalTag].second);

			auto it = m_QuestRewards.find(finalTag);
			if (it != m_QuestRewards.end())
				m_curMainQuestReward = m_QuestRewards[finalTag];

			m_iMainQuestIndex++;

			DeleteQuest(QUEST_MAIN, finalTag);
		}

		else
			UpdateQuestContent(QUEST_MAIN, L"", L"");
	}
	break;
	case QUEST_SUB0:
	{
		if (!m_bindSubQuests.empty())
		{
			wstring subQuestTag = L"Sub Quest";

			wstringstream ss;
			ss << subQuestTag << m_iSubQuestIndex;

			wstring finalTag = ss.str();

			m_curBindSubQuest0 = m_bindSubQuests[finalTag];
			UpdateQuestContent(QUEST_SUB0, m_QuestContents[QUEST_SUB0][finalTag].first, m_QuestContents[QUEST_SUB0][finalTag].second);

			auto it = m_QuestRewards.find(finalTag);
			if (it != m_QuestRewards.end())
				m_curSubQuest0Reward = m_QuestRewards[finalTag];

			m_iSubQuestIndex++;

			DeleteQuest(QUEST_SUB0, finalTag);
		}

		else
			UpdateQuestContent(QUEST_SUB0, L"", L"");
	}
	break;
	case QUEST_SUB1:
	{
		if (!m_bindSubQuests.empty())
		{
			wstring subQuestTag = L"Sub Quest";

			wstringstream ss;
			ss << subQuestTag << m_iSubQuestIndex;

			wstring finalTag = ss.str();

			m_curBindSubQuest1 = m_bindSubQuests[finalTag];
			UpdateQuestContent(QUEST_SUB1, m_QuestContents[QUEST_SUB1][finalTag].first, m_QuestContents[QUEST_SUB1][finalTag].second);

			auto it = m_QuestRewards.find(finalTag);
			if (it != m_QuestRewards.end())
				m_curSubQuest1Reward = m_QuestRewards[finalTag];

			m_iSubQuestIndex++;

			DeleteQuest(QUEST_SUB1, finalTag);
		}

		else
			UpdateQuestContent(QUEST_SUB1, L"", L"");
	}
	break;
	}*/
#pragma endregion

	// 중복 재생 안되게 바꿀 것
	//GAMEINSTANCE->PlaySoundW("UI_Text_Mission_Draw_01_A", 1.f);

	if (_questType == QUEST_MAIN)
	{
		if (!m_bindMainQuests.empty())
		{
			wstring questKey = _questKey;

			auto it = m_bindMainQuests.find(questKey);
			if (it != m_bindMainQuests.end())
				m_curBindMainQuest = m_bindMainQuests[questKey];
			else
				m_curBindMainQuest = nullptr;

			auto it1 = m_QuestContents.find(questKey);
			if (it1 != m_QuestContents.end()) {
				UpdateQuestContent(QUEST_MAIN, m_QuestContents[questKey].first, m_QuestContents[questKey].second);
				m_strCurrentMain = m_QuestContents[questKey].first;

				for (auto& pNPC : m_pNPCs)
				{
					if (pNPC.second)
						pNPC.second->SetCurMainQuestKey(m_QuestContents[questKey].first);
				}

				//m_MainQuestTitle = m_QuestContents[questKey].first;
			}
			else {
				m_strCurrentMain = TEXT("");
			}

			auto it2 = m_QuestRewards.find(questKey);
			if (it2 != m_QuestRewards.end())
				m_curMainQuestReward = m_QuestRewards[questKey];
			else
				m_curMainQuestReward = nullptr;

			auto it3 = m_NextQuests.find(questKey);
			if (it3 != m_NextQuests.end())
			{
				m_curMainNextQuest = m_NextQuests[questKey];
			}
			else
				m_curMainNextQuest = nullptr;

			DeleteQuest(QUEST_MAIN, questKey);
		}

		else
			UpdateQuestContent(QUEST_MAIN, L"", L"");
	}

	else if (_questType == QUEST_SUB0)
	{
		wstring questKey = _questKey;

		if (m_bindSubQuests.empty() && m_curBindSubQuest0 == nullptr)
			UpdateQuestContent(QUEST_SUB0, L"", L"");

		if (!m_bindSubQuests.empty())
		{
			auto it = m_bindSubQuests.find(questKey);
			if (it != m_bindSubQuests.end())
				m_curBindSubQuest0 = m_bindSubQuests[questKey];
			else
				m_curBindSubQuest0 = nullptr;

			auto it1 = m_QuestContents.find(questKey);
			if (it1 != m_QuestContents.end()) {
				UpdateQuestContent(QUEST_SUB0, m_QuestContents[questKey].first, m_QuestContents[questKey].second);
				m_strCurrentSub0 = m_QuestContents[questKey].first;

				for (auto& pNPC : m_pNPCs)
					if (pNPC.second)
						pNPC.second->SetCurSubQuest0Key(m_QuestContents[questKey].first);

				//m_SubQuest0Title = m_QuestContents[questKey].first;
			}
			else {
				m_strCurrentSub0 = TEXT("");
			}

			auto it2 = m_QuestRewards.find(questKey);
			if (it2 != m_QuestRewards.end())
				m_curSubQuest0Reward = m_QuestRewards[questKey];
			else
				m_curSubQuest0Reward = nullptr;

			auto it3 = m_NextQuests.find(questKey);
			if (it3 != m_NextQuests.end())
				m_curSubNextQuest0 = m_NextQuests[questKey];
			else
				m_curSubNextQuest0 = nullptr;

			DeleteQuest(QUEST_SUB0, questKey);
		}
	}

	else if (_questType == QUEST_SUB1)
	{
		wstring questKey = _questKey;

		if (m_bindSubQuests.empty() && m_curBindSubQuest1 == nullptr)
			UpdateQuestContent(QUEST_SUB1, L"", L"");

		else if (!m_bindSubQuests.empty())
		{
			wstring questKey = _questKey;

			auto it = m_bindSubQuests.find(questKey);
			if (it != m_bindSubQuests.end())
				m_curBindSubQuest1 = m_bindSubQuests[questKey];
			else
				m_curBindSubQuest1 = nullptr;

			auto it1 = m_QuestContents.find(questKey);
			if (it1 != m_QuestContents.end()) {
				UpdateQuestContent(QUEST_SUB1, m_QuestContents[questKey].first, m_QuestContents[questKey].second);
				m_strCurrentSub1 = m_QuestContents[questKey].first;

				for (auto& pNPC : m_pNPCs)
					if (pNPC.second)
						pNPC.second->SetCurSubQuest1Key(m_QuestContents[questKey].first);

				//m_SubQuest1Title = m_QuestContents[questKey].first;
			}
			else {
				m_strCurrentSub1 = TEXT("");
			}

			auto it2 = m_QuestRewards.find(questKey);
			if (it2 != m_QuestRewards.end())
				m_curSubQuest1Reward = m_QuestRewards[questKey];
			else
				m_curSubQuest1Reward = nullptr;

			auto it3 = m_NextQuests.find(questKey);
			if (it3 != m_NextQuests.end())
				m_curSubNextQuest1 = m_NextQuests[questKey];
			else
				m_curSubNextQuest1 = nullptr;

			DeleteQuest(QUEST_SUB1, questKey);
		}
	}
}

void CQuestMgr::UpdateHuntingQuest(QUEST_TYPE _questType, wstring _strQuestKey, string _strTargetKey, _uint _count)
{
	auto it = m_multipleTargets.find(_strQuestKey);
	if (it == m_multipleTargets.end())
	{
		unordered_map<string, pair<string, pair<_uint, _uint>>> multipleTargets;
		
		pair<string, pair<_uint, _uint>> targetPair = make_pair(_strTargetKey, make_pair(0, _count));

		multipleTargets.emplace(_strTargetKey, targetPair);

		m_multipleTargets.emplace(_strQuestKey, multipleTargets);
	}

	else
	{
		unordered_map<string, pair<string, pair<_uint, _uint>>> multipleTargets;
		multipleTargets = m_multipleTargets[_strQuestKey];

		auto multiIt = multipleTargets.find(_strTargetKey);
		if (multiIt == multipleTargets.end())
		{
			pair<string, pair<_uint, _uint>> targetPair = make_pair(_strTargetKey, make_pair(0, _count));

			m_multipleTargets[_strQuestKey].emplace(_strTargetKey, targetPair);
		}

		else
		{
			m_multipleTargets[_strQuestKey][_strTargetKey].second.first += _count;
		}
	}
}

void CQuestMgr::UpdateHuntingContent(QUEST_TYPE _questType, wstring _strQuestKey, const string& _strFont, _float4 _vTitleFontColor, _float4 _vContentFontColor)
{
	if (!m_pQuestUI.expired())
	{
		auto it = m_multipleTargets.find(_strQuestKey);
		if (it == m_multipleTargets.end())
			return;

		unordered_map<string, pair<string, pair<_uint, _uint>>> multipleTargets = it->second;

		wstring huntingContent;

		for (auto& targetKey : multipleTargets)
		{
			wstring displayKey;
			/* 퀘스트에 표시될 몬스터 이름 */
			if (targetKey.first == "LivingArmor_Soldier")
				displayKey = L"리빙아머 전사";
			else if (targetKey.first == "LivingArmor_Archer")
				displayKey = L"리빙아머 궁수";
			else if (targetKey.first == "LivingArmor_Mage")
				displayKey = L"리빙아머 대마법사";
			else if (targetKey.first == "L1Midget_Sling")
				displayKey = L"고블린 돌팔매꾼";
			else if (targetKey.first == "L1Midget_Sling")
				displayKey = L"고블린 돌팔매꾼";
			else if (targetKey.first == "L1Midget_Shaman")
				displayKey = L"고블린 주술사";
			else if (targetKey.first == "L1Midget_Footman")
				displayKey = L"고블린 역사";
			else if (targetKey.first == "M_Orc_Warrior_001")
				displayKey = L"오크 전사";
			else if (targetKey.first == "M_Orc_Archer")
				displayKey = L"오크 궁수";
			else if (targetKey.first == "M_Orc_Shaman")
				displayKey = L"오크 주술사";
			else if (targetKey.first == "M_Orc_LowDarkCommander")
				displayKey = L"블랙 오크 지휘관";
			else if (targetKey.first == "M_L1Midget_Berserker")
				displayKey = L"대왕 고블린 버서커";
			else if (targetKey.first == "M_UndeadArmy_Knight")
				displayKey = L"적기사";

			wstring monsterCount = displayKey + L"(" + to_wstring(targetKey.second.second.first) + L" / " + to_wstring(targetKey.second.second.second) + L")\n";

			huntingContent += monsterCount;
		}

		m_pQuestUI.lock()->UpdateQuestContentNonAuto(_questType, m_strCurrentMain, huntingContent, _strFont, _vTitleFontColor, _vContentFontColor);
	}
}

void CQuestMgr::UpdateEventActiveQuest(QUEST_TYPE _questType, wstring _strQuestKey, _uint _count)
{
	auto it = m_multipleEvents.find(_strQuestKey);
	if (it == m_multipleEvents.end())
	{
		pair<_uint, _uint> eventPair = make_pair(0, _count);

		m_multipleEvents.emplace(_strQuestKey, eventPair);
	}

	else
	{
		m_multipleEvents[_strQuestKey].first += _count;
	}
}

void CQuestMgr::UpdateEventContent(QUEST_TYPE _questType, wstring _strQuestKey, const string& _strFont, _float4 _vTitleFontColor, _float4 _vContentFontColor)
{
	if (!m_pQuestUI.expired())
	{
		auto it = m_multipleEvents.find(_strQuestKey);
		if (it == m_multipleEvents.end())
			return;

		pair<_uint, _uint> multipleEvents = it->second;

		wstring eventContent;

		switch (_questType)
		{
		case QUEST_MAIN:
		{
			eventContent = m_curMainQuestContent + L" (" + to_wstring(multipleEvents.first) + L" / " + to_wstring(multipleEvents.second) + L")";
		}
		break;
		case QUEST_SUB0:
		{
			eventContent = m_curSubQuest0Content + L" (" + to_wstring(multipleEvents.first) + L" / " + to_wstring(multipleEvents.second) + L")";
		}
		break;
		case QUEST_SUB1:
		{
			eventContent = m_curSubQuest1Content + L" (" + to_wstring(multipleEvents.first) + L" / " + to_wstring(multipleEvents.second) + L")";
		}
		break;
		}

		m_pQuestUI.lock()->AppendQuestContent(_questType, eventContent);
	}
}

HRESULT CQuestMgr::BindQuestUI()
{
	shared_ptr<CUIInstanceQuest> pUI = dynamic_pointer_cast<CUIInstanceQuest>(UIMGR->FindUI("MainQuest"));
	if (pUI == nullptr)
		return E_FAIL;

	m_pQuestUI = pUI;

	return S_OK;
}

HRESULT CQuestMgr::TestBindQuest()
{
	class HuntingQuest
	{
	public:
		HuntingQuest(QUEST_TYPE _eQuestType, _uint _maxNum, string _strTagetTag)
			: m_eQuestType(_eQuestType), m_iMaxNum(_maxNum), m_strTargetModelTag(_strTagetTag)
		{

		}

	public:
		_bool operator()(shared_ptr<CPolymorph> _pPlayer, list<weak_ptr<CGameObject>>& _pMonsters, string& _strEventTag)
		{
			if (_pPlayer)
			{
				for (auto it = _pMonsters.begin(); it != _pMonsters.end();)
				{
					shared_ptr<CMonster> targetMonster = dynamic_pointer_cast<CMonster>((*it).lock());
					if (!targetMonster)
						continue;

					if (!targetMonster)
						continue;

					if (!targetMonster)
						continue;

					if (targetMonster->GetModelName() == m_strTargetModelTag && targetMonster->GetDead())
					{
						m_iInitNum += 1;

						if (m_iInitNum >= m_iMaxNum)
							return true;

						(*it).reset();
						it = _pMonsters.erase(it);
					}

					else
						++it;
				}
			}

			return false;
		}

	private:
		_uint m_iInitNum{ 0 };
		_uint m_iMaxNum{ 0 };
		string m_strTargetModelTag{ "" };

	public:
		QUEST_TYPE m_eQuestType{ QUEST_END };

	};

	class FollowQuest
	{
	public:
		FollowQuest(QUEST_TYPE _eQuestType, _float3 _vDestPos) : m_eQuestType(_eQuestType), m_vDestPos(_vDestPos) {}

		_bool operator()(shared_ptr<CPolymorph> _pPlayer, list<weak_ptr<CGameObject>>& _pObjects, string& _strEventTag)
		{
			if (_pPlayer)
			{
				m_fDist = SimpleMath::Vector3::Distance(_pPlayer->GETPOS, m_vDestPos);

				switch (m_eQuestType)
				{
				case QUEST_MAIN:
				{
					weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("MainQuestIndicatorPanel_0"));
					weak_ptr<CUIInstanceIndicator> pUI = dynamic_pointer_cast<CUIInstanceIndicator>(UIMGR->FindUI("MainQuestIndicatorUV_0"));
					if (!pUI.expired())
					{
						pUIPanel.lock()->SetIsRender(true);
						pUI.lock()->UpdateDistance(m_fDist);

						_float3 destPos = m_vDestPos + _float3(0.f, 1.5f, 0.f);
						destPos = SimpleMath::Vector3::Transform(destPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
						destPos = SimpleMath::Vector3::Transform(destPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

						if (destPos.z < 0.f || destPos.z > 1.f)
						{
							pUIPanel.lock()->SetIsRender(false);
						}

						else
						{
							_float posX = (destPos.x + 1.f) * 0.5f;
							_float posY = (destPos.y - 1.f) * -0.5f;

							posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
							posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

							pUIPanel.lock()->SetUIPos(posX, posY);
						}
					}

					if (m_fDist <= 10.f)
					{
						pUIPanel.lock()->SetIsRender(false);
						return true;
					}
				}
				break;
				case QUEST_SUB0:
				{
					weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("SubQuestIndicatorPanel0"));
					weak_ptr<CUIInstanceIndicator> pUI = dynamic_pointer_cast<CUIInstanceIndicator>(UIMGR->FindUI("SubQuestIndicatorUV0"));
					if (!pUI.expired())
					{
						pUIPanel.lock()->SetIsRender(true);
						pUI.lock()->UpdateDistance(m_fDist);

						_float3 destPos = m_vDestPos + _float3(0.f, 1.5f, 0.f);
						destPos = SimpleMath::Vector3::Transform(destPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
						destPos = SimpleMath::Vector3::Transform(destPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

						if (destPos.z < 0.f || destPos.z > 1.f)
						{
							pUIPanel.lock()->SetIsRender(false);
						}

						else
						{
							_float posX = (destPos.x + 1.f) * 0.5f;
							_float posY = (destPos.y - 1.f) * -0.5f;

							posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
							posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

							pUIPanel.lock()->SetUIPos(posX, posY);
						}
					}

					if (m_fDist <= 5.f)
					{
						pUIPanel.lock()->SetIsRender(false);
						return true;
					}
				}
				break;
				case QUEST_SUB1:
				{
					weak_ptr<CUIPanel> pUIPanel = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("SubQuestIndicatorPanel1"));
					weak_ptr<CUIInstanceIndicator> pUI = dynamic_pointer_cast<CUIInstanceIndicator>(UIMGR->FindUI("SubQuestIndicatorUV1"));
					if (!pUI.expired())
					{
						pUIPanel.lock()->SetIsRender(true);
						pUI.lock()->UpdateDistance(m_fDist);

						_float3 destPos = m_vDestPos + _float3(0.f, 1.5f, 0.f);
						destPos = SimpleMath::Vector3::Transform(destPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
						destPos = SimpleMath::Vector3::Transform(destPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

						if (destPos.z < 0.f || destPos.z > 1.f)
						{
							pUIPanel.lock()->SetIsRender(false);
						}

						else
						{
							_float posX = (destPos.x + 1.f) * 0.5f;
							_float posY = (destPos.y - 1.f) * -0.5f;

							posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
							posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

							pUIPanel.lock()->SetUIPos(posX, posY);
						}
					}

					if (m_fDist <= 5.f)
					{
						pUIPanel.lock()->SetIsRender(false);
						return true;
					}
				}
				break;
				}
			}

			return false;
		}

	private:
		_float3 m_vDestPos{};
		_float m_fDist{};

	private:
		QUEST_TYPE m_eQuestType{ QUEST_END };

	};

	/*NPC와의 대화 퀘스트*/
	class MeetNpcQuest
	{
	public:
		MeetNpcQuest(QUEST_TYPE _eQuestType, string _strTagetTag) : m_eQuestType(_eQuestType), m_strTargetModelTag(_strTagetTag) {}

		_bool operator()(shared_ptr<CPolymorph> _pPlayer, list<weak_ptr<CGameObject>>& _pObjects, string& _strEventTag)
		{
			shared_ptr<CNPC> pNPC = QUESTMGR->GetNPC(m_strTargetModelTag);
			if (pNPC != nullptr)
				UIMGR->SetCurQuestNPC(m_eQuestType, pNPC);

			if (_pPlayer)
			{
				switch (m_eQuestType)
				{
				case QUEST_MAIN:
				{
					for (auto& pObj : _pObjects) {

						shared_ptr<CNPC> pNPC = dynamic_pointer_cast<CNPC>(pObj.lock());

						if (!pNPC)
							continue;

						/*대화한 NPC와 타겟 NPC이름이 같다면*/
						if (m_strTargetModelTag == pNPC->GetModelName()) {
							UIMGR->SetCurQuestNPC(m_eQuestType, nullptr);

							return true;
						}
					}
				}
				break;
				case QUEST_SUB0:
				{
					for (auto& pObj : _pObjects) {

						shared_ptr<CNPC> pNPC = dynamic_pointer_cast<CNPC>(pObj.lock());
						if (!pNPC)
							continue;

						/*대화한 NPC와 타겟 NPC이름이 같다면*/
						if (m_strTargetModelTag == pNPC->GetModelName()) {
							UIMGR->SetCurQuestNPC(m_eQuestType, nullptr);

							return true;
						}
					}
				}
				break;
				case QUEST_SUB1:
				{
					for (auto& pObj : _pObjects) {

						shared_ptr<CNPC> pNPC = dynamic_pointer_cast<CNPC>(pObj.lock());

						if (!pNPC)
							continue;

						/*대화한 NPC와 타겟 NPC이름이 같다면*/
						if (m_strTargetModelTag == pNPC->GetModelName()) {
							UIMGR->SetCurQuestNPC(m_eQuestType, nullptr);

							return true;
						}
					}
					
				}
				break;
				}
			}

			return false;
		}

	private:
		string m_strTargetModelTag{ "" };

	private:
		QUEST_TYPE m_eQuestType{ QUEST_END };

	};

	/*다중 타겟 사냥 퀘스트*/
	class HuntingMultipleQuest
	{
	public:
		HuntingMultipleQuest(QUEST_TYPE _eQuestType, wstring _questKey, unordered_map<string, _uint> _pTargets)
			: m_eQuestType(_eQuestType), m_strQuestKey(_questKey), m_Targets(_pTargets)
		{
			for (auto& target : m_Targets)
				QUESTMGR->UpdateHuntingQuest(m_eQuestType, m_strQuestKey, target.first, target.second);
		}

	public:
		_bool operator()(shared_ptr<CPolymorph> _pPlayer, list<weak_ptr<CGameObject>>& _pMonsters, string& _strEventTag)
		{
			if (_pPlayer)
			{
				for (auto it = _pMonsters.begin(); it != _pMonsters.end();)
				{
					shared_ptr<CMonster> targetMonster = dynamic_pointer_cast<CMonster>((*it).lock());

					if (!targetMonster)
						continue;

					auto iter = m_Targets.find(targetMonster->GetModelName());

					if (m_Targets.end() != iter && targetMonster->GetDead())
					{
						iter->second -= 1;

						QUESTMGR->UpdateHuntingQuest(m_eQuestType, m_strQuestKey,iter->first, 1);

						if (iter->second <= 0) {
							m_Targets.erase(iter);
						}

						(*it).reset();
						it = _pMonsters.erase(it);

					}

					else
						++it;
				}

				if (m_Targets.empty())
					return true;

			}

			return false;
		}

	private:
		unordered_map<string, _uint> m_Targets;

	public:
		QUEST_TYPE m_eQuestType{ QUEST_END };
		wstring m_strQuestKey{ L"" };

	};

	/*특수 이벤트 + 특정 횟수 달성*/
	class EventActiveQuest
	{
	public:
		EventActiveQuest(QUEST_TYPE _eQuestType, wstring _questKey, string _strEventName, _uint _iMaxNum)
			: m_eQuestType(_eQuestType), m_strQuestKey(_questKey), m_strEventName(_strEventName), m_iMaxNum(_iMaxNum)
		{
			QUESTMGR->UpdateEventActiveQuest(m_eQuestType, m_strQuestKey, m_iMaxNum);
		}

	public:
		_bool operator()(shared_ptr<CPolymorph> _pPlayer, list<weak_ptr<CGameObject>>& _pMonsters, string& _strEventTag)
		{
			if (_pPlayer)
			{
				if (_strEventTag == m_strEventName) {

					++m_iCurrentAccNum;
					QUESTMGR->UpdateEventActiveQuest(m_eQuestType, m_strQuestKey, 1);
					
				}

				if (m_iMaxNum <= m_iCurrentAccNum)
					return true;
			}

			return false;
		}

	private:
		string m_strEventName = "";
		_uint m_iMaxNum = 0;
		_uint m_iCurrentAccNum = 0;

	public:
		QUEST_TYPE m_eQuestType{ QUEST_END };
		wstring m_strQuestKey{ L"" };

	};

	class QuestReward
	{
	public:
		QuestReward(shared_ptr<CItemBase> _pItem) : m_pItem(_pItem) {}

	public:
		_bool operator()(shared_ptr<CPolymorph> _pPlayer, list<weak_ptr<CGameObject>>& _pObjects, string& _strEventTag)
		{
			/* 보상 아이템 생성 */
			if (m_pItem == nullptr)
				return false;

			ITEMMGR->AddItemToInventory(m_pItem);
			shared_ptr<CUIQuestComplete> pFoundUI = dynamic_pointer_cast<CUIQuestComplete>(UIMGR->FindUI("QuestCompleteInstance"));
			if (pFoundUI != nullptr)
				pFoundUI->SetQuestRewardItem(m_pItem);

			return true;
		}

	private:
		shared_ptr<CItemBase> m_pItem{ nullptr };

	};

	class NextQuest
	{
	public:
		QUEST_TYPE m_eQuestType{ QUEST_END };
		wstring m_strNextQuest{ L"" };

	public:
		NextQuest(QUEST_TYPE _questType, wstring _strNextQuest) : m_eQuestType(_questType), m_strNextQuest(_strNextQuest) {}

	public:
		_bool operator()(shared_ptr<CPolymorph> _pPlayer, list<weak_ptr<CGameObject>>& _pObjects, string& _strEventTag)
		{
			QUESTMGR->UpdateQuest(m_eQuestType, m_strNextQuest);

			return true;
		}
	};

	/**************************************Main Quest************************************/
	{
		/*메인 0*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func = MeetNpcQuest(QUEST_MAIN, "CT_Named_ArthurTalon_SK");
		//function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func = FollowQuest(QUEST_MAIN, _float3(42.f, 13.f, 53.f));

		/*메인1*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func1 = MeetNpcQuest(QUEST_MAIN, "CT_Named_RuffaloRice_SK");

		/*메인2*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func2 = EventActiveQuest(QUEST_MAIN, TEXT("마을 구성원들에게 인사하기"), "Event_MeetResident", 4);

		/*메인3*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func3 = EventActiveQuest(QUEST_MAIN, TEXT("시작이 반이다"), "Event_Training", 1);

		/*메인4*/
		unordered_map<string, _uint> pTargets1;
		pTargets1.emplace("L1Midget_Footman", 3);
		pTargets1.emplace("L1Midget_Shaman", 2);
		pTargets1.emplace("L1Midget_Sling", 3);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func4 = HuntingMultipleQuest(QUEST_MAIN, TEXT("자격 증명(1)"), pTargets1);

		/*메인5*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func5 = HuntingQuest(QUEST_MAIN, 1, "M_L1Midget_Berserker");

		/*메인6*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func6 = MeetNpcQuest(QUEST_MAIN, "CT_Named_RuffaloRice_SK");

		/*메인7*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func7 = FollowQuest(QUEST_MAIN, _float3(337.98f, -70.11f, 4.95f));

		/*메인8*/

		unordered_map<string, _uint> pTargets2;
		pTargets2.emplace("M_Orc_Warrior_001", 2);
		pTargets2.emplace("M_Orc_Shaman", 2);
		pTargets2.emplace("M_Orc_Archer", 4);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func8 = HuntingMultipleQuest(QUEST_MAIN, TEXT("오크 소탕하기(1)"), pTargets2);

		/*메인9*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func9 = HuntingQuest(QUEST_MAIN, 1, "M_Orc_LowDarkCommander");

		/*메인10*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func10 = MeetNpcQuest(QUEST_MAIN, "CT_Named_RuffaloRice_SK");

		/*메인11*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func11 = EventActiveQuest(QUEST_MAIN, TEXT("던전으로 가는 길"), "Event_Catapult", 1);

		/*메인12*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func12 = FollowQuest(QUEST_MAIN, _float3(464.f, -67.f, 328.f));

		/*메인13*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> func13 = EventActiveQuest(QUEST_MAIN, TEXT("더 어두운 곳으로"), "Event_EnterUnderWorld", 1);


		m_bindMainQuests.emplace(TEXT("메인0"), func);
		m_bindMainQuests.emplace(TEXT("메인1"), func1);
		m_bindMainQuests.emplace(TEXT("메인2"), func2);
		m_bindMainQuests.emplace(TEXT("메인3"), func3);
		m_bindMainQuests.emplace(TEXT("메인4"), func4);
		m_bindMainQuests.emplace(TEXT("메인5"), func5);
		m_bindMainQuests.emplace(TEXT("메인6"), func6);
		m_bindMainQuests.emplace(TEXT("메인7"), func7);
		m_bindMainQuests.emplace(TEXT("메인8"), func8);
		m_bindMainQuests.emplace(TEXT("메인9"), func9);
		m_bindMainQuests.emplace(TEXT("메인10"), func10);
		m_bindMainQuests.emplace(TEXT("메인11"), func11);
		m_bindMainQuests.emplace(TEXT("메인12"), func12);
		m_bindMainQuests.emplace(TEXT("메인13"), func13);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc1 = NextQuest(QUEST_MAIN, L"메인1");
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc2 = NextQuest(QUEST_MAIN, L"메인2");
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc3 = NextQuest(QUEST_MAIN, L"메인3");
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc4 = NextQuest(QUEST_MAIN, L"메인4");
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc5 = NextQuest(QUEST_MAIN, L"메인5");
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc6 = NextQuest(QUEST_MAIN, L"메인6");
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc7 = NextQuest(QUEST_MAIN, L"메인7");
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc8 = NextQuest(QUEST_MAIN, L"메인8");
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc9 = NextQuest(QUEST_MAIN, L"메인9");
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc10 = NextQuest(QUEST_MAIN, L"메인10");
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc11 = NextQuest(QUEST_MAIN, L"메인11");
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc12 = NextQuest(QUEST_MAIN, L"메인12");
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc13 = NextQuest(QUEST_MAIN, L"메인13");

		/* Next Quest */
		m_NextQuests.emplace(TEXT("메인0"), NextQuestFunc1);
		m_NextQuests.emplace(TEXT("메인1"), NextQuestFunc2);
		m_NextQuests.emplace(TEXT("메인2"), NextQuestFunc3);
		m_NextQuests.emplace(TEXT("메인3"), NextQuestFunc4);
		m_NextQuests.emplace(TEXT("메인4"), NextQuestFunc5);
		m_NextQuests.emplace(TEXT("메인5"), NextQuestFunc6);
		m_NextQuests.emplace(TEXT("메인6"), NextQuestFunc7);
		m_NextQuests.emplace(TEXT("메인7"), NextQuestFunc8);
		m_NextQuests.emplace(TEXT("메인8"), NextQuestFunc9);
		m_NextQuests.emplace(TEXT("메인9"), NextQuestFunc10);
		m_NextQuests.emplace(TEXT("메인10"), NextQuestFunc11);
		m_NextQuests.emplace(TEXT("메인11"), NextQuestFunc12);
		m_NextQuests.emplace(TEXT("메인12"), NextQuestFunc13);

		CItemBase::ItemDesc itemDesc0{};
		itemDesc0.strItemTag = "방어구 제작석";
		itemDesc0.eItemType = ITEM_MISC;
		itemDesc0.strTexTag = "I_Enchant_Armor_KAA_001";
		itemDesc0.strModelTag = "";
		itemDesc0.numItem = 4;

		shared_ptr<CItemMisc> pItemMisc0 = CItemMisc::Create(itemDesc0);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> QuestReward1 = QuestReward(pItemMisc0);

		m_QuestRewards.emplace(TEXT("메인6"), QuestReward1);

		CItemBase::ItemDesc itemDesc1{};
		itemDesc1.strItemTag = "소형 회복 물약";
		itemDesc1.eItemType = ITEM_USABLE;
		itemDesc1.eItemUsableType = ITEM_USABLE_HP;
		itemDesc1.strTexTag = "I_Health_Potion_1";
		itemDesc1.iValue = 100;
		itemDesc1.numItem = 3;
		itemDesc1.iPrice = 50;
		itemDesc1.wstrItemDescription = TEXT("체력을 조금 회복시켜주는 물약");

		shared_ptr<CItemUsable> pItemUsable0 = CItemUsable::Create(itemDesc1);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> QuestRewardUsable0 = QuestReward(pItemUsable0);

		m_QuestRewards.emplace(TEXT("메인3"), QuestRewardUsable0);

		CItemBase::ItemDesc itemDesc99{};
		itemDesc99.strItemTag = "희귀한 마린드";
		itemDesc99.eItemType = ITEM_MISC;
		itemDesc99.strTexTag = "I_SkillGrowth_Polymorph_Sapphire_004";
		itemDesc99.strModelTag = "";
		itemDesc99.iValue = 200;
		itemDesc99.numItem = 2;

		shared_ptr<CItemMisc> pItemMisc9 = CItemMisc::Create(itemDesc99);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> QuestRewardMisc9 = QuestReward(pItemMisc9);

		m_QuestRewards.emplace(TEXT("메인9"), QuestRewardMisc9);
	}

	/*Sub Quest(Village)*/
	{
		/*마을주민 부탁 : 로티에게 물건 전달*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc1 = MeetNpcQuest(QUEST_SUB1, "CT_Named_Lottie_SK");
		
		/*마을주민 부탁 : 비행 퀘스트(전설의 꽃 전달)*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc2 = EventActiveQuest(QUEST_SUB0, TEXT("레오나드의 부탁"), "Event_FindFlower", 1);
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc3 = MeetNpcQuest(QUEST_SUB0, "CT_Named_LeonardA_SK");

		/*마을주민 부탁 : 잃어버린 물건 찾기(미니게임)*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc4 = EventActiveQuest(QUEST_SUB1, TEXT("잃어버린 물건 찾기"), "Event_FindMinigame", 1);
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc5 = MeetNpcQuest(QUEST_SUB1, "CT_Named_Davinci_SK");

		/*마을주민 부탁 : 아이(애신) 찾기*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc6 = EventActiveQuest(QUEST_SUB0, TEXT("사라진 아이"), "Event_FindChild", 1);
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc7 = MeetNpcQuest(QUEST_SUB0,"CT_Named_Ashien_SK");
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc8 = MeetNpcQuest(QUEST_SUB0,"CT_Named_JaniceCarter_SK");


		m_bindSubQuests.emplace(TEXT("마을서브1"), SubFunc1);

		m_bindSubQuests.emplace(TEXT("마을서브2-1"), SubFunc2);
		m_bindSubQuests.emplace(TEXT("마을서브2-2"), SubFunc3);

		m_bindSubQuests.emplace(TEXT("마을서브3-1"), SubFunc4);
		m_bindSubQuests.emplace(TEXT("마을서브3-2"), SubFunc5);

		m_bindSubQuests.emplace(TEXT("마을서브4-1"), SubFunc6);
		m_bindSubQuests.emplace(TEXT("마을서브4-2"), SubFunc7);
		m_bindSubQuests.emplace(TEXT("마을서브4-3"), SubFunc8);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc1 = NextQuest(QUEST_SUB0, L"마을서브2-2");
		m_NextQuests.emplace(TEXT("마을서브2-1"), NextQuestFunc1);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc2 = NextQuest(QUEST_SUB1, L"마을서브3-2");
		m_NextQuests.emplace(TEXT("마을서브3-1"), NextQuestFunc2);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc3 = NextQuest(QUEST_SUB0, L"마을서브4-2");
		m_NextQuests.emplace(TEXT("마을서브4-1"), NextQuestFunc3);
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc4 = NextQuest(QUEST_SUB0, L"마을서브4-3");
		m_NextQuests.emplace(TEXT("마을서브4-2"), NextQuestFunc4);

		CItemBase::ItemDesc itemDesc{};
		itemDesc.strItemTag = "중형 회복 물약";
		itemDesc.eItemType = ITEM_USABLE;
		itemDesc.eItemUsableType = ITEM_USABLE_HP;
		itemDesc.strTexTag = "I_Health_Potion_2";
		itemDesc.iValue = 500;
		itemDesc.numItem = 2;
		itemDesc.iPrice = 100;
		itemDesc.wstrItemDescription = TEXT("체력을 적당히 회복시켜주는 물약");

		shared_ptr<CItemUsable> pItemUsable5 = CItemUsable::Create(itemDesc);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> QuestRewardLeonard = QuestReward(pItemUsable5);

		m_QuestRewards.emplace(TEXT("마을서브2-2"), QuestRewardLeonard);

		CItemBase::ItemDesc itemDesc1{};
		itemDesc1.strItemTag = "날카로운 발톱";
		itemDesc1.eItemType = ITEM_MISC;
		itemDesc1.strTexTag = "I_Claw_001";
		itemDesc1.strModelTag = "";
		itemDesc1.numItem = 2;

		shared_ptr<CItemMisc> pItemMisc1 = CItemMisc::Create(itemDesc1);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> QuestReward2 = QuestReward(pItemMisc1);

		m_QuestRewards.emplace(TEXT("마을서브3-2"), QuestReward2);

		CItemBase::ItemDesc itemDescCross{};
		itemDescCross.strItemTag = "석궁";
		itemDescCross.eItemType = ITEM_MISC;
		itemDescCross.eItemEquipType = ITEM_EQUIP_END;
		itemDescCross.strTexTag = "IT_P_Crossbow_20001";
		itemDescCross.strModelTag = "";
		itemDescCross.numItem = 1;

		shared_ptr<CItemMisc> pItemCross = CItemMisc::Create(itemDescCross);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> QuestRewardCross = QuestReward(pItemCross);

		m_QuestRewards.emplace(TEXT("마을서브4-3"), QuestRewardCross);

		// IT_P_Staff_00014
		CItemBase::ItemDesc itemDescMagicWeapon{};
		itemDescMagicWeapon.strItemTag = "지팡이";
		itemDescMagicWeapon.eItemType = ITEM_MISC;
		itemDescMagicWeapon.eItemEquipType = ITEM_EQUIP_END;
		itemDescMagicWeapon.strTexTag = "IT_P_Staff_00014";
		itemDescMagicWeapon.strModelTag = "";
		itemDescMagicWeapon.numItem = 1;

		shared_ptr<CItemMisc> pItemMagic = CItemMisc::Create(itemDescMagicWeapon);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> QuestRewardMagic = QuestReward(pItemMagic);

		// 어디에 집어넣더라?
		m_QuestRewards.emplace(TEXT("메인10"), QuestRewardMagic);
	}

	/*Sub Quest(Dungeon)*/
	{

		/*던전 서브퀘스트1(사라진 약혼자)*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc1 = MeetNpcQuest(QUEST_SUB0, "CT_Named_Lottie_SK"); //로티와 대화
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc2 = MeetNpcQuest(QUEST_SUB0, "CT_Named_RobertGhost_SK"); //유령 로버트와 대화
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc3 = EventActiveQuest(QUEST_SUB0, TEXT("약혼 반지 찾기") ,"Event_FindRing", 1); //반지찾기
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc4 = MeetNpcQuest(QUEST_SUB0, "CT_Named_RobertGhost_SK"); //로버트와 대화


		/*던전 서브퀘스트1(NPC 찾기)*/
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc5 = MeetNpcQuest(QUEST_SUB1, "CT_Named_ArthurTalon_SK"); //데이빗과 대화
		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> SubFunc6 = MeetNpcQuest(QUEST_SUB1, "CT_Named_Lukas_SK"); //루카스와 대화


		m_bindSubQuests.emplace(TEXT("던전서브1-1"), SubFunc1);
		m_bindSubQuests.emplace(TEXT("던전서브1-2"), SubFunc2);
		m_bindSubQuests.emplace(TEXT("던전서브1-3"), SubFunc3);
		m_bindSubQuests.emplace(TEXT("던전서브1-4"), SubFunc4);

		m_bindSubQuests.emplace(TEXT("던전서브2-1"), SubFunc5);
		m_bindSubQuests.emplace(TEXT("던전서브2-2"), SubFunc6);


		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc1 = NextQuest(QUEST_SUB0, L"던전서브1-2");
		m_NextQuests.emplace(TEXT("던전서브1-1"), NextQuestFunc1);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc2 = NextQuest(QUEST_SUB0, L"던전서브1-3");
		m_NextQuests.emplace(TEXT("던전서브1-2"), NextQuestFunc2);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc3 = NextQuest(QUEST_SUB0, L"던전서브1-4");
		m_NextQuests.emplace(TEXT("던전서브1-3"), NextQuestFunc3);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> NextQuestFunc4 = NextQuest(QUEST_SUB1, L"던전서브2-2");
		m_NextQuests.emplace(TEXT("던전서브2-1"), NextQuestFunc4);



		CItemBase::ItemDesc itemDescRing{};
		itemDescRing.strItemTag = "반지";
		itemDescRing.eItemType = ITEM_MISC;
		itemDescRing.eItemEquipType = ITEM_EQUIP_END;
		itemDescRing.strTexTag = "IT_P_Ring_00020";
		itemDescRing.strModelTag = "";
		itemDescRing.numItem = 1;

		shared_ptr<CItemMisc> pItemRing = CItemMisc::Create(itemDescRing);

		function<_bool(shared_ptr<CPolymorph>, list<weak_ptr<CGameObject>>&, string&)> QuestRewardRing = QuestReward(pItemRing);

		// 어디에 집어넣더라?
		m_QuestRewards.emplace(TEXT("던전서브1-3"), QuestRewardRing);
	}

	/*CItemBase::ItemDesc itemDesc{};
	itemDesc.strItemTag = "Misc Item4";
	itemDesc.eItemType = ITEM_MISC;
	itemDesc.strTexTag = "I_GrowthWater_004";
	itemDesc.strModelTag = "";
	itemDesc.numItem = 999;*/

	//shared_ptr<CItemMisc> pItemMisc = CItemMisc::Create(itemDesc);

	/* Quest Reward */
	//m_QuestRewards.emplace(TEXT("메인0"), func5);		// 보상 퀘스트가 있을 경우 반드시 퀘스트를 클릭해야 완료가 됩니다


	return S_OK;
}

void CQuestMgr::QuestKeyInput()
{
	if (GAMEINSTANCE->KeyDown(DIK_COMMA))
	{
		switch (m_iNumPressed)
		{
		case 0:
		{
			UpdateQuest(QUEST_MAIN, L"메인3"); // 훈련장
			//UIMGR->SetCurQuestNPC(QUEST_MAIN, dynamic_pointer_cast<CNPC_David>(GetNPC("CT_Named_ArthurTalon_SK")));
			UpdateQuest(QUEST_SUB0, L"마을서브4-1"); // 사라진 아이
			UIMGR->SetCurQuestNPC(QUEST_SUB0, nullptr);
			UIMGR->SetCurQuestNPC(QUEST_SUB1, nullptr);

			weak_ptr<CNPC_David> pDavid = dynamic_pointer_cast<CNPC_David>(GetNPC("CT_Named_ArthurTalon_SK"));
			if (!pDavid.expired())
			{
				pDavid.lock()->IncreaseDialogueOrder();
				pDavid.lock()->SetMoveToTrainingTrigger(true);
				pDavid.lock()->SetQuestSkipTime(1.f);
			}

			/*CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "중형 회복 물약";
			itemDesc.eItemType = ITEM_USABLE;
			itemDesc.eItemUsableType = ITEM_USABLE_HP;
			itemDesc.strTexTag = "I_Health_Potion_2";
			itemDesc.iValue = 500;
			itemDesc.numItem = 2;
			itemDesc.iPrice = 100;
			itemDesc.wstrItemDescription = TEXT("체력을 적당히 회복시켜주는 물약");

			shared_ptr<CItemUsable> pItemUsable5 = CItemUsable::Create(itemDesc);

			ITEMMGR->AddItemToInventory(pItemUsable5);*/
		}
		break;
		case 1:
		{
			UpdateQuest(QUEST_MAIN, L"메인4"); // 고블린 잡몹
			UIMGR->SetCurQuestNPC(QUEST_MAIN, nullptr);

		}
		break;
		case 2:
		{
			UpdateQuest(QUEST_MAIN, L"메인5"); // 고블린 필드보스
			UIMGR->SetCurQuestNPC(QUEST_MAIN, nullptr);
			//UpdateQuest(QUEST_SUB0, L"마을서브4-1"); // 사라진 아이
			ITEMMGR->PrepareCraftItem();
		}
		break;
		case 3:
		{
			// Fonos 시작
			weak_ptr<CUIBackGround> pUI = dynamic_pointer_cast<CUIBackGround>(UIMGR->FindUI("PCInfoIconBG"));
			if (!pUI.expired())
			{
				pUI.lock()->SetTexIndex(1);
			}
			UIMGR->SetCurQuestNPC(QUEST_MAIN, nullptr);
			UpdateQuest(QUEST_MAIN, L"메인8"); // 오크 소탕하기
			m_curBindSubQuest0 = nullptr;
			m_curBindSubQuest1 = nullptr;
			AddFirstSetToInventory();
		}
		break;
		case 4:
		{
			UIMGR->SetCurQuestNPC(QUEST_MAIN, nullptr);
			UpdateQuest(QUEST_MAIN, L"메인9"); // 준혜 미니게임 + 오크 필드보스
			m_curBindSubQuest0 = nullptr;
			m_curBindSubQuest1 = nullptr;
		}
		break;
		case 5:
		{

			UIMGR->SetCurQuestNPC(QUEST_MAIN, dynamic_pointer_cast<CNPC_Lottie>(GetNPC("CT_Named_RuffaloRice_SK")));
			UpdateQuest(QUEST_MAIN, L"메인10");
			m_curBindSubQuest0 = nullptr;
			m_curBindSubQuest1 = nullptr;
			UIMGR->SetCurQuestNPC(QUEST_SUB0, nullptr);
			UIMGR->SetCurQuestNPC(QUEST_SUB1, nullptr);

			weak_ptr<CUIBackGround> pUI = dynamic_pointer_cast<CUIBackGround>(UIMGR->FindUI("PCInfoIconBG"));
			if (!pUI.expired())
			{
				pUI.lock()->SetTexIndex(2);
			}

			AddCraftItem();
		}
		break;
		case 6:
		{
			ITEMMGR->PrepareCraftItem();
			UpdateQuest(QUEST_MAIN, L"메인11"); // 동찬 미니게임 + 금주 미니게임
			UIMGR->SetCurQuestNPC(QUEST_MAIN, nullptr);
			//m_curBindSubQuest0 = nullptr;
			//m_curBindSubQuest1 = nullptr;
			UpdateQuest(QUEST_SUB0, TEXT("던전서브1-1"));
			UIMGR->SetCurQuestNPC(QUEST_SUB0, dynamic_pointer_cast<CNPC_Lottie>(GetNPC("CT_Named_Lottie_SK")));
			UpdateQuest(QUEST_SUB1, TEXT("던전서브2-1"));
			UIMGR->SetCurQuestNPC(QUEST_SUB1, dynamic_pointer_cast<CNPC_David>(GetNPC("CT_Named_ArthurTalon_SK")));

			AddSecondSetToInventory();
		}
		break;
		case 7:
		{

			// Dungeon Main 시작
			UpdateQuest(QUEST_MAIN, L"메인13");
			UIMGR->SetCurQuestNPC(QUEST_MAIN, nullptr);
			UpdateQuest(QUEST_SUB0, TEXT("던전서브1-2"));
			UpdateQuest(QUEST_SUB1, TEXT("던전서브2-2"));
			UIMGR->SetCurQuestNPC(QUEST_SUB0, nullptr);
			UIMGR->SetCurQuestNPC(QUEST_SUB1, nullptr);
		}
		break;
		case 8:
		{
			ITEMMGR->IncreaseMoney(17550);

			// Dungeon Final
			// 최종 퀘스트 추가 필요?
			UIMGR->SetCurQuestNPC(QUEST_MAIN, nullptr);
			UIMGR->SetCurQuestNPC(QUEST_SUB0, nullptr);
			UIMGR->SetCurQuestNPC(QUEST_SUB1, nullptr);
			m_curBindSubQuest0 = nullptr;
			m_curBindSubQuest1 = nullptr;
		}
		break;
		}

		m_iNumPressed++;
	}
}

void CQuestMgr::AddFirstSetToInventory()
{
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
			return;

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
			return;

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
			return;

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
			return;

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
			return;

		ITEMMGR->AddItemToInventory(Part);
	}
}

void CQuestMgr::AddSecondSetToInventory()
{
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
			return;

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
			return;

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
			return;

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
			return;

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
			return;

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
			return;

		ITEMMGR->AddItemToInventory(Part);
	}

	{
		CItemBase::ItemDesc itemDesc{};
		itemDesc.strItemTag = "대형 회복 물약";
		itemDesc.eItemType = ITEM_USABLE;
		itemDesc.eItemUsableType = ITEM_USABLE_HP;
		itemDesc.strTexTag = "I_Health_Potion_3";
		itemDesc.iValue = 1000;
		itemDesc.numItem = 20;
		itemDesc.iPrice = 200;
		itemDesc.wstrItemDescription = TEXT("체력을 많이 회복시켜주는 물약");

		shared_ptr<CItemUsable> item = CItemUsable::Create(itemDesc);
		if (item == nullptr)
			return;

		ITEMMGR->AddItemToInventory(item);
	}
}

void CQuestMgr::AddCraftItem()
{
	CItemBase::ItemDesc itemDesc99{};
	itemDesc99.strItemTag = "희귀한 마린드";
	itemDesc99.eItemType = ITEM_MISC;
	itemDesc99.strTexTag = "I_SkillGrowth_Polymorph_Sapphire_004";
	itemDesc99.strModelTag = "";
	itemDesc99.iValue = 200;
	itemDesc99.numItem = 2;

	shared_ptr<CItemMisc> pItemMisc9 = CItemMisc::Create(itemDesc99);
	if (pItemMisc9 == nullptr)
		return;

	ITEMMGR->AddItemToInventory(pItemMisc9);
}

HRESULT CQuestMgr::PrepareNPCs()
{
	// NPC
	if (m_bIsOnce)
	{
		shared_ptr<CNPC_Lukas> pNPC_Lukas = CNPC_Lukas::Create(_float3(71.8f, -10.6f, 315.f), 0, _float3(64.2f, -10.6f, 350.f));
		shared_ptr<CNPC_Robert> pNPC_Robert = CNPC_Robert::Create(_float3(91.2f, -10.6f, 300.f), _float3(92.4f, -10.6f, 300.f));
		shared_ptr<CNPC_Lottie> pNPC_Lottie = CNPC_Lottie::Create(_float3(80.1f, -10.6f, 374.f), _float3(80.3f, -10.6f, 372.4f));
		shared_ptr<CNPC_JaniceCarter> pNPC_JancieCarter = CNPC_JaniceCarter::Create(_float3(139.f, -10.6f, 411.f), _float3(138.f, -10.6f, 412.f));
		shared_ptr<CNPC_Davinci> pNPC_Davinci = CNPC_Davinci::Create(_float3(46.3f, -10.6f, 334.7f), _float3(48.6f, -10.6f, 337.f));
		shared_ptr<CNPC_LeonardA> pNPC_LeonardA = CNPC_LeonardA::Create(_float3(49.f, -10.6f, 364.8f), _float3(50.7f, -10.6f, 362.6f));
		shared_ptr<CNPC_RuffaloRice> pNPC_RuffaloRice = CNPC_RuffaloRice::Create(_float3(52.6f, -7.f, 464.5f), _float3(60.7f, -7.f, 464.5f));
		shared_ptr<CNPC_Elleia> pNPC_Elleia = CNPC_Elleia::Create(_float3(86.16f, -10.6f, 408.8f), _float3(86.13f, -10.6f, 410.4f));
		shared_ptr<CNPC_Ashien> pNPC_Ashien = CNPC_Ashien::Create(_float3(146.f, -45.f, 25.7f), _float3(146.f, -46.f, 26.2f));
		shared_ptr<CNPC_David> pNPC_David = CNPC_David::Create(_float3(107.f, -10.68f, 369.7f), _float3(104.5f, -10.68f, 370.f),
			_float3(119.f, -10.68f, 461.f), _float3(160.f, -10.68f, 461.f));

		QUESTMGR->AddNPC(pNPC_Lukas->GetModelName(), pNPC_Lukas);
		QUESTMGR->AddNPC(pNPC_Robert->GetModelName(), pNPC_Robert);
		QUESTMGR->AddNPC(pNPC_Ashien->GetModelName(), pNPC_Ashien);
		QUESTMGR->AddNPC(pNPC_Lottie->GetModelName(), pNPC_Lottie);
		QUESTMGR->AddNPC(pNPC_JancieCarter->GetModelName(), pNPC_JancieCarter);
		QUESTMGR->AddNPC(pNPC_Davinci->GetModelName(), pNPC_Davinci);
		QUESTMGR->AddNPC(pNPC_LeonardA->GetModelName(), pNPC_LeonardA);
		QUESTMGR->AddNPC(pNPC_RuffaloRice->GetModelName(), pNPC_RuffaloRice);
		QUESTMGR->AddNPC(pNPC_Elleia->GetModelName(), pNPC_Elleia);
		QUESTMGR->AddNPC(pNPC_David->GetModelName(), pNPC_David);

		DIALOGUEMGR->AddNPC(pNPC_Lukas->GetDisplayName(), pNPC_Lukas);
		DIALOGUEMGR->AddNPC(pNPC_Robert->GetDisplayName(), pNPC_Robert);
		DIALOGUEMGR->AddNPC(pNPC_Ashien->GetDisplayName(), pNPC_Ashien);
		DIALOGUEMGR->AddNPC(pNPC_Lottie->GetDisplayName(), pNPC_Lottie);
		DIALOGUEMGR->AddNPC(pNPC_JancieCarter->GetDisplayName(), pNPC_JancieCarter);
		DIALOGUEMGR->AddNPC(pNPC_Davinci->GetDisplayName(), pNPC_Davinci);
		DIALOGUEMGR->AddNPC(pNPC_LeonardA->GetDisplayName(), pNPC_LeonardA);
		DIALOGUEMGR->AddNPC(pNPC_RuffaloRice->GetDisplayName(), pNPC_RuffaloRice);
		DIALOGUEMGR->AddNPC(pNPC_Elleia->GetDisplayName(), pNPC_Elleia);
		DIALOGUEMGR->AddNPC(pNPC_David->GetDisplayName(), pNPC_David);

		m_bIsOnce = false;
	}

	else
	{
		//shared_ptr<CNPC_Lukas> pNPC_Lukas = dynamic_pointer_cast<CNPC_Lukas>(QUESTMGR->GetNPC("CT_Named_Lukas_SK"));
		shared_ptr<CNPC_Robert> pNPC_Robert = dynamic_pointer_cast<CNPC_Robert>(QUESTMGR->GetNPC("CT_Named_Robert_SK"));
		shared_ptr<CNPC_Lottie> pNPC_Lottie = dynamic_pointer_cast<CNPC_Lottie>(QUESTMGR->GetNPC("CT_Named_Lottie_SK"));
		shared_ptr<CNPC_JaniceCarter> pNPC_JancieCarter = dynamic_pointer_cast<CNPC_JaniceCarter>(QUESTMGR->GetNPC("CT_Named_JaniceCarter_SK"));
		shared_ptr<CNPC_Davinci> pNPC_Davinci = dynamic_pointer_cast<CNPC_Davinci>(QUESTMGR->GetNPC("CT_Named_Davinci_SK"));
		shared_ptr<CNPC_LeonardA> pNPC_LeonardA = dynamic_pointer_cast<CNPC_LeonardA>(QUESTMGR->GetNPC("CT_Named_LeonardA_SK"));
		shared_ptr<CNPC_RuffaloRice> pNPC_RuffaloRice = dynamic_pointer_cast<CNPC_RuffaloRice>(QUESTMGR->GetNPC("CT_Named_RuffaloRice_SK"));
		shared_ptr<CNPC_Elleia> pNPC_Elleia = dynamic_pointer_cast<CNPC_Elleia>(QUESTMGR->GetNPC("CT_Named_Elleia_SK"));
		shared_ptr<CNPC_Ashien> pNPC_Ashien = dynamic_pointer_cast<CNPC_Ashien>(QUESTMGR->GetNPC("CT_Named_Ashien_SK"));
		shared_ptr<CNPC_David> pNPC_David = dynamic_pointer_cast<CNPC_David>(QUESTMGR->GetNPC("CT_Named_ArthurTalon_SK"));

		/*{
			pNPC_Lukas->SetGenType(0);
			pNPC_Lukas->SetInitLookPos(_float3(64.2f, -10.6f, 350.f));
			pNPC_Lukas->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(71.8f, -10.6f, 315.f));
		}*/

		{
			pNPC_Robert->SetInitLookPos(_float3(92.4f, -10.6f, 300.f));
			pNPC_Robert->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(91.2f, -10.6f, 300.f));
		}

		{
			pNPC_Lottie->SetInitLookPos(_float3(80.3f, -10.6f, 372.4f));
			pNPC_Lottie->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(80.1f, -10.6f, 374.f));
		}

		{
			pNPC_JancieCarter->SetInitLookPos(_float3(138.f, -10.6f, 412.f));
			pNPC_JancieCarter->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(139.f, -10.6f, 411.f));
		}

		{
			pNPC_Davinci->SetInitLookPos(_float3(48.6f, -10.6f, 337.f));
			pNPC_Davinci->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(46.3f, -10.6f, 334.7f));
		}

		{
			pNPC_LeonardA->SetInitLookPos(_float3(50.7f, -10.6f, 362.6f));
			pNPC_LeonardA->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(49.f, -10.6f, 364.8f));
		}

		{
			pNPC_RuffaloRice->SetInitLookPos(_float3(60.7f, -7.f, 464.5f));
			pNPC_RuffaloRice->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(52.6f, -7.f, 464.5f));
		}

		{
			pNPC_Elleia->SetInitLookPos(_float3(86.13f, -10.6f, 410.4f));
			pNPC_Elleia->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(86.16f, -10.6f, 408.8f));
		}

		{
			pNPC_Ashien->SetInitLookPos(_float3(146.f, -46.f, 26.2f));
			pNPC_Ashien->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(146.f, -45.f, 25.7f));
		}

		{
			pNPC_David->SetInitLookPos(_float3(104.5f, -10.68f, 370.f));
			pNPC_David->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(107.f, -10.68f, 369.7f));
			pNPC_David->SetInitPos(_float3(107.f, -10.68f, 369.7f));
			pNPC_David->SetMovePos(_float3(119.f, -10.68f, 461.f));
			pNPC_David->SetSecondPos(_float3(160.f, -10.68f, 461.f));
		}
	}

	return S_OK;
}
