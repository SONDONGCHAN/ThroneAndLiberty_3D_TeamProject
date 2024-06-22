#pragma once

#include "InteractionObj.h"
#include "Statue.h"

BEGIN(Engine)
class CModel;
class CTexture;
END

BEGIN(Client)

class CDungeonGate : public CInteractionObj
{
public:
	CDungeonGate();
	virtual ~CDungeonGate() = default;

public:
	virtual HRESULT Initialize(_float3 _vPosition);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render();

public:
	void		SimulationOn();
	void		SimulationOff();

public:
	virtual void InteractionOn();
	void	SetMatchStatue1(weak_ptr<CStatue> _pMatchStatue) { m_pMatchDungeonGate1 = _pMatchStatue; }
	void	SetMatchStatue2(weak_ptr<CStatue> _pMatchStatue) { m_pMatchDungeonGate2 = _pMatchStatue; }
	void	SetMatchStatue3(weak_ptr<CStatue> _pMatchStatue) { m_pMatchDungeonGate3 = _pMatchStatue; }
	void	SetMatchStatue4(weak_ptr<CStatue> _pMatchStatue) { m_pMatchDungeonGate4 = _pMatchStatue; }
	
protected:
	HRESULT			AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void	OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void	OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void	OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

protected:
	_bool			MoveSlide(_float _fTimeDelta);
	void			MovePos(_float3 _vPos);

public:
	static shared_ptr<CDungeonGate> Create(_float3 _vPosition);

protected:
	shared_ptr<CRigidBody>	m_pRigidBody = { nullptr };				// Rigid Body
	string m_strModelName = "";										// Model 이름

private:
	_float	m_fCalculateTime1 = 0.f;					// 계산용 타이머

	_bool	m_bAllStatueCorrect = false;				// 다른 조각상들이 맞게되었는지
	_bool	m_bOpen = false;

	_float3	m_vCurrentPos = _float3(0.f, 0.f, 0.f);

	weak_ptr<CStatue>		m_pMatchDungeonGate1;
	weak_ptr<CStatue>		m_pMatchDungeonGate2;
	weak_ptr<CStatue>		m_pMatchDungeonGate3;
	weak_ptr<CStatue>		m_pMatchDungeonGate4;
};

END