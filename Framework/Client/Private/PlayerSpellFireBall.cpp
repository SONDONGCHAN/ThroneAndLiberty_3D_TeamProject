#include "PlayerSpellFireBall.h"
#include "Model.h"
#include "RigidBody.h"

CPlayerSpellFireBall::CPlayerSpellFireBall()
{
}

CPlayerSpellFireBall::~CPlayerSpellFireBall()
{
}

HRESULT CPlayerSpellFireBall::Initialize()
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 8;
	pArg->fRotationPerSec = XMConvertToRadians(90.0f);
	__super::Initialize(pArg);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;

    m_pTransformCom->SetScaling(_float3(20.f, 20.f, 1.f));
    AddRigidBody();
    DisableProjectile();

	return S_OK;
}

void CPlayerSpellFireBall::PriorityTick(_float _fTimeDelta)
{
}

void CPlayerSpellFireBall::Tick(_float _fTimeDelta)
{
    if (m_isContact)
        m_fExplosionLifeTime -= _fTimeDelta;
    else
    {
        GoToTargetPos(_fTimeDelta);
        m_fLifeTime -= _fTimeDelta;
    }

    if (!m_isContact && (m_fLifeTime <= 0.f))
        m_isContact = true;

    m_pTransformCom->Tick(_fTimeDelta);
}

void CPlayerSpellFireBall::LateTick(_float _fTimeDelta)
{
    if (m_isContact && (m_fExplosionLifeTime <= 0.f))
        m_isCollision = true;

    if (m_isContact && (m_fExplosionLifeTime == 0.1f) )
    {
        m_pRigidBody->DisableCollision("Player_Spell_FireBall");
        m_pRigidBody->EnableCollision("Player_Spell_FireBall_Explosion");
        // 합쳐지는 이펙트
        EFFECTMGR->PlayEffect("Fire_Burst", shared_from_this());
        GAMEINSTANCE->PlaySoundW("Player_Staff_Skill_1_End", 0.15f);
    }
    if(m_fLifeTime <= 0.f)
        m_isContact = true;

    if (m_isCollision)
    {
        DisableProjectile();
        return;
    }

#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
    
}

HRESULT CPlayerSpellFireBall::Render()
{
    //__super::Render();

	return S_OK;
}

HRESULT CPlayerSpellFireBall::RenderGlow()
{

    return S_OK;
}

void CPlayerSpellFireBall::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    if (_ColData.eColFlag == COL_MONSTER)
    {
        if (_szMyShapeTag == "Player_Spell_FireBall")
        {
            m_isContact = true;
        }
    }
}

void CPlayerSpellFireBall::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerSpellFireBall::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerSpellFireBall::EnableProjectile(_float3 _vMyStartPos, _float3 _vTargetPos, PlayerProjAbnormal _ePlayerProjAbnormal, _bool isDistinguish, _uint _iIndex)
{
    //m_eMyAbNormal = _ePlayerProjAbnormal;

    //_float3 vTargetDir = _vTargetPos - _vMyStartPos;
    //vTargetDir.Normalize();

    //m_pTransformCom->SetState(CTransform::STATE_POSITION, _vMyStartPos);
    //m_pTransformCom->LookAtDir(vTargetDir);
    //m_pRigidBody->EnableCollision("Player_Spell_FireBall");
    //m_pRigidBody->SimulationOn();
    //SetEnable(true);

    m_eMyAbNormal = _ePlayerProjAbnormal;
    
    _float3 vTargetDir = _vTargetPos - _vMyStartPos;
    vTargetDir.Normalize();
    _float vTargetDistance = SimpleMath::Vector3::Distance(_vMyStartPos, _vTargetPos);
    _float fDuringTime = vTargetDistance / 8.f;
    
    m_fDegreeSpeed = (1.f / fDuringTime) * 180.f;
      
    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vMyStartPos);
    m_pTransformCom->LookAtDir(vTargetDir);
    
    //_float fUpdir = GAMEINSTANCE->PickRandomFloat(-1.f, 1.f);
    _float fUpdir = (_iIndex * (72.f/360.f) * 4.f );

    m_pTransformCom->Turn(vTargetDir, fUpdir);

    EFFECTMGR->PlayEffect("Plr_FireBall", shared_from_this());
    
    m_fCurveScale = 0.5f + fDuringTime;
    m_pRigidBody->EnableCollision("Player_Spell_FireBall");
    m_pRigidBody->SimulationOn();
    SetEnable(true);
}

void CPlayerSpellFireBall::DisableProjectile()
{
    //리뉴얼추가
    m_fCurveScale = 1.f;
    m_fDegreeSpeed = 0.f;
    m_fPreDegree = 0.f;
    m_fDegree = 0.f;

    m_fExplosionLifeTime = 0.1f;
    m_isContact = false;
    m_fLifeTime = 2.5f;
    m_isCollision = false;
    m_pRigidBody->DisableCollision("Player_Spell_FireBall_Explosion");
    m_pRigidBody->SimulationOff();
    SetEnable(false);
}

void CPlayerSpellFireBall::GoToTargetPos(_float _fTimeDelta)
{
   // m_pTransformCom->GoStraight(_fTimeDelta);

    if (m_fDegree >= 180.f)
        m_isContact = true;

    m_fPreDegree = m_fDegree;
    m_pTransformCom->GoStraight(_fTimeDelta);
    m_fDegree += m_fDegreeSpeed * _fTimeDelta;
    
    _float DeltaDistance = sin(XMConvertToRadians(m_fDegree)) - sin(XMConvertToRadians(m_fPreDegree));
    DeltaDistance *= m_fCurveScale;
    m_pTransformCom->GoUpFreeDistance(DeltaDistance);

}

HRESULT CPlayerSpellFireBall::AddRigidBody()
{
    // 체크박스
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
    RectDesc.vSize = _float3{ 0.4f, 0.4f, 1.f };
    RectDesc.vOffSetPosition = { 0.0f, 0.0f, 0.0f };
    RectDesc.strShapeTag = "Player_Spell_FireBall";
    RigidBodyDesc.pGeometry = &RectDesc;
    
    RigidBodyDesc.eThisColFlag = COL_CHECKBOX;
    RigidBodyDesc.eTargetColFlag = COL_MONSTER;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
    
    m_pTransformCom->SetRigidBody(m_pRigidBody);

    // 폭발범위
    RigidBodyDesc.isInitCol = false;
    GeometrySphere SphereDesc;
    SphereDesc.fRadius = 1.f;
    SphereDesc.vOffSetPosition = { 0.0f, 0.f, 0.0f };
    SphereDesc.strShapeTag = "Player_Spell_FireBall_Explosion";
    RigidBodyDesc.pGeometry = &SphereDesc;
    
    RigidBodyDesc.eThisColFlag = COL_PLAYERPROJECTILE;
    RigidBodyDesc.eTargetColFlag = COL_MONSTER | COL_STATIC;
    m_pRigidBody->CreateShape(&RigidBodyDesc);

	return S_OK;
}

shared_ptr<CPlayerSpellFireBall> CPlayerSpellFireBall::Create()
{
	shared_ptr<CPlayerSpellFireBall> pInstance = make_shared<CPlayerSpellFireBall>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CPlayerSpellFireBall");

	return pInstance;
}
