#pragma once
#include "Monster.h"

BEGIN(Engine)
class CModel;
class CRigidBody;
END

BEGIN(Client)

class CPuppet : public CMonster
{
public:
	struct Puppet_Desc
	{
		_float3 vMyPos;
	};

public:
	CPuppet();
	virtual ~CPuppet()  override;

public:
	virtual HRESULT Initialize(shared_ptr<Puppet_Desc> _pPuppetDesc);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render()  override;

public:
	virtual HRESULT RenderShadow() override;

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;

//private:
//	shared_ptr<CModel>			m_pModelCom = { nullptr };
//	shared_ptr<CRigidBody>		m_pRigidBody = { nullptr };

private:
	HRESULT AddRigidBody(string _strModelKey);		// RigidBody Ãß°¡

public:
	static shared_ptr<CPuppet> Create(shared_ptr<Puppet_Desc> _pPuppetDesc);
};

END