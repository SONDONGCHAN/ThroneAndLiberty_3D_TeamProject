#include "BindMagic.h"
#include "Model.h"
#include "RigidBody.h"
#include "EffectMgr.h"

CBindMagic::CBindMagic()
{
}

CBindMagic::~CBindMagic()
{
}

HRESULT CBindMagic::Initialize(BINDMAGIC_DESC* _pBindMagicDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    PROJECTILE_DESC* pProjectileDesc = (PROJECTILE_DESC*)_pBindMagicDesc;

    __super::Initialize(pProjectileDesc, _pTransformDesc);

    // 소유자 지정해주기
    m_pOwner = _pBindMagicDesc->pOwner;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;
    
    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

    // 발사체 종류 정해주기
    //m_iProjectileType = PJ_DIRECT;

    AddRigidBody();

    SimulationOff();

    m_IsEnabled = false;
    m_fDistanceToTarget = 30.f;

    //m_vLockOnOffSetPosition = { 0.0f,0.2f,0.0f };

    return S_OK;
}

void CBindMagic::PriorityTick(_float _fTimeDelta)
{
    if (m_bOnlyRender)
        return;

    __super::PriorityTick(_fTimeDelta);
}

void CBindMagic::Tick(_float _fTimeDelta)
{
    if (m_bStartEffect)
    {
        EFFECTMGR->PlayEffect("Boss_BindMagic_Fire", shared_from_this());
        GAMEINSTANCE->PlaySoundW("Final_SFX_BindMagicFire", m_fSound);
        m_bStartEffect = false;
    }

    // 보스 뒤 주변에 띄워주는 마법일때
    if (m_iMagicType == BM_T_SPINBACK)
    {
        if (m_bReadyFire)
        {
            switch (m_iCircleMagicType)
            {
            case 0:
                MovePos(m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) +
                    _float3(0.f, 3.f, 0.f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_UP) * m_fCircleMagicLength * cos(((18.f - 90.f) / 180.f) * 3.14f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_RIGHT) * m_fCircleMagicLength * sin(((18.f - 90.f) / 180.f) * 3.14f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_LOOK) * -m_fZLength);
                break;
            case 1:
                MovePos(m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) +
                    _float3(0.f, 3.f, 0.f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_UP) * m_fCircleMagicLength * cos(((90.f - 90.f) / 180.f) * 3.14f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_RIGHT) * m_fCircleMagicLength * sin(((90.f - 90.f) / 180.f) * 3.14f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_LOOK) * -m_fZLength);
                break;
            case 2:
                MovePos(m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) +
                    _float3(0.f, 3.f, 0.f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_UP) * m_fCircleMagicLength * cos(((162.f - 90.f) / 180.f) * 3.14f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_RIGHT) * m_fCircleMagicLength * sin(((162.f - 90.f) / 180.f) * 3.14f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_LOOK) * -m_fZLength);
                break;
            case 3:
                MovePos(m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) +
                    _float3(0.f, 3.f, 0.f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_UP) * m_fCircleMagicLength * cos(((234.f - 90.f) / 180.f) * 3.14f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_RIGHT) * m_fCircleMagicLength * sin(((234.f - 90.f) / 180.f) * 3.14f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_LOOK) * -m_fZLength);
                break;
            case 4:
                MovePos(m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) +
                    _float3(0.f, 3.f, 0.f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_UP) * m_fCircleMagicLength * cos(((306.f - 90.f) / 180.f) * 3.14f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_RIGHT) * m_fCircleMagicLength * sin(((306.f - 90.f) / 180.f) * 3.14f) +
                    m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_LOOK) * -m_fZLength);
                break;
            }
        }
    }

    CheckCollisionActive();

    if (m_bOnlyRender)
        return;
    
    __super::Tick(_fTimeDelta);

    //m_pTransformCom->Turn(_float3(1.f, 0.f, 0.f), _fTimeDelta);

    // 비활성화 조건 -> 나중에는 피직스로 바닥 충돌하면
    if (m_fDistanceToTarget > m_fEnableDistance)
    {
        m_vCurrentPos.y = 200.f;
        m_fDistanceToTarget = 30.f;
        SimulationOff();
        m_IsEnabled = false;
    }

    m_pTransformCom->Tick(_fTimeDelta);
}

void CBindMagic::LateTick(_float _fTimeDelta)
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

HRESULT CBindMagic::Render()
{
    if (!m_IsEnabled)
        return S_OK;
    
    __super::Render();
    
    return S_OK;
}

HRESULT CBindMagic::AddRigidBody()
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
    RectDesc.vSize.y = 1.f;
    RectDesc.vSize.z = 1.f;
    RectDesc.vOffSetPosition = { 0.f, 0.0f, 0.0f };
    RectDesc.vOffSetRotation = { 0.0f, 0.0f, 0.f };
    RectDesc.strShapeTag = "BindMagic_Body";
    m_strShapeTag = "BindMagic_Body";
    RigidBodyDesc.pGeometry = &RectDesc;

    RigidBodyDesc.eThisColFlag = COL_MONSTERPROJECTILE;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER | COL_PLAYERPARRYING;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
    
    m_pTransformCom->SetRigidBody(m_pRigidBody);
    
    return S_OK;
}

void CBindMagic::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    if (_ColData.eColFlag & COL_CHECKBOX)
        return;

    if (_ColData.eColFlag & COL_PLAYER && !m_bOnlyRender)
    {
        GAMEINSTANCE->PlaySoundW("Final_SFX_BindMagicHitted", 0.5f);
        m_pTransformCom->SetPositionParam(1, 100.f);
        m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
        m_fDistanceToTarget = 30.f;
        m_bActiveOff = true;
    }

    // 기둥에 부딪쳤을때
    if (_ColData.eColFlag & COL_STATIC && !m_bOnlyRender)
    {
        GAMEINSTANCE->PlaySoundW("Final_SFX_BindMagicHitted", 0.5f);
        m_pTransformCom->SetPositionParam(1, 100.f);
        m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
        m_fDistanceToTarget = 30.f;
        m_bActiveOff = true;
    }

    // 플레이어 무기에 부딪쳤을때
    if ((_ColData.eColFlag & COL_PLAYERPARRYING) && !m_bOnlyRender)
    {
        GAMEINSTANCE->PlaySoundW("Final_SFX_BindMagicHitted", 0.5f);
        m_pTransformCom->SetPositionParam(1, 100.f);
        m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
        m_fDistanceToTarget = 30.f;
        m_bActiveOff = true;
    }
}

void CBindMagic::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CBindMagic::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

shared_ptr<CBindMagic> CBindMagic::Create(BINDMAGIC_DESC* _pBindMagicDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    shared_ptr<CBindMagic> pInstance = make_shared<CBindMagic>();

    if (FAILED(pInstance->Initialize(_pBindMagicDesc, _pTransformDesc)))
        MSG_BOX("Failed to Create : BindMagic");

    return pInstance;
}
