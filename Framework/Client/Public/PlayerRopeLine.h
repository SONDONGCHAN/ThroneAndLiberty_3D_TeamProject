#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayerRopeLine : public CGameObject
{
public:
	struct ROPELINE_DESC
	{
		shared_ptr<CTransform> pParentTransform{ nullptr };
		_float4x4* pSocketMatrix{ nullptr };
		_float4x4* pTargetMatrix{ nullptr };
	};

public:
	CPlayerRopeLine();
	virtual ~CPlayerRopeLine()  override;

public:
	virtual HRESULT Initialize(shared_ptr<ROPELINE_DESC> _pRopeLineDesc);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render()  override;
	virtual HRESULT RenderGlow() override;

private:
	shared_ptr<CTransform>	m_pParentTransform = { nullptr };
	_float4x4* m_pSocketMatrix = { nullptr };
	_float4x4* m_pTargetMatrix = { nullptr };

public:
	static shared_ptr<CPlayerRopeLine> Create(shared_ptr<ROPELINE_DESC> _pRopeLineDesc);

};
END