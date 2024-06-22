#pragma once

#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture : public CComponent
{
public:
	CTexture(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize(const wstring& _strTextureFilePath, _uint _iNumTextures);

public:
	HRESULT BindShaderResource(const _char* _pConstantName, _uint _iIndex);
	HRESULT BindShaderResources(const _char* _pConstantName);


public:
	D3D11_TEXTURE2D_DESC GetTextureDesc();
	vector<wrl::ComPtr<ID3D11ShaderResourceView>> GetSRVs() { return m_SRVs; }
	wrl::ComPtr<ID3D11ShaderResourceView> GetSRV() { return m_SRV; }

private:
	_uint m_iNumTextures = { 0 };
	vector<wrl::ComPtr<ID3D11ShaderResourceView>> m_SRVs;
	wrl::ComPtr<ID3D11ShaderResourceView> m_SRV;

private:
	wstring m_strFilePath = TEXT("");

public:
	static shared_ptr<CTexture> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, const wstring& _strTextureFilepath, _uint _iNumTextures);

};

END
