#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CGraphicDev final
{
public:
	CGraphicDev();
	~CGraphicDev();

public:
	HRESULT ReadyGraphicDevice(const GRAPHIC_DESC& _GraphicDesc);
	HRESULT ClearBackBufferView(_float4 _vClearColor);
	HRESULT ClearDepthStencilView();
	HRESULT Present();


private:

	wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> m_pDeviceContext = nullptr;
	wrl::ComPtr<IDXGISwapChain> m_pSwapChain = nullptr;
	wrl::ComPtr<ID3D11RenderTargetView> m_pBackBufferRTV = nullptr;
	wrl::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = nullptr;

private:
	HRESULT ReadySwapChain(HWND _hWnd, _bool _isWindowed, _uint _iWinCX, _uint _iWinCY);
	HRESULT ReadyBackBufferRenderTargetView();
	HRESULT ReadyDepthStencilRenderTargetView(_uint _iWinCX, _uint _iWinCY);

public:
	static std::shared_ptr<CGraphicDev> Create(const GRAPHIC_DESC& _GraphicDesc);


	wrl::ComPtr<ID3D11Device> GetDeviceInfo() { return m_pDevice; }
	wrl::ComPtr<ID3D11DeviceContext> GetDeviceContextInfo() { return m_pDeviceContext; }

	void Free();
};

END