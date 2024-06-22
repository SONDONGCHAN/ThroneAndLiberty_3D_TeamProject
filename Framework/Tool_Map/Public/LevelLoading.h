#pragma once
#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Tool_Map)
class CLoader;

class CLevelLoading : public CLevel
{
private:
	CLevelLoading();
	DESTRUCTOR(CLevelLoading)

public:
	virtual HRESULT Initialize(LEVEL _eNextLevel);
	virtual void Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_UI();

private:
	shared_ptr<CLoader> m_pLoader{};
	LEVEL m_eNextLevel{ LEVEL_END };

public:
	static shared_ptr<CLevelLoading> Create(LEVEL _eNextLevel);
};

END