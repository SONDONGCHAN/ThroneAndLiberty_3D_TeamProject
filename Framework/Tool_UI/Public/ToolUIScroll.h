#pragma once

#include "ToolUIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Tool_UI)

class CToolUIBG;

class CToolUIScroll : public CToolUIBase
{
	using Super = CToolUIBase;

public:
	CToolUIScroll();
	virtual ~CToolUIScroll();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public: /* Get & Set Function */
	void SetVisibleCenterAndSizeY(_float2 _vVisibleCenter, _float _fVisibleYSize);
	void SetUIBG(shared_ptr<CToolUIBG> pUIBG) { m_pUIBG = pUIBG; }

private:
	void UpdateVisibleRange();

private:
	void ScrollKeyInput();
	bool IsScrollMouseOn();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

private:
	HRESULT PrepareVisibleUI(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CToolUIScroll> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));

private:
	_float m_fRatio{ 1.f };

private:
	_float2 m_vOriginPos{};
	_float m_fDifDistFromOrigin{ 0.f };

private:
	_float2 m_vVisibleCenter{};
	_float m_fVisibleYSize{ 0.f };
	_float2 m_vParentCenter{};
	_float m_fParentYSize{ 0.f };

	_float m_fVisibleUVPosY{ 0.f };
	_float m_fVisibleUVSizeY{ 0.f };

private:
	weak_ptr<CToolUIBG> m_pUIBG;

private:
	_float2 m_vPickedPos{};

private:
	_bool m_isScrolling{ false };

};

END
