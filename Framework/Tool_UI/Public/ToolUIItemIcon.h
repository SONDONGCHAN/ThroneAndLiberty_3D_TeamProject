#pragma once

#include "ToolUIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Tool_UI)

class CToolUIItemIcon : public CToolUIBase
{
	using Super = CToolUIBase;

public:
	CToolUIItemIcon();
	virtual ~CToolUIItemIcon();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public: /* Get & Set Function */
	bool IsRender(_float _fVisibleYTop, _float _fVisibleYBottom);

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CToolUIItemIcon> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _initPos = _float2(0.f, 0.f));

private:
	_uint m_iSlotUV{ 0 }; // 0일 경우 Top이 걸친거고, 1일 경우 Bottom이 걸친것이다, 2일 경우 무조건 영역 안
	_float m_fYRatio{ 0 };

};

END
