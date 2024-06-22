#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CGameObject;
class CVIInstancing;

END

BEGIN(Client)

class CUIUVBar : public CUIBase
{
	using Super = CUIBase;

public:
	CUIUVBar();
	virtual ~CUIUVBar();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc);
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public:
	void SetBar(_float _fCurHP, _float _fMaxHP);

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CUIUVBar> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc);

private:
	_float m_fRatio{ 1.f };
	_float m_fBindRatio{ 1.f };
	_float m_fCurHP{ 0.f };
	_float m_fPrevHP{ 0.f };
	_float m_fRatioHP{ 0.f };
	_float m_fMaxHP{ 0.f };
	_float m_fRatioTime{ 0.f };

private:
	_bool m_bStart{ false };

};

END
