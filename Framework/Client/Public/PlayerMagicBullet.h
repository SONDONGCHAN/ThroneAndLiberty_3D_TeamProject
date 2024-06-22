#pragma once
#include "PlayerProjectile.h"
#include "Player.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayerMagicBullet : public CPlayerProjectile
{
	enum Curve_Type { CURVE_SIN360, CURVE_SIN180, CURVE_END};

public:
	CPlayerMagicBullet();
	virtual ~CPlayerMagicBullet()  override;

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
	_float	m_fLifeTime		= { 2.f };
	_float	m_fDegreeSpeed	= { 0.f };
	_float	m_fDegree		= { 0.f };
	_float	m_fPreDegree	= { 0.f };
	_float	m_fCurveScale	= { 1.f };
	Curve_Type	m_eCurveType = { CURVE_SIN360 };

	weak_ptr<CPlayer>		m_pOwnerObject;

public:
	static shared_ptr<CPlayerMagicBullet> Create(weak_ptr<CPlayer> _pPlayer);

};

END