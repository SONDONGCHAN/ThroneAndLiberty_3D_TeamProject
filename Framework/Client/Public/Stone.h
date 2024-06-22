#pragma once

#include "GameObject.h"
#include "Projectile.h"
#include "Client_Defines.h"

BEGIN(Client)


class CStone final: public CProjectile
{
public:
	struct STONE_DESC
	{
		CProjectile::PROJECTILE_DESC tProjectile;
	};

public:
	CStone();
	virtual ~CStone()  override;

public:
	virtual HRESULT Initialize(STONE_DESC* _pStoneDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);
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
	static shared_ptr<CStone> Create(STONE_DESC* _pStoneDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);
};

END