#pragma once

#include "Monster.h"

// 패턴
// 공격종류(아래공격1(짧경), 아래공격2(기절), 회전공격(날라가기(강), 대쉬공격1(긴경), 대쉬공격2(감전),
// 위로공격1(짧경), 위로공격2(날라가기(약))
// 패턴 무한반복: 대쉬공격1, 아래공격2, 회전공격, 대쉬공격2, 대쉬공격1, 위로공격1, 아래공격1, 위로공격2
// 패링가능: 아래공격2, 위로공격2, 대쉬공격1

// Dialog Order 순서
enum NPC_DavidMonster_DialogOrderState
{
	DAVIDMONSTER_DI_PRACTICEMEET2,			// 주인공한테 맞기 FSM
	DAVIDMONSTER_DI_PRACTICEMEET3,			// 주인공 떄리기 FSM
	DAVIDMONSTER_DI_PRACTICEMEET4,			// 주인공과 대련 FSM
};

// Base 상태
enum NPC_DavidMonster_BaseState
{
	DAVIDMONSTER_B_BATTLENORMAL,
	DAVIDMONSTER_B_HITTED,
	DAVIDMONSTER_B_HITTED2,
	DAVIDMONSTER_B_HITTED3,
	DAVIDMONSTER_B_STUN,
	DAVIDMONSTER_B_SHOCKED,
	DAVIDMONSTER_B_DOWN,
	DAVIDMONSTER_B_NONE
};

// 거리에 따른 상태
enum NPC_DavidMonster_DistanceState
{
	DAVIDMONSTER_D_ATTACKRANGE,					// 일반공격 거리
	DAVIDMONSTER_D_MOVERANGE,					// 움직일 거리
	DAVIDMONSTER_D_OUTRANGE
};

// 특정 액션 상태
enum NPC_DavidMonster_ActionState
{
	DAVIDMONSTER_A_BLADEDANCE1 = 0,			// 회전 공격1
	DAVIDMONSTER_A_BLADEDANCE2 = 1,			// 회전 공격2
	DAVIDMONSTER_A_BLADEDANCE3 = 2,			// 회전 공격3
	DAVIDMONSTER_A_BLOCKED = 3,
	DAVIDMONSTER_A_SWORDIN = 8,				// 검 집어넣기
	DAVIDMONSTER_A_SWORDOUT = 9,			// 검 꺼내기
	DAVIDMONSTER_A_TALK1 = 10,
	DAVIDMONSTER_A_TALK2 = 11,
	DAVIDMONSTER_A_TALK3 = 12,
	DAVIDMONSTER_A_TALK4 = 13,
	DAVIDMONSTER_A_DASH1 = 14,				// 대쉬 공격1(앞까지)
	DAVIDMONSTER_A_DASH2 = 15,				// 대쉬 공격2(관통)
	DAVIDMONSTER_A_IDLE = 18,
	DAVIDMONSTER_A_DOWN3 = 19,
	DAVIDMONSTER_A_DOWN2 = 20,
	DAVIDMONSTER_A_DOWN1 = 21,
	DAVIDMONSTER_A_FRONTBE = 29,
	DAVIDMONSTER_A_RUN = 31,				// 달리기
	DAVIDMONSTER_A_SHOCKED = 32,			// 감전
	DAVIDMONSTER_A_STDALT = 33,				// 전투태세
	DAVIDMONSTER_A_STUN = 34,
	DAVIDMONSTER_A_ATT1 = 35,				// 공격1(아래로찍기)
	DAVIDMONSTER_A_ATT1_2 = 36,				// 공격1(아래로찍기)
	DAVIDMONSTER_A_ATT2 = 37,				// 공격2(위로올리기)
	DAVIDMONSTER_A_ATT2_2 = 38,				// 공격2(위로올리기)
	DAVIDMONSTER_A_ALTRUN = 39,				// 전투상태 달리기
	DAVIDMONSTER_A_ALTWALK = 40,			// 전투상태 걷기
	DAVIDMONSTER_A_WALK = 45,
};

BEGIN(Client)

class CNPC_MonsterDavid final : public CMonster
{
public:
	CNPC_MonsterDavid();
	virtual ~CNPC_MonsterDavid() = default;

public:
	HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;
	virtual HRESULT RenderShadow()  override;

public:
	static shared_ptr<CNPC_MonsterDavid> Create(_float3 _vCreatePos);
	void		SetDialogState(_int _iNum);
	_bool		GetSwitch() { return m_bSwitch; }
	void		SetSwitch(_bool	_bSwitch) { m_bSwitch = _bSwitch; }
	void		GoSwitchPosition();
	void		SetNPCPos(_float3 _vCreatePos) { m_vNPCPos = _vCreatePos; }
	_bool		GetDoingEnd() { return m_bDoingEnd; }

protected:
	HRESULT			AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void	OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void	OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void	OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	virtual void EnableCollision(const char* _strShapeTag) override;
	virtual void DisableCollision(const char* _strShapeTag) override;

	HRESULT		AddPartObjects(string _strModelKey);
	void		UpdateDistanceState();
	void		RenderBackWeapon();
	void		RenderHandWeapon();
	void		ResetTimer();

	void		FSM_PracticeMeet2(float _fTimeDelta);	// 맞기
	void		FSM_PracticeMeet3(float _fTimeDelta);	// 때리기
	void		FSM_PracticeMeet4(float _fTimeDelta);	// 대련

	// 상태이상 FSM
	void		FSM_Hitted(_float _fTimeDelta);
	void		FSM_Hitted2(_float _fTimeDelta);
	void		FSM_Hitted3(_float _fTimeDelta);
	void		FSM_Stun(_float _fTimeDelta);
	void		FSM_Shocked(_float _fTimeDelta);
	void		FSM_Down(_float _fTimeDelta);

	void		UpdateDir(_float _fTimeDelta);

	_bool		CheckLoose();
	void		CheckAttacked();

	_int		NextAttack();

	// 특정 패턴 애니메이션 자동으로 돌리기
	// 회전공격 
	_bool		UpdateSpinState(_float _fTimeDelta);
	// 앞까지 대쉬
	_bool		UpdateFrontDashState(_float _fTimeDelta, _float _fFrontDistance = 1.5f);
	// 뚫는 대쉬
	_bool		UpdateThroughDashState(float _fTimeDelta, _float _fDistance = 8.f);
	// 아래공격1
	_bool		UpdateBottomAttack1(_float _fTimeDelta);
	// 아래공격2
	_bool		UpdateBottomAttack2(_float _fTimeDelta);
	// 위로공격1
	_bool		UpdateUpAttack1(_float _fTimeDelta);
	// 위로공격2
	_bool		UpdateUpAttack2(_float _fTimeDelta);
	
	// 거리
	// 대화 상호작용을 위한 거리
	_float		m_fMeetRange = 10.f;
	_float		m_fCloseRange = 3.f;

	// 전투를 위한 거리
	_float		m_fAttack1Range = 2.f;		// 근접공격 Range
	_float		m_fOutRange = 10000.f;		// 범위에서 나간 Range

private:
	_float		m_fBacKWalkRange = 5.f;

	_float3		m_vNPCPos = _float3(0.f, 0.f, 0.f);	// 본체의 생성위치

	_float		m_fWalkRemainDistance = 0.05f;

	// 애니메이션 속도조절
	_float		m_fActionAnimSpeed[100] = { 0, };
	// 애니메이션 재생 속도
	_float		m_fAnimSpeed = 1.f;

	_float		m_fCalculateTime1 = 0.f;				// 계산용 Time1
	_float		m_fCalculateTime2 = 0.f;				// 계산용 Time2
	_float		m_fCalculateTime3 = 0.f;				// 계산용 Time3
	
	_int		m_iDistanceState = DAVIDMONSTER_D_MOVERANGE;		// 거리에 따른 상태
	_int		m_iActionState = DAVIDMONSTER_A_IDLE;				// 행동상태
	_int		m_iBaseState = DAVIDMONSTER_B_NONE;					// Base 상태
	_int		m_iDialogOrder = DAVIDMONSTER_DI_PRACTICEMEET2;		// 이 순서에 따라 상태가 변경
	
	// 애니메이션 시간
	_float		m_fWeaponOuTime = m_fKeyFrameSpeed * 18.f;	// 무기 꺼내는 시간
	_float		m_fWeaponInTime = m_fKeyFrameSpeed * 22.f;	// 무기 집어넣는 시간

	// 대쉬공격
	_float		m_fDashStartTime = m_fKeyFrameSpeed * 5.f;	// 대쉬 시작 시간
	_float		m_fDashEndTime = m_fKeyFrameSpeed * 12.f;

	_bool		m_bHandWeaponRender = false;
	_bool		m_bBackWeaponRender = true;

	// 대화순서2에서의 일반공격 간격
	_float		m_fD2AttackIntervel = 3.f;				// 일반공격 간격
	_float		m_fD2LimitTime = 60.f;					// 제한시간

	// 대화순서3에서의 공격들 간격
	_float		m_fD3AttackIntervel = 3.f;				// 공격들 간격
	//_float		m_fD3AttackIntervel = 9.f;			// 공격들 간격
	_int		m_iD3AttackNum = 0;						// 공격 순서
	_int		m_iMaxD3AttackNum = 6;
	_int		m_iD3AttackSkill = 0;

	// 대화순서4에서의 대련
	_float		m_fStandTime = 0.5f;		// 기술 사이마다 서있는 시간

	_bool		m_bNotHitted = false;		// 강제로 못막게 막기

	_bool		m_bDialog1State[20] = { 0, };		// 주인공한테 맞기
	_bool		m_bDialog2State[20] = { 0, };		// 주인공 떄리기
	_bool		m_bDialog3State[20] = { 0, };		// 주인공과 대련
	_bool		m_bDialog3CanHitted = false;		// 대화순서4에서 맞을수 있는 타이밍

	_bool		m_bSwitch = false;		// true면 NPC버젼과 자리바꾸기
	_bool		m_bDoingEnd = false;	// 마지막 대련 끝내면 true

	// 대쉬관련
	_float		m_fDashDistanceXZ = 0.f;				// 대쉬 수평거리
	_float		m_fDashTime = 0.f;						// 대쉬 시간
	_float		m_fDashSpeed = 0.f;						// 대쉬 속도
	_float3		m_vDashDir = _float3(0.f, 0.f, 0.f);	// 대쉬 방향

	_bool		m_bParriedBefore = false;			// 대쉬 상태에서 패링받았는지

	// 패링가능한 콜라이더 정리
	const char* m_strSpinAttCollider1 = "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_BladeDance";
	const char* m_strSpinAttCollider2 = "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_BladeDance.001";
	const char* m_strSpinAttCollider3 = "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_BladeDance.002";
	const char* m_strDashAttCollider1 = "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_Dash";
	const char* m_strDashAttCollider2 = "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_Dash.001";
	const char* m_stDownAttCollider1 = "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingA";
	const char* m_stDownAttCollider2 = "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingA.001";
	const char* m_stUpAttCollider1 = "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingB";
	const char* m_stUpAttCollider2 = "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingB.001";


	/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 0.02f, 0.02f, 0.07f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };
};

END