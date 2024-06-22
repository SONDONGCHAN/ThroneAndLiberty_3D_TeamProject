#pragma once

#include "GameObject.h"
#include "PartObject.h"
#include "Client_Defines.h"
#include "RigidBody.h"
#include "EffectMgr.h"


BEGIN(Engine)
class CModel;
class CTexture;
END

BEGIN(Client)

enum ProjectileType
{
	PJ_DIRECT = 0,
	PJ_READYDIRECT,					// 대기후 직선으로 빠르게
	PJ_GUIDED,
	PJ_READYGUIDED,					// 대기후 유도탄
	PJ_FALL,						// 떨어지기
	PJ_SUMMON,						// 소환
	PJ_NONE
};

enum ProjectileAbnormal
{
	PJ_A_NORMAL,					// 일반 피격
	PJ_A_SHORTHITTED,				// 짧은 경직
	PJ_A_LONGHITTED,				// 긴 경직
	PJ_A_GOOUT,						// 날라가기
	PJ_A_BIND,						// 속박
	PJ_A_SHOCK						// 감전
};


class CProjectile : public CGameObject
{
public:
	struct PROJECTILE_DESC
	{
		shared_ptr<CGameObject> pTarget = nullptr;
	};

public:
	CProjectile();
	virtual ~CProjectile()  override;

public:
	virtual HRESULT Initialize(PROJECTILE_DESC* _pProjectileDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render()  override;
	virtual HRESULT RenderGlow() override;

public:
	_float3	GetCurrentPos() { return m_vCurrentPos; }
	void	SetOnlyRender(_bool _bOnlyRender) { m_bOnlyRender = _bOnlyRender; }
	void	OnCollisionProjectile();
	void	OffCollisionProjectile();
	void	CheckCollisionActive();
	void	UseGlow() { m_bUseGlow = true; };
	void	SimulationOn() { m_pRigidBody->SimulationOn(); m_bSimulationOn = true; }
	void	SimulationOff() { m_pRigidBody->SimulationOff();  m_bSimulationOn = false;}

public:
	_int		GetPower() { return m_iPower; }
	void		SetPower(_int _iPower) { m_iPower = _iPower; }
	_int		GetAbnormal() { return m_iAbnormal; }
	void		SetAbnormal(_int _iAbnormal) { m_iAbnormal = _iAbnormal; }
	void		UpdateTargetPos();
	void		UpdateTargetDir(_float3 _vUp);
	void		UpdateDirection();	// 발사체 방향까지 휘어야 할때
	void		MoveToTarget(_float _fTimeDelta);	// 타겟으로 이동
	void		MoveToDir(_float _fTimeDelta);
	void		SetProjectileType(_int _iProjectileType) { m_iProjectileType = _iProjectileType; }
	void		SetReady(_float _fReadyTime) { m_fReadyTime = _fReadyTime; m_bReadyFire = true; }
	void		OnEffect() { m_bStartEffect = true; }

protected:
	void		MovePos(_float3 _vPos);

protected:
	shared_ptr<CModel> m_pModelCom = nullptr;
	shared_ptr<CRigidBody>	m_pRigidBody = { nullptr };				// Rigid Body
	
protected:
	_bool			m_bSimulationOn = false;			// 시뮬레이션이 작동됐는지

	// Tick순서에 따른 충돌체 꺼주기 위함
	_bool			m_bCollisionActive = false;
	_bool			m_bPrevCollisionActive = false;
	const char*		m_strShapeTag = "";

	_bool			m_bActiveOff = false;			// m_bEnable을 latetick마지막에 호출해주기 위함

	_bool			m_bStartEffect = false;

	// 타겟
	weak_ptr<CGameObject>		m_pTarget;
	_float3			m_vTargetPos = _float3(0.f, 0.f, 0.f);
	_float3			m_vTargetDir = _float3(0.f, 0.f, 0.f);
	_float3			m_vCurrentPos = _float3(0.f, 0.f, 0.f);
	_float			m_fDistanceToTarget = 30.f;	// 줄어들면 없어지는 조건때문에 30

	weak_ptr<CGameObject>		m_pOwner;

	// 계산용 시간
	_float			m_fCalculateTime1 = 0.f;
	_float			m_fCalculateTime2 = 0.f;
	_float			m_fCalculateTime3 = 0.f;

	// 생명시간
	_float			m_fLifeTime = 0.f;

	// 발사체 타입
	_int			m_iProjectileType = PJ_DIRECT;
	// 발사체 발사전 대기시간
	_float			m_fReadyTime = 1.f;
	_bool			m_bReadyFire = true;
	// 발사체 위력
	_int			m_iPower = 1;	// (1~3, 3일수록 강력)
	// 발사체 상태이상
	_int			m_iAbnormal = PJ_A_NORMAL;

	// Render만 되게
	_bool			m_bOnlyRender = false;

	// Glow용
	_bool			m_bUseGlow = false;			// 글로우 할지말지
	_bool			m_bUseGlowColor = false;	// 글로우 색상 넣을지 말지 (안넣으면 색깔 없이 살짝 빛남)
	_float4			m_vGlowColor = { 1.f, 1.f, 1.f, 1.f };	// 글로우 색상
};

END