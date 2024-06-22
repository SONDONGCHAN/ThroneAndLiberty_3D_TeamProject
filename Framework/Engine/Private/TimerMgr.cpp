#include "TimerMgr.h"
#include "Timer.h"

CTimerMgr::CTimerMgr()
{
}

_float CTimerMgr::ComputeTimeDelta(const wstring& _strTimerTag)
{
    shared_ptr<CTimer> pTimer = FindTimer(_strTimerTag);

    if (!pTimer)
        return 0.0f;

    return pTimer->ComputeTimeDelta();
}

void CTimerMgr::SetTimerOffset(const wstring& _strTimerTag, _float _fOffset, _float _fDuration)
{
    shared_ptr<CTimer> pTimer = FindTimer(_strTimerTag);

    if (!pTimer)
        return;

    pTimer->SetTimeOffset(_fOffset, _fDuration);
}

HRESULT CTimerMgr::AddTimer(const wstring& _strTimerTag)
{
    shared_ptr<CTimer> pTimer = FindTimer(_strTimerTag);

    if (pTimer)
        return E_FAIL;

    pTimer = CTimer::Create();

    if (!pTimer)
        return E_FAIL;

    m_Timers.insert({ _strTimerTag , pTimer });

    return S_OK;
}

shared_ptr<class CTimer> CTimerMgr::FindTimer(const wstring& _strTimerTag)
{
    auto iter = m_Timers.find(_strTimerTag);

    if (m_Timers.end() == iter)
        return nullptr;

    return iter->second;
}

shared_ptr<CTimerMgr> CTimerMgr::Create()
{
    return make_shared<CTimerMgr>();
}
