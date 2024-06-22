#pragma once
#include "GameObject.h"

class CEffectHelper : public CGameObject
{
public:
	struct HELPER_KEYFRAME
	{
		_float	fTime;

		_float3 vPos;
		_float3 vLook;
	};
	struct EffectHelper_Desc
	{
		vector<HELPER_KEYFRAME> HelperKeyFrames;

		_float	fLifeTime;
		_bool	bFollowOwnerPos;

		_int	iLookType;		// -1 : World, 0 : Follow, 1 : Normal
	};

public:
	CEffectHelper();
	virtual ~CEffectHelper() override;

public:
	virtual HRESULT Initialize(EffectHelper_Desc eDesc);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);

public:
	void			Play(_float _fLifeTime, shared_ptr<CGameObject> _pOwner);

private:
	EffectHelper_Desc		m_eDesc;
	_bool					m_bPlay = false;
	weak_ptr<CGameObject>	m_pOwner;

public:
	static shared_ptr<CEffectHelper> Create(EffectHelper_Desc eDesc);
};

