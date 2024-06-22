#include "TargetDirection.h"
#include "GameInstance.h"
#include "Model.h"

CTargetDirection::CTargetDirection()
{
}

HRESULT CTargetDirection::Initialize(shared_ptr<TargetDirection_Desc> _pTargetDirectionDesc)
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 1.f;
	pArg->fRotationPerSec = XMConvertToRadians(10.0f);
	__super::Initialize(pArg.get());

	m_pModel = GAMEINSTANCE->GetModel("TargetDirection_01");

	m_MyDesc.pParentTransform = _pTargetDirectionDesc->pParentTransform;

	m_eShaderType = ST_NONANIM;
	m_iShaderPass = 0;

	m_pTransformCom->SetScaling(30.f, 30.f, 30.f);

	_float3 vMyPos = m_MyDesc.pParentTransform->GetState(CTransform::STATE_POSITION);
	vMyPos.y += 8.f;
	m_pTransformCom->SetState(CTransform::STATE_POSITION, vMyPos);

	vMyPos.x += 1.f;
	m_pTransformCom->LookAtForLandObject(vMyPos);

	return S_OK;
}

void CTargetDirection::PriorityTick(_float _fTimeDelta)
{
}

void CTargetDirection::Tick(_float _fTimeDelta)
{

}

void CTargetDirection::LateTick(_float _fTimeDelta)
{
	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CTargetDirection::Render()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	_uint iNumMeshes = m_pModel->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	 {
		GAMEINSTANCE->BeginNonAnimModel(m_pModel, i);
	}

	return S_OK;
}

shared_ptr<CTargetDirection> CTargetDirection::Create(shared_ptr<TargetDirection_Desc> _pTargetDirectionDesc)
{
	shared_ptr<CTargetDirection> pTargetDirection = make_shared<CTargetDirection>();

	if (FAILED(pTargetDirection->Initialize(_pTargetDirectionDesc)))
		MSG_BOX("Failed to Create : CTargetDirection");

	return pTargetDirection;
}
