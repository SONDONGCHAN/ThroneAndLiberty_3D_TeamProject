#pragma once
#include "Polymorph.h"

BEGIN(Engine)

END

BEGIN(Client)
class CCameraSpherical;

class CWolf : public CPolymorph
{
public:
	enum ANIMAL_BASESTATE
	{
		STATE_LOOP, STATE_ACTIVE, STATE_END
	};
	enum WOLF_ANIMATION
	{
		ANIMAL_WOLF_EMOTION_1			= 9, //o
		ANIMAL_WOLF_EMOTION_2			= 0, //o
		ANIMAL_WOLF_IDLE				= 1,
		ANIMAL_WOLF_RUN_NORMAL			= 7, //o
		ANIMAL_WOLF_RUN_FAST			= 6, //o
		ANIMAL_WOLF_RUN_NORMAL_TO_FAST	= 8,
		ANIMAL_WOLF_JUMP_START			= 5, //o
		ANIMAL_WOLF_JUMP_LOOP			= 3,
		ANIMAL_WOLF_JUMP_END			= 2, //o
		ANIMAL_WOLF_JUMP_END_RUN		= 4 //o
	};

public:
	CWolf();
	virtual ~CWolf()  override;

public:
	virtual HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render()  override;

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;

	virtual void EnableCollision(const char* _strShapeTag) override;
	virtual void DisableCollision(const char* _strShapeTag) override;

	virtual void PlayEffect(string _strGroupID) override;

public:
	virtual HRESULT RenderShadow() override;

private:
	// FSM 제어
	void	LoopStateFSM(_float _fTimeDelta);
	void	ActiveStateFSM(_float _fTimeDelta);

	// 세부 제어
	 _bool	MoveControl(_float _fTimeDelta, _bool _bMove = true);	// 이동 제어	
	 void	LandingControl();	// 착지 제어
	 _bool	EmotionControl();	// 감정표현 제어


	// 공통 제어 
	void	CommonTick(_float _fTimeDelta);
	void	ChangeAnim(_uint _iAnimIndex, _float _fChangingDelay, _bool _isLoop = false, _float _fCancleRatio = 1.f);
	_float3	SetDirectionFromCamera(_float _fDgree, _bool _isSoft = true);	// 카메라를 기준으로 방향을 설정
	_bool	PlayAnimationSet();											// 애니메이션 세트 재생
	void	CancleAnimationSet();										// 애니메이션 세트 재생 캔슬
	void	TriggerAnimationSet(string _strSetName, _uint _SetFlag);	// 애니메이션 세트 재생 트리거
	void	PolymorphControl(_float _fTimeDelta);	// 변신 컨트롤

private:
	_uint	m_iSetTrigger	= { false };	// 애니메이션 세트 재생 플래그 [0==종료, 1==!Loop, 2==Loop]
	_uint	m_iAnimSetCount = { 0 };		// 애니메이션 세트 카운트
	_bool	m_isJumpState	= { false };		// 점프 상태인지
	_bool	m_isLanding		= { true };	// 착지상태
	_int	m_iJumpFlag		= { -1 };		// 점프 상태 플래그 [-1==점프x, 0==제자리점프, 1==이동점프, 2==질주점프]
	_bool	m_bChanging		= { false };	// 사람으로 변신 중,,
	_float	m_fChangeingTime = { 0.f };		// 사람으로 변신하는데 걸리는 시간

	_float			m_fJumpMinActiveTime = { 0.f };	// 점프 최소 유지 시간
	_bool			m_isJumpMinActive = { false };	// 점프 최소 유지 중

	vector<tuple<WOLF_ANIMATION, _float, _float>>	m_vecAnimationSet;	// 현재 애니메이션 세트

	_bool	m_isMoveState = { false };	// 움직이는 상태인지
	_bool	m_isSprintState = { false }; // 질주 상태인지

private:
	ANIMAL_BASESTATE	m_eCurrentBaseState = { STATE_LOOP };		// 현재 대 상태
	WOLF_ANIMATION		m_eCurrentAnimation = { ANIMAL_WOLF_IDLE };	// 현재 애니메이션 (전체 or 상체)

private:
	_int	m_iStepEffectIdx = -1;
	string	m_strStepEffectKey = "Wolf_Dust";
	_int	m_iTestKeyInput = 0;

private:
	// 애니메이션 연속 동작 세트들 map<동작 이름, vector<tuple<애니메이션종류, 보간시간, 캔슬비율 > > >
	map<string, vector<tuple<WOLF_ANIMATION, _float, _float > > >	m_AnimationSets;

private:
	HRESULT AddRigidBody(string _strModelKey);		// RigidBody 추가
	HRESULT MakeAnimationSet();						// 애니메이션 세트 등록


public:
	static shared_ptr<CWolf> Create();

};

END
