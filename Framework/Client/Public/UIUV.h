#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUIUV : public CUIBase
{
	using Super = CUIBase;

public:
	CUIUV();
	virtual ~CUIUV();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _vUVs);
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public: /* Get & Set Function */

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CUIUV> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, _float2 _vUVs);

};

END
