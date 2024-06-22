#include "FontMgr.h"
#include "CustomFont.h"

CFontMgr::CFontMgr(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice(_pDevice),
    m_pContext(_pContext)
{
}

HRESULT CFontMgr::Initialize()
{
    return S_OK;
}

HRESULT CFontMgr::AddFont(const string& _strFontTag, const wstring& _strFontFilePath)
{
    if (nullptr != FindFont(_strFontTag))
        return E_FAIL;

    shared_ptr<CCustomFont> pFont = CCustomFont::Create(m_pDevice, m_pContext, _strFontFilePath);

    if (!pFont)
        return E_FAIL;

    m_Fonts.emplace(_strFontTag, pFont);

    return S_OK;
}

HRESULT CFontMgr::Render(const string& _strFontTag, const wstring& _strText, const _float2& _vPosition, CCustomFont::EFONT_ALIGNMENT _eFontAlignment, _color _vColor, _float _fRotation, const _float2& _vOrigin, _float _fScale)
{
    shared_ptr<CCustomFont> pFont = FindFont(_strFontTag);

    if (!pFont)
        return E_FAIL;

    return pFont->Render(_strText, _vPosition, _eFontAlignment, _vColor, _fRotation, _vOrigin, _fScale);
}

shared_ptr<class CCustomFont> CFontMgr::FindFont(const string& _strFontTag)
{
    auto iter = m_Fonts.find(_strFontTag);

    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

shared_ptr<CFontMgr> CFontMgr::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
{
    shared_ptr<CFontMgr> pInstance = make_shared<CFontMgr>(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CFontMgr");

    return pInstance;
}
