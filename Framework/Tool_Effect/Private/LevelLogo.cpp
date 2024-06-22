#include "LevelLogo.h"
#include "CameraFree.h"

CLevelLogo::CLevelLogo()
{
}

HRESULT CLevelLogo::Initialize()
{
    ReadyLight();

    if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(ReadyCamera(TEXT("Layer_Camera"))))
        return E_FAIL;

    return S_OK;
}

void CLevelLogo::Tick(_float _fTimeDelta)
{
}

void CLevelLogo::LateTick(_float _fTimeDelta)
{
}

HRESULT CLevelLogo::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevelLogo::ReadyLight()
{
    LIGHT_DESC		LightDesc = {};

    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, -1.f, 0.f);
    LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
    LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;

    if (FAILED(GAMEINSTANCE->AddLight(LightDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelLogo::ReadyCamera(const wstring& strLayerTag)
{
    shared_ptr<CGameObject> pCam;

    CCameraFree::CAMERAFREE_DESC pCamFreeDesc = {};
    pCamFreeDesc.fMouseSensor = 0.05f;

    CCameraFree::CAMERA_DESC pCamDesc = {};
    pCamDesc.vEye = _float3(0.0f, 30.f, -25.f);
    pCamDesc.vAt = _float3(0.0f, 0.0f, 0.f);
    pCamDesc.fFovy = XMConvertToRadians(60.0f);
    pCamDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    pCamDesc.fNear = 0.2f;
    pCamDesc.fFar = 1000.f;

    CTransform::TRANSFORM_DESC pTransDesc = {};
    pTransDesc.fSpeedPerSec = 20.f;
    pTransDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    pCam = CCameraFree::Create(true, &pCamFreeDesc, &pCamDesc, &pTransDesc);

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, strLayerTag, pCam)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelLogo::ReadyLayerBackGround(const wstring& strLayerTag)
{

    //shared_ptr<CTestLogo> pLogo = CTestLogo::Create();

    //if(FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, strLayerTag, pLogo)))
    //    return E_FAIL;

    /*shared_ptr<CTestModel> pModel = CTestModel::Create();

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, strLayerTag, pModel)))
        return E_FAIL;*/

    return S_OK;
}

shared_ptr<CLevelLogo> CLevelLogo::Create()
{
    shared_ptr<CLevelLogo> pInstance = make_shared<CLevelLogo>();

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CLevelLogo");

    return pInstance;
}
