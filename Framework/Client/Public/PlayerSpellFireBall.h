#pragma once
#include "PlayerProjectile.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayerSpellFireBall : public CPlayerProjectile
{
public:
	CPlayerSpellFireBall();
	virtual ~CPlayerSpellFireBall()  override;

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
	void	GoToTargetPos(_float _fTimeDelta);
		
private:
	HRESULT AddRigidBody();

private:
	_bool	m_isContact				= { false };
	_bool	m_isCollision			= { false };
	_float	m_fLifeTime				= { 2.5f };
	_float	m_fExplosionLifeTime	= { 0.1f };

	_float	m_fDegreeSpeed			= { 0.f };
	_float	m_fDegree				= { 0.f };
	_float	m_fPreDegree			= { 0.f };
	_float	m_fCurveScale			= { 1.f };

public:
	static shared_ptr<CPlayerSpellFireBall> Create();

};

END