#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUIPanel : public CUIBase
{
	using Super = CUIBase;

public:
	CUIPanel();
	virtual ~CUIPanel();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc);
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public:
	void SetIsFailed(_bool _bIsFailed) { m_bIsFailed = _bIsFailed; }

public:
	void SetVisibleRange(_float _fVisibleTop, _float _fVisibleBottom);

private:
	HRESULT AddUIRenderGroup();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CUIPanel> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc);

private:
	_uint m_iRenderState{ 0 }; // 0 -> has No Child(Render Texture), 1 -> Has Child(Render None)

private:
	_float m_fVisibleTop{ 0.f };
	_float m_fVisibleBottom{ 0.f };

private:
	_bool m_bIsFailed{ false };

};

END
