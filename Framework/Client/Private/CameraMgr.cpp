
#include "CameraMgr.h"

#include "CameraFree.h"
#include "CameraSpherical.h"
#include "CameraFilm.h"
#include "UIMgr.h"

IMPLEMENT_SINGLETON(CCameraMgr)

CCameraMgr::CCameraMgr()
{
}

CCameraMgr::~CCameraMgr()
{
}

void CCameraMgr::AddCamera(const string& _strCameraTag, shared_ptr<CCamera> _pCamera)
{
	shared_ptr<CCamera> pCamera = FindCamera(_strCameraTag);
	if (!pCamera && _pCamera)
	{
		m_Cameras.insert({ _strCameraTag, _pCamera });
	}
}

void CCameraMgr::AddFilmCamera(const string& _strCameraTag, shared_ptr<CCameraFilm> _pCamera)
{
	shared_ptr<CCameraFilm> pFilmCamera = FindFilmCamera(_strCameraTag);
	if (!pFilmCamera && _pCamera)
	{
		m_FilmCameras.insert({ _strCameraTag, _pCamera });
	}
}

void CCameraMgr::CameraTurnOn(const string& _strCameraTag)
{
	shared_ptr<CCamera> pCamera = FindCamera(_strCameraTag);
	if (pCamera)
	{
		if (m_pRecordingCamera.lock())
		{
			m_pRecordingCamera.lock()->CameraTurnOff();
			pCamera->SetMouseLock(m_pRecordingCamera.lock()->GetMouseLock());
		}
		pCamera->CameraTurnOn();
		m_pRecordingCamera = pCamera;
	}
}

void CCameraMgr::CameraTurnOn(const string& _strCameraTag, const _float3& _vEye, const _float3& _vAt)
{
	shared_ptr<CCamera> pCamera = FindCamera(_strCameraTag);
	if (pCamera)
	{
		if (m_pRecordingCamera.lock())
		{
			m_pRecordingCamera.lock()->CameraTurnOff();
		}
		pCamera->CameraTurnOn();
		pCamera->SetEyeAt(_vEye, _vAt);
		m_pRecordingCamera = pCamera;
	}
}

void CCameraMgr::CameraTurnOn(const string& _strCameraTag, float _fTime, float _fWeight)
{
	shared_ptr<CCamera> pCamera = FindCamera(_strCameraTag);
	if (pCamera)
	{
		if (m_pRecordingCamera.lock())
		{
			pCamera->LerpEyeAt(_fTime, _fWeight, m_pRecordingCamera.lock()->GetEye(), m_pRecordingCamera.lock()->GetAt(), pCamera->GetEye(), pCamera->GetAt());
			m_pRecordingCamera.lock()->CameraTurnOff();
		}
		pCamera->CameraTurnOn();
		m_pRecordingCamera = pCamera;
	}
}

void CCameraMgr::CameraTurnOn(const string& _strCameraTag, const _float3& _vEye, const _float3& _vAt, float _fTime, float _fWeight)
{
	shared_ptr<CCamera> pCamera = FindCamera(_strCameraTag);
	if (pCamera)
	{
		if (m_pRecordingCamera.lock())
		{
			pCamera->LerpEyeAt(_fTime, _fWeight, m_pRecordingCamera.lock()->GetEye(), m_pRecordingCamera.lock()->GetAt(), _vEye, _vAt);
			m_pRecordingCamera.lock()->CameraTurnOff();
		}
		pCamera->CameraTurnOn();
		m_pRecordingCamera = pCamera;
	}
}

void CCameraMgr::FilmCameraTurnOn(const string& _strCameraTag, bool _isHUDActive)
{
	shared_ptr<CCameraFilm> pFilmCamera = FindFilmCamera(_strCameraTag);
	if (pFilmCamera)
	{
		if (m_pRecordingCamera.lock())
		{
			m_pRecordingCamera.lock()->CameraTurnOff();
		}
		if (m_pRecordingFilmCamera.lock())
		{
			m_pRecordingFilmCamera.lock()->ResetCamera();
		}
		m_pRecordingFilmCamera = pFilmCamera;
		pFilmCamera->StartProduction();
		UIMGR->DeactivateHUD();
		UIMGR->SetIsCameraDirecting(true);
		m_isHUDActive = _isHUDActive;
	}
}

void CCameraMgr::FilmCameraTurnOn(const vector<string>& _vecCameraTags, bool _isHUDActive)
{
	if ((UINT)_vecCameraTags.size() > 0)
	{
		m_vecFilmCameraTags.clear();
		m_vecFilmCameraTags = move(_vecCameraTags);
		memset(&m_iCurFilmIndex, 0, sizeof(int));
		m_iFilmCnt = (int)m_vecFilmCameraTags.size();
		FilmCameraTurnOn(m_vecFilmCameraTags[0], _isHUDActive);
	}
}

void CCameraMgr::FilmCameraTurnOn(const string& _strCameraTag, const _float3& _vLook, const _float3& _vOffSetPosition, bool _isHUDActive, bool _isDeactiveHUD)
{
	shared_ptr<CCameraFilm> pFilmCamera = FindFilmCamera(_strCameraTag);
	if (pFilmCamera)
	{
		if (m_pRecordingCamera.lock())
		{
			m_pRecordingCamera.lock()->CameraTurnOff();
		}
		if (m_pRecordingFilmCamera.lock())
		{
			m_pRecordingFilmCamera.lock()->ResetCamera();
		}
		m_pRecordingFilmCamera = pFilmCamera;
		pFilmCamera->StartProduction(_vLook, _vOffSetPosition);
		m_isHUDActive = _isHUDActive;
		if (_isDeactiveHUD)
		{
			UIMGR->DeactivateHUD();
		}
		UIMGR->SetIsCameraDirecting(true);
	}
}

void CCameraMgr::FilmEnd()
{
	if (m_pRecordingCamera.lock())
	{
		m_pRecordingCamera.lock()->CameraTurnOn();
	}
	if (m_pRecordingFilmCamera.lock())
	{
		m_pRecordingFilmCamera.lock()->ResetCamera();
		if (m_iCurFilmIndex + 1 < m_iFilmCnt)
		{
			FilmCameraTurnOn(m_vecFilmCameraTags[++m_iCurFilmIndex]);
		}
		else
		{
			m_pRecordingFilmCamera.reset();
			m_vecFilmCameraTags.clear();
			memset(&m_iCurFilmIndex, 0, sizeof(int));
			memset(&m_iFilmCnt, 0, sizeof(int));
			if (m_isHUDActive)
			{
				UIMGR->ActivateHUD();
			}
			if (GAMEINSTANCE->isFadeOut() || GAMEINSTANCE->isBlackOut())
			{
				GAMEINSTANCE->SetFadeEffect(false, 0.7f);
			}
			UIMGR->SetIsCameraDirecting(false);
		}
	}
}

void CCameraMgr::FilmEnd(const _float3& _vEye, const _float3& _vAt)
{
	if (m_pRecordingFilmCamera.lock())
	{
		m_pRecordingFilmCamera.lock()->ResetCamera();
		m_pRecordingFilmCamera.reset();
	}
	if (m_pRecordingCamera.lock())
	{
		m_pRecordingCamera.lock()->SetEyeAt(_vEye, _vAt);
		m_pRecordingCamera.lock()->CameraTurnOn();
	}
}

ROT_DESC CCameraMgr::RotSetting(_float3 vRotAmplitude, _float3 vRotFrequency, _float2 vBlendInOut, _float2 vInOutWeight, ESHAKEFUNC eShakeFunc, bool isOffSetRand)
{
	ROT_DESC RotDesc{};
	RotDesc.vRotAmplitude = vRotAmplitude;
	RotDesc.vRotFrequency = vRotFrequency;
	RotDesc.vBlendInOut = vBlendInOut;
	RotDesc.vInOutWeight = vInOutWeight;
	RotDesc.eShakeFunc = eShakeFunc;
	RotDesc.isOffSetRand = isOffSetRand;
	return RotDesc;
}

LOC_DESC CCameraMgr::LocSetting(_float3 vLocAmplitude, _float3 vLocFrequency, _float2 vBlendInOut, _float2 vInOutWeight, ESHAKEFUNC eShakeFunc, bool isOffSetRand)
{
	LOC_DESC LocDesc{};
	LocDesc.vLocAmplitude = vLocAmplitude;
	LocDesc.vLocFrequency = vLocFrequency;
	LocDesc.vBlendInOut = vBlendInOut;
	LocDesc.vInOutWeight = vInOutWeight;
	LocDesc.eShakeFunc = eShakeFunc;
	LocDesc.isOffSetRand = isOffSetRand;
	return LocDesc;
}

FOV_DESC CCameraMgr::FovSetting(float fFovyAmplitude, float fFovyFrequency, _float2 vBlendInOut, _float2 vInOutWeight, ESHAKEFUNC eShakeFunc)
{
	FOV_DESC FovDesc{};
	FovDesc.fFovyAmplitude = fFovyAmplitude;
	FovDesc.fFovyFrequency = fFovyFrequency;
	FovDesc.vBlendInOut = vBlendInOut;
	FovDesc.vInOutWeight = vInOutWeight;
	FovDesc.eShakeFunc = eShakeFunc;
	return FovDesc;
}

void CCameraMgr::CameraShake(UINT _eShakeFlags, float _fDuration, ROT_DESC* _pRotDesc, LOC_DESC* _pLocDesc, FOV_DESC* _pFovDesc)
{
	if (m_pRecordingCamera.lock())
	{
		m_pRecordingCamera.lock()->CamShake(_eShakeFlags, _fDuration, _pRotDesc, _pLocDesc, _pFovDesc);
	}
}

void CCameraMgr::CameraShakeStop()
{
	if (m_pRecordingCamera.lock())
	{
		m_pRecordingCamera.lock()->CamShakeStop();
	}
}

map<const string, weak_ptr<CCamera>>* CCameraMgr::GetCameras()
{
	return &m_Cameras;
}

void CCameraMgr::SwitchingMouseLock()
{
	if (m_pRecordingCamera.lock())
	{
		m_pRecordingCamera.lock()->SwitchingMouseLock();
	}
}

void CCameraMgr::MouseLockOn()
{
	if (m_pRecordingCamera.lock())
	{
		m_pRecordingCamera.lock()->MouseLockOn();
	}
}

void CCameraMgr::MouseLockOff()
{
	if (m_pRecordingCamera.lock())
	{
		m_pRecordingCamera.lock()->SwitchingMouseLock();
	}
}

shared_ptr<CCamera> CCameraMgr::FindCamera(const string& _strCameraTag)
{
	auto iter = m_Cameras.find(_strCameraTag);
	if (m_Cameras.end() == iter)
		return nullptr;

	return iter->second.lock();
}

shared_ptr<CCameraFilm> CCameraMgr::FindFilmCamera(const string& _strCameraTag)
{
	auto iter = m_FilmCameras.find(_strCameraTag);
	if (m_FilmCameras.end() == iter)
		return nullptr;

	return iter->second.lock();
}

void CCameraMgr::DeleteCamera(const string& _strCameraTag)
{
	auto iter = m_Cameras.find(_strCameraTag);
	if (m_Cameras.end() != iter)
	{
		m_Cameras.erase(iter);
	}
}

void CCameraMgr::DeleteCamera(shared_ptr<CCamera> _pThis)
{
	auto iter = find_if(m_Cameras.begin(), m_Cameras.end(), [&](pair<const string, weak_ptr<CCamera>>& _Pair)->bool { return _pThis == _Pair.second.lock(); });
	if (m_Cameras.end() != iter)
	{
		m_Cameras.erase(iter);
	}
}

void CCameraMgr::DeleteFilmCamera(const string& _strCameraTag)
{
	auto iter = m_FilmCameras.find(_strCameraTag);
	if (m_FilmCameras.end() != iter)
	{
		m_FilmCameras.erase(iter);
	}
}

void CCameraMgr::DeleteFilmCamera(shared_ptr<CCameraFilm> _pThis)
{
	auto iter = find_if(m_FilmCameras.begin(), m_FilmCameras.end(), [&](pair<const string, weak_ptr<CCameraFilm>>& _Pair)->bool {return _pThis == _Pair.second.lock(); });
	if (m_FilmCameras.end() != iter)
	{
		m_FilmCameras.erase(iter);
	}
}

void CCameraMgr::ClearLevel()
{
	for (auto iter = m_Cameras.begin(); iter != m_Cameras.end();)
	{
		if ((*iter).second.lock())
		{
			iter++;
		}
		else
		{
			iter = m_Cameras.erase(iter);
		}
	}
	for (auto iter = m_FilmCameras.begin(); iter != m_FilmCameras.end();)
	{
		if ((*iter).second.lock())
		{
			iter++;
		}
		else
		{
			iter = m_FilmCameras.erase(iter);
		}
	}
}

void CCameraMgr::Clear()
{
	m_Cameras.clear();
	m_FilmCameras.clear();
}
