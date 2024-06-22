#pragma once

#include "DirectXTK/SimpleMath.h"

#include "Engine_Defines.h"

BEGIN(Engine)

class CCustomFont final
{
public:
	enum EFONT_ALIGNMENT {
		FA_CENTER,
		FA_LEFT,
		FA_RIGHT,
		FA_CUSTOMCENTER,
		FA_END
	};

public:
	CCustomFont(wrl::ComPtr<ID3D11Device> _pDevice , wrl::ComPtr<ID3D11DeviceContext> _pContext);
	~CCustomFont() = default;

public:
	HRESULT Initialize(const wstring& _strFontFilePath);
	HRESULT Render(const wstring& _strText, const _float2& _vPosition, EFONT_ALIGNMENT _eFontAlignment, _color _vColor, _float _fRotation, const _float2& _vOrigin, _float _fScale);

public:
	shared_ptr<SpriteFont> GetFont() { return m_pFont; }

private:
	wrl::ComPtr<ID3D11Device> m_pDevice = { nullptr };
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

private:
	shared_ptr<SpriteBatch> m_pBatch = { nullptr };
	shared_ptr<SpriteFont> m_pFont = { nullptr };

public:
	static shared_ptr<CCustomFont> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, const wstring& _strFontFilePath);

};

END