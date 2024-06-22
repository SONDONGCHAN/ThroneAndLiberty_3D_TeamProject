#pragma once

#include "Monster.h"

// 패턴
// 공격종류(일반공격, 내려찍기(한번)미끄러져서, 내려찍기(여러번미끄러져서)
// 회전공격(1,2, 다가가며, 1은 빠르고 각도가 적게 변함, 2는 느리고 각도가 크게 변함), 점프내려찍기(1,2, 1은 배로 찍기, 2는 칼로 찍기))
// 패턴1: 점프내려찍기1 -> 뛰다가(멀면) -> 내려찍기1 -> 회전공격1 -> 스턴상태
// 패턴2: 점프내려찍기2 -> 뛰다가(멀면) -> 내려찍기1 -> 뛰다가(멀면) -> 일반공격
// 패턴3: 회전공격2 -> 스턴상태 -> 걷다가 뜀(멀면) -> 내려찍기2(빠르게) -> 점프내려찍기2 -> 뛰다가(멀면) -> 일반공격
// 패턴4: 점프내려찍기2 -> 점프내려찍기2 -> 점프내려찍기2 -> 뛰다가(멀면) -> 내려찍기2(빠르게)
// 패링가능: 일반공격

// Skill 상태
enum MI_B_SH_SkillState
{
	MI_B_SH_S_SKILL1 = 0,	// 패턴1
	MI_B_SH_S_SKILL2,		// 패턴2
	MI_B_SH_S_SKILL3,		// 패턴3
	MI_B_SH_S_SKILL4,		// 패턴4
	MI_B_SH_S_NOSKILL		// x
};

// Base 상태
enum MI_B_SH_BaseState
{
	MI_B_SH_B_BEFOREMEET = 0,
	MI_B_SH_B_GENERATE,
	MI_B_SH_B_NORMAL,
	MI_B_SH_B_BLOCKED,
	MI_B_SH_B_HITTED,
	MI_B_SH_B_HITTED2,
	MI_B_SH_B_HITTED3,
	MI_B_SH_B_STUN,
	MI_B_SH_B_SHOCKED,
	MI_B_SH_B_DOWN,
	MI_B_SH_B_DEATH
};

// 거리에 따른 상태
enum MI_B_SH_DistanceState
{
	MI_B_SH_D_MOVERANGE = 0,
	MI_B_SH_D_ATTACKRANGE,
	MI_B_SH_D_OUTRANGE,
	MI_B_SH_D_BEFOREMEET
};

// 특정 액션 상태
enum MI_B_SH_ActionState
{
	MI_B_SH_A_BLOCKED = 0,
	MI_B_SH_A_COMBATSTART = 5,
	MI_B_SH_A_DEAD = 6,
	MI_B_SH_A_DEATH = 7,
	MI_B_SH_A_DOWN3 = 10,
	MI_B_SH_A_DOWN2 = 11,
	MI_B_SH_A_DOWN1 = 12,
	MI_B_SH_A_FRONTBE = 16,
	MI_B_SH_A_RUN = 22,
	MI_B_SH_A_RUNFL = 23,
	MI_B_SH_A_RUNFR = 24,
	MI_B_SH_A_SEARCH = 25,
	MI_B_SH_A_SHOCKED = 26,
	MI_B_SH_A_ATTACK1_A = 29,		// 일반공격
	MI_B_SH_A_ATTACK1_B = 30,
	MI_B_SH_A_ATTACK1_C = 31,
	MI_B_SH_A_ATTACK2_A = 32,		// 내려찍기2
	MI_B_SH_A_ATTACK2_B = 33,
	MI_B_SH_A_ATTACK3_A = 35,		// 회전공격1
	MI_B_SH_A_ATTACK3_B = 36,
	MI_B_SH_A_ATTACK4_A = 37,		// 회전공격2
	MI_B_SH_A_ATTACK4_B = 38,
	MI_B_SH_A_ATTACK5_A = 39,		// 내려찍기1
	MI_B_SH_A_ATTACK5_B = 40,
	MI_B_SH_A_ATTACK5_C = 41,
	MI_B_SH_A_JUMPATTACK1_C = 42,
	MI_B_SH_A_JUMPATTACK1_B = 43,
	MI_B_SH_A_JUMPATTACK1_A = 44,	// 점프공격1
	MI_B_SH_A_JUMPATTACK2_C = 45,
	MI_B_SH_A_JUMPATTACK2_B = 46,
	MI_B_SH_A_JUMPATTACK2_A = 47,	// 점프공격2
	MI_B_SH_A_APPEAR = 48,			// 뛰어나오기
	MI_B_SH_A_DANCE1 = 49,		
	MI_B_SH_A_DANCE2 = 50,
	MI_B_SH_A_DANCE3 = 51,
	MI_B_SH_A_STDALT = 57,			// 전투자세
	MI_B_SH_A_STDALT1 = 58,
	MI_B_SH_A_STDALT2 = 59,
	MI_B_SH_A_STDIDLE = 60,
	MI_B_SH_A_STUNE = 61,
	MI_B_SH_A_STUNL = 62,
	MI_B_SH_A_STUNS = 63,
	MI_B_SH_A_WALK = 64
};


BEGIN(Client)

class CMidget_Berserker_Shadow final : public CMonster
{
public:
	struct LIVINGARMOR_SOLDIER_DESC
	{
		CMonster::MONSTER_DESC pTarget;
	};

public:
	CMidget_Berserker_Shadow();
	virtual ~CMidget_Berserker_Shadow()  override;

public:
	HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public: /* UI Get Function */
	_int GetCurParryingStack() { return m_iParryingStack; }
	_int GetMaxParryingStack() { return m_iBParryingStack; }
	_float GetHPRatio();
	_int GetBossActionState() { return m_iActionState; }

public:
	static shared_ptr<CMidget_Berserker_Shadow> Create(_float3 _vCreatePos);
	void	SetShadowActiveOn(_bool _bActiveOn) { m_bShadowActiveOn = _bActiveOn; }

protected:
	HRESULT AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag);		// 닿는 순간
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag);	// 닿아 있을 때
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag);		// 끝날때

	virtual void EnableCollision(const char* _strShapeTag) override;
	virtual void DisableCollision(const char* _strShapeTag) override;

	void		ResetBoss();		// 보스 리셋

	HRESULT		AddPartObjects(string _strModelKey);
	void		UpdateDistanceState();

	// 시간 구간에서 특정속도로 보는방향으로 미끄러지게하기
	void		UpdateMoveSlide(_float _fSlideStart, _float _fSlideEnd, _float _fSpeed, _float _fTimeDelta);
	void		UpdateAttack2MoveSlide(_float _fTimeDelta);
	_int		CheckParryingStack(_int _iParryingStack);

	// BaseState에 따른 FSM
	void		FSM_BeforeMeet(_float _fTimeDelta);
	void		FSM_Generate(_float _fTimeDelta);
	void		FSM_Normal(_float _fTimeDelta);
	void		FSM_Blocked(_float _fTimeDelta);
	void		FSM_Hitted(_float _fTimeDelta);
	void		FSM_Hitted2(_float _fTimeDelta);
	void		FSM_Hitted3(_float _fTimeDelta);
	void		FSM_Stun(_float _fTimeDelta);
	void		FSM_Shocked(_float _fTimeDelta);
	void		FSM_Down(_float _fTimeDelta);
	void		FSM_Death(_float _fTimeDelta);

	void		CheckDead();
	void		CheckAttacked();
	
private:
	_int		m_iBaseState = MI_B_SH_B_BEFOREMEET;			// Base상태
	_int		m_iDistanceState = MI_B_SH_D_BEFOREMEET;		// 거리에 따른 상태
	_int		m_iActionState = MI_B_SH_A_STDIDLE;			// 행동상태
	_int		m_iSkillState = MI_B_SH_S_NOSKILL;				// 스킬상태

	_float		m_fMeleeRange = 2.f;					// 근접공격 Range
	_float		m_fAttack1Range = 3.f;					// 내려찍기 Range
	_float		m_fOutRange = 10000.f;					// 범위에서 나간 Range
	_float		m_fMeetRange = 100.f;					// 조우 Range

	_float		m_fCalculateTime = 0.f;						// 계산용 Time
	_float		m_fCalculateTime2 = 0.f;					// 계산용 Time2

	_bool		m_bSkill1State[10] = { 0, };
	_bool		m_bSkill2State[10] = { 0, };
	_bool		m_bSkill3State[10] = { 0, };
	_bool		m_bSkill4State[10] = { 0, };

	_bool		m_bShadowActiveOn = false;

	// 회전공격 이후 스턴중
	_bool		m_bWhileStun = false;

	// 점멸 혹은 점프할 위치
	_float3		m_vMovePos = _float3(0.f, 0.f, 0.f);

	// 점프 속도와 방향 갱신
	void		UpdateJumpDirSpeed1();	// Jump1 갱신
	void		UpdateJumpDirSpeed2();	// Jump2 갱신

	// 회전갱신
	void		UpdateDir(_float _fTimeDelta);

	// 점프 방향, 속도
	_float3		m_vJumpNormalMoveDir = _float3(0.f, 0.f, 0.f);
	_float		m_fJumpMoveSpeed = 1.f;

	// 애니메이션 속도 조절
	_float		m_fAnimSpeed = 1.f;
	_float		m_fActionAnimSpeed[100] = { 0, };

	// 키프레임 시간 관련
	_float		m_fJump1UpTime = m_fKeyFrameSpeed * 5;
	_float		m_fSpin1EndTime = m_fKeyFrameSpeed * 93;
	_float		m_fAttack2Move1EndTime = m_fKeyFrameSpeed * 8;
	_float		m_fAttack2Move2StartTime = m_fKeyFrameSpeed * 24;
	_float		m_fAttack2Move2EndTime = m_fKeyFrameSpeed * 28;
	_float		m_fAttack2Move3StartTime = m_fKeyFrameSpeed * 36;
	_float		m_fAttack2Move3EndTime = m_fKeyFrameSpeed * 41;

	// 패링 스택
	_int		m_iParryingStack = 0;
	_int		m_iBParryingStack = 3;

	_int		m_iAtt1ParryStack = 1;
	_int		m_iAtt5ParryStack = 2;
	_int		m_iJumpAtt2ParryStack = 3;
	_int		m_iParryType = 0;		// 0: 내려찍기1, 1: 내려치기, 2: 점프 내려치기

	// InSkill 관련

	// 공격마다 미끄러지는 속도
	_float		m_fA2SlideSpeed = 5.f;
	_float		m_fA5SlideSpeed = 3.f;

	// Skill1
	_float		m_fSkill1StandTime = 1.f;
	_float		m_fSkill1RunSpeed = 5.f;
	_float		m_fSkill1SpinSpeed = 8.f;

	// Skill2
	_float		m_fSkill2StandTime = 1.f;
	_float		m_fSkill2RunSpeed = 5.f;

	// Skill3
	_float		m_fSkill3StandTime = 1.f;
	_float		m_fSkill3WalkSpeed = 1.f;
	_float		m_fSkill3WalkTime = 2.f;
	_float		m_fSkill3RunSpeed = 5.f;
	_float		m_fSkill3SpinSpeed = 4.f;

	// Skill4
	_float		m_fSkill4StandTime = 1.f;
	_float		m_fSkill4RunSpeed = 5.f;

	// 스킬순서 고정
	_int		m_iSkillOrder[100] = { 0,3,2,1,3,1,2,0,0,3,2,1,3,1,2,0,0,3,2,1,3,1,2,0,0,3,2,1,3,1,2,0 };
	//_int		m_iSkillOrder[100] = { 1,1,1,1,1,1,1,1,1 };
	_int		m_iPatternNum = 0;

	// 콜라이더 정리
	// 근접 내려찍기1
	const char* m_strATT5Collider = "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkdmgD.001";
	// 근접 검 내려치기
	const char* m_strATT1Collider = "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkdmgA.001";
	// 점프 칼 내려치기
	const char* m_strJumpATT2Collider = "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkmovBE";

	_bool		m_bBug = false;
};

END