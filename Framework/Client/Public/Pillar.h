#pragma once

#include "GameObject.h"
#include "Projectile.h"
#include "Client_Defines.h"

BEGIN(Client)


class CPillar final: public CProjectile
{
public:
	struct PILLAR_DESC
	{
		CProjectile::PROJECTILE_DESC tProjectile;
		_int	iType;
	};

public:
	CPillar();
	virtual ~CPillar()  override;

public:
	virtual HRESULT Initialize(PILLAR_DESC* _pPillarDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render()  override;
	void			SetMoveDown(_bool _bMove) { m_bCanMoveDown = _bMove; }
	void			SetMoveUp(_bool _bMove) { m_bCanMoveUp = _bMove; }
	void			PlayMoveUpEffect();
	void			SetCreatePos(_float3 _vCreatePos) { m_vPillarCreatePos = _vCreatePos; }

public:
	static shared_ptr<CPillar> Create(PILLAR_DESC* _pPillarDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);

protected:
	HRESULT		AddRigidBody();		// RigidBody 추가
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

private:
	_int	m_iType = 1;
	_bool	m_bCanMoveUp = false;
	_bool	m_bCanMoveDown = false;
	_bool	m_bOffCollisionToMoveDown = false;
	_float	m_fMoveSpeed = 0.75f;
	_float	m_fMoveDistance = 0.f;
	_float3 m_vPillarCreatePos = _float3(0.f, 0.f, 0.f);

private:
	shared_ptr<class CEffectAttachObj>		m_vecPillarEffect;
};

END