#pragma once

#include "Engine_Defines.h"
#include "Client_Enums.h"

#include <process.h>
#include <string>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include <stdio.h>
#include <thread>

#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_stdlib.h"

#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif

/*Console Print Wrapping*/
#define console printf_s

/*Effect*/
#define	MAX_TIMETABLE	5
#define MAX_COLORSPLIT	4
#define MAX_PARTICLE	500

namespace Client
{
	enum LEVEL {
		LEVEL_STATIC, //0
		LEVEL_LOGO, //1
		LEVEL_DUNGEON_MAIN, //2
		LEVEL_DUNGEON_FINAL, //3
		LEVEL_VILLAGE, //4
		LEVEL_FONOS, //5
		LEVEL_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace std;
using namespace Client;