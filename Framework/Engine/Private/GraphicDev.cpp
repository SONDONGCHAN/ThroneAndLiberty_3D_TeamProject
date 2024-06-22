#include "GraphicDev.h"

CGraphicDev::CGraphicDev()
    :m_pDevice(nullptr)
    ,m_pDeviceContext(nullptr)
{
}

CGraphicDev::~CGraphicDev()
{
	//Free();
}

HRESULT CGraphicDev::ReadyGraphicDevice(const GRAPHIC_DESC& _GraphicDesc)
{
	_uint		iFlag = 0;

//#ifdef _DEBUG
//	iFlag = D3D11_CREATE_DEVICE_DEBUG;
//#endif
	D3D_FEATURE_LEVEL			FeatureLV;

	/* 그래픽 장치를 초기화한다. */
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &FeatureLV, &m_pDeviceContext)))
		return E_FAIL;

	if (FAILED(ReadySwapChain(_GraphicDesc.hWnd, _GraphicDesc.isWindowed, _GraphicDesc.iWinSizeX, _GraphicDesc.iWinSizeY)))
		return E_FAIL;

	if (FAILED(ReadyBackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(ReadyDepthStencilRenderTargetView(_GraphicDesc.iWinSizeX, _GraphicDesc.iWinSizeY)))
		return E_FAIL;

	wrl::ComPtr<ID3D11RenderTargetView> pRTVs[1] = {
		m_pBackBufferRTV,
	};


	m_pDeviceContext->OMSetRenderTargets(1, pRTVs->GetAddressOf(),
		m_pDepthStencilView.Get());

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)_GraphicDesc.iWinSizeX;
	ViewPortDesc.Height = (_float)_GraphicDesc.iWinSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CGraphicDev::ClearBackBufferView(_float4 _vClearColor)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV.Get(), (_float*)&_vClearColor);

	return S_OK;
}

HRESULT CGraphicDev::ClearDepthStencilView()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphicDev::Present()
{
	if (nullptr == m_pSwapChain)
		return E_FAIL;

	m_pBackBufferRTV;

	return m_pSwapChain->Present(0, 0);
}

HRESULT CGraphicDev::ReadySwapChain(HWND _hWnd, _bool _isWindowed, _uint _iWinCX, _uint _iWinCY)
{
	wrl::ComPtr <IDXGIDevice> pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	wrl::ComPtr<IDXGIAdapter> pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	wrl::ComPtr <IDXGIFactory> pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	/* 스왑체인을 생성한다. = 텍스쳐를 생성하는 행위 + 스왑하는 형태  */
	DXGI_SWAP_CHAIN_DESC		SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	/*텍스처(백버퍼)를 생성하는 행위*/
	SwapChain.BufferDesc.Width = _iWinCX;
	SwapChain.BufferDesc.Height = _iWinCY;


	SwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChain.BufferCount = 1;

	/*스왑하는 형태*/
	SwapChain.BufferDesc.RefreshRate.Numerator = 60;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;
	SwapChain.SampleDesc.Quality = 0;
	SwapChain.SampleDesc.Count = 1;

	SwapChain.OutputWindow = _hWnd;
	SwapChain.Windowed = _isWindowed;
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	/* 백버퍼라는 텍스처를 생성했다. */
	if (FAILED(pFactory->CreateSwapChain(m_pDevice.Get(), &SwapChain, &m_pSwapChain)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGraphicDev::ReadyBackBufferRenderTargetView()
{
	if (nullptr == m_pDevice)
		return E_FAIL;


	wrl::ComPtr<ID3D11Texture2D> pBackBufferTexture = nullptr;

	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pBackBufferTexture.GetAddressOf())))
		return E_FAIL;


	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture.Get(), nullptr, &m_pBackBufferRTV)))
		return E_FAIL;


	return S_OK;
}

HRESULT CGraphicDev::ReadyDepthStencilRenderTargetView(_uint _iWinCX, _uint _iWinCY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	wrl::ComPtr<ID3D11Texture2D> pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = _iWinCX;
	TextureDesc.Height = _iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, pDepthStencilTexture.GetAddressOf())))
		return E_FAIL;



	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture.Get(), nullptr, m_pDepthStencilView.GetAddressOf())))
		return E_FAIL;


	return S_OK;
}


std::shared_ptr<CGraphicDev> CGraphicDev::Create(const GRAPHIC_DESC& _GraphicDesc)
{
	shared_ptr<CGraphicDev> pInstance = make_shared<CGraphicDev>();

	if (FAILED(pInstance->ReadyGraphicDevice(_GraphicDesc)))
	{
		MSG_BOX("Failed to Created : CGraphic_Device");
	}

	return pInstance;
}

void CGraphicDev::Free()
{

	//-------------------------------------Com객체 디버깅 로그--------------------------------------------------------------

#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug* d3dDebug;
	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr))
	{
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
		OutputDebugStringW(L"----- ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ \r ");

		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	}
	if (d3dDebug != nullptr)            d3dDebug->Release();
#endif


}
