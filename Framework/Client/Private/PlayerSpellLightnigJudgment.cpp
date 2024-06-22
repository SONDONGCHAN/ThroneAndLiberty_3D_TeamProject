#include "PlayerSpellLightnigJudgment.h"
#include "Model.h"
#include "RigidBody.h"

CPlayerSpellLightnigJudgment::CPlayerSpellLightnigJudgment()
{
}

CPlayerSpellLightnigJudgment::~CPlayerSpellLightnigJudgment()
{
}

HRESULT CPlayerSpellLightnigJudgment::Initialize()
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 0.f;
	pArg->fRotationPerSec = XMConvertToRadians(90.0f);
	__super::Initialize(pArg);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;

    m_pTransformCom->SetScaling(_float3(20.f, 100.f, 1.f));
    AddRigidBody();
    DisableProjectile();

	return S_OK;
}

void CPlayerSpellLightnigJudgment::PriorityTick(_float _fTimeDelta)
{
}

void CPlayerSpellLightnigJudgment::Tick(_float _fTimeDelta)
{
    m_fLifeTime -= _fTimeDelta;
    m_pTransformCom->Tick(_fTimeDelta);
}

void CPlayerSpellLightnigJudgment::LateTick(_float _fTimeDelta)
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

HRESULT CPlayerSpellLightnigJudgment::Render()
{
    __super::Render();

	return S_OK;
}

HRESULT CPlayerSpellLightnigJudgment::RenderGlow()
{

    return S_OK;
}

void CPlayerSpellLightnigJudgment::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerSpellLightnigJudgment::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerSpellLightnigJudgment::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerSpellLightnigJudgment::EnableProjectile(_float3 _vMyStartPos, _float3 _vTargetPos, PlayerProjAbnormal _ePlayerProjAbnormal, _bool isDistinguish, _uint _iIndex)
{
    m_eMyAbNormal = _ePlayerProjAbnormal;
    
    _float3 vTargetDir = _vTargetPos - _vMyStartPos;
    vTargetDir.y = 0.f;
    vTargetDir.Normalize();

    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vTargetPos);
    m_pTransformCom->LookAtDir(vTargetDir);

    EFFECTMGR->PlayEffect("Plr_LightningJudgement", shared_from_this());
    GAMEINSTANCE->PlaySoundW("Player_Staff_Skill_4", 0.8f);

    m_pRigidBody->SimulationOn();
    SetEnable(true);
}

void CPlayerSpellLightnigJudgment::DisableProjectile()
{
    m_fLifeTime = 0.2f;
    m_pRigidBody->SimulationOff();
    SetEnable(false);
}

HRESULT CPlayerSpellLightnigJudgment::AddRigidBody()
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
    RectDesc.vSize = _float3{ 2.5f, 5.f, 2.5f };
    RectDesc.vOffSetPosition = { 0.f, 2.5f, 0.f };
    RectDesc.strShapeTag = "Player_Spell_LightnigJudgment";
    RigidBodyDesc.pGeometry = &RectDesc;
    
    RigidBodyDesc.eThisColFlag = COL_PLAYERPROJECTILE;
    RigidBodyDesc.eTargetColFlag = COL_MONSTER | COL_STATIC;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
    
    m_pTransformCom->SetRigidBody(m_pRigidBody);
	return S_OK;
}

shared_ptr<CPlayerSpellLightnigJudgment> CPlayerSpellLightnigJudgment::Create()
{
	shared_ptr<CPlayerSpellLightnigJudgment> pInstance = make_shared<CPlayerSpellLightnigJudgment>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CPlayerSpellLightnigJudgment");

	return pInstance;
}
