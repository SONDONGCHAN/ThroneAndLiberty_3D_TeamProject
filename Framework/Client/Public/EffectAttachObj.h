#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CEffectAttachObj : public CGameObject
{
public:
	CEffectAttachObj();
	virtual ~CEffectAttachObj() = default;

public:
	virtual HRESULT Initialize(_float3 _vPos, string _strEffectKey, _bool _bStart);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render()  override;

public:
	HRESULT			PlayEffect();
	virtual void	PlayEffect(string _strGroupID) override;
	HRESULT			StopEffect();

public:
	void			SetLifeTime(_bool _bLoop, _float _fDuration);

public:
	static shared_ptr<CEffectAttachObj> Create(_float3 _vPos, string _strEffectKey, _bool _bStart = true);

private:
	_bool	m_bLoop = false;
	_float	m_fDuration = 0.f;

private:
	string	m_strEffectKey = "";
	_int	m_iEffectIdx = -1;
};

END