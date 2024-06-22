#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CTargetDirection : public CGameObject
{
public:
	struct TargetDirection_Desc
	{
		shared_ptr<CTransform> pParentTransform = { nullptr };
	};

public:
	CTargetDirection();
	virtual ~CTargetDirection() = default;

public:
	virtual HRESULT Initialize(shared_ptr<TargetDirection_Desc> _pTargetDirectionDesc);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render();


public:
	static shared_ptr<CTargetDirection> Create(shared_ptr<TargetDirection_Desc> _pTargetDirectionDesc);

private:
	shared_ptr<CModel>		m_pModel = { nullptr };
	TargetDirection_Desc	m_MyDesc;
	_float4x4				m_WorldMatrix = {};
};

END