#include "EventMgr.h"
#include "MonsterPool.h"
#include "AnimalPool.h"
#include "EventTrigger.h"
#include "CameraMgr.h"
#include "UIMgr.h"

#include "Player.h"
#include "Catapult.h"
#include "FonosAmitoy.h"
#include "Midget_Berserker.h"
#include "Orc_DarkCommander.h"
#include "EffectAttachObj.h"

#include "QuestMgr.h"
#include "CharacterMgr.h"

#include "LevelLoading.h"

IMPLEMENT_SINGLETON(CEventMgr)

CEventMgr::CEventMgr()
{
}

HRESULT CEventMgr::Initialize()
{
	m_pMonsterPool = CMonsterPool::Create();
	m_pAnimalPool = CAnimalPool::Create();

	if (!m_pMonsterPool)
		return E_FAIL;

	if (!m_pAnimalPool)
		return E_FAIL;

	return S_OK;
}

void CEventMgr::CreateField1Event()
{
	if(nullptr == m_pPlayer.lock())
		m_pPlayer.lock() = dynamic_pointer_cast<CPlayer>(GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player")));

	vector<GeometrySphere> pSpheres;

	EventDesc desc = {};
	ZeroMemory(&desc, sizeof(desc));
	GeometrySphere sphereDesc = {};

	// MonsterSpawn
	desc.eEventType = EEVENT_TYPE::EMONSTER_SPAWNER;
	desc.pEvent = nullptr;
	
	/*Field1_MonsterSpawn1*/
	m_Events.emplace("Field1_MonsterSpawn1", desc);
	sphereDesc.fRadius = 11.f;
	sphereDesc.strShapeTag = "Field1_MonsterSpawn1";
	//sphereDesc.vOffSetPosition = _float3(168.f, -66.f, 201.f);
	sphereDesc.vOffSetPosition = _float3(190.68f, -63.87f, 242.69f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	/*Field1_MonsterSpawn2*/
	m_Events.emplace("Field1_MonsterSpawn2", desc);
	sphereDesc.fRadius = 10.f;
	sphereDesc.strShapeTag = "Field1_MonsterSpawn2";
	sphereDesc.vOffSetPosition = _float3(139.4f, -70.f, 137.7f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	/*Field1_BossSpawn*/
	m_Events.emplace("Field1_BossSpawn", desc);
	sphereDesc.fRadius = 8.f;
	sphereDesc.strShapeTag = "Field1_BossSpawn";
	sphereDesc.vOffSetPosition = _float3(80.2f, -66.f, 85.3f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	///*Portal Event(to Fonos)*/
	desc.eEventType = EEVENT_TYPE::EPORTAL;
	desc.pEvent = nullptr;

	m_Events.emplace("Event_Portal_Fonos", desc);
	sphereDesc.fRadius = 5.f;
	sphereDesc.strShapeTag = "Event_Portal_Fonos";
	sphereDesc.vOffSetPosition = _float3(337.98f, -72.11f, 4.95f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	m_Events.emplace("Event_Portal_Dungeon", desc);

	sphereDesc.fRadius = 10.f;
	sphereDesc.strShapeTag = "Event_Portal_Dungeon";
	sphereDesc.vOffSetPosition = _float3(520.f, -70.f, 352.7f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);

	pSpheres.push_back(sphereDesc);

	// AnimalSpawn
	desc.eEventType = EEVENT_TYPE::EANIMAL_SPAWNER;
	desc.pEvent = nullptr;

	/*Field1_CrowSpawn1*/
	m_Events.emplace("Field1_CrowSpawn1", desc);
	sphereDesc.fRadius = 12.f;
	sphereDesc.strShapeTag = "Field1_CrowSpawn1";
	//sphereDesc.vOffSetPosition = _float3(168.f, -66.f, 200.f);
	sphereDesc.vOffSetPosition = _float3(190.68f, -63.87f, 242.69f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	/*Field1_CrowSpawn2*/
	m_Events.emplace("Field1_CrowSpawn2", desc);
	sphereDesc.fRadius = 8.f;
	sphereDesc.strShapeTag = "Field1_CrowSpawn2";
	sphereDesc.vOffSetPosition = _float3(170.7f, -71.4f, 137.4f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);
	
	/*Field1_DogSpawn*/
	m_Events.emplace("Field1_DogSpawn", desc);
	sphereDesc.fRadius = 8.f;
	sphereDesc.strShapeTag = "Field1_DogSpawn";
	sphereDesc.vOffSetPosition = _float3(85.f, -65.f, 87.f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	/*Ready Catapult*/
	desc.eEventType = EEVENT_TYPE::EREADY_FOR_CATAPULT;
	desc.pEvent = nullptr;
	m_Events.emplace("Event_Ready_Catapult", desc);
	sphereDesc.fRadius = 2.f;
	sphereDesc.strShapeTag = "Event_Ready_Catapult";
	sphereDesc.vOffSetPosition = _float3(248.8f, -64.8f, 339.75f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	/* Boss Ready Event */
	desc.eEventType = EEVENT_TYPE::EREADY_FOR_BOSS;
	desc.pEvent = nullptr;
	m_Events.emplace("BossReady_Field", desc);
	sphereDesc.fRadius = 10.f;
	sphereDesc.strShapeTag = "BossReady_Field";
	sphereDesc.vOffSetPosition = _float3(103.99f, -71.72f, 44.74f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);


	/*Field Sound Event*/
	desc.eEventType = EEVENT_TYPE::ESOUND_EVENT;
	desc.pEvent = nullptr;
	m_Events.emplace("SoundEvent_Village", desc);

	sphereDesc.fRadius = 198.f;
	//198 > 196
	sphereDesc.strShapeTag = "SoundEvent_Village";
	sphereDesc.vOffSetPosition = _float3(23.82f, 75.48f, 471.90f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	/*Ready Effect*/
	desc.eEventType = EEVENT_TYPE::EEFFECT;
	desc.pEvent = nullptr;

	m_Events.emplace("Event_Field_Portal_Effect", desc);
	sphereDesc.fRadius = 20.f;
	sphereDesc.strShapeTag = "Event_Field_Portal_Effect";
	sphereDesc.vOffSetPosition = _float3(464.01f, -67.f, 328.09);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);
	
	m_pEventTrigger = CEventTrigger::Create(&pSpheres);
	GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Event"), m_pEventTrigger);



}

void CEventMgr::CreateField2Event()
{
	if (nullptr == m_pPlayer.lock())
		m_pPlayer.lock() = dynamic_pointer_cast<CPlayer>(GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player")));

	vector<GeometrySphere> pSpheres;

	EventDesc desc = {};
	ZeroMemory(&desc, sizeof(desc));
	GeometrySphere sphereDesc = {};

	// MonsterSpawn
	desc.eEventType = EEVENT_TYPE::EMONSTER_SPAWNER;
	desc.pEvent = nullptr;

	/*Field2_MonsterSpawn1*/
	m_Events.emplace("Field2_MonsterSpawn1", desc);
	sphereDesc.fRadius = 8.f;
	sphereDesc.strShapeTag = "Field2_MonsterSpawn1";
	sphereDesc.vOffSetPosition = _float3(75.7f, 4.2f, 22.f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	/*Field2_MonsterSpawn2*/
	m_Events.emplace("Field2_MonsterSpawn2", desc);
	sphereDesc.fRadius = 8.f;
	sphereDesc.strShapeTag = "Field2_MonsterSpawn2";
	sphereDesc.vOffSetPosition = _float3(35.2f, 9.77f, 46.2f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	/*Field2_BossSpawn*/
	m_Events.emplace("Field2_BossSpawn", desc);
	sphereDesc.fRadius = 8.f;
	sphereDesc.strShapeTag = "Field2_BossSpawn";
	//sphereDesc.vOffSetPosition = _float3(132.4f, 10.3f, 114.3f);
	sphereDesc.vOffSetPosition = _float3(122.41f, 6.10f, 81.87f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	///*Portal Event*/
	desc.eEventType = EEVENT_TYPE::EPORTAL;
	desc.pEvent = nullptr;

	m_Events.emplace("Event_Portal_Village", desc);

	sphereDesc.fRadius = 5.f;
	sphereDesc.strShapeTag = "Event_Portal_Village";
	sphereDesc.vOffSetPosition = _float3(183.6f, 10.07f, 92.54f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	
	pSpheres.push_back(sphereDesc);

	/* Boss Ready Event */
	desc.eEventType = EEVENT_TYPE::EREADY_FOR_BOSS;
	desc.pEvent = nullptr;

	m_Events.emplace("BossReady_Fonos", desc);

	sphereDesc.fRadius = 8.f;
	sphereDesc.strShapeTag = "BossReady_Fonos";
	sphereDesc.vOffSetPosition = _float3(142.9f, 8.5f, 64.1f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);

	pSpheres.push_back(sphereDesc);

	/*Finish Fonos MiniGame*/
	desc.eEventType = EEVENT_TYPE::EFINISH_FONOSMNG;
	desc.pEvent = nullptr;

	m_Events.emplace("Event_Finish_FonosMNG", desc);

	sphereDesc.fRadius = 2.f;
	sphereDesc.strShapeTag = "Event_Finish_FonosMNG";
	sphereDesc.vOffSetPosition = _float3(35.688f, 16.617f, 145.930f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	/*Ready Effect*/
	desc.eEventType = EEVENT_TYPE::EEFFECT;
	desc.pEvent = nullptr;

	m_Events.emplace("Event_Fonos_Effect", desc);

	sphereDesc.fRadius = 20.f;
	sphereDesc.strShapeTag = "Event_Fonos_Effect";
	sphereDesc.vOffSetPosition = _float3(11.42f, 15.0f, 66.48f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	m_pEventTrigger = CEventTrigger::Create(&pSpheres);
	GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Event"), m_pEventTrigger);
}

void CEventMgr::CreateDungeonMainEvent()
{
	if (nullptr == m_pPlayer.lock())
		m_pPlayer.lock() = dynamic_pointer_cast<CPlayer>(GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player")));

	vector<GeometrySphere> pSpheres;

	EventDesc desc = {};
	ZeroMemory(&desc, sizeof(desc));
	GeometrySphere sphereDesc = {};

	/*Dungeon_MonsterSpawn1*/
	desc.eEventType = EEVENT_TYPE::EMONSTER_SPAWNER;
	desc.pEvent = nullptr;

	m_Events.emplace("Dungeon_MonsterSpawn1", desc);
	
	sphereDesc.fRadius = 4.f;
	sphereDesc.strShapeTag = "Dungeon_MonsterSpawn1";
	sphereDesc.vOffSetPosition = _float3(110.f, 0.f, 32.f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);

	pSpheres.push_back(sphereDesc);
	
	/*Dungeon_MonsterSpawn2*/
	desc.eEventType = EEVENT_TYPE::EMONSTER_SPAWNER;
	desc.pEvent = nullptr;
	
	m_Events.emplace("Dungeon_MonsterSpawn2", desc);

	sphereDesc.fRadius = 4.f;
	sphereDesc.strShapeTag = "Dungeon_MonsterSpawn2";
	sphereDesc.vOffSetPosition = _float3(-67.f, 0.f, 103.f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);

	pSpheres.push_back(sphereDesc);
	
	/*Portal Event*/
	desc.eEventType = EEVENT_TYPE::EPORTAL;
	desc.pEvent = nullptr;

	m_Events.emplace("Event_Portal_Boss", desc);

	sphereDesc.fRadius = 4.f;
	sphereDesc.strShapeTag = "Event_Portal_Boss";
	sphereDesc.vOffSetPosition = _float3(21.f, -7.f, 78.f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);

	pSpheres.push_back(sphereDesc);

	/*Dungeon Healing Event*/
	desc.eEventType = EEVENT_TYPE::EHEALING_EVENT;
	desc.pEvent = nullptr;
	m_Events.emplace("Healing_Event", desc);
	
	sphereDesc.fRadius = 20.f;
	sphereDesc.strShapeTag = "Healing_Event";
	sphereDesc.vOffSetPosition = _float3(-90.f, 0.f, 103.f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);

	/* Camera Event */
	m_pEventTrigger = CEventTrigger::Create(&pSpheres);
	GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Event"), m_pEventTrigger);
}

void CEventMgr::CreateDungeonFinalEvent()
{
	if (nullptr == m_pPlayer.lock())
		m_pPlayer.lock() = dynamic_pointer_cast<CPlayer>(GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player")));

	vector<GeometrySphere> pSpheres;

	EventDesc desc = {};
	ZeroMemory(&desc, sizeof(desc));

	GeometrySphere sphereDesc = {};
	/* Boss Ready Event */
	desc.eEventType = EEVENT_TYPE::EREADY_FOR_BOSS;
	desc.pEvent = nullptr;
	m_Events.emplace("BossReady_Final", desc);
	sphereDesc.fRadius = 2.f;
	sphereDesc.strShapeTag = "BossReady_Final";
	sphereDesc.vOffSetPosition = _float3(21.2f, -15.9f, 136.2f);
	sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	pSpheres.push_back(sphereDesc);


	m_pEventTrigger = CEventTrigger::Create(&pSpheres);
	GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Event"), m_pEventTrigger);


	//vector<GeometrySphere> pSpheres;

	//EventDesc desc = {};
	//ZeroMemory(&desc, sizeof(desc));
	//GeometrySphere sphereDesc = {};

	//// MonsterSpawn
	//desc.eEventType = EEVENT_TYPE::EMONSTER_SPAWNER;
	//desc.pEvent = nullptr;

	///*Field1_MonsterSpawn1*/

	//m_Events.emplace("Field1_MonsterSpawn1", desc);

	//sphereDesc.fRadius = 4.f;
	//sphereDesc.strShapeTag = "Field1_MonsterSpawn1";
	//sphereDesc.vOffSetPosition = _float3(166.f, -67.f, 197.f);
	//sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);

	//pSpheres.push_back(sphereDesc);

	///*Field1_MonsterSpawn2*/
	//desc.eEventType = EEVENT_TYPE::EMONSTER_SPAWNER;
	//desc.pEvent = nullptr;

	//m_Events.emplace("Field1_MonsterSpawn2", desc);

	//sphereDesc.fRadius = 4.f;
	//sphereDesc.strShapeTag = "Field1_MonsterSpawn2";
	//sphereDesc.vOffSetPosition = _float3(137.f, -69.f, 128.f);
	//sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);

	//pSpheres.push_back(sphereDesc);

	///*Field1_BossSpawn*/
	//desc.eEventType = EEVENT_TYPE::EMONSTER_SPAWNER;
	//desc.pEvent = nullptr;\ \


	//m_Events.emplace("Field1_BossSpawn", desc);

	//sphereDesc.fRadius = 4.f;
	//sphereDesc.strShapeTag = "Field1_BossSpawn";
	//sphereDesc.vOffSetPosition = _float3(96.f, -69.f, 74.f);
	//sphereDesc.vOffSetRotation = _float3(0.f, 0.f, 0.f);
	
	//pSpheres.push_back(sphereDesc);
	
	//m_pEventTrigger = CEventTrigger::Create(&pSpheres);
	//GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Event"), m_pEventTrigger);
}

void CEventMgr::EventActive(string _strEventID)
{
	auto iter = m_Events.find(_strEventID);

	if (m_Events.end() == iter)
		return;

	switch (iter->second.eEventType)
	{
		/*컷씬*/
		case ECUTSCENE:
		{

		}
		break;
		/*몬스터 스포너*/
		case EMONSTER_SPAWNER:
		{
			ActiveMonster(_strEventID);
		}
		break;
		/*동물 스포너*/
		case EANIMAL_SPAWNER:
		{
			ActiveAnimal(_strEventID);
		}
		break;
		/*포탈*/
		case EPORTAL:
		{
			ActivePortal(_strEventID);
		}
		break;
		/*카메라 이벤트*/
		case ECAMERA_EVENT:
		{
			CCameraMgr::GetInstance()->FilmCameraTurnOn(_strEventID);
		}
		break;
		/*보스방 준비*/
		case EREADY_FOR_BOSS:
		{
			ReadyForBossRoom(_strEventID);
		}
		break;
		case ESOUND_EVENT:
		{
			GAMEINSTANCE->SwitchingBGM();
		}
		break;
		/*투석기 미니게임 준비*/
		case EREADY_FOR_CATAPULT:
		{
			ReadyForCatapult(_strEventID);
		}
		break;
		/*포노스 미니게임 종료*/
		case EFINISH_FONOSMNG:
		{
			FinishFonosMNG(_strEventID);
		}
		break;
		/*던전 힐링방 이벤트*/
		case EHEALING_EVENT:
		{
			HealingEvent(_strEventID);
		}
		break;
		/*이펙트 생성 트리거*/
		case EEFFECT:
		{
			ShowEffect(_strEventID);
		}
		break;
	}
}

void CEventMgr::ClearEvent()
{
	m_Events.clear();
	
	if(m_pEventTrigger)
		m_pEventTrigger = nullptr;
	
	//if(m_pMonsterPool)
	//	m_pMonsterPool->ClearPool();
}

void CEventMgr::BindPlayer(shared_ptr<CPlayer> _pPlayer)
{
	m_pPlayer = _pPlayer;
}

void CEventMgr::BindCatapult(shared_ptr<CCatapult> _pCatapult)
{
	m_pCatapult = _pCatapult;
}

void CEventMgr::BindFonosAmitoy(shared_ptr<CFonosAmitoy> _pFonosAmitoy)
{
	m_pFonosAmitoy = _pFonosAmitoy;
}

void CEventMgr::BindBoss1(shared_ptr<CMidget_Berserker> _pBoss1)
{
	m_pMidgetBerserker = _pBoss1;
}

void CEventMgr::BindBoss2(shared_ptr<COrc_DarkCommander> _pBoss2)
{
	m_pDarkCommander = _pBoss2;
}

void CEventMgr::ActiveMonster(string _strEventID)
{
	m_pMonsterPool->ActiveMonster(_strEventID);
}

void CEventMgr::ActiveAnimal(string _strEventID)
{
	m_pAnimalPool->ActiveAnimal(_strEventID);
}

void CEventMgr::ActivePortal(string _strEventID)
{
	shared_ptr<CGameObject> pPlr = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
	CHARACTERMGR->SetInDungeon(false);
	dynamic_pointer_cast<CPlayer>(pPlr)->SetInDungeon(false);
	UIMGR->UITickOff();
	GAMEINSTANCE->StopAll();

	if ("Event_Portal_Village" == _strEventID)
	{
		GAMEINSTANCE->SetLoadingLevel(CLevelLoading::Create(LEVEL_VILLAGE));
		GAMEINSTANCE->SetChangeLevelSignal(LEVEL_VILLAGE);
	}
	else if("Event_Portal_Boss" == _strEventID)
	{
		GAMEINSTANCE->SetLoadingLevel(CLevelLoading::Create(LEVEL_DUNGEON_FINAL));
		GAMEINSTANCE->SetChangeLevelSignal(LEVEL_DUNGEON_FINAL);
	}
	else if ("Event_Portal_Fonos" == _strEventID) {
		GAMEINSTANCE->SetLoadingLevel(CLevelLoading::Create(LEVEL_FONOS));
		GAMEINSTANCE->SetChangeLevelSignal(LEVEL_FONOS);
	}
	else if ("Event_Portal_Dungeon" == _strEventID) {
		GAMEINSTANCE->SetLoadingLevel(CLevelLoading::Create(LEVEL_DUNGEON_MAIN));
		GAMEINSTANCE->SetChangeLevelSignal(LEVEL_DUNGEON_MAIN);
	}
}

void CEventMgr::ReadyForBossRoom(string _strEventID)
{
	if ("BossReady_Field" == _strEventID)
	{
		m_pPlayer.lock()->LockOnChangeToBoss();
		vector<string> vecFieldBossFilmCamera;
		vecFieldBossFilmCamera.emplace_back("FieldBossMidget1");
		vecFieldBossFilmCamera.emplace_back("FieldBossMidget2");
		vecFieldBossFilmCamera.emplace_back("FieldBossMidget3");
		CCameraMgr::GetInstance()->FilmCameraTurnOn(vecFieldBossFilmCamera);
		m_pMidgetBerserker.lock()->CameraStart();
	}
	else if ("BossReady_Fonos" == _strEventID)
	{
		m_pPlayer.lock()->LockOnChangeToBoss();
		vector<string> vecFieldBossFilmCamera;
		vecFieldBossFilmCamera.emplace_back("FieldBossOrc1");
		vecFieldBossFilmCamera.emplace_back("FieldBossOrc2");
		CCameraMgr::GetInstance()->FilmCameraTurnOn(vecFieldBossFilmCamera);
		//CCameraMgr::GetInstance()->FilmCameraTurnOn("FieldBossOrc");
		m_pDarkCommander.lock()->CameraStart();
	}
	else if ("BossReady_Final" == _strEventID)
	{
		m_pPlayer.lock()->LockOnChangeToBoss();
	}
}

void CEventMgr::ReadyForCatapult(string _strEventID)
{
	if ("Event_Ready_Catapult" == _strEventID)
	{
		m_pCatapult.lock()->CatapultOn();
	}
}

void CEventMgr::FinishFonosMNG(string _strEventID)
{
	if ("Event_Finish_FonosMNG" == _strEventID)
	{
		m_pFonosAmitoy.lock()->MiniGameCeremony();
	}
}

void CEventMgr::HealingEvent(string _strEventID)
{
	if ("Healing_Event" == _strEventID)
	{
		m_pPlayer.lock()->SetHealing();
	}
}

void CEventMgr::ShowEffect(string _strEventID)
{
	if ("Event_Fonos_Effect" == _strEventID)
	{
		if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, TEXT("Layer_Environment"), CEffectAttachObj::Create(_float3(3.41f, 17.f, 89.72f), "Env_FireSmoke_Up"))))
			return;
		if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, TEXT("Layer_Environment"), CEffectAttachObj::Create(_float3(42.53f, 19.48f, 77.66f), "Env_FireSmoke_Up"))))
			return;
	}

	else if ("Event_Field_Portal_Effect" == _strEventID)
	{
		if (FAILED(GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_Environment"), CEffectAttachObj::Create(_float3(510.f, -70.f, 352.7f), "Entrance"))))
			return;
	}
}