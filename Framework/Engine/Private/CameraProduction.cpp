#include "CameraProduction.h"
#include "Camera.h"

CCameraProduction::CCameraProduction()
{
}

CCameraProduction::CCameraProduction(const shared_ptr<CCameraProduction> _pOriginal)
	: m_fLerpTime(_pOriginal->m_fLerpTime), m_eFadeFlag(_pOriginal->m_eFadeFlag), m_fFadeInOutTime(_pOriginal->m_fFadeInOutTime), m_vEye(_pOriginal->m_vEye), m_vAt(_pOriginal->m_vAt)
    ,m_hasCameraShake(_pOriginal->m_hasCameraShake), m_fShakeStartTime(_pOriginal->m_fShakeStartTime), m_iShakeFlags(_pOriginal->m_iShakeFlags), m_fShakeDuration(_pOriginal->m_fShakeDuration)
{
	memcpy_s(&m_RotDesc, sizeof(ROT_DESC), &_pOriginal->m_RotDesc, sizeof(ROT_DESC));
	memcpy_s(&m_LocDesc, sizeof(LOC_DESC), &_pOriginal->m_LocDesc, sizeof(LOC_DESC));
	memcpy_s(&m_FovDesc, sizeof(FOV_DESC), &_pOriginal->m_FovDesc, sizeof(FOV_DESC));
}

shared_ptr<CCameraProduction> CCameraProduction::Create(float _fLerpTime, FXMVECTOR _vEye, FXMVECTOR _vAt)
{
    shared_ptr<CCameraProduction> pInstance = make_shared<CCameraProduction>();

    if (FAILED(pInstance->Initialize(_fLerpTime, _vEye, _vAt)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CCameraProduction::Create", MB_OK);
        pInstance.reset();
        return nullptr;
    }

    return pInstance;
}

shared_ptr<CCameraProduction> CCameraProduction::Convert(const shared_ptr<CCameraProduction> _pOriginal, FXMMATRIX _ConvertMatrix)
{
    shared_ptr<CCameraProduction> pInstance = make_shared<CCameraProduction>(_pOriginal);

    if (FAILED(pInstance->InitConvert(_ConvertMatrix)))
    {
        MessageBox(nullptr, L"Convert Failed", L"CCameraProduction::Convert", MB_OK);
        pInstance.reset();
        return nullptr;
    }

    return pInstance;
}

HRESULT CCameraProduction::Initialize(float _fLerpTime, FXMVECTOR _vEye, FXMVECTOR _vAt)
{
    m_fLerpTime = _fLerpTime;
    XMStoreFloat3(&m_vEye, _vEye);
    XMStoreFloat3(&m_vAt, _vAt);

    return S_OK;
}

HRESULT CCameraProduction::InitConvert(FXMMATRIX _ConvertMatrix)
{
	XMStoreFloat3(&m_vEye, XMVector3TransformCoord(XMLoadFloat3(&m_vEye), _ConvertMatrix));
	XMStoreFloat3(&m_vAt, XMVector3TransformCoord(XMLoadFloat3(&m_vAt), _ConvertMatrix));

    return S_OK;
}

HRESULT CCameraProduction::SetFadeInfo(UINT _eFade, float _fFadeStartTime, float _fFadeDuration)
{
    m_eFadeFlag = (EFADE)_eFade;
    m_fFadeInOutTime = _fFadeStartTime;
    m_fFadeInOutDuration = _fFadeDuration;

    return S_OK;
}

HRESULT CCameraProduction::SetShakeInfo(float _fShakeStartTime, UINT _iShakeFlags, float _fDuration, ROT_DESC* _RotDesc, LOC_DESC* _LocDesc, FOV_DESC* _FovDesc)
{
    m_hasCameraShake = true;
    m_fShakeStartTime = _fShakeStartTime;
    m_iShakeFlags = _iShakeFlags;
    m_fShakeDuration = _fDuration;

	if (m_iShakeFlags & SHAKE_ROT && _RotDesc)
		memcpy_s(&m_RotDesc, sizeof(ROT_DESC), _RotDesc, sizeof(ROT_DESC));
	if (m_iShakeFlags & SHAKE_LOC && _LocDesc)
		memcpy_s(&m_LocDesc, sizeof(LOC_DESC), _LocDesc, sizeof(LOC_DESC));
	if (m_iShakeFlags & SHAKE_FOV && _FovDesc)
		memcpy_s(&m_FovDesc, sizeof(FOV_DESC), _FovDesc, sizeof(FOV_DESC));

    return S_OK;
}

HRESULT CCameraProduction::GetFadeInfo(UINT* _Out_FadeFlag, float* _Out_fFadeInOutTime, float* _Out_fFadeInOutDuration)
{
	if (_Out_FadeFlag)
		*_Out_FadeFlag = m_eFadeFlag;
	else
		return E_FAIL;

	if (_Out_fFadeInOutTime)
		*_Out_fFadeInOutTime = m_fFadeInOutTime;
	else
		return E_FAIL;

	if (_Out_fFadeInOutDuration)
		*_Out_fFadeInOutDuration = m_fFadeInOutDuration;
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraProduction::GetShakeInfo(UINT* _Out_ShakeFlags, float* _Out_fShakeDuration, ROT_DESC* _Out_RotDesc, LOC_DESC* _Out_LocDesc, FOV_DESC* _Out_FovDesc)
{
    if (_Out_ShakeFlags)
        *_Out_ShakeFlags = m_iShakeFlags;
    else
        return E_FAIL;

    if (_Out_fShakeDuration)
        *_Out_fShakeDuration = m_fShakeDuration;
    else
        return E_FAIL;

    if (m_iShakeFlags & SHAKE_ROT && _Out_RotDesc)
        memcpy_s(_Out_RotDesc, sizeof(ROT_DESC), &m_RotDesc, sizeof(ROT_DESC));
    if (m_iShakeFlags & SHAKE_LOC && _Out_LocDesc)
        memcpy_s(_Out_LocDesc, sizeof(LOC_DESC), &m_LocDesc, sizeof(LOC_DESC));
    if (m_iShakeFlags & SHAKE_FOV && _Out_FovDesc)
        memcpy_s(_Out_FovDesc, sizeof(FOV_DESC), &m_FovDesc, sizeof(FOV_DESC));

        return S_OK;
}
