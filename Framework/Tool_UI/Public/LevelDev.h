#pragma once

#include "Engine_Defines.h"
#include "Tool_Defines.h"
#include "Level.h"
#include "VIInstancePoint.h"
#include "ToolUIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

namespace Tool_UI
{
	class CToolUIMgr;
	
	class CLevelDev final : public CLevel
	{
	public:
		CLevelDev();
		virtual ~CLevelDev();

	public:
		virtual HRESULT Initialize();
		virtual void Tick(_float _fTimeDelta);
		virtual void LateTick(_float _fTimeDelta);
		virtual HRESULT Render();

	private:
		void CreateUI(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc);
		void SaveUI(std::ofstream& _of, const string& _strUITag);
		void LoadUI(std::ifstream& _if);
		void LoadUIComplete(std::ifstream & _if);

		void SaveUIJSON(Json::Value& root, const string& _strUITag, const string& _strSaveFile = "");
		void SaveUIInstanceJSON(Json::Value& root, const string& _strUITag, const string& _strSaveFile = "");
		void SaveUIInstanceNormal(Json::Value& root, const string& _strUITag, const string& _strSaveFile = "");
		void LoadUIJSON(Json::Value& root, const string& _strUITag);
		void LoadUIInstanceJSON(Json::Value& root, const string& _strUITag);

	private:
		HRESULT ReadyLayerBackGround(const wstring& _strLayerTag);
		HRESULT ReadyLight();
		HRESULT ReadyCamera(const wstring& _strLayerTag);

	private:
		HRESULT LoadTextureTags(const wstring& _strResourcePath);

	public:
		static shared_ptr<CLevelDev> Create();

	private: /* UI */
		shared_ptr<CToolUIMgr> m_pToolUIManager{ nullptr };

		/* Create UI */
		Engine::CVIInstancing::InstanceDesc m_pUIInstanceDesc{};
		array<_float, 3> m_UICenter{ 0.f, 0.f, 0.f };
		array<_float, 2> m_UISize{ 0.f, 0.f };
		array<_float, 2> m_UISpeed{ 0.f, 0.f };
		bool m_isLoopUpper{ false };
		array<_float, 4> m_vUIColor{ 0.f, 0.f, 0.f, 0.f };
		_float m_fUIDuration{ 0.f };
		_float m_fUIScaleY{ 0.f };
		array<_float, 2> m_UIuv{ 0.f, 0.f };

		Tool_UI::CToolUIBase::ToolUIDesc m_toolUIDesc{};
		string m_strUITag;
		_int m_iUIType{ static_cast<_uint>(TOOL_UI_END) };
		_int m_iUILayer{ static_cast<_uint>(TOOL_UI_LAYER_END) };
		_int m_iUITexType{ static_cast<_uint>(TOOL_UI_TEX_END) };
		_int m_iUISlotType{ static_cast<_uint>(TOOL_SLOT_END) };

		_uint m_iCurUIShader{ 0 };
		string m_strUIDiffuseTag;
		_bool m_isMasked{ false };
		string m_strUIMaskTag;
		_bool m_isNoised{ false };
		string m_strUINoiseTag;
		_int m_iNumUIInstance{ 0 };
		_float m_fUIDiscardAlpha{ 0.f };
		_float m_fZOrder{ 0.f };
		_int m_iUIItemType{ static_cast<_uint>(TOOL_ITEM_END) };
		_float m_fVisibleSizeY{ 0.f };
		char m_cTextScripts[MAX_PATH] = "";
		string m_strTextScript;

		/* Modify UI */
		array<_float, 2> m_UIPos{ 0.f, 0.f };
		array<_float, 2> m_UIScale{ 0.f, 0.f };
		array<_float, 3> m_UIRotation{ 0.f, 0.f, 0.f };
		array<_float, 4> m_vModUIColor{ 0.f, 0.f, 0.f, 0.f };

		string m_strSwapUITag;

		_float m_fModUIDiscardAlpha{ 0.f };
		_float m_fzOrder{ 0.f };
		_int m_iShaderPassIndex{ 0 };

		vector<string> m_texTags; // 텍스쳐의 이름들을 미리 로드해두기 위한 Container
		vector<string> m_createTexTags; // UI를 생성하기 위해 넣는 텍스쳐를 모아놓는 Container

		vector<string> m_strUIList; // 생성한 UI들의 List를 모아두는 Container

		_uint m_selectedUIIndex{ 0 };
		string m_strSelUITag; // List내 UI를 선택했을 때 그 UI의 Tag를 저장

		string m_strSwapTexTag;
		vector<string> m_swapTags;

		_uint m_selUIIndex{ 0 }; // 활성화 시킬 UI의 Index
		string m_strLatestSelUITag; // 활성화 할 UI의 Tag를 저장

		vector<string> m_strChildUIList; // 선택한 UI의 자식 UI들의 Tag들을 모아놓은 Container
		_uint m_selectedUIChildIndex{ 0 };
		string m_strSelUIChildTag;

		unordered_map<string, shared_ptr<CToolUIBase>>* m_allUIs;
		vector<string> m_allUITags;
		_uint m_selectedAllUIIndex{ 0 };
		string m_strAllUITag;

		vector<weak_ptr<CToolUIBase>>* m_allActivatedUIs;
		vector<string> m_allActivatedUITags;
		_uint m_selectedAllActivatedUIIndex{ 0 };
		string m_strAllActivatedUITag;

	private: /* Parsing */
		string m_strSavePath = "../Bin/DataFiles/";
		string m_strSaveFile;
		string m_strLoadFile;
		string m_strLoadNoiseFile;
		string m_strLoadJsonFile;

		unordered_map<string, vector<string>> m_uiChildrenTags;
		vector<string> m_strLoadUIList;

	};
}
