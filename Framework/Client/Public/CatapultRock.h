#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CRigidBody;
class CCamera;
END

BEGIN(Client)

class CCatapult;

class CCatapultRock : public CGameObject
{
public:
	struct CatapultRock_Desc
	{
		shared_ptr<CTransform> pParentTransform = { nullptr };
		_float4x4* pSocketMatrix = { nullptr };
		weak_ptr<CCatapult> pCatapult;
	};

public:
	CCatapultRock();
	virtual ~CCatapultRock() = default;

public:
	virtual HRESULT Initialize(shared_ptr<CatapultRock_Desc> _pCatapultRockDesc, _float3 _vPosition);
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
	void	SetOnCatapultBarrel();
	void	ShootRock(_float3 _vDir, _float _fForce);
	void	OnCatapultRockCamera() { m_IsCamOn = true; }

public:
	_bool	GetIsUsed() { return m_IsOn; }
	void	StartDissolve();

private:
	void	AddRigidBody(string _strModelKey);

public:
	static shared_ptr<CCatapultRock> Create(shared_ptr<CatapultRock_Desc> _pCatapultRockDesc, _float3 _vPosition);

private:
	shared_ptr<CRigidBody>	m_pRigidBody	= { nullptr };
	shared_ptr<CModel>		m_pModel		= { nullptr };
	weak_ptr<CCamera>		m_pCamera;
	weak_ptr<CCatapult>		m_pCatapult;

private:
	_bool					m_IsCamOn		= { false };
	_bool					m_IsOn			= { false };
	_bool					m_IsUsed		= { false };
	CatapultRock_Desc		m_MyDesc;
	_float3					m_vShootDir		= {};
	_float4x4				m_WorldMatrix	= {};
	_float4x4				m_OffsetMatrix	= {};

	_bool					m_IsHit = { false };
	_float					m_fAfterHitTime = { 3.f };

};

END