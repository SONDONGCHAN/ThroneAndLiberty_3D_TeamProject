#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CCameraProduction final : public std::enable_shared_from_this<CCameraProduction>
{
public:
	CCameraProduction();
	CCameraProduction(const shared_ptr<CCameraProduction> _pOriginal);
	~CCameraProduction() = default;

public:
	static shared_ptr<CCameraProduction> Create(float _fLerpTime, FXMVECTOR _vEye, FXMVECTOR _vAt);
	static shared_ptr<CCameraProduction> Convert(const shared_ptr<CCameraProduction> _pOriginal, FXMMATRIX _ConvertMatrix);

public:
	HRESULT Initialize(float _fLerpTime, FXMVECTOR _vEye, FXMVECTOR _vAt);
	HRESULT InitConvert(FXMMATRIX _ConvertMatrix);
	HRESULT SetFadeInfo(UINT _eFade, float _fFadeStartTime, float _fFadeDuration);
	HRESULT SetShakeInfo(float _fShakeStartTime, UINT _iShakeFlags, float _fDuration, ROT_DESC* _RotDesc, LOC_DESC* _LocDesc, FOV_DESC* _FovDesc);

public:
	float		GetLerpTime() { return m_fLerpTime; }
	XMVECTOR	GetEye() { return XMLoadFloat3(&m_vEye); }
	XMVECTOR	GetAt() { return XMLoadFloat3(&m_vAt); }
	
	EFADE		GetFade() { return m_eFadeFlag; }
	HRESULT		GetFadeInfo(UINT* _Out_FadeFlag, float* _Out_fFadeInOutTime, float* _Out_fFadeInOutDuration);

	bool		hasCamShake() { return m_hasCameraShake; }
	float		GetShakeStartTime() { return m_fShakeStartTime; }
	HRESULT		GetShakeInfo(UINT* _Out_ShakeFlags, float* _Out_fShakeDuration, ROT_DESC* _Out_RotDesc, LOC_DESC* _Out_LocDesc, FOV_DESC* _Out_FovDesc);

private:
	float		m_fLerpTime{ 0.0f };

	XMFLOAT3	m_vEye{};
	XMFLOAT3	m_vAt{};

	EFADE		m_eFadeFlag{ FADE_NONE };
	float		m_fFadeInOutTime{ 0.0f };
	float		m_fFadeInOutDuration{ 0.0f };

	bool		m_hasCameraShake{ false };
	float		m_fShakeStartTime{ 0.0f };

	UINT		m_iShakeFlags{ SHAKE_NONE };
	float		m_fShakeDuration{ 0.0f };
	ROT_DESC	m_RotDesc{};
	LOC_DESC	m_LocDesc{};
	FOV_DESC	m_FovDesc{};
};

END