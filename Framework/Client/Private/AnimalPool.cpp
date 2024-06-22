#include "AnimalPool.h"

#include "Rabbit.h"
#include "Deer.h"
#include "Crow.h"
#include "Retrievel.h"

#include "GameInstance.h"
#include "NPC.h"


CAnimalPool::CAnimalPool()
{
}

HRESULT CAnimalPool::Initialize()
{
	_float3 vPos = { 0.f, 0.f, 0.f };

	// 동물 생성
	for (size_t i = EANIMALTYPE::EDEER; i <= EANIMALTYPE::ERETRIEVEL; ++i) {

		for (size_t j = 0; j < m_iMaxAnimal1Num; ++j) {

			shared_ptr<CNPC> pAnimal;
			
			switch (i)
			{
			case Client::CAnimalPool::EDEER:

				pAnimal = CDeer::Create(vPos, _float3(), 0);

				break;
			case Client::CAnimalPool::ERABBIT:

				pAnimal = CRabbit::Create(vPos, _float3(), 0);

				break;
			case Client::CAnimalPool::ERETRIEVEL:

				pAnimal = CRetrievel::Create(vPos, _float3(), 0);

				break;
			}

			if (pAnimal) {

				pAnimal->SetEnable(false);
				GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Animal"), pAnimal);
				m_pAnimals[i].push_back(pAnimal);
			}
		}
	}
	for (size_t i = EANIMALTYPE::ECROW; i <= EANIMALTYPE::ECROW; ++i) {

		for (size_t j = 0; j < m_iMaxAnimal2Num; ++j) {

			shared_ptr<CNPC> pAnimal;

			switch (i)
			{
			case Client::CAnimalPool::ECROW:

				pAnimal = CCrow::Create(vPos, _float3(), 0);

				break;
			}

			if (pAnimal) {

				pAnimal->SetEnable(false);
				GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Animal"), pAnimal);
				m_pAnimals[i].push_back(pAnimal);
			}
		}
	}
	for (size_t i = EANIMALTYPE::ERETRIEVEL2; i <= EANIMALTYPE::ERETRIEVEL2; ++i) {

		for (size_t j = 0; j < m_iMaxAnimal3Num; ++j) {

			shared_ptr<CNPC> pAnimal;

			switch (i)
			{
			case Client::CAnimalPool::ERETRIEVEL2:
				
				pAnimal = CRetrievel::Create(vPos, _float3(), 1);

				break;
			}

			if (pAnimal) {

				pAnimal->SetEnable(false);
				GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Animal"), pAnimal);
				m_pAnimals[i].push_back(pAnimal);
			}
		}
	}

	return S_OK;
}

void CAnimalPool::ActiveAnimal(vector<SpawnDesc>* _pSpawnInfo)
{
	for (auto& iter : *_pSpawnInfo) {
		
		for (auto& pAnimal : m_pAnimals[iter.eAnimalType]) {
			if (!pAnimal->IsEnabled()) {
				pAnimal->SetCurrentPos(iter.vPosition);
				//Animal on
				pAnimal->SetEnable(true);
				break;
			}
		}
	}
}

void CAnimalPool::ActiveAnimal(string _eventKeyName)
{
	// 고블린 필드 까마귀 트리거1
	if ("Field1_CrowSpawn1" == _eventKeyName) {
		AwakeAnimal(EANIMALTYPE::ECROW, _float3(181.15f, -71.f, 142.2f), _float3(181.f, -66.f, 142.5f));
		//AwakeAnimal(EANIMALTYPE::ECROW, _float3(179.7f, -69.5f, 144.5f), _float3(180.1f, -66.f, 144.5f));
		AwakeAnimal(EANIMALTYPE::ECROW, _float3(185.f, -71.f, 140.8f), _float3(185.5f, -66.f, 141.3f));
		AwakeAnimal(EANIMALTYPE::ECROW, _float3(182.9f, -71.f, 139.f), _float3(182.9f, -66.f, 138.f));
		AwakeAnimal(EANIMALTYPE::ECROW, _float3(180.f, -71.f, 139.f), _float3(181.f, -66.f, 140.f));
		AwakeAnimal(EANIMALTYPE::ECROW, _float3(174.1f, -71.f, 140.4f), _float3(174.6f, -66.f, 140.4f));
	}
	// 고블린 필드 까마귀 트리거2
	if ("Field1_CrowSpawn2" == _eventKeyName) {
		AwakeAnimal(EANIMALTYPE::ECROW, _float3(108.5f, -66.f, 131.5f), _float3(108.3f, -66.f, 131.f));
		AwakeAnimal(EANIMALTYPE::ECROW, _float3(106.8f, -66.f, 128.f), _float3(108.3f, -66.f, 127.8f));
		AwakeAnimal(EANIMALTYPE::ECROW, _float3(110.3f, -66.f, 127.f), _float3(110.1f, -66.f, 127.f));
		AwakeAnimal(EANIMALTYPE::ECROW, _float3(109.6f, -66.f, 124.f), _float3(109.6f, -66.f, 125.f));
		AwakeAnimal(EANIMALTYPE::ECROW, _float3(108.4f, -66.f, 125.f), _float3(108.3f, -66.f, 124.6f));
		AwakeAnimal(EANIMALTYPE::ECROW, _float3(106.6f, -66.f, 124.f), _float3(106.9f, -66.f, 124.6f));
	}
	// 고블린 필드 리트리버 무리 소환 트리거
	/*if ("Field1_DogSpawn" == _eventKeyName) {
		AwakeAnimal(EANIMALTYPE::ERETRIEVEL, _float3(61.2f, -73.8f, 22.6f), _float3(0.f, 0.f, 0.f));
		AwakeAnimal(EANIMALTYPE::ERETRIEVEL2, _float3(60.f, -73.8f, 20.8f), _float3(0.f, 0.f, 0.f));
		AwakeAnimal(EANIMALTYPE::ERETRIEVEL2, _float3(58.8f, -73.8f, 19.f), _float3(0.f, 0.f, 0.f));
		AwakeAnimal(EANIMALTYPE::ERETRIEVEL2, _float3(62.4f, -73.8f, 20.8f), _float3(0.f, 0.f, 0.f));
		AwakeAnimal(EANIMALTYPE::ERETRIEVEL2, _float3(63.6f, -73.8f, 19.f), _float3(0.f, 0.f, 0.f));
	}*/
}

void CAnimalPool::ClearPool()
{
	for (auto& iter : m_pAnimals) {
		iter.clear();
	}
}

void CAnimalPool::AwakeAnimal(EANIMALTYPE _eAnimalType, _float3 _vPosition, _float3 _vLookPos)
{
	for (auto& pAnimal : m_pAnimals[_eAnimalType]) {
		if (!pAnimal->IsEnabled()) {
			pAnimal->SpawnAnimal(_vPosition, _vLookPos);
			break;
		}
	}
}

shared_ptr<CAnimalPool> CAnimalPool::Create()
{
	shared_ptr<CAnimalPool> pAnimalPool = make_shared<CAnimalPool>();

	if (FAILED(pAnimalPool->Initialize()))
		MSG_BOX("Failed to Create : CAnimalPool");

	return pAnimalPool;
}
