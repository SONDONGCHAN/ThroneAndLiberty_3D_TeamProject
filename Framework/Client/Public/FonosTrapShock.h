#pragma once

#include "FonosTrap.h"

BEGIN(Client)

class CFonosTrapShock final : public CFonosTrap
{

public:
	CFonosTrapShock();
	virtual ~CFonosTrapShock();

public:
	static shared_ptr<CFonosTrapShock> Create(TRAP_TYPE _eTrapType, _float3 _vPos, _float3 _vLook);

public:
	HRESULT Initialize(TRAP_TYPE _eTrapType, _float3 _vPos, _float3 _vLook);
	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
};

END