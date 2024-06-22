#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUIInstanceBossAbnormal : public CUIBase
{
	using Super = CUIBase;

public:
	CUIInstanceBossAbnormal();
	virtual ~CUIInstanceBossAbnormal();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void SetGobState(_int _iBossActionState) { m_iGobState = _iBossActionState; }
	void SetOrcState(_int _iBossActionState) { m_iOrcState = _iBossActionState; }
	void SetFinalBossState(_int _iBossActionState) { m_iFinalBossState = _iBossActionState; }

public:
	void ResetStates();

private:
	void UpdateTexture();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUIInstanceBossAbnormal> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	_int m_iGobState{ -1 };
	_int m_iOrcState{ -1 };
	_int m_iFinalBossState{ -1 };

};

END
