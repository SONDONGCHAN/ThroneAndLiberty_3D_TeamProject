#pragma once

#include "NPC.h"

// Base 상태
enum NPC_Davinci_BaseState
{
	DAVINCI_B_BEFOREMEET = 0,			// 상호작용도 안하고 멀때
	DAVINCI_B_MEET,						// 상호작용은 안했으나 상호작용이 가능할정도로 가까울때
	DAVINCI_B_INTERACT,					// 상호작용할때
};

// 거리에 따른 상태
enum NPC_Davinci_DistanceState
{
	DAVINCI_D_FARRANGE = 0,				// 멀때
	DAVINCI_D_MEETRANGE					// 가까울때
};

// 특정 액션 상태
enum NPC_Davinci_ActionState
{
	DAVINCI_A_BASE1 = 1,
	DAVINCI_A_BASE2 = 2,
	DAVINCI_A_IDLE = 7,
	DAVINCI_A_TURNL130 = 35,
	DAVINCI_A_TURNL80 = 36,
	DAVINCI_A_TURNR130 = 37,
	DAVINCI_A_TURNR80 = 38,
	DAVINCI_A_RUN = 20,
	DAVINCI_A_WALK = 41,
};

BEGIN(Client)

class CNPC_Davinci final : public CNPC
{
public:
	CNPC_Davinci();
	virtual ~CNPC_Davinci() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CNPC_Davinci> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f, 0.f, 0.f));

public:
	virtual void InteractionOn();

protected:
	HRESULT			AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void	OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void	OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void	OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	//HRESULT		AddPartObjects(string _strModelKey);
	void		UpdateDistanceState();

	// BaseState 따른 FSM
	void		FSM_BeforeMeet(_float _fTimeDelta);
	void		FSM_Meet(_float _fTimeDelta);
	void		FSM_Interact(_float _fTimeDelta);

	// 거리
	_float		m_fMeetRange = 3.f;

private:
	_int		m_iBaseState = DAVINCI_B_BEFOREMEET;		// Base상태
	_int		m_iDistanceState = DAVINCI_D_FARRANGE;	// 거리에 따른 상태
	_int		m_iActionState = DAVINCI_A_IDLE;			// 행동상태

	/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 0.02f, 0.02f, 0.07f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };
};

END