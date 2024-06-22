#include "Texture.h"
#include "Shader.h"
#include "GameInstance.h"

CTexture::CTexture(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	:CComponent(_pDevice, _pContext)
{
}

HRESULT CTexture::Initialize(const wstring& _strTextureFilePath, _uint _iNumTextures)
{

	m_iNumTextures = _iNumTextures;
	m_strFilePath = _strTextureFilePath;
	_tchar szEXT[MAX_PATH] = TEXT("");

	_wsplitpath_s(_strTextureFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

	for (size_t i = 0; i < _iNumTextures; i++) {

		HRESULT hr = { 0 };

		wrl::ComPtr<ID3D11ShaderResourceView> pSRV = { nullptr };

		if (false == lstrcmp(szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice.Get(), _strTextureFilePath.c_str(), nullptr, &m_SRV);
		else if (false == lstrcmp(szEXT, TEXT(".tga")))
			hr = E_FAIL;
		else
			//hr = CreateWICTextureFromFile(m_pDevice.Get(), _strTextureFilePath.c_str(), nullptr, &m_SRV);
			hr = CreateWICTextureFromFileEx(m_pDevice.Get(), m_pContext.Get(), _strTextureFilePath.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS, WIC_LOADER_DEFAULT, nullptr, &m_SRV);

		if (FAILED(hr))
			return E_FAIL;

	}

	return S_OK;
}

HRESULT CTexture::BindShaderResource(const _char* _pConstantName, _uint _iIndex)
{
	if (_iIndex >= m_iNumTextures)
		return E_FAIL;

	return S_OK;
	// return GAMEINSTANCE->BindSRV(_pConstantName, m_SRV);
}

HRESULT CTexture::BindShaderResources(const _char* _pConstantName)
{
	return S_OK;
	//
	// return GAMEINSTANCE->BindSRVs(_pConstantName, m_SRV.GetAddressOf(), m_iNumTextures);
}

D3D11_TEXTURE2D_DESC CTexture::GetTextureDesc()
{
	wrl::ComPtr<ID3D11Texture2D> pTexture2D;
	wrl::ComPtr<ID3D11Resource> pResource;
	D3D11_TEXTURE2D_DESC Desc;

	m_SRV->GetResource(&pResource);

	pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture2D);
	pTexture2D->GetDesc(&Desc);

	return Desc;
}

shared_ptr<CTexture> CTexture::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, const wstring& _strTextureFilepath, _uint _iNumTextures)
{
	shared_ptr<CTexture> pTexture = make_shared<CTexture>(_pDevice, _pContext);

	if (FAILED(pTexture->Initialize(_strTextureFilepath, _iNumTextures)))
		MSG_BOX("Failed to Create : CTexture");

	return pTexture;
}
