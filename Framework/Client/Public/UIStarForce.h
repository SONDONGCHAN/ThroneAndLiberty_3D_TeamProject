#pragma once

#include "UIBase.h"

BEGIN(Engine)

class CVIInstancing;

END

BEGIN(Client)

class CUIStarForce : public CUIBase
{
	using Super = CUIBase;

public:
	CUIStarForce();
	virtual ~CUIStarForce();

public:
	virtual HRESULT Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void SetLeftAndRightEnd(_float _fLeftEndX, _float _fRightEndX);
	void ResetStarForce();

	void SetIsPressed(_bool _bIsPressed) { m_bIsPressed = _bIsPressed; }

public:
	HRESULT RenderFont();

private:
	void StarForceKeyInput();

private:
	void InitUpdateTexture();
	void UpdateTexture();

private:
	HRESULT AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

public:
	static shared_ptr<CUIStarForce> Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc, vector<_float2>& _vUIPos, vector<_float2>& _vUISize);

private:
	_float m_fTimeAcc{ 0.f };

private:
	_uint m_iInstanceRenderState{ 0 };

private:
	vector<_float2> m_vSlotUVRatio;

private:
	_float m_fLeftEndX{};
	_float m_fRightEndX{};
	_bool m_bIsRight{ true };
	_float m_fMovePow{ 5.f };
	
private:
	_bool m_bIsPressed{ false };
	_float m_fDisappearTime{ 0.f };

private:
	_bool m_bIsFailed{ false }; // 제작 실패?
	_bool m_bIsSuccess{ false }; // 제작 성공?

private:
	_bool m_bIsAvailbleToBuy{ false };

};

END
