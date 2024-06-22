#pragma once

#include "NPC.h"

// GenType
enum NPC_DwarfMusician_GenType
{
	DWARFMUSICIAN_G_SOCLOOPING1 = 0,
	DWARFMUSICIAN_G_SOCLOOPING2,
	DWARFMUSICIAN_G_SOCLOOPING3,
	DWARFMUSICIAN_G_IDLE,
	DWARFMUSICIAN_G_TALK1,
	DWARFMUSICIAN_G_TALK2,
};

// 특정 액션 상태
enum NPC_DwarfMusician_ActionState
{
	DWARFMUSICIAN_A_SOCLOOPING1 = 0,
	DWARFMUSICIAN_A_SOCLOOPING2 = 1,
	DWARFMUSICIAN_A_SOCLOOPING3 = 2,
	DWARFMUSICIAN_A_IDLE = 3,
	DWARFMUSICIAN_A_TALK1 = 4,
	DWARFMUSICIAN_A_TALK2 = 5,
};

BEGIN(Client)

class CNPC_DwarfMusician final : public CNPC
{
public:
	CNPC_DwarfMusician();
	virtual ~CNPC_DwarfMusician() = default;

public:
	HRESULT Initialize(_float3 _vCreatePos);
	virtual void	PriorityTick(_float _fTimeDelta) override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render()  override;

public:
	static shared_ptr<CNPC_DwarfMusician> Create(_float3 _vCreatePos, _float3 _vInitLookPos = _float3(0.f, 0.f, 0.f),
		_int _iGenType = 3);

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
	_int		m_iActionState = DWARFMUSICIAN_A_IDLE;			// 행동상태

	/*Hair Color*/
private:
	_float3 m_vHairMainColor = { 0.02f, 0.02f, 0.07f };
	_float3 m_vHairSubColor = { 0.4f, 0.4f, 0.4f };
};

END