#include "MainApp.h"
#include "LevelLoading.h"

#include "GameInstance.h"

CMainApp::CMainApp()
{
}

#ifdef CHECK_REFERENCE_COUNT
CMainApp::~CMainApp()
{
    CGameInstance::DestroyInstance();
}
#endif // CHECK_REFERENCE_COUNT

HRESULT CMainApp::Initialize()
{
    /*Random Seed*/
    srand((unsigned)time(NULL));

    GRAPHIC_DESC GraphicDesc = {};

    GraphicDesc.hWnd = g_hWnd;
    GraphicDesc.isWindowed = true;
    GraphicDesc.iWinSizeX = g_iWinCX;
    GraphicDesc.iWinSizeY = g_iWinCY;

    if (FAILED(CGameInstance::GetInstance()->InitializeEngine(g_hInst, LEVEL_END, GraphicDesc)))
        return E_FAIL;

    CGameInstance::GetInstance()->LoadLevelResource(LEVEL_MAPTOOL);
    
    /*로딩 레벨을 미리 세팅한 후 이후에도 계속 재활용한다*/
    shared_ptr<CLevelLoading> pLoadingLevel = CLevelLoading::Create(LEVEL_ATELIER);
    GAMEINSTANCE->SetLoadingLevel(pLoadingLevel);

    GAMEINSTANCE->DebugRenderSwitch();
    GAMEINSTANCE->SwitchingFog(false);
    GAMEINSTANCE->SwitchingDOF(false);


    if (FAILED(OpenLevel(LEVEL_ATELIER)))
        return E_FAIL;

    return S_OK;
}

void CMainApp::Tick(_float _fTimeDelta)
{
    CGameInstance::GetInstance()->TickEngine(_fTimeDelta);
}

HRESULT CMainApp::Render()
{
    if (FAILED(CGameInstance::GetInstance()->ClearBackBufferView(_float4(0.01f, 0.7f, 0.31f, 1.f))))
        return E_FAIL;

    if (FAILED(CGameInstance::GetInstance()->ClearDepthStencilView()))
        return E_FAIL;

    if (FAILED(CGameInstance::GetInstance()->Draw()))
        return E_FAIL;
    
    if (FAILED(CGameInstance::GetInstance()->Present()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::OpenLevel(LEVEL _eStartLevel)
{
    GAMEINSTANCE->OpenLevel(LEVEL_LOADING, CLevelLoading::Create(_eStartLevel));

    return S_OK;
}

shared_ptr<CMainApp> CMainApp::Create()
{
    shared_ptr<CMainApp> pInstance = WRAPPING(CMainApp)();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CMainApp");
        pInstance.reset();
    }

    return pInstance;
}