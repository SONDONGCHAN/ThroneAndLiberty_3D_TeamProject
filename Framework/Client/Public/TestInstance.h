#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CInstancingModel;
END

BEGIN(Client)

class CTestInstance : public CGameObject
{
public:
	CTestInstance();
	virtual ~CTestInstance();

public:
	virtual HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private:
	string m_strOriginModelKey = "";
	shared_ptr<CInstancingModel> m_pModel = nullptr;

public:
	static shared_ptr<CTestInstance> Create();

};

END