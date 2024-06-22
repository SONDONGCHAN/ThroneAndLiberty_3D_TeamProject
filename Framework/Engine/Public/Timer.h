#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CTimer
{
public:
	CTimer();
	~CTimer() = default;

public:
	HRESULT Initialize(void);
	_float ComputeTimeDelta(void);

public:
	/*------------------------------
	_fOffset : 슬로우 모션 가중치
	_fDuration : 슬로우 모션이 진행될 시간
	--------------------------------*/
	void SetTimeOffset(_float _fOffset , _float _fDuration) { 
		m_fTimeOffset = _fOffset;
		m_fSlowDuration = _fDuration;
		m_fSlowAccTime = 0.f;
		m_IsSlowMotion = true;
	}

private:
	/*카메라 DeltaTime 가중치(SlowMotion이 필요할 때 사용)*/
	_float m_fTimeOffset = 1.f;
	_float m_fSlowDuration = 0.f;
	_float m_fSlowAccTime = 0.f;
	_bool m_IsSlowMotion = false;

private:
	LARGE_INTEGER m_FrameTime;
	LARGE_INTEGER m_FixTime;
	LARGE_INTEGER m_LastTime;
	LARGE_INTEGER m_CpuTick;

	_float m_fTimeDelta = 0.f;

public:
	static shared_ptr<CTimer> Create(void);

};

END
