#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUIOptionCheckBox : public CUIBase
{
	using Super = CUIBase;

public:
	CUIOptionCheckBox();
	virtual ~CUIOptionCheckBox();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public: /* Get & Set Function */
	void SetIsChecked(_bool _bIsChecked, _uint _iIndex) { m_bIsChecked[_iIndex] = _bIsChecked; }
	_bool IsChecked(_uint _iIndex) { return m_bIsChecked[_iIndex]; }

	void SetColorAlpha(_float4 _vColorAlpha) { m_vColorAlpha = _vColorAlpha; }

public:
	HRESULT RenderFont();

private:
	void OptionKeyInput();

private:
	void InitUpdateTexture();
	void UpdateTexture();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUIOptionCheckBox> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	vector<_bool> m_bIsChecked;
	
private:
	_float4 m_vColorAlpha{ 1.f, 1.f, 1.f, 1.f };

};

END
