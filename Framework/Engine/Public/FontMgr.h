#pragma once

#include "Engine_Defines.h"
#include "CustomFont.h"

BEGIN(Engine)

class CFontMgr
{
public:
	CFontMgr(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	~CFontMgr() = default;

public:
	HRESULT Initialize();
	HRESULT AddFont(const string& _strFontTag, const wstring& _strFontFilePath);
	HRESULT Render(const string& _strFontTag, const wstring& _strText, const _float2& _vPosition, CCustomFont::EFONT_ALIGNMENT _eFontAlignment, _color _vColor = {1.f, 1.f, 1.f, 1.f}, _float _fRotation = 0.f, const _float2& _vOrigin = _float2(0.f, 0.f), _float _fScale = 1.f);

public:
	shared_ptr<CCustomFont> FindFont(const string& _strFontTag);

private:
	wrl::ComPtr<ID3D11Device> m_pDevice = { nullptr };
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

private:
	unordered_map<string, shared_ptr<CCustomFont>> m_Fonts;

public:
	static shared_ptr<CFontMgr> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);

};

END