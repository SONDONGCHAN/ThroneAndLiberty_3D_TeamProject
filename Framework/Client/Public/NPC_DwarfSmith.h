#pragma once

#include "NPC.h"

// GenType
enum NPC_DwarfSmith_GenType
{
	DWARFSMITH_G_TALK1 = 0,
	DWARFSMITH_G_TALK2,
	DWARFSMITH_G_TALK3,
	DWARFSMITH_G_IDLE,
	DWARFSMITH_G_IDLE2,
	DWARFSMITH_G_SIT,
};

// 특정 액션 상태
enum NPC_DwarfSmith_ActionState
{
	DWARFSMITH_A_IDLE = 0,
	DWARFSMITH_A_SIT = 2,
	DWARFSMITH_A_IDLE2 = 4,
	DWARFSMITH_A_TALK1 = 5,
	DWARFSMITH_A_TALK2 = 6,
	DWARFSMITH_A_TALK3 = 7,
};

BEGIN(Client)

class CNPC_DwarfSmith final : public CNPC
{
public:
	CNPC_DwarfSmith();
	virtual ~CNPC_DwarfSmith() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CNPC_DwarfSmith> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f, 0.f, 0.f),
		_int _iGenType = 0);

public:
	virtual void InteractionOn();

protected:
	HRESULT			AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void	OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void	OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void	OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

	// BaseState 따른 FSM
	void		FSM_BeforeMeet(_float _fTimeDelta);
	void		FSM_Meet(_float _fTimeDelta);
	void		FSM_Interact(_float _fTimeDelta);
	
	// 거리
	_float		m_fMeetRange = 3.f;

private:
	_int		m_iActionState = DWARFSMITH_A_TALK1;			// 행동상태

	/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 0.02f, 0.02f, 0.07f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };
};

END