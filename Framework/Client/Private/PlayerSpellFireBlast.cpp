#include "PlayerSpellFireBlast.h"
#include "Model.h"
#include "RigidBody.h"

CPlayerSpellFireBlast::CPlayerSpellFireBlast()
{
}

CPlayerSpellFireBlast::~CPlayerSpellFireBlast()
{
}

HRESULT CPlayerSpellFireBlast::Initialize()
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 1.f;
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

void CPlayerSpellFireBlast::PriorityTick(_float _fTimeDelta)
{
}

void CPlayerSpellFireBlast::Tick(_float _fTimeDelta)
{
    m_fLifeTime -= _fTimeDelta;
    m_pTransformCom->Tick(_fTimeDelta);
}

void CPlayerSpellFireBlast::LateTick(_float _fTimeDelta)
{
    if ((m_fLifeTime <= 0.f) )
    {
        m_iRepeatCount -= 1;

        if (m_iRepeatCount <= 0)
        {
            DisableProjectile();
            return;
        }
        else
        {
            m_fLifeTime = 0.1f;
            m_pTransformCom->GoStraight(4.f);


            switch (m_iRepeatCount)
            {
            case 1:
                EFFECTMGR->PlayEffect("Plr_IceBlast_Final", shared_from_this());
                GAMEINSTANCE->PlaySoundW("Player_Staff_Skill_3", 0.5f);
                break;
            case 2:
                EFFECTMGR->PlayEffect("Plr_IceBlast_Forth", shared_from_this());
                GAMEINSTANCE->PlaySoundW("Player_Staff_Skill_3", 0.5f);
                break;
            case 3:
                EFFECTMGR->PlayEffect("Plr_IceBlast_Third", shared_from_this());
                GAMEINSTANCE->PlaySoundW("Player_Staff_Skill_3", 0.5f);
                break;
            case 4:
                EFFECTMGR->PlayEffect("Plr_IceBlast_Second", shared_from_this());
                GAMEINSTANCE->PlaySoundW("Player_Staff_Skill_3", 0.5f);
                break;
            default:
                break;
            }
        }
       
    }

#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
    
}

HRESULT CPlayerSpellFireBlast::Render()
{
    //__super::Render();

	return S_OK;
}

HRESULT CPlayerSpellFireBlast::RenderGlow()
{

    return S_OK;
}

void CPlayerSpellFireBlast::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerSpellFireBlast::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerSpellFireBlast::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerSpellFireBlast::EnableProjectile(_float3 _vMyStartPos, _float3 _vTargetPos, PlayerProjAbnormal _ePlayerProjAbnormal, _bool isDistinguish, _uint _iIndex)
{
    m_eMyAbNormal = _ePlayerProjAbnormal;
    
    _float3 vTargetDir = _vTargetPos - _vMyStartPos;
    vTargetDir.y = 0.f;
    vTargetDir.Normalize();

    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vTargetPos);
    m_pTransformCom->LookAtDir(vTargetDir);

    EFFECTMGR->PlayEffect("Plr_IceBlast_First", shared_from_this());
    GAMEINSTANCE->PlaySoundW("Player_Staff_Skill_3", 0.5f);
    m_pRigidBody->SimulationOn();
    SetEnable(true);
}

void CPlayerSpellFireBlast::DisableProjectile()
{
    m_iRepeatCount = 5;
    m_fLifeTime = 0.1f;
    m_pRigidBody->SimulationOff();
    SetEnable(false);
}

HRESULT CPlayerSpellFireBlast::AddRigidBody()
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
    RectDesc.vSize = _float3{ 4.f, 3.f, 4.f };
    RectDesc.vOffSetPosition = { 0.0f, 0.f, 0.0f };
    RectDesc.strShapeTag = "Player_Spell_FireBlast";
    RigidBodyDesc.pGeometry = &RectDesc;
    
    RigidBodyDesc.eThisColFlag = COL_PLAYERPROJECTILE;
    RigidBodyDesc.eTargetColFlag = COL_MONSTER | COL_STATIC;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
    
    m_pTransformCom->SetRigidBody(m_pRigidBody);
	return S_OK;
}

shared_ptr<CPlayerSpellFireBlast> CPlayerSpellFireBlast::Create()
{
	shared_ptr<CPlayerSpellFireBlast> pInstance = make_shared<CPlayerSpellFireBlast>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CPlayerSpellFireBlast");

	return pInstance;
}
