#pragma once

#include "InteractionObj.h"

BEGIN(Client)

class CCorpse : public CInteractionObj
{
public:
	CCorpse();
	virtual ~CCorpse() = default;

public:
	virtual HRESULT Initialize(_float3 _vPosition);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render();

public:
	virtual void InteractionOn();

public:
	static shared_ptr<CCorpse> Create(_float3 _vPosition);

};

END