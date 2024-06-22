#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CPartObject : public Engine::CGameObject
{
public:
	struct PARTOBJ_DESC
	{
		shared_ptr<CTransform> pParentTransform = nullptr;
	};

public:
	CPartObject();
	virtual ~CPartObject() override;

	void	SetActiveOn(_bool _bActiveOn) { m_bActiveOn = _bActiveOn; }
	_bool	GetActiveOn() { return m_bActiveOn; }

public:
	virtual HRESULT Initialize(PARTOBJ_DESC* _pPartObjDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);
	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_bool					m_bActiveOn = true;

	shared_ptr<CTransform>	m_pParentTransform = nullptr;
	_float4x4				m_WorldMatrix;
};

END

