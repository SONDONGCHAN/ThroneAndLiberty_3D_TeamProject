#include "EffectParticle.h"

CEffectParticle::CEffectParticle()
{
}

CEffectParticle::CEffectParticle(const CEffectParticle& rhs)
{
}

CEffectParticle::~CEffectParticle()
{
}

HRESULT CEffectParticle::Initialize(EffectDesc* pDesc)
{
	return E_NOTIMPL;
}

void CEffectParticle::PriorityTick(_float _fTimeDelta)
{
}

void CEffectParticle::Tick(_float _fTimeDelta)
{
}

void CEffectParticle::LateTick(_float _fTimeDelta)
{
}

HRESULT CEffectParticle::Render()
{
	return E_NOTIMPL;
}

shared_ptr<CEffectParticle> CEffectParticle::Create(EffectDesc* pDesc)
{
	return shared_ptr<CEffectParticle>();
}
