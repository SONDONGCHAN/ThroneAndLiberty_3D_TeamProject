#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CItemBase;

class CUISlot : public CUIBase
{
	using Super = CUIBase;

public:
	CUISlot();
	virtual ~CUISlot();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public: /* Get & Set Function */
	bool IsRender(_float _fVisibleYTop, _float _fVisibleYBottom);

	_bool IsEmpty() { return m_isEmpty; }
	void SetIsEmpty(_bool _isEmpty) { m_isEmpty = _isEmpty; }

	void SetSlotTag(string& _strSlotTag) { m_strSlotTag = _strSlotTag; }

	void SetItemType(UI_ITEM_TYPE _eItemType) { m_eUIItemType = _eItemType; }
	_uint GetItemType() { return static_cast<_uint>(m_eUIItemType); }

	_uint GetSlotIndex() { return m_iSlotIndex; }

	/* Mouse Slot */
	shared_ptr<CItemBase> GetSlotItem() { return m_pItem; }
	
	void SetPrevSlotIndex(_uint _iPrevSlotIndex) { m_iPrevSlot = _iPrevSlotIndex; }
	_uint GetPrevSlotIndex() { return m_iPrevSlot; }

	void SetPrevSlotType(UI_SLOT_TYPE _ePrevSlotType) { m_ePrevSlotType = _ePrevSlotType; }
	UI_SLOT_TYPE GetPrevSlotType() { return m_ePrevSlotType; }

public:
	void AddItemToSlot(shared_ptr<CItemBase> _pItem);
	void RemoveItemFromSlot();

private:
	void FollowMouse();
	void SlotKeyInput();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CUISlot> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));

private:
	_uint m_iSlotUV{ 0 }; // 0일 경우 Top이 걸친거고, 1일 경우 Bottom이 걸친것이다, 2일 경우 무조건 영역 안
	_float m_fYRatio{ 0 };

private: /* Only For MouseSlot */
	string m_strSlotTag;
	_uint m_iPrevSlot{ 0 };
	UI_SLOT_TYPE m_ePrevSlotType{ SLOT_END };

private:
	UI_ITEM_TYPE m_eUIItemType{ UI_ITEM_END };

private:
	_float m_fVisibleTop{ 0.f };
	_float m_fVisibleBottom{ 0.f };

private:
	_float m_fExpandUV{ 0.f };

private:
	_float m_fTimeAcc{ 0.f };

private: /* Item */
	_uint m_iSlotIndex{ 0 };
	_bool m_isEmpty{ true };

private:
	shared_ptr<CItemBase> m_pItem;

};

END
