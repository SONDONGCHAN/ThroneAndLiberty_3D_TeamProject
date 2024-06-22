#pragma once

#include "Engine_Defines.h"
#include "PxEventCallback.h"

BEGIN(Engine)

class CRigidBody;

class CPhysXMgr
{
public:
	CPhysXMgr();
	~CPhysXMgr();

public:
	static shared_ptr<CPhysXMgr> Create();

public:
	HRESULT Initialize();
	void Tick(float _fTimeDelta);

public:
	PxPhysics*	GetPhysics() { return m_pPhysics; }
	PxScene*	GetScene() { return m_pScene; }
	void SetSimultation(bool _isSimulation) { m_isSimulation = _isSimulation; }
	
	shared_ptr<CRigidBody> CloneRigidBody(void* _pArg);

public:
	void Free();

private:
	// Foundation을 생성하는데 필요한 변수
	PxDefaultAllocator m_DefaultAllocator;
	PxDefaultErrorCallback m_DefaultErrorCallback;
	PxFoundation* m_pFoundation = nullptr;

	// PxFoundation이 있어야 m_Physics를 생성할 수 있다.
	PxPhysics* m_pPhysics = nullptr;

	// GPU 사용을 위한 객체
	PxCudaContextManager* m_pCudaContextManager = nullptr;

	// CPU 리소스를 효율적으로 공유할 수 있도록 하기 위해 구현
	PxDefaultCpuDispatcher* m_pDispatcher = nullptr;

	// 시뮬레이션을 위한 새로운 공간
	PxScene* m_pScene = nullptr;

	// 시뮬레이션 이벤트를 수신하기 위해 사용자가 구현
	CPxEventCallback* m_pPxEventCallback = nullptr;

#ifdef _PVD
	PxPvd* m_pPvd = nullptr;
#endif

	bool m_isSimulation = false;

	shared_ptr<CRigidBody> m_pProtoRigidBody = nullptr;

	float m_fTrackPosition{ 0.0f };
	float m_fTime60{ 1.0f / 60.0f };
};

END