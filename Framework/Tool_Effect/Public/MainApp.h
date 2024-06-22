#pragma once

#include "Tool_Defines.h"

BEGIN(Tool_Effect)

class CMainApp final
{
public:
	CMainApp();
	~CMainApp();

public:
	HRESULT	Initialize();
	void	Tick(_float _fTimeDelta);
	HRESULT	Render();

private:
	HRESULT	OpenLevel(LEVEL _eStartLevel);

public:
	void	Free();
};

END