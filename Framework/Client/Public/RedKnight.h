#pragma once

#include "Monster.h"
#include "Pillar.h"
#include "FallingSword.h"
#include "Slash.h"
#include "BindMagic.h"
#include "RedKnight_Shadow.h"
#include "Midget_Berserker_Shadow.h"
#include "Orc_DarkCommander_Shadow.h"
//시스템: 각각 패턴이 끝날때 플레이어와의 위치를 기준으로 근거리면 근거리 패턴으로, 중거리면 중거리 패턴으로, 장거리면 장거리 패턴으로
//패링 : 근접공격(2콤보, 3콤보)중에 마지막 공격을 패링하면 패링스택이 1이 오르고 보스에게서 빠른 패링모션이 나오게 된다(짧은 공격타임)
//플레이어에게 대쉬공격을 패링하면 패링스택이 3이 오르고 보스에게서 빠른 패링모션이 나오게 된다(짧은 공격타임)
//패링스택 : 패링을 계속 하여 패링스택이 5가 되게 되면 0으로 초기화 되고 동시에 상태이상이 먹히는 느린 패링모션이 뜨고 동시에 현재 진행하던 패턴이 강제 종	료 된다(공격타임)
//
//1페이즈
//근거리
//1패턴 : 순간이동 제스쳐와 함께 플레이어 근처로 순간이동 후에 3콤보 공격 이후에 플레이어를 관통대쉬를 하여 중거리로 간 후에 큰 참격을 플레이어에게 날림
//2패턴 : 그림자 소환 제스처 1번 이후 점프하여 땅을 찍는 그림자를 소환한후에 다음 그림자 소환 제스처 이후에
//2콤보 공격을 하는 그림자 2마리를 차례로 소환 이후에 플레이어를 관통대쉬를 한 후에 큰 참격을 플레이어에게 날림
//
//중거리
//1패턴 : 빠른참격 모션과 함께 빠른참격을 날린후에 플레이어를 향해 찍기 공격을 하여 장판범위 공격을 한 후에 순간이동 제스쳐와 함께 플레이어 근처로
//순간이동 한 후에 2콤보 공격을 하고 점프 이동
//2패턴 : 참격을 날리는 그림자 2마리를 자신의 주위에 차례로 소환후에 플레이어 근처에 2콤보 공격을 하는 2마리를 차례로 소환한후에 점프 이동
//
//원거리
//1패턴 : 빠르게 3연 참격을 날린후에 점프한후에 플레이어에게 관통대쉬를 하고 이후에 플레이어 방향으로 타겟팅 대쉬를 하고 2콤보 공격->근거리
//2패턴 : 플레이어를 향해 관통대쉬를 하는 그림자 2마리를 차례로 소환한후에 플레이어의 배후로 점프하여 플레이어에게 타겟팅 대쉬를 하고 3콤보 공격->근거리
//
//피가 75퍼센트로 닳게 되면 전면의 왕좌위로 보스가 점프하여 이동하고 특수페이즈1로 변경
//
//특수페이즈1
//1. 특수 모션과 함께 필드에 5개의 거대 검을 떨어뜨림
//2. 그림자 소환 모션과 함께 4마리의 그림자를 왕좌위 자신의 배후에 소환하고 잠시후 그림자들은 필드위 각각의 위치로 점프하여 이동한다
//3. 검중에 하나가 빨간색으로 빛나기 시작하고 위치를 잡은 그림자들은 플레이어를 향해 큰 참격을 날리기 시작하는데 빨간 검으로 그림자들의 참격이 맞게 유도하여 검의 피를 깎아야 한다 그러면 검의 빨간 빛이 꺼지게 되고 다른 검이 빛이 나게 된다 이를 3개의 검의 빛이 꺼질때까지 반복한다
//4. 그림자들은 사망모션과 함께 죽고, 왕좌위의 보스는 스턴이 걸리는데, 피를 깎은 검들은 황금빛이 되어 떠오르고 왕좌위 보스에게 가서 떨어지고 보스는 왕좌에서 필드로 엎어지고 몇초후 보스가 다시 일어나 2페이즈로 가게된다

enum RK_SkillState
{
	RK_S_INSKILL1 = 0,	// 가까운거리 일정거리이내 1)
	RK_S_INSKILL2,		// 가까운거리 일정거리이내 2)
	RK_S_MIDDLESKILL1,	// 중간거리	 일정거리이상 1)
	RK_S_MIDDLESKILL2,	// 중간거리   일정거리이상 2)
	RK_S_OUTSKILL1,		// 먼거리 1)
	RK_S_OUTSKILL2,		// 먼거리 2)
	RK_S_OUTSKILL3,		// 먼거리 3)
	RK_S_OUTSKILL4,		// 먼거리 4)
	RK_S_SPECIAL1,		// 기둥 떨어뜨리기
	RK_S_NOSKILL		// x
};

// Phase
enum RK_Phase
{
	RK_P_NORMAL = 0,
	RK_P_GIMIC1,
	RK_P_TOFURY1,
	RK_P_FURY1,
	RK_P_GIMIC2,
	RK_P_TOFURY2,
	RK_P_FURY2
};

// Base 상태
enum RK_BaseState
{
	RK_B_BEFOREMEET = 0,
	RK_B_BEALERT,
	RK_B_ALERT,
	RK_B_NORMAL,
	RK_B_GIMICPATTERN1,
	RK_B_WHILEFURY,
	RK_B_FURY,
	RK_B_GIMICPATTERN2,
	RK_B_WHILEFURY2,
	RK_B_FURY2,
	RK_B_BLOCKED,
	RK_B_HITTED,
	RK_B_HITTED2,
	RK_B_HITTED3,
	RK_B_STUN,
	RK_B_SHOCKED,
	RK_B_DOWN,
	RK_B_DEATH
};

// 거리에 따른 상태
enum RK_DistanceState
{
	RK_D_IN1RANGE = 0,			// 가까운거리
	RK_D_IN2RANGE,				// 중간거리
	RK_D_OUTRANGE,				// 원거리
	RK_D_BEFOREMEET			
};

// 특정 액션 상태
enum RK_ActionState
{
	RK_A_BLOCKED = 0,
	RK_A_DASHATT1 = 3,
	RK_A_DASHWHILE = 5,
	RK_A_DASHSTART = 6,
	RK_A_SPECIAL1 = 7,				// 점프후 찍기(광역 장판공격)
	RK_A_SHORTJUMP = 8,
	RK_A_SPECIAL2 = 9,				// 점프후 2번찍기(초광역 장판공격)
	RK_A_RUNFL = 12,
	RK_A_RUNFR = 13,
	RK_A_MAGIC = 15,				// 속박마법
	RK_A_TOFURY = 16,
	RK_A_CHARGESLASH = 17,			// 차징슬래쉬
	RK_A_FRONTQUAKE = 18,			// 전방지진
	RK_A_GOUP = 19,
	RK_A_DEATH = 27,
	RK_A_DIE = 28,
	RK_A_DOWN3 = 29,
	RK_A_DOWN2 = 30,
	RK_A_DOWN1 = 31,
	RK_A_FRONTDOWN3 = 32,
	RK_A_FRONTDOWN2 = 33,
	RK_A_FRONTDOWN1 = 34,
	RK_A_HITTED = 38,
	RK_A_RUN = 41,
	RK_A_SHOCKED = 42,
	RK_A_GROUNDMAGIC = 43,			// 일직선 장판 마법
	RK_A_SUMMONSHADOW = 44,			// 그림자 소환
	RK_A_MAGIC2 = 45,				// 검 떨어뜨리기
	RK_A_TELEPORT = 46,				// 텔레포트
	RK_A_WAITTOALERT = 47,
	RK_A_WAIT = 48,
	RK_A_ALERTTOWAIT = 49,
	RK_A_SHOTSWORD = 50,			// 검 쳐서 날리기
	RK_A_ATT_D1 = 52,
	RK_A_ATT_D2 = 53,
	RK_A_ATT_D3 = 54,
	RK_A_ATT_D4 = 55,
	RK_A_ATT_L1 = 57,
	RK_A_ATT_L2 = 58,
	RK_A_ATT_L3 = 59,
	RK_A_ATT_L4 = 60,
	RK_A_ATT_LM1 = 62,
	RK_A_ATT_LM2 = 63,
	RK_A_ATT_U1 = 65,
	RK_A_ATT_U2 = 66,
	RK_A_ATT_U3 = 67,
	RK_A_ATT_U4 = 68,
	RK_A_ATT_U5 = 69,
	RK_A_SUMMON = 71,				// 검 떨어뜨리기
	RK_A_ALT = 72,
	RK_A_IDLE = 73,
	RK_A_STUN = 74,
	RK_A_WALK = 75
};

BEGIN(Client)

class CRedKnight final : public CMonster
{
public:
	struct REDKNIGHT_DESC
	{
		CMonster::MONSTER_DESC pTarget;
	};

public:
	CRedKnight();
	virtual ~CRedKnight()  override;

public:
	HRESULT			Initialize();
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
	static shared_ptr<CRedKnight> Create(_float3 _vCreatePos);
	
protected:
	void		ActivePillar();			// 기둥 소환
	void		ActivePillarMoveUp();	// 기둥 올라오게
	void		ActivePillarMoveDown();	// 기둥 내려가게
	void		ActiveSword();		// 검 소환
	void		ActiveGimicShadow4();					// 기믹 분신들 소환
	void		Active2ComboShadow(_int _iNum);			// 2콤보 분신들 소환
	void		Active3ComboShadow(_int _iNum);			// 3콤보 분신들 소환
	void		ActiveJumpAttackShadow(_int _iNum, _int _iType = 0);		// 장판공격 분신들 소환
	void		ActiveSlashShadow(_int _iNum);			// 참격 분신들 소환
	void		ActiveDashThroughShadow(_int _iNum, _int _iType = 0);	// 돌진 분신들 소환
	void		ActiveBindGroundShadow(_int _iNum, _int _iType = 0);	// 속박장판 분신들 소환
	void		ActiveQuakeShadow(_int _iNum, _int _iType = 0);	// 속박장판 분신들 소환
	void		ActiveMidgetBerserkerShadow();			// 고블린 보스 유령 소환
	void		ActiveDarkOrcShadow();					// 오크 보스 유령 소환
	HRESULT		AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때
	
	virtual void EnableCollision(const char* _strShapeTag) override;
	virtual void DisableCollision(const char* _strShapeTag) override;
	
	void		ResetBoss();		// 보스 리셋
	void		ResetTimer();
	void		ResetSkillState();	// 스킬 상태 리셋
	void		ActiveSlash(_int _iSlashType);
	void		ActiveBindMagic(_int _iBindMagicType, _int _iCircleMagicType = 0, _float _fReadyTime = 0.f);
	
	HRESULT		AddPartObjects(string _strModelKey);
	void		UpdateDistanceState();
	// 공격중에 회전 갱신
	void		UpdateAttackDir(_float _fTimeDelta);
	// 느린참격중에 회전 갱신
	void		UpdateSlashDir(_float _fTimeDelta);

	// 시간 구간에서 특정속도로 보는방향으로 미끄러지게하기
	void		UpdateMoveSlide(_float _fSlideStart, _float _fSlideEnd, _float _fSpeed, _float _fTimeDelta);
	// 원위에서 위치 갱신
	void		UpdateCirclePos(_bool _bDir, _float _fTimeDelta);
	// 점프 속도와 방향 갱신
	void		UpdateJumpDirSpeed();
	void		UpdateJumpToThroneDirSpeed();
	void		UpdateSpecial1DirSpeed();
	void		UpdateSpecial2DirSpeed();

	_float3		CalculateBindary(_float3 _vPos);		// 도출 위치가 경계선을 지나버리면 그 경계까지만 가도록

	_int		CheckParryingStack(_int _iParryingStack);

	_float3		CalculateTeleportFrontPos();	// 플레이어 앞으로 순간이동할 위치 계산
	_float3		CalculateTeleportBackPos();		// 플레이어 앞으로 순간이동할 위치 계산
	_float3		CalculateTeleportFarBackPos();		// 플레이어 앞으로 순간이동할 위치 계산
	_float3		CalculateCircleRunPos();		// 돌기 시작할 위치 계산
	_float3		CalculateDodgePos();			// 회피점프시 점프이동할 위치 계산(중거리)
	_float3		CalculateDodge2Pos();			// 회피점프시 점프이동할 위치 계산(장거리)
	

	// 랜덤
	_float3		RandomJumpFrontOrBackPos();
	_float3		JumpFarBackPos();
	_float3		RandomBlinkPos();			// 플레이어 가깝게 무작위 점멸 위치
	_float3		RandomBlinkSlashPos();		// 플레이어 참격 위치 무작위 점멸 위치
	_float3		RandomBlinkDashPos();		// 플레이어 돌진 위치 무작위 점멸 위치
	_float3		RandomDashOutPos(_float _fDashOutRange = 10.f, _int _iType = 0);			// 랜덤 경유 돌진위치
	// 랜덤 맵 사이드 위치(맵 나오면)
	_int		RandomSlowSlash();
	_int		RandomNormal2ComboStartAttack();
	_int		RandomNormal3ComboStartAttack();

	// 특정 패턴 애니메이션 자동으로 돌리기
	// 2Combo
	_bool		Update2ComboAttackState(float _fTimeDelta);
	// 3Combo
	_bool		Update3ComboAttackState(float _fTimeDelta);
	// 참격
	_bool		UpdateAttackState(float _fTimeDelta);
	// 관통대쉬
	_bool		UpdateThroughDashState(float _fTimeDelta, _float _fSpeed = 100.f, _float _fDistance = 30.f);
	// 대쉬중 관통대쉬 시작
	_bool		UpdateAThroughDashState(float _fTimeDelta, _float _fSpeed = 150.f, _float _fDistance = 30.f);
	// 앞까지 대쉬
	_bool		UpdateFrontDashState(float _fTimeDelta, _float _fSpeed = 100.f, _float _fFrontDistance = 4.f);
	// 점프상태에서 관통대쉬
	_bool		UpdateJumpUpDashState(float _fTimeDelta, _float _fSpeed = 100.f, _float _fDistance = 30.f);
	// 점프해서 관통대쉬
	_bool		UpdateJumpDashState(float _fTimeDelta, _float _fSpeed = 100.f, _float _fDistance = 30.f);
	// 특정지점으로 빠지는 대쉬
	_bool		UpdatePositionDashState(float _fTimeDelta, _float _fSpeed = 100.f);
	
	// BaseState에 따른 FSM
	void		FSM_BeforeMeet(_float _fTimeDelta);
	void		FSM_Alert(_float _fTimeDelta);
	void		FSM_BeAlert(_float _fTimeDelta);
	void		FSM_Normal(_float _fTimeDelta);
	void		FSM_GimicPattern1(_float _fTimeDelta);
	void		FSM_WhileFury(_float _fTimeDelta);
	void		FSM_Fury(_float _fTimeDelta);
	void		FSM_GimicPattern2(_float _fTimeDelta);
	void		FSM_WhileFury2(_float _fTimeDelta);
	void		FSM_Fury2(_float _fTimeDelta);
	void		FSM_Blocked(_float _fTimeDelta);
	void		FSM_Hitted(_float _fTimeDelta);
	void		FSM_Hitted2(_float _fTimeDelta);
	void		FSM_Hitted3(_float _fTimeDelta);
	void		FSM_Stun(_float _fTimeDelta);
	void		FSM_Shocked(_float _fTimeDelta);
	void		FSM_Down(_float _fTimeDelta);
	void		FSM_Death(_float _fTimeDelta); 

	void		CheckGimicPattern1();
	void		CheckGimicPattern2();
	void		CheckDead();
	void		CheckAttacked();

private:
	_bool		m_bFirstCinematic = false;				// 첫번째 연출조건
	_bool		m_bFirstCinematicEnd = false;			// 첫번째 연출 끝났는지 여부
	_int		m_iEffectIdx = 0;

	_int		m_iBaseState = RK_B_BEFOREMEET;			// Base상태
	_int		m_iDistanceState = RK_D_BEFOREMEET;		// 거리에 따른 상태
	_int		m_iActionState = RK_A_IDLE;				// 행동상태
	_int		m_iSkillState = RK_S_NOSKILL;
	
	_float		m_fMeleeRange = 4.f;		// 근접공격 Range
	_float		m_fMelee2Range = 2.f;		// 근접공격 Range(점프용)
	_float		m_fMelee3Range = 20.f;		// 배후로 멀리가는 Range 점프
	_float		m_fPillarNearRange = 10.f;
	_float		m_fPillarMiddleRange = 25.f;
	_float		m_fPillarFarRange = 40.f;
	_float		m_fBlinkSlashRange = 10.f;
	_float		m_fBlinkDashRange = 30.f;
	_float		m_fShortRange = 10.f;		// 플레이어가 가까운 거리에서 가까운 거리
	_float		m_fInRange = 30.f;			// 플레이어가 가까운 거리 
	_float		m_fOutRange = 10000.f;		// 플레이어가 먼 거리
	_float		m_fDashOutRange = 10.f;
	
	_float		m_fFightRange = 40.f;
	_float		m_fMeetRange = 50.f;		// 조우 Range
	
	_float		m_fCalculateTime = 0.f;						// 계산용 Time
	_float		m_fCalculateTime2 = 0.f;					// 계산용 Time2
	_float		m_fCalculateTime3 = 0.f;					// 계산용 Time3
	_float		m_fCalculateTime4 = 0.f;					// 계산용 Time3

	_int		m_i2ComboType = 10;		// 2콤보의 종류
	_int		m_i3ComboType = 10;		// 3콤보의 종류
	
	// 애니메이션 속도 조절
	_float		m_fAnimSpeed = 1.f;
	_float		m_fActionAnimSpeed[100] = { 0, };

	// 키프레임 시간 관련
	_float		m_fFastJumpStartTime = m_fKeyFrameSpeed * 12;
	_float		m_fFastJumpEndTime = m_fKeyFrameSpeed * 20;
	_float		m_fSpecial1JumpStartTime = m_fKeyFrameSpeed * 11;
	_float		m_fSpecial1JumpEndTime = m_fKeyFrameSpeed * 31;
	_float		m_fSpecial2JumpStartTime = m_fKeyFrameSpeed * 11;
	_float		m_fSpecial2JumpEndTime = m_fKeyFrameSpeed * 31;
	_float		m_fChargeSlashEndDirTime = m_fKeyFrameSpeed * 50.f * 1.f/ 1.f;
	_float		m_fChargeSlashStartSlashTime = m_fKeyFrameSpeed * 58.f / 2.f * 1.f / 1.f;
	_float		m_fBindMagicStartTime = m_fKeyFrameSpeed * 31.f / 2.f;
	_float		m_fBindMagicFireTime = m_fKeyFrameSpeed * 30.f / 2.f;
	_float		m_fSpecial2FirstTime = m_fKeyFrameSpeed * 33.f;
	_float		m_fSpecial2SecondTime = m_fKeyFrameSpeed * 144.f;
	_float		m_fQuakeStartTime = m_fKeyFrameSpeed * 40.f;
	_float		m_fJumpAttackEndTime = m_fKeyFrameSpeed * 38.f;
	_float		m_fFinalRoarEndTime = m_fKeyFrameSpeed * 40.f * 4.f;
	_float		m_fQuakeStartTime2 = m_fKeyFrameSpeed * 48.f;

	_bool		m_bQuakeSoundStart = false;
	_bool		m_bQuakeCameraStart = false;
	_bool		m_bJumpAttackSoundStart = false;
	_bool		m_bFinalRoarSoundStart = false;
	
	// InSkill 관련
	_bool		m_bSkill1State[10] = { 0, };		// 근거리1
	_bool		m_bSkill2State[10] = { 0, };		// 중거리1
	_bool		m_bSkill3State[10] = { 0, };		// 기둥소환1	
	_bool		m_bSkill4State[10] = { 0, };		// 근거리2
	_bool		m_bSkill5State[10] = { 0, };		// 중거리2

	// OutSkill 관련
	_bool		m_bRaSkill1State[10] = { 0, };		// 원거리1
	_bool		m_bRaSkill2State[10] = { 0, };		// 원거리2

	// GimicPattern1 관련
	_bool		m_bGP1SkillState[10] = { 0, };
	_float		m_fGP1DownSpeed = 5.f;

	// GimicPattern2 관련
	_bool		m_bGP2SkillState[10] = { 0, };
	
	// 닷지할 위치, 거리
	_float3		m_vDodgePos = _float3(0.f, 0.f, 0.f);
	_float		m_vDodgeRange1 = 25.f;			//InrangeSkill1 이후
	_float		m_vDodgeRange2 = 35.f;			//InrangeSkill2 이후

	// 점멸 혹은 점프할 위치
	_float3		m_vMovePos = _float3(0.f, 0.f, 0.f);

	// center방향
	_float3		m_vTargetCenterDir = _float3(0.f, 0.f, 0.f);

	// 점프 방향, 속도
	_float3		m_vJumpNormalMoveDir = _float3(0.f, 0.f, 0.f);
	_float		m_fJumpMoveSpeed = 1.f;
	_float		m_fJumpMoveSpeedY = 0.f;

	// 특수패턴으로 가는 체력
	_float		m_fGimicPattern1Hp = 1500.f;
	_float		m_fGimicPattern2Hp = 800.f;
	_float		m_fFury1Hp = 1300.f;
	_float		m_fFury2Hp = 200.f;
	_float		m_fSwordPower = 200.f;

	// 광폭화로 빨라지는 속도
	_float		m_fFuryActionSpeed = 1.5f;

	// 공격마다 미끄러지는 속도
	_float		m_fL2SlideSpeed = 20.f;
	_float		m_fLM2SlideSpeed = 20.f;
	_float		m_fL42SlideSpeed = 20.f;
	_float		m_fU2SlideSpeed = 20.f;
	_float		m_fU3SlideSpeed = 20.f;
	_float		m_fU5SlideSpeed = 20.f;

	// 고정값
	// 공용
	_float		m_fAnimNomalJumpChangeTime = 0.5f;
	_float		m_fAnimNormalAttackStartTime = 0.1f;
	_float		m_fAnimAttackAfterDashStartTime = 0.3f;
	_float		m_fAnimNormalAltChangeTime = 0.1f;
	_float		m_fAnimNormalFastSlashChangeTime = 0.5f;
	_float		m_fAnimNormalRunChangeTime = 0.5f;
	_float		m_fDashDistanceXZ = 0.f;				// 대쉬 수평거리
	_float		m_fDashDistanceXZGround = 0.f;			// 땅에 닿기전 대쉬 수평거리
	_float		m_fDashDistanceY = 0.f;					// 대쉬 수직거리
	_float		m_fDashTime = 0.f;						// 대쉬 시간
	_float		m_fDashYTime = 0.f;						// 대쉬Y 내려가는 시간
	_float		m_fDashSpeedY = 0.f;					// 대쉬 수직속도
	_float3		m_vDashDir = _float3(0.f, 0.f, 0.f);	// 대쉬 방향
	_float		m_fDashBeforeY = 0.f;					// 점프해서 대쉬 직전 y
	_float		m_fDashUpDistance = 5.5f;				// 점프 대쉬해서 올라가는 y
	_float		m_fGroundPosY = -30.8f;					// 보스존에서 보스 y
	_float		m_fDashOutSleepSpeed = 3.f;				// 빠지는 대쉬 미끄러지는 속도

	_float3		m_vDashOutPos = _float3(0.f, 0.f, 0.f);
	_float		m_fDistanceToDashOut = 1000.f;
	_float3		m_vDashOutDir = _float3(0.f, 0.f, 0.f);

	// InRange
	// Skill1
	_float		m_fIn1Skill1DashDistanceXZ = 0.f;		// 대쉬 수평거리
	_float		m_fIn1Skill1DashDistanceY = 0.f;		// 대쉬 수직거리
	_float		m_fIn1Skill1DashTime = 0.f;				// 대쉬 시간
	_float		m_fIn1Skill1DashSpeedXZ = 100.f;		// 대쉬 수평속도(고정값)
	_float		m_fIn1Skill1DashSpeedY = 0.f;			// 대쉬 수직속도
	_float3		m_fIn1Skill1DashDir = _float3(0.f, 0.f, 0.f);
	_float		m_fIn1Skill1StandTime = 0.5f;
	_float		m_fIn1Skill1WalkTime = 5.f;
	_float		m_fIn1Skill1Combo3Speed = 2.f;
	_float		m_fIn1Skill1Combo2Speed = 1.5f;
	_float		m_fIn1Skill1SlashSpeed = 1.5f;
	//_float	m_fIn1Skill1WalkTime = 0.3f;
	_float		m_fIn1Skill1WalkSpeed = 1.f;
	_float		m_fIn1To2ComboTime = 0.2f;
	_float		m_fIn1ToDashTime = 1.f;
	_float		m_fIn1Skill1NRunSpeed = 12.f;	// normal
	_float		m_fIn1Skill1RunSpeed = 12.f;	// fury
	_float		m_fIn1Skill1BlinkTime = 1.f;	// fury
	_float		m_fIn1FSkill1Combo3Speed = 1.5f;	// fury
	_float		m_fIn1FSkill1Combo2Speed = 1.8f;	// fury
	_float		m_fIn1FSkill1SlashSpeed = 1.2f;	// fury
	_float		m_fIn1Skill1AttackIntervel = 0.7f;	// fury

	// Skill2
	_float		m_fIn1Skill2Combo3Speed = 1.3f;
	_float		m_fIn1Skill2Combo2Speed = 1.5f;
	_float		m_fIn1Skill2SlashSpeed = 1.f;
	_float		m_fIn2Skill2StandTime = 1.f;
	_float		m_fIn2Skill2WalkTime = 5.f;
	_float		m_fIn2To2ComboTime = 0.2f;
	_float		m_fIn2Skill2CircleRadian = 3.14f / 180.f * 120.f;
	_float		m_fIn2Skill2CircleDistance = 15.f;
	_float		m_fIn2Skill2CircleRunSpeed = 25.f;
	_float		m_fIn2Skill2FCircleRunSpeed = 50.f;	// fury
	_float3		m_fIn2Skill2CircleCenterPos = _float3(0.f, 0.f, 0.f);
	_float		m_fIn2Skill2CircleRunStartTime = 0.1f;
	_float		m_fIn2Skill2CircleTime = 5.f;
	_float		m_fIn1FSkill2Combo3Speed = 1.5f;	// fury
	_float		m_fIn1FSkill2Combo2Speed = 1.5f;	// fury
	_float		m_fIn1FSkill2SlashSpeed = 1.2f;	// fury
	_float		m_fIn1FSkill2SpecialAnimTime = 0.5f;

	// Skill3
	_float		m_fIn1Skill3StandTime = 1.f;
	_float		m_fIn2Skill3SpecialAnimTime = 0.5f;

	// Skill4
	
	// Skill5

	// Fury1
	// Skill1
	_float		m_fF1In1StandTime = 0.5f;
	_float		m_fF1In1Combo2Speed = 2.f;
	_float		m_fF1In1ToDashTime = 1.f;

	// Skill2
	_float		m_fF1In2StandTime = 0.5f;
	_float		m_fF1In2Combo3Speed = 2.f;

	// Skill3
	_float		m_fF1Middle1StandTime = 1.f;
	_bool		m_bF1Middle1FirstSummon = false;

	// Skill4
	_float		m_fF1Middle2StandTime = 0.5f;
	_float		m_fF1Middle2SlashSpeed = 3.f;

	// Skill5
	_float		m_fF1Range1StandTime = 0.5f;
	_float		m_fSummonIntervel = 1.f;
	_float		m_fSummonIntervel2 = 1.5f;
	_int		m_iMagicCount = 0;
	_int		m_iBMagicCount = 5;

	// 분신
	vector<shared_ptr<CRedKnight_Shadow>>		m_vecRedKnightShadow;
	_int		m_iRedKnightShadowCount = 12;
	_int		m_iRedKnightGimicShadowCount = 4;
	
	vector<shared_ptr<CMidget_Berserker_Shadow>>		m_vecMidgetBerserkerShadow;
	_int		m_iMidgetBerserkerShadowCount = 1;

	vector<shared_ptr<COrc_DarkCommander_Shadow>>		m_vecOrcDarkCommanderShadow;
	_int		m_iOrcDarkCommanderShadowCount = 1;

	// 특수패턴1
	// 떨어지는 검
	vector<shared_ptr<CFallingSword>>		m_vecFallingSword;
	_int		m_iFallingSwordCount = 5;
	_int		m_iTargetSwordIndex = 0;
	_bool		m_bBeforeTargetSword = true;	// 빛나는 검 없는 상태
	_int		m_iTargetCount = 0;				// 빛나는 검 count
	_int		m_iBTargetCount = 3;			// 빛나는 검 countMax
	_float		m_fGP1DownTime = 2.f;
	_bool		m_bSummonShadow[10] = { 0, };
	_float		m_fGimicDisruptShadowTime = 7.f;
	
	// 기둥
	vector<shared_ptr<CPillar>>		m_vecPillar;
	_int		m_iPillarCount = 10;
	_float		m_fGimic2ReadyTime = 10.f;	// 기둥 올라오게 하고 기다리는 시간
	_float		m_fGimic23SlashSpeed = 2.f;
	_int		m_bGimic2SlashType = 0;	// 1이면 3연참, 2이면 차징참격, 3면 속박마법
	_float		m_fGimic2TimeLimit = 50.f; // 50.f
	_float		m_fGimic2BeforePillarDownTime = 3.f;

	_bool		m_bSpecial2FirstGround = false;
	_bool		m_bSpecial2SecondGround = false;

	// OutRange
	// Skill1
	_float		m_fOutSkill1SlashSpeed = 2.f;
	_float		m_fOutSkill1Combo2Speed = 1.5f;
	_float		m_fOutSkill1StandTime = 1.f;
	//_int		m_iOutSkill1SlashTotal = 1;

	// Skill2
	_float		m_fOutSkill2Combo3Speed = 2.f;
	_float		m_fOutSkill2StandTime = 1.f;
	_float		m_fOutSkill2WalkSpeed = 1.f;

	// Skill3
	_bool		m_bHealthUnder125 = false;
	_float		m_fOutSkill3StandTime = 1.f;
	_float		m_fIn1Skill3SlashSpeed = 2.f;
	_float		m_fOutSkill3SpecialAnimTime = 0.5f;
	_int		m_iOutSkill3SlashCount = 0;
	_int		m_iOutSkill3BSlashCount = 3;

	// 위에 올라가서 속박마법 소환 패턴
	// Skill4
	_bool		m_bOutSkill4ShadowStart = false;
	_bool		m_bOutSkill4Summon[6] = { 0, };
	_float		m_fOutSkill4StandTime = 0.5f;
	_bool		m_bOutSkill4FirstDash = true;
	_float		m_fOutSkill4RunSpeed = 8.f;
	_float		m_fOutSkill4RunTime = 2.f;
	_int		m_iOutSkill4DashCount = 0;
	_int		m_iOutSkill4DashTotalCount = 5;
	_float		m_fOutSkill4AltAfterDashTime = 0.5f;
	_float		m_fOutSkill4AnimAttackSpeed = 1.5f;

	// 방해하는 그림자 순서
	_int		m_iGimic1ShadowOrder[100] = { 0,1,2,1,0,2,0,1,2,0,0,1,2,1,0,2,0,1,2,0,0,1,2,1,0,2,0,1,2,0 };
	_int		m_iGimic1ShadowNum = 0;

	// 근접 패턴 고정
	_int		m_iMeleeSkillOrder[100] = { 0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1 };
	//_int		m_iMeleeSkillOrder[100] = { 1,1,1,1,1,1,1,1,1,1,1 };
	_int		m_iMeleePatternNum = 0;

	// 중거리 패턴 고정
	_int		m_iMiddleRangeSkillOrder[100] = { 2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3 };
	//_int		m_iMiddleRangeSkillOrder[100] = { 2,2,2,2,2,2,2,2,2,2,2,2 };
	_int		m_iMiddleRangePatternNum = 0;
	
	// 원거리 패턴 고정
	_int		m_iRangeSkillOrder[100] = { 4,5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4,5 };
	_int		m_iRangeFurySkillOrder[100] = { 5,5,4,5,5,4,5,5,4,5,5,4,5,5,4,5,5,4,5,5,4,5,5,4,5,5,4,5,5,4,5,5 };
	//_int		m_iRangeSkillOrder[100] = { 4,4,4,4,4,4,4,4,4,4,4,4,4 };
	_int		m_iRangePatternNum = 0;
	
	// 패링 스택
	_int		m_iParryingStack = 0;
	_int		m_iBParryingStack = 5;

	_int		m_iAttParryStack = 1;
	_int		m_iDashParryStack = 3;
	_int		m_iParryType = 0;		// 0: 기본공격, 1: 대쉬

	// 잔상색
	_float4		m_vOutDashColor = _float4(1.f, 1.f, 1.f, 1.f);
	_float4		m_vThroughDashColor = _float4(1.f, 1.f, 1.f, 1.f);
	_float4		m_vFrontDashColor = _float4(0.5f, 0.f, 0.f, 1.f);

	// 참격
	vector<shared_ptr<CSlash>>		m_vecSlashLD;
	vector<shared_ptr<CSlash>>		m_vecSlashLU;
	vector<shared_ptr<CSlash>>		m_vecSlashLR;
	vector<shared_ptr<CSlash>>		m_vecSlashLRD;
	vector<shared_ptr<CSlash>>		m_vecSlashD;
	vector<shared_ptr<CSlash>>		m_vecSlashU;
	vector<shared_ptr<CSlash>>		m_vecSlashR;
	vector<shared_ptr<CSlash>>		m_vecSlashBig;
	_int		m_iSlashCount = 5;
	_bool		m_bFastComboSlash = false;		// 연속 3회콤보시 참격이 붙어서 나감
	
	_float		m_fFastSlashStartTime = m_fKeyFrameSpeed * 25.f;
	_bool		m_bSlashGen = false;
	_float		m_fFastSlashSpeed = 35.f;
	_float		m_fFFastSlashSpeed = 40.f;
	_float		m_fLongSlashSpeed = 20.f;
	_float		m_fBigSlashSpeed = 100.f;

	_bool		m_bJumpAttack1EffectOn = false;

	// 속박마법
	vector<shared_ptr<CBindMagic>>		m_vecBindMagic;

	_int		m_iBindMagicCount = 10;
	_float		m_fBindMagicSpeed = 50.f;
	_float		m_fBindGuidedMagicSpeed = 20.f;
	_float		m_fBindGuidedMagicSpeed2 = 10.f;
	_bool		m_bBindMagicGen = false;
	_bool		m_bBindCircleMagicStart = false;
	_bool		m_bBindCircleMagicStop = false;
	_float		m_fBindIntervel = 1.f;
	_int		m_iBindMagicGenNum = 0;
	_float		m_fOutRange2ReadyTime = 20.f;

	// 위치정리
	_float3		m_vCenterPos = _float3(0.f, 0.f, 0.f);
	// 맵 꼭짓점
	_float3		m_vRightTopPos = _float3(0.f, 0.f, 0.f);
	_float3		m_vRightBottomPos = _float3(0.f, 0.f, 0.f);
	_float3		m_vLeftTopPos = _float3(0.f, 0.f, 0.f);
	_float3		m_vLeftBottomPos = _float3(0.f, 0.f, 0.f);

	_float3		m_vSwordFallPos1 = _float3(0.f, 0.f, 0.f);	// 우상단
	_float3		m_vSwordFallPos2 = _float3(0.f, 0.f, 0.f);	// 좌상단
	_float3		m_vSwordFallPos3 = _float3(0.f, 0.f, 0.f);	// 좌하단
	_float3		m_vSwordFallPos4 = _float3(0.f, 0.f, 0.f);	// 우하단
	_float3		m_vSwordFallPos5 = _float3(0.f, 0.f, 0.f);	// 중앙

	_float3		m_vFrontThronePos = _float3(0.f, 0.f, 0.f); // 왕좌 앞
	
	_float		m_fRightEnd = 0.f;
	_float		m_fLeftEnd = 0.f;
	_float		m_fTopEnd = 0.f;
	_float		m_fBottomEnd = 0.f;
	
	_float		m_fZHLength = 50.f;			// 사각형 움직일수 있는 맵의 길이의 반
	_float		m_fZHLength2 = 30.f;			// 사각형 움직일수 있는 맵의 길이의 반
	_float		m_fCircleRadius = 45.f;		// 도는 원의 반지름

	_int		m_iPhase = RK_P_NORMAL;

	// 콜라이더 정리
	// 근접 공격
	const char* m_strATTCollider1 = "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA";
	const char* m_strATTCollider2 = "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA.002";
	const char* m_strATTCollider3 = "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA.004";
	const char* m_strATTCollider4 = "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB";
	const char* m_strATTCollider5 = "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB.002";
	const char* m_strATTCollider6 = "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB.004";
	const char* m_strATTCollider7 = "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB_M";
	const char* m_strATTCollider8 = "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB_M.002";
	const char* m_strATTCollider9 = "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC";
	const char* m_strATTCollider10 = "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.002";
	const char* m_strATTCollider11 = "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.003";
	const char* m_strATTCollider12 = "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.005";
	// 대쉬 공격
	const char* m_strDashAttCollider = "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_DashAttack_E.001";

	/* For.Trail */
private:
	shared_ptr<class CPointTrail> m_pWeaponTrail;
	HRESULT AddWeaponTrail();		// Add Weapon Trail
	HRESULT ChangeTrailType();		// Change WeaponType, TrailColor

	/* Motion Trail */
	shared_ptr<class CMotionTrail> m_pMotionTrail;
	HRESULT AddMotionTrail(_int _iTrailCnt, _float3 _vTrailColor, _float _fTrailCntPerTick);
	HRESULT SetUseMotionTrail(_bool _bUseMotionTrail, _bool _bUseTimer = false, _float _fTimer = 0.f, _bool _bChangeColor = false, _float4 _vColor = _float4());
};

END