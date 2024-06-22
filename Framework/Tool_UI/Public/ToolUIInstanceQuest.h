#pragma once

#include "ToolUIBase.h"

BEGIN(Engine)

class CCustomFont;
class CVIInstanceUI;

END

BEGIN(Tool_UI)

class CToolUIInstanceQuest : public CToolUIBase
{
	using Super = CToolUIBase;

public:
	CToolUIInstanceQuest();
	virtual ~CToolUIInstanceQuest();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize, shared_ptr<CCustomFont> _pCustomFont);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CToolUIInstanceQuest> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize, shared_ptr<CCustomFont> _pCustomFont);

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	weak_ptr<CCustomFont> m_Font;

private:
	wstring m_strPanelText{ L"ÄÜÅÙÃ÷ ¾Ë¸®¹Ì" };
	wstring m_strMainQuestTitle{ L"" };
	wstring m_strMainQuestContent{ L"" };

	wstring m_strSubQuestTitle0{ L"" };
	wstring m_strSubQuestContent0{ L"" };
	wstring m_strSubQuestTitle1{ L"" };
	wstring m_strSubQuestContent1{ L"" };

};

END
