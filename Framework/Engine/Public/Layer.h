#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CLayer final
{
public:
	CLayer();
	~CLayer();

public:
	HRESULT Initialize();
	void PriorityTick(_float _fTimeDelta);
	void Tick(_float _fTimeDelta);
	void LateTick(_float _fTimeDelta);

public:
	/*특정 레이어에 바로 오브젝트를 추가*/
	HRESULT AddGameObject(shared_ptr<class CGameObject> _pGameObject);
	void ClearLayer();

public:
	list<shared_ptr<class CGameObject>>* GetObjectList() { return &m_GameObjects; }
	shared_ptr<class CGameObject> GetGameObject();

private:
	list<shared_ptr<class CGameObject>> m_GameObjects;

public:
	static shared_ptr<CLayer> Create();


};

END
