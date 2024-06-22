#pragma once

#include "Trap.h"

BEGIN(Client)

class CTrapStone final : public CTrap
{
public:
	CTrapStone();
	virtual ~CTrapStone() = default;

public:
	static shared_ptr<CTrapStone> Create();

public:
	HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;

public:
	void SettingStone(const _float3& _vPosition, float _fSpeed);

private:
	float m_fTimeAcc{ 0.0f };
	bool m_isStart{ false };
	float m_fSpeed{ 0.0f };
};

END