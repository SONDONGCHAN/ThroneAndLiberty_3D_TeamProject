#include "RigidBody.h"
#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"
#include "VITerrain.h"
#include "Transform.h"

#include "VIPhysX.h"



CRigidBody::CRigidBody(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CRigidBody::CRigidBody(const shared_ptr<CRigidBody> _pOrigin)
	: CComponent(_pOrigin)
{
}

CRigidBody::~CRigidBody()
{
    Free();
}

shared_ptr<CRigidBody> CRigidBody::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
{
    shared_ptr<CRigidBody> pInstance = make_shared<CRigidBody>(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CRigidBody::Create", MB_OK);
        pInstance.reset();
        return nullptr;
    }

    return pInstance;
}

shared_ptr<CComponent> CRigidBody::Clone(void* _pArg)
{
    shared_ptr<CComponent> pClone = make_shared<CRigidBody>(static_pointer_cast<CRigidBody>(shared_from_this()));

    if (FAILED(pClone->InitializeClone(_pArg)))
    {
        MessageBox(nullptr, L"InitializeClone Failed", L"CRigidBody::Clone", MB_OK);
        pClone.reset();
        return nullptr;
    }

    return pClone;
}

HRESULT CRigidBody::Initialize()
{
#ifdef _DEBUG
    

#endif // _DEBUG

    return S_OK;
}

HRESULT CRigidBody::InitializeClone(void* _pArg)
{
    if (!_pArg)
    {
        MessageBox(nullptr, L"Argument nullptr", L"CRigidBody::InitializeClone", MB_OK);
        return E_FAIL;
    }

    shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
    PxPhysics* pPhysx = pGameInstance->GetPhysics();

	RIGIDBODYDESC* pDesc = (RIGIDBODYDESC*)_pArg;

    if (FAILED(CheckArgument(pDesc)))
    {
        MessageBox(nullptr, L"CheckArgument Failed", L"CRigidBody::InitializeClone", MB_OK);
        return E_FAIL;
    }

    m_isStatic  = pDesc->isStatic;
    m_pScene = pGameInstance->GetScene();

    if (FAILED(CreateActor(pDesc)))
    {
        MessageBox(nullptr, L"CreateActor Failed", L"CRigidBody::InitializeClone", MB_OK);
        return E_FAIL;
    }

	if (FAILED(CreateShape(pDesc, false)))
    {
        MessageBox(nullptr, L"CreateShape Failed", L"CRigidBody::InitializeClone", MB_OK);
        return E_FAIL;
    }

    if (false == m_pScene->addActor(*m_pActor))
    {
        MessageBox(nullptr, L"addActor Failed", L"CRigidBody::InitializeClone", MB_OK);
        return E_FAIL;
    }

	return S_OK;
}

HRESULT CRigidBody::CreateShape(RIGIDBODYDESC* _pDesc, bool _isShared)
{
    shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
    PxPhysics* pPhysx = pGameInstance->GetPhysics();

    m_pMaterial = pPhysx->createMaterial(_pDesc->fStaticFriction, _pDesc->fDynamicFriction, _pDesc->fRestitution);
    if (!m_pMaterial)
    {
        MessageBox(nullptr, L"createMaterial Failed", L"CRigidBody::InitializeClone", MB_OK);
        return E_FAIL;
    }
	//m_pMaterial->setFlag(PxMaterialFlag::eDISABLE_STRONG_FRICTION, true);

	if (FAILED(CreateGeometry(_pDesc, _isShared)))
    {
        MessageBox(nullptr, L"CreateGeometry Failed", L"CRigidBody::InitializeClone", MB_OK);
        return E_FAIL;
    }

    return S_OK;
}

_float3 CRigidBody::GetPosition()
{
    return ToFloat3(m_pActor->getGlobalPose().p);;
}

_quaternion CRigidBody::GetQuat()
{
	return ToQuat(m_pActor->getGlobalPose().q);
}

HRESULT CRigidBody::ChangeShape(const char* _szShapeTag)
{
    PxShape* pShape = Find_Shape(_szShapeTag);
	if (!pShape) { return E_FAIL; }

    // 기존 Shape들 제거
	UINT iNumShapes = m_pActor->getNbShapes();
	vector<PxShape*> Shapes;
    Shapes.reserve(iNumShapes);
	m_pActor->getShapes(&Shapes[0], iNumShapes);
    for (UINT i = 0; i < iNumShapes; i++)
    {
        m_pActor->detachShape(*Shapes[i]);
    }

    // 다른 Shape로 변경
    m_pActor->attachShape(*pShape);

    return S_OK;
}

HRESULT CRigidBody::AttachShape(const char* _szShapeTag)
{
	PxShape* pShape = Find_Shape(_szShapeTag);
	if (!pShape) { return E_FAIL; }

	bool AlreadyAttached = false;

	UINT iNumShapes = m_pActor->getNbShapes();
	vector<PxShape*> Shapes;
	Shapes.reserve(iNumShapes);
	m_pActor->getShapes(&Shapes[0], iNumShapes);
	for (UINT i = 0; i < iNumShapes; i++)
	{
		if (Shapes[i] == pShape)
			AlreadyAttached = true;
	}

	// 이미 붙어있는 shape가 아니면 shape를 덧붙인다
	if (false == AlreadyAttached)
		m_pActor->attachShape(*pShape);

	return S_OK;
}

void CRigidBody::SetPosition(const _float3& _vPosition, bool _isAutowake)
{
    PxTransform vTransform = PxTransform(ToPxVec3(_vPosition), ToPxQuat(GetQuat()));
    if (vTransform.isValid())
    {
        m_pActor->setGlobalPose(vTransform, _isAutowake);
    }
}

void CRigidBody::SetRotation(const _quaternion& _vQuat, bool _isAutowake)
{
    PxTransform vTransform = PxTransform(ToPxVec3(GetPosition()), ToPxQuat(_vQuat));
    if (vTransform.isValid())
    {
        m_pActor->setGlobalPose(vTransform, _isAutowake);
    }
}

void CRigidBody::SetGlobalPos(const _float3& _vPosition, const _quaternion& _Quat, bool _isAutowake)
{
    PxTransform vTransform = ToPxTransform(_vPosition, _Quat);
    if (vTransform.isValid())
    {
		m_pActor->setGlobalPose(vTransform, _isAutowake);
    }
}

void CRigidBody::EnableCollision(const char* _szShapeTag)
{
    PxShape* pShape = Find_Shape(_szShapeTag);
    if (pShape)
    {
        PxFilterData FilterData = pShape->getSimulationFilterData();
        m_pActor->detachShape(*pShape);
        FilterData.word2 |= ENABLE_COLLISION;
        pShape->setSimulationFilterData(FilterData);
        m_pActor->attachShape(*pShape);
    }
}

void CRigidBody::DisableCollision(const char* _szShapeTag)
{
    PxShape* pShape = Find_Shape(_szShapeTag);
    if (pShape)
    {
        PxFilterData FilterData = pShape->getSimulationFilterData();
        m_pActor->detachShape(*pShape);
        FilterData.word2 &= ~(PxU32)ENABLE_COLLISION;
        pShape->setSimulationFilterData(FilterData);
        m_pActor->attachShape(*pShape);
    }
}

void CRigidBody::WakeUp()
{
    if (PxActorType::eRIGID_DYNAMIC == m_pActor->getType())
    {
        static_cast<PxRigidDynamic*>(m_pActor)->wakeUp();
    }
}

_float3 CRigidBody::GetLinearVelocity()
{
    if (PxActorType::eRIGID_DYNAMIC == m_pActor->getType())
    {
        return ToFloat3(static_cast<PxRigidDynamic*>(m_pActor)->getLinearVelocity());
    }
    return { 0.0f,0.0f,0.0f };
}

_float3 CRigidBody::GetAngularVelocity()
{
    if (PxActorType::eRIGID_DYNAMIC == m_pActor->getType())
    {
        return ToFloat3(static_cast<PxRigidDynamic*>(m_pActor)->getAngularVelocity());
    }
    return { 0.0f,0.0f,0.0f };
}

void CRigidBody::SetLinearVelocity(const PxVec3& _vLinVel, bool _isAutowake)
{
	if (PxActorType::eRIGID_DYNAMIC == m_pActor->getType() && !(m_pActor->getActorFlags() & PxActorFlag::eDISABLE_SIMULATION))
	{
		static_cast<PxRigidDynamic*>(m_pActor)->setLinearVelocity(_vLinVel, _isAutowake);
	}
}

void CRigidBody::SetAngularVelocity(const PxVec3& _vAngVel, bool _isAutowake)
{
	if (PxActorType::eRIGID_DYNAMIC == m_pActor->getType() && !(m_pActor->getActorFlags() & PxActorFlag::eDISABLE_SIMULATION))
	{
		static_cast<PxRigidDynamic*>(m_pActor)->setAngularVelocity(_vAngVel, _isAutowake);
	}
}

void CRigidBody::SetMaxLinearVelocity(float _fMaxLinVel)
{
    if (PxActorType::eRIGID_DYNAMIC == m_pActor->getType())
    {
        static_cast<PxRigidDynamic*>(m_pActor)->setMaxLinearVelocity(_fMaxLinVel);
    }
}

void CRigidBody::SetLockFlags(UINT _eLockFlags, bool _isLock)
{
    if (_eLockFlags && m_pActor)
    {
        PxRigidDynamic* pRigidDynamic = m_pActor->is<PxRigidDynamic>();
        if (pRigidDynamic)
        {
            if (LOCK_POS_X & _eLockFlags)
            {
                pRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, _isLock);
            }
            if (LOCK_POS_Y & _eLockFlags)
            {
                pRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, _isLock);
            }
            if (LOCK_POS_Z & _eLockFlags)
            {
                pRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, _isLock);
            }
            if (LOCK_ROT_X & _eLockFlags)
            {
                pRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, _isLock);
            }
            if (LOCK_ROT_Y & _eLockFlags)
            {
                pRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, _isLock);
            }
            if (LOCK_ROT_Z & _eLockFlags)
            {
                pRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, _isLock);
            }
        }
    }
}

void CRigidBody::AddForce(const PxVec3& _vForce, PxForceMode::Enum _eMode, bool _isAutowake)
{
    if (PxActorType::eRIGID_DYNAMIC == m_pActor->getType() && !(m_pActor->getActorFlags() & PxActorFlag::eDISABLE_SIMULATION))
    {
        static_cast<PxRigidDynamic*>(m_pActor)->addForce(_vForce, _eMode, _isAutowake);
    }
}

void CRigidBody::SetMass(float _fMass)
{
    if (PxActorType::eRIGID_DYNAMIC == m_pActor->getType())
    {
        static_cast<PxRigidDynamic*>(m_pActor)->setMass(_fMass);
    }
}

void CRigidBody::GravityOn()
{
    if (PxActorType::eRIGID_DYNAMIC == m_pActor->getType())
    {
        static_cast<PxRigidDynamic*>(m_pActor)->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
    }
}

void CRigidBody::GravityOff()
{
    if (PxActorType::eRIGID_DYNAMIC == m_pActor->getType()) 
    {
        static_cast<PxRigidDynamic*>(m_pActor)->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
    }
}

void CRigidBody::SimulationOn()
{
    m_pActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
}

void CRigidBody::SimulationOff()
{
    m_pActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
}

HRESULT CRigidBody::CheckArgument(RIGIDBODYDESC* _pDesc)
{
	if (nullptr == _pDesc->pOwnerObject.lock()) { MessageBox(nullptr, L"OwnerObject nullptr", L"CRigidBody::CheckArgument", MB_OK); return E_FAIL; }
	if (nullptr == _pDesc->pGeometry) { MessageBox(nullptr, L"Geometry nullptr", L"CRigidBody::CheckArgument", MB_OK); return E_FAIL; }

    return S_OK;
}

HRESULT CRigidBody::CreateActor(RIGIDBODYDESC* _pDesc)
{
    shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
    PxPhysics* pPhysx = pGameInstance->GetPhysics();

	PxTransform Transform = ToPxTransform(_pDesc->pTransform->GetState(CTransform::STATE_POSITION), _pDesc->pTransform->GetQuat());

    if (m_isStatic)
    {
        m_pActor = pPhysx->createRigidStatic(Transform);
    }
    else
    {
        m_pActor = pPhysx->createRigidDynamic(Transform);
        if (!_pDesc->isGravity)
        {
            m_pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
        }
        //static_cast<PxRigidBody*>(m_pActor)->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		SetLockFlags(_pDesc->eLock_Flag, true);
		_float3 RotDegree = _pDesc->pGeometry->vOffSetRotation;
        _quaternion Quat = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(RotDegree.x), XMConvertToRadians(RotDegree.y), XMConvertToRadians(RotDegree.z));
        PxTransform OffSetTransform(ToPxVec3(_pDesc->pGeometry->vOffSetPosition), ToPxQuat(Quat));
        static_cast<PxRigidDynamic*>(m_pActor)->setCMassLocalPose(OffSetTransform);
    }
	
#ifdef _PVD
    m_pActor->setActorFlag(PxActorFlag::eVISUALIZATION, true);
#endif // _PVD


    return S_OK;
}

HRESULT CRigidBody::CreateGeometry(RIGIDBODYDESC* _pDesc, bool _isShared)
{
    shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
    PxPhysics* pPhysx = pGameInstance->GetPhysics();

	// ShapeFlag
    PxShapeFlags eShapeFlags = PxShapeFlag::eSIMULATION_SHAPE;


#ifdef _PVD
    eShapeFlags |= PxShapeFlag::eVISUALIZATION;
#endif // _PVD

	// PxFilterData
	// Collision Data Setting
	// word0 : 내 충돌값
	// word1 : 내가 충돌할 값들
	// word2 : 충돌 처리를 할 것인가 
	PxFilterData FilterData;
	FilterData.word0 = _pDesc->eThisColFlag;
	FilterData.word1 = _pDesc->eTargetColFlag;
	FilterData.word2 = _pDesc->isInitCol ? ENABLE_COLLISION : NONE_WORD;
    if (_pDesc->isTrigger)
        FilterData.word2 |= IS_TRIGGER;
    

    // Create Geometry and Shape
    Geometry* pGeometry= _pDesc->pGeometry;
	
	PxShape* pShape = nullptr;

    _float3 RotDegree = (pGeometry)->vOffSetRotation;
    _quaternion Quat = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(RotDegree.x), XMConvertToRadians(RotDegree.y), XMConvertToRadians(RotDegree.z));
    PxTransform OffSetTransform(ToPxVec3(pGeometry->vOffSetPosition), ToPxQuat(Quat));

	switch (pGeometry->GetType())
	{
	case GT_SPHERE:
	{
		GeometrySphere* SphereGeometry = static_cast<GeometrySphere*>(pGeometry);
		pShape = pPhysx->createShape(PxSphereGeometry(SphereGeometry->fRadius), *m_pMaterial, _isShared, eShapeFlags);
		CreateBuffer(pShape, SphereGeometry->fRadius * 2.0f, SphereGeometry->fRadius * 2.0f, SphereGeometry->fRadius * 2.0f, SphereGeometry->vOffSetPosition, Quat);
	}
	break;
	case GT_BOX:
	{
		GeometryBox* BoxGeometry = static_cast<GeometryBox*>(pGeometry);
		pShape = pPhysx->createShape(PxBoxGeometry(BoxGeometry->vSize.x * 0.5f, BoxGeometry->vSize.y * 0.5f, BoxGeometry->vSize.z * 0.5f), *m_pMaterial, _isShared, eShapeFlags);
		CreateBuffer(pShape, BoxGeometry->vSize.x, BoxGeometry->vSize.y, BoxGeometry->vSize.z, BoxGeometry->vOffSetPosition, Quat);
	}
	break;
	case GT_CAPSULE:
	{
		GeometryCapsule* CapsuleGeometry = static_cast<GeometryCapsule*>(pGeometry);
		pShape = pPhysx->createShape(PxCapsuleGeometry(CapsuleGeometry->fRadius, CapsuleGeometry->fHeight * 0.5f), *m_pMaterial, _isShared, eShapeFlags);
		CreateBuffer(pShape, CapsuleGeometry->fRadius * 2.0f + CapsuleGeometry->fHeight, CapsuleGeometry->fRadius * 2.0f, CapsuleGeometry->fRadius * 2.0f, CapsuleGeometry->vOffSetPosition, Quat);
	}
	break;
	/*case GT_MODEL_CONVEXMESH:
	{
    
        
	}
	break;*/
	case GT_MODEL_TRIANGLEMESH:
	{
        GeometryTriangleMesh* TriangleMeshGeometry = static_cast<GeometryTriangleMesh*>(pGeometry);
		_float4x4 WorldMatrix = _pDesc->pTransform->GetWorldMatrix();
        _float3 vScale;
        _quaternion Quat;
        _float3 vPos;
        WorldMatrix.Decompose(vScale, Quat, vPos);
        _float4x4 ScaleMat = XMMatrixScalingFromVector(vScale);
        shared_ptr<CModel> pModel = TriangleMeshGeometry->pModel;
		UINT iNumMesh = pModel->GetNumMeshes();
		const vector<shared_ptr<CMesh>>& vecMesh = pModel->GetMeshes();
		vector<_float3> ConvertPos;
        
        for (UINT i = 0; i < iNumMesh; i++)
        {
            ConvertPos.clear();

			PxTriangleMeshDesc meshDesc;
            vector<_float3>* pVertices = vecMesh[i]->GetVerticesPos();
            vector<_uint>* pIndices = vecMesh[i]->GetIndices();
            UINT iNumVertice = (UINT)(*pVertices).size();
            ConvertPos.reserve(iNumVertice);
            for (UINT j = 0; j < iNumVertice; j++)
            {
				ConvertPos.emplace_back(XMVector3TransformCoord((*pVertices)[j], ScaleMat));
            }

			meshDesc.points.count       = iNumVertice;
			meshDesc.points.data        = ConvertPos.data();
			meshDesc.points.stride      = sizeof(_float3);
			meshDesc.triangles.count    = vecMesh[i]->GetFaceNum();
			meshDesc.triangles.data     = (*pIndices).data();
			meshDesc.triangles.stride   = 3 * sizeof(UINT);

            PxTolerancesScale scale;
            PxCookingParams params(scale);

            PxDefaultMemoryOutputStream outBuffer;
			if (!PxCookTriangleMesh(params, meshDesc, outBuffer)) { MessageBox(nullptr, L"CookTriangleMesh Failed", L"CRigidBody::CreateGeometry", MB_OK); return E_FAIL; }
			
            PxDefaultMemoryInputData inputBuffer(outBuffer.getData(), outBuffer.getSize());
			PxTriangleMesh* pTriMesh = pPhysx->createTriangleMesh(inputBuffer);
			if (!pTriMesh) { MessageBox(nullptr, L"pTriMesh nullptr", L"CRigidBody::CreateGeometry", MB_OK); return E_FAIL; }

            PxTriangleMeshGeometry MeshGeometry = PxTriangleMeshGeometry(pTriMesh);
			pShape = pPhysx->createShape(MeshGeometry, *m_pMaterial, true, eShapeFlags);
			if (!pShape) { MessageBox(nullptr, L"pShape nullptr", L"CRigidBody::CreateGeometry", MB_OK); return E_FAIL; }

			pShape->setSimulationFilterData(FilterData);
			pShape->setLocalPose(OffSetTransform);

			shared_ptr<COLLISIONDATA> pUserData = make_shared<COLLISIONDATA>();
			pUserData->pGameObject = _pDesc->pOwnerObject;
			pUserData->eColFlag = _pDesc->eThisColFlag;
			pUserData->szShapeTag = pGeometry->strShapeTag;
            pUserData->pArg = _pDesc->pArg;
			pShape->userData = pUserData.get();

			m_pActor->attachShape(*pShape);

			m_Shapes.insert({ pGeometry->strShapeTag ,{pShape,pUserData} });
		}

		return S_OK;
	}
	break;
	case GT_HEIGHTFILED:
	{
		GeometryHeightField* HeightFieldGeometry = static_cast<GeometryHeightField*>(pGeometry);
		shared_ptr<CVITerrain> pVIBuffer = HeightFieldGeometry->pVITerrain;
		PxHeightFieldDesc PxHeightFieldDesc;

		PxHeightFieldDesc.nbRows = pVIBuffer->GetNumX();
		PxHeightFieldDesc.nbColumns = pVIBuffer->GetNumZ();

        const vector<_float3>* pVertices = pVIBuffer->GetVerticesPos();

		PxHeightFieldSample* pSample = new PxHeightFieldSample[PxHeightFieldDesc.nbRows * PxHeightFieldDesc.nbColumns];
		for (UINT z = 0; z < PxHeightFieldDesc.nbColumns; z++)
		{
			for (UINT x = 0; x < PxHeightFieldDesc.nbRows; x++)
			{
                UINT index = x + PxHeightFieldDesc.nbRows * z;
                UINT PxIndex = z + PxHeightFieldDesc.nbColumns * x;
				pSample[PxIndex].height = PxI16((*pVertices)[index].y * 32.767f);
			}
		}
		PxHeightFieldDesc.samples.data = pSample;
		PxHeightFieldDesc.samples.stride = sizeof(PxHeightFieldSample);
         
		PxHeightField* pHeightField = PxCreateHeightField(PxHeightFieldDesc);
		Safe_Delete_Array(pSample);

		PxHeightFieldGeometry PxGeometry(
			pHeightField, PxMeshGeometryFlags(),
			1.0f / 32.767f, 1.0f, 1.0f);

		pShape = pPhysx->createShape(PxGeometry, *m_pMaterial, _isShared, eShapeFlags);
        PX_RELEASE(pHeightField);
	}
	break;
	}

	if (!pShape) { MessageBox(nullptr, L"pShape nullptr", L"CRigidBody::CreateGeometry", MB_OK); return E_FAIL; }

	pShape->setSimulationFilterData(FilterData);
	pShape->setLocalPose(OffSetTransform);

    shared_ptr<COLLISIONDATA> pUserData = make_shared<COLLISIONDATA>();
    pUserData->pGameObject = _pDesc->pOwnerObject;
    pUserData->eColFlag = _pDesc->eThisColFlag;
    pUserData->szShapeTag = pGeometry->strShapeTag;
    pUserData->pArg = _pDesc->pArg;

    pShape->userData = pUserData.get();

	m_pActor->attachShape(*pShape);

	m_Shapes.insert({ pGeometry->strShapeTag ,{pShape,pUserData} });

	return S_OK;
}

PxShape* CRigidBody::Find_Shape(const char* _szShapeTag)
{
    auto iter = m_Shapes.find(_szShapeTag);
	if (m_Shapes.end() == iter)
        return nullptr;

	return (*iter).second.first;
}


HRESULT CRigidBody::Render()
{
    GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_CUBE, 1);
	GAMEINSTANCE->BindWVPMatrixPerspective(ToFloat4x4(m_pActor->getGlobalPose()));

	UINT iNumShape = (UINT)m_lstVIBuffer.size();
	for (auto& Pair : m_lstVIBuffer)
	{
		_color vColor{};
        PxU32 Word2 = Pair.first->getSimulationFilterData().word2;
		bool EnableCol = (Word2 & ENABLE_COLLISION) != 0;
        bool isTrigger = (Word2 & IS_TRIGGER) != 0;

        if (EnableCol && isTrigger)
            vColor = { 0.458823f,0.976471f,0.301961f,1.0f };    // 녹색
        //     else if (isTrigger)
                 //vColor = { 0.752941f,0.752941f,0.752941f,1.0f };    // 회색
        else if (EnableCol)
            vColor = { 1.0f,0.992157f,0.333333f,1.0f };         // 노란색
        //else
        //    vColor = { 0.0f,0.0f,0.0f,1.0f };                   // 검정색
        else
            continue;

        GAMEINSTANCE->BindRawValue("g_RGBColor", &vColor, sizeof(XMFLOAT4));
  
		if (FAILED(GAMEINSTANCE->BeginShaderBuffer(Pair.second)))
		{
			MessageBox(nullptr, L"RigidBody Render Failed", L"CRigidBody::Render", MB_OK);
			return E_FAIL;
		}
	}

    return S_OK;
}

HRESULT CRigidBody::CreateBuffer(PxShape* _pShape, float _fSizeX, float _fSizeY, float _fSizeZ, const _float3& _vOffsetPosition, const _quaternion& _Quat)
{
    CVIPhysX::DESC Desc{};
    Desc._vSize = { _fSizeX,_fSizeY ,_fSizeZ };
    Desc.vOffSetPosition = _vOffsetPosition;
    Desc.vOffSetQuat = _Quat;

	shared_ptr<CVIPhysX> pVIBuffer = CVIPhysX::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), &Desc);
	if (nullptr == pVIBuffer)
        return E_FAIL;

	m_lstVIBuffer.push_back({ _pShape,pVIBuffer });

	return S_OK;
}


void CRigidBody::Free()
{
    m_lstVIBuffer.clear();

	if (m_pActor)
	{
		for (auto& pPair : m_Shapes)
		{
			pPair.second.first->userData = nullptr;
			m_pActor->detachShape(*pPair.second.first);
		}
		m_pScene->removeActor(*m_pActor);
		PX_RELEASE(m_pActor);
	}
	m_Shapes.clear();
}
