#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUIOptionSlider : public CUIBase
{
	using Super = CUIBase;

public:
	CUIOptionSlider();
	virtual ~CUIOptionSlider();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT RenderFont();

public: /* Get & Set Function */
	_float GetSliderRatio(_uint _iSlotIndex) { return m_fSliderRatios[_iSlotIndex]; }

	void SetColorAlpha(_float4 _vColorAlpha) { m_vColorAlpha = _vColorAlpha; }

	void SetOtherNonRender(_int _iIndex);

private:
	void InitUpdateTexture();
	void UpdateTexture();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUIOptionSlider> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	_float m_fSliderMin{ 0.f };
	_float m_fSliderMax{ 0.f };
	vector<_float> m_fSliderRatios; // 0 - Saturate, 1 - Constrast, 2 - MiddleGray, 

private:
	vector<_bool> m_bIsSelected;

private:
	_float m_fConstrast = 0.601f;
	_float m_fMiddleGray = 0.511f;
	_float m_fSaturate = 0.58f;

private:
	_float4 m_vColorAlpha{ 1.f, 1.f, 1.f, 1.f };

};

END
