#include "Loader.h"
#include "GameInstance.h"

#include "Level.h"
#include "LevelAnimToolLogo.h"
#include "LevelAnimToolEdit.h"

CLoader::CLoader() {

}

CLoader::~CLoader()
{
    if(m_pThread.joinable())
       m_pThread.join();
}



HRESULT ThreadMain(shared_ptr<CLoader> _pThis, LEVEL _eLevel)
{
    if (FAILED(CoInitializeEx(nullptr, 0)))
        return E_FAIL;

    shared_ptr<CLoader> pLoader = _pThis;

    pLoader->LoadingSelector(_eLevel);

	return S_OK;
}

HRESULT CLoader::Initialize()
{

	return S_OK;
}

HRESULT CLoader::LoadingSelector(LEVEL _eLevel)
{
    shared_ptr<CLevel> pLevel;

    HRESULT hr = {};

    //console("데이터 로딩 및 레벨 생성 중입니다\n");

    switch ((_eLevel))
    {

    case LEVEL_LOGO:
     
        LoadingForLogo();
        m_pNextLevel = CLevelAnimToolLogo::Create();
        m_IsFinished = true;
        //console("로딩이 완료되었습니다. 스페이스바를 누르면 시작합니다\n");
        break;

    case LEVEL_EDIT:
       // LoadingForDungeon();
        m_pNextLevel = CLevelAnimToolEdit::Create();
        m_IsFinished = true;

        break;

    default:
        break;
    }

    return S_OK;
}

void CLoader::StartLoading(LEVEL _eLevel)
{
    /*초기화*/
    m_IsFinished = false;

    /*스레드 동작*/
    m_pThread = thread(ThreadMain, shared_from_this(), _eLevel);

    /*스레드 해제*/
    m_pThread.join();
}

void CLoader::StartLevel()
{
    GAMEINSTANCE->StartLevel(m_pNextLevel);    

}

HRESULT CLoader::LoadingForLogo()
{
    //GAMEINSTANCE->LoadLevelResource(LEVEL::LEVEL_LOGO);

    return S_OK;
}

HRESULT CLoader::LoadingForField()
{
	return S_OK;
}

HRESULT CLoader::LoadingForEdit()
{
    //GAMEINSTANCE->LoadLevelResource(LEVEL::LEVEL_EDIT);

	return S_OK;
}

HRESULT CLoader::LoadingForTower()
{
	return S_OK;
}

shared_ptr<CLoader> CLoader::Create()
{
    shared_ptr<CLoader> pInstance = make_shared<CLoader>();

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CLoader");

    return pInstance;
}
