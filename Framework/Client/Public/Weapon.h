#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CTexture;
END

BEGIN(Client)

class CWeapon final : public CPartObject
{
public:
	struct WEAPON_DESC
	{
		CPartObject::PARTOBJ_DESC tPartObject;
		_float4x4* pSocketMatrix{ nullptr };
		string strModelKey;
		_float4x4 OffsetMatrixNormal;
		_float4x4 OffsetMatrixSituation;
	};
	
public:
	CWeapon();
	virtual ~CWeapon() override;

public:
	HRESULT Initialize(WEAPON_DESC* _pWeaponDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);
	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT RenderShadow() override;
	virtual HRESULT RenderGlow() override;
	virtual HRESULT RenderEdgeDepth() override;
	virtual HRESULT RenderEdge() override;

public:
	void	OnRimLight(_float3 _fRimLight = _float3(1.f, 0.f, 0.f), _float _fPow = 1.5f);
	void	OffRimLight();
	void	SwitchOffsetToNormal() { m_CurrentOffset = m_OffsetMatrixNormal;}
	void	SwitchOffsetToSituation() { m_CurrentOffset = m_OffsetMatrixSituation;}
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때
	void	UpdateTickWeapon(const char* _szModelKey);
	shared_ptr<CRigidBody> GetRigidBody() { return m_pRigidBody; }
	_bool	GetParried() { return m_bParried; }
	void	SetParried(_bool _bParried) { m_bParried = _bParried; }
	_bool	GetCollisionActive() { return m_bCollisionActive; }
	void	SetCollsionActive(_bool _bCollisionActive) { m_bCollisionActive = _bCollisionActive; }
	
protected:
	HRESULT		AddRigidBody(shared_ptr<Geometry> _pGeometry, ECOLLISIONFLAG _iThisCol, UINT _iTargetCol);		// RigidBody 추가
	
protected:
	shared_ptr<CModel> m_pModelCom = nullptr;
	shared_ptr<CRigidBody>	m_pRigidBody = { nullptr };				// Rigid Body

private:
	// 계산용 Time
	_float		m_fCalculateTime1 = 0.f;

	_bool		m_bParried = false;
	_bool		m_bCollisionActive = true;
	_bool		m_bPrevCollisionActive = true;

	_bool		m_bUseGlow = false;
	_bool		m_bUseGlowColor = false;
	_float4		m_vGlowColor = { 1.f, 1.f, 1.f, 1.f };

	_bool		m_bUseGlowTime = false;
	_float		m_fGlowTime = 0.f;

	_bool		m_bActiveRimLight = false;
	_float3		m_vRimLight = _float3(1.f, 0.f, 0.f);
	_float		m_fRimPow = 0.7f;

private:
	_float4x4*	m_pSocketMatrix = nullptr;
	string		m_strModelKey;
	_float4x4	m_OffsetMatrixNormal	= { XMMatrixIdentity() };
	_float4x4	m_OffsetMatrixSituation = { XMMatrixIdentity() };
	_float4x4	m_CurrentOffset			= { XMMatrixIdentity() };

	const char* m_strShapeTag = "";

public:
	_float4x4*	GetWeaponWorld() { return &m_WorldMatrix; }
	void		SetUseTrail(_bool _bUseTrail, _float4 _vMeshTrailColor, _bool _bUseMeshTrailTime, _float _fMeshTrailTime);
	void		SetUseGlow(_bool _bUseGlow, _bool _bUseGlowTime = false, _float _fGlowTime = 0.f) { m_bUseGlow = _bUseGlow; m_bUseGlowTime = _bUseGlowTime, m_fGlowTime = _fGlowTime; }
	void		SetUseGlowColor(_bool _bUseGlowColor, _float4 _vGlowColor);
	void		AddTrail(_uint _iTrailCnt, _uint _iTickPerTrailGen);

private:
	shared_ptr<class CMeshTrail>	m_pMeshTrail;
	_bool		m_bUseTrail = false;
	_bool		m_bUseTrailTimer = false;
	_float		m_fTrailTimer = 0.f;
	_uint		m_iGeneratedTrail = 0;
	_uint		m_iTickPerTrailGen = 1;

public:
	static shared_ptr<CWeapon> Create(WEAPON_DESC* _pWeaponDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);
};

END

