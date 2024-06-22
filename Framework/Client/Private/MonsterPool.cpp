#include "MonsterPool.h"

#include "Midget_Footman.h"
#include "Midget_Shaman.h"
#include "Midget_Sling.h"
#include "Midget_Berserker.h"

#include "Orc_Archer.h"
#include "Orc_Shaman.h"
#include "Orc_Soldier.h"
#include "Orc_DarkCommander.h"

#include "LivingArmor_Archer.h"
#include "LivingArmor_Mage.h"
#include "LivingArmor_Soldier.h"

#include "GameInstance.h"
#include "Monster.h"


CMonsterPool::CMonsterPool()
{
}

HRESULT CMonsterPool::Initialize()
{
	_float3 vPos = { 0.f, 0.f, 0.f };

	// 고블린 몹 생성
	for (size_t i = EMONTYPE::EMIDGET_FOOTMAN; i <= EMONTYPE::EMIDGET_SLING; ++i) {

		for (size_t j = 0; j < m_iMaxMonsterNum; ++j) {

			shared_ptr<CMonster> pMonster;
			
			switch (i)
			{
			case Client::CMonsterPool::EMIDGET_FOOTMAN:

				pMonster = CMidget_Footman::Create(vPos);

				break;
			case Client::CMonsterPool::EMIDGET_SHAMAN:

				pMonster = CMidget_Shaman::Create(vPos);

				break;
			case Client::CMonsterPool::EMIDGET_SLING:

				pMonster = CMidget_Sling::Create(vPos, 0);

				break;
			}

			if (pMonster) {

				pMonster->SetEnable(false);
				GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Monster"), pMonster);
				m_pMonsters[i].push_back(pMonster);
			}
		}
	}

	// 오크 몹 생성
	for (size_t i = EMONTYPE::EORC_ARCHER; i <= EMONTYPE::EORC_SOLDIER; ++i) {

		for (size_t j = 0; j < m_iMaxMonsterNum; ++j) {

			shared_ptr<CMonster> pMonster;

			switch (i)
			{
			case Client::CMonsterPool::EORC_ARCHER:
				switch (j)
				{
				case 0:
					pMonster = COrc_Archer::Create(vPos, _float3(45.7f, 12.5f, 25.7f));
					break;
				case 1:
					pMonster = COrc_Archer::Create(vPos, _float3(25.4f, 12.8f, 27.f));
					break;
				case 2:
					pMonster = COrc_Archer::Create(vPos, _float3(29.5f, 16.6f, 99.7f));
					break;
				case 3:
					pMonster = COrc_Archer::Create(vPos, _float3(33.7f, 16.8f, 75.2f));
					break;
				}

				break;
			case Client::CMonsterPool::EORC_SHAMAN:

				pMonster = COrc_Shaman::Create(vPos);

				break;
			case Client::CMonsterPool::EORC_SOLDIER:

				pMonster = COrc_Soldier::Create(vPos);

				break;
			}

			if (pMonster) {

				pMonster->SetEnable(false);
				GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Monster"), pMonster);
				m_pMonsters[i].push_back(pMonster);
			}
		}
	}

	// 리빙아머 몹 생성
	for (size_t i = EMONTYPE::ELIVINGARMOR_ARCHER; i <= EMONTYPE::ELIVINGARMOR_SOLDIER; ++i) {

		for (size_t j = 0; j < m_iMaxMonsterNum; ++j) {

			shared_ptr<CMonster> pMonster;

			switch (i)
			{
			case Client::CMonsterPool::ELIVINGARMOR_ARCHER:

				pMonster = CLivingArmor_Archer::Create(vPos);

				break;
			case Client::CMonsterPool::ELIVINGARMOR_MAGE:

				pMonster = CLivingArmor_Mage::Create(vPos);

				break;
			case Client::CMonsterPool::ELIVINGARMOR_SOLDIER:

				pMonster = CLivingArmor_Soldier::Create(vPos);

				break;
			}

			if (pMonster) {

				pMonster->SetEnable(false);
				GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Monster"), pMonster);
				m_pMonsters[i].push_back(pMonster);
			}
		}
	}

	// 필드보스 생성
	for (size_t i = EMONTYPE::EMIDGET_BERSERKER; i <= EMONTYPE::EORC_ORCFIELDBOSS; ++i) {

		for (size_t j = 0; j < m_iFieldBossNum; ++j) {

			shared_ptr<CMonster> pMonster;

			switch (i)
			{
			case Client::CMonsterPool::EMIDGET_BERSERKER:

				pMonster = CMidget_Berserker::Create(vPos);

				break;
			case Client::CMonsterPool::EORC_ORCFIELDBOSS:
				
				pMonster = COrc_DarkCommander::Create(vPos);

				break;
			}

			if (pMonster) {

				pMonster->SetEnable(false);
				GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Monster"), pMonster);
				m_pMonsters[i].push_back(pMonster);
			}
		}
	}

	return S_OK;
}

void CMonsterPool::ActiveMonster(vector<SpawnDesc>* _pSpawnInfo)
{
	for (auto& iter : *_pSpawnInfo) {
		
		for (auto& pMon : m_pMonsters[iter.eMonType]) {
			if (!pMon->GetDead() && !pMon->IsEnabled()) {
				pMon->SetCurrentPos(iter.vPosition);
				//monster on
				pMon->SetEnable(true);
				break;
			}
		}
	}
}

void CMonsterPool::ActiveMonster(string _eventKeyName)
{
	// 던전 몬스터
	if ("Dungeon_MonsterSpawn1" == _eventKeyName) {

		AwakeMonster(EMONTYPE::ELIVINGARMOR_SOLDIER, _float3(129.f, 1.f, 99.f));
		AwakeMonster(EMONTYPE::ELIVINGARMOR_SOLDIER, _float3(137.f, 1.f, 99.f));
		AwakeMonster(EMONTYPE::ELIVINGARMOR_ARCHER, _float3(128.f, 1.f, 104.f));
		AwakeMonster(EMONTYPE::ELIVINGARMOR_ARCHER, _float3(138.f, 1.f, 104.f));
	}
	else if ("Dungeon_MonsterSpawn2" == _eventKeyName) {
		AwakeMonster(EMONTYPE::ELIVINGARMOR_MAGE, _float3(-90.5f, 1.f, 35.f));
		AwakeMonster(EMONTYPE::ELIVINGARMOR_SOLDIER, _float3(-93.f, 1.f, 38.f));
		AwakeMonster(EMONTYPE::ELIVINGARMOR_ARCHER, _float3(-88.f, 1.f, 38.f));
	}
	// 필드1 몬스터
	else if ("Field1_MonsterSpawn1" == _eventKeyName) {
		
		AwakeMonster(EMONTYPE::EMIDGET_FOOTMAN, _float3(143.f, -70.f, 140.5f), _float3(141.f, -70.f, 139.f));
		AwakeMonster(EMONTYPE::EMIDGET_SHAMAN, _float3(140.f, -70.f, 137.f), _float3(142.f, -70.f, 135.f));
		AwakeMonster(EMONTYPE::EMIDGET_SLING, _float3(137.6f, -70.f, 130.6f));
		AwakeMonster(EMONTYPE::EMIDGET_SLING, _float3(136.7f, -69.f, 142.f));
	}
	else if ("Field1_MonsterSpawn2" == _eventKeyName) {

		AwakeMonster(EMONTYPE::EMIDGET_FOOTMAN, _float3(103.5f, -66.f, 105.f), _float3(99.f, -66.f, 104.f));
		AwakeMonster(EMONTYPE::EMIDGET_FOOTMAN, _float3(98.f, -65.6f, 107.f), _float3(99.f, -66.f, 104.f));
		AwakeMonster(EMONTYPE::EMIDGET_SHAMAN, _float3(96.f, -65.6f, 101.8f), _float3(99.f, -66.f, 104.f));
		AwakeMonster(EMONTYPE::EMIDGET_SLING, _float3(100.f, -66.f, 99.4f), _float3(99.f, -66.f, 104.f));
	}
	else if ("Field1_BossSpawn" == _eventKeyName) {

		AwakeMonster(EMONTYPE::EMIDGET_BERSERKER, _float3(147.6f, -72.f, 55.5f), _float3(141.1f, -72.f, 54.8f));
	}
	// 필드2 몬스터
	else if ("Field2_MonsterSpawn1" == _eventKeyName) {

		AwakeMonster(EMONTYPE::EORC_SOLDIER, _float3(37.7f, 8.f, 19.f), _float3(39.5f, 10.f, 25.7f));
		AwakeMonster(EMONTYPE::EORC_SHAMAN, _float3(34.7f, 10.f, 22.8f), _float3(43.8f, 8.8f, 19.f));
		AwakeMonster(EMONTYPE::EORC_ARCHER, _float3(32.f, 10.f, 27.f), _float3(39.5f, 10.f, 25.7f));
		AwakeMonster(EMONTYPE::EORC_ARCHER, _float3(39.5f, 10.f, 25.7f), _float3(32.f, 10.f, 27.f));
	}
	else if ("Field2_MonsterSpawn2" == _eventKeyName) {

		AwakeMonster(EMONTYPE::EORC_SOLDIER, _float3(24.8f, 14.f, 95.6f), _float3(26.6f, 13.4f, 98.f));
		AwakeMonster(EMONTYPE::EORC_SHAMAN, _float3(29.3f, 17.f, 88.3f), _float3(26.7f, 13.4f, 92.3f));
		AwakeMonster(EMONTYPE::EORC_ARCHER, _float3(33.3f, 15.f, 93.5f), _float3(24.3f, 13.4f, 86.7f));
		AwakeMonster(EMONTYPE::EORC_ARCHER, _float3(28.7f, 15.f, 80.9f), _float3(20.7f, 13.4f, 82.f));
	}
	else if ("Field2_BossSpawn" == _eventKeyName) {
		
		AwakeMonster(EMONTYPE::EORC_ORCFIELDBOSS, _float3(165.2f, 10.5f, 82.f), _float3(162.2f, 10.5f, 79.6f));
	}
}

void CMonsterPool::ClearPool()
{
	for (auto& iter : m_pMonsters) {
		iter.clear();
	}
}

void CMonsterPool::AwakeMonster(EMONTYPE _eMonsterType, _float3 _vPosition, _float3 _vLookPos)
{
	for (auto& pMon : m_pMonsters[_eMonsterType]) {
		if (!pMon->GetDead() && !pMon->IsEnabled()) {
			pMon->SpawnMoster(_vPosition, _vLookPos);
			break;
		}
	}
}

shared_ptr<CMonsterPool> CMonsterPool::Create()
{
	shared_ptr<CMonsterPool> pMonsterPool = make_shared<CMonsterPool>();

	if (FAILED(pMonsterPool->Initialize()))
		MSG_BOX("Failed to Create : CMonsterPool");

	return pMonsterPool;
}
