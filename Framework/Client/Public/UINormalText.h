#pragma once

#include "UIBase.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CVIInstancing;
class CCustomFont;

END

BEGIN(Client)

class CUINormalText : public CUIBase
{
	using Super = CUIBase;

public:
	CUINormalText();
	virtual ~CUINormalText();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, shared_ptr<CCustomFont> _pCustomFont, wstring _strScript);
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public:
	RECT& GetFontSize() { return m_Rect; }

	void SwapText(const wstring& _strScript);

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CUINormalText> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, shared_ptr<CCustomFont> _pCustomFont, wstring _strScript);

private:
	RECT m_Rect{};
	RECT m_PrevRect{};

private:
	_uint m_iInstanceRenderState{ 0 };


	weak_ptr<CCustomFont> m_Font;

private:
	wstring m_strScript;

private:
	_float m_fTimeAcc{ 0.f };

};

END
