#include "Terrain.h"
#include "VITerrain.h"
#include "Texture.h"
#include "RigidBody.h"

#include <fstream>

CTerrain::CTerrain()
{
}

CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Initialize(ifstream& _InFileStream, _bool _IsField)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	string strBufferDataFilePath = "..\\Bin\\DataFiles\\BufferData";
	string strWeightDataFilePath = "..\\Bin\\DataFiles\\WeightData";

	string strBufferDataFileName{};
	string strWightDataFileName{};

#pragma region Load Terrain Data File
	// 1. Terrain World Matrix 읽기
	_float4x4 matWorld{};
	_InFileStream.read(reinterpret_cast<char*>(&matWorld), sizeof(_float4x4));
	m_pTransformCom->SetWorldMatrix(matWorld);

	// 2. Buffer Data File 이름 읽기.
	_char szLoadBufferDataFileName[MAX_PATH] = "";
	_InFileStream.read(szLoadBufferDataFileName, sizeof(szLoadBufferDataFileName));
	strBufferDataFileName = szLoadBufferDataFileName;

	// 3. Base Diffuse Texture 이름 읽기.
	char szBaseDiffuseTextureName[MAX_PATH]{};
	_InFileStream.read(szBaseDiffuseTextureName, sizeof(szBaseDiffuseTextureName));
	m_strBaseDiffuseTextureName = szBaseDiffuseTextureName;

	// 4. Weight Data File 이름 읽기.
	_char szWeightDataFileName[MAX_PATH] = "";
	_InFileStream.read(szWeightDataFileName, sizeof(szWeightDataFileName));
	strWightDataFileName = szWeightDataFileName;

	// 이건 외부에서 하든 안에서 하든 자유
	_InFileStream.close();
#pragma endregion Load Terrain Data File

#pragma region Load Buffer Data File
	strBufferDataFilePath = strBufferDataFilePath + "\\" + strBufferDataFileName;

	ifstream InBufferBinaryFile(strBufferDataFilePath, ios::in | ios::binary);
	if (InBufferBinaryFile.is_open())
	{
		m_pVIBufferCom = CVITerrain::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), InBufferBinaryFile);
		m_Components.emplace(TEXT("Com_VIBuffer"), m_pVIBufferCom);

		InBufferBinaryFile.close();
	}
#pragma endregion Load Buffer Data File
	

#pragma region Load Weight Data File
	strWeightDataFilePath = strWeightDataFilePath + "\\" + strWightDataFileName;

	ifstream InWeightBinaryFile(strWeightDataFilePath, ios::in | ios::binary);
	if (InWeightBinaryFile.is_open())
	{

		// 1. Weight Texture의 해상도(일반적으로 2048).
		_uint iWeightTextureSize{};
		InWeightBinaryFile.read(reinterpret_cast<char*>(&iWeightTextureSize), sizeof(_uint));

		// 2. Weight Texture의 개수.
		_uint iNumWeightTextures{};
		InWeightBinaryFile.read(reinterpret_cast<char*>(&iNumWeightTextures), sizeof(_uint));

		m_vecWeightDescs.resize(iNumWeightTextures);

		_uint iNumPixelSize = iWeightTextureSize * iWeightTextureSize;


		// Weight Data 개수 만큼 반복
		for (_uint iCntWeightTextures = 0; iCntWeightTextures < iNumWeightTextures; iCntWeightTextures++)
		{
			// 3-1) Weight Data Name (Map Tool에서만 사용. 버려도 됨)
			_char szLoadWeightDescName[MAX_PATH] = "";
			InWeightBinaryFile.read(szLoadWeightDescName, sizeof(szLoadWeightDescName));
			//m_vecWeightDataDesc[iCntWeightTextures].strName = szLoadWeightDescName;

			// 3-1-1) 해당 Weight Data에 속한 Blend Texture의 개수(4개 고정)
			for (_uint iCntBlendTextures = 0; iCntBlendTextures < BLEND_END; iCntBlendTextures++)
			{
				// 3-1-1-1) Blend Texture Naem 읽기.
				_char szBlendTextureFileName[MAX_PATH] = "";
				InWeightBinaryFile.read(szBlendTextureFileName, sizeof(szBlendTextureFileName));
				m_vecWeightDescs[iCntWeightTextures].arrBlendDiffusesName[iCntBlendTextures] = szBlendTextureFileName;
			}

			vector<_uint> vecPixel(iNumPixelSize);
			_float4 fPixelColor{};

			for (_uint iCntPixelSize = 0; iCntPixelSize < iNumPixelSize; iCntPixelSize++)
			{
				InWeightBinaryFile.read(reinterpret_cast<char*>(&fPixelColor), sizeof(_float4));

				vecPixel[iCntPixelSize] = D3DCOLOR_RGBA
											(
												static_cast<_uint>(fPixelColor.x),
												static_cast<_uint>(fPixelColor.y),
												static_cast<_uint>(fPixelColor.z),
												static_cast<_uint>(fPixelColor.w)
											);
			}

			ComPtr<ID3D11Texture2D> pWeightTexture2D{};

			D3D11_TEXTURE2D_DESC TextureDesc;
			ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

			TextureDesc.Width = iWeightTextureSize;
			TextureDesc.Height = iWeightTextureSize;
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

			SubResourceData.pSysMem = vecPixel.data();
			SubResourceData.SysMemPitch = iWeightTextureSize * 4;
			
			if (FAILED(GAMEINSTANCE->GetDeviceInfo()->CreateTexture2D(&TextureDesc, &SubResourceData, pWeightTexture2D.GetAddressOf())))
			{
				MSG_BOX("Failed to Created : Weight Texture2D!");
				InWeightBinaryFile.close();
				return E_FAIL;
			}
			ComPtr<ID3D11ShaderResourceView> pWeightSRV{};

			if (FAILED(GAMEINSTANCE->GetDeviceInfo()->CreateShaderResourceView(pWeightTexture2D.Get(), nullptr, pWeightSRV.GetAddressOf())))
			{
				MSG_BOX("Failed to Created : Weight SRV!");
				InWeightBinaryFile.close();
				return E_FAIL;
			}

			m_vecWeightDescs[iCntWeightTextures].pWeightSRV = pWeightSRV;
		}

		InWeightBinaryFile.close();
	}
#pragma endregion Load Weight Data File

	m_eShaderType = ESHADER_TYPE::ST_NORTEX;
	m_iShaderPass = 0;

	if (_IsField) {

		CRigidBody::RIGIDBODYDESC RigidBodyDesc;

		RigidBodyDesc.pOwnerObject = shared_from_this();
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.isGravity = false;
		RigidBodyDesc.pTransform = m_pTransformCom;
		RigidBodyDesc.fStaticFriction = 1.0f;
		RigidBodyDesc.fDynamicFriction = 0.0f;
		RigidBodyDesc.fRestitution = 0.0f;
		GeometryHeightField HeighFieldDesc{};
		HeighFieldDesc.vOffSetPosition = { 0.0f,0.0f,0.0f };
		HeighFieldDesc.vOffSetRotation = { 0.0f,0.0f,0.0f };
		HeighFieldDesc.strShapeTag = "HeightField";
		HeighFieldDesc.pVITerrain = m_pVIBufferCom;
		RigidBodyDesc.pGeometry = &HeighFieldDesc;
		RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ALL;
		RigidBodyDesc.eThisColFlag = COL_STATIC;
		RigidBodyDesc.eTargetColFlag = COL_PLAYER | COL_MONSTER | COL_NPC | COL_MONSTERPROJECTILE | COL_STATIC | COL_AMITOY | COL_INTERACTIONOBJ;
		m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

		m_pTransformCom->SetRigidBody(m_pRigidBody);
	}

	return S_OK;
}

void CTerrain::PriorityTick(_float _fTimeDelta)
{
}

void CTerrain::Tick(_float _fTimeDelta)
{
}

void CTerrain::LateTick(_float _fTimeDelta)
{
	if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
}

HRESULT CTerrain::Render()
{
	// 모델
	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass); //
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix()); //

	GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strBaseDiffuseTextureName); //

	_uint iNumWeightSRVs = static_cast<_uint>(m_vecWeightDescs.size());

	GAMEINSTANCE->BindRawValue("g_iNumWeightTexture", &iNumWeightSRVs, sizeof(_uint));

	for (_uint iCntWeightSRVs = 0; iCntWeightSRVs < iNumWeightSRVs; iCntWeightSRVs++)
	{
		string strConstantName = "g_WeightTexture_" + to_string(iCntWeightSRVs);
		GAMEINSTANCE->BindSRVDirect(strConstantName.c_str(), m_vecWeightDescs[iCntWeightSRVs].pWeightSRV);

		strConstantName = "g_BlendTexture_Red_" + to_string(iCntWeightSRVs);
		GAMEINSTANCE->BindSRV(strConstantName.c_str(), m_vecWeightDescs[iCntWeightSRVs].arrBlendDiffusesName[BLEND_RED]);

		strConstantName = "g_BlendTexture_Green_" + to_string(iCntWeightSRVs);
		GAMEINSTANCE->BindSRV(strConstantName.c_str(), m_vecWeightDescs[iCntWeightSRVs].arrBlendDiffusesName[BLEND_GREEN]);

		strConstantName = "g_BlendTexture_Blue_" + to_string(iCntWeightSRVs);
		GAMEINSTANCE->BindSRV(strConstantName.c_str(), m_vecWeightDescs[iCntWeightSRVs].arrBlendDiffusesName[BLEND_BLUE]);

		strConstantName = "g_BlendTexture_Alpha_" + to_string(iCntWeightSRVs);
		GAMEINSTANCE->BindSRV(strConstantName.c_str(), m_vecWeightDescs[iCntWeightSRVs].arrBlendDiffusesName[BLEND_ALPHA]);
	}

	GAMEINSTANCE->BeginShaderBuffer(m_pVIBufferCom);

	return S_OK;
}

shared_ptr<CTerrain> CTerrain::Create(ifstream& _InFileStream, _bool _IsField)
{
	shared_ptr<CTerrain> pInstance = make_shared<CTerrain>();

	if (FAILED(pInstance->Initialize(_InFileStream, _IsField)))
	{
		MSG_BOX("Failed to Created : CTerrain");
		pInstance.reset();
	}

	return pInstance;
}