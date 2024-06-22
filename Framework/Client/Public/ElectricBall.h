#pragma once

#include "GameObject.h"
#include "Projectile.h"
#include "Client_Defines.h"

BEGIN(Client)

class CElectricBall final: public CProjectile
{
public:
	struct ELECTRICMAGIC_DESC
	{
		CProjectile::PROJECTILE_DESC tProjectile;
		shared_ptr<CGameObject> pOwner = nullptr;
	};

public:
	CElectricBall();
	virtual ~CElectricBall()  override;

public:
	virtual HRESULT Initialize(ELECTRICMAGIC_DESC* _pElectricBallDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render()  override;

	void	SetType(_int _iType) { m_iType = _iType; }

protected:
	HRESULT		AddRigidBody();		// RigidBody 추가
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

public:
	static shared_ptr<CElectricBall> Create(ELECTRICMAGIC_DESC* _pElectricBallDesc, CTransform::TRANSFORM_DESC* _pTransformDesc, _int _iType = 0);

private:
	_float		m_fEnableDistance = 100.f;
	_int		m_iType = 0;
};

END