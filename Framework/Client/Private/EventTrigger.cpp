#include "EventTrigger.h"
#include "RigidBody.h"
#include "EventMgr.h"
#include "UIMgr.h"

#include "Map.h"

CEventTrigger::CEventTrigger()
{
}

HRESULT CEventTrigger::Initialize(vector<GeometrySphere>* _Geometries)
{
	if(FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (FAILED(AddRigidBody(_Geometries)))
		return E_FAIL;


	return S_OK;
}

void CEventTrigger::LateTick(_float _fTimeDelta)
{
	if (m_isColl && m_isCollLost && ("SoundEvent_Village" == m_strCurrentLostCollTag) && ("SoundEvent_Village" == m_strCurrentCollTag))
	{
		m_isColl = false;
		m_isCollLost = false;
		m_strCurrentCollTag = "";
		m_strCurrentLostCollTag = "";
	}

	if (m_isColl) 
	{
		//Event Fire
		CEventMgr::GetInstance()->EventActive(m_strCurrentCollTag.c_str());

		if ( ("SoundEvent_Village" != m_strCurrentCollTag) && ("Healing_Event" != m_strCurrentCollTag))
		{
			m_pRigidBody->DisableCollision(m_strCurrentCollTag.c_str());
		}

		else if ("SoundEvent_Village" == m_strCurrentCollTag)
		{
			// 마을 안일 때 처리
			static_pointer_cast<CMap>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_Map")))->SetAreaType(CMap::AREA_IN_VILLAGE);
			UIMGR->SetMapTitle(LEVEL_VILLAGE);
			UIMGR->ActivateMapTitle();
		}
		m_isColl = false;
		m_strCurrentCollTag = "";
	}

	if (m_isCollLost) 
	{

		if ("SoundEvent_Village" == m_strCurrentLostCollTag)
		{
			//마을 밖으로 나갈때 처리 
			CEventMgr::GetInstance()->EventActive(m_strCurrentLostCollTag.c_str());
			static_pointer_cast<CMap>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_Map")))->SetAreaType(CMap::AREA_OUT_VILLAGE);
			UIMGR->SetMapTitle(10);
			UIMGR->ActivateMapTitle();
		}
		else if ("Healing_Event" == m_strCurrentLostCollTag)
		{
			CEventMgr::GetInstance()->EventActive(m_strCurrentLostCollTag.c_str());
		}

		m_isCollLost = false;
		m_strCurrentLostCollTag = "";
	}


#ifdef _DEBUG
	if (m_pRigidBody)
		GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG

}

void CEventTrigger::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	m_isColl = true;
	m_strCurrentCollTag = _szMyShapeTag;
}

void CEventTrigger::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CEventTrigger::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	m_isCollLost = true;
	m_strCurrentLostCollTag = _szMyShapeTag;
}

HRESULT CEventTrigger::AddRigidBody( vector<GeometrySphere>* _Geometries)
{
	//m_pRigidBody = CRigidBody::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo());

//	m_pRigidBody->DisableCollision()

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	//==본체==
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.0f;

	//if (!m_pRigidBody)
	//	return E_FAIL;

	GeometrySphere SphereDesc;

	_int iCount = 0;

	//Create Event Collider Shape
	for (auto& iter : *_Geometries) {
		
		SphereDesc.fRadius = iter.fRadius;
		SphereDesc.strShapeTag = iter.strShapeTag;
		SphereDesc.vOffSetPosition = iter.vOffSetPosition;
		SphereDesc.vOffSetRotation = iter.vOffSetRotation;

		RigidBodyDesc.eThisColFlag = COL_TRIGGER;
		RigidBodyDesc.eTargetColFlag = COL_PLAYER | COL_AMITOY;
		RigidBodyDesc.pGeometry = &SphereDesc;

		/*First Desc*/
		if (0 == iCount) {
			m_pRigidBody = GAMEINSTANCE->CloneRigidBody((void*)&RigidBodyDesc);
		}
		else 
		{
			m_pRigidBody->CreateShape(&RigidBodyDesc);
		}

		++iCount;
	}

	return S_OK;
}

shared_ptr<CEventTrigger> CEventTrigger::Create(vector<GeometrySphere>* _Geometries)
{
	shared_ptr<CEventTrigger> pTrigger = make_shared<CEventTrigger>();

	if (FAILED(pTrigger->Initialize(_Geometries)))
		MSG_BOX("Failed to Create : CEventTrigger");

	return pTrigger;
}
