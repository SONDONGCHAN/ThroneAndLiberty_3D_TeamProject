#pragma once

#include "NPC.h"

// Base 상태
enum NPC_RuffaloRice_BaseState
{
	RUFFALO_B_BEFOREMEET = 0,			// 상호작용도 안하고 멀때
	RUFFALO_B_MEET,						// 상호작용은 안했으나 상호작용이 가능할정도로 가까울때
	RUFFALO_B_INTERACT,					// 상호작용할때
};

// 거리에 따른 상태
enum NPC_RuffaloRice_DistanceState
{
	RUFFALO_D_FARRANGE = 0,				// 멀때
	RUFFALO_D_MEETRANGE					// 가까울때
};

// 특정 액션 상태
enum NPC_RuffaloRice_ActionState
{
	RUFFALO_A_BASE = 0,
	RUFFALO_A_IDLE = 4,
	RUFFALO_A_TALK1 = 5,
	RUFFALO_A_TALK2 = 6,
	RUFFALO_A_TALK3 = 7,
	RUFFALO_A_TALK4 = 8,
	RUFFALO_A_TALK5 = 9,
	RUFFALO_A_TALK6 = 10,
	RUFFALO_A_TALK7 = 11,
	RUFFALO_A_TURNL130 = 16,
	RUFFALO_A_TURNL80 = 17,
	RUFFALO_A_TURNR130 = 19,
	RUFFALO_A_TURNR80 = 20
};

BEGIN(Client)

class CNPC_RuffaloRice final : public CNPC
{
public:
	CNPC_RuffaloRice();
	virtual ~CNPC_RuffaloRice() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CNPC_RuffaloRice> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f, 0.f, 0.f));

public:
	virtual void InteractionOn();

protected:
	HRESULT			AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void	OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void	OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void	OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	HRESULT		AddPartObjects(string _strModelKey);
	void		UpdateDistanceState();

	// BaseState 따른 FSM
	void		FSM_BeforeMeet(_float _fTimeDelta);
	void		FSM_Meet(_float _fTimeDelta);
	void		FSM_Interact(_float _fTimeDelta);

	// 거리
	_float		m_fMeetRange = 3.f;

private:
	_int		m_iBaseState = RUFFALO_B_BEFOREMEET;		// Base상태
	_int		m_iDistanceState = RUFFALO_D_FARRANGE;	// 거리에 따른 상태
	_int		m_iActionState = RUFFALO_A_IDLE;			// 행동상태

	/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 201.f / 255.f, 201.f / 255.f, 201.f / 255.f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };

	_float3 m_vFurMainColor = { 184.f / 255.f, 188.f / 255.f, 195.f / 255.f };
	_float3 m_vFurSubColor = { 0.4f, 0.4f, 0.4f };
};

END