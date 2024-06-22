#pragma once

#include "Tool_CC_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CInstancingModel;
class CTexture;
END

class CEnvironmentObject : public CGameObject
{
public:
	CEnvironmentObject();
	~CEnvironmentObject();

public:
	string GetModelKey() { return m_strModelKey; }

public:
	virtual HRESULT Initialize(const string& _strModelKey, vector<_float4x4>& _vecMatWorld, vector<_float3>& _vCenterPoint, vector<_float>& _fRadius);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderEdge() override;

public:
	virtual void AddInstancingWorldMatrix(const _float4x4& _WorldMatrix) override;

private:
	string m_strModelKey{};
	shared_ptr<CModel> m_pModelCom{};
	shared_ptr<CInstancingModel> m_pInstancingModelCom{};
	vector<_float4x4> m_CulledInstanceWorldMatrix;
	vector<_float3> m_vecCenterPoint{};
	vector<_float> m_vecRadius{};

public:
	static shared_ptr<CEnvironmentObject> Create(const string& _strModelKey, vector<_float4x4>& _vecMatWorld, vector<_float3>& _vCenterPoint, vector<_float>& _fRadius);
};
