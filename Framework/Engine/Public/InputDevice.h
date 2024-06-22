#ifndef InputDev_h__
#define InputDev_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class CInputDevice
{

public:
	CInputDevice();
	virtual ~CInputDevice(void) = default;

public:
	_byte	GetDIKeyState(_ubyte _byKeyID) { return m_byKeyState[_byKeyID]; }
	_byte	GetDIMouseState(MOUSEKEYSTATE _eMouse) { return m_tMouseState.rgbButtons[_eMouse]; }

	_long	GetDIMouseMove(MOUSEMOVESTATE _eMouseState)
	{
		return *(((_long*)&m_tMouseState) + _eMouseState);
	}

	_bool KeyDown(_ubyte _eKeyID);
	_bool KeyUp(_ubyte _eKeyID);
	_bool KeyPressing(_ubyte _eKeyID);

	_byte MouseDown(MOUSEKEYSTATE _eMouse);
	_byte MousePressing(MOUSEKEYSTATE _eMouse);
	_byte MouseUp(MOUSEKEYSTATE _eMouse);

public:
	HRESULT ReadyInputDev(HINSTANCE _hInst, HWND _hWnd);
	void	Tick(void);

private:

	wrl::ComPtr<IDirectInput8> m_pInputSDK = nullptr;

private:

	wrl::ComPtr<IDirectInputDevice8> m_pKeyBoard = nullptr;
	wrl::ComPtr<IDirectInputDevice8> m_pMouse = nullptr;

private:
	_byte	m_byKeyState[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	_byte	m_preKeyState[256];
	_bool	m_bKeyState[256];

	DIMOUSESTATE			m_tMouseState;
	DIMOUSESTATE			m_preMouseState;

public:
	static shared_ptr<CInputDevice> Create(HINSTANCE hInst, HWND hWnd);

};

END
#endif // InputDev_h__