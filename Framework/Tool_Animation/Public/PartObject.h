#pragma once

#include "GameObject.h"
#include "Tool_Animation_Defines.h"

BEGIN(Tool_Animation)

class CPartObject : public CGameObject
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

public:
	virtual HRESULT Initialize(PARTOBJ_DESC* _pPartObjDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);
	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	// 추후에 Enable 작동하면 삭제 예정
	_bool			m_bActiveOn = true;

	shared_ptr<CTransform>	m_pParentTransform = nullptr;
	_float4x4				m_WorldMatrix;
};

END

