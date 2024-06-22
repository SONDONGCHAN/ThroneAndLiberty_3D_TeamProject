#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CRigidBody;
END

BEGIN(Client)

class CWallStone : public CGameObject
{
public:
	CWallStone();
	virtual ~CWallStone() = default;

public:
	virtual HRESULT Initialize(_float3 _vStartPos, _float3 _vScale, _uint _StoneIndex);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render();

public:
	virtual HRESULT RenderShadow() override;

public:
	void	DisableStone();
	void	StopStone();

private:
	void	AddRigidBody(string _strModelKey, _float3 _vColScale, _float3 _OffSetPos);

public:
	static shared_ptr<CWallStone> Create(_float3 _vStartPos, _float3 _vScale, _uint _StoneIndex);

private:
	shared_ptr<CModel>		m_pModel = { nullptr };
	shared_ptr<CRigidBody>	m_pRigidBody = { nullptr };
	_uint					m_iMyStoneIndex = { 0 };

};

END