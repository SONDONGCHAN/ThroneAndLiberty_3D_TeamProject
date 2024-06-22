#pragma once

#include "Monster.h"
// 패턴
// 공격종류(오른쪽으로, 왼쪽으로, 오른쪽중단으로, 3연격, 2연격, 돌아서공격)
// 특수(달려서 박치기, 특수연격, 포효(광역기), 지면강타(직선장판공격))
// 패턴1: 달려서 박치기, 달리기, 돌아서 공격(패링), 2연격, 포효
// 패턴2: 달려서 박치기, 달리기, 특수연격(패링), 서있다가 지면강타
// 패턴3: 달려서 박치기, 서있다가 지면강타, 달리기, 3연격(패링), 포효
// 패턴4: 달려서 박치기, 달리기, 왼쪽으로 공격, 특수연격(패링), 달리기, 2연격
// 패링가능: 돌아서공격, 3연격의 마지막, 특수연격 마지막
// 특수 시스템: 대쉬중 지형지물에 부딪치면 스턴

// Skill 상태
enum OC_DC_SH_SkillState
{
	OC_DC_SH_S_SKILL1 = 0,	// 패턴1
	OC_DC_SH_S_SKILL2,		// 패턴2
	OC_DC_SH_S_SKILL3,		// 패턴3
	OC_DC_SH_S_SKILL4,		// 패턴4
	OC_DC_SH_S_NOSKILL		// x
};

// Base 상태
enum OC_DC_SH_BaseState
{
	OC_DC_SH_B_BEFOREMEET = 0,
	OC_DC_SH_B_GENERATE,
	OC_DC_SH_B_NORMAL,
	OC_DC_SH_B_BLOCKED,
	OC_DC_SH_B_HITTED,
	OC_DC_SH_B_HITTED2,
	OC_DC_SH_B_HITTED3,
	OC_DC_SH_B_STUN,
	OC_DC_SH_B_SHOCKED,
	OC_DC_SH_B_DOWN,
	OC_DC_SH_B_DEATH
};

// 거리에 따른 상태
enum OC_DC_SH_DistanceState
{
	OC_DC_SH_D_MOVERANGE = 0,
	OC_DC_SH_D_ATTACKRANGE,
	OC_DC_SH_D_OUTRANGE,
	OC_DC_SH_D_BEFOREMEET
};

// 특정 액션 상태
enum OC_DC_SH_ActionState
{
	OC_DC_SH_A_DEAD = 4,
	OC_DC_SH_A_DEATH = 6,
	OC_DC_SH_A_DOWN3 = 9,
	OC_DC_SH_A_DOWN2 = 10,
	OC_DC_SH_A_DOWN1 = 11,
	OC_DC_SH_A_FRONTBE = 18,
	OC_DC_SH_A_BLOCKED = 21,
	OC_DC_SH_A_RUN = 23,
	OC_DC_SH_A_SHOCKED = 25,
	OC_DC_SH_A_FURY = 26,
	OC_DC_SH_A_SPECIAL = 27,
	OC_DC_SH_A_ATTACK1_A = 28,		// 오른쪽으로
	OC_DC_SH_A_ATTACK1_A2 = 29,	// 오른쪽으로
	OC_DC_SH_A_ATTACK1_B = 30,		// 왼쪽으로
	OC_DC_SH_A_ATTACK1_B2 = 31,	// 왼쪽으로
	OC_DC_SH_A_ATTACK1_C = 32,		// 오른쪽중단으로
	OC_DC_SH_A_ATTACK1_C2 = 33,	// 오른쪽중단으로
	OC_DC_SH_A_ATTACK2_A1 = 34,	// 3연격 시작
	OC_DC_SH_A_ATTACK2_A2 = 35,
	OC_DC_SH_A_ATTACK2_A3 = 36,
	OC_DC_SH_A_ATTACK2_A4 = 37,
	OC_DC_SH_A_ATTACK2_A5 = 38,
	OC_DC_SH_A_ATTACK2_A6 = 39,
	OC_DC_SH_A_ATTACK2_B1 = 40,	// 2연격 시작
	OC_DC_SH_A_ATTACK2_B2 = 41,
	OC_DC_SH_A_ATTACK2_B3 = 42,
	OC_DC_SH_A_ATTACK2_B4 = 43,
	OC_DC_SH_A_ATTACK2_C1 = 44,	// 돌기 공격
	OC_DC_SH_A_ATTACK2_C2 = 45,
	OC_DC_SH_A_ATTACK3_A1 = 46,	// 특수 연격
	OC_DC_SH_A_ATTACK3_A2 = 47,
	OC_DC_SH_A_ATTACK3_A3 = 48,
	OC_DC_SH_A_ATTACK3_A4 = 49,
	OC_DC_SH_A_ATTACK3_A5 = 50,
	OC_DC_SH_A_ATTACK3_A6 = 51,
	OC_DC_SH_A_ATTACK3_A7 = 52,
	OC_DC_SH_A_ATTACK4 = 53,		// 직선장판강타공격
	OC_DC_SH_A_DASH3 = 55,
	OC_DC_SH_A_DASH2 = 56,
	OC_DC_SH_A_DASH1 = 57,
	OC_DC_SH_A_SKTRFA1 = 58,
	OC_DC_SH_A_SKTRFA2 = 59,
	OC_DC_SH_A_APPEAR = 60,
	OC_DC_SH_A_STDALT = 62,
	OC_DC_SH_A_STDIDLE = 63,
	OC_DC_SH_A_STUN = 64,
	OC_DC_SH_A_WALK = 65,
};


BEGIN(Client)

class COrc_DarkCommander_Shadow final : public CMonster
{
public:
	struct DARKCOMMANDER_DESC
	{
		CMonster::MONSTER_DESC pTarget;
	};

public:
	COrc_DarkCommander_Shadow();
	virtual ~COrc_DarkCommander_Shadow()  override;

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
	static shared_ptr<COrc_DarkCommander_Shadow> Create(_float3 _vCreatePos);
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

	_bool		Update2ComboState(_float _fTimeDelta);		// 2연격 업데이트
	_bool		UpdateSAttackState(_float _fTimeDelta);		// 돌기공격 업데이트
	_bool		UpdateRAttackState(_float _fTimeDelta);		// 오른쪽공격 업데이트
	_bool		UpdateRMAttackState(_float _fTimeDelta);	// 오른쪽중단공격 업데이트
	_bool		UpdateLAttackState(_float _fTimeDelta);		// 오른쪽공격 업데이트
	_bool		Update3ComboState(_float _fTimeDelta);		// 3연격 업데이트
	_bool		UpdateSpecialState(_float _fTimeDelta);		// 특수연격 업데이트

	void		ResetTime();
	void		CheckDead();
	void		CheckAttacked();
	_bool		CheckAnimFinish();
	
private:
	_int		m_iBaseState = OC_DC_SH_B_BEFOREMEET;			// Base상태
	_int		m_iDistanceState = OC_DC_SH_D_BEFOREMEET;		// 거리에 따른 상태
	_int		m_iActionState = OC_DC_SH_A_STDIDLE;			// 행동상태
	_int		m_iSkillState = OC_DC_SH_S_NOSKILL;			// 스킬상태

	_float		m_fMeleeRange = 2.f;					// 근접공격 Range
	_float		m_fAttack1Range = 3.f;					// 근접공격 Range
	_float		m_fDashEndRange = 5.f;					// 대쉬해서 부딪치는 거리
	_float		m_fOutRange = 10000.f;					// 범위에서 나간 Range
	_float		m_fMeetRange = 100.f;					// 조우 Range
	
	_float		m_fCalculateTime = 0.f;						// 계산용 Time
	_float		m_fCalculateTime2 = 0.f;					// 계산용 Time2

	_bool		m_bSkill1State[10] = { 0, };
	_bool		m_bSkill2State[10] = { 0, };
	_bool		m_bSkill3State[10] = { 0, };
	_bool		m_bSkill4State[10] = { 0, };

	_bool		m_bShadowActiveOn = false;

	// 점멸 혹은 점프할 위치
	_float3		m_vMovePos = _float3(0.f, 0.f, 0.f);

	// 회전갱신
	void		UpdateDir(_float _fTimeDelta);

	// 점프 방향, 속도
	_float3		m_vJumpNormalMoveDir = _float3(0.f, 0.f, 0.f);
	_float		m_fJumpMoveSpeed = 1.f;

	// 애니메이션 속도 조절
	_float		m_fAnimSpeed = 1.f;
	_float		m_fActionAnimSpeed[100] = { 0, };
	_float		m_fAttack4AnimSpeed = 0.7f;
	_float		m_fBeforeQuakeTime = 2.5f;

	// 키프레임 시간 관련
	
	// 패링 스택
	_int		m_iParryingStack = 0;
	_int		m_iBParryingStack = 3;

	_int		m_iSAttParryStack = 1;
	_int		m_i3ComboAttParryStack = 1;
	_int		m_iSpecialAttParryStack = 2;
	_int		m_iParryType = 0;		// 0: 돌아서 공격, 1: 3연격의 마지막, 2: 특수연격 마지막

	// InSkill 관련

	// 공격마다 미끄러지는 속도
	_float		m_fDash3SlideSpeed = 50.f;
	_float		m_f2Combo1SlideSpeed = 10.f;
	_float		m_f2Combo2SlideSpeed = 10.f;
	_float		m_f3Combo1SlideSpeed = 10.f;
	_float		m_f3Combo2SlideSpeed = 10.f;
	_float		m_f3Combo3SlideSpeed = 10.f;
	_float		m_fSAttackSlideSpeed = 10.f;
	_float		m_fRAttackSlideSpeed = 10.f;
	_float		m_fLAttackSlideSpeed = 10.f;
	_float		m_fSCombo1SlideSpeed = 10.f;
	_float		m_fSCombo2SlideSpeed = 2.f;
	_float		m_fSCombo3SlideSpeed = 25.f;
	_float		m_fSCombo4SlideSpeed = 20.f;

	// 공용
	_float		m_fRunSpeed = 8.f;

	// Skill1
	_float		m_fSkill1StandTime = 1.f;
	_float		m_fSkill1DashSpeed = 20.f;
	_float		m_fSkill1TimeBeforeFury = 1.f;

	// Skill2
	_float		m_fSkill2StandTime = 1.f;
	_float		m_fSkill2DashSpeed = 20.f;
	_float		m_fSkill2MovePos = 3.f;
	
	// Skill3
	_float		m_fSkill3StandTime = 1.f;
	_float		m_fSkill3DashSpeed = 20.f;
	_float		m_fSkill3TimeBeforeFury = 1.5f;

	// Skill4
	_float		m_fSkill4StandTime = 1.f;
	_float		m_fSkill4DashSpeed = 20.f;

	// 스킬순서 고정
	_int		m_iSkillOrder[100] = { 3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3 };
	//_int		m_iSkillOrder[100] = { 1,2,1,2,1,2,1,2,1,2 };
	_int		m_iPatternNum = 0;

	// 콜라이더 정리
	// 3연격의 마지막
	const char* m_strATT1Collider = "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB.005";
	// 돌아서 공격
	const char* m_strATT2Collider = "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB_3.001";
	// 특수연격 막타
	const char* m_strATT3Collider = "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD.006";
	
	_bool		m_bBug = false;
};

END