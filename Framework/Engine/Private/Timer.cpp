#include "Timer.h"

CTimer::CTimer()
{
}

HRESULT CTimer::Initialize(void)
{
	QueryPerformanceCounter(&m_FrameTime);
	QueryPerformanceCounter(&m_FixTime);
	QueryPerformanceCounter(&m_LastTime);
	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

_float CTimer::ComputeTimeDelta(void)
{
	QueryPerformanceCounter(&m_FrameTime);

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_FixTime = m_FrameTime;
	}

	m_fTimeDelta = (float(m_FrameTime.QuadPart - m_LastTime.QuadPart) / m_CpuTick.QuadPart);
	m_LastTime = m_FrameTime;

	if(m_IsSlowMotion){

		m_fSlowAccTime += m_fTimeDelta;

		/*세팅해둔 TimeOffset을 곱해 기존 DeltaTime에 가중치 적용*/
		m_fTimeDelta *= m_fTimeOffset;

		if (m_fSlowAccTime >= m_fSlowDuration) {
			m_fSlowAccTime = 0.f;
			m_fSlowDuration = 0.f;
			m_fTimeOffset = 1.f;
			m_IsSlowMotion = false;
		}
	}

	return m_fTimeDelta;
}

shared_ptr<CTimer> CTimer::Create(void)
{
	shared_ptr<CTimer> pInstance = make_shared<CTimer>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CTimer");

	return pInstance;
}
