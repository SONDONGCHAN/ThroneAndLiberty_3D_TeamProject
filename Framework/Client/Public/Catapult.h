#pragma once
#include "InteractionObj.h"
#include "CatapultRock.h"
#include "TargetDirection.h"


BEGIN(Engine)
class CModel;
class CRigidBody;
END

BEGIN(Client)
class CPlayer;
class CWallStone;


class CCatapult : public CInteractionObj
{
public:
	enum ECatapult_State
	{
		CATAPULT_OFF, CATAPULT_OFF_TO_ON, CATAPULT_ON, CATAPULT_ON_TO_OFF, CATAPULT_END
	};

public:
	CCatapult();
	virtual ~CCatapult() = default;

public:
	virtual HRESULT Initialize(_float3 _vPosition);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render();

public:
	virtual HRESULT RenderShadow() override;

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;

public:
	void	ShootOn();
	void	CatapultOn();
	void	CatapultOff();
	void	OnCatapultCamera();
	void	BindWallStones(vector<shared_ptr<CWallStone>> _vecWallStones);

public:
	_float3	GetCatapultCameraEye();
	_float3	GetCatapultCameraAt();

private:
	_bool	CheckClear();
	void	CheckDisableWallStone();

private:
	void	AddRigidBody(string _strModelKey);
	void	AddCatapultRock();
	void	AddTargetDir();


private:
	void	KeyInput(_float _fTimeDelta);

public:
	virtual void InteractionOn() override;

public:
	static shared_ptr<CCatapult> Create(_float3 _vPosition);
	
private:
	shared_ptr<CRigidBody>	m_pRigidBody = { nullptr };
	ECatapult_State			m_eCatapultState = { CATAPULT_OFF };
	_float					m_fShootCoolTime = { 0.1f };
	_float					m_fCurrentShootCoolTime = { 0.1f };
	_bool					m_isShooted		= { false };
	_float					m_fAfterShoot	= { 0.f };
	_bool					m_isSeparate = { false };
	_bool					m_IsCatapultOn = { false };
	_bool					m_isDissolving = { false };
	_float					m_fDissolveTime= { 0.f };

	vector<shared_ptr<CCatapultRock>>	m_pCatapultRocks = {};
	weak_ptr<CCatapultRock>				m_pCurrentCatapultRock;
	shared_ptr<CTargetDirection>		m_pTargetDirection;

	_float	m_fCurrentAngle = 0.f;

	_float	m_fCurrentShootPower	= 20.f;
	_float	m_fMaxShootPower		= 40.f;
	_float	m_fMinShootPower		= 20.f;

	_float3 m_vStaticCameraEye = {0.f, 0.f, 0.f };
	_float3 m_vStaticCameraAt = {0.f, 0.f, 0.f};

	vector<shared_ptr<CWallStone>> m_vecWallStones;
};

END