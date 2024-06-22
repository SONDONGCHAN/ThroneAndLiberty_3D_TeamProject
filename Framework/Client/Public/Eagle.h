#pragma once
#include "Polymorph.h"

BEGIN(Engine)

END

BEGIN(Client)
class CCameraSpherical;

class CEagle : public CPolymorph
{
public:
	enum ANIMAL_BASESTATE
	{
		STATE_LOOP, STATE_ACTIVE, STATE_END
	};
	enum EAGLE_ANIMATION
	{
		ANIMAL_EAGLE_EMOTION_1		= 0, // o

		ANIMAL_EAGLE_TRANS_START	= 31, // o

		ANIMAL_EAGLE_TRANS_LAND_START	= 16, // 
		ANIMAL_EAGLE_TRANS_LAND_END		= 18, // o

		ANIMAL_EAGLE_TRANS_LAND_DIVE = 2, // o

		ANIMAL_EAGLE_GLIDING_NORMAL			= 19, //
		ANIMAL_EAGLE_GLIDING_NORMAL_UP		= 27, // o
		ANIMAL_EAGLE_GLIDING_NORMAL_L_START = 21, // o
		ANIMAL_EAGLE_GLIDING_NORMAL_L		= 20, //
		ANIMAL_EAGLE_GLIDING_NORMAL_L_UP	= 28, // o
		ANIMAL_EAGLE_GLIDING_NORMAL_R_START = 23, // o
		ANIMAL_EAGLE_GLIDING_NORMAL_R		= 22, //
		ANIMAL_EAGLE_GLIDING_NORMAL_R_UP	= 29, // o

		ANIMAL_EAGLE_GLIDING_NORMAL_TO_FAST = 26, // o
		ANIMAL_EAGLE_GLIDING_FAST_TO_NORMAL	= 11, // o

		ANIMAL_EAGLE_GLIDING_FAST			= 6, //
		ANIMAL_EAGLE_GLIDING_FAST_UP		= 12, // o
		ANIMAL_EAGLE_GLIDING_FAST_L_START	= 8, // o
		ANIMAL_EAGLE_GLIDING_FAST_L			= 7, //
		ANIMAL_EAGLE_GLIDING_FAST_L_UP		= 13, // o
		ANIMAL_EAGLE_GLIDING_FAST_R_START	= 10, // o
		ANIMAL_EAGLE_GLIDING_FAST_R			= 9, //
		ANIMAL_EAGLE_GLIDING_FAST_R_UP		= 14, // o

		ANIMAL_EAGLE_GLIDING_NORMAL_TO_DIVE	= 25, // o
		ANIMAL_EAGLE_GLIDING_DIVE			= 1, //
		ANIMAL_EAGLE_GLIDING_DIVE_TO_NORMAL = 5, // o
	};

public:
	CEagle();
	virtual ~CEagle()  override;

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
	_bool	EmotionControl();						// 감정표현 제어
	_bool	SprintControl(_float _fTimeDelta);		// 가속 제어
	_bool	DiveControl(_float _fTimeDelta);		// 강하 제어
	_bool	DirectionControl(_float _fTimeDelta);	// 방향 제어
	void	GravityControl(_float _fTimeDelta);		// 중력 제어
	void	EffectControl();						// 이펙트 제어

	// 공통 제어 
	void	CommonTick(_float _fTimeDelta);
	void	ChangeAnim(_uint _iAnimIndex, _float _fChangingDelay, _bool _isLoop = false, _float _fCancleRatio = 1.f);
	_float3	SetDirectionFromCamera(_float _fDgree, _bool _isSoft = true);	// 카메라를 기준으로 방향을 설정
	_bool	PlayAnimationSet();											// 애니메이션 세트 재생
	void	CancleAnimationSet();										// 애니메이션 세트 재생 캔슬
	void	TriggerAnimationSet(string _strSetName, _uint _SetFlag);	// 애니메이션 세트 재생 트리거
	void	PolymorphControl(_float _fTimeDelta);	// 변신 컨트롤

private:
	_uint	m_iSetTrigger		= { false };	// 애니메이션 세트 재생 플래그 [0==종료, 1==!Loop, 2==Loop]
	_uint	m_iAnimSetCount		= { 0 };		// 애니메이션 세트 카운트
	_bool	m_bChanging			= { false };	// 변신 중,,
	_float	m_fChangeingTime	= { 0.f };		// 변신 시간
	_bool	m_isSprint			= { false };	// 고속비행상태
	_bool	m_isPrevSprint		= { false };	// 이전 고속비행상태
	_bool	m_isDive			= { false };	// 하강상태
	_bool	m_isPrevDive		= { false };	// 이전 하강상태
	_float	m_fDiveFrontSpeed	= { 11.f };		// 하강 시, 전진 속도 보간값
	_uint	m_iDirection		= { 1 };	// 방향상태 [0==좌회전, 1==정면, 2==우회전]


	_uint	m_iLanded = { 0 };		// 0:공중, 1:일반착지, 2:다이브착지

	// 중력 제어
	_float	m_fUpGravity		= { -4.f };		// 상승시 속도 
	_float	m_fCurrentGravity	= { 0.f };		// 현재 하강속도 
	_float	m_fGravityAccel		= { 0.8f };		// 중력 가속도
	
	vector<tuple<EAGLE_ANIMATION, _float, _float>>	m_vecAnimationSet;	// 현재 애니메이션 세트

private:
	ANIMAL_BASESTATE	m_eCurrentBaseState = { STATE_LOOP };		// 현재 대 상태
	EAGLE_ANIMATION		m_eCurrentAnimation = { ANIMAL_EAGLE_TRANS_START };	// 현재 애니메이션 (전체 or 상체)

private:
	_int	m_iStepEffectIdx = -1;
	string	m_strStepEffectKey = "Eagle_Leave";
	_int	m_iStepEffectTest = 0;

private:
	// 애니메이션 연속 동작 세트들 map<동작 이름, vector<tuple<애니메이션종류, 보간시간, 캔슬비율 > > >
	map<string, vector<tuple<EAGLE_ANIMATION, _float, _float > > >	m_AnimationSets;

private:
	HRESULT AddRigidBody(string _strModelKey);		// RigidBody 추가
	HRESULT MakeAnimationSet();						// 애니메이션 세트 등록


public:
	static shared_ptr<CEagle> Create();

};

END
