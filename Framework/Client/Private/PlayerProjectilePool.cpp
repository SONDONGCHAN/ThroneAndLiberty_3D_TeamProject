#include "PlayerProjectilePool.h"
#include "PlayerProjectile.h"
#include "PlayerArrowBasic.h"
#include "PlayerArrowElectric.h"
#include "PlayerArrowPuriousFire.h"
#include "PlayerArrowCurve.h"
#include "PlayerMagicBullet.h"
#include "PlayerSpellFireBall.h"
#include "PlayerSpellIceSpear.h"
#include "PlayerSpellFireBlast.h"
#include "PlayerSpellLightnigJudgment.h"
#include "PlayerSpellMeteor.h"
#include "PlayerRope.h"
#include "Player.h"



CPlayerProjectilePool::CPlayerProjectilePool()
{
}

HRESULT CPlayerProjectilePool::Initialize(shared_ptr<CPlayer> _pPlayer)
{
	m_pPlayer = _pPlayer;

	m_pProjectileNum[PLAYER_ARROW_BASIC]		= 20;
	m_pProjectileNum[PLAYER_ARROW_ELECTRIC]		= 2;
	m_pProjectileNum[PLAYER_ARROW_FURIOUSFIRE]	= 24;
	m_pProjectileNum[PLAYER_ARROW_CURVE]		= 30;

	m_pProjectileNum[PLAYER_MAGIC_BULLET]	 = 3;
	m_pProjectileNum[PLAYER_SPELL_FIREBALL]  = 5;
	m_pProjectileNum[PLAYER_SPELL_ICESPEAR]  = 3;
	m_pProjectileNum[PLAYER_SPELL_FIREBLAST] = 3;
	m_pProjectileNum[PLAYER_SPELL_LIGHTINGJUDGMENT] = 2;
	m_pProjectileNum[PLAYER_SPELL_METEOR] = 1;

	m_pProjectileNum[PLAYER_ROPE_ACTION_BULLET] = 1;

	for (_uint i = PTTYPE::PLAYER_ARROW_BASIC; i < PTTYPE::END_TYPE; ++i)
	{
		for (_uint j = 0; j < m_pProjectileNum[i] ; ++j)
		{
			shared_ptr<CPlayerProjectile> pProjectile;

			switch (i)
			{
			case PLAYER_ARROW_BASIC:
				pProjectile = CPlayerArrowBasic::Create(m_pPlayer);
				break;
			case PLAYER_ARROW_ELECTRIC:
				pProjectile = CPlayerArrowElectric::Create();
				break;
			case PLAYER_ARROW_FURIOUSFIRE:
				pProjectile = CPlayerArrowPuriousFire::Create();
				break;
			case PLAYER_ARROW_CURVE:
				pProjectile = CPlayerArrowCurve::Create();
				break;
			case PLAYER_MAGIC_BULLET:
				pProjectile = CPlayerMagicBullet::Create(m_pPlayer);
				break;
			case PLAYER_SPELL_FIREBALL:
				pProjectile = CPlayerSpellFireBall::Create();
				break;
			case PLAYER_SPELL_ICESPEAR:
				pProjectile = CPlayerSpellIceSpear::Create();
				break;
			case PLAYER_SPELL_FIREBLAST:
				pProjectile = CPlayerSpellFireBlast::Create();
				break;
			case PLAYER_SPELL_LIGHTINGJUDGMENT:
				pProjectile = CPlayerSpellLightnigJudgment::Create();
				break;
			case PLAYER_SPELL_METEOR:
				pProjectile = CPlayerSpellMeteor::Create();
				break;
			case PLAYER_ROPE_ACTION_BULLET:
				pProjectile = CPlayerRope::Create(m_pPlayer);
				break;
			case END_TYPE:
				break;

			default:
				break;
			}

			if (pProjectile)
			{
				GAMEINSTANCE->AddObject(LEVEL_STATIC, TEXT("Layer_Player_Projectile"), pProjectile);
				m_pProjectiles[i].push_back(pProjectile);
			}
		}
	}
	return S_OK;
}

void CPlayerProjectilePool::AwakeProjectile(PTTYPE _eProjectileType, _float3 _vMyStartPos, _float3 _vTargetPos, PlayerProjAbnormal _ePlayerProjAbnormal, _bool isDistinguish, _uint _iIndex)
{
	for (auto& pProjectile : m_pProjectiles[_eProjectileType])
	{
		if (!pProjectile->IsEnabled())
		{
			pProjectile->EnableProjectile(_vMyStartPos, _vTargetPos, _ePlayerProjAbnormal, isDistinguish, _iIndex);
			break;
		}
	}
}

shared_ptr<class CPlayerProjectile> CPlayerProjectilePool::GetProjectile(PTTYPE _eProjectileType, _uint _iIndex)
{
	if (_eProjectileType >= END_TYPE)
		return nullptr;

	if((m_pProjectiles[_eProjectileType]).size() <= _iIndex )
		return nullptr;

	return (m_pProjectiles[_eProjectileType])[_iIndex]; 
}

shared_ptr<CPlayerProjectilePool> CPlayerProjectilePool::Create(shared_ptr<CPlayer> _pPlayer)
{
	shared_ptr<CPlayerProjectilePool> pProjectilePool = make_shared<CPlayerProjectilePool>();

	if (FAILED(pProjectilePool->Initialize(_pPlayer)))
		MSG_BOX("Failed to Create : CPlayerProjectilePool");

	return pProjectilePool;
}
