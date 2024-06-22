#pragma once

#include "InteractionObj.h"

class CLegendFlower : public CInteractionObj
{
public:
	CLegendFlower();
	virtual ~CLegendFlower() = default;

public:
	virtual HRESULT Initialize(_float3 _vPosition);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render();

public:
	virtual void InteractionOn();

private:
	_bool			m_bEffectOn = false;
	_bool			m_bDisappearOn = false;
	_float			m_fDisappearTime = 1.f;

public:
	static shared_ptr<CLegendFlower> Create(_float3 _vPos);

};

