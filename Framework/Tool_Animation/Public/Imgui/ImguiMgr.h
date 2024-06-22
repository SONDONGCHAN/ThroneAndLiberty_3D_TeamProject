#pragma once

#include "Engine_Defines.h"
#include "Assimp_Defines.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"


BEGIN(Engine)
class CModel;
class CAnimation;
END

BEGIN(Assimp)

class CImguiMgr
{
	DECLARE_SINGLETON(CImguiMgr)

private:
	CImguiMgr();
	~CImguiMgr();

public:
	HRESULT Initialize();
	HRESULT StartFrame();

public:
	HRESULT Render(void);

private:
	void TestDialog();

public:
	void ShutDown();

private:
	wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

};

END

