#pragma once

#include "NPC.h"

// Base 상태
enum NPC_Ashien_BaseState
{
	ASHIEN_B_BEFOREMEET = 0,			// 상호작용도 안하고 멀때
	ASHIEN_B_MEET,						// 상호작용은 안했으나 상호작용이 가능할정도로 가까울때
	ASHIEN_B_INTERACT,					// 상호작용할때
};

// 거리에 따른 상태
enum NPC_Ashien_DistanceState
{
	ASHIEN_D_FARRANGE = 0,				// 멀때
	ASHIEN_D_MEETRANGE					// 가까울때
};

// 특정 액션 상태
enum NPC_Ashien_ActionState
{
	ASHIEN_A_TALK1 = 0,
	ASHIEN_A_TALK2 = 1,
	ASHIEN_A_TALK3 = 2,
	ASHIEN_A_TALK4 = 3,			// 돌아가서
	ASHIEN_A_TALK5 = 4,
	ASHIEN_A_IDLE = 13,
	ASHIEN_A_TURNL130 = 14,
	ASHIEN_A_TURNL80 = 15,
	ASHIEN_A_TURNR130 = 16,
	ASHIEN_A_TURNR80 = 17
};

BEGIN(Client)

class CNPC_Ashien final : public CNPC
{
public:
	CNPC_Ashien();
	virtual ~CNPC_Ashien() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CNPC_Ashien> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f,0.f,0.f));

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
	_int		m_iBaseState = ASHIEN_B_BEFOREMEET;		// Base상태
	_int		m_iDistanceState = ASHIEN_D_FARRANGE;	// 거리에 따른 상태
	_int		m_iActionState = ASHIEN_A_IDLE;			// 행동상태

	/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 194.f / 255.f, 165.f / 255.f, 90.f / 255.f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };
};

END