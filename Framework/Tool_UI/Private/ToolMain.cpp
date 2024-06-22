#include "pch.h"
#include "ToolMain.h"
#include "LevelDev.h"
#include "ImGuiMgr.h"

#include "GameInstance.h"

Tool_UI::CToolMain::CToolMain()
{

}

Tool_UI::CToolMain::~CToolMain()
{
	Free();
}

HRESULT Tool_UI::CToolMain::Initialize()
{
    /*Random Seed*/
    srand((unsigned)time(NULL));

    GRAPHIC_DESC GraphicDesc = {};

    GraphicDesc.hWnd = g_hWnd;
    GraphicDesc.isWindowed = true;
    GraphicDesc.iWinSizeX = g_iWinSizeX;
    GraphicDesc.iWinSizeY = g_iWinSizeY;

    if (FAILED(GAMEINSTANCE->InitializeEngine(g_hInst, TOOL_LEVEL_END, GraphicDesc)))
        return E_FAIL;

    GAMEINSTANCE->LoadLevelResource(TOOL_LEVEL_STATIC);

    if (FAILED(OpenLevel(TOOL_LEVEL_DEV)))
        return E_FAIL;

    if (FAILED(Tool_UI::CImGuiMgr::GetInstance()->Initialize()))
        return E_FAIL;

    return S_OK;
}

void Tool_UI::CToolMain::Tick(_float _fTimeDelta)
{
    if (FAILED(Tool_UI::CImGuiMgr::GetInstance()->StartFrame()))
        return;

    GAMEINSTANCE->TickEngine(_fTimeDelta);
}

HRESULT Tool_UI::CToolMain::Render()
{
    if (FAILED(GAMEINSTANCE->ClearBackBufferView(_float4(0.f, 0.5f, 0.5f, 1.f))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->ClearDepthStencilView()))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->Draw()))
        return E_FAIL;

    if (FAILED(Tool_UI::CImGuiMgr::GetInstance()->Render()))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->Present()))
        return E_FAIL;

    return S_OK;
}

HRESULT Tool_UI::CToolMain::OpenLevel(ETOOL_LEVEL _eStartLevel)
{
    GAMEINSTANCE->OpenLevel(TOOL_LEVEL_DEV, Tool_UI::CLevelDev::Create());

	return S_OK;
}

void Tool_UI::CToolMain::Free()
{
	GAMEINSTANCE->DestroyInstance();
}
