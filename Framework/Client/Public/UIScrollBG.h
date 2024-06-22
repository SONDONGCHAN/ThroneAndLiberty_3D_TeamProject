#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUIScroll;

class CUIScrollBG : public CUIBase
{
	using Super = CUIBase;

public:
	CUIScrollBG();
	virtual ~CUIScrollBG();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float _fVisibleSizeY, _float2 _initPos = _float2(0.f, 0.f));
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public: /* Get & Set */
	void SetScrollUI(shared_ptr<CUIScroll> pScroll) { m_pScroll = pScroll; }

	_float2 GetVisibleCenter() { return m_vVisibleCenter; }
	_float GetOriginVisibleSizeY() { return m_fOriginVisibleYSize; }
	_float GetVisibleSizeY() { return m_fVisibleYSize; }

	void SetVisibleRange();
	void SetVisibleSizeY(_float _fVisibleSizeY);

	void SetBGInitPos(_float _fPosX, _float _fPosY);

private:
	void UpdateVisibleRange();
	void BGKeyInput();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CUIScrollBG> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float _fVisibleSizeY, _float2 _initPos = _float2(0.f, 0.f));

private:
	weak_ptr<CUIScroll> m_pScroll;

private:
	_float m_fRatio{ 1.f };

private:
	_float2 m_vOriginPos{};
	_float m_fDifDistFromOrigin{ 0.f };

private:
	_float2 m_vVisibleCenter{};
	_float m_fOriginVisibleYSize{ 0.f };
	_float m_fVisibleYSize{ 0.f };

	_float m_fVisibleUVPosY{ 0.f };
	_float m_fVisibleUVSizeY{ 0.f };

};

END
