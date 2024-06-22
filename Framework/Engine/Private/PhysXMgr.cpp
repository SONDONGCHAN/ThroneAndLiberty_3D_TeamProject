#include "PhysXMgr.h"
#include "GameInstance.h"
#include "RigidBody.h"

CPhysXMgr::CPhysXMgr()
{
}

CPhysXMgr::~CPhysXMgr()
{
}

shared_ptr<CPhysXMgr> CPhysXMgr::Create()
{
	shared_ptr<CPhysXMgr> pInstance = make_shared<CPhysXMgr>();

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CPhysXMgr::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CPhysXMgr::Initialize()
{
	// Foundation Create
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocator, m_DefaultErrorCallback);
	if (!m_pFoundation) { MessageBox(nullptr, L"PxCreateFoundation Failed", L"CPhysxMgr::Initialize", MB_OK); return E_FAIL; }

#ifdef _PVD
	m_pPvd = physx::PxCreatePvd(*m_pFoundation);
	if (!m_pPvd) { MessageBox(nullptr, L"PxCreatePvd Failed", L"CPhysxMgr::Initialize", MB_OK); return E_FAIL; }
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	if (!m_pPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL))
		return E_FAIL;

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd);
#else
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true);
#endif // _PVD

	if (!m_pPhysics) { MessageBox(nullptr, L"PxCreatePhysics Failed", L"CPhysxMgr::Initialize", MB_OK); return E_FAIL; }

	// CudaContextManager Create
	PxCudaContextManagerDesc cudaContextManagerDesc;
	m_pCudaContextManager = PxCreateCudaContextManager(*m_pFoundation, cudaContextManagerDesc, PxGetProfilerCallback());
	if (m_pCudaContextManager)
	{
		if (!m_pCudaContextManager->contextIsValid())
			PX_RELEASE(m_pCudaContextManager);
	}

	// Dispatcher Create
	m_pDispatcher = PxDefaultCpuDispatcherCreate(3);
	//m_pDispatcher = PxDefaultCpuDispatcherCreate(4, nullptr, PxDefaultCpuDispatcherWaitForWorkMode::eYIELD_THREAD);
	if (!m_pDispatcher) { MessageBox(nullptr, L"PxDefaultCpuDispatcherCreate Failed", L"CPhysxMgr::Initialize", MB_OK); return E_FAIL; }

	// EventCallback Create
	m_pPxEventCallback = CPxEventCallback::Create();
	if (!m_pPxEventCallback) { MessageBox(nullptr, L"CPxEventCallback::Create Failed", L"CPhysxMgr::Initialize", MB_OK); return E_FAIL; }

	// Scene Setting
	PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f * 1.5f, 0.0f);		// Gravity Set
	sceneDesc.cpuDispatcher = m_pDispatcher;					// Dispatcher Set
	//sceneDesc.filterShader = PxDefaultSimulationFilterShader;	// Filter Set
	sceneDesc.filterShader = MySimulationFilterShader;			// Filter Set, 내가 만든거
	sceneDesc.simulationEventCallback = m_pPxEventCallback;		// 내가 만든거
	sceneDesc.solverBatchSize = 256;
	sceneDesc.solverArticulationBatchSize = 32;
	sceneDesc.contactReportStreamBufferSize = 16384;
	sceneDesc.contactPairSlabSize = 512;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;

	// GPU Setting
	sceneDesc.cudaContextManager = m_pCudaContextManager;		// CudaContextManager Set
	sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_FRICTION_EVERY_ITERATION;
	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	sceneDesc.gpuMaxNumPartitions = 8;
	sceneDesc.gpuMaxNumStaticPartitions = 255;
	
	if (!sceneDesc.isValid())
		return E_FAIL;

	// Scene Create
	m_pScene = m_pPhysics->createScene(sceneDesc);
	if (!m_pScene) { MessageBox(nullptr, L"createScene Failed", L"CPhysxMgr::Initialize", MB_OK); return E_FAIL; }

#ifdef _PVD
	m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 0.5f);
	m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
#endif // _PVD

	m_pProtoRigidBody = CRigidBody::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo());
	if (!m_pProtoRigidBody) { MessageBox(nullptr, L"m_pProtoRigidBody nullptr", L"CPhysxMgr::Initialize", MB_OK); return E_FAIL; }

	/*PxRigidStatic* groundPlane = PxCreatePlane(*m_pPhysics, PxPlane(0.0f, 1.0f, 0.0f, 0.0f), *m_pPhysics->createMaterial(0.5f, 0.5f, 0.0f));
	m_pScene->addActor(*groundPlane);*/

	return S_OK;
}

void CPhysXMgr::Tick(float _fTimeDelta)
{
	if (m_isSimulation)
	{
		/*m_fTimeAcc += _fTimeDelta;
		if (m_fTimeAcc < m_fTime60)
			return;

		m_fTimeAcc -= m_fTime60;*/
		m_pScene->simulate(m_fTime60);
		m_pScene->fetchResults(true);
	}
}

shared_ptr<CRigidBody> CPhysXMgr::CloneRigidBody(void* _pArg)
{
	return static_pointer_cast<CRigidBody>(m_pProtoRigidBody->Clone(_pArg));
}

void CPhysXMgr::Free()
{
	PX_RELEASE(m_pScene);
	PX_RELEASE(m_pPxEventCallback);
	PX_RELEASE(m_pDispatcher);
	PX_RELEASE(m_pPhysics);
#ifdef _PVD
	if (m_pPvd)
	{
		PxPvdTransport* transport = m_pPvd->getTransport();
		transport->disconnect();
		PX_RELEASE(m_pPvd);
		PX_RELEASE(transport);
	}
#endif // _PVD
	PX_RELEASE(m_pCudaContextManager);
	PX_RELEASE(m_pFoundation);
}
