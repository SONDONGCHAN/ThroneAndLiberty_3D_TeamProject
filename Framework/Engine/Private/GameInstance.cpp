#include "GameInstance.h"
#include "GraphicDev.h"
#include "InputDevice.h"
#include "TimerMgr.h"
#include "PipeLine.h"
#include "LevelMgr.h"
#include "ShaderHandler.h"
#include "TargetMgr.h"
#include "ObjectMgr.h"
#include "ResourceMgr.h"
#include "PhysXMgr.h"
#include "LightMgr.h"
#include "SoundMgr.h"
#include "RandomPicker.h"
#include "ThreadPool.h"
#include "JsonParsingMgr.h"
#include "PickingMgr.h"
#include "Calculator.h"
#include "Easing.h"
#include "Frustum.h"
#include "Octree.h"
#include "VideoMgr.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

CGameInstance::~CGameInstance()
{
	Free();
}

HRESULT CGameInstance::InitializeEngine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& _GraphicDesc)
{
	m_pGraphicDev = CGraphicDev::Create(_GraphicDesc);

	if (!m_pGraphicDev)
		return E_FAIL;

	m_pInputDev = CInputDevice::Create(hInst, _GraphicDesc.hWnd);

	if (!m_pInputDev)
		return E_FAIL;

	m_pPhysXMgr = CPhysXMgr::Create();

	if (!m_pPhysXMgr)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();

	if (!m_pPipeLine)
		return E_FAIL;

	m_pTimeMgr = CTimerMgr::Create();

	if (!m_pTimeMgr)
		return E_FAIL;

	m_pLevelMgr = CLevelMgr::Create();

	if (!m_pLevelMgr)
		return E_FAIL;

	m_pObjectMgr = CObjectMgr::Create(iNumLevels);

	if (!m_pObjectMgr)
		return E_FAIL;


	m_pShaderHandler = CShaderHandler::Create(m_pGraphicDev->GetDeviceInfo(), m_pGraphicDev->GetDeviceContextInfo(), (_float)_GraphicDesc.iWinSizeX, (_float)_GraphicDesc.iWinSizeY);

	if (!m_pShaderHandler)
		return E_FAIL;

	m_pResourceMgr = CResourceMgr::Create(m_pGraphicDev->GetDeviceInfo(), m_pGraphicDev->GetDeviceContextInfo());

	if (!m_pResourceMgr)
		return E_FAIL;

	m_pTargetMgr = CTargetMgr::Create(m_pGraphicDev->GetDeviceInfo(), m_pGraphicDev->GetDeviceContextInfo());

	if (!m_pTargetMgr)
		return E_FAIL;

	m_pTargetMgr->CreateCopyBuffer(_GraphicDesc.iWinSizeX, _GraphicDesc.iWinSizeY);

	m_pSoundMgr = CSoundMgr::Create();

	if (!m_pSoundMgr) {
		return E_FAIL;
	}

	m_pRenderer = CRenderer::Create(m_pGraphicDev->GetDeviceInfo(), m_pGraphicDev->GetDeviceContextInfo());

	if (!m_pRenderer)
		return E_FAIL;

	m_pLightMgr = CLightMgr::Create(m_pGraphicDev->GetDeviceInfo(), m_pGraphicDev->GetDeviceContextInfo());

	if (!m_pLightMgr)
		return E_FAIL;

	m_pRandomPicker = CRandomPicker::Create();

	if (!m_pRandomPicker)
		return E_FAIL;

	m_pJsonParser = CJsonParsingMgr::Create();

	if (!m_pJsonParser)
		return E_FAIL;

	m_pPickingMgr = CPickingMgr::Create(_GraphicDesc);

	if (!m_pPickingMgr)
		return E_FAIL;

	m_pCalculator = CCalculator::Create();

	if (!m_pCalculator)
		return E_FAIL;

	m_pEasing = CEasing::Create();

	if (!m_pEasing)
		return E_FAIL;

	m_pFontMgr = CFontMgr::Create(m_pGraphicDev->GetDeviceInfo(), m_pGraphicDev->GetDeviceContextInfo());

	if (!m_pFontMgr)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();

	if (!m_pFrustum)
		return E_FAIL;

	m_pVideoMgr = CVideoMgr::Create(_GraphicDesc.hWnd);

	if (!m_pVideoMgr)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::TickEngine(_float _fTimeDelta)
{
	if (!m_pLevelMgr ||
		!m_pObjectMgr)
		return;

	m_pLevelMgr->Tick(_fTimeDelta);					// etc


	m_pInputDev->Tick();							// Renew Key Input

	m_pObjectMgr->PriorityTick(_fTimeDelta);		// ObjPriorityTick *

	m_pPipeLine->Tick();							// Renew View,Proj

	m_pFrustum->Tick();								// Renew Frustum

	if (m_pOctree)
		m_pOctree->Culling();						// Renew CulledBox using Frustum

	if (m_pOctree)
		m_pOctree->PriorityTick(_fTimeDelta);		// Culled ObjPriorityTick *

#ifdef _DEBUG
	m_pPickingMgr->Tick(_fTimeDelta);				// ?
#endif

	m_pObjectMgr->Tick(_fTimeDelta);				// ObjTick *

	if (m_pOctree)
		m_pOctree->Tick(_fTimeDelta);				// Culled ObjTick *

	m_pPhysXMgr->Tick(_fTimeDelta);					// PhysX Simulation

	m_pObjectMgr->LateTick(_fTimeDelta);			// ObjLateTick *
	m_pLightMgr->LateTick();						// Light LateTick *

	if (m_pOctree)
		m_pOctree->LateTick(_fTimeDelta);			// Culled ObjLateTick *

	m_pSoundMgr->Tick(_fTimeDelta);
}

HRESULT CGameInstance::Draw()
{
	if (nullptr == m_pGraphicDev)
		return E_FAIL;

	m_pRenderer->Render();
	m_pLevelMgr->Render();
	return S_OK;
}

HRESULT CGameInstance::ClearBackBufferView(const _float4& _vClearColor)
{
	if (!m_pGraphicDev)
		return E_FAIL;

	return m_pGraphicDev->ClearBackBufferView(_vClearColor);
}

HRESULT CGameInstance::ClearDepthStencilView()
{
	if (!m_pGraphicDev)
		return E_FAIL;

	return m_pGraphicDev->ClearDepthStencilView();
}

HRESULT CGameInstance::Present()
{
	if (!m_pGraphicDev)
		return E_FAIL;

	return m_pGraphicDev->Present();
}

void CGameInstance::Clear(_uint _iLevelIndex)
{
	if (nullptr == m_pObjectMgr)
		return;

	m_pObjectMgr->Clear(_iLevelIndex);

}

wrl::ComPtr<ID3D11Device> CGameInstance::GetDeviceInfo()
{
	return m_pGraphicDev->GetDeviceInfo();
}

wrl::ComPtr<ID3D11DeviceContext> CGameInstance::GetDeviceContextInfo()
{
	return m_pGraphicDev->GetDeviceContextInfo();
}

_byte CGameInstance::GetDIKeyState(_ubyte _byKeyID)
{
	return m_pInputDev->GetDIKeyState(_byKeyID);
}

_byte CGameInstance::GetDIMouseState(MOUSEKEYSTATE _eMouse)
{
	if (!m_pInputDev)
		return _byte();

	return m_pInputDev->GetDIMouseState(_eMouse);
}

_long CGameInstance::GetDIMouseMove(MOUSEMOVESTATE _eMouseState)
{
	if (!m_pInputDev)
		return _long();

	return m_pInputDev->GetDIMouseMove(_eMouseState);
}

_bool CGameInstance::KeyDown(_ubyte _eKeyID)
{
	if (!m_pInputDev)
		return false;

	return m_pInputDev->KeyDown(_eKeyID);
}

_bool CGameInstance::KeyUp(_ubyte _eKeyID)
{
	if (!m_pInputDev)
		return false;

	return m_pInputDev->KeyUp(_eKeyID);
}

_bool CGameInstance::KeyPressing(_ubyte _eKeyID)
{
	if (!m_pInputDev)
		return false;

	return m_pInputDev->KeyPressing(_eKeyID);
}

_byte CGameInstance::MouseDown(MOUSEKEYSTATE _eMouse)
{
	return m_pInputDev->MouseDown(_eMouse);
}

_byte CGameInstance::MousePressing(MOUSEKEYSTATE _eMouse)
{
	return m_pInputDev->MousePressing(_eMouse);
}

_byte CGameInstance::MouseUp(MOUSEKEYSTATE _eMouse)
{
	return m_pInputDev->MouseUp(_eMouse);
}

_float CGameInstance::ComputeTimeDelta(const wstring& _pTimerTag)
{
	if (!m_pTimeMgr)
		return 0.f;

	return m_pTimeMgr->ComputeTimeDelta(_pTimerTag);
}

HRESULT CGameInstance::AddTimer(const wstring& _pTimerTag)
{
	if (!m_pTimeMgr)
		return E_FAIL;

	return m_pTimeMgr->AddTimer(_pTimerTag);
}

void CGameInstance::SetTimerOffset(const wstring& _pTimerTag, _float _fTimeOffset, _float _fDuration)
{
	if (!m_pTimeMgr)
		return;

	m_pTimeMgr->SetTimerOffset(_pTimerTag, _fTimeOffset, _fDuration);

}

void CGameInstance::SetTransform(CPipeLine::ED3DTRANSFORMSTATE _eState, _float4x4 _TransformMatrix)
{
	if (!m_pPipeLine)
		return;

	m_pPipeLine->SetTransform(_eState, _TransformMatrix);
}

void CGameInstance::SetShadowProjMat(_float4x4* _shadowProjMat)
{
    m_pPipeLine->SetShadowProjMat(_shadowProjMat);
}

void CGameInstance::SetShadowViewMat(_float4x4* _shadowViewMat)
{
    m_pPipeLine->SetShadowViewMat(_shadowViewMat);
}

void CGameInstance::SetShadowWorldMat(_float4x4* _shadowWorldMat)
{
    m_pPipeLine->SetShadowWorldMat(_shadowWorldMat);
}

_float4x4* CGameInstance::GetShadowProjMat()
{
    return  m_pPipeLine->GetShadowProjMat();
}

_float4x4* CGameInstance::GetShadowViewMat()
{
    return  m_pPipeLine->GetShadowViewMat();
}

_float4x4* CGameInstance::GetShadowReverseProjMat()
{
	return  m_pPipeLine->GetShadowWorldMat();
}

_float4x4 CGameInstance::GetTransformMatrix(CPipeLine::ED3DTRANSFORMSTATE _eState)
{
	if (!m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->GetTransformMatrix(_eState);
}

_float4x4 CGameInstance::GetTransformMatrixInverse(CPipeLine::ED3DTRANSFORMSTATE _eState)
{
	if (!m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->GetTransformMatrixInverse(_eState);
}

_float4x4 CGameInstance::GetPrevTransformMatrix(CPipeLine::ED3DTRANSFORMSTATE _eState)
{
	return m_pPipeLine->GetPrevTransformMatrix(_eState);
}

_float4 CGameInstance::GetCamPosition()
{
	if (!m_pPipeLine)
		return _float4();

	return m_pPipeLine->GetCamPosition();
}

_float3 CGameInstance::GetCamLook()
{
	if (!m_pPipeLine)
		return _float3();

	return m_pPipeLine->GetCamLook();
}

float CGameInstance::DistanceFromCam(const _float3& _vPosition)
{
	return XMVector3Length(GetCamPosition() - _vPosition).m128_f32[0];
}

_uint CGameInstance::GetCurrentLevel()
{
	if (!m_pLevelMgr)
		return 0;

	return m_pLevelMgr->GetCurrentLevel();
}

HRESULT CGameInstance::OpenLevel(_uint _iLevelIndex, shared_ptr<class CLevel> _pNewLevel)
{
	if (!m_pLevelMgr)
		return E_FAIL;

	return m_pLevelMgr->OpenLevel(_iLevelIndex, _pNewLevel);
}

HRESULT CGameInstance::OpenLevel2(_uint _iLevelIndex, shared_ptr<class CLevel> _pNewLevel)
{
	if (!m_pLevelMgr)
		return E_FAIL;

	return m_pLevelMgr->OpenLevel2(_iLevelIndex, _pNewLevel);
}

HRESULT CGameInstance::OpenLevelForClient(_uint _iLevelIndex)
{
	if (!m_pLevelMgr)
		return E_FAIL;

	m_pRenderer->Clear();
	m_pLightMgr->ClearAllLight();

	return m_pLevelMgr->OpenLevelForClient(_iLevelIndex);
}

HRESULT CGameInstance::StartLevel(shared_ptr<class CLevel> _pLevel)
{
	return  m_pLevelMgr->StartLevel(_pLevel);
}

void CGameInstance::SetLoadingLevel(shared_ptr<CLevel> _pLoadingLevel)
{
	if (!m_pLevelMgr)
		return;

	m_pLevelMgr->SetLoadingLevel(_pLoadingLevel);
}

bool CGameInstance::isLoadingLevel()
{
	if (!m_pLevelMgr)
		return false;

	return m_pLevelMgr->isLoadingLevel();
}

void CGameInstance::LevelBGMOn()
{
	m_pLevelMgr->LevelBGMOn();

}

void CGameInstance::LevelBGMOff()
{
	m_pLevelMgr->LevelBGMOff();
}

void CGameInstance::SwitchingBGM()
{
	m_pLevelMgr->SwitchingBGM();
}

void CGameInstance::SwitchingBGM2()
{
	m_pLevelMgr->SwitchingBGM2();
}

void CGameInstance::SwitchingBGM3()
{
	m_pLevelMgr->SwitchingBGM3();
}

HRESULT CGameInstance::AddObject(_uint iLevelIndex, const wstring& strLayerTag, shared_ptr<class CGameObject> _pGameObject)
{
	return m_pObjectMgr->AddObject(iLevelIndex, strLayerTag, _pGameObject);
}

void CGameInstance::ClearLayer(_uint _iLevelIndex, const wstring& _strLayerTag)
{
	m_pObjectMgr->ClearLayer(_iLevelIndex, _strLayerTag);
}

void CGameInstance::ClearLevel(_uint _iLevelIndex)
{
	m_pObjectMgr->Clear(_iLevelIndex);
}

void CGameInstance::SetChangeLevelSignal( _uint _iChangeLevel)
{
	m_pObjectMgr->SetChangeLevelSignal( _iChangeLevel);

}

shared_ptr<CGameObject> CGameInstance::GetGameObject(_uint iLevelIndex, const wstring& strLayerTag)
{
	return m_pObjectMgr->GetGameObject(iLevelIndex, strLayerTag);
}

list<shared_ptr<class CGameObject>>* CGameInstance::GetObjectList(_uint _iLevelIndex, const wstring& _strLayerTag)
{
    return m_pObjectMgr->GetObjectList(_iLevelIndex, _strLayerTag);
}

void CGameInstance::LoadLevelResource(_uint _iLevelIdx)
{ 
	if (!m_pResourceMgr)
		return;

	m_pResourceMgr->LoadLevelResource(_iLevelIdx);
}

shared_ptr<class CTexture> CGameInstance::GetTexture(string _strTexKey)
{
	return m_pResourceMgr->GetTexture(_strTexKey);
}

shared_ptr<class CModel> CGameInstance::GetModel(string _strTexKey)
{
	return m_pResourceMgr->GetModel(_strTexKey);
}

shared_ptr<class CVIBuffer> CGameInstance::GetBuffer(string _strBufferKey)
{
	return m_pResourceMgr->GetBuffer(_strBufferKey);
}

wrl::ComPtr<ID3D11ShaderResourceView> CGameInstance::GetSRV(string _strSRVTag)
{
	return m_pResourceMgr->GetSRV(_strSRVTag);
}

void CGameInstance::ClearResource()
{
	return m_pResourceMgr->ClearResource();
}

_bool CGameInstance::GetStaticResLoadDone()
{
	return m_pResourceMgr->GetStaticResLoadDone();
}

HRESULT CGameInstance::ReadyShader(ESHADER_TYPE _eShderType, _uint iPassIndex, _uint iTechniqueIndex)
{
	return m_pShaderHandler->ReadyShader(_eShderType, iPassIndex, iTechniqueIndex);
}

HRESULT CGameInstance::BindWVPMatrixPerspective(_float4x4 _worldMat)
{
	return m_pShaderHandler->BindWVPMatrixPerspective(_worldMat);
}

HRESULT CGameInstance::BindWVPMatrixOrthoGraphic(_float4x4 _worldMat)
{
	return m_pShaderHandler->BindWVPMatrixOrthographic(_worldMat);
}

HRESULT CGameInstance::BindRawValue(const _char* _pConstantName, const void* _pData, _uint _iLength)
{
	return m_pShaderHandler->BindRawValue(_pConstantName, _pData, _iLength);
}

HRESULT CGameInstance::BindMatrix(const _char* _pConstantName, const _float4x4* _pMatrix)
{
	return m_pShaderHandler->BindMatrix(_pConstantName, _pMatrix);
}

HRESULT CGameInstance::BindMatrices(const _char* _pConstantName, const _float4x4* _pMatrices, _uint _iNumMatrices)
{
	return m_pShaderHandler->BindMatrices(_pConstantName, _pMatrices, _iNumMatrices);
}

HRESULT CGameInstance::BindSRV(const _char* _pConstantName, string _strTexTag)
{
	return m_pShaderHandler->BindSRV(_pConstantName, _strTexTag);
}

HRESULT CGameInstance::BindSRVDirect(const _char* _pConstantName, wrl::ComPtr<ID3D11ShaderResourceView> _pSRV)
{
	return m_pShaderHandler->BindSRVDirect(_pConstantName, _pSRV);
}

HRESULT CGameInstance::BindFloatVectorArray(const _char* _pConstantName, const _float4* _pVectors, _uint _iNumVectors)
{
	return m_pShaderHandler->BindFloatVectorArray(_pConstantName, _pVectors, _iNumVectors);
}

HRESULT CGameInstance::BindUAV(const _char* _pConstantName, wrl::ComPtr<ID3D11UnorderedAccessView> _pUAV)
{
	return m_pShaderHandler->BindUAV(_pConstantName, _pUAV);
}

HRESULT CGameInstance::BeginShader()
{
	return m_pShaderHandler->BeginShader();
}

HRESULT CGameInstance::BeginCS(_uint _iThreadX, _uint _iThreadY, _uint _iThreadZ)
{
	return m_pShaderHandler->BeginCS(_iThreadX, _iThreadY, _iThreadZ);
}

HRESULT CGameInstance::BeginNonAnimModel(shared_ptr<class CModel> _pModel, _uint _iMeshNum)
{
	return m_pShaderHandler->BeginNonAnimModel(_pModel, _iMeshNum);
}

HRESULT CGameInstance::BeginAnimModel(shared_ptr<class CModel> _pModel, _uint _iMeshNum)
{
	return m_pShaderHandler->BeginAnimModel(_pModel, _iMeshNum);
}

HRESULT CGameInstance::BeginAnimModelBone(shared_ptr<class CModel> _pModel, _float4x4* BoneMatrices, _uint _iMeshNum)
{
	return m_pShaderHandler->BeginAnimModelBone(_pModel, BoneMatrices, _iMeshNum);
}

HRESULT CGameInstance::BeginNonAnimInstance(shared_ptr<class CInstancingModel> _pModel, _uint _iMeshNum)
{
    return m_pShaderHandler->BeginNonAnimInstance(_pModel, _iMeshNum);
}

HRESULT CGameInstance::BeginShaderBuffer(shared_ptr<class CVIBuffer> _pVIBuffer)
{
	return m_pShaderHandler->BeginShaderBuffer(_pVIBuffer);
}

HRESULT CGameInstance::BindLightProjMatrix()
{
    return m_pShaderHandler->BindLightProjMatrix();
}

HRESULT CGameInstance::BindLightVPMatrix()
{
    return m_pShaderHandler->BindLightVPMatrix();
}

void CGameInstance::ChangePassIdx(_uint _iPassIdx)
{
	m_pShaderHandler->ChangePassIdx(_iPassIdx);
}

HRESULT CGameInstance::AddRenderTarget(const wstring& _strTargetTag, _uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
	return m_pTargetMgr->AddRenderTarget(_strTargetTag, _iSizeX, _iSizeY, _ePixelFormat, _vClearColor);
}

HRESULT CGameInstance::AddRenderArrTarget(const wstring& _strTargetTag, _uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor, _uint _iArrSize)
{
    return m_pTargetMgr->AddRenderTargetTex2DArr(_strTargetTag, _iSizeX, _iSizeY, _ePixelFormat, _vClearColor, _iArrSize);
}

HRESULT CGameInstance::AddMRT(const wstring& _strMRTTag, const wstring& _strTargetTag)
{
	return m_pTargetMgr->AddMRT(_strMRTTag, _strTargetTag);
}

HRESULT CGameInstance::BeginMRT(const wstring& _strMRTTag)
{
	return m_pTargetMgr->BeginMRT(_strMRTTag);
}

HRESULT CGameInstance::BeginMRT(const wstring& _strMRTTag, ID3D11DepthStencilView* _pDSV)
{
    return m_pTargetMgr->BeginMRT(_strMRTTag, _pDSV);
}

HRESULT CGameInstance::EndMRT()
{
	return m_pTargetMgr->EndMRT();
}

HRESULT CGameInstance::BindTargetSRV(const wstring& _strTargetTag, const _char* _pConstantName)
{
	return m_pTargetMgr->BindSRV(_strTargetTag, _pConstantName);
}

HRESULT CGameInstance::BindTargetUAV(const wstring& _strTargetTag, const _char* _pConstantName)
{
	return m_pTargetMgr->BindUAV(_strTargetTag, _pConstantName);
}

HRESULT CGameInstance::BindBackBufferSRV(const _char* _pConstantName)
{
	return m_pTargetMgr->BindBackBufferSRV(_pConstantName);
}

ComPtr<ID3D11Texture2D> CGameInstance::GetTexture2DForTarget(const wstring& _strTargetTag)
{
    return m_pTargetMgr->GetTexture2DForTarget(_strTargetTag);
}

HRESULT CGameInstance::CreateTargetSRV(const wstring& _strTargetTag)
{
	return m_pTargetMgr->CreateTargetSRV(_strTargetTag);
}

//#ifdef _DEBUG
HRESULT CGameInstance::ReadyDebug(const wstring& _strTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY)
{
	return m_pTargetMgr->ReadyDebug(_strTargetTag, _fX, _fY, _fSizeX, _fSizeY);
}

HRESULT CGameInstance::RenderMRT(const wstring& _strMRTTag)
{
	return m_pTargetMgr->RenderMRT(_strMRTTag);
}
//#endif

HRESULT CGameInstance::AddRenderGroup(CRenderer::ERENDER_GROUP _eRenderGroup, shared_ptr<class CGameObject> _pGameObject)
{
	return m_pRenderer->AddRenderGroup(_eRenderGroup, _pGameObject);
}

HRESULT CGameInstance::AddUIRenderGroup(CRenderer::ERENDER_UI_GROUP _eUIRenderGroup, shared_ptr<class CGameObject> _pGameObject)
{
	return m_pRenderer->AddUIRenderGroup(_eUIRenderGroup, _pGameObject);
}

HRESULT CGameInstance::AddCascadeGroup(_uint _iCascadeNum, shared_ptr<class CGameObject> _pGameObject)
{
    return m_pRenderer->AddCascadeGroup(_iCascadeNum, _pGameObject);
}

HRESULT CGameInstance::SetMotionBlur(_float _fDuration, _float _fBlurPower)
{
	return m_pRenderer->SetMotionBlur(_fDuration, _fBlurPower);
}

HRESULT CGameInstance::SetZoomBlur(_float _fDuration, _float _fBlurPower, _float3 _vBlurPosW)
{
	return m_pRenderer->SetZoomBlur(_fDuration, _fBlurPower, _vBlurPosW);
}

HRESULT CGameInstance::SetFadeEffect(_bool _IsFadeOut, _float _fTotalDuration, _bool _IsContinual, _float _fBlackOutDuration)
{
	return m_pRenderer->SetFadeEffect(_IsFadeOut, _fTotalDuration, _IsContinual, _fBlackOutDuration);
}

bool CGameInstance::isFadeOut()
{
	return m_pRenderer->isFadeOut();
}

bool CGameInstance::isBlackOut()
{
	return m_pRenderer->isBlackOut();
}

HRESULT CGameInstance::SetChromaticAberration(_bool _IsChromaticAberrationOn)
{
	return m_pRenderer->SetChromaticAberration(_IsChromaticAberrationOn);
}

HRESULT CGameInstance::SetScreenNoise(_bool _IsScreenNoiseOn)
{
	return m_pRenderer->SetScreenNoise(_IsScreenNoiseOn);
}

void CGameInstance::RendererOptionSwitching(_uint _iLevel)
{
	m_pRenderer->RendererOptionSwitching(_iLevel);
}

void CGameInstance::SwitchingToneMappingOption(_bool _IsToneMappingOn)
{
	m_pRenderer->SwitchingToneMappingOption(_IsToneMappingOn);
}

void CGameInstance::SwitchingPBROption(_bool _IsToneMappingOn)
{
	m_pRenderer->SwitchingPBROption(_IsToneMappingOn);
}

void CGameInstance::SwitchingAOOptions(_bool _IsAOMappingOn)
{
	m_pRenderer->SwitchingAOOption(_IsAOMappingOn);
}

void CGameInstance::SwitchingHDROption(_bool _IsHDROn)
{
	m_pRenderer->SwitchingHDROption(_IsHDROn);
}

void CGameInstance::SwitchingDOF(_bool _IsDOFOn)
{
	m_pRenderer->SwitchingDOF(_IsDOFOn);
}

void CGameInstance::SwitchingFog(_bool _IsFogOn)
{
	m_pRenderer->SwitchingFog(_IsFogOn);
}

void CGameInstance::SwitchingShadowOption(_bool _IsShadowOn)
{
	m_pRenderer->SwitchingShadowOption(_IsShadowOn);
}


void CGameInstance::SetToneMappingProperties(_float _fSaturate, _float _fConstrast, _float _fMiddleGray)
{
	m_pRenderer->g_fSaturate = _fSaturate;
	m_pRenderer->g_fMiddleGray = _fMiddleGray;
	m_pRenderer->g_fConstrast = _fConstrast;

}

void CGameInstance::SwitchingMotionBlur()
{
	m_pRenderer->SwitchingMotionBlur();

}

void CGameInstance::DebugColliderSwitch()
{
	m_pRenderer->DebugColliderSwitch();

}

void CGameInstance::ChangeAOPower(_float _fAOPower)
{
	m_pRenderer->ChangeAOPower(_fAOPower);
}

void CGameInstance::PartitionScreenHalf(_bool _IsPartition)
{
	m_pRenderer->PartitionScreenHalf(_IsPartition);
}

void CGameInstance::SwitchiongTargetGroup1Mode()
{
	m_pRenderer->SwitchingTargetGroup1Mode();
}

void CGameInstance::SwitchingZoomBlurOff()
{
	m_pRenderer->SwitchingZoomBlurOff();
}

void CGameInstance::SwtichingEffectOn()
{
	m_pRenderer->SwitchingEffectOption();
}


//#ifdef _DEBUG
void CGameInstance::DebugRenderSwitch()
{
	return m_pRenderer->DebugRenderSwitch();
}

void CGameInstance::SetRenderGourpIdx(_int _iGroupIdx)
{
	if (_iGroupIdx != 0 && _iGroupIdx != 1) {
		return;
	}

	m_pRenderer->SetRenderTargetGroup(_iGroupIdx);
}

HRESULT CGameInstance::AddDebugComponent(shared_ptr<class CComponent> _pComponent)
{
	if (!m_pRenderer)
		return E_FAIL;

	return m_pRenderer->AddDebugComponent(_pComponent);
}
//#endif


PxPhysics* CGameInstance::GetPhysics() const
{
	if (!m_pPhysXMgr)
		return nullptr;

	return m_pPhysXMgr->GetPhysics();
}

PxScene* CGameInstance::GetScene() const
{
	if (!m_pPhysXMgr)
		return nullptr;

	return m_pPhysXMgr->GetScene();
}

void CGameInstance::SetSimultation(bool _isSimulation)
{
	if (!m_pPhysXMgr)
		return;

	m_pPhysXMgr->SetSimultation(_isSimulation);
}

shared_ptr<CRigidBody> CGameInstance::CloneRigidBody(void* _pArg)
{
	if (!m_pPhysXMgr)
		return nullptr;

	return m_pPhysXMgr->CloneRigidBody(_pArg);
}

HRESULT CGameInstance::AddLight(const LIGHT_DESC& _LightDesc, _uint* _iIndex)
{
	return m_pLightMgr->AddLight(_LightDesc, _iIndex);
}

LIGHT_DESC* CGameInstance::GetLightDesc(_uint _iIndex)
{
	return m_pLightMgr->GetLightDesc(_iIndex);
}

HRESULT CGameInstance::RenderLight()
{
	return m_pLightMgr->Render();
}

void CGameInstance::SetLightEnabled(_uint _iIndex, _bool m_IsEnabled)
{
	m_pLightMgr->SetLightEnable(_iIndex, m_IsEnabled);
}

_float3 CGameInstance::GetLightDir()
{
    return m_pLightMgr->GetLightDir();
}

void CGameInstance::DeleteLight(_int _iLightIdx)
{
	return m_pLightMgr->DeleteLight(_iLightIdx);

}

void CGameInstance::PlaySoundW(const string _strSoundKey, float _fVolume)
{
	if (!m_pSoundMgr)
		return;

	m_pSoundMgr->PlaySoundW(_strSoundKey, _fVolume);

}

void CGameInstance::PlaySoundL(const string _strSoundKey, float _fVolume)
{
	if (!m_pSoundMgr)
		return;

	m_pSoundMgr->PlaySoundL(_strSoundKey, _fVolume);
}

void CGameInstance::PlaySoundNonOverLap(const string _strSoundKey, float _fVolume)
{
	if (!m_pSoundMgr)
		return;

	m_pSoundMgr->PlaySoundNonOverLap(_strSoundKey, _fVolume);
}

void CGameInstance::PlayBGM(string _strSoundKey, float _fVolume)
{
	if (!m_pSoundMgr)
		return;

	m_pSoundMgr->PlayBGM(_strSoundKey, _fVolume);

}

void CGameInstance::PlayBGMFadeIn(string _strSoundKey, _float _fTargetVolume, _float _fDurTime)
{
	if (!m_pSoundMgr)
		return;

	m_pSoundMgr->PlayBGMFadeIn(_strSoundKey, _fTargetVolume, _fDurTime);
}

void CGameInstance::StopSound(const string _strSoundKey){
	if (!m_pSoundMgr)
		return;

	m_pSoundMgr->StopSound(_strSoundKey);

}

void CGameInstance::StopSoundFadeOut(const string _strSoundKey, _float _fDurTime)
{
	if (!m_pSoundMgr)
		return;

	m_pSoundMgr->StopSoundFadeOut(_strSoundKey, _fDurTime);
}

void CGameInstance::StopAll()
{

	if (!m_pSoundMgr)
		return;

	m_pSoundMgr->StopAll();
}

void CGameInstance::SetChannelVolume(string _strSoundKey, float fVolume)
{
	if (!m_pSoundMgr)
		return;

	m_pSoundMgr->SetChannelVolume(_strSoundKey, fVolume);

}

_int CGameInstance::PickRandomInt(_int _iStartLimit, _int _iEndLimit, _bool _bSort)
{
	if (!m_pRandomPicker)
		return 0;

	return m_pRandomPicker->PickRandomInt(_iStartLimit, _iEndLimit, _bSort);
}

_float CGameInstance::PickRandomFloat(_float _fStartLimit, _float _fEndLimit, _bool _bSort)
{
	if (!m_pRandomPicker)
		return 0;

	return m_pRandomPicker->PickRandomFloat(_fStartLimit, _fEndLimit, _bSort);
}

void CGameInstance::WriteJson(Json::Value _value, const wstring& _strSavePath)
{
	if (!m_pJsonParser)
		return;

	m_pJsonParser->WriteJson(_value, _strSavePath);

}

Json::Value CGameInstance::ReadJson(const wstring& _strReadPath)
{
	return m_pJsonParser->ReadJson(_strReadPath);
}

void CGameInstance::GetWorldRay(_float3& _vRayPos, _float3& _vRayDir)
{
	if (!m_pPickingMgr)
		return;

	m_pPickingMgr->GetWorldRay(_vRayPos, _vRayDir);
}

HRESULT CGameInstance::ComputePicking(_uint _iTarget)
{
    if (!m_pPickingMgr)
        return E_FAIL;

    return m_pPickingMgr->ComputePicking(_iTarget);
}

//shared_ptr<CGameObject> CGameInstance::GetPickedObject()
//{
//    if (!m_pPickingMgr)
//        return nullptr;
//
//    return m_pPickingMgr->GetPickedObject();
//}


_uint CGameInstance::GetPickedObjectID()
{
    return m_pPickingMgr->GetPickedObjectID();
}

_uint CGameInstance::GetIdxPickedVertex()
{
    return m_pPickingMgr->GetIdxPickedVertex();
}

_float3 CGameInstance::GetWorldPosPicked()
{
	return m_pPickingMgr->GetWorldPosPicked();
}

_float CGameInstance::Lerp(_float _fStart, _float _fEnd, _float _fRatio)
{
	if (!m_pCalculator)
		return 0;

	return m_pCalculator->Lerp(_fStart, _fEnd, _fRatio);
}

_float CGameInstance::Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float _fStartValue, _float _fEndValue)
{
	if (!m_pEasing)
		return 0;

	return m_pEasing->Ease(_eEase, _eElapsedTime, _fDuration, _fStartValue, _fEndValue);
}

_float2 CGameInstance::Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float2 _fStartValue, _float2 _fEndValue)
{
	if (!m_pEasing)
		return { 0.f, 0.f };

	return m_pEasing->Ease(_eEase, _eElapsedTime, _fDuration, _fStartValue, _fEndValue);
}

_float3 CGameInstance::Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float3 _fStartValue, _float3 _fEndValue)
{
	if (!m_pEasing)
		return { 0.f, 0.f, 0.f };

	return m_pEasing->Ease(_eEase, _eElapsedTime, _fDuration, _fStartValue, _fEndValue);
}

_float4 CGameInstance::Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float4 _fStartValue, _float4 _fEndValue)
{
	if (!m_pEasing)
		return { 0.f, 0.f, 0.f, 0.f };

	return m_pEasing->Ease(_eEase, _eElapsedTime, _fDuration, _fStartValue, _fEndValue);
}

const char** CGameInstance::GetEaseTypeKeys()
{
	return m_pEasing->GetEaseTypeKeys();
}

HRESULT CGameInstance::AddFont(const string& _strFontTag, const wstring& _strFontFilePath)
{
	if (!m_pFontMgr)
		return E_FAIL;

	return m_pFontMgr->AddFont(_strFontTag, _strFontFilePath);
}

HRESULT CGameInstance::RenderFont(const string& _strFontTag, const wstring& _strText, const _float2& _vPosition, CCustomFont::EFONT_ALIGNMENT _eFontAlignment, _color _vColor, _float _fRotation, const _float2& _vOrigin, _float _fScale)
{
	if (!m_pFontMgr)
		return E_FAIL;

	return m_pFontMgr->Render(_strFontTag, _strText, _vPosition, _eFontAlignment, _vColor, _fRotation, _vOrigin, _fScale);
}

shared_ptr<CCustomFont> CGameInstance::FindFont(const string& _strFontTag)
{
	if (!m_pFontMgr)
		return nullptr;

	return m_pFontMgr->FindFont(_strFontTag);
}

wstring CGameInstance::StringToWString(const string& _inString)
{
	std::wstring wideString;
	size_t size = _inString.size() + 1;
	wideString.resize(size, L'\0');
	size_t convertedChars{ 0 };
	mbstowcs_s(&convertedChars, &wideString[0], size, _inString.c_str(), _TRUNCATE);
	return wideString;
}

string CGameInstance::WStringToString(const wstring& _inWString)
{
	std::string nString;
	size_t size = _inWString.size() * MB_CUR_MAX + 1;
	nString.resize(size, '\0');
	size_t convertedChars{ 0 };
	wcstombs_s(&convertedChars, &nString[0], size, _inWString.c_str(), _TRUNCATE);
	return nString;
}

ContainmentType CGameInstance::isInWorldSpace(const BoundingBox& _BoundingBox)
{
	if (!m_pFrustum)
		return CONTAINS;

	return m_pFrustum->isInWorldSpace(_BoundingBox);
}

bool CGameInstance::isInWorldSpace(FXMVECTOR _vPosition, float _fRadius)
{
	if (!m_pFrustum)
		return true;

	return m_pFrustum->isInWorldSpace(_vPosition, _fRadius);
}

void CGameInstance::MakeLocalSpace(FXMMATRIX _WorldMatrix)
{
	if (!m_pFrustum)
		return;

	return m_pFrustum->MakeLocalSpace(_WorldMatrix);
}

bool CGameInstance::isInLocalSpace(FXMMATRIX _WorldMatrix, FXMVECTOR _vPosition, float _fRadius)
{
	if (!m_pFrustum)
		return true;

	return m_pFrustum->isInLocalSpace(_WorldMatrix, _vPosition, _fRadius);
}

void CGameInstance::DeleteOctree()
{
	m_pOctree.reset();
}

HRESULT CGameInstance::CreateOctree(const _float3& _vCenter, const float& _fHalfWidth, const int& _iDepthLimit)
{
	m_pOctree = COctree::Create(_vCenter, _fHalfWidth, _iDepthLimit);
	if (!m_pOctree)
		return E_FAIL;

	return S_OK;
}

bool CGameInstance::AddStaticObject(shared_ptr<class CGameObject> _pGameObject, const _float3& _vPoint, const float& _fRadius)
{
	if (!m_pOctree)
		return false;

	return m_pOctree->AddStaticObject(_pGameObject, _vPoint, _fRadius);
}

bool CGameInstance::AddStaticInstance(const string& _strModelTag, shared_ptr<class CGameObject> _pGameObject, const _float3& _vPoint, const _float4x4& _vWorldMatrix, const float& _fRadius)
{
	if (!m_pOctree)
		return false;

	return m_pOctree->AddStaticInstance(_strModelTag, _pGameObject, _vPoint, _vWorldMatrix, _fRadius);
}

void CGameInstance::PlayVideo(wstring _strPath)
{
	if (!m_pVideoMgr)
		return;

	m_pVideoMgr->PlayVideo(_strPath);

}

void CGameInstance::Free()
{
	if (m_pSoundMgr) {
		m_pSoundMgr->Free();
	}

	m_pOctree.reset();
	if (m_pRenderer)
	{
		m_pRenderer->Clear();
	}
	if (m_pObjectMgr)
	{
		m_pObjectMgr->Clear();
	}
	if (m_pPhysXMgr)
	{
		m_pPhysXMgr->Free();
	}
}
