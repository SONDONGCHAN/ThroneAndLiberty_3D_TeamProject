#pragma once

#include "Client_Defines.h"
#include "TrapDoor.h"
#include "Mirror.h"
#include "TrapElectric.h"
#include "TrapStone.h"

BEGIN(Client)

class CTrapController final : public CGameObject
{
public:
	CTrapController();
	virtual ~CTrapController();

public:
	static shared_ptr<CTrapController> Create(const _float3& _vPosition);

public:
	HRESULT Initialize(const _float3& _vPosition);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();
	
public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag);
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag);
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag);

private:
	void CreateDoor();
	void CreateMirror();
	void CreateElectric();
	void CreateStone();

	void SpawnMirror();
	void SpawnElectric();
	void SpawnStone(float _fSpeed);
	
	void StopMirror();
	void StopElectric();

	void Reset();

private:
	shared_ptr<CRigidBody> m_pRigidBody{ nullptr };
	shared_ptr<CTrapDoor> m_pTrapDoors[2]{ nullptr };
	vector<shared_ptr<CMirror>> m_vecMirror;
	vector<shared_ptr<CTrapElectric>> m_vecElectric;
	vector<shared_ptr<CTrapStone>> m_vecStone;
	bool m_isCollision1{ false };
	bool m_isCollision2{ false };
	bool m_isCollision3{ false };
	bool m_isRun{ false };
	float m_fTimeAcc{ 0.0f };
	float m_fTimeAccElectric{ 6.0f };
	float m_fTimeAccStone{ 0.0f };

	UINT m_iElectricPhase{ 0 };
};

END