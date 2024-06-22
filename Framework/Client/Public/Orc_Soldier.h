#pragma once

#include "Monster.h"
// 공격(공격1, 공격2(회전공격)), 샤먼의 버프에 걸리면 버프(공속, 공격력 up),
// 공격 패링가능

// CoolTime
enum OC_S_CoolTime
{
	OC_S_C_NORMALATTACK1 = 0,
	OC_S_C_NORMALATTACK2,
	OC_S_C_FURYATTACK1,
	OC_S_C_FURYATTACK2,
};

// Base 상태
enum OC_S_BaseState
{
	OC_S_B_BEFOREMEET = 0,
	OC_S_B_GENERATE,
	OC_S_B_NORMAL,
	OC_S_B_WHILEFURY,
	OC_S_B_FURY,
	OC_S_B_HITTED,
	OC_S_B_HITTED2,
	OC_S_B_HITTED3,
	OC_S_B_STUN,
	OC_S_B_SHOCKED,
	OC_S_B_DOWN,
	OC_S_B_DEATH
};

// 거리에 따른 상태
enum OC_S_DistanceState
{
	OC_S_D_MOVERANGE = 0,
	OC_S_D_ATTACKRANGE,
	OC_S_D_OUTRANGE,
	OC_S_D_BEFOREMEET
};

// 특정 액션 상태
enum OC_S_ActionState
{
	OC_S_A_BLOCKED = 0,
	OC_S_A_COMBATSTART = 2,
	OC_S_A_RUNFL = 4,
	OC_S_A_RUNFR = 5,
	OC_S_A_SEARCH = 6,
	OC_S_A_BUFF = 21,
	OC_S_A_STDALT1 = 25,
	OC_S_A_STDALT2 = 26,
	OC_S_A_DEAD = 31,
	OC_S_A_DEATH = 32,
	OC_S_A_DOWN3 = 33,
	OC_S_A_DOWN2 = 34,
	OC_S_A_DOWN1 = 35,
	OC_S_A_BACKBE = 39,
	OC_S_A_BACKBL = 40,
	OC_S_A_BACKBS = 41,
	OC_S_A_FRONTBE = 42,
	OC_S_A_FRONTBL = 43,
	OC_S_A_FRONTBS = 44,
	OC_S_A_RUN = 45,
	OC_S_A_SHOCKED = 46,
	OC_S_A_ATTACK1 = 50,
	OC_S_A_ATTACK2 = 51,
	OC_S_A_STDALT = 54,
	OC_S_A_STDIDL = 55,
	OC_S_A_STUN = 56,
	OC_S_A_WALK = 57
};


BEGIN(Client)

class COrc_Soldier final : public CMonster
{
public:
	struct ORC_SOLDIER_DESC
	{
		CMonster::MONSTER_DESC pTarget;
	};

public:
	COrc_Soldier();
	virtual ~COrc_Soldier()  override;

public:
	HRESULT			Initialize();
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<COrc_Soldier> Create(_float3 _vCreatePos);

protected:
	HRESULT		AddRigidBody(string _strModelKey);		// RigidBody 추가
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
	_int		m_iBaseState = OC_S_B_BEFOREMEET;		// Base상태
	_int		m_iDistanceState = OC_S_D_BEFOREMEET;	// 거리에 따른 상태
	_int		m_iActionState = OC_S_A_STDIDL;		// 행동상태

	_float		m_fAttack1Range = 2.f;		// 근접공격 Range
	_float		m_fOutRange = 10000.f;		// 범위에서 나간 Range
	_float		m_fMeetRange = 15.f;		// 조우 Range
	
	_float		m_fCalculateTime = 0.f;					// 계산용 Time
	_float		m_fCombatStartMoveStartTime = 20.f * 0.05f;

	_bool		m_bCombatStartMove = false;

	// 쿨타임
	_bool		m_bAttackReady[4] = { true, false, true , false };
	_float		m_fAttackCoolTime[4] = { 2.5f, 3.f, 2.2f, 2.3f };
	_float		m_fBAttackCoolTime[4] = { 2.5f, 3.f, 2.2f, 2.3f };

	_int		m_NormalAttackCount = 2;	// Normal 상태 공격 종류
	_int		m_FuryAttackCount = 2;		// Fury 상태 공격 종류

	// 버프시 공격이 조금 빨라지고 공격이 세지기
	// 광폭화되는 hp -> 나중에는 샤먼의 버프마법에 걸리면 되는걸로
	_bool		m_bBuff = false;

	_float		m_fFuryHp = 50.f;

	// 광폭화로 빨라지는 속도
	_float		m_fFuryActionSpeed = 1.2f;

	// 광폭화시 속도
	_float		m_fFurySpeed = 3.f;

	// 콜라이더 정리
	const char* m_strATT1Collider1 = "M_Orc_Warrior_001_SK.ao|M_Orc_Warrior_USkdmgA";
	const char* m_strATT2Collider1 = "M_Orc_Warrior_001_SK.ao|M_Orc_Warrior_USkdmgB";

	const char* m_strBUFFCollider = "M_Orc_Shaman_SK.ao|M_Orc_Shaman_USkdmgB";
};

END