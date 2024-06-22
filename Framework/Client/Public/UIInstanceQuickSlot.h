#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CItemBase;

class CUIInstanceQuickSlot : public CUIBase
{
	using Super = CUIBase;

public:
	CUIInstanceQuickSlot();
	virtual ~CUIInstanceQuickSlot();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT RenderFont();

public:
	_bool IsEmpty(_uint _iSlotIndex);

public:
	vector<shared_ptr<CItemBase>>* GetItemQuickSlots() { return &m_pItems; }

public:
	void SetQuickSlotScaling(_uint _slotIndex);

public:
	void AddItemToSlot(shared_ptr<CItemBase> _pItem);
	void AddItemToSlotByIndex(_uint _iSlotIndex, shared_ptr<CItemBase> _pItem);
	void RemoveItemFromSlot(_uint _iItemIndex);

private:
	void UpdateTexture();
	void SlotKeyInput();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUIInstanceQuickSlot> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private: /* Item */
	vector<shared_ptr<CItemBase>> m_pItems;
	
	_uint m_iCurWeaponIndex{ 0 };
	_uint m_iPrevWeaponIndex{ 0 };

};

END
