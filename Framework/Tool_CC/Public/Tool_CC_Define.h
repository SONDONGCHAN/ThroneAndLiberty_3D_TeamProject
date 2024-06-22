#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuizmo.h"
#include "Engine_Defines.h"
#include "GameInstance.h"

#include <chrono>
#include <ctime>
#include <process.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include <stdio.h>
#include <thread>

#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif

/*Console Print Wrapping*/
#define console printf_s

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace std;

const unsigned int g_iWinSizeX = 1280;
const unsigned int g_iWinSizeY = 720;

enum ELEVELS { LEVEL_STATIC, LEVEL_EDITOR, LEVEL_END };