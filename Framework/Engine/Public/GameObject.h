#pragma once

#include "Engine_Defines.h"
#include "Transform.h"
#include "GameInstance.h"

BEGIN(Engine)

struct COLLISIONDATA
{
	~COLLISIONDATA() { pGameObject.reset(); Safe_Delete(pArg); }
	weak_ptr<CGameObject> pGameObject;
	ECOLLISIONFLAG eColFlag = ECOLLISIONFLAG::COL_NONE;
	string szShapeTag = "";
	void* pArg = nullptr;
};

class ENGINE_DLL CGameObject abstract : public std::enable_shared_from_this<CGameObject>
{
public:
	CGameObject();
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize(CTransform::TRANSFORM_DESC* _pTransformDesc);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderGlow() { return S_OK; }
	// 임시 이름
	virtual HRESULT RenderEdgeDepth() { return S_OK; }
	virtual HRESULT RenderEdge() { return S_OK; }
	virtual HRESULT RenderDistortion() { return S_OK; }
	// Pixel Picking
	virtual HRESULT RenderPixelPicking() { return S_OK; }

public:
	virtual HRESULT RenderShadow() { return S_OK; }
	HRESULT RenderDepth();

public:
	void AddComponent(const wstring& _strComponentTag, shared_ptr<CComponent> _pComponent);
	shared_ptr<CComponent> GetComponent(const wstring& _strComponentTag);
	shared_ptr<CTransform> GetTransform();

public:
	bool IsActive() { return m_IsActive; }
	bool IsEnabled() { return m_IsEnabled; }
	void SetActive(_bool _bActive) { m_IsActive = _bActive; };
	void SetEnable(_bool _bEnable) { m_IsEnabled = _bEnable; };
	bool IsEdgeRender() { return m_IsEdgeRender; }
	void SetEdgeRender(_bool _bEdgeRender) { m_IsEdgeRender = _bEdgeRender; }
	bool IsDissolve() { return m_bDslv; }
	void SetDissolve(_bool _bDissolve, _bool _bDissolveType, _float _fDissolveTime) 
	{
		m_bDslv = _bDissolve; m_bDslvType = _bDissolveType; m_bDslvType ? (m_fDslvTime = _fDissolveTime) : (m_fDslvTime = 0.f); m_fMaxDslvTime = _fDissolveTime;
	}
	void SetDissolveInfo(_float _fDslvThick = 1.f, _float4 _vDslvColor = _float4(0.427f, 0.227f, 0.573f, 1.f), const char* _strDslvTexKey = "Noise_1001")
	{
		m_fDslvThick = _fDslvThick; m_vDslvColor = _vDslvColor; m_strDslvTexKey = _strDslvTexKey;
	}
	_float3 GetLockOnOffSetPosition();

	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag);
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag);
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag);

	virtual void AddInstancingWorldMatrix(const _float4x4& _WorldMatrix) {};
public:
	// 애니메이션에서 호출하는 Collider Trigger 함수 입니다.
	virtual void DisableCollision(const char* _strShapeTag) {};
	virtual void EnableCollision(const char* _strShapeTag) {};
	// 애니메이션에서 호출하는 Effect Trigger 함수 입니다.
	virtual void PlayEffect(string _strGroupID) {};
	// 애니메이션에서 호출하는 Effect 종료 함수 입니다.
	virtual void StopEffect(string _strGroupID) {};
	// 애니메이션에서 호출하는 Projectile Trigger 함수 입니다.
	virtual void ShootProjectile(_int _eProjectileOwner) {};

public:
	HRESULT UsingRimLight(_float3 _vRimColor, _float _fRimPow = 1.5f);

public:
	/*AlphaBlend로 Render하는 객체의 소팅을 위해 카메라로부터의 거리를 반환하는 함수*/
	_float GetDistanceFromCam();

protected:
	/*false일시 객체를 제거*/
	_bool m_IsActive = true;
	/*false일시 객체 비활성화 -> 재활용을 위해 제거하진 않고 Tick + Render를 돌리지 않는 상태*/
	_bool m_IsEnabled = true;

	_float3 m_vLockOnOffSetPosition;

/*Components*/
protected:
	map<const wstring, shared_ptr<class CComponent>> m_Components;
	shared_ptr<CTransform> m_pTransformCom = nullptr;

/*Shadow*/
protected:
	_bool m_IsDrawShadow = false;
	_bool m_IsCascadeObj = false;
	_uint m_iCurrentCascadeNum = 0;

	void ComputeCascadeNum();

/* Dissolve */
protected:
	_bool			m_bDslv = false;
	_bool			m_bDslvType = false; // 0 : Dissolve In, 1 : Dissolve Out
	_float			m_fDslvTime = 0.f;
	_float			m_fMaxDslvTime = 1.f;
	_float			m_fDslvThick = 1.f;
	_float4			m_vDslvColor = { 1.f, 1.f, 1.f, 1.f };
	const char*		m_strDslvTexKey = "Noise_1001";

	virtual HRESULT	UpdateDslv(_float _fTimeDelta);

protected:
	_int			m_iEffectIdx = -1;	// 이펙트 종료를 위해 들고 있는 이펙트 인덱스

protected:
	/*기본 오브젝트 Render에 사용할 Pass Index*/
	_uint m_iShaderPass = 0;
	/*기본 오브젝트 Render에 사용할 셰이더 타입 */
	ESHADER_TYPE m_eShaderType = ESHADER_TYPE::ST_END;

protected:
	_bool m_IsEdgeRender = false;
};

END