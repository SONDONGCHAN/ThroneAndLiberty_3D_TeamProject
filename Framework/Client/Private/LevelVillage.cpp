#include "LevelVillage.h"
#include "TestTerrain.h"
#include "Map.h"
#include "ParsingColliders.h"

#include "CameraSpherical.h"
#include "CameraMgr.h"
#include "CameraFilm.h"

#include "EffectMgr.h"
#include "EventMgr.h"

#include "QuestMgr.h"
#include "DialogueMgr.h"

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
#include "NPC_DwarfSmith.h"
#include "NPC_DwarfMerchant.h"
#include "NPC_DwarfMusician.h"
#include "NPC_Knight.h"
#include "NPC_Soldier.h"
#include "NPC_WResident.h"
#include "NPC_WResident2.h"
#include "NPC_WResident3.h"
#include "NPC_Resident1.h"
#include "NPC_Resident2.h"

#include "NPC_MonsterDavid.h"

#include "CharacterMgr.h"
#include "Player.h"

#include "NPC_Ashien.h"

#include "SkyDome.h"
#include "Wolf.h"
#include "Eagle.h"

#include "DungeonDoor.h"
#include "EnterStatue.h"

#include "Catapult.h"
#include "Puppet.h"

#include "Retrievel.h"

#include "LegendFlower.h"
#include "EffectAttachObj.h"
#include "WallStone.h"

#include "VillageMiniGameMgr.h"
#include "UIMgr.h"


CLevelVillage::CLevelVillage()
{
}

CLevelVillage::~CLevelVillage()
{
    CCameraMgr::GetInstance()->ClearLevel();
}

HRESULT CLevelVillage::Initialize()
{

    GAMEINSTANCE->SetSimultation(false);


    if (FAILED(ReadyLight()))
        return E_FAIL;

	if (FAILED(ReadyCamera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(ReadyLayerMap(TEXT("Layer_Map"))))
		return E_FAIL;

    if (FAILED(ReadyCollider()))
        return E_FAIL;
    
    if (FAILED(ReadyLayerPlayer(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(ReadyPracticeRoom(TEXT("Layer_PracticeRoom"))))
        return E_FAIL;

    if (FAILED(ReadyLayerCatapult(TEXT("Layer_Catapult"))))
        return E_FAIL;

    if (FAILED(ReadyMonsterNPC(TEXT("Layer_MonsterNPC"))))
        return E_FAIL;

    if (FAILED(EFFECTMGR->Initialize()))
        return E_FAIL;

	if (FAILED(ReadyInteractObj(TEXT("Layer_InteractionObj"))))
		return E_FAIL;

	if (FAILED(SetLevelEvent()))
		return E_FAIL;


    if (FAILED(ReadyEnvironmentObj(TEXT("Layer_EnvironmentObj"))))
        return E_FAIL;

    if (FAILED(ReadyVillageMiniGame(L"Layer_VillageMiniGame")))
        return E_FAIL;



    UIMGR->UITickOn();

   // ReadyLevelBGM();

    return S_OK;
}

void CLevelVillage::Tick(_float _fTimeDelta)
{
    GAMEINSTANCE->SetSimultation(true);
}

void CLevelVillage::LateTick(_float _fTimeDelta)
{
}

HRESULT CLevelVillage::Render()
{
    return S_OK;
}

void CLevelVillage::ReadyLevelBGM()
{
    m_VillageSounds.emplace("BGM_Village", 0.3f);
    m_VillageSounds.emplace("AMB_Village_Voice", 1.4f);
    
    m_FieldSounds.emplace("BGM_Field", 0.4f);
    m_FieldSounds.emplace("AMB_Midgard_Field_SFX_01", 1.1f);

    m_BossSounds.emplace("BGM_MidgetKing", 0.5f);
    //m_BossSounds.emplace("AMB_Midget_Boss", 1.1f);
    
    m_AmbientSounds = m_FieldSounds;
    LevelBGMOn();

    //GAMEINSTANCE->PlayBGM("BGM_Village", 0.75f);
    //GAMEINSTANCE->PlayBGM("AMB_Village_Voice", 2.f);
}

HRESULT CLevelVillage::ReadyLight()
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

    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vDiffuse = _float4(1.f, 0.4f, 0.1f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fCullDistance = 50.f;
    LightDesc.fRange = 5.f;
    LightDesc.vPosition = _float4(99.4f, -64.9f, 103.6f, 1.f);
    LightDesc.bUsingNoise = true;
     
    if (FAILED(GAMEINSTANCE->AddLight(LightDesc)))
        return E_FAIL;


    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fCullDistance = 50.f;
    LightDesc.fRange = 20.f;
    LightDesc.vPosition = _float4(516.f, -72.f, 352.6f, 1.f);
    LightDesc.bUsingNoise = false;

    if (FAILED(GAMEINSTANCE->AddLight(LightDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelVillage::ReadyCamera(const wstring& strLayerTag)
{
    shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
    shared_ptr<CCameraMgr> pCameraMgr = CCameraMgr::GetInstance();

    CCamera::CAMERA_DESC pCamDesc = {};
    pCamDesc.fFovy = XMConvertToRadians(60.0f);
    pCamDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    pCamDesc.fNear = 0.1f;
    pCamDesc.fFar = 1000.f;

    shared_ptr<CCameraFilm> AmitoyGetItem = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/AmitoyGetItem.json");
    if (FAILED(pGameInstance->AddObject(LEVEL_VILLAGE, strLayerTag, AmitoyGetItem)))
		return E_FAIL;

	pCameraMgr->AddFilmCamera("AmitoyGetItem", AmitoyGetItem);

	shared_ptr<CCameraFilm> FieldBossMidget = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FieldBossMidget1.json");
	if (FAILED(pGameInstance->AddObject(LEVEL_VILLAGE, strLayerTag, FieldBossMidget)))
		return E_FAIL;

	pCameraMgr->AddFilmCamera("FieldBossMidget1", FieldBossMidget);

	FieldBossMidget = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FieldBossMidget2.json");
	if (FAILED(pGameInstance->AddObject(LEVEL_VILLAGE, strLayerTag, FieldBossMidget)))
		return E_FAIL;

	pCameraMgr->AddFilmCamera("FieldBossMidget2", FieldBossMidget);

	FieldBossMidget = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/FieldBossMidget3.json");
	if (FAILED(pGameInstance->AddObject(LEVEL_VILLAGE, strLayerTag, FieldBossMidget)))
		return E_FAIL;

	pCameraMgr->AddFilmCamera("FieldBossMidget3", FieldBossMidget);

	return S_OK;
}

HRESULT CLevelVillage::ReadyLayerMap(const wstring& strLayerTag)
{
    string strMapPath = "..\\..\\Client\\Bin\\DataFiles\\MapData\\Village.dat";

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

HRESULT CLevelVillage::ReadyLayerPlayer(const wstring& _strLayerTag)
{
    shared_ptr<CGameObject> pPlr = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));

    if (!pPlr) {

        shared_ptr<CPlayer> pPlayer = CPlayer::Create();

        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, _strLayerTag, pPlayer)))
            return E_FAIL;

        /*Polymorph*/

        CHARACTERMGR->AddPolymorph("Form_Human", static_pointer_cast<CPolymorph>(pPlayer));
        CHARACTERMGR->SetCurrentPolymorph("Form_Human");
        pPlayer->SetInDungeon(false);

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
        pPlayer->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(104.2f, -10.6f, 370.f));
    }
    else {

        CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->SetState(CTransform::STATE_POSITION, _float3(104.2f, -10.6f, 370.f));
        CHARACTERMGR->GetCurPolymorph().lock()->GetTransform()->Tick(0.016f);
        CHARACTERMGR->SetInDungeon(false);
        dynamic_pointer_cast<CPlayer>(pPlr)->SetInDungeon(false);
    }

    return S_OK;
}


HRESULT CLevelVillage::ReadyMonsterNPC(const wstring& _strLayerTag)
{
    shared_ptr<CNPC_MonsterDavid> pMonsterDavid = CNPC_MonsterDavid::Create(_float3(97.8f, -1000.f, 331.5f));

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_VILLAGE, _strLayerTag, pMonsterDavid)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelVillage::ReadyInteractObj(const wstring& _strLayerTag)
{   
    // Main NPC
    //shared_ptr<CNPC_Lukas> pNPC_Lukas = CNPC_Lukas::Create(_float3(71.8f, -10.6f, 315.f), 0, _float3(64.2f, -10.6f, 350.f));
    //shared_ptr<CNPC_Robert> pNPC_Robert = CNPC_Robert::Create(_float3(91.2f, -10.6f, 300.f), _float3(92.4f, -10.6f, 300.f));
    //shared_ptr<CNPC_Lottie> pNPC_Lottie = CNPC_Lottie::Create(_float3(80.1f, -10.6f, 374.f), _float3(80.3f, -10.6f, 372.4f));
    //shared_ptr<CNPC_JaniceCarter> pNPC_JancieCarter = CNPC_JaniceCarter::Create(_float3(139.f, -10.6f, 411.f), _float3(138.f, -10.6f, 412.f));
    //shared_ptr<CNPC_Davinci> pNPC_Davinci = CNPC_Davinci::Create(_float3(46.3f, -10.6f, 334.7f), _float3(48.6f, -10.6f, 337.f));
    //shared_ptr<CNPC_LeonardA> pNPC_LeonardA = CNPC_LeonardA::Create(_float3(49.f, -10.6f, 364.8f), _float3(50.7f, -10.6f, 362.6f));
    //shared_ptr<CNPC_RuffaloRice> pNPC_RuffaloRice = CNPC_RuffaloRice::Create(_float3(52.6f, -7.f, 464.5f), _float3(60.7f, -7.f, 464.5f));
    //shared_ptr<CNPC_Elleia> pNPC_Elleia = CNPC_Elleia::Create(_float3(86.9f, -10.6f, 410.f), _float3(87.7f, -10.6f, 411.5f));
    //shared_ptr<CNPC_Ashien> pNPC_Ashien = CNPC_Ashien::Create(_float3(146.f, -45.f, 25.7f), _float3(146.f, -46.f, 26.2f));
    //shared_ptr<CNPC_David> pNPC_David = CNPC_David::Create(_float3(107.f, -10.68f, 369.7f), _float3(104.5f, -10.68f, 370.f),
    //    _float3(119.f, -10.68f, 461.f), _float3(160.f, -10.68f, 461.f));

    //GAMEINSTANCE->AddStaticObject(pNPC_Lukas, { 71.8f, -10.6f, 315.f }, 2.0f);
    //GAMEINSTANCE->AddStaticObject(pNPC_Robert, { 91.2f, -10.6f, 300.f }, 2.0f);
    //GAMEINSTANCE->AddStaticObject(pNPC_Lottie, { 80.1f, -10.6f, 374.f }, 2.0f);
    //GAMEINSTANCE->AddStaticObject(pNPC_JancieCarter, { 139.f, -10.6f, 411.f }, 2.0f);
    //GAMEINSTANCE->AddStaticObject(pNPC_Davinci, { 46.3f, -10.6f, 334.7f }, 2.0f);
    //GAMEINSTANCE->AddStaticObject(pNPC_LeonardA, { 49.f, -10.6f, 364.8f }, 2.0f);
    //GAMEINSTANCE->AddStaticObject(pNPC_RuffaloRice, { 52.6f, -7.f, 464.5f }, 2.0f);
    //GAMEINSTANCE->AddStaticObject(pNPC_Elleia, { 86.9f, -10.6f, 410.f }, 2.0f);
    //GAMEINSTANCE->AddStaticObject(pNPC_Ashien, { 146.f, -45.f, 25.7f }, 2.0f);

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_LightObj"), CEffectAttachObj::Create(_float3(99.4f, -65.9f, 103.6f), "Env_Fire"))))
        return E_FAIL;


    if (FAILED(QUESTMGR->PrepareNPCs()))
        return E_FAIL;

    //GAMEINSTANCE->AddStaticObject(QUESTMGR->GetNPC("CT_Named_Lukas_SK"), { 71.8f, -10.6f, 331.5f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(QUESTMGR->GetNPC("CT_Named_Robert_SK"), { 91.2f, -10.6f, 300.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(QUESTMGR->GetNPC("CT_Named_Lottie_SK"), { 80.1f, -10.6f, 374.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(QUESTMGR->GetNPC("CT_Named_JaniceCarter_SK"), { 139.f, -10.6f, 411.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(QUESTMGR->GetNPC("CT_Named_Davinci_SK"), { 46.3f, -10.6f, 334.7f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(QUESTMGR->GetNPC("CT_Named_LeonardA_SK"), { 49.f, -10.6f, 364.8f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(QUESTMGR->GetNPC("CT_Named_RuffaloRice_SK"), { 52.6f, -7.f, 464.5f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(QUESTMGR->GetNPC("CT_Named_Elleia_SK"), { 86.16f, -10.6f, 408.8f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(QUESTMGR->GetNPC("CT_Named_Ashien_SK"), { 146.f, -45.f, 25.7f }, 2.0f);

    // 훈련교관 David
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_VILLAGE, _strLayerTag, QUESTMGR->GetNPC("CT_Named_ArthurTalon_SK"))))
        return E_FAIL;
    
    _float3 DavinciPos = _float3(46.3f, -10.6f, 334.7f);
    _float3 FondPos = _float3(65.6f, -10.6f, 350.f);
    _float3 MusicianPos = _float3(66.f, -10.6f, 380.f);
    _float3 TrainStatuePos = _float3(129.f, -10.6f, 468.f);

    // 리트리버
    shared_ptr<CRetrievel> pRetrievel1 = CRetrievel::Create(_float3(148.6f, -10.6f, 408.f), _float3(146.49f, -10.6f, 408.f), 1);
    shared_ptr<CRetrievel> pRetrievel2 = CRetrievel::Create(_float3(124.7f, -10.6f, 467.f), TrainStatuePos, 1);
    shared_ptr<CRetrievel> pRetrievel3 = CRetrievel::Create(_float3(102.f, -10.6f, 441.5f), _float3(104.5f, -10.6f, 437.f), 1);

    // Dummy NPC
    // 드워프
    // 드워프 블랙스미스
    shared_ptr<CNPC_DwarfSmith> pDummyDwarfSmith1 = CNPC_DwarfSmith::Create(_float3(50.f, -10.6f, 339.f), DavinciPos, DWARFSMITH_G_TALK1);                          // Daivinci 앞
    shared_ptr<CNPC_DwarfSmith> pDummyDwarfSmith2 = CNPC_DwarfSmith::Create(_float3(79.45f, -10.6f, 410.f), _float3(80.13f, -10.6f, 411.7f), DWARFSMITH_G_TALK2);    // 제작상인 근처
    shared_ptr<CNPC_DwarfSmith> pDummyDwarfSmith3 = CNPC_DwarfSmith::Create(_float3(86.28f, -10.6f, 412.49f), _float3(86.16f, -10.6f, 408.8f), DWARFSMITH_G_TALK3);      // 제작상인 근처
    // 드워프 상인
    shared_ptr<CNPC_DwarfMerchant> pDummyDwarfMerchant1 = CNPC_DwarfMerchant::Create(_float3(50.5f, -10.6f, 337.f), DavinciPos, DWARFMERCHANT_G_TALK); // Daivinci 앞
    shared_ptr<CNPC_DwarfMerchant> pDummyDwarfMerchant2 = CNPC_DwarfMerchant::Create(_float3(92.3f, -10.6f, 404.f), _float3(92.f, -10.6f, 405.f), DWARFMERCHANT_G_TALK); // Daivinci 앞
    // 드워프 뮤지션
    shared_ptr<CNPC_DwarfMusician> pDummyDwarfMusician1 = CNPC_DwarfMusician::Create(_float3(66.f, -10.6f, 380.f), _float3(65.6f, -10.6f, 350.f), DWARFMUSICIAN_G_SOCLOOPING2);   // 동상 앞

    // 남성
    // 상인
    shared_ptr<CNPC_Resident1> pDummyResident1_1 = CNPC_Resident1::Create(_float3(37.7f, -10.6f, 353.3f), _float3(65.6f, -10.6f, 350.f), RESIDENT1_G_IDLE, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    shared_ptr<CNPC_Resident1> pDummyResident1_2 = CNPC_Resident1::Create(_float3(125.f, -10.6f, 468.f), TrainStatuePos, RESIDENT1_G_IDLE, _float3(0.f, 0.f, 0.f));
    shared_ptr<CNPC_Resident1> pTalkResident1_1 = CNPC_Resident1::Create(_float3(151.7f, -10.6f, 418.6f), _float3(151.6f, -10.6f, 417.f), RESIDENT1_G_TALK, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    shared_ptr<CNPC_Resident1> pAngryResident1_1 = CNPC_Resident1::Create(_float3(47.f, -10.6f, 340.7f), DavinciPos, RESIDENT1_G_ANGRY, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    // 행인
    shared_ptr<CNPC_Resident2> pSitResident2_1 = CNPC_Resident2::Create(_float3(67.3f, -10.6f, 376.4f), MusicianPos, RESIDENT2_G_IDLE, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    shared_ptr<CNPC_Resident2> pSitResident2_2 = CNPC_Resident2::Create(_float3(125.5f, -10.6f, 470.5f), TrainStatuePos, RESIDENT2_G_IDLE, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    shared_ptr<CNPC_Resident2> pTalkResident2_1 = CNPC_Resident2::Create(_float3(148.2f, -10.6f, 402.f), _float3(150.f, -10.6f, 404.f), RESIDENT2_G_TALK, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    shared_ptr<CNPC_Resident2> pTalkResident2_2 = CNPC_Resident2::Create(_float3(120.6f, -10.6f, 425.f), _float3(120.6f, -10.6f, 423.25f), RESIDENT2_G_TALK, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    shared_ptr<CNPC_Resident2> pWalkResident2_1 = CNPC_Resident2::Create(_float3(77.4f,-10.6f,341.f), _float3(59.f, -10.6f, 341.4f), RESIDENT2_G_WALK200, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    
    // 여성
    // 일반인1
    shared_ptr<CNPC_WResident> pSitWResident1_1 = CNPC_WResident::Create(_float3(63.1f, -10.6f, 376.f), MusicianPos, WRESIDENT_G_NIGHTSEE, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    shared_ptr<CNPC_WResident> pSitWResident1_2 = CNPC_WResident::Create(_float3(103.5f, -10.6f, 451.f), _float3(104.f, -10.6f, 451.f), WRESIDENT_G_NIGHTSEE, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    shared_ptr<CNPC_WResident> pTalkWResident1_1 = CNPC_WResident::Create(_float3(65.6f, -10.6f, 369.f), _float3(68.f, -10.6f, 368.f), WRESIDENT_G_SELL1, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    shared_ptr<CNPC_WResident> pTalk2WResident1_1 = CNPC_WResident::Create(_float3(151.6f, -10.6f, 417.f), _float3(152.f, -10.6f, 421.4f), WRESIDENT_G_SELL1, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    shared_ptr<CNPC_WResident> pTalk2WResident1_2 = CNPC_WResident::Create(_float3(120.6f, -10.6f, 423.25f), _float3(120.6f, -10.6f, 425.f), WRESIDENT_G_SELL1, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    // 블랙스미스
    shared_ptr<CNPC_WResident2> pDummyWResident2_1 = CNPC_WResident2::Create(_float3(84.f, -10.6f, 403.f), _float3(84.7f, -10.6f, 404.2f), WRESIDENT2_G_WORK2, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    // 일반인2
    shared_ptr<CNPC_WResident3> pSitWResident3_1 = CNPC_WResident3::Create(_float3(68.4f, -10.6f, 377.f), MusicianPos, WRESIDENT3_G_UPSEE, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    shared_ptr<CNPC_WResident3> pTalk1Resident3_1 = CNPC_WResident3::Create(_float3(150.f, -10.6f, 404.f), _float3(148.2f, -10.6f, 402.f), WRESIDENT3_G_SELL1, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    shared_ptr<CNPC_WResident3> pTalk2Resident3_1 = CNPC_WResident3::Create(_float3(68.f, -10.6f, 368.f), _float3(65.6f, -10.6f, 369.f), WRESIDENT3_G_SELL1, _float3(194.f / 255.f, 165.f / 255.f, 90.f / 255.f));
    
    // 기사
    // 경계
    shared_ptr<CNPC_Knight> pDummyKnight1 = CNPC_Knight::Create(_float3(82.f, -10.6f, 376.f), _float3(83.f, -10.6f, 373.8f), KNIGHT_G_IDLE);   // 동상 앞
    shared_ptr<CNPC_Knight> pDummyKnight2 = CNPC_Knight::Create(_float3(52.f, -7.f, 461.6f), _float3(53.f, -7.f, 461.6f), KNIGHT_G_IDLE);
    shared_ptr<CNPC_Knight> pDummyKnight3 = CNPC_Knight::Create(_float3(52.f, -7.f, 467.5f), _float3(53.f, -7.f, 467.5f), KNIGHT_G_IDLE);
    // 수다
    shared_ptr<CNPC_Knight> pTalkKnight1 = CNPC_Knight::Create(_float3(83.3f, -10.6f, 336.f), _float3(85.3f, -10.6f, 336.f), KNIGHT_G_TALK);
    shared_ptr<CNPC_Knight> pTalkKnight2 = CNPC_Knight::Create(_float3(136.5f, -10.6f, 411.f), _float3(139.f, -10.6f, 411.f), KNIGHT_G_TALK);
    // 대련
    shared_ptr<CNPC_Knight> pPracticeKnight1 = CNPC_Knight::Create(_float3(135.f, -10.6f, 451.5f), _float3(137.f, -10.6f, 451.5f), KNIGHT_G_PRACTICE1);
    shared_ptr<CNPC_Knight> pPracticeKnight2 = CNPC_Knight::Create(_float3(137.f, -10.6f, 451.5f), _float3(135.f, -10.6f, 451.5f), KNIGHT_G_PRACTICE1);
    shared_ptr<CNPC_Knight> pPracticeKnight3 = CNPC_Knight::Create(_float3(145.f, -10.6f, 451.5f), _float3(147.f, -10.6f, 451.5f), KNIGHT_G_PRACTICE1);
    shared_ptr<CNPC_Knight> pPracticeKnight4 = CNPC_Knight::Create(_float3(147.f, -10.6f, 451.5f), _float3(145.f, -10.6f, 451.5f), KNIGHT_G_PRACTICE1);
    // 순찰
    //shared_ptr<CNPC_Knight> pPatrolKnight1 = CNPC_Knight::Create(_float3(103.f, -10.6f, 428.f), _float3(104.f, -10.6f, 43.f), KNIGHT_G_WALK);

    // 병사
    // 문 경계
    shared_ptr<CNPC_Soldier> pDummySoldier1 = CNPC_Soldier::Create(_float3(99.f, -10.6f, 309.f), _float3(100.f, -10.6f, 307.3f), SOLDIER_G_IDLE);
    shared_ptr<CNPC_Soldier> pDummySoldier2 = CNPC_Soldier::Create(_float3(80.5f, -10.6f, 314.f), FondPos, SOLDIER_G_IDLE);
    shared_ptr<CNPC_Soldier> pDummySoldier3 = CNPC_Soldier::Create(_float3(93.6f, -10.6f, 298.f), _float3(96.f, -10.6f, 297.f), SOLDIER_G_IDLE);
    shared_ptr<CNPC_Soldier> pDummySoldier4 = CNPC_Soldier::Create(_float3(133.8f, -10.6f, 421.6f), _float3(133.6f, -10.6f, 419.2f), SOLDIER_G_IDLE);
    shared_ptr<CNPC_Soldier> pDummySoldier5 = CNPC_Soldier::Create(_float3(166.2f, -10.6f, 400.6f), _float3(139.f, -10.6f, 411.f), SOLDIER_G_IDLE);
    shared_ptr<CNPC_Soldier> pDummySoldier6 = CNPC_Soldier::Create(_float3(108.f, -10.6f, 456.7f), _float3(109.f, -10.6f, 456.7f), SOLDIER_G_IDLE);
    shared_ptr<CNPC_Soldier> pDummySoldier7 = CNPC_Soldier::Create(_float3(108.f, -10.6f, 472.f), _float3(109.f, -10.6f, 472.f), SOLDIER_G_IDLE);
    // 순찰
    shared_ptr<CNPC_Soldier> pPatrolSoldier1 = CNPC_Soldier::Create(_float3(57.4f, -10.6f, 369.f), _float3(49.1f, -10.6f, 343.4f), SOLDIER_G_WALK200);
    shared_ptr<CNPC_Soldier> pPatrolSoldier2 = CNPC_Soldier::Create(_float3(102.6f, -10.6f, 410.f), _float3(102.6f, -10.6f, 435.f), SOLDIER_G_WALK200);
    shared_ptr<CNPC_Soldier> pPatrolSoldier3 = CNPC_Soldier::Create(_float3(64.f, -10.6f, 358.f), _float3(77.7f, -10.6f, 358.f), SOLDIER_G_WALK200);
    shared_ptr<CNPC_Soldier> pPatrolSoldier4 = CNPC_Soldier::Create(_float3(76.f, -6.8f, 461.63f), _float3(56.f, -6.8f, 461.63f), SOLDIER_G_WALK200);
    shared_ptr<CNPC_Soldier> pPatrolSoldier5 = CNPC_Soldier::Create(_float3(80.f, -6.8f, 467.f), _float3(108.f, -10.6f, 467.f), SOLDIER_G_WALK200);
    // 수다
    shared_ptr<CNPC_Soldier> pTalkSoldier1 = CNPC_Soldier::Create(_float3(85.3f, -10.6f, 336.f), _float3(83.3f, -10.6f, 336.f), SOLDIER_G_LAUGH);
    shared_ptr<CNPC_Soldier> pTalkSoldier2 = CNPC_Soldier::Create(_float3(80.13f, -10.6f, 411.7f), _float3(79.45f, -10.6f, 410.f), SOLDIER_G_LAUGH);
    // 훈련
    shared_ptr<CNPC_Soldier> pPracticeSoldier1 = CNPC_Soldier::Create(_float3(145.f, -10.6f, 482.f), _float3(145.f, -10.6f, 480.f), SOLDIER_G_PRACTICE);
    shared_ptr<CNPC_Soldier> pPracticeSoldier2 = CNPC_Soldier::Create(_float3(135.f, -10.6f, 482.f), _float3(135.f, -10.6f, 480.f), SOLDIER_G_PRACTICE);

    // 리트리버
    GAMEINSTANCE->AddStaticObject(pRetrievel1, { 148.6f, -10.6f, 408.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pRetrievel2, { 124.7f, -10.6f, 467.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pRetrievel3, { 102.f, -10.6f, 441.5f }, 2.0f);
    
    // 드위프
    GAMEINSTANCE->AddStaticObject(pDummyDwarfSmith1, { 50.f, -10.6f, 339.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummyDwarfSmith2, { 79.45f, -10.6f, 410.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummyDwarfSmith3, { 86.28f, -10.6f, 412.49f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummyDwarfMerchant1, { 50.5f, -10.6f, 337.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummyDwarfMerchant2, { 92.3f, -10.6f, 404.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummyDwarfMusician1, { 66.f, -10.6f, 380.f }, 2.0f);
    
    // 남성
    GAMEINSTANCE->AddStaticObject(pDummyResident1_1, { 37.7f, -10.6f, 353.3f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummyResident1_2, { 125.f, -10.6f, 468.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pTalkResident1_1, { 151.7f, -10.6f, 418.6f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pAngryResident1_1, { 47.f, -10.6f, 340.7f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pSitResident2_1, { 67.3f, -10.6f, 376.4f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pSitResident2_2, { 125.5f, -10.6f, 470.5f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pTalkResident2_1, { 148.2f, -10.6f, 402.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pTalkResident2_2, { 120.6f, -10.6f, 425.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pWalkResident2_1, { 77.4f,-10.6f,341.f }, 30.0f);

    // 여성
    GAMEINSTANCE->AddStaticObject(pSitWResident1_1, { 63.1f, -10.6f, 376.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pSitWResident1_2, { 103.5f, -10.6f, 451.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pTalkWResident1_1, { 65.6f, -10.6f, 369.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pTalk2WResident1_1, { 151.6f, -10.6f, 417.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pTalk2WResident1_2, { 120.6f, -10.6f, 423.25f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummyWResident2_1, { 84.f, -10.6f, 403.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pSitWResident3_1, { 68.4f, -10.6f, 377.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pTalk1Resident3_1, { 150.f, -10.6f, 404.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pTalk2Resident3_1, { 68.f, -10.6f, 368.f }, 2.0f);
    
    // 병사
    GAMEINSTANCE->AddStaticObject(pDummyKnight1, { 82.f, -10.6f, 376.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummyKnight2, { 52.f, -7.f, 461.6f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummyKnight3, { 52.f, -7.f, 467.5f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pTalkKnight1, { 83.3f, -10.6f, 336.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pTalkKnight2, { 136.5f, -10.6f, 411.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pPracticeKnight1, { 135.f, -10.6f, 451.5f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pPracticeKnight2, { 137.f, -10.6f, 451.5f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pPracticeKnight3, { 145.f, -10.6f, 451.5f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pPracticeKnight4, { 147.f, -10.6f, 451.5f }, 2.0f);
    //GAMEINSTANCE->AddStaticObject(pPatrolKnight1, { 103.f, -10.6f, 428.f }, 30.0f);
    GAMEINSTANCE->AddStaticObject(pDummySoldier1, { 99.f, -10.6f, 309.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummySoldier2, { 80.5f, -10.6f, 314.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummySoldier3, { 93.6f, -10.6f, 298.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummySoldier4, { 133.8f, -10.6f, 421.6f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummySoldier5, { 166.2f, -10.6f, 400.6f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummySoldier6, { 108.f, -10.6f, 456.7f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pDummySoldier7, { 108.f, -10.6f, 472.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pPatrolSoldier1, { 57.4f, -10.6f, 369.f }, 30.0f);
    GAMEINSTANCE->AddStaticObject(pPatrolSoldier2, { 102.6f, -10.6f, 410.f }, 30.0f);
    GAMEINSTANCE->AddStaticObject(pPatrolSoldier3, { 64.f, -10.6f, 358.f }, 30.0f);
    GAMEINSTANCE->AddStaticObject(pPatrolSoldier4, { 76.f, -6.8f, 461.63f }, 50.0f);
    GAMEINSTANCE->AddStaticObject(pPatrolSoldier5, { 80.f, -6.8f, 467.f }, 50.0f);
    GAMEINSTANCE->AddStaticObject(pTalkSoldier1, { 85.3f, -10.6f, 336.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pTalkSoldier2, { 80.13f, -10.6f, 411.7f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pPracticeSoldier1, { 145.f, -10.6f, 482.f }, 2.0f);
    GAMEINSTANCE->AddStaticObject(pPracticeSoldier2, { 135.f, -10.6f, 482.f }, 2.0f);

    //Main NPC Quest 추가
    //QUESTMGR->AddNPC(pNPC_Lukas->GetModelName(), pNPC_Lukas);
    ////QUESTMGR->AddNPC(pNPC_Lukas_Dungeon->GetModelName(), pNPC_Lukas_Dungeon);
    ////QUESTMGR->AddNPC(pNPC_RobertGhost->GetModelName(), pNPC_RobertGhost);
    //QUESTMGR->AddNPC(pNPC_Robert->GetModelName(), pNPC_Robert);
    //QUESTMGR->AddNPC(pNPC_Ashien->GetModelName(), pNPC_Ashien);
    //QUESTMGR->AddNPC(pNPC_Lottie->GetModelName(), pNPC_Lottie);
    //QUESTMGR->AddNPC(pNPC_JancieCarter->GetModelName(), pNPC_JancieCarter);
    //QUESTMGR->AddNPC(pNPC_Davinci->GetModelName(), pNPC_Davinci);
    //QUESTMGR->AddNPC(pNPC_LeonardA->GetModelName(), pNPC_LeonardA);
    //QUESTMGR->AddNPC(pNPC_RuffaloRice->GetModelName(), pNPC_RuffaloRice);
    //QUESTMGR->AddNPC(pNPC_Elleia->GetModelName(), pNPC_Elleia);
    //QUESTMGR->AddNPC(pNPC_David->GetModelName(), pNPC_David);

    //DIALOGUEMGR->AddNPC(pNPC_Lukas->GetDisplayName(), pNPC_Lukas);
    ////DIALOGUEMGR->AddNPC(pNPC_Lukas_Dungeon->GetDisplayName(), pNPC_Lukas_Dungeon);
    ////DIALOGUEMGR->AddNPC(pNPC_RobertGhost->GetDisplayName(), pNPC_RobertGhost);
    //DIALOGUEMGR->AddNPC(pNPC_Robert->GetDisplayName(), pNPC_Robert);
    //DIALOGUEMGR->AddNPC(pNPC_Ashien->GetDisplayName(), pNPC_Ashien);
    //DIALOGUEMGR->AddNPC(pNPC_Lottie->GetDisplayName(), pNPC_Lottie);
    //DIALOGUEMGR->AddNPC(pNPC_JancieCarter->GetDisplayName(), pNPC_JancieCarter);
    //DIALOGUEMGR->AddNPC(pNPC_Davinci->GetDisplayName(), pNPC_Davinci);
    //DIALOGUEMGR->AddNPC(pNPC_LeonardA->GetDisplayName(), pNPC_LeonardA);
    //DIALOGUEMGR->AddNPC(pNPC_RuffaloRice->GetDisplayName(), pNPC_RuffaloRice);
    //DIALOGUEMGR->AddNPC(pNPC_Elleia->GetDisplayName(), pNPC_Elleia);
    //DIALOGUEMGR->AddNPC(pNPC_David->GetDisplayName(), pNPC_David);
    
    /*Dungeon Enter Puzzle*/
    shared_ptr<CDungeonDoor> pDoor = CDungeonDoor::Create();
    GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_InteractionObj"), pDoor);
    //GAMEINSTANCE->AddStaticObject(pDoor, _float3(490.f, -69.f, 355.78f), 10.f);

    /*Legend Flower(Quest)*/
    shared_ptr<CLegendFlower> pFlower = CLegendFlower::Create(_float3(432.85f, -15.30f, 129.65f));
    GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_InteractionObj"), pFlower);
    
    //GAMEINSTANCE->AddStaticObject(pFlower, _float3(432.85f, -15.30f, 129.65f), 50.f);

    return S_OK;
}

HRESULT CLevelVillage::ReadyCollider()
{
    shared_ptr<CParsingColliders> pColliders = CParsingColliders::Create("..\\..\\Tool_CC\\Bin\\DataFiles\\Collider\\ColliderVillage.dat");

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_VILLAGE, L"MapCollider", pColliders)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelVillage::ReadyLayerCatapult(const wstring& _strLayerTag)
{
    shared_ptr<CCatapult> pCatapult = CCatapult::Create(_float3(280.8f, -63.9f, 341.3f));
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_VILLAGE, L"Catapult", pCatapult)))
        return E_FAIL;

    //7.2f, 3.f, 4.2f

    _float3 vStartPos   = { 373.f, -62.f, 360.2f };
    _float3 vScale      = { 2.f, 3.f, 2.f };
    _uint   iStoneIndex = { 0 };
    vector<shared_ptr<CWallStone>> vecWallStones;

    for (_uint i = 0; i < 8; i++)
    {
        vStartPos.z = 360.2f;

        for (_uint j = 0; j < 4; j++)
        {            
            shared_ptr<CWallStone> pWallStone = CWallStone::Create(vStartPos, vScale, iStoneIndex);
            if (FAILED(GAMEINSTANCE->AddObject(LEVEL_VILLAGE, L"WallStone", pWallStone)))
                return E_FAIL;
            
            vecWallStones.push_back(pWallStone);
            vStartPos.z -= (4.3f);
        }

        vStartPos.y += (3.1f);
    }

    pCatapult->BindWallStones(vecWallStones);
    
    return S_OK;
}

HRESULT CLevelVillage::ReadyEnvironmentObj(const wstring& _strLayerTag)
{
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_VILLAGE, _strLayerTag, CEffectAttachObj::Create(_float3(337.98f, -72.11f, 4.95f), "Portal2"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelVillage::ReadyPracticeRoom(const wstring& _strLayerTag)
{
    shared_ptr<CPuppet::Puppet_Desc> pPuppetDesc1 = make_shared<CPuppet::Puppet_Desc>();
    pPuppetDesc1->vMyPos = _float3(155.f, -10.6f, 480.f);
    shared_ptr<CPuppet::Puppet_Desc> pPuppetDesc2 = make_shared<CPuppet::Puppet_Desc>();
    pPuppetDesc2->vMyPos = _float3(150.f, -10.6f, 480.f);
    shared_ptr<CPuppet::Puppet_Desc> pPuppetDesc3 = make_shared<CPuppet::Puppet_Desc>();
    pPuppetDesc3->vMyPos = _float3(145.f, -10.6f, 480.f);
    shared_ptr<CPuppet::Puppet_Desc> pPuppetDesc4 = make_shared<CPuppet::Puppet_Desc>();
    pPuppetDesc4->vMyPos = _float3(140.f, -10.6f, 480.f);
    shared_ptr<CPuppet::Puppet_Desc> pPuppetDesc5 = make_shared<CPuppet::Puppet_Desc>();
    pPuppetDesc5->vMyPos = _float3(135.f, -10.6f, 480.f);
    //pPuppetDesc4->vMyPos = _float3(155.f, -10.6f, 490.f);
    //shared_ptr<CPuppet::Puppet_Desc> pPuppetDesc5 = make_shared<CPuppet::Puppet_Desc>();
    //pPuppetDesc5->vMyPos = _float3(150.f, -10.6f, 490.f);
    //shared_ptr<CPuppet::Puppet_Desc> pPuppetDesc6 = make_shared<CPuppet::Puppet_Desc>();
    //pPuppetDesc6->vMyPos = _float3(145.f, -10.6f, 490.f);

    shared_ptr<CPuppet> pPuppet1 = CPuppet::Create(pPuppetDesc1);
    GAMEINSTANCE->AddStaticObject(pPuppet1, _float3(155.f, -10.6f, 480.f));
    shared_ptr<CPuppet> pPuppet2 = CPuppet::Create(pPuppetDesc2);
    GAMEINSTANCE->AddStaticObject(pPuppet2, _float3(150.f, -10.6f, 480.f));
    shared_ptr<CPuppet> pPuppet3 = CPuppet::Create(pPuppetDesc3);
    GAMEINSTANCE->AddStaticObject(pPuppet3, _float3(145.f, -10.6f, 480.f));
    shared_ptr<CPuppet> pPuppet4 = CPuppet::Create(pPuppetDesc4);
    GAMEINSTANCE->AddStaticObject(pPuppet4, _float3(140.f, -10.6f, 480.f));
    shared_ptr<CPuppet> pPuppet5 = CPuppet::Create(pPuppetDesc5);
    GAMEINSTANCE->AddStaticObject(pPuppet5, _float3(135.f, -10.6f, 480.f));
    //shared_ptr<CPuppet> pPuppet4 = CPuppet::Create(pPuppetDesc4);
    //GAMEINSTANCE->AddStaticObject(pPuppet4, _float3(155.f, -10.6f, 490.f));
    //shared_ptr<CPuppet> pPuppet5 = CPuppet::Create(pPuppetDesc5);
    //GAMEINSTANCE->AddStaticObject(pPuppet5, _float3(150.f, -10.6f, 490.f));
    //shared_ptr<CPuppet> pPuppet6 = CPuppet::Create(pPuppetDesc6);
    //GAMEINSTANCE->AddStaticObject(pPuppet6, _float3(145.f, -10.6f, 490.f));


    return S_OK;
}

HRESULT CLevelVillage::SetLevelEvent()
{
    if (FAILED(CEventMgr::GetInstance()->Initialize()))
        return E_FAIL;

    CEventMgr::GetInstance()->CreateField1Event();


	return S_OK;
}

HRESULT CLevelVillage::ReadyVillageMiniGame(const wstring& _strLayerTag)
{
	shared_ptr<CVillageMiniGameMgr> pMiniGameMgr = CVillageMiniGameMgr::Create();

	if (pMiniGameMgr)
	{
		GAMEINSTANCE->AddObject(LEVEL_VILLAGE, _strLayerTag, pMiniGameMgr);
	}

    return S_OK;
}

void CLevelVillage::SwitchingBGM()
{
    LevelBGMOff();

    /*빌리지 -> 필드*/
    if (m_IsVillage) {
        m_AmbientSounds = m_FieldSounds;
    }
    /*필드 -> 빌리지*/
    else {
        m_AmbientSounds = m_VillageSounds;
    }

    LevelBGMOn();
    m_IsVillage = !m_IsVillage;

}

void CLevelVillage::SwitchingBGM2()
{
    LevelBGMOff();

    /*보스 -> 필드*/
    if (m_IsBoss) {
        m_AmbientSounds = m_FieldSounds;
    }
    /*필드 -> 보스*/
    else {
        m_AmbientSounds = m_BossSounds;
    }

    LevelBGMOn();
    m_IsBoss = !m_IsBoss;
}

shared_ptr<CLevelVillage> CLevelVillage::Create()
{
    shared_ptr<CLevelVillage> pLevel = make_shared<CLevelVillage>();

    if (FAILED(pLevel->Initialize()))
        MSG_BOX("Failed to Create : CLevelVillage");

    return pLevel;
}
