#pragma once
#include "AnimToolImguiManager.h"
#include "Tool_Animation_Defines.h"

BEGIN(Tool_Animation)

class CMainAnimTool final
{
public:
	CMainAnimTool();
	~CMainAnimTool();

public:
	HRESULT Initialize();
	void Tick(_float _fTimeDelta);
	HRESULT Render();

private:
	HRESULT OpenLevel(LEVEL _eStartLevel);

public:
	void Free();

};

END