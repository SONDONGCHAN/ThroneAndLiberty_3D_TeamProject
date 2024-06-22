#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CLevel
{

public:
	CLevel();
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float _fTimeDelta);
	virtual void LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	/*엔진에서 Loading Level을 보관하고 재사용하기 위함*/
	/*일반 레벨에서는 사용하지 않고 Loading Level만 재정의해서 사용합니다*/
	virtual HRESULT ReadyNextLevel(_uint _iNextLevel) { return S_OK; }

public:
	void LevelBGMOn();
	void LevelBGMOff();
	
	virtual void SwitchingBGM() {};
	virtual void SwitchingBGM2() {};
	virtual void SwitchingBGM3() {};
	virtual void SwitchingBossBGM() {};
	virtual void SwitchingMiniGameBGM() {};


protected:
	HRESULT ReadyLevelLight(wstring _strPath);

protected:
	void ReadyLevelBGM() {};

	unordered_map<string, _float> m_AmbientSounds = {};

public:
	void Free();

};

END