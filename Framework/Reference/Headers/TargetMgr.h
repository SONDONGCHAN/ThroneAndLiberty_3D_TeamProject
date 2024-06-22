#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CTargetMgr
{
public:
	CTargetMgr(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	~CTargetMgr() = default;

public:
	//For. Pixel Picking
	ComPtr<ID3D11Texture2D> GetTexture2DForTarget(const wstring& _strTargetTag);

public:
	HRESULT Initialize();
	HRESULT AddRenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT AddRenderTargetTex2DArr(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _uint _iArrSize);
	HRESULT AddMRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT BeginMRT(const wstring& strMRTTag);
	HRESULT BeginMRT(const wstring& strMRTTag, ID3D11DepthStencilView* _pDSV);
	HRESULT EndMRT();
	HRESULT BindSRV(const wstring& _strTargetTag, const _char* _pConstantName);
	HRESULT BindUAV(const wstring& _strTargetTag, const _char* _pConstantName);
	HRESULT CreateTargetSRV(const wstring& _strTargetTag);

public:
	/*Post Process Effect 수행 시 BackBufferView를 얻어오기 위한 CopyBuffer를 생성*/
	HRESULT CreateCopyBuffer(_uint iSizeX, _uint iSizeY);

	/*Post Process Effect 수행 때 CopyBackBuffer를 Bind해주는 Function*/
	HRESULT BindBackBufferSRV( const _char* _pConstantName);

//#ifdef _DEBUG
public:
	HRESULT ReadyDebug(const wstring& _strTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY);
	HRESULT RenderMRT(const wstring& _strMRTTag);
//#endif

private:

	wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

	/*For. Post Process*/
	wrl::ComPtr<ID3D11Texture2D> m_pCopyRes = nullptr;
	wrl::ComPtr<ID3D11ShaderResourceView> m_pBackBufferSRV = nullptr;


	ID3D11RenderTargetView* m_pBackBufferView = { nullptr };
	wrl::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = { nullptr };


private:
	map<const wstring, shared_ptr<class CRenderTarget>> m_RenderTargets;
	map<const wstring, list<shared_ptr<class CRenderTarget>>> m_MRTs;

private:
	shared_ptr<class CRenderTarget> FindRenderTarget(const wstring& _strTargetTag);
	list<shared_ptr<class CRenderTarget>>* FindMRT(const wstring& _strMRTTag);

public:
	static shared_ptr<CTargetMgr> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);




};

END