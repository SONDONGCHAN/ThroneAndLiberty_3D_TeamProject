#include "pch.h"
#include "CameraSpherical.h"
#include "Monster.h"
#include "BindMagic.h"

Client::CCameraSpherical::CCameraSpherical()
    : CCamera()
{
}

shared_ptr<CCameraSpherical> Client::CCameraSpherical::Create()
{
    shared_ptr<CCameraSpherical> pInstance = make_shared<CCameraSpherical>();

    if (FAILED(pInstance->Initialize()))
    {
        MessageBox(nullptr, L"Initialize ", L"CCameraSpherical::Create", MB_OK);
        pInstance.reset();
        return nullptr;
    }

    return pInstance;
}

HRESULT Client::CCameraSpherical::Initialize()
{
    CAMERA_DESC CamDesc{};
    CamDesc.fAspect = (float)g_iWinSizeX / (float)g_iWinSizeY;
    CamDesc.fFovy = XM_PI * 0.3333333f;

	if (FAILED(CCamera::Initialize(&CamDesc)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CCameraSpherical::Initialize", MB_OK);
		return E_FAIL;
	}

    m_vOffSetPos = { 0.0f,2.0f,0.0f };
	m_vLockOnOffSetPos = { 0.0f,2.2f,0.0f };
    m_fMouseSensitivity = 0.2f;

	m_fDefaultFovy = m_fFovy;

    return S_OK;
}

void Client::CCameraSpherical::PriorityTick(_float _fTimeDelta)
{
	if (!m_pOwnerObject.lock())
	{
		m_IsActive = false;
		m_isRecording = false;
	}

	KeyInput(_fTimeDelta);
	MouseMove(_fTimeDelta);
	RefineCoordinate();

	m_fProjRadius = m_vSphericalPos.x * sin(m_vSphericalPos.y);
	CameraMove(_fTimeDelta);

	ZoomTick();

	CCamera::PriorityTick(_fTimeDelta);

	if (m_isRecording)
	{
		SetUpTransformMatices();
        SetUpCascadeDesc();

    }
}

void Client::CCameraSpherical::Tick(_float _fTimeDelta)
{
}

void Client::CCameraSpherical::LateTick(_float _fTimeDelta)
{
   
}

HRESULT Client::CCameraSpherical::Render()
{
    return S_OK;
}

void Client::CCameraSpherical::SetOffSetPosition(_float3 _vPosition)
{
    m_vOffSetPos = _vPosition;
}

HRESULT Client::CCameraSpherical::SetOwner(shared_ptr<CGameObject> _pOwner)
{
    if (_pOwner)
    {
        if (m_pOwnerObject.lock())
        {
            m_pOwnerObject = _pOwner;
        }
        else
        {
            m_pOwnerObject = _pOwner;
            m_vCamAt = m_pOwnerObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) + m_vOffSetPos;
        }
    }
    else
    {
        MessageBox(nullptr, L"Owner is nullptr", L"CCameraSpherical::SetOwner", MB_OK);
        return E_FAIL;
    }

    return S_OK;
}

void Client::CCameraSpherical::LockOn(shared_ptr<CGameObject> _pTarget)
{
    if (_pTarget)
    {
		if (nullptr == m_pTargetObject.lock())
        {
			m_vLockOnPosition = m_vCamAt + m_pTransformCom->GetState(CTransform::STATE_LOOK);
            //m_vLockOnPosition = _pTarget->GetTransform()->GetState(CTransform::STATE_POSITION) + static_pointer_cast<CMonster>(_pTarget)->GetLockOnOffSetPosition();
        }
        m_pTargetObject = _pTarget;
		m_isLockOn = true;
    }
}

void Client::CCameraSpherical::LockOff()
{
    if (m_pTargetObject.lock())
    {
		m_pTargetObject.reset();
    }
}

void Client::CCameraSpherical::CameraZoom(bool _isZoom)
{
	m_isZoom = _isZoom;
}

_float3 Client::CCameraSpherical::ComputeSphericalCoordinate(_float3 _vAt)
{
	_float3 vPos = _vAt;

	vPos.x += m_fProjRadius * cos(m_vSphericalPos.z);
	vPos.y += m_vSphericalPos.x * cos(m_vSphericalPos.y);
	vPos.z += m_fProjRadius * sin(m_vSphericalPos.z);

	return vPos;
}

_float3 Client::CCameraSpherical::ComputeSphericalCoordinateLockOn(_float3 _vAt, _float3 _vTargetPosition)
{
    _float3 vPos = _vAt;
    _float3 vTargetPosition = _vTargetPosition;

	_float3 vBackWard = XMVector3Normalize(vPos - vTargetPosition);
	_float3 vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -vBackWard));
	_float3 vDir = XMVector3Normalize(vBackWard * 3.0f + vRight);

	vPos += m_vSphericalPos.x * vDir;
    m_vSphericalPos = ConvertSpherical(vDir);

	return vPos;
}

void Client::CCameraSpherical::CursorFixCenter()
{
    POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

    ClientToScreen(g_hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);
}

void Client::CCameraSpherical::KeyInput(float _fTimeDelta)
{
	if (m_isRecording)
    {
        /*
        if (GAMEINSTANCE->KeyDown(DIK_L))
        {    
            m_IsCurrentMouseLock = !m_IsCurrentMouseLock;

            if (m_IsCurrentMouseLock) {
                ShowCursor(false);
            }
            else {
                ShowCursor(true);
            }

            SwitchingMouseLock();

        }
        */
      

        long dz = GAMEINSTANCE->GetDIMouseMove(DIMS_Z);
        if (dz)
        {
            m_vSphericalPos.x += dz >= 0 ? -0.2f : 0.2f;
        }

        if (GAMEINSTANCE->MouseDown(DIM_MB))
        {
            m_vSphericalPos.x = 4.0f;
        }
    }
}

void Client::CCameraSpherical::MouseMove(float _fTimeDelta)
{
	if (m_isRecording && m_IsMouseLock)
	{
		if (!m_isLockOn)
		{
			long dx = GAMEINSTANCE->GetDIMouseMove(DIMS_X);
			long dy = GAMEINSTANCE->GetDIMouseMove(DIMS_Y);

			if (dy)
			{
				m_vSphericalPos.y -= (float)dy * m_fMouseSensitivity * _fTimeDelta;
			}
			if (dx)
			{
				m_vSphericalPos.z -= (float)dx * m_fMouseSensitivity * _fTimeDelta;
			}

		}
		CursorFixCenter();
	}
}

void Client::CCameraSpherical::CameraMove(float _fTimeDelta)
{
	if (m_pOwnerObject.lock())
	{
		if (m_isZoom)
		{
			_float3 vRightOffset = XMVector3Normalize(m_pTransformCom->GetState(CTransform::STATE_RIGHT)) * 0.45f;
			m_vCamAt = LinearIp(m_vCamAt, 
				m_pOwnerObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) + _float3{0.0f, 2.0f, 0.0f} + vRightOffset, 0.25f);
		}
		else if (m_pTargetObject.lock())
		{
			m_vCamAt = LinearIp(m_vCamAt, m_pOwnerObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) + m_vLockOnOffSetPos, m_fOwnerLerpRatio);
		}
		else
		{
			m_vCamAt = LinearIp(m_vCamAt, m_pOwnerObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) + m_vOffSetPos, m_fOwnerLerpRatio);
		}

		_float3 vCamPos;
		_float3 vLook;

		if (m_pTargetObject.lock())
		{
            m_vLockOnPosition = LinearIp(m_vLockOnPosition, m_pTargetObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) + m_pTargetObject.lock()->GetLockOnOffSetPosition(), 0.1f);

			vCamPos = ComputeSphericalCoordinateLockOn(m_vCamAt, m_vLockOnPosition);
			vLook = XMVector3Normalize(m_vLockOnPosition - vCamPos);
		}
		else
		{
			if (m_isLockOn)
			{
				vCamPos = ComputeSphericalCoordinate(m_vCamAt);
				vLook = XMVector3Normalize(m_vLockOnPosition - vCamPos);
				m_vSphericalPos = ConvertSpherical(-vLook);
				m_fLockOffTimeAcc += _fTimeDelta;
				if (m_fLockOffTimeAcc > 0.1f)
				{
					memset(&m_fLockOffTimeAcc, 0, sizeof(float));
					m_isLockOn = false;
				}
			}
			else
			{
				vCamPos = ComputeSphericalCoordinate(m_vCamAt);
				vLook = XMVector3Normalize(m_vCamAt - vCamPos);
			}
		}

		_float3 vRight = XMVector3Normalize(XMVector3Cross(_float3{ 0.0f,1.0f,0.0f }, vLook));
		_float3 vUp = XMVector3Cross(vLook, vRight);

		_float4x4 WorldMat;
		WorldMat.Right(vRight);
		WorldMat.Up(vUp);
		WorldMat.Forward(vLook);
		WorldMat.Translation(vCamPos);

		m_pTransformCom->SetWorldMatrix(WorldMat);
	}
}

void Client::CCameraSpherical::RefineCoordinate()
{
	if (m_vSphericalPos.x < 0.2f) { m_vSphericalPos.x = 0.2f; }
	else if (m_vSphericalPos.x > 10.0f) { m_vSphericalPos.x = 10.0f; }
	if (m_vSphericalPos.y < 0.1f) { m_vSphericalPos.y = 0.1f; }
	else if (m_vSphericalPos.y > XM_PI - 1.0f) { m_vSphericalPos.y = XM_PI - 1.0f; }
	if (m_vSphericalPos.z < 0.0f) { m_vSphericalPos.z += XM_2PI; }
	else if (m_vSphericalPos.z >= XM_2PI) { m_vSphericalPos.z -= XM_2PI; }
}

_float3 Client::CCameraSpherical::ConvertSpherical(_float3 _vPos)
{
	float fRadius = m_vSphericalPos.x;
	float fTheta = acos(_vPos.y);
    float fPi = atan2(_vPos.z, _vPos.x);
    if (fPi < 0)
        fPi += XM_2PI;

    return { fRadius, fTheta, fPi };
}

void Client::CCameraSpherical::ZoomTick()
{
	if (m_isZoom)
	{
		m_fFovy = LinearIp(m_fFovy, XM_PI / 5.0f, 0.1f);
		//m_vSphericalPos.x = LinearIp(m_vSphericalPos.x, 3.0f, 0.1f);
	}
	else
	{
		m_fFovy = LinearIp(m_fFovy, m_fDefaultFovy, 0.15f);
		//m_vSphericalPos.x = LinearIp(m_vSphericalPos.x, 4.0f, 0.1f);
	}
}
