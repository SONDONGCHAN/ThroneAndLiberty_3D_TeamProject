#include "PlayerSpellIceSpear.h"
#include "Model.h"
#include "RigidBody.h"

CPlayerSpellIceSpear::CPlayerSpellIceSpear()
{
}

CPlayerSpellIceSpear::~CPlayerSpellIceSpear()
{
}

HRESULT CPlayerSpellIceSpear::Initialize()
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 15.f;
	pArg->fRotationPerSec = XMConvertToRadians(90.0f);
	__super::Initialize(pArg);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;

    m_pTransformCom->SetScaling(_float3(10.f, 10.f, 1.f));
    AddRigidBody();
    DisableProjectile();

	return S_OK;
}

void CPlayerSpellIceSpear::PriorityTick(_float _fTimeDelta)
{
}

void CPlayerSpellIceSpear::Tick(_float _fTimeDelta)
{
    GoToTargetPos(_fTimeDelta);
    m_fLifeTime -= _fTimeDelta;

    m_pTransformCom->Tick(_fTimeDelta);
}

void CPlayerSpellIceSpear::LateTick(_float _fTimeDelta)
{
    if ( (m_fLifeTime <= 0.f) )
    {
        DisableProjectile();
        return;
    }

#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
}

HRESULT CPlayerSpellIceSpear::Render()
{
    __super::Render();

	return S_OK;
}

HRESULT CPlayerSpellIceSpear::RenderGlow()
{
    return S_OK;
}

void CPlayerSpellIceSpear::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    if (_ColData.eColFlag == COL_MONSTER)
    {
        GAMEINSTANCE->PlaySoundW("Player_Staff_Skill_2_Hit", 0.4f);
    }
}

void CPlayerSpellIceSpear::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerSpellIceSpear::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerSpellIceSpear::EnableProjectile(_float3 _vMyStartPos, _float3 _vTargetPos, PlayerProjAbnormal _ePlayerProjAbnormal, _bool isDistinguish, _uint _iIndex)
{
    m_eMyAbNormal = _ePlayerProjAbnormal;

    _float3 vTargetDir = _vTargetPos - _vMyStartPos;
    vTargetDir.Normalize();

    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vMyStartPos);
    m_pTransformCom->LookAtDir(vTargetDir);

    EFFECTMGR->PlayEffect("Plr_IceSpear", shared_from_this());

    m_pRigidBody->SimulationOn();
    SetEnable(true);
}

void CPlayerSpellIceSpear::DisableProjectile()
{
    m_fLifeTime = 1.f;
    m_pRigidBody->SimulationOff();
    SetEnable(false);
}

void CPlayerSpellIceSpear::GoToTargetPos(_float _fTimeDelta)
{
    m_pTransformCom->GoStraight(_fTimeDelta);
}

HRESULT CPlayerSpellIceSpear::AddRigidBody()
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
    RectDesc.vSize = _float3{ 0.3f, 0.3f, 1.f };
    RectDesc.vOffSetPosition = { 0.0f, 0.0f, 0.0f };
    RectDesc.strShapeTag = "Player_Spell_IceSpear";
    RigidBodyDesc.pGeometry = &RectDesc;
    
    RigidBodyDesc.eThisColFlag = COL_PLAYERPROJECTILE;
    RigidBodyDesc.eTargetColFlag = COL_MONSTER | COL_STATIC;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
    
    m_pTransformCom->SetRigidBody(m_pRigidBody);
	return S_OK;
}

shared_ptr<CPlayerSpellIceSpear> CPlayerSpellIceSpear::Create()
{
	shared_ptr<CPlayerSpellIceSpear> pInstance = make_shared<CPlayerSpellIceSpear>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CPlayerSpellIceSpear");

	return pInstance;
}
