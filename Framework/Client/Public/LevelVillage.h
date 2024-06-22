#pragma once

#include "Client_Defines.h"
#include "Level.h"


BEGIN(Client)

class CLevelVillage : public CLevel
{
public:
	CLevelVillage();
	virtual ~CLevelVillage();

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float _fTimeDelta);
	virtual void LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

protected:
	virtual void ReadyLevelBGM();

private:
	HRESULT ReadyLight();
	HRESULT ReadyCamera(const wstring& strLayerTag);
	HRESULT ReadyLayerMap(const wstring& strLayerTag);
	HRESULT ReadyLayerPlayer(const wstring& _strLayerTag);
	HRESULT ReadyMonsterNPC(const wstring& _strLayerTag);
	HRESULT ReadyInteractObj(const wstring& _strLayerTag);
	HRESULT ReadyCollider();
	HRESULT ReadyLayerCatapult(const wstring& _strLayerTag);
	HRESULT ReadyEnvironmentObj(const wstring& _strLayerTag);

	HRESULT ReadyPracticeRoom(const wstring& _strLayerTag);

	HRESULT SetLevelEvent();
	
	HRESULT ReadyVillageMiniGame(const wstring& _strLayerTag);

public:
	virtual void SwitchingBGM() override;
	virtual void SwitchingBGM2() override;



private:
	unordered_map<string, _float> m_VillageSounds;
	unordered_map<string, _float> m_FieldSounds;
	unordered_map<string, _float> m_BossSounds;


	_bool m_IsVillage = false;
	_bool m_IsBoss = false;


public:
	static shared_ptr<CLevelVillage> Create();

};

END