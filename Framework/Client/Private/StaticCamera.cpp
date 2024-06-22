#include "pch.h"
#include "StaticCamera.h"
#include "CameraMgr.h"

CStaticCamera::CStaticCamera()
{
}

shared_ptr<CStaticCamera> CStaticCamera::Create()
{
	shared_ptr<CStaticCamera> pInstance = make_shared<CStaticCamera>();

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CStaticCamera::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CStaticCamera::Initialize()
{
	CAMERA_DESC CamDesc{};
	CamDesc.fAspect = (float)g_iWinSizeX / (float)g_iWinSizeY;
	CamDesc.fFovy = XM_PI * 0.3333333f;

	if (FAILED(CCamera::Initialize(&CamDesc)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CCameraSpherical::Initialize", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

void CStaticCamera::PriorityTick(_float fTimeDelta)
{
	if (m_isRecording)
	{
		CCamera::PriorityTick(fTimeDelta);
		SetUpTransformMatices();
		SetUpCascadeDesc();
	}
}

void CStaticCamera::Tick(_float fTimeDelta)
{

}

void CStaticCamera::LateTick(_float fTimeDelta)
{

}

HRESULT CStaticCamera::Render()
{
	return S_OK;
}