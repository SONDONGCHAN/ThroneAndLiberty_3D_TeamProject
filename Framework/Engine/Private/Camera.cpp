#include "Camera.h"
#include "GameInstance.h"

CCamera::CCamera()
    :CGameObject()
{
}

HRESULT CCamera::Initialize(CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc)
{
	if (!_pCamDes)
        return E_FAIL;

    if (FAILED(__super::Initialize(_TransDesc)))
        return E_FAIL;

    m_fFovy = _pCamDes->fFovy;
	m_fShakeFovy = m_fFovy;
    m_fNear = _pCamDes->fNear;
    m_fFar = _pCamDes->fFar;
    m_fAspect = _pCamDes->fAspect;

    m_pTransformCom->SetState(CTransform::STATE_POSITION, _pCamDes->vEye);
    m_pTransformCom->LookAt(_pCamDes->vAt);

    return S_OK;
}

void CCamera::PriorityTick(_float fTimeDelta)
{
	TickLerpEyeAt(fTimeDelta);
	TickLerpFovy(fTimeDelta);
	TickShake(fTimeDelta);
}

void CCamera::Tick(_float fTimeDelta)
{
}

void CCamera::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
    return S_OK;
}

void CCamera::LerpEyeAt(float _fTime, float _fWeight, const _float3& _vStartEye, const _float3& _vStartAt, const _float3& _vEndEye, const _float3& _vEndAt)
{
	m_isEyeAtLerp = true;
	memset(&m_fEyeAtAccTime, 0, sizeof(float));
	memcpy_s(&m_fEyeAtLerpTime, sizeof(float), &_fTime, sizeof(float));
	memcpy_s(&m_fEyeAtLerpWeight, sizeof(float), &_fWeight, sizeof(float));
	memcpy_s(&m_vStartEye, sizeof(_float3), &_vStartEye, sizeof(_float3));
	memcpy_s(&m_vStartAt, sizeof(_float3), &_vStartAt, sizeof(_float3));
	memcpy_s(&m_vEndEye, sizeof(_float3), &_vEndEye, sizeof(_float3));
	memcpy_s(&m_vEndAt, sizeof(_float3), &_vEndAt, sizeof(_float3));
	SetEyeAt(_vStartEye, _vStartAt);
}

void CCamera::LerpFovy(float _fFovy, float _fTime)
{
	m_isFovyLerp = true;
	memset(&m_fFovyAccTime, 0, sizeof(float));
	memcpy_s(&m_fFovyLerpTime, sizeof(float), &_fTime, sizeof(float));
	memcpy_s(&m_fStartFovy, sizeof(float), &m_fFovy, sizeof(float));
	memcpy_s(&m_fLerpToFovy, sizeof(float), &_fFovy, sizeof(float));
}

void CCamera::SetEyeAt(const _float3& _vEye, const _float3& _vAt)
{
	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vEye);
	m_pTransformCom->LookAt(_vAt);
	m_vCamAt = _vAt;
}

void CCamera::SetFovEyeAt(float _fFov, const _float3& _vEye, const _float3& _vAt)
{
	m_fFovy = _fFov;
	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vEye);
	m_pTransformCom->LookAt(_vAt);
}

void CCamera::CameraTurnOn()
{
    m_isRecording = true;
}

void CCamera::CameraTurnOff()
{
	m_isRecording = false;
}

bool CCamera::isRecording()
{
	return m_isRecording;
}

bool CCamera::isCamShake()
{
	return m_isShake;
}

void CCamera::CamShake(UINT _eShakeFlags, float _fDuration, ROT_DESC* _pRotDesc, LOC_DESC* _pLocDesc, FOV_DESC* _pFovDesc)
{
	if (nullptr != _pRotDesc || nullptr != _pLocDesc || nullptr != _pFovDesc)
	{
		memcpy_s(&m_eShakeFlag, sizeof(UINT), &_eShakeFlags, sizeof(UINT));
		m_isShake = true;
		memcpy_s(&m_fShakeDuration, sizeof(float), &_fDuration, sizeof(float));
		memset(&m_fShakeAccTime, 0, sizeof(float));
		shared_ptr<CGameInstance> pGameinstance = GAMEINSTANCE;
		if (m_eShakeFlag & SHAKE_ROT && _pRotDesc)
		{
			memcpy_s(m_fRotAmplitude, sizeof(_float3), &_pRotDesc->vRotAmplitude, sizeof(_float3));
			memcpy_s(m_fRotFrequency, sizeof(_float3), &_pRotDesc->vRotFrequency, sizeof(_float3));
			memcpy_s(&m_vRotBlendInOut, sizeof(_float2), &_pRotDesc->vBlendInOut, sizeof(_float2));
			memcpy_s(&m_vRotBlendWeight, sizeof(_float2), &_pRotDesc->vInOutWeight, sizeof(_float2));
			m_eShakeFunc[ROT] = _pRotDesc->eShakeFunc;
			if (_pRotDesc->isOffSetRand)
			{
				for (UINT i = PITCH; i < AXIS_END; i++)
				{
					m_fRotRandOffSet[i] = pGameinstance->PickRandomFloat(0.0f, XM_2PI);
				}
			}
			else
			{
				memset(m_fRotRandOffSet, 0, sizeof(float) * 3);
			}
		}
		if (m_eShakeFlag & SHAKE_LOC && _pLocDesc)
		{
			memcpy_s(m_fLocAmplitude, sizeof(_float3), &_pLocDesc->vLocAmplitude, sizeof(_float3));
			memcpy_s(m_fLocFrequency, sizeof(_float3), &_pLocDesc->vLocFrequency, sizeof(_float3));
			memcpy_s(&m_vLocBlendInOut, sizeof(_float2), &_pLocDesc->vBlendInOut, sizeof(_float2));
			memcpy_s(&m_vLocBlendWeight, sizeof(_float2), &_pLocDesc->vInOutWeight, sizeof(_float2));
			m_eShakeFunc[LOC] = _pLocDesc->eShakeFunc;
			if (_pLocDesc->isOffSetRand)
			{
				for (UINT i = PITCH; i < AXIS_END; i++)
				{
					m_fLocRandOffSet[i] = pGameinstance->PickRandomFloat(0.0f, XM_2PI);
				}
			}
			else
			{
				memset(m_fLocRandOffSet, 0, sizeof(float) * 3);
			}

		}
		if (m_eShakeFlag & SHAKE_FOV && _pFovDesc)
		{
			memcpy_s(&m_fFovyAmplitude, sizeof(float), &_pFovDesc->fFovyAmplitude, sizeof(float));
			memcpy_s(&m_fFovyFrequency, sizeof(float), &_pFovDesc->fFovyFrequency, sizeof(float));
			memcpy_s(&m_vFovyBlendInOut, sizeof(_float2), &_pFovDesc->vBlendInOut, sizeof(_float2));
			memcpy_s(&m_vFovyBlendWeight, sizeof(_float2), &_pFovDesc->vInOutWeight, sizeof(_float2));
			m_eShakeFunc[FOV] = _pFovDesc->eShakeFunc;
		}
		m_ShakeWorldMatrix = m_pTransformCom->GetWorldMatrix();
	}
}

void CCamera::CamShakeRot(ESHAKEFUNC _eShakeFunc, float _fDuration, const _float3& _vRotAmplitude, const _float3& _vRotFrequency, bool _isOffSetRand , _float2 _vBlendInOut, _float2 _vInOutWeight)
{
	m_eShakeFlag = SHAKE_ROT;
	m_eShakeFunc[ROT] = _eShakeFunc;
	m_isShake = true;
	memcpy_s(&m_fShakeDuration, sizeof(float), &_fDuration, sizeof(float));
	memset(&m_fShakeAccTime, 0, sizeof(float));
	memcpy_s(m_fRotAmplitude, sizeof(_float3), &_vRotAmplitude, sizeof(_float3));
	memcpy_s(m_fRotFrequency, sizeof(_float3), &_vRotFrequency, sizeof(_float3));
	memcpy_s(&m_vRotBlendInOut, sizeof(_float2), &_vBlendInOut, sizeof(_float2));
	memcpy_s(&m_vRotBlendWeight, sizeof(_float2), &_vInOutWeight, sizeof(_float2));
	if (_isOffSetRand)
	{
		shared_ptr<CGameInstance> pGameinstance = GAMEINSTANCE;
		for (UINT i = PITCH; i < AXIS_END; i++)
		{
			m_fRotRandOffSet[i] = pGameinstance->PickRandomFloat(0.0f, XM_2PI);
		}
	}
	else
	{
		memset(m_fRotRandOffSet, 0, sizeof(float) * 3);
	}
	
	m_ShakeWorldMatrix = m_pTransformCom->GetWorldMatrix();
}

void CCamera::CamShakeLoc(ESHAKEFUNC _eShakeFunc, float _fDuration, const _float3& _vLocAmplitude, const _float3& _vLocFrequency, bool _isOffSetRand , _float2 _vBlendInOut, _float2 _vInOutWeight)
{
	m_eShakeFlag = SHAKE_LOC;
	m_eShakeFunc[LOC] = _eShakeFunc;
	m_isShake = true;
	memcpy_s(&m_fShakeDuration, sizeof(float), &_fDuration, sizeof(float));
	memset(&m_fShakeAccTime, 0, sizeof(float));
	memcpy_s(m_fLocAmplitude, sizeof(_float3), &_vLocAmplitude, sizeof(_float3));
	memcpy_s(m_fLocFrequency, sizeof(_float3), &_vLocFrequency, sizeof(_float3));
	memcpy_s(&m_vLocBlendInOut, sizeof(_float2), &_vBlendInOut, sizeof(_float2));
	memcpy_s(&m_vLocBlendWeight, sizeof(_float2), &_vInOutWeight, sizeof(_float2));
	if (_isOffSetRand)
	{
		shared_ptr<CGameInstance> pGameinstance = GAMEINSTANCE;
		for (UINT i = PITCH; i < AXIS_END; i++)
		{
			m_fLocRandOffSet[i] = pGameinstance->PickRandomFloat(0.0f, XM_2PI);
		}
	}
	else
	{
		memset(m_fRotRandOffSet, 0, sizeof(float) * 3);
	}
	m_ShakeWorldMatrix = m_pTransformCom->GetWorldMatrix();
}

void CCamera::CamShakeFov(ESHAKEFUNC _eShakeFunc, float _fDuration, float _fFovyAmplitude, float _fFovyFrequency, _float2 _vBlendInOut, _float2 _vInOutWeight)
{
	m_eShakeFlag = SHAKE_FOV;
	m_eShakeFunc[FOV] = _eShakeFunc;
	m_isShake = true;
	memcpy_s(&m_fShakeDuration, sizeof(float), &_fDuration, sizeof(float));
	memset(&m_fShakeAccTime, 0, sizeof(float));
	memcpy_s(&m_fFovyAmplitude, sizeof(float), &_fFovyAmplitude, sizeof(float));
	memcpy_s(&m_fFovyFrequency, sizeof(float), &_fFovyFrequency, sizeof(float));
	memcpy_s(&m_vFovyBlendInOut, sizeof(_float2), &_vBlendInOut, sizeof(_float2));
	memcpy_s(&m_vFovyBlendWeight, sizeof(_float2), &_vInOutWeight, sizeof(_float2));
	m_ShakeWorldMatrix = m_pTransformCom->GetWorldMatrix();
}

void CCamera::CamShakeStop()
{
	m_isShake = false;
	m_eShakeFlag = SHAKE_NONE;
}

HRESULT CCamera::SetUpTransformMatices()
{
	if (m_isShake)
	{
		GAMEINSTANCE->SetTransform(CPipeLine::D3DTS_VIEW, m_ShakeWorldMatrix.Invert());
		GAMEINSTANCE->SetTransform(CPipeLine::D3DTS_PROJ, _float4x4(XMMatrixPerspectiveFovLH(m_fShakeFovy, m_fAspect, m_fNear, m_fFar)));
	}
    else
	{
		GAMEINSTANCE->SetTransform(CPipeLine::D3DTS_VIEW, m_pTransformCom->GetWorldMatrixInverse());
		GAMEINSTANCE->SetTransform(CPipeLine::D3DTS_PROJ, _float4x4(XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar)));
	}

	return S_OK;
}

void CCamera::TickLerpEyeAt(float _fTimeDelta)
{
	if (m_isEyeAtLerp)
	{
		if (m_fEyeAtAccTime < m_fEyeAtLerpTime)
		{
			SetEyeAt(LinearIp(GetEye(), m_vEndEye, m_fEyeAtLerpWeight), LinearIp(m_vCamAt, m_vEndAt, m_fEyeAtLerpWeight));

			m_fEyeAtAccTime += _fTimeDelta;
		}
		else
		{
			SetEyeAt(m_vEndEye, m_vEndAt);
			memset(&m_fEyeAtAccTime, 0, sizeof(float));
			memset(&m_fEyeAtLerpTime, 0, sizeof(float));
			memset(&m_fEyeAtLerpWeight, 0, sizeof(float));
			m_isEyeAtLerp = false;
		}
	}
}

void CCamera::TickLerpFovy(float _fTimeDelta)
{
	if (m_isFovyLerp)
	{
		if (m_fFovyAccTime < m_fFovyLerpTime)
		{
			m_fFovy = LinearIp(m_fStartFovy, m_fLerpToFovy, m_fFovyAccTime / m_fFovyLerpTime);

			m_fFovyAccTime += _fTimeDelta;
		}
		else
		{
			memcpy_s(&m_fFovy, sizeof(float), &m_fLerpToFovy, sizeof(float));
			memset(&m_fFovyAccTime, 0, sizeof(float));
			memset(&m_fFovyLerpTime, 0, sizeof(float));
			m_isFovyLerp = false;
		}
	}
}

void CCamera::TickShake(float _fTimeDelta)
{
    if (m_isShake)
    {
        if (m_fShakeAccTime < m_fShakeDuration)
		{
			shared_ptr<CGameInstance> pGameinstance = GAMEINSTANCE;
			m_ShakeWorldMatrix = m_pTransformCom->GetWorldMatrix();
			float fRatio = m_fShakeAccTime / m_fShakeDuration;
			if (m_eShakeFlag & SHAKE_LOC)
			{
				_float3 LocVec;
				float fLerp = 1.0f;
				if (m_fShakeAccTime < m_vLocBlendInOut.x)
				{
					fLerp = pow(m_fShakeAccTime / m_vLocBlendInOut.x, m_vLocBlendWeight.x);
				}
				else if (m_fShakeAccTime > m_fShakeDuration - m_vLocBlendInOut.y)
				{
					fLerp = pow((m_fShakeDuration - m_fShakeAccTime) / m_vLocBlendInOut.y, m_vLocBlendWeight.y);
				}
				else
				{
					fLerp = 1.0f;
				}
				if (m_eShakeFunc[LOC] == SHAKE_WAVE)
				{
					LocVec.x = ComputeShakeWave(m_fLocAmplitude[PITCH], m_fLocFrequency[PITCH], m_fLocRandOffSet[PITCH], fRatio) * fLerp;
					LocVec.y = ComputeShakeWave(m_fLocAmplitude[YAW], m_fLocFrequency[YAW], m_fLocRandOffSet[YAW], fRatio) * fLerp;
					LocVec.z = ComputeShakeWave(m_fLocAmplitude[ROLL], m_fLocFrequency[ROLL], m_fLocRandOffSet[ROLL], fRatio) * fLerp;
				}
				else if (m_eShakeFunc[LOC] == SHAKE_LINEAR)
				{
					LocVec.x = ComputeShakeLinear(m_fLocAmplitude[PITCH], m_fLocFrequency[PITCH], m_fLocRandOffSet[PITCH]) * fLerp;
					LocVec.y = ComputeShakeLinear(m_fLocAmplitude[YAW], m_fLocFrequency[YAW], m_fLocRandOffSet[YAW]) * fLerp;
					LocVec.z = ComputeShakeLinear(m_fLocAmplitude[ROLL], m_fLocFrequency[ROLL], m_fLocRandOffSet[ROLL]) * fLerp;
				}
				else
				{

				}
				_float3 vPos = m_ShakeWorldMatrix.Translation() + m_ShakeWorldMatrix.Right() * LocVec.x + m_ShakeWorldMatrix.Up() * LocVec.y + m_ShakeWorldMatrix.Forward() * LocVec.z;
				m_ShakeWorldMatrix.Translation(vPos);
			}
			if (m_eShakeFlag & SHAKE_ROT)
			{
				_float3 RotVec;
				float fLerp = 1.0f;
				if (m_fShakeAccTime < m_vRotBlendInOut.x)
				{
					fLerp = pow(m_fShakeAccTime / m_vRotBlendInOut.x, m_vRotBlendWeight.x);
				}
				else if (m_fShakeAccTime > m_fShakeDuration - m_vRotBlendInOut.y)
				{
					fLerp = pow((m_fShakeDuration - m_fShakeAccTime) / m_vRotBlendInOut.y, m_vRotBlendWeight.y);
				}
				else
				{
					fLerp = 1.0f;
				}
				if (m_eShakeFunc[ROT] == SHAKE_WAVE)
				{
					RotVec.x = ComputeShakeWave(m_fRotAmplitude[PITCH], m_fRotFrequency[PITCH], m_fRotRandOffSet[PITCH], fRatio) * fLerp;
					RotVec.y = ComputeShakeWave(m_fRotAmplitude[YAW], m_fRotFrequency[YAW], m_fRotRandOffSet[YAW], fRatio) * fLerp;
					RotVec.z = ComputeShakeWave(m_fRotAmplitude[ROLL], m_fRotFrequency[ROLL], m_fRotRandOffSet[ROLL], fRatio) * fLerp;
				}
				else if (m_eShakeFunc[ROT] == SHAKE_LINEAR)
				{
					RotVec.x = ComputeShakeLinear(m_fRotAmplitude[PITCH], m_fRotFrequency[PITCH], m_fRotRandOffSet[PITCH]) * fLerp;
					RotVec.y = ComputeShakeLinear(m_fRotAmplitude[YAW], m_fRotFrequency[YAW], m_fRotRandOffSet[YAW]) * fLerp;
					RotVec.z = ComputeShakeLinear(m_fRotAmplitude[ROLL], m_fRotFrequency[ROLL], m_fRotRandOffSet[ROLL]) * fLerp;
				}
				else
				{

				}
				_float3 vPos = m_ShakeWorldMatrix.Translation();
				m_ShakeWorldMatrix.Translation({ 0.0f,0.0f,0.0f });
				m_ShakeWorldMatrix = m_ShakeWorldMatrix * XMMatrixRotationAxis(m_ShakeWorldMatrix.Right(), RotVec.x) * XMMatrixRotationAxis(m_ShakeWorldMatrix.Up(), RotVec.y) * XMMatrixRotationAxis(m_ShakeWorldMatrix.Forward(), RotVec.z);
				m_ShakeWorldMatrix.Translation(vPos);
			}
			if (m_eShakeFlag & SHAKE_FOV)
			{
				float fLerp = 1.0f;
				if (m_fShakeAccTime < m_vFovyBlendInOut.x)
				{
					fLerp = pow(m_fShakeAccTime / m_vFovyBlendInOut.x, m_vFovyBlendWeight.x);
				}
				else if (m_fShakeAccTime > m_fShakeDuration - m_vFovyBlendInOut.y)
				{
					fLerp = pow((m_fShakeDuration - m_fShakeAccTime) / m_vFovyBlendInOut.y, m_vFovyBlendWeight.y);
				}
				else
				{
					fLerp = 1.0f;
				}
				if (m_eShakeFunc[FOV] == SHAKE_WAVE)
				{
					m_fShakeFovy = m_fFovy + ComputeShakeWave(m_fFovyAmplitude, m_fFovyFrequency, 0.0f, fRatio) * fLerp;
				}
				else if (m_eShakeFunc[FOV] == SHAKE_LINEAR)
				{
					m_fShakeFovy = m_fFovy + ComputeShakeLinear(m_fFovyAmplitude, m_fFovyFrequency, 0.0f) * fLerp;
				}
				else
				{

				}
            }

			m_fShakeAccTime += _fTimeDelta;
        }
        else
        {
            m_eShakeFlag = SHAKE_NONE;
            m_isShake = false;
			memcpy_s(&m_fShakeFovy, sizeof(float), &m_fFovy, sizeof(float));
        }
    }
}

_float CCamera::ComputeShakeWave(float _fAmplitude, float _fFrequency, float _fOffSet, float _fRatio)
{
	return _fAmplitude == 0.0f ? 0.0f : _fAmplitude * sin(XM_2PI * _fFrequency * _fRatio + _fOffSet);
}

_float CCamera::ComputeShakeLinear(float _fAmplitude, float _fFrequency, float _fOffSet)
{
	if (_fAmplitude == 0.0f)
		return 0.0f;
	float fTime = m_fShakeAccTime + _fOffSet;
	float fHaltFreqTime = m_fShakeDuration * 0.5f / _fFrequency;
	float fRatio = fTime / fHaltFreqTime;
	UINT Cnt = UINT(fRatio);
	float ComputeRatio = Cnt % 2 == 0 ? fRatio - (float)Cnt : 1.0f - (fRatio - (float)Cnt);
	return LinearIp(-_fAmplitude, _fAmplitude, ComputeRatio);
}

HRESULT CCamera::CreateLightFrustum()
{
	return S_OK;
}

HRESULT CCamera::SetUpCascadeDesc()
{
	{
		{
			//카메라의 역행렬
			_float4x4 WorldMat = m_isShake ? m_ShakeWorldMatrix : m_pTransformCom->GetWorldMatrix();
			_float4x4 ViewMat = WorldMat.Invert();

			_float3 vLightDir = GAMEINSTANCE->GetLightDir();

			//시야각을 통한 수직 시야각(Vertical)
			_float fTanHalfVFov = m_isShake ? tanf(m_fShakeFovy * 0.5f) : tanf(m_fFovy * 0.5f);
			//시야각을 통한 수평 시야각(Horizontal)
			_float fTanHalfHFov = fTanHalfVFov * m_fAspect;

			m_fCascadeEnd[0] = m_fNear;
			m_fCascadeEnd[1] = 20.f;
			m_fCascadeEnd[2] = 35.f;
			m_fCascadeEnd[3] = 200.f;

			/*이 이상의 Depth를 가진 오브젝트들의 그림자는 생략*/

			//3개의 절두체로 나누기 때문에 절두체 위치 구하는걸 3번 반복
			for (_uint i = 0; i < 3; ++i) {

				// X+, Y+ 의 near와 far 점을 각각 구함
				//거리로  Near Far의 X,Y길이를 구하는 거기 때문에 별도 수정 필요없음
				float fXNear = m_fCascadeEnd[i] * fTanHalfHFov;
				float fXFar = m_fCascadeEnd[i + 1] * fTanHalfHFov;
				float fYNear = m_fCascadeEnd[i] * fTanHalfVFov;
				float fYFar = m_fCascadeEnd[i + 1] * fTanHalfVFov;
				//구한 점을 통해서 절두체의 Near, Far 평면을 구성함
				_float4 vFrustumCorners[8] =
				{
					{fXNear,fYNear,m_fCascadeEnd[i],1.0f},
					{-fXNear,fYNear,m_fCascadeEnd[i],1.0f},
					{fXNear,-fYNear,m_fCascadeEnd[i],1.0f},
					{-fXNear,-fYNear,m_fCascadeEnd[i],1.0f},
					//far
					{fXFar,fYFar,m_fCascadeEnd[i + 1],1.0f},
					{-fXFar,fYFar,m_fCascadeEnd[i + 1],1.0f},
					{fXFar,-fYFar,m_fCascadeEnd[i + 1],1.0f},
					{-fXFar,-fYFar,m_fCascadeEnd[i + 1],1.0f}
				};


				_float4 vCenterPos = _float4();

				//절두체 중심점 구하기
				for (_uint j = 0; j < 8; ++j)
				{
					vFrustumCorners[j] = XMVector4Transform(vFrustumCorners[j], WorldMat);
					vCenterPos += vFrustumCorners[j];
				}

				vCenterPos /= 8.f;

				float fRadius = 0.f;

				for (_uint j = 0; j < 8; ++j) {

					float fDistance = (vFrustumCorners[j] - vCenterPos).Length();
					fRadius = max(fDistance, fRadius);
				}


				fRadius = ceil(fRadius * 16.f) / 16.f;

				_float3 vMaxExtents = _float3(fRadius, fRadius, fRadius + 60.f);
				_float3 vMinExtents = -vMaxExtents;

				_float3 vShadowCamPos = _float3(vCenterPos) + (vLightDir * vMinExtents.z);
				//Light World Matrix
				//_float4x4 lightMatrix = _float4x4::CreateWorld(vShadowCamPos, _float3(vCenterPos.x, vCenterPos.y, vCenterPos.z), _float3(0.0f, 1.0f, 0.0f));
				
				if (fabs((vShadowCamPos - m_vPrevCenterPos[i]).Length()) < 5.f) {
					break;
				}
				
				m_vPrevCenterPos[i] = vShadowCamPos;

				_float4x4 lightMatrix = _float4x4::CreateWorld(vShadowCamPos, -vLightDir, _float3(0.0f, 1.0f, 0.0f));
				//-vLightDir
				_float4x4 lightInv = lightMatrix.Invert();
				m_shadowView[i] = lightInv;

				_float3 vCascadeExtents = vMaxExtents - vMinExtents;

				//vCascadeExtents.z += (1000.f - vCascadeExtents.z);
				//float fNear = vCascadeExtents.z - m_fCascadeEnd[i + 1];
			   // _float4x4 matProj = XMMatrixOrthographicLH(vCascadeExtents.x, vCascadeExtents.y, 1000.f, fNear);
				_float4x4 matProj = XMMatrixOrthographicLH(vCascadeExtents.x, vCascadeExtents.y, 0.1f, vCascadeExtents.z);
				_float4x4 matReverseProj = XMMatrixOrthographicLH(vCascadeExtents.x, vCascadeExtents.y, vCascadeExtents.z, 0.f);
				m_shadowOrthoProj[i] = matProj;
				m_shadowOrthoReverseProj[i] = matReverseProj;
			}
		}
	}
	
    /*PipeLine에 Shadow Projection 등록*/
    GAMEINSTANCE->SetShadowProjMat(m_shadowOrthoProj);
    GAMEINSTANCE->SetShadowViewMat(m_shadowView);
	GAMEINSTANCE->SetShadowWorldMat(m_shadowOrthoReverseProj);
   
    return S_OK;
}
