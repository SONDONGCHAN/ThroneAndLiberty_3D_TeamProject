#pragma once

#include "Trap.h"

BEGIN(Client)

class CTrapDoor final : public CTrap
{
public:
	CTrapDoor();
	virtual ~CTrapDoor() = default;

public:
	static shared_ptr<CTrapDoor> Create(const _float3& _vPosition);

public:
	HRESULT Initialize(const _float3& _vPosition);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;

public:
	void DoorClose();
	void DoorOpen();

private:
	bool m_isDoorAction{ false };
	bool m_isClosing{ false };
	float m_fTimeAcc{ 0.0f };

};

END