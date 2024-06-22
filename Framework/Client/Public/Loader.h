#pragma once

#include "Client_Defines.h"

BEGIN(Engine)
class CLevel;
END


BEGIN(Client)

class CLoader final : public enable_shared_from_this<CLoader>
{
public:
	CLoader();
	~CLoader();

public:
	HRESULT Initialize(LEVEL _eNextLevel);
	/*다음 레벨을 생성하고 Initialize까지 마칩니다*/
	HRESULT LoadingSelector();
	_bool isFinished() const { return m_IsFinished; }


public:
	HRESULT LevelInitialize();

public:
	_bool ThreadJoin();

public:
	void StartLoading(LEVEL _eLevel);
	void StartLevel();

private:
	
	HRESULT LoadingForLogo();
	HRESULT LoadingForVillage();
	HRESULT LoadingForFonos();
	HRESULT LoadingForDungeonMain();
	HRESULT LoadingForDungeonFinal();

private:
	shared_ptr<CLevel> m_pNextLevel = nullptr;

private:
	thread m_pThread;

	HANDLE m_hThread = { 0 };
	CRITICAL_SECTION m_CriticalSection = {};
	LEVEL m_eNextLevel = { LEVEL_END };
	_bool m_IsFinished = false;

private:
	wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

private:
	_bool m_IsStaticLoadDone = false;

public:
	//static shared_ptr<CLoader> Create(LEVEL _eNextLevel);
	static CLoader* Create(LEVEL _eNextLevel);
	void Free();

};

END