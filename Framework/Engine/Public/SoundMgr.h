#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CSoundMgr
{
private:
	struct FadeSound_Desc
	{
		_int	iChannelID;
		_float	fTargetVolume;
		_float	fCurrentVolume;
		_float	fTickPerVolume;
	};

public:
	CSoundMgr();
	virtual ~CSoundMgr();


public:
	HRESULT Initialize();
	void Tick(_float _fTimeDelta);


public:
	void PlaySoundW(const string _strSoundKey, float _fVolume);
	void PlaySoundL(const string _strSoundKey,  float _fVolume);
	void PlaySoundNonOverLap(const string _strSoundKey, float _fVolume);
	void PlayBGMFadeIn(const string _strSoundKey, _float _fTargetVolume, _float _fDurTime); // Fade In BGM
	void PlayBGM(string _strSoundKey, float _fVolume);
	void StopSound(const string _strSoundKey);
	void StopSoundFadeOut(const string _strSoundKey,_float _fDurTime); // Fade Out Stop Sound
	void StopAll();
	void SetChannelVolume(string _strSoundKey, float fVolume);


private:
	_int FindEmptySoundSlot();
	void UpdateFadeIn(_float _fTimeDelta);
	void UpdateFadeOut(_float _fTimeDelta);
	_bool FindIsPlayingSound(const string _strSoundKey);

private:
	void	StoreAllSoundPath(const filesystem::path& CurrentPath, const string& FileExtension);
	void	FindAllSoundPath(const string& _strBasePath, const string& _FileExtension);

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<string, FMOD::Sound*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD::Channel* m_pChannelArr[FMOD_MAX_CHANNEL_WIDTH];

	string m_pCurrentPlaykey[FMOD_MAX_CHANNEL_WIDTH];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD::System* m_pSystem;
	void* m_pExtraDriverData{ nullptr };

private:
	map<string, shared_ptr<FadeSound_Desc>> m_mapFadeInChannels;
	map<string, shared_ptr<FadeSound_Desc>> m_mapFadeOutChannels;


public:
	static shared_ptr<CSoundMgr> Create();
	void Free();

};

END