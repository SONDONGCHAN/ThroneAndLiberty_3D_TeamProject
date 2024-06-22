#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CTimerMgr final
{
public:
	CTimerMgr();
	~CTimerMgr() = default;

public:
	_float ComputeTimeDelta(const wstring& _strTimerTag);

	/*특정 Timer의 Delta Time 가중치 설정*/
	void SetTimerOffset(const wstring& _strTimerTag, _float _fOffset , _float _fDuration);

public:
	HRESULT AddTimer(const wstring& _strTimerTag);

private:
	shared_ptr<class CTimer> FindTimer(const wstring& _strTimerTag);

private:
	map<const wstring, shared_ptr<class CTimer>> m_Timers;

public:
	static shared_ptr<CTimerMgr> Create();

};

END