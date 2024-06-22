#include "Pillar.h"
#include "Model.h"
#include "RigidBody.h"
#include "EffectAttachObj.h"

CPillar::CPillar()
{
}

CPillar::~CPillar()
{
}

HRESULT CPillar::Initialize(PILLAR_DESC* _pPillarDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    PROJECTILE_DESC* pProjectileDesc = (PROJECTILE_DESC*)_pPillarDesc;

    __super::Initialize(pProjectileDesc, _pTransformDesc);

    m_iType = _pPillarDesc->iType;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;

    m_pModelCom = GAMEINSTANCE->GetModel("BG_IronPillar_01_01");

    // 타입에 따라 높이 다르게 해주기
    m_pTransformCom->SetScaling(1.5f, 0.4f * m_iType, 1.5f);

    // 발사체 종류 정해주기
    m_iProjectileType = PJ_FALL;

    AddRigidBody();

    SimulationOff();

    m_IsEnabled = false;

    m_vecPillarEffect = CEffectAttachObj::Create(_float3(0.f, 0.f, 0.f), "Pillar_Up", false);
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Effect"), m_vecPillarEffect)))
        return E_FAIL;

    return S_OK;
}

void CPillar::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);
}

void CPillar::Tick(_float _fTimeDelta)
{
    CheckCollisionActive();

    __super::Tick(_fTimeDelta);

    if (m_bCanMoveUp)
    {
        m_fMoveDistance += _fTimeDelta * m_fMoveSpeed * _float(m_iType);
        MovePos(_float3(m_vCurrentPos.x, m_vCurrentPos.y + _fTimeDelta * m_fMoveSpeed * _float(m_iType), m_vCurrentPos.z));
        m_vecPillarEffect->GetTransform()->SetState(CTransform::STATE_POSITION, m_vecPillarEffect->GetTransform()->GetState(CTransform::STATE_POSITION) + _float3(0.f, _fTimeDelta * m_fMoveSpeed * _float(m_iType), 0.f));
        if (m_fMoveDistance > (6.f * (m_iType) + 0.1f))
        {
            GAMEINSTANCE->StopSound("Final_SFX_Quake");
            m_fMoveDistance = 0.f;
            m_bCanMoveUp = false;
            OnCollisionProjectile();
        }
    }

    if (m_bCanMoveDown && !m_bOffCollisionToMoveDown)
    {
        m_bOffCollisionToMoveDown = true;
        OffCollisionProjectile();
    }

    if (m_bCanMoveDown)
    {
        m_fMoveDistance += _fTimeDelta * m_fMoveSpeed * _float(m_iType);
        MovePos(_float3(m_vCurrentPos.x, m_vCurrentPos.y - _fTimeDelta * m_fMoveSpeed * _float(m_iType), m_vCurrentPos.z));
        if (m_fMoveDistance > (6.f * (m_iType)+0.1f))
        {
            m_fMoveDistance = 0.f;
            m_bCanMoveDown = false;
        }
    }
    
    m_pTransformCom->Tick(_fTimeDelta);
}

void CPillar::LateTick(_float _fTimeDelta)
{
    m_pTransformCom->LateTick(_fTimeDelta);

    __super::LateTick(_fTimeDelta);
    
    if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 25.f))
    {
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
    }
}

HRESULT CPillar::Render()
{
    if (!m_IsEnabled)
        return S_OK;
    
    __super::Render();

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }
    
    return S_OK;
}

void CPillar::PlayMoveUpEffect()
{
    _float3 vPillarEffectPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
    vPillarEffectPos.y = -30.8f;
    m_vecPillarEffect->GetTransform()->SetState(CTransform::STATE_POSITION, vPillarEffectPos);
    EFFECTMGR->PlayEffect("Pillar_Up", m_vecPillarEffect);
}

shared_ptr<CPillar> CPillar::Create(PILLAR_DESC* _pPillarDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    shared_ptr<CPillar> pInstance = make_shared<CPillar>();

    if (FAILED(pInstance->Initialize(_pPillarDesc, _pTransformDesc)))
        MSG_BOX("Failed to Create : Pillar");

    return pInstance;
}

HRESULT CPillar::AddRigidBody()
{
    CRigidBody::RIGIDBODYDESC RigidBodyDesc;
    
    /* Create Actor */
//bool			isStatic = false;	/* 정적인 객체는 true */
//bool			isTrigger = false;	/* 트리거 객체는 true */
//bool			isGravity = true;		/* 중력을 적용 받는 객체는 true, (static 은 중력을 받지않음)*/
//bool			isInitCol = true;		/* 생성하자마자 충돌을 끄려면 false */
//_float4x4		WorldMatrix = XMMatrixIdentity();	/* Transform 꺼 통째로 던지면 됨 */
//UINT			eLock_Flag = ELOCK_FLAG::NONE; /* Static은 필요없음,CRigidBody::ELOCK_FLAG */
//
///* Create Shape */
//weak_ptr<CGameObject> pOwnerObject;				/* 소유 객체의 포인터 */
//float			fStaticFriction = 0.5f;		/* 정지 마찰력(0.0f~) */
//float			fDynamicFriction = 0.5f;		/* 운동 마찰력(0.0f~) */
//float			fRestitution = 0.0f;		/* 탄성력(0.0f~1.0f) */
//Geometry* pGeometry = nullptr;	/* 모양(GeometrySphere, GeometryBox, etc) */
//ECOLLISIONFLAG	eThisColFlag = COL_NONE;	/* 자기 자신의 충돌 Flag */
//UINT			eTargetColFlag = COL_NONE; /* 충돌할 객체의 Flag를 추가 ex) COL_MONSTER | COL_ITEM  */

    RigidBodyDesc.isStatic = true;
    RigidBodyDesc.isTrigger = false;
    RigidBodyDesc.isGravity = true;
    RigidBodyDesc.isInitCol = false;
    RigidBodyDesc.pTransform = m_pTransformCom;
    RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;

    RigidBodyDesc.pOwnerObject = shared_from_this();
    RigidBodyDesc.fStaticFriction = 1.0f;
    RigidBodyDesc.fDynamicFriction = 0.0f;
    RigidBodyDesc.fRestitution = 0.0f;

    GeometryBox RectDesc;
    RectDesc.vSize.x = 6.f;
    RectDesc.vSize.y = 6.f * m_iType;
    RectDesc.vSize.z = 6.f;
    RectDesc.vOffSetPosition = { 0.f, 3.f * m_iType, 0.f };
    RectDesc.vOffSetRotation = { 0.0f, 0.0f, 0.0f };
    RectDesc.strShapeTag = "Pillar_Body";
    m_strShapeTag = "Pillar_Body";
    RigidBodyDesc.pGeometry = &RectDesc;

    RigidBodyDesc.eThisColFlag = COL_STATIC;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER | COL_MONSTERPROJECTILE | COL_ROPE;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

    m_pTransformCom->SetRigidBody(m_pRigidBody);
    m_pRigidBody->SetMass(30.0f);

    return S_OK;
}

void CPillar::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPillar::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPillar::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}
