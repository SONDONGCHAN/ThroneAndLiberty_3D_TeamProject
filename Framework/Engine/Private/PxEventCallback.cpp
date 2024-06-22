#include "PxEventCallback.h"
#include "GameInstance.h"
#include "LevelMgr.h"
#include "GameObject.h"

CPxEventCallback::CPxEventCallback()
{
}

CPxEventCallback* CPxEventCallback::Create()
{
	return new CPxEventCallback();
}

void CPxEventCallback::release()
{
	delete this;
}

void CPxEventCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
}

void CPxEventCallback::onWake(PxActor** actors, PxU32 count)
{
}

void CPxEventCallback::onSleep(PxActor** actors, PxU32 count)
{
}

void CPxEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	if (GAMEINSTANCE->isLoadingLevel())
		return;

	for (UINT i = 0; i < nbPairs; i++)
	{
		const PxContactPair& current = pairs[i];

		if (nullptr == current.shapes[0]->userData || nullptr == current.shapes[1]->userData) { continue; }

		COLLISIONDATA* pData0 = (COLLISIONDATA*)(current.shapes[0]->userData);
		COLLISIONDATA* pData1 = (COLLISIONDATA*)(current.shapes[1]->userData);

		weak_ptr<CGameObject> pGameObject0 = pData0->pGameObject.lock();
		weak_ptr<CGameObject> pGameObject1 = pData1->pGameObject.lock();
		if (nullptr == pGameObject0.lock() || nullptr == pGameObject1.lock()) { continue; }

		if (current.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			pGameObject0.lock()->OnContactFound(*pData1, pData0->szShapeTag);
			pGameObject1.lock()->OnContactFound(*pData0, pData1->szShapeTag);
			if (!pGameObject0.lock()->IsActive() || !pGameObject1.lock()->IsActive())
			{
				pGameObject0.lock()->OnContactLost(*pData1, pData0->szShapeTag);
				pGameObject1.lock()->OnContactLost(*pData0, pData1->szShapeTag);
			}
		}

		if (current.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			pGameObject0.lock()->OnContactPersist(*pData1, pData0->szShapeTag);
			pGameObject1.lock()->OnContactPersist(*pData0, pData1->szShapeTag);
			if (!pGameObject0.lock()->IsActive() || !pGameObject1.lock()->IsActive())
			{
				pGameObject0.lock()->OnContactLost(*pData1, pData0->szShapeTag);
				pGameObject1.lock()->OnContactLost(*pData0, pData1->szShapeTag);
			}
		}

		if (current.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			pGameObject0.lock()->OnContactLost(*pData1, pData0->szShapeTag);
			pGameObject1.lock()->OnContactLost(*pData0, pData1->szShapeTag);
		}
	}
}

void CPxEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
}

void CPxEventCallback::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
}
