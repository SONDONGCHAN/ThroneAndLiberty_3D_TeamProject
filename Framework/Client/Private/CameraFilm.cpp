#include "CameraFilm.h"
#include "Film.h"
#include "CameraMgr.h"

Client::CCameraFilm::CCameraFilm()
    : CCamera()
{
}

shared_ptr<CCameraFilm> Client::CCameraFilm::Create(CAMERA_DESC* _pCameraDesc, const wstring& _wstrFilePath)
{
	shared_ptr<CCameraFilm> pInstance = make_shared<CCameraFilm>();

	if (FAILED(pInstance->Initialize(_pCameraDesc, _wstrFilePath)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CCameraFilm::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT Client::CCameraFilm::Initialize(CAMERA_DESC* _pCameraDesc, const wstring& _wstrFilePath)
{
	if (FAILED(CCamera::Initialize(_pCameraDesc)))
		return E_FAIL;

	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	m_pFilm = CFilm::Create(pGameInstance->GetDeviceInfo(), pGameInstance->GetDeviceContextInfo(), _wstrFilePath);
	if (nullptr == m_pFilm)
		return E_FAIL;


    return S_OK;
}

void Client::CCameraFilm::PriorityTick(_float fTimeDelta)
{
	if (m_isStart)
	{
		if (!m_pFilm->isFinished())
		{
			m_pFilm->PlayProduction(fTimeDelta, static_pointer_cast<CCamera>(shared_from_this()), m_isOriginal);
			CCamera::PriorityTick(fTimeDelta);
			SetUpTransformMatices();
			SetUpCascadeDesc();
		}
		else
		{
			CCameraMgr::GetInstance()->FilmEnd();
		}
	}
}

void Client::CCameraFilm::Tick(_float fTimeDelta)
{
}

void Client::CCameraFilm::LateTick(_float fTimeDelta)
{
}

HRESULT Client::CCameraFilm::Render()
{
    return S_OK;
}

HRESULT Client::CCameraFilm::StartProduction()
{
	m_pFilm->PrepareFilm();
	m_isStart = true;
	m_isFinish = false;
	m_isOriginal = true;

	return S_OK;
}

HRESULT Client::CCameraFilm::StartProduction(const _float3& _vLook, const _float3& _vPosition)
{
	m_pFilm->PrepareFilm(_vLook, _vPosition);
	m_isStart = true;
	m_isFinish = false;
	m_isOriginal = false;

	return S_OK;
}

HRESULT Client::CCameraFilm::ResetCamera()
{
	m_pFilm->EndProduction();
	m_isStart = false;
	m_isFinish = true;

	return S_OK;
}
