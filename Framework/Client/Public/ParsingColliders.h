#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRigidBody;
END

BEGIN(Client)

class CParsingColliders final : public CGameObject
{
private:
	struct ColliderData final :public CGameObject
	{
		shared_ptr<CRigidBody> m_pRigidBody = nullptr;
	};

public:
	CParsingColliders();
	~CParsingColliders();

public:
	static shared_ptr<CParsingColliders> Create(const char* _szFilePath);

public:
	HRESULT Initialize(const char* _szFilePath);
	virtual void LateTick(_float _fTimeDelta);
	void Clear();

private:
	list<shared_ptr<ColliderData>> m_lstColliders;
};

END