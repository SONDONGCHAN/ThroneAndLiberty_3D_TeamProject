#pragma once

#include "Tool_Defines.h"
#include "Engine_Defines.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

namespace Tool_UI
{
	class CImGuiMgr
	{
		DECLARE_SINGLETON(Tool_UI::CImGuiMgr)

	private:
		CImGuiMgr();
		~CImGuiMgr();

	public:
		HRESULT Initialize();
		HRESULT StartFrame();

	public:
		HRESULT Render();

	private:
		void TestDialog();

	public:
		void ShutDown();

	private:
		wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
		wrl::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

	};
}
