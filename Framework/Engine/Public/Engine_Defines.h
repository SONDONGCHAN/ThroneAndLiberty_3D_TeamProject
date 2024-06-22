#pragma once

#pragma comment(lib, "user32.lib")
#pragma warning (disable : 4251)

#include <Strmif.h>
#include <control.h>
#include <dshow.h>
#include <initguid.h>

#pragma comment(lib, "strmiids.lib")

/*DirectX Library*/
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>
#define DIRECTINPUT_VERSION 0x0800
#define M_PI 3.14159265358979323846
#include <dinput.h>
#include <Effects11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK\ScreenGrab.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/SimpleMath.h>
using namespace DirectX;

#include <random>
#define MAX_BONE 512

#include "Engine_Enums.h"
#include "Engine_Function.h"
#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"
using namespace Engine;

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>


/*ComPtr*/
#include <wrl.h>
namespace wrl = Microsoft::WRL;
using namespace wrl;

#define GAMEINSTANCE CGameInstance::GetInstance() 

/*Shadow Lighting*/
#define CASCADE_NUM 3   //절두체 캐스케이드 갯수
#define SHADOWMAP_WIDTH 3000    // 셰도우 맵 가로 크기
#define SHADOWMAP_HEIGHT 2000   // 셰도우 맵 세로 크기 

/*SmartPointer*/
#include <memory>
#include <iostream>
/*File Reader*/
#include <filesystem>
/*STL*/
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <string>
#include <locale>
using namespace std;

/*Physics*/
#include "PhysX_Defines.h"


/*Json*/
#include "Json/json.h"
using namespace Json;

#include "Fmod/fmod.h"
#include "Fmod/fmod.hpp"
#include "Fmod/fmod_common.h"
#include "Fmod/fmod_memoryinfo.h"
#include "Fmod/fmod_dsp.h"
#include "Fmod/fmod_output.h"
#include "Fmod/fmod_dsp_effects.h"
#include "Fmod/fmod_errors.h"
#include "Fmod/fmod_codec.h"

#define MAXCHANNEL 32