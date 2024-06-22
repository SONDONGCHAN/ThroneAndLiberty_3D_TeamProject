#pragma once

#include "GameObject.h"

class CMessage : public CGameObject
{

public:
	CMessage();
	virtual ~CMessage() = default;

public:
	virtual HRESULT Initialize( _uint _iChestNum);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render();

private:
	_float2 m_vSize = _float2();

public:
	static shared_ptr<CMessage> Create(_uint _iChestNum);
};

