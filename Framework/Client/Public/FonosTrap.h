#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "EffectMgr.h"

BEGIN(Client)

class CFonosTrap : public CGameObject
{
public:
	enum TRAP_TYPE
	{
		TRAP_BACK,
		TRAP_SLOW,
		TRAP_SHOCK,
		TRAP_MOVE,
		TRAP_END
	};

public:
	CFonosTrap();
	virtual ~CFonosTrap();

public:
	HRESULT Initialize(TRAP_TYPE _eTrapType, Geometry* _pGeometry, float _fRadius, _float3 _vPos, _float3 _vLook);
	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;

protected:
	shared_ptr<CModel> m_pModelCom = nullptr;
	shared_ptr<CRigidBody> m_pRigidBody = nullptr;

protected:
	TRAP_TYPE	m_eTrapType = TRAP_END;
	_float		m_fRadius	= 1.f;
};

END