#pragma once

#include "GameObject.h"
#include "Projectile.h"
#include "Client_Defines.h"

BEGIN(Client)


class CShockWave final: public CProjectile
{
public:
	struct SHOCKWAVE_DESC
	{
		CProjectile::PROJECTILE_DESC tProjectile;
	};

public:
	CShockWave();
	virtual ~CShockWave()  override;

public:
	virtual HRESULT Initialize(SHOCKWAVE_DESC* _pShockWaveDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render()  override;

protected:
	HRESULT		AddRigidBody();		// RigidBody 추가
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

public:
	static shared_ptr<CShockWave> Create(SHOCKWAVE_DESC* _pShockWaveDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);

private:
	_float	m_fCalculateTime = 0.f;			// 계산용 타이머

	_float	m_fCollisionTime = 1.f;
};

END