#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CTrap abstract : public CGameObject
{
public:
	CTrap();
	virtual ~CTrap() = default;

public:
	HRESULT Initialize();

protected:
	shared_ptr<class CModel> m_pModelCom{ nullptr };
	shared_ptr<class CRigidBody> m_pRigidBody{ nullptr };
};

END