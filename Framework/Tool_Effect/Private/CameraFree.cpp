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

	return S_OK;
}

void CCameraFree::PriorityTick(_float _fTimeDelta)
{
	if (GAMEINSTANCE->KeyDown(DIK_Q))
		m_IsCamLock = !m_IsCamLock;

	if (m_IsCamLock)
	{
		//_float3 vDir;

		//if (GAMEINSTANCE->KeyPressing(DIK_W))
		//{
		//	vDir = m_pTransformCom->GetState(CTransform::STATE_LOOK);
		//	vDir.y = 0.f;

		//	if (GAMEINSTANCE->KeyPressing(DIK_LSHIFT))
		//		m_pTransformCom->GoDir(_fTimeDelta * 3.f, vDir);

		//	else
		//		m_pTransformCom->GoDir(_fTimeDelta, vDir);
		//}

		//if (GAMEINSTANCE->KeyPressing(DIK_S))
		//{
		//	vDir = -m_pTransformCom->GetState(CTransform::STATE_LOOK);
		//	vDir.y = 0.f;

		//	if (GAMEINSTANCE->KeyPressing(DIK_LSHIFT))
		//		m_pTransformCom->GoDir(_fTimeDelta * 3.f, vDir);

		//	else
		//		m_pTransformCom->GoDir(_fTimeDelta, vDir);
		//}

		//if (GAMEINSTANCE->KeyPressing(DIK_A))
		//{
		//	vDir = -m_pTransformCom->GetState(CTransform::STATE_RIGHT);
		//	vDir.y = 0.f;

		//	if (GAMEINSTANCE->KeyPressing(DIK_LSHIFT))
		//		m_pTransformCom->GoDir(_fTimeDelta * 3.f, vDir);

		//	else
		//		m_pTransformCom->GoDir(_fTimeDelta, vDir);
		//}

		//if (GAMEINSTANCE->KeyPressing(DIK_D))
		//{
		//	vDir = m_pTransformCom->GetState(CTransform::STATE_RIGHT);
		//	vDir.y = 0.f;

		//	if (GAMEINSTANCE->KeyPressing(DIK_LSHIFT))
		//		m_pTransformCom->GoDir(_fTimeDelta * 3.f, vDir);

		//	else
		//		m_pTransformCom->GoDir(_fTimeDelta, vDir);
		//}
	}

	else
	{
		if (GAMEINSTANCE->KeyPressing(DIK_W))
		{
			if (GAMEINSTANCE->KeyPressing(DIK_LSHIFT))
				m_pTransformCom->GoStraight(_fTimeDelta * 3.f);

			else
				m_pTransformCom->GoStraight(_fTimeDelta);
		}

		if (GAMEINSTANCE->KeyPressing(DIK_S))
		{
			if (GAMEINSTANCE->KeyPressing(DIK_LSHIFT))
				m_pTransformCom->GoBackward(_fTimeDelta * 3.f);

			else
				m_pTransformCom->GoBackward(_fTimeDelta);
		}

		if (GAMEINSTANCE->KeyPressing(DIK_A))
		{
			if (GAMEINSTANCE->KeyPressing(DIK_LSHIFT))
				m_pTransformCom->GoLeft(_fTimeDelta * 3.f);

			else
				m_pTransformCom->GoLeft(_fTimeDelta);
		}

		if (GAMEINSTANCE->KeyPressing(DIK_D))
		{
			if (GAMEINSTANCE->KeyPressing(DIK_LSHIFT))
				m_pTransformCom->GoRight(_fTimeDelta * 3.f);

			else
				m_pTransformCom->GoRight(_fTimeDelta);
		}

		_long MouseMove = 0;

		if (MouseMove = CGameInstance::GetInstance()->GetDIMouseMove(DIMS_X))
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _fTimeDelta * MouseMove * m_fMouseSensor);

		if (MouseMove = CGameInstance::GetInstance()->GetDIMouseMove(DIMS_Y))
			m_pTransformCom->Turn({ m_pTransformCom->GetState(CTransform::STATE_RIGHT).x , m_pTransformCom->GetState(CTransform::STATE_RIGHT).y , m_pTransformCom->GetState(CTransform::STATE_RIGHT).z }
		, _fTimeDelta * MouseMove * m_fMouseSensor);
	}

	if (FAILED(SetUpTransformMatices()))
		return;

}

void CCameraFree::Tick(_float _fTimeDelta)
{
}

void CCameraFree::LateTick(_float _fTimeDelta)
{
}

HRESULT CCameraFree::Render()
{
	return S_OK;

}

void CCameraFree::KeyInput()
{

}

shared_ptr<CCameraFree> CCameraFree::Create(_bool _IsEnable, CAMERAFREE_DESC* _pCamFreeDesc, CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc)
{
	shared_ptr<CCameraFree> pInstance = make_shared<CCameraFree>();

	if (FAILED(pInstance->Initialize(_IsEnable, _pCamFreeDesc, _pCamDes, _TransDesc)))
		MSG_BOX("Failed to Create : CCamFree");


	return pInstance;
}
