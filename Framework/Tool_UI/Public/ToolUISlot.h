#pragma once

#include "ToolUIBase.h"

BEGIN(Engine)

class CTexture;
class CVIInstancing;

END

BEGIN(Tool_UI)

class CToolUISlot : public CToolUIBase
{
	using Super = CToolUIBase;

public:
	struct SlotDesc
	{
		_bool isFilled{ false };
		string strImageTag;
		/* 아이템 or 스킬의 정보를 나타내는 팝업을 저장? */

		/* 아니면 아이템의 weak_ptr 저장? */

		_float fExpandUV{ 0.f };
	};

public:
	CToolUISlot();
	virtual ~CToolUISlot();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public: /* Get & Set Function */
	bool IsRender(_float _fVisibleYTop, _float _fVisibleYBottom);

	SlotDesc& GetSlotDesc() { return m_SlotDesc; }

	_bool IsFilled() { return m_SlotDesc.isFilled; }
	void SetIsFilled(_bool _isFilled) { m_SlotDesc.isFilled = _isFilled; }

	string& GetImageTag() { return m_SlotDesc.strImageTag; }
	void SetImageTag(string& _strImageTag) { m_SlotDesc.strImageTag = _strImageTag; }

	void SetSlotTag(string& _strSlotTag) { m_strSlotTag = _strSlotTag; }

	void SetItemType(ETOOL_ITEM_TYPE _eItemType) { m_eItemType = _eItemType; }
	_uint GetItemType() { return static_cast<_uint>(m_eItemType); }

private:
	void FollowMouse();
	void SlotKeyInput();
	_bool SlotIsMouseOn();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CToolUISlot> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));

private:
	_uint m_iSlotUV{ 0 }; // 0일 경우 Top이 걸친거고, 1일 경우 Bottom이 걸친것이다, 2일 경우 무조건 영역 안
	_float m_fYRatio{ 0 };

private:
	SlotDesc m_SlotDesc{};

private: /* Only For MouseSlot */
	string m_strSlotTag;

private:
	ETOOL_ITEM_TYPE m_eItemType{ TOOL_ITEM_END };

private:
	_float m_fVisibleTop{ 0.f };
	_float m_fVisibleBottom{ 0.f };

};

END
