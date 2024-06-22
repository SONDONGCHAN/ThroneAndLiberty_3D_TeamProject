#pragma once

#include "Monster.h"
#include "Slash.h"

// 패턴
enum RKS_Pattern
{
	RKS_P_GIMIC1 = 1,
	RKS_P_2COMBO,
	RKS_P_3COMBO,
	RKS_P_JUMPATTACK,
	RKS_P_SLASH,
	RKS_P_DASHTHROUGH,
	RKS_P_BINDGROUND,
	RKS_P_QUAKE
};

// 특정 액션 상태
enum RKS_ActionState
{
	RKS_A_BLOCKED = 0,
	RKS_A_DASHATT1 = 3,
	RKS_A_DASHWHILE = 5,
	RKS_A_DASHSTART = 6,
	RKS_A_SPECIAL1 = 7,				// 점프후 찍기(광역 장판공격)
	RKS_A_SHORTJUMP = 8,
	RKS_A_SPECIAL2 = 9,				// 점프후 2번찍기(초광역 장판공격)
	RKS_A_RUNFL = 12,
	RKS_A_RUNFR = 13,
	RKS_A_MAGIC = 15,				// 속박마법
	RKS_A_TOFURY = 16,
	RKS_A_CHARGESLASH = 17,			// 차징슬래쉬
	RKS_A_FRONTQUAKE = 18,			// 전방지진
	RKS_A_GOUP = 19,
	RKS_A_DEATH = 27,
	RKS_A_DIE = 28,
	RKS_A_DOWN3 = 29,
	RKS_A_DOWN2 = 30,
	RKS_A_DOWN1 = 31,
	RKS_A_FRONTDOWN3 = 32,
	RKS_A_FRONTDOWN2 = 33,
	RKS_A_FRONTDOWN1 = 34,
	RKS_A_HITTED = 38,
	RKS_A_RUN = 41,
	RKS_A_SHOCKED = 42,
	RKS_A_GROUNDMAGIC = 43,			// 일직선 장판 마법
	RKS_A_SUMMONSHADOW = 44,			// 그림자 소환
	RKS_A_MAGIC2 = 45,				// 검 떨어뜨리기, 쳐서 날릴 검소환, 기둥 떨어뜨리기,
	// 기둥 일직선으로 떨어뜨리기
	RKS_A_TELEPORT = 46,				// 텔레포트
	RKS_A_WAITTOALERT = 47,
	RKS_A_WAIT = 48,
	RKS_A_ALERTTOWAIT = 49,
	RKS_A_SHOTSWORD = 50,			// 검 쳐서 날리기
	RKS_A_ATT_D1 = 52,
	RKS_A_ATT_D2 = 53,
	RKS_A_ATT_D3 = 54,
	RKS_A_ATT_D4 = 55,
	RKS_A_ATT_L1 = 57,
	RKS_A_ATT_L2 = 58,
	RKS_A_ATT_L3 = 59,
	RKS_A_ATT_L4 = 60,
	RKS_A_ATT_LM1 = 62,
	RKS_A_ATT_LM2 = 63,
	RKS_A_ATT_U1 = 65,
	RKS_A_ATT_U2 = 66,
	RKS_A_ATT_U3 = 67,
	RKS_A_ATT_U4 = 68,
	RKS_A_ATT_U5 = 69,
	RKS_A_SUMMON = 71,
	RKS_A_ALT = 72,
	RKS_A_IDLE = 73,
	RKS_A_STUN = 74,
	RKS_A_WALK = 75
};


BEGIN(Client)

class CRedKnight_Shadow final : public CMonster
{
public:
	struct ORC_SOLDIER_DESC
	{
		CMonster::MONSTER_DESC pTarget;
	};

public:
	CRedKnight_Shadow();
	virtual ~CRedKnight_Shadow()  override;

public:
	HRESULT			Initialize();
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	void			StartPattern(_int _iPatternNum, _int _iType = 0);
	void			SetJumpPos(_float3 _vJumpPos) { m_vJumpPos = _vJumpPos; }
	static			shared_ptr<CRedKnight_Shadow> Create(_float3 _vCreatePos, _float3 _vJumpPos);
	void			SetDead(_bool _bDead) { m_bDead = _bDead; }
	void			SetGenPos(_float3 _vGenPos) { m_vGenBossPos = _vGenPos; }
	void			SetGimic1Pos(_float3 _vThronePos) { m_vGimic1Pattern1Pos = _vThronePos; }

protected:
	HRESULT			AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void	OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void	OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void	OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	virtual void	EnableCollision(const char* _strShapeTag) override;
	virtual void	DisableCollision(const char* _strShapeTag) override;

	HRESULT			AddPartObjects(string _strModelKey);

	// 패턴
	void			Gimic1Pattern(_float _fTimeDelta);
	void			Combo2Pattern(_float _fTimeDelta);
	void			Combo3Pattern(_float _fTimeDelta);
	void			JumpAttackPattern(_float _fTimeDelta);
	void			SlashPattern(_float _fTimeDelta);
	void			DashThroughPattern(_float _fTimeDelta);
	void			BindGroundPattern(_float _fTimeDelta);
	void			QuakePattern(_float _fTimeDelta);

	void			ReBatch();	// 할일 끝난 그림자 초기화

	_float3			CalculateBindary(_float3 _vPos);		// 도출 위치가 경계선을 지나버리면 그 경계까지만 가도록

	// 랜덤으로 반환
	_int			RandomSlowSlash();
	_int			RandomNormal2ComboStartAttack();
	_int			RandomNormal3ComboStartAttack();

	_float3			RandomBlinkPos();		// 플레이어 가깝게 무작위 점멸 위치
	_float3			RandomBlinkSlashPos();		// 플레이어 참격 위치 무작위 점멸 위치
	_float3			RandomBlinkJumpAttackPos();		// 플레이어 점프공격 위치 무작위 점멸 위치
	_float3			RandomBlinkDashPos();		// 플레이어 돌진 위치 무작위 점멸 위치

	// 콤보 활성화
	_bool			Update2ComboAttackState(float _fTimeDelta);
	_bool			Update3ComboAttackState(float _fTimeDelta);
	_bool			UpdateAttackState(float _fTimeDelta);

	// 점프 속도와 방향 갱신
	void			UpdateSpecial1DirSpeed();
	// 점멸 혹은 점프할 위치
	_float3		m_vMovePos = _float3(0.f, 0.f, 0.f);
	// 점프 방향, 속도
	_float3		m_vJumpNormalMoveDir = _float3(0.f, 0.f, 0.f);
	_float		m_fJumpMoveSpeed = 1.f;

	// 참격 활성화
	void			ActiveSlash(_int _iSlashType);

	// 공격중에 회전 갱신
	void			UpdateAttackDir(_float _fTimeDelta);
	// 느린참격중에 회전 갱신
	void			UpdateSlashDir(_float _fTimeDelta);
	// 시간 구간에서 특정속도로 보는방향으로 미끄러지게하기
	void			UpdateMoveSlide(_float _fSlideStart, _float _fSlideEnd, _float _fSpeed, _float _fTimeDelta);

private:
	_float3		m_vRimLightColor = _float3(1.f, 0.f, 0.f);

	_bool		m_bJumpAttack1EffectOn = false;

	_int		m_iActionState = RKS_A_ALT;		// 행동상태
	_float		m_fCalculateTime = 0.f;					// 계산용 Time
	_float		m_fCalculateTime2 = 0.f;					// 계산용 Time
	_float		m_fCalculateTime3 = 0.f;					// 계산용 Time

	_int		m_iPatternType = 0;		// 패턴종류	1: 기믹패턴1 시작
	_bool		m_bPatternStart = false;
	_bool		m_bSkill1State[5] = { 0, };		// 기믹패턴1
	_bool		m_bSkill2State[5] = { 0, };		// 2콤보
	_bool		m_bSkill3State[5] = { 0, };		// 3콤보
	_bool		m_bSkill4State[5] = { 0, };		// 점프공격
	_bool		m_bSkill5State[5] = { 0, };		// 참격
	_bool		m_bSkill6State[5] = { 0, };		// 대쉬
	_bool		m_bSkill7State[5] = { 0, };		// 속박장판
	_bool		m_bSkill8State[5] = { 0, };		// 전방지진

	// 콤보
	_int		m_i2ComboType = 10;		// 2콤보의 종류
	_int		m_i3ComboType = 10;		// 3콤보의 종류

	_int		m_iGenType = 0;			// 0: 일반적, 1: 특수

	_float		m_fDismissTime = 1.f;

	// 공격마다 미끄러지는 속도
	_float		m_fL2SlideSpeed = 20.f;
	_float		m_fLM2SlideSpeed = 20.f;
	_float		m_fL42SlideSpeed = 20.f;
	_float		m_fU2SlideSpeed = 20.f;
	_float		m_fU3SlideSpeed = 20.f;
	_float		m_fU5SlideSpeed = 20.f;

	vector<shared_ptr<CSlash>>		m_vecSlashLD;
	vector<shared_ptr<CSlash>>		m_vecSlashLU;
	vector<shared_ptr<CSlash>>		m_vecSlashLR;
	vector<shared_ptr<CSlash>>		m_vecSlashLRD;
	vector<shared_ptr<CSlash>>		m_vecSlashD;
	vector<shared_ptr<CSlash>>		m_vecSlashU;
	vector<shared_ptr<CSlash>>		m_vecSlashR;
	_int							m_iSlashCount = 5;

	_float		m_fAnimSpeed = 1.f;

	_float3		m_vJumpPos = _float3(0.f, 0.f, 0.f);

	// 점멸 혹은 점프할 위치
	_float3		m_vSMovePos = _float3(0.f, 0.f, 0.f);

	_float3		m_vGenBossPos = _float3(0.f, 0.f, 0.f);

	// 점프 방향, 속도
	_float3		m_vSJumpNormalMoveDir = _float3(0.f, 0.f, 0.f);
	_float		m_fSJumpMoveSpeed = 1.f;

	_float3		m_vGimic1Pattern1Pos = _float3(0.f, 0.f, 0.f);

	// Dash
	_float		m_fIn1Skill1DashDistanceXZ = 0.f;		// 대쉬 수평거리
	_float		m_fIn1Skill1DashDistanceY = 0.f;		// 대쉬 수직거리
	_float		m_fIn1Skill1DashTime = 0.f;				// 대쉬 시간
	_float		m_fIn1Skill1DashSpeedXZ = 100.f;		// 대쉬 수평속도(고정값)
	_float3		m_fIn1Skill1DashDir = _float3(0.f, 0.f, 0.f);

	// 키프레임 시간 관련
	_float		m_fFastJumpStartTime = m_fKeyFrameSpeed * 12;
	_float		m_fFastJumpEndTime = m_fKeyFrameSpeed * 20;
	_float		m_fSpecial1JumpStartTime = m_fKeyFrameSpeed * 11;
	_float		m_fSpecial1JumpEndTime = m_fKeyFrameSpeed * 31;
	_float		m_fSpecial2JumpStartTime = m_fKeyFrameSpeed * 11;
	_float		m_fSpecial2JumpEndTime = m_fKeyFrameSpeed * 31;
	_float		m_fQuakeStartTime = m_fKeyFrameSpeed * 40.f;
	_float		m_fJumpAttackEndTime = m_fKeyFrameSpeed * 38.f;

	_bool		m_bQuakeSoundStart = false;
	_bool		m_bJumpAttackSoundStart = false;


	_bool		m_bSSlashGen = false;
	_int		m_iGimicSlashCount = 0;
	_int		m_iGimicBSlashCount = 100;

	// 참격 속도
	_float		m_fFastSlashSpeed = 35.f;
	_float		m_fFFastSlashSpeed = 45.f;
	_float		m_fLongSlashSpeed = 20.f;

	_float		m_fStandTime = 1.f;

	_bool		m_bDead = false;

	// 사운드
	_float		m_fEffectSize = 0.5f;


	// 경계
	_float		m_fRightEnd = 0.f;
	_float		m_fLeftEnd = 0.f;
	_float		m_fTopEnd = 0.f;
	_float		m_fBottomEnd = 0.f;
};

END