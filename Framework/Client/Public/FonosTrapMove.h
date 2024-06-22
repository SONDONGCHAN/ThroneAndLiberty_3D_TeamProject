#pragma once

#include "FonosTrap.h"

BEGIN(Client)

class CFonosTrapMove final : public CFonosTrap
{
public:
	enum MOVE_TRAP_TYPE
	{
		MOVE_UD,
		MOVE_LR
	};

public:
	CFonosTrapMove();
	virtual ~CFonosTrapMove();

public:
	static shared_ptr<CFonosTrapMove> Create(TRAP_TYPE _eTrapType, _float3 _vPos, _float3 _vLook);

public:
	HRESULT Initialize(TRAP_TYPE _eTrapType, _float3 _vPos, _float3 _vLook);
	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	SetMoveType(MOVE_TRAP_TYPE _eMoveType, _bool _bStartDir, _float _fSpeed);

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;

private:
	_float		m_fMaxMoveAmount = 3.f;
	_float		m_fMoveAmount = 0.f;
	_float		m_fMoveSpeed = 1.f;

	_bool		m_bMoveDir = false;
	MOVE_TRAP_TYPE		m_eMoveType = MOVE_UD;
};

END