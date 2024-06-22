
#include "MainApp.h"
#include "LevelLogo.h"

#include "ImGuiMgr.h"
#include "GameInstance.h"

#include "LevelLoading.h"

CMainApp::CMainApp()
{
}

CMainApp::~CMainApp()
{
    Free();
}

HRESULT CMainApp::Initialize()
{
    /*Random Seed*/
    srand((unsigned)time(NULL));

    GRAPHIC_DESC GraphicDesc = {};

    GraphicDesc.hWnd = g_hWnd;
    GraphicDesc.isWindowed = true;
    GraphicDesc.iWinSizeX = g_iWinSizeX;
    GraphicDesc.iWinSizeY = g_iWinSizeY;

    if (FAILED(GAMEINSTANCE->InitializeEngine(g_hInst, LEVEL_END, GraphicDesc)))
        return E_FAIL;

    GAMEINSTANCE->LoadLevelResource(LEVEL_EFFECTTOOL);
    GAMEINSTANCE->LoadLevelResource(LEVEL_STATIC);

    /*로딩 레벨을 미리 세팅한 후 이후에도 계속 재활용한다*/
    shared_ptr<CLevelLoading> pLoadingLevel = CLevelLoading::Create();
    GAMEINSTANCE->SetLoadingLevel(pLoadingLevel);

    CImGuiMgr::GetInstance()->Initialize();

    if (FAILED(OpenLevel(LEVEL_LOGO)))
        return E_FAIL;

    return S_OK;
}

void CMainApp::Tick(_float _fTimeDelta)
{
    GAMEINSTANCE->TickEngine(_fTimeDelta);
    CImGuiMgr::GetInstance()->Tick(_fTimeDelta);
   
}

HRESULT CMainApp::Render()
{
    if (FAILED(GAMEINSTANCE->ClearBackBufferView(_float4(0.659f, 0.659f, 0.659f, 1.f))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->ClearDepthStencilView()))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->Draw()))
        return E_FAIL;

    if (FAILED(CImGuiMgr::GetInstance()->Render()))
        return E_FAIL; 

    if (FAILED(GAMEINSTANCE->Present()))
        return E_FAIL;

    return S_OK;
}



HRESULT CMainApp::OpenLevel(LEVEL _eStartLevel)
{
    GAMEINSTANCE->OpenLevel(LEVEL_LOADING, CLevelLogo::Create());

    return S_OK;
}

void CMainApp::Free()
{
    CGameInstance::DestroyInstance();

}
