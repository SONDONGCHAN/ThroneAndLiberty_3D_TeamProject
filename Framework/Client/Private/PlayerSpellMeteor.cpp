#include "PlayerSpellMeteor.h"
#include "Model.h"
#include "RigidBody.h"

CPlayerSpellMeteor::CPlayerSpellMeteor()
{
}

CPlayerSpellMeteor::~CPlayerSpellMeteor()
{
}

HRESULT CPlayerSpellMeteor::Initialize()
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 21.f;
	pArg->fRotationPerSec = XMConvertToRadians(90.0f);
	__super::Initialize(pArg);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;

    m_pTransformCom->SetScaling(_float3(40.f, 100.f, 2.f));
    AddRigidBody();
    DisableProjectile();

    m_fDropTime = 1.f / 3.f;
	return S_OK;
}

void CPlayerSpellMeteor::PriorityTick(_float _fTimeDelta)
{
}

void CPlayerSpellMeteor::Tick(_float _fTimeDelta)
{
    if (m_isCrash)
    {
        m_fLifeTime -= _fTimeDelta;
    }
    else
        GoToTargetPos(_fTimeDelta);

    m_pTransformCom->Tick(_fTimeDelta);
}

void CPlayerSpellMeteor::LateTick(_float _fTimeDelta)
{
    if ((m_fLifeTime <= 0.f) )
    {
        DisableProjectile();
        return;
    }

#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
    
}

HRESULT CPlayerSpellMeteor::Render()
{
    __super::Render();

	return S_OK;
}

HRESULT CPlayerSpellMeteor::RenderGlow()
{
    return S_OK;
}

void CPlayerSpellMeteor::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerSpellMeteor::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerSpellMeteor::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerSpellMeteor::EnableProjectile(_float3 _vMyStartPos, _float3 _vTargetPos, PlayerProjAbnormal _ePlayerProjAbnormal, _bool isDistinguish, _uint _iIndex)
{
    m_eMyAbNormal = _ePlayerProjAbnormal;
    
    _float3 vTargetDir = _vTargetPos - _vMyStartPos;
    vTargetDir.y = 0.f;
    vTargetDir.Normalize();

    _float3 vStartPos = _vTargetPos;
    vStartPos.y += 7.f;
    m_pTransformCom->SetState(CTransform::STATE_POSITION, vStartPos);
    m_pTransformCom->LookAtDir(vTargetDir);

    // ¶³¾îÁö´Â ÀÌÆåÆ®
    EFFECTMGR->PlayEffect("Plr_Meteo_Ball", shared_from_this());

    m_pRigidBody->SimulationOn();
    SetEnable(true);
}

void CPlayerSpellMeteor::DisableProjectile()
{
    m_fDropTime = 1.f / 3.f;
    m_isCrash = false;
    m_fLifeTime = 0.2f;
    m_pRigidBody->DisableCollision("Player_Spell_Meteor");
    m_pRigidBody->SimulationOff();
    SetEnable(false);
}

void CPlayerSpellMeteor::GoToTargetPos(_float _fTimeDelta)
{
    m_pTransformCom->GoDDown(_fTimeDelta);
    m_fDropTime -= _fTimeDelta;

    if (m_fDropTime <= 0.f)
    {
        m_pRigidBody->EnableCollision("Player_Spell_Meteor");
        m_isCrash = true;
        EFFECTMGR->PlayEffect("Plr_Meteo_Burst", shared_from_this());
        EFFECTMGR->PlayAnimEffect("Plr_Meteor_Burst_Anim", shared_from_this());
        GAMEINSTANCE->PlaySoundW("Player_Staff_Skill_5_End", 0.7f);
    }
}

HRESULT CPlayerSpellMeteor::AddRigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
    
    RigidBodyDesc.isStatic = false;
    RigidBodyDesc.isTrigger = true;
    RigidBodyDesc.isGravity = false;
    RigidBodyDesc.isInitCol = false;
    RigidBodyDesc.pTransform = m_pTransformCom;
    RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;
    
    RigidBodyDesc.pOwnerObject = shared_from_this();
    RigidBodyDesc.fStaticFriction = 1.0f;
    RigidBodyDesc.fDynamicFriction = 0.0f;
    RigidBodyDesc.fRestitution = 0.0f;
    
    GeometryBox RectDesc;
    RectDesc.vSize = _float3{ 6.f, 5.f, 6.f };
    RectDesc.vOffSetPosition = { 0.f, 2.f, 0.f };
    RectDesc.strShapeTag = "Player_Spell_Meteor";
    RigidBodyDesc.pGeometry = &RectDesc;
    
    RigidBodyDesc.eThisColFlag = COL_PLAYERPROJECTILE;
    RigidBodyDesc.eTargetColFlag = COL_MONSTER | COL_STATIC;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
    
    m_pTransformCom->SetRigidBody(m_pRigidBody);
	return S_OK;
}

shared_ptr<CPlayerSpellMeteor> CPlayerSpellMeteor::Create()
{
	shared_ptr<CPlayerSpellMeteor> pInstance = make_shared<CPlayerSpellMeteor>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CPlayerSpellMeteor");

	return pInstance;
}
