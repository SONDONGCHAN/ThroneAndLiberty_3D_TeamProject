#pragma once


#include "Client_Defines.h"

BEGIN(Client)

class CMonsterPool
{
public:
	enum EMONTYPE {
		EMIDGET_FOOTMAN,
		EMIDGET_SHAMAN,
		EMIDGET_SLING,
		EORC_ARCHER,
		EORC_SHAMAN,
		EORC_SOLDIER,
		ELIVINGARMOR_ARCHER,
		ELIVINGARMOR_MAGE,
		ELIVINGARMOR_SOLDIER,
		EMIDGET_BERSERKER,
		EORC_ORCFIELDBOSS,
		END_ENUM
	};

	struct SpawnDesc {
		EMONTYPE eMonType;
		_float3 vPosition;
	};

public:
	CMonsterPool();
	~CMonsterPool() = default;

public:
	/*Create Monster Pool*/
	HRESULT Initialize();

	/*Spawn Monster*/
	void ActiveMonster(vector<SpawnDesc>* _pSpawnInfo);
	void ActiveMonster(string _eventKeyName);

public:
	void ClearPool();

private:
	void AwakeMonster(EMONTYPE _eMonsterType, _float3 _vPosition, _float3 _vLookPos = _float3(0.f, 0.f, 0.f));

private:
	/*Monster List*/
	vector<shared_ptr<class CMonster>> m_pMonsters[EMONTYPE::END_ENUM];

private:
	_uint m_iMaxMonsterNum = 4;
	_uint m_iFieldBossNum = 1;

public:
	static shared_ptr<CMonsterPool> Create();
	
};

END