#include "CustomFont.h"

CCustomFont::CCustomFont(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    : m_pDevice(_pDevice),
    m_pContext(_pContext)
{
}

HRESULT CCustomFont::Initialize(const wstring& _strFontFilePath)
{
    m_pBatch = make_shared<SpriteBatch>(m_pContext.Get());
    m_pFont = make_shared<SpriteFont>(m_pDevice.Get(), _strFontFilePath.c_str());

    return S_OK;
}

HRESULT CCustomFont::Render(const wstring& _strText, const _float2& _vPosition, EFONT_ALIGNMENT _eFontAlignment, _color _vColor, _float _fRotation, const _float2& _vOrigin, _float _fScale)
{

    RECT rc = m_pFont->MeasureDrawBounds(_strText.c_str(), _vPosition);

    _float fHeight = (_float)(rc.bottom - rc.top);
    _float fWidth = (_float)(rc.right - rc.left);
    _float2 vOriginPos;

    switch (_eFontAlignment)
    {
    /*vPosition을 중심으로 font 배치*/
    case Engine::CCustomFont::FA_CENTER:
        vOriginPos = _vPosition + _float2(fWidth / 2.f, fHeight / 2.f);
        break;
     /*vPosition을 왼쪽으로 두고 font 배치*/
    case Engine::CCustomFont::FA_LEFT:
        //vOriginPos = _vPosition + _float2(-fWidth, fHeight / 2.f);
        vOriginPos = _vPosition + _float2(0.f, fHeight / 2.f);
        break;
    /*vPosition을 오른쪽으로 두고 font 배치*/
    case Engine::CCustomFont::FA_RIGHT:
        vOriginPos = _vPosition + _float2(fWidth, fHeight / 2.f);
        break;
    case Engine::CCustomFont::FA_CUSTOMCENTER:
    {
        vOriginPos = _vPosition - _float2(fWidth / 2.f, 0.f);
    }
    break;
    case Engine::CCustomFont::FA_END:
        vOriginPos = _vPosition;
        break;
    default:
        break;
    }

    m_pContext->GSSetShader(nullptr, nullptr, 0);
    m_pBatch->Begin();
    m_pFont->DrawString(m_pBatch.get(), _strText.c_str(), vOriginPos, _vColor, _fRotation, _vOrigin, _fScale);
    m_pBatch->End();

    return S_OK;
}

shared_ptr<CCustomFont> CCustomFont::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, const wstring& _strFontFilePath)
{
    shared_ptr<CCustomFont> pInstance = make_shared<CCustomFont>(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_strFontFilePath)))
        MSG_BOX("Failed to Create : CCustomFont");

    return pInstance;
}
