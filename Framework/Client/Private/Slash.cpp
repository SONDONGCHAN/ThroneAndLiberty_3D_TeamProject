#include "Slash.h"
#include "Model.h"
#include "RigidBody.h"
#include "EffectMgr.h"

CSlash::CSlash()
{
}

CSlash::~CSlash()
{
}

HRESULT CSlash::Initialize(SLASH_DESC* _pSlashDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    PROJECTILE_DESC* pProjectileDesc = (PROJECTILE_DESC*)_pSlashDesc;

    __super::Initialize(pProjectileDesc, _pTransformDesc);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;
    
    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

    // 슬래쉬 타입 설정
    m_iSlashType = _pSlashDesc->m_iSlashType;
    // 발사체 종류 정해주기
    m_iProjectileType = PJ_DIRECT;

    AddRigidBody();

    SimulationOff();

    m_IsEnabled = false;
    m_fDistanceToTarget = 30.f;

    return S_OK;
}

void CSlash::PriorityTick(_float _fTimeDelta)
{
    if (m_bOnlyRender)
        return;

    __super::PriorityTick(_fTimeDelta);
}

void CSlash::Tick(_float _fTimeDelta)
{
    if (m_bStartEffect)
    {
        switch (m_iSlashType)
        {
        case SLASHLDOWN:
            EFFECTMGR->PlayEffect("BossSlashLDown", shared_from_this());
            EFFECTMGR->PlayEffect("Boss_Slash_Particle", shared_from_this());
            GAMEINSTANCE->PlaySoundW("Final_SFX_SlashFire1", 0.5f);
            break;
        case SLASHLUP:
            EFFECTMGR->PlayEffect("BossSlashLUp", shared_from_this());
            EFFECTMGR->PlayEffect("Boss_Slash_Particle", shared_from_this());
            GAMEINSTANCE->PlaySoundW("Final_SFX_SlashFire1", 0.5f);
            break;
        case SLASHLRIGHT:
            EFFECTMGR->PlayEffect("BossSlashLRight", shared_from_this());
            EFFECTMGR->PlayEffect("Boss_Slash_Particle", shared_from_this());
            GAMEINSTANCE->PlaySoundW("Final_SFX_SlashFire1", 0.5f);
            break;
        case SLASHLRIGHTDOWN:
            EFFECTMGR->PlayEffect("BossSlashLRightDown", shared_from_this());
            EFFECTMGR->PlayEffect("Boss_Slash_Particle", shared_from_this());
            GAMEINSTANCE->PlaySoundW("Final_SFX_SlashFire1", 0.5f);
            break;
        case SLASHDOWN:
            EFFECTMGR->PlayEffect("BossSlashDown", shared_from_this());
            EFFECTMGR->PlayEffect("Boss_Slash_Particle", shared_from_this());
            GAMEINSTANCE->PlaySoundW("Final_SFX_SlashFire2", 0.5f);
            break;
        case SLASHUP:
            EFFECTMGR->PlayEffect("BossSlashUp", shared_from_this());
            EFFECTMGR->PlayEffect("Boss_Slash_Particle", shared_from_this());
            GAMEINSTANCE->PlaySoundW("Final_SFX_SlashFire2", 0.5f);
            break;
        case SLASHRIGHT:
            EFFECTMGR->PlayEffect("BossSlashRight", shared_from_this());
            EFFECTMGR->PlayEffect("Boss_Slash_Particle", shared_from_this());
            GAMEINSTANCE->PlaySoundW("Final_SFX_SlashFire2", 0.5f);
            break;
            // 여기 거대 참격
        case SLASHBIG:
            EFFECTMGR->PlayEffect("BossSlashCharge", shared_from_this());
            // EFFECTMGR->PlayEffect("Boss_Slash_Particle", shared_from_this());
            GAMEINSTANCE->PlaySoundW("Final_SFX_ChargeSlashFire", 0.5f);
            break;
        }
        m_bStartEffect = false;
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

void CSlash::LateTick(_float _fTimeDelta)
{
    if (!m_bOnlyRender)
    {
        m_pTransformCom->LateTick(_fTimeDelta);

        __super::LateTick(_fTimeDelta);
    }

    if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 3.f))
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

HRESULT CSlash::Render()
{
    if (!m_IsEnabled)
        return S_OK;
    
    __super::Render();

    /*_uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }*/
    
    return S_OK;
}

HRESULT CSlash::AddRigidBody()
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
    switch (m_iSlashType)
    {
    case SLASHLDOWN:
        RectDesc.vSize.x = 1.f;
        RectDesc.vSize.y = 7.f;
        RectDesc.vSize.z = 1.5f;
        RectDesc.vOffSetPosition = { 0.f, 3.5f, 0.5f };
        RectDesc.vOffSetRotation = { 0.0f, 0.0f, 0.f };
        RectDesc.strShapeTag = "LSlash_Body";
        m_strShapeTag = "LSlash_Body";
        break;
    case SLASHLUP:
        RectDesc.vSize.x = 1.f;
        RectDesc.vSize.y = 7.f;
        RectDesc.vSize.z = 1.5f;
        RectDesc.vOffSetPosition = { 0.5f, 3.5f, 0.5f };
        RectDesc.vOffSetRotation = { 0.0f, 0.0f, -20.f };
        RectDesc.strShapeTag = "LSlash_Body";
        m_strShapeTag = "LSlash_Body";
        break;
    case SLASHLRIGHT:
        RectDesc.vSize.x = 8.f;
        RectDesc.vSize.y = 1.f;
        RectDesc.vSize.z = 1.5f;
        RectDesc.vOffSetPosition = { 0.f, 1.5f, 0.5f };
        RectDesc.vOffSetRotation = { 0.0f, 0.0f, 0.0f };
        RectDesc.strShapeTag = "LSlash_Body";
        m_strShapeTag = "LSlash_Body";
        break;
    case SLASHLRIGHTDOWN:
        RectDesc.vSize.x = 8.f;
        RectDesc.vSize.y = 1.f;
        RectDesc.vSize.z = 1.5f;
        RectDesc.vOffSetPosition = { 0.f, 1.6f, 0.5f };
        RectDesc.vOffSetRotation = { 0.0f, 0.0f, 15.f };
        RectDesc.strShapeTag = "LSlash_Body";
        m_strShapeTag = "LSlash_Body";
        break;
    case SLASHDOWN:
        RectDesc.vSize.x = 1.f;
        RectDesc.vSize.y = 3.f;
        RectDesc.vSize.z = 1.5f;
        RectDesc.vOffSetPosition = { 0.f, 1.5f, 0.5f };
        RectDesc.vOffSetRotation = { 0.0f, 0.0f, 0.f };
        RectDesc.strShapeTag = "Slash_Body";
        m_strShapeTag = "Slash_Body";
        break;
    case SLASHUP:
        RectDesc.vSize.x = 1.f;
        RectDesc.vSize.y = 3.f;
        RectDesc.vSize.z = 1.5f;
        RectDesc.vOffSetPosition = { 0.f, 1.5f, 0.5f };
        RectDesc.vOffSetRotation = { 0.0f, 0.0f, -20.f };
        RectDesc.strShapeTag = "Slash_Body";
        m_strShapeTag = "Slash_Body";
        break;
    case SLASHRIGHT:
        RectDesc.vSize.x = 3.f;
        RectDesc.vSize.y = 1.f;
        RectDesc.vSize.z = 1.5f;
        RectDesc.vOffSetPosition = { 0.f, 1.5f, 0.5f };
        RectDesc.vOffSetRotation = { 0.0f, 0.0f, 0.0f };
        RectDesc.strShapeTag = "Slash_Body";
        m_strShapeTag = "Slash_Body";
        break;
    case SLASHBIG:
        RectDesc.vSize.x = 18.f;
        RectDesc.vSize.y = 1.5f;
        RectDesc.vSize.z = 1.5f;
        RectDesc.vOffSetPosition = { 0.f, -0.5f, 0.5f };
        RectDesc.vOffSetRotation = { 0.0f, 0.0f, -40.f };
        RectDesc.strShapeTag = "BigSlash_Body";
        m_strShapeTag = "BigSlash_Body";
        break;
    }
    RigidBodyDesc.pGeometry = &RectDesc;

    RigidBodyDesc.eThisColFlag = COL_MONSTERPROJECTILE;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER | COL_PLAYERPARRYING;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

    m_pTransformCom->SetRigidBody(m_pRigidBody);
    
    return S_OK;
}

void CSlash::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    // 떨어진 검 제외하고 다른 static 무시하도록 -> 상, 하 참격 바닥에 닿으면 없어지는 경우가 있어서
    if (_ColData.eColFlag & COL_STATIC && (_ColData.szShapeTag != "FallingSword_Body" &&
        _ColData.szShapeTag != "Pillar_Body"))
        return;

    // 플레이어에 부딪쳤을때 관통
    if (_ColData.eColFlag & COL_PLAYER && !m_bOnlyRender)
    {
    }

    // 검이나 기둥에 부딪쳤을때
    if (_ColData.eColFlag & COL_STATIC && (_ColData.szShapeTag == "FallingSword_Body" || _ColData.szShapeTag == "Pillar_Body") && !m_bOnlyRender
        && (_szMyShapeTag == "Slash_Body" || _szMyShapeTag == "LSlash_Body"))
    {
        m_pTransformCom->SetPositionParam(1, 100.f);
        m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
        m_fDistanceToTarget = 30.f;
        m_bActiveOff = true;
    }
    // 플레이어가 패링했을때(거대참격 빼고)
    if (_ColData.eColFlag & COL_PLAYERPARRYING && !m_bOnlyRender && m_iSlashType != SLASHBIG)
    {
        m_pTransformCom->SetPositionParam(1, 100.f);
        m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
        m_fDistanceToTarget = 30.f;
        m_bActiveOff = true;
    }
}

void CSlash::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CSlash::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

shared_ptr<CSlash> CSlash::Create(SLASH_DESC* _pSlashDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    shared_ptr<CSlash> pInstance = make_shared<CSlash>();

    if (FAILED(pInstance->Initialize(_pSlashDesc, _pTransformDesc)))
        MSG_BOX("Failed to Create : Slash");

    return pInstance;
}
