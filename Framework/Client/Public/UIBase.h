#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "VIInstancing.h"

BEGIN(Engine)

class CGameObject;
class CVIInstancePoint;
class CVIInstanceUI;

END

BEGIN(Client)

class CUIBase abstract : public Engine::CGameObject
{
	using Super = CGameObject;

public:
	struct UIDesc
	{
		string strTag;
		UI_LAYER eUILayer{ UI_LAYER_END };
		UI_TYPE eUIType{ UI_END };
		UI_TEX_TYPE eUITexType{ UI_TEX_END };
		UI_SLOT_TYPE eUISlotType{ SLOT_END };
		_bool isMasked{ false };
		_bool isNoised{ false };
		_uint iShaderPassIdx{ 0 };
		_float fZOrder{ 0.f };
		_float fDiscardAlpha{ 0.f };
	};

public:
	CUIBase();
	virtual ~CUIBase();

public:
	virtual HRESULT Initialize();
	virtual void PriorityTick(_float _deltaTime) override;
	virtual void Tick(_float _deltaTime) override;
	virtual void LateTick(_float _deltaTime) override;
	virtual HRESULT Render() override;

public: /* Get & Set Function */
	string& GetUITag() { return m_UIDesc.strTag; } // UI의 Tag를 반환, UIManager에서 UI를 찾거나 할 때 쓰인다
	void SetUITag(const string& _strTag) { m_UIDesc.strTag = _strTag; }

	_float2 GetUIPos() { return _float2(m_fX, m_fY); } // UI의 위치를 반환
	void SetUIPos(_float _fPosX, _float _fPosY); // UI의 위치를 변경

	_float2 GetUISize() { return _float2(m_fSizeX, m_fSizeY); } // UI의 크기를 반환
	void SetBufferSize(_float _fSizeX, _float _fSizeY); // UI의 크기(Scale)를 변경

	_bool IsFinished(); // UI의 애니메이션이 끝났는지? (미구현)

	void SetIsSelected(_bool _isSelected) { m_isSelected = _isSelected; }
	_bool IsSelected() { return m_isSelected; } // 이 UI가 선택이 되어 있는지?

	_bool IsMouseOn(); // 마우스가 UI안에 있는지 체크
	_int IsInstanceMouseOn();
	_bool IsInstanceMouseOn(_uint _iIndex);

	void SetZOrder(_float _fZOrder) { m_UIDesc.fZOrder = _fZOrder; }
	_float GetZOrder() { return m_UIDesc.fZOrder; } // UI의 ZOrder를 반환
	_float GetSelectedZOrder() { return m_fSelectedZOrder; } // UI가 선택되어 있을 경우 이걸 반환(무조건 가장 앞에 있게)

	void SetDiscardAlpha(_float _fDiscardAlpha) { m_UIDesc.fDiscardAlpha = _fDiscardAlpha; }

	void SetBindWorldMat(_float4x4 _worldMat) { m_BindWorldMat = _worldMat; } // 현재까지 아직 안쓰임(추후 수정 가능성 높음)

	void SetTexIndex(_uint _texIndex) { m_iTexIndex = _texIndex; } // Texture가 여러 개 있을 경우 상황에 따라 바꿔줄 수 있다
	_uint GetTexIndex() { return m_iTexIndex; } // 현재 TextureIndex를 반환

	_bool IsActivated() { return m_isActivated; } // UI가 활성화 되어 있는지 체크

	UI_TYPE GetUIType() { return m_UIDesc.eUIType; } // UI의 타입을 반환
	UI_SLOT_TYPE GetSlotType() { return m_UIDesc.eUISlotType; }

	shared_ptr<CGameObject> GetUIOwner(); // 이 UI의 소유 객체를 반환
	void SetUIOwner(shared_ptr<CGameObject> _pGameObject) { m_pOwner = _pGameObject; }

	_bool IsChild(); // 자식개체인지 체크

	unordered_map<string, weak_ptr<CUIBase>>& GetChildrenList() { return m_pUIChildren; }

	void SwapTextures(vector<string>& _swapTexTags) { m_strTexKeys.swap(_swapTexTags); }

	vector<string>& GetTexKeys() { return m_strTexKeys; }

	void SetIsWorldUI(_bool _isWorldUI) { m_isWorldUI = _isWorldUI; }

	void SetNonResetRender(_bool _isRender);
	void SetIsRender(_bool _isRender);
	_bool IsRender() { return m_isRender; }

	void SetIsPickable(_bool _isPickable) { m_isPickable = _isPickable; }
	_bool IsPickable() { return m_isPickable; }

	void SetIsLoadingDone(_bool _isLoadingDone) { m_isLoadingDone = _isLoadingDone; }
	_bool IsLoadingDone() { return m_isLoadingDone; }

public:
	void AddUIChild(const string& _strUITag, shared_ptr<CUIBase> _pUIChild); // UI의 자식 UI를 추가
	void AddUIParent(shared_ptr<CUIBase> _uiParent); // 이 UI의 부모 UI를 설정

	void RemoveUIChild(const string& _strUIChildTag); // 특정 자식 UI를 삭제(미구현)
	void RemoveAllUIChild(); // 자식 UI들을 전부 삭제(미구현)
	void RemoveUIParent(); // 부모 UI를 삭제(미구현)

	void SaveAllChildren(vector<shared_ptr<CUIBase>>& _outAllChildren);

public:
	void RotateUI(_float _xValue, _float _yValue, _float _zValue); // UI의 회전 -> Z축만 먹힘

public:
	void ActivateUI(); // 특정 조건에서만 UI가 작동해야 할 경우 활용
	void DeactivateUI(); // 특정 조건에서만 UI가 작동해야 할 경우 활용

	/* 알파값을 활용해 UI가 나타나거나 사라지는 효과를 주고 싶을 경우 */
	void ResetInstanceData(); // 알파값을 초기화

public: /* Slider */
	void ConvertMousePosToUV(_float _mouseX, _float _mouseY);

public:
	void PrepareUVs(_float _fX, _float _fY); // 텍스쳐 이미지가 아틀라스일 경우 UV를 맞춰서 저장해둔다

protected:
	void KeyInput();

protected:
	UIDesc m_UIDesc{};
	_uint m_iTexIndex{ 0 };

protected:
	vector<string> m_strTexKeys;
	string m_strMaskKey;
	string m_strNoiseKey;
	shared_ptr<CVIInstancePoint> m_pBuffer{ nullptr };
	shared_ptr<CVIInstanceUI> m_pInstanceBuffer{ nullptr };

protected:
	weak_ptr<CUIBase> m_pUIParent;
	unordered_map<string, weak_ptr<CUIBase>> m_pUIChildren;

	//_float3 m_vLocalPos{}; // 부모를 기준으로 한 상대 위치

protected: /* 이 UI를 소유하는 객체 */
	weak_ptr<CGameObject> m_pOwner;

protected:
	vector<_float4> m_UVs;
	_uint m_iUVIndex{ 0 };

protected:
	_bool m_isActivated{ false };
	_bool m_isSelected{ false };

protected:
	_float m_fX{ 0.f }, m_fY{ 0.f }, m_fSizeX{ 0.f }, m_fSizeY{ 0.f };
	_float4x4 m_ViewMat{}, m_ProjMat{}, m_BindWorldMat{};
	_float4x4 m_RotationMat{};

protected: /* ZOrder */
	_float m_fSelectedZOrder{ 0.0f };

protected: /* 자식 UI일 때 위치 보정 */
	_float3 m_vDir{};
	_float m_fDist{ 0.f };

	_float3 m_vPrevParentPos{ FLT_MAX, FLT_MAX, FLT_MAX };

protected: /* Button */
	function<void(void)> m_activeFunc;

protected: /* Slider */
	_float2 m_uvMouse{ 0.5f, 0.5f };
	_float m_fUVRadius{ 0.f };

protected:
	_float m_fTimeAcc{ 0.f };

protected:
	_bool m_isHovered{ false };
	_bool m_isRender{ true };
	_bool m_isPickable{ false };

protected: /* Set World UI */
	_bool m_isWorldUI{ false };
	_bool m_isOnce{ true };

protected: /* 인스턴싱 용 */
	vector<_float2> m_vUIPos;
	vector<_float2> m_vUISize;

protected:
	_bool m_isLoadingDone{ true };

};

END
