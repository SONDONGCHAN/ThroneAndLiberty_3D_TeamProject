#pragma once

#include "NPC.h"

// GenType
enum NPC_Knight_GenType
{
	KNIGHT_G_WALK,				// 순찰
	KNIGHT_G_PRACTICE1,			// 훈련1
	KNIGHT_G_PRACTICE2,			// 훈련2
	KNIGHT_G_TALK,				// 대화1
	KNIGHT_G_LAUGH,				// 웃기
	KNIGHT_G_IDLE				// 서있기			
};

// 특정 액션 상태
enum NPC_Knight_ActionState
{
	KNIGHT_A_ATT1 = 0,
	KNIGHT_A_ATT2 = 1,
	KNIGHT_A_ALT = 2,
	KNIGHT_A_TALK1 = 3,
	KNIGHT_A_TALK2 = 4,
	KNIGHT_A_TALK3 = 5,
	KNIGHT_A_TRAIN3 = 6,
	KNIGHT_A_TRAIN2 = 7,
	KNIGHT_A_TRAIN1 = 8,
	KNIGHT_A_ALT1 = 9,
	KNIGHT_A_IDLE1 = 10,
	KNIGHT_A_IDLE2 = 11,
	KNIGHT_A_LAUGH3 = 12,
	KNIGHT_A_LAUGH2 = 13,
	KNIGHT_A_LAUGH1 = 14,
	KNIGHT_A_WALK = 15
};

BEGIN(Client)

class CNPC_Knight final : public CNPC
{
public:
	CNPC_Knight();
	virtual ~CNPC_Knight() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CNPC_Knight> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f, 0.f, 0.f),
		_int _iGenType = 0);

public:
	virtual void InteractionOn();
	void	SetPatrolPos(_float3 _vPatrolPos) { m_vPatrolPos = _vPatrolPos; }

protected:
	HRESULT			AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void	OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void	OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void	OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	HRESULT		AddPartObjects(string _strModelKey);

	// GenType에 따른 FSM
	void		FSM_Walk(_float _fTimeDelta);
	void		FSM_Practice1(_float _fTimeDelta);
	void		FSM_Practice2(_float _fTimeDelta);
	void		FSM_Talk(_float _fTimeDelta);
	void		FSM_Laugh(_float _fTimeDelta);
	void		FSM_Idle(_float _fTimeDelta);

	// 거리
	_float		m_fMeetRange = 3.f;

private:
	_int		m_iActionState = KNIGHT_A_IDLE1;			// 행동상태

	_float3		m_vPatrolPos = _float3(0.f, 0.f, 0.f);			// 순찰지점
	_bool		m_bPatrolDirection = true;						// 순찰 방향

	_bool		m_bTrainStart = false;
	_float		m_fTrainIntervel = 1.f;
	/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 0.02f, 0.02f, 0.07f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };
};

END