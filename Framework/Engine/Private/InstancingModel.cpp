#include "InstancingModel.h"
#include "VIInstanceMesh.h"
#include "Material.h"
#include "Model.h"
#include "Mesh.h"

#include "GameInstance.h"

CInstancingModel::CInstancingModel(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :CComponent(_pDevice, _pContext)
{
}

HRESULT CInstancingModel::Initialize(shared_ptr<class CModel> _pOriginModel, vector<_float4x4>* _ObjMatrices)
{
    /*메테리얼 복사*/
    m_Materials = *(_pOriginModel->GetMaterial());
	const vector<shared_ptr<CMesh>>& OriginMeshes = _pOriginModel->GetMeshes();

	UINT iNum = (UINT)OriginMeshes.size();
	m_Meshes.reserve(iNum);
	for (UINT i = 0; i < iNum; ++i) {

		shared_ptr<CVIInstanceMesh> pMesh = CVIInstanceMesh::Create(m_pDevice, m_pContext, OriginMeshes[i], _ObjMatrices);
		m_Meshes.push_back(pMesh);
	}

    m_iNumMeshes = iNum;
	m_iInstanceNum = (UINT)_ObjMatrices->size();

	return S_OK;
}

HRESULT CInstancingModel::Render(_uint _iMeshIndex)
{
    m_Meshes[_iMeshIndex]->BindBuffers();
    m_Meshes[_iMeshIndex]->Render();

    return S_OK;
}

HRESULT CInstancingModel::BindMaterialShaderResource(_uint _iMeshIndex, ETEXTURE_TYPE _eMaterialType, const _char* _pConstantName)
{
    _uint iMaterialIndex = m_Meshes[_iMeshIndex]->GetMaterialIndex();
    string strTexKey = m_Materials[iMaterialIndex]->GetTextures()[_eMaterialType];

    return GAMEINSTANCE->BindSRV(_pConstantName, strTexKey);
}

HRESULT CInstancingModel::DiscardAndFill(_float4x4* _WorldMatrixArray, UINT _iArrayNum)
{
	for (auto& pMesh : m_Meshes)
	{
		if (FAILED(pMesh->DiscardAndFill(_WorldMatrixArray, _iArrayNum)))
			return E_FAIL;
	}

	return S_OK;
}

shared_ptr<CInstancingModel> CInstancingModel::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, shared_ptr<class CModel> _pOriginModel, vector<_float4x4>* _ObjMatrices)
{
    shared_ptr<CInstancingModel> pInstance = make_shared<CInstancingModel>(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pOriginModel, _ObjMatrices)))
        MSG_BOX("Failed to Create : CInstancingModel");

    return pInstance;
}
