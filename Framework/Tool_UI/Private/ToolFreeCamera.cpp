#include "pch.h"
#include "ToolFreeCamera.h"

#include "GameInstance.h"

Tool_UI::CToolFreeCamera::CToolFreeCamera()
	: CCamera()
{

}

Tool_UI::CToolFreeCamera::~CToolFreeCamera()
{

}

HRESULT Tool_UI::CToolFreeCamera::Initialize(_bool _IsEnable, TOOLCAMERAFREE_DESC* _pCamFreeDesc, CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc)
{
	m_fMouseSensor = _pCamFreeDesc->fMouseSensor;

	if (FAILED(Super::Initialize(_pCamDes, _TransDesc)))
		return E_FAIL;

	return S_OK;
}

void Tool_UI::CToolFreeCamera::PriorityTick(_float fTimeDelta)
{
	if (FAILED(SetUpTransformMatices()))
		return;
}

void Tool_UI::CToolFreeCamera::Tick(_float fTimeDelta)
{

}

void Tool_UI::CToolFreeCamera::LateTick(_float fTimeDelta)
{

}

HRESULT Tool_UI::CToolFreeCamera::Render()
{

	return S_OK;
}

void Tool_UI::CToolFreeCamera::KeyInput()
{
}

shared_ptr<Tool_UI::CToolFreeCamera> Tool_UI::CToolFreeCamera::Create(_bool _IsEnable, TOOLCAMERAFREE_DESC* _pCamFreeDesc, CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc)
{
	shared_ptr<CToolFreeCamera> pInstance = make_shared<CToolFreeCamera>();

	if (FAILED(pInstance->Initialize(_IsEnable, _pCamFreeDesc, _pCamDes, _TransDesc)))
		MSG_BOX("Failed to Create : CToolFreeCamera");


	return pInstance;
}
