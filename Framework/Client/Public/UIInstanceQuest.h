#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUIInstanceQuest : public CUIBase
{
	using Super = CUIBase;

public:
	CUIInstanceQuest();
	virtual ~CUIInstanceQuest();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void UpdateQuestContent(QUEST_TYPE _questType, const wstring& _questTitle, const wstring& _questContent, const string& _strFont = "Font_AdenL10", _float4 _vTitleFontColor = _float4(0.f, 0.f, 0.f, 0.f), _float4 _vContentFontColor = _float4(0.f, 0.f, 0.f, 0.f));
	void UpdateQuestContentNonAuto(QUEST_TYPE _questType, const wstring& _questTitle, const wstring& _questContent, const string& _strFont = "Font_AdenL10", _float4 _vTitleFontColor = _float4(0.f, 0.f, 0.f, 0.f), _float4 _vContentFontColor = _float4(0.f, 0.f, 0.f, 0.f));
	void AppendQuestContent(QUEST_TYPE _questType, const wstring& _questContent);

private:
	void UpdateTexture();
	void QuestKeyInput();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUIInstanceQuest> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	weak_ptr<CCustomFont> m_Font;

private:
	// 한 줄에 띄어쓰기 포함 22
	// 두 줄이 한계 총 42 -> \n도 포함해야 되므로
	wstring m_strPanelText{ L"퀘스트" }; // Pos6

	//wstring m_strMainQuestTitle{ L"저항군 의뢰" }; // 9
	//wstring m_strMainQuestContent{ L"캐슬러 마을의 해안 절벽 아래에서 의문의\n문서 확인" }; // 10

	//wstring m_strSubQuestTitle0{ L"저항군 의뢰" }; // 14
	//wstring m_strSubQuestContent0{ L"캐슬러 마을의 해안 절벽 아래에서 의문의\n문서 확인" }; // 15
	//wstring m_strSubQuestTitle1{ L"저항군 의뢰" }; // 17
	//wstring m_strSubQuestContent1{ L"캐슬러 마을의 해안 절벽 아래에서 의문의\n문서 확인" }; // 18

	wstring m_strMainQuestTitle{ L"" }; // 9
	wstring m_strMainQuestContent{ L"" }; // 10

	wstring m_strSubQuestTitle0{ L"" }; // 14
	wstring m_strSubQuestContent0{ L"" }; // 15
	wstring m_strSubQuestTitle1{ L"" }; // 17
	wstring m_strSubQuestContent1{ L"" }; // 18

	/* Font Tag */
	string m_strMainQuestFont{ "Font_AdenL10" };
	string m_strSubQuestFont0{ "Font_AdenL10" };
	string m_strSubQuestFont1{ "Font_AdenL10" };

	/* Font Color */
	_float4 m_vMainTitleFontColor{ 1.f, 1.f, 1.f, 1.f };
	_float4 m_vMainContentFontColor{ 1.f, 1.f, 1.f, 1.f };
	_float4 m_vSubTitleFontColor0{ 1.f, 1.f, 1.f, 1.f };
	_float4 m_vSubContentFontColor0{ 1.f, 1.f, 1.f, 1.f };
	_float4 m_vSubTitleFontColor1{ 1.f, 1.f, 1.f, 1.f };
	_float4 m_vSubContentFontColor1{ 1.f, 1.f, 1.f, 1.f };

};

END
