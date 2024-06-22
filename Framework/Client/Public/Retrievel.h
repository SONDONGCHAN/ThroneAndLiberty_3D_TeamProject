#pragma once

#include "NPC.h"

// 강아지 튀어나오게 하기(천천히 무리로 지나가는데 상호작용하면 나를 본후에 한번 짖고 대기후에 다시 갈길을 감)
// 크기가 큰거, 작은거 두가지

enum Retrievel_GenType
{
	RETRIEVEL_G_RUN = 0,
	RETRIEVEL_G_EAT = 1,
};

// Base 상태
enum Retrievel_BaseState
{
	RETRIEVEL_B_BEFOREMEET = 0,			// 상호작용도 안하고 멀때
	RETRIEVEL_B_MEET,						// 상호작용은 안했으나 상호작용이 가능할정도로 가까울때
	RETRIEVEL_B_INTERACT,
};

// 거리에 따른 상태
enum Retrievel_DistanceState
{
	RETRIEVEL_D_FARRANGE = 0,				// 멀때
	RETRIEVEL_D_MEETRANGE					// 가까울때
};

// 특정 액션 상태
enum Retrievel_ActionState
{
	RETRIEVEL_A_IDLE = 0,
	RETRIEVEL_A_CHARM3 = 1,
	RETRIEVEL_A_CHARM2 = 2,
	RETRIEVEL_A_CHARM1 = 3,
	RETRIEVEL_A_EAT3 = 4,
	RETRIEVEL_A_EAT2 = 5,
	RETRIEVEL_A_EAT1 = 6,
	RETRIEVEL_A_TALK = 7,
	RETRIEVEL_A_WALK = 12
};

BEGIN(Client)

class CRetrievel final : public CNPC
{
public:
	CRetrievel();
	virtual ~CRetrievel() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CRetrievel> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f,0.f,0.f), _int _iGenType = 0);

public:
	virtual void InteractionOn();
	
protected:
	HRESULT			AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void	OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void	OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void	OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	void		UpdateDistanceState();

	// BaseState 따른 FSM
	void		FSM_BeforeMeet(_float _fTimeDelta);
	void		FSM_Meet(_float _fTimeDelta);
	void		FSM_Interact(_float _fTimeDelta);

	_float		m_fInitRotation = 3.14f * 0.2f;

	// 거리
	_float		m_fMeetRange = 5.f;

	_float		m_fEatIntervel = 2.f;		// 먹는 주기

	_float		m_fWalkSpeed = 2.f;
	_float		m_fLifeTime = 20.f;			// 생명기간

	_bool		m_bInteractDog = false;
	_bool		m_bFirstInit = true;

private:
	_int		m_iBaseState = RETRIEVEL_B_BEFOREMEET;			// Base상태
	_int		m_iDistanceState = RETRIEVEL_D_FARRANGE;	// 거리에 따른 상태
	_int		m_iActionState = RETRIEVEL_A_TALK;			// 행동상태
};

END