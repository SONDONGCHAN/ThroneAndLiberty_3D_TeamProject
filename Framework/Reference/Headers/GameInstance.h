#pragma once

#include "Engine_Defines.h"
#include "PipeLine.h"
#include "Renderer.h"
#include "FontMgr.h"

#pragma region Collider
#include "Collider.h"
#include "Bounding_Sphere.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#pragma endregion

BEGIN(Engine)

class ENGINE_DLL CGameInstance
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	~CGameInstance();

public:
	HRESULT InitializeEngine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& _GraphicDesc);
	void TickEngine(_float _fTimeDelta);

	HRESULT Draw();
	HRESULT ClearBackBufferView(const _float4& _vClearColor);
	HRESULT ClearDepthStencilView();
	HRESULT Present();

	void Clear(_uint _iLevelIndex);

	/*For.GraphicDev*/
public:
	wrl::ComPtr<ID3D11Device> GetDeviceInfo();
	wrl::ComPtr<ID3D11DeviceContext> GetDeviceContextInfo();

	/*For. InputDevice*/
public:
	_byte	GetDIKeyState(_ubyte _byKeyID);
	_byte	GetDIMouseState(MOUSEKEYSTATE _eMouse);
	_long	GetDIMouseMove(MOUSEMOVESTATE _eMouseState);

	_bool KeyDown(_ubyte _eKeyID);
	_bool KeyUp(_ubyte _eKeyID);
	_bool KeyPressing(_ubyte _eKeyID);

	_byte MouseDown(MOUSEKEYSTATE _eMouse);
	_byte MousePressing(MOUSEKEYSTATE _eMouse);
	_byte MouseUp(MOUSEKEYSTATE _eMouse);

	/*For. TimeManager*/
public:
	_float ComputeTimeDelta(const wstring& _pTimerTag);
	HRESULT AddTimer(const wstring& _pTimerTag);
	void SetTimerOffset(const wstring& _pTimerTag, _float _fTimeOffset, _float _fDuration);

	/*For. PipeLine*/
public:
	void SetTransform(CPipeLine::ED3DTRANSFORMSTATE _eState, _float4x4 _TransformMatrix);
	void SetShadowProjMat(_float4x4* _shadowProjMat);
	void SetShadowViewMat(_float4x4* _shadowViewMat);
	void SetShadowWorldMat(_float4x4* _shadowWorldMat);
	_float4x4* GetShadowProjMat();
	_float4x4* GetShadowViewMat();
	_float4x4* GetShadowReverseProjMat();
	_float4x4 GetTransformMatrix(CPipeLine::ED3DTRANSFORMSTATE _eState);
	_float4x4 GetTransformMatrixInverse(CPipeLine::ED3DTRANSFORMSTATE _eState);
	_float4x4 GetPrevTransformMatrix(CPipeLine::ED3DTRANSFORMSTATE _eState);
	_float4 GetCamPosition();
	_float3 GetCamLook();
	float DistanceFromCam(const _float3& _vPosition);


	/*For. Level Manager*/
public:
	_uint GetCurrentLevel();
	HRESULT OpenLevel(_uint _iLevelIndex, shared_ptr<class CLevel> _pNewLevel);
	HRESULT OpenLevel2(_uint _iLevelIndex, shared_ptr<class CLevel> _pNewLevel);
	HRESULT OpenLevelForClient(_uint _iLevelIndex);
	HRESULT StartLevel(shared_ptr<class CLevel> _pLevel);
	void SetLoadingLevel(shared_ptr<class CLevel> _pLoadingLevel);
	bool isLoadingLevel();

	void LevelBGMOn();
	void LevelBGMOff();
	void SwitchingBGM();
	void SwitchingBGM2();
	void SwitchingBGM3();

	/*For. Object Manager*/
public:
	HRESULT AddObject(_uint iLevelIndex, const wstring& strLayerTag, shared_ptr<class CGameObject> _pGameObject);
	void ClearLayer(_uint _iLevelIndex, const wstring& _strLayerTag);
	void ClearLevel(_uint _iLevelIndex);

	void SetChangeLevelSignal(_uint _iChangeLevel);
	
	shared_ptr<class CGameObject> GetGameObject(_uint iLevelIndex, const wstring& strLayerTag);
	list<shared_ptr<class CGameObject>>* GetObjectList(_uint _iLevelIndex, const wstring& _strLayerTag);


	/*For. Resource Manager*/
public:
	void LoadLevelResource(_uint _iLevelIdx);
	shared_ptr<class CTexture> GetTexture(string _strTexKey);
	shared_ptr<class CModel> GetModel(string _strTexKey);
	shared_ptr<class CVIBuffer> GetBuffer(string _strBufferKey);

	wrl::ComPtr<ID3D11ShaderResourceView> GetSRV(string _strSRVTag);

	void ClearResource();
	_bool GetStaticResLoadDone();

	/*For. Shader Handler*/
public:
	HRESULT ReadyShader(ESHADER_TYPE _eShderType, _uint iPassIndex, _uint iTechniqueIndex = 0);
	HRESULT BindWVPMatrixPerspective(_float4x4 _worldMat);
	HRESULT BindWVPMatrixOrthoGraphic(_float4x4 _worldMat);
	HRESULT BindRawValue(const _char* _pConstantName, const void* _pData, _uint _iLength);
	HRESULT BindMatrix(const _char* _pConstantName, const _float4x4* _pMatrix);
	HRESULT BindMatrices(const _char* _pConstantName, const _float4x4* _pMatrices, _uint _iNumMatrices);
	HRESULT BindSRV(const _char* _pConstantName, string _strTexTag);
	HRESULT BindSRVDirect(const _char* _pConstantName, wrl::ComPtr<ID3D11ShaderResourceView> _pSRV);
	HRESULT BindFloatVectorArray(const _char* _pConstantName, const _float4* _pVectors, _uint _iNumVectors);
	HRESULT BindUAV(const _char* _pConstantName, wrl::ComPtr<ID3D11UnorderedAccessView> _pUAV);

	HRESULT BeginShader();
	HRESULT BeginCS(_uint _iThreadX, _uint _iThreadY, _uint _iThreadZ);
	HRESULT BeginNonAnimModel(shared_ptr<class CModel> _pModel, _uint _iMeshNum);
	HRESULT BeginAnimModel(shared_ptr<class CModel> _pModel, _uint _iMeshNum);
	HRESULT BeginAnimModelBone(shared_ptr<class CModel> _pModel, _float4x4* BoneMatrices, _uint _iMeshNum);
	HRESULT BeginNonAnimInstance(shared_ptr<class CInstancingModel> _pModel, _uint _iMeshNum);
	HRESULT BeginShaderBuffer(shared_ptr<class CVIBuffer> _pVIBuffer);

	HRESULT BindLightProjMatrix();
	HRESULT BindLightVPMatrix();

	void ChangePassIdx(_uint _iPassIdx);


	/*For. Target Manager*/
public:
	HRESULT AddRenderTarget(const wstring& _strTargetTag, _uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);
	HRESULT AddRenderArrTarget(const wstring& _strTargetTag, _uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor, _uint _iArrSize);
	HRESULT AddMRT(const wstring& _strMRTTag, const wstring& _strTargetTag);
	HRESULT BeginMRT(const wstring& _strMRTTag);
	HRESULT BeginMRT(const wstring& _strMRTTag, ID3D11DepthStencilView* _pDSV);
	HRESULT EndMRT();
	HRESULT BindTargetSRV(const wstring& _strTargetTag, const _char* _pConstantName);
	HRESULT BindTargetUAV(const wstring& _strTargetTag, const _char* _pConstantName);
	HRESULT BindBackBufferSRV( const _char* _pConstantName);
	ComPtr<ID3D11Texture2D> GetTexture2DForTarget(const wstring& _strTargetTag);

	HRESULT CreateTargetSRV(const wstring& _strTargetTag);

//#ifdef _DEBUG
	HRESULT ReadyDebug(const wstring& _strTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY);
	HRESULT RenderMRT(const wstring& _strMRTTag);
//#endif

	/*For. Renderer*/
public:
	HRESULT AddRenderGroup(CRenderer::ERENDER_GROUP _eRenderGroup, shared_ptr<class CGameObject> _pGameObject);
	HRESULT AddUIRenderGroup(CRenderer::ERENDER_UI_GROUP _eUIRenderGroup, shared_ptr<class CGameObject> _pGameObject);
	HRESULT AddCascadeGroup(_uint _iCascadeNum, shared_ptr<class CGameObject> _pGameObject);
	HRESULT SetMotionBlur(_float _fDuration, _float _fBlurPower = 0.03f);
	HRESULT SetZoomBlur(_float _fDuration, _float _fBlurPower, _float3 _vBlurPosW);
	HRESULT SetFadeEffect(_bool _IsFadeOut, _float _fTotalDuration, _bool _IsContinual= false, _float _fBlackOutDuration = 0.f);
	HRESULT SetChromaticAberration(_bool _IsChromaticAberrationOn);
	HRESULT SetScreenNoise(_bool _IsScreenNoiseOn);


	bool isFadeOut();
	bool isBlackOut();

	void SwitchingShadowOption(_bool _IsShadowOn);
	void RendererOptionSwitching(_uint _iLevel);


	void SwitchingToneMappingOption(_bool _IsToneMappingOn);
	void SwitchingPBROption(_bool _IsToneMappingOn);
	void SwitchingAOOptions(_bool _IsAOMappingOn);
	void SwitchingHDROption(_bool _IsHDROn);	
	void SwitchingDOF(_bool _IsDOFOn);	
	void SwitchingFog(_bool _IsFogOn);
	void SetToneMappingProperties(_float _fSaturate, _float _fConstrast, _float _fMiddleGray);

	void SwitchingMotionBlur();

	void DebugColliderSwitch();
	void ChangeAOPower(_float _fAOPower);

	void PartitionScreenHalf(_bool _IsPartition);
	void SwitchiongTargetGroup1Mode();
	void SwitchingZoomBlurOff();
	void SwtichingEffectOn();

//#ifdef _DEBUG
	void DebugRenderSwitch();
	void SetRenderGourpIdx(_int _iGroupIdx);
	HRESULT AddDebugComponent(shared_ptr<class CComponent> _pComponent);
//#endif // _DEBUG



	/*For. PhysX Manager*/
public:
	PxPhysics* GetPhysics() const;
	PxScene* GetScene() const;
	void SetSimultation(bool _isSimulation);
	shared_ptr<class CRigidBody> CloneRigidBody(void* _pArg);

	/*For. Light Manager*/
public:
	HRESULT AddLight(const LIGHT_DESC& _LightDesc, _uint* _iIndex = nullptr);
	LIGHT_DESC* GetLightDesc(_uint _iIndex);
	HRESULT RenderLight();
	void SetLightEnabled(_uint _iIndex, _bool m_IsEnabled);
	_float3 GetLightDir();
	void DeleteLight(_int _iLightIdx);

	/*For. Sound Manager*/
public:
	void PlaySoundW(const string _strSoundKey, float _fVolume);
	void PlaySoundL(const string _strSoundKey, float _fVolume);
	void PlaySoundNonOverLap(const string _strSoundKey, float _fVolume);
	void PlayBGM(string _strSoundKey, float _fVolume);
	void PlayBGMFadeIn(string _strSoundKey, _float _fTargetVolume, _float _fDurTime);
	void StopSound(const string _strSoundKey);
	void StopSoundFadeOut(const string _strSoundKey, _float _fDurTime);
	void StopAll();
	void SetChannelVolume(string _strSoundKey, float fVolume);

	/*For. Random Picker*/
public:
	_int PickRandomInt(_int _iStartLimit, _int _iEndLimit, _bool _bSort = false);
	_float PickRandomFloat(_float _fStartLimit, _float _fEndLimit, _bool _bSort = false);

	/*For. Json Parser*/
	void WriteJson(Json::Value _value, const wstring& _strSavePath);
	Json::Value ReadJson(const wstring& _strReadPath);

	/*For. PickingMgr*/
public:
	void GetWorldRay(_float3& _vRayPos, _float3& _vRayDir);
	HRESULT ComputePicking(_uint _iTarget);
	//shared_ptr<CGameObject> GetPickedObject();
	_uint GetPickedObjectID();
	_uint GetIdxPickedVertex();
	_float3 GetWorldPosPicked();

	/*For. Calculator*/
public:
	_float Lerp(_float _fStart, _float _fEnd, _float _fRatio);

	/* For. Easing */
public:
	_float	Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float _fStartValue, _float _fEndValue);
	_float2	Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float2 _fStartValue, _float2 _fEndValue);
	_float3	Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float3 _fStartValue, _float3 _fEndValue);
	_float4	Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float4 _fStartValue, _float4 _fEndValue);

	const char** GetEaseTypeKeys();

/* For. Font */
public:
	HRESULT AddFont(const string& _strFontTag, const wstring& _strFontFilePath);
	HRESULT RenderFont(const string& _strFontTag, const wstring& _strText, const _float2& _vPosition, CCustomFont::EFONT_ALIGNMENT _eFontAlignment, _color _vColor = { 1.f, 1.f, 1.f, 1.f }, _float _fRotation = 0.f, const _float2& _vOrigin = _float2(0.f, 0.f), _float _fScale = 1.f);
	shared_ptr<CCustomFont> FindFont(const string& _strFontTag);
	wstring StringToWString(const string& _inString);
	string WStringToString(const wstring& _inWString);

	/*For. Frustum*/
public:
	ContainmentType isInWorldSpace(const BoundingBox& _BoundingBox);	// DISJOINT = 0, INTERSECTS = 1, CONTAINS = 2
	bool isInWorldSpace(FXMVECTOR _vPosition, float _fRadius = 0.0f);
	void MakeLocalSpace(FXMMATRIX _WorldMatrix);
	bool isInLocalSpace(FXMMATRIX _WorldMatrix, FXMVECTOR _vPosition, float _fRadius = 0.0f);

	/*For. Octree*/
public:
	void DeleteOctree();
	HRESULT CreateOctree(const _float3& _vCenter, const float& _fHalfWidth = 256.0f, const int& _iDepthLimit = 4);
	bool AddStaticObject(shared_ptr<class CGameObject> _pGameObject, const _float3& _vPoint, const float& _fRadius = 0.0f);
	bool AddStaticInstance(const string& _strModelTag, shared_ptr<class CGameObject> _pGameObject, const _float3& _vPoint, const _float4x4& _vWorldMatrix, const float& _fRadius = 0.0f);

	/*For . Video Manager*/
public:
	void PlayVideo(wstring _strPath);


private:
	shared_ptr<class CGraphicDev> m_pGraphicDev = nullptr;
	shared_ptr<class CInputDevice> m_pInputDev = nullptr;
	shared_ptr<class CTimerMgr> m_pTimeMgr = nullptr;
	shared_ptr<CPipeLine> m_pPipeLine = nullptr;
	shared_ptr<class CLevelMgr> m_pLevelMgr = nullptr;
	shared_ptr<class CObjectMgr> m_pObjectMgr = nullptr;
	shared_ptr<class CShaderHandler> m_pShaderHandler = nullptr;
	shared_ptr<class CTargetMgr> m_pTargetMgr = nullptr;
	shared_ptr<CRenderer> m_pRenderer = nullptr;
	shared_ptr<class CResourceMgr> m_pResourceMgr = nullptr;
	shared_ptr<class CPhysXMgr> m_pPhysXMgr = nullptr;
	shared_ptr<class CLightMgr> m_pLightMgr = nullptr;
	shared_ptr<class CRandomPicker> m_pRandomPicker = nullptr;
	shared_ptr<class CJsonParsingMgr> m_pJsonParser = nullptr;
	shared_ptr<class CPickingMgr> m_pPickingMgr = nullptr;
	shared_ptr<class CCalculator> m_pCalculator = nullptr;
	shared_ptr<class CEasing>	m_pEasing = nullptr;
	shared_ptr<class CFontMgr> m_pFontMgr = nullptr;
	shared_ptr<class CFrustum> m_pFrustum = nullptr;
	shared_ptr<class COctree> m_pOctree = nullptr;
	shared_ptr<class CSoundMgr> m_pSoundMgr = nullptr;
	shared_ptr<class CVideoMgr> m_pVideoMgr = nullptr;

public:
	void Free();

};

END