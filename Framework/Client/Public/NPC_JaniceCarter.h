#pragma once

#include "NPC.h"

// Base 상태
enum NPC_JaniceCarter_BaseState
{
	JANICECARTER_B_BEFOREMEET = 0,			// 상호작용도 안하고 멀때
	JANICECARTER_B_MEET,						// 상호작용은 안했으나 상호작용이 가능할정도로 가까울때
	JANICECARTER_B_INTERACT,					// 상호작용할때
};

// 거리에 따른 상태
enum NPC_JaniceCarter_DistanceState
{
	JANICECARTER_D_FARRANGE = 0,				// 멀때
	JANICECARTER_D_MEETRANGE					// 가까울때
};

// 특정 액션 상태
enum NPC_JaniceCarter_ActionState
{
	JANICECARTER_A_IDLE = 2,
	JANICECARTER_A_RUN = 9,
	JANICECARTER_A_TALK1 = 33,
	JANICECARTER_A_TALK2 = 34,
	JANICECARTER_A_TALK3 = 35,
	JANICECARTER_A_TALK4 = 36,
	JANICECARTER_A_TALK5 = 37,				// Aesine돌아오고 나서
	JANICECARTER_A_TURNL130 = 39,
	JANICECARTER_A_TURNL80 = 40,
	JANICECARTER_A_TURNR130 = 41,
	JANICECARTER_A_TURNR80 = 42,
	JANICECARTER_A_WALK = 43,
};

BEGIN(Client)

class CNPC_JaniceCarter final : public CNPC
{
public:
	CNPC_JaniceCarter();
	virtual ~CNPC_JaniceCarter() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CNPC_JaniceCarter> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f, 0.f, 0.f));

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
	_int		m_iBaseState = JANICECARTER_B_BEFOREMEET;		// Base상태
	_int		m_iDistanceState = JANICECARTER_D_FARRANGE;		// 거리에 따른 상태
	_int		m_iActionState = JANICECARTER_A_IDLE;			// 행동상태

	/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 0.02f, 0.02f, 0.07f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };

	_float3 m_vEyeBrowMainColor = { 0.02f, 0.02f, 0.07f };
	_float3 m_vEyeBrowSubColor = { 0.4f, 0.4f, 0.4f };


};

END