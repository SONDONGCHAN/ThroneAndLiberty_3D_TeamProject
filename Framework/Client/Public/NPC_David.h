#pragma once

#include "NPC.h"
#include "NPC_MonsterDavid.h"

// Dialog Order 순서
enum NPC_David_DialogOrderState
{
	DAVID_DI_FIRSTMEET = 0,			// 처음 만났을때
	DAVID_DI_MOVETOTRAINING,	// 트레이닝 장소로 이동
	DAVID_DI_PRACTICEMEET1,			// 훈련장에서 상태, 플레이어는 허수아비 때리기
	DAVID_DI_PRACTICEMEET2,			// 주인공한테 맞기
	DAVID_DI_PRACTICEMEET3,			// 주인공 떄리기
	DAVID_DI_PRACTICEMEET4,			// 주인공과 대련
	DAVID_DI_PRACTICEMEET5,			// 대련이후
	DAVID_DI_MOVEBACKTO				// 원래위치로 돌아가기
};

// Base 상태
enum NPC_David_BaseState
{
	DAVID_B_NORMAL,	
	DAVID_B_HITTED,
	DAVID_B_HITTED2,
	DAVID_B_HITTED3,
	DAVID_B_STUN,
	DAVID_B_SHOCKED,
	DAVID_B_DOWN
};

// 거리에 따른 상태
enum NPC_David_DistanceState
{
	DAVID_D_FARRANGE = 0,			// 멀때
	DAVID_D_MIDDLE,					// 중간
	DAVID_D_CLOSE,					// 매우 가까움
};

// 특정 액션 상태
enum NPC_David_ActionState
{
	DAVID_A_BLADEDANCE1 = 0,		// 회전 공격1
	DAVID_A_BLADEDANCE2 = 1,		// 회전 공격2
	DAVID_A_BLADEDANCE3 = 2,		// 회전 공격3
	DAVID_A_BLOCKED = 3,
	DAVID_A_SWORDIN = 8,			// 검 집어넣기
	DAVID_A_SWORDOUT = 9,			// 검 꺼내기
	DAVID_A_TALK1 = 10,				// 가리키며 말
	DAVID_A_TALK2 = 11,				// 왼쪽지시
	DAVID_A_TALK3 = 12,				// 당황
	DAVID_A_TALK4 = 13,				// 의아함
	DAVID_A_DASH1 = 14,				// 대쉬 공격1
	DAVID_A_DASH2 = 15,				// 대쉬 공격2
	DAVID_A_IDLE = 18,
	DAVID_A_DOWN3 = 19,
	DAVID_A_DOWN2 = 20,
	DAVID_A_DOWN1 = 21,
	DAVID_A_FRONTBE = 29,
	DAVID_A_RUN = 31,				// 달리기
	DAVID_A_SHOCKED = 32,			// 감전
	DAVID_A_STDALT = 33,			// 전투태세
	DAVID_A_STUN = 34,			
	DAVID_A_ATT1 = 35,				// 공격1(아래로찍기)
	DAVID_A_ATT1_2 = 36,			// 공격1(아래로찍기)
	DAVID_A_ATT2 = 37,				// 공격2(위로올리기)
	DAVID_A_ATT2_2 = 38,			// 공격2(위로올리기)
	DAVID_A_ALTRUN = 39,			// 전투상태 달리기
	DAVID_A_ALTWALK = 40,			// 전투상태 걷기
	DAVID_A_WALK = 45,
};

BEGIN(Client)

class CNPC_David final : public CNPC
{
public:
	CNPC_David();
	virtual ~CNPC_David() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CNPC_David> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f, 0.f, 0.f),
		_float3 _vMovePos = _float3(0.f, 0.f, 0.f), _float3 _vSecondPos = _float3(0.f, 0.f, 0.f));

public:
	virtual void InteractionOn();

	void		SetMovePos(_float3 _vMovePos) { m_vMovePos = _vMovePos; }
	void		SetSecondPos(_float3 _vSecondPos) { m_vTrainPos = _vSecondPos; }
	void		SetInitPos(_float3 _vInitPos) { m_vInitPos = _vInitPos; }

public: /* For Quest Skip */
	void SetMoveToTrainingTrigger(_bool _bMoveToTrainingTrigger) { m_bMoveToTraining1Trigger = _bMoveToTrainingTrigger; }
	void SetQuestSkipTime(_float _fQuestSkipTime) { m_fQuestSkipTime = _fQuestSkipTime; }
	void IncreaseDialogueOrder() { m_iDialogOrder++; }

protected:
	HRESULT			AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void	OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void	OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void	OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	HRESULT		AddPartObjects(string _strModelKey);
	void		UpdateDistanceState();
	void		RenderBackWeapon();
	void		RenderHandWeapon();
	void		ResetTimer();
	void		AfterSwitch();

	// DialogOrder상태에 따른 상태
	void		FSM_FirstMeet(float _fTimeDelta);
	void		FSM_MoveToTraining(float _fTimeDelta);
	void		FSM_PracticeMeet1(float _fTimeDelta);
	void		FSM_PracticeMeet2(float _fTimeDelta);
	void		FSM_PracticeMeet3(float _fTimeDelta);
	void		FSM_PracticeMeet4(float _fTimeDelta);
	void		FSM_PracticeMeet5(float _fTimeDelta);
	void		FSM_MoveBackTo(float _fTimeDelta);

	// 거리
	// 대화 상호작용을 위한 거리
	_float		m_fMeetRange = 10.f;
	_float		m_fCloseRange = 3.f;

	// 전투를 위한 거리
	_float		m_fAttack1Range = 2.f;		// 근접공격 Range
	_float		m_fOutRange = 10000.f;		// 범위에서 나간 Range



private:
	_bool		m_bMoveToTraining1Trigger = false;
	_bool		m_bMoveStart = false;
	_bool		m_bMoveToTraining1 = false;	// true이면 Traning으로 이동시작
	_bool		m_bMoveToTraining2 = false;	// true이면 Traning으로 이동시작
	_bool		m_bMoveToBack1 = false;	// true이면 원래위치로 이동시작
	_bool		m_bMoveToBack2 = false;	// true이면 원래위치로 이동시작
	_bool		m_bTrainStartTiming = false;

	// 훈련시 받을 위치
	_float3		m_vCreatePos = _float3(0.f, 0.f, 0.f);

	// 애니메이션 속도조절
	_float		m_fActionAnimSpeed[100] = { 0, };

	_int		m_iBaseState = DAVID_B_NORMAL;			// Base 상태
	_int		m_iDistanceState = DAVID_D_FARRANGE;	// 거리에 따른 상태
	_int		m_iActionState = DAVID_A_IDLE;			// 행동상태

	// 애니메이션 시간
	_float		m_fWeaponOuTime = m_fKeyFrameSpeed * 18.f;	// 무기 꺼내는 시간
	_float		m_fWeaponInTime = m_fKeyFrameSpeed * 22.f;	// 무기 집어넣는 시간

	_float3		m_vMovePos = _float3(0.f, 0.f, 0.f);		// 훈련장 가는 경유지
	_float3		m_vTrainPos = _float3(0.f, 0.f, 0.f);		// 훈련장에서의 위치
	_float3		m_vInitPos = _float3(0.f, 0.f, 0.f);		// 원래 위치

	_bool		m_bHandWeaponRender = false;
	_bool		m_bBackWeaponRender = true;

	// 애니메이션 변경 시간
	_float		m_fToIdle = 0.5f;
	_float		m_fToTalk = 0.5f;

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

private: /* Dialogue Complete */
	_bool m_bIsDialogueCompleted{ false };

private: /* For Quest Skip */
	_float m_fQuestSkipTime{ 20.f };

};

END