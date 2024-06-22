#include "LevelAnimToolEdit.h"
#include "GameInstance.h"
#include "AnimToolCamera.h"

CLevelAnimToolEdit::CLevelAnimToolEdit()
{
}

HRESULT CLevelAnimToolEdit::Initialize()
{
    if (FAILED(ReadyLight()))
        return E_FAIL;

    if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(ReadyCamera(TEXT("Layer_Camera"))))
        return E_FAIL;

    return S_OK;
}

void CLevelAnimToolEdit::Tick(_float _fTimeDelta)
{
}

void CLevelAnimToolEdit::LateTick(_float _fTimeDelta)
{
}

HRESULT CLevelAnimToolEdit::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("에디터 레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevelAnimToolEdit::ReadyLight()
{
    LIGHT_DESC		LightDesc = {};
    
    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
    LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    
    if (FAILED(GAMEINSTANCE->AddLight(LightDesc)))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CLevelAnimToolEdit::ReadyLayerBackGround(const wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevelAnimToolEdit::ReadyCamera(const wstring& strLayerTag)
{
    shared_ptr<CGameObject> pCam;

    CAnimToolCamera::CAMERAFREE_DESC pCamFreeDesc = {};
    pCamFreeDesc.fMouseSensor = 0.05f;

    CAnimToolCamera::CAMERA_DESC pCamDesc = {};
    pCamDesc.vEye = _float3(0.0f, 3.f, -5.f);
    pCamDesc.vAt = _float3(0.0f, 2.f, 0.f);
    pCamDesc.fFovy = XMConvertToRadians(60.0f);
    pCamDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    pCamDesc.fNear = 0.2f;
    pCamDesc.fFar = 1000.f;

    CTransform::TRANSFORM_DESC pTransDesc = {};
    pTransDesc.fSpeedPerSec = 5.f;
    pTransDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    pCam = CAnimToolCamera::Create(true, &pCamFreeDesc, &pCamDesc, &pTransDesc);

    if (FAILED(CGameInstance::GetInstance()->AddObject(LEVEL_LOGO, strLayerTag, pCam)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelAnimToolEdit::ReadyLayerAnimObject(const wstring& strLayerTag)
{
    return S_OK;
}

shared_ptr<CLevelAnimToolEdit> CLevelAnimToolEdit::Create()
{
    shared_ptr<CLevelAnimToolEdit> pInstance = make_shared<CLevelAnimToolEdit>();

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CLevelAnimToolEdit");

    return pInstance;
}
