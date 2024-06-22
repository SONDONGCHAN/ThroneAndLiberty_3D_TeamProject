#include "EffectMesh.h"
#include "Model.h"

CEffectMesh::CEffectMesh()
{
}

CEffectMesh::CEffectMesh(const CEffectMesh& rhs)
{
}

CEffectMesh::~CEffectMesh()
{
}

HRESULT CEffectMesh::Initialize(EffectMeshDesc* pDesc)
{
	// 선행되어야 하는 코드임
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	Initialize_Desc(pDesc);
	Reset();

	// 독립적인 회전을 하는 애는, 미리 회전값으로 바꿔놓고, 회전 속도와 가속도만 적용해주기
	if (-1 == m_eEffectDesc.iRotFollowType)
		m_pTransformCom->RotationAll(m_eEffectDesc.vRotation.x, m_eEffectDesc.vRotation.y, m_eEffectDesc.vRotation.z);

	m_pModelCom = GAMEINSTANCE->GetModel(m_eEffectDesc.strModelKey);
	m_fDistortionTimer = 0.f;

	// 테스트용 코드
	//  SetOwner(GAMEINSTANCE->GetGameObject(LEVEL_LOGO, TEXT("Layer_AnimObject")));

	return S_OK;
}

void CEffectMesh::PriorityTick(_float _fTimeDelta)
{
	__super::PriorityTick(_fTimeDelta);
}

void CEffectMesh::Tick(_float _fTimeDelta)
{
	m_ePrevState = m_eState;

	if (!m_bStart)
		return;

	if (m_bFinished)
		return;

	if (!m_bPlaying)
		return;

	__super::Tick(_fTimeDelta);

	if (m_eEffectDesc.bUseDiffuse)
	{
		if (1 == m_eEffectDesc.iDiffuseUVType)
		{
			m_vDiffuseUVScroll += m_eEffectDesc.fDiffuseUVScroll;
		}

		else if (2 == m_eEffectDesc.iDiffuseUVType)
		{
			if (m_fActiveTime > 1.f / m_eEffectDesc.fDiffuseUVAtlasSpeed * m_fSpeedTime * m_iDiffuseUVIdx)
			{
				m_iDiffuseUVIdx++;
				m_vDiffuseUVPivot.x += (1.f / m_eEffectDesc.iDiffuseUVAtlasCnt.x);

				if (1 <= m_vDiffuseUVPivot.x)
				{
					m_vDiffuseUVPivot.x = 0;
					m_vDiffuseUVPivot.y += (1.f / m_eEffectDesc.iDiffuseUVAtlasCnt.y);
				}
			}
			if (m_iDiffuseUVIdx >= m_eEffectDesc.iDiffuseUVAtlasCnt.x * m_eEffectDesc.iDiffuseUVAtlasCnt.y)
			{
				if (!m_eEffectDesc.bLoop)
				{
					m_eState = EFFECT_STATE_FINISH;
					m_bFinished = true;

					return;
				}
				else
				{
					m_iDiffuseUVIdx = 0;
					m_vDiffuseUVPivot.x = 0.f;
					m_vDiffuseUVPivot.y = 0.f;
				}
			}
		}
	}

	if (m_eEffectDesc.bUseMask)
	{
		if (1 == m_eEffectDesc.iMaskUVType)
		{
			m_vMaskUVScroll += m_eEffectDesc.fMaskUVScroll;
		}

		else if (2 == m_eEffectDesc.iMaskUVType)
		{
			if (0.f == m_fActiveTime)
			{
				m_iMaskUVIdx = 0;
				m_vMaskUVPivot = { 0.f, 0.f };
			}

			if (m_fActiveTime > 1.f / m_eEffectDesc.fMaskUVAtlasSpeed * m_fSpeedTime * m_iMaskUVIdx)
			{
				m_iMaskUVIdx++;

				if (m_eEffectDesc.iMaskUVAtlasCnt.x * m_eEffectDesc.iMaskUVAtlasCnt.y <= m_iMaskUVIdx
					&& !m_eEffectDesc.bLoop)
				{
					m_eState = EFFECT_STATE_FINISH;
					m_bFinished = true;
				}

				m_vMaskUVPivot.x += (1.f / m_eEffectDesc.iMaskUVAtlasCnt.x);

				if (1 <= m_vMaskUVPivot.x)
				{
					m_vMaskUVPivot.x = 0;
					m_vMaskUVPivot.y += (1.f / m_eEffectDesc.iMaskUVAtlasCnt.y);
				}
			}
		}
	}

	if (m_eEffectDesc.bUseNoise)
	{
		if (1 == m_eEffectDesc.iNoiseUVType)
		{
			m_vNoiseUVScroll += m_eEffectDesc.fNoiseUVScroll;
		}

		else if (2 == m_eEffectDesc.iNoiseUVType)
		{
			if (m_fActiveTime > 1.f / m_eEffectDesc.fNoiseUVAtlasSpeed * m_fSpeedTime * m_iNoiseUVIdx)
			{
				m_iNoiseUVIdx++;
				m_vNoiseUVPivot.x += (1.f / m_eEffectDesc.iNoiseUVAtlasCnt.x);

				if (1 <= m_vNoiseUVPivot.x)
				{
					m_vNoiseUVPivot.x = 0;
					m_vNoiseUVPivot.y += (1.f / m_eEffectDesc.iNoiseUVAtlasCnt.y);
				}
			}
		}
	}

	if (m_eEffectDesc.bUseColor)
	{
		memcpy(m_vCurColor, m_eEffectDesc.KeyFrames.GetCurColor(m_fActiveTime, m_eEffectDesc.iColorSplitArea), sizeof(_float4[4]));
	}

	m_vCurScale = m_eEffectDesc.KeyFrames.GetCurScale(m_fActiveTime);
	m_vCurTrans = m_eEffectDesc.KeyFrames.GetCurTrans(m_fActiveTime);

	// Scale
	switch (m_eEffectDesc.iScaleFollowType)
	{
	case -1:
		m_pTransformCom->SetScaling(m_vCurScale);
		break;
	case 0:
		// 오너 객체가 사라졌을 때 소멸
		if (!CheckOwnerEnable())
			return;

		if (m_pOwner.lock())
		{
			m_pTransformCom->SetScaling(m_pOwner.lock()->GetTransform()->GetScaled() + m_vCurScale);
		}
		break;
	case 1:
		// 오너 객체가 사라졌을 때 소멸
		if (!CheckOwnerEnable())
			return;
		if (m_pSFollowBoneMat)
			m_pTransformCom->SetScaling(_float3((*m_pSFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Right().Length(), (*m_pSFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Up().Length(), (*m_pSFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Forward().Length()) + m_vCurScale);
		break;
	default:
		break;
	}

	// Rotation
	switch (m_eEffectDesc.iRotFollowType)
	{
	case -1:
		// World
		if (m_bFirstRot)
		{
			m_pTransformCom->RotationAll(0.f, 0.f, 0.f);

			// 1. 독립적인 회전값 적용하기
			m_pTransformCom->RotationPlus({ 1.f, 0.f, 0.f }, m_eEffectDesc.vRotation.x);
			m_pTransformCom->RotationPlus({ 0.f, 1.f, 0.f }, m_eEffectDesc.vRotation.y);
			m_pTransformCom->RotationPlus({ 0.f, 0.f, 1.f }, m_eEffectDesc.vRotation.z);

			m_bFirstRot = false;
		}

		// 2. 회전 속도, 회전 가속도 적용하기
		// 회전 속도
		m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, m_eEffectDesc.vTurnVel.x * m_fSpeedTime);
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, m_eEffectDesc.vTurnVel.y * m_fSpeedTime);
		m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, m_eEffectDesc.vTurnVel.z * m_fSpeedTime);

		// 회전 가속도
		m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, m_eEffectDesc.vTurnAcc.x * m_fSpeedTime * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, m_eEffectDesc.vTurnAcc.y * m_fSpeedTime * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, m_eEffectDesc.vTurnAcc.z * m_fSpeedTime * m_fActiveTime);
		break;
	case 0:
		// Object + No Follow
		if (m_bFirstRot)
		{
			// 0. 부모의 라업룩을 적용하기
			if (m_pOwner.lock())
			{
				_quaternion CombinedQuat = m_pOwner.lock()->GetTransform()->GetQuat();

				m_pTransformCom->SetQuaternion(CombinedQuat);
			}

			// 1. 독립적인 회전값 적용하기
			m_pTransformCom->RotationPlus({ 1.f, 0.f, 0.f }, m_eEffectDesc.vRotation.x);
			m_pTransformCom->RotationPlus({ 0.f, 1.f, 0.f }, m_eEffectDesc.vRotation.y);
			m_pTransformCom->RotationPlus({ 0.f, 0.f, 1.f }, m_eEffectDesc.vRotation.z);

			m_bFirstRot = false;
		}

		// 2. 회전 속도, 회전 가속도 적용하기
		// 회전 속도
		m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, m_eEffectDesc.vTurnVel.x * m_fSpeedTime);
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, m_eEffectDesc.vTurnVel.y * m_fSpeedTime);
		m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, m_eEffectDesc.vTurnVel.z * m_fSpeedTime);

		// 회전 가속도
		m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, m_eEffectDesc.vTurnAcc.x * m_fSpeedTime * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, m_eEffectDesc.vTurnAcc.y * m_fSpeedTime * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, m_eEffectDesc.vTurnAcc.z * m_fSpeedTime * m_fActiveTime);

		break;
	case 1:
		// Bone + No Follow
		if (m_bFirstRot)
		{
			// 0. 부모의 라업룩을 적용하기
			if (m_pRFollowBoneMat)
				m_pTransformCom->SetQuaternion(XMQuaternionRotationMatrix(*m_pRFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()));

			// 1. 독립적인 회전값 적용하기
			m_pTransformCom->RotationPlus({ 1.f, 0.f, 0.f }, m_eEffectDesc.vRotation.x);
			m_pTransformCom->RotationPlus({ 0.f, 1.f, 0.f }, m_eEffectDesc.vRotation.y);
			m_pTransformCom->RotationPlus({ 0.f, 0.f, 1.f }, m_eEffectDesc.vRotation.z);

			m_bFirstRot = false;
		}

		// 2. 회전 속도, 회전 가속도 적용하기
		// 회전 속도
		m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, m_eEffectDesc.vTurnVel.x * m_fSpeedTime);
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, m_eEffectDesc.vTurnVel.y * m_fSpeedTime);
		m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, m_eEffectDesc.vTurnVel.z * m_fSpeedTime);

		// 회전 가속도
		m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, m_eEffectDesc.vTurnAcc.x * m_fSpeedTime * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, m_eEffectDesc.vTurnAcc.y * m_fSpeedTime * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, m_eEffectDesc.vTurnAcc.z * m_fSpeedTime * m_fActiveTime);

		break;
	case 2:
		// Object + Follow
		// 이것도 같이
		// 0. 부모의 라업룩을 적용하기
		if (m_pOwner.lock())
		{
			_quaternion CombinedQuat = m_pOwner.lock()->GetTransform()->GetQuat();

			m_pTransformCom->SetQuaternion(CombinedQuat);
		}
		//m_pTransformCom->SetState(CTransform::STATE_LOOK, m_pRFollowBoneMat->Forward());
		// 2. 독립적인 회전값 적용하기
		m_pTransformCom->RotationPlus({ 1.f, 0.f, 0.f }, m_eEffectDesc.vRotation.x);
		m_pTransformCom->RotationPlus({ 0.f, 1.f, 0.f }, m_eEffectDesc.vRotation.y);
		m_pTransformCom->RotationPlus({ 0.f, 0.f, 1.f }, m_eEffectDesc.vRotation.z);
		// 3. 회전 속도, 회전 가속도 적용하기
		// 회전 속도
		m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, m_eEffectDesc.vTurnVel.x * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, m_eEffectDesc.vTurnVel.y * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, m_eEffectDesc.vTurnVel.z * m_fActiveTime);
		// 회전 가속도
		m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, m_eEffectDesc.vTurnAcc.x * m_fActiveTime * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, m_eEffectDesc.vTurnAcc.y * m_fActiveTime * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, m_eEffectDesc.vTurnAcc.z * m_fActiveTime * m_fActiveTime);
		break;
	case 3:
		// Bone + Follow
		// 0. 부모의 라업룩을 적용하기
		if (m_pRFollowBoneMat)
			m_pTransformCom->SetQuaternion(XMQuaternionRotationMatrix(*m_pRFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()));

		// 1. 독립적인 회전값 적용하기
		m_pTransformCom->RotationPlus({ 1.f, 0.f, 0.f }, m_eEffectDesc.vRotation.x);
		m_pTransformCom->RotationPlus({ 0.f, 1.f, 0.f }, m_eEffectDesc.vRotation.y);
		m_pTransformCom->RotationPlus({ 0.f, 0.f, 1.f }, m_eEffectDesc.vRotation.z);

		// 3. 회전 속도, 회전 가속도 적용하기
		// 회전 속도
		m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, m_eEffectDesc.vTurnVel.x * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, m_eEffectDesc.vTurnVel.y * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, m_eEffectDesc.vTurnVel.z * m_fActiveTime);
		// 회전 가속도
		m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, m_eEffectDesc.vTurnAcc.x * m_fActiveTime * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, m_eEffectDesc.vTurnAcc.y * m_fActiveTime * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, m_eEffectDesc.vTurnAcc.z * m_fActiveTime * m_fActiveTime);
		break;
	case 4:
		// 오너 객체가 사라졌을 때 소멸
		if (!CheckOwnerEnable())
			return;
		// 이거 좀 이상한듯? 다시 생각해보기
		m_pTransformCom->RotationAll(0.f, 0.f, 0.f);

		// 1. 독립적인 회전값 적용하기
		m_pTransformCom->RotationPlus({ 1.f, 0.f, 0.f }, m_eEffectDesc.vRotation.x);
		m_pTransformCom->RotationPlus({ 0.f, 1.f, 0.f }, m_eEffectDesc.vRotation.y);
		m_pTransformCom->RotationPlus({ 0.f, 0.f, 1.f }, m_eEffectDesc.vRotation.z);

		// 2. 부모의 라업룩을 추가로 적용하기
		if (m_pOwner.lock())
		{
			_quaternion CombinedQuat = m_pTransformCom->GetQuat() * m_pOwner.lock()->GetTransform()->GetQuat();

			m_pTransformCom->SetQuaternion(CombinedQuat);
		}
		// 3. 회전 속도, 회전 가속도 적용하기
		// 회전 속도
		m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, m_eEffectDesc.vTurnVel.x * m_fSpeedTime);
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, m_eEffectDesc.vTurnVel.y * m_fSpeedTime);
		m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, m_eEffectDesc.vTurnVel.z * m_fSpeedTime);

		// 회전 가속도
		m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, m_eEffectDesc.vTurnAcc.x * m_fSpeedTime * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, m_eEffectDesc.vTurnAcc.y * m_fSpeedTime * m_fActiveTime);
		m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, m_eEffectDesc.vTurnAcc.z * m_fSpeedTime * m_fActiveTime);
		break;
	default:
		break;
	}

	// Translation
	switch (m_eEffectDesc.iTransFollowType)
	{
	case -1:
		// World
		m_pTransformCom->SetState(CTransform::STATE_POSITION, m_vCurTrans);
		break;
	case 0:
		// Object && No Follow
		if (m_pOwner.lock() && m_bFirstTrans)
		{
			_float3 vTransOnOwner = (SimpleMath::Matrix::CreateTranslation(m_vCurTrans) * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Translation();
			m_pTransformCom->SetState(CTransform::STATE_POSITION, vTransOnOwner);
			m_bFirstTrans = false;
		}
		break;
	case 1:
		// Bone && No Follow
		if (m_pTFollowBoneMat && m_bFirstTrans)
		{
			_float3 vTransOnOwner = (SimpleMath::Matrix::CreateTranslation(m_vCurTrans) * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Translation();
			m_pTransformCom->SetState(CTransform::STATE_POSITION, ((*m_pTFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Translation() - m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_POSITION)) + vTransOnOwner);
			m_bFirstTrans = false;
		}
		break;
	case 2:
		// Object && Follow
		if (m_pOwner.lock())
		{
			_float3 vTransOnOwner = (SimpleMath::Matrix::CreateTranslation(m_vCurTrans) * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Translation();
			m_pTransformCom->SetState(CTransform::STATE_POSITION, vTransOnOwner);

		}
		break;
	case 3:
		// Bone && Follow
		if (m_pTFollowBoneMat)
		{
			_float3 vTransOnOwner = (SimpleMath::Matrix::CreateTranslation(m_vCurTrans) * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Translation();
			m_pTransformCom->SetState(CTransform::STATE_POSITION, ((*m_pTFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Translation() - m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_POSITION)) + vTransOnOwner);
		}
		break;
	default:
		break;
	}

	if (m_eEffectDesc.bUseDslv && !m_bDslv)
	{
		// Dslv In
		if (0 == m_eEffectDesc.iDslvType)
		{
			SetDissolve(true, false, m_eEffectDesc.fDslvDuration);
			SetDissolveInfo(m_eEffectDesc.fDslvThick, m_eEffectDesc.vDslvColor, m_eEffectDesc.strDslvTexKey.c_str());
		}

		// Dslv Out
		else
		{
			if (m_fActiveTime < m_eEffectDesc.fDslvStartTime)
				return;

			SetDissolve(true, true, m_eEffectDesc.fDslvDuration);
			SetDissolveInfo(m_eEffectDesc.fDslvThick, m_eEffectDesc.vDslvColor, m_eEffectDesc.strDslvTexKey.c_str());
		}

		m_bDslv = true;
	}
}

void CEffectMesh::LateTick(_float _fTimeDelta)
{
	if (!m_bStart)
		return;

	if (m_bFinished)
		return;

	if (!m_bPlaying)
		return;

	if (m_bDslv)
		UpdateDslv(_fTimeDelta);

	m_eEffectDesc.iRenderGroup = CRenderer::RENDER_NONLIGHT;

	__super::LateTick(_fTimeDelta);

	if (m_eEffectDesc.bUseDistortion)
	{
		m_fDistortionTimer += _fTimeDelta;
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_DISTORTION, shared_from_this());
	}

	m_eEffectBindDesc.fMaskUVScroll = m_vMaskUVScroll;
	m_eEffectBindDesc.fNoiseUVScroll = m_vNoiseUVScroll;
	m_eEffectBindDesc.fDiffuseUVScroll = m_vDiffuseUVScroll;

	m_eEffectBindDesc.vColor1 = m_vCurColor[0];
	m_eEffectBindDesc.vColor2 = m_vCurColor[1];
	m_eEffectBindDesc.vColor3 = m_vCurColor[2];
	m_eEffectBindDesc.vColor4 = m_vCurColor[3];
}

HRESULT CEffectMesh::Render()
{
	if (m_eEffectDesc.bBillBoard)
	{
		_float4 vCamPosV4 = GAMEINSTANCE->GetCamPosition();
		_float3 vCamPos = _float3(vCamPosV4.x, vCamPosV4.y, vCamPosV4.z);

		_float3 vCurPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);

		_float3 vBillBoardLook = vCamPos - vCurPos;

		// 오브젝트의 크기 저장
		_float3 vOriginalScale = m_pTransformCom->GetScaled();

		// 방향을 카메라와 평행하게 설정
		m_pTransformCom->LookAtDir(vBillBoardLook);

		float angleX = m_eEffectDesc.vRotation.x;
		float angleY = m_eEffectDesc.vRotation.y;
		float angleZ = m_eEffectDesc.vRotation.z;

		// 회전 행렬 생성
		SimpleMath::Matrix rotationMatrix = SimpleMath::Matrix::CreateRotationX(angleX) *
			SimpleMath::Matrix::CreateRotationY(angleY) *
			SimpleMath::Matrix::CreateRotationZ(angleZ);

		// 현재 변환 행렬 얻기
		SimpleMath::Matrix transformMatrix = m_pTransformCom->GetWorldMatrix();

		// 회전 행렬 적용
		transformMatrix *= rotationMatrix;

		// 새로운 변환 행렬 설정
		m_pTransformCom->SetWorldMatrix(transformMatrix);

		// 저장된 크기를 다시 적용
		m_pTransformCom->SetScaling(vOriginalScale);
	}

	//// super에서 Shader 사전준비 작업, 공통 변수들 Bind 해주기
	//if (FAILED(__super::Render()))
	//	return E_FAIL;

	GAMEINSTANCE->ReadyShader(ST_EFFECTMESH, m_iShaderPass);

	if (m_eEffectDesc.bUseDiffuse)
		GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTextureKey[TEX_DIFFUSE]);

	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	if (m_eEffectDesc.bUseMask)
		GAMEINSTANCE->BindSRV("g_MaskTexture", m_strTextureKey[TEX_MASK]);

	if (m_eEffectDesc.bUseNoise)
		GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strTextureKey[TEX_NOISE]);

	if (2 == m_eEffectDesc.iDiffuseUVType)
		GAMEINSTANCE->BindRawValue("g_vDiffuseUVPivot", &m_vDiffuseUVPivot, sizeof(_float2));

	if (2 == m_eEffectDesc.iNoiseUVType)
		GAMEINSTANCE->BindRawValue("g_vNoiseUVPivot", &m_vNoiseUVPivot, sizeof(_float2));

	if (2 == m_eEffectDesc.iMaskUVType)
		GAMEINSTANCE->BindRawValue("g_vMaskUVPivot", &m_vMaskUVPivot, sizeof(_float2));


	GAMEINSTANCE->BindRawValue("g_EffectDesc", &m_eEffectBindDesc, sizeof(EffectMeshBindDesc));

	if (m_bDslv)
	{
		if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &m_bDslv, sizeof(_bool))))
			return E_FAIL;

		_float g_fDslvValue = m_fDslvTime / m_fMaxDslvTime;


		if (FAILED(GAMEINSTANCE->BindRawValue("g_fDslvValue", &g_fDslvValue, sizeof(_float))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_fDslvThick", &m_fDslvThick, sizeof(_float))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_vDslvColor", &m_vDslvColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindSRV("g_DslvTexture", m_strDslvTexKey)))
			return E_FAIL;
	}

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++) {
		GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
	}

	_bool _bDslvFalse = false;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &_bDslvFalse, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectMesh::RenderGlow()
{
	_float4 vBlurColor = { 0.f, 0.f, 0.f, 0.f };
	/*if (bUseBlurColor)
	{*/
	vBlurColor = m_eEffectDesc.KeyFrames.GetCurGlowColor(m_fActiveTime);
	/*vBlurColor = { 1.f, 1.f, 1.f, 1.f };
	vBlurColor.w = m_eEffectBindDesc.vColor1.w * 2.f;*/
	//}

	GAMEINSTANCE->ReadyShader(ST_EFFECTMESH, 1);

	GAMEINSTANCE->BindRawValue("g_bUseBlurColor", &m_eEffectDesc.bUseGlowColor, sizeof(_int));
	GAMEINSTANCE->BindRawValue("g_vBlurColor", &vBlurColor, sizeof(_float4));

	if (m_eEffectDesc.bBillBoard)
	{
		_float4 vCamPosV4 = GAMEINSTANCE->GetCamPosition();
		_float3 vCamPos = _float3(vCamPosV4.x, vCamPosV4.y, vCamPosV4.z);

		_float3 vCurPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);

		_float3 vBillBoardLook = vCamPos - vCurPos;

		// 오브젝트의 크기 저장
		_float3 vOriginalScale = m_pTransformCom->GetScaled();

		// 방향을 카메라와 평행하게 설정
		m_pTransformCom->LookAtDir(vBillBoardLook);

		float angleX = XMConvertToRadians(m_eEffectDesc.vRotation.x);
		float angleY = XMConvertToRadians(m_eEffectDesc.vRotation.y);
		float angleZ = XMConvertToRadians(m_eEffectDesc.vRotation.z);

		// 회전 행렬 생성
		SimpleMath::Matrix rotationMatrix = SimpleMath::Matrix::CreateRotationX(angleX) *
			SimpleMath::Matrix::CreateRotationY(angleY) *
			SimpleMath::Matrix::CreateRotationZ(angleZ);

		// 현재 변환 행렬 얻기
		SimpleMath::Matrix transformMatrix = m_pTransformCom->GetWorldMatrix();

		// 회전 행렬 적용
		transformMatrix *= rotationMatrix;

		// 새로운 변환 행렬 설정
		m_pTransformCom->SetWorldMatrix(transformMatrix);

		// 저장된 크기를 다시 적용
		m_pTransformCom->SetScaling(vOriginalScale);
	}

	if (m_eEffectDesc.bUseDiffuse)
		GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTextureKey[TEX_DIFFUSE]);

	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	if (m_eEffectDesc.bUseMask)
		GAMEINSTANCE->BindSRV("g_MaskTexture", m_strTextureKey[TEX_MASK]);

	if (m_eEffectDesc.bUseNoise)
		GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strTextureKey[TEX_NOISE]);

	if (2 == m_eEffectDesc.iDiffuseUVType)
		GAMEINSTANCE->BindRawValue("g_vDiffuseUVPivot", &m_vDiffuseUVPivot, sizeof(_float2));

	if (2 == m_eEffectDesc.iNoiseUVType)
		GAMEINSTANCE->BindRawValue("g_vNoiseUVPivot", &m_vNoiseUVPivot, sizeof(_float2));

	if (2 == m_eEffectDesc.iMaskUVType)
		GAMEINSTANCE->BindRawValue("g_vMaskUVPivot", &m_vMaskUVPivot, sizeof(_float2));


	GAMEINSTANCE->BindRawValue("g_EffectDesc", &m_eEffectBindDesc, sizeof(EffectMeshBindDesc));

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++) {
		GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
	}

	return S_OK;
}

HRESULT CEffectMesh::RenderDistortion()
{
	if (!m_eEffectDesc.bUseDistortion)
		return E_FAIL;

	GAMEINSTANCE->ReadyShader(ST_EFFECTMESH, 2);

	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	GAMEINSTANCE->BindSRV("g_DistortionTexture", m_eEffectDesc.strDistortionKey);

	GAMEINSTANCE->BindRawValue("g_fDistortionTimer", &m_fDistortionTimer, sizeof(_float));
	GAMEINSTANCE->BindRawValue("g_fDistortionSpeed", &m_eEffectDesc.fDistortionSpeed, sizeof(_float));
	GAMEINSTANCE->BindRawValue("g_fDistortionWeight", &m_eEffectDesc.fDistortionWeight, sizeof(_float));

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++) {
		GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
	}

	return S_OK;
}

HRESULT CEffectMesh::SetOwner(shared_ptr<CGameObject> _pOwner)
{
	if (FAILED(__super::SetOwner(_pOwner)))
		return E_FAIL;

	m_vStartTrans = m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);

	if ("" != m_eEffectDesc.strScaleFollowBoneKey)
	{
		m_pSFollowBoneMat = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr(m_eEffectDesc.strScaleFollowBoneKey.c_str());
	}

	if ("" != m_eEffectDesc.strRotFollowBoneKey)
	{
		m_pRFollowBoneMat = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr(m_eEffectDesc.strRotFollowBoneKey.c_str());
	}

	if ("" != m_eEffectDesc.strTransFollowBoneKey)
	{
		m_pTFollowBoneMat = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr(m_eEffectDesc.strTransFollowBoneKey.c_str());
	}

	return S_OK;
}

void CEffectMesh::Reset()
{
	__super::Reset();

	m_vDiffuseUVScroll = { 0.f, 0.f };
	m_iDiffuseUVIdx = 0;
	m_vDiffuseUVPivot = { 0.f, 0.f };
	m_vMaskUVScroll = { 0.f, 0.f };
	m_iMaskUVIdx = 0;
	m_vMaskUVPivot = { 0.f, 0.f };
	m_vNoiseUVScroll = { 0.f, 0.f };
	m_iNoiseUVIdx = 0;
	m_vNoiseUVPivot = { 0.f, 0.f };
	m_pSFollowBoneMat = nullptr;
	m_pRFollowBoneMat = nullptr;
	m_pTFollowBoneMat = nullptr;
	m_fDistortionTimer = 0.f;
	m_bSetStartTrans = false;
	m_pOwner.lock().reset();
}

HRESULT CEffectMesh::Initialize_Desc(EffectMeshDesc* pDesc)
{
	vector<COLOR_KEYFRAME> colorKeyDescs = {};
	COLOR_KEYFRAME colorKey = {};

	for (auto& iter : pDesc->KeyFrames.ColorKeyFrames) {

		colorKey.eEaseType = iter.eEaseType;
		colorKey.fTime = iter.fTime;

		memcpy(&colorKey.vColor, iter.vColor, sizeof(_float4) * 4);

		colorKeyDescs.emplace_back(colorKey);
	}

	vector<GLOWCOLOR_KEYFRAME> glowColorKeyDescs = {};
	GLOWCOLOR_KEYFRAME glowColorKey = {};

	for (auto& iter : pDesc->KeyFrames.GlowColorKeyFrames) {

		glowColorKey.fTime = iter.fTime;
		glowColorKey.vGlowColor = iter.vGlowColor;

		glowColorKeyDescs.emplace_back(glowColorKey);
	}

	vector<SCALE_KEYFRAME> scaleKeyDescs = {};
	SCALE_KEYFRAME scaleKey = {};

	for (auto& iter : pDesc->KeyFrames.ScaleKeyFrames) {

		scaleKey.eEaseType = iter.eEaseType;
		scaleKey.fTime = iter.fTime;
		scaleKey.vScale = iter.vScale;

		scaleKeyDescs.emplace_back(scaleKey);
	}

	vector<TRANS_KEYFRAME> transKeyDescs = {};
	TRANS_KEYFRAME transKey = {};

	for (auto& iter : pDesc->KeyFrames.TransKeyFrames) {

		transKey.eEaseType = iter.eEaseType;
		transKey.fTime = iter.fTime;
		transKey.vTranslation = iter.vTranslation;

		transKeyDescs.emplace_back(transKey);
	}

	m_eEffectDesc.strModelKey = pDesc->strModelKey;
	m_eEffectDesc.bBillBoard = pDesc->bBillBoard;
	m_eEffectDesc.iRenderGroup = pDesc->iRenderGroup;

	m_eEffectDesc.bUseMask = pDesc->bUseMask;
	m_eEffectDesc.strTexMaskKey = pDesc->strTexMaskKey;
	m_eEffectDesc.bUseNoise = pDesc->bUseNoise;
	m_eEffectDesc.strTexNoiseKey = pDesc->strTexNoiseKey;
	m_eEffectDesc.bUseColor = pDesc->bUseColor;
	m_eEffectDesc.bUseDiffuse = pDesc->bUseDiffuse;

	m_eEffectDesc.KeyFrames.ColorKeyFrames = colorKeyDescs;
	m_eEffectDesc.KeyFrames.ScaleKeyFrames = scaleKeyDescs;
	m_eEffectDesc.KeyFrames.TransKeyFrames = transKeyDescs;
	m_eEffectDesc.KeyFrames.GlowColorKeyFrames = glowColorKeyDescs;

	m_eEffectDesc.iDiffuseSamplerType = pDesc->iDiffuseSamplerType;
	m_eEffectDesc.iDiffuseUVType = pDesc->iDiffuseUVType;

	m_eEffectDesc.fDiffuseDiscardValue = pDesc->fDiffuseDiscardValue;
	m_eEffectDesc.iDiffuseDiscardArea = pDesc->iDiffuseDiscardArea;

	m_eEffectDesc.fDiffuseUVScroll = pDesc->fDiffuseUVScroll;
	m_eEffectDesc.iDiffuseUVAtlasCnt = pDesc->iDiffuseUVAtlasCnt;
	m_eEffectDesc.fDiffuseUVAtlasSpeed = pDesc->fDiffuseUVAtlasSpeed;

	m_eEffectDesc.iMaskSamplerType = pDesc->iMaskSamplerType;
	m_eEffectDesc.iMaskUVType = pDesc->iMaskUVType;

	m_eEffectDesc.fMaskDiscardValue = pDesc->fMaskDiscardValue;
	m_eEffectDesc.iMaskDiscardArea = pDesc->iMaskDiscardArea;

	m_eEffectDesc.fMaskUVScroll = pDesc->fMaskUVScroll;
	m_eEffectDesc.iMaskUVAtlasCnt = pDesc->iMaskUVAtlasCnt;
	m_eEffectDesc.fMaskUVAtlasSpeed = pDesc->fMaskUVAtlasSpeed;

	m_eEffectDesc.iNoiseSamplerType = pDesc->iNoiseSamplerType;
	m_eEffectDesc.iNoiseUVType = pDesc->iNoiseUVType;

	m_eEffectDesc.fNoiseDiscardValue = pDesc->fNoiseDiscardValue;
	m_eEffectDesc.iNoiseDiscardArea = pDesc->iNoiseDiscardArea;

	m_eEffectDesc.fNoiseUVScroll = pDesc->fNoiseUVScroll;
	m_eEffectDesc.iNoiseUVAtlasCnt = pDesc->iNoiseUVAtlasCnt;
	m_eEffectDesc.fNoiseUVAtlasSpeed = pDesc->fNoiseUVAtlasSpeed;

	m_eEffectDesc.iDiffuseSamplerType = pDesc->iDiffuseSamplerType;
	m_eEffectDesc.iDiffuseUVType = pDesc->iDiffuseUVType;

	m_eEffectDesc.fDiffuseDiscardValue = pDesc->fDiffuseDiscardValue;
	m_eEffectDesc.iDiffuseDiscardArea = pDesc->iDiffuseDiscardArea;

	m_eEffectDesc.fDiffuseUVScroll = pDesc->fDiffuseUVScroll;
	m_eEffectDesc.iDiffuseUVAtlasCnt = pDesc->iDiffuseUVAtlasCnt;
	m_eEffectDesc.fDiffuseUVAtlasSpeed = pDesc->fDiffuseUVAtlasSpeed;

	m_eEffectDesc.iColorSetCnt = pDesc->iColorSetCnt;
	m_eEffectDesc.iColorSplitArea = pDesc->iColorSplitArea;
	m_eEffectDesc.vColorSplitter = pDesc->vColorSplitter;

	m_eEffectDesc.iScaleFollowType = pDesc->iScaleFollowType;
	m_eEffectDesc.strScaleFollowBoneKey = pDesc->strScaleFollowBoneKey;
	m_eEffectDesc.iScaleSetCnt = pDesc->iScaleSetCnt;

	m_eEffectDesc.iRotFollowType = pDesc->iRotFollowType;
	m_eEffectDesc.strRotFollowBoneKey = pDesc->strRotFollowBoneKey;

	m_eEffectDesc.bUseGlowColor = pDesc->bUseGlowColor;
	m_eEffectDesc.iGlowColorSetCnt = pDesc->iGlowColorSetCnt;

	m_eEffectDesc.vRotation = pDesc->vRotation;
	m_eEffectDesc.vTurnVel = pDesc->vTurnVel;
	m_eEffectDesc.vTurnAcc = pDesc->vTurnAcc;

	m_eEffectDesc.iTransFollowType = pDesc->iTransFollowType;
	m_eEffectDesc.strTransFollowBoneKey = pDesc->strTransFollowBoneKey;
	m_eEffectDesc.iTransSetCnt = pDesc->iTransSetCnt;

	m_eEffectDesc.bUseDistortion = pDesc->bUseDistortion;
	m_eEffectDesc.strDistortionKey = pDesc->strDistortionKey;
	m_eEffectDesc.fDistortionSpeed = pDesc->fDistortionSpeed;
	m_eEffectDesc.fDistortionWeight = pDesc->fDistortionWeight;

	m_eEffectDesc.bUseDslv = pDesc->bUseDslv;
	m_eEffectDesc.bUseDslv = pDesc->bUseDslv;
	m_eEffectDesc.iDslvType = pDesc->iDslvType;
	m_eEffectDesc.bDslvGlow = pDesc->bDslvGlow;
	m_eEffectDesc.fDslvStartTime = pDesc->fDslvStartTime;
	m_eEffectDesc.fDslvDuration = pDesc->fDslvDuration;
	m_eEffectDesc.fDslvThick = pDesc->fDslvThick;
	m_eEffectDesc.vDslvColor = pDesc->vDslvColor;
	m_eEffectDesc.vDslvGlowColor = pDesc->vDslvGlowColor;
	m_eEffectDesc.strDslvTexKey = pDesc->strDslvTexKey;

	if (m_eEffectDesc.bUseMask)
		m_strTextureKey[TEX_MASK] = m_eEffectDesc.strTexMaskKey;

	if (m_eEffectDesc.bUseNoise)
		m_strTextureKey[TEX_NOISE] = m_eEffectDesc.strTexNoiseKey;

	//============================================

	m_eEffectBindDesc.fDuration = m_eEffectDesc.fDuration;
	m_eEffectBindDesc.bBillBoard = m_eEffectDesc.bBillBoard;
	m_eEffectBindDesc.bUseDiffuse = m_eEffectDesc.bUseDiffuse;
	m_eEffectBindDesc.bUseMask = m_eEffectDesc.bUseMask;
	m_eEffectBindDesc.bUseNoise = m_eEffectDesc.bUseNoise;
	m_eEffectBindDesc.bUseColor = m_eEffectDesc.bUseColor;

	m_eEffectBindDesc.iDiffuseSamplerType = m_eEffectDesc.iDiffuseSamplerType;
	m_eEffectBindDesc.iDiffuseUVType = m_eEffectDesc.iDiffuseUVType;
	m_eEffectBindDesc.fDiffuseDiscardValue = m_eEffectDesc.fDiffuseDiscardValue;
	m_eEffectBindDesc.iDiffuseDiscardArea = m_eEffectDesc.iDiffuseDiscardArea;
	m_eEffectBindDesc.iDiffuseUVAtlasCnt = m_eEffectDesc.iDiffuseUVAtlasCnt;
	m_eEffectBindDesc.fDiffuseUVAtlasSpeed = m_eEffectDesc.fDiffuseUVAtlasSpeed;

	m_eEffectBindDesc.iMaskSamplerType = m_eEffectDesc.iMaskSamplerType;
	m_eEffectBindDesc.iMaskUVType = m_eEffectDesc.iMaskUVType;
	m_eEffectBindDesc.fMaskDiscardValue = m_eEffectDesc.fMaskDiscardValue;
	m_eEffectBindDesc.iMaskDiscardArea = m_eEffectDesc.iMaskDiscardArea;
	m_eEffectBindDesc.iMaskUVAtlasCnt = m_eEffectDesc.iMaskUVAtlasCnt;
	m_eEffectBindDesc.fMaskUVAtlasSpeed = m_eEffectDesc.fMaskUVAtlasSpeed;

	m_eEffectBindDesc.iNoiseSamplerType = m_eEffectDesc.iNoiseSamplerType;
	m_eEffectBindDesc.iNoiseUVType = m_eEffectDesc.iNoiseUVType;
	m_eEffectBindDesc.fNoiseDiscardValue = m_eEffectDesc.fNoiseDiscardValue;
	m_eEffectBindDesc.iNoiseDiscardArea = m_eEffectDesc.iNoiseDiscardArea;
	m_eEffectBindDesc.iNoiseUVAtlasCnt = m_eEffectDesc.iNoiseUVAtlasCnt;
	m_eEffectBindDesc.fNoiseUVAtlasSpeed = m_eEffectDesc.fNoiseUVAtlasSpeed;

	m_eEffectBindDesc.iColorSplitArea = m_eEffectDesc.iColorSplitArea;
	m_eEffectBindDesc.vColorSplitter = m_eEffectDesc.vColorSplitter;

	return S_OK;
}

shared_ptr<CEffectMesh> CEffectMesh::Create(EffectMeshDesc* pDesc)
{
	shared_ptr<CEffectMesh> pInstance = make_shared<CEffectMesh>();

	if (FAILED(pInstance->Initialize(pDesc)))
		MSG_BOX("Failed to Create : CEffectMesh");

	return pInstance;
}