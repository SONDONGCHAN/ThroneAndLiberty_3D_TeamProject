#pragma once

#include "NPC.h"

// Base 상태
enum NPC_Henry_BaseState
{
	HENRY_B_BEFOREMEET = 0,			// 상호작용도 안했고 멀때
	HENRY_B_ALERT,					// 상호작용은 안했으나 가까울때
	HENRY_B_READY,					// 상호작용은 안했으나 상호작용이 가능할정도로 가까울때
	HENRY_B_NORMAL,					// 상호작용할때
};

// 거리에 따른 상태
enum NPC_Henry_DistanceState
{
	HENRY_D_FARRANGE = 0,			// 멀때
	HENRY_D_MEETRANGE,				// 가까울때
	HENRY_D_READYRANGE,				// 매우 가까울때
};

// 특정 액션 상태
enum NPC_Henry_ActionState
{
	HENRY_A_IDLE = 4,
	HENRY_A_RUN = 5,
	HENRY_A_TURNL130 = 14,
	HENRY_A_TURNL80 = 15,
	HENRY_A_TURNR130 = 16,
	HENRY_A_TURNR80 = 17,
	HENRY_A_WALK = 19,
};

BEGIN(Client)

class CNPC_Henry final : public CNPC
{
public:
	CNPC_Henry();
	virtual ~CNPC_Henry() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CNPC_Henry> Create(_float3 _vCreatePos);

public:
	virtual void InteractionOn();

protected:
	HRESULT			AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void	OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void	OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void	OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	HRESULT		AddPartObjects(string _strModelKey);
	void		UpdateDistanceState();

	// BaseState 따른 FSM
	void		FSM_BeforeMeet(_float _fTimeDelta);
	void		FSM_Meet(_float _fTimeDelta);
	void		FSM_Ready(_float _fTimeDelta);
	void		FSM_Normal(_float _fTimeDelta);

	// 거리
	_float		m_fMeetRange = 10.f;
	_float		m_fCloseRange = 3.f;

private:
	_int		m_iBaseState = HENRY_B_BEFOREMEET;		// Base상태
	_int		m_iDistanceState = HENRY_D_FARRANGE;	// 거리에 따른 상태
	_int		m_iActionState = HENRY_A_IDLE;			// 행동상태

	/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 194.f / 255.f, 165.f / 255.f, 90.f / 255.f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };
};

END