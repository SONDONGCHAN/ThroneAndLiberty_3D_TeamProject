#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevelLogo final : public CLevel
{
public:
	CLevelLogo();
	virtual ~CLevelLogo();

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float _fTimeDelta);
	virtual void LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT ReadyLayerBackGround(const wstring& strLayerTag);

public:
	static shared_ptr<CLevelLogo> Create();
};

END