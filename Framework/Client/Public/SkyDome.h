#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CModel;

END

BEGIN(Client)

class CSkyDome : public CGameObject
{
public:
	CSkyDome();
	virtual ~CSkyDome() = default;

public:
	virtual HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render()  override;

private:
	shared_ptr<CModel> m_pSkyModel = nullptr;

public:
	static shared_ptr<CSkyDome> Create();

};

END