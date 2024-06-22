#pragma once


#include "Client_Defines.h"

BEGIN(Client)

class CAnimalPool
{
public:
	enum EANIMALTYPE {
		EDEER,						// 나는 사슴
		ERABBIT,					// 나는 토끼
		ERETRIEVEL,					// 리트리버 대형
		ECROW,						// 까마귀
		ERETRIEVEL2,				// 리트리버 소형
		END_ENUM
	};

	struct SpawnDesc {
		EANIMALTYPE eAnimalType;
		_float3 vPosition;
	};

public:
	CAnimalPool();
	~CAnimalPool() = default;

public:
	/*Create Animal Pool*/
	HRESULT Initialize();

	/*Spawn Animal*/
	void ActiveAnimal(vector<SpawnDesc>* _pSpawnInfo);
	void ActiveAnimal(string _eventKeyName);

public:
	void ClearPool();
	
private:
	void AwakeAnimal(EANIMALTYPE _eAnimalType, _float3 _vPosition, _float3 _vLookPos = _float3(0.f, 0.f, 0.f));

private:
	/*Animal List*/
	vector<shared_ptr<class CNPC>> m_pAnimals[EANIMALTYPE::END_ENUM];

private:
	_uint m_iMaxAnimal1Num = 1;			// 단일 개체	
	_uint m_iMaxAnimal2Num = 20;		// 까마귀 수
	_uint m_iMaxAnimal3Num = 5;			// 새끼 강아지 수

public:
	static shared_ptr<CAnimalPool> Create();
	
};

END