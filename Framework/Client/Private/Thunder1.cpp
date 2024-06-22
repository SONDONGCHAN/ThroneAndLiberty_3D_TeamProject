#include "Thunder1.h"
#include "Model.h"
#include "RigidBody.h"
#include "EffectMgr.h"

CThunder1::CThunder1()
{
}

CThunder1::~CThunder1()
{
}

HRESULT CThunder1::Initialize(THUNDERMAGIC_DESC* _pThunder1Desc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    PROJECTILE_DESC* pProjectileDesc = (PROJECTILE_DESC*)_pThunder1Desc;

    __super::Initialize(pProjectileDesc, _pTransformDesc);

    // 소유자 지정해주기
    m_pOwner = _pThunder1Desc->pOwner;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;
    
    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

    // 발사체 종류 정해주기
    m_iProjectileType = PJ_SUMMON;
    
    AddRigidBody();

    SimulationOff();

    // 생명시간 설정
    m_fLifeTime = 3.f;

    m_IsEnabled = false;

    return S_OK;
}

void CThunder1::PriorityTick(_float _fTimeDelta)
{
    if (m_bOnlyRender)
        return;

    __super::PriorityTick(_fTimeDelta);
}

void CThunder1::Tick(_float _fTimeDelta)
{
    if (m_bStartEffect)
    {
        /*EFFECTMGR->PlayEffect("Boss_BindMagic_Fire", shared_from_this());*/
        m_bStartEffect = false;
    }
    
    CheckCollisionActive();

    if (m_bOnlyRender)
        return;
    
    __super::Tick(_fTimeDelta);

    m_fCalculateTime1 += _fTimeDelta;

    // 콜리젼 켜지는 시간(1초후)
    if (m_fCalculateTime1 > m_fCollisionOnTime && !m_bThunderCollsionOn)
    {
        m_bThunderCollsionOn = true;
        OnCollisionProjectile();
    }

    // 비활성화 조건
    if (m_fLifeTime < m_fCalculateTime1)
    {
        GAMEINSTANCE->StopSound("MidgetSH_Lightning");
        m_fCalculateTime1 = 0.f;
        SimulationOff();
        m_bThunderCollsionOn = false;
        m_IsEnabled = false;
    }

    //m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), _fTimeDelta);

    // 비활성화 조건 -> 나중에는 피직스로 바닥 충돌하면
    /*if (m_fDistanceToTarget > m_fEnableDistance)
    {
        m_vCurrentPos.y = 200.f;
        m_fDistanceToTarget = 30.f;
        SimulationOff();
        m_IsEnabled = false;
    }*/

    m_pTransformCom->Tick(_fTimeDelta);
}

void CThunder1::LateTick(_float _fTimeDelta)
{
    if (!m_bOnlyRender)
    {
        m_pTransformCom->LateTick(_fTimeDelta);

        __super::LateTick(_fTimeDelta);
    }
    
    if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 4.0f))
    {
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
    }

    if (m_bActiveOff)
    {
        m_pRigidBody->DisableCollision(m_strShapeTag);
        m_bCollisionActive = false;
        m_bPrevCollisionActive = false;
        m_bActiveOff = false;
        SimulationOff();
        m_IsEnabled = false;
    }
}

HRESULT CThunder1::Render()
{
    if (!m_IsEnabled)
        return S_OK;
    
    __super::Render();
    
    return S_OK;
}

HRESULT CThunder1::AddRigidBody()
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
    RectDesc.vSize.x = 1.f;
    RectDesc.vSize.y = 4.f;
    RectDesc.vSize.z = 1.f;
    RectDesc.vOffSetPosition = { 0.f, 2.f, 0.0f };
    RectDesc.vOffSetRotation = { 0.0f, 0.0f, 0.f };
    RectDesc.strShapeTag = "Thunder1_Body";
    m_strShapeTag = "Thunder1_Body";
    RigidBodyDesc.pGeometry = &RectDesc;

    RigidBodyDesc.eThisColFlag = COL_MONSTERPROJECTILE;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER | COL_PLAYERPARRYING /*| COL_CHECKBOX*/;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
    
    m_pTransformCom->SetRigidBody(m_pRigidBody);
    
    return S_OK;
}

void CThunder1::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    if (_ColData.eColFlag & COL_CHECKBOX)
        return;

    if (_ColData.eColFlag & COL_PLAYER && !m_bOnlyRender)
    {
        //m_pTransformCom->SetPositionParam(1, 100.f);
        //m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
        //m_bActiveOff = true;
    }
}

void CThunder1::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CThunder1::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

shared_ptr<CThunder1> CThunder1::Create(THUNDERMAGIC_DESC* _pThunder1Desc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    shared_ptr<CThunder1> pInstance = make_shared<CThunder1>();

    if (FAILED(pInstance->Initialize(_pThunder1Desc, _pTransformDesc)))
        MSG_BOX("Failed to Create : Thunder1");

    return pInstance;
}
