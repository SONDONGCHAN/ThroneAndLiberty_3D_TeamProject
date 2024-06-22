#pragma once

#include "NPC.h"

// Base 상태
enum NPC_Lottie_BaseState
{
	LOTTIE_B_BEFOREMEET = 0,			// 상호작용도 안하고 멀때
	LOTTIE_B_MEET,						// 상호작용은 안했으나 상호작용이 가능할정도로 가까울때
	LOTTIE_B_INTERACT,					// 상호작용할때
};

// 거리에 따른 상태
enum NPC_Lottie_DistanceState
{
	LOTTIE_D_FARRANGE = 0,				// 멀때
	LOTTIE_D_MEETRANGE					// 가까울때
};

// 특정 액션 상태
enum NPC_Lottie_ActionState
{
	LOTTIE_A_RUN = 0,
	LOTTIE_A_TALKEND = 2,
	LOTTIE_A_TALK1 = 3,
	LOTTIE_A_IDLE = 9,
	LOTTIE_A_LOOKSKY2 = 18,
	LOTTIE_A_LOOKSKY1 = 20,
	LOTTIE_A_IDLE2 = 26,
	LOTTIE_A_TURNL130 = 28,
	LOTTIE_A_TURNL80 = 29,
	LOTTIE_A_TURNR130 = 30,
	LOTTIE_A_TURNR80 = 31,
	LOTTIE_A_WALK = 33,
};

BEGIN(Client)

class CNPC_Lottie final : public CNPC
{
public:
	CNPC_Lottie();
	virtual ~CNPC_Lottie() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CNPC_Lottie> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f, 0.f, 0.f), _float3 _vCheckPoint1 = _float3(0.f, 0.f, 0.f),
		_float3 _vCheckPoint2 = _float3(0.f, 0.f, 0.f), _float3 _vCheckPoint3 = _float3(0.f, 0.f, 0.f));

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
	_int		m_iBaseState = LOTTIE_B_BEFOREMEET;		// Base상태
	_int		m_iDistanceState = LOTTIE_D_FARRANGE;	// 거리에 따른 상태
	_int		m_iActionState = LOTTIE_A_IDLE;			// 행동상태

	/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 194.f / 255.f, 64.f / 255.f, 33.f / 255.f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };

	_float3 m_vEyeBrowMainColor = { 0.02f, 0.02f, 0.07f };
	_float3 m_vEyeBrowSubColor = { 0.4f, 0.4f, 0.4f };

	_float3	m_vCheckPoint1 = _float3(0.f, 0.f, 0.f);
	_float3	m_vCheckPoint2 = _float3(0.f, 0.f, 0.f);
	_float3	m_vCheckPoint3 = _float3(0.f, 0.f, 0.f);

};

END