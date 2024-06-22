#include "SoundMgr.h"

CSoundMgr::CSoundMgr()
{
}

CSoundMgr::~CSoundMgr()
{
}

HRESULT CSoundMgr::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	if (FMOD_OK != FMOD::System_Create(&m_pSystem))
		return E_FAIL;

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	if (FMOD_OK != m_pSystem->init(FMOD_MAX_CHANNEL_WIDTH, FMOD_INIT_NORMAL, m_pExtraDriverData))
		return E_FAIL;

	FindAllSoundPath("../../Client/Bin/Sounds/", ".wav");

	ZeroMemory(m_pChannelArr, sizeof(FMOD::Channel) * FMOD_MAX_CHANNEL_WIDTH);

	return S_OK;
}

void CSoundMgr::Tick(_float _fTimeDelta)
{
	m_pSystem->update();

	_bool isPlaying = false;
	
	for (UINT i = 0; i < MAXCHANNEL; i++)
	{
		if (nullptr != m_pChannelArr[i])
		{
			m_pChannelArr[i]->isPlaying(&isPlaying);

			if (false == isPlaying)
			{
				m_pChannelArr[i]->stop();
				m_pCurrentPlaykey[i] = "";
				m_pChannelArr[i] = nullptr;
			}
		}
	}

	if(m_mapFadeInChannels.size() != 0)
		UpdateFadeIn(_fTimeDelta);

	if (m_mapFadeOutChannels.size() != 0)
		UpdateFadeOut(_fTimeDelta);
}

void CSoundMgr::PlaySoundW(const string _strSoundKey, float _fVolume)
{
	auto iter = m_mapSound.find(_strSoundKey);

	if (iter == m_mapSound.end())
		return;

	_int iChannelID =  FindEmptySoundSlot();

	if (-1 != iChannelID) {

		m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[iChannelID]);
		m_pChannelArr[iChannelID]->setMode(FMOD_LOOP_OFF);
		m_pChannelArr[iChannelID]->setVolume(_fVolume);
		m_pCurrentPlaykey[iChannelID] = _strSoundKey;
	}
}

void CSoundMgr::PlaySoundL(const string _strSoundKey, float _fVolume)
{
	auto iter = m_mapSound.find(_strSoundKey);

	if (iter == m_mapSound.end())
		return;

	_int iChannelID = FindEmptySoundSlot();

	if (-1 != iChannelID) {

		m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[iChannelID]);
		m_pChannelArr[iChannelID]->setMode(FMOD_LOOP_NORMAL);
		m_pChannelArr[iChannelID]->setVolume(_fVolume);
		m_pCurrentPlaykey[iChannelID] = _strSoundKey;
	}

}

void CSoundMgr::PlaySoundNonOverLap(const string _strSoundKey, float _fVolume)
{
	if (FindIsPlayingSound(_strSoundKey))
		return;

	PlaySoundW(_strSoundKey, _fVolume);
}

void CSoundMgr::PlayBGMFadeIn(const string _strSoundKey, _float _fTargetVolume, _float _fDurTime)
{
	 auto iter = m_mapSound.find(_strSoundKey);
	 
	 if (iter == m_mapSound.end())
	 	return;
	 
	 _int iChannelID = FindEmptySoundSlot();
	 
	 if (-1 != iChannelID) {
	 
	 	m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[iChannelID]);
		m_pChannelArr[iChannelID]->setMode(FMOD_LOOP_NORMAL);
	 	m_pChannelArr[iChannelID]->setVolume(0.f);
	 	m_pCurrentPlaykey[iChannelID] = _strSoundKey;

		shared_ptr<FadeSound_Desc> FadeSound_Desc = make_shared<CSoundMgr::FadeSound_Desc>();

		FadeSound_Desc->iChannelID = iChannelID;
		FadeSound_Desc->fTargetVolume = _fTargetVolume;
		FadeSound_Desc->fCurrentVolume = 0.f;
		FadeSound_Desc->fTickPerVolume = (_fTargetVolume/_fDurTime);

		m_mapFadeInChannels.emplace(_strSoundKey, FadeSound_Desc);

		string	strFadeOutSoundKey = { "" };
		_int	iChannel = -1;
		for (auto& pFadeOutChannel : m_mapFadeOutChannels)
		{
			if (pFadeOutChannel.first == _strSoundKey)
			{
				strFadeOutSoundKey = pFadeOutChannel.first;
				iChannel = pFadeOutChannel.second->iChannelID;
				break;
			}
		}
		
		if ( (strFadeOutSoundKey != "") && (iChannel != (-1)) )
		{
			m_pChannelArr[iChannel]->stop();
			m_mapFadeOutChannels.erase(strFadeOutSoundKey);
		}

	 }
}

void CSoundMgr::PlayBGM(string _strSoundKey, float _fVolume)
{
	auto iter = m_mapSound.find(_strSoundKey);

	if (iter == m_mapSound.end())
		return;

	_int iChannelID = FindEmptySoundSlot();

	if (-1 != iChannelID) {

		m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[iChannelID]);
		m_pChannelArr[iChannelID]->setMode(FMOD_LOOP_NORMAL);
		m_pChannelArr[iChannelID]->setVolume(_fVolume);
		m_pCurrentPlaykey[iChannelID] = _strSoundKey;
	}

}

void CSoundMgr::StopSound(const string _strSoundKey)
{
	for(_int i = 0 ; i < MAXCHANNEL; ++i)
	{
		if (_strSoundKey == m_pCurrentPlaykey[i]) {
			m_pChannelArr[i]->stop();
			break;
		}
	}
}

void CSoundMgr::StopSoundFadeOut(const string _strSoundKey, _float _fDurTime)
{
	for (_int i = 0; i < MAXCHANNEL; ++i)
	{
		if (_strSoundKey == m_pCurrentPlaykey[i]) {

			_float fVolume;
			m_pChannelArr[i]->getVolume(&fVolume);

			shared_ptr<FadeSound_Desc> FadeSound_Desc = make_shared<CSoundMgr::FadeSound_Desc>();
			
			FadeSound_Desc->iChannelID = i;
			FadeSound_Desc->fTargetVolume = 0;
			FadeSound_Desc->fCurrentVolume = fVolume;
			FadeSound_Desc->fTickPerVolume = (fVolume /_fDurTime);
			
			m_mapFadeOutChannels.emplace(_strSoundKey, FadeSound_Desc);

			string strFadeInSoundKey = { "" };

			for (auto& pFadeInChannel : m_mapFadeInChannels)
			{
				if (pFadeInChannel.first == _strSoundKey)
				{
					strFadeInSoundKey = pFadeInChannel.first;
					break;
				}
			}

			if (strFadeInSoundKey != "")
				m_mapFadeInChannels.erase(strFadeInSoundKey);
			
			break;
		}
	}
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		m_pChannelArr[i]->stop();
}

void CSoundMgr::SetChannelVolume(string _strSoundKey, float fVolume)
{
	for (_int i = 0; i < MAXCHANNEL; ++i)
	{
		if (_strSoundKey == m_pCurrentPlaykey[i]) {
			m_pChannelArr[i]->setVolume(fVolume);
			break;
		}
	}

}

_int CSoundMgr::FindEmptySoundSlot()
{
	/*If All Channel Full, Return -1*/
	_int iChannelNum = -1;
	_bool isPlaying = false;

	for (_int i = 0; i < MAXCHANNEL; ++i) {

		m_pChannelArr[i]->isPlaying(&isPlaying);

		if (!isPlaying) {
			iChannelNum = i;
			break;
		}
	}
	
	return iChannelNum;
}

void CSoundMgr::UpdateFadeIn(_float _fTimeDelta)
{
	vector<string> vecFadeFinished;

	for (auto& pFadeInChannel : m_mapFadeInChannels)
	{
		pFadeInChannel.second->fCurrentVolume +=  ((1.f/60.f) * (pFadeInChannel.second->fTickPerVolume));

		if (pFadeInChannel.second->fCurrentVolume >= pFadeInChannel.second->fTargetVolume)
		{
			pFadeInChannel.second->fCurrentVolume = pFadeInChannel.second->fTargetVolume;
			m_pChannelArr[pFadeInChannel.second->iChannelID]->setVolume(pFadeInChannel.second->fCurrentVolume);

			vecFadeFinished.push_back(pFadeInChannel.first);
			continue;
		}

		m_pChannelArr[pFadeInChannel.second->iChannelID]->setVolume(pFadeInChannel.second->fCurrentVolume);
	}

	if (vecFadeFinished.size() != 0)
	{
		for (auto pFadeFinished : vecFadeFinished)
		{
			m_mapFadeInChannels.erase(pFadeFinished);
		}
	}
}

void CSoundMgr::UpdateFadeOut(_float _fTimeDelta)
{
	vector<pair<string,_int>> vecFadeFinished;
	
	for (auto& pFadeOutChannel : m_mapFadeOutChannels)
	{
		pFadeOutChannel.second->fCurrentVolume -= ((1.f / 60.f) * (pFadeOutChannel.second->fTickPerVolume));
	
		if (pFadeOutChannel.second->fCurrentVolume <= pFadeOutChannel.second->fTargetVolume)
		{
			pair<string, _int> FadeFinishedData;
			FadeFinishedData.first = pFadeOutChannel.first;
			FadeFinishedData.second = pFadeOutChannel.second->iChannelID;
			vecFadeFinished.push_back(FadeFinishedData);
			continue;
		}
	
		m_pChannelArr[pFadeOutChannel.second->iChannelID]->setVolume(pFadeOutChannel.second->fCurrentVolume);
	}
	
	if (vecFadeFinished.size() != 0)
	{
		for (auto pFadeFinished : vecFadeFinished)
		{
			m_pChannelArr[pFadeFinished.second]->stop();
			m_mapFadeOutChannels.erase(pFadeFinished.first);
		}
	}
}

_bool CSoundMgr::FindIsPlayingSound(const string _strSoundKey)
{
	_bool	bIsPlaying = false;

	for (_int i = 0; i < MAXCHANNEL; ++i)
	{
		if (_strSoundKey == m_pCurrentPlaykey[i]) 
		{
			bIsPlaying = true;
			break;
		}
	}

	return bIsPlaying;
}

void CSoundMgr::StoreAllSoundPath(const filesystem::path& CurrentPath, const string& FileExtension)
{
	for (const auto& entry : filesystem::directory_iterator(CurrentPath))
	{
		if (filesystem::is_regular_file(entry) && entry.path().extension() == FileExtension)
		{
			FMOD::Sound* pSound = nullptr;

			string strPath = entry.path().stem().string();

			FMOD_RESULT eRes = m_pSystem->createSound(entry.path().string().c_str(), FMOD_LOOP_OFF, 0, &pSound);

			if (eRes == FMOD_OK)
			{
				m_mapSound.emplace(strPath, pSound);
			}
		}

		else if (filesystem::is_directory(entry))
		{
			StoreAllSoundPath(entry.path(), FileExtension);
		}
	}

}

void CSoundMgr::FindAllSoundPath(const string& _strBasePath, const string& _FileExtension)
{
	filesystem::path fullPath = _strBasePath;
	StoreAllSoundPath(fullPath, _FileExtension);
}

shared_ptr<CSoundMgr> CSoundMgr::Create()
{
	shared_ptr<CSoundMgr> pSoundMgr = make_shared<CSoundMgr>();

	if (FAILED(pSoundMgr->Initialize()))
		MSG_BOX("Failed to Create : CSoundMgr");

	return pSoundMgr;
}

void CSoundMgr::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		Mypair.second->release();
	}

	m_mapSound.clear();
	m_pSystem->release();
	m_pSystem->close();

}
