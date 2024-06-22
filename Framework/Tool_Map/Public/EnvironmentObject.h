#pragma once

#include "GameObject.h"
#include "Tool_Defines.h"


BEGIN(Engine)
class CModel;
class CInstancingModel;
class CTexture;

class CCollider;
END

BEGIN(Tool_Map)
class CGroupObject;

class CEnvironmentObject : public CGameObject
{
private:
	CEnvironmentObject();
	DESTRUCTOR(CEnvironmentObject)

public:
	string GetModelKey() { return m_strModelKey; }
	_uint GetObjID() { return m_iObjID; }
	shared_ptr<CModel> GetModel() { return m_pModelCom; }
	shared_ptr<CGroupObject> GetOnerGroupObject() { return m_pOnerGroupObject.lock(); }
	_bool GetIsGroup() { return m_bIsGroup; }
	void SetOnerGroupObject(shared_ptr<CGroupObject> _pOnerGroupObject) { m_pOnerGroupObject = _pOnerGroupObject; }
	void SetIsGroup(_bool _IsGroup) { m_bIsGroup = _IsGroup; }

	void SetCollider();
	void SetRadius(_float _fRadius) { m_fRadius = _fRadius; }
	void SetCenterPoint(_float3 _vCenterPoint) { m_vCenterPoint = _vCenterPoint; }

public:
	virtual HRESULT Initialize(string _strModelKey, _float4x4 _matWorld = _float4x4::Identity);
	virtual HRESULT Initialize(string _strModelKey, vector<_float4x4> _vecMatWorld);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderPixelPicking();
	virtual HRESULT RenderEdgeDepth() override;
	virtual HRESULT RenderEdge() override;

public:
	static _bool m_bIsReanderPixelPicking;

private:
	_bool m_bIsGroup{};
	
	_float m_fRadius{};
	_float3 m_vCenterPoint{};

	_float4x4 m_matOnerworld{};

	_uint m_iObjID{};
	string m_strModelKey{};

	weak_ptr<CGroupObject> m_pOnerGroupObject{};
	shared_ptr<CInstancingModel> m_pInstancingModelCom{};
	shared_ptr<CModel> m_pModelCom{};

	static _uint IDCount;

	shared_ptr<CCollider> m_pSphereCollider = { nullptr };

public:
	static shared_ptr<CEnvironmentObject> Create(string _strModelKey, _float4x4 _matWorld = _float4x4::Identity);
	static shared_ptr<CEnvironmentObject> Create(string _strModelKey, vector<_float4x4> _vecMatWorld);
};

END