#pragma once

namespace Engine
{
	class CModel;
	class CVITerrain;
}

namespace physx
{
	struct ENGINE_DLL Geometry abstract
	{
	protected:
		Geometry() = delete;
		Geometry(EGEOMETRY_TYPE _eType) :m_eGeometryType(_eType) {};
		EGEOMETRY_TYPE m_eGeometryType = GT_END;
	public:
		EGEOMETRY_TYPE GetType() { return m_eGeometryType; };
		const char* strShapeTag = "ShapeTag";	// 이 모양이 가질 태그
		_float3 vOffSetPosition = { 0.0f,0.0f,0.0f };	// 이 모양의 오프셋 좌표
		_float3 vOffSetRotation = { 0.0f,0.0f,0.0f };	// 이 모양의 오프셋 회전(Degree)
	};

	struct ENGINE_DLL GeometrySphere final : public Geometry
	{
		GeometrySphere() : Geometry(GT_SPHERE) {}
		float fRadius = 0.0f;	// 객체의 반지름(월드 사이즈)
	};

	struct ENGINE_DLL GeometryBox final : public Geometry
	{
		GeometryBox() : Geometry(GT_BOX) {}
		_float3 vSize = { 0.0f,0.0f,0.0f };	// 객체의 x,y,z 크기(월드 사이즈)
	};

	struct ENGINE_DLL GeometryCapsule final : public Geometry
	{
		GeometryCapsule() : Geometry(GT_CAPSULE) {}
		float fRadius = 0.0f;	// 구 반지름(월드 사이즈)
		float fHeight = 0.0f;	// 높이(월드 사이즈)
	};

	//struct ENGINE_DLL GeometryConvexMesh final : public Geometry
	//{
	//	GeometryConvexMesh() : Geometry(GT_MODEL_CONVEXMESH), pModel(nullptr) {}
	//	shared_ptr<CModel> pModel = nullptr; // 객체의 모델 컴포넌트
	//};

	struct ENGINE_DLL GeometryTriangleMesh final : public Geometry
	{
		GeometryTriangleMesh() : Geometry(GT_MODEL_TRIANGLEMESH), pModel(nullptr) {}
		shared_ptr<CModel> pModel = nullptr; // 객체의 모델 컴포넌트
	};

	struct ENGINE_DLL GeometryHeightField final : public Geometry
	{
		GeometryHeightField() : Geometry(GT_HEIGHTFILED), pVITerrain(nullptr) {}
		shared_ptr<CVITerrain> pVITerrain = nullptr; // 지형의 버퍼
	};

	inline PxFilterFlags MySimulationFilterShader(
		PxFilterObjectAttributes /* attributes0 */, PxFilterData filterData0,
		PxFilterObjectAttributes /* attributes1 */, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		PX_UNUSED(filterData0);
		PX_UNUSED(filterData1);
		PX_UNUSED(constantBlockSize);
		PX_UNUSED(constantBlock);

		PxU32 ColFlag0	= filterData0.word0;	/* A 객체의 충돌 플래그 */
		PxU32 ColEnums0	= filterData0.word1;	/* A 객체와 충돌되는 플래그들 */
		PxU32 Flags0	= filterData0.word2;	/* A 객체의 기타 플래그들, PhysX_Enums.h의 EWORD2_FLAGS 참조 */

		PxU32 ColFlag1	= filterData1.word0;	/* B 객체의 충돌 플래그 */
		PxU32 ColEnums1 = filterData1.word1;	/* B 객체와 충돌되는 플래그들 */
		PxU32 Flags1	= filterData1.word2;	/* B 객체의 기타 플래그들, PhysX_Enums.h의 EWORD2_FLAGS 참조 */

		// 두 객체가 간섭하지 않는다.
		// 또는, 두 객체 중에 누군가 충돌을 비활성화 해놓음
		if ((ColFlag0 & ColEnums1) == COL_NONE || (ColFlag1 & ColEnums0) == COL_NONE || (Flags0 & ENABLE_COLLISION) == 0 || (Flags1 & ENABLE_COLLISION) == 0)
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eSUPPRESS;
		}

		// 이 아래는 둘 다 충돌 활성화 해놓음

		pairFlags = PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_PERSISTS | PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eDETECT_DISCRETE_CONTACT;

		// 둘 다 트리거가 아니다
		// 물리적인 충돌 플래그도 추가
		if ((Flags0 & IS_TRIGGER) == 0 && (Flags1 & IS_TRIGGER) == 0)
		{
			pairFlags |= PxPairFlag::eSOLVE_CONTACT; //| PxPairFlag::eDETECT_CCD_CONTACT;
		}

		return PxFilterFlag::eDEFAULT;
	}
}
