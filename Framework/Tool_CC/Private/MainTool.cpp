#include "pch.h"
#include "MainTool.h"
#include "LevelEditor.h"

shared_ptr<CMainTool> CMainTool::m_pInstance = nullptr;

CMainTool::CMainTool()
{
}

CMainTool::~CMainTool()
{
	Free();
}

HRESULT CMainTool::Initialize()
{
	/*Random Seed*/
	srand((unsigned)time(NULL));

    GRAPHIC_DESC GraphicDesc = {};

    GraphicDesc.hWnd = g_hWnd;
    GraphicDesc.isWindowed = true;
    GraphicDesc.iWinSizeX = g_iWinSizeX;
    GraphicDesc.iWinSizeY = g_iWinSizeY;

    if (FAILED(GAMEINSTANCE->InitializeEngine(g_hInst, LEVEL_END, GraphicDesc)))
    {
        MessageBox(nullptr, L"InitializeEngine Failed", L"CMainTool::Initialize", MB_OK);
        return E_FAIL;
    }

    m_pDevice = GAMEINSTANCE->GetDeviceInfo();
    m_pContext = GAMEINSTANCE->GetDeviceContextInfo();

    if (FAILED(Setup_ImGui()))
    {
        MessageBox(nullptr, L"Setup_ImGui Failed", L"CMainTool::Initialize", MB_OK);
        return E_FAIL;
    }
    /*
    	if (0 == _iLevelIdx) {
		StaticResourceLoad();
	}
	else if (1 == _iLevelIdx) {
		LogoResourceLoad();
	}
	// LEVEL_DUNGEON_MAIN
	else if (2 == _iLevelIdx) {
		DungeonMainResourceLoad();
	}
	// LEVEL_DUNGEON_FINAL
	else if (3 == _iLevelIdx) {
		DungeonFinalResourceLoad();
	}
	// LEVEL_VILLAGE
	else if (4 == _iLevelIdx) {
		VillageResourceLoad();
	}
	//LEVEL_FONOS
	else if (5 == _iLevelIdx) {
		FonosResourceLoad();
	}
	else if (10 == _iLevelIdx) {
		MapToolResourceLoad();
	}

	else if (20 == _iLevelIdx)
		EffectToolResourceLoad();
    */
    GAMEINSTANCE->LoadLevelResource(5);
    //GAMEINSTANCE->LoadLevelResource(3);
    GAMEINSTANCE->LoadLevelResource(LEVEL_STATIC);

    GAMEINSTANCE->OpenLevel(LEVEL_EDITOR, CLevelEditor::Create());

	return S_OK;
}

void CMainTool::Tick(float _fTimeDelta)
{
    GAMEINSTANCE->TickEngine(_fTimeDelta);
}

HRESULT CMainTool::Render()
{
    ImGui::Render();

    if (FAILED(CGameInstance::GetInstance()->ClearBackBufferView(_float4(0.f, 0.f, 1.f, 1.f))))
        return E_FAIL;

    if (FAILED(CGameInstance::GetInstance()->ClearDepthStencilView()))
        return E_FAIL;

    if (FAILED(CGameInstance::GetInstance()->Draw()))
        return E_FAIL;

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (FAILED(CGameInstance::GetInstance()->Present()))
        return E_FAIL;

	return S_OK;
}

HRESULT CMainTool::Setup_ImGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    ImGui::StyleColorsClassic();
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4{ 1.0f,0.0f,1.0f,0.05f };

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get());

    return S_OK;
}

void CMainTool::Free()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

	m_pContext = nullptr;
	m_pDevice = nullptr;

	CGameInstance::DestroyInstance();
}