#include "Loader.h"
#include "GameInstance.h"

#include "Level.h"
#include "LevelLogo.h"

CLoader::CLoader() {

}

CLoader::~CLoader()
{
    if (m_pThread.joinable())
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

    switch ((_eLevel))
    {

    case LEVEL_LOGO:
        LoadingForLogo();
        m_pNextLevel = CLevelLogo::Create();
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
    GAMEINSTANCE->LoadLevelResource(LEVEL::LEVEL_LOGO);

    return S_OK;
}

shared_ptr<CLoader> CLoader::Create()
{
    shared_ptr<CLoader> pInstance = make_shared<CLoader>();

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CLoader");

    return pInstance;
}
