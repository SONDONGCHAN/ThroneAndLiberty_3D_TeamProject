#pragma once

#include "Tool_CC_Define.h"
#include "Camera_Direction.h"

class CCameraKeyFrame final : public CGameObject
{
public:
	CCameraKeyFrame();
	~CCameraKeyFrame();

public:
	static shared_ptr<CCameraKeyFrame> Create(_float3 _vPosition, _float3 _vLook);

public:
	HRESULT Initialize(_float3 _vPosition, _float3 _vLook);
	virtual void PriorityTick(float _fTimeDelta);
	virtual void Tick(float _fTimeDelta);
	virtual void LateTick(float _fTimeDelta);
	virtual HRESULT Render();

public:
	shared_ptr<CTransform> GetTransformAt() { return m_pTransform_At; }
	float GetLerpTime() { return m_fLerpTime; }

	int	  GetFade() { return m_eFade; }
	float GetFadeInOutTime() { return m_fFadeInOutStartTime; }
	float GetFadeInOutDurtaion() { return m_fFadeInOutDuration; }
	bool  GetFadeInfo(UINT* _eFade, float* _fInOutTime, float* _fInOutDurtaion);

	bool  GetisCameraShake() { return m_isCameraShake; }
	UINT  GetShakeFlag() { return m_iShakeFlags; }
	float GetShakeTime() { return m_fShakeStartTime; }
	bool  GetShakeInfo(UINT* _iShakeFlags, float* _fStartTime, float* _fDuration, ROT_DESC* _OutRotDesc, LOC_DESC* _OutLocDesc, FOV_DESC* _OutFovDesc);

public:
	void SetLerpTime(float _fLerpTime) { m_fLerpTime = _fLerpTime; if (m_fFadeInOutStartTime > m_fLerpTime) { m_fFadeInOutStartTime = m_fLerpTime; } }
	void SetFade(int _eFade) { m_eFade = (EFADE)_eFade; }
	void SetFadeInOutStartTime(float _fStartTime) { m_fFadeInOutStartTime = _fStartTime; }
	void SetFadeInOutDuration(float _fDuration) { m_fFadeInOutDuration = _fDuration; }
	void SetCameraShake(float _fShakeStartTime, UINT _eShakeFlags, float _fDuration, ROT_DESC* _pRotDesc = nullptr, LOC_DESC* _pLocDesc = nullptr, FOV_DESC* _pFovDesc = nullptr);
	void SetShakeTime(float _fTime) { m_fShakeStartTime = _fTime; }

	void ResetFade();
	void ResetShake();

private:
	_float3 m_vPrePos;
	_float3 m_vPreAtPos;

	shared_ptr<CTransform> m_pTransform_At = nullptr;

	float m_fLerpTime = 1.0f;

	EFADE m_eFade{ FADE_NONE };		// 이건 나중에 페이드 인아웃 관리하는 매니저에 던질것
	float m_fFadeInOutStartTime{ 0.0f };
	float m_fFadeInOutDuration{ 0.0f };

	bool m_isCameraShake{ false };
	float m_fShakeStartTime{ 0.0f };

	/* Shake 변수 */
	UINT	m_iShakeFlags{ SHAKE_NONE };
	float	m_fShakeDuration = 0.0f;
	ROT_DESC m_RotDesc{};
	LOC_DESC m_LocDesc{};
	FOV_DESC m_FovDesc{};
};

