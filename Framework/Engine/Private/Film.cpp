#include "Film.h"
#include "GameInstance.h"
#include "CameraProduction.h"
#include "Camera.h"

CFilm::CFilm(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	: CComponent(_pDevice, _pContext)
{
}

shared_ptr<CFilm> CFilm::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, const wstring& _wstrFilePath)
{
	shared_ptr<CFilm> pInstance = make_shared<CFilm>(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_wstrFilePath)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CFilm::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CFilm::Initialize(const wstring& _wstrFilePath)
{
	Json::Value Film = GAMEINSTANCE->ReadJson(_wstrFilePath);
	UINT NumProductions = (UINT)Film.size();
	m_OriginalCameraProductions.reserve(NumProductions);
	_float3 vEye;
	_float3 vAt;
	float fLerpTime{ 0.0f };

	UINT iFadeFlag{ 0 };
	float fFadeTime{ 0.0f };
	float fFadeDuration{ 0.0f };

	UINT iShakeFlags{ 0 };
	float fStartTime{ 0.0f };
	float fDuration{ 0.0f };
	ROT_DESC RotDesc{};
	LOC_DESC LocDesc{};
	FOV_DESC FovDesc{};

	for (UINT i = 0; i < NumProductions; i++)
	{
		string strNum = to_string(i);

		vEye.x = Film[strNum]["Eye"][0].asFloat();
		vEye.y = Film[strNum]["Eye"][1].asFloat();
		vEye.z = Film[strNum]["Eye"][2].asFloat();
		vAt.x = Film[strNum]["At"][0].asFloat();
		vAt.y = Film[strNum]["At"][1].asFloat();
		vAt.z = Film[strNum]["At"][2].asFloat();
		fLerpTime = Film[strNum]["LerpTime"].asFloat();

		shared_ptr<CCameraProduction> pLoadProduction = CCameraProduction::Create(fLerpTime, vEye, vAt);

		if (Film[strNum].isMember("FadeInfo"))
		{
			iFadeFlag = Film[strNum]["FadeInfo"]["FadeFlag"].asUInt();
			fFadeTime = Film[strNum]["FadeInfo"]["FadeTime"].asFloat();
			fFadeDuration = Film[strNum]["FadeInfo"]["FadeDuration"].asFloat();
			pLoadProduction->SetFadeInfo(iFadeFlag, fFadeTime, fFadeDuration);
		}

		if (Film[strNum].isMember("ShakeInfo"))
		{
			iShakeFlags = Film[strNum]["ShakeInfo"]["ShakeFlags"].asUInt();
			fStartTime = Film[strNum]["ShakeInfo"]["StartTime"].asFloat();
			fDuration = Film[strNum]["ShakeInfo"]["Duration"].asFloat();

			if (iShakeFlags & SHAKE_ROT)
			{
				RotDesc.vRotAmplitude.x = Film[strNum]["ShakeInfo"]["RotDesc"]["Amplitude"][0].asFloat();
				RotDesc.vRotAmplitude.y = Film[strNum]["ShakeInfo"]["RotDesc"]["Amplitude"][1].asFloat();
				RotDesc.vRotAmplitude.z = Film[strNum]["ShakeInfo"]["RotDesc"]["Amplitude"][2].asFloat();

				RotDesc.vRotFrequency.x = Film[strNum]["ShakeInfo"]["RotDesc"]["Frequency"][0].asFloat();
				RotDesc.vRotFrequency.y = Film[strNum]["ShakeInfo"]["RotDesc"]["Frequency"][1].asFloat();
				RotDesc.vRotFrequency.z = Film[strNum]["ShakeInfo"]["RotDesc"]["Frequency"][2].asFloat();

				RotDesc.vBlendInOut.x = Film[strNum]["ShakeInfo"]["RotDesc"]["BlendInOut"][0].asFloat();
				RotDesc.vBlendInOut.y = Film[strNum]["ShakeInfo"]["RotDesc"]["BlendInOut"][1].asFloat();

				RotDesc.vInOutWeight.x = Film[strNum]["ShakeInfo"]["RotDesc"]["InOutWeight"][0].asFloat();
				RotDesc.vInOutWeight.y = Film[strNum]["ShakeInfo"]["RotDesc"]["InOutWeight"][1].asFloat();

				RotDesc.eShakeFunc = (ESHAKEFUNC)Film[strNum]["ShakeInfo"]["RotDesc"]["ShakeFunc"].asUInt();

				RotDesc.isOffSetRand = Film[strNum]["ShakeInfo"]["RotDesc"]["isOffSetRand"].asBool();
			}
			if (iShakeFlags & SHAKE_LOC)
			{
				LocDesc.vLocAmplitude.x = Film[strNum]["ShakeInfo"]["LocDesc"]["Amplitude"][0].asFloat();
				LocDesc.vLocAmplitude.y = Film[strNum]["ShakeInfo"]["LocDesc"]["Amplitude"][1].asFloat();
				LocDesc.vLocAmplitude.z = Film[strNum]["ShakeInfo"]["LocDesc"]["Amplitude"][2].asFloat();

				LocDesc.vLocFrequency.x = Film[strNum]["ShakeInfo"]["LocDesc"]["Frequency"][0].asFloat();
				LocDesc.vLocFrequency.y = Film[strNum]["ShakeInfo"]["LocDesc"]["Frequency"][1].asFloat();
				LocDesc.vLocFrequency.z = Film[strNum]["ShakeInfo"]["LocDesc"]["Frequency"][2].asFloat();

				LocDesc.vBlendInOut.x = Film[strNum]["ShakeInfo"]["LocDesc"]["BlendInOut"][0].asFloat();
				LocDesc.vBlendInOut.y = Film[strNum]["ShakeInfo"]["LocDesc"]["BlendInOut"][1].asFloat();

				LocDesc.vInOutWeight.x = Film[strNum]["ShakeInfo"]["LocDesc"]["InOutWeight"][0].asFloat();
				LocDesc.vInOutWeight.y = Film[strNum]["ShakeInfo"]["LocDesc"]["InOutWeight"][1].asFloat();

				LocDesc.eShakeFunc = (ESHAKEFUNC)Film[strNum]["ShakeInfo"]["LocDesc"]["ShakeFunc"].asUInt();

				LocDesc.isOffSetRand = Film[strNum]["ShakeInfo"]["LocDesc"]["isOffSetRand"].asBool();
			}
			if (iShakeFlags & SHAKE_FOV)
			{
				FovDesc.fFovyAmplitude = Film[strNum]["ShakeInfo"]["FovDesc"]["Amplitude"].asFloat();

				FovDesc.fFovyFrequency = Film[strNum]["ShakeInfo"]["FovDesc"]["Frequency"].asFloat();

				FovDesc.vBlendInOut.x = Film[strNum]["ShakeInfo"]["FovDesc"]["BlendInOut"][0].asFloat();
				FovDesc.vBlendInOut.y = Film[strNum]["ShakeInfo"]["FovDesc"]["BlendInOut"][1].asFloat();

				FovDesc.vInOutWeight.x = Film[strNum]["ShakeInfo"]["FovDesc"]["InOutWeight"][0].asFloat();
				FovDesc.vInOutWeight.y = Film[strNum]["ShakeInfo"]["FovDesc"]["InOutWeight"][1].asFloat();

				FovDesc.eShakeFunc = (ESHAKEFUNC)Film[strNum]["ShakeInfo"]["FovDesc"]["ShakeFunc"].asUInt();
			}

			pLoadProduction->SetShakeInfo(fStartTime,iShakeFlags, fDuration, &RotDesc, &LocDesc, &FovDesc);
		}

		m_OriginalCameraProductions.emplace_back(pLoadProduction);
	}

	return S_OK;
}

HRESULT CFilm::PrepareFilm()
{
	memset(&m_iCurrentProduction, 0, sizeof(int));
	memset(&m_fTrackPosition, 0, sizeof(float));
	m_isDoneFade = false;
	m_isDoneShake = false;
	m_isFinished = false;

	return S_OK;
}

HRESULT CFilm::PrepareFilm(const _float3& _vLook, const _float3& _vPosition)
{
	m_ConvertCameraProductions.clear();
	m_ConvertCameraProductions.reserve(m_OriginalCameraProductions.size());

	XMVECTOR vLook = XMLoadFloat3(&_vLook);
	XMVECTOR vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
	vLook = XMVector3Cross(vRight, vUp);
	
	XMVECTOR vStandard = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	float fRadian{ 0.0f };

	if (XMVectorGetY(XMVector3Cross(vStandard, vLook)) >= 0)
	{
		fRadian = acosf(XMVectorGetX(XMVector3Dot(vStandard, vLook)));
	}
	else
	{
		fRadian = XM_2PI - acosf(XMVectorGetX(XMVector3Dot(vStandard, vLook)));
	}

	XMMATRIX ConvertMatrix = XMMatrixRotationY(fRadian) * XMMatrixTranslationFromVector(_vPosition);
	

	for (auto& pProduction : m_OriginalCameraProductions)
	{
		m_ConvertCameraProductions.emplace_back(CCameraProduction::Convert(pProduction, ConvertMatrix));
	}

	memset(&m_iCurrentProduction, 0, sizeof(int));
	memset(&m_fTrackPosition, 0, sizeof(float));
	m_isDoneFade = false;
	m_isDoneShake = false;
	m_isFinished = false;

	return S_OK;
}

HRESULT CFilm::PlayProduction(float _fTimeDelta, shared_ptr<CCamera> _pCamera, bool _isPlayOriginal)
{
	vector<shared_ptr<CCameraProduction>>& vecCameraProductions = _isPlayOriginal ? m_OriginalCameraProductions : m_ConvertCameraProductions;

	int NumProductions{ (int)vecCameraProductions.size() };
	if (m_iCurrentProduction < NumProductions)
	{
		// Fade InOut
		if (vecCameraProductions[m_iCurrentProduction]->GetFade() && !m_isDoneFade)
		{
			UINT eFade{ 0 };
			float fFadeInOutTime{ 0.0f };
			float fFadeInOutDuration{ 0.0f };
			vecCameraProductions[m_iCurrentProduction]->GetFadeInfo(&eFade, &fFadeInOutTime, &fFadeInOutDuration);
			if (m_fTrackPosition >= fFadeInOutTime)
			{
				GAMEINSTANCE->SetFadeEffect(eFade == FADE_IN ? false : true, fFadeInOutDuration);
				m_isDoneFade = true;
			}
		}

		// Shake
		if (vecCameraProductions[m_iCurrentProduction]->hasCamShake() && !m_isDoneShake && m_fTrackPosition >= vecCameraProductions[m_iCurrentProduction]->GetShakeStartTime())
		{
			UINT ShakeFlags{ 0 };
			float fDuration{ 0.0f };
			ROT_DESC Rot{};
			LOC_DESC Loc{};
			FOV_DESC Fov{};
			vecCameraProductions[m_iCurrentProduction]->GetShakeInfo(&ShakeFlags, &fDuration, &Rot, &Loc, &Fov);
			_pCamera->CamShake(ShakeFlags, fDuration, &Rot, &Loc, &Fov);
			m_isDoneShake = true;
		}

		float CurrentLerpTime{ vecCameraProductions[m_iCurrentProduction]->GetLerpTime() };
		int NumLeftProduction{ NumProductions - m_iCurrentProduction - 1 };	

		// Interpolation
		if (m_fTrackPosition < CurrentLerpTime)
		{
			float fRatio{ m_fTrackPosition / CurrentLerpTime };

			if (NumLeftProduction >= 2)
			{
				if (m_iCurrentProduction > 0)
				{
					CatmullRomFunc(_pCamera, -1, 0, 1, 2, fRatio, _isPlayOriginal);
				}
				else
				{
					CatmullRomFunc(_pCamera, 0, 0, 1, 2, fRatio, _isPlayOriginal);
				}
			}
			else if (NumLeftProduction == 1)
			{
				if (m_iCurrentProduction > 0)
				{
					CatmullRomFunc(_pCamera, -1, 0, 1, 1, fRatio, _isPlayOriginal);
				}
				else
				{
					LinearFunc(_pCamera, fRatio, _isPlayOriginal);
				}
			}
			else
			{
				shared_ptr<CTransform> pCamTransform = _pCamera->GetTransform();
				pCamTransform->SetState(CTransform::STATE_POSITION, vecCameraProductions[m_iCurrentProduction]->GetEye());
				pCamTransform->LookAt(vecCameraProductions[m_iCurrentProduction]->GetAt());
			}
			m_fTrackPosition += _fTimeDelta;
		}
		else
		{
			m_iCurrentProduction++;
			memset(&m_fTrackPosition, 0, sizeof(float));
			m_isDoneFade = false;
			m_isDoneShake = false;
			PlayProduction(_fTimeDelta, _pCamera, _isPlayOriginal);
		}
	}
	else
	{
		EndProduction();
	}

	return S_OK;
}

HRESULT CFilm::EndProduction()
{
	memset(&m_iCurrentProduction, 0, sizeof(int));
	memset(&m_fTrackPosition, 0, sizeof(float));
	m_isDoneFade = false;
	m_isDoneShake = false;
	m_isFinished = true;

	return S_OK;
}

void CFilm::GetEndEyeAt(_float3* _vEye, _float3* _vAt, bool _isOriginal)
{
	vector<shared_ptr<CCameraProduction>>& vecCameraProductions = _isOriginal ? m_OriginalCameraProductions : m_ConvertCameraProductions;
	*_vEye = vecCameraProductions.back()->GetEye();
	*_vAt = vecCameraProductions.back()->GetAt();
}

void CFilm::CatmullRomFunc(shared_ptr<CCamera> _pCamera, int _i0, int _i1, int _i2, int _i3, float _fRatio, bool _isOriginal)
{
	if (_pCamera)
	{
		vector<shared_ptr<CCameraProduction>>& vecCameraProductions = _isOriginal ? m_OriginalCameraProductions : m_ConvertCameraProductions;

		XMVECTOR PreEye = vecCameraProductions[m_iCurrentProduction + _i0]->GetEye();
		XMVECTOR CurEye = vecCameraProductions[m_iCurrentProduction + _i1]->GetEye();
		XMVECTOR NextEye = vecCameraProductions[m_iCurrentProduction + _i2]->GetEye();
		XMVECTOR NNextEye = vecCameraProductions[m_iCurrentProduction + _i3]->GetEye();

		XMVECTOR PreAt = vecCameraProductions[m_iCurrentProduction + _i0]->GetAt();
		XMVECTOR CurAt = vecCameraProductions[m_iCurrentProduction + _i1]->GetAt();
		XMVECTOR NextAt = vecCameraProductions[m_iCurrentProduction + _i2]->GetAt();
		XMVECTOR NNextAt = vecCameraProductions[m_iCurrentProduction + _i3]->GetAt();

		shared_ptr<CTransform> pCamTransform = _pCamera->GetTransform();
		pCamTransform->SetState(CTransform::STATE_POSITION, XMVectorCatmullRom(PreEye, CurEye, NextEye, NNextEye, _fRatio));
		pCamTransform->LookAt(XMVectorCatmullRom(PreAt, CurAt, NextAt, NNextAt, _fRatio));
	}
}

void CFilm::LinearFunc(shared_ptr<CCamera> _pCamera, float _fRatio, bool _isOriginal)
{
	if (_pCamera)
	{
		vector<shared_ptr<CCameraProduction>>& vecCameraProductions = _isOriginal ? m_OriginalCameraProductions : m_ConvertCameraProductions;

		XMVECTOR CurEye = vecCameraProductions[m_iCurrentProduction]->GetEye();
		XMVECTOR NextEye = vecCameraProductions[m_iCurrentProduction + 1]->GetEye();

		XMVECTOR CurAt = vecCameraProductions[m_iCurrentProduction]->GetAt();
		XMVECTOR NextAt = vecCameraProductions[m_iCurrentProduction + 1]->GetAt();

		shared_ptr<CTransform> pCamTransform = _pCamera->GetTransform();
		pCamTransform->SetState(CTransform::STATE_POSITION, XMVectorLerp(CurEye, NextEye, _fRatio));
		pCamTransform->LookAt(XMVectorLerp(CurAt, NextAt, _fRatio));
	}
}
