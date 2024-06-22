#include "PlayerArrowBasic.h"
#include "Model.h"
#include "RigidBody.h"

#include "EffectMgr.h"

CPlayerArrowBasic::CPlayerArrowBasic()
{
}

CPlayerArrowBasic::~CPlayerArrowBasic()
{
}

HRESULT CPlayerArrowBasic::Initialize(weak_ptr<CPlayer> _pPlayer)
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 20.f;
	pArg->fRotationPerSec = XMConvertToRadians(90.0f);
	__super::Initialize(pArg);

    m_pOwnerObject = _pPlayer;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;

    m_pTransformCom->SetScaling(_float3(0.5f, 0.5f, 0.5f));
    AddRigidBody();
    DisableProjectile();

	return S_OK;
}

void CPlayerArrowBasic::PriorityTick(_float _fTimeDelta)
{
}

void CPlayerArrowBasic::Tick(_float _fTimeDelta)
{
    GoToTargetPos(_fTimeDelta);
    m_fLifeTime -= _fTimeDelta;

    m_pTransformCom->Tick(_fTimeDelta);
}

void CPlayerArrowBasic::LateTick(_float _fTimeDelta)
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

HRESULT CPlayerArrowBasic::Render()
{
    __super::Render();

	return S_OK;
}

HRESULT CPlayerArrowBasic::RenderGlow()
{
    return S_OK;
}

void CPlayerArrowBasic::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    if (_ColData.eColFlag == COL_MONSTER)
    {
        GAMEINSTANCE->PlaySoundW("Player_CrossBow_Hit_1", 0.6f);
        m_pOwnerObject.lock()->AddSupremeGauge(2.f);
        m_isCollision = true;
    }
}

void CPlayerArrowBasic::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{

}

void CPlayerArrowBasic::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerArrowBasic::EnableProjectile(_float3 _vMyStartPos, _float3 _vTargetPos, PlayerProjAbnormal _ePlayerProjAbnormal, _bool isDistinguish , _uint _iIndex )
{
    m_eMyAbNormal = _ePlayerProjAbnormal;

    _float3 vTargetDir = _vTargetPos - _vMyStartPos;
    vTargetDir.Normalize();

    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vMyStartPos);
    m_pTransformCom->LookAtDir(vTargetDir);

    _int iRandArrow = rand() % 3;

    m_iEffectIdx = EFFECTMGR->PlayEffect("Plr_Arrow_Basic", shared_from_this());

    m_pRigidBody->SimulationOn();
    SetEnable(true);
}

void CPlayerArrowBasic::DisableProjectile()
{
    EFFECTMGR->StopEffect("Plr_Arrow_Basic", m_iEffectIdx);
    m_fLifeTime = 1.5f;
    m_isCollision = false;
    m_pRigidBody->SimulationOff();
    SetEnable(false);
}

void CPlayerArrowBasic::GoToTargetPos(_float _fTimeDelta)
{
    m_pTransformCom->GoStraight(_fTimeDelta);
}

HRESULT CPlayerArrowBasic::AddRigidBody()
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
    RectDesc.vSize = _float3{ 0.1f, 0.1f, 0.6f };
    RectDesc.vOffSetPosition = { 0.0f, 0.0f, 0.0f };
    RectDesc.strShapeTag = "Player_Arrow_Basic";
    RigidBodyDesc.pGeometry = &RectDesc;
    
    RigidBodyDesc.eThisColFlag = COL_PLAYERPROJECTILE;
    RigidBodyDesc.eTargetColFlag = COL_MONSTER;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
    
    m_pTransformCom->SetRigidBody(m_pRigidBody);
	return S_OK;
}

shared_ptr<CPlayerArrowBasic> CPlayerArrowBasic::Create(weak_ptr<CPlayer> _pPlayer)
{
	shared_ptr<CPlayerArrowBasic> pInstance = make_shared<CPlayerArrowBasic>();

	if (FAILED(pInstance->Initialize(_pPlayer)))
		MSG_BOX("Failed to Create : CPlayerArrowBasic");

	return pInstance;
}
