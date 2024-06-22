#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUIMapTitle : public CUIBase
{
	using Super = CUIBase;

public:
	CUIMapTitle();
	virtual ~CUIMapTitle();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void RenderScript();

public:
	HRESULT RenderFont();

public: /* Get & Set Function */
	void SetDisplayContent(const wstring& _strDisplayContent);

private:
	void InitUpdateTexture();
	void UpdateTexture();
	void UpdateAfter();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUIMapTitle> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_float m_fTimeAcc{ 3.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	_bool m_bIsScriptEnabled{ false };

private:
	wstring m_strDisplayContent{ L"" };

private:
	_float m_fAlphaTime{ 0.f };
	_float m_fAlpha0Time{ 0.f };
	_float m_fDeltaTime{ 0.f };

private:
	_bool m_bIsDone{ false };

};

END
