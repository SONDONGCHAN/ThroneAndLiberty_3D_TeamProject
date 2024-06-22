#pragma once
#include "Polymorph.h"

#include "Client_Defines.h"
#include "Weapon.h"
#include "Equipment.h"
#include "PlayerRopeLine.h"

BEGIN(Engine)
class CModel;
class CRigidBody;
END

BEGIN(Client)
class CCameraSpherical;

class CPlayer : public CPolymorph
{
public:
	enum PLAYER_WEAPON_TYPE
	{
		WEAPON_TYPE_SWORD, WEAPON_TYPE_CROSSBOW, WEAPON_TYPE_STAFF, WEAPON_TYPE_END
	};
	enum PLAYER_BASESTATE
	{
		STATE_LOOP, STATE_ACTIVE, STATE_ISDAMAGED, STATE_END
	};
	enum PLAYER_ABNORMAL
	{
		ABNORMAL_NORMAL,					// 일반 피격
		ABNORMAL_SHORTHITTED,				// 짧은 경직
		ABNORMAL_LONGHITTED,				// 긴 경직
		ABNORMAL_BIND,						// 속박
		ABNORMAL_SHOCK,						// 감전
		ABNORMAL_GOOUT,						// 날라가기(약)
		ABNORMAL_GOOUT_STRONG				// 날라가기(강)
	};
	enum PLAYER_SWORD_SKILL
	{
		SWORD_SKILL_PARRYING, SWORD_SKILL_BRAVE_STRIKE, SWORD_SKILL_RUSH_ATTACK, 
		SWORD_SKILL_GAIA_CRASH, SWORD_SKILL_UPPER_ATTACK, SWORD_SKILL_SPIN_ATTACK,
		SWORD_SKILL_END
	};
	enum PLAYER_CROSSBOW_SKILL
	{
		CROSSBOW_SKILL_PARRYING, CROSSBOW_SKILL_RAPIDSHOT, CROSSBOW_SKILL_MULTIPLESHOT,
		CROSSBOW_SKILL_INDOMITABLESPIRIT, CROSSBOW_SKILL_ELECTRICSHOT, CROSSBOW_SKILL_FURIOUSFIRE,
		CROSSBOW_SKILL_END	
	};
	enum PLAYER_STAFF_SKILL
	{
		STAFF_SKILL_PARRYING, STAFF_SKILL_FIREBALL, STAFF_SKILL_ICESPEAR,
		STAFF_SKILL_FIREBLAST, STAFF_SKILL_LIGHTNIGJUDGMENT, STAFF_SKILL_METEOR,
		STAFF_SKILL_END
	};
	enum PLAYER_ANIMATION
	{
		//====================================
		// Sword
		//====================================	
		//===========Sword 기본 모션===========
		WEAPON_SWORD_IDLE = 311,
		WEAPON_SWORD_RUN = 349, //o
		WEAPON_SWORD_ROLLING = 361, //o
		WEAPON_SWORD_PUTOUT = 313, //o

		WEAPON_SWORD_JUMP_READY = 328, 
		WEAPON_SWORD_JUMP_MAIN = 333, //o
		WEAPON_SWORD_JUMP_LOOP = 325, 
		WEAPON_SWORD_JUMP_LAND = 324, //o

		WEAPON_SWORD_JUMP_READY_RUN = 329,
		WEAPON_SWORD_JUMP_MAIN_RUN = 334, //o
		WEAPON_SWORD_JUMP_LOOP_RUN = 326,
		WEAPON_SWORD_JUMP_LAND_RUN = 331,

		WEAPON_SWORD_PARRYING = 369, // o
		WEAPON_SWORD_PARRYING_ATTACK = 370,
		//=======================================

		//===========Sword 공격 모션===========
		WEAPON_SWORD_ATTACK_COMBO_1 = 375,
		WEAPON_SWORD_ATTACK_COMBO_2 = 376,
		WEAPON_SWORD_ATTACK_COMBO_3 = 377,
		WEAPON_SWORD_ATTACK_COMBO_4 = 403,

		WEAPON_SWORD_SKILL_BRAVE_STRIKE_1 = 394,
		WEAPON_SWORD_SKILL_BRAVE_STRIKE_2 = 395,
		WEAPON_SWORD_SKILL_BRAVE_STRIKE_3 = 397,

		WEAPON_SWORD_SKILL_RUSH_ATTACK = 360,

		WEAPON_SWORD_SKILL_GAIA_CRASH = 389,

		WEAPON_SWORD_SKILL_UPPER_ATTACK = 410,

		WEAPON_SWORD_SKILL_SPIN_ATTACK_1 = 405,
		WEAPON_SWORD_SKILL_SPIN_ATTACK_2 = 406,
		WEAPON_SWORD_SKILL_SPIN_ATTACK_3 = 407,
		//=======================================

		//============Sword 필살 모션============
		WEAPON_SWORD_SUPREME_SKILL_READY	= 382,
		WEAPON_SWORD_SUPREME_SKILL_ATTACK	= 370,
		//=======================================
		
		//===========Sword 피격 모션===========
		WEAPON_SWORD_DAMAGED_NORMAL_B		= 304,
		WEAPON_SWORD_DAMAGED_NORMAL_F		= 305,
		WEAPON_SWORD_DAMAGED_NORMAL_L		= 306,
		WEAPON_SWORD_DAMAGED_NORMAL_R		= 307,
		WEAPON_SWORD_DAMAGED_STIFF_SHORT	= 274, //o
		WEAPON_SWORD_DAMAGED_STIFF_LONG_S	= 280,
		WEAPON_SWORD_DAMAGED_STIFF_LONG_L	= 279, //o
		WEAPON_SWORD_DAMAGED_STIFF_LONG_E	= 278,
		WEAPON_SWORD_DAMAGED_DOWN_START		= 286,//o
		WEAPON_SWORD_DAMAGED_DOWN_LOOP		= 284,
		WEAPON_SWORD_DAMAGED_DOWN_END		= 282,
		WEAPON_SWORD_DAMAGED_DOWN_LONG_START= 285,
		WEAPON_SWORD_DAMAGED_DOWN_LONG_LOOP	= 283, //o
		WEAPON_SWORD_DAMAGED_DOWN_LONG_END	= 281,
		WEAPON_SWORD_DAMAGED_SHOCK			= 293,
		WEAPON_SWORD_DAMAGED_STUN			= 294, //o
		//========================================


		//=======================================
		// CrossBow
		//=======================================
		//===========CrossBow 기본 모션===========
		WEAPON_CROSSBOW_IDLE = 52,
		WEAPON_CROSSBOW_RUN = 112, //o
		WEAPON_CROSSBOW_BACKSTEP = 136,
		WEAPON_CROSSBOW_ROLLING = 412, //o
		WEAPON_CROSSBOW_PUTOUT = 56, //o

		WEAPON_CROSSBOW_JUMP_READY = 93,
		WEAPON_CROSSBOW_JUMP_MAIN = 96, //o
		WEAPON_CROSSBOW_JUMP_LOOP = 89,
		WEAPON_CROSSBOW_JUMP_LAND = 88, //o

		WEAPON_CROSSBOW_JUMP_READY_RUN = 92,
		WEAPON_CROSSBOW_JUMP_MAIN_RUN = 97, //o
		WEAPON_CROSSBOW_JUMP_LOOP_RUN = 90,
		WEAPON_CROSSBOW_JUMP_LAND_RUN = 94,

		WEAPON_CROSSBOW_PARRYING = 42,
		//=======================================

		//===========CrossBow 공격 모션===========
		WEAPON_CROSSBOW_ATTACK_COMBO_1 = 159,
		WEAPON_CROSSBOW_ATTACK_COMBO_2 = 161,
		WEAPON_CROSSBOW_ATTACK_COMBO_3 = 163,
		WEAPON_CROSSBOW_ATTACK_COMBO_4 = 165,

		WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_1 = 162,
		WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_2 = 164,
		WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_3 = 147,
		WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_4 = 145,

		WEAPON_CROSSBOW_SKILL_RAPIDSHOT			= 74, // 분리가능

		WEAPON_CROSSBOW_SKILL_MULTIPLESHOT		= 68, // 분리가능

		WEAPON_CROSSBOW_SKILL_INDOMITABLESPIRIT = 64, // 분리가능

		WEAPON_CROSSBOW_SKILL_ELECTRICSHOT		= 152,// 분리불가능

		WEAPON_CROSSBOW_SKILL_FURIOUSFIRE_READY = 82, // 분리불가능
		WEAPON_CROSSBOW_SKILL_FURIOUSFIRE_LOOP	= 81, // 분리불가능
		WEAPON_CROSSBOW_SKILL_FURIOUSFIRE_END	= 80, // 분리불가능
		//=======================================

		//===========CrossBow 피격 모션===========
		WEAPON_CROSSBOW_DAMAGED_NORMAL_B		= 46,
		WEAPON_CROSSBOW_DAMAGED_NORMAL_F		= 47,
		WEAPON_CROSSBOW_DAMAGED_NORMAL_L		= 48,
		WEAPON_CROSSBOW_DAMAGED_NORMAL_R		= 49,
		WEAPON_CROSSBOW_DAMAGED_STIFF_SHORT		= 274,	// 양손검 모션
		WEAPON_CROSSBOW_DAMAGED_STIFF_LONG_S	= 19,
		WEAPON_CROSSBOW_DAMAGED_STIFF_LONG_L	= 18, //
		WEAPON_CROSSBOW_DAMAGED_STIFF_LONG_E	= 17,
		WEAPON_CROSSBOW_DAMAGED_DOWN_START		= 25, //
		WEAPON_CROSSBOW_DAMAGED_DOWN_LOOP		= 23,
		WEAPON_CROSSBOW_DAMAGED_DOWN_END		= 21,
		WEAPON_CROSSBOW_DAMAGED_DOWN_LONG_START = 24,
		WEAPON_CROSSBOW_DAMAGED_DOWN_LONG_LOOP	= 22, //
		WEAPON_CROSSBOW_DAMAGED_DOWN_LONG_END	= 20,
		WEAPON_CROSSBOW_DAMAGED_SHOCK			= 293, // 양손검 모션
		WEAPON_CROSSBOW_DAMAGED_STUN			= 32, //
		//========================================


		//=======================================
		// Staff
		//=======================================
		//===========Staff 기본 모션===========
		WEAPON_STAFF_IDLE		= 192,
		WEAPON_STAFF_RUN		= 222, //o
		WEAPON_STAFF_ROLLING	= 234, //o
		WEAPON_STAFF_PUTOUT		= 193, //o

		WEAPON_STAFF_JUMP_READY	= 201,
		WEAPON_STAFF_JUMP_MAIN	= 206, //o 
		WEAPON_STAFF_JUMP_LOOP	= 199,
		WEAPON_STAFF_JUMP_LAND	= 198, //o 

		WEAPON_STAFF_JUMP_READY_RUN = 202,
		WEAPON_STAFF_JUMP_MAIN_RUN	= 207, //o
		WEAPON_STAFF_JUMP_LOOP_RUN	= 200,
		WEAPON_STAFF_JUMP_LAND_RUN	= 204,

		WEAPON_STAFF_PARRYING = 245,
		//=======================================

		//===========Staff 공격 모션===========
		WEAPON_STAFF_ATTACK_COMBO_1 = 237,
		WEAPON_STAFF_ATTACK_COMBO_2 = 239,
		WEAPON_STAFF_ATTACK_COMBO_3 = 240,

		WEAPON_STAFF_SKILL_FIREBALL			= 264,
		WEAPON_STAFF_SKILL_ICESPEAR			= 230,
		WEAPON_STAFF_SKILL_FIREBLAST		= 266,
		WEAPON_STAFF_SKILL_LIGHTNIGJUDGMENT = 229,
		WEAPON_STAFF_SKILL_METEOR			= 268,

		//=======================================

		//===========Staff 피격 모션===========
		WEAPON_STAFF_DAMAGED_NORMAL_B		= 184,
		WEAPON_STAFF_DAMAGED_NORMAL_F		= 185,
		WEAPON_STAFF_DAMAGED_NORMAL_L		= 186,
		WEAPON_STAFF_DAMAGED_NORMAL_R		= 187,
		WEAPON_STAFF_DAMAGED_STIFF_SHORT	= 274, // 양손검 모션 
		WEAPON_STAFF_DAMAGED_STIFF_LONG_S	= 19, // 크로스보우 모션 
		WEAPON_STAFF_DAMAGED_STIFF_LONG_L	= 18, // 크로스보우 모션
		WEAPON_STAFF_DAMAGED_STIFF_LONG_E	= 17, // 크로스보우 모션
		WEAPON_STAFF_DAMAGED_DOWN_START		= 25, // 크로스보우 모션
		WEAPON_STAFF_DAMAGED_DOWN_LOOP		= 23, // 크로스보우 모션
		WEAPON_STAFF_DAMAGED_DOWN_END		= 21, // 크로스보우 모션
		WEAPON_STAFF_DAMAGED_DOWN_LONG_START= 24, // 크로스보우 모션
		WEAPON_STAFF_DAMAGED_DOWN_LONG_LOOP	= 22, // 크로스보우 모션
		WEAPON_STAFF_DAMAGED_DOWN_LONG_END	= 20, // 크로스보우 모션
		WEAPON_STAFF_DAMAGED_SHOCK			= 177,
		WEAPON_STAFF_DAMAGED_STUN			= 178, //
		//========================================
		
		
		//========================================
		// 록온 이동
		LOCK_ON_MOVE_F = 418,
		LOCK_ON_MOVE_L = 421,
		LOCK_ON_MOVE_R = 422,
		LOCK_ON_MOVE_B = 417,
		LOCK_ON_MOVE_FL = 419,
		LOCK_ON_MOVE_FR = 420,
		LOCK_ON_MOVE_BL = 417,
		LOCK_ON_MOVE_BR = 417,
		//========================================


		//========================================
		// 변신 애니메이션
		POLYMORPH_HUMAN_TO_WOLF		= 413, 
		POLYMORPH_WOLF_TO_HUMAN		= 415,
		POLYMORPH_HUMAN_TO_WOLF_RUN = 414,  //o
		POLYMORPH_WOLF_TO_HUMAN_RUN = 416,  //o 
		//========================================

		//========================================
		// 로프액션 애니메이션
		ROPE_ACTION_ALL = 425,
		//========================================

	};
	enum PLAYER_PROJECTILE_OWNER
	{
		CROSSBOW_L, CROSSBOW_R, STAFF, STAFF_HAND, TARGET, FRONT, END_PROJECTILE_OWNER
	};

public:
	CPlayer();
	virtual ~CPlayer()  override;

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
	virtual void ShootProjectile(_int _eProjectileOwner) override;		// Projectile 발사 

public:
	virtual HRESULT RenderShadow() override;

public:
	void	RestoreHp() { m_fCurrentHp = m_fMaxHp; }
	void	SimulationOff();
	void	SetInDungeon(_bool _IsDungeon) { m_IsDungeonLevel = _IsDungeon; }
	void	LockOnChangeToBoss();	// 록온을 보스전 모드로
	void	LockOnChangeToWeapon();	// 록온을 무기별로
	void	LockOnOff();			// 록온끄기
	void	AddSupremeGauge(_float _AddGauge);	// 필살기 게이지 증가


private:
	// FSM 제어
	void	LoopStateFSM(_float _fTimeDelta);
	void	ActiveStateFSM(_float _fTimeDelta);
	void	IsDamagedFSM(_float _fTimeDelta);

	_float	m_fCalculateTime = 0.f;

	// 씬 이동시
	_bool	m_bSceneChange = false;
	_int	m_iChangeScene = 0;

	// 세부 제어
	_bool	MoveControlNormal(_float _fTimeDelta, _bool _bMove = true);	// 일반 이동 상태 제어 [ fasle : 이동x, 이동값 여부만 반환 ] 
	_bool	MoveControlSeparate(_float _fTimeDelta);		// 분리 이동 상태 제어
	_bool	MoveControlSkill(_float _fTimeDelta);			// 스킬 이동 상태 제어
	_bool	ComboAttackControl(_float _fTimeDelta);			// 콤보 공격 제어
	_bool	SkillControl();									// 스킬 공격 제어
	_bool	CheckWeaponChange();							// 무기 타입 변경 체크
	void	AnimationActive(_float _fTimeDelta);			// 애니메이션에 필요한 상태 처리
	void	PlayReservedAnim();								// 예약된 애니메이션 재생
	_bool	PlayAnimationSet();								// 애니메이션 세트 재생
	void	CancleAnimationSet();							// 애니메이션 세트 재생 캔슬
	void	TriggerAnimationSet(string _strSetName, _uint _SetFlag);	// 애니메이션 세트 재생 트리거
	_bool	AssaultAnim(_uint _iAnimIndex, _float _fChangingDelay, _float _fCancleRatio = 1.f);	// 어썰트 애니메이션 
	void	TriggerAssaultAnimSet(string _strSetName, _uint _SetFlag);	// 어썰트 애니메이션 세트
	void	DamagedStateControl();							// 피격 상태 제어
	_bool	RopeActionControl();							// 로프액션 제어


	// 공통 제어
	void	CommonTick(_float _fTimeDelta);					// 공통적으로 돌릴 Tick들을 모아둠
	void	DashMove(_float _fTimeDelta, _float _fRatio);	// 대쉬
	_float3	SetDirectionFromCamera(_float _fDgree, _bool _isSoft = true, _bool _bGetDir = false);	// 카메라를 기준으로 플레이어의 방향을 설정
	void	SetSeparateDir(_float _fTimeDelta, _float _fDgree);	// 상 하체 분리 시, [이동방향, 애니메이션방향] 을 설정
	void	SetToLockOnDir();						// 록온 된 타겟 방향으로 바라봅니다.
	_bool	ChangeAnim(_uint _iAnimIndex, _float _fChangingDelay, _bool _isLoop = false, _float _fCancleRatio = 1.f);	// 애니메이션 변경
	_bool	ReserveAnim(PLAYER_ANIMATION _eReservationAnim, _float _fDashSpeed = 0.f, _float _fCancelRatio = 1.f);		// 애니메이션 예약
	_bool	PlaySkill(_uint _iSkillIndex);			// 스킬 재생
	_bool	PlaySupremeSkill();			// 필살기 재생
	_bool	CheckCanSeparate(_uint _iAnimIndex);	// 현재 애니메이션이 분리 가능한 애니메이션인지 반환
	_bool	CheckCanMovement(_uint _iAnimIndex);	// 현재 애니메이션이 이동 가능한 애니메이션인지 반환
	_bool	CheckSkillAnim(_uint _iAnimIndex);		// 현재 애니메이션이 스킬 사용 애니메이션인지 반환
	_bool	CheckIgnoreAbnormalAnim(_uint _iAnimIndex);		// 현재 애니메이션이 피격 모션 무시 애니메이션인지 반환
	void	LockOnControl();						// 록온 제어
	void	LandingControl();						// 착지 제어
	void	CheckTouched();							// 타겟과 닿아있는지 체크
	void	ShootBuffProjectile(_float3 _vStartPos, _float3 _vTargetPos);		// 버프 추가타 발사
	void	PolymorphControl(_float _fTimeDelta);	// 변신 컨트롤


	// 파츠 랜더
	void	TickWeapon(_float _fTimeDelta);					// 선택된 무기를 Tick
	void	LateTickWeapon(_float _fTimeDelta);				// 선택된 무기를 LateTick
	void	TickEquipment(_float _fTimeDelta);				// 선택된 장비를 Tick
	void	LateTickEquipment(_float _fTimeDelta);			// 선택된 장비를 LateTick

public:
	// 파츠 관리 임시
	void	ChangeEquipment(ITEM_EQUIP_TYPE _eparts, string _ModelName);
	void	NoneEquipment(ITEM_EQUIP_TYPE _eparts);

public:
	void	SetPlayerHp(_float _fHpVariation);
	void	SetUIInputState(_bool _isInputStop) { m_isUIInputStop = _isInputStop; }
	void	SetAllInputState(_bool _isInputStop);
	void	SetRopeTargetPos(_float3 _vRopeTargetPos);
	void	SetHealing();


public:
	_float	GetPlayerHp() { return m_fCurrentHp; }
	_float	GetPlayerHpRatio() { return (m_fCurrentHp/m_fMaxHp) ; }
	PLAYER_ANIMATION	GetCurrentAnimation() { return m_eCurrentAnimation; }
	_bool	IsCrossBowBuffed() { return m_bCrossBowBuff; }


private:
	HRESULT MakeAnimationSet();		// 애니메이션 세트 등록
	HRESULT SettingSkillCoolTime();	// 스킬 쿨타임 세팅
	HRESULT AddSeparateAnims();		// 상, 하체 분리 가능한 애니메이션들 등록
	HRESULT AddMovementAnims();		// 이동만 가능한 애니메이션들 등록
	HRESULT AddSkillAnims();		// 스킬 애니메이션들 등록
	HRESULT AddIgnoreAbnormalAnims();	// 피격모션 무시 가능한 애니메이션들 등록
	HRESULT SetAnimOwner();			// 애니메이션들의 오너 등록
	HRESULT CreateCamera();

private:
	HRESULT AddWeapons();		// 무기 추가
	HRESULT AddEquipments();	// 장비 추가
	HRESULT AddRigidBody(string _strModelKey);	// RigidBody 추가
	HRESULT AddProjectilePool();				// ProjectilePool 추가
	HRESULT AddProjectileTriggers(string _strModelKey);	// ProjectileTriggers 추가
	HRESULT AddRopeLine();		// 로프액션 시, 그려질 라인 추가


public:
	HRESULT ReadyPlayerLight();
	
/*Player Lighting*/
private:
	_uint m_iPlayerLightNum = 0;
	_uint m_iPlayerLightNum2 = 0;


private:
	_bool*					 m_pSeparating= { nullptr };	// 상,하체 분리 중 인지
	vector<PLAYER_ANIMATION> m_SeparateAnims;				// 상, 하체 분리 가능한 애니메이션들
	vector<PLAYER_ANIMATION> m_MovementAnims;				// 이동만 가능한 애니메이션들
	vector<PLAYER_ANIMATION> m_SKillAnims;					// 스킬 애니메이션들
	vector<PLAYER_ANIMATION> m_IgnoreAbnormalAnims;			// 피격모션 무시 가능한 애니메이션들


	_float				m_fDashSpeed		= { 5.f };		// 대쉬 속도

	_float				m_fReserveDashSpeed = { 0.f };		// 대쉬 속도 예약
	_float				m_fReserveCancelRatio = { 1.f };	// CancelRatio 예약
	PLAYER_ANIMATION	m_eReservedAnimation = {};			// 예약 애니메이션 보관
	_bool				m_isReserved = { false };			// 예약된 애니메이션이 있는지
	_float				m_fReservationTimer = { 0.f };		// 예약시간 체크

	_uint				m_iComboCount	= { 0 };			// 콤보 카운트
	_float				m_iComboTimer	= { 0.f };			// 콤보 시간 체크

	_uint				m_iSetTrigger	= { false };	// 애니메이션 세트 재생 플래그 [0==종료, 1==!Loop, 2==Loop]
	_uint				m_iAnimSetCount = { 0 };		// 애니메이션 세트 카운트
	vector<tuple<PLAYER_ANIMATION, _float, _float>>	m_vecAnimationSet;	// 현재 애니메이션 세트

	_float	m_fSKillCoolTime[WEAPON_TYPE_END][SWORD_SKILL_END] = {};	//스킬 쿨타임
	_float	m_fCurrentCoolTime[WEAPON_TYPE_END][SWORD_SKILL_END] = {};	// 남은 쿨타임
	_bool	m_bCanUseSkill[WEAPON_TYPE_END][SWORD_SKILL_END] = {};		// 스킬 사용 가능 여부

	_bool	m_isSeparateLockOn	= { false };			// 카메라 록온 상태인지
	weak_ptr<CGameObject>		m_pCurrentLockOnTarget;	// 록온 대상 오브젝트
	
	_int	m_iCurrentLockOnCursor	= { -1 };			// 현재 록온 커서

	_bool	m_isJumpState	= { true };		// 점프 상태인지
	_bool	m_isLanding		= { false };	// 착지상태

	vector<string>		m_vecParrieds;		// 페링 상태
	map<string, _int>	m_mapAbnormals;		// 피격 상태

	_float			m_fMaxHp		= 1000.f;	// 최대 Hp
	_float			m_fCurrentHp	= 1000.f;	// 현재 Hp

	_bool			m_bRenderHair	= { true };		// 머리카락 랜더 On/Off
	_bool			m_isUIInputStop	= { false };	// KeyInput On/Off (UI)
	_bool			m_isAllInputStop = { false };	// KeyInput On/Off (전부)
	_bool			m_isTouched		= { false };	// 현재 타겟과 닿아있는 상태인지.
	_bool			m_bCrossBowBuff = { false };	// 크로스 보우 자벞 On/Off
	_float			m_fCrossBowBuffPersistTime = { 0 };		// 크로스 보우 자벞 지속시간

	_float			m_fEagleFormDuringTime	= { 0.5f };		// 이글폼 변신 대기 시간(디졸브시간)
	_bool			m_isEagleChanging		= { false };	// 이글폼으로 변신 중인지

	string			m_strEffectGroupID = {""};				// 이펙트 그룹의 ID
	_int			m_EffectGroupIDX	= { -1 };			// 같은종류의 다수인 이펙트 그룹중 몇 번 인덱스인지.

	_float3			m_vRopeTargetDir	= { 0.f, 0.f, 0.f };	// 로프가 닿은 위치를 향한 방향(크기 포함)
	_bool			m_isRopeTarget		= { false };	// 로프 타겟이 있는지
	_float			m_fRopeAnimDur		= { 0.f };		// 로프액션을 시작하고 흐른 시간
	_float			m_fRopeMoveSpeed	= { 0.f };		// 로프액션이동 속도
	_bool			m_isGravity			= { true };		// 중력 적용중인지
	_bool			m_bRopeRend			= { false };	// 로프 랜더하는지

	_bool			m_bCrossHair		= { false };	// 크로스헤어 켜져 있는지 

	_float			m_fSupremeGaugeMax		= { 100 };	// 최대 필살기 게이지
	_float			m_fCurrentSupremeGauge	= { 0 };	// 필살기 게이지
	_bool			m_bCanUseSupreme		= { false };	// 필살기 게이지

	_uint			m_iLockOnMode = { 0 };  // [0==무기별록온 / 1==보스록온 / 2==Off록온]

	_float			m_fJumpMinActiveTime = { 0.f };	// 점프 최소 유지 시간
	_bool			m_isJumpMinActive = { false };	// 점프 최소 유지 중
	_bool			m_bFuriousFire	= { false };
	_float3			m_vFuriousFireStartPos	= { 0.f, 0.f, 0.f };
	_float3			m_vFuriousFireTargetPos = { 0.f, 0.f, 0.f };

	_bool			m_bHealing = { false };
	_float			m_fHealingDelay = { 0.f };

private:
	_bool			m_IsDungeonLevel = false;

private:
	PLAYER_WEAPON_TYPE		m_eCurrentWeapon		= { WEAPON_TYPE_SWORD };	// 현재 무기 타입 상태
	PLAYER_BASESTATE		m_eCurrentBaseState		= { STATE_LOOP };			// 현재 플레이어 대 상태
	PLAYER_ANIMATION		m_eCurrentAnimation		= { WEAPON_SWORD_IDLE };	// 현재 플레이어 애니메이션 (전체 or 상체)

private:
	// 애니메이션 연속 동작 세트들 map<동작 이름, vector<tuple<애니메이션종류, 보간시간, 캔슬비율 > > >
	map<string, vector<tuple<PLAYER_ANIMATION, _float, _float > > >	m_AnimationSets;	

private:
	map<string, shared_ptr<CWeapon>>		m_pPlayerWeapons;
	vector<shared_ptr<CEquipment>>			m_pPlayerEquipments;
	map<string, shared_ptr<CModel>>			m_pPolymorphs;
	vector<weak_ptr<CGameObject>>			m_pLockOnTargets;		// 록온 가능한 타겟들
	vector<weak_ptr<CGameObject>>			m_pTouchedTargets;;		// 닿아있는 타겟들
	shared_ptr<CPlayerRopeLine>				m_pRopeLine = { nullptr };

private:
	shared_ptr<CWeapon>						m_pCurrentWeapon;
	shared_ptr<class CPlayerProjectilePool> m_pProjectilePool;
	_float4x4* pRHandMatrix = { nullptr };
	_float4x4* pLHandMatrix = { nullptr };

/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 0.02f, 0.02f, 0.07f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };

/* For.Trail */
private:
	shared_ptr<class CPointTrail> m_pWeaponTrail;
	HRESULT AddWeaponTrail();		// Add Weapon Trail
	HRESULT ChangeTrailType();		// Change WeaponType, TrailColor
	void	SetWeaponGlow(_bool _bGlow, _bool _bUseGlowColor = false, _float4 _vGlowColor = _float4( 1.f, 1.f, 1.f, 1.f ), _bool _bUseGlowTime = false, _float _fGlowTime = 0.f);
	void	SetWeaponMeshTrail(_bool _bUseMeshTrail, _float4 _vMeshTrailColor = _float4(1.f, 1.f, 1.f, 1.f), _bool _bUseMeshTrailTime = false, _float _fMeshTrailTime = 0.f);
	_bool	TestKeyInput();

	HRESULT AddMotionTrail(_int _iTrailCnt);
	vector<shared_ptr<class CMotionTrail>> m_pMotionTrail;
	HRESULT SetUseMotionTrail(_bool _bUseMotionTrail, _bool _bUseTimer = false, _float _fTimer = 0.f, _bool _bChangeColor = false, _float4 _vColor = _float4());

public:
	static shared_ptr<CPlayer> Create();
};

END