#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)

class CTexture;

END

BEGIN(Client)

class CTestLogo : public CGameObject
{
public:
	CTestLogo();
	virtual ~CTestLogo() = default;

public:
	virtual HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private:
	string m_strKeyName = "";
	shared_ptr<CTexture> m_pLogoTex = nullptr;

public:
	static shared_ptr<CTestLogo> Create();

};

END