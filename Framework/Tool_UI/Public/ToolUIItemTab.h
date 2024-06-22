#pragma once

#include "ToolUIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Tool_UI)

class CToolUIItemTab : public CToolUIBase
{
	using Super = CToolUIBase;

public:
	CToolUIItemTab();
	virtual ~CToolUIItemTab();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public:
	_bool IsTabActivated() { return m_isTabActivated; }

public:
	void DeactivateOtherTabs();

	void ActivateTab();
	void DeactivateTab();

private:
	void ItemTabKeyInput();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CToolUIItemTab> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));

private:
	_float m_fVisibleTop{ 0.f };
	_float m_fVisibleBottom{ 0.f };

private:
	_bool m_isTabActivated{ false };

private:
	vector<weak_ptr<CToolUIItemTab>> m_otherTabs;

};

END
