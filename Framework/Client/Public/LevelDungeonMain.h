#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevelDungeonMain : public CLevel
{
public:
	CLevelDungeonMain();
	virtual ~CLevelDungeonMain();

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float _fTimeDelta);
	virtual void LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

protected:
	virtual void ReadyLevelBGM();

private:
	void KeyInput();

private:
	HRESULT ReadyLight();
	HRESULT ReadyCamera(const wstring& _strLayerTag);
	HRESULT ReadyLayerBackGround(const wstring& _strLayerTag);
	HRESULT ReadyLayerMap(const wstring& _strLayerTag);
	HRESULT ReadyLayerPlayer(const wstring& _strLayerTag);
	HRESULT ReadyMonster(const wstring& _strLayerTag);
	HRESULT ReadyInteractObj(const wstring& _strLayerTag);
	HRESULT ReadyCollider();

	HRESULT ReadyPracticeRoom(const wstring& _strLayerTag);

	HRESULT SetLevelEvent();
	HRESULT TestAmitoy();
	HRESULT TestMNGObject();

	HRESULT TestTrapController();

public:
	virtual void SwitchingBGM() override;
	virtual void SwitchingMiniGameBGM() override;

private:
	unordered_map<string, _float> m_DungeonMainSounds;
	unordered_map<string, _float> m_BossSounds;
	unordered_map<string, _float> m_MiniGameSounds;

	_bool m_IsBoss = false;
	_bool m_IsMiniGame	= false;


public:
	static shared_ptr<CLevelDungeonMain> Create();
};

END