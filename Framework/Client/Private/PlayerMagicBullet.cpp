#include "PlayerMagicBullet.h"
#include "Model.h"
#include "RigidBody.h"

CPlayerMagicBullet::CPlayerMagicBullet()
{
}

CPlayerMagicBullet::~CPlayerMagicBullet()
{
}

HRESULT CPlayerMagicBullet::Initialize(weak_ptr<CPlayer> _pPlayer)
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 15.f;
	pArg->fRotationPerSec = XMConvertToRadians(90.0f);
	__super::Initialize(pArg);

    m_pOwnerObject = _pPlayer;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;

    m_pTransformCom->SetScaling(_float3(1.f, 1.f, 1.f));
    AddRigidBody();
    DisableProjectile();

	return S_OK;
}

void CPlayerMagicBullet::PriorityTick(_float _fTimeDelta)
{
}

void CPlayerMagicBullet::Tick(_float _fTimeDelta)
{
    GoToTargetPos(_fTimeDelta);
    m_fLifeTime -= _fTimeDelta;

    m_pTransformCom->Tick(_fTimeDelta);
}

void CPlayerMagicBullet::LateTick(_float _fTimeDelta)
{
    if (m_isCollision || (m_fLifeTime <= 0.f) )
    {
        DisableProjectile();
        return;
    }

#ifdef _DEBUG
    if (m_pRigidBody)
        GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
}

HRESULT CPlayerMagicBullet::Render()
{
    __super::Render();

	return S_OK;
}

HRESULT CPlayerMagicBullet::RenderGlow()
{
    return S_OK;
}

void CPlayerMagicBullet::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    if (_ColData.eColFlag == COL_MONSTER)
    {
        m_pOwnerObject.lock()->AddSupremeGauge(5.f);
        m_isCollision = true;
        GAMEINSTANCE->PlaySoundW("Player_Staff_Hit", 1.f);
    }
}

void CPlayerMagicBullet::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerMagicBullet::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerMagicBullet::EnableProjectile(_float3 _vMyStartPos, _float3 _vTargetPos, PlayerProjAbnormal _ePlayerProjAbnormal, _bool isDistinguish, _uint _iIndex)
{
    m_eMyAbNormal = _ePlayerProjAbnormal;

    _float3 vTargetDir = _vTargetPos - _vMyStartPos;
    vTargetDir.Normalize();
    _float vTargetDistance = SimpleMath::Vector3::Distance(_vMyStartPos, _vTargetPos);
    _float fDuringTime = vTargetDistance / 15.f;
    
    m_eCurveType = (Curve_Type)GAMEINSTANCE->PickRandomInt(0, 2);

    if (CURVE_SIN360 == (Curve_Type)m_eCurveType)
    {
        m_fDegreeSpeed = (1.f / fDuringTime) * 360.f;
    }
    else  if (CURVE_SIN180 == (Curve_Type)m_eCurveType)
    {
        m_fDegreeSpeed = (1.f / fDuringTime) * 180.f;
    }

    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vMyStartPos);
    m_pTransformCom->LookAtDir(vTargetDir);

    _float fUpdir = GAMEINSTANCE->PickRandomFloat(-1.f, 1.f);
    m_pTransformCom->Turn(vTargetDir, fUpdir);

    m_fCurveScale = GAMEINSTANCE->PickRandomFloat(0.5f, (0.7f + fDuringTime) );

    EFFECTMGR->PlayEffect("Plr_MagicBullet", shared_from_this());

    m_pRigidBody->SimulationOn();
    SetEnable(true);
}

void CPlayerMagicBullet::DisableProjectile()
{
    m_eCurveType = CURVE_SIN360;
    m_fCurveScale = 1.f;
    m_fDegreeSpeed = 0.f;
    m_fPreDegree = 0.f;
    m_fDegree = 0.f;
    m_fLifeTime = 2.f;
    m_isCollision = false;
    m_pRigidBody->SimulationOff();
    SetEnable(false);
}

void CPlayerMagicBullet::GoToTargetPos(_float _fTimeDelta)
{
    if (CURVE_SIN360 == (Curve_Type)m_eCurveType)
    {
        if (m_fDegree >= 360.f)
            m_isCollision = true;
    }
    else  if (CURVE_SIN180 == (Curve_Type)m_eCurveType)
    {
        if (m_fDegree >= 180.f)
            m_isCollision = true;
    }

    m_fPreDegree = m_fDegree;
    m_pTransformCom->GoStraight(_fTimeDelta);
    m_fDegree += m_fDegreeSpeed * _fTimeDelta;

    _float DeltaDistance = sin(XMConvertToRadians(m_fDegree) ) - sin(XMConvertToRadians(m_fPreDegree));
    DeltaDistance *= m_fCurveScale;
    m_pTransformCom->GoUpFreeDistance(DeltaDistance);
}

HRESULT CPlayerMagicBullet::AddRigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
    
    RigidBodyDesc.isStatic = false;
    RigidBodyDesc.isTrigger = true;
    RigidBodyDesc.isGravity = false;
    RigidBodyDesc.isInitCol = true;
    RigidBodyDesc.pTransform = m_pTransformCom;
    RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;
    
    RigidBodyDesc.pOwnerObject = shared_from_this();
    RigidBodyDesc.fStaticFriction = 1.0f;
    RigidBodyDesc.fDynamicFriction = 0.0f;
    RigidBodyDesc.fRestitution = 0.0f;
    
    GeometryBox RectDesc;
    RectDesc.vSize = _float3{ 0.2f, 0.2f, 0.2f };
    RectDesc.vOffSetPosition = { 0.0f, 0.0f, 0.0f };
    RectDesc.strShapeTag = "Player_Magic_Bullet";
    RigidBodyDesc.pGeometry = &RectDesc;
    
    RigidBodyDesc.eThisColFlag = COL_PLAYERPROJECTILE;
    RigidBodyDesc.eTargetColFlag = COL_MONSTER;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
    
    m_pTransformCom->SetRigidBody(m_pRigidBody);
	return S_OK;
}

shared_ptr<CPlayerMagicBullet> CPlayerMagicBullet::Create(weak_ptr<CPlayer> _pPlayer)
{
	shared_ptr<CPlayerMagicBullet> pInstance = make_shared<CPlayerMagicBullet>();

	if (FAILED(pInstance->Initialize(_pPlayer)))
		MSG_BOX("Failed to Create : CPlayerMagicBullet");

	return pInstance;
}
