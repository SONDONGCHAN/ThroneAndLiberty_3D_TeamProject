#pragma once

#include "NPC.h"

enum Crow_GenType
{
	CROW_G_RUN = 0,
	CROW_G_EAT = 1,
};

// Base 상태
enum Crow_BaseState
{
	CROW_B_BEFOREMEET = 0,			// 상호작용도 안하고 멀때
	CROW_B_MEET,						// 상호작용은 안했으나 상호작용이 가능할정도로 가까울때
	CROW_B_INTERACT,
};

// 거리에 따른 상태
enum Crow_DistanceState
{
	CROW_D_FARRANGE = 0,				// 멀때
	CROW_D_MEETRANGE					// 가까울때
};

// 특정 액션 상태
enum Crow_ActionState
{
	CROW_A_STARTFLY = 1,
	CROW_A_FLY = 2,
	CROW_A_STARTFLY2 = 3,
	CROW_A_IDLE = 4,
	CROW_A_IDLEMOTION = 5,
	CROW_A_EATLAND = 6,
	CROW_A_WALK = 7,
};

BEGIN(Client)

class CCrow final : public CNPC
{
public:
	CCrow();
	virtual ~CCrow() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CCrow> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f,0.f,0.f), _int _iGenType = 0);

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

	// 거리
	_float		m_fMeetRange = 10.f;

	_float		m_fLifeTime = 5.f;			// 생명기간

private:
	_int		m_iBaseState = CROW_B_BEFOREMEET;		// Base상태
	_int		m_iDistanceState = CROW_D_FARRANGE;	// 거리에 따른 상태
	_int		m_iActionState = CROW_A_EATLAND;			// 행동상태
};

END