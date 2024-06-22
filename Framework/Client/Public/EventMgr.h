#pragma once

#include "Client_Defines.h"

BEGIN(Client)
class CPlayer;
class CCatapult;
class CFonosAmitoy;
class CMidget_Berserker;
class COrc_DarkCommander;


class CEventMgr
{
	DECLARE_SINGLETON(CEventMgr)

public:
	enum EEVENT_TYPE {
		ECUTSCENE,
		EMONSTER_SPAWNER,
		EANIMAL_SPAWNER,
		EPORTAL,
		ECAMERA_EVENT,
		EREADY_FOR_BOSS,
		EREADY_FOR_CATAPULT,
		ESOUND_EVENT,
		EHEALING_EVENT,
		EFINISH_FONOSMNG,
		EEFFECT,
		END_ENUM
	};

	struct EventDesc {
		EEVENT_TYPE eEventType;
		shared_ptr<class CEvent> pEvent = nullptr;
	};

public:
	CEventMgr();
	~CEventMgr() = default;

public:
	HRESULT Initialize();

/*Create Event Each Levels*/
public:
	void CreateField1Event();
	void CreateField2Event();
	void CreateDungeonMainEvent();
	void CreateDungeonFinalEvent();

public:
	void EventActive(string _strEventID);
	void ClearEvent();

public:
	void BindPlayer(shared_ptr<CPlayer> _pPlayer);
	void BindCatapult(shared_ptr<CCatapult> _pCatapult);
	void BindFonosAmitoy(shared_ptr<CFonosAmitoy> _pFonosAmitoy);
	void BindBoss1(shared_ptr<CMidget_Berserker> _pBoss1);
	void BindBoss2(shared_ptr<COrc_DarkCommander> _pBoss2);

private:
	void ActiveMonster(string _strEventID);
	void ActiveAnimal(string _strEventID);
	void ActivePortal(string _strEventID);
	void ReadyForBossRoom(string _strEventID);
	void ReadyForCatapult(string _strEventID);
	void FinishFonosMNG(string _strEventID);
	void HealingEvent(string _strEventID);

	void ShowEffect(string _strEventID);


private:
	shared_ptr<class CEventTrigger> m_pEventTrigger;
	shared_ptr<class CMonsterPool> m_pMonsterPool;
	shared_ptr<class CAnimalPool> m_pAnimalPool;

private:
	weak_ptr<CPlayer>	m_pPlayer;
	weak_ptr<CCatapult> m_pCatapult;
	weak_ptr<CFonosAmitoy> m_pFonosAmitoy;
	weak_ptr<CMidget_Berserker> m_pMidgetBerserker;
	weak_ptr<COrc_DarkCommander> m_pDarkCommander;


private:
	/*Event List*/
	unordered_map<string, EventDesc> m_Events;

};

END