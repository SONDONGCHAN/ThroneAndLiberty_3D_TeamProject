
#include "MainApp.h"

#include "GameInstance.h"
#include "LevelLogo.h"

#include "ImguiMgr.h"
#include "UIMgr.h"
#include "QuestMgr.h"
#include "DialogueMgr.h"

#include "LevelLoading.h"
#include "ThreadPool.h"
#include "EventMgr.h"
#include "CharacterMgr.h"
#include "CameraMgr.h"
#include "StaticCamera.h"
#include "CameraFree.h"

#include "UIPanel.h"

#include "GameMgr.h"



CMainApp::CMainApp()
{
}

CMainApp::~CMainApp()
{
    Free();
}

HRESULT CMainApp::Initialize()
{
    /*Random Seed*/
    srand((unsigned int)time(NULL));

    GRAPHIC_DESC GraphicDesc = {};
                                                                                                                               
    GraphicDesc.hWnd = g_hWnd;
    GraphicDesc.isWindowed = true;
    GraphicDesc.iWinSizeX = g_iWinSizeX;
    GraphicDesc.iWinSizeY = g_iWinSizeY;

    if (FAILED(CGameInstance::GetInstance()->InitializeEngine(g_hInst, LEVEL_END, GraphicDesc)))
        return E_FAIL;

    CGameInstance::GetInstance()->LoadLevelResource(LEVEL_LOGO);

    m_pImguiMgr = CImguiMgr::Create();


    UILoader();
    //
    /***콘솔 사용법***/
    /*console(작성할 내용); <printf 같은 형식으로 작성하시면 됩니다*/
    /*디버그 때만 콘솔이 돌아가도록 설정해둬서 되도록 내용 확인이 끝나면 출력부는 지워주세요*/
#ifdef _DEBUG
    /*디버깅용 콘솔 세팅*/
    if (AllocConsole()) {
        freopen_s(&pFile, "CONOUT$", "w", stdout);
    }
#endif

    /*로딩 레벨을 미리 세팅한 후 이후에도 계속 재활용한다*/
    //shared_ptr<CLevelLoading> pLoadingLevel = CLevelLoading::Create();
    //GAMEINSTANCE->SetLoadingLevel(pLoadingLevel);

    if (FAILED(OpenLevel(LEVEL_LOGO)))
        return E_FAIL;

    shared_ptr<CStaticCamera> pCamera = CStaticCamera::Create();
    if (pCamera)
    {
        GAMEINSTANCE->AddObject(LEVEL_STATIC, L"Layer_Camera", pCamera);
        CCameraMgr::GetInstance()->AddCamera("StaticCamera", pCamera);
    }

    CCamera::CAMERA_DESC pCamDesc = {};
    pCamDesc.fFovy = XMConvertToRadians(60.0f);
    pCamDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    pCamDesc.fNear = 0.1f;
    pCamDesc.fFar = 1000.f;

    CCameraFree::CAMERAFREE_DESC FreeDesc = {};
    FreeDesc.fMouseSensor = 0.5f;

    CTransform::TRANSFORM_DESC TransformDesc = {};
    TransformDesc.fRotationPerSec = XMConvertToRadians(10.f);
    TransformDesc.fSpeedPerSec = 100.f;


    shared_ptr<CCameraFree> pFreeCam = CCameraFree::Create(false, &FreeDesc, &pCamDesc, &TransformDesc);
    if (pFreeCam)
    {
        GAMEINSTANCE->AddObject(LEVEL_STATIC, L"Layer_Camera", pFreeCam);
        CCameraMgr::GetInstance()->AddCamera("FreeCamera", pFreeCam);
    }


    ShowCursor(false);

    return S_OK;
}

void CMainApp::Tick(_float _fTimeDelta)
{ 
    UIMGR->Tick(_fTimeDelta);
    CGameInstance::GetInstance()->TickEngine(_fTimeDelta);
    QUESTMGR->Tick(_fTimeDelta);
    DIALOGUEMGR->Tick(_fTimeDelta);
    CGameMgr::GetInstance()->Tick(_fTimeDelta);

//#ifdef _DEBUG
    KeyInput();
    if (m_IsDebugOptionOn) {
        m_pImguiMgr->Tick(_fTimeDelta);
    }
//#endif // _DEBUG

    m_fTimeAcc += _fTimeDelta;
    if (m_fTimeAcc >= 1.0f)
    {
        wsprintf(m_szFPS, L"FPS : %d", m_iFPS);
        m_iFPS = 0;
        m_fTimeAcc = 0.0f;
    }
    SetWindowText(g_hWnd, m_szFPS);

}

HRESULT CMainApp::Render()
{
    if (FAILED(CGameInstance::GetInstance()->ClearBackBufferView(_float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    if (FAILED(CGameInstance::GetInstance()->ClearDepthStencilView()))
        return E_FAIL;

    if (FAILED(CGameInstance::GetInstance()->Draw()))
        return E_FAIL;

    //Imgui Render

//#ifdef _DEBUG
    if (m_IsDebugOptionOn) {
        m_pImguiMgr->Render();
    }
//#endif // _DEBUG

    m_iFPS++;

    if (FAILED(CGameInstance::GetInstance()->Present()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::OpenLevel(LEVEL _eStartLevel)
{

    GAMEINSTANCE->SetLoadingLevel(CLevelLoading::Create(LEVEL_LOGO));
    GAMEINSTANCE->OpenLevelForClient(LEVEL_LOGO);
    //GAMEINSTANCE->OpenLevel2(LEVEL_LOGO, CLevelLoading::Create(LEVEL_LOGO));

    return S_OK;
}

HRESULT CMainApp::UILoader()
{
    GAMEINSTANCE->LoadLevelResource(LEVEL::LEVEL_STATIC);

    if (FAILED(GAMEINSTANCE->AddFont("Font_AsiaG", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/AsiaGothic.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_AdenL", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/AdenL.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_AdenL8", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/AdenL8.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_AdenL10", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/AdenL10.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_ArnoPro8", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/ArnoPro8.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_ArnoPro10", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/ArnoPro10.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_DroidFall8", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/DroidFall8.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_DroidFall10", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/DroidFall10.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_DNF10", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/DNF10.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_SuiteL10", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/SuiteL10.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_SuiteL12", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/SuiteL12.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_SuiteL15", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/SuiteL15.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_DNFB10", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/DnfForgedB10.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_DNFB12", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/DnfForgedB12.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_DNFB14", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/DnfForgedB14.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_DNFB16", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/DnfForgedB16.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_DNFB18", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/DnfForgedB18.spritefont"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_DNFB24", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/DnfForgedB24.spritefont"))))
        return E_FAIL;

    if (FAILED(UIMGR->PreLoadUIsJASON(TEXT("../Bin/Resources/Level_Static/Textures/UI/UIData/"))))
        return E_FAIL;

    if (FAILED(UIMGR->Initialize()))
        return E_FAIL;

    shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("QuickSlotPanel"));
    if (pUI)
        pUI->SetUIPos(pUI->GetUIPos().x, pUI->GetUIPos().y - 20.f);

    shared_ptr<CUIPanel> pTutorial = dynamic_pointer_cast<CUIPanel>(UIMGR->FindUI("TutorialPanel"));
    if (pTutorial)
        pTutorial->SetUIPos(-420.f, -103.0f);

    UIMGR->DeactivateUI("InventoryPanel");

    UIMGR->DeactivateUI("CraftAllPanel");

    UIMGR->DeactivateUI("TradeAllPanel");

    UIMGR->DeactivateItemToolTip();

    UIMGR->DeactivateSpeechBallon();

    UIMGR->DeactivateOption();

    UIMGR->DeactivateStarForce();

    UIMGR->DeactivateTradeItemToolTip();

    UIMGR->DeactivateCatapultTutorial();

    if (FAILED(DIALOGUEMGR->Initialize()))
        return E_FAIL;

    if (FAILED(QUESTMGR->Initialize()))
        return E_FAIL;

    UIMGR->UITickOff();


    return S_OK;
}

void CMainApp::KeyInput()
{
    if (GAMEINSTANCE->KeyDown(DIK_GRAVE)) {

        m_IsDebugOptionOn = !m_IsDebugOptionOn;
    }

    /*Partition Screen(Left : Diffuse Only , Right : Complete Screen)*/
    if (GAMEINSTANCE->KeyDown(DIK_MINUS)) {

        m_IsHalfScreen = !m_IsHalfScreen;
        GAMEINSTANCE->PartitionScreenHalf(m_IsHalfScreen);

    }

    if (GAMEINSTANCE->KeyDown(DIK_EQUALS)) {

        m_IsUIOn = !m_IsUIOn;

        if (m_IsUIOn) {
            UIMGR->ActivateHUD();
        }
        else
        {
            UIMGR->DeactivateHUD();
        }

    }

    if (GAMEINSTANCE->KeyDown(DIK_F9))
    {
        
        m_IsQuestUIOn = !m_IsQuestUIOn;

        if (m_IsQuestUIOn) {
            
            UIMGR->ActivateUI("QuestPanel");

        }
        else {

            UIMGR->DeactivateUI("QuestPanel");
        }
    }


    if (GAMEINSTANCE->KeyPressing(DIK_RSHIFT)) {
        if (GAMEINSTANCE->KeyDown(DIK_T))
        {
            GAMEINSTANCE->SwitchiongTargetGroup1Mode();
        }
        if (GAMEINSTANCE->KeyDown(DIK_R))
        {
            GAMEINSTANCE->SwitchingZoomBlurOff();
        }
        if (GAMEINSTANCE->KeyDown(DIK_E))
        {
            GAMEINSTANCE->SwtichingEffectOn();
        }
    }

    if (GAMEINSTANCE->KeyDown(DIK_F8)) {

        GAMEINSTANCE->SwitchingMotionBlur();
    }


}

void CMainApp::Free()
{

#ifdef _DEBUG
    /*콘솔 종료*/
    fclose(pFile);
    FreeConsole();
#endif
    CCharacterMgr::DestroyInstance();
    CEventMgr::DestroyInstance();
    CCameraMgr::DestroyInstance();
    CQuestMgr::DestroyInstance();
    CGameInstance::DestroyInstance();

}
