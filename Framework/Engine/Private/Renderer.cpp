#include "Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"

_uint g_iSizeX = 8192;
_uint g_iSizeY = 4608;

//float CRenderer::g_fConstrast = 0.541f;
//float CRenderer::g_fMiddleGray = 0.521f;
//float CRenderer::g_fSaturate = 0.54f;

// 얘들이 초기값
float CRenderer::g_fConstrast = 0.511f;
float CRenderer::g_fMiddleGray = 0.481f;
float CRenderer::g_fSaturate = 0.56f;

float CRenderer::g_fFogHeight = 0.f;
//float CRenderer::g_fFogStart = 60.f;
float CRenderer::g_fFogStart = 100.f;
float CRenderer::g_fFogEnd = 500.f;

_float3 CRenderer::g_vFogColor = _float3(0.2f, 0.3f, 0.4f);


CRenderer::CRenderer(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	:m_pDevice(_pDevice),
	m_pContext(_pContext)
{
}

CRenderer::~CRenderer()
{
	m_pOutlineDSV->Release();
	m_pCascadeShadowArrDSV->Release();
}

HRESULT CRenderer::Initialize()
{
	D3D11_VIEWPORT ViewPortDesc{};

	_uint iNumViewPorts = 1;

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewPortDesc);


	ReadyCascadeDSV();
	ReadyOutlineDSV();


	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_PreDepth"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Diffuse"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Normal"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Shade"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_FinalShadow"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Depth"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Edge_Depth"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Edge"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Trail"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Glow"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Blur_X"), (_uint)m_vDownSampleSize1.x, (_uint)m_vDownSampleSize1.y, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Blur_Y"), (_uint)m_vDownSampleSize1.x, (_uint)m_vDownSampleSize1.y, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Distortion"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	///*Glow Down&UpSample*/
	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Glow_DownSample"), (_uint)m_vDownSampleSize1.x, (_uint)m_vDownSampleSize1.y, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Glow_UpSample"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_ARM"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Rim"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Outline"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_MotionBlur"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Specular"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderArrTarget(TEXT("Target_Cascade"), (_uint)m_vCascadeSize.x, (_uint)m_vCascadeSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f), 3)))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Shadow"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/*HDR Bloom*/
	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Luminance"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_DownSample1"), (_uint)m_vDownSampleSize1.x, (_uint)m_vDownSampleSize1.x, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_DownSample2"), (_uint)m_vDownSampleSize2.x, (_uint)m_vDownSampleSize2.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_DownSample3"), (_uint)m_vDownSampleSize3.x, (_uint)m_vDownSampleSize3.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_UpSample1"), (_uint)m_vDownSampleSize2.x, (_uint)m_vDownSampleSize2.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_UpSample2"), (_uint)m_vDownSampleSize1.x, (_uint)m_vDownSampleSize1.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_UpSample3"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_BlurH_125"), (_uint)m_vDownSampleSize3.x, (_uint)m_vDownSampleSize3.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_BlurHV_125"), (_uint)m_vDownSampleSize3.x, (_uint)m_vDownSampleSize3.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_BlurH_25"), (_uint)m_vDownSampleSize2.x, (_uint)m_vDownSampleSize2.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_BlurHV_25"), (_uint)m_vDownSampleSize2.x, (_uint)m_vDownSampleSize2.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_BlurH_5"), (_uint)m_vDownSampleSize1.x, (_uint)m_vDownSampleSize1.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_BlurHV_5"), (_uint)m_vDownSampleSize1.x, (_uint)m_vDownSampleSize1.y, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	///*Depth Of Field*/

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_DOF_BLURV"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_DOF_BLURHV"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	///*Compute Shader Test*/
	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_Test"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_Test"), TEXT("Target_Test"))))
		return E_FAIL;
	///**/

	//// Pixel Picking
	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_PixelPicking"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->AddRenderTarget(TEXT("Target_PixelPicking_View"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/*Set MRT*/

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_PreDepth"), TEXT("Target_PreDepth"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_GameObjects"), TEXT("Target_ARM"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_GameObjects"), TEXT("Target_Rim"))))
		return E_FAIL;

	// Pixel Picking
	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_PixelPicking"), TEXT("Target_PixelPicking"))))
		return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_PixelPicking"), TEXT("Target_PixelPicking_View"))))
		return E_FAIL;
	

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_GameObjects"), TEXT("Target_Outline"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_LightAcc"), TEXT("Target_FinalShadow"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_Trail"), TEXT("Target_Trail"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_Edge_Depth"), TEXT("Target_Edge_Depth"))))
		return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_Edge"), TEXT("Target_Edge"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_Glow"), TEXT("Target_Glow"))))
		return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
		return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_Blur_Y"), TEXT("Target_Blur_Y"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_Glow_DownSample"), TEXT("Target_Glow_DownSample"))))
		return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_Glow_UpSample"), TEXT("Target_Glow_UpSample"))))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_MotionBlur"), TEXT("Target_MotionBlur"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;

	/*Array Test*/
	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_Cascade"), TEXT("Target_Cascade"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_Shadow"), TEXT("Target_Shadow"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_Luminance"), TEXT("Target_Luminance"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_DownSample1"), TEXT("Target_DownSample1"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_DownSample2"), TEXT("Target_DownSample2"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_DownSample3"), TEXT("Target_DownSample3"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_UpSample1"), TEXT("Target_UpSample1"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_UpSample2"), TEXT("Target_UpSample2"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_UpSample3"), TEXT("Target_UpSample3"))))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_BlurH_125"), TEXT("Target_BlurH_125"))))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_BlurHV_125"), TEXT("Target_BlurHV_125"))))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_BlurH_25"), TEXT("Target_BlurH_25"))))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_BlurHV_25"), TEXT("Target_BlurHV_25"))))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_BlurH_5"), TEXT("Target_BlurH_5"))))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_BlurHV_5"), TEXT("Target_BlurHV_5"))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_DOF_BLURV"), TEXT("Target_DOF_BLURV"))))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->AddMRT(TEXT("MRT_DOF_BLURHV"), TEXT("Target_DOF_BLURHV"))))
		return E_FAIL;


	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewPortDesc.Width;
	m_WorldMatrix._22 = ViewPortDesc.Height;



/*Group 1*/
	
	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_Edge"), m_fTargetSizeGroup1 * 0.5f, m_fTargetSizeGroup1 * 0.5f, m_fTargetSizeGroup1, m_fTargetSizeGroup1)))
	//	return E_FAIL;

	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_Edge_Depth"), m_fTargetSizeGroup1 * 0.5f, m_fTargetSizeGroup1 * 0.5f + m_fTargetSizeGroup1, m_fTargetSizeGroup1, m_fTargetSizeGroup1)))
	//	return E_FAIL;

	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_Distortion"), m_fTargetSizeGroup1 * 0.5f, m_fTargetSizeGroup1 * 0.5f, m_fTargetSizeGroup1, m_fTargetSizeGroup1)))
	//	return E_FAIL;

/*Group 2*/

	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_Diffuse"), m_fTargetSizeGroup2 * 0.5f, m_fTargetSizeGroup2 * 0.5f, m_fTargetSizeGroup2, m_fTargetSizeGroup2)))
	//	return E_FAIL;

	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_Normal"), m_fTargetSizeGroup2 * 0.5f, m_fTargetSizeGroup2 * 0.5f + m_fTargetSizeGroup2, m_fTargetSizeGroup2, m_fTargetSizeGroup2)))
	//	return E_FAIL;
	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_Depth"), m_fTargetSizeGroup2 * 0.5f, m_fTargetSizeGroup2 * 0.5f + m_fTargetSizeGroup2 * 2.f, m_fTargetSizeGroup2, m_fTargetSizeGroup2)))
	//	return E_FAIL;
	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_Shade"), m_fTargetSizeGroup2 * 0.5f, m_fTargetSizeGroup2 * 0.5f  + m_fTargetSizeGroup2 * 3.f, m_fTargetSizeGroup2, m_fTargetSizeGroup2)))
	//	return E_FAIL;

	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_Specular"), m_fTargetSizeGroup2 * 0.5f + m_fTargetSizeGroup2, m_fTargetSizeGroup2 * 0.5f, m_fTargetSizeGroup2, m_fTargetSizeGroup2)))
	//	return E_FAIL;
	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_ARM"), m_fTargetSizeGroup2 * 0.5f + m_fTargetSizeGroup2, m_fTargetSizeGroup2 * 0.5f + m_fTargetSizeGroup2, m_fTargetSizeGroup2, m_fTargetSizeGroup2)))
	//	return E_FAIL;

	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_Rim"), m_fTargetSizeGroup2 * 0.5f + m_fTargetSizeGroup2, m_fTargetSizeGroup2 * 0.5f + m_fTargetSizeGroup2 * 2.f, m_fTargetSizeGroup2, m_fTargetSizeGroup2)))
	//	return E_FAIL;

	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_Luminance"), 1280 -  m_fTargetSizeGroup2 * 0.5f, m_fTargetSizeGroup2 * 0.5f , m_fTargetSizeGroup2, m_fTargetSizeGroup2)))
	//	return E_FAIL;

	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_Cascade"), 1280 - m_fTargetSizeGroup2 * 0.5f, m_fTargetSizeGroup2 * 0.5f + m_fTargetSizeGroup2, m_fTargetSizeGroup2, m_fTargetSizeGroup2)))
	//	return E_FAIL;

	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_Glow"), 1280 - m_fTargetSizeGroup2 * 0.5f, m_fTargetSizeGroup2 * 0.5f + m_fTargetSizeGroup2 * 2.f, m_fTargetSizeGroup2, m_fTargetSizeGroup2)))
	//	return E_FAIL;

	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_Trail"), 1280 - m_fTargetSizeGroup2 * 0.5f, m_fTargetSizeGroup2 * 0.5f + m_fTargetSizeGroup2 * 3.f, m_fTargetSizeGroup2, m_fTargetSizeGroup2)))
	//	return E_FAIL;

	// Pixel Picking
	//if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_PixelPicking"), 100.0f, 300.0f, 200.0f, 200.0f)))
	//	return E_FAIL;
	if (FAILED(CGameInstance::GetInstance()->ReadyDebug(TEXT("Target_PixelPicking_View"), 100.0f, 100.0f, 200.0f, 200.0f)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::ReadyCascadeDSV()
{
	/*Cascade ArrayDSV(Test)*/

	//m_pCascadeShadowArrDSV


	ComPtr<ID3D11Texture2D> pDepthStencilArrTexture = nullptr;


	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = (UINT)m_vCascadeSize.x;
	TextureDesc.Height = (UINT)m_vCascadeSize.y;
	TextureDesc.MipLevels = 0;
	TextureDesc.ArraySize = 3;
	TextureDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	//DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilArrTexture)))
		return E_FAIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZeroMemory(&DepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	DepthStencilViewDesc.Flags = 0;
	DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	DepthStencilViewDesc.Texture2DArray.MipSlice = 0;
	DepthStencilViewDesc.Texture2DArray.ArraySize = 3;
	DepthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilArrTexture.Get(), &DepthStencilViewDesc, &m_pCascadeShadowArrDSV)))
		return E_FAIL;


	return S_OK;

}

HRESULT CRenderer::ReadyIBL()
{
	return E_NOTIMPL;
}

HRESULT CRenderer::ReadyUAV()
{

	/*D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 1280.f;
	TextureDesc.Height = 720.f;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = _ePixelFormat;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE |
		D3D11_BIND_UNORDERED_ACCESS;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 1;

	D3D11_SHADER_RESOURCE_VIEW_DESC tSrvDesc{};
	tSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	tSrvDesc.Texture2D.MostDetailedMip = 0;
	tSrvDesc.Texture2D.MipLevels = 1;

	D3D11_RENDER_TARGET_VIEW_DESC tRtvDesc{};
	tRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	tRtvDesc.Texture2D.MipSlice = 0;

	D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc{};
	tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D.Get(), &tRtvDesc, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D.Get(), &tSrvDesc, &m_pSRV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pTexture2D.Get(), &tUAVDesc, &m_pUAV)))
		return E_FAIL;*/


	return S_OK;
}

HRESULT CRenderer::ReadyOutlineDSV()
{
	D3D11_VIEWPORT		ViewportDesc;

	_uint				iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	ComPtr<ID3D11Texture2D> pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 1280;
	TextureDesc.Height = 720;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZeroMemory(&DepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	DepthStencilViewDesc.Flags = 0;
	DepthStencilViewDesc.Format = TextureDesc.Format;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture.Get(), nullptr, &m_pOutlineDSV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::AddRenderGroup(ERENDER_GROUP _eRenderGroup, shared_ptr<class CGameObject> _pGameObject)
{
	if (_eRenderGroup >= ERENDER_GROUP::RENDER_END)
		return E_FAIL;

	m_RenderObjects[_eRenderGroup].push_back(_pGameObject);

	return S_OK;
}

HRESULT CRenderer::AddUIRenderGroup(ERENDER_UI_GROUP _eUIRenderGroup, shared_ptr<class CGameObject> _pGameObject)
{
	if (_eUIRenderGroup >= ERENDER_UI_GROUP::RENDER_UI_END)
		return E_FAIL;

	m_RenderUIs[_eUIRenderGroup].push_back(_pGameObject);

	return S_OK;
}

HRESULT CRenderer::AddCascadeGroup(_uint _iCascadeNum, shared_ptr<class CGameObject> _pGameObject)
{
	//if (_iCascadeNum > m_iCascadeNum)
	//	return E_FAIL;

	//m_RenderCascade[_iCascadeNum].push_back(_pGameObject);


	m_CascadeList.push_back(_pGameObject);

	return S_OK;
}

HRESULT CRenderer::Render()
{
	if (FAILED(RenderPriority()))
		return E_FAIL;

	if (FAILED(RenderCascadeShadow()))
		return E_FAIL;

	// Pixel Picking
	if (FAILED(RenderPixelPicking()))
		return E_FAIL;

	if (FAILED(RenderNonBlend()))
		return E_FAIL;

	if (FAILED(RenderLight()))
		return E_FAIL;

	if (FAILED(RenderFinal()))
		return E_FAIL;

	if (FAILED(RenderDOF()))
		return E_FAIL;

	if (FAILED(RenderNonLight()))
		return E_FAIL;

	if (FAILED(RenderEdge()))
		return E_FAIL;

	if (FAILED(RenderHDR()))
		return E_FAIL;

	if (FAILED(RenderBlend()))
		return E_FAIL;

	if (FAILED(RenderTrail()))
		return E_FAIL;

	if (FAILED(RenderGlow()))
		return E_FAIL;


	if (FAILED(RenderDistortion()))
		return E_FAIL;


	if (FAILED(RenderPostProcess()))
		return E_FAIL;

	if (m_IsHDRRender) {
		if (FAILED(RenderScreenTone()))
			return E_FAIL;
	}

	if (FAILED(RenderFxaa()))
		return E_FAIL;

//#ifdef _DEBUG
	//if (m_IsDebugMode) {
	if (FAILED(RenderDebug()))
		return E_FAIL;
	//}
	
	m_DebugCom.clear();
//#endif

	if (FAILED(RenderWorldUI()))
		return E_FAIL;

	if (FAILED(RenderUI()))
		return E_FAIL;

	if (FAILED(RenderUIWorld()))
		return E_FAIL;

	if (FAILED(RenderUIHUD()))
		return E_FAIL;

	if (FAILED(RenderUIContent()))
		return E_FAIL;

	if (FAILED(RenderUIInventory()))
		return E_FAIL;

	if (FAILED(RenderUIPopUp()))
		return E_FAIL;

	if (FAILED(RenderUIEffect()))
		return E_FAIL;

	if (FAILED(RenderFadeInOut()))
		return E_FAIL;

	if (m_IsPartitionScreen) {
		if (FAILED(RenderForFilmVideo()))
			return E_FAIL;
	}

	return S_OK;
}

void CRenderer::Clear()
{
	m_DebugCom.clear();
	for (UINT i = 0; i < RENDER_END; i++)
	{
		m_RenderObjects[i].clear();
	}
}

HRESULT CRenderer::SetMotionBlur(_float _fDuration, _float _fPower)
{

	if (m_IsMotionBlurActive) {
		m_IsMotionBlurOn = true;
		m_fMotionBlurAccTime = 0.f;
		m_fMotionBlurDuration = _fDuration;

		m_fMotionBlurPower = _fPower;

		m_PrevViewMat = GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW);

	}
	return S_OK;
}

HRESULT CRenderer::SetZoomBlur(_float _fDuration, _float _fPower, _float3 _vWorldPos)
{
	m_bZoomBlurOn = true;
	m_fZoomBlurAccTime = 0.f;
	m_fZoomBlurDuration = _fDuration;

	m_fZoomBlurPower = _fPower * 0.01f;

	m_vZoomBlurPosW = _vWorldPos;

	return S_OK;
}

HRESULT CRenderer::SetFadeEffect(_bool _IsFadeOut, _float _fTotalDuration, _bool _IsContinual, _float _fBlackOutTime)
{
	m_IsContinual = _IsContinual;
	m_fBlackOutDuration = _fBlackOutTime;

	m_IsBlackOut = false;

	//FadeOut-BlackOut-FadeIn
	if (_IsContinual) {



	}
	//FadeOut or FadeIn
	else {

		if (_IsFadeOut) {
			m_IsFadeOut = true;
			m_IsFadeIn = false;
		}
		else {
			m_IsFadeIn = true;
			m_IsFadeOut = false;
		}

		m_fFadeDuration = _fTotalDuration;
		m_fFadeAccTime = 0.f;
	}


	return S_OK;
}

HRESULT CRenderer::SetChromaticAberration(_bool _IsChromaticAberrationOn)
{
	m_IsChromaticAberrationOn = _IsChromaticAberrationOn;

	return S_OK;
}

HRESULT CRenderer::SetScreenNoise(_bool _IsScreenNoiseOn)
{
	if (!_IsScreenNoiseOn) {
		m_fNoiseTime = 0.f;
	}

	m_IsScreenNoiseOn = _IsScreenNoiseOn;

	return S_OK;
}

HRESULT CRenderer::RenderDepth()
{
	if (FAILED(CGameInstance::GetInstance()->BeginMRT(TEXT("MRT_PreDepth"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_NONBLEND]) {
		if (nullptr != pGameObject)
			pGameObject->RenderDepth();
	}

	if (FAILED(CGameInstance::GetInstance()->EndMRT()))
		return E_FAIL;

	return S_OK;
}

// Pixel Picking
HRESULT CRenderer::RenderPixelPicking()
{
	if (FAILED(CGameInstance::GetInstance()->BeginMRT(TEXT("MRT_PixelPicking"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_PIXELPICKING]) {
		if (nullptr != pGameObject)
			pGameObject->RenderPixelPicking();
	}

	m_RenderObjects[RENDER_PIXELPICKING].clear();


	if (FAILED(CGameInstance::GetInstance()->EndMRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::RenderPriority()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY]) {
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::RenderNonBlend()
{
	if (FAILED(CGameInstance::GetInstance()->BeginMRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_NONBLEND]) {
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	m_RenderObjects[RENDER_NONBLEND].clear();


	if (FAILED(CGameInstance::GetInstance()->EndMRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::RenderLight()
{
	if (FAILED(CGameInstance::GetInstance()->BeginMRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	GAMEINSTANCE->ReadyShader(m_eDeferredST, 0);
	GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix);

	_float4x4 ProjMatInv = CGameInstance::GetInstance()->GetTransformMatrixInverse(CPipeLine::D3DTS_PROJ);
	_float4x4 ViewMatInv = CGameInstance::GetInstance()->GetTransformMatrixInverse(CPipeLine::D3DTS_VIEW);
	_float4 vCamPos = CGameInstance::GetInstance()->GetCamPosition();
	_float3 vCamLook = CGameInstance::GetInstance()->GetCamLook();

	//g_NoiseTexture
	

	if (FAILED(GAMEINSTANCE->BindMatrix("g_ProjMatrixInv", &ProjMatInv)))
		return E_FAIL;
	if (FAILED(GAMEINSTANCE->BindMatrix("g_ViewMatrixInv", &ViewMatInv)))
		return E_FAIL;
	if (FAILED(GAMEINSTANCE->BindRawValue("g_vCamPosition", &vCamPos, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_IsPBROn", &m_IsPBROn, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_IsAOOn", &m_IsAOOn, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_fAOPower", &m_fAOPower, sizeof(_float))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_vCamLook", &vCamLook, sizeof(_float3))))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->BindTargetSRV(TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->BindTargetSRV(TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->BindTargetSRV(TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->BindTargetSRV(TEXT("Target_ARM"), "g_ARMTexture")))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->BindTargetSRV(TEXT("Target_Cascade"), "g_CascadeTexture1")))
		return E_FAIL;


	/*if (FAILED(CGameInstance::GetInstance()->BindTargetSRV(TEXT("Target_Cascade2"), "g_CascadeTexture2")))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->BindTargetSRV(TEXT("Target_Cascade3"), "g_CascadeTexture3")))
		return E_FAIL;*/

	GAMEINSTANCE->BindLightProjMatrix();

	_float3 vLightDir = GAMEINSTANCE->GetLightDir();
	_float4 vLightDir2 = _float4(vLightDir.x, vLightDir.y, vLightDir.z, 0.f);

	if (FAILED(GAMEINSTANCE->BindRawValue("g_vLightDir", &vLightDir2, sizeof(_float4))))
		return E_FAIL;

	GAMEINSTANCE->RenderLight();

	if (FAILED(CGameInstance::GetInstance()->EndMRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::RenderEdge()
{
	if (m_RenderObjects[RENDER_EDGE].empty())
		return S_OK;

	//=================Depth====================//
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1280.f;
	ViewPortDesc.Height = (_float)720.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	wstring szTargetName = L"MRT_Edge_Depth";

	if (FAILED(CGameInstance::GetInstance()->BeginMRT(szTargetName, m_pOutlineDSV)))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_EDGE])
	{
		pGameObject->RenderEdgeDepth();
	}

	if (FAILED(CGameInstance::GetInstance()->EndMRT()))
		return E_FAIL;

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1280.0f;
	ViewPortDesc.Height = (_float)720.0f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	//============== Pos + Normal =================//

	if (FAILED(CGameInstance::GetInstance()->BeginMRT(TEXT("MRT_Edge"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_EDGE]) {
		if (nullptr != pGameObject)
			pGameObject->RenderEdge();
	}

	m_RenderObjects[RENDER_EDGE].clear();


	if (FAILED(CGameInstance::GetInstance()->EndMRT()))
		return E_FAIL;

	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_POSTPROCESS, 4);
	GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix);
	GAMEINSTANCE->BindBackBufferSRV("g_BackBufferTexture");

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Edge_Depth"), "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Edge"), "g_EdgeTexture")))
		return E_FAIL;

	GAMEINSTANCE->BeginShader();

	return S_OK;
}

HRESULT CRenderer::RenderFinal()
{
	GAMEINSTANCE->ReadyShader(m_eDeferredST, 3);
	GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix);

	_float3 vLightDir = GAMEINSTANCE->GetLightDir();
	_float4 vLightDir2 = _float4(vLightDir.x, vLightDir.y, vLightDir.z, 0.f);

	if (FAILED(GAMEINSTANCE->BindRawValue("g_vLightDir", &vLightDir2, sizeof(_float4))))
		return E_FAIL;

	_float4x4 ProjMatInv = CGameInstance::GetInstance()->GetTransformMatrixInverse(CPipeLine::D3DTS_PROJ);
	_float4x4 ViewMatInv = CGameInstance::GetInstance()->GetTransformMatrixInverse(CPipeLine::D3DTS_VIEW);

	if (FAILED(GAMEINSTANCE->BindMatrix("g_ProjMatrixInv", &ProjMatInv)))
		return E_FAIL;
	if (FAILED(GAMEINSTANCE->BindMatrix("g_ViewMatrixInv", &ViewMatInv)))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Shade"), "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Rim"), "g_RimTexture")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_FinalShadow"), "g_FinalShadowTexture")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Specular"), "g_SpecularTexture")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_IsUsingFog", &m_UsingFog, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_fFogEnd", &g_fFogEnd, sizeof(_float))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_fFogStart", &g_fFogStart, sizeof(_float))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_vFogColor", &g_vFogColor, sizeof(_float3))))
		return E_FAIL;

	_float4 vCamPos = CGameInstance::GetInstance()->GetCamPosition();

	if (FAILED(GAMEINSTANCE->BindRawValue("g_vCamPosition", &vCamPos, sizeof(_float4))))
		return E_FAIL;


	if (FAILED(CGameInstance::GetInstance()->BindTargetSRV(TEXT("Target_Shadow"), "g_ShadowTexture")))
		return E_FAIL;


	GAMEINSTANCE->BeginShader();

	return S_OK;
}

HRESULT CRenderer::RenderNonLight()
{
	if (m_RenderObjects[RENDER_NONLIGHT].empty())
		return S_OK;

	for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}


HRESULT CRenderer::RenderBlur()
{
	if (m_RenderObjects[RENDER_BLUR].empty())
		return S_OK;

	SortRenderList(RENDER_BLUR);

	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_Glow"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_BLUR])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	if (FAILED(GAMEINSTANCE->EndMRT()))
		return E_FAIL;

	m_RenderObjects[RENDER_GLOW].clear();

	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_POSTPROCESS, 1);
	GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix);
	GAMEINSTANCE->BindBackBufferSRV("g_BackBufferTexture");

	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_Blur_X"))))
		return E_FAIL;
	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Glow"), "g_BlurTexture")))
		return E_FAIL;

	GAMEINSTANCE->BeginShader();

	if (FAILED(GAMEINSTANCE->EndMRT()))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_Blur_Y"))))
		return E_FAIL;
	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Blur_X"), "g_BlurTexture")))
		return E_FAIL;

	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_POSTPROCESS, 2);
	GAMEINSTANCE->BeginShader();

	if (FAILED(GAMEINSTANCE->EndMRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::RenderGlow()
{
	if (m_RenderObjects[RENDER_GLOW].empty())
		return S_OK;

	if (!m_IsEffectOn)
	{
		m_RenderObjects[RENDER_GLOW].clear();
		return S_OK;
	}

	else
		GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_POSTPROCESS, 0);

	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_Glow"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_GLOW])
	{
		if (nullptr != pGameObject)
			pGameObject->RenderGlow();
	}

	if (FAILED(GAMEINSTANCE->EndMRT()))
		return E_FAIL;

	m_RenderObjects[RENDER_GLOW].clear();


	/*DownSample Test*/

	if (FAILED(GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_COMPUTE, 3)))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Glow"), "g_Input")))
		return E_FAIL;

	_uint iWidth = 1280 / 5.f;

	if (FAILED(GAMEINSTANCE->BindTargetUAV(TEXT("Target_Glow_DownSample"), "g_Output")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BeginCS(16, 9, 1)))
		return E_FAIL;

	ID3D11UnorderedAccessView* ppNullUAV[] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, ppNullUAV, nullptr);

	if (FAILED(GAMEINSTANCE->CreateTargetSRV(TEXT("Target_Glow_DownSample"))))
		return E_FAIL;


	/*Blur X */
	if (FAILED(GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_COMPUTE, 1)))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Glow_DownSample"), "g_Input")))
		return E_FAIL;

	iWidth = 1280;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_TextureSize", &iWidth, sizeof(iWidth))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetUAV(TEXT("Target_Blur_X"), "g_Output")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BeginCS(16, 9, 1)))
		return E_FAIL;

	m_pContext->CSSetUnorderedAccessViews(0, 1, ppNullUAV, nullptr);

	if (FAILED(GAMEINSTANCE->CreateTargetSRV(TEXT("Target_Blur_X"))))
		return E_FAIL;


	/*BlurY*/

	if (FAILED(GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_COMPUTE, 2)))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Blur_X"), "g_Input")))
		return E_FAIL;

	_uint iHeight = 720;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_TextureSize", &iHeight, sizeof(iHeight))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetUAV(TEXT("Target_Blur_Y"), "g_Output")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BeginCS(16, 9, 1)))
		return E_FAIL;

	m_pContext->CSSetUnorderedAccessViews(0, 1, ppNullUAV, nullptr);
	if (FAILED(GAMEINSTANCE->CreateTargetSRV(TEXT("Target_Blur_Y"))))
		return E_FAIL;



	/*UpSampling*/
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1280.f;
	ViewPortDesc.Height = (_float)720.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	/*Test Upsampling*/

	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 7);
	ViewPortDesc.Width = (_float)1280.f;
	ViewPortDesc.Height = (_float)720.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	float fAspect = ViewPortDesc.Height / ViewPortDesc.Width;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_fAspect", &fAspect, sizeof(fAspect))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_Glow_UpSample"))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Blur_Y"), "g_DownSampleTexture")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BeginShader()))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->EndMRT()))
		return E_FAIL;

	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_POSTPROCESS, 0);

	GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix);
	GAMEINSTANCE->BindBackBufferSRV("g_BackBufferTexture");

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Glow_UpSample"), "g_BlendTexture")))
		return E_FAIL;
	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Glow"), "g_BlurTexture")))
		return E_FAIL;

	GAMEINSTANCE->BeginShader();

	return S_OK;
}

HRESULT CRenderer::RenderDistortion()
{
	if (m_RenderObjects[RENDER_DISTORTION].empty())
		return S_OK;

	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_Distortion"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_DISTORTION])
	{
		if (nullptr != pGameObject)
			pGameObject->RenderDistortion();
	}

	m_RenderObjects[RENDER_DISTORTION].clear();

	if (FAILED(GAMEINSTANCE->EndMRT()))
		return E_FAIL;

	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_POSTPROCESS, 3);
	GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix);

	GAMEINSTANCE->BindBackBufferSRV("g_BackBufferTexture");

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Distortion"), "g_DistortionTexture")))
		return E_FAIL;

	GAMEINSTANCE->BeginShader();

	return S_OK;
}

HRESULT CRenderer::RenderBlend()
{
	if (m_RenderObjects[RENDER_BLEND].empty())
		return S_OK;

	if (!m_IsEffectOn)
	{
		m_RenderObjects[RENDER_BLEND].clear();
		return S_OK;
	}

	SortRenderList(RENDER_BLEND);

	for (auto& pGameObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::RenderTrail()
{

	/*Render Trail*/

	if (m_RenderObjects[RENDER_TRAIL].empty())
		return S_OK;

	if (GAMEINSTANCE->BeginMRT(TEXT("MRT_Trail")))
		return E_FAIL;

	SortRenderList(RENDER_TRAIL);

	for (auto& pGameObject : m_RenderObjects[RENDER_TRAIL])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	m_RenderObjects[RENDER_TRAIL].clear();

	if (GAMEINSTANCE->EndMRT())
		return E_FAIL;

	/*Blend Trail Target*/

	GAMEINSTANCE->ReadyShader(ST_POSTPROCESS, 6);

	if (FAILED(GAMEINSTANCE->BindBackBufferSRV("g_BackBufferTexture")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Trail"), "g_BlendTexture")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BeginShader()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::RenderDOF()
{
	/*Depth of Field*/
	
	/*Blur Y */
	if (FAILED(GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_COMPUTE, 2)))
		return E_FAIL;
	
	if (FAILED(GAMEINSTANCE->BindBackBufferSRV("g_Input")))
		return E_FAIL;
	
	_uint iHeight = 720;
	
	if (FAILED(GAMEINSTANCE->BindRawValue("g_TextureSize", &iHeight, sizeof(iHeight))))
		return E_FAIL;
	
	if (FAILED(GAMEINSTANCE->BindTargetUAV(TEXT("Target_DOF_BLURV"), "g_Output")))
		return E_FAIL;
	
	if (FAILED(GAMEINSTANCE->BeginCS(80, 45, 1)))
		return E_FAIL;
	
	ID3D11UnorderedAccessView* ppNullUAV[] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, ppNullUAV, nullptr);
	
	if (FAILED(GAMEINSTANCE->CreateTargetSRV(TEXT("Target_DOF_BLURV"))))
		return E_FAIL;
	
	
	/*BlurY*/
	
	if (FAILED(GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_COMPUTE, 1)))
		return E_FAIL;
	
	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_DOF_BLURV"), "g_Input")))
		return E_FAIL;
	
	_uint iWidth = 1280;
	
	if (FAILED(GAMEINSTANCE->BindRawValue("g_TextureSize", &iWidth, sizeof(iWidth))))
		return E_FAIL;
	
	if (FAILED(GAMEINSTANCE->BindTargetUAV(TEXT("Target_DOF_BLURHV"), "g_Output")))
		return E_FAIL;
	
	if (FAILED(GAMEINSTANCE->BeginCS(80, 45, 1)))
		return E_FAIL;
	
	m_pContext->CSSetUnorderedAccessViews(0, 1, ppNullUAV, nullptr);
	if (FAILED(GAMEINSTANCE->CreateTargetSRV(TEXT("Target_DOF_BLURHV"))))
		return E_FAIL;
	
	
	
	/*Blend Texture*/
	if (FAILED(GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_DOF, 0)))
		return E_FAIL;
	
	if (GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix))
		return E_FAIL;
	
	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_DOF_BLURHV"), "g_BlurTexture")))
		return E_FAIL;
	
	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;
	
	if (FAILED(GAMEINSTANCE->BindBackBufferSRV("g_BackBufferTexture")))
		return E_FAIL;

	if(FAILED(GAMEINSTANCE->BindRawValue("g_UsingDOF", &m_UsingDOF , sizeof(_bool))))
		return E_FAIL;


	
	if (FAILED(GAMEINSTANCE->BeginShader()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::RenderCascadeShadow()
{

	if (m_IsShadowOn) {

		D3D11_VIEWPORT			ViewPortDesc;
		ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
		ViewPortDesc.TopLeftX = 0;
		ViewPortDesc.TopLeftY = 0;
		ViewPortDesc.Width = (_float)m_vCascadeSize.x;
		ViewPortDesc.Height = (_float)m_vCascadeSize.y;
		ViewPortDesc.MinDepth = 0.f;
		ViewPortDesc.MaxDepth = 1.f;

		m_pContext->RSSetViewports(1, &ViewPortDesc);

		if (FAILED(CGameInstance::GetInstance()->BeginMRT(TEXT("MRT_Cascade"), m_pCascadeShadowArrDSV)))
			return E_FAIL;

		for (auto& pGameObject : m_CascadeList)
		{
			pGameObject->RenderShadow();
		}

		if (FAILED(CGameInstance::GetInstance()->EndMRT()))
			return E_FAIL;

		m_CascadeList.clear();

		ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
		ViewPortDesc.TopLeftX = 0;
		ViewPortDesc.TopLeftY = 0;
		ViewPortDesc.Width = (_float)1280.0f;
		ViewPortDesc.Height = (_float)720.0f;
		ViewPortDesc.MinDepth = 0.f;
		ViewPortDesc.MaxDepth = 1.f;

		m_pContext->RSSetViewports(1, &ViewPortDesc);
	}
	else {
		if (FAILED(CGameInstance::GetInstance()->BeginMRT(TEXT("MRT_Cascade"), m_pCascadeShadowArrDSV)))
			return E_FAIL;
		m_CascadeList.clear();

		if (FAILED(CGameInstance::GetInstance()->EndMRT()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::RenderPostProcess()
{

	if (m_IsChromaticAberrationOn) {
		if (GAMEINSTANCE->ReadyShader(ST_POSTPROCESS, 7))
			return E_FAIL;

		if (GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindBackBufferSRV("g_BackBufferTexture")))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BeginShader()))
			return E_FAIL;
	}


	if (m_IsMotionBlurOn) {



		if (GAMEINSTANCE->ReadyShader(ST_POSTPROCESS, 5))
			return E_FAIL;

		if (GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindBackBufferSRV("g_BackBufferTexture")))
			return E_FAIL;

		_float4x4 ProjMatInv = CGameInstance::GetInstance()->GetTransformMatrixInverse(CPipeLine::D3DTS_PROJ);
		_float4x4 ViewMatInv = CGameInstance::GetInstance()->GetTransformMatrixInverse(CPipeLine::D3DTS_VIEW);

		if (FAILED(GAMEINSTANCE->BindMatrix("g_ProjMatrixInv", &ProjMatInv)))
			return E_FAIL;
		if (FAILED(GAMEINSTANCE->BindMatrix("g_ViewMatrixInv", &ViewMatInv)))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_fMotionBlurPower", &m_fMotionBlurPower, sizeof(m_fMotionBlurPower))))
			return E_FAIL;

		if (FAILED(CGameInstance::GetInstance()->BindTargetSRV(TEXT("Target_Depth"), "g_DepthTexture")))
			return E_FAIL;

		_float4x4 prevViewMatrix = GAMEINSTANCE->GetPrevTransformMatrix(CPipeLine::D3DTS_VIEW);

		if (FAILED(GAMEINSTANCE->BindMatrix("g_PrevViewMatrix", &m_PrevViewMat)))
			return E_FAIL;

		_float4x4 perspectiveMat = GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ);

		if (FAILED(GAMEINSTANCE->BindMatrix("g_PerspectiveProjMat", &perspectiveMat)))
			return E_FAIL;


		if (FAILED(GAMEINSTANCE->BeginShader()))
			return E_FAIL;

		m_PrevViewMat = GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW);


		m_fMotionBlurAccTime += 0.016f;

		if (m_fMotionBlurDuration <= m_fMotionBlurAccTime) {

			m_IsMotionBlurOn = false;
			m_fMotionBlurAccTime = 0.f;
		}
	}

	if (m_bZoomBlurOn) {

		if (m_bZoomBlurOff)
		{
			/*if (GAMEINSTANCE->ReadyShader(ST_POSTPROCESS, 13))
				return E_FAIL;*/

			return S_OK;
		}
		else
		{
			if (GAMEINSTANCE->ReadyShader(ST_POSTPROCESS, 8))
				return E_FAIL;
		}

		if (GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindBackBufferSRV("g_BackBufferTexture")))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_fZoomBlurPower", &m_fZoomBlurPower, sizeof(_float))))
			return E_FAIL;

		_float3 vWorldToProj;

		vWorldToProj = SimpleMath::Vector3::Transform(m_vZoomBlurPosW, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
		vWorldToProj = SimpleMath::Vector3::Transform(vWorldToProj, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

		_float2 vZoomPoint = { 0.5f, 0.5f };

		if (FAILED(GAMEINSTANCE->BindRawValue("g_vZoomPoint", &vZoomPoint, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BeginShader()))
			return E_FAIL;

		m_fZoomBlurAccTime += 0.016f;

		// Stop
		if (m_fZoomBlurDuration <= m_fZoomBlurAccTime) {

			m_bZoomBlurOn = false;
			m_fZoomBlurAccTime = 0.f;
		}

		// Power Up
		else if (m_fZoomBlurDuration / 2.f >= m_fZoomBlurAccTime) {
			m_fZoomBlurPower *= 1.01f;
		}

		// Power Down
		else
		{
			m_fZoomBlurPower /= 1.01f;
		}
	}


	if (m_IsScreenNoiseOn) {

		m_fNoiseTime += 0.016f;

		if (GAMEINSTANCE->ReadyShader(ST_POSTPROCESS, 11))
			return E_FAIL;

		if (GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindBackBufferSRV("g_BackBufferTexture")))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_fDeltaTime", &m_fNoiseTime, sizeof(_float))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BeginShader()))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CRenderer::RenderHDR()
{
	if (!m_IsHDROn)
		return S_OK;

	/*Get Luminance*/
	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 6);
	GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix);

	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_Luminance"))))
		return E_FAIL;


	GAMEINSTANCE->BindBackBufferSRV("g_BackBuffer");
	GAMEINSTANCE->BeginShader();

	if (FAILED(GAMEINSTANCE->EndMRT()))
		return E_FAIL;



	/*DownSample Test*/

	if (FAILED(GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_COMPUTE, 3)))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Luminance"), "g_Input")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetUAV(TEXT("Target_DownSample1"), "g_Output")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BeginCS(16, 9, 1)))
		return E_FAIL;

	ID3D11UnorderedAccessView* ppNullUAV[] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, ppNullUAV, nullptr);

	if (FAILED(GAMEINSTANCE->CreateTargetSRV(TEXT("Target_DownSample1"))))
		return E_FAIL;

	{
		/*Blur X */
		if (FAILED(GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_COMPUTE, 1)))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_DownSample1"), "g_Input")))
			return E_FAIL;

		_uint iWidth = 1280 / 5;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_TextureSize", &iWidth, sizeof(iWidth))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindTargetUAV(TEXT("Target_BlurH_5"), "g_Output")))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BeginCS(16, 9, 1)))
			return E_FAIL;

		m_pContext->CSSetUnorderedAccessViews(0, 1, ppNullUAV, nullptr);

		if (FAILED(GAMEINSTANCE->CreateTargetSRV(TEXT("Target_BlurH_5"))))
			return E_FAIL;


		/*BlurY*/

		if (FAILED(GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_COMPUTE, 2)))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_BlurH_5"), "g_Input")))
			return E_FAIL;

		_uint iHeight = 720 / 5;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_TextureSize", &iHeight, sizeof(iHeight))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindTargetUAV(TEXT("Target_BlurHV_5"), "g_Output")))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BeginCS(16, 9, 1)))
			return E_FAIL;

		m_pContext->CSSetUnorderedAccessViews(0, 1, ppNullUAV, nullptr);

		if (FAILED(GAMEINSTANCE->CreateTargetSRV(TEXT("Target_BlurHV_5"))))
			return E_FAIL;


		D3D11_VIEWPORT			ViewPortDesc;
		ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
		ViewPortDesc.TopLeftX = 0;
		ViewPortDesc.TopLeftY = 0;
		ViewPortDesc.Width = (_float)1280.f;
		ViewPortDesc.Height = (_float)720.f;
		ViewPortDesc.MinDepth = 0.f;
		ViewPortDesc.MaxDepth = 1.f;

		m_pContext->RSSetViewports(1, &ViewPortDesc);

		/*Test Upsampling*/

		GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 7);
		ViewPortDesc.Width = (_float)1280.f;
		ViewPortDesc.Height = (_float)720.f;

		m_pContext->RSSetViewports(1, &ViewPortDesc);

		if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_UpSample3"))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_BlurHV_5"), "g_DownSampleTexture")))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BeginShader()))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->EndMRT()))
			return E_FAIL;

	}

	///*DownSampling2*/


	{
		//GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 1);

		//	D3D11_VIEWPORT			ViewPortDesc;
		//	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
		//	ViewPortDesc.Width = (_float)m_vDownSampleSize2.x;
		//	ViewPortDesc.Height = (_float)m_vDownSampleSize2.y;


		//	m_pContext->RSSetViewports(1, &ViewPortDesc);


		//	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_DownSample2"))))
		//		return E_FAIL;

		//	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_DownSample1"), "g_BackBuffer")))
		//		return E_FAIL;

		//	GAMEINSTANCE->BeginShader();

		//	if (FAILED(GAMEINSTANCE->EndMRT()))
		//		return E_FAIL;

		//	/*25x25 Blur*/

		//	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 4);

		//	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_BlurH_25"))))
		//		return E_FAIL;


		//	/*Texel Offset*/
		//	float fTexOffset = 5.f / 1280.f;
		//	if (FAILED(GAMEINSTANCE->BindRawValue("g_fTexOffset", &fTexOffset, sizeof(fTexOffset))))
		//		return E_FAIL;

		//	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_DownSample2"), "g_DownSampleTexture")))
		//		return E_FAIL;

		//	GAMEINSTANCE->BeginShader();

		//	if (FAILED(GAMEINSTANCE->EndMRT()))
		//		return E_FAIL;


		//	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 5);

		//	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_BlurHV_25"))))
		//		return E_FAIL;

		//	/*Texel Offset*/
		//	fTexOffset = 5.f / 720.f;
		//	if (FAILED(GAMEINSTANCE->BindRawValue("g_fTexOffset", &fTexOffset, sizeof(fTexOffset))))
		//		return E_FAIL;

		//	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_BlurH_25"), "g_DownSampleTexture")))
		//		return E_FAIL;

		//	GAMEINSTANCE->BeginShader();

		//	if (FAILED(GAMEINSTANCE->EndMRT()))
		//		return E_FAIL;


		//	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 3);
		//	ViewPortDesc.Width = (_float)m_vDownSampleSize1.x;
		//	ViewPortDesc.Height = (_float)m_vDownSampleSize1.y;

		//	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_UpSample2"))))
		//		return E_FAIL;

		//	float fAspect = ViewPortDesc.Height / ViewPortDesc.Width;

		//	if (FAILED(GAMEINSTANCE->BindRawValue("g_fAspect", &fAspect, sizeof(fAspect))))
		//		return E_FAIL;

		//	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_BlurHV_25"), "g_BloomTexture")))
		//		return E_FAIL;

		//	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_BlurHV_5"), "g_DownSampleTexture")))
		//		return E_FAIL;

		//	if (FAILED(GAMEINSTANCE->BeginShader()))
		//		return E_FAIL;

		//	if (FAILED(GAMEINSTANCE->EndMRT()))
		//		return E_FAIL;


		//	///*UpSampling*/
		//	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 7);
		//	ViewPortDesc.Width = (_float)1280.f;
		//	ViewPortDesc.Height = (_float)720.f;

		//	m_pContext->RSSetViewports(1, &ViewPortDesc);

		//	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_UpSample3"))))
		//		return E_FAIL;

		//	fAspect = ViewPortDesc.Height / ViewPortDesc.Width;

		//	if (FAILED(GAMEINSTANCE->BindRawValue("g_fAspect", &fAspect, sizeof(fAspect))))
		//		return E_FAIL;
		//	
		//	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_UpSample2"), "g_DownSampleTexture")))
		//		return E_FAIL;

		//	if (FAILED(GAMEINSTANCE->BeginShader()))
		//		return E_FAIL;

		//	if (FAILED(GAMEINSTANCE->EndMRT()))
		//		return E_FAIL;


	}


	///*DownSampling3*/

	//GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 1);

	//{
	//	D3D11_VIEWPORT			ViewPortDesc;
	//	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	//	ViewPortDesc.TopLeftX = 0;
	//	ViewPortDesc.TopLeftY = 0;
	//	ViewPortDesc.Width = (_float)m_vDownSampleSize3.x;
	//	ViewPortDesc.Height = (_float)m_vDownSampleSize3.y;
	//	ViewPortDesc.MinDepth = 0.f;
	//	ViewPortDesc.MaxDepth = 1.f;

	//	m_pContext->RSSetViewports(1, &ViewPortDesc);
	//
	//	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_DownSample3"))))
	//		return E_FAIL;

	//	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_DownSample2"), "g_BackBuffer")))
	//		return E_FAIL;

	//	GAMEINSTANCE->BeginShader();

	//	if (FAILED(GAMEINSTANCE->EndMRT()))
	//		return E_FAIL;


	//	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 4);

	//	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_BlurH_125"))))
	//		return E_FAIL;

	//	/*Texel Offset*/
	//	float fTexOffset = 1.f/ (1280.f * 125.f);
	//	if (FAILED(GAMEINSTANCE->BindRawValue("g_fTexOffset", &fTexOffset, sizeof(fTexOffset))))
	//		return E_FAIL;


	//	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_DownSample3"), "g_DownSampleTexture")))
	//		return E_FAIL;

	//	GAMEINSTANCE->BeginShader();

	//	if (FAILED(GAMEINSTANCE->EndMRT()))
	//		return E_FAIL;

	//	/*125x125 Blur*/

	//	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 5);

	//	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_BlurHV_125"))))
	//		return E_FAIL;

	//	/*Texel Offset*/
	//	fTexOffset = 1.f / (720.f * 125.f);
	//	if (FAILED(GAMEINSTANCE->BindRawValue("g_fTexOffset", &fTexOffset, sizeof(fTexOffset))))
	//		return E_FAIL;

	//	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_BlurH_125"), "g_DownSampleTexture")))
	//		return E_FAIL;

	//	if (FAILED(GAMEINSTANCE->BeginShader()))
	//		return E_FAIL;

	//	if (FAILED(GAMEINSTANCE->EndMRT()))
	//		return E_FAIL;
	//}


	///*UpSampling1*/

	//{
	//	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 3);

	//	D3D11_VIEWPORT			ViewPortDesc;
	//	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	//	ViewPortDesc.TopLeftX = 0;
	//	ViewPortDesc.TopLeftY = 0;
	//	ViewPortDesc.Width = (_float)m_vDownSampleSize2.x;
	//	ViewPortDesc.Height = (_float)m_vDownSampleSize2.y;
	//	ViewPortDesc.MinDepth = 0.f;
	//	ViewPortDesc.MaxDepth = 1.f;

	//	m_pContext->RSSetViewports(1, &ViewPortDesc);


	//	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_UpSample1"))))
	//		return E_FAIL;

	//	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_BlurH_125"), "g_BloomTexture")))
	//		return E_FAIL;

	//	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_DownSample2"), "g_DownSampleTexture")))
	//		return E_FAIL;

	//	if (FAILED(GAMEINSTANCE->BeginShader()))
	//		return E_FAIL;

	//	if (FAILED(GAMEINSTANCE->EndMRT()))
	//		return E_FAIL;
	//}

	///*Upsampling2*/

	//{
	//	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 3);

	//	D3D11_VIEWPORT			ViewPortDesc;
	//	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	//	ViewPortDesc.TopLeftX = 0;
	//	ViewPortDesc.TopLeftY = 0;
	//	ViewPortDesc.Width = (_float)m_vDownSampleSize1.x;
	//	ViewPortDesc.Height = (_float)m_vDownSampleSize1.y;
	//	ViewPortDesc.MinDepth = 0.f;
	//	ViewPortDesc.MaxDepth = 1.f;

	//	m_pContext->RSSetViewports(1, &ViewPortDesc);

	//	if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_UpSample2"))))
	//		return E_FAIL;

	//	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_BlurH_25"), "g_BloomTexture")))
	//		return E_FAIL;

	//	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_DownSample1"), "g_DownSampleTexture")))
	//		return E_FAIL;

	//	if (FAILED(GAMEINSTANCE->BeginShader()))
	//		return E_FAIL;

	//	if (FAILED(GAMEINSTANCE->EndMRT()))
	//		return E_FAIL;
	//}
	//

	{

		//if (FAILED(GAMEINSTANCE->BeginMRT(TEXT("MRT_UpSample3"))))
		//	return E_FAIL;

		////if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_BlurH_5"), "g_BloomTexture")))
		////	return E_FAIL;

		////if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_UpSample2"), "g_DownSampleTexture")))
		////	return E_FAIL;

		//if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_DownSample1"), "g_BloomTexture")))
		//	return E_FAIL;

		//if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_DownSample1"), "g_DownSampleTexture")))
		//	return E_FAIL;

		//if (FAILED(GAMEINSTANCE->BeginShader()))
		//	return E_FAIL;

		//if (FAILED(GAMEINSTANCE->EndMRT()))
		//	return E_FAIL;

	}

	////Blend

	//GAMEINSTANCE->BindBackBufferSRV("g_BloomTexture");
	////if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_BlurHV_5"), "g_BloomTexture")))
	////	return E_FAIL;

	//if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_UpSample3"), "g_DownSampleTexture")))
	//	return E_FAIL;

	//GAMEINSTANCE->BeginShader();



	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_POSTPROCESS, 6);
	GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix);
	GAMEINSTANCE->BindBackBufferSRV("g_BackBufferTexture");

	//MRT_UpSample3

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_UpSample3"), "g_BlendTexture")))
		return E_FAIL;

	////Target_BlurHV_125
	GAMEINSTANCE->BeginShader();



	return S_OK;
}



HRESULT CRenderer::RenderFxaa()
{

	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_FXAA, 0);
	GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix);

	if (FAILED(GAMEINSTANCE->BindBackBufferSRV("g_FinalTarget")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BeginShader()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::RenderScreenTone()
{
	if (!m_pLUTTexture) {
		m_pLUTTexture = GAMEINSTANCE->GetSRV("LUT_ENV_OW_Standard_Postariz16");
	}


	GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_HDR, 2);
	GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix);

	GAMEINSTANCE->BindRawValue("g_fSaturation", &g_fSaturate, sizeof(g_fSaturate));
	GAMEINSTANCE->BindRawValue("g_fContrast", &g_fConstrast, sizeof(g_fConstrast));
	GAMEINSTANCE->BindRawValue("g_fMiddleGray", &g_fMiddleGray, sizeof(g_fMiddleGray));

	GAMEINSTANCE->BindSRVDirect("g_LUTTexture", m_pLUTTexture);

	if (FAILED(GAMEINSTANCE->BindBackBufferSRV("g_BackBuffer")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BeginShader()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::RenderWorldUI()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_WORLDUI]) {
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	m_RenderObjects[RENDER_WORLDUI].clear();

	return S_OK;
}

HRESULT CRenderer::RenderUI()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_UI]) {
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::RenderUIWorld()
{
	for (auto& pGameObject : m_RenderUIs[RENDER_UI_WORLD]) {
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	m_RenderUIs[RENDER_UI_WORLD].clear();

	return S_OK;
}

HRESULT CRenderer::RenderUIHUD()
{
	for (auto& pGameObject : m_RenderUIs[RENDER_UI_HUD]) {
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	m_RenderUIs[RENDER_UI_HUD].clear();

	return S_OK;
}

HRESULT CRenderer::RenderUIContent()
{
	for (auto& pGameObject : m_RenderUIs[RENDER_UI_CONTENT]) {
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	m_RenderUIs[RENDER_UI_CONTENT].clear();

	return S_OK;
}

HRESULT CRenderer::RenderUIInventory()
{
	for (auto& pGameObject : m_RenderUIs[RENDER_UI_INVENTORY]) {
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	m_RenderUIs[RENDER_UI_INVENTORY].clear();

	return S_OK;
}

HRESULT CRenderer::RenderUIPopUp()
{
	for (auto& pGameObject : m_RenderUIs[RENDER_UI_POPUP]) {
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	m_RenderUIs[RENDER_UI_POPUP].clear();

	return S_OK;
}

HRESULT CRenderer::RenderUIEffect()
{
	for (auto& pGameObject : m_RenderUIs[RENDER_UI_EFFECT]) {
		if (nullptr != pGameObject)
			pGameObject->Render();
	}

	m_RenderUIs[RENDER_UI_EFFECT].clear();

	return S_OK;
}

HRESULT CRenderer::RenderFadeInOut()
{
	if (m_IsBlackOut) {

		GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_POSTPROCESS, 10);

		if (FAILED(GAMEINSTANCE->BeginShader()))
			return E_FAIL;

	}
	else {



		if (!m_IsFadeIn && !m_IsFadeOut)
			return S_OK;

		GAMEINSTANCE->ReadyShader(ESHADER_TYPE::ST_POSTPROCESS, 9);


		m_fFadeAccTime += 0.016f;

		_float fRate = BtwA2B(m_fFadeDuration != 0.0f ? m_fFadeAccTime / m_fFadeDuration : 1.0f, 0.0f, 1.0f);

		if (m_IsFadeOut)
		{
			fRate = 1.f - fRate;
		}

		if (FAILED(GAMEINSTANCE->BindRawValue("g_fFadeRate", &fRate, sizeof(fRate))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindBackBufferSRV("g_BackBufferTexture")))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BeginShader()))
			return E_FAIL;

		if (m_fFadeDuration <= m_fFadeAccTime) {

			if (m_IsFadeOut) {
				m_IsBlackOut = true;
			}

			m_IsFadeIn = false;
			m_IsFadeOut = false;
			m_fFadeAccTime = m_fFadeDuration;
		}

	}

	return S_OK;
}

HRESULT CRenderer::RenderForFilmVideo()
{

	if (GAMEINSTANCE->ReadyShader(ST_POSTPROCESS, 12))
		return E_FAIL;

	if (GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindBackBufferSRV("g_BackBufferTexture")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindTargetSRV(TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BeginShader()))
		return E_FAIL;

	return S_OK;
}

void CRenderer::SortRenderList(ERENDER_GROUP _eRenderGroup)
{
	m_RenderObjects[_eRenderGroup].sort([](shared_ptr<CGameObject> _pSrc, shared_ptr<CGameObject> _pDst)
		{
			return _pSrc->GetDistanceFromCam() > _pDst->GetDistanceFromCam();
		});
}

//#ifdef _DEBUG
HRESULT CRenderer::AddDebugComponent(shared_ptr<class CComponent> _pComponent)
{

	m_DebugCom.push_back(_pComponent);

	return S_OK;
}
//#endif



//#ifdef _DEBUG
HRESULT CRenderer::RenderDebug()
{

	/*Render Target Debug*/
	if (m_IsRenderTargetOn) {

		GAMEINSTANCE->ReadyShader(m_eDeferredST, 0);

		GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_WorldMatrix);

		if (0 == m_iCurretRenderGroupIdx) {
			if (m_IsTargetGroup1Mode)
			{
				CGameInstance::GetInstance()->RenderMRT(TEXT("MRT_Edge"));
				CGameInstance::GetInstance()->RenderMRT(TEXT("MRT_Edge_Depth"));
				CGameInstance::GetInstance()->RenderMRT(TEXT("MRT_Glow_DownSample"));

			}
			else
			{
				CGameInstance::GetInstance()->RenderMRT(TEXT("MRT_PixelPicking"));
				CGameInstance::GetInstance()->RenderMRT(TEXT("MRT_Distortion"));
				CGameInstance::GetInstance()->RenderMRT(TEXT("MRT_Glow_DownSample"));

			}
		}
		else if (1 == m_iCurretRenderGroupIdx) {


			CGameInstance::GetInstance()->RenderMRT(TEXT("MRT_GameObjects"));
			CGameInstance::GetInstance()->RenderMRT(TEXT("MRT_LightAcc"));
			CGameInstance::GetInstance()->RenderMRT(TEXT("MRT_Luminance"));
			CGameInstance::GetInstance()->RenderMRT(TEXT("MRT_Cascade"));
			CGameInstance::GetInstance()->RenderMRT(TEXT("MRT_Glow"));
			CGameInstance::GetInstance()->RenderMRT(TEXT("MRT_Trail"));

			CGameInstance::GetInstance()->RenderMRT(TEXT("MRT_Glow_DownSample"));

		}


		GAMEINSTANCE->BeginShader();

	}


	/*Collider Debug*/
	if (m_IsDebugMode) {

		for (auto& pComponent : m_DebugCom)
		{
			pComponent->Render();
		}

	}

	return S_OK;
}

//#endif

void CRenderer::RendererOptionSwitching(_uint _iLevel)
{
	//Dungeon
	if (2 == _iLevel || 3 == _iLevel) {

		CRenderer::g_fConstrast = 0.541f;
		CRenderer::g_fMiddleGray = 0.521f;
		CRenderer::g_fSaturate = 0.54f;
		CRenderer::g_fFogStart = 60.f;

	}
	//village
	else if (4 == _iLevel || 5 == _iLevel)
	{
		CRenderer::g_fConstrast = 0.601f;
		CRenderer::g_fMiddleGray = 0.511f;
		CRenderer::g_fSaturate = 0.58f;
		CRenderer::g_fFogStart = 350.f;
	}


	if (3 == _iLevel) {

		m_UsingDOF = false;
		m_UsingFog = false;
	}

}

shared_ptr<CRenderer> CRenderer::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
{
	shared_ptr<CRenderer> pInstance = make_shared<CRenderer>(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CRenderer");

	return pInstance;
}
