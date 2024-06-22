#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUIInstanceDialogue : public CUIBase
{
	using Super = CUIBase;

public:
	CUIInstanceDialogue();
	virtual ~CUIInstanceDialogue();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void UpdateDialogue(const wstring& _strNPC, const wstring& _strContent, const wstring& _strInteraction, const string& _strNPCFont, const string& _strContentFont, const string& _strInteractionFont, _float4 _vNPCColor, _float4 _vContentColor, _float4 _vInteractionColor);
	
private:
	void UpdateTexture();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUIInstanceDialogue> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	wstring m_strNPCTag{ L"노스" };
	wstring m_strContent{ L"이 의뢰 기한이 촉박해서 말이요. 의뢰를 하기로 했던 저항군 친구는 이미 멀리 떠나버렸고." };
	wstring m_strInteractionTag{ L"다음" };

		/* Font Tag */
	string m_strNPCFont{ "Font_SuiteL12" };
	string m_strContentFont{ "Font_SuiteL10" };
	string m_strInteractionFont{ "Font_SuiteL10" };

	/* Font Color */
	_float4 m_vNPCColor{ 1.f, 1.f, 1.f, 1.f };
	_float4 m_vContentColor{ 1.f, 1.f, 1.f, 1.f };
	_float4 m_vInteractionColor{ 1.f, 1.f, 1.f, 1.f };

};

END
