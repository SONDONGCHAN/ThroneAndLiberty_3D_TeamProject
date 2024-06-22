#pragma once

#ifdef _DEBUG
#pragma comment(lib, "PhysXd/PhysX_64.lib")
#pragma comment(lib, "PhysXd/PhysXCommon_64.lib")
#pragma comment(lib, "PhysXd/PhysXCooking_64.lib")
#pragma comment(lib, "PhysXd/PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXd/PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysXd/PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "PhysXd/SimulationController_static_64.lib")
#pragma comment(lib, "PhysXd/SceneQuery_static_64.lib")
#pragma comment(lib, "PhysXd/PhysXTask_static_64.lib")
#pragma comment(lib, "PhysXd/LowLevel_static_64.lib")
#pragma comment(lib, "PhysXd/LowLevelAABB_static_64.lib")
#pragma comment(lib, "PhysXd/LowLevelDynamics_static_64.lib")

#else

#pragma comment(lib, "PhysX/PhysX_64.lib")
#pragma comment(lib, "PhysX/PhysXCommon_64.lib")
#pragma comment(lib, "PhysX/PhysXCooking_64.lib")
#pragma comment(lib, "PhysX/PhysXFoundation_64.lib")
#pragma comment(lib, "PhysX/PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysX/PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "PhysX/SimulationController_static_64.lib")
#pragma comment(lib, "PhysX/SceneQuery_static_64.lib")
#pragma comment(lib, "PhysX/PhysXTask_static_64.lib")
#pragma comment(lib, "PhysX/LowLevel_static_64.lib")
#pragma comment(lib, "PhysX/LowLevelAABB_static_64.lib")
#pragma comment(lib, "PhysX/LowLevelDynamics_static_64.lib")
#pragma comment(lib, "PhysX/PhysXPvdSDK_static_64.lib")

#endif // _DEBUG

#include "PxPhysicsAPI.h"

using namespace physx;

#include "PhysX_Enums.h"
#include "PhysX_Struct.h"
#include "PhysX_Converter.h"

