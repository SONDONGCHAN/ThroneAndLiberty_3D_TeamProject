#include "pch.h"
#include "CameraKeyFrame.h"

CCameraKeyFrame::CCameraKeyFrame()
{
}

CCameraKeyFrame::~CCameraKeyFrame()
{
}

shared_ptr<CCameraKeyFrame> CCameraKeyFrame::Create(_float3 _vPosition, _float3 _vLook)
{
	shared_ptr<CCameraKeyFrame> pInstance = make_shared<CCameraKeyFrame>();

	if (FAILED(pInstance->Initialize(_vPosition, _vLook)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CCameraKeyFrame::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CCameraKeyFrame::Initialize(_float3 _vPosition, _float3 _vLook)
{
	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;

	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);
	m_eShaderType = ESHADER_TYPE::ST_CUBE;
	m_iShaderPass = 1;

	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	m_pTransform_At = CTransform::Create(pGameInstance->GetDeviceInfo(), pGameInstance->GetDeviceContextInfo(), nullptr);

	m_pTransform_At->SetState(CTransform::STATE_POSITION, _vPosition + _vLook * 3.0f);

	return S_OK;
}

void CCameraKeyFrame::PriorityTick(float _fTimeDelta)
{
}

void CCameraKeyFrame::Tick(float _fTimeDelta)
{
}

void CCameraKeyFrame::LateTick(float _fTimeDelta)
{
	_float3 vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_float3 vAtPos = m_pTransform_At->GetState(CTransform::STATE_POSITION);
	if (m_vPrePos != vPos || m_vPreAtPos != vAtPos && vPos != vAtPos)
	{
		m_vPrePos = vPos;
		m_vPreAtPos = vAtPos;
		m_pTransformCom->LookAt(vAtPos);
	}
	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONLIGHT, shared_from_this());
}

HRESULT CCameraKeyFrame::Render()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
	pGameInstance->ReadyShader(m_eShaderType, m_iShaderPass);
	pGameInstance->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
	pGameInstance->BindRawValue("g_RGBColor", _color{ 0.0f,1.0f,0.0f,1.0f }, sizeof(_color));		// ³ì»ö
	pGameInstance->BeginShader();

	pGameInstance->BindWVPMatrixPerspective(m_pTransform_At->GetWorldMatrix());
	pGameInstance->BindRawValue("g_RGBColor", _color{ 1.0f,1.0f,0.0f,1.0f }, sizeof(_color));		// ³ë¶û»ö
	pGameInstance->BeginShader();
	
	_float3 vPos = LinearIp(m_pTransformCom->GetState(CTransform::STATE_POSITION), m_pTransform_At->GetState(CTransform::STATE_POSITION), 0.5f);
	_float3 vDir = m_pTransform_At->GetState(CTransform::STATE_POSITION) - m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_float vLength = XMVector3Length(vDir).m128_f32[0];

	_float3 vLook = XMVector3Normalize(vDir);
	_float3 vRight = XMVector3Cross(_float3{ 0.0f,1.0f,0.0f }, vLook);
	_float3 vUp = XMVector3Cross(vLook, vRight);

	vRight *= 0.1f;
	vUp *= 0.1f;

	_float4x4 LineWorldMat;
	LineWorldMat.Right(vRight);
	LineWorldMat.Up(vUp);
	LineWorldMat.Forward(vDir);
	LineWorldMat.Translation(vPos);

	pGameInstance->BindWVPMatrixPerspective(LineWorldMat);
	pGameInstance->BindRawValue("g_RGBColor", _color{ 1.0f,1.0f,1.0f,1.0f }, sizeof(_color));		// ³ë¶û»ö
	pGameInstance->BeginShader();

	return S_OK;
}

bool CCameraKeyFrame::GetFadeInfo(UINT* _eFade, float* _fInOutStartTime, float* _fInOutDurtaion)
{
	if (_eFade)
	{
		*_eFade = m_eFade;
	}
	if (_fInOutStartTime)
	{
		*_fInOutStartTime = m_fFadeInOutStartTime;
	}
	if (_fInOutDurtaion)
	{
		*_fInOutDurtaion = m_fFadeInOutDuration;
	}
	if (m_eFade == FADE_NONE)
	{
		return false;
	}

	return true;
}

bool CCameraKeyFrame::GetShakeInfo(UINT* _iShakeFlags, float* _fStartTime, float* _fDuration, ROT_DESC* _OutRotDesc, LOC_DESC* _OutLocDesc, FOV_DESC* _OutFovDesc)
{
	if (m_isCameraShake)
	{
		if (_iShakeFlags)
		{
			*_iShakeFlags = m_iShakeFlags;
		}
		if (_fStartTime)
		{
			*_fStartTime = m_fShakeStartTime;
		}
		if (_fDuration)
		{
			*_fDuration = m_fShakeDuration;
		}
		if (m_iShakeFlags & SHAKE_ROT && _OutRotDesc)
		{
			memcpy_s(_OutRotDesc, sizeof(ROT_DESC), &m_RotDesc, sizeof(ROT_DESC));
		}
		if (m_iShakeFlags & SHAKE_LOC && _OutLocDesc)
		{
			memcpy_s(_OutLocDesc, sizeof(LOC_DESC), &m_LocDesc, sizeof(LOC_DESC));
		}
		if (m_iShakeFlags & SHAKE_FOV && _OutFovDesc)
		{
			memcpy_s(_OutFovDesc, sizeof(FOV_DESC), &m_FovDesc, sizeof(FOV_DESC));
		}
		return true;
	}
	else
	{
		return false;
	}
}

void CCameraKeyFrame::SetCameraShake(float _fShakeStartTime, UINT _eShakeFlags, float _fDuration, ROT_DESC* _pRotDesc, LOC_DESC* _pLocDesc, FOV_DESC* _pFovDesc)
{
	if (_eShakeFlags && _fDuration > 0.0f)
	{
		m_fShakeStartTime = _fShakeStartTime;
		m_isCameraShake = true;
		m_iShakeFlags = _eShakeFlags;
		m_fShakeDuration = _fDuration;
		if (m_iShakeFlags & SHAKE_ROT && _pRotDesc)
			memcpy_s(&m_RotDesc, 48, _pRotDesc, 48);
		if (m_iShakeFlags & SHAKE_LOC && _pLocDesc)
			memcpy_s(&m_LocDesc, 48, _pLocDesc, 48);
		if (m_iShakeFlags & SHAKE_FOV && _pFovDesc)
			memcpy_s(&m_FovDesc, 28, _pFovDesc, 28);
	}
}

void CCameraKeyFrame::ResetFade()
{
	m_eFade = FADE_NONE;
	m_fFadeInOutStartTime = 0.0f;
	m_fFadeInOutDuration = 0.0f;
}

void CCameraKeyFrame::ResetShake()
{
	m_isCameraShake = false;
	m_iShakeFlags = SHAKE_NONE;
	m_fShakeDuration = 0.0f;
	m_RotDesc = ROT_DESC();
	m_LocDesc = LOC_DESC();
	m_FovDesc = FOV_DESC();
}
