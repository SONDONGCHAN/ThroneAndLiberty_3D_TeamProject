#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CPxEventCallback : public PxSimulationEventCallback
{
private:
	CPxEventCallback();
	virtual ~CPxEventCallback() = default;

public:
	static CPxEventCallback* Create();
	void release();
	
public:
	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
	virtual void onWake(PxActor** actors, PxU32 count) override;
	virtual void onSleep(PxActor** actors, PxU32 count) override;
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
	virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override;
};

END