#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUIInstanceItemBG : public CUIBase
{
	using Super = CUIBase;

public:
	CUIInstanceItemBG();
	virtual ~CUIInstanceItemBG();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void IsRender(_float _fVisibleYTop, _float _fVisibleYBottom);

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUIInstanceItemBG> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_uint m_isRight{ 0 };

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iSlotUV{ 0 }; // 0일 경우 Top이 걸친거고, 1일 경우 Bottom이 걸친것이다, 2일 경우 무조건 영역 안
	_float m_fYRatio{ 0 };

private:
	_float m_fVisibleTop{ 0.f };
	_float m_fVisibleBottom{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

};

END
