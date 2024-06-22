#pragma once

#include "InteractionObj.h"

BEGIN(Client)

class CMirror final : public CInteractionObj
{
public:
	enum EDIRECTION { M_RIGHT, M_BACK, M_LEFT, M_FRONT, M_END };

public:
	CMirror();
	virtual  ~CMirror() = default;

public:
	static shared_ptr<CMirror> Create(const _float4x4& _vWorldMatrix, const _float3& _vColor, EDIRECTION _eDir);

public:
	virtual HRESULT Initialize(const _float4x4& _vWorldMatrix, const _float3& _vColor, EDIRECTION _eDir);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render() override;

public:
	virtual void InteractionOn() override;
	virtual void InteractionOff() override;
	void LinkMirror(shared_ptr<CMirror> _pLinkedMirror);
	
public:
	void SpawnMirror();
	void DespawnMiorror();


private:
	weak_ptr<CMirror> m_pLinkedMirror;
	_float3 m_vColor;
	float m_fTimeAcc{ 0.0f };
	bool m_isUp{ false };
	EDIRECTION m_eDirection{ M_END };
	FOV_DESC m_FovDesc{};
	bool m_isDespawn{ false };
	float m_fDespawnTime{ 0.0f };
};

END