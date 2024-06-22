#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevelDungeonFinal : public CLevel
{
public:
	CLevelDungeonFinal();
	virtual ~CLevelDungeonFinal();

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float _fTimeDelta);
	virtual void LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

protected:
	virtual void ReadyLevelBGM();
	
private:
	HRESULT ReadyLight();
	HRESULT ReadyLayerMap(const wstring& strLayerTag);
	HRESULT ReadyMonster(const wstring& _strLayerTag);
	HRESULT ReadyEnvEffect(const wstring& _strLayerTag);
	HRESULT ReadyCollider();
	HRESULT ReadyCamera();
	HRESULT ReadyLayerPlayer(const wstring& _strLayerTag);
	
	HRESULT SetLevelEvent();

public:
	virtual void SwitchingBGM() override;			// 1Æä
	virtual void SwitchingBGM2() override;		// 2Æä
	virtual void SwitchingBGM3() override;

private:
	unordered_map<string, _float> m_EntranceSounds;
	unordered_map<string, _float> m_Phase1Sounds;
	unordered_map<string, _float> m_Phase2Sounds;

	_bool m_bPhase1 = false;
	_bool m_bPhase2 = false;

public:
	static shared_ptr<CLevelDungeonFinal> Create();

};

END
