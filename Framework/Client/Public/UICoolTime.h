#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUICoolTime : public CUIBase
{
	using Super = CUIBase;

public:
	CUICoolTime();
	virtual ~CUICoolTime();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc);
	virtual void PriorityTick(_float _fDeltaTime) override;
	virtual void Tick(_float _fDeltaTime) override;
	virtual void LateTick(_float _fDeltaTime) override;
	virtual HRESULT Render() override;

public:
	void SetCurCoolTime(_float _fCurCoolTime) { m_fCurCoolTime = _fCurCoolTime; }
	void SetMaxCoolTime(_float _fMaxCoolTime) { m_fMaxCoolTime = _fMaxCoolTime; }

private:
	void CoolTimeKeyInput();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc);

public:
	static shared_ptr<CUICoolTime> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc);

private:
	_bool m_iIsUsed{ 0 };
	_float m_fCurCoolTime{ 0.f };
	_float m_fMaxCoolTime{ 5.f };
	_float m_fCoolTimeRatio{ 0.f };
	_float2 m_vPole{ 0.5f, 1.f };

private:
	_bool m_isBind{ false };

};

END
