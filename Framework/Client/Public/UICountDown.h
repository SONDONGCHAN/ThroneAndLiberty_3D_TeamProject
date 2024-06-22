#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUICountDown : public CUIBase
{
	using Super = CUIBase;

public:
	CUICountDown();
	virtual ~CUICountDown();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void RenderScript();

public:
	HRESULT RenderFont();

public: /* Get & Set Function */
	void SetDisplayContent(const wstring& _strDisplayContent) { m_strDisplayContent = _strDisplayContent; }
	void ResetStartSignal();
	_bool GetStartSignal() { return m_bIsStartSignal; }

	void UpdateRemainTime(_float _fRemainTime) { m_fCurRemainTime = _fRemainTime; }

	void UpdateNumFound(_int _curFound, _int _maxFound);

private:
	void InitUpdateTexture();
	void UpdateTexture();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUICountDown> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_float m_fTimeAcc{ 3.f };
	_float m_fTimeDelta = 0.016f;

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	_bool m_bIsScriptEnabled{ false };

private:
	wstring m_strDisplayContent{ L"" };

private:
	_bool m_bIsStartSignal{ false };

private:
	_float m_fCurRemainTime{ -1.f };

private:
	_int m_iNumCurFound{ -1 };
	_int m_iNumMaxFound{ 3 };

};

END
