#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CDungeonDoor : public CGameObject
{
public:
	CDungeonDoor();
	virtual ~CDungeonDoor() = default;

public:
	virtual HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render();

public:
	void EnergyConnect(_int _iNextIdx);
	void EnergyDisConnect(_int _iNextIdx);

private:
	void DoorOpen(_float _fTimeDelta);

private:
	shared_ptr<CModel> m_pDoorModel1 = nullptr;
	shared_ptr<CModel> m_pDoorModel2 = nullptr;

	shared_ptr<CModel> m_pDoorLockModel = nullptr;

	shared_ptr<CTransform> m_pDoorTransform2 = nullptr;
	shared_ptr<CTransform> m_pDoorLockTransform = nullptr;


private:
	vector<weak_ptr<class CEnterStatue>> m_Statues;
	_bool m_IsDoorOpen = false;
	_bool m_IsOpenDone = false;
	_bool m_IsSoundTriggerOn = false;
	_float m_fDoorOpenTime = 0.f;

public:
	static shared_ptr<CDungeonDoor> Create();


};

END
