#pragma once

#include "UIBase.h"
#include "ItemBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUITradeSlot : public CUIBase
{
	using Super = CUIBase;

public:
	CUITradeSlot();
	virtual ~CUITradeSlot();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT RenderFont();

public:
	CItemBase::ItemDesc& GetItemDesc(_uint _iSlotIndex);

	_int GetCurSelectedIdx() { return m_iCurSelectedIdx; }

public:
	_bool AddItemDescToSlot(CItemBase::ItemDesc& _itemDesc);
	_bool DeleteItemDescFromSlot(_uint _iSlotIndex);

	_bool IncreaseNumItem();
	_bool DecreaseNumItem();

	_bool ClearBuySlots();

	vector<CItemBase::ItemDesc>& GetBuySlotDescs();

private:
	void TradeKeyInput();

private:
	void InitUpdateTexture();
	void UpdateTexture();

private:
	HRESULT PrepareSlots();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUITradeSlot> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	vector<CItemBase::ItemDesc> m_SlotDescs; // 0 ~ 11, 12 ~ 31
	vector<CItemBase::ItemDesc> m_buySlotDescs;
	vector<_bool> m_bIsSelecteds;

	_int m_iCurSelectedIdx{ -1 };

};

END
