#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUIItemHover : public CUIBase
{
	using Super = CUIBase;

public:
	CUIItemHover();
	virtual ~CUIItemHover();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public: /* Get & Set Function */
	bool IsRender(_float _fVisibleYTop, _float _fVisibleYBottom);

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CUIItemHover> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));

private:
	_uint m_iSlotUV{ 0 }; // 0일 경우 Top이 걸친거고, 1일 경우 Bottom이 걸친것이다, 2일 경우 무조건 영역 안
	_float m_fYRatio{ 0 };

private:
	_float m_fVisibleTop{ 0.f };
	_float m_fVisibleBottom{ 0.f };

private:
	bool m_isHovered{ false };

};

END
