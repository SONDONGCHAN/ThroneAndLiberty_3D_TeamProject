#pragma once
#include "Tool_Animation_Defines.h"
#include "Level.h"

BEGIN(Tool_Animation)

class CLevelAnimToolEdit : public CLevel
{
public:
	CLevelAnimToolEdit();
	virtual ~CLevelAnimToolEdit() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float _fTimeDelta);
	virtual void LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT ReadyLight();
	HRESULT ReadyLayerBackGround(const wstring& strLayerTag);
	HRESULT ReadyCamera(const wstring& strLayerTag);
	HRESULT ReadyLayerAnimObject(const wstring& strLayerTag);

public:
	static shared_ptr<CLevelAnimToolEdit> Create();
};

END