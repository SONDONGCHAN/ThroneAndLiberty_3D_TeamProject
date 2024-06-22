#include "LevelMgr.h"
#include "Level.h"

#include "GameInstance.h"

CLevelMgr::CLevelMgr()
{
}

HRESULT CLevelMgr::Initialize()
{
    return S_OK;
}

void CLevelMgr::Tick(_float _fTimeDelta)
{
    if (!m_pCurrentLevel)
        return;

    m_pCurrentLevel->Tick(_fTimeDelta);

}

HRESULT CLevelMgr::Render()
{
    if (!m_pCurrentLevel)
        return E_FAIL;

    m_pCurrentLevel->Render();

    return S_OK;
}

HRESULT CLevelMgr::OpenLevel(_uint _iLevelIndex, shared_ptr<CLevel> _pLevel)
{
    //if (!m_pLoadingLevel)
    //    return E_FAIL;

    /*이전 레벨을 날리고 사용하던 자원도 날리기*/
    if (m_pCurrentLevel) {
        //LevelClear
        m_pCurrentLevel.reset();
        //ResourceMgr->ClearRes
    }

    /*로딩 레벨에 인자로 받은 레벨을 준비하라는 신호를 보냄*/
    m_iLevelIndex = _iLevelIndex;
	if (m_pLoadingLevel)
		m_pLoadingLevel->ReadyNextLevel(_iLevelIndex);
    m_pCurrentLevel = _pLevel;

    return S_OK;
}

HRESULT CLevelMgr::OpenLevelForClient(_uint _iLevelIndex)
{
    if (!m_pLoadingLevel)
        return E_FAIL;

    /*이전 레벨을 날리고 사용하던 자원도 날리기*/
    if (m_pCurrentLevel) {
        //LevelClear 
       // GAMEINSTANCE->ClearLevel(m_iLevelIndex);
        GAMEINSTANCE->ClearLevel(m_iLevelIndex);
        m_pCurrentLevel.reset();
        GAMEINSTANCE->ClearResource();
        GAMEINSTANCE->DeleteOctree();
    }

    /*로딩 레벨에 인자로 받은 레벨을 준비하라는 신호를 보냄*/
    m_iLevelIndex = _iLevelIndex;
    m_pLoadingLevel->ReadyNextLevel(_iLevelIndex);
    m_pCurrentLevel = m_pLoadingLevel;
    m_IsLoadingLevel = true;

    return S_OK;
}

HRESULT CLevelMgr::OpenLevel2(_uint _iLevelIndex, shared_ptr<CLevel> _pLevel)
{

    /*이전 레벨을 날리고 사용하던 자원도 날리기*/
    if (m_pCurrentLevel) {
        GAMEINSTANCE->ClearLevel(m_iLevelIndex);
        m_pCurrentLevel.reset();
        GAMEINSTANCE->ClearResource();
        GAMEINSTANCE->DeleteOctree();
    }

    /*로딩 레벨에 인자로 받은 레벨을 준비하라는 신호를 보냄*/
    m_iLevelIndex = _iLevelIndex;
    _pLevel->ReadyNextLevel(_iLevelIndex);
    m_pCurrentLevel = _pLevel;
    m_IsLoadingLevel = true;

    return S_OK;
}

HRESULT CLevelMgr::StartLevel(shared_ptr<class CLevel> _pNextLevel)
{
    m_pCurrentLevel = _pNextLevel;
    m_IsLoadingLevel = false;


    return S_OK;
}

void CLevelMgr::LevelBGMOn()
{
    m_pCurrentLevel->LevelBGMOn();

}

void CLevelMgr::LevelBGMOff()
{
    m_pCurrentLevel->LevelBGMOff();
}

void CLevelMgr::SwitchingBGM()
{
    m_pCurrentLevel->SwitchingBGM();
}

void CLevelMgr::SwitchingBGM2()
{
    m_pCurrentLevel->SwitchingBGM2();
}

void CLevelMgr::SwitchingBGM3()
{
    m_pCurrentLevel->SwitchingBGM3();
}

shared_ptr<CLevelMgr> CLevelMgr::Create()
{
    shared_ptr<CLevelMgr> pInstance = make_shared<CLevelMgr>();

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Created : CLevelMgr");

    return pInstance;
}
