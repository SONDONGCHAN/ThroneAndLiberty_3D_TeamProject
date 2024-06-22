 #include "Map.h"

#include <fstream>

#include "Terrain.h"
#include "VITerrain.h"
#include "EnvironmentObject.h"

struct OBJ_DESC
{
	vector<_float4x4> vecMatWorld{};
	vector<_float3> vecCenterPoint{};
	vector<_float> vecRadius{};
};

CMap::CMap()
{
}

CMap::~CMap()
{
}

HRESULT CMap::Initialize(const string& _strMapFilePath, _bool _IsField)
{
	//m_IsField = _IsField;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	ifstream InBinaryFile(_strMapFilePath, ios::in | ios::binary);
	if (InBinaryFile.is_open())
	{
		// 해당 Map의 Name 읽기.
		char szMapName[MAX_PATH]{};
		InBinaryFile.read(szMapName, sizeof(szMapName));

		m_strMapName = szMapName;

		// 해당 Map의 World Matrix 읽기.
		_float4x4 matMapWorld{};
		InBinaryFile.read(reinterpret_cast<char*>(&matMapWorld), sizeof(_float4x4));

		m_pTransformCom->SetWorldMatrix(matMapWorld);

		_bool bIsTerrain{};
		InBinaryFile.read(reinterpret_cast<char*>(&bIsTerrain), sizeof(_bool));
		if (true == bIsTerrain)
		{
			m_IsField = true;

			string strTerrainDataFilePath = "..\\Bin\\DataFiles\\TerrainData";

			_char szTerrainDataFileName[MAX_PATH] = "";
			InBinaryFile.read(szTerrainDataFileName, sizeof(szTerrainDataFileName));
			string strTerrainDataFileName = szTerrainDataFileName;

			strTerrainDataFilePath = strTerrainDataFilePath + "\\" + strTerrainDataFileName + ".dat";

			ifstream InTerrainBinaryFile(strTerrainDataFilePath, ios::in | ios::binary);
			if (InTerrainBinaryFile.is_open())
			{
				m_pTerrain = CTerrain::Create(InTerrainBinaryFile, m_IsField);
				InTerrainBinaryFile.close();
			}
		}
		else
			m_IsField = false;

		

		//////////////////////////////////////////////
		array<map<string, OBJ_DESC>, 3> arrObjectDescGroup{};
		
		//map<string, OBJ_DESC> mapObjectDesc{};

		

		//map<string, OBJ_DESC> mapInObjectDesc{};
		//map<string, OBJ_DESC> mapOutObjectDesc{};
		//////////////////////////////////////////////


		// 해당 Map의 Area 수를 저장.
		_uint iNumAreas{};
		InBinaryFile.read(reinterpret_cast<char*>(&iNumAreas), sizeof(_uint));

		// 각 Area에 대한 Name과 World Matrix 저장.
		for (_uint iCntAreas = 0; iCntAreas < iNumAreas; iCntAreas++)
		{
			// Area의 Name
			_char szAreaName[MAX_PATH] = "";
			InBinaryFile.read(szAreaName, sizeof(szAreaName));
			string strAreaName = szAreaName;

			AREA eAreaType{};

			if (strAreaName.find("Kastleton") != string::npos)
				eAreaType = AREA_IN_VILLAGE;
			else if (strAreaName.find("Urstella") != string::npos)
				eAreaType = AREA_OUT_VILLAGE;
			else
				eAreaType = AREA_DEFAULT;

			// Area의 World Matrix
			_float4x4 matAreaWorld{};
			InBinaryFile.read(reinterpret_cast<char*>(&matAreaWorld), sizeof(_float4x4));

			//shared_ptr<CArea> pArea = CArea::Create(szAreaName, matAreaWorld);
			//m_pMap->AddArea(pArea);

			// Area에 속한 Model 수
			_uint iNumModels{};
			InBinaryFile.read(reinterpret_cast<char*>(&iNumModels), sizeof(_uint));

			for (_uint iCntModels = 0; iCntModels < iNumModels; iCntModels++)
			{
				string strModelKey{};
				// Model의 Key
				_char szModelKey[MAX_PATH] = "";
				InBinaryFile.read(szModelKey, sizeof(szModelKey));
				strModelKey = szModelKey;
				
				auto iter = arrObjectDescGroup[eAreaType].find(strModelKey);
				if (arrObjectDescGroup[eAreaType].end() == iter)
				{
					arrObjectDescGroup[eAreaType].emplace(strModelKey, OBJ_DESC{});
				}

				// 해당 Model을 공유하는 Objects의 수
				_uint iNumObjects{};
				InBinaryFile.read(reinterpret_cast<char*>(&iNumObjects), sizeof(_uint));

				for (_uint iCntObjects = 0; iCntObjects < iNumObjects; iCntObjects++)
				{
					_float4x4 matObjectWorld{};
					InBinaryFile.read(reinterpret_cast<char*>(&matObjectWorld), sizeof(_float4x4));
					arrObjectDescGroup[eAreaType][szModelKey].vecMatWorld.push_back(matObjectWorld);

					_float3 vCenterPoint{};
					InBinaryFile.read(reinterpret_cast<char*>(&vCenterPoint), sizeof(_float3));
					arrObjectDescGroup[eAreaType][szModelKey].vecCenterPoint.push_back(vCenterPoint);

					_float fRadius{};
					InBinaryFile.read(reinterpret_cast<char*>(&fRadius), sizeof(_float));
					arrObjectDescGroup[eAreaType][szModelKey].vecRadius.push_back(fRadius);
				}


			}



			_uint iNumGroupObjects{};
			InBinaryFile.read(reinterpret_cast<char*>(&iNumGroupObjects), sizeof(_uint));

			for (_uint iCntGroupObjects = 0; iCntGroupObjects < iNumGroupObjects; iCntGroupObjects++)
			{
				// MapTool용이므로, 버림.
				_char szGroupObjectName[MAX_PATH] = "";
				InBinaryFile.read(szGroupObjectName, sizeof(szGroupObjectName));

				// 연산용으므로 보관
				_float4x4 matGroupObjectWorld{};
				InBinaryFile.read(reinterpret_cast<char*>(&matGroupObjectWorld), sizeof(_float4x4));


				_uint iNumMemObjects{};
				InBinaryFile.read(reinterpret_cast<char*>(&iNumMemObjects), sizeof(_uint));

				//vector<CGroupObject::MEMBER_DESC> vecMemberObjectDesc(iNumMemObjects);

				for (_uint iCntMemObjects = 0; iCntMemObjects < iNumMemObjects; iCntMemObjects++)
				{
					//CGroupObject::MEMBER_DESC MemberObjectDesc{};
					// 이름
					string strMemberObjectModelKey{};
					_char szMemberObjectModelKey[MAX_PATH] = "";
					InBinaryFile.read(szMemberObjectModelKey, sizeof(szMemberObjectModelKey));
					strMemberObjectModelKey = szMemberObjectModelKey;

					auto iter = arrObjectDescGroup[eAreaType].find(strMemberObjectModelKey);
					if (arrObjectDescGroup[eAreaType].end() == iter)
					{
						arrObjectDescGroup[eAreaType].emplace(strMemberObjectModelKey, OBJ_DESC{});
					}

					// 월드행렬
					_float4x4 matMemverObjectWorld{};
					InBinaryFile.read(reinterpret_cast<char*>(&matMemverObjectWorld), sizeof(_float4x4));
					arrObjectDescGroup[eAreaType][strMemberObjectModelKey].vecMatWorld.push_back(matMemverObjectWorld * matGroupObjectWorld);

					// 중점
					_float3 vCenterPoint{};
					InBinaryFile.read(reinterpret_cast<char*>(&vCenterPoint), sizeof(_float3));
					arrObjectDescGroup[eAreaType][strMemberObjectModelKey].vecCenterPoint.push_back(vCenterPoint);


					// 반지름
					_float fRadius{};
					InBinaryFile.read(reinterpret_cast<char*>(&fRadius), sizeof(_float));
					arrObjectDescGroup[eAreaType][strMemberObjectModelKey].vecRadius.push_back(fRadius);
				}
			}














		}


		for (auto mapObjectDesc : arrObjectDescGroup)
		{
			
		}
			

		for (_uint iCntAreas = 0; iCntAreas < AREA_END; iCntAreas++)
		{
			for (auto ObjectDesc : arrObjectDescGroup[iCntAreas])
			{
				shared_ptr<CEnvironmentObject> pObject = CEnvironmentObject::Create(ObjectDesc.first, ObjectDesc.second.vecMatWorld, ObjectDesc.second.vecCenterPoint, ObjectDesc.second.vecRadius);
				arrObjectGroups[iCntAreas].push_back(pObject);
			}
			
		}

		//////////////////////////////////////////////
		





		InBinaryFile.close();
	}
	else
	{
		MSG_BOX("Failed to Read : Binary File");
		return E_FAIL;
	}

	return S_OK;
}

void CMap::PriorityTick(_float _fTimeDelta)
{
	if (m_eCurrentArea != m_eChangetArea)
		m_eCurrentArea = m_eChangetArea;

	if (m_IsField) {
		if (nullptr != m_pTerrain)
			m_pTerrain->PriorityTick(_fTimeDelta);

		//for (auto ObjectGroup : arrObjectGroups)
		//{
		//	for (auto pObject : ObjectGroup)
		//		pObject->PriorityTick(_fTimeDelta);
		//}

		for (const auto& pObject : arrObjectGroups[AREA_DEFAULT])
			pObject->PriorityTick(_fTimeDelta);

		if (AREA_DEFAULT != m_eCurrentArea)
		{
			for (const auto& pObject : arrObjectGroups[m_eCurrentArea])
				pObject->PriorityTick(_fTimeDelta);
		}
	}
	else {

		for (const auto& pObject : arrObjectGroups[AREA_DEFAULT])
			pObject->PriorityTick(_fTimeDelta);

	}
	
}

void CMap::Tick(_float _fTimeDelta)
{

	if (m_IsField) {
		if (nullptr != m_pTerrain)
			m_pTerrain->Tick(_fTimeDelta);

		//for (auto ObjectGroup : arrObjectGroups)
		//{
		//	for (auto pObject : ObjectGroup)
		//		pObject->Tick(_fTimeDelta);
		//}

		for (const auto& pObject : arrObjectGroups[AREA_DEFAULT])
			pObject->Tick(_fTimeDelta);

		if (AREA_DEFAULT != m_eCurrentArea)
		{
			for (const auto& pObject : arrObjectGroups[m_eCurrentArea])
				pObject->Tick(_fTimeDelta);
		}
	}
	else {

		for (const auto& pObject : arrObjectGroups[AREA_DEFAULT])
			pObject->Tick(_fTimeDelta);


	}
	
}

void CMap::LateTick(_float _fTimeDelta)
{
	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

	if (m_IsField) {
		if (nullptr != m_pTerrain)
			m_pTerrain->LateTick(_fTimeDelta);

		//for (auto ObjectGroup : arrObjectGroups)
		//{
		//	for (auto pObject : ObjectGroup)
		//		pObject->LateTick(_fTimeDelta);
		//}

		for (const auto& pObject : arrObjectGroups[AREA_DEFAULT])
			pObject->LateTick(_fTimeDelta);

		if (AREA_DEFAULT != m_eCurrentArea)
		{
			for (const auto& pObject : arrObjectGroups[m_eCurrentArea])
				pObject->LateTick(_fTimeDelta);
		}
	}
	else {
		for (const auto& pObject : arrObjectGroups[AREA_DEFAULT])
			pObject->LateTick(_fTimeDelta);


	}
}

HRESULT CMap::Render()
{
	if (m_IsField)
	{
		switch (m_eCurrentArea)
		{
		case Client::CMap::AREA_DEFAULT:
		{
			for (const auto& pObject : arrObjectGroups[AREA_IN_VILLAGE])
				pObject->ClearInstancingWorldMatrix();
			for (const auto& pObject : arrObjectGroups[AREA_OUT_VILLAGE])
				pObject->ClearInstancingWorldMatrix();
		}
		break;

		case Client::CMap::AREA_IN_VILLAGE:
		{
			for (const auto& pObject : arrObjectGroups[AREA_OUT_VILLAGE])
				pObject->ClearInstancingWorldMatrix();
		}
		break;

		case Client::CMap::AREA_OUT_VILLAGE:
		{
			for (const auto& pObject : arrObjectGroups[AREA_IN_VILLAGE])
				pObject->ClearInstancingWorldMatrix();
		}
		break;
		}
	}
	return S_OK;
}

shared_ptr<CMap> CMap::Create(const string& _strMapFilePath, _bool _IsField)
{
	shared_ptr<CMap> pInstance = make_shared<CMap>();

	if (FAILED(pInstance->Initialize(_strMapFilePath, _IsField)))
	{
		MSG_BOX("Failed to Created : CMap");
		pInstance.reset();
	}

	return pInstance;
}


