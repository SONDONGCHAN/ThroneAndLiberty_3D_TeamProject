#include "MainAnimTool.h"

#include "GameInstance.h"
#include "LevelAnimToolLogo.h"
#include "LevelAnimToolEdit.h"
#include "LevelLoading.h"

CMainAnimTool::CMainAnimTool()
{
}

CMainAnimTool::~CMainAnimTool()
{
	Free();
}

HRESULT CMainAnimTool::Initialize()
{
   srand((unsigned)time(NULL));

    GRAPHIC_DESC GraphicDesc = {};

    GraphicDesc.hWnd = g_hWnd;
    GraphicDesc.isWindowed = true;
    GraphicDesc.iWinSizeX = g_iWinSizeX;
    GraphicDesc.iWinSizeY = g_iWinSizeY;

    if (FAILED(CGameInstance::GetInstance()->InitializeEngine(g_hInst, LEVEL_END, GraphicDesc)))
        return E_FAIL;

    CGameInstance::GetInstance()->LoadLevelResource(LEVEL_STATIC);
    
    /*로딩 레벨을 미리 세팅한 후 이후에도 계속 재활용한다*/
    shared_ptr<CLevelLoading> pLoadingLevel = CLevelLoading::Create();
    GAMEINSTANCE->SetLoadingLevel(pLoadingLevel);

    if (FAILED(CAnimToolImguiManager::GetInstance()->Initialize()))
       return E_FAIL;



    if (FAILED(OpenLevel(LEVEL_EDIT)))
        return E_FAIL;

    return S_OK;
}

void CMainAnimTool::Tick(_float _fTimeDelta)
{
    CGameInstance::GetInstance()->TickEngine(_fTimeDelta);
    CAnimToolImguiManager::GetInstance()->ImGuiTick();

}

HRESULT CMainAnimTool::Render()
{
    if (FAILED(CGameInstance::GetInstance()->ClearBackBufferView(_float4(0.f, 0.5f, 0.5f, 1.f))))
        return E_FAIL;

    if (FAILED(CGameInstance::GetInstance()->ClearDepthStencilView()))
        return E_FAIL;

    if (FAILED(CGameInstance::GetInstance()->Draw()))
        return E_FAIL;

   CAnimToolImguiManager::GetInstance()->ImGuiRender();

    if (FAILED(CGameInstance::GetInstance()->Present()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainAnimTool::OpenLevel(LEVEL _eStartLevel)
{
	GAMEINSTANCE->OpenLevel(LEVEL_LOADING, CLevelAnimToolEdit::Create());

	return S_OK;
}

void CMainAnimTool::Free()
{
    CAnimToolImguiManager::DestroyInstance();
    CGameInstance::DestroyInstance();


}
