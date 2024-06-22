#include "imgui.h"
#include "imgui_internal.h"

#include "ToolMgr.h"

#include "MapTool.h"
#include "TerrainTool.h"
#include "ObjectTool.h"

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "CameraFree.h"

IMPLEMENT_SINGLETON_MAPTOOL(CToolMgr)

CToolMgr::CToolMgr()
{
}

#ifdef CHECK_REFERENCE_COUNT
CToolMgr::~CToolMgr()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
#endif // CHECK_REFERENCE_COUNT

HRESULT CToolMgr::Initialize()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(DEVICE.Get(), CONTEXT.Get());

#pragma region Ready Camera
	m_pFreeCamera = static_pointer_cast<CCameraFree>(GAMEINSTANCE->GetGameObject(LEVEL_ATELIER, TEXT("Layer_Camera")));
	m_bIsCamOptionWindow = true;
	m_pMouseSensor = m_pFreeCamera->GetMouseSensor();
	m_pComTransform = static_pointer_cast<CTransform>(m_pFreeCamera->GetComponent(TEXT("Com_Transform")));
	m_fCamSpeed = 10.f;
	m_fCamRotationSpeed = XMConvertToRadians(90.0f);
#pragma endregion

	m_pTerrainTool = CTerrainTool::Create();
	m_pObjectTool = CObjectTool::Create();
	m_pMapTool = CMapTool::Create();

	return S_OK;
}

void CToolMgr::Tick(_float _fTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	
	OtherOptions();
	if (false == m_bIsToolsPause)
	{
		InputEvent();
		m_pMapTool->Tick(_fTimeDelta);
	}

#pragma region Demo_Window

	// 1. 큰 데모 창을 보여줍니다(샘플 코드의 대부분은 ImGui::ShowDemoWindow()! 해당 코드를 검색하여 Dear ImGui!에 대해 자세히 알아볼 수 있습니다.).
	if (m_bIsSowDemo)
		ImGui::ShowDemoWindow(&m_bIsSowDemo);

	// 2. 우리가 직접 만든 간단한 창을 보여주세요. Begin/End 쌍을 사용하여 명명된 창을 만듭니다.
	{


		ImGui::Begin("Hello, world!");                          // "Hello, world!"라는 창을 만들고 창에 추가합니다.

		ImGui::Text("This is some useful text.");               // 일부 텍스트 표시(형식 문자열을 사용할 수도 있음)
		ImGui::Checkbox("Demo Window", &m_bIsSowDemo);      // 창의 열림/닫힘 상태를 저장하는 bool 변수 수정
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &m_fAdjustableValue, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&m_fClearColor); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			m_iCounter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", m_iCounter);

		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

#pragma endregion


}

HRESULT CToolMgr::Render()
{
	if (m_bFirstCheck == false)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	m_bFirstCheck = false;

	return S_OK;
}

void CToolMgr::OtherOptions()
{
	ImGui::Begin("Camera Options", &m_bIsCamOptionWindow);

	
	if (ImGui::CollapsingHeader("Camera"))
	{
		m_fCamSpeed = m_pComTransform.lock()->GetSpeed();
		m_fCamRotationSpeed = XMConvertToDegrees(m_pComTransform.lock()->GetTurnSpeed());
		ImGui::PushItemWidth(150);
		ImGui::SliderFloat("Speed", &m_fCamSpeed, 0.f, 100.f, "%.1f");
		ImGui::SliderFloat("Turn Speed", &m_fCamRotationSpeed, 0.f, 180.f, "%.1f");
		ImGui::SliderFloat("Mouse Sensor", m_pMouseSensor, 0.f, 1.f, "%.2f");
		m_pComTransform.lock()->SetSpeed(m_fCamSpeed);
		m_pComTransform.lock()->SetTurnSpeed(XMConvertToRadians(m_fCamRotationSpeed));
	}
	
	if (GAMEINSTANCE->KeyPressing(DIK_RCONTROL) && GAMEINSTANCE->KeyDown(DIK_F1))
		m_bIsHotkeyLock = !m_bIsHotkeyLock;
	if (GAMEINSTANCE->KeyPressing(DIK_RCONTROL) && GAMEINSTANCE->KeyDown(DIK_F2))
		m_bIsToolsPause = !m_bIsToolsPause;

	ImGui::Checkbox("Hotkey Lock", &m_bIsHotkeyLock);
	ImGui::Checkbox("Tools Pause", &m_bIsToolsPause);

	ImGui::End();

	


}

void CToolMgr::InputEvent()
{
	if (GAMEINSTANCE->MouseDown(DIM_RB))
	{
		m_bIsMouseLock = !m_bIsMouseLock;
		m_pFreeCamera->ChangeLockState();
	}

	if (true == m_bIsMouseLock)
		Fix_Mouse();
}

void CToolMgr::Fix_Mouse()
{
	m_bIsMouseLock = true;
	POINT ptMouse{ g_iWinCX >> 1, g_iWinCY >> 1 };
	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

shared_ptr<CToolMgr> CToolMgr::Create()
{
	shared_ptr<CToolMgr> pInstance = WRAPPING(CToolMgr)();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CToolMgr");
		pInstance.reset();
	}

	return pInstance;
}
