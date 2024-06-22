#include "LevelDungeonMain.h"
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

#include "Amitoy.h"
#include "MNGObject.h"
#include "MNGQuestItem.h"
#include "TrapController.h"

#include "NPC_Henry.h"
#include "NPC_Lottie.h"
#include "NPC_JaniceCarter.h"
#include "NPC_Lukas.h"
#include "NPC_Davinci.h"
#include "NPC_Robert.h"
#include "NPC_LeonardA.h"
#include "NPC_RuffaloRice.h"
#include "NPC_Elleia.h"
#include "NPC_David.h"
#include "NPC_RobertGhost.h"
#include "NPC_Ashien.h"

#include "NPC_MonsterDavid.h"

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
#include "Corpse.h"
#include "Statue.h"
#include "DungeonGate.h"
#include "EffectAttachObj.h"

#include "Puppet.h"

#include "EnterStatue.h"

CLevelDungeonMain::CLevelDungeonMain()
{
}

CLevelDungeonMain::~CLevelDungeonMain()
{
    CCameraMgr::GetInstance()->ClearLevel();
}

HRESULT CLevelDungeonMain::Initialize()
{

    GAMEINSTANCE->StopAll();


    if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(ReadyLayerMap(TEXT("Layer_Map"))))
        return E_FAIL;

    if (FAILED(ReadyLayerPlayer(TEXT("Layer_Player"))))
        return E_FAIL;

    //if (FAILED(ReadyPracticeRoom(TEXT("Layer_PracticeRoom"))))
    //    return E_FAIL;
    
    if (FAILED(ReadyMonster(TEXT("Layer_Monster"))))
        return E_FAIL;

    if (FAILED(ReadyInteractObj(TEXT("Layer_InteractionObj"))))
        return E_FAIL;

    if (FAILED(ReadyCollider()))
        return E_FAIL;

    if (FAILED(SetLevelEvent()))
        return E_FAIL;

    if (FAILED(TestTrapController()))
        return E_FAIL;

    if (FAILED(ReadyCamera(L"Layer_Camera")))
        return E_FAIL;

    /*if (FAILED(TestAmitoy()))
        return E_FAIL;

    if (FAILED(TestMNGObject()))
        return E_FAIL;*/

    if (FAILED(EFFECTMGR->Initialize()))
        return E_FAIL;

    if (FAILED(ReadyLight()))
        return E_FAIL;

    if(FAILED(ReadyLevelLight(TEXT("Light_Dungeon"))))
        return E_FAIL;

    if (FAILED(ReadyCollider()))
        return E_FAIL;

   // ReadyLevelBGM();

    UIMGR->UITickOn();


    return S_OK;
}

void CLevelDungeonMain::Tick(_float _fTimeDelta)
{
    GAMEINSTANCE->SetSimultation(true);

    KeyInput();
}

void CLevelDungeonMain::LateTick(_float _fTimeDelta)
{
}

HRESULT CLevelDungeonMain::Render()
{

    //#ifdef _DEBUG
    //    SetWindowText(g_hWnd, TEXT("게임 플레이 레벨입니다."));
    //#endif

    return S_OK;
}

void CLevelDungeonMain::ReadyLevelBGM()
{
    /*Base*/
    m_DungeonMainSounds.emplace("BGM_DungeonAmbient_Base", 1.f);
    m_DungeonMainSounds.emplace("AMB_02_02_Dungeon_P_Fenrir_SFX_01", 1.f);
    m_DungeonMainSounds.emplace("BGM_RuinDungeon", 0.5f);

    m_BossSounds.emplace("BGM_LivingArmorMage", 0.5f);

    //m_MiniGameSounds.emplace("BGM_Dungeon_MiniGame", 0.8f);
    //m_MiniGameSounds.emplace("AMB_Dungeon_MiniGame", 1.1f);

    m_AmbientSounds = m_DungeonMainSounds;

    LevelBGMOn();

}

void CLevelDungeonMain::KeyInput()
{
    /*Fonos*/
    //if (GAMEINSTANCE->KeyDown(DIK_8)) {

    //    GAMEINSTANCE->SetChangeLevelSignal(LEVEL_FONOS);
    //}

}

HRESULT CLevelDungeonMain::ReadyLayerBackGround(const wstring& strLayerTag)
{

    return S_OK;
}


HRESULT CLevelDungeonMain::ReadyLight()
{

    LIGHT_DESC		LightDesc = {};

    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDirection.Normalize();
    LightDesc.vDiffuse = _float4(0.1f, 0.1f, 0.15f, 1.f);
    LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;

    if (FAILED(GAMEINSTANCE->AddLight(LightDesc)))
        return E_FAIL;

    /*힐링방*/
    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.bUsingNoise = true;
    LightDesc.vPosition = _float4(-90.f, 0.f, 104.f, 1.f);
    LightDesc.fCullDistance = 100.f;
    LightDesc.fRange = 35.f;
    LightDesc.vDiffuse = _float4(0.f, 1.f, 0.735f, 1.f);
    LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;

    if (FAILED(GAMEINSTANCE->AddLight(LightDesc)))
        return E_FAIL;

    if (FAILED(ReadyLevelLight(TEXT("Light_Dungeon"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-16.8f, 3.4f, 15.f), "Env_Fire"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(95.5f, 3.4f, 85.f), "Env_Fire"))))
        return E_FAIL;
    //if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(58.f, 3.4f, 85.f), "Env_Fire"))))
    //    return E_FAIL;


    /*벽 랜턴*/
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(17.f, 8.8f, 131.5f), "Env_Fire"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(25.5f, 8.8f, 131.5f), "Env_Fire"))))
        return E_FAIL;

        /*위치조정 완료*/

    //if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(113.86f,13.73f,86.8f), "Env_Fire2"))))
    // return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(113.77f,14.f, 120.8f), "Env_Fire2"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(113.77f, 14.f, 86.62f), "Env_Fire2"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(153.f,13.86f,120.74f), "Env_Fire2"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(152.8f,14.f,86.62f), "Env_Fire2"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(152.6f,14.f,49.84f), "Env_Fire2"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(153.1f,14.f,15.47f), "Env_Fire2"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(113.7f,14.f,15.5f), "Env_Fire2"))))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(113.7f,14.f,49.7f), "Env_Fire2"))))
        return E_FAIL;

    //
    {
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(95.57f, 3.43f, 14.74f), "Env_Fire"))))
            return E_FAIL;
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(58.7f, 3.54f, 14.64f), "Env_Fire"))))
            return E_FAIL;
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(58.65f, 3.51f, 51.55f), "Env_Fire"))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(95.37f, 3.6f, 52.f), "Env_Fire"))))
            return E_FAIL;
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-16.9f, 3.6f, 51.8f), "Env_Fire"))))
            return E_FAIL;
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-53.6f, 3.4f, 52.13f), "Env_Fire"))))
            return E_FAIL;
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-53.5f, 3.4f, 15.f), "Env_Fire"))))
            return E_FAIL;
    }

    {
        /*방 하나 */
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-71.5f, 14.f, 15.48f), "Env_Fire2"))))
            return E_FAIL;
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-71.5f, 14.f, 49.7f), "Env_Fire2"))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-110.f, 14.f, 15.54f), "Env_Fire2"))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-110.f, 14.f, 49.39f), "Env_Fire2"))))
            return E_FAIL;
        /**/
    }

    /*수치재서 만든 방*/
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-109.26f,13.81f,86.18f), "Env_Fire2"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-109.26f, 13.81f, 120.21f), "Env_Fire2"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-70.2f, 13.81f, 120.21f), "Env_Fire2"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-70.2f, 13.81f, 86.21f), "Env_Fire2"))))
        return E_FAIL;
    /**/

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-119.54f,13.94f,120.51f), "Env_Fire2"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(95.38f, 3.4f, 122.16f), "Env_Fire"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-15.7f, 3.4f, 84.98f), "Env_Fire"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-52.46f,3.4f,122.4f), "Env_Fire2"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-16.f,3.4f,122.f), "Env_Fire2"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(58.7f,3.19f,122.28f), "Env_Fire"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(58.7f, 3.16f, 85.32f), "Env_Fire"))))
        return E_FAIL;
   
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(58.73f, 3.22f, 122.51f), "Env_Fire"))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(-52.69f, 3.4f, 84.91f), "Env_Fire"))))
        return E_FAIL;


    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_Effect"), CEffectAttachObj::Create(_float3(-90.f, 0.f, 104.f), "Heal_Room"))))
        return E_FAIL;


    return S_OK;
}

HRESULT CLevelDungeonMain::ReadyCamera(const wstring& strLayerTag)
{
    shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
    shared_ptr<CCameraMgr> pCameraMgr = CCameraMgr::GetInstance();

    CCamera::CAMERA_DESC pCamDesc{};
    pCamDesc.fFovy = XMConvertToRadians(60.0f);
    pCamDesc.fAspect = (float)g_iWinSizeX / (float)g_iWinSizeY;
    pCamDesc.fNear = 0.1f;
    pCamDesc.fFar = 1000.f;

    shared_ptr<CCameraFilm> TrapDoorShake = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/TrapDoorShake.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_MAIN, strLayerTag, TrapDoorShake)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("TrapDoorShake", TrapDoorShake);


    shared_ptr<CCameraFilm> TrapDoorOpen = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/TrapDoorOpen.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_MAIN, strLayerTag, TrapDoorOpen)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("TrapDoorOpen", TrapDoorOpen);


    shared_ptr<CCameraFilm> TrapRoom = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/TrapRoom.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_DUNGEON_MAIN, strLayerTag, TrapRoom)))
        return E_FAIL;

    pCameraMgr->AddFilmCamera("TrapRoom", TrapRoom);

    return S_OK;
}

HRESULT CLevelDungeonMain::ReadyLayerMap(const wstring& strLayerTag)
{
    //Dungeon_Main_Obj
    string strMapPath = "..\\..\\Client\\Bin\\DataFiles\\MapData\\Dungeon_Main_Test7.dat";
    shared_ptr<CMap> pMap = CMap::Create(strMapPath, false);
    if (nullptr == pMap)
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_Map"), pMap)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelDungeonMain::ReadyMonster(const wstring& _strLayerTag)
{
    return S_OK;
}

HRESULT CLevelDungeonMain::ReadyInteractObj(const wstring& _strLayerTag)
{
    // 상자
    shared_ptr<CChest> pChest = CChest::Create(_float3(87.7f, 0.f, 23.9f), 1);
    GAMEINSTANCE->AddStaticObject(pChest, _float3(87.7f, 0.f, 23.9f), 3.f);

    pChest = CChest::Create(_float3(-24.6f, 2.15f, 102.12f), 2);
    GAMEINSTANCE->AddStaticObject(pChest, _float3(-24.6f, 2.15f, 102.12f), 3.f);

    pChest = CChest::Create(_float3(-21.6f, 0.f, 42.6f), 3);
    GAMEINSTANCE->AddStaticObject(pChest, _float3(-21.6f, 0.f, 42.6f), 3.f);


    // 시체
    shared_ptr<CCorpse> pCorpse = CCorpse::Create(_float3(-90.5f, 0.1f, 42.f));
    GAMEINSTANCE->AddStaticObject(pCorpse, _float3(-90.5f, 0.1f, 42.f), 2.f);

    // 문지기 석상
    shared_ptr<CStatue> pStatue1 = CStatue::Create(_float3(14.f, 0.f, 60.f), 0);
    GAMEINSTANCE->AddStaticObject(pStatue1, _float3(14.f, 0.f, 60.f), 10.f);
    shared_ptr<CStatue> pStatue2 = CStatue::Create(_float3(14.f, 0.f, 76.f), 1);
    GAMEINSTANCE->AddStaticObject(pStatue2, _float3(14.f, 0.f, 76.f), 10.f);
    shared_ptr<CStatue> pStatue3 = CStatue::Create(_float3(27.f, 0.f, 60.f), 2);
    GAMEINSTANCE->AddStaticObject(pStatue3, _float3(27.f, 0.f, 60.f), 10.f);
    shared_ptr<CStatue> pStatue4 = CStatue::Create(_float3(27.f, 0.f, 76.f), 3);
    GAMEINSTANCE->AddStaticObject(pStatue4, _float3(27.f, 0.f, 76.f), 10.f);
    // 던전에 배치된 석상
    shared_ptr<CStatue> pNonRotStatue1 = CStatue::Create(_float3(-90.f, 0.f, 103.f), 0, 1);
    GAMEINSTANCE->AddStaticObject(pNonRotStatue1, _float3(-90.f, 0.f, 103.f), 10.f);
    shared_ptr<CStatue> pNonRotStatue2 = CStatue::Create(_float3(133.5f, 0.f, 104.f), 1, 1);
    GAMEINSTANCE->AddStaticObject(pNonRotStatue2, _float3(133.5f, 0.f, 104.f), 10.f);
    shared_ptr<CStatue> pNonRotStatue3 = CStatue::Create(_float3(-91.f, 0.f, 32.f), 2, 1);
    GAMEINSTANCE->AddStaticObject(pNonRotStatue3, _float3(-91.f, 0.f, 32.f), 10.f);
    shared_ptr<CStatue> pNonRotStatue4 = CStatue::Create(_float3(133.5f, 0.f, 33.f), 3, 1);
    GAMEINSTANCE->AddStaticObject(pNonRotStatue4, _float3(133.5f, 0.f, 33.f), 10.f);
    // 매치해주기
    pStatue1->SetMatchStatue(pNonRotStatue1);
    pStatue2->SetMatchStatue(pNonRotStatue2);
    pStatue3->SetMatchStatue(pNonRotStatue3);
    pStatue4->SetMatchStatue(pNonRotStatue4);

    // 던전문
    shared_ptr<CDungeonGate> pDungeonGate = CDungeonGate::Create(_float3(20.8f, 0.f, 67.5f));
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, _strLayerTag, pDungeonGate)))
        return E_FAIL;
    pDungeonGate->SetMatchStatue1(pStatue1);
    pDungeonGate->SetMatchStatue2(pStatue2);
    pDungeonGate->SetMatchStatue3(pStatue3);
    pDungeonGate->SetMatchStatue4(pStatue4);

    // NPC
    shared_ptr<CNPC_Lukas> pNPC_Lukas_Dungeon = CNPC_Lukas::Create(_float3(-94.7f, 0.05f, 6.4f), 1, _float3(-94.7f, 0.05f, 9.f));
    shared_ptr<CNPC_RobertGhost> pNPC_RobertGhost = CNPC_RobertGhost::Create(_float3(21.f, 1.6f, 29.f), _float3(21.f, 0.1f, 25.2f));
    
    GAMEINSTANCE->AddStaticObject(pNPC_Lukas_Dungeon, { -94.7f, 0.05f, 6.4f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pNPC_RobertGhost, { 21.f, 1.6f, 29.f }, 2.0f);

    QUESTMGR->AddNPC(pNPC_Lukas_Dungeon->GetModelName(), pNPC_Lukas_Dungeon);
    QUESTMGR->AddNPC(pNPC_RobertGhost->GetModelName(), pNPC_RobertGhost);

    DIALOGUEMGR->AddNPC(pNPC_Lukas_Dungeon->GetDisplayName(), pNPC_Lukas_Dungeon);
    DIALOGUEMGR->AddNPC(pNPC_RobertGhost->GetDisplayName(), pNPC_RobertGhost);

 
    return S_OK;
}

HRESULT CLevelDungeonMain::ReadyPracticeRoom(const wstring& _strLayerTag)
{
    shared_ptr<CPuppet::Puppet_Desc> pPuppetDesc = make_shared<CPuppet::Puppet_Desc>();
    pPuppetDesc->vMyPos = _float3(9.99813366f, 0.0f, 20.0656071f);

    shared_ptr<CPuppet> pPuppet = CPuppet::Create(pPuppetDesc);
    GAMEINSTANCE->AddStaticObject(pPuppet, _float3(9.99813366f, 0.0f, 20.0656071f));

  

    return S_OK;
}

HRESULT CLevelDungeonMain::ReadyLayerPlayer(const wstring& strLayerTag)
{
    shared_ptr<CGameObject> pPlr = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));

    if (!pPlr) {

        shared_ptr<CPlayer> pPlayer = CPlayer::Create();

        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, strLayerTag, pPlayer)))
            return E_FAIL;

        /*Polymorph*/
        
        CHARACTERMGR->AddPolymorph("Form_Human", static_pointer_cast<CPolymorph>(pPlayer));
        CHARACTERMGR->SetCurrentPolymorph("Form_Human");

        shared_ptr<CWolf> pWolf = CWolf::Create();

        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, strLayerTag, pWolf)))
            return E_FAIL;

        pWolf->SetEnable(false);

        CHARACTERMGR->AddPolymorph("Form_Wolf", static_pointer_cast<CWolf>(pWolf));


        shared_ptr<CEagle> pEagle = CEagle::Create();

        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, strLayerTag, pEagle)))
            return E_FAIL;

        pEagle->SetEnable(false);
        CHARACTERMGR->AddPolymorph("Form_Eagle", static_pointer_cast<CEagle>(pEagle));
        pPlayer->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(21.14f, 0.13f, 5.63f));


        CHARACTERMGR->ReadyLight();
        pPlayer->SetInDungeon(true);
        CHARACTERMGR->SetInDungeon(true);

    }
    else {

        CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(21.14f, 0.13f, 5.63f));
        CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->LookAtDir(_float3(0.f, 0.f, 1.f));
        CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->Tick(0.016f);
        CHARACTERMGR->ReadyLight();
        CHARACTERMGR->SetInDungeon(true); 
        dynamic_pointer_cast<CPlayer>(pPlr)->SetInDungeon(true);
    }

    return S_OK;
}


HRESULT CLevelDungeonMain::ReadyCollider()
{
    shared_ptr<CParsingColliders> pColliders = CParsingColliders::Create("..\\..\\Tool_CC\\Bin\\DataFiles\\Collider\\ColliderDungeonB1.dat");

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, L"MapCollider", pColliders)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelDungeonMain::SetLevelEvent()
{
    if (FAILED(CEventMgr::GetInstance()->Initialize()))
        return E_FAIL;

    CEventMgr::GetInstance()->CreateDungeonMainEvent();


    return S_OK;
}

HRESULT CLevelDungeonMain::TestAmitoy()
{
    shared_ptr<CAmitoy> pAmitoy = CAmitoy::Create();
   
    if (pAmitoy)
    {
        pAmitoy->GetTransform()->SetState(CTransform::STATE_POSITION, { 10.0f,10.0f,10.0f });
        GAMEINSTANCE->AddObject(LEVEL_STATIC, L"TestAmitoy", pAmitoy);
        CCameraMgr::GetInstance()->CameraTurnOn("AmitoyCamera");
    }


    return S_OK;
}

HRESULT CLevelDungeonMain::TestMNGObject()
{
    _float4x4 WorldMat = XMMatrixTranslation(15.0f, 10.0f, 15.0f);

    GeometryBox BoxDesc{};
    BoxDesc.strShapeTag = "Box";
    BoxDesc.vSize = { 1.0f,1.0f,1.0f };
    BoxDesc.vOffSetPosition = { 0.0f,0.5f,0.0f };

    for (UINT i = 0; i < 3; i++)
    {
		WorldMat.m[3][0] += 2.0f;
		shared_ptr<CMNGObject> pObject = CMNGObject::Create("BG_Box_02_01_SM", WorldMat, &BoxDesc, 2.0f);
		if (pObject)
		{
			GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, L"TestBox", pObject);
		}
    }

    WorldMat = XMMatrixTranslation(15.0f, 10.0f, 17.0f);

    for (UINT i = 0; i < 3; i++)
    {
        WorldMat.m[3][0] += 2.0f;
		shared_ptr<CMNGObject> pObject = CMNGObject::Create("BG_Box_02_01_Boom", WorldMat, &BoxDesc, 2.0f, true);
        if (pObject)
        {
            GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, L"TestBox", pObject);
        }
    }

    _float4x4 WorldMat2 = XMMatrixTranslation(10.0f, 0.1f, 20.0f);

    GeometrySphere SphereDesc{};
    SphereDesc.strShapeTag = "QuestItem";
    SphereDesc.fRadius = 1.0f;

    shared_ptr<CMNGQuestItem> pObject2 = CMNGQuestItem::Create("DragonBook", WorldMat2, &SphereDesc, 1.0f);
    GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, L"TestQuestItem", pObject2);

    WorldMat2 = XMMatrixTranslation(12.0f, 0.1f, 20.0f);

    pObject2 = CMNGQuestItem::Create("BurgerPortrait", WorldMat2, &SphereDesc, 1.0f);
    GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, L"TestQuestItem", pObject2);

    WorldMat2 = XMMatrixTranslation(14.0f, 0.1f, 20.0f);

    pObject2 = CMNGQuestItem::Create("JusinScroll", WorldMat2, &SphereDesc, 1.0f);
    GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, L"TestQuestItem", pObject2);

    return S_OK;
}

HRESULT CLevelDungeonMain::TestTrapController()
{
	shared_ptr<CTrapController> pController = CTrapController::Create({ 98.0f,1.0f,103.6f });
    GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, L"DungeonTrap", pController);

    return S_OK;
}

void CLevelDungeonMain::SwitchingBGM()
{
    LevelBGMOff();

    /*보스 -> 필드*/
    if (m_IsBoss) {
        m_AmbientSounds = m_DungeonMainSounds;
    }
    /*필드 -> 보스*/
    else {
        m_AmbientSounds = m_BossSounds;
    }

    LevelBGMOn();
    m_IsBoss = !m_IsBoss;
}

void CLevelDungeonMain::SwitchingMiniGameBGM()
{
    LevelBGMOff();
    
    /*미니게임 -> 던전*/
    if (m_IsMiniGame) {
        m_AmbientSounds = m_DungeonMainSounds;
    }
    /*던전 -> 미니게임*/
    else {
        m_AmbientSounds = m_MiniGameSounds;
    }
    
    LevelBGMOn();
    m_IsMiniGame = !m_IsMiniGame;
}

shared_ptr<CLevelDungeonMain> CLevelDungeonMain::Create()
{
    shared_ptr<CLevelDungeonMain> pInstance = make_shared<CLevelDungeonMain>();

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CLevelDungeonMain");

    return pInstance;
}
