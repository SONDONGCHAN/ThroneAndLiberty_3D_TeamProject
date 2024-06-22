#pragma once

#include "Tool_Defines.h"
#include "Engine_Defines.h"
#include "GameObject.h"
#include "VIInstancing.h"

BEGIN(Engine)

class CGameObject;
class CTexture;
class CVIInstancePoint;
class CVIInstanceUI;
class CVIInstancing;

END

class CToolUIInstance : public CGameObject
{
	using Super = CGameObject;

public:
	struct UIInstDesc
	{
		string m_strTag;
		ETOOL_UI_LAYER eUILayer{ TOOL_UI_LAYER_END };
		ETOOL_UI_TYPE eUIType{ TOOL_UI_END };
		ETOOL_SLOT_TYPE eUISlotType{ TOOL_SLOT_END };
		_bool isMasked{ false };
		_bool isNoised{ false };
		_uint iShaderPassIdx{ 0 };
		_float fZOrder{ 0.1f };
		_float fDiscardAlpha{ 0.f };
	};

public:
	CToolUIInstance();
	virtual ~CToolUIInstance();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIInstance::UIInstDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _uiPos, vector<_float2>& _uiSize);
	virtual void PriorityTick(_float _deltaTime) override;
	virtual void Tick(_float _deltaTime) override;
	virtual void LateTick(_float _deltaTime) override;
	virtual HRESULT Render() override;

public: /* Get & Set Function */
	string& GetUITag() { return m_UIDesc.m_strTag; }
	void SetUITag(const string& _strTag) { m_UIDesc.m_strTag = _strTag; }

	_float2 GetUIPos() { return _float2(m_fX, m_fY); }
	void SetUIPos(_float _fPosX, _float _fPosY); // UI의 위치를 변경
	_float2 GetUISize() { return _float2(m_fSizeX, m_fSizeY); }
	void SetBufferSize(_float _fSizeX, _float _fSizeY); // UI의 크기(Scale)를 변경

	void SetColor(_float4 _vColor);
	_float4 GetColor() { return m_InstanceDesc.vColor; }

	_bool IsFinished();

	void SetIsSelected(_bool _isSelected) { m_isSelected = _isSelected; }
	_bool IsSelected() { return m_isSelected; } // 이 UI가 선택이 되어 있는지?

	_bool IsMouseOn(); // 마우스가 UI안에 있는지 체크

	void SetZOrder(_float _fZOrder) { m_UIDesc.fZOrder = _fZOrder; }
	_float GetZOrder() { return m_UIDesc.fZOrder; }
	_float GetSelectedZOrder() { return m_fSelectedZOrder; }

	void SetDiscardAlpha(_float _fDiscardAlpha) { m_UIDesc.fDiscardAlpha = _fDiscardAlpha; }
	_float GetDiscardAlpha() { return m_UIDesc.fDiscardAlpha; }

	void SetBindWorldMat(_float4x4 _worldMat) { m_BindWorldMat = _worldMat; }

	void SetTexIndex(_uint _texIndex) { m_iTexIndex = _texIndex; }

	_bool IsActivated() { return m_isActivated; } // UI가 활성화 되어 있는지 체크

	ETOOL_UI_TYPE GetUIType() { return m_UIDesc.eUIType; } // UI의 타입을 반환

	shared_ptr<CGameObject> GetUIOwner();
	void SetUIOwner(shared_ptr<CGameObject> _pGameObject) { m_pOwner = _pGameObject; }

	_bool IsChild(); // 자식개체인지 체크

	vector<weak_ptr<CToolUIInstance>>& GetChildrenList() { return m_pUIChildren; }

	void SwapTextures(vector<string>& _swapTexTags) { m_strTexKeys.swap(_swapTexTags); }

	/* 파싱용 */
	InstanceUI& GetInstanceDesc() { return m_InstanceDesc; }
	UIInstDesc& GetToolUIDesc() { return m_UIDesc; }
	vector<weak_ptr<CToolUIInstance>>& GetUIChildren() { return m_pUIChildren; }
	shared_ptr<CToolUIInstance> GetUIParent();
	vector<string>& GetTexKeys() { return m_strTexKeys; }
	string& GetMaskKey() { return m_strMaskKey; }
	string& GetNoiseKey() { return m_strNoiseKey; }
	vector<_float4>& GetAtlasUVs() { return m_UVs; }
	_uint GetShaderPass() { return m_iShaderPass; }

	void SetShaderPassIndex(_uint _passIndex);

public:
	void AddUIChild(shared_ptr<CToolUIInstance> _pUIChild); // UI의 자식 UI를 추가
	void AddUIParent(shared_ptr<CToolUIInstance> _uiParent); // 이 UI의 부모 UI를 설정

	void RemoveUIChild(const string& _strUIChildTag);
	void RemoveAllUIChild();
	void RemoveUIParent();

public:
	void RotateUI(_float _xValue, _float _yValue, _float _zValue);

public:
	void ActivateUI(); // 특정 조건에서만 UI가 작동해야 할 경우 활용
	void DeactivateUI(); // 특정 조건에서만 UI가 작동해야 할 경우 활용

	/* 알파값을 활용해 UI가 나타나거나 사라지는 효과를 주고 싶을 경우 */
	void ResetInstanceData(); // 알파값을 초기화

public: /* Slider */
	void ConvertMousePosToUV(_float _mouseX, _float _mouseY);

public:
	void PrepareUVs(_float _fX, _float _fY);

protected:
	void KeyInput();

protected:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _uiPos, vector<_float2>& _uiSize);

public:
	static shared_ptr<CToolUIInstance> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIInstance::UIInstDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _uiPos, vector<_float2>& _uiSize);

protected:
	vector<string> m_strTexKeys;
	_uint m_iTexIndex{ 0 };
	string m_strMaskKey;
	string m_strNoiseKey;
	shared_ptr<CVIInstancePoint> m_pBuffer{ nullptr };
	shared_ptr<CVIInstanceUI> m_pInstanceBuffer{ nullptr };

protected:
	string m_strUIParentKey;
	weak_ptr<CToolUIInstance> m_pUIParent;
	vector<string> m_strUIChildKeys;
	vector<weak_ptr<CToolUIInstance>> m_pUIChildren;

protected:
	weak_ptr<CGameObject> m_pOwner;

protected:
	vector<_float4> m_UVs;
	_uint m_iUVIndex{ 0 };

protected:
	_bool m_isActivated{ false };
	_bool m_isSelected{ false };

protected:
	_float m_fX{ 0.f }, m_fY{ 0.f }, m_fSizeX{ 0.f }, m_fSizeY{ 0.f };
	_float m_fSelectedZOrder{ 0.f };
	_float4x4 m_ViewMat{}, m_ProjMat{}, m_BindWorldMat{};
	_float4x4 m_RotationMat{};

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

	vector<_float2> m_uiPos;
	vector<_float2> m_uiSize;

protected: /* 파싱 용 */
	InstanceUI m_InstanceDesc{};
	UIInstDesc m_UIDesc{};

};
