#pragma once
#include "Tool_Defines.h"

BEGIN(Tool_Map)

class CMainApp
{
private:
	CMainApp();
	~CMainApp();

public:
	HRESULT Initialize();
	void Tick(_float _fTimeDelta);
	HRESULT Render();

public:
	HRESULT OpenLevel(LEVEL _eStartLevel);

public:
	static shared_ptr<CMainApp> Create();
};

END