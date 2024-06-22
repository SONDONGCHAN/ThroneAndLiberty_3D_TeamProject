#pragma once

#include "Engine_Defines.h"

//#include <iostream>
//#include <process.h>
//#include <string>
//#include <cstring>
//#include <unordered_set>
//#include <io.h>
#include "GameInstance.h"

#define	MAX_TIMETABLE	5
#define MAX_COLORSPLIT	4
#define MAX_PARTICLE	2000

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"

//#include "ImGuizmo\ImGuizmo.h"

//#include <wincodec.h>
//#pragma comment(lib, "windowscodecs.lib")

namespace Tool_Effect
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_TOOL, LEVEL_END, LEVEL_EFFECTTOOL = 20 };

	enum TEX_TYPE { TEX_DIFFUSE, TEX_MASK, TEX_NOISE, TEX_END };
	enum RSC_TYPE { RSC_DIFFUSE, RSC_MASK, RSC_NOISE, RSC_PARTICLE, RSC_MODEL, RSC_END };
	enum SAMPLE_TYPE { SAMPLE_WRAP, SAMPLE_MIRROR, SAMPLE_CLAMP, SAMPLE_BORDER, SAMPLE_END };
	enum UV_TYPE { UV_FIXED, UV_SCROLL, UV_ATLAS, UV_END };
	
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;

using namespace std;
using namespace Tool_Effect;
