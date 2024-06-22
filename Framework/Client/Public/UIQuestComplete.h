#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CItemBase;

class CUIQuestComplete : public CUIBase
{
	using Super = CUIBase;

public:
	CUIQuestComplete();
	virtual ~CUIQuestComplete();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT RenderFont();

public: /* Get & Set Function */
	void SetQuestTitle(wstring _strQuestTitle) { m_strQuestTitle = _strQuestTitle; }
	void SetQuestRewardItem(shared_ptr<CItemBase> _pRewardItem) { m_pRewardItem = _pRewardItem; }
	void SetQuestReward(const wstring& _strQuestExp, const wstring& _strQuestMoney);

public:
	void ResetQuestReward();

private:
	void InitUpdateTexture();
	void UpdateTexture();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUIQuestComplete> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	wstring m_strQuestTitle{ L"퀘스트 타이틀" };
	wstring m_strQuestExp{ L"" };
	wstring m_strQuestMoney{ L"" };

private:
	shared_ptr<CItemBase> m_pRewardItem{ nullptr };

};

END
