#include "LevelFonos.h"
#include "Map.h"
#include "ParsingColliders.h"

#include "EventMgr.h"
#include "UIMgr.h"

#include "Player.h"
#include "CharacterMgr.h"
#include "SkyDome.h"
#include "Eagle.h"

#include "CameraFilm.h"
#include "CameraMgr.h"

#include "FonosAmitoy.h"
#include "FonosTrapShock.h"
#include "FonosTrapSlow.h"
#include "FonosTrapMove.h"
#include "FonosSuperJump.h"

#include "EffectAttachObj.h"


CLevelFonos::CLevelFonos()
{
}

HRESULT CLevelFonos::Initialize()
{

    GAMEINSTANCE->StopAll();

    if (FAILED(ReadyLight()))
        return E_FAIL;

    if (FAILED(ReadyCamera()))
        return E_FAIL;

    if (FAILED(ReadyLayerMap(TEXT("Layer_Map"))))
        return E_FAIL;

    if (FAILED(ReadyLayerPlayer(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(EFFECTMGR->Initialize()))
        return E_FAIL;

    if (FAILED(ReadyEnvironmentObj(TEXT("Layer_Environment"))))
        return E_FAIL;

    if (FAILED(ReadyMiniGame()))
        return E_FAIL;

    if (FAILED(SetLevelEvent()))
        return E_FAIL;

    if (FAILED(ReadyCollider()))
        return E_FAIL;

   // ReadyLayerBGM();


    /*if (FAILED(TestAmitoy()))
        return E_FAIL;*/

    UIMGR->UITickOn();

    return S_OK;
}

void CLevelFonos::Tick(_float _fTimeDelta)
{
    GAMEINSTANCE->SetSimultation(true);
}

void CLevelFonos::LateTick(_float _fTimeDelta)
{
}

HRESULT CLevelFonos::Render()
{
    return S_OK;
}

HRESULT CLevelFonos::ReadyLight()
{
    LIGHT_DESC		LightDesc = {};

    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDirection.Normalize();
    LightDesc.vDiffuse = _float4(0.85f, 0.9f, 0.85f, 1.f);
    LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;

    if (FAILED(GAMEINSTANCE->AddLight(LightDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelFonos::ReadyCamera()
{
    shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
    shared_ptr<CCameraMgr> pCameraMgr = CCameraMgr::GetInstance();

    CCamera::CAMERA_DESC pCamDesc{};
    pCamDesc.fFovy = XMConvertToRadians(60.0f);
    pCamDesc.fAspect = (float)g_iWinSizeX / (float)g_iWinSizeY;
    pCamDesc.fNear = 0.1f;
    pCamDesc.fFar = 1000.f;

    shared_ptr<CCameraFilm> FonosMiniGameCamera = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FonosMiniGame1.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_FONOS, L"Layer_Camera", FonosMiniGameCamera)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FonosMiniGame1", FonosMiniGameCamera);

    FonosMiniGameCamera = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FonosMiniGame2.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_FONOS, L"Layer_Camera", FonosMiniGameCamera)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FonosMiniGame2", FonosMiniGameCamera);

    FonosMiniGameCamera = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FonosMiniGameCeremony1.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_FONOS, L"Layer_Camera", FonosMiniGameCamera)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FonosMiniGameCeremony1", FonosMiniGameCamera);

    FonosMiniGameCamera = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FonosMiniGameCeremony2.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_FONOS, L"Layer_Camera", FonosMiniGameCamera)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FonosMiniGameCeremony2", FonosMiniGameCamera);

    shared_ptr<CCameraFilm> FieldBossOrc = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FieldBossOrc1.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_FONOS, L"Layer_Camera", FieldBossOrc)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FieldBossOrc1", FieldBossOrc);

    FieldBossOrc = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FieldBossOrc2.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_FONOS, L"Layer_Camera", FieldBossOrc)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FieldBossOrc2", FieldBossOrc);

    return S_OK;
}

HRESULT CLevelFonos::ReadyLayerMap(const wstring& strLayerTag)
{
    string strMapPath = "..\\..\\Client\\Bin\\DataFiles\\MapData\\Fonos.dat";
    shared_ptr<CMap> pMap = CMap::Create(strMapPath, true);
    if (nullptr == pMap)
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_Map"), pMap)))
        return E_FAIL;

    shared_ptr<CSkyDome> pSky = CSkyDome::Create();

    if (!pSky)
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_Sky"), pSky)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelFonos::ReadyLayerPlayer(const wstring& _strLayerTag)
{
    shared_ptr<CGameObject> pPlr = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));

    if (!pPlr) {

        shared_ptr<CPlayer> pPlayer = CPlayer::Create();

        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, _strLayerTag, pPlayer)))
            return E_FAIL;

        /*Polymorph*/

        CHARACTERMGR->AddPolymorph("Form_Human", static_pointer_cast<CPolymorph>(pPlayer));
        CHARACTERMGR->SetCurrentPolymorph("Form_Human");
        pPlayer->SetInDungeon(true);
        pPlayer->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(81.05f, 5.f, 9.f));

        shared_ptr<CWolf> pWolf = CWolf::Create();

        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, _strLayerTag, pWolf)))
            return E_FAIL;

        pWolf->SetEnable(false);

        CHARACTERMGR->AddPolymorph("Form_Wolf", static_pointer_cast<CWolf>(pWolf));


        shared_ptr<CEagle> pEagle = CEagle::Create();

        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, _strLayerTag, pEagle)))
            return E_FAIL;

        pEagle->SetEnable(false);

        CHARACTERMGR->AddPolymorph("Form_Eagle", static_pointer_cast<CEagle>(pEagle));


    }
    else {

        CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(80.63f, 1.24f, 5.58f));
        CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->LookAtForLandObject(_float3(81.26, 1.67, 10.50));
        CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->Tick(0.016f);
        CHARACTERMGR->SetInDungeon(false);
        dynamic_pointer_cast<CPlayer>(pPlr)->SetInDungeon(false);

    }

    return S_OK;
}

HRESULT CLevelFonos::ReadyCollider()
{
    shared_ptr<CParsingColliders> pColliders = CParsingColliders::Create("..\\..\\Tool_CC\\Bin\\DataFiles\\Collider\\ColliderFonos.dat");

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, L"MapCollider", pColliders)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelFonos::ReadyMiniGame()
{
    shared_ptr<CGameObject> pPlr = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
    //pPlr->SetEnable(false);

    shared_ptr<CFonosAmitoy> pAmitoy = CFonosAmitoy::Create();

    if (pAmitoy)
    {
        pAmitoy->GetTransform()->SetState(CTransform::STATE_POSITION, { 54.76f, 15.f, 112.22f });
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Layer_Amitoy", pAmitoy);
        pAmitoy->SetEnable(true);
        //CCameraMgr::GetInstance()->CameraTurnOn("FonosAmitoyCamera");
    }

    shared_ptr<CFonosTrap> pFonosTrap;

    // Trap Slow
    {
        pFonosTrap = CFonosTrapSlow::Create(CFonosTrap::TRAP_SLOW, _float3(31.f, 16.48f, 129.65f), _float3(0.3f, 0.f, 3.f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

        pFonosTrap = CFonosTrapSlow::Create(CFonosTrap::TRAP_SLOW, _float3(16.28f, 16.48f, 147.47f), _float3(1.f, 0.f, 0.f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

        pFonosTrap = CFonosTrapSlow::Create(CFonosTrap::TRAP_SLOW, _float3(27.f, 16.48f, 160.39f), _float3(0.f, 0.f, 1.f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

        pFonosTrap = CFonosTrapSlow::Create(CFonosTrap::TRAP_SLOW, _float3(28.8f, 16.48f, 157.41f), _float3(1.f, 0.f, 1.f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

        pFonosTrap = CFonosTrapSlow::Create(CFonosTrap::TRAP_SLOW, _float3(37.09f, 16.48f, 161.93f), _float3(0.3f, 0.f, 3.f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

        pFonosTrap = CFonosTrapSlow::Create(CFonosTrap::TRAP_SLOW, _float3(55.44f, 16.48f, 138.74f), _float3(1.f, 0.f, 0.f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

        pFonosTrap = CFonosTrapSlow::Create(CFonosTrap::TRAP_SLOW, _float3(44.23f, 16.48f, 139.16f), _float3(0.f, 0.f, 1.f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

        pFonosTrap = CFonosTrapSlow::Create(CFonosTrap::TRAP_SLOW, _float3(41.62f, 16.48f, 140.25f), _float3(1.f, 0.f, 1.f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
    }

    // Trap Shock
    {
        pFonosTrap = CFonosTrapShock::Create(CFonosTrap::TRAP_SHOCK, _float3(24.f, 16.5f, 138.f), _float3(26.02f, 17.179f, 139.67f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

        pFonosTrap = CFonosTrapShock::Create(CFonosTrap::TRAP_SHOCK, _float3(19.8f, 16.48f, 154.f), _float3(15.85f, 18.09f, 157.79f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

        pFonosTrap = CFonosTrapShock::Create(CFonosTrap::TRAP_SHOCK, _float3(33.91f, 16.48f, 161.37f), _float3(32.96f, 0.f, 163.68f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

        pFonosTrap = CFonosTrapShock::Create(CFonosTrap::TRAP_SHOCK, _float3(65.37f, 16.48f, 144.12f), _float3(15.85f, 18.09f, 157.79f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

        pFonosTrap = CFonosTrapShock::Create(CFonosTrap::TRAP_SHOCK, _float3(45.73f, 22.38f, 156.04f), _float3(43.72f, 22.53f, 154.81f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
    }

    // Trap Move
    {
        pFonosTrap = CFonosTrapMove::Create(CFonosTrap::TRAP_MOVE, _float3(23.77f, 16.62f, 150.f), _float3(54.76f, 15.f, 112.22f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
        dynamic_pointer_cast<CFonosTrapMove>(pFonosTrap)->SetMoveType(CFonosTrapMove::MOVE_UD, true, 2.f);

        pFonosTrap = CFonosTrapMove::Create(CFonosTrap::TRAP_MOVE, _float3(63.37f, 22.84f, 133.26f), _float3(54.76f, 15.f, 112.22f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
        dynamic_pointer_cast<CFonosTrapMove>(pFonosTrap)->SetMoveType(CFonosTrapMove::MOVE_UD, true, 2.f);

        pFonosTrap = CFonosTrapMove::Create(CFonosTrap::TRAP_MOVE, _float3(71.77f, 16.62f, 146.92f), _float3(54.27f, 14.42f, 111.55f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
        dynamic_pointer_cast<CFonosTrapMove>(pFonosTrap)->SetMoveType(CFonosTrapMove::MOVE_UD, true, 2.f);

        pFonosTrap = CFonosTrapMove::Create(CFonosTrap::TRAP_MOVE, _float3(70.25f, 19.62f, 148.16f), _float3(54.76f, 15.f, 112.22f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
        dynamic_pointer_cast<CFonosTrapMove>(pFonosTrap)->SetMoveType(CFonosTrapMove::MOVE_UD, false, 2.f);

        pFonosTrap = CFonosTrapMove::Create(CFonosTrap::TRAP_MOVE, _float3(70.78f, 16.62f, 147.40f), _float3(54.76f, 15.f, 112.22f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
        dynamic_pointer_cast<CFonosTrapMove>(pFonosTrap)->SetMoveType(CFonosTrapMove::MOVE_UD, true, 2.5f);

        pFonosTrap = CFonosTrapMove::Create(CFonosTrap::TRAP_MOVE, _float3(50.15f, 22.41f, 147.72f), _float3(49.17f, 22.53f, 148.78f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
        dynamic_pointer_cast<CFonosTrapMove>(pFonosTrap)->SetMoveType(CFonosTrapMove::MOVE_LR, true, 3.f);

        pFonosTrap = CFonosTrapMove::Create(CFonosTrap::TRAP_MOVE, _float3(53.91f, 22.6f, 141.26f), _float3(52.83f, 22.41f, 142.87f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
        dynamic_pointer_cast<CFonosTrapMove>(pFonosTrap)->SetMoveType(CFonosTrapMove::MOVE_LR, true, 3.f);

        pFonosTrap = CFonosTrapMove::Create(CFonosTrap::TRAP_MOVE, _float3(56.17f, 16.62f, 140.86f), _float3(54.76f, 15.f, 112.22f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
        dynamic_pointer_cast<CFonosTrapMove>(pFonosTrap)->SetMoveType(CFonosTrapMove::MOVE_LR, false, 4.f);
    }

    // Trap SuperJump
    {
        pFonosTrap = CFonosSuperJump::Create(CFonosTrap::TRAP_END, _float3(41.80f, 15.9f, 161.27f), _float3(-1.f, 0.f, 0.1f));
        GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
    }

    return S_OK;
}

HRESULT CLevelFonos::ReadyEnvironmentObj(const wstring& _strLayerTag)
{
    // Env_Smoke_Up
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(80.6f, 3.36f, 33.54f), "Env_Smoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(59.14f, 16.4f, 28.31f), "Env_Smoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(55.35f, 15.11f, 42.46f), "Env_Smoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(10.23f, 7.97f, 14.08f), "Env_Smoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(54.71f, 10.05f, 57.44f), "Env_Smoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(17.87f, 16.48f, 161.95f), "Env_Smoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(104.85f, 17.40f, 146.94f), "Env_Smoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(130.44f, 7.32f, 73.80f), "Env_Smoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(131.94f, 11.60f, 123.83f), "Env_Smoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(124.97f, 10.07f, 110.37f), "Env_Smoke_Up"))))
        return E_FAIL;
    
    // Env_LightSmoke_Up
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(69.21f, 9.f, 28.17f), "Env_LightSmoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(60.82f, 9.16f, 7.52f), "Env_LightSmoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(57.39f, 9.61f, 22.44f), "Env_LightSmoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(6.79f, 10.55f, 38.71f), "Env_LightSmoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(106.91f, 13.43f, 112.06f), "Env_LightSmoke_Up"))))
        return E_FAIL;

    // Env_FireSmoke_Up
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(76.63f, 13.55f, 33.65f), "Env_FireSmoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(65.5f, 30.52f, 46.14f), "Env_FireSmoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(10.85f, 12.19f, 18.83f), "Env_FireSmoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(66.09f, 7.87f, 23.49f), "Env_FireSmoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(69.79f, 7.72f, 15.99f), "Env_FireSmoke_Up"))))
        return E_FAIL;
    /*if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(3.41f, 17.f, 89.72f), "Env_FireSmoke_Up"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(42.53f, 19.48f, 77.66f), "Env_FireSmoke_Up"))))
        return E_FAIL;*/
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, _strLayerTag, CEffectAttachObj::Create(_float3(115.80f, 9.0f, 88.39f), "Env_FireSmoke_Up"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelFonos::SetLevelEvent()
{
    if (FAILED(CEventMgr::GetInstance()->Initialize()))
        return E_FAIL;

    CEventMgr::GetInstance()->CreateField2Event();


    return S_OK;
}

void CLevelFonos::ReadyLayerBGM()
{
    m_FonosSounds.emplace("BGM_Fonos", 1.4f);
    m_FonosSounds.emplace("AMB_Fonos_Fire", 1.f);

    m_BossSounds.emplace("BGM_DarkCommander", 0.5f);
    //m_BossSounds.emplace("AMB_Orc_Boss", 1.1f);

    m_MiniGameSounds.emplace("FA_BGM_2", 0.5f);
    //m_MiniGameSounds.emplace("AMB_Fonos_MiniGame", 1.1f);

    m_AmbientSounds = m_FonosSounds;

    LevelBGMOn();
}

void CLevelFonos::SwitchingBGM()
{
    LevelBGMOff();

    /*보스 -> 필드*/
    if (m_IsBoss) {
        m_AmbientSounds = m_FonosSounds;
    }
    /*필드 -> 보스*/
    else {
        m_AmbientSounds = m_BossSounds;
    }

    LevelBGMOn();
    m_IsBoss = !m_IsBoss;
}

void CLevelFonos::SwitchingMiniGameBGM()
{
    LevelBGMOff();

    /*미니게임 -> 필드*/
    if (m_IsMiniGame) {
        m_AmbientSounds = m_FonosSounds;
    }
    /*필드 -> 미니게임*/
    else {
        m_AmbientSounds = m_MiniGameSounds;
    }

    LevelBGMOn();
    m_IsMiniGame = !m_IsMiniGame;
}

HRESULT CLevelFonos::TestAmitoy()
{
    //shared_ptr<CGameObject> pPlr = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
    //pPlr->SetEnable(false);

    //shared_ptr<CFonosAmitoy> pAmitoy = CFonosAmitoy::Create();

    //if (pAmitoy)
    //{
    //    pAmitoy->GetTransform()->SetState(CTransform::STATE_POSITION, { 55.7, 15.37, 104.62 });
    //    GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Layer_Amitoy", pAmitoy);
    //    CCameraMgr::GetInstance()->CameraTurnOn("FonosAmitoyCamera");
    //}

    //shared_ptr<CFonosTrap> pFonosTrap;
    //
    //// Trap Slow
    //{
    //    pFonosTrap = CFonosTrapSlow::Create(CFonosTrap::TRAP_SLOW, _float3(31.f, 16.48f, 129.65f), _float3(0.f, 0.f, 1.f));
    //    GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
    //    
    //    pFonosTrap = CFonosTrapSlow::Create(CFonosTrap::TRAP_SLOW, _float3(16.28f, 16.48f, 147.47f), _float3(0.f, 0.f, 1.f));
    //    GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

    //    pFonosTrap = CFonosTrapSlow::Create(CFonosTrap::TRAP_SLOW, _float3(27.f, 16.48f, 160.39f), _float3(0.f, 0.f, 1.f));
    //    GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

    //    pFonosTrap = CFonosTrapSlow::Create(CFonosTrap::TRAP_SLOW, _float3(28.8f, 16.48f, 157.41f), _float3(0.f, 0.f, 1.f));
    //    GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
    //}
    //
    //// Trap Shock
    //{
    //    pFonosTrap = CFonosTrapShock::Create(CFonosTrap::TRAP_SHOCK, _float3(24.f, 16.5f, 138.f), _float3(1.f, 0.f, 0.f));
    //    GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

    //    pFonosTrap = CFonosTrapShock::Create(CFonosTrap::TRAP_SHOCK, _float3(19.8f, 16.48f, 154.f), _float3(1.f, 0.f, 0.f));
    //    GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);

    //    pFonosTrap = CFonosTrapShock::Create(CFonosTrap::TRAP_SHOCK, _float3(33.91f, 16.48f, 161.37f), _float3(1.f, 0.f, 0.f));
    //    GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
    //}

    //// Trap SuperJump
    //{
    //    pFonosTrap = CFonosSuperJump::Create(CFonosTrap::TRAP_END, _float3(41.80f, 16.48f, 161.27f), _float3(0.f, 0.f, 1.f));
    //    GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Fonos_MiniGame", pFonosTrap);
    //}

    return S_OK;
}

shared_ptr<CLevelFonos> CLevelFonos::Create()
{
    shared_ptr<CLevelFonos> pLevel = make_shared<CLevelFonos>();

    if (FAILED(pLevel->Initialize()))
        MSG_BOX("Failed to Create : CLevelFonos");

    return pLevel;
}
