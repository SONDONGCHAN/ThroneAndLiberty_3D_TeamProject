#pragma once

#include "NPC.h"

// GenType
enum NPC_WResident3_GenType
{
	WRESIDENT3_G_IDLE = 0,
	WRESIDENT3_G_WALK,
	WRESIDENT3_G_UPSEE,
	WRESIDENT3_G_SELL1,
	WRESIDENT3_G_SELL2,
};

// 특정 액션 상태
enum NPC_WResident3_ActionState
{
	WRESIDENT3_A_IDLE = 0,
	WRESIDENT3_A_WALK = 1,
	WRESIDENT3_A_UPSEE = 2,
	WRESIDENT3_A_SELL1 = 3,
	WRESIDENT3_A_SELL2 = 4,
};

BEGIN(Client)

class CNPC_WResident3 final : public CNPC
{
public:
	CNPC_WResident3();
	virtual ~CNPC_WResident3() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CNPC_WResident3> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f, 0.f, 0.f),
		_int _iGenType = 0, _float3 _vHairColor = _float3(0.02f, 0.02f, 0.07f), _float3 _vHairSubColor = _float3(0.4f, 0.4f, 0.4f));

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
	void		FSM_Idle(_float _fTimeDelta);
	void		FSM_Walk(_float _fTimeDelta);
	
	// 거리
	_float		m_fMeetRange = 3.f;

private:
	_int		m_iActionState = WRESIDENT3_A_IDLE;			// 행동상태
	_bool		m_bPatrolDirection = false;
	_float3		m_vPatrolPos = _float3(0.f, 0.f, 0.f);

	/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 0.02f, 0.02f, 0.07f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };
};

END