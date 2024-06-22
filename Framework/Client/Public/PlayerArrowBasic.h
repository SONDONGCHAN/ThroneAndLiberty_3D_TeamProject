#pragma once
#include "PlayerProjectile.h"
#include "Player.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayerArrowBasic : public CPlayerProjectile
{
public:
	CPlayerArrowBasic();
	virtual ~CPlayerArrowBasic()  override;

public:
	virtual HRESULT Initialize(weak_ptr<CPlayer> _pPlayer);
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
	_bool	m_isCollision	= { false };
	_float	m_fLifeTime		= { 1.5f };
	weak_ptr<CPlayer>		m_pOwnerObject;

public:
	static shared_ptr<CPlayerArrowBasic> Create(weak_ptr<CPlayer> _pPlayer);

};

END