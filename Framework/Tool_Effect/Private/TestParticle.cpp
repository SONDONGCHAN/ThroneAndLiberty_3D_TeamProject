#include "ParticleSystem.h"
#include "VIInstanceParticle.h"
#include "TestParticle.h"
#include "Model.h"

CTestParticle::CTestParticle()
{
}

CTestParticle::CTestParticle(const CTestParticle& rhs)
{
}

CTestParticle::~CTestParticle()
{
}

HRESULT CTestParticle::Initialize(_int _iNumInstance, string _strTexKey)
{
	CGameObject::Initialize(nullptr);

	m_eParticleSystem.strTexDiffuseKey = _strTexKey;
	m_eParticleSystem.iMaxParticles = _iNumInstance;

	m_eParticleSystem.iEffectType = EFFECT_PARTICLE;

	m_eParticleSystem.iShaderType = ESHADER_TYPE::ST_PARTICLE;
	m_eParticleSystem.iShaderPass = 0;

	m_eParticleSystem.iRenderGroup = CRenderer::RENDER_BLEND;

	m_eParticleSystem.fDuration = 10.f;
	m_eParticleSystem.StartTrackPos = 0.f;

	m_eParticleSystem.bLoop = true;
	m_eParticleSystem.bUseDiffuse = true;

	m_pDevice = GAMEINSTANCE->GetDeviceInfo();
	m_pContext = GAMEINSTANCE->GetDeviceContextInfo();

	m_eShaderType = (ESHADER_TYPE)m_eEffectDesc.iShaderType;
	m_iShaderPass = m_eEffectDesc.iShaderPass;
	m_iRenderGroup = (CRenderer::ERENDER_GROUP)m_eEffectDesc.iRenderGroup;

	m_pTransformCom->SetTurnSpeed(1.f);
	m_iCurKeyFrameIdx = 0;

	if (m_eParticleSystem.bUsePosKeyFrame)
	{
		m_vStartPosMin = m_eParticleSystem.KeyFrames.PosKeyFrames.begin()->vPosMin;
		m_vStartPosMax = m_eParticleSystem.KeyFrames.PosKeyFrames.begin()->vPosMax;
	}

	m_vStartSpeedMin = { -1.f, 1.f, -1.f };
	m_vStartSpeedMax = { 1.f, 4.f, 1.f };

	m_vStartScaleMin = { 0.1f, 0.1f, 0.1f };
	m_vStartScaleMax = { 1.f, 1.f, 1.f };

	m_vStartColorMin = { 0.8f, 0.2f, 0.1f, 1.f };
	m_vStartColorMax = { 0.9f, 0.4f, 0.3f, 1.f };

	// 파티클 입자 생성
	m_pParticles = new PARTICLE[m_eParticleSystem.iMaxParticles];

	for (_int i = 0; i < m_eParticleSystem.iMaxParticles; i++)
	{
		m_pParticles[i].bActive = false;
	}

	m_iCurrentParticleCnt = 0;

	// 테스트용 코드
	SetOwner(GAMEINSTANCE->GetGameObject(LEVEL_LOGO, TEXT("Layer_AnimObject")));

	m_pVIBufferCom = dynamic_pointer_cast<CVIInstanceParticle>(GAMEINSTANCE->GetBuffer("Buffer_InstanceParticle")->Clone(&m_eParticleSystem.iMaxParticles));
	if (m_pVIBufferCom == nullptr)
		return E_FAIL;

	m_bEmitted = false;
	m_bFinished = false;
	m_bCanEmit = true;

	m_bEntered = true;

	return S_OK;
}

void CTestParticle::PriorityTick(_float _fTimeDelta)
{
}

void CTestParticle::Tick(_float _fTimeDelta)
{

	if (!m_bPlaying)
		return;

	m_fSpeedTime = _fTimeDelta * m_fSpeed;
	m_fActiveTime += m_fSpeedTime;

	// 전체 재생 끝났을 때
	if (m_eParticleSystem.fDuration <= m_fActiveTime)
	{
		m_bCanEmit = false;
	}

	if (m_bEntered)
	{
		if (m_eParticleSystem.iPosType == 1)
		{
			m_OwnerWorld = m_pOwner.lock()->GetTransform()->GetWorldMatrix();
		}

		else if (m_eParticleSystem.iPosType == 2)
		{
			m_OwnerWorld = (*m_pFollowBoneMat) * m_pOwner.lock()->GetTransform()->GetWorldMatrix();
		}
		m_bEntered = false;
	}

	// 파티클 시스템 정보 결정

	// 수명 다 한 애들은 bActive = false로 바꿔주기
	CheckDeadParticles(m_fSpeedTime);

	// 초 당 발생 파티클 계산해서 방출하기
	if (m_bCanEmit)
		SpawnParticles(m_fSpeedTime);

	// 파티클 업데이트
	UpdateParticles(m_fSpeedTime);

	m_pVIBufferCom->Update(m_pParticles, m_eParticleSystem.iMaxParticles);
}

void CTestParticle::LateTick(_float _fTimeDelta)
{
	m_fSpeedTime = _fTimeDelta * m_fSpeed;

	if (CRenderer::RENDER_GLOW == m_eParticleSystem.iRenderGroup)
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_BLEND, shared_from_this());
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_GLOW, shared_from_this());
	}
	else if (CRenderer::RENDER_BLUR == m_eParticleSystem.iRenderGroup)
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_GLOW, shared_from_this());
	}
	else
	{
		GAMEINSTANCE->AddRenderGroup((CRenderer::ERENDER_GROUP)m_eParticleSystem.iRenderGroup, shared_from_this());
	}
}

HRESULT CTestParticle::Render()
{
	if (m_eParticleSystem.bUsePivot)
		GAMEINSTANCE->ReadyShader((ESHADER_TYPE)m_eParticleSystem.iShaderType, 2);

	else
		GAMEINSTANCE->ReadyShader((ESHADER_TYPE)m_eParticleSystem.iShaderType, m_eParticleSystem.iShaderPass);

	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	_float4 vCamPos = GAMEINSTANCE->GetCamPosition();

	if (GAMEINSTANCE->BindRawValue("g_vCamPosition", &vCamPos, sizeof(_float4)))
		return E_FAIL;

	if (GAMEINSTANCE->BindRawValue("g_bUseColor", &m_eParticleSystem.bUseColor, sizeof(_bool)))
		return E_FAIL;

	GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_eParticleSystem.strTexDiffuseKey);

	if (FAILED(GAMEINSTANCE->BeginShader()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->BindBuffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestParticle::RenderGlow()
{
	// 원본이랑 합쳐질게 아니라, 그냥 블러만 먹일거면 기존 원본을 갖고 해야됨.
	if (CRenderer::RENDER_BLUR == m_eEffectDesc.iRenderGroup)
	{
		if (m_eParticleSystem.bUsePivot)
			GAMEINSTANCE->ReadyShader((ESHADER_TYPE)m_eParticleSystem.iShaderType, 2);

		else
			GAMEINSTANCE->ReadyShader((ESHADER_TYPE)m_eParticleSystem.iShaderType, m_eParticleSystem.iShaderPass);
	}

	// 그게 아니면 색상값만 가지고 그려도 된다. (마스크, 노이즈? 일단 그려보고 생각하자)
	else
	{
		_bool bUseBlurColor = m_eParticleSystem.bUseGlowColor;
		_float4 vBlurColor = m_eParticleSystem.vGlowColor;

		GAMEINSTANCE->BindRawValue("g_bUseBlurColor", &bUseBlurColor, sizeof(_int));
		GAMEINSTANCE->BindRawValue("g_vBlurColor", &vBlurColor, sizeof(_float4));

		if (m_eParticleSystem.bUsePivot)
			GAMEINSTANCE->ReadyShader((ESHADER_TYPE)m_eParticleSystem.iShaderType, 3);

		else
			GAMEINSTANCE->ReadyShader(ST_PARTICLE, 1);
	}

	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	_float4 vCamPos = GAMEINSTANCE->GetCamPosition();

	if (GAMEINSTANCE->BindRawValue("g_vCamPosition", &vCamPos, sizeof(_float4)))
		return E_FAIL;

	GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_eParticleSystem.strTexDiffuseKey);

	if (FAILED(GAMEINSTANCE->BeginShader()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->BindBuffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CTestParticle::SpawnParticles(_float _fTimeDelta)
{
	switch (m_eParticleSystem.iSpawnType)
	{
	case 0:
		m_fSpawnTime += _fTimeDelta;

		while (true)
		{
			// 기준 초 마다 생성
			// 맥시멈 개수보다 현재 개수가 작을 때 생성
			if (m_fSpawnTime >= (1.f / m_eParticleSystem.fSpawnNumPerSec)
				&& m_iCurrentParticleCnt < m_eParticleSystem.iMaxParticles)
			{
				m_fSpawnTime -= (1.f / m_eParticleSystem.fSpawnNumPerSec);

				for (_int i = 0; i < m_eParticleSystem.iMaxParticles; i++)
				{
					// 파티클 생성 코드
					if (!m_pParticles[i].bActive)
					{
						CreateParticle(i);
						break;
					}
				}
			}
			else if (m_iCurrentParticleCnt >= m_eParticleSystem.iMaxParticles)
			{
				m_fSpawnTime = 0.f;
				break;
			}
			else
			{
				break;
			}
		}

		break;
	case 1:
		// 현재 개수가 0일 때 생성

		if (0 == m_iCurrentParticleCnt)
		{
			for (_int i = 0; i < m_eParticleSystem.iMaxParticles; i++)
			{
				// 파티클 생성 코드
				CreateParticle(i);
			}
		}
		break;
	case 2:

		break;
	default:
		break;
	}
}

void CTestParticle::UpdateParticles(_float _fTimeDelta)
{
	_float3 vNextSpeed;
	_float3 vNextScale;
	_float4 vNextColor;

	for (_int i = 0; i < m_eParticleSystem.iMaxParticles; i++)
	{
		if (!m_pParticles[i].bActive)
			continue;

		m_pParticles[i].fLifeTime -= _fTimeDelta;
		
		// 0. Pivot
		if (m_eParticleSystem.bUsePivot && !m_eParticleSystem.iBillBoardType)
		{
			m_pParticles[i].vPivot = _float4(m_pParticles[i].vPosition.x, m_pParticles[i].vPosition.y, m_pParticles[i].vPosition.z, 1.f);
		}

		// 1. Speed
		// 키프레임에서 그 때 그 때 스피드 받아오는 경우
		if (!m_eParticleSystem.bSpeedMode)
		{
			vNextSpeed = m_eParticleSystem.KeyFrames.GetNextSpeed(m_pParticles[i].iSpeedIdx, m_pParticles[i].fStartLifeTime - m_pParticles[i].fLifeTime);

			_int iKeyFramesCnt = m_eParticleSystem.KeyFrames.SpeedKeyFrames.size();

			if (0 == iKeyFramesCnt || 1 == iKeyFramesCnt)
			{
			}

			// 마지막 키프레임일 때 (키프레임이 하나일 때는 항상 여기로)
			else if (vNextSpeed.x == -101.f)
			{
				m_pParticles[i].vCurSpeed = m_eParticleSystem.KeyFrames.GetCurSpeed(m_pParticles[i].iSpeedIdx, m_pParticles[i].fStartLifeTime - m_pParticles[i].fLifeTime, m_pParticles[i].vSpeed, m_pParticles[i].vNextSpeed);
			}

			// 다음 키프레임으로 아직 안넘어갔을 때
			else if (vNextSpeed.y == -101.f)
			{
				m_pParticles[i].vCurSpeed = m_eParticleSystem.KeyFrames.GetCurSpeed(m_pParticles[i].iSpeedIdx, m_pParticles[i].fStartLifeTime - m_pParticles[i].fLifeTime, m_pParticles[i].vSpeed, m_pParticles[i].vNextSpeed);
			}

			// 한 키프레임을 처음 넘어간 순간
			else
			{
				m_pParticles[i].iSpeedIdx++;
				m_pParticles[i].vSpeed = m_pParticles[i].vNextSpeed;
				m_pParticles[i].vCurSpeed = m_pParticles[i].vSpeed;
				m_pParticles[i].vNextSpeed = vNextSpeed;
			}
		}

		else
		{
			m_pParticles[i].vCurSpeed += m_pParticles[i].vSpeedAcc * _fTimeDelta;
		}

		// Haze Effect
		if (m_eParticleSystem.bUseHazeEffect)
		{
			m_pParticles[i].fHazeTime += m_fSpeedTime;

			if (m_pParticles[i].fHazeTime >= m_pParticles[i].fLifeTime)
			{
				m_pParticles[i].fHazeTime = 0.f;
				m_pParticles[i].vHazeSpeed.x = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vHazeSpeedMin.x, m_eParticleSystem.vHazeSpeedMax.x, true);
				m_pParticles[i].vHazeSpeed.y = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vHazeSpeedMin.y, m_eParticleSystem.vHazeSpeedMax.y, true);
				m_pParticles[i].vHazeSpeed.z = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vHazeSpeedMin.z, m_eParticleSystem.vHazeSpeedMax.z, true);
			}
		}

		m_vCurSpeed = (m_pParticles[i].vCurSpeed + m_pParticles[i].vHazeSpeed) * _fTimeDelta;

		if (m_eParticleSystem.bRelativeOwner)
		{
			m_vCurSpeedMatrix = SimpleMath::Matrix::CreateTranslation(m_vCurSpeed);
			m_vCurSpeed = (m_vCurSpeedMatrix * m_pParticles[i].OwnerMat).Translation();
		}

		// Last. Implosion일 경우 Dir을 재설정해서 만들어놨던 m_vCurSpeed를 값만 추출하여 Dir에 적용한다.
		if (m_eParticleSystem.iBillBoardType == 1)
		{
			m_vCurDir = m_pParticles[i].vPivot - m_pParticles[i].vPosition;
			m_vCurDir.Normalize();

			m_vCurSpeed = _float3(m_vCurDir.x * fabs(m_vCurSpeed.x), m_vCurDir.y * fabs(m_vCurSpeed.x), m_vCurDir.z * fabs(m_vCurSpeed.x));

			m_pParticles[i].vPosition += m_vCurSpeed;

			_float fDist = 0.f;
			_float3 vDist = m_pParticles[i].vPosition - _float3(m_pParticles[i].vPivot.x, m_pParticles[i].vPivot.y, m_pParticles[i].vPivot.z);

			fDist = vDist.Length();

			if (fDist < m_vCurSpeed.Length() || m_vCurSpeed.Length() <= 0.0f || fDist <= (m_pParticles[i].vCurScale.y) / 8.f)
			{
				m_pParticles[i].fLifeTime = 0.f;
			}
		}

		else
			m_pParticles[i].vPosition += m_vCurSpeed;
		
		// 2. Scale
		vNextScale = m_eParticleSystem.KeyFrames.GetNextScale(m_pParticles[i].iScaleIdx, m_pParticles[i].fStartLifeTime - m_pParticles[i].fLifeTime);

		_int iKeyFramesCnt = m_eParticleSystem.KeyFrames.ScaleKeyFrames.size();

		if (0 == iKeyFramesCnt || 1 == iKeyFramesCnt)
		{
		}

		// 마지막 키프레임일 때 (키프레임이 하나일 때는 항상 여기로)
		else if (vNextScale.x == -101.f)
		{
			m_pParticles[i].vCurScale = m_eParticleSystem.KeyFrames.GetCurScale(m_pParticles[i].iScaleIdx, m_pParticles[i].fStartLifeTime - m_pParticles[i].fLifeTime, m_pParticles[i].vScale, m_pParticles[i].vNextScale);
		}

		// 다음 키프레임으로 아직 안넘어갔을 때
		else if (vNextScale.y == -101.f)
		{
			m_pParticles[i].vCurScale = m_eParticleSystem.KeyFrames.GetCurScale(m_pParticles[i].iScaleIdx, m_pParticles[i].fStartLifeTime - m_pParticles[i].fLifeTime, m_pParticles[i].vScale, m_pParticles[i].vNextScale);
		}

		// 한 키프레임을 처음 넘어간 순간
		else
		{
			m_pParticles[i].iScaleIdx++;
			m_pParticles[i].vScale = m_pParticles[i].vNextScale;
			m_pParticles[i].vCurScale = m_pParticles[i].vScale;
			m_pParticles[i].vNextScale = vNextScale;
		}

		// 3. Color
		if (m_eParticleSystem.bUseColor)
		{
			vNextColor = m_eParticleSystem.KeyFrames.GetNextColor(m_pParticles[i].iColorIdx, m_pParticles[i].fStartLifeTime - m_pParticles[i].fLifeTime);

			_int iKeyFramesCnt = m_eParticleSystem.KeyFrames.ColorKeyFrames.size();

			if (0 == iKeyFramesCnt || 1 == iKeyFramesCnt)
			{
			}

			// 마지막 키프레임일 때 (키프레임이 하나일 때는 항상 여기로)
			else if (vNextColor.x == -101.f)
			{
				m_pParticles[i].vCurColor = m_eParticleSystem.KeyFrames.GetCurColor(m_pParticles[i].iColorIdx, m_pParticles[i].fStartLifeTime - m_pParticles[i].fLifeTime, m_pParticles[i].vColor, m_pParticles[i].vNextColor);
			}

			// 다음 키프레임으로 아직 안넘어갔을 때
			else if (vNextColor.y == -101.f)
			{
				m_pParticles[i].vCurColor = m_eParticleSystem.KeyFrames.GetCurColor(m_pParticles[i].iColorIdx, m_pParticles[i].fStartLifeTime - m_pParticles[i].fLifeTime, m_pParticles[i].vColor, m_pParticles[i].vNextColor);
			}

			// 한 키프레임을 처음 넘어간 순간
			else
			{
				m_pParticles[i].iColorIdx++;
				m_pParticles[i].vColor = m_pParticles[i].vNextColor;
				m_pParticles[i].vCurColor = m_pParticles[i].vColor;
				m_pParticles[i].vNextColor = vNextColor;
			}
		}
	}
}

void CTestParticle::CheckDeadParticles(_float _fTimeDelta)
{
	for (_int i = 0; i < m_eParticleSystem.iMaxParticles; i++)
	{
		if (m_pParticles[i].bActive && 0.f >= m_pParticles[i].fLifeTime)
		{
			m_pParticles[i].bActive = false;
			m_iCurrentParticleCnt -= 1;
		}
	}

	// 모든 파티클이 다 소멸되었을 때 종료
	if (!m_bCanEmit)
	{
		if (m_iCurrentParticleCnt == 0)
		{
			m_bFinished = true;
			m_bEntered = false;
		}
	}

	// 시간도 다 지나고 모든 파티클이 다 소멸되었을 때
	if (m_bFinished)
	{
		if (m_eParticleSystem.bLoop)
		{
			// ResetParticles
			m_fActiveTime = 0.f;
			m_bEmitted = false;
			m_bFinished = false;
			m_bCanEmit = true;
		}

		else
		{
			// ResetParticles
			m_fActiveTime = 0.f;
			m_bEmitted = false;
			m_bFinished = true;
			m_bCanEmit = false;

			// 멈춤
		}
	}
}

void CTestParticle::CreateParticle(_int _iIndex)
{
	// 파티클 생성 시 해줘야 할 것

	// 0. 파티클 위치 설정
	if (0 == m_eParticleSystem.iPosType)
	{
		if (m_eParticleSystem.bUsePosKeyFrame)
		{
			_float3 vCurMinPos = m_eParticleSystem.KeyFrames.GetCurMinPos(m_fActiveTime);
			_float3 vCurMaxPos = m_eParticleSystem.KeyFrames.GetCurMaxPos(m_fActiveTime);

			m_pParticles[_iIndex].vPosition.x = GAMEINSTANCE->PickRandomFloat(vCurMinPos.x, vCurMaxPos.x, true);
			m_pParticles[_iIndex].vPosition.y = GAMEINSTANCE->PickRandomFloat(vCurMinPos.y, vCurMaxPos.y, true);
			m_pParticles[_iIndex].vPosition.z = GAMEINSTANCE->PickRandomFloat(vCurMinPos.z, vCurMaxPos.z, true);
		}

		else
		{
			m_pParticles[_iIndex].vPosition.x = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vPosMin.x, m_eParticleSystem.vPosMax.x, true);
			m_pParticles[_iIndex].vPosition.y = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vPosMin.y, m_eParticleSystem.vPosMax.y, true);
			m_pParticles[_iIndex].vPosition.z = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vPosMin.z, m_eParticleSystem.vPosMax.z, true);
		}
	}

	else
	{
		if (m_eParticleSystem.bPosFollow)
		{
			// 오너 객체가 사라졌을 때 생성 종료
			if (!m_pOwner.lock() || !m_pOwner.lock()->IsEnabled())
				return;

			if (1 == m_eParticleSystem.iPosType)
				m_OwnerWorld = m_pOwner.lock()->GetTransform()->GetWorldMatrix();

			else if (2 == m_eParticleSystem.iPosType)
				m_OwnerWorld = *m_pFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix();
		}

		if (m_eParticleSystem.bUsePosKeyFrame)
		{
			_float3 vCurMinPos = m_eParticleSystem.KeyFrames.GetCurMinPos(m_fActiveTime);
			_float3 vCurMaxPos = m_eParticleSystem.KeyFrames.GetCurMaxPos(m_fActiveTime);

			m_vCurPos.x = GAMEINSTANCE->PickRandomFloat(vCurMinPos.x, vCurMaxPos.x, true);
			m_vCurPos.y = GAMEINSTANCE->PickRandomFloat(vCurMinPos.y, vCurMaxPos.y, true);
			m_vCurPos.z = GAMEINSTANCE->PickRandomFloat(vCurMinPos.z, vCurMaxPos.z, true);
		}

		else
		{
			m_vCurPos.x = (GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vPosMin.x, m_eParticleSystem.vPosMax.x, true));
			m_vCurPos.y = (GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vPosMin.y, m_eParticleSystem.vPosMax.y, true));
			m_vCurPos.z = (GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vPosMin.z, m_eParticleSystem.vPosMax.z, true));
		}

		m_vCurPosMatrix = SimpleMath::Matrix::CreateTranslation(m_vCurPos);
		m_vCurPos = (m_vCurPosMatrix * m_OwnerWorld).Translation();

		m_pParticles[_iIndex].vPosition = m_vCurPos;

		if (m_eParticleSystem.bUsePivot)
		{
			m_vCurPivot.x = (GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vPivotMin.x, m_eParticleSystem.vPivotMax.x, true));
			m_vCurPivot.y = (GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vPivotMin.y, m_eParticleSystem.vPivotMax.y, true));
			m_vCurPivot.z = (GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vPivotMin.z, m_eParticleSystem.vPivotMax.z, true));

			m_vCurPivotMatrix = SimpleMath::Matrix::CreateTranslation(m_vCurPivot);
			m_vCurPivot = (m_vCurPivotMatrix * m_OwnerWorld).Translation();

			m_pParticles[_iIndex].vPivot = _float4(m_vCurPivot.x, m_vCurPivot.y, m_vCurPivot.z, 1.f);
		}
	}

	m_pParticles[_iIndex].OwnerMat = m_OwnerWorld;
	m_pParticles[_iIndex].OwnerMat.Translation(_float3::Zero);

	// 1. 수명 범위 내 결정
	m_pParticles[_iIndex].fLifeTime = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vLifeTime.x, m_eParticleSystem.vLifeTime.y, true);
	m_pParticles[_iIndex].fStartLifeTime = m_pParticles[_iIndex].fLifeTime;

	m_pParticles[_iIndex].iColorIdx = 0;
	m_pParticles[_iIndex].iSpeedIdx = 0;
	m_pParticles[_iIndex].iScaleIdx = 0;

	// 2. 속도 모드에 따라서
	if (!m_eParticleSystem.bSpeedMode)
	{
		// 2-0-0. 속도 첫번째 키프레임 읽어서 속도 결정
		// 2-0-1. 속도 범위 내 결정
		m_pParticles[_iIndex].vSpeed.x = GAMEINSTANCE->PickRandomFloat(m_vStartSpeedMin.x, m_vStartSpeedMax.x, true);
		m_pParticles[_iIndex].vSpeed.y = GAMEINSTANCE->PickRandomFloat(m_vStartSpeedMin.y, m_vStartSpeedMax.y, true);
		m_pParticles[_iIndex].vSpeed.z = GAMEINSTANCE->PickRandomFloat(m_vStartSpeedMin.x, m_vStartSpeedMax.z, true);
	}
	else
	{
		// 2-1-0. 파티클 속도 범위 내 결정
		m_pParticles[_iIndex].vSpeed.x = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vStartSpeedMin.x, m_eParticleSystem.vStartSpeedMax.x, true);
		m_pParticles[_iIndex].vSpeed.y = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vStartSpeedMin.y, m_eParticleSystem.vStartSpeedMax.y, true);
		m_pParticles[_iIndex].vSpeed.z = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vStartSpeedMin.z, m_eParticleSystem.vStartSpeedMax.z, true);

		// 2-1-1. 파티클 가속도 범위 내 결정
		m_pParticles[_iIndex].vSpeedAcc.x = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vSpeedAccMin.x, m_eParticleSystem.vSpeedAccMax.x, true);
		m_pParticles[_iIndex].vSpeedAcc.y = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vSpeedAccMin.y, m_eParticleSystem.vSpeedAccMax.y, true);
		m_pParticles[_iIndex].vSpeedAcc.z = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vSpeedAccMin.z, m_eParticleSystem.vSpeedAccMax.z, true);
	}

	m_pParticles[_iIndex].vCurSpeed.x = m_pParticles[_iIndex].vSpeed.x;
	m_pParticles[_iIndex].vCurSpeed.y = m_pParticles[_iIndex].vSpeed.y;
	m_pParticles[_iIndex].vCurSpeed.z = m_pParticles[_iIndex].vSpeed.z;

	m_pParticles[_iIndex].vNextSpeed.x = m_pParticles[_iIndex].vSpeed.x;
	m_pParticles[_iIndex].vNextSpeed.y = m_pParticles[_iIndex].vSpeed.y;
	m_pParticles[_iIndex].vNextSpeed.z = m_pParticles[_iIndex].vSpeed.z;

	// 3. 크기 첫번째 키프레임 읽어서 크기 결정
	m_pParticles[_iIndex].vScale.x = GAMEINSTANCE->PickRandomFloat(m_vStartScaleMin.x, m_vStartScaleMax.x, true);
	m_pParticles[_iIndex].vScale.y = GAMEINSTANCE->PickRandomFloat(m_vStartScaleMin.y, m_vStartScaleMax.y, true);
	m_pParticles[_iIndex].vScale.z = GAMEINSTANCE->PickRandomFloat(m_vStartScaleMin.z, m_vStartScaleMax.z, true);

	m_pParticles[_iIndex].vCurScale.x = m_pParticles[_iIndex].vScale.x;
	m_pParticles[_iIndex].vCurScale.y = m_pParticles[_iIndex].vScale.y;
	m_pParticles[_iIndex].vCurScale.z = m_pParticles[_iIndex].vScale.z;

	m_pParticles[_iIndex].vNextScale.x = m_pParticles[_iIndex].vScale.x;
	m_pParticles[_iIndex].vNextScale.y = m_pParticles[_iIndex].vScale.y;
	m_pParticles[_iIndex].vNextScale.z = m_pParticles[_iIndex].vScale.z;

	// 4. 회전 범위 내 결정
	m_pParticles[_iIndex].vRotation.x = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vRotationMin.x, m_eParticleSystem.vRotationMax.x);
	m_pParticles[_iIndex].vRotation.y = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vRotationMin.y, m_eParticleSystem.vRotationMax.y);
	m_pParticles[_iIndex].vRotation.z = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vRotationMin.z, m_eParticleSystem.vRotationMax.z);

	// 5. 회전 속도 범위 내 결정
	m_pParticles[_iIndex].vTurnVel.x = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vTurnVelMin.x, m_eParticleSystem.vTurnVelMax.x, true);
	m_pParticles[_iIndex].vTurnVel.y = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vTurnVelMin.y, m_eParticleSystem.vTurnVelMax.y, true);
	m_pParticles[_iIndex].vTurnVel.z = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vTurnVelMin.z, m_eParticleSystem.vTurnVelMax.z, true);

	// 5. 회전 가속도 범위 내 결정
	m_pParticles[_iIndex].vTurnAcc.x = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vTurnAccMin.x, m_eParticleSystem.vTurnAccMax.x, true);
	m_pParticles[_iIndex].vTurnAcc.y = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vTurnAccMin.y, m_eParticleSystem.vTurnAccMax.y, true);
	m_pParticles[_iIndex].vTurnAcc.z = GAMEINSTANCE->PickRandomFloat(m_eParticleSystem.vTurnAccMin.z, m_eParticleSystem.vTurnAccMax.z, true);

	// 6. 색상 범위 내 결정
	_int iRand = rand() % 2;

	if (0 == iRand)
	{
		m_pParticles[_iIndex].vColor.x = GAMEINSTANCE->PickRandomFloat(m_vStartColorMin.x - 0.01f, m_vStartColorMin.x + 0.01f, true);
		m_pParticles[_iIndex].vColor.y = GAMEINSTANCE->PickRandomFloat(m_vStartColorMin.y - 0.01f, m_vStartColorMin.y + 0.01f, true);
		m_pParticles[_iIndex].vColor.z = GAMEINSTANCE->PickRandomFloat(m_vStartColorMin.z - 0.01f, m_vStartColorMin.z + 0.01f, true);
		m_pParticles[_iIndex].vColor.w = GAMEINSTANCE->PickRandomFloat(m_vStartColorMin.w - 0.01f, m_vStartColorMin.w + 0.01f, true);
	}
	else
	{
		m_pParticles[_iIndex].vColor.x = GAMEINSTANCE->PickRandomFloat(m_vStartColorMax.x - 0.01f, m_vStartColorMax.x + 0.01f, true);
		m_pParticles[_iIndex].vColor.y = GAMEINSTANCE->PickRandomFloat(m_vStartColorMax.y - 0.01f, m_vStartColorMax.y + 0.01f, true);
		m_pParticles[_iIndex].vColor.z = GAMEINSTANCE->PickRandomFloat(m_vStartColorMax.z - 0.01f, m_vStartColorMax.z + 0.01f, true);
		m_pParticles[_iIndex].vColor.w = GAMEINSTANCE->PickRandomFloat(m_vStartColorMax.w - 0.01f, m_vStartColorMax.w + 0.01f, true);
	}

	m_pParticles[_iIndex].vCurColor.x = m_pParticles[_iIndex].vColor.x;
	m_pParticles[_iIndex].vCurColor.y = m_pParticles[_iIndex].vColor.y;
	m_pParticles[_iIndex].vCurColor.z = m_pParticles[_iIndex].vColor.z;
	m_pParticles[_iIndex].vCurColor.w = m_pParticles[_iIndex].vColor.w;

	m_pParticles[_iIndex].vNextColor.x = m_pParticles[_iIndex].vColor.x;
	m_pParticles[_iIndex].vNextColor.y = m_pParticles[_iIndex].vColor.y;
	m_pParticles[_iIndex].vNextColor.z = m_pParticles[_iIndex].vColor.z;
	m_pParticles[_iIndex].vNextColor.w = m_pParticles[_iIndex].vColor.w;

	m_pParticles[_iIndex].bActive = true;
	m_iCurrentParticleCnt++;

	m_bEmitted = true;
}

void CTestParticle::SetDefaultInfo(_int _iShaderType, _int _iShaderPass, _int _iRenderGroup)
{
	m_eParticleSystem.iShaderType = (ESHADER_TYPE)_iShaderType;
	m_eParticleSystem.iShaderPass = _iShaderPass;
	m_eParticleSystem.iRenderGroup = _iRenderGroup;

	m_bEntered = true;
}

void CTestParticle::SetMainInfo(_int _iMaxParticles, _float _fDuration, _float _fStartTrackPos, _float2 _fLifeTimeRange, _bool _bLoop, _int _iBillBoardType, string _strTexKey, _int _iSpawnType, _int _iSpawnNumPerSec)
{
	m_eParticleSystem.iMaxParticles = _iMaxParticles;
	m_eParticleSystem.fDuration = _fDuration;
	m_eParticleSystem.vLifeTime = _fLifeTimeRange;
	m_eParticleSystem.bLoop = _bLoop;
	m_eParticleSystem.iBillBoardType = _iBillBoardType;
	m_eParticleSystem.strTexDiffuseKey = _strTexKey;
	m_eParticleSystem.iSpawnType = _iSpawnType;
	m_eParticleSystem.fSpawnNumPerSec = (_float)_iSpawnNumPerSec;
	m_eParticleSystem.StartTrackPos = (_double)_fStartTrackPos;

	m_bEntered = true;

}

void CTestParticle::SetPosInfo(_int _iPosType, _bool _bPosFollow, string _strPosFollowBoneKey,
						_int _iPosSetCnt, _float* _fPosTime, _float _vPosMin[5][3], _float _vPosMax[5][3], _int* _iPosEasing, _bool _bUsePosKeyFrame)
{
	m_eParticleSystem.iPosType = _iPosType;
	m_eParticleSystem.bPosFollow = _bPosFollow;
	m_eParticleSystem.strPosFollowBoneKey = _strPosFollowBoneKey;
	m_eParticleSystem.bUsePosKeyFrame = _bUsePosKeyFrame;

	if (0 != m_eParticleSystem.iPosType)
	{
		SetOwner(GAMEINSTANCE->GetGameObject(LEVEL_LOGO, TEXT("Layer_AnimObject")));

		if (2 == m_eParticleSystem.iPosType)
		{
			if ("" == m_eParticleSystem.strPosFollowBoneKey)
			{
				m_eParticleSystem.iPosType = 1;
				return;
			}

			m_pFollowBoneMat = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr(m_eParticleSystem.strPosFollowBoneKey.c_str());

			if (!m_pFollowBoneMat)
			{
				m_eParticleSystem.iPosType = 1;
			}
		}

		if (!m_eParticleSystem.bPosFollow)
		{
			m_OwnerWorld = m_pOwner.lock()->GetTransform()->GetWorldMatrix();
		}
	}

	if (_bUsePosKeyFrame)
	{
		vector<CParticleSystem::POSITION_KEYFRAME_PARTICLE> PosKeyFrames;

		for (_int i = 0; i < _iPosSetCnt; ++i)
		{
			CParticleSystem::POSITION_KEYFRAME_PARTICLE positionKey = {};

			positionKey.fTime = _fPosTime[i];
			positionKey.vPosMin = _float3(_vPosMin[i][0], _vPosMin[i][1], _vPosMin[i][2]);
			positionKey.vPosMax = _float3(_vPosMax[i][0], _vPosMax[i][1], _vPosMax[i][2]);
			positionKey.eEaseType = _iPosEasing[i];

			PosKeyFrames.push_back(positionKey);
		}

		m_eParticleSystem.KeyFrames.PosKeyFrames = PosKeyFrames;

		if (0 >= m_eParticleSystem.KeyFrames.PosKeyFrames.size())
		{
			m_vStartPosMin = { 0.f, 0.f, 0.f };
			m_vStartPosMax = { 0.f, 0.f, 0.f };
		}

		else
		{
			m_vStartPosMin = m_eParticleSystem.KeyFrames.PosKeyFrames.begin()->vPosMin;
			m_vStartPosMax = m_eParticleSystem.KeyFrames.PosKeyFrames.begin()->vPosMax;
		}
	}

	else
	{
		m_eParticleSystem.vPosMin = _float3(_vPosMin[0][0], _vPosMin[0][1], _vPosMin[0][2]);
		m_eParticleSystem.vPosMax = _float3(_vPosMax[0][0], _vPosMax[0][1], _vPosMax[0][2]);
	}

	m_bEntered = true;
}

void CTestParticle::SetSpeedInfo(_bool _bSpeedMode, _int _iSpeedSetCnt, _float* _fSpeedTimes, _float _vSpeedMins[5][3], _float _vSpeedMax[5][3], _int* _iSpeedEasing)
{
	m_eParticleSystem.bSpeedMode = _bSpeedMode;

	vector<CParticleSystem::SPEED_KEYFRAME_PARTICLE> SpeedKeyFrames;

	for (_int i = 0; i < _iSpeedSetCnt; ++i)
	{
		CParticleSystem::SPEED_KEYFRAME_PARTICLE speedKey = {};

		speedKey.fTime = _fSpeedTimes[i];
		speedKey.vSpeedMin = _float3(_vSpeedMins[i][0], _vSpeedMins[i][1], _vSpeedMins[i][2]);
		speedKey.vSpeedMax = _float3(_vSpeedMax[i][0], _vSpeedMax[i][1], _vSpeedMax[i][2]);
		speedKey.eEaseType = _iSpeedEasing[i];

		SpeedKeyFrames.push_back(speedKey);
	}

	m_eParticleSystem.KeyFrames.SpeedKeyFrames = SpeedKeyFrames;

	if (0 >= m_eParticleSystem.KeyFrames.SpeedKeyFrames.size())
	{
		m_vStartSpeedMin = { 0.f, 0.f, 0.f };
		m_vStartSpeedMax = { 0.f, 0.f, 0.f };
	}

	else
	{
		// 세팅 해줘야하나???
		m_vStartSpeedMin = m_eParticleSystem.KeyFrames.SpeedKeyFrames.begin()->vSpeedMin;
		m_vStartSpeedMax = m_eParticleSystem.KeyFrames.SpeedKeyFrames.begin()->vSpeedMax;
	}

	m_bEntered = true;
}

void CTestParticle::SetSpeedInfo(_bool _bSpeedMode, _float3 _vStartSpeedMin, _float3 _vStartSpeedMax, _float3 _vSpeedAccMin, _float3 _vSpeedAccMax)
{
	m_eParticleSystem.bSpeedMode = _bSpeedMode;

	m_eParticleSystem.vStartSpeedMin = _vStartSpeedMin;
	m_eParticleSystem.vStartSpeedMax = _vStartSpeedMax;

	m_eParticleSystem.vSpeedAccMin = _vSpeedAccMin;
	m_eParticleSystem.vSpeedAccMax = _vSpeedAccMax;

	m_bEntered = true;
}

void CTestParticle::SetScaleInfo(_int _iScaleSetCnt, _float* _fScaleTimes, _float _vScaleMin[5][3], _float _vScaleMax[5][3], _int* _iScaleEasing)
{
	vector<CParticleSystem::SCALE_KEYFRAME_PARTICLE> ScaleKeyFrames;

	for (_int i = 0; i < _iScaleSetCnt; ++i)
	{
		CParticleSystem::SCALE_KEYFRAME_PARTICLE scaleKey = {};

		scaleKey.fTime = _fScaleTimes[i];
		scaleKey.vScaleMin = _float3(_vScaleMin[i][0], _vScaleMin[i][1], _vScaleMin[i][2]);
		scaleKey.vScaleMax = _float3(_vScaleMax[i][0], _vScaleMax[i][1], _vScaleMax[i][2]);
		scaleKey.eEaseType = _iScaleEasing[i];

		ScaleKeyFrames.push_back(scaleKey);
	}

	m_eParticleSystem.KeyFrames.ScaleKeyFrames = ScaleKeyFrames;

	if (0 >= m_eParticleSystem.KeyFrames.ScaleKeyFrames.size())
	{
		m_vStartScaleMin = {0.f, 0.f, 0.f};
		m_vStartScaleMax = {0.f, 0.f, 0.f};
	}

	else
	{
		m_vStartScaleMin = m_eParticleSystem.KeyFrames.ScaleKeyFrames.begin()->vScaleMin;
		m_vStartScaleMax = m_eParticleSystem.KeyFrames.ScaleKeyFrames.begin()->vScaleMax;
	}

	m_bEntered = true;
}

void CTestParticle::SetRotationInfo(_float* _vRotationMin, _float* _vRotationMax, _float* _vTurnVelMin, _float* _vTurnVelMax, _float* _fTurnAccMin, _float* _fTurnAccMax)
{
	m_eParticleSystem.vRotationMin = _float3(_vRotationMin[0], _vRotationMin[1], _vRotationMin[2]);
	m_eParticleSystem.vRotationMax = _float3(_vRotationMax[0], _vRotationMax[1], _vRotationMax[2]);

	m_eParticleSystem.vTurnVelMin = _float3(_vTurnVelMin[0], _vTurnVelMin[1], _vTurnVelMin[2]);
	m_eParticleSystem.vTurnVelMax = _float3(_vTurnVelMax[0], _vTurnVelMax[1], _vTurnVelMax[2]);

	m_eParticleSystem.vTurnAccMin = _float3(_fTurnAccMin[0], _fTurnAccMin[1], _fTurnAccMin[2]);
	m_eParticleSystem.vTurnAccMax = _float3(_fTurnAccMax[0], _fTurnAccMax[1], _fTurnAccMax[2]);

	m_bEntered = true;
}

void CTestParticle::SetColorInfo(_bool _bUseColor, _int _iColorSetCnt, _float* _fColorTimes, _float _vColorMin[5][4], _float _vColorMax[5][4], _int* _iColorEasing)
{
	m_eParticleSystem.bUseColor = _bUseColor;

	if (!m_eParticleSystem.bUseColor)
	{
		m_vStartColorMin = { 0.f, 0.f, 0.f, 0.f };
		m_vStartColorMax = { 0.f, 0.f, 0.f, 0.f };

		return;
	}

	vector<CParticleSystem::COLOR_KEYFRAME_PARTICLE> ColorKeyFrames;

	for (_int i = 0; i < _iColorSetCnt; ++i)
	{
		CParticleSystem::COLOR_KEYFRAME_PARTICLE colorKey = {};

		colorKey.fTime = _fColorTimes[i];
		colorKey.vColorMin = _float4(_vColorMin[i][0], _vColorMin[i][1], _vColorMin[i][2], _vColorMin[i][3]);
		colorKey.vColorMax = _float4(_vColorMax[i][0], _vColorMax[i][1], _vColorMax[i][2], _vColorMax[i][3]);
		colorKey.eEaseType = _iColorEasing[i];

		ColorKeyFrames.push_back(colorKey);
	}

	m_eParticleSystem.KeyFrames.ColorKeyFrames = ColorKeyFrames;

	if (0 >= m_eParticleSystem.KeyFrames.ColorKeyFrames.size())
	{
		m_vStartColorMin = { 1.f, 1.f, 1.f, 1.f };
		m_vStartColorMax = { 1.f, 1.f, 1.f, 1.f };
	}

	else
	{
		m_vStartColorMin = m_eParticleSystem.KeyFrames.ColorKeyFrames.begin()->vColorMin;
		m_vStartColorMax = m_eParticleSystem.KeyFrames.ColorKeyFrames.begin()->vColorMax;
	}

	m_bEntered = true;
}

void CTestParticle::SetShaderInfo(_bool _bUseGlowColor, _float _vGlowColor[4])
{
	m_eParticleSystem.bUseGlowColor = _bUseGlowColor;
	m_eParticleSystem.vGlowColor = _float4(_vGlowColor[0], _vGlowColor[1], _vGlowColor[2], _vGlowColor[3]);

	m_bEntered = true;
}

void CTestParticle::SetHazeInfo(_bool _bUseHazeEffect, _float _vHazeSpeedMin[3], _float _vHazeSpeedMax[3], _bool _bRelativeOwner)
{
	m_eParticleSystem.bUseHazeEffect = _bUseHazeEffect;
	m_eParticleSystem.vHazeSpeedMin = _float3(_vHazeSpeedMin[0], _vHazeSpeedMin[1], _vHazeSpeedMin[2]);
	m_eParticleSystem.vHazeSpeedMax = _float3(_vHazeSpeedMax[0], _vHazeSpeedMax[1], _vHazeSpeedMax[2]);

	m_eParticleSystem.bRelativeOwner = _bRelativeOwner;

	m_bEntered = true;
}

void CTestParticle::SetPivotInfo(_bool _bUsePivot, _float _vPivotMin[3], _float _vPivotMax[3])
{
	m_eParticleSystem.bUsePivot = _bUsePivot;
	m_eParticleSystem.vPivotMin = _float3(_vPivotMin[0], _vPivotMin[1], _vPivotMin[2]);
	m_eParticleSystem.vPivotMax = _float3(_vPivotMax[0], _vPivotMax[1], _vPivotMax[2]);

	m_bEntered = true;
}

void CTestParticle::SetImplosionInfo(_bool _bUseImplosion)
{
	if (_bUseImplosion)
	{
	}
}



shared_ptr<CTestParticle> CTestParticle::Create(_int _iNumInstance, string _strTexKey)
{
	shared_ptr<CTestParticle> pInstance = make_shared<CTestParticle>();

	if (FAILED(pInstance->Initialize(_iNumInstance, _strTexKey)))
		MSG_BOX("Failed to Create : CTestParticle");

	return pInstance;
}
