#include "Transform.h"
#include "RigidBody.h"

CTransform::CTransform(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :CComponent(_pDevice, _pContext)
{
}

shared_ptr<CTransform> CTransform::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, TRANSFORM_DESC* _pDesc)
{
    shared_ptr<CTransform> pInstance = make_shared<CTransform>(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pDesc)))
        MSG_BOX("Failed to Create : CTransform");

    return pInstance;
}

HRESULT CTransform::Initialize(TRANSFORM_DESC* _pDesc)
{
    m_WorldMatrix = SimpleMath::Matrix::Identity;

    if (_pDesc)
    {
		m_fSpeedPerSec      = _pDesc->fSpeedPerSec;
		m_fRotationPerSec   = _pDesc->fRotationPerSec;
    }

    return S_OK;
}

_float3 CTransform::GetState(STATE _eState)
{
    switch (_eState)
    {
    case STATE_RIGHT:
    {
        return m_WorldMatrix.Right();
    }
    break;
    case STATE_UP:
    {
        return m_WorldMatrix.Up();
    }
    break;
    case STATE_LOOK:
    {
        return m_WorldMatrix.Forward();
    }
    break;
    case STATE_POSITION:
    {
        return m_WorldMatrix.Translation();
    }
    break;
    default:
    {
        return _float3(-1.0f, -1.0f, -1.0f);
    }
    break;
    }
}

_float3 CTransform::GetScaled()
{
   return _float3(m_WorldMatrix.Right().Length(), m_WorldMatrix.Up().Length(), m_WorldMatrix.Forward().Length());
}

_quaternion CTransform::GetQuat()
{
    return XMQuaternionRotationMatrix(GetRotationMatrix());
}

_float4x4 CTransform::GetWorldMatrix()
{
    return m_WorldMatrix;
}

_float4x4 CTransform::GetWorldMatrixInverse()
{
    return m_WorldMatrix.Invert();
}

_float4x4* CTransform::GetWorldMatrixPtr()
{
    return &m_WorldMatrix;
}

_float4x4 CTransform::GetRotationMatrix()
{
    _float4x4 MatQuat;
    MatQuat.Right(XMVector3Normalize(m_WorldMatrix.Right()));
    MatQuat.Up(XMVector3Normalize(m_WorldMatrix.Up()));
    MatQuat.Forward(XMVector3Normalize(m_WorldMatrix.Forward()));

    return MatQuat;
}

void CTransform::SetScaling(_float3 _vScale)
{
    SetScaling(_vScale.x, _vScale.y, _vScale.z);
}

void CTransform::SetScaling(_float _fX, _float _fY, _float _fZ)
{
    _float3 vNormalizedRight;
    m_WorldMatrix.Right().Normalize(vNormalizedRight);
    m_WorldMatrix.Right(vNormalizedRight * _fX);

    _float3 vNormalizedUp;
    m_WorldMatrix.Up().Normalize(vNormalizedUp);
    m_WorldMatrix.Up(vNormalizedUp * _fY);

    _float3 vNormalizedForward;
    m_WorldMatrix.Forward().Normalize(vNormalizedForward);
    m_WorldMatrix.Forward(vNormalizedForward * _fZ);
}

void CTransform::SetState(STATE _eState, _float3 _vState)
{
    switch (_eState)
    {
    case STATE_RIGHT:
    {
        m_WorldMatrix.Right(_vState);
    }
    break;
    case STATE_UP:
    {
        m_WorldMatrix.Up(_vState);
    }
    break;
    case STATE_LOOK:
    {
        m_WorldMatrix.Forward(_vState);
    }
    break;
    case STATE_POSITION:
    {
        m_WorldMatrix.Translation(_vState);
        if(m_pRigidBody.lock())
			m_Movement_States |= MS_POSITION;
    }
    break;
    }
}              

void CTransform::SetQuaternion(const _quaternion& _vQuaternion)
{
	_float3 vScale = GetScaled();
	_float4x4 MatScale = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
	_float4x4 QuaternionMatrix = XMMatrixRotationQuaternion(_vQuaternion);
	_float4x4 MatRot = MatScale * QuaternionMatrix;

	m_WorldMatrix.Right(MatRot.Right());
	m_WorldMatrix.Up(MatRot.Up());
	m_WorldMatrix.Forward(MatRot.Forward());
}

void CTransform::PlusQuaternion(const _quaternion& _vQuaternion)
{    
    _quaternion existingRotation = SimpleMath::Quaternion::CreateFromRotationMatrix(m_WorldMatrix);

    _quaternion newRotation = _vQuaternion * existingRotation;

    _float4x4 rotationMatrix = SimpleMath::Matrix::CreateFromQuaternion(newRotation);

    _float3 scale = GetScaled();
    _float4x4 scaleMatrix = SimpleMath::Matrix::CreateScale(scale.x, scale.y, scale.z);
    _float4x4 transformMatrix = scaleMatrix * rotationMatrix;

    m_WorldMatrix.Right(transformMatrix.Right());
    m_WorldMatrix.Up(transformMatrix.Up());
    m_WorldMatrix.Forward(transformMatrix.Forward());
}

void CTransform::SetWorldMatrix(_float4x4 _worldMat)
{
	m_WorldMatrix = _worldMat;
}

void CTransform::SetSpeed(_float _fSpeed)
{
    m_fSpeedPerSec = _fSpeed;
}

void CTransform::SetTurnSpeed(_float _fTurnSpeed)
{
    m_fRotationPerSec = _fTurnSpeed;
}

void CTransform::SetUpDir(_float3 _vUpDir)
{
    _float3 vScaled = GetScaled();
    
    _float3 vUp = _vUpDir;
    _float3 vLook = m_WorldMatrix.Forward();
    _float3 vRight = vUp.Cross(vLook);
    
    vRight.Normalize();
    vUp.Normalize();
    vLook.Normalize();
    
    SetState(STATE_RIGHT, vRight * vScaled.x);
    SetState(STATE_UP, vUp * vScaled.y);
    SetState(STATE_LOOK, vLook * vScaled.z);
    
    if (m_pRigidBody.lock())
    {
        m_Movement_States |= MS_ROTATION;
    }
}

void CTransform::Turn(_float3 _vAxis, _float _fTimeDelta)
{
    _float4x4 RotationMatrix = SimpleMath::Matrix::CreateFromAxisAngle(_vAxis, m_fRotationPerSec * _fTimeDelta);

    for (UINT i = STATE_RIGHT; i < STATE_POSITION; i++) {

        _float3 vStateDir = GetState(STATE(i));
        SetState(STATE(i), vStateDir.TransformNormal(vStateDir, RotationMatrix));
    }

    if (m_pRigidBody.lock())
    {
        m_Movement_States |= MS_ROTATION;
    }
}

void CTransform::TurnToDir(_float3 _vDir, _float _fTimeDelta)
{

    _float2 vDirXZ = _float2(_vDir.x, _vDir.z);
    _float2 vLookXZ = _float2(m_WorldMatrix.Forward().x, m_WorldMatrix.Forward().z);

    // 적이 오른쪽
    if ((vDirXZ.Cross(vLookXZ)).y > 1.f)
    {
        Turn(_float3(0.f, 1.f, 0.f), _fTimeDelta);
    }
    // 적이 왼쪽
    if ((vDirXZ.Cross(vLookXZ)).y < -1.f)
    {
        Turn(_float3(0.f, 1.f, 0.f), -_fTimeDelta);
    }
    else
    {

    }

    if (m_pRigidBody.lock())
    {
        m_Movement_States |= MS_ROTATION;
    }
}

void CTransform::Rotation(_float3 _vAxis, _float _fRadian)
{
	_float4x4 RotationMatrix = SimpleMath::Matrix::CreateFromAxisAngle(_vAxis, _fRadian);
	_float3 vScaled = GetScaled();

	for (UINT i = STATE_RIGHT; i < STATE_POSITION; i++) {

		_float3 vTemp = _float3(0.f, 0.f, 0.f);
		*((_float*)&vTemp + i) = 1.f * *((_float*)&vScaled + i);
		SetState(STATE(i), vTemp.TransformNormal(vTemp, RotationMatrix));
	}

    if (m_pRigidBody.lock())
    {
        m_Movement_States |= MS_ROTATION;
    }
}

void CTransform::RotationAll(_float _fRadianX, _float _fRadianY, _float _fRadianZ)
{
    // 각 축을 중심으로 회전하는 쿼터니언 생성
    DirectX::SimpleMath::Quaternion rotationX = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(1.f, 0.f, 0.f), _fRadianX);
    DirectX::SimpleMath::Quaternion rotationY = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0.f, 1.f, 0.f), _fRadianY);
    DirectX::SimpleMath::Quaternion rotationZ = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0.f, 0.f, 1.f), _fRadianZ);

    // 세 개의 회전을 결합하여 총 회전 쿼터니언 생성
    DirectX::SimpleMath::Quaternion totalRotation = rotationX * rotationY * rotationZ;

    // 현재 크기를 가져옴
    _float3 scaled = GetScaled();

    // 회전된 각도를 저장할 변수
    _float3 rotatedAngles;

    // 회전 행렬을 쿼터니언으로 변환
    DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(totalRotation);

    // 회전 행렬을 이용하여 각 상태를 변환
    for (UINT i = STATE_RIGHT; i < STATE_POSITION; i++) {
        _float3 vTemp = _float3(0.f, 0.f, 0.f);
        *((_float*)&vTemp + i) = 1.f * *((_float*)&scaled + i);
        DirectX::SimpleMath::Vector3 transformed = DirectX::SimpleMath::Vector3::TransformNormal(vTemp, rotationMatrix);
        SetState(STATE(i), transformed);
    }

    if (m_pRigidBody.lock())
    {
        m_Movement_States |= MS_ROTATION;
    }
}

void CTransform::RotationPlus(_float3 _vAxis, _float _fRadian)
{
	_float4x4 RotationMatrix = SimpleMath::Matrix::CreateFromAxisAngle(_vAxis, _fRadian);

	for (UINT i = STATE_RIGHT; i < STATE_POSITION; i++)
	{
		_float3		vStateDir = GetState(STATE(i));
		SetState(STATE(i), vStateDir.TransformNormal(vStateDir, RotationMatrix));
	}

    if (m_pRigidBody.lock())
    {
        m_Movement_States |= MS_ROTATION;
    }
}

void CTransform::RotationPlusScale(_float3 _vAxis, _float _fRadian)
{
    _float4x4 RotationMatrix = SimpleMath::Matrix::CreateFromAxisAngle(_vAxis, _fRadian);

    for (UINT i = STATE_RIGHT; i < STATE_POSITION; i++)
    {
        _float3 vStateDir = GetState(STATE(i));
        // 스케일을 적용한 후 회전 행렬을 사용하여 회전시킵니다.
        vStateDir *= GetScaled(); // 스케일 적용
        vStateDir = DirectX::XMVector3TransformNormal(vStateDir, RotationMatrix); // 회전
        SetState(STATE(i), vStateDir);
    }
}

void CTransform::MoveTo(_float3 _vPoint, _float _fLimit, _float _fTimeDelta)
{
    _float3 vPosition = GetState(STATE_POSITION);
    _float3 vDir = _vPoint - vPosition;
   
    _float fDistance = vDir.Length();
    vDir.Normalize();

    if (fDistance > _fLimit)
        vPosition += vDir * m_fSpeedPerSec * _fTimeDelta;

    SetState(STATE_POSITION, vPosition);

}

void CTransform::LookAt(_float3 _vPoint)
{
    _float3 vScaled = GetScaled();

    _float3 vPosition = GetState(CTransform::STATE_POSITION);

    _float3 vLook = _vPoint - vPosition;

    _float3 vRight = _float3(0.f, 1.f, 0.f).Cross(vLook);
    _float3 vUp = vLook.Cross(vRight);

    vRight.Normalize();
    vUp.Normalize();
    vLook.Normalize();

    SetState(STATE_RIGHT, vRight * vScaled.x);
    SetState(STATE_UP, vUp * vScaled.y);
    SetState(STATE_LOOK, vLook * vScaled.z);

    //_float3 vScaled = GetScaled();
    //
    //_float3 vPosition = GetState(CTransform::STATE_POSITION);
    //
    //_float3 vLook = _vPoint - vPosition;
    //_float3 vRight = XMVector3Cross({ 0.f, 1.f, 0.f, 0.f }, vLook);
    //_float3 vUp = XMVector3Cross(vLook, vRight);
    //
    //SetState(STATE_RIGHT, XMVector3Normalize(vRight)* vScaled.x);
    //SetState(STATE_UP, XMVector3Normalize(vUp)* vScaled.y);
    //SetState(STATE_LOOK, XMVector3Normalize(vLook)* vScaled.z);
    if (m_pRigidBody.lock())
    {
		m_Movement_States |= MS_ROTATION;
    }
}

void CTransform::LookAtDir(_float3 _vDir)
{
    _float3 vScaled = GetScaled();
   
    _float3 vLook = _vDir;
    
    _float3 vRight = _float3(0.f, 1.f, 0.f).Cross(vLook);
    _float3 vUp = vLook.Cross(vRight);
    
    vRight.Normalize();
    vUp.Normalize();
    vLook.Normalize();
    
    SetState(STATE_RIGHT, vRight * vScaled.x);
    SetState(STATE_UP, vUp * vScaled.y);
    SetState(STATE_LOOK, vLook * vScaled.z);

    if (m_pRigidBody.lock())
    {
        m_Movement_States |= MS_ROTATION;
    }
}

void CTransform::LookAtForLandObject(_float3 _vPoint)
{
    _float3 vScaled = GetScaled();
    _float3 vPosition = GetState(CTransform::STATE_POSITION);
    _float3	vLook = _vPoint - vPosition;
    _float3 vRight = _float3(0.f, 1.f, 0.f).Cross(vLook);

    vLook = vRight.Cross(_float3(0.f, 1.f, 0.f));

    vLook.Normalize();
    vRight.Normalize();

    SetState(STATE_RIGHT, vRight * vScaled.x);
    SetState(STATE_LOOK, vLook * vScaled.z);

    if (m_pRigidBody.lock())
    {
        m_Movement_States |= MS_ROTATION;
    }
}

void CTransform::LookAtDirForLandObject(_float3 _vDir)
{
    _float3 vScaled = GetScaled();
    _float3	vLook = _vDir;
    _float3 vRight = _float3(0.f, 1.f, 0.f).Cross(vLook);

    vLook = vRight.Cross(_float3(0.f, 1.f, 0.f));

    vLook.Normalize();
    vRight.Normalize();

    SetState(STATE_RIGHT, vRight * vScaled.x);
    SetState(STATE_LOOK, vLook * vScaled.z);

    if (m_pRigidBody.lock())
    {
        m_Movement_States |= MS_ROTATION;
    }
}

void CTransform::SetPositionParam(_int _iType, _float _fParam)
{
    _float3		vPosition = GetState(STATE_POSITION);
    switch (_iType)
    {
    case 0:
        vPosition.x = _fParam;
        SetState(STATE_POSITION, vPosition);
        break;
    case 1:
        vPosition.y = _fParam;
        SetState(STATE_POSITION, vPosition);
        break;
    case 2:
        vPosition.z = _fParam;
        SetState(STATE_POSITION, vPosition);
        break;
    }
}

void CTransform::GoStraight(_float _fTimeDelta)
{
	_float3 vForward  = m_WorldMatrix.Forward(); vForward.Normalize();

	_float3 vDeltaPos = vForward * m_fSpeedPerSec;
	AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoStraightLeft(_float _fTimeDelta)
{
    _float3 vForward     = m_WorldMatrix.Forward(); vForward.Normalize();
    _float3 vLeft        = m_WorldMatrix.Left();    vLeft.Normalize();
    _float3 vForwardLeft = vForward + vLeft;        vForwardLeft.Normalize();

    _float3 vDeltaPos    = vForwardLeft * m_fSpeedPerSec;
	AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoStraightRight(_float _fTimeDelta)
{
    _float3 vForward      = m_WorldMatrix.Forward(); vForward.Normalize();
    _float3 vRight        = m_WorldMatrix.Right();   vRight.Normalize();
	_float3 vForwardRight = vForward + vRight;       vForwardRight.Normalize();

    _float3 vDeltaPos     = vForwardRight * m_fSpeedPerSec;
    AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoBackward(_float _fTimeDelta)
{
	_float3 vBackward = m_WorldMatrix.Backward(); vBackward.Normalize();

	_float3 vDeltaPos = vBackward * m_fSpeedPerSec;
	AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoBackwardLeft(_float _fTimeDelta)
{
	_float3 vBackward     = m_WorldMatrix.Backward(); vBackward.Normalize();
	_float3 vLeft         = m_WorldMatrix.Left();     vLeft.Normalize();
	_float3 vBackwardLeft = vBackward + vLeft;        vBackwardLeft.Normalize();

	_float3 vDeltaPos     = vBackwardLeft * m_fSpeedPerSec;
    AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoBackwardRight(_float _fTimeDelta)
{
    _float3 vBackward      = m_WorldMatrix.Backward(); vBackward.Normalize();
    _float3 vRight         = m_WorldMatrix.Right();    vRight.Normalize();
    _float3 vBackwardRight = vBackward + vRight;       vBackwardRight.Normalize();

    _float3 vDeltaPos      = vBackwardRight * m_fSpeedPerSec;
    AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoLeft(_float _fTimeDelta)
{
	_float3	vLeft     = m_WorldMatrix.Left(); vLeft.Normalize();

	_float3 vDeltaPos = vLeft * m_fSpeedPerSec;
    AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoRight(_float _fTimeDelta)
{
	_float3 vRight    = m_WorldMatrix.Right(); vRight.Normalize();

	_float3 vDeltaPos = vRight * m_fSpeedPerSec;
	AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoStraightFreeSpeed(_float _fTimeDelta, _float _Speed)
{
    _float3 vForward = m_WorldMatrix.Forward(); vForward.Normalize();

    _float3 vDeltaPos = vForward * _Speed;
	AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoUpFreeDistance(_float Distance)
{
    _float3	vUp = m_WorldMatrix.Up(); vUp.Normalize();

    _float3 vDeltaPos = vUp * Distance;
    AddPosition(vDeltaPos, 1.f);
}

void CTransform::GoDStraight(_float _fTimeDelta)
{
	_float3	vForward  = { 0.0f,0.0f,1.0f };

	_float3 vDeltaPos = vForward * m_fSpeedPerSec;
	AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoDBackward(_float _fTimeDelta)
{
	_float3 vBackward = { 0.0f,0.0f,-1.0f };

	_float3 vDeltaPos = vBackward * m_fSpeedPerSec;
	AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoDLeft(_float _fTimeDelta)
{
	_float3 vLeft     = { -1.0f,0.0f,0.0f };

    _float3 vDeltaPos = vLeft * m_fSpeedPerSec;
    AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoDRight(_float _fTimeDelta)
{
	_float3	vRight    = { 1.0f,0.0f,0.0f };

    _float3 vDeltaPos = vRight * m_fSpeedPerSec;
    AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoDDown(_float _fTimeDelta)
{
    _float3	vDown = { 0.0f,-1.0f,0.0f };

    _float3 vDeltaPos = vDown * m_fSpeedPerSec;
    AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoDDownFreeSpeed(_float _fTimeDelta, _float _Speed)
{
    _float3	vDown = { 0.0f,-1.0f,0.0f };
    
    _float3 vDeltaPos = vDown * _Speed;
    AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoDir(_float _fTimeDelta, _float3 _vDir)
{
	_float3	vDirection = _vDir; vDirection.Normalize();
    
	_float3 vDeltaPos = vDirection * m_fSpeedPerSec;
    AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoDirFreeSpeed(_float _fTimeDelta, _float3 _vDir, _float _Speed)
{
    _float3	vDirection = _vDir; vDirection.Normalize();
    
    _float3 vDeltaPos = vDirection * _Speed;
    AddPosition(vDeltaPos, _fTimeDelta);
}

void CTransform::GoDirNotNormalize(_float _fTimeDelta, _float3 _vDir, _float _Speed)
{
    _float3	vDirection = _vDir; 
    
    _float3 vDeltaPos = vDirection * _Speed;
    AddPosition(vDeltaPos, _fTimeDelta);
}

HRESULT CTransform::SetRigidBody(shared_ptr<CRigidBody> _pRigidBody)
{
	if (!_pRigidBody)
    {
		MessageBox(nullptr, L"RigidBody nullptr", L"CTransform::SetRigidBody", MB_OK);
        return E_FAIL;
    }

	m_pRigidBody = _pRigidBody;
	return S_OK;
}

void CTransform::SetMaxLiearVelocity(float _fMaxLinVel)
{
    auto pRigidBody = m_pRigidBody.lock();
    if (pRigidBody)
    {
		pRigidBody->SetMaxLinearVelocity(_fMaxLinVel);
    }
}

void CTransform::Jump(_float _fJumpForce)
{
    auto pRigidBody = m_pRigidBody.lock();
    if (pRigidBody)
    {
		pRigidBody->AddForce(ToPxVec3({ 0.0f, _fJumpForce, 0.0f }));
    }
}

void CTransform::AddForce(const _float3& _vForce, PxForceMode::Enum _eMode, bool _isAutowake)
{
    auto pRigidBody = m_pRigidBody.lock();
    if (pRigidBody)
    {
		pRigidBody->AddForce(ToPxVec3(_vForce), _eMode, _isAutowake);
    }
}

void CTransform::Tick(_float _fTimeDelta)
{
    auto pRigidBody = m_pRigidBody.lock();
    if (pRigidBody)
    {
		if (m_Movement_States & MS_POSITION || m_Movement_States & MS_ROTATION)
        {
			pRigidBody->SetGlobalPos(m_WorldMatrix.Translation(), GetQuat());
		}

        m_Velocity.y = BtwA2B(pRigidBody->GetLinearVelocity().y, -20.0f, 20.0f);
        pRigidBody->SetLinearVelocity(ToPxVec3(m_Velocity));
        pRigidBody->SetAngularVelocity({ 0.0f,0.0f,0.0f });

        m_Movement_States = MS_NONE;
	}
}

void CTransform::LateTick(_float _fTimeDelta)
{
    auto pRigidBody = m_pRigidBody.lock();
    if (pRigidBody)
    {
		m_WorldMatrix.Translation(pRigidBody->GetPosition());
    }
}

void CTransform::LateTickFragment(_float _fTimeDelta)
{
    auto pRigidBody = m_pRigidBody.lock();
    if (pRigidBody)
    {
        m_WorldMatrix.Translation(pRigidBody->GetPosition());
        SetQuaternion(pRigidBody->GetQuat());
    }
}

void CTransform::AddPosition(const _float3& _vDeltaPos, _float _fTimeDelta)
{
    m_WorldMatrix.Translation(m_WorldMatrix.Translation() + _vDeltaPos * _fTimeDelta);

    if (m_pRigidBody.lock())
    {
        m_Movement_States |= MS_POSITION;
    }
}

