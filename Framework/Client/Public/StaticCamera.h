#pragma once

#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CStaticCamera final : public CCamera
{
public:
	CStaticCamera();
	virtual ~CStaticCamera() = default;

public:
	static shared_ptr<CStaticCamera> Create();

public:
	HRESULT Initialize();
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
};

END