#pragma once

#include "InteractionObj.h"

BEGIN(Client)

class CChest : public CInteractionObj
{
public:
	CChest();
	virtual ~CChest() = default;

public:
	virtual HRESULT Initialize(_float3 _vPosition, _uint _iChestNum);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render();

public:
	virtual void InteractionOn();

private:
	shared_ptr<class CUIBackGround> m_pMessageUI = nullptr;
	string m_strUIKey = "";

	_uint m_iChestNum = 0;

	_bool m_IsMessageOn = false;

	_float m_fMaxAppearTime = 6.f;
	_float m_fCurrentAccTime = 0.f;
	_float m_fAlphaRate = 0.f;

	_bool m_IsComplete = false;
	_bool m_IsSwitchingDone = false;
	_bool m_IsFadeIn = false;

public:
	static shared_ptr<CChest> Create(_float3 _vPosition, _uint _iChestNum);

};

END