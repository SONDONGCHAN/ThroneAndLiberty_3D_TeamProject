#include "ElectricBall.h"
#include "Model.h"
#include "RigidBody.h"
#include "EffectMgr.h"

CElectricBall::CElectricBall()
{
}

CElectricBall::~CElectricBall()
{
}

HRESULT CElectricBall::Initialize(ELECTRICMAGIC_DESC* _pElectricBallDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    PROJECTILE_DESC* pProjectileDesc = (PROJECTILE_DESC*)_pElectricBallDesc;

    __super::Initialize(pProjectileDesc, _pTransformDesc);

    // 소유자 지정해주기
    m_pOwner = _pElectricBallDesc->pOwner;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;
    
    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

    // 발사체 종류 정해주기
    m_iProjectileType = PJ_DIRECT;
    
    AddRigidBody();

    SimulationOff();

    m_IsEnabled = false;
    m_fDistanceToTarget = 30.f;

    return S_OK;
}

void CElectricBall::PriorityTick(_float _fTimeDelta)
{
    if (m_bOnlyRender)
        return;

    __super::PriorityTick(_fTimeDelta);
}

void CElectricBall::Tick(_float _fTimeDelta)
{
    if (m_bStartEffect)
    {
        //EFFECTMGR->PlayEffect("Boss_BindMagic_Fire", shared_from_this());
        m_bStartEffect = false;
    }

    CheckCollisionActive();

    if (m_bOnlyRender)
        return;
    
    __super::Tick(_fTimeDelta);

    //m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), _fTimeDelta);

    // 비활성화 조건
    if (m_fDistanceToTarget > m_fEnableDistance)
    {
        m_vCurrentPos.y = 200.f;
        m_fDistanceToTarget = 30.f;
        SimulationOff();
        m_IsEnabled = false;
    }

    m_pTransformCom->Tick(_fTimeDelta);
}

void CElectricBall::LateTick(_float _fTimeDelta)
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

HRESULT CElectricBall::Render()
{
    if (!m_IsEnabled)
        return S_OK;
    
    __super::Render();
    
    return S_OK;
}

HRESULT CElectricBall::AddRigidBody()
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
    switch (m_iType)
    {
    case 0:
        RectDesc.vSize.x = 1.2f;
        RectDesc.vSize.y = 1.2f;
        RectDesc.vSize.z = 1.2f;
        break;
    case 1:
        RectDesc.vSize.x = 0.6f;
        RectDesc.vSize.y = 0.6f;
        RectDesc.vSize.z = 0.6f;
        break;
    }
    RectDesc.vOffSetPosition = { 0.f, 0.0f, 0.0f };
    RectDesc.vOffSetRotation = { 0.0f, 0.0f, 0.f };
    RectDesc.strShapeTag = "ElectricBall_Body";
    m_strShapeTag = "ElectricBall_Body";
    RigidBodyDesc.pGeometry = &RectDesc;

    RigidBodyDesc.eThisColFlag = COL_MONSTERPROJECTILE;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER | COL_PLAYERPARRYING /*| COL_CHECKBOX*/;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
    
    m_pTransformCom->SetRigidBody(m_pRigidBody);
    
    return S_OK;
}

void CElectricBall::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    if (_ColData.eColFlag & COL_CHECKBOX)
        return;

    if (_ColData.eColFlag & COL_PLAYER && !m_bOnlyRender)
    {
        if (m_iProjectileType == PJ_DIRECT)
        {
            GAMEINSTANCE->PlaySoundW("LM_SFX_ElectricBall1Hitted", 0.5f);
        }
        else if (m_iProjectileType == PJ_GUIDED)
        {
            GAMEINSTANCE->PlaySoundW("LM_SFX_ElectricBall2Hitted", 0.5f);
        }

        m_pTransformCom->SetPositionParam(1, 100.f);
        m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
        m_fDistanceToTarget = 30.f;
        m_bActiveOff = true;
    }
    
    // 기둥에 부딪쳤을때
    if (_ColData.eColFlag & COL_STATIC && !m_bOnlyRender)
    {
        if (m_iProjectileType == PJ_DIRECT)
        {
            GAMEINSTANCE->PlaySoundW("LM_SFX_ElectricBall1Hitted", 0.5f);
        }
        else if (m_iProjectileType == PJ_GUIDED)
        {
            GAMEINSTANCE->PlaySoundW("LM_SFX_ElectricBall2Hitted", 0.5f);
        }
        m_pTransformCom->SetPositionParam(1, 100.f);
        m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
        m_fDistanceToTarget = 30.f;
        m_bActiveOff = true;
    }
    
    // 플레이어 무기에 부딪쳤을때
    if ((_ColData.eColFlag & COL_PLAYERPARRYING) && !m_bOnlyRender)
    {
        if (m_iProjectileType == PJ_DIRECT)
        {
            GAMEINSTANCE->PlaySoundW("LM_SFX_ElectricBall1Hitted", 0.5f);
        }
        else if (m_iProjectileType == PJ_GUIDED)
        {
            GAMEINSTANCE->PlaySoundW("LM_SFX_ElectricBall2Hitted", 0.5f);
        }
        m_pTransformCom->SetPositionParam(1, 100.f);
        m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
        m_fDistanceToTarget = 30.f;
        m_bActiveOff = true;
    }
}

void CElectricBall::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CElectricBall::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

shared_ptr<CElectricBall> CElectricBall::Create(ELECTRICMAGIC_DESC* _pElectricBallDesc, CTransform::TRANSFORM_DESC* _pTransformDesc, _int _iType)
{
    shared_ptr<CElectricBall> pInstance = make_shared<CElectricBall>();

    pInstance->SetType(_iType);
    if (FAILED(pInstance->Initialize(_pElectricBallDesc, _pTransformDesc)))
        MSG_BOX("Failed to Create : ElectricBall");

    return pInstance;
}
