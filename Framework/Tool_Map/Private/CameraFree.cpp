#include "CameraFree.h"
#include "GameInstance.h"

CCameraFree::CCameraFree()
	:CCamera()
{
}

#ifdef CHECK_REFERENCE_COUNT
CCameraFree::~CCameraFree()
{
}
#endif // CHECK_REFERENCE_COUNT

HRESULT CCameraFree::Initialize(_bool _IsEnable, CAMERAFREE_DESC* _pCamFreeDesc, CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc)
{
	m_fMouseSensor = _pCamFreeDesc->fMouseSensor;

	if (FAILED(__super::Initialize(_pCamDes, _TransDesc)))
		return E_FAIL;

	return S_OK;
}

void CCameraFree::PriorityTick(_float fTimeDelta)
{

	KeyInput(fTimeDelta);

	if (FAILED(SetUpTransformMatices()))
		return;

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

void CCameraFree::KeyInput(_float _fTimeDelta)
{

	if (GAMEINSTANCE->KeyPressing(DIK_W))
	{
		m_pTransformCom->GoStraight(_fTimeDelta);
	}

	if (GAMEINSTANCE->KeyPressing(DIK_S))
	{
		m_pTransformCom->GoBackward(_fTimeDelta);
	}

	if (GAMEINSTANCE->KeyPressing(DIK_A))
	{
		m_pTransformCom->GoLeft(_fTimeDelta);
	}

	if (GAMEINSTANCE->KeyPressing(DIK_D))
	{
		m_pTransformCom->GoRight(_fTimeDelta);
	}

	if (!m_IsCamLock) {

		_long MouseMove = 0;

		if (MouseMove = CGameInstance::GetInstance()->GetDIMouseMove(DIMS_X))
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _fTimeDelta * MouseMove * m_fMouseSensor);

		if (MouseMove = CGameInstance::GetInstance()->GetDIMouseMove(DIMS_Y))
			m_pTransformCom->Turn({ m_pTransformCom->GetState(CTransform::STATE_RIGHT).x , m_pTransformCom->GetState(CTransform::STATE_RIGHT).y , m_pTransformCom->GetState(CTransform::STATE_RIGHT).z }
		, _fTimeDelta * MouseMove * m_fMouseSensor);

	}

	// Camera Lock
	/*
	if (GAMEINSTANCE->KeyDown(DIK_L))
	{
		m_IsCamLock = !m_IsCamLock;
	}
	*/
}

shared_ptr<CCameraFree> CCameraFree::Create(_bool _IsEnable, CAMERAFREE_DESC* _pCamFreeDesc, CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc)
{
	shared_ptr<CCameraFree> pInstance = WRAPPING(CCameraFree)();

	if (FAILED(pInstance->Initialize(_IsEnable, _pCamFreeDesc, _pCamDes, _TransDesc)))
	{
		MSG_BOX("Failed to Created : CCamFree");
		pInstance.reset();
	}

	return pInstance;
}
