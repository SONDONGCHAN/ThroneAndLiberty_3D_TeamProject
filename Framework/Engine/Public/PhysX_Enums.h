#pragma once

namespace physx
{
	enum ECOLLISIONFLAG		/* WORD0, WORD1*/
	{
		COL_NONE			= 0,		/* 무엇과도 충돌 하지 않는다. */
		COL_STATIC			= 1 << 0,	/* 지형과 건물 같은 Static 객체 */
		COL_PLAYER			= 1 << 1,
		COL_PLAYERWEAPON	= 1 << 2,
		COL_PLAYERSKILL		= 1 << 3,
		COL_PLAYERPROJECTILE	= 1 << 4,
		COL_PLAYERPARRYING		= 1 << 5,
		COL_CHECKBOX			= 1 << 6,
		COL_ROPE				= 1 << 7,

		COL_NPC = 1 << 9,						// NPC HitBox
		COL_MONSTER			= 1 << 10,			// 몬스터 HitBox
		COL_MONSTERWEAPON	= 1 << 11,			// 몬스터 무기
		COL_MONSTERSKILL		= 1 << 12,
		COL_MONSTERPROJECTILE	= 1 << 13,	// Projectile

		COL_TRIGGER = 1 << 14,

		COL_TRAP = 1 << 15,
		COL_AMITOY = 1 << 19,					// 아미토이
		COL_AMITOY_ATTACK = 1 << 20,			// 아미토이 공격

		COL_INTERACTIONOBJ = 1 << 21,
		/* 충돌할 플래그 추가할 것 */
		COL_END = 1 << 30
	};

	enum EWORD2_FLAGS /* PxFilterData.word2 */
	{
		NONE_WORD = 0,
		ENABLE_COLLISION = 1 << 0,
		IS_TRIGGER = 1 << 1

	};

	enum EGEOMETRY_TYPE
	{
		GT_SPHERE,					// 구 모양
		GT_BOX,						// OBB 모양
		GT_CAPSULE,					// 알약 모양(주로, 플레이어, 몬스터 등)
		//GT_MODEL_CONVEXMESH,		// 간단한 메쉬 모양(동적, 정적)
		GT_MODEL_TRIANGLEMESH,		// 정교한 메쉬 모양(정적)
		GT_HEIGHTFILED,				// 높이맵(지형)
		GT_END
	};
}