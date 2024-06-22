#pragma once

#include "Monster.h"
#include "Axe.h"
// 공격(공격1, 공격2(도끼 던지기)), 주기적으로 땅을 파고 들어가 배후 무작위 위치로 나옴
// 공격 패링가능, 도끼 패링가능

// CoolTime
enum MI_F_CoolTime
{
	MI_F_C_NORMALATTACK1 = 0,
	MI_F_C_NORMALATTACK2,
	MI_F_C_NORMALATTACK3			// 바닥 들어가기
};

// Base 상태
enum MI_F_BaseState
{
	MI_F_B_BEFOREMEET = 0,
	MI_F_B_ALERT,
	MI_F_B_NORMAL,
	MI_F_B_HITTED,
	MI_F_B_HITTED2,
	MI_F_B_HITTED3,
	MI_F_B_STUN,
	MI_F_B_SHOCKED,
	MI_F_B_DOWN,
	MI_F_B_DEATH
};

// 거리에 따른 상태
enum MI_F_DistanceState
{
	MI_F_D_MOVERANGE = 0,
	MI_F_D_ATTACK1RANGE,
	MI_F_D_ATTACK2RANGE,
	MI_F_D_OUTRANGE,
	MI_F_D_BEFOREMEET
};

// 특정 액션 상태
enum MI_F_ActionState
{
	MI_F_A_BLOCKED = 0,
	MI_F_A_DOWN3 = 3,
	MI_F_A_DOWN2 = 4,
	MI_F_A_DOWN1 = 5,
	MI_F_A_RUNFL = 7,
	MI_F_A_RUNFR = 8,
	MI_F_A_SEARCH = 9,
	MI_F_A_ATTACK1 = 10,	// 근공
	MI_F_A_ATTACK2 = 11,	// 도끼
	MI_F_A_DANCE1 = 19,
	MI_F_A_DANCE2 = 20,
	MI_F_A_DANCE3 = 21,
	MI_F_A_DISAPPEAR = 22,	// 도망가기
	MI_F_A_APPEAR = 27,
	MI_F_A_THREAT = 31,
	MI_F_A_STDALT = 32,
	MI_F_A_DEAD = 40,
	MI_F_A_DEATH = 41,
	MI_F_A_FRONTBE = 48,
	MI_F_A_FRONTBL = 49,
	MI_F_A_FRONTBS = 50,
	MI_F_A_RUN = 51,
	MI_F_A_SHOCKED = 52,
	MI_F_A_STDIDL = 53,
	MI_F_A_STUN = 54,
	MI_F_A_WALK = 55
};


BEGIN(Client)

class CMidget_Footman final : public CMonster
{
public:
	struct MIDGET_FOOTMAN_DESC
	{
		CMonster::MONSTER_DESC pTarget;
	};

public:
	CMidget_Footman();
	virtual ~CMidget_Footman()  override;

public:
	HRESULT			Initialize();
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CMidget_Footman> Create(_float3 _vCreatePos);

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
	void		ActiveNormalAxe();

	// BaseState에 따른 FSM
	void		FSM_BeforeMeet(_float _fTimeDelta);
	void		FSM_Alert(_float _fTimeDelta);
	void		FSM_Normal(_float _fTimeDelta);
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
	_int		m_iBaseState = MI_F_B_BEFOREMEET;		// Base상태
	_int		m_iDistanceState = MI_F_D_BEFOREMEET;	// 거리에 따른 상태
	_int		m_iActionState = MI_F_A_STDIDL;		// 행동상태

	_float		m_fAttack1Range = 2.f;		// 근접공격 Range
	_float		m_fAttack2Range = 10.f;		// 도끼던지기, 굴파기 Range
	_float		m_fOutRange = 10000.f;		// 범위에서 나간 Range
	_float		m_fMeetRange = 20.f;		// 조우 Range
	_float		m_fFightRange = 9.f;		// 싸움 Range

	_float		m_fCalculateTime = 0.f;						// 계산용 Time
	_float		m_fThrowAxeTime = 40.f * 0.05f;
	_bool		m_bAxeGen = false;

	_float		m_fDigTime = 0.f;		// 바닥에 있는 시간

	// 쿨타임
	_bool		m_bAttackReady[4] = { true, false, false};
	_float		m_fAttackCoolTime[4] = { 2.f, 2.8f, 4.f};
	_float		m_fBAttackCoolTime[4] = { 2.f, 2.8f, 4.f };

	_int		m_NormalAttackCount = 3;	// Normal 상태 공격 종류

	// 콜라이더 정리
	const char*		m_strATT1Collider1 = "M_L1Midget_Footman_SK.ao|M_L1Midget_Footman_SkdmgA";

	// 도끼
	vector<shared_ptr<CAxe>>		m_vecAxe;
	_int		m_iAxeCount = 5;

	/*Beard Color*/
private:
	_float3 m_vHairMainColor = { 1.f, 1.f, 1.f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };
};

END