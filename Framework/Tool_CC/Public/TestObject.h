#pragma once

#include "Tool_CC_Define.h"
#include "GameObject.h"

class CTestObject final: public CGameObject
{
public:
	CTestObject();
	~CTestObject();

public:
	static shared_ptr<CTestObject> Create();

public:
	HRESULT Initialize();
	virtual void PriorityTick(float _fTimeDelta);
	virtual void Tick(float _fTimeDelta);
	virtual void LateTick(float _fTimeDelta);
	virtual HRESULT Render();

};

