#include "EffectHelper.h"

CEffectHelper::CEffectHelper()
{
}

CEffectHelper::~CEffectHelper()
{
}

HRESULT CEffectHelper::Initialize(EffectHelper_Desc eDesc)
{
	m_eDesc = eDesc;

	__super::Initialize(nullptr);

	return S_OK;
}

void CEffectHelper::PriorityTick(_float _fTimeDelta)
{
	m_eDesc.fLifeTime -= _fTimeDelta;

	if (0 >= m_eDesc.fLifeTime)
	{
		m_bPlay = false;
		m_IsEnabled = false;
	}
}

void CEffectHelper::Tick(_float _fTimeDelta)
{
	
}

void CEffectHelper::Play(_float _fLifeTime, shared_ptr<CGameObject> _pOwner)
{
	m_bPlay = true;
	m_IsEnabled = true;

	m_pOwner = _pOwner;

	_float3 vStartPos = m_eDesc.HelperKeyFrames.begin()->vPos;
	_float4x4 StartOwnerMatrix;

	if (m_eDesc.bFollowOwnerPos && m_pOwner.lock())
	{
		switch (m_eDesc.iLookType)
		{
		case -1:
			m_pTransformCom->SetState(CTransform::STATE_POSITION, vStartPos + m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_POSITION));
			break;
		//case 0:
		//	StartOwnerMatrix = m_pOwner.lock()->GetTransform()->GetWorldMatrix();
		//	// 부모의 월드 행렬에서 회전 부분(3x3 부분)을 추출하여 Z 축 벡터를 얻어냅니다.
		//	_float3 parentWorldZAxis = -SimpleMath::Vector3::TransformNormal(SimpleMath::Vector3::UnitZ, StartOwnerMatrix);

		//	// 자식의 로컬 포지션 벡터를 부모의 월드 행렬의 Look 벡터에 대한 회전 행렬로 변환합니다.
		//	_float3 rotatedLocalPosition = SimpleMath::Vector3::Transform(vStartPos, SimpleMath::Matrix::CreateWorld(SimpleMath::Vector3::Zero, parentWorldZAxis, SimpleMath::Vector3::UnitY));

		//	// 부모의 월드 행렬의 위치 벡터에 회전된 자식의 로컬 포지션 벡터를 더하여 자식의 월드 행렬의 위치를 계산합니다.
		//	_float3 childWorldPosition = StartOwnerMatrix.Translation() + rotatedLocalPosition;

		//	// 자식의 월드 행렬을 구성합니다.
		//	vStartPos = SimpleMath::Matrix::CreateTranslation(childWorldPosition).Translation();

		//	m_pTransformCom->SetState(CTransform::STATE_POSITION, vStartPos);
		//	break;
		//case 1:
		//	StartOwnerMatrix = m_pOwner.lock()->GetTransform()->GetWorldMatrix();
		//	// 부모의 월드 행렬에서 회전 부분(3x3 부분)을 추출하여 Z 축 벡터를 얻어냅니다.
		//	_float3 parentWorldZAxis = -SimpleMath::Vector3::TransformNormal(SimpleMath::Vector3::UnitZ, StartOwnerMatrix);

		//	// 자식의 로컬 포지션 벡터를 부모의 월드 행렬의 Look 벡터에 대한 회전 행렬로 변환합니다.
		//	_float3 rotatedLocalPosition = SimpleMath::Vector3::Transform(vStartPos, SimpleMath::Matrix::CreateWorld(SimpleMath::Vector3::Zero, parentWorldZAxis, SimpleMath::Vector3::UnitY));

		//	// 부모의 월드 행렬의 위치 벡터에 회전된 자식의 로컬 포지션 벡터를 더하여 자식의 월드 행렬의 위치를 계산합니다.
		//	_float3 childWorldPosition = StartOwnerMatrix.Translation() + rotatedLocalPosition;

		//	// 자식의 월드 행렬을 구성합니다.
		//	vStartPos = SimpleMath::Matrix::CreateTranslation(childWorldPosition).Translation();

		//	m_pTransformCom->SetState(CTransform::STATE_POSITION, vStartPos);
		//	break;
		default:
			break;
		}
	}

	else
	{
		m_pTransformCom->SetState(CTransform::STATE_POSITION, vStartPos);
	}
}

shared_ptr<CEffectHelper> CEffectHelper::Create(EffectHelper_Desc eDesc)
{
	shared_ptr<CEffectHelper> pInstance = make_shared<CEffectHelper>();

	if (FAILED(pInstance->Initialize(eDesc)))
		MSG_BOX("Failed to Create : CEffectHelper");

	return pInstance;
}
