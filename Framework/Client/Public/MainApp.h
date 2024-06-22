#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CMainApp final
{
public:
	CMainApp();
	~CMainApp();

public:
	HRESULT Initialize();
	void Tick(_float _fTimeDelta);
	HRESULT Render();

private:
	HRESULT OpenLevel(LEVEL _eStartLevel);

	HRESULT UILoader();

	shared_ptr<class CImguiMgr> m_pImguiMgr = nullptr;

private:
	void KeyInput();

/*For. Debug Console Print*/
private:
	FILE* pFile = nullptr;
	_bool m_IsDebugOptionOn = false;
	_bool m_IsHalfScreen = false;
	_bool m_IsUIOn = true;
	_bool m_IsQuestUIOn = true;

	UINT m_iFPS{ 0 };
	wchar_t m_szFPS[MAX_PATH]{ L"" };
	float m_fTimeAcc{ 0.0f };

public:
	void Free();


};

END