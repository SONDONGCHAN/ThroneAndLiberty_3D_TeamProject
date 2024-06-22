#pragma once


/* =============================================== */
//	[Shader]
// 
//  .hlsl 파일 이름 format : Shader_(셰이더 이름)_(ELEMENT_DESC KeyTag)
//	Element Desc Tag 이름은 Engine Typedef를 참고한다
//
/* =============================================== */


#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CShader final
{
public:
	CShader(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	~CShader() = default;

public:
	virtual HRESULT Initialize(const wstring& _strShaderPath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);

public:
	/*고정 수치를 시작단계에 미리 Bind 시켜둡니다*/
	/* Screen Size, Far */
	HRESULT BindBaseValue(_float _fScreenX, _float _fScreenY , _float _fFar);

public:
	HRESULT Begin(_uint iPassIndex, _uint iTechniqueIndex = 0);
	HRESULT BindRawValue(const _char* _pConstantName, const void* _pData, _uint _iLength);
	HRESULT BindMatrix(const _char* pConstantName, const _float4x4* pMatrix);
	HRESULT BindMatrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices);
	HRESULT BindSRV(const _char* pConstantName, wrl::ComPtr<ID3D11ShaderResourceView> _pSRV);
	HRESULT BindSRVs(const _char* pConstantName, ID3D11ShaderResourceView** _pSRV, _uint _iNumTextures);
	HRESULT BindFloatVectorArray(const _char* _pConstantName, const _float4* _pVectors, _uint _iNumVectors);
	HRESULT BindUAV(const _char* _pConstantName, wrl::ComPtr<ID3D11UnorderedAccessView> _pUAV);

private:
	wrl::ComPtr<ID3DX11Effect> m_pEffect = { nullptr };
	vector<wrl::ComPtr<ID3D11InputLayout>> m_InputLayouts;

private:
	wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

public:
	static shared_ptr<CShader> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, const wstring& _strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pElements, _uint _iNumElements);

};

END