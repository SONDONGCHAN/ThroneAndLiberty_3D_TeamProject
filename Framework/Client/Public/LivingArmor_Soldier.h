#pragma once

#include "Monster.h"
// 광폭화(체력 일정 이하시 행동 빨라짐), 공격(검공격, 주먹강공격), 패링가능

// CoolTime
enum LA_S_CoolTime
{
	LA_S_C_NORMALATTACK1 = 0,
	LA_S_C_NORMALATTACK2,
	LA_S_C_FURYATTACK1,
	LA_S_C_FURYATTACK2
};

// Base 상태
enum LA_S_BaseState
{
	LA_S_B_BEFOREMEET = 0,
	LA_S_B_GENERATE,
	LA_S_B_NORMAL,
	LA_S_B_WHILEFURY,
	LA_S_B_FURY,
	LA_S_B_HITTED,
	LA_S_B_HITTED2,
	LA_S_B_HITTED3,
	LA_S_B_STUN,
	LA_S_B_SHOCKED,
	LA_S_B_DOWN,
	LA_S_B_DEATH
};

// 거리에 따른 상태
enum LA_S_DistanceState
{
	LA_S_D_MOVERANGE = 0,
	LA_S_D_ATTACKRANGE,
	LA_S_D_OUTRANGE,
	LA_S_D_BEFOREMEET
};

// 특정 액션 상태
enum LA_S_ActionState
{
	LA_S_A_BLOCKED = 0,
	LA_S_A_COMBATSTART = 5,
	LA_S_A_DEAD = 6,
	LA_S_A_DEATH = 7,
	LA_S_A_KDWN3 = 10,
	LA_S_A_KDWN2 = 11,
	LA_S_A_KDWN1 = 12,
	LA_S_A_FRONTBE = 16,
	LA_S_A_FRONTBL = 17,
	LA_S_A_FRONTBS = 18,
	LA_S_A_RUN = 22,
	LA_S_A_SEARCH = 23,
	LA_S_A_SHOCKED = 24,
	LA_S_A_FURY = 25,
	LA_S_A_ATTACK1 = 26,	// 칼공격
	LA_S_A_ATTACK2 = 27,	// 주먹공격
	LA_S_A_APPEAR = 28,
	LA_S_A_STDALT = 29,
	LA_S_A_STDALT1 = 30,
	LA_S_A_STDALT2 = 31,
	LA_S_A_STDIDL = 32,
	LA_S_A_STUN = 33,
	LA_S_A_WALK = 34
};


BEGIN(Client)

class CLivingArmor_Soldier final : public CMonster
{
public:
	struct LIVINGARMOR_SOLDIER_DESC
	{
		CMonster::MONSTER_DESC pTarget;
	};
	
public:
	CLivingArmor_Soldier();
	virtual ~CLivingArmor_Soldier()  override;

public:
	HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CLivingArmor_Soldier> Create(_float3 _vCreatePos);

protected:
	HRESULT AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	virtual void EnableCollision(const char* _strShapeTag) override;
	virtual void DisableCollision(const char* _strShapeTag) override;
	
	HRESULT		AddPartObjects(string _strModelKey);
	void		UpdateDistanceState();
	void		UpdateCoolTime(_float _fTimeDelta);

	// BaseState에 따른 FSM
	void		FSM_BeforeMeet(_float _fTimeDelta);
	void		FSM_Generate(_float _fTimeDelta);
	void		FSM_Normal(_float _fTimeDelta);
	void		FSM_WhileFury(_float _fTimeDelta);
	void		FSM_Fury(_float _fTimeDelta);
	void		FSM_Hitted(_float _fTimeDelta);
	void		FSM_Hitted2(_float _fTimeDelta);
	void		FSM_Hitted3(_float _fTimeDelta);
	void		FSM_Stun(_float _fTimeDelta);
	void		FSM_Shocked(_float _fTimeDelta);
	void		FSM_Down(_float _fTimeDelta);
	void		FSM_Death(_float _fTimeDelta);

	void		CheckFury();
	void		CheckDead();
	void		CheckAttacked();

private:
	_int		m_iEffectIdx = 0;

	_int		m_iBaseState = LA_S_B_BEFOREMEET;		// Base상태
	_int		m_iDistanceState = LA_S_D_BEFOREMEET;	// 거리에 따른 상태
	_int		m_iActionState = LA_S_A_DEAD;			// 행동상태

	_float		m_fAttack1Range = 1.5f;					// 공격 Range
	_float		m_fOutRange = 10000.f;					// 범위에서 나간 Range
	_float		m_fMeetRange = 20.f;					// 조우 Range

	// 쿨타임
	_bool		m_bAttackReady[4] = { true, false, false, true };
	_float		m_fAttackCoolTime[4] = { 1.8f, 2.5f, 1.4f, 2.2f };
	_float		m_fBAttackCoolTime[4] = { 1.8f, 2.5f, 1.4f, 2.2f };

	_int		m_NormalAttackCount = 2;	// Normal 상태 공격 종류
	_int		m_FuryAttackCount = 2;		// Fury 상태 공격 종류

	// 광폭화되는 hp
	_float		m_fFuryHp = 75.f;
	// 광폭화로 빨라지는 속도
	_float		m_fFuryActionSpeed = 1.2f;

	// 광폭화시 속도
	_float		m_fFurySpeed = 2.4f;

	// 콜라이더 정리
	const char* m_strATT1Collider1 = "M_LivingArmor_Soldier_SK.ao|M_LivingArmor_Soldier_SkdmgB";
	const char* m_strATT2Collider1 = "M_LivingArmor_Soldier_SK.ao|M_LivingArmor_Soldier_SkdmgC";
};

END