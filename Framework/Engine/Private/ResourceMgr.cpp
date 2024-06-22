#include "ResourceMgr.h"

#include <filesystem>
#include <iostream>

#include "Texture.h"
#include "Model.h"
#include "VIInstancePoint.h"
#include "VIInstanceParticle.h"
#include "VIPointTrail.h"

#include "ThreadPool.h"

CResourceMgr::CResourceMgr(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	:m_pDevice(_pDevice),
	m_pContext(_pContext)
{
}


void CResourceMgr::LoadLevelResource(_uint _iLevelIdx)
{
	if (0 == _iLevelIdx) {
		StaticResourceLoad();
	}
	else if (1 == _iLevelIdx) {
		LogoResourceLoad();
	}
	// LEVEL_DUNGEON_MAIN
	else if (2 == _iLevelIdx) {
		DungeonMainResourceLoad();
	}
	// LEVEL_DUNGEON_FINAL
	else if (3 == _iLevelIdx) {
		DungeonFinalResourceLoad();
	}
	// LEVEL_VILLAGE
	else if (4 == _iLevelIdx) {
		VillageResourceLoad();
	}
	//LEVEL_FONOS
	else if (5 == _iLevelIdx) {
		FonosResourceLoad();
	}
	else if (10 == _iLevelIdx) {
		MapToolResourceLoad();
	}

	else if (20 == _iLevelIdx)
		EffectToolResourceLoad();

}

void CResourceMgr::ClearResource()
{
	for (auto iter = m_SRVs.begin(); iter != m_SRVs.end(); ) {
		/*Static Option이 false일시, 그 레벨이 지나면 지워준다*/
		if (!(*iter).second.second) {
			iter = m_SRVs.erase(iter);
		}
		else {
			++iter;
		}
	}

	for (auto iter = m_Models.begin(); iter != m_Models.end(); ) {
		/*Static Option이 false일시, 그 레벨이 지나면 지워준다*/
		if (!(*iter).second.second) {
			iter->second.first.reset();
			iter = m_Models.erase(iter);
		}
		else {
			++iter;
		}
	}

}

void CResourceMgr::StaticResourceLoad()
{
	m_IsStaticRes = true;

	wstring strStaticPath = TEXT("..\\..\\Client\\Bin\\Resources\\Level_Static");

	LoadTexture(strStaticPath);
	LoadModels(strStaticPath);
	LoadBuffers();

	m_IsStaticResDone = true;
}

void CResourceMgr::LogoResourceLoad()
{
	m_IsStaticRes = false;

	wstring strStaticPath = TEXT("..\\..\\Client\\Bin\\Resources\\Level_Logo");

	LoadTexture(strStaticPath);
	LoadModels(strStaticPath);
	LoadBuffers();
}

void CResourceMgr::DungeonMainResourceLoad()
{
	m_IsStaticRes = true;

	wstring strStaticPath = TEXT("..\\..\\Client\\Bin\\Resources\\Level_Dungeon_Main");

	LoadTexture(strStaticPath);
	LoadModels(strStaticPath);
}

void CResourceMgr::DungeonFinalResourceLoad()
{
	m_IsStaticRes = false;

	wstring strStaticPath = TEXT("..\\..\\Client\\Bin\\Resources\\Level_Dungeon_Final");

	LoadTexture(strStaticPath);
	LoadModels(strStaticPath);
}

void CResourceMgr::MapToolResourceLoad()
{
	wstring strStaticPath = TEXT("..\\..\\Tool_Map\\Bin\\Resources\\Level_MapTool");

	std::vector< std::future<int> > results;

	LoadTexture(strStaticPath);
	LoadModels(strStaticPath);
	LoadBuffers();
}

void CResourceMgr::EffectToolResourceLoad()
{
	wstring strStaticPath = TEXT("..\\..\\Tool_Effect\\Bin\\Resources\\Level_EffectTool");

	std::vector< std::future<int> > results;

	LoadTexture(strStaticPath);
	LoadModels(strStaticPath);
	LoadBuffers();
}

void CResourceMgr::VillageResourceLoad()
{
	m_IsStaticRes = true;

	if (!m_IsFieldResLoadDone) {

		wstring strStaticPath = TEXT("..\\..\\Client\\Bin\\Resources\\Level_Village");

		LoadTexture(strStaticPath);
		LoadModels(strStaticPath);

		m_IsFieldResLoadDone = true;
	}

}

void CResourceMgr::FonosResourceLoad()
{
	m_IsStaticRes = false;

	wstring strStaticPath = TEXT("..\\..\\Client\\Bin\\Resources\\Level_Fonos");

	LoadTexture(strStaticPath);
	LoadModels(strStaticPath);
}

void CResourceMgr::LoadTexture(const wstring& _strTexPath)
{
	filesystem::path basePath(_strTexPath);

	for (const auto& entry : filesystem::directory_iterator(basePath)) {
		/*if. File : .png 라면 texture Create*/
		if (entry.is_regular_file()) {
			if (".png" == entry.path().extension() ||
				".dds" == entry.path().extension()) {
				string strFileName = entry.path().filename().stem().string();
				CreateSRV(entry.path().wstring(), entry.path().extension(), strFileName);
			}
		}
		/*if. Directory : 계속 다음 폴더를 순회*/
		else if(entry.is_directory()){

			LoadTexture(entry.path());
		}
	}
}


void CResourceMgr::LoadModels(const wstring& _strModelPath)
{
	filesystem::path basePath(_strModelPath);

	for (const auto& entry : filesystem::directory_iterator(basePath)) {
		if (entry.is_regular_file()) {
			if (".dat" == entry.path().extension()) {
				string strFileName = entry.path().filename().stem().string();
				m_Models.emplace(strFileName, pair{ CModel::Create(m_pDevice, m_pContext, entry.path().string().c_str(), entry.path().filename().stem().wstring()), m_IsStaticRes });
			}
		}
		/*if. Directory : 계속 다음 폴더를 순회*/
		else if (entry.is_directory()) {
			LoadModels(entry.path());
		}
	}

}
void CResourceMgr::LoadBuffers()
{
#pragma region VIInstancePoint
	shared_ptr<CVIBuffer> pInstance = CVIInstancePoint::Create(m_pDevice, m_pContext);
	if (pInstance == nullptr)
		return;

	m_Buffers.emplace("Buffer_InstancePoint", pInstance);
#pragma endregion

#pragma region VIInstanceParticle
	pInstance = CVIInstanceParticle::Create(m_pDevice, m_pContext);
	if (pInstance == nullptr)
		return;

	m_Buffers.emplace("Buffer_InstanceParticle", pInstance);
#pragma endregion

#pragma region VIPointTrail
	pInstance = CVIPointTrail::Create(m_pDevice, m_pContext);
	if (pInstance == nullptr)
		return;

	m_Buffers.emplace("Buffer_PointTrail", pInstance);
#pragma endregion
}

void CResourceMgr::LoadHDRCube()
{




}

HRESULT CResourceMgr::CreateSRV(wstring _strTexturePath, wstring _strExt, string _strTexName)
{

	wrl::ComPtr<ID3D11ShaderResourceView> pSRV = { nullptr };
	HRESULT hr;

	if (false == lstrcmp(_strExt.c_str(), TEXT(".dds")))
		hr = CreateDDSTextureFromFile(m_pDevice.Get(), _strTexturePath.c_str(), nullptr, &pSRV);
	else if (false == lstrcmp(_strExt.c_str(), TEXT(".tga")))
		hr = E_FAIL;
	else
		hr = CreateWICTextureFromFile(m_pDevice.Get(), _strTexturePath.c_str(), nullptr, &pSRV);
		//hr = CreateWICTextureFromFileEx(m_pDevice.Get(), m_pContext.Get(), _strTexturePath.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS, WIC_LOADER_DEFAULT, nullptr, &pSRV);

	if (FAILED(hr))
		return E_FAIL;

	m_SRVs.emplace(_strTexName, pair{ pSRV, m_IsStaticRes });

	return S_OK;
}

shared_ptr<CTexture> CResourceMgr::GetTexture(string _strTexKey)
{
	auto iter = m_Textures.find(_strTexKey);

	if (iter == m_Textures.end())
		return nullptr;

	return iter->second.first;
}

shared_ptr<class CModel> CResourceMgr::GetModel(string _strModelKey)
{
	auto iter = m_Models.find(_strModelKey);

	if (iter == m_Models.end())
		return nullptr;

	CModel::TYPE eType = iter->second.first->GetModelType();

	/*애님 모델일때 : 복사 후 반납*/
	if (CModel::TYPE_ANIM == eType) {
		return dynamic_pointer_cast<CModel>(iter->second.first->Clone(nullptr));
	}

	return iter->second.first;
}

shared_ptr<CVIBuffer> CResourceMgr::GetBuffer(string _strBufferKey)
{
	auto iter = m_Buffers.find(_strBufferKey);

	if (iter == m_Buffers.end())
		return nullptr;

	return iter->second;
}

wrl::ComPtr<ID3D11ShaderResourceView> CResourceMgr::GetSRV(string _strTexTag)
{
	auto iter = m_SRVs.find(_strTexTag);
		
	if (iter == m_SRVs.end())
		return nullptr;

	return iter->second.first;
}

void CResourceMgr::Free()
{
	m_Textures.clear();
	m_Models.clear();

}

shared_ptr<CResourceMgr> CResourceMgr::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
{
	return make_shared<CResourceMgr>(_pDevice, _pContext);
}
