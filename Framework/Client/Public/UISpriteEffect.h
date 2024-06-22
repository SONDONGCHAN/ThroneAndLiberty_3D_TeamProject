#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUISpriteEffect : public CUIBase
{
	using Super = CUIBase;

public:
	CUISpriteEffect();
	virtual ~CUISpriteEffect();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize, _float2 _vUVs);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void SetIsLoop(_bool _bIsLoop) { m_bIsLoop = _bIsLoop; }
	_bool IsLoop() { return m_bIsLoop; }

	void SetFollowUI(shared_ptr<CUIBase> _pUI) { m_pFollowUI = _pUI; }
	void SetFollowPos(_float2 _vFollowPos) { m_vFollowPos = _vFollowPos; }

	void SetIsPlaying(_bool _bIsPlaying) { m_bIsPlaying = _bIsPlaying; }
	_bool IsPlaying() { return m_bIsPlaying; }

public:
	void ResetTimer();

private:
	void InitUpdateTexture();
	void UpdateTexture();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUISpriteEffect> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize, _float2 _vUVs);

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	_bool m_bIsLoop{ false };

private:
	weak_ptr<CUIBase> m_pFollowUI;

private:
	_float2 m_vFollowPos{ 0.f, 0.f };

private:
	_bool m_bIsPlaying{ false };

};

END
