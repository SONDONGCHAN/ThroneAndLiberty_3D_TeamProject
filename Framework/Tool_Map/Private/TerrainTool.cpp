#include "TerrainTool.h"

#include "ToolMgr.h"

#include "Terrain.h"
#include "VITerrain.h"

#include "PickingMgr.h"

CTerrainTool::CTerrainTool()
{
}

#ifdef CHECK_REFERENCE_COUNT
CTerrainTool::~CTerrainTool()
{
}
#endif // CHECK_REFERENCE_COUNT


// 쓰레드가 실행시킬 함수를 구현해놔야겠네
void CTerrainTool::UpdateWeightDataDesc(int num)
{
	_uint iWeightDataDescIndex = num;

	for (const auto& DeltaWeight : m_mapDeltaWeight)
	{

		if (m_iSelectedWeightDescIndex == iWeightDataDescIndex)
		{
			_uint iSelectedBlendDiffuseChannelIndex = m_vecWeightDataDesc[iWeightDataDescIndex].iSelectedBlendDiffuseChannelIndex;

			for (_uint iCntBlendDiffuseIndex = 0; iCntBlendDiffuseIndex < BLEND_END; iCntBlendDiffuseIndex++)
			{
				_float& fSelectedWeightChannel = (*((_float*)(&(m_vecWeightDataDesc[iWeightDataDescIndex].vecPixelColor[DeltaWeight.first])) + iCntBlendDiffuseIndex));

				if (iSelectedBlendDiffuseChannelIndex == iCntBlendDiffuseIndex)
				{
					fSelectedWeightChannel += DeltaWeight.second;
					if (255.f < fSelectedWeightChannel)
						fSelectedWeightChannel = 255.f;
				}
				else
				{
					fSelectedWeightChannel -= DeltaWeight.second;
					if (0.f > fSelectedWeightChannel)
						fSelectedWeightChannel = 0.f;
				}
			}
		}

		else
		{
			for (_uint iCntBlendDiffuseIndex = 0; iCntBlendDiffuseIndex < BLEND_END; iCntBlendDiffuseIndex++)
			{
				_float& fSelectedWeightChannel = (*((_float*)(&(m_vecWeightDataDesc[iWeightDataDescIndex].vecPixelColor[DeltaWeight.first])) + iCntBlendDiffuseIndex));

				fSelectedWeightChannel -= DeltaWeight.second;
				if (0.f > fSelectedWeightChannel)
					fSelectedWeightChannel = 0.f;
			}
		}


		m_vecWeightDataDesc[iWeightDataDescIndex].vecPixel[DeltaWeight.first] = D3DCOLOR_RGBA
		(
			static_cast<_uint>(m_vecWeightDataDesc[iWeightDataDescIndex].vecPixelColor[DeltaWeight.first].x),
			static_cast<_uint>(m_vecWeightDataDesc[iWeightDataDescIndex].vecPixelColor[DeltaWeight.first].y),
			static_cast<_uint>(m_vecWeightDataDesc[iWeightDataDescIndex].vecPixelColor[DeltaWeight.first].z),
			static_cast<_uint>(m_vecWeightDataDesc[iWeightDataDescIndex].vecPixelColor[DeltaWeight.first].w)
		);



	}


	m_vecJoinableCheck[iWeightDataDescIndex] = true;

}




HRESULT CTerrainTool::Initialize()
{
	MakeList(m_strTerrainDataFilePath); //dat
	MakeList(m_strBufferDataFilePath); //dat

	MakeList(m_strDiffuseTextureFilePath); //png

	return S_OK;
}

void CTerrainTool::Tick(_float _fTimeDelta)
{
	FileTerrain();

	TestTick();

	EditTerrain();

	EditHight();
	if (nullptr != m_pTerrain.lock() && true == m_bIsBrush)
	{
		m_pTerrain.lock()->SetBrushRange(m_fRadius);
		if (FAILED(GAMEINSTANCE->ComputePicking(CPickingMgr::TYPE_PIXEL)))
			return;

		m_pTerrain.lock()->SetBrushPos(GAMEINSTANCE->GetWorldPosPicked());
	}

	InputEvent(_fTimeDelta);
}

HRESULT CTerrainTool::TestTick()
{

	// 사용중이라면
	if (true == m_ebIsUsingEditWeight)
	{
		_bool bIsJoinable = true;



		for (_uint iNum = 0; iNum <= m_iCountWeightDataDescIndex; iNum++)
		{
			if (m_vecThreads[iNum].joinable())
			{
				m_vecJoinableCheck[iNum] = true;
			}
		}

		for (const auto& bJoinableCheck : m_vecJoinableCheck)
		{
			bIsJoinable = bIsJoinable && bJoinableCheck;
		}

		if (true == bIsJoinable)
		{

			for (_uint iNum = 0; iNum <= m_iCountWeightDataDescIndex; iNum++)
			{
				m_vecThreads[iNum].join();
			}

			m_vecThreads.clear();

			ComPtr<ID3D11Texture2D> pWeightTexture2D{};

			D3D11_TEXTURE2D_DESC TextureDesc;
			ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

			TextureDesc.Width = m_iWeightTextureSize;
			TextureDesc.Height = m_iWeightTextureSize;
			TextureDesc.MipLevels = 1;
			TextureDesc.ArraySize = 1;
			TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			TextureDesc.SampleDesc.Quality = 0;
			TextureDesc.SampleDesc.Count = 1;

			TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
			TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			TextureDesc.MiscFlags = 0;


			_uint iNumWeightDataDesc = static_cast<_uint>(m_vecWeightDataDesc.size());

			iNumWeightDataDesc = static_cast<_uint>(m_vecWeightDataDesc.size());
			for (_uint iCntWeightDataDesc = 0; iCntWeightDataDesc < iNumWeightDataDesc; iCntWeightDataDesc++)
			{
				D3D11_SUBRESOURCE_DATA			SubResourceData;
				ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

				SubResourceData.pSysMem = m_vecWeightDataDesc[iCntWeightDataDesc].vecPixel.data();
				SubResourceData.SysMemPitch = m_iWeightTextureSize * 4;

				if (FAILED(DEVICE->CreateTexture2D(&TextureDesc, &SubResourceData, pWeightTexture2D.GetAddressOf())))
					return E_FAIL;

				ComPtr<ID3D11ShaderResourceView> pWeightSRV{};

				if (FAILED(DEVICE->CreateShaderResourceView(pWeightTexture2D.Get(), nullptr, pWeightSRV.GetAddressOf())))
					return E_FAIL;

				m_pTerrain.lock()->GetWeightDescs()[iCntWeightDataDesc].pWeightSRV = pWeightSRV;
			}



			m_ebIsUsingEditWeight = false;
		}
		else
			return E_FAIL;
	}
}

void CTerrainTool::MakeList(string _strResourcePath)
{
	filesystem::path basePath(_strResourcePath);

	for (const auto& entry : filesystem::directory_iterator(basePath))
	{
		if (entry.is_directory())
			MakeList(entry.path().string());
		else
		{
			if ("Buffer" == basePath.stem().string())
				m_vecBufferDataFiles.push_back(entry.path().stem().string());

			else if ("Terrain" == basePath.stem().string())
				m_vecLoadTerrainDataFile.push_back(entry.path().stem().string());

			//else if ("Weight" == basePath.stem().string())
			//	m_vecLoadWeightTextures.push_back(entry.path().stem().string());

			else if ("Diffuse" == basePath.stem().string())
				m_vecDiffuseTextureFiles.push_back(entry.path().stem().string());


		}
	}
}

HRESULT CTerrainTool::LoadTerrain(string& _strLoadTerrainFileName)
{
	_float4x4 matWorld{};
	string strLoadBufferDataFileName{};
	string strBaseDiffuseTextureName{};
	string strLoadWightDataFileName{};

	_uint iNumDiffuseTextureFiles = static_cast<_uint>(m_vecDiffuseTextureFiles.size());

	string strLoadTerrainDataFilePath = m_strTerrainDataFilePath + "\\" + _strLoadTerrainFileName + ".dat";

	ifstream InTerrainBinaryFile(strLoadTerrainDataFilePath, ios::in | ios::binary);
	if (InTerrainBinaryFile.is_open())
	{
		// 1. Terrain World Matrix 읽기.
		InTerrainBinaryFile.read(reinterpret_cast<char*>(&matWorld), sizeof(_float4x4));

		// 2. Buffer Data File Naem 읽기.
		_char szLoadBufferDataFileName[MAX_PATH] = "";
		InTerrainBinaryFile.read(szLoadBufferDataFileName, sizeof(szLoadBufferDataFileName));
		strLoadBufferDataFileName = szLoadBufferDataFileName;

		// 3. Base Diffuse Texture Name 읽기.
		_char szBaseDiffuseTextureName[MAX_PATH] = "";
		InTerrainBinaryFile.read(szBaseDiffuseTextureName, sizeof(szBaseDiffuseTextureName));
		strBaseDiffuseTextureName = szBaseDiffuseTextureName;

		for (_uint iCntDiffuseTextureFiles = 0; iCntDiffuseTextureFiles < iNumDiffuseTextureFiles; iCntDiffuseTextureFiles++)
		{
			if (strBaseDiffuseTextureName == m_vecDiffuseTextureFiles[iCntDiffuseTextureFiles])
			{
				m_SelectedBaseDiffuseTextureIndex = iCntDiffuseTextureFiles;
				break;
			}
		}

		// 4. Weight Data File Name 저장.
		_char szLoadWeightDataFileName[MAX_PATH] = "";
		InTerrainBinaryFile.read(szLoadWeightDataFileName, sizeof(szLoadWeightDataFileName));
		strLoadWightDataFileName = szLoadWeightDataFileName;

		InTerrainBinaryFile.close();
	}
	else
	{
		MSG_BOX("Failed to Read : Binary File");
		return E_FAIL;
	}

	MSG_BOX("Read Binary Success!");

	string strBufferDataFilePath = m_strBufferDataFilePath + "\\" + strLoadBufferDataFileName;

	ifstream InBufferBinaryFile(strBufferDataFilePath, ios::in | ios::binary);
	if (InBufferBinaryFile.is_open())
	{
		shared_ptr<CTerrain> pTerrain = CTerrain::Create(matWorld, CVITerrain::Create(DEVICE, CONTEXT, InBufferBinaryFile));
		GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Terrain"), pTerrain);
		m_pTerrain = pTerrain;

		InBufferBinaryFile.close();
	}

	m_pTerrain.lock()->SetBaseDiffuseTextureName(m_vecDiffuseTextureFiles[m_SelectedBaseDiffuseTextureIndex]);

	string strWeightDataFilePath = m_strWeightDataFilePath + "\\" + strLoadWightDataFileName;



	//vector<array<string, BLEND_END>> vecBlendTexturesFileNameGroup{};
	//vector<ComPtr<ID3D11ShaderResourceView>> vecWeightSRVs{};





	ifstream InWeightBinaryFile(strWeightDataFilePath, ios::in | ios::binary);
	if (InWeightBinaryFile.is_open())
	{




		// 1. Weight Texture의 크기.
		InWeightBinaryFile.read(reinterpret_cast<char*>(&m_iWeightTextureSize), sizeof(_uint));
		// 2. Weight Texture의 개수.
		_uint iNumWeightTextures{};
		InWeightBinaryFile.read(reinterpret_cast<char*>(&iNumWeightTextures), sizeof(_uint));

		m_vecWeightDataDesc.resize(iNumWeightTextures);

		_uint iNumPixelSize = m_iWeightTextureSize * m_iWeightTextureSize;

		// 3. 반복 시작
		m_pTerrain.lock()->GetWeightDescs().resize(iNumWeightTextures);

		for (_uint iCntWeightTextures = 0; iCntWeightTextures < iNumWeightTextures; iCntWeightTextures++)
		{

			_char szLoadWeightDescName[MAX_PATH] = "";
			InWeightBinaryFile.read(szLoadWeightDescName, sizeof(szLoadWeightDescName));
			m_vecWeightDataDesc[iCntWeightTextures].strName = szLoadWeightDescName;

			for (_uint iCntBlendTextures = 0; iCntBlendTextures < BLEND_END; iCntBlendTextures++)
			{
				// 1. Blend Texture Naem 읽기.
				_char szLoadBlendTextureFileName[MAX_PATH] = "";
				InWeightBinaryFile.read(szLoadBlendTextureFileName, sizeof(szLoadBlendTextureFileName));
				m_pTerrain.lock()->GetWeightDescs()[iCntWeightTextures].arrBlendDiffusesName[iCntBlendTextures] = szLoadBlendTextureFileName;

				for (_uint iCntDiffuseTextureFiles = 0; iCntDiffuseTextureFiles < iNumDiffuseTextureFiles; iCntDiffuseTextureFiles++)
				{
					if (szLoadBlendTextureFileName == m_vecDiffuseTextureFiles[iCntDiffuseTextureFiles])
					{
						m_vecWeightDataDesc[iCntWeightTextures].arrBlendDiffuseIndex[iCntBlendTextures] = iCntDiffuseTextureFiles;
						break;
					}
				}

			}

			m_vecWeightDataDesc[iCntWeightTextures].vecPixelColor.resize(m_iWeightTextureSize * m_iWeightTextureSize);
			m_vecWeightDataDesc[iCntWeightTextures].vecPixel.resize(m_iWeightTextureSize * m_iWeightTextureSize);

			_float4 fPixelColor{};

			for (_uint iCntPixelSize = 0; iCntPixelSize < iNumPixelSize; iCntPixelSize++)
			{
				InWeightBinaryFile.read(reinterpret_cast<char*>(&fPixelColor), sizeof(_float4));
				m_vecWeightDataDesc[iCntWeightTextures].vecPixelColor[iCntPixelSize] = fPixelColor;
				m_vecWeightDataDesc[iCntWeightTextures].vecPixel[iCntPixelSize]
					= D3DCOLOR_RGBA
					(
						static_cast<_uint>(fPixelColor.x),
						static_cast<_uint>(fPixelColor.y),
						static_cast<_uint>(fPixelColor.z),
						static_cast<_uint>(fPixelColor.w)
					);
			}

			
			
			m_pTerrain.lock()->GetWeightDescs()[iCntWeightTextures].pWeightSRV = CreateWeightSRV(m_vecWeightDataDesc[iCntWeightTextures].vecPixel);
		}

		InWeightBinaryFile.close();
	}

	return S_OK;
}

HRESULT CTerrainTool::SaveFileNameCheck()
{
	_bool bIsEmpty{};

	if (true == m_strSaveTerrainDataFileName.empty())
	{
		MSG_BOX("Empty File Name: Terrain Data!");
		bIsEmpty = true;
	}

	if (true == m_strSaveBufferDataFileName.empty())
	{
		MSG_BOX("Empty File Name: Buffer Data!");
		bIsEmpty = true;
	}
	if (true == m_strSaveWeightDataFileName.empty())
	{
		MSG_BOX("Empty File Name: Weight Data!");
		bIsEmpty = true;
	}

	if (true == bIsEmpty)
		return E_FAIL;
	else
		SaveTerrain();
	
	return S_OK;
}

HRESULT CTerrainTool::SaveTerrain()
{
#pragma region SaveTerrainDataFile
	string strSaveTerrainDataFilePath = m_strTerrainDataFilePath + "\\" + m_strSaveTerrainDataFileName + ".dat";

	ofstream OutTerrainBinaryFile(strSaveTerrainDataFilePath, ios::out | ios::binary);
	if (OutTerrainBinaryFile.is_open())
	{
		// 1. Terrain World Matrix 저장.
		_float4x4 matWorld = m_pTerrain.lock()->GetTransform()->GetWorldMatrix();
		OutTerrainBinaryFile.write(reinterpret_cast<const char*>(&matWorld), sizeof(_float4x4));

		// 2. Buffer Data File Naem 저장.
		_char szSaveBufferDataFileName[MAX_PATH] = "";
		string strSaveBufferDataFileName = m_strSaveBufferDataFileName + ".dat";
		strcpy_s(szSaveBufferDataFileName, strSaveBufferDataFileName.c_str());
		OutTerrainBinaryFile.write(szSaveBufferDataFileName, sizeof(szSaveBufferDataFileName));

		// 3. Base Diffuse Texture Name 저장.
		_char szBaseDiffuseTextureName[MAX_PATH] = "";
		strcpy_s(szBaseDiffuseTextureName, m_pTerrain.lock()->GetBaseDiffuseTextureName().c_str());
		OutTerrainBinaryFile.write(szBaseDiffuseTextureName, sizeof(szBaseDiffuseTextureName));

		// 4. Weight Data File Name 저장.
		_char szSaveWeightDataFileName[MAX_PATH] = "";
		string strSaveWeightDataFileName = m_strSaveWeightDataFileName + ".dat";
		strcpy_s(szSaveWeightDataFileName, strSaveWeightDataFileName.c_str());
		OutTerrainBinaryFile.write(szSaveWeightDataFileName, sizeof(szSaveWeightDataFileName));
		
		MSG_BOX("Succeeded to Binary : Terrain Data File!");
	}
	else // 바이너리 쓰기 실패
	{
		MSG_BOX("Failed to Binary : Terrain Data File!");
		OutTerrainBinaryFile.close();
		return E_FAIL;
	}
	
	OutTerrainBinaryFile.close();
#pragma endregion //SaveTerrainDataFile
	

	
#pragma region SaveBufferDataFile
	string strSaveBufferDataFilePath = m_strBufferDataFilePath + "\\" + m_strSaveBufferDataFileName + ".dat";

	ofstream OutBufferBinaryFile(strSaveBufferDataFilePath, ios::out | ios::binary);
	if (OutBufferBinaryFile.is_open())
	{
		static_pointer_cast<CVITerrain>(m_pTerrain.lock()->GetComponent(TEXT("Com_VIBuffer")))->SaveBinary(OutBufferBinaryFile);
		
		MSG_BOX("Succeeded to Binary : Buffer Data File!");
	}
	else // 바이너리 쓰기 실패
	{
		MSG_BOX("Failed to Binary : Buffer Data File!");
		OutBufferBinaryFile.close();
		return E_FAIL;
	}

	OutBufferBinaryFile.close();
#pragma endregion //SaveBufferDataFile



#pragma region Save Weight Data File
	string strSaveWeightDataFilePath = m_strWeightDataFilePath + "\\" + m_strSaveWeightDataFileName + ".dat";

	ofstream OutWeightBinaryFile(strSaveWeightDataFilePath, ios::out | ios::binary);
	if (OutWeightBinaryFile.is_open())
	{
		// Wight Size
		_uint iWightTextureSize = m_iWeightTextureSize;
		OutWeightBinaryFile.write(reinterpret_cast<const char*>(&iWightTextureSize), sizeof(_uint));

		// Wight Desc 개수
		_uint iNumWeightDesc = static_cast<_uint>(m_vecWeightDataDesc.size());

		OutWeightBinaryFile.write(reinterpret_cast<const char*>(&iNumWeightDesc), sizeof(_uint));

		for (_uint iCntWeightDesc = 0; iCntWeightDesc < iNumWeightDesc; iCntWeightDesc++)
		{
			// 1) Wight Desc Name
			_char szWeightDescName[MAX_PATH] = "";
			strcpy_s(szWeightDescName, m_vecWeightDataDesc[iCntWeightDesc].strName.c_str());

			OutWeightBinaryFile.write(szWeightDescName, sizeof(szWeightDescName));

			// 2) Wight Desc Blend Diffuse Name
			for (_uint iCntBlendDiffuseIndex = 0; iCntBlendDiffuseIndex < BLEND_END; iCntBlendDiffuseIndex++)
			{
				string strBlendDiffuseName = m_vecDiffuseTextureFiles[m_vecWeightDataDesc[iCntWeightDesc].arrBlendDiffuseIndex[iCntBlendDiffuseIndex]];
				_char szBlendDiffuseName[MAX_PATH] = "";
				strcpy_s(szBlendDiffuseName, strBlendDiffuseName.c_str());

				OutWeightBinaryFile.write(szBlendDiffuseName, sizeof(szBlendDiffuseName));
			}

			_uint iNumWeightTexturePixels = iWightTextureSize * iWightTextureSize;
			for (_uint iCntWeightTexturePixels = 0; iCntWeightTexturePixels < iNumWeightTexturePixels; iCntWeightTexturePixels++)
			{
				_float4 vPixelColor = m_vecWeightDataDesc[iCntWeightDesc].vecPixelColor[iCntWeightTexturePixels];

				OutWeightBinaryFile.write(reinterpret_cast<const char*>(&vPixelColor), sizeof(_float4));
			}

		}
		

		// 각 웨이트 픽셀 정보
		// 각 웨이트 블랜드 덱
		MSG_BOX("Succeeded to Binary : Weight Data File!");
	}
	else // 바이너리 쓰기 실패
	{
		MSG_BOX("Failed to Binary : Weight Data File!");
		OutWeightBinaryFile.close();
		return E_FAIL;
	}

	OutWeightBinaryFile.close();
#pragma endregion // Save Weight Data File

	return S_OK;
}

void CTerrainTool::FileTerrain()
{
#pragma region Load Terrain Data File
	if (ImGui::TreeNodeEx("Load Terrain", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushItemWidth(200);
		SelectedComboListIndex("##Load Terrain Data File List", m_vecLoadTerrainDataFile, m_iIdxSelectedLoadTerrainDataFile); ImGui::SameLine();
		ImGui::Text("Terrain Name(.dat)");

		if (ImGui::Button("Load", ImVec2(96, 19)))
			LoadTerrain(m_vecLoadTerrainDataFile[m_iIdxSelectedLoadTerrainDataFile]);

		ImGui::SameLine();

		if (ImGui::Button("Reload", ImVec2(96, 19)))
		{
			m_vecLoadTerrainDataFile.clear();
			MakeList(m_strTerrainDataFilePath);
		}
		ImGui::TreePop();
	}
#pragma endregion // Load Terrain Data File

#pragma region Save Terrain Data File
	if (ImGui::TreeNodeEx("Save Terrain", ImGuiTreeNodeFlags_DefaultOpen))
	{
		m_strSaveTerrainDataFileName.resize(MAX_PATH);
		ImGui::PushItemWidth(200);
		ImGui::InputText("##Save Terrain Data File Name", &m_strSaveTerrainDataFileName[0], m_strSaveTerrainDataFileName.size()); ImGui::SameLine();
		m_strSaveTerrainDataFileName.resize(m_strSaveTerrainDataFileName.find_last_not_of('\0') + 1);  // 문자열을 마지막 문자 이후로 잘라냅니다.
		ImGui::Text("Terrain Name(.dat)");

		m_strSaveBufferDataFileName.resize(MAX_PATH);
		ImGui::PushItemWidth(200);
		ImGui::InputText("##Save Buffer Data File Name", &m_strSaveBufferDataFileName[0], m_strSaveBufferDataFileName.size()); ImGui::SameLine();
		m_strSaveBufferDataFileName.resize(m_strSaveBufferDataFileName.find_last_not_of('\0') + 1);  // 문자열을 마지막 문자 이후로 잘라냅니다.
		ImGui::Text("Buffer Name(.dat)");

		m_strSaveWeightDataFileName.resize(MAX_PATH);
		ImGui::PushItemWidth(200);
		ImGui::InputText("##Save Weight Data File Name", &m_strSaveWeightDataFileName[0], m_strSaveWeightDataFileName.size()); ImGui::SameLine();
		m_strSaveWeightDataFileName.resize(m_strSaveWeightDataFileName.find_last_not_of('\0') + 1);  // 문자열을 마지막 문자 이후로 잘라냅니다.
		ImGui::Text("Weight Name(.dat)");
	
		if (ImGui::Button("Save", ImVec2(96, 19)))
		{
			SaveFileNameCheck();
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset", ImVec2(96, 19)))
		{
			m_strSaveTerrainDataFileName.clear();
			m_strSaveBufferDataFileName.clear();
			m_strSaveWeightDataFileName.clear();
		}

		ImGui::TreePop();
	}
#pragma endregion // Save Terrain Data File


#pragma region Clear Terrain Data File
	if (ImGui::TreeNodeEx("Clear Terrain", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Delete", ImVec2(200, 19)))
		{
			if (nullptr == m_pTerrain.lock())
				MSG_BOX("Failed : Terrain Nullptr!");
			else
			{
				m_pTerrain.lock()->SetActive(false);

				m_strSaveTerrainDataFileName.clear();
				m_strSaveBufferDataFileName.clear();
				m_strSaveWeightDataFileName.clear();
			}
		}
		ImGui::TreePop();
	}
#pragma endregion // Clear Terrain Data File
}


/*
HRESULT CTerrainTool::LoadTextures()
{
	SelectedListIndex("##Load Texture List", m_vecDiffuseTextureFiles, m_iIdxSelectedLoadDiffuseTextureFile);
	return S_OK;
}
*/
/*
HRESULT CTerrainTool::MakeTerrain()
{
	
	if (ImGui::Button("Create"))
	{
		shared_ptr<CVITerrain> pVITerrain{};

		// 버퍼 택 1
		if (BUFFER_CREATE == m_iSelectedLoadBufferType)
		{
			pVITerrain = CVITerrain::Create(DEVICE, CONTEXT, m_arrCreateSize);
		}

		else if (BUFFER_BINARYFILE == m_iSelectedLoadBufferType)
		{
			string strBufferFilePath = m_strBufferFilePath + m_vecLoadBufferFile[m_iIdxSelectedLoadBufferFile] + ".dat";

			ifstream InBinaryFile(strBufferFilePath, ios::in | ios::binary);
			if (InBinaryFile.is_open())
			{
				pVITerrain = CVITerrain::Create(DEVICE, CONTEXT, InBinaryFile);
				InBinaryFile.close();
			}
		}

		// 텍스쳐 불러오기
		string TextureName = m_vecLoadTextures[m_iIdxSelectedLoadTextures];

		shared_ptr<CTerrain> pTerrain = CTerrain::Create(pVITerrain, TextureName);
		GAMEINSTANCE->AddObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Terrain"), pTerrain);
		m_pTerrain = pTerrain;
	}
	
	return S_OK;
}
*/




void CTerrainTool::EditGizmo()
{
	if (nullptr == m_pTerrain.lock())
		ImGui::SetNextItemOpen(false);


	if (ImGui::TreeNodeEx("Edit Gizmo", ImGuiTreeNodeFlags_None))
	{
		if (nullptr == m_pTerrain.lock())
		{
			MSG_BOX("Unable to Access : Selected Object Nullptr!");
			ImGui::TreePop();
			return;
		}

		_float4x4 matTerrain = m_pTerrain.lock()->GetTransform()->GetWorldMatrix();

		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);

		float vMeshMatrix[16] = {
			   matTerrain._11, matTerrain._12, matTerrain._13, matTerrain._14,
			   matTerrain._21, matTerrain._22, matTerrain._23, matTerrain._24,
			   matTerrain._31, matTerrain._32, matTerrain._33, matTerrain._34,
			   matTerrain._41, matTerrain._42, matTerrain._43, matTerrain._44,
		};

		// 마우스 
		
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

		// 입력창
		float vScale[3], vRotation[3], vTranslation[3];
		ImGuizmo::DecomposeMatrixToComponents(vMeshMatrix, vTranslation, vRotation, vScale);
		ImGui::InputFloat3("S", vScale, "%.5f");
		ImGui::InputFloat3("R", vRotation, "%.5f");
		ImGui::InputFloat3("T", vTranslation, "%.5f");
		ImGuizmo::RecomposeMatrixFromComponents(vTranslation, vRotation, vScale, vMeshMatrix);

		// 월드 & 로컬 버튼
		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}


		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::Manipulate(GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW).m[0], GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ).m[0], mCurrentGizmoOperation, mCurrentGizmoMode, vMeshMatrix, NULL, NULL);
		//_float4x4;
		_float4 vRight{ vMeshMatrix[0], vMeshMatrix[1],vMeshMatrix[2], vMeshMatrix[3] };
		_float4 vUp{ vMeshMatrix[4], vMeshMatrix[5],vMeshMatrix[6], vMeshMatrix[7] };
		_float4 vLook{ vMeshMatrix[8], vMeshMatrix[9],vMeshMatrix[10], vMeshMatrix[11] };
		_float4 vPos{ vMeshMatrix[12], vMeshMatrix[13],vMeshMatrix[14], vMeshMatrix[15] };

		_float4x4 matWorld(vRight, vUp, vLook, vPos);

		m_pTerrain.lock()->GetTransform()->SetWorldMatrix(matWorld);
		ImGui::TreePop();
	}
}

void CTerrainTool::EditTerrain()
{
	
	if (ImGui::TreeNodeEx("Edit Terrain", ImGuiTreeNodeFlags_DefaultOpen))
	{
		EditGizmo();

		EditBuffer();

		EditBaseDiffuse();

		EditWeight();




		/*
		if (ImGui::TreeNodeEx("Edit Weight Texture", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (nullptr == m_pTerrain.lock())
			{
				ImGui::TreePop();
				ImGui::TreePop();
				return;
			}

			//EditWeightTexture();

			ImGui::TreePop(); // Edit Weight Texture
		}
		*/
		ImGui::TreePop();
	}
	
	return;

}

void CTerrainTool::EditBuffer()
{
	if (ImGui::TreeNodeEx("Edit Buffer", ImGuiTreeNodeFlags_None))
	{

#pragma region [추후 수정] Create Buffer
//#pragma region Create Buffer
//	ImGui::SeparatorText("Create Buffer");
//
//	ImGui::PushItemWidth(200);
//	ImGui::InputInt3("##Create Buffer Size", m_arrCreateSize.data(), ImGuiInputTextFlags_CharsDecimal); ImGui::SameLine();
//	m_arrCreateSize[AXIS_Y] = 0;
//	m_arrCreateSize[AXIS_Z] = m_arrCreateSize[AXIS_X];
//	ImGui::Text("Create Size");
//
//	m_strCreateBufferName.resize(MAX_PATH);
//	ImGui::PushItemWidth(200);
//	ImGui::InputText("##Create Buffer Name", &m_strCreateBufferName[0], m_strCreateBufferName.size()); ImGui::SameLine();
//	m_strCreateBufferName.resize(m_strCreateBufferName.find_last_not_of('\0') + 1);  // 문자열을 마지막 문자 이후로 잘라냅니다.
//	ImGui::Text("Create Name(.dat)");
//
//	if (ImGui::Button("Create", ImVec2(200, 19)))
//		CreateBuffer();
//#pragma endregion //Create Buffer
#pragma endregion // [추후 수정] Create Buffer

	ImGui::SeparatorText("Change Buffer");
	ImGui::PushItemWidth(200);
	SelectedComboListIndex("##Buffer Data File List", m_vecBufferDataFiles, m_SelectedBufferDataFileIndex); ImGui::SameLine();
	ImGui::Text("Buffer List");

#pragma region Change Buffer Data File
	if (ImGui::Button("Change", ImVec2(96, 19)))
	{
		if (true == m_strSaveBufferDataFileName.empty())
			MSG_BOX("Empty File Name: Terrain Data!");
		else
		{
			string strSaveBufferDataFilePath = m_strBufferDataFilePath + "\\" + m_strSaveBufferDataFileName + ".dat";

			ofstream OutBufferBinaryFile(strSaveBufferDataFilePath, ios::out | ios::binary);
			if (OutBufferBinaryFile.is_open())
				m_pTerrain.lock()->GetVIBuffer()->SaveBinary(OutBufferBinaryFile);

			OutBufferBinaryFile.close();
			m_strSaveBufferDataFileName.clear();
		}

		string strBufferDataFilePath = m_strBufferDataFilePath + "\\" + m_vecBufferDataFiles[m_SelectedBufferDataFileIndex] + ".dat";

		ifstream InBufferBinaryFile(strBufferDataFilePath, ios::in | ios::binary);

		if (InBufferBinaryFile.is_open())
		{
			shared_ptr<CVITerrain> pVITerrain = CVITerrain::Create(DEVICE, CONTEXT, InBufferBinaryFile);
			m_pTerrain.lock()->SetVIBuffer(pVITerrain);
			InBufferBinaryFile.close();
		}
	}
#pragma endregion // Change Buffer Data File

	ImGui::SameLine();

#pragma region Reload Buffer Data File List
	if (ImGui::Button("Reload", ImVec2(96, 19)))
	{
		m_vecBufferDataFiles.clear();
		MakeList(m_strBufferDataFilePath);
	}
#pragma endregion // Reload Buffer Data File List

#pragma region Save Buffer Data File List
	ImGui::SeparatorText("Save Buffer");

	if (ImGui::Button("Save", ImVec2(200, 19)))
	{
		if (true == m_strSaveBufferDataFileName.empty())
			MSG_BOX("Empty File Name: Terrain Data!");
		else
		{
			string strSaveBufferDataFilePath = m_strBufferDataFilePath + "\\" + m_strSaveBufferDataFileName + ".dat";

			ofstream OutBufferBinaryFile(strSaveBufferDataFilePath, ios::out | ios::binary);
			if (OutBufferBinaryFile.is_open())
				m_pTerrain.lock()->GetVIBuffer()->SaveBinary(OutBufferBinaryFile);

			OutBufferBinaryFile.close();
			m_strSaveBufferDataFileName.clear();
		}
	}
#pragma endregion // Save Buffer Data File List

#pragma region Buffer View Option
	ImGui::SeparatorText("Save View Option");

#pragma endregion // Buffer View Option

#pragma region Edit Buffer Hight

#pragma endregion // Edit Buffer Hight
	ImGui::TreePop(); // Edit Weight Texture
	}
}

void CTerrainTool::EditBaseDiffuse()
{
	if (ImGui::TreeNodeEx("Edit Base Diffuse", ImGuiTreeNodeFlags_None))
	{
		ImGui::SeparatorText("Change Base Diffuse");
		ImGui::PushItemWidth(200);
		SelectedComboListIndex("##Base Diffuse Texture File List", m_vecDiffuseTextureFiles, m_SelectedBaseDiffuseTextureIndex); ImGui::SameLine();
		ImGui::Text("Diffuse List");

#pragma region Change Base Diffuse
		if (ImGui::Button("Change", ImVec2(96, 19)))
			m_pTerrain.lock()->SetBaseDiffuseTextureName(m_vecDiffuseTextureFiles[m_SelectedBaseDiffuseTextureIndex]);
#pragma endregion // Change Base Diffuse

		ImGui::SameLine();

#pragma region Reload Base Diffuse List
		if (ImGui::Button("Reload", ImVec2(96, 19)))
		{
			m_vecDiffuseTextureFiles.clear();
			MakeList(m_strDiffuseTextureFilePath);
		}
#pragma endregion // Reload Base Diffuse List

		ImGui::TreePop(); // Edit Weight Texture
	}
}

void CTerrainTool::EditWeight()
{
	if (ImGui::TreeNodeEx("Edit Weight", ImGuiTreeNodeFlags_None))
	{
		if (ImGui::TreeNodeEx("Edit Weight Desc", ImGuiTreeNodeFlags_None))
		{
			EditWeightDesc();
			ImGui::TreePop(); // Edit Weight Texture
		}

		if (ImGui::TreeNodeEx("Edit Blend Diffuse", ImGuiTreeNodeFlags_None))
		{
			EditBlendDiffuse();
			ImGui::TreePop(); // Edit Weight Texture
		}

		ImGui::TreePop(); // Edit Weight Texture
	}
}

void CTerrainTool::EditWeightDesc()
{
#pragma region Add Weight Desc
	ImGui::SeparatorText("Add Weight Desc");

	// Description Name
	
	m_strAddWeightDescName.resize(MAX_PATH);
	ImGui::PushItemWidth(200);
	ImGui::InputText("##Add Weight Desc Name", &m_strAddWeightDescName[0], m_strAddWeightDescName.size()); ImGui::SameLine();
	m_strAddWeightDescName.resize(m_strAddWeightDescName.find_last_not_of('\0') + 1);  // 문자열을 마지막 문자 이후로 잘라냅니다.
	ImGui::Text("Weight Desc Name");

	// Create
	if (ImGui::Button("Add", ImVec2(200, 19)))
	{
		if (5 <= static_cast<_uint>(m_vecWeightDataDesc.size()))
			MSG_BOX("Maximum : Weight Desc");
		else
		{
			WEIGHTDATA_DESC NewWeightDataDesc{};
			NewWeightDataDesc.strName = m_strAddWeightDescName;
			NewWeightDataDesc.vecPixelColor.resize(m_iWeightTextureSize * m_iWeightTextureSize);
			NewWeightDataDesc.vecPixel.resize(m_iWeightTextureSize * m_iWeightTextureSize);

			m_vecWeightDataDesc.push_back(NewWeightDataDesc);
			m_strAddWeightDescName.clear();

			CTerrain::WEIGHT_DESC NewWeightDesc{};
			NewWeightDesc.pWeightSRV = CreateWeightSRV(NewWeightDataDesc.vecPixel);
			for (_uint iCntBlendDiffusesName = 0; iCntBlendDiffusesName < BLEND_END; iCntBlendDiffusesName++)
				NewWeightDesc.arrBlendDiffusesName[iCntBlendDiffusesName] = m_vecDiffuseTextureFiles[NewWeightDataDesc.arrBlendDiffuseIndex[iCntBlendDiffusesName]];
			m_pTerrain.lock()->GetWeightDescs().push_back(NewWeightDesc);
		}
	}
#pragma endregion // Add Weight Desc


#pragma region Clear Weight Desc
	ImGui::SeparatorText("Clear Weight Desc");
	vector<string> vecWeightDescNames{};
	for (const auto& WeightDesc : m_vecWeightDataDesc)
		vecWeightDescNames.push_back(WeightDesc.strName);

	_uint iNumWeightDesc = static_cast<_uint>(vecWeightDescNames.size());

	SelectedComboListIndex("##Crear Weight Desc List", vecWeightDescNames, m_iSelectedClearWeightDescIndex); ImGui::SameLine();

	ImGui::Text("Weight Desc Name");

	if (ImGui::Button("Reset", ImVec2(96, 19)))
	{
		if (0 > m_iSelectedClearWeightDescIndex)
			MSG_BOX("Empty : Weight Desc!");
		else // 0 < iNumWeightDesc
		{
			m_vecWeightDataDesc[m_iSelectedClearWeightDescIndex].vecPixelColor.clear();
			m_vecWeightDataDesc[m_iSelectedClearWeightDescIndex].vecPixelColor.resize(m_iWeightTextureSize * m_iWeightTextureSize);

			m_vecWeightDataDesc[m_iSelectedClearWeightDescIndex].vecPixel.clear();
			m_vecWeightDataDesc[m_iSelectedClearWeightDescIndex].vecPixel.resize(m_iWeightTextureSize * m_iWeightTextureSize);

			m_vecWeightDataDesc[m_iSelectedClearWeightDescIndex].arrBlendDiffuseIndex = array<_uint, BLEND_END>();
			m_vecWeightDataDesc[m_iSelectedClearWeightDescIndex].iSelectedBlendDiffuseChannelIndex = 0;
			
			CTerrain::WEIGHT_DESC NewWeightDesc{};
			NewWeightDesc.pWeightSRV = CreateWeightSRV(m_vecWeightDataDesc[m_iSelectedClearWeightDescIndex].vecPixel);
			for (_uint iCntBlendDiffusesName = 0; iCntBlendDiffusesName < BLEND_END; iCntBlendDiffusesName++)
				NewWeightDesc.arrBlendDiffusesName[iCntBlendDiffusesName] = m_vecWeightDataDesc[m_iSelectedClearWeightDescIndex].arrBlendDiffuseIndex[iCntBlendDiffusesName];
			
			m_pTerrain.lock()->GetWeightDescs()[m_iSelectedClearWeightDescIndex] = NewWeightDesc;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete", ImVec2(96, 19)))
	{
		if (0 > m_iSelectedClearWeightDescIndex)
			MSG_BOX("Empty : Weight Desc!");
		else // 0 < iNumWeightDesc
		{
			m_vecWeightDataDesc.erase(m_vecWeightDataDesc.begin() + m_iSelectedClearWeightDescIndex);
			
			if (0 < m_iSelectedClearWeightDescIndex)
				--m_iSelectedClearWeightDescIndex;

			if (0 < m_iSelectedWeightDescIndex)
				--m_iSelectedWeightDescIndex;

			m_pTerrain.lock()->GetWeightDescs().erase(m_pTerrain.lock()->GetWeightDescs().begin() + m_iSelectedClearWeightDescIndex);
		}
	}
#pragma endregion // Clear Weight Desc
}

void CTerrainTool::EditBlendDiffuse()
{

	ImGui::SeparatorText("Edit Weight Desc");
	ImGui::PushItemWidth(200);

	vector<string> vecWeightDescNames{};
	for (const auto& WeightDesc : m_vecWeightDataDesc)
		vecWeightDescNames.push_back(WeightDesc.strName);

	SelectedComboListIndex("##Edit Weight Desc List", vecWeightDescNames, m_iSelectedWeightDescIndex); ImGui::SameLine();

	ImGui::Text("Edit Weight Desc List");

	//Add Weight Desc
	//Delect Weight Desc




#pragma region Edit Weight Texture

	ImGui::SeparatorText("Edit Blend Diffuse");
	//ImGui::PushItemWidth(200);
	//SelectedListIndex("##Edit Weight Texture List", m_vecDiffuseTextureFiles, m_iIdxSelectedLoadWeightTextures); ImGui::SameLine();
	//ImGui::Text("Edit Name");
	//m_vecLoadWeightTextures[m_iIdxSelectedLoadWeightTextures].

	//
	_uint iNumWeightDesc = static_cast<_uint>(m_vecWeightDataDesc.size());
	if (0 < iNumWeightDesc)
	{
		
		ImGui::RadioButton("##Select Blend Diffuse Type Red", &(m_vecWeightDataDesc[m_iSelectedWeightDescIndex].iSelectedBlendDiffuseChannelIndex), BLEND_RED); ImGui::SameLine(); ImGui::SameLine();
		SelectedComboListIndex("##Blend Texture Type Red List", m_vecDiffuseTextureFiles, m_vecWeightDataDesc[m_iSelectedWeightDescIndex].arrBlendDiffuseIndex[BLEND_RED]);
		m_pTerrain.lock()->GetWeightDescs()[m_iSelectedWeightDescIndex].arrBlendDiffusesName[BLEND_RED] = m_vecDiffuseTextureFiles[m_vecWeightDataDesc[m_iSelectedWeightDescIndex].arrBlendDiffuseIndex[BLEND_RED]];

		ImGui::RadioButton("##Select Blend Diffuse Type Green", &(m_vecWeightDataDesc[m_iSelectedWeightDescIndex].iSelectedBlendDiffuseChannelIndex), BLEND_GREEN); ImGui::SameLine(); ImGui::SameLine();
		SelectedComboListIndex("##Blend Texture Type Green List", m_vecDiffuseTextureFiles, m_vecWeightDataDesc[m_iSelectedWeightDescIndex].arrBlendDiffuseIndex[BLEND_GREEN]);
		m_pTerrain.lock()->GetWeightDescs()[m_iSelectedWeightDescIndex].arrBlendDiffusesName[BLEND_GREEN] = m_vecDiffuseTextureFiles[m_vecWeightDataDesc[m_iSelectedWeightDescIndex].arrBlendDiffuseIndex[BLEND_GREEN]];;

		ImGui::RadioButton("##Select Blend Diffuse Type Blue", &(m_vecWeightDataDesc[m_iSelectedWeightDescIndex].iSelectedBlendDiffuseChannelIndex), BLEND_BLUE); ImGui::SameLine(); ImGui::SameLine();
		SelectedComboListIndex("##Blend Texture Type Blue List", m_vecDiffuseTextureFiles, m_vecWeightDataDesc[m_iSelectedWeightDescIndex].arrBlendDiffuseIndex[BLEND_BLUE]);
		m_pTerrain.lock()->GetWeightDescs()[m_iSelectedWeightDescIndex].arrBlendDiffusesName[BLEND_BLUE] = m_vecDiffuseTextureFiles[m_vecWeightDataDesc[m_iSelectedWeightDescIndex].arrBlendDiffuseIndex[BLEND_BLUE]];;

		ImGui::RadioButton("##Select Blend Diffuse Type Alpha", &(m_vecWeightDataDesc[m_iSelectedWeightDescIndex].iSelectedBlendDiffuseChannelIndex), BLEND_ALPHA); ImGui::SameLine(); ImGui::SameLine();
		SelectedComboListIndex("##Blend Texture Type Alpha List", m_vecDiffuseTextureFiles, m_vecWeightDataDesc[m_iSelectedWeightDescIndex].arrBlendDiffuseIndex[BLEND_ALPHA]);
		m_pTerrain.lock()->GetWeightDescs()[m_iSelectedWeightDescIndex].arrBlendDiffusesName[BLEND_ALPHA] = m_vecDiffuseTextureFiles[m_vecWeightDataDesc[m_iSelectedWeightDescIndex].arrBlendDiffuseIndex[BLEND_ALPHA]];;
	}

#pragma endregion //Edit Weight Texture
}

void CTerrainTool::EditWeightTexture()
{
#pragma region Create Weight Texture
	ImGui::SeparatorText("Create Weight Texture");

	m_strCreateWeightTextureName.resize(MAX_PATH);
	ImGui::PushItemWidth(200);
	ImGui::InputText("##Create Weight Texture Name", &m_strCreateWeightTextureName[0], m_strCreateWeightTextureName.size()); ImGui::SameLine();

	m_strCreateWeightTextureName.resize(m_strCreateWeightTextureName.find_last_not_of('\0') + 1);  // 문자열을 마지막 문자 이후로 잘라냅니다.

	ImGui::Text("Create Name");

	if (ImGui::Button("Create", ImVec2(200, 19)))
	{
		//CreateWeightTexture();
		m_strCreateWeightTextureName.clear();
	}
#pragma endregion //Create Weight Texture


#pragma region Load Weight Texture
	ImGui::SeparatorText("Load Weight Texture");
	ImGui::PushItemWidth(200);
	SelectedComboListIndex("##Load Weight Texture List", m_vecLoadWeightTextures, m_iIdxSelectedLoadWeightTextures); ImGui::SameLine();
	ImGui::Text("Load Name");

	if (ImGui::Button("Load", ImVec2(96, 19)))
	{
		LoadWeightTexture();
		m_vecEditWeightTextures[0] = m_vecLoadWeightTextures[m_iIdxSelectedLoadWeightTextures];
	}
	ImGui::SameLine();
	if (ImGui::Button("Update", ImVec2(96, 19)))
		UpdateWeightTextureList();
#pragma endregion //Load Weight Texture


#pragma region Save Weight Texture
	ImGui::SeparatorText("Save Weight Texture");
	m_strSaveWeightTextureName.resize(MAX_PATH);
	ImGui::PushItemWidth(200);
	ImGui::InputText("##Save Weight Texture Name", &m_strSaveWeightTextureName[0], m_strSaveWeightTextureName.size()); ImGui::SameLine();
	ImGui::Text("Save Name");

	if (ImGui::Button("Save", ImVec2(200, 19)))
	{
		//SaveWeightTexture();
		m_strCreateWeightTextureName.clear();
	}
#pragma endregion //Save Weight Texture


#pragma region Edit Weight Texture
	ImGui::SeparatorText("Edit Weight Texture");
	//ImGui::PushItemWidth(200);
	//SelectedListIndex("##Edit Weight Texture List", m_vecEditWeightTextures, m_iIdxSelectedLoadWeightTextures); ImGui::SameLine();
	//ImGui::Text("Edit Name");
	//m_vecLoadWeightTextures[m_iIdxSelectedLoadWeightTextures].
	if (ImGui::TreeNodeEx(m_vecLoadWeightTextures[m_iIdxSelectedLoadWeightTextures].c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		/*
		string a{};

		ImGui::RadioButton("##Select Blend Texture Type Red", &m_iSelectBlendTextureType, BLEND_RED); ImGui::SameLine(); ImGui::SameLine();
		SelectedListIndex("##Blend Texture Type Red List", m_vecDiffuseTextureFiles, m_iIdxSelectedBlendTextures[BLEND_RED]);
		m_pTerrain.lock()->SetBlendTexture(CTerrain::BLEND_RED, m_vecDiffuseTextureFiles[m_iIdxSelectedBlendTextures[BLEND_RED]]);
		a = m_vecDiffuseTextureFiles[m_iIdxSelectedBlendTextures[BLEND_RED]];

		ImGui::RadioButton("##Select Blend Texture Type Green", &m_iSelectBlendTextureType, BLEND_GREEN); ImGui::SameLine(); ImGui::SameLine();
		SelectedListIndex("##Blend Texture Type Green List", m_vecDiffuseTextureFiles, m_iIdxSelectedBlendTextures[BLEND_GREEN]);
		m_pTerrain.lock()->SetBlendTexture(CTerrain::BLEND_GREEN, m_vecDiffuseTextureFiles[m_iIdxSelectedBlendTextures[BLEND_GREEN]]);
		a = m_vecDiffuseTextureFiles[m_iIdxSelectedBlendTextures[BLEND_GREEN]];

		ImGui::RadioButton("##Select Blend Texture Type Blue", &m_iSelectBlendTextureType, BLEND_BLUE); ImGui::SameLine(); ImGui::SameLine();
		SelectedListIndex("##Blend Texture Type Blue List", m_vecDiffuseTextureFiles, m_iIdxSelectedBlendTextures[BLEND_BLUE]);
		m_pTerrain.lock()->SetBlendTexture(CTerrain::BLEND_BLUE, m_vecDiffuseTextureFiles[m_iIdxSelectedBlendTextures[BLEND_BLUE]]);
		a = m_vecDiffuseTextureFiles[m_iIdxSelectedBlendTextures[BLEND_BLUE]];

		ImGui::RadioButton("##Select Blend Texture Type Alpha", &m_iSelectBlendTextureType, BLEND_ALPHA); ImGui::SameLine(); ImGui::SameLine();
		SelectedListIndex("##Blend Texture Type Alpha List", m_vecDiffuseTextureFiles, m_iIdxSelectedBlendTextures[BLEND_ALPHA]);
		m_pTerrain.lock()->SetBlendTexture(CTerrain::BLEND_ALPHA, m_vecDiffuseTextureFiles[m_iIdxSelectedBlendTextures[BLEND_ALPHA]]);
		a = m_vecDiffuseTextureFiles[m_iIdxSelectedBlendTextures[BLEND_ALPHA]];
		*/
		ImGui::TreePop();
	}
#pragma endregion //Edit Weight Texture
}

void CTerrainTool::EditHight()
{
	ImGui::SeparatorText("Brush Option");

	ImGui::PushItemWidth(150);
	ImGui::SliderFloat("Radius", &m_fRadius, 0.1f, 25.f, "%.1f");
	ImGui::SliderFloat("weight", &m_fSharp, 0.1f, 25.f, "%.1f");

}


ComPtr<ID3D11ShaderResourceView> CTerrainTool::CreateWeightSRV(vector<_uint>& _vecPixel)
{
	ComPtr<ID3D11Texture2D> pWeightTexture2D{};

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = m_iWeightTextureSize;
	TextureDesc.Height = m_iWeightTextureSize;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA	SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	SubResourceData.pSysMem = _vecPixel.data();
	SubResourceData.SysMemPitch = m_iWeightTextureSize * 4;

	if (FAILED(DEVICE->CreateTexture2D(&TextureDesc, &SubResourceData, pWeightTexture2D.GetAddressOf())))
		return nullptr;

	ComPtr<ID3D11ShaderResourceView> pWeightSRV{};

	if (FAILED(DEVICE->CreateShaderResourceView(pWeightTexture2D.Get(), nullptr, pWeightSRV.GetAddressOf())))
		return nullptr;
	
	return pWeightSRV;
}

HRESULT CTerrainTool::LoadWeightTexture()
{
	/*
	ComPtr<ID3D11Texture2D> pWeightTexture2D{};

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = m_iWeightTextureSize;
	TextureDesc.Height = m_iWeightTextureSize;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	m_vecPixel.resize(TextureDesc.Width * TextureDesc.Height);
	m_vecPixelColor.resize(TextureDesc.Width * TextureDesc.Height);

	m_vecPixel[iPixelIndex] = D3DCOLOR_RGBA
	(
		static_cast<_uint>(m_vecPixelColor[iPixelIndex].x),
		static_cast<_uint>(m_vecPixelColor[iPixelIndex].y),
		static_cast<_uint>(m_vecPixelColor[iPixelIndex].z),
		static_cast<_uint>(m_vecPixelColor[iPixelIndex].w)
	);


	D3D11_SUBRESOURCE_DATA	SubResourceData;
	ZeroMemory(&SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	SubResourceData.pSysMem = m_vecPixel.data();
	SubResourceData.SysMemPitch = 2048 * 4;

	if (FAILED(DEVICE->CreateTexture2D(&TextureDesc, &SubResourceData, pWeightTexture2D.GetAddressOf())))
		return E_FAIL;

	ComPtr<ID3D11ShaderResourceView> pWeightSRV{};

	if (FAILED(DEVICE->CreateShaderResourceView(pWeightTexture2D.Get(), nullptr, pWeightSRV.GetAddressOf())))
		return E_FAIL;

	m_pTerrain.lock()->SetWeightSRV(pWeightSRV);
	*/
	return S_OK;
}

void CTerrainTool::UpdateWeightTextureList()
{

}

HRESULT CTerrainTool::EditWeightTexture(_float _fTimeDelta)
{
	if (true == m_ebIsUsingEditWeight)
	{
		return E_FAIL;
	}

	else // (false == m_ebIsUsingEditWeight)
	{
		m_ebIsUsingEditWeight = true; // 사용 중으로 변환

		m_mapDeltaWeight.clear();

		_float fWeightTextureSize = static_cast<_float>(m_iWeightTextureSize);
		POINT ptMouse{};
		// 마우스 스크린 좌표를 가져오기

		if (FAILED(GAMEINSTANCE->ComputePicking(CPickingMgr::TYPE_PIXEL)))
			return E_FAIL;
		m_vPosPicked = GAMEINSTANCE->GetWorldPosPicked();

		shared_ptr<CVITerrain> pVITerrain = m_pTerrain.lock()->GetVIBuffer();
		_int iNumVerticesX = pVITerrain->GetNumX();
		_int iNumVerticesZ = pVITerrain->GetNumZ();

		// 터레인 로컬 좌표로 변환.
		_float4x4 matInvWorld = m_pTerrain.lock()->GetTransform()->GetWorldMatrixInverse();
		_float3 vLocalPos = SimpleMath::Vector3::Transform(m_vPosPicked, matInvWorld);

		// 해당 좌표가 터레인에서 비율이 어느정도인지 찾아내야한다
		_float fTerrainPosRatioX = vLocalPos.x / iNumVerticesX;
		_float fTerrainPosRatioZ = vLocalPos.z / iNumVerticesZ;

		_float fRadiusRatio = m_fRadius / iNumVerticesX;



		_float3 vWeightPos{};
		vWeightPos.x = (fTerrainPosRatioX * fWeightTextureSize);
		vWeightPos.z = (fTerrainPosRatioZ * fWeightTextureSize);

		_float fWeightMapRadius = fRadiusRatio * fWeightTextureSize;

		//범위에 해당되는 픽셀 집합을 구한다.
		_int iRectLT_X = max(static_cast<_int>(floor(vWeightPos.x - fWeightMapRadius)), 0);
		_int iRectLT_Z = max(static_cast<_int>(floor(vWeightPos.z - fWeightMapRadius)), 0);

		_int iRectRB_X = min(static_cast<_int>(floor(vWeightPos.x + fWeightMapRadius)), fWeightTextureSize);
		_int iRectRB_Z = min(static_cast<_int>(floor(vWeightPos.z + fWeightMapRadius)), fWeightTextureSize);

		//_uint iWeightDataDescIndex = m_iCountWeightDataDescIndex;

		//_float fWeightTextureSize = static_cast<_float>(m_iWeightTextureSize);

		for (_int iCntRectZ = iRectLT_Z; iCntRectZ < iRectRB_Z; ++iCntRectZ)
		{
			for (_int iCntRectX = iRectLT_X; iCntRectX < iRectRB_X; ++iCntRectX)
			{
				// 가상의 픽셀 좌표를 만들고
				_float3 vPixelPos = _float3(static_cast<_float>(iCntRectX), 0.f, static_cast<_float>(iCntRectZ));
				// 픽셀좌표와 실제 변환된 로컬좌표를 비교해서 한번더 걸러낸다.

				_float fDistance = (vPixelPos - vWeightPos).Length();

				// 거리가 변환 반지름 보다 크면 컨티뉴
				if (fDistance > fWeightMapRadius)
					continue;
				else
				{
					_float fAcc = fDistance / fWeightMapRadius;
					_float fWeight = (1.f - fAcc) * m_fSharp;

					// 픽셀 가중치에 합산.
					_uint iPixelIndex = static_cast<_uint>((iCntRectZ * static_cast<_int>(fWeightTextureSize)) + iCntRectX);

					m_mapDeltaWeight.emplace(iPixelIndex, fWeight);
				}
			}
		}

		m_vecThreads.clear();
		m_vecJoinableCheck.clear();

		_uint iNumWeightDataDesc = static_cast<_uint>(m_vecWeightDataDesc.size());

		m_vecThreads.resize(iNumWeightDataDesc);
		m_vecJoinableCheck.resize(iNumWeightDataDesc);

		for (_uint iNum = 0; iNum < iNumWeightDataDesc; iNum++)
		{
			m_vecJoinableCheck[iNum] = false;
			m_iCountWeightDataDescIndex = iNum;
			m_vecThreads[iNum] = std::thread(&CTerrainTool::UpdateWeightDataDesc, shared_from_this(), iNum);
			
		}

		return S_OK;
	}
}

void CTerrainTool::InputEvent(_float _fTimeDelta)
{
	if (GAMEINSTANCE->KeyPressing(DIK_R) && false == m_ebIsUsingEditWeight && false == MANAGER->GetIsHotkeyLock())
	{
		EditWeightTexture(_fTimeDelta);
	}
		//CreateWeightTexture();

	if (GAMEINSTANCE->KeyDown(DIK_F) && false == MANAGER->GetIsHotkeyLock())
		m_pTerrain.lock()->ChangeViewMode();
	//CreateWeightTexture();

	if (nullptr == m_pTerrain.lock())
		return;

	if (GAMEINSTANCE->KeyDown(DIK_L) && false == MANAGER->GetIsHotkeyLock())
		m_bIsBrush = true;
	
	if (GAMEINSTANCE->KeyPressing(DIK_Q) && false == MANAGER->GetIsHotkeyLock())
		UpdateHightPos(m_fSharp, _fTimeDelta);

	if (GAMEINSTANCE->KeyPressing(DIK_E) && false == MANAGER->GetIsHotkeyLock())
		UpdateHightPos(-m_fSharp, _fTimeDelta);
	
	if (GAMEINSTANCE->KeyPressing(DIK_Z) && false == MANAGER->GetIsHotkeyLock())
		UpdateHightFlatPos(m_fTerrainFlatPivotY, _fTimeDelta);

	if (GAMEINSTANCE->KeyDown(DIK_X) && false == MANAGER->GetIsHotkeyLock())
	{
		if (FAILED(GAMEINSTANCE->ComputePicking(CPickingMgr::TYPE_PIXEL)))
			return;
		m_fTerrainFlatPivotY = GAMEINSTANCE->GetWorldPosPicked().y;
	}
}

void CTerrainTool::SelectedComboListIndex(string _strListName, vector<string>& _vecList, _uint& _iSelectedIndex)
{
	_uint iNumListSize = static_cast<_uint>(_vecList.size());

	if (0 == iNumListSize)
	{
		if (ImGui::BeginCombo("##Empty ComboBox", "Is Empty!"))
		{
			ImGui::EndCombo();

			_iSelectedIndex = 0;
		}
		
		return;
	}

	if (ImGui::BeginCombo(_strListName.c_str(), _vecList[_iSelectedIndex].c_str()))
	{
		for (_uint iCntListSize = 0; iCntListSize < iNumListSize; iCntListSize++)

		{
			bool bIsSelected = (_iSelectedIndex == iCntListSize);
			if (ImGui::Selectable(_vecList[iCntListSize].c_str(), bIsSelected))
				_iSelectedIndex = iCntListSize;

			if (true == bIsSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void CTerrainTool::UpdateHightPos(_float _fPower, _float _fTimeDelta)
{
	if (FAILED(GAMEINSTANCE->ComputePicking(CPickingMgr::TYPE_PIXEL)))
		return;
	m_vPosPicked = GAMEINSTANCE->GetWorldPosPicked();
	
	shared_ptr<CVITerrain> pVITerrain = static_pointer_cast<CVITerrain>(m_pTerrain.lock()->GetVIBuffer());
	_int iNumVerticesX = pVITerrain->GetNumX();
	_int iNumVerticesZ = pVITerrain->GetNumZ();

	// 터레인 로컬 좌표로 변환.
	_float4x4 matInvWorld = m_pTerrain.lock()->GetTransform()->GetWorldMatrixInverse();
	_float3 vLocalPos = SimpleMath::Vector3::Transform(m_vPosPicked, matInvWorld);
	
	// 현재 정점 간격을 임의로 1로 잡고.
	_int iPivotX = static_cast<_int>(round(vLocalPos.x / m_fInterval));
	_int iPivotZ = static_cast<_int>(round(vLocalPos.z / m_fInterval));

	_int iOffset = static_cast<_int>(m_fRadius / m_fInterval);

	vector<_uint> vecEditableIndices{};

	for (_int iCntOffsetZ = -iOffset; iCntOffsetZ <= iOffset; ++iCntOffsetZ)
	{
		for (_int iCntOffsetX = -iOffset; iCntOffsetX <= iOffset; ++iCntOffsetX)
		{
			_int iIdxX = iPivotX + iCntOffsetX;
			if (0 > iIdxX || iNumVerticesX <= iIdxX)
				continue;

			_int iIdxZ = iPivotZ + iCntOffsetZ;
			if (0 > iIdxZ || iNumVerticesZ <= iIdxZ)
				continue;

			vecEditableIndices.push_back(static_cast<_uint>((iIdxZ * iNumVerticesX) + iIdxX));
		}
	}
	
	pVITerrain->UpdateHight(vecEditableIndices, vLocalPos, m_fRadius, _fPower, _fTimeDelta);
	

}

void CTerrainTool::UpdateHightFlatPos(_float _fPower, _float _fTimeDelta)
{
	if (FAILED(GAMEINSTANCE->ComputePicking(CPickingMgr::TYPE_PIXEL)))
		return;
	m_vPosPicked = GAMEINSTANCE->GetWorldPosPicked();

	shared_ptr<CVITerrain> pVITerrain = static_pointer_cast<CVITerrain>(m_pTerrain.lock()->GetVIBuffer());
	_int iNumVerticesX = pVITerrain->GetNumX();
	_int iNumVerticesZ = pVITerrain->GetNumZ();

	// 터레인 로컬 좌표로 변환.
	_float4x4 matInvWorld = m_pTerrain.lock()->GetTransform()->GetWorldMatrixInverse();
	_float3 vLocalPos = SimpleMath::Vector3::Transform(m_vPosPicked, matInvWorld);

	// 현재 정점 간격을 임의로 1로 잡고.
	_int iPivotX = static_cast<_int>(round(vLocalPos.x / m_fInterval));
	_int iPivotZ = static_cast<_int>(round(vLocalPos.z / m_fInterval));

	_int iOffset = static_cast<_int>(m_fRadius / m_fInterval);

	vector<_uint> vecEditableIndices{};

	for (_int iCntOffsetZ = -iOffset; iCntOffsetZ <= iOffset; ++iCntOffsetZ)
	{
		for (_int iCntOffsetX = -iOffset; iCntOffsetX <= iOffset; ++iCntOffsetX)
		{
			_int iIdxX = iPivotX + iCntOffsetX;
			if (0 > iIdxX || iNumVerticesX <= iIdxX)
				continue;

			_int iIdxZ = iPivotZ + iCntOffsetZ;
			if (0 > iIdxZ || iNumVerticesZ <= iIdxZ)
				continue;

			vecEditableIndices.push_back(static_cast<_uint>((iIdxZ * iNumVerticesX) + iIdxX));
		}
	}

	pVITerrain->UpdateHightFlat(vecEditableIndices, vLocalPos, m_fRadius, _fPower);

}

shared_ptr<CTerrainTool> CTerrainTool::Create()
{
	shared_ptr<CTerrainTool> pInstance = WRAPPING(CTerrainTool)();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTerrainTool");
		pInstance.reset();
	}

	return pInstance;
}
