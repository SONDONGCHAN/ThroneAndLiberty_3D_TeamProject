#pragma once

#include "Monster.h"
#include "Arrow.h"

class CArrow;
// 공격(근접공격, 일반발사), 샤먼의 버프에 걸리면 버프(공속, 공격력 up),
// 근접공격 패링가능, 화살 패링가능

// CoolTime
enum OC_AC_CoolTime
{
	OC_AC_C_NORMALATTACK1 = 0,
	OC_AC_C_NORMALATTACK2,
	OC_AC_C_FURYATTACK1,
	OC_AC_C_FURYATTACK2,
};

// Base 상태
enum OC_AC_BaseState
{
	OC_AC_B_BEFOREMEET = 0,
	OC_AC_B_GENERATE,
	OC_AC_B_NORMAL,
	OC_AC_B_WHILEFURY,
	OC_AC_B_FURY,
	OC_AC_B_HITTED,
	OC_AC_B_HITTED2,
	OC_AC_B_HITTED3,
	OC_AC_B_STUN,
	OC_AC_B_SHOCKED,
	OC_AC_B_DOWN,
	OC_AC_B_DEATH
};

// 거리에 따른 상태
enum OC_AC_DistanceState
{
	OC_AC_D_MOVERANGE = 0,
	OC_AC_D_ATTACK1RANGE,
	OC_AC_D_ATTACK2RANGE,
	OC_AC_D_OUTRANGE,
	OC_AC_D_BEFOREMEET
};

// 특정 액션 상태
enum OC_AC_ActionState
{
	OC_AC_A_BLOCKED = 0,
	OC_AC_A_COMBATSTART = 1,
	OC_AC_A_RUNFL = 5,
	OC_AC_A_RUNFR = 6,
	OC_AC_A_SEARCH = 7,
	OC_AC_A_STDALT1 = 22,
	OC_AC_A_STDALT2 = 23,
	OC_AC_A_DEAD = 28,
	OC_AC_A_DEATH = 29,
	OC_AC_A_DOWN3 = 30,
	OC_AC_A_DOWN2 = 31,
	OC_AC_A_DOWN1 = 32,
	OC_AC_A_FRONTBE = 39,
	OC_AC_A_FRONTBL = 40,
	OC_AC_A_FRONTBS = 41,
	OC_AC_A_RUN = 42,
	OC_AC_A_SHOCKED = 43,
	OC_AC_A_BUFF = 44,
	OC_AC_A_ATTACK1 = 45,	// 근접공격
	OC_AC_A_ATTACK2 = 46,	// 화살발사
	OC_AC_A_JUMPTO1 = 47,	// 점프 준비
	OC_AC_A_JUMPTO2 = 48,	// 점프 위로
	OC_AC_A_JUMPTO3 = 49,	// 점프 아래
	OC_AC_A_JUMPTO4 = 50,	// 착지
	OC_AC_A_STDALT = 51,
	OC_AC_A_STDIDL = 52,
	OC_AC_A_STUN = 53,
	OC_AC_A_WALK = 54
};

BEGIN(Client)

class COrc_Archer final : public CMonster
{
public:
	struct ORC_ARCHER_DESC
	{
		CMonster::MONSTER_DESC pTarget;
	};

public:
	COrc_Archer();
	virtual ~COrc_Archer()  override;

public:
	HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<COrc_Archer> Create(_float3 _vCreatePos, _float3 _vCertainPos);
	void		SetCertainPos(_float3 _vCertainPos) { m_vCertainPos = _vCertainPos; };
	
protected:
	HRESULT AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	virtual void EnableCollision(const char* _strShapeTag) override;
	virtual void DisableCollision(const char* _strShapeTag) override;

	HRESULT		AddPartObjects(string _strModelKey);
	void		UpdateDistanceState();
	void		UpdateCoolTimeRange(_float _fTimeDelta);
	void		UpdateCoolTimeMelee(_float _fTimeDelta);	// 근거리 쿨
	void		ActiveNormalArrow();
	void		ActiveBuffArrow();

	// BaseState 따른 FSM
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
	_int		m_iBaseState = OC_AC_B_BEFOREMEET;			// Base상태
	_int		m_iDistanceState = OC_AC_D_BEFOREMEET;		// 거리에 따른 상태
	_int		m_iActionState = OC_AC_A_STDALT2;			// 행동상태
	
	_float		m_fAttack1CoolTimeRange = 5.f;				// 근접공격 쿨주는 거리
	_float		m_fAttack1Range = 2.f;						// 근접공격 Range
	_float		m_fAttack2Range = 100.f;					// 화살 Range
	_float		m_fOutRange = 10000.f;						// 범위에서 나간 Range
	_float		m_fMeetRange = 25.f;						// 조우 Range

	_bool		m_bDeadBack = true;				// 죽어서 잠시 밀림

	// 화살 파츠 Render관리
	_float		m_fCalculateTime = 0.f;						// 계산용 Time
	_float		m_fCalculateTime2 = 0.f;						// 계산용 Time
	_float		m_fAttack2ArrowGenTime = 19.f * 0.05f;
	_float		m_fAttack2ArrowOutTime = 48.f * 0.05f;
	_float		m_fFAttack2ArrowGenTime = 19.f * 0.05f / 1.5f;
	_float		m_fFAttack2ArrowOutTime = 48.f * 0.05f / 1.5f;

	_float		m_fCombatStartMoveStartTime = 22.f * 0.05f;

	_bool		m_bPartArrowGen = false;
	_bool		m_bCombatStartMove = false;

	_bool		m_bJumpEnd = false;

	// 쿨타임
	_bool		m_bAttackReady[4] = { false, true, false, true };
	_float		m_fAttackCoolTime[4] = { 3.f, 4.f, 2.f, 3.f };
	_float		m_fBAttackCoolTime[4] = { 3.f, 4.f, 2.f, 3.f };

	_int		m_NormalAttackCount = 2;	// Normal 상태 공격 종류
	_int		m_FuryAttackCount = 2;		// Fury 상태 공격 종류


	// 버프시 공격이 조금 빨라지고 공격이 세지기
	// 광폭화되는 hp -> 나중에는 샤먼의 버프마법에 걸리면 되는걸로
	_bool		m_bBuff = false;

	_float		m_fFuryHp = 50.f;

	// 광폭화로 빨라지는 속도
	_float		m_fFuryActionSpeed = 1.2f; 

	// 광폭화시 속도
	_float		m_fFurySpeed = 2.5f;

	// 시작시 이동할 특정 위치
	_float3		m_vCertainPos = _float3(0.f, 0.f, 0.f);
	_float3		m_vJumpUpPos = _float3(0.f, 0.f, 0.f);
	_float3		m_vJumpStartPos = _float3(0.f, 0.f, 0.f);
	_float		m_vJumpHeight = 1.f;
	_float		m_fJumpUpTime = 0.3f;
	_float		m_fJumpDownTime = 0.f;

	// 화살
	vector<shared_ptr<CArrow>>		m_vecArrow;
	_int		m_iArrowCount = 10;
	
	// 콜라이더 정리
	const char* m_strATT1Collider1 = "M_Orc_Archer_SK.ao|M_Orc_Archer_USkdmgA";

	const char* m_strBUFFCollider = "M_Orc_Shaman_SK.ao|M_Orc_Shaman_USkdmgB";
};

END