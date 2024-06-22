#pragma once
#include "Tool_Defines.h"
#include "ImGuizmo.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Tool_Map)
class CCameraFree;

class CMapTool;
class CTerrainTool;
class CObjectTool;

class CTerrain;
class CEnvironmentObject;

class CToolMgr
{
	DECLARE_SINGLETON_MAPTOOL(CToolMgr)

private:
	CToolMgr();
	DESTRUCTOR(CToolMgr)

public:
	_bool GetIsHotkeyLock() { return m_bIsHotkeyLock; }

public:
	HRESULT Initialize();
	void Tick(_float _fTimeDelta);
	HRESULT Render();

public:
	shared_ptr<CMapTool> GetMapTool() { return m_pMapTool; }
	shared_ptr<CTerrainTool> GetTerrainTool() { return m_pTerrainTool; }
	shared_ptr<CObjectTool> GetObjectTool() { return m_pObjectTool; }

private:
	void OtherOptions();
	void InputEvent();
	void Fix_Mouse();

private:
#pragma region Demo_Window
	_bool m_bIsSowDemo{};
	_int m_iCounter{};
	_float m_fAdjustableValue{};
	_float4 m_fClearColor{ 1.f, 1.f, 1.f, 1.f };
#pragma endregion

	bool show_another_window{ true };

	bool m_bFirstCheck{ true };

	_bool m_bIsHotkeyLock{};
	_bool m_bIsToolsPause{};

	_bool m_bIsMouseLock{ true };

	shared_ptr<CCameraFree> m_pFreeCamera{};

	shared_ptr<CMapTool> m_pMapTool{};
	shared_ptr<CTerrainTool> m_pTerrainTool{};
	shared_ptr<CObjectTool> m_pObjectTool{};

	_bool m_bIsCamOptionWindow{};
	_float m_fCamSpeed{};
	_float m_fCamRotationSpeed{};
	_float* m_pMouseSensor{};
	weak_ptr<CTransform> m_pComTransform{};

public:
	static shared_ptr<CToolMgr> Create();
};
END