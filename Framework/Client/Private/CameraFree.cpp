#include "CameraFree.h"
#include "GameInstance.h"

CCameraFree::CCameraFree()
	:CCamera()
{
}

CCameraFree::~CCameraFree()
{
}

HRESULT CCameraFree::Initialize(_bool _IsEnable, CAMERAFREE_DESC* _pCamFreeDesc, CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc)
{
	m_fMouseSensor = _pCamFreeDesc->fMouseSensor;

	if (FAILED(__super::Initialize(_pCamDes, _TransDesc)))
		return E_FAIL;

	m_IsMouseLock = false;
	return S_OK;
}

void CCameraFree::PriorityTick(_float fTimeDelta)
{
	if (m_isRecording)
	{
		KeyInput(fTimeDelta);
		if (m_IsMouseLock) {
			CursorFixCenter();
		}

		__super::PriorityTick(fTimeDelta);

		if (FAILED(SetUpTransformMatices()))
			return;
	}

}

void CCameraFree::Tick(_float fTimeDelta)
{

}

void CCameraFree::LateTick(_float fTimeDelta)
{
}

HRESULT CCameraFree::Render()
{
	return S_OK;

}

void CCameraFree::CursorFixCenter()
{

	POINT	ptMouse{ Client::g_iWinSizeX >> 1, Client::g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);

}

void CCameraFree::KeyInput(_float _fTimeDelta)
{

	if (GAMEINSTANCE->KeyPressing(DIK_UP))
	{
		m_pTransformCom->GoStraight(_fTimeDelta);
	}

	if (GAMEINSTANCE->KeyPressing(DIK_DOWN))
	{
		m_pTransformCom->GoBackward(_fTimeDelta);
	}

	if (GAMEINSTANCE->KeyPressing(DIK_LEFT))
	{
		m_pTransformCom->GoLeft(_fTimeDelta);
	}

	if (GAMEINSTANCE->KeyPressing(DIK_RIGHT))
	{
		m_pTransformCom->GoRight(_fTimeDelta);
	}

	if (m_IsMouseLock) {

		_long MouseMove = 0;

		if (MouseMove = CGameInstance::GetInstance()->GetDIMouseMove(DIMS_X))
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _fTimeDelta * MouseMove * m_fMouseSensor);

		if (MouseMove = CGameInstance::GetInstance()->GetDIMouseMove(DIMS_Y))
			m_pTransformCom->Turn(m_pTransformCom->GetState(CTransform::STATE_RIGHT), _fTimeDelta * MouseMove * m_fMouseSensor);

	}

}

shared_ptr<CCameraFree> CCameraFree::Create(_bool _IsEnable, CAMERAFREE_DESC* _pCamFreeDesc, CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc)
{
	shared_ptr<CCameraFree> pInstance = make_shared<CCameraFree>();

	if (FAILED(pInstance->Initialize(_IsEnable, _pCamFreeDesc, _pCamDes, _TransDesc)))
		MSG_BOX("Failed to Create : CCamFree");


	return pInstance;
}
