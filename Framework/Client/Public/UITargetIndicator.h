#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUITargetIndicator : public CUIBase
{
	using Super = CUIBase;

public:
	CUITargetIndicator();
	virtual ~CUITargetIndicator();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc);
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public:
	HRESULT RenderFont();

public:
	void SetTargetInfo(const wstring& _strTargetInfo);
	void SetTargetDist(const wstring& _strTargetDist);

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CUITargetIndicator> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc);

private:
	_float m_fTimeAcc{ 0.f };

private:
	wstring m_strTargetInfo{ L"" };
	wstring m_strTargetDist{ L"" };

};

END
