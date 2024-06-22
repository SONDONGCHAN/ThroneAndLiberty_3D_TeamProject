#include "pch.h"
#include "ParsingColliders.h"
#include "RigidBody.h"
#include <fstream>
#include <filesystem>
#include <iostream>

CParsingColliders::CParsingColliders()
{
}

CParsingColliders::~CParsingColliders()
{
	Clear();
}

shared_ptr<CParsingColliders> CParsingColliders::Create(const char* _szFilePath)
{
	shared_ptr<CParsingColliders> pInstance = make_shared<CParsingColliders>();

	if (FAILED(pInstance->Initialize(_szFilePath)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CParsingColliders::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CParsingColliders::Initialize(const char* _szFilePath)
{
	// 파일 경로
	filesystem::path LoadFilePath = _szFilePath;

	ifstream inFile(LoadFilePath, ios::in | ios::binary);

	if (inFile.is_open())
	{
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;

		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.isGravity = false;
		RigidBodyDesc.isInitCol = true;
		RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ALL;

		RigidBodyDesc.fStaticFriction = 1.0f;
		RigidBodyDesc.fDynamicFriction = 0.0f;
		RigidBodyDesc.fRestitution = 0.0f;

		RigidBodyDesc.eThisColFlag = COL_STATIC;
		RigidBodyDesc.eTargetColFlag = COL_PLAYER | COL_MONSTER | COL_NPC | COL_MONSTERPROJECTILE | COL_STATIC | COL_ROPE | COL_AMITOY | COL_INTERACTIONOBJ;

		int iColliderNum = 0;
		inFile.read(reinterpret_cast<char*>(&iColliderNum), sizeof(int));

		Geometry* pGeomertry{ nullptr };
		GeometrySphere GeoSph{};
		GeometryBox GeoBox{};
		GeometryCapsule GeoCap{};

		for (int i = 0; i < iColliderNum; i++)
		{
			// World Position
			_float3 vPos;
			inFile.read(reinterpret_cast<char*>(&vPos), sizeof(_float3));

			// EGEOMETRY_TYPE
			EGEOMETRY_TYPE GeometryType{};
			inFile.read(reinterpret_cast<char*>(&GeometryType), sizeof(EGEOMETRY_TYPE));

			// OffSetRotation
			_float3 vOffSetRot;
			inFile.read(reinterpret_cast<char*>(&vOffSetRot), sizeof(_float3));

			// Geometry Data
			switch (GeometryType)
			{
			case GT_SPHERE:
			{
				float fRadius = 0.0f;
				inFile.read(reinterpret_cast<char*>(&fRadius), sizeof(float));
				GeoSph.fRadius = fRadius;
				pGeomertry = &GeoSph;
			}
			break;
			case GT_BOX:
			{
				_float3 vSize;
				inFile.read(reinterpret_cast<char*>(&vSize), sizeof(_float3));
				GeoBox.vSize = vSize;
				pGeomertry = &GeoBox;
			}
			break;
			case GT_CAPSULE:
			{
				float fRadius = 0.0f;
				float fHeight = 0.0f;
				inFile.read(reinterpret_cast<char*>(&fRadius), sizeof(float));
				inFile.read(reinterpret_cast<char*>(&fHeight), sizeof(float));
				GeoCap.fRadius = fRadius;
				GeoCap.fHeight = fHeight;
				pGeomertry = &GeoCap;
			}
			break;
			}

			pGeomertry->vOffSetRotation = vOffSetRot;

			shared_ptr<ColliderData> pColData = make_shared<ColliderData>();
			pColData->Initialize(nullptr);
			shared_ptr<CTransform> pTransform = pColData->GetTransform();
			pTransform->SetState(CTransform::STATE_POSITION, vPos);

			pGeomertry->strShapeTag = "Static_Collider";
			RigidBodyDesc.pTransform = pTransform;
			RigidBodyDesc.pOwnerObject = pColData;
			RigidBodyDesc.pGeometry = pGeomertry;

			pColData->m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

			m_lstColliders.emplace_back(pColData);
		}

		inFile.close();
	}
	else
	{
		MessageBox(nullptr, L"File Open Failed", L"System", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

void CParsingColliders::LateTick(_float _fTimeDelta)
{
#ifdef _DEBUG
	for (auto pColliderData : m_lstColliders)
	{
		if(pColliderData->m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(pColliderData->m_pRigidBody);
	}
#endif // _DEBUG
}

void CParsingColliders::Clear()
{
	m_lstColliders.clear();
}
