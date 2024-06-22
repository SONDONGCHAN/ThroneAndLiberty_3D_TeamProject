#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

class CAnimEffect : public CGameObject
{
public:
	struct ANIMEFFECT_DESC
	{
		string		strModelKey = "";
		double		StartTrackPos = 0;

		_bool		bStartOnOwnerTrackPos = false;
		double		StartOwnerTrackPos = 0;
		

		_float3		vScale		= { 1.f, 1.f, 1.f };
		_float3		vRotation = { 0.f, 0.f, 0.f };
		_bool		bRotRelatedOwner = false;
		_bool		bRotFollowOwner = false;
		_float3		vPosition = { 0.f, 0.f, 0.f };
		_bool		bPosRelatedOwner = false;
		_bool		bPosFollowOwner = false;
		_int		iPosType = 0;
		
		_bool		bUseDistortion = false;
		string		strDistortionKey = "";
		_float		fDistortionSpeed = 1.f;
		_float		fDistortionWeight = 1.f;

		_bool		bUseGlow = false;
		_bool		bUseGlowColor = false;
		_float4		vGlowColor = { 0.f, 0.f, 0.f, 0.f };

		_bool       bUseDslv = false;
		_int        iDslvType = 0;  // 0 : Dslv In, 1 : Dslv Out
		_float      fDslvStartTime = 0.f;
		_float      fDslvDuration = 0.f;
		_float      fDslvThick = 1.f;
		_float4     vDslvColor = { 1.f, 1.f, 1.f, 1.f };
		string      strDslvTexKey = "";
	};

public:
	CAnimEffect();
	virtual ~CAnimEffect();

public:
	virtual HRESULT Initialize();
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderDistortion() override;

private:
	ANIMEFFECT_DESC	m_eAnimEffectDesc;
	shared_ptr<class CModel> m_pModelCom;

private:
	_bool			m_bPlayAnimation = false;
	_float			m_fDistortionTimer = 0.f;
	_bool			m_bStart = false;

	weak_ptr<CGameObject>	m_pOwner;
	weak_ptr<CModel>		m_pOwnerModel;
	_float4x4				m_OwnerMat;

public:
	void			SetAnimEffectDesc(ANIMEFFECT_DESC _AnimEffectDesc);
	void			Play(shared_ptr<CGameObject> _pOwner);

public:
	static shared_ptr<CAnimEffect> Create();
};

