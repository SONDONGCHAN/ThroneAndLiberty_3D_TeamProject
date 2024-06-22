#pragma once

#include "NPC.h"

// GenType
enum NPC_WResident_GenType
{
	WRESIDENT_G_WALK = 0,
	WRESIDENT_G_IDLE1,
	WRESIDENT_G_NIGHTSEE,
	WRESIDENT_G_IDLE2,
	WRESIDENT_G_SELL1,
	WRESIDENT_G_SELL2,
};

// 특정 액션 상태
enum NPC_WResident_ActionState
{
	WRESIDENT_A_WALK = 0,
	WRESIDENT_A_IDLE1 = 1,
	WRESIDENT_A_NIGHTSEE = 2,
	WRESIDENT_A_IDLE2 = 3,
	WRESIDENT_A_SELL1 = 4,
	WRESIDENT_A_SELL2 = 5,
};

BEGIN(Client)

class CNPC_WResident final : public CNPC
{
public:
	CNPC_WResident();
	virtual ~CNPC_WResident() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CNPC_WResident> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f, 0.f, 0.f),
		_int _iGenType = 3, _float3 _vHairColor = _float3(0.02f, 0.02f, 0.07f), _float3 _vHairSubColor = _float3(0.4f, 0.4f, 0.4f));

public:
	virtual void InteractionOn();
	void	SetPatrolPos(_float3 _vPatrolPos) { m_vPatrolPos = _vPatrolPos; }
	void	SetHairMainColor(_float3 _vHairMainColor) { m_vHairMainColor = _vHairMainColor; }
	void	SetHairSubColor(_float3 _vHairSubColor) { m_vHairSubColor = _vHairSubColor; }

protected:
	HRESULT			AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void	OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void	OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void	OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	// BaseState 따른 FSM
	void		FSM_Seller(_float _fTimeDelta);
	void		FSM_Idle(_float _fTimeDelta);
	void		FSM_Walk(_float _fTimeDelta);
	
	// 거리
	_float		m_fMeetRange = 3.f;

private:
	_int		m_iActionState = WRESIDENT_A_IDLE1;			// 행동상태
	_bool		m_bPatrolDirection = false;
	_float3		m_vPatrolPos = _float3(0.f, 0.f, 0.f);

	/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 0.02f, 0.02f, 0.07f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };
};

END