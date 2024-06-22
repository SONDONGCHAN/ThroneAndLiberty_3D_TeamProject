#pragma once

#include "InteractionObj.h"
#include "RigidBody.h"

BEGIN(Client)

enum MEMORY_TYPE
{
	MEMORY_MIDGET = 0,
	MEMORY_ORC,
};

class CMemory : public CInteractionObj
{
public:
	CMemory();
	virtual ~CMemory() = default;

public:
	virtual HRESULT Initialize(_float3 _vPosition);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render();

public:
	virtual void InteractionOn();
	void	SetCreatePos(_float3 _vCreatePos) { m_vCreatePos = _vCreatePos; }

	void	SimulationOn() { m_pRigidBody->SimulationOn();}
	void	SimulationOff() { m_pRigidBody->SimulationOff();}

	void	SetLookDir(_float3 _vLookDir) { m_vLookDir = _vLookDir; }
	void	SetGenType(_int _iGenType) { m_iGenType = _iGenType; }

	HRESULT		AddRigidBody();		// RigidBody 추가
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

public:
	static shared_ptr<CMemory> Create(_float3 _vPosition, _int _iGenType);

protected:
	shared_ptr<CRigidBody>	m_pRigidBody = { nullptr };		// Rigid Body

private:
	_float3			m_vCurrentPos = _float3(0.f, 0.f, 0.f);
	_bool			m_bUp = true;
	_bool			m_bMove = false;
	_float3			m_vLookDir = _float3(0.f, 0.f, 0.f);
	_bool			m_bFallGround = false;
	_float3			m_vCreatePos = _float3(0.f, 0.f, 0.f);

	_int			m_iGenType = 0;
	_float			m_fMoveTime = 0.f;

	_bool			m_IsInteractionDone = false;
	_bool			m_IsNoiseDone = false;
	_bool			m_IsZoomDone = false;
	_bool			m_IsFadeEvent = false;

	_float			m_fEventAccTime = 0.f;

	_float			m_fCalculateTime = 0.f;			// 계산용타임
};

END