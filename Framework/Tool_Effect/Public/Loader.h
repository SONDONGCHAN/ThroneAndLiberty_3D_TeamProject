#pragma once

#include "Tool_Defines.h"

BEGIN(Engine)
class CLevel;
END


BEGIN(Tool_Effect)

class CLoader final : public enable_shared_from_this<CLoader>
{
public:
	CLoader();
	~CLoader();

public:
	HRESULT Initialize();
	/*다음 레벨을 생성하고 Initialize까지 마칩니다*/
	HRESULT LoadingSelector(LEVEL _eLevel);
	_bool isFinished() const { return m_IsFinished; }


public:
	void StartLoading(LEVEL _eLevel);
	void StartLevel();

private:

	HRESULT LoadingForLogo();

private:
	shared_ptr<CLevel> m_pNextLevel = nullptr;

private:
	thread m_pThread;
	CRITICAL_SECTION m_CriticalSection = {};
	LEVEL m_eNextLevel = { LEVEL_END };
	_bool m_IsFinished = false;

private:
	wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

public:
	static shared_ptr<CLoader> Create();

};

END