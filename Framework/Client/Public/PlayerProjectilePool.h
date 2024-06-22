#pragma once
#include "Client_Defines.h"
#include "PlayerProjectile.h"

BEGIN(Client)

class CPlayer;

class CPlayerProjectilePool
{
public:
	enum PTTYPE 
	{
		PLAYER_ARROW_BASIC,
		PLAYER_ARROW_ELECTRIC,
		PLAYER_ARROW_FURIOUSFIRE,
		PLAYER_ARROW_CURVE,

		PLAYER_MAGIC_BULLET,

		PLAYER_SPELL_FIREBALL,
		PLAYER_SPELL_ICESPEAR,
		PLAYER_SPELL_FIREBLAST,
		PLAYER_SPELL_LIGHTINGJUDGMENT,
		PLAYER_SPELL_METEOR,

		PLAYER_ROPE_ACTION_BULLET,

		END_TYPE
	};

public:
	CPlayerProjectilePool();
	~CPlayerProjectilePool() = default;

public:
	HRESULT Initialize(shared_ptr<CPlayer> _pPlayer);

public:
	void AwakeProjectile(PTTYPE _eProjectileType, _float3 _vMyStartPos, _float3 _vTargetPos, PlayerProjAbnormal _ePlayerProjAbnormal = PLAYER_PJ_A_NORMAL, _bool isDistinguish = false, _uint _iIndex = 0);
	shared_ptr<class CPlayerProjectile> GetProjectile(PTTYPE _eProjectileType, _uint _iIndex);


private:
	vector<shared_ptr<class CPlayerProjectile>> m_pProjectiles[PTTYPE::END_TYPE];
	_uint	m_pProjectileNum[PTTYPE::END_TYPE];
	weak_ptr<CPlayer> m_pPlayer; 

public:
	static shared_ptr<CPlayerProjectilePool> Create(shared_ptr<CPlayer> _pPlayer);
};

END