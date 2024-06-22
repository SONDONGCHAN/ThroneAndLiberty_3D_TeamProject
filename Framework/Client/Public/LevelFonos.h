#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)


class CLevelFonos : public CLevel
{
public:
	CLevelFonos();
	virtual ~CLevelFonos() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float _fTimeDelta);
	virtual void LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT ReadyLight();
	HRESULT ReadyCamera();
	HRESULT ReadyLayerMap(const wstring& strLayerTag);
	HRESULT ReadyLayerPlayer(const wstring& _strLayerTag);
	HRESULT ReadyCollider();
	HRESULT ReadyMiniGame();
	HRESULT ReadyEnvironmentObj(const wstring& _strLayerTag);

	HRESULT SetLevelEvent();

protected:
	virtual void ReadyLayerBGM();

public:
	virtual void SwitchingBGM() override;
	virtual void SwitchingMiniGameBGM() override;

private:
	unordered_map<string, _float> m_FonosSounds;
	unordered_map<string, _float> m_BossSounds;
	unordered_map<string, _float> m_MiniGameSounds;

	_bool m_IsBoss		= false;
	_bool m_IsMiniGame	= false;

private:
	HRESULT TestAmitoy();

public:
	static shared_ptr<CLevelFonos> Create();

};

END