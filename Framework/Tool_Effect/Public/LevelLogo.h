#pragma once

#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Tool_Effect)

class CLevelLogo final : public CLevel
{
public:
	CLevelLogo();
	virtual ~CLevelLogo() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float _fTimeDelta);
	virtual void LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT ReadyLayerBackGround(const wstring& strLayerTag);
	HRESULT ReadyLight();
	HRESULT ReadyCamera(const wstring& strLayerTag);


public:
	static shared_ptr<CLevelLogo> Create();

};

END