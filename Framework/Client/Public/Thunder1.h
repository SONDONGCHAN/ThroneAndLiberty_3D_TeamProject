#pragma once

#include "GameObject.h"
#include "Projectile.h"
#include "Client_Defines.h"

BEGIN(Client)

class CThunder1 final: public CProjectile
{
public:
	struct THUNDERMAGIC_DESC
	{
		CProjectile::PROJECTILE_DESC tProjectile;
		shared_ptr<CGameObject> pOwner = nullptr;
	};

public:
	CThunder1();
	virtual ~CThunder1()  override;

public:
	virtual HRESULT Initialize(THUNDERMAGIC_DESC* _pThunder1Desc, CTransform::TRANSFORM_DESC* _pTransformDesc);
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
	static shared_ptr<CThunder1> Create(THUNDERMAGIC_DESC* _pThunder1Desc, CTransform::TRANSFORM_DESC* _pTransformDesc);

private:
	_float	m_fCollisionOnTime = 1.f;
	_bool	m_bThunderCollsionOn = false;
};

END