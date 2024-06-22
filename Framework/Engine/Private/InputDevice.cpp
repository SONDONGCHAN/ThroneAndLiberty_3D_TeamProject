#include "InputDevice.h"

CInputDevice::CInputDevice()
{
}

_bool CInputDevice::KeyDown(_ubyte eKeyID)
{
	if (!(m_preKeyState[eKeyID] & 0x80) && (m_byKeyState[eKeyID] & 0x80))
		return true;

	return false;
}

_bool CInputDevice::KeyUp(_ubyte eKeyID)
{
	if ((m_preKeyState[eKeyID] & 0x80) && !(m_byKeyState[eKeyID] & 0x80))
		return true;

	return false;
}

_bool CInputDevice::KeyPressing(_ubyte eKeyID)
{
	if ((m_preKeyState[eKeyID] & 0x80) && (m_byKeyState[eKeyID] & 0x80))
		return true;

	return false;
}

_byte CInputDevice::MouseDown(MOUSEKEYSTATE _eMouse)
{
	return ((m_tMouseState.rgbButtons[_eMouse]) && !(m_preMouseState.rgbButtons[_eMouse]));
}

_byte CInputDevice::MousePressing(MOUSEKEYSTATE _eMouse)
{
	return ((m_tMouseState.rgbButtons[_eMouse]) && (m_preMouseState.rgbButtons[_eMouse]));
}

_byte CInputDevice::MouseUp(MOUSEKEYSTATE _eMouse)
{
	return (!(m_tMouseState.rgbButtons[_eMouse]) && !(m_preMouseState.rgbButtons[_eMouse]));
}

HRESULT CInputDevice::ReadyInputDev(HINSTANCE hInst, HWND hWnd)
{
	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		nullptr)))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyBoard->Acquire();

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->Acquire();


	return S_OK;
}

void CInputDevice::Tick(void)
{
	memcpy(m_preKeyState, m_byKeyState, sizeof(_byte) * 256);
	memcpy(&m_preMouseState, &m_tMouseState, sizeof(m_tMouseState));

	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

shared_ptr<CInputDevice> CInputDevice::Create(HINSTANCE _hInst, HWND _hWnd)
{
	shared_ptr<CInputDevice> pInstance = make_shared<CInputDevice>();

	if (FAILED(pInstance->ReadyInputDev(_hInst, _hWnd)))
		MSG_BOX("Failed to Created : CInput_Device");

	return pInstance;
}
