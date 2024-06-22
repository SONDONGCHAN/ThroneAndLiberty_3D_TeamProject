#pragma once
#include <fstream>

#include <thread>
#include <limits>

#include <array>
#include <map>
#include <set>

#include "Engine_Defines.h"
#include "GameInstance.h"

#include "Tool_Macro.h"

#include "imgui.h"

namespace Tool_Map
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_ATELIER, LEVEL_END, LEVEL_MAPTOOL = 10 };
	enum AXISTYPE { AXIS_X, AXIS_Y, AXIS_Z, AXIS_END };

	const unsigned int g_iWinCX = 1280;
	const unsigned int g_iWinCY = 720;
}

//using std::filesystem::directory_iterator;

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace Tool_Map;

