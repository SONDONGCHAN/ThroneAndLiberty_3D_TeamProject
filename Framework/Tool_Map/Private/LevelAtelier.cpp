#include "LevelAtelier.h"
#include "ToolMgr.h"
#include "CameraFree.h"

#include "EnvironmentObject.h"

LevelAtelier::LevelAtelier()
	: CLevel()
{
}

#ifdef CHECK_REFERENCE_COUNT
LevelAtelier::~LevelAtelier()
{
	m_pToolMgr.reset();
}
#endif // CHECK_REFERENCE_COUNT

HRESULT LevelAtelier::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Stage(TEXT("Layer_Stage"))))
		return E_FAIL;

	m_pToolMgr = MANAGER;
	m_pToolMgr->Initialize();
	//m_pToolMgr::
	if (m_pToolMgr == nullptr)
		E_FAIL;

	return S_OK;
}

void LevelAtelier::Tick(_float _fTimeDelta)
{
	m_pToolMgr->Tick(_fTimeDelta);

#ifdef _DEBUG
	m_fTimeAcc += _fTimeDelta;
#endif // _DEBUG
}

HRESULT LevelAtelier::Render()
{
	if (FAILED(m_pToolMgr->Render()))
		return E_FAIL;

#ifdef _DEBUG
	++m_iNumRender;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS:%d"), m_iNumRender);

		m_iNumRender = 0;
		m_fTimeAcc = 0.f;
	}
	SetWindowText(g_hWnd, m_szFPS);
#endif // _DEBUG

	return S_OK;
}

HRESULT LevelAtelier::Ready_Lights()
{
	LIGHT_DESC LightDesc = {};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.9f, 0.9f, 0.9f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);

 	if (FAILED(GAMEINSTANCE->AddLight(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT LevelAtelier::Ready_Layer_Camera(const wstring& _strLayerTag)
{
	shared_ptr<CGameObject> pCam;

	CCameraFree::CAMERAFREE_DESC pCamFreeDesc = {};
	pCamFreeDesc.fMouseSensor = 0.05f;

	CCameraFree::CAMERA_DESC pCamDesc = {};
	pCamDesc.vEye = _float3(-0.5f, 0.5f, -0.5f);
	pCamDesc.vAt = _float3(0.0f, 0.0f, 0.f);
	pCamDesc.fFovy = XMConvertToRadians(60.0f);
	pCamDesc.fAspect = (_float)g_iWinCX / g_iWinCY;
	pCamDesc.fNear = 0.01f;
	pCamDesc.fFar = 1000.f;

	CTransform::TRANSFORM_DESC pTransDesc = {};
	pTransDesc.fSpeedPerSec = 10.f;
	pTransDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	pCam = CCameraFree::Create(true, &pCamFreeDesc, &pCamDesc, &pTransDesc);

	if (FAILED(CGameInstance::GetInstance()->AddObject(LEVEL_ATELIER, _strLayerTag, pCam)))
		return E_FAIL;

	return S_OK;
}

HRESULT LevelAtelier::Ready_Layer_Stage(const wstring& _strLayerTag)
{

	return S_OK;
}

shared_ptr<LevelAtelier> LevelAtelier::Create()
{
	shared_ptr<LevelAtelier> pInstance = WRAPPING(LevelAtelier)();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : LevelAtelier");
		pInstance.reset();
	}

	return pInstance;
}
