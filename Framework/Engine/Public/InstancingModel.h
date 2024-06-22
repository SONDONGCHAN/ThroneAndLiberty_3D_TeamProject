#pragma once

#include "Component.h"


/*NonAnim Model 고정 
필요에 따라서 Anim Model을 추가할 수 있습니다
*/

BEGIN(Engine)

class ENGINE_DLL CInstancingModel : public CComponent
{
public:
	CInstancingModel(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual ~CInstancingModel() = default;

public:
	HRESULT Initialize(shared_ptr<class CModel> _pOriginModel, vector<_float4x4>* _ObjMatrices);
	HRESULT Render(_uint iMeshIndex);

public:
	HRESULT BindMaterialShaderResource(_uint _iMeshIndex, ETEXTURE_TYPE _eMaterialType, const _char* _pConstantName);

public:
	_uint GetNumMeshes() const { return m_iNumMeshes; }
	vector<shared_ptr<class CVIInstanceMesh>> GetMeshes() { return m_Meshes; }
	vector<shared_ptr<class CMaterial>>* GetMaterial() { return &m_Materials; }
	UINT GetInstanceNum() { return m_iInstanceNum; }
	HRESULT DiscardAndFill(_float4x4* _WorldMatrixArray, UINT _iArrayNum);

private:
	_float4x4 m_PivotMatrix;

private:
	_uint m_iNumMeshes = { 0 };
	_uint m_iNumMaterials = { 0 };

	vector<shared_ptr<class CMaterial>> m_Materials;
	vector<shared_ptr<class CVIInstanceMesh>> m_Meshes;

	_uint m_iInstanceNum = { 0 };

public:
	static shared_ptr<CInstancingModel> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, shared_ptr<class CModel> _pOriginModel, vector<_float4x4>* _ObjMatrices);
};


END
