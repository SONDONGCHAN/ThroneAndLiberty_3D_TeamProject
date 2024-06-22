#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUIScrollBG;

class CUIScroll : public CUIBase
{
	using Super = CUIBase;

public:
	CUIScroll();
	virtual ~CUIScroll();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public: /* Get & Set Function */
	void SetVisibleCenterAndSizeY(_float2 _vVisibleCenter, _float _fVisibleYSize);
	void SetUIBG(shared_ptr<CUIScrollBG> pUIBG) { m_pUIBG = pUIBG; }

	_bool IsScrolling() { return m_isScrolling; }

private:
	void UpdateVisibleRange();

private:
	void ScrollKeyInput();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CUIScroll> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));

private:
	_float m_fRatio{ 1.f };

private:
	_float2 m_vOriginPos{};
	_float m_fDifDistFromOrigin{ 0.f };

private:
	_float2 m_vVisibleCenter{};
	_float m_fVisibleYSize{ 0.f };
	_float2 m_vParentCenter{};
	_float m_fParentYSize{ 0.f };

	_float m_fVisibleUVPosY{ 0.f };
	_float m_fVisibleUVSizeY{ 0.f };

private:
	weak_ptr<CUIScrollBG> m_pUIBG;

private:
	_float2 m_vPickedPos{};

private:
	_bool m_isScrolling{ false };

};

END
