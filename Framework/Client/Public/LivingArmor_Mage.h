#pragma once

#include "Monster.h"
#include "ElectricBall.h"
// 광폭화(체력 일정 이하시 행동 빨라짐, 특수공격 추가), 공격(화염구 발사, 에너지파 발사, 장판공격)

// CoolTime
enum LA_MG_CoolTime
{
	LA_MG_C_NORMALATTACK1 = 0,
	LA_MG_C_NORMALATTACK2,
	LA_MG_C_NORMALATTACK3,
	LA_MG_C_FURYATTACK1,
	LA_MG_C_FURYATTACK2,
	LA_MG_C_FURYATTACK3,
	LA_MG_C_FURYATTACK4
};

// Base 상태
enum LA_MG_BaseState
{
	LA_MG_B_BEFOREMEET = 0,
	LA_MG_B_GENERATE,
	LA_MG_B_NORMAL,
	LA_MG_B_WHILEFURY,
	LA_MG_B_FURY,
	LA_MG_B_HITTED,
	LA_MG_B_HITTED2,
	LA_MG_B_HITTED3,
	LA_MG_B_STUN,
	LA_MG_B_SHOCKED,
	LA_MG_B_DOWN,
	LA_MG_B_DEATH
};

// 거리에 따른 상태
enum LA_MG_DistanceState
{
	LA_MG_D_MOVERANGE = 0,
	LA_MG_D_ATTACKRANGE1,	// 장판공격 가능 거리에 있을때
	LA_MG_D_ATTACKRANGE2,	// 그 외 원거리
	LA_MG_D_OUTRANGE,
	LA_MG_D_BEFOREMEET
};

// 특정 액션 상태
enum LA_MG_ActionState
{
	LA_MG_A_COMBATSTART = 4,
	LA_MG_A_DEAD = 5,
	LA_MG_A_DOWN3 = 8,
	LA_MG_A_DOWN2 = 9,
	LA_MG_A_DOWN1 = 10,
	LA_MG_A_FRONTBE = 14,
	LA_MG_A_FRONTBL = 15,
	LA_MG_A_FRONTBS = 16,
	LA_MG_A_RUN = 20,
	LA_MG_A_SEARCH3 = 21,
	LA_MG_A_SEARCH2 = 22,
	LA_MG_A_SEARCH1 = 23,
	LA_MG_A_SHOCKED = 24,
	LA_MG_A_FURY = 25,
	LA_MG_A_ATTACK1 = 26,	// 투사체
	LA_MG_A_ATTACK2 = 27,	// 에너지포
	LA_MG_A_ATTACK3 = 28,	// 장판공격
	LA_MG_A_ATTACK4 = 29,	// 대규모 장판공격
	//LA_MG_A_ATTACK5 = 30,
	LA_MG_A_APPEAR = 34,
	LA_MG_A_STDALT1 = 35,
	LA_MG_A_STDALT2 = 36,
	LA_MG_A_STDIDL = 37,
	LA_MG_A_STUN = 38,
	LA_MG_A_WALK = 39
};

BEGIN(Client)

class CLivingArmor_Mage final : public CMonster
{
public:
	struct LIVINGARMOR_MAGE_DESC
	{
		CMonster::MONSTER_DESC pTarget;
	};

public:
	CLivingArmor_Mage();
	virtual ~CLivingArmor_Mage()  override;

public:
	HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CLivingArmor_Mage> Create(_float3 _vCreatePos);

protected:
	HRESULT AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	virtual void EnableCollision(const char* _strShapeTag) override;
	virtual void DisableCollision(const char* _strShapeTag) override;

	void		ActiveElectricBall();			// 직선 구체 발사
	void		ActiveElectricBall2(_float3 _vCreatePos, _int _iCount = 5);			// 유도 구체들(Count개) 소환
	void		DisActiveElectricBall();

	HRESULT		AddPartObjects(string _strModelKey);
	void		UpdateDistanceState();
	void		UpdateCoolTime(_float _fTimeDelta);

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

	_float3		RandomBindMagic2Pos();

private:
	_int		m_iEffectIdx = 0;
	_int		m_iEffectIdx2 = 0;

	_int		m_iBaseState = LA_MG_B_BEFOREMEET;		// Base상태
	_int		m_iDistanceState = LA_MG_D_BEFOREMEET;	// 거리에 따른 상태
	_int		m_iActionState = LA_MG_A_DOWN2;			// 행동상태

	_float		m_fAttack1Range = 10.f;					// 공격1 Range	att2,3 fury att2,3,4
	_float		m_fAttack2Range = 50.f;					// 공격2 Range	att1,2 fury att1,2,4
	_float		m_fOutRange = 10000.f;					// 범위에서 나간 Range
	_float		m_fMeetRange = 23.f;					// 조우 Range

	_float		m_fCalculateTime = 0.f;
	_float		m_fCalculateTime2 = 0.f;

	_bool		m_bEnergyGen = true;

	// 전기포
	vector<shared_ptr<CElectricBall>>		m_vecElectricBall;			// 직선 구체
	vector<shared_ptr<CElectricBall>>		m_vecElectricBall2;			// 유도 구체

	_int		m_iElectricBallCount = 5;
	_int		m_iElectricBallCount2 = 20;
	_int		m_iElectricBallFireCount = 0;

	_float		m_fBallSummonIntervel1 = 20.f;
	_float		m_fBallSummonIntervel2 = 10.f;
	_float		m_fCalculateSummonTime = 0.f;

	_float		m_fElectricBallStartTime = m_fKeyFrameSpeed * 40.f;
	_float		m_fElectricBallFStartTime = m_fKeyFrameSpeed * 40.f / 12.f * 10.f;
	_bool		m_bElectricBallFire = false;

	// 키프레임 시간 관련
	_float		m_fEnergyStartTime = m_fKeyFrameSpeed * 48;
	_float		m_fEnergyEndTime = m_fKeyFrameSpeed * 98;
	_float		m_fFEnergyStartTime = m_fKeyFrameSpeed * 98 / 6.f * 5.f;
	_float		m_fFEnergyEndTime = m_fKeyFrameSpeed * 98 / 6.f * 5.f;

	_bool		m_bEnergyStart = false;
	_bool		m_bEnergyEnd = false;

	// 쿨타임
	_bool		m_bAttackReady[7] = { false, true, true, false, false, false, true };
	_float		m_fAttackCoolTime[7] = { 1.6f, 2.8f, 3.f, 2.5f, 2.8f, 2.2f, 2.5f };
	_float		m_fBAttackCoolTime[7] = { 1.6f, 2.8f, 3.f, 2.5f, 2.8f, 2.2f, 2.5f };
	
	_int		m_NormalAttackCount = 3;	// Normal 상태 공격 종류
	_int		m_FuryAttackCount = 4;		// Fury 상태 공격 종류

	// 광폭화되는 hp
	_float		m_fFuryHp = 150.f;
	// 광폭화로 빨라지는 속도
	_float		m_fFuryActionSpeed = 1.2f;

	// 광폭화시 속도
	_float		m_fFurySpeed = 1.2f;
};

END