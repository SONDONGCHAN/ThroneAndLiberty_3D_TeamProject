#pragma once
#include "PlayerProjectile.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayerSpellFireBlast : public CPlayerProjectile
{
public:
	CPlayerSpellFireBlast();
	virtual ~CPlayerSpellFireBlast()  override;

public:
	virtual HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render()  override;
	virtual HRESULT RenderGlow() override;

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		

public:
	void	EnableProjectile(_float3 _vMyStartPos, _float3 _vTargetPos, PlayerProjAbnormal _ePlayerProjAbnormal, _bool isDistinguish = false, _uint _iIndex = 0) override;
	void	DisableProjectile();
		
private:
	HRESULT AddRigidBody();

private:
	_float	m_fLifeTime		= { 0.1f };
	_uint	m_iRepeatCount	= { 5 };



public:
	static shared_ptr<CPlayerSpellFireBlast> Create();

};

END