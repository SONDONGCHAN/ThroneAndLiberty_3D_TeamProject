#include "pch.h"
#include "ImGuiMgr.h"
#include <Windows.h>
#include <locale>

#include "GameInstance.h"

IMPLEMENT_SINGLETON(Tool_UI::CImGuiMgr)

Tool_UI::CImGuiMgr::CImGuiMgr()
{

}

Tool_UI::CImGuiMgr::~CImGuiMgr()
{
	ShutDown();
}

HRESULT Tool_UI::CImGuiMgr::Initialize()
{
	m_pDevice = GAMEINSTANCE->GetDeviceInfo();
	m_pContext = GAMEINSTANCE->GetDeviceContextInfo();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	io.ImeWindowHandle = g_hWnd;
	io.Fonts->AddFontFromFileTTF("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/Font_Face_KR_Body_AsiaNGD.ttf", 12.0f, 0, io.Fonts->GetGlyphRangesKorean());

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get());

	return S_OK;
}

HRESULT Tool_UI::CImGuiMgr::StartFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return S_OK;
}

HRESULT Tool_UI::CImGuiMgr::Render()
{
	//TestDialog();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

void Tool_UI::CImGuiMgr::TestDialog()
{
	ImGui::Begin("Map Object Creator");

	ImGui::SeparatorText("Object Spawner");

	ImGui::End();
}

void Tool_UI::CImGuiMgr::ShutDown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
