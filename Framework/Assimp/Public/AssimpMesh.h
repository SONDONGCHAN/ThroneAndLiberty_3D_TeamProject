#pragma once

#include "Assimp_Defines.h"
#include "AssimpModel.h"
#include "VIBuffer.h"

BEGIN(Assimp)

class CAssimpMesh : public CVIBuffer
{
public:
	CAssimpMesh(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual ~CAssimpMesh();


public:
	_uint GetMaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize(_uint _iAnimFlag, const aiMesh* pAIMesh, shared_ptr<CAssimpModel> _pModel, _float4x4 PivotMatrix);

	_float4x4 GetRootOffset(_uint _iNum) { return m_OffsetMatrices[_iNum]; };

public:
	void SetUpBoneMatrices(_float4x4* _pBoneMatirces, vector<shared_ptr<class CBone>>& _Bones);

public:
	HRESULT ReadyVertexBufferNonAnim(const aiMesh* _pAIMesh, _float4x4 PivotMatrix);
	HRESULT ReadyVertexBufferAnim(const aiMesh* _pAIMesh, shared_ptr<CAssimpModel> _pModel);

public:
	void ParsingMeshData(HANDLE _handle);
	void ParsingAnimMeshDatat(ofstream& out);

private:
	_char m_szName[MAX_PATH] = "";
	_uint m_iMaterialIndex = { 0 };

	_uint m_iNumBones = { 0 };
	vector<_uint> m_BoneIndices;
	vector<_float4x4> m_OffsetMatrices;

	shared_ptr<CAssimpModel> m_pModel = nullptr;

	//나중에 지우기이
private:
	const aiMesh* m_pAIMesh;
	VTXANIMMESH* m_pVertices;
public:
	static shared_ptr<CAssimpMesh> Create(_uint _iAnimFlag, wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, const aiMesh* pAIMesh, shared_ptr<CAssimpModel> _pModel, _float4x4 PivotMatrix);


};

END