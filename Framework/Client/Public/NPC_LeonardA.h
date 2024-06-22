#pragma once

#include "NPC.h"

// Base 상태
enum NPC_LeonardA_BaseState
{
	LEONARD_B_BEFOREMEET = 0,			// 상호작용도 안하고 멀때
	LEONARD_B_MEET,						// 상호작용은 안했으나 상호작용이 가능할정도로 가까울때
	LEONARD_B_INTERACT,					// 상호작용할때
};

// 거리에 따른 상태
enum NPC_LeonardA_DistanceState
{
	LEONARD_D_FARRANGE = 0,				// 멀때
	LEONARD_D_MEETRANGE					// 가까울때
};

// 특정 액션 상태
enum NPC_LeonardA_ActionState
{
	LEONARD_A_WALK = 7,
	LEONARD_A_RUN = 8,
	LEONARD_A_SOCLOOPINGE = 22,
	LEONARD_A_SOCLOOPINGL = 23,
	LEONARD_A_SOCLOOPINGS = 26,
	LEONARD_A_IDLE = 30,
	LEONARD_A_TURNL130 = 31,
	LEONARD_A_TURNL80 = 32,
	LEONARD_A_TURNR130 = 33,
	LEONARD_A_TURNR80 = 34,
	LEONARD_A_TALKE = 47,
	LEONARD_A_TALKL = 48,
	LEONARD_A_TALKS = 49,
};

BEGIN(Client)

class CNPC_LeonardA final : public CNPC
{
public:
	CNPC_LeonardA();
	virtual ~CNPC_LeonardA() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CNPC_LeonardA> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f, 0.f, 0.f));

public:
	virtual void InteractionOn();

protected:
	HRESULT			AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void	OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void	OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void	OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	//HRESULT		AddPartObjects(string _strModelKey);
	void		UpdateDistanceState();

	// BaseState 따른 FSM
	void		FSM_BeforeMeet(_float _fTimeDelta);
	void		FSM_Meet(_float _fTimeDelta);
	void		FSM_Interact(_float _fTimeDelta);

	// 거리
	_float		m_fMeetRange = 3.f;

private:
	_int		m_iBaseState = LEONARD_B_BEFOREMEET;		// Base상태
	_int		m_iDistanceState = LEONARD_D_FARRANGE;	// 거리에 따른 상태
	_int		m_iActionState = LEONARD_A_IDLE;			// 행동상태

	/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 0.02f, 0.02f, 0.07f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };
};

END