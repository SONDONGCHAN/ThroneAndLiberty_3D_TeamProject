#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Amitoy.h"
#include "MNGObject.h"
#include "MNGQuestItem.h"


BEGIN(Client)

class CVillageMiniGameMgr : public CGameObject
{
public:
	CVillageMiniGameMgr();
	virtual ~CVillageMiniGameMgr();

	static shared_ptr<CVillageMiniGameMgr> Create();

public:
	HRESULT Initialize();
	virtual void PriorityTick(_float _fTimeDelta);
	virtual void Tick(_float _fTimeDelta);
	virtual void LateTick(_float _fTimeDelta);

public:
	void StartMiniGame();
	void ClearMiniGame();

private:
	HRESULT CreateAmitoy();
	HRESULT CreateObjects();
	HRESULT CreateQuestItems();

	HRESULT SpawnAmitoy();
	HRESULT SpawnObjects();
	HRESULT SpawnQuestItems();

	HRESULT DespanwAmitoy();
	HRESULT DespawnObjects();
	

private:
	static bool m_isCleared;
	bool m_isStarted{ false };
	bool m_isAllAcquired{ false };
	shared_ptr<CAmitoy> m_pAmitoy{ nullptr };
	vector<shared_ptr<CMNGObject>> m_vecObjects;
	vector<shared_ptr<CMNGQuestItem>> m_vecQuestItems;
};

END