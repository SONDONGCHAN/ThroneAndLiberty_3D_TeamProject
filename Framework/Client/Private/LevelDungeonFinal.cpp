#include "LevelDungeonFinal.h"
#include "TestLogo.h"
#include "TestTerrain.h"

#include "LivingArmor_Soldier.h"
#include "LivingArmor_Archer.h"
#include "LivingArmor_Mage.h"
#include "Orc_Soldier.h"
#include "Orc_Archer.h"
#include "Orc_Shaman.h"
#include "Midget_Footman.h"
#include "Midget_Sling.h"
#include "Midget_Shaman.h"
#include "Midget_Berserker.h"
#include "Orc_DarkCommander.h"
#include "RedKnight.h"

#include "EffectAttachObj.h"

#include "NPC_Lukas.h"

#include "Weapon.h"
#include "Arrow.h"
#include "player.h"
#include "UIMgr.h"
#include "ParsingColliders.h"
#include "Wolf.h"
#include "Eagle.h"
#include "Polymorph.h"
#include "TestInstance.h"
#include "QuestMgr.h"

#include "Map.h"

#include "CameraSpherical.h"
#include "CameraFilm.h"
#include "CameraMgr.h"

#include "EffectMgr.h"
#include "EventMgr.h"
#include "CharacterMgr.h"

#include "Chest.h"

CLevelDungeonFinal::CLevelDungeonFinal()
{
}

CLevelDungeonFinal::~CLevelDungeonFinal()
{
    CCameraMgr::GetInstance()->ClearLevel();
}

HRESULT CLevelDungeonFinal::Initialize()
{

    GAMEINSTANCE->StopAll();

    if (FAILED(ReadyLight()))
        return E_FAIL;

    if (FAILED(ReadyLayerMap(TEXT("Layer_Map"))))
        return E_FAIL;

    if (FAILED(ReadyLayerPlayer(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(ReadyMonster(TEXT("Layer_Monster"))))
        return E_FAIL;

    if (FAILED(ReadyCollider()))
      return E_FAIL;
    
    if (FAILED(ReadyCamera()))
        return E_FAIL;

    if (FAILED(SetLevelEvent()))
        return E_FAIL;

   //ReadyLevelBGM();

    if (FAILED(EFFECTMGR->Initialize()))
        return E_FAIL;

    if (FAILED(ReadyEnvEffect(TEXT("Layer_LightEnv"))))
        return E_FAIL;

    UIMGR->UITickOn();


    return S_OK;
}

void CLevelDungeonFinal::Tick(_float _fTimeDelta)
{
    GAMEINSTANCE->SetSimultation(true);
}

void CLevelDungeonFinal::LateTick(_float _fTimeDelta)
{
}

HRESULT CLevelDungeonFinal::Render()
{

//#ifdef _DEBUG
//    SetWindowText(g_hWnd, TEXT("게임 플레이 레벨입니다."));
//#endif

    return S_OK;
}

void CLevelDungeonFinal::ReadyLevelBGM()
{
    /*Base*/
    m_EntranceSounds.emplace("Final_StartBGM", 0.5f);

    m_Phase1Sounds.emplace("Final_Phase1BGM", 0.5f);

    m_Phase2Sounds.emplace("Final_Phase2BGM", 0.5f);

    //m_MiniGameSounds.emplace("BGM_Dungeon_MiniGame", 0.8f);
    //m_MiniGameSounds.emplace("AMB_Dungeon_MiniGame", 1.1f);

    m_AmbientSounds = m_EntranceSounds;
    
    LevelBGMOn();
}


HRESULT CLevelDungeonFinal::ReadyLight()
{

    LIGHT_DESC		LightDesc = {};

    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDirection.Normalize();
    LightDesc.vDiffuse = _float4(0.2f, 0.2f, 0.25f, 1.f);
    LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;

    if (FAILED(GAMEINSTANCE->AddLight(LightDesc)))
        return E_FAIL;

    //Light_Dungeon_Final

    if (FAILED(ReadyLevelLight(TEXT("Light_Dungeon_Final"))))
        return E_FAIL;

    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vDiffuse = _float4(0.08f, 0.0f, 0.f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fCullDistance = 200.f;
    LightDesc.fRange = 30.f;
    LightDesc.vPosition = _float4(21.26f, -27.43f, 311.16f, 1.f);
    LightDesc.bUsingNoise = false;

    if (FAILED(GAMEINSTANCE->AddLight(LightDesc)))
        return E_FAIL;




    return S_OK;
}


HRESULT CLevelDungeonFinal::ReadyLayerMap(const wstring& strLayerTag)
{
    string strMapPath = "..\\..\\Client\\Bin\\DataFiles\\MapData\\Dungeon_Final9.dat";
    shared_ptr<CMap> pMap = CMap::Create(strMapPath, false);
    if (nullptr == pMap)
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Map"), pMap)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelDungeonFinal::ReadyMonster(const wstring& _strLayerTag)
{
    //// 보스
    shared_ptr<CRedKnight> pModelRedKnight = CRedKnight::Create(_float3(21.f, -27.f, 310.f));
    //
    //// 필드보스
    //shared_ptr<CMidget_Berserker> pModelMidgetBerserker = CMidget_Berserker::Create(_float3(20.f, -25.f, 235.f));
    //shared_ptr<COrc_DarkCommander> pModelDarkCommander = COrc_DarkCommander::Create(_float3(20.f, -25.f, 235.f));

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, _strLayerTag, pModelRedKnight)))
        return E_FAIL;
    //if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, _strLayerTag, pModelMidgetBerserker)))
    //    return E_FAIL;
    //if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, _strLayerTag, pModelDarkCommander)))
    //    return E_FAIL;
    
    return S_OK;
}

HRESULT CLevelDungeonFinal::ReadyEnvEffect(const wstring& _strLayerTag)
{
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(21.11f, -11.5f, 120.74f), "Env_Fire"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(25.22f, -9.7f, 101.89f), "Env_Fire"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(17.02f, -9.7f, 101.89f), "Env_Fire"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(9.2f, -27.99f, 296.73f), "Env_Fire"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(32.87f, -27.99f, 296.73f), "Env_Fire"))))
        return E_FAIL;


    return S_OK;
}

HRESULT CLevelDungeonFinal::ReadyCollider()
{
    shared_ptr<CParsingColliders> pColliders = CParsingColliders::Create("..\\..\\Tool_CC\\Bin\\DataFiles\\Collider\\ColliderDungeonB2.dat");

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, L"MapCollider", pColliders)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelDungeonFinal::ReadyCamera()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
	shared_ptr<CCameraMgr> pCameraMgr = CCameraMgr::GetInstance();

	CCamera::CAMERA_DESC pCamDesc{};
	pCamDesc.fFovy = XMConvertToRadians(60.0f);
	pCamDesc.fAspect = (float)g_iWinSizeX / (float)g_iWinSizeY;
	pCamDesc.fNear = 0.1f;
	pCamDesc.fFar = 1000.f;

	shared_ptr<CCameraFilm> FinalBossEncounterCamera = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossEncounter1.json");
	if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossEncounterCamera)))
		return E_FAIL;

	pCameraMgr->AddFilmCamera("FinalBossEncounter1", FinalBossEncounterCamera);


	FinalBossEncounterCamera = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossEncounter2.json");
	if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossEncounterCamera)))
		return E_FAIL;

	pCameraMgr->AddFilmCamera("FinalBossEncounter2", FinalBossEncounterCamera);


	FinalBossEncounterCamera = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossEncounter3.json");
	if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossEncounterCamera)))
		return E_FAIL;

	pCameraMgr->AddFilmCamera("FinalBossEncounter3", FinalBossEncounterCamera);

    shared_ptr<CCameraFilm> FinalBossPillarUp = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossPillarUp1.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossPillarUp)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossPillarUp1", FinalBossPillarUp);

    FinalBossPillarUp = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossPillarUp2.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossPillarUp)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossPillarUp2", FinalBossPillarUp);

    shared_ptr<CCameraFilm> FinalBossFury = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossFury1.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossFury)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossFury1", FinalBossFury);

    FinalBossFury = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossFury2.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossFury)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossFury2", FinalBossFury);

    FinalBossFury = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossFury3.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossFury)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossFury3", FinalBossFury);

    shared_ptr<CCameraFilm> FinalBossSpawnSword = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossSpawnSword1.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossSpawnSword)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossSpawnSword1", FinalBossSpawnSword);

    FinalBossSpawnSword = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossSpawnSword2.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossSpawnSword)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossSpawnSword2", FinalBossSpawnSword);

    FinalBossSpawnSword = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossSpawnSword3.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossSpawnSword)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossSpawnSword3", FinalBossSpawnSword);

    FinalBossSpawnSword = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossSpawnSword4.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossSpawnSword)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossSpawnSword4", FinalBossSpawnSword);

    shared_ptr<CCameraFilm> FinalBossFallingSword = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossFallingSword1.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossFallingSword)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossFallingSword1", FinalBossFallingSword);

    FinalBossFallingSword = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossFallingSword2.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossFallingSword)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossFallingSword2", FinalBossFallingSword);

    FinalBossFallingSword = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossFallingSword3.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossFallingSword)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossFallingSword3", FinalBossFallingSword);

    FinalBossFallingSword = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossFallingSword4.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossFallingSword)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossFallingSword4", FinalBossFallingSword);

    FinalBossFallingSword = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FinalBossFallingSword5.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_FINAL, L"Layer_Camera", FinalBossFallingSword)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("FinalBossFallingSword5", FinalBossFallingSword);
	return S_OK;
}

HRESULT CLevelDungeonFinal::ReadyLayerPlayer(const wstring& _strLayerTag)
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

        CHARACTERMGR->ReadyLight();
        pPlayer->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(21.3f, -9.4f, 86.1f));


    }
    else {

        CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(21.3f, -9.4f, 86.1f));
        CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->LookAtDir(_float3(0.f, 0.f, 1.f));
        CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->Tick(0.016f);
        CHARACTERMGR->ReadyLight();
        CHARACTERMGR->SetInDungeon(true);
        dynamic_pointer_cast<CPlayer>(pPlr)->SetInDungeon(true);
    }

    return S_OK;
}

HRESULT CLevelDungeonFinal::SetLevelEvent()
{   
    if (FAILED(CEventMgr::GetInstance()->Initialize()))
        return E_FAIL;

    CEventMgr::GetInstance()->CreateDungeonFinalEvent();
    
    return S_OK;
}

void CLevelDungeonFinal::SwitchingBGM()
{
    LevelBGMOff();

    /*등장 -> 페이즈1*/
    if (!m_bPhase1) {
        m_AmbientSounds = m_Phase1Sounds;
    }

    LevelBGMOn();
    m_bPhase1 = !m_bPhase1;
}

void CLevelDungeonFinal::SwitchingBGM2()
{
    LevelBGMOff();

    /*적막 -> 페이즈2*/
    if (!m_bPhase2) {
        m_AmbientSounds = m_Phase2Sounds;
    }

    LevelBGMOn();
    m_bPhase2 = !m_bPhase2;
}

void CLevelDungeonFinal::SwitchingBGM3()
{
}

shared_ptr<CLevelDungeonFinal> CLevelDungeonFinal::Create()
{
    shared_ptr<CLevelDungeonFinal> pInstance = make_shared<CLevelDungeonFinal>();

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CLevelDungeonFinal");

    return pInstance;
}
                 