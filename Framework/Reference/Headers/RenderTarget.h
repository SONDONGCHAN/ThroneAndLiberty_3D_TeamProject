#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CRenderTarget
{
public:
	CRenderTarget(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	~CRenderTarget() = default;

public:
	//For. Pixel Picking
	wrl::ComPtr<ID3D11Texture2D> GetTexture2D() { return m_pTexture2D; }
	wrl::ComPtr<ID3D11RenderTargetView> GetRTV() const {
		return m_pRTV;
	}

public:
	HRESULT Initialize(_uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);
	HRESULT InitializeTex2DArr(_uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor, _uint _iArrSize);
	HRESULT InitializeTex3D(_uint _iSizeX, _uint _iSizeY, _uint _iDepth, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);
	
	HRESULT Clear();
	HRESULT BindSRV(const _char* _pConstantName);
	HRESULT BindUAV(const _char* _pConstantName);

	HRESULT CreateSRV();

	_bool GetIsDepthTarget() { return m_IsDepthTex; }

//#ifdef _DEBUG
public:
	HRESULT ReadyDebug(_float _fX, _float _fY, _float _fSizeX, _float _fSizeY, const wstring& _strTargetName);
	HRESULT Render();
//#endif

private:
	wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

	wrl::ComPtr<ID3D11Texture2D> m_pTexture2D = nullptr;
	wrl::ComPtr<ID3D11Texture3D> m_pTexture3D = nullptr;
	wrl::ComPtr<ID3D11RenderTargetView> m_pRTV = nullptr;
	wrl::ComPtr<ID3D11ShaderResourceView> m_pSRV = nullptr;
	wrl::ComPtr<ID3D11UnorderedAccessView> m_pUAV = nullptr;

	_float4 m_vClearColor = _float4();

private:
	_float4x4 m_WorldMatrix = _float4x4::Identity;
	_float2 m_vTargetNamePos = _float2();

	wstring m_strTargetName = TEXT("");
		
private:
	_bool m_IsDepthTex = false;

public:
	static shared_ptr<CRenderTarget> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, _uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);
	static shared_ptr<CRenderTarget> CreateTex2DArr(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, _uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor, _uint _iArrNum);
	void Free();

};

END