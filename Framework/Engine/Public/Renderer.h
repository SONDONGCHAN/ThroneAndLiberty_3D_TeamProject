#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CRenderer final
{
public:
	enum ERENDER_GROUP {
		RENDER_PRIORITY,
		RENDER_NONBLEND,
		RENDER_EDGE,
		RENDER_NONLIGHT,
		RENDER_BLUR,
		RENDER_GLOW,
		RENDER_DISTORTION,
		RENDER_BLEND,
		RENDER_TRAIL,
		RENDER_SHADOW,
		RENDER_WORLDUI,
		RENDER_UI,
		RENDER_PIXELPICKING, // Pixel Picking
		RENDER_END
	};

	enum ERENDER_UI_GROUP
	{
		RENDER_UI_WORLD,
		RENDER_UI_HUD, // HP, Quick, Quest
		RENDER_UI_CONTENT, // Craft, Trade
		RENDER_UI_INVENTORY, // Inventory
		RENDER_UI_POPUP, // QuestComplete, Tooltip, StarForce
		RENDER_UI_EFFECT, // UIEffects, Success or Fail?

		RENDER_UI_END
	};

public:
	CRenderer(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual ~CRenderer();

public:
	HRESULT Initialize();
	HRESULT AddRenderGroup(ERENDER_GROUP _eRenderGroup, shared_ptr<class CGameObject> _pGameObject);
	HRESULT AddUIRenderGroup(ERENDER_UI_GROUP _eUIRenderGroup, shared_ptr<class CGameObject> _pGameObject);
	HRESULT AddCascadeGroup(_uint _iCascadeNum, shared_ptr<class CGameObject> _pGameObject);
	HRESULT Render();

	void Clear();

	bool isFadeOut() { return m_IsFadeOut; }
	bool isBlackOut() { return m_IsBlackOut; }

public:
	HRESULT SetMotionBlur(_float _fDuration, _float _fPower);
	HRESULT SetZoomBlur(_float _fDuration, _float _fPower, _float3 _vWorldPos);
	HRESULT SetFadeEffect(_bool _IsFadeOut, _float _fTotalDuration, _bool _IsContinual, _float _fBlackOutTime);
	HRESULT SetChromaticAberration(_bool _IsChromaticAberrationOn);
	HRESULT SetScreenNoise(_bool _IsScreenNoiseOn);

private:

	/*z PrePass를 위해 미리 깊이를 저장해둠*/
	HRESULT RenderDepth();

	// Pixel Picking
	HRESULT RenderPixelPicking();

	HRESULT RenderPriority();
	HRESULT RenderNonBlend();
	HRESULT RenderLight();
	HRESULT RenderEdge();
	HRESULT RenderFinal();
	HRESULT RenderNonLight();
	HRESULT RenderBlur();
	HRESULT RenderGlow();
	HRESULT RenderDistortion();
	HRESULT RenderBlend();
	HRESULT RenderTrail();
	HRESULT RenderDOF();
	HRESULT RenderCascadeShadow();
	HRESULT RenderPostProcess();
	HRESULT RenderHDR();
	HRESULT RenderFxaa();
	HRESULT RenderScreenTone();

	HRESULT RenderWorldUI();
	HRESULT RenderUI();

	HRESULT RenderUIWorld();
	HRESULT RenderUIHUD();
	HRESULT RenderUIContent();
	HRESULT RenderUIInventory();
	HRESULT RenderUIPopUp();
	HRESULT RenderUIEffect();

	HRESULT RenderFadeInOut();

	HRESULT RenderForFilmVideo();

private:
	HRESULT ReadyCascadeDSV();
	HRESULT ReadyIBL();
	HRESULT ReadyOutlineDSV();
	HRESULT ReadyUAV();


	wrl::ComPtr<ID3D11UnorderedAccessView> m_pUAVs;
	wrl::ComPtr<ID3D11ShaderResourceView> m_pSRVs;

private:
	void SortRenderList(ERENDER_GROUP _eRenderGroup);

private:
	list<shared_ptr<class CGameObject>> m_RenderObjects[RENDER_END];
	list<shared_ptr<class CGameObject>> m_RenderUIs[RENDER_UI_END];
	list<shared_ptr<class CGameObject>> m_CascadeList;

//#ifdef _DEBUG
public:
	HRESULT AddDebugComponent(shared_ptr<class CComponent> _pComponent);

	void DebugColliderSwitch() { m_IsDebugMode = !m_IsDebugMode; }
	void DebugRenderSwitch() { m_IsRenderTargetOn = !m_IsRenderTargetOn; }
	void SetRenderTargetGroup(_uint _iGroupIdx) { m_iCurretRenderGroupIdx = _iGroupIdx; }


private:
	list<shared_ptr<class CComponent>> m_DebugCom;

	HRESULT RenderDebug();


//#endif

public:

	void SwitchingToneMappingOption(_bool _IsOptionOn) { m_IsHDRRender = _IsOptionOn; }
	void SwitchingPBROption(_bool _IsOptionOn) { m_IsPBROn = _IsOptionOn; }
	void SwitchingAOOption(_bool _IsOptionOn) { m_IsAOOn = _IsOptionOn; }
	void SwitchingHDROption(_bool _IsOptionOn) { m_IsHDROn = _IsOptionOn; }
	void SwitchingDOF(_bool _IsOptionOn) { m_UsingDOF = _IsOptionOn; }
	void SwitchingFog(_bool _IsOptionOn) { m_UsingFog = _IsOptionOn; }

	void SwitchingMotionBlur() { m_IsMotionBlurActive = !m_IsMotionBlurActive; }

	void ChangeAOPower(_float _fAOPower) { m_fAOPower = _fAOPower; }
	void PartitionScreenHalf(_bool _IsPartition) { m_IsPartitionScreen = _IsPartition; }
	void SwitchingTargetGroup1Mode() { m_IsTargetGroup1Mode = !m_IsTargetGroup1Mode; }
	void SwitchingZoomBlurOff() { m_bZoomBlurOff = !m_bZoomBlurOff; }

public:
	void SwitchingShadowOption(_bool _IsShadowOn) { m_IsShadowOn = _IsShadowOn; }
	void RendererOptionSwitching(_uint _iLevel);
	void SwitchingEffectOption() { m_IsEffectOn = !m_IsEffectOn; }


private:
	_float m_fTargetSizeGroup1 = 250.f;
	_float m_fTargetSizeGroup2 = 150.f;


private:
	wrl::ComPtr<ID3D11DepthStencilView> m_pCascadeDSV = nullptr;
	wrl::ComPtr<ID3D11ShaderResourceView> m_pCascadeSRV = nullptr;
	ID3D11DepthStencilView* m_pCascadeShadowArrDSV;

private:
	ID3D11DepthStencilView* m_pOutlineDSV;

private:
	ESHADER_TYPE m_eDeferredST = ESHADER_TYPE::ST_DEFERRED;


	_float4x4 m_WorldMatrix = _float4x4();

private:
	wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

	ID3D11ShaderResourceView* m_pShadowSRV = nullptr;

private:
	_bool m_IsDebugMode = false;
	_bool m_IsRenderTargetOn = false;

	_int m_iCurretRenderGroupIdx = 0;

	_bool m_IsHDRRender = true;
	_bool m_IsPBROn = true;
	_bool m_IsAOOn = true;
	_bool m_IsHDROn = true;
	_bool m_IsShadowOn = true;

	_bool m_IsMotionBlurActive = true;

	_bool m_UsingDOF = true;
	_bool m_UsingFog = true;
	_bool m_IsScreenNoiseOn = false;

	_bool m_IsPartitionScreen = false;
	_bool m_IsTargetGroup1Mode = false;
	_bool m_IsEffectOn = true;

	_float m_fAOPower = 7.f;
	_float m_fNoiseTime = 0.f;


/*FadeIn&Out*/
private:

	_bool m_IsMotionBlurOn = false;
	_bool m_IsFadeIn = false;
	_bool m_IsFadeOut = false;
	_bool m_IsBlackOut = false;

	_bool m_IsContinual = false;

	_float m_fFadeDuration = 0.f;
	_float m_fFadeAccTime = 0.f;
	
	_float m_fBlackOutAccTime = 0.f;
	_float m_fBlackOutDuration = 0.f;


private:
	_float2 m_vCascadeSize = { 1280.f * 2.f , 720.f * 2.f };

private:
	_float2 m_vDownSampleSize1 = { 1280.f / 5.f , 720.f / 5.f };
	_float2 m_vDownSampleSize2 = { 1280.f / 25.f , 720.f / 25.f };
	_float2 m_vDownSampleSize3 = { 1280.f / 125.f , 720.f / 125.f };

private:
	_float m_fMotionBlurDuration = 0.f;
	_float m_fMotionBlurAccTime = 0.f;

	_float m_fMotionBlurPower = 30.f;

	_float4x4 m_PrevViewMat = _float4x4::Identity;

	_bool	m_bZoomBlurOn = false;
	_float	m_fZoomBlurDuration = 0.f;
	_float	m_fZoomBlurAccTime = 0.f;
	_float	m_fZoomBlurPower = 30.f;
	_float3	m_vZoomBlurPosW = { 0.f, 0.f, 0.f };
	_bool	m_bZoomBlurOff = false;

private:
	_bool m_IsChromaticAberrationOn = false;

	/*Tone Mapping*/
public:
	static float g_fConstrast, g_fMiddleGray, g_fSaturate;
	static float g_fFogHeight, g_fFogStart, g_fFogEnd;
	static _float3 g_vFogColor;


	wrl::ComPtr<ID3D11ShaderResourceView> m_pLUTTexture = nullptr;

public:
	static shared_ptr<CRenderer> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);

};

END