#pragma once

#include "Monster.h"
#include "BindMagic2.h"
#include "Thunder1.h"
// 공격(번개소환, 속박마법, 근접공격), 근접공격 패링가능
// 기본 바닥에서 올라오기

// CoolTime
enum MI_SH_CoolTime
{
	MI_SH_C_NORMALATTACK1 = 0,
	MI_SH_C_NORMALATTACK2,
	MI_SH_C_NORMALATTACK3
};

// Base 상태
enum MI_SH_BaseState
{
	MI_SH_B_BEFOREMEET = 0,
	MI_SH_B_GENERATE,
	MI_SH_B_NORMAL,
	MI_SH_B_HITTED,
	MI_SH_B_HITTED2,
	MI_SH_B_HITTED3,
	MI_SH_B_STUN,
	MI_SH_B_SHOCKED,
	MI_SH_B_DOWN,
	MI_SH_B_DEATH
};

// 거리에 따른 상태
enum MI_SH_DistanceState
{
	MI_SH_D_MOVERANGE = 0,
	MI_SH_D_ATTACK1RANGE,
	MI_SH_D_ATTACK2RANGE,
	MI_SH_D_OUTRANGE,
	MI_SH_D_BEFOREMEET
};

// 특정 액션 상태
enum MI_SH_ActionState
{
	MI_SH_A_BLOCKED = 0,
	MI_SH_A_DEATH = 1,
	MI_SH_A_DEAD = 2,
	MI_SH_A_DOWN3 = 5,
	MI_SH_A_DOWN2 = 6,
	MI_SH_A_DOWN1 = 7,
	MI_SH_A_FRONTBE = 11,
	MI_SH_A_FRONTBL = 12,
	MI_SH_A_FRONTBS = 13,
	MI_SH_A_RUN = 18,
	MI_SH_A_SEARCH = 21,
	MI_SH_A_SHOCKED = 22,
	MI_SH_A_ATTACK1 = 25,	// 속박마법
	MI_SH_A_ATTACK2 = 26,	// 번개마법
	MI_SH_A_ATTACK3 = 27,	// 근접공격
	MI_SH_A_DANCE1 = 28,
	MI_SH_A_DANCE2 = 29,
	MI_SH_A_DANCE3 = 30,
	MI_SH_A_STDALT = 34,
	MI_SH_A_STDIDL = 35,
	MI_SH_A_STUN = 36,
	MI_SH_A_WALK = 37
};

BEGIN(Client)

class CMidget_Shaman final : public CMonster
{
public:
	struct MIDGET_SHAMN_DESC
	{
		CMonster::MONSTER_DESC pTarget;
	};

public:
	CMidget_Shaman();
	virtual ~CMidget_Shaman()  override;

public:
	HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CMidget_Shaman> Create(_float3 _vCreatePos);

protected:
	HRESULT		AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	virtual void EnableCollision(const char* _strShapeTag) override;
	virtual void DisableCollision(const char* _strShapeTag) override;

	void		ActiveBindMagic2();
	void		ActiveThunderMagic();

	HRESULT		AddPartObjects(string _strModelKey);
	void		UpdateDistanceState();
	void		UpdateCoolTimeRange(_float _fTimeDelta);
	void		UpdateCoolTimeMelee(_float _fTimeDelta);

	// BaseState 따른 FSM
	void		FSM_BeforeMeet(_float _fTimeDelta);
	void		FSM_Generate(_float _fTimeDelta);
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
	_int		m_iBaseState = MI_SH_B_BEFOREMEET;		// Base상태
	_int		m_iDistanceState = MI_SH_D_BEFOREMEET;	// 거리에 따른 상태
	_int		m_iActionState = MI_SH_A_DANCE3;			// 행동상태

	_float		m_fAttack1CoolTimeRange = 5.f;			// 근접공격 쿨주는 거리
	_float		m_fAttack1Range = 2.2f;					// 공격 Range
	_float		m_fAttack2Range = 14.f;					// 마법 Range
	_float		m_fOutRange = 10000.f;					// 범위에서 나간 Range
	_float		m_fMeetRange = 20.f;					// 조우 Range
	_float		m_fFightRange = 10.f;					// 싸움 Range

	_float		m_fCalculateTime = 0.f;					// 계산용 Time

	// 속박마법
	vector<shared_ptr<CBindMagic2>>		m_vecBindMagic2;
	_int		m_iBindMagic2Count = 3;

	_float		m_fBindMagic2StartTime = m_fKeyFrameSpeed * 33.f;
	_bool		m_bBindMagic2Fire = false;

	// 번개마법
	vector<shared_ptr<CThunder1>>		m_vecThunderMagic;
	_int		m_iThunder1Count = 3;

	_float		m_fThunderMagicStartTime = m_fKeyFrameSpeed * 48.f;
	_bool		m_bThunderMagicFire = false;
	
	// 쿨타임
	_bool		m_bAttackReady[3] = { true, false, false };
	_float		m_fAttackCoolTime[3] = { 3.f, 4.f, 1.5f };
	_float		m_fBAttackCoolTime[3] = { 3.f, 4.f, 1.5f };

	// 콜라이더 정리
	const char* m_strATT3Collider1 = "M_L1Midget_Shaman_SK.ao|M_L1Midget_Shaman_SkdmgC";

	_int		m_NormalAttackCount = 3;	// Normal 상태 공격 종류

	/*Beard Color*/
private:
	_float3 m_vHairMainColor = { 1.f, 1.f, 1.f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };

private: /* UI Test */
	_bool m_isOnce{ true };

};

END