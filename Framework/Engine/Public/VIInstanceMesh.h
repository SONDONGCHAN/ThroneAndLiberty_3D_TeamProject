#pragma once

#include "VIInstancing.h"

BEGIN(Engine)

class CVIInstanceMesh : public CVIInstancing
{
public:
	CVIInstanceMesh(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	CVIInstanceMesh(const shared_ptr<CVIInstanceMesh> _rhs);
	virtual ~CVIInstanceMesh() = default;

public:
	virtual HRESULT Initialize(shared_ptr<class CMesh> _pMesh, vector<_float4x4>* _pMatrixVec);
	virtual HRESULT InitializeClone(_uint _iNumInstance, void* _pArg);

public:
	_uint GetMaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	HRESULT DiscardAndFill(_float4x4* _WorldMatrixArray, UINT _iArrayNum);

public:
	vector<_uint> m_vIndicesNum;

private:
	_char m_szName[MAX_PATH] = "";
	_uint m_iMaterialIndex = { 0 };

	_uint m_iNumBones = { 0 };
	vector<_uint> m_BoneIndices;
	vector<_float4x4> m_OffsetMatrices;

	_uint m_iFaceNum = 0;
public:
	static shared_ptr<CVIInstanceMesh> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, shared_ptr<class CMesh> _pMesh, vector<_float4x4>* _pMatrixVec);
	virtual shared_ptr<CComponent> Clone(_uint _iNumInstance, void* _pArg);


};

END