#include "LevelLogo.h"
#include "TestLogo.h"
#include "UIBackGround.h"
#include "VIInstancePoint.h"
#include "UIMgr.h"

#include "LevelLoading.h"

CLevelLogo::CLevelLogo()
{
}

CLevelLogo::~CLevelLogo()
{
    UIMGR->DeleteSelectedUI("UI_Logo");
}

HRESULT CLevelLogo::Initialize()
{
    //GAMEINSTANCE->PlaySoundW("BGM_MainTheme", 1.f);

    if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    return S_OK;
}

void CLevelLogo::Tick(_float _fTimeDelta)
{
    if (GAMEINSTANCE->KeyDown(DIK_SPACE)) {
        
        //GAMEINSTANCE->OpenLevelForClient(LEVEL_DUNGEON_MAIN);
        //GAMEINSTANCE->OpenLevelForClient(LEVEL_FONOS);
        //GAMEINSTANCE->OpenLevelForClient(LEVEL_DUNGEON_FINAL);
        
        /*Dungeon Main*/
        /*GAMEINSTANCE->SetLoadingLevel(CLevelLoading::Create(LEVEL_DUNGEON_MAIN));
        GAMEINSTANCE->OpenLevelForClient(LEVEL_DUNGEON_MAIN);*/

        /*Village*/
        GAMEINSTANCE->SetLoadingLevel(CLevelLoading::Create(LEVEL_VILLAGE));
        GAMEINSTANCE->OpenLevelForClient(LEVEL_VILLAGE);

        // GAMEINSTANCE->OpenLevel2(LEVEL_VILLAGE, CLevelLoading::Create(LEVEL_VILLAGE));

    }
}

void CLevelLogo::LateTick(_float _fTimeDelta)
{
}

HRESULT CLevelLogo::Render()
{
    return S_OK;
}

HRESULT CLevelLogo::ReadyLayerBackGround(const wstring& strLayerTag)
{

   //shared_ptr<CTestLogo> pLogo = CTestLogo::Create();
   //
   //if(FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, strLayerTag, pLogo)))
   //    return E_FAIL;

    CUIBase::UIDesc uiDesc{};
    uiDesc.strTag = "UI_Logo";
    uiDesc.eUILayer = UI_POPUP_FULL;
    uiDesc.eUIType = UI_BACKGROUND;
    uiDesc.eUISlotType = SLOT_END;
    uiDesc.eUITexType = UI_TEX_D;
    uiDesc.fZOrder = 0.f;
    uiDesc.iShaderPassIdx = 0;
    uiDesc.isMasked = false;
    uiDesc.isNoised = false;
    uiDesc.iShaderPassIdx = 0;
    
    CVIInstancing::InstanceDesc instanceDesc{};
    instanceDesc.vSize = _float2(1280.f, 1280.f);
    instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
    instanceDesc.fSizeY = 720.f;

    vector<string> texs;
    texs.emplace_back("UI_Logo");
    shared_ptr<CUIBackGround> pLogo = CUIBackGround::Create(texs, "", "", uiDesc, instanceDesc);
    if (pLogo == nullptr)
        return E_FAIL;

    if (FAILED(UIMGR->AddUI("UI_Logo", pLogo)))
        return E_FAIL;

    UIMGR->ActivateUI("UI_Logo");

    return S_OK;
}

shared_ptr<CLevelLogo> CLevelLogo::Create()
{
    shared_ptr<CLevelLogo> pInstance = make_shared<CLevelLogo>();

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CLevelLogo");

    return pInstance;
}
