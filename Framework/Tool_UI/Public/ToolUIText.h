#pragma once

#include "ToolUIBase.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class CVIInstancing;
class CCustomFont;

END

BEGIN(Tool_UI)

class CToolUIText : public CToolUIBase
{
	using Super = CToolUIBase;

public:
	CToolUIText();
	virtual ~CToolUIText();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, shared_ptr<CCustomFont> _pCustomFont, wstring _strScript);
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public:
	RECT& GetFontSize() { return m_Rect; }

	void SwapText(const string& _strScript);

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

private:
	HRESULT PrepareCoverUI();

public:
	static shared_ptr<CToolUIText> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, shared_ptr<CCustomFont> _pCustomFont, wstring _strScript);

private:
	RECT m_Rect;
	RECT m_PrevRect;

	weak_ptr<CCustomFont> m_Font;

private:
	wstring m_strScript;

};

END
