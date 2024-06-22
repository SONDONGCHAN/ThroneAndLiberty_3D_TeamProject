#pragma once

#include "Trap.h"

BEGIN(Client)

class CTrapElectric final: public CTrap
{
public:
	enum EELECTRICMODE
	{
		ELEC_GO,ELEC_SPIN,ELEC_END
	};
public:
	CTrapElectric();
	virtual ~CTrapElectric() = default;

public:
	static shared_ptr<CTrapElectric> Create();

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
	void SettingElectric(EELECTRICMODE _eMode, const _float4x4& _WorldMatrix, float _fSpeed);
	void TrapStop();

private:
	float m_fTimeAcc{ 0.0f };
	bool m_isStart{ false };
	EELECTRICMODE m_eMode{ ELEC_END };
	float m_fSpeed{ 0.0f };
};

END