#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CLevel;

class CLevelMgr final
{
public:
	CLevelMgr();
	~CLevelMgr() = default;

public:
	HRESULT Initialize();
	void Tick(_float _fTimeDelta);

	/*디버깅 or 확인용 폰트 출력용*/
	HRESULT Render();
	
	/*Funtion.로딩 레벨에 레벨 전환 신호를 보내고 준비하기 위함*/
	HRESULT OpenLevel(_uint _iLevelIndex, shared_ptr<CLevel> _pLevel);
	HRESULT OpenLevelForClient(_uint _iLevelIndex);
	/*Funtion.로딩을 마친 후 실질적인 레벨 교환*/
	HRESULT OpenLevel2(_uint _iLevelIndex, shared_ptr<CLevel> _pLevel);
	HRESULT StartLevel(shared_ptr<class CLevel> _pNextLevel);

public:
	_uint GetCurrentLevel() { return m_iLevelIndex; }
	void SetLoadingLevel(shared_ptr<class CLevel> _pLoadingLevel) { m_pLoadingLevel = _pLoadingLevel; }
	bool isLoadingLevel() { return m_IsLoadingLevel; }

public:
	void LevelBGMOn();
	void LevelBGMOff();
	
	void SwitchingBGM();
	void SwitchingBGM2();
	void SwitchingBGM3();

private:
	_uint m_iLevelIndex = { 0 };
	shared_ptr<class CLevel> m_pCurrentLevel = { nullptr };

private:
	/*로딩 레벨을 지우지 않고 재사용합니다*/
	shared_ptr<class CLevel> m_pLoadingLevel = { nullptr };

private:
	_bool m_IsLoadingLevel = false;

public:
	static shared_ptr<CLevelMgr> Create();

};

END