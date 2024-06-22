#pragma once
#include "Tool_Animation_Defines.h"
#include "Level.h"

BEGIN(Tool_Animation)

class CLevelAnimToolLogo : public CLevel
{
public:
	CLevelAnimToolLogo();
	virtual ~CLevelAnimToolLogo() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float _fTimeDelta);
	virtual void LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT ReadyLayerBackGround(const wstring& strLayerTag);
	HRESULT ReadyCamera(const wstring& strLayerTag);

public:
	static shared_ptr<CLevelAnimToolLogo> Create();

};

END