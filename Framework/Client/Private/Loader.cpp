#include "Loader.h"
#include "GameInstance.h"

#include "Level.h"
#include "LevelLogo.h"

#include "LevelDungeonMain.h"
#include "LevelDungeonFinal.h"
#include "LevelVillage.h"
#include "LevelFonos.h"

#include "UIMgr.h"
#include "UIPanel.h"
#include "QuestMgr.h"
#include "DialogueMgr.h"

#include "EventMgr.h"
#include "EffectMgr.h"

CLoader::CLoader() {

}

CLoader::~CLoader()
{
    Free();
    //if(m_pThread.joinable())
    //   m_pThread.join();
}

typedef unsigned(__stdcall* _beginthreadex_proc_type)(void*);


_uint APIENTRY wThreadMain(void* pArg)
{
    if (FAILED(CoInitializeEx(nullptr, 0)))
        return -1;

    CLoader* pLoader = static_cast<CLoader*>(pArg);

    pLoader->LoadingSelector();

    return 0;
}

HRESULT ThreadMain(shared_ptr<CLoader> _pThis, LEVEL _eLevel)
{
    //if (FAILED(CoInitializeEx(nullptr, 0)))
    //    return E_FAIL;

    //shared_ptr<CLoader> pLoader = _pThis;

    //pLoader->LoadingSelector(_eLevel);

	return S_OK;
}

HRESULT CLoader::Initialize(LEVEL _eNextLevel)
{
    m_eNextLevel = _eNextLevel;

	return S_OK;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    

HRESULT CLoader::LoadingSelector()
{
    EnterCriticalSection(&m_CriticalSection);

    CEventMgr::GetInstance()->ClearEvent();
    shared_ptr<CLevel> pLevel;

    HRESULT hr = {};

    console("데이터 로딩 및 레벨 생성 중입니다\n");

    GAMEINSTANCE->RendererOptionSwitching(m_eNextLevel);

    if (!m_IsStaticLoadDone && m_eNextLevel == LEVEL_LOGO) {

        LevelInitialize();
    }
    
    switch ((m_eNextLevel))
    {

    case LEVEL_LOGO:
     
        LoadingForLogo();
        m_pNextLevel = CLevelLogo::Create();
        console("로딩이 완료되었습니다. 스페이스바를 누르면 시작합니다\n");
        m_IsFinished = true;
        break;

    case LEVEL_VILLAGE:
        hr = LoadingForVillage();
		GAMEINSTANCE->CreateOctree({ 250.0f,0.0f,250.0f }, 512.0f, 4);
        m_pNextLevel = CLevelVillage::Create();
        UIMGR->SetMapTitle(LEVEL_VILLAGE);
        UIMGR->ActivateMapTitle();
        m_IsFinished = true;

        break;

    case LEVEL_DUNGEON_MAIN:
        LoadingForDungeonMain();
		GAMEINSTANCE->CreateOctree({ 21.15f,0.0f,40.0f }, 160.0f, 4);
        m_pNextLevel = CLevelDungeonMain::Create();
        m_IsFinished = true;
        UIMGR->SetMapTitle(LEVEL_DUNGEON_MAIN);
        UIMGR->ActivateMapTitle();
        break;

    case LEVEL_DUNGEON_FINAL:
        LoadingForDungeonFinal();
        GAMEINSTANCE->CreateOctree({ 21.15f,-16.0f,220.0f }, 176.0f, 4);
        m_pNextLevel = CLevelDungeonFinal::Create();
        m_IsFinished = true;
        UIMGR->SetMapTitle(LEVEL_DUNGEON_FINAL);
        UIMGR->ActivateMapTitle();
        break;

    case LEVEL_FONOS:
        LoadingForFonos();
		GAMEINSTANCE->CreateOctree({ 128,0.0f,128.0f }, 224.0f, 4);
        m_pNextLevel = CLevelFonos::Create();
        m_IsFinished = true;
        UIMGR->SetMapTitle(LEVEL_FONOS);
        UIMGR->ActivateMapTitle();
        break;

    default:
        break;
    }

    LeaveCriticalSection(&m_CriticalSection);

    return S_OK;
}

HRESULT CLoader::LevelInitialize()
{  

   


    return S_OK;
}

_bool CLoader::ThreadJoin()
{
    if (m_pThread.joinable())
    {
        m_pThread.join();

        return true;
    }

    return false;
}

void CLoader::StartLoading(LEVEL _eLevel)
{
    /*초기화*/
    //m_IsFinished = false;
    ///*스레드 동작*/
    //m_pThread = thread(ThreadMain, shared_from_this(), _eLevel);

    ///*스레드 해제*/
    //m_pThread.join();

    InitializeCriticalSection(&m_CriticalSection);

    m_hThread = (HANDLE)_beginthreadex(nullptr, 0, wThreadMain, this, 0, nullptr);

    if (0 == m_hThread)
        return;
}

void CLoader::StartLevel()
{
    GAMEINSTANCE->StartLevel(m_pNextLevel);    

}

HRESULT CLoader::LoadingForLogo()
{
    GAMEINSTANCE->LoadLevelResource(LEVEL::LEVEL_LOGO);
   
    return S_OK;
}

HRESULT CLoader::LoadingForVillage()
{
    //if (FAILED(LevelInitialize()))
    //    return E_FAIL;
  
    GAMEINSTANCE->LoadLevelResource(LEVEL::LEVEL_VILLAGE);

    return S_OK;
}

HRESULT CLoader::LoadingForFonos()
{
    /*For Test*/
    GAMEINSTANCE->LoadLevelResource(LEVEL::LEVEL_FONOS);

    return S_OK;
}

HRESULT CLoader::LoadingForDungeonMain()
{

    GAMEINSTANCE->LoadLevelResource(LEVEL::LEVEL_DUNGEON_MAIN);

   
    return S_OK;
}

HRESULT CLoader::LoadingForDungeonFinal()
{
    //GAMEINSTANCE->LoadLevelResource(LEVEL::LEVEL_STATIC);
   
    GAMEINSTANCE->LoadLevelResource(LEVEL::LEVEL_DUNGEON_FINAL);

    /*if (FAILED(UIMGR->Initialize()))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_AsiaG", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/AsiaGothic.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_AdenL", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/AdenL.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_AdenL8", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/AdenL8.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_AdenL10", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/AdenL10.spritefont"))))
        return E_FAIL;

    if (FAILED(UIMGR->PreLoadUIsJASON(TEXT("../Bin/Resources/Level_Static/Textures/UI/UIData/"))))
        return E_FAIL;

    UIMGR->DeactivateUI("InventoryPanel");

    UIMGR->DeactivateUI("CraftAllPanel");

    UIMGR->DeactivateUI("TradeAllPanel");

    UIMGR->DeactivateItemToolTip();

    UIMGR->DeactivateSpeechBallon();

    UIMGR->DeactivateOption();

    UIMGR->DeactivateTradeItemToolTip();

    if (FAILED(QUESTMGR->Initialize()))
        return E_FAIL;

    if (FAILED(DIALOGUEMGR->Initialize()))
        return E_FAIL;*/

	return S_OK;
}


CLoader* CLoader::Create(LEVEL _eNextLevel)
{
    //shared_ptr<CLoader> pInstance = make_shared<CLoader>();
    CLoader* pInstance = new CLoader();
        //make_shared<CLoader>();

    if (FAILED(pInstance->Initialize(_eNextLevel))) {
        MSG_BOX("Failed to Create : CLoader");
        delete pInstance;
        pInstance = nullptr;
    }

    return pInstance;
}

void CLoader::Free()
{
    WaitForSingleObject(m_hThread, INFINITE);

    DeleteCriticalSection(&m_CriticalSection);
    DeleteObject(m_hThread);
    CloseHandle(m_hThread);
}
