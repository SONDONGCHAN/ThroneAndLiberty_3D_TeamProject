#pragma once

#include "GameObject.h"
#include "Projectile.h"
#include "Client_Defines.h"

BEGIN(Client)

enum SlashType
{
	SLASHLDOWN = 0,					// 긴 아래방향
	SLASHLUP,						// 긴 위방향
	SLASHLRIGHT,					// 긴 오른쪽 위방향
	SLASHLRIGHTDOWN,				// 긴 오른쪽 아래방향
	SLASHDOWN,						// 짧은 아래방향
	SLASHUP,						// 짧은 위방향
	SLASHRIGHT,						// 짧은 오른쪽 위방향
	SLASHBIG						// 거대 참격
};	

class CSlash final: public CProjectile
{
public:
	struct SLASH_DESC
	{
		CProjectile::PROJECTILE_DESC tProjectile;
		_int	m_iSlashType{ 0 };
	};

public:
	CSlash();
	virtual ~CSlash()  override;

public:
	virtual HRESULT Initialize(SLASH_DESC* _pSlashDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);
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
	static shared_ptr<CSlash> Create(SLASH_DESC* _pSlashDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);

private:
	_int		m_iSlashType = 0;
	_float		m_fEnableDistance = 160.f;
};

END