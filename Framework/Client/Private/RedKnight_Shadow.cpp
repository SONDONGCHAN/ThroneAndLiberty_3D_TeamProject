#include "RedKnight_Shadow.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "Slash.h"
#include "Animation.h"
#include "EffectMgr.h"

CRedKnight_Shadow::CRedKnight_Shadow()
    : CMonster()
{
}

CRedKnight_Shadow::~CRedKnight_Shadow()
{
}

HRESULT CRedKnight_Shadow::Initialize()
{
    shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();

    pArg->fSpeedPerSec = 1.f;
    pArg->fRotationPerSec = XMConvertToRadians(1.0f);

    shared_ptr<MONSTER_DESC> pMonsterDesc = make_shared<CMonster::MONSTER_DESC>();
    pMonsterDesc->pTarget = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));

    __super::Initialize(pMonsterDesc.get(), pArg.get());

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_ANIM;

    m_pModelCom = GAMEINSTANCE->GetModel("M_UndeadArmy_Knight");
    m_strModelName = "M_UndeadArmy_Knight_Shadow";
    AddComponent(L"Com_Model", m_pModelCom);
    m_pTransformCom->SetScaling(1.3f, 1.3f, 1.3f);
    m_pTransformCom->Rotation(_float3(0.f, 1.f, 0.f), 3.14f);

    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);

    if (FAILED(AddPartObjects("N_Sword2h_UndeadArmyKnight")))
        return E_FAIL;

    

    // 참격 집어넣기
    // 참격 종류별로 생성후 집어넣기
    shared_ptr<CSlash::SLASH_DESC> pSlashDesc = make_shared<CSlash::SLASH_DESC>();
    pSlashDesc->tProjectile.pTarget = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));

    shared_ptr<CTransform::TRANSFORM_DESC> pTransformDesc = make_shared<CTransform::TRANSFORM_DESC>();
    pTransformDesc->fSpeedPerSec = 10.f;
    pTransformDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    for (int i = SLASHLDOWN; i <= SLASHRIGHT; i++)
    {
        pSlashDesc->m_iSlashType = i;
        for (int j = 0; j < m_iSlashCount; j++)
        {
            shared_ptr<CSlash> pSlash = CSlash::Create(pSlashDesc.get(), pTransformDesc.get());
            if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Projectile"), pSlash)))
                return E_FAIL;
            switch (i)
            {
            case SLASHLDOWN:
                m_vecSlashLD.push_back(pSlash);
                break;
            case SLASHLUP:
                m_vecSlashLU.push_back(pSlash);
                break;
            case SLASHLRIGHT:
                m_vecSlashLR.push_back(pSlash);
                break;
            case SLASHLRIGHTDOWN:
                m_vecSlashLRD.push_back(pSlash);
                break;
            case SLASHDOWN:
                m_vecSlashD.push_back(pSlash);
                break;
            case SLASHUP:
                m_vecSlashU.push_back(pSlash);
                break;
            case SLASHRIGHT:
                m_vecSlashR.push_back(pSlash);
                break;
            }
        }
    }

    // 무기 림라이트 적용
    SetWeaponRimLight(true, _float3(1.f, 0.f, 0.f), 1.5f);

    // 스탯
    m_fHp = 100.f;
    m_fSpeed = 1.5f;

    m_IsEnabled = false;

    AddRigidBody("M_UndeadArmy_Knight");
    m_pRigidBody->SimulationOff();

    m_vLockOnOffSetPosition = { 0.0f,1.f,0.0f };

    /*Trail Pass Number*/
    m_iShaderPass = 6;

    // 움직일수 있는 존의 각각 경계선 설정 
    m_fRightEnd = 70.f;
    m_fLeftEnd = -30.f;
    m_fTopEnd = 285.f;
    m_fBottomEnd = 185.f;

    return S_OK;
}

void CRedKnight_Shadow::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);
    m_vCurrentPos = CalculateBindary(m_vCurrentPos);
    m_vTargetPos = CalculateBindary(m_vTargetPos);
    
    _float2 vTargetPosXZ = _float2(m_vTargetPos.x, m_vTargetPos.z);
    _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
    m_fDistanceToTarget = (vTargetPosXZ - vCurrentPosXZ).Length();
    
    m_vTargetDir = m_vTargetPos - m_vCurrentPos;
    _float3 vTargetDirXZ = _float3(m_vTargetDir.x, 0.f, m_vTargetDir.z);
    vTargetDirXZ.Normalize(m_vNormTargetDirXZ);
}

void CRedKnight_Shadow::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    //분신 패턴
    switch (m_iPatternType)
    {
    case RKS_P_GIMIC1:
        Gimic1Pattern(_fTimeDelta);
        break;
    case RKS_P_2COMBO:
        Combo2Pattern(_fTimeDelta);
        break;
    case RKS_P_3COMBO:
        Combo3Pattern(_fTimeDelta);
        break;
    case RKS_P_JUMPATTACK:
        JumpAttackPattern(_fTimeDelta);
        break;
    case RKS_P_SLASH:
        SlashPattern(_fTimeDelta);
        break;
    case RKS_P_DASHTHROUGH:
        DashThroughPattern(_fTimeDelta);
        break;
    case RKS_P_BINDGROUND:
        BindGroundPattern(_fTimeDelta);
        break;
    case RKS_P_QUAKE:
        QuakePattern(_fTimeDelta);
        break;
    }

    // 사망 받기
    if (m_bDead)
    {
        m_iActionState = RKS_A_DEATH;
        m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
        m_bDead = false;
        m_fCalculateTime = 0.f;
    }
    if (m_iActionState == RKS_A_DEATH)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            // 패턴 값 초기화
            ReBatch();
        }
    }

    _float3 vCurrentPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
    
    m_pModelCom->PlayAnimation(_fTimeDelta * m_fAnimSpeed);

    m_pTransformCom->Tick(_fTimeDelta);
}

void CRedKnight_Shadow::LateTick(_float _fTimeDelta)
{
    m_IsEdgeRender = false;

    m_pModelCom->CheckDisableAnim();
    m_pTransformCom->LateTick(_fTimeDelta);

    __super::LateTick(_fTimeDelta);

    if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 5.f))
    {
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_TRAIL, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
    }
}

HRESULT CRedKnight_Shadow::Render()
{
    __super::Render();

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    UsingRimLight(m_vRimLightColor, 0.5f);

    for (size_t i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->BeginAnimModel(m_pModelCom, (_uint)i);
    }

    /*RimLight Off*/
    _bool bRimOff = false;
    GAMEINSTANCE->BindRawValue("g_IsUsingRim", &bRimOff, sizeof(bRimOff));

    return S_OK;
}

shared_ptr<CRedKnight_Shadow> CRedKnight_Shadow::Create(_float3 _vCreatePos, _float3 _vJumpPos)
{
    shared_ptr<CRedKnight_Shadow> pInstance = make_shared<CRedKnight_Shadow>();

    pInstance->SetCurrentPos(_vCreatePos); // 위치 지정
    pInstance->SetGimic1Pos(_vCreatePos); // 위치 지정
    pInstance->SetJumpPos(_vJumpPos);   // 점프할 위치 지정
    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CRedKnight_Shadow");

    return pInstance;
}

HRESULT CRedKnight_Shadow::AddRigidBody(string _strModelKey)
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

    RigidBodyDesc.isStatic = false;
    RigidBodyDesc.isTrigger = false;
    RigidBodyDesc.isGravity = true;
    RigidBodyDesc.isInitCol = true;
    RigidBodyDesc.pTransform = m_pTransformCom;
    RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;

    RigidBodyDesc.pOwnerObject = shared_from_this();
    RigidBodyDesc.fStaticFriction = 1.0f;
    RigidBodyDesc.fDynamicFriction = 0.0f;
    RigidBodyDesc.fRestitution = 0.0f;

    GeometryCapsule CapsuleDesc;
    CapsuleDesc.fHeight = 2.4f;
    CapsuleDesc.fRadius = 0.6f;
    CapsuleDesc.vOffSetPosition = { 0.0f, 1.8f, 0.0f };
    CapsuleDesc.vOffSetRotation = { 0.0f, 0.0f, 90.0f };
    CapsuleDesc.strShapeTag = "RedKnight_Shadow_Body";
    RigidBodyDesc.pGeometry = &CapsuleDesc;

    RigidBodyDesc.eThisColFlag = COL_MONSTER;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYERWEAPON | COL_PLAYERSKILL | COL_MONSTERSKILL;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

    m_pTransformCom->SetRigidBody(m_pRigidBody);
    m_pRigidBody->SetMass(10.0f);

    //=========================================
    //=============JSON읽어서 저장==============

    wstring wstrModelName(_strModelKey.begin(), _strModelKey.end());
    wstring strCompletePath = L"..\\..\\Tool_Animation\\Bin\\DataFiles\\" + wstrModelName + TEXT(".json");

    Json::Value Model;

    filesystem::path path(strCompletePath);
    if (!(filesystem::exists(path) && filesystem::is_regular_file(path)))
        return S_OK;

    Model = GAMEINSTANCE->ReadJson(strCompletePath);

    auto iter = Model.begin();
    RigidBodyDesc.isStatic = false;
    RigidBodyDesc.isTrigger = true;
    RigidBodyDesc.isInitCol = false;

    GeometryBox		TriggerBoxDesc;
    GeometrySphere  TriggerSphereDesc;
    GeometryCapsule TriggerCapsuleDesc;

    for (iter; iter != Model.end(); ++iter)
    {
        string strAnimIndex = iter.key().asString();
        _uint iAnimIndex = atoi(strAnimIndex.c_str());

        _float3 vOffsetPos = { Model[strAnimIndex]["ShapeInfo"]["OffSetPos"][0].asFloat(),
                                Model[strAnimIndex]["ShapeInfo"]["OffSetPos"][1].asFloat(),
                                Model[strAnimIndex]["ShapeInfo"]["OffSetPos"][2].asFloat() };
        _float3 vOffsetRot = { Model[strAnimIndex]["ShapeInfo"]["OffSetRot"][0].asFloat(),
                                Model[strAnimIndex]["ShapeInfo"]["OffSetRot"][1].asFloat(),
                                Model[strAnimIndex]["ShapeInfo"]["OffSetRot"][2].asFloat() };

        const char* strShapeTag = Model[strAnimIndex]["AnimName"].asCString();

        _uint iGeoType = Model[strAnimIndex]["ShapeInfo"]["Type"].asInt();

        if (iGeoType == 1)
        {
            TriggerBoxDesc.vSize = _float3(Model[strAnimIndex]["ShapeInfo"]["Size"]["BoxSize"][0].asFloat(),
                Model[strAnimIndex]["ShapeInfo"]["Size"]["BoxSize"][1].asFloat(),
                Model[strAnimIndex]["ShapeInfo"]["Size"]["BoxSize"][2].asFloat());

            TriggerBoxDesc.strShapeTag = strShapeTag;

            TriggerBoxDesc.vOffSetPosition = vOffsetPos;
            TriggerBoxDesc.vOffSetRotation = vOffsetRot;

            RigidBodyDesc.pGeometry = &TriggerBoxDesc;
        }

        else if (iGeoType == 2)
        {
            TriggerSphereDesc.fRadius = Model[strAnimIndex]["ShapeInfo"]["Size"]["SphereRadious"].asFloat();

            TriggerSphereDesc.strShapeTag = strShapeTag;

            TriggerSphereDesc.vOffSetPosition = vOffsetPos;
            TriggerSphereDesc.vOffSetRotation = vOffsetRot;

            RigidBodyDesc.pGeometry = &TriggerSphereDesc;
        }

        else if (iGeoType == 3)
        {
            TriggerCapsuleDesc.fHeight = Model[strAnimIndex]["ShapeInfo"]["Size"]["CapsuleHeight"].asFloat();
            TriggerCapsuleDesc.fRadius = Model[strAnimIndex]["ShapeInfo"]["Size"]["CapsuleRadius"].asFloat();

            TriggerCapsuleDesc.strShapeTag = strShapeTag;

            TriggerCapsuleDesc.vOffSetPosition = vOffsetPos;
            TriggerCapsuleDesc.vOffSetRotation = vOffsetRot;

            RigidBodyDesc.pGeometry = &TriggerCapsuleDesc;
        }

        RigidBodyDesc.eThisColFlag = (ECOLLISIONFLAG)Model[strAnimIndex]["ShapeInfo"]["MyFlag"].asInt();
        RigidBodyDesc.eTargetColFlag = (ECOLLISIONFLAG)Model[strAnimIndex]["ShapeInfo"]["TargetFlag"].asInt();

        m_pRigidBody->CreateShape(&RigidBodyDesc);

        vector<shared_ptr<class CAnimation>> vecAnims = m_pModelCom->GetAnimations();
        vecAnims[iAnimIndex]->SetOwnerObject(shared_from_this());
        vecAnims[iAnimIndex]->SetColStartTrackPos(Model[strAnimIndex]["TrackPos"]["StartPos"].asFloat());
        vecAnims[iAnimIndex]->SetColEndTrackPos(Model[strAnimIndex]["TrackPos"]["EndPos"].asFloat());
    }

    //=========================================
    //=========================================

    return S_OK;
}

void CRedKnight_Shadow::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CRedKnight_Shadow::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    
}

void CRedKnight_Shadow::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CRedKnight_Shadow::EnableCollision(const char* _strShapeTag)
{
    m_pRigidBody->EnableCollision(_strShapeTag);
}

void CRedKnight_Shadow::DisableCollision(const char* _strShapeTag)
{
    m_pRigidBody->DisableCollision(_strShapeTag);
}

HRESULT CRedKnight_Shadow::AddPartObjects(string _strModelKey)
{
    shared_ptr<CWeapon::WEAPON_DESC> pWeaponDesc = make_shared<CWeapon::WEAPON_DESC>();
    pWeaponDesc->tPartObject.pParentTransform = m_pTransformCom;
    pWeaponDesc->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_RightHand");
    pWeaponDesc->strModelKey = _strModelKey;
    pWeaponDesc->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(-90.f)) *
        SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(-90.f));

    shared_ptr<CTransform::TRANSFORM_DESC> pTransformDesc = make_shared<CTransform::TRANSFORM_DESC>();

    pTransformDesc->fSpeedPerSec = 10.f;
    pTransformDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    shared_ptr<CWeapon> pWeapon = CWeapon::Create(pWeaponDesc.get(), pTransformDesc.get());
    if (nullptr == pWeapon)
        return E_FAIL;

    shared_ptr<CPartObject> pPartObject = static_pointer_cast<CPartObject>(pWeapon);
    m_pMonsterParts.emplace("Part_Weapon", pPartObject);

    return S_OK;
}

void CRedKnight_Shadow::Gimic1Pattern(_float _fTimeDelta)
{
    // 서있다가 특정 지점으로 점프하기
    if (m_bPatternStart)
    {
        m_vRimLightColor = _float3(1.f, 0.f, 0.f);
        SetWeaponRimLight(true, _float3(1.f, 0.f, 0.f), 1.5f);
        MovePos(m_vGimic1Pattern1Pos);
        m_pTransformCom->Rotation(_float3(0.f, 1.f, 0.f), 3.14f);
        m_bPatternStart = false;
        m_bSkill1State[9] = true;
        m_fCalculateTime = 0.f;
    }
    if (m_bSkill1State[9])
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_fCalculateTime > 3.f)
        {
            m_fCalculateTime = 0.f;
            m_bSkill1State[9] = false;
            m_bSkill1State[0] = true;
        }
    }
    if (m_bSkill1State[0])
    {
        if (m_iActionState == RKS_A_ALT)
        {
            m_fAnimSpeed = 1.f;
            m_fCalculateTime = 0.f;
            m_iActionState = RKS_A_SHORTJUMP;
            m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
            //UpdateDirection();
            // 점프 방향, 속도 설정
            m_vSMovePos = m_vJumpPos;

            _float3 vTargetDirXZ = m_vJumpPos - m_vCurrentPos;
            vTargetDirXZ.Normalize(m_vSJumpNormalMoveDir);

            _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
            _float2 vMovePosXZ = _float2(m_vSMovePos.x, m_vSMovePos.z);
            _float fFastJumpDistance = (vCurrentPosXZ - vMovePosXZ).Length();

            //_float fFastJumpDistance = (m_vCurrentPos - m_vMovePos).Length();
            m_fSJumpMoveSpeed = fFastJumpDistance / (m_fKeyFrameSpeed * 8);
            m_bSkill1State[0] = false;
            m_bSkill1State[1] = true;
        }
    }
    // 회피 점프
    if (m_bSkill1State[1])
    {
        if (m_iActionState == RKS_A_SHORTJUMP)
        {
            m_fCalculateTime += _fTimeDelta;
            if ((m_fCalculateTime > (m_fKeyFrameSpeed * 12 + 0.5f)) && (m_fCalculateTime < (m_fKeyFrameSpeed * 20 + 0.5f)))
            {
                MovePos(m_vSJumpNormalMoveDir * _fTimeDelta * m_fSJumpMoveSpeed + m_vCurrentPos);
            }
            if (m_fCalculateTime > m_fKeyFrameSpeed * 20)
            {
                m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_pTransformCom->LookAtDir(_float3(50.f, 0.f, 50.f) - _float3(m_vCurrentPos.x, 0.f, m_vCurrentPos.z));
                m_fCalculateTime = 0.f;
                m_iActionState = RKS_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f, true);
            }
        }
        if (m_iActionState == RKS_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            if (m_fCalculateTime > 3.f)
            {
                m_fCalculateTime = 0.f;
                m_bSkill1State[1] = false;
                m_bSkill1State[2] = true;
                m_iActionState = RandomSlowSlash();;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
            }
        }
    }
    // 참격
    if (m_bSkill1State[2])
    {
        if (UpdateAttackState(_fTimeDelta))
        {
        }
        if (m_iActionState == RKS_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > m_fStandTime)
            {
                m_fStandTime = _float(GAMEINSTANCE->PickRandomInt(3, 5));
                m_iGimicSlashCount++;
                m_fCalculateTime = 0.f;
                m_fAnimSpeed = 1.f;
                m_iActionState = RandomSlowSlash();
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
            }
        }

        if (m_iGimicSlashCount > m_iGimicBSlashCount)
        {
            m_fAnimSpeed = 1.f;
            m_bSkill1State[2] = false;
        }
    }
}

void CRedKnight_Shadow::Combo2Pattern(_float _fTimeDelta)
{
    // 지정 위치로 이동
    if (m_bPatternStart)
    {
        m_vRimLightColor = _float3(1.f, 0.f, 0.f);
        SetWeaponRimLight(true, _float3(1.f, 0.f, 0.f), 1.5f);
        m_bPatternStart = false;
        MovePos(RandomBlinkPos());
        UpdateDirection();
        m_fCalculateTime = 0.f;
        m_bSkill2State[0] = true;
        m_fAnimSpeed = 1.5f;
        m_iActionState = RandomNormal2ComboStartAttack();
        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
    }
    if (m_bSkill2State[0])
    {
        if (Update2ComboAttackState(_fTimeDelta))
        {
            EFFECTMGR->PlayEffect("Boss_Shadow_Disappear", shared_from_this());
            m_iActionState = RKS_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        if (m_iActionState == RKS_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > m_fDismissTime)
            {
                m_fCalculateTime = 0.f;
                m_bSkill2State[0] = false;
                // 패턴 값 초기화
                ReBatch();
            }
        }
    }
}

void CRedKnight_Shadow::Combo3Pattern(_float _fTimeDelta)
{
    // 지정 위치로 이동
    if (m_bPatternStart)
    {
        m_vRimLightColor = _float3(1.f, 0.f, 0.f);
        SetWeaponRimLight(true, _float3(1.f, 0.f, 0.f), 1.5f);
        m_bPatternStart = false;
        MovePos(RandomBlinkPos());
        UpdateDirection();
        m_fCalculateTime = 0.f;
        m_bSkill2State[0] = true;
        m_fAnimSpeed = 1.5f;
        m_iActionState = RandomNormal2ComboStartAttack();
        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
    }
    if (m_bSkill2State[0])
    {
        if (Update2ComboAttackState(_fTimeDelta))
        {
            EFFECTMGR->PlayEffect("Boss_Shadow_Disappear", shared_from_this());
            m_iActionState = RKS_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        if (m_iActionState == RKS_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > m_fDismissTime)
            {
                m_fCalculateTime = 0.f;
                m_bSkill2State[0] = false;
                // 패턴 값 초기화
                ReBatch();
            }
        }
    }
}

void CRedKnight_Shadow::JumpAttackPattern(_float _fTimeDelta)
{
    // 지정 위치로 이동
    if (m_bPatternStart)
    {
        m_vRimLightColor = _float3(219.f/255.f, 135.f/255.f, 35.f/255.f);
        SetWeaponRimLight(true, _float3(219.f/255.f, 135.f/255.f, 35.f/255.f), 1.5f);
        m_bPatternStart = false;
        MovePos(RandomBlinkJumpAttackPos());
        m_vTargetDir = m_vTargetPos - m_vCurrentPos;
        m_iActionState = RKS_A_SPECIAL1;
        m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
        m_bSkill4State[0] = true;
        UpdateDirection();
        // 점프 방향, 속도 설정
        m_vMovePos = m_vTargetPos;
        UpdateSpecial1DirSpeed();
    }
    if (m_bSkill4State[0])
    {
        if (m_iActionState == RKS_A_SPECIAL1)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > 0.5f && !m_bJumpAttack1EffectOn)
            {
                EFFECTMGR->PlayEffect("Boss_Special1", shared_from_this());
                m_bJumpAttack1EffectOn = true;
            }

            if (m_fCalculateTime > m_fJumpAttackEndTime && !m_bJumpAttackSoundStart)
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_JumpAtt1", m_fEffectSize);
                m_bJumpAttackSoundStart = true;
            }

            if ((m_fCalculateTime > (m_fSpecial1JumpStartTime + 0.5f)) && (m_fCalculateTime < (m_fSpecial1JumpEndTime + 0.5f)))
            {
                MovePos(m_vJumpNormalMoveDir * _fTimeDelta * m_fJumpMoveSpeed + m_vCurrentPos);
            }

            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_bJumpAttackSoundStart = false;
                EFFECTMGR->PlayEffect("Boss_Shadow_Disappear", shared_from_this());
                m_fCalculateTime = 0.f;
                //m_iActionState = RKS_A_ALT;
                //m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
                m_bJumpAttack1EffectOn = false;
                //m_bSkill4State[1] = true;
                m_iActionState = RKS_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            }
        }
        if (m_iActionState == RKS_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > m_fDismissTime)
            {
                m_fCalculateTime = 0.f;
                m_bSkill4State[0] = false;
                // 패턴 값 초기화
                ReBatch();
            }
        }
    }
    //if (m_bSkill4State[1])
    //{
    //    m_fCalculateTime += _fTimeDelta;
    //    if (m_fCalculateTime > 2.f)
    //    {
    //        m_fCalculateTime = 0.f;
    //        m_bSkill4State[1] = false;
    //        // 패턴 값 초기화
    //        ReBatch();
    //    }
    //}
}

void CRedKnight_Shadow::SlashPattern(_float _fTimeDelta)
{
    // 지정 위치로 이동
    if (m_bPatternStart)
    {
        m_vRimLightColor = _float3(1.f, 0.f, 0.f);
        SetWeaponRimLight(true, _float3(1.f, 0.f, 0.f), 1.5f);
        m_fCalculateTime = 0.f;
        m_bPatternStart = false;
        MovePos(RandomBlinkSlashPos());
        UpdateDirection();
        m_fCalculateTime = 0.f;
        m_bSkill5State[0] = true;
        m_iActionState = RandomSlowSlash();
        m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
    }
    if (m_bSkill5State[0])
    {
        if (UpdateAttackState(_fTimeDelta))
        {
            EFFECTMGR->PlayEffect("Boss_Shadow_Disappear", shared_from_this());
            m_iActionState = RKS_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        if (m_iActionState == RKS_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > m_fDismissTime)
            {
                m_fCalculateTime = 0.f;
                // 패턴 값 초기화
                m_bSkill5State[0] = false;
                ReBatch();
            }
        }
    }
}

void CRedKnight_Shadow::DashThroughPattern(_float _fTimeDelta)
{
    // 지정 위치로 이동
    if (m_bPatternStart)
    {
        m_vRimLightColor = _float3(219.f/255.f, 135.f/255.f, 35.f/255.f);
        SetWeaponRimLight(true, _float3(219.f/255.f, 135.f/255.f, 35.f/255.f), 1.5f);
        m_fCalculateTime = 0.f;
        m_bPatternStart = false;
        MovePos(RandomBlinkDashPos());
        m_vTargetDir = m_vTargetPos - m_vCurrentPos;
        _float3 vTargetDirXZ = _float3(m_vTargetDir.x, 0.f, m_vTargetDir.z);
        vTargetDirXZ.Normalize(m_vNormTargetDirXZ);
        
        UpdateDirection();
        m_fAnimSpeed = 0.3f;
        m_fCalculateTime = 0.f;
        m_bSkill6State[0] = true;
        m_iActionState = RKS_A_DASHSTART;
        m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
    }
    if (m_bSkill6State[0])
    {
        if (m_iActionState == RKS_A_DASHSTART)
        {
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            m_fCalculateTime += _fTimeDelta;

            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fAnimSpeed = 1.f;
                m_fCalculateTime = 0.f;
                m_fIn1Skill1DashDistanceXZ = m_fDistanceToTarget + 10.f;
                m_fIn1Skill1DashTime = m_fIn1Skill1DashDistanceXZ / m_fIn1Skill1DashSpeedXZ;
                m_iActionState = RKS_A_DASHATT1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                m_fIn1Skill1DashDir = m_vNormTargetDirXZ;
                UpdateDirection();
                GAMEINSTANCE->PlaySoundW("Final_SFX_Dash1", m_fEffectSize);
            }
        }
        if (m_iActionState == RKS_A_DASHATT1)
        {
            m_fCalculateTime += _fTimeDelta;
            MovePos(CalculateBindary(m_vCurrentPos + m_fIn1Skill1DashDir * m_fIn1Skill1DashSpeedXZ * _fTimeDelta));
            // 시간지나면
            if (m_fCalculateTime > m_fIn1Skill1DashTime)
            {
                /*EFFECTMGR->PlayEffect("Boss_Shadow_Disappear", shared_from_this());
                m_iActionState = RKS_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);*/
                // 패턴 값 초기화
                m_fCalculateTime = 0.f;
                m_bSkill6State[0] = false;
                ReBatch();
            }
        }
        //if (m_iActionState == RKS_A_ALT)
        //{
        //    m_fCalculateTime += _fTimeDelta;
        //    if (m_fCalculateTime > m_fDismissTime)
        //    {
        //        m_fCalculateTime = 0.f;
        //        // 패턴 값 초기화
        //        m_bSkill6State[0] = false;
        //        ReBatch();
        //    }
        //}
    }
}

void CRedKnight_Shadow::BindGroundPattern(_float _fTimeDelta)
{
    // 지정 위치로 이동
    if (m_bPatternStart)
    {
        m_vRimLightColor = _float3(1.f, 1.f, 0.f);
        SetWeaponRimLight(true, _float3(1.f, 1.f, 0.f), 1.5f);
        m_fCalculateTime = 0.f;
        m_bPatternStart = false;
        MovePos(RandomBlinkSlashPos());
        UpdateDirection();
        m_fCalculateTime = 0.f;
        m_fAnimSpeed = 1.2f;
        m_bSkill7State[0] = true;
        m_iActionState = RKS_A_GROUNDMAGIC;
        EFFECTMGR->PlayEffect("Boss_GroundMagic", shared_from_this());
        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
        GAMEINSTANCE->PlaySoundW("Final_SFX_GoundMagic", m_fEffectSize);
    }
    if (m_bSkill7State[0])
    {
        if (m_iActionState == RKS_A_GROUNDMAGIC)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                EFFECTMGR->PlayEffect("Boss_Shadow_Disappear", shared_from_this());
                m_iActionState = RKS_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            }
        }
        if (m_iActionState == RKS_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > m_fDismissTime)
            {
                m_fCalculateTime = 0.f;
                // 패턴 값 초기화
                m_bSkill7State[0] = false;
                ReBatch();
            }
        }
    }
}

void CRedKnight_Shadow::QuakePattern(_float _fTimeDelta)
{
    // 지정 위치로 이동
    if (m_bPatternStart)
    {
        m_vRimLightColor = _float3(219.f/255.f, 135.f/255.f, 35.f/255.f);
        SetWeaponRimLight(true, _float3(219.f/255.f, 135.f/255.f, 35.f/255.f), 1.5f);
        m_fCalculateTime = 0.f;
        m_bPatternStart = false;
        MovePos(RandomBlinkSlashPos());
        UpdateDirection();
        m_fCalculateTime = 0.f;
        m_fAnimSpeed = 1.f;
        m_bSkill8State[0] = true;
        m_iActionState = RKS_A_FRONTQUAKE;
        EFFECTMGR->PlayEffect("Boss_EarthQuake_Anim", shared_from_this());
        EFFECTMGR->PlayEffect("Boss_EarthQuake", shared_from_this());
        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
    }
    if (m_bSkill8State[0])
    {
        if (m_iActionState == RKS_A_FRONTQUAKE)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > m_fQuakeStartTime && !m_bQuakeSoundStart)
            {
                m_fCalculateTime = 0.f;
                GAMEINSTANCE->PlaySoundW("Final_SFX_Quake", m_fEffectSize);
                m_bQuakeSoundStart = true;
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_bQuakeSoundStart = false;
                EFFECTMGR->PlayEffect("Boss_Shadow_Disappear", shared_from_this());
                m_iActionState = RKS_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            }
        }
        if (m_iActionState == RKS_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > m_fDismissTime)
            {
                m_fCalculateTime = 0.f;
                // 패턴 값 초기화
                m_bSkill8State[0] = false;
                ReBatch();
            }
        }
    }
}

void CRedKnight_Shadow::ReBatch()
{
    m_iPatternType = 0;
    m_fHp = 100.f;
    m_fAnimSpeed = 1.f;
    m_iActionState = RKS_A_ALT;
    MovePos(m_vCurrentPos + _float3(0.f, 100.f, 0.f));
    m_fCalculateTime = 0.f;
    m_fCalculateTime2 = 0.f;
    m_fCalculateTime3 = 0.f;
    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
    m_IsEnabled = false;
    SimulationOff();
}

_float3 CRedKnight_Shadow::CalculateBindary(_float3 _vPos)
{
    _float3 _vReturnPos = _vPos;
    if (_vReturnPos.x > m_fRightEnd)
        _vReturnPos.x = m_fRightEnd;
    if (_vReturnPos.x < m_fLeftEnd)
        _vReturnPos.x = m_fLeftEnd;
    if (_vReturnPos.z > m_fTopEnd)
        _vReturnPos.z = m_fTopEnd;
    if (_vReturnPos.z < m_fBottomEnd)
        _vReturnPos.z = m_fBottomEnd;

    return _vReturnPos;
}

_int CRedKnight_Shadow::RandomSlowSlash()
{
    _int iReturnState = 0;
    switch (GAMEINSTANCE->PickRandomInt(0, 4))
    {
    case 0:
        iReturnState = RKS_A_ATT_D3;
        break;
    case 1:
        iReturnState = RKS_A_ATT_U4;
        break;
    case 2:
        iReturnState = RKS_A_ATT_L3;
        break;
    case 3:
        iReturnState = RKS_A_ATT_LM1;
        break;
    }
    return iReturnState;
}

_bool CRedKnight_Shadow::UpdateAttackState(float _fTimeDelta)
{
    UpdateSlashDir(_fTimeDelta);
    m_fCalculateTime += _fTimeDelta;
    if (m_iActionState == RKS_A_ATT_D3 && (m_fCalculateTime > 0.1f))
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            m_iActionState = RKS_A_ATT_D4;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            //m_bSlashGen = true;
            ActiveSlash(SLASHLDOWN);
        }
    }
    if (m_iActionState == RKS_A_ATT_D4 && (m_fCalculateTime > 0.1f))
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            /*m_iActionState = RKS_A_ATT_L3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);*/
            m_iActionState = RKS_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            return true;
        }
    }
    if (m_iActionState == RKS_A_ATT_L3 && (m_fCalculateTime > 0.1f))
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            m_iActionState = RKS_A_ATT_L4;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            ActiveSlash(SLASHLRIGHT);
        }
    }
    if (m_iActionState == RKS_A_ATT_L4 && (m_fCalculateTime > 0.1f))
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            /*m_iActionState = RKS_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);*/
            m_iActionState = RKS_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            return true;
        }
    }
    if (m_iActionState == RKS_A_ATT_U4 && (m_fCalculateTime > 0.1f))
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            m_iActionState = RKS_A_ATT_U5;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            ActiveSlash(SLASHLUP);
        }
    }
    if (m_iActionState == RKS_A_ATT_U5 && (m_fCalculateTime > 0.1f))
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            /*m_iActionState = RKS_A_ATT_LM1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);*/
            m_iActionState = RKS_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            return true;
        }
    }
    if (m_iActionState == RKS_A_ATT_LM1 && (m_fCalculateTime > 0.1f))
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            m_iActionState = RKS_A_ATT_LM2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            ActiveSlash(SLASHLRIGHTDOWN);
        }
    }
    if (m_iActionState == RKS_A_ATT_LM2 && (m_fCalculateTime > 0.1f))
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            /*m_iActionState = RKS_A_ATT_D3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);*/
            m_iActionState = RKS_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.3f, true);
            return true;
        }
    }
    return false;
}

_bool CRedKnight_Shadow::Update2ComboAttackState(float _fTimeDelta)
{
    if ((m_iActionState == RKS_A_ATT_L1 && m_i2ComboType == 1) || (m_iActionState == RKS_A_ATT_LM1 && m_i2ComboType == 0)
        || (m_iActionState == RKS_A_ATT_D3 && m_i2ComboType == 3) || (m_iActionState == RKS_A_ATT_U4 && m_i2ComboType == 2))
    {
        m_fCalculateTime3 += _fTimeDelta;
    }
    UpdateAttackDir(_fTimeDelta);
    switch (m_i2ComboType)
    {
    case 0:
        if ((m_iActionState == RKS_A_ATT_LM1) && (m_fCalculateTime3 > 0.05f))
        {
            m_fCalculateTime3 = 0.f;
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = RKS_A_ATT_LM2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack1", m_fEffectSize);
            }
        }
        else if (m_iActionState == RKS_A_ATT_LM2)
        {
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 2.f + 0.1f, m_fKeyFrameSpeed * 8.f + 0.1f, m_fLM2SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RKS_A_ATT_U1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
            }
        }
        else if (m_iActionState == RKS_A_ATT_U1)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack2", m_fEffectSize);
                m_iActionState = RKS_A_ATT_U5;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            }
        }
        else if (m_iActionState == RKS_A_ATT_U5)
        {
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 10.f + 0.1f, m_fU5SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RKS_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i2ComboType = 10;
                return true;
            }
        }
        break;
    case 1:
        if ((m_iActionState == RKS_A_ATT_L1) && (m_fCalculateTime3 > 0.05f))
        {
            m_fCalculateTime3 = 0.f;
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack1", m_fEffectSize);
                m_iActionState = RKS_A_ATT_L2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            }
        }
        else if (m_iActionState == RKS_A_ATT_L2)
        {
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 2.f + 0.1f, m_fKeyFrameSpeed * 8.f + 0.1f, m_fL2SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RKS_A_ATT_D1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
            }
        }
        else if (m_iActionState == RKS_A_ATT_D1)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack2", m_fEffectSize);
                m_iActionState = RKS_A_ATT_D2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            }
        }
        else if (m_iActionState == RKS_A_ATT_D2)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = RKS_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i2ComboType = 10;
                return true;
            }
        }
        break;
    case 2:
        if (m_iActionState == RKS_A_ATT_U4 && (m_fCalculateTime3 > 0.05f))
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack2", m_fEffectSize);
                m_fCalculateTime3 = 0.f;
                m_iActionState = RKS_A_ATT_U3;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            }
        }
        else if (m_iActionState == RKS_A_ATT_U3)
        {
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 10.f + 0.1f, m_fU3SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RKS_A_ATT_D1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
            }
        }
        else if (m_iActionState == RKS_A_ATT_D1)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack1", m_fEffectSize);
                m_iActionState = RKS_A_ATT_D2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            }
        }
        else if (m_iActionState == RKS_A_ATT_D2)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = RKS_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i2ComboType = 10;
                return true;
            }
        }
        break;
    }
    return false;
}

_bool CRedKnight_Shadow::Update3ComboAttackState(float _fTimeDelta)
{
    return _bool();
}

void CRedKnight_Shadow::UpdateSpecial1DirSpeed()
{
    _float3 vTargetDirXZ = _float3(m_vTargetDir.x, 0.f, m_vTargetDir.z);
    vTargetDirXZ.Normalize(m_vJumpNormalMoveDir);

    _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
    _float2 vMovePosXZ = _float2(m_vMovePos.x, m_vMovePos.z);
    _float fFastJumpDistance = (vCurrentPosXZ - vMovePosXZ).Length();

    //_float fFastJumpDistance = (m_vCurrentPos - m_vMovePos).Length();
    m_fJumpMoveSpeed = fFastJumpDistance / (m_fKeyFrameSpeed * 20);
}

void CRedKnight_Shadow::StartPattern(_int _iPatternNum, _int _iType)
{
    EFFECTMGR->PlayEffect("Boss_Shadow_Appear", shared_from_this());
    m_iGenType = _iType;
    m_iPatternType = _iPatternNum;
    m_bPatternStart = true;
    SimulationOn();
    GAMEINSTANCE->PlaySoundW("Final_SFX_ShadowOn", 0.5f);
}

void CRedKnight_Shadow::ActiveSlash(_int _iSlashType)
{
    for (int i = 0; i < m_iSlashCount; i++)
    {
        if (_iSlashType == SLASHLDOWN)
        {
            if (!(m_vecSlashLD[i]->IsEnabled()))
            {
                m_vecSlashLD[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos /*+ _float3(0.f, 1.5f, 0.f)*/);
                m_vecSlashLD[i]->UpdateTargetPos();
                m_vecSlashLD[i]->UpdateTargetDir(_float3(0.f, 0.f, 0.f));
                m_vecSlashLD[i]->UpdateDirection();
                m_vecSlashLD[i]->GetTransform()->SetSpeed(m_fFastSlashSpeed);
                m_vecSlashLD[i]->SetProjectileType(PJ_DIRECT);
                m_vecSlashLD[i]->SetEnable(true);
                m_vecSlashLD[i]->OnCollisionProjectile();
                m_vecSlashLD[i]->OnEffect();
                m_vecSlashLD[i]->SetPower(3);
                m_vecSlashLD[i]->SetAbnormal(PJ_A_SHORTHITTED);
                break;
            }
        }
        else if (_iSlashType == SLASHLUP)
        {
            if (!(m_vecSlashLU[i]->IsEnabled()))
            {
                m_vecSlashLU[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos /*+ _float3(0.f, 1.5f, 0.f)*/);
                m_vecSlashLU[i]->UpdateTargetPos();
                m_vecSlashLU[i]->UpdateTargetDir(_float3(0.f, 0.f, 0.f));
                m_vecSlashLU[i]->UpdateDirection();
                m_vecSlashLU[i]->GetTransform()->SetSpeed(m_fFastSlashSpeed);
                m_vecSlashLU[i]->SetProjectileType(PJ_DIRECT);
                m_vecSlashLU[i]->SetEnable(true);
                m_vecSlashLU[i]->OnCollisionProjectile();
                m_vecSlashLU[i]->OnEffect();
                m_vecSlashLU[i]->SetPower(3);
                m_vecSlashLU[i]->SetAbnormal(PJ_A_SHORTHITTED);
                break;
            }
        }
        else if (_iSlashType == SLASHLRIGHT)
        {
            if (!(m_vecSlashLR[i]->IsEnabled()))
            {
                m_vecSlashLR[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos /*+ _float3(0.f, 1.5f, 0.f)*/);
                m_vecSlashLR[i]->UpdateTargetPos();
                m_vecSlashLR[i]->UpdateTargetDir(_float3(0.f, 0.f, 0.f));
                m_vecSlashLR[i]->UpdateDirection();
                m_vecSlashLR[i]->GetTransform()->SetSpeed(m_fLongSlashSpeed);
                m_vecSlashLR[i]->SetProjectileType(PJ_DIRECT);
                m_vecSlashLR[i]->SetEnable(true);
                m_vecSlashLR[i]->OnCollisionProjectile();
                m_vecSlashLR[i]->OnEffect();
                m_vecSlashLR[i]->SetPower(3);
                m_vecSlashLR[i]->SetAbnormal(PJ_A_SHORTHITTED);
                break;
            }
        }
        else if (_iSlashType == SLASHLRIGHTDOWN)
        {
            if (!(m_vecSlashLRD[i]->IsEnabled()))
            {
                m_vecSlashLRD[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos /*+ _float3(0.f, 1.5f, 0.f)*/);
                m_vecSlashLRD[i]->UpdateTargetPos();
                m_vecSlashLRD[i]->UpdateTargetDir(_float3(0.f, 0.f, 0.f));
                m_vecSlashLRD[i]->UpdateDirection();
                m_vecSlashLRD[i]->GetTransform()->SetSpeed(m_fLongSlashSpeed);
                m_vecSlashLRD[i]->SetProjectileType(PJ_DIRECT);
                m_vecSlashLRD[i]->SetEnable(true);
                m_vecSlashLRD[i]->OnCollisionProjectile();
                m_vecSlashLRD[i]->OnEffect();
                m_vecSlashLRD[i]->SetPower(3);
                m_vecSlashLRD[i]->SetAbnormal(PJ_A_SHORTHITTED);
                break;
            }
        }
        else if (_iSlashType == SLASHDOWN)
        {
            if (!(m_vecSlashD[i]->IsEnabled()))
            {
                m_vecSlashD[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos /*+ _float3(0.f, 1.5f, 0.f)*/);
                m_vecSlashD[i]->UpdateTargetPos();
                m_vecSlashD[i]->UpdateTargetDir(_float3(0.f, 0.f, 0.f));
                m_vecSlashD[i]->UpdateDirection();
                m_vecSlashD[i]->GetTransform()->SetSpeed(m_fFFastSlashSpeed);
                m_vecSlashD[i]->SetProjectileType(PJ_DIRECT);
                m_vecSlashD[i]->SetEnable(true);
                m_vecSlashD[i]->OnCollisionProjectile();
                m_vecSlashD[i]->OnEffect();
                m_vecSlashD[i]->SetPower(1);
                m_vecSlashD[i]->SetAbnormal(PJ_A_SHORTHITTED);
                break;
            }
        }
        else if (_iSlashType == SLASHUP)
        {
            if (!(m_vecSlashU[i]->IsEnabled()))
            {
                m_vecSlashU[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos /*+ _float3(0.f, 1.5f, 0.f)*/);
                m_vecSlashU[i]->UpdateTargetPos();
                m_vecSlashU[i]->UpdateTargetDir(_float3(0.f, 0.f, 0.f));
                m_vecSlashU[i]->UpdateDirection();
                m_vecSlashU[i]->GetTransform()->SetSpeed(m_fFFastSlashSpeed);
                m_vecSlashU[i]->SetProjectileType(PJ_DIRECT);
                m_vecSlashU[i]->SetEnable(true);
                m_vecSlashU[i]->OnCollisionProjectile();
                m_vecSlashU[i]->OnEffect();
                m_vecSlashU[i]->SetPower(1);
                m_vecSlashU[i]->SetAbnormal(PJ_A_SHORTHITTED);
                break;
            }
        }
        else if (_iSlashType == SLASHRIGHT)
        {
            if (!(m_vecSlashR[i]->IsEnabled()))
            {
                m_vecSlashR[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos /*+ _float3(0.f, 1.5f, 0.f)*/);
                m_vecSlashR[i]->UpdateTargetPos();
                m_vecSlashR[i]->UpdateTargetDir(_float3(0.f, 0.f, 0.f));
                m_vecSlashR[i]->UpdateDirection();
                m_vecSlashR[i]->GetTransform()->SetSpeed(m_fFFastSlashSpeed);
                m_vecSlashR[i]->SetProjectileType(PJ_DIRECT);
                m_vecSlashR[i]->SetEnable(true);
                m_vecSlashR[i]->OnCollisionProjectile();
                m_vecSlashR[i]->OnEffect();
                m_vecSlashR[i]->SetPower(1);
                m_vecSlashR[i]->SetAbnormal(PJ_A_SHORTHITTED);
                break;
            }
        }
    }
}

_int CRedKnight_Shadow::RandomNormal2ComboStartAttack()
{
    _int iReturnState = 0;
    switch (GAMEINSTANCE->PickRandomInt(0, 3))
    {
    case 0:
        iReturnState = RKS_A_ATT_LM1;
        m_i2ComboType = 0;
        break;
    case 1:
        iReturnState = RKS_A_ATT_L1;
        m_i2ComboType = 1;
        break;
    case 2:
        iReturnState = RKS_A_ATT_U4;
        m_i2ComboType = 2;
        break;
    }
    return iReturnState;
}

_int CRedKnight_Shadow::RandomNormal3ComboStartAttack()
{
    _int iReturnState = 0;
    switch (GAMEINSTANCE->PickRandomInt(0, 4))
    {
    case 0:
        iReturnState = RKS_A_ATT_LM1;
        m_i3ComboType = 0;
        break;
    case 1:
        iReturnState = RKS_A_ATT_L1;
        m_i3ComboType = 1;
        break;
    case 2:
        iReturnState = RKS_A_ATT_U4;
        m_i3ComboType = 2;
        break;
    case 3:
        iReturnState = RKS_A_ATT_D3;
        m_i3ComboType = 3;
        break;
    }
    return iReturnState;
}

void CRedKnight_Shadow::UpdateAttackDir(_float _fTimeDelta)
{
    if (m_fDistanceToTarget < 20.f)
    {
        switch (m_iActionState)
        {
        case RKS_A_ATT_D1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 540.f * _fTimeDelta);
            break;
        case RKS_A_ATT_D3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 540.f * _fTimeDelta);
            break;
        case RKS_A_ATT_L1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 360.f * _fTimeDelta);
            break;
        case RKS_A_ATT_L3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 360.f * _fTimeDelta);
            break;
        case RKS_A_ATT_LM1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 360.f * _fTimeDelta);
            break;
        case RKS_A_ATT_U1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 360.f * _fTimeDelta);
            break;
        case RKS_A_ATT_U4:
            m_pTransformCom->TurnToDir(m_vTargetDir, 360.f * _fTimeDelta);
            break;
        }
    }
    else
    {
        switch (m_iActionState)
        {
        case RKS_A_ATT_D1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RKS_A_ATT_D3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RKS_A_ATT_L1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RKS_A_ATT_L3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RKS_A_ATT_LM1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RKS_A_ATT_U1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RKS_A_ATT_U4:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        }
    }
}

void CRedKnight_Shadow::UpdateSlashDir(_float _fTimeDelta)
{
    if (m_fDistanceToTarget < 20.f)
    {
        switch (m_iActionState)
        {
        case RKS_A_ATT_D3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case RKS_A_ATT_D4:
            //m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RKS_A_ATT_L3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case RKS_A_ATT_L4:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RKS_A_ATT_U4:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case RKS_A_ATT_U5:
            //m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        }
    }
    else
    {
        switch (m_iActionState)
        {
        case RKS_A_ATT_D3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RKS_A_ATT_D4:
            //m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RKS_A_ATT_L3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RKS_A_ATT_L4:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RKS_A_ATT_U4:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RKS_A_ATT_U5:
            //m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        }
    }
}

_float3 CRedKnight_Shadow::RandomBlinkPos()
{
    _float3 vTargetDir = _float3((float)GAMEINSTANCE->PickRandomInt(-100, 100), 0.f, (float)GAMEINSTANCE->PickRandomInt(-100, 100));
    vTargetDir.Normalize();
    
    _float3 vPos = m_vTargetPos + vTargetDir * 2.f;
    _float3 vReturnPos = CalculateBindary(vPos);

    return vReturnPos;
}

_float3 CRedKnight_Shadow::RandomBlinkSlashPos()
{
    _float3 vTargetDir = _float3((float)GAMEINSTANCE->PickRandomInt(-100, 100), 0.f, (float)GAMEINSTANCE->PickRandomInt(-100, 100));
    vTargetDir.Normalize();

    _float3 vReturnPos;

    if (m_iGenType == 0)
    {
        _float3 vPos = m_vGenBossPos + vTargetDir * 10.f;
        vReturnPos = CalculateBindary(vPos);
    }
    else
    {
        _float3 vPos = m_vTargetPos + vTargetDir * 8.f;
        vReturnPos = CalculateBindary(vPos);
    }

    return vReturnPos;
}

_float3 CRedKnight_Shadow::RandomBlinkJumpAttackPos()
{
    _float3 vTargetDir = _float3((float)GAMEINSTANCE->PickRandomInt(-100, 100), 0.f, (float)GAMEINSTANCE->PickRandomInt(-100, 100));
    vTargetDir.Normalize();

    _float3 vReturnPos;
    if (m_iGenType == 0)
    {
        _float3 vPos = m_vGenBossPos + vTargetDir * 5.f;
        vReturnPos = CalculateBindary(vPos);
    }
    else
    {
        _float3 vPos = _float3(20.f, -25.f, 205.f);
        vPos.y = m_vTargetPos.y;
        vReturnPos = CalculateBindary(vPos);
    }

    return vReturnPos;
}

void CRedKnight_Shadow::UpdateMoveSlide(_float _fSlideStart, _float _fSlideEnd, _float _fSpeed, _float _fTimeDelta)
{
    if ((m_fCalculateTime > (_fSlideStart / m_fAnimSpeed)) && (m_fCalculateTime < (_fSlideEnd / m_fAnimSpeed)))
    {
        MovePos(m_vNormLookDirXZ * _fTimeDelta * _fSpeed + m_vCurrentPos);
    }
}

_float3 CRedKnight_Shadow::RandomBlinkDashPos()
{
    _float3 vTargetDir = _float3((float)GAMEINSTANCE->PickRandomInt(-100, 100), 0.f, (float)GAMEINSTANCE->PickRandomInt(-100, 100));
    vTargetDir.Normalize();

    _float3 vReturnPos;
    if (m_iGenType == 0)
    {
        _float3 vPos = m_vGenBossPos + vTargetDir * 5.f;
        vReturnPos = CalculateBindary(vPos);
    }
    else
    {
        _float3 vPos = _float3(20.f, -25.f, 265.f);
        vPos.y = m_vTargetPos.y;
        vReturnPos = CalculateBindary(vPos);
    }

    return vReturnPos;
}
