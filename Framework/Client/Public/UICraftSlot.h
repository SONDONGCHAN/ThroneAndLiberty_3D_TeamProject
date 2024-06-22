#pragma once

#include "UIBase.h"
#include "ItemBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUICraftSlot : public CUIBase
{
	using Super = CUIBase;

public:
	CUICraftSlot();
	virtual ~CUICraftSlot();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	CItemBase::ItemDesc& GetItemDesc(_uint _iSlotIdx) { return m_ItemDescs[_iSlotIdx]; }

public:
	void RenderFonts();

private:
	void InitUpdateTexture();
	void UpdateTexture();

private:
	HRESULT PrepareItemDescs();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUICraftSlot> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	vector<CItemBase::ItemDesc> m_ItemDescs;

};

END
