#include "Orc_Shaman.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "Animation.h"
#include "EffectMgr.h"
#include "PlayerProjectile.h"
#include "FireBall.h"

COrc_Shaman::COrc_Shaman()
    : CMonster()
{
}

COrc_Shaman::~COrc_Shaman()
{
}

HRESULT COrc_Shaman::Initialize()
{
    shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();

    pArg->fSpeedPerSec = 1.f;
    pArg->fRotationPerSec = XMConvertToRadians(90.0f);

    shared_ptr<MONSTER_DESC> pMonsterDesc = make_shared<CMonster::MONSTER_DESC>();
    pMonsterDesc->pTarget = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
    __super::Initialize(pMonsterDesc.get(), pArg.get());

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_ANIM;
    m_iShaderPass = 0;

    m_pModelCom = GAMEINSTANCE->GetModel("M_Orc_Shaman");
    m_strModelName = "M_Orc_Shaman";
    AddComponent(L"Com_Model", m_pModelCom);
    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

    m_pModelCom->ChangeAnim(m_iActionState);

    if (FAILED(AddPartObjects("IT_N_OrcShaman")))
        return E_FAIL;

    shared_ptr<CTransform::TRANSFORM_DESC> pTransformDesc = make_shared<CTransform::TRANSFORM_DESC>();

    // 화염구마법 생성후 넣어주기
    shared_ptr<CFireBall::FIREBALLMAGIC_DESC> pFireBallDesc = make_shared<CFireBall::FIREBALLMAGIC_DESC>();
    pFireBallDesc->tProjectile.pTarget = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
    pFireBallDesc->pOwner = shared_from_this();

    pTransformDesc->fSpeedPerSec = 10.f;
    pTransformDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    for (int j = 0; j < m_iFireBallCount; j++)
    {
        shared_ptr<CFireBall> pFireBall = CFireBall::Create(pFireBallDesc.get(), pTransformDesc.get());
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, TEXT("Layer_Projectile"), pFireBall)))
            return E_FAIL;
        m_vecFireBall.push_back(pFireBall);
    }

    // 스탯
    m_fHp = 100.f;
    m_fMaxHp = 100.f;
    m_fSpeed = 1.5f;

    AddRigidBody("M_Orc_Shaman");
    m_pRigidBody->SimulationOff();
    //m_pRigidBody->SimulationOn();

    m_vLockOnOffSetPosition = { 0.0f,1.5f,0.0f };

    /* Create World UI */
    PrepareMonsterHPBar(TEXT("오크 주술사"));

    return S_OK;
}

void COrc_Shaman::PriorityTick(_float _fTimeDelta)
{
   /* if (GAMEINSTANCE->KeyDown(DIK_T))
    {
        m_fHp -= 30.f;
    }*/

    __super::PriorityTick(_fTimeDelta);
}

void COrc_Shaman::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    // Base상태에 따른 FSM
    switch (m_iBaseState)
    {
    case OC_SH_B_BEFOREMEET:
        FSM_BeforeMeet(_fTimeDelta);
        break;
    case OC_SH_B_GENERATE:
        FSM_Generate(_fTimeDelta);
        break;
    case OC_SH_B_NORMAL:
        FSM_Normal(_fTimeDelta);
        break;
    case OC_SH_B_HITTED:
        FSM_Hitted(_fTimeDelta);
        break;
    case OC_SH_B_HITTED2:
        FSM_Hitted2(_fTimeDelta);
        break;
    case OC_SH_B_HITTED3:
        FSM_Hitted3(_fTimeDelta);
        break;
    case OC_SH_B_STUN:
        FSM_Stun(_fTimeDelta);
        break;
    case OC_SH_B_SHOCKED:
        FSM_Shocked(_fTimeDelta);
        break;
    case OC_SH_B_DOWN:
        FSM_Down(_fTimeDelta);
        break;
    case OC_SH_B_DEATH:
        FSM_Death(_fTimeDelta);
        break;
    }

    if (m_iActionState == OC_SH_A_BLOCKED && !(m_iActionState == OC_SH_A_FRONTBE))
    {
        m_pModelCom->PlayAnimation(0.8f * _fTimeDelta);
    }
    else if (m_iActionState == OC_SH_A_FRONTBE)
    {
        switch (m_iBaseState)
        {
        case OC_SH_B_HITTED:
            m_pModelCom->PlayAnimation(1.f * _fTimeDelta);
            break;
        case OC_SH_B_HITTED2:
            m_pModelCom->PlayAnimation(0.6f * _fTimeDelta);
            break;
        case OC_SH_B_HITTED3:
            m_pModelCom->PlayAnimation(0.3f * _fTimeDelta);
            break;
        case OC_SH_B_DOWN:
            m_pModelCom->PlayAnimation(1.2f * _fTimeDelta);
            break;
        }
    }
    else
        m_pModelCom->PlayAnimation(_fTimeDelta);

    m_pTransformCom->Tick(_fTimeDelta);
}

void COrc_Shaman::LateTick(_float _fTimeDelta)
{
    m_pModelCom->CheckDisableAnim();
    m_pTransformCom->LateTick(_fTimeDelta);

    __super::LateTick(_fTimeDelta);

    if (m_bDslv)
        UpdateDslv(_fTimeDelta);

    if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 3.f))
    {
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
        GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());


#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
    }
}

HRESULT COrc_Shaman::Render()
{
    __super::Render();

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++) {
        if (m_bDslv)
        {
            if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &m_bDslv, sizeof(_bool))))
                return E_FAIL;
            _float g_fDslvValue = m_fDslvTime / m_fMaxDslvTime;
            if (FAILED(GAMEINSTANCE->BindRawValue("g_fDslvValue", &g_fDslvValue, sizeof(_float))))
                return E_FAIL;
            if (FAILED(GAMEINSTANCE->BindRawValue("g_fDslvThick", &m_fDslvThick, sizeof(_float))))
                return E_FAIL;
            if (FAILED(GAMEINSTANCE->BindRawValue("g_vDslvColor", &m_vDslvColor, sizeof(_float4))))
                return E_FAIL;
            if (FAILED(GAMEINSTANCE->BindSRV("g_DslvTexture", m_strDslvTexKey)))
                return E_FAIL;
        }

        GAMEINSTANCE->BeginAnimModel(m_pModelCom, (_uint)i);
    }
    if (m_bUsingRimLight)
    {
        _bool bRimOff = false;
        GAMEINSTANCE->BindRawValue("g_IsUsingRim", &bRimOff, sizeof(bRimOff));
    }

    _bool bDslvFalse = false;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &bDslvFalse, sizeof(_bool))))
        return E_FAIL;

    return S_OK;
}

shared_ptr<COrc_Shaman> COrc_Shaman::Create(_float3 _vCreatePos)
{
    shared_ptr<COrc_Shaman> pInstance = make_shared<COrc_Shaman>();

    pInstance->SetCurrentPos(_vCreatePos); // 위치 지정
    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : COrc_Shaman");

    return pInstance;
}

_bool COrc_Shaman::IsParried()
{
    // OC_SH_A_ATTACK1할때 패링가능
    if ((m_iActionState == OC_SH_A_ATTACK1) && dynamic_pointer_cast<CWeapon>(m_pMonsterParts.begin()->second)->GetParried())
    {
        //OffCollisionWeapon();
        return true;
    }
    else
        return false;
}

HRESULT COrc_Shaman::AddRigidBody(string _strModelKey)
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
    CapsuleDesc.fHeight = 1.5f;
    CapsuleDesc.fRadius = 0.25f;
    _float3 vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
    CapsuleDesc.vOffSetPosition = { 0.0f, 0.9f, 0.0f };
    CapsuleDesc.vOffSetRotation = { 0.0f, 0.0f, 90.0f };
    CapsuleDesc.strShapeTag = "Orc_Shaman_Body";
    RigidBodyDesc.pGeometry = &CapsuleDesc;

    RigidBodyDesc.eThisColFlag = COL_MONSTER;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER | COL_PLAYERWEAPON | COL_PLAYERPROJECTILE | COL_PLAYERSKILL | COL_CHECKBOX | COL_MONSTER;
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

void COrc_Shaman::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    if (_ColData.eColFlag == COL_STATIC || _ColData.eColFlag == COL_PLAYER || _ColData.eColFlag == COL_CHECKBOX || _ColData.eColFlag & COL_MONSTER)
        return;

    // 일반피격
    if ((m_iActionState == OC_SH_A_STDALT) || (m_iActionState == OC_SH_A_RUN) || (m_iActionState == OC_SH_A_BLOCKED)
        || ((m_iActionState == OC_SH_A_FRONTBE) && ((m_iBaseState != OC_SH_B_HITTED2) && (m_iBaseState != OC_SH_B_HITTED3))))
    {
        if ((_ColData.eColFlag == COL_PLAYERWEAPON) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_1") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_2") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_3")))
        {
            m_bHitted = true;
        }
        if ((_ColData.eColFlag == COL_PLAYERSKILL) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack2")))
        {
            m_bHitted = true;
        }

        if (_ColData.eColFlag & COL_PLAYERPROJECTILE)
        {
            PlayerProjAbnormal iAbnormal = dynamic_pointer_cast<CPlayerProjectile>(_ColData.pGameObject.lock())->GetAbnormal();

            if (iAbnormal == PLAYER_PJ_A_NORMAL)
            {
                m_bHitted = true;
            }
        }
    }

    // 짧은경직
    if ((m_iBaseState == OC_SH_B_NORMAL) || (m_iBaseState == OC_SH_B_HITTED) || (m_iBaseState == OC_SH_B_HITTED2))
    {
        if ((_ColData.eColFlag == COL_PLAYERSKILL) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack3") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_CM_SW2_DashAttack") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_1") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_2") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_3")))
        {
            m_bHitted2 = true;
        }

        if (_ColData.eColFlag & COL_PLAYERPROJECTILE)
        {
            PlayerProjAbnormal iAbnormal = dynamic_pointer_cast<CPlayerProjectile>(_ColData.pGameObject.lock())->GetAbnormal();

            if (iAbnormal == PLAYER_PJ_A_STIFF_SHORT)
            {
                m_bHitted2 = true;
            }
        }
    }

    // 긴경직
    if ((m_iBaseState == OC_SH_B_NORMAL) || (m_iBaseState == OC_SH_B_HITTED)
        || (m_iBaseState == OC_SH_B_HITTED2) || (m_iBaseState == OC_SH_B_HITTED3))
    {
        if (_ColData.eColFlag & COL_PLAYERPROJECTILE)
        {
            PlayerProjAbnormal iAbnormal = dynamic_pointer_cast<CPlayerProjectile>(_ColData.pGameObject.lock())->GetAbnormal();

            if (iAbnormal == PLAYER_PJ_A_STIFF_LONG)
            {
                m_bHitted3 = true;
            }
        }
    }

    // 스턴
    if ((m_iBaseState == OC_SH_B_NORMAL) || (m_iBaseState == OC_SH_B_HITTED)
        || (m_iBaseState == OC_SH_B_HITTED2) || (m_iBaseState == OC_SH_B_HITTED3) || (m_iBaseState == OC_SH_B_STUN))
    {
        if ((_ColData.eColFlag == COL_PLAYERSKILL) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_GaiaCrash_Legend")))
        {
            m_bStun = true;
        }

        if (_ColData.eColFlag & COL_PLAYERPROJECTILE)
        {
            PlayerProjAbnormal iAbnormal = dynamic_pointer_cast<CPlayerProjectile>(_ColData.pGameObject.lock())->GetAbnormal();

            if (iAbnormal == PLAYER_PJ_A_STUN)
            {
                m_bStun = true;
            }
        }
    }

    // 감전
    if ((m_iBaseState == OC_SH_B_NORMAL) || (m_iBaseState == OC_SH_B_HITTED)
        || (m_iBaseState == OC_SH_B_HITTED2) || (m_iBaseState == OC_SH_B_HITTED3) || (m_iBaseState == OC_SH_B_STUN)
        || (m_iBaseState == OC_SH_B_SHOCKED))
    {
        if (_ColData.eColFlag & COL_PLAYERPROJECTILE)
        {
            PlayerProjAbnormal iAbnormal = dynamic_pointer_cast<CPlayerProjectile>(_ColData.pGameObject.lock())->GetAbnormal();

            if (iAbnormal == PLAYER_PJ_A_SHOCK)
            {
                m_bShocked = true;
            }
        }
    }

    // 넘어뜨리기
    if ((m_iBaseState == OC_SH_B_NORMAL) || (m_iBaseState == OC_SH_B_HITTED)
        || (m_iBaseState == OC_SH_B_HITTED2) || (m_iBaseState == OC_SH_B_HITTED3) || (m_iBaseState == OC_SH_B_STUN)
        || (m_iBaseState == OC_SH_B_SHOCKED) || (m_iBaseState == OC_SH_B_DOWN))
    {
        if ((_ColData.eColFlag == COL_PLAYERWEAPON) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_PowerAttack")))
        {
            m_bDown = true;
        }
        if ((_ColData.eColFlag == COL_PLAYERSKILL) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_UpperAttack")))
        {
            m_bDown = true;
        }
        if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_ActionSkill_ContextSkill")))
        {
            m_bDown = true;
        }

        if (_ColData.eColFlag & COL_PLAYERPROJECTILE)
        {
            PlayerProjAbnormal iAbnormal = dynamic_pointer_cast<CPlayerProjectile>(_ColData.pGameObject.lock())->GetAbnormal();

            if (iAbnormal == PLAYER_PJ_A_DOWN)
            {
                m_bDown = true;
            }
        }
    }

    // 패링
    if ((_szMyShapeTag == m_strATT1Collider1) && (_ColData.eColFlag == COL_PLAYERPARRYING))
    {
        m_bParried = true;
    }

    // 무적기일때는 제외하고 체력감소
    if (((m_iBaseState == OC_SH_B_NORMAL) || (m_iBaseState == OC_SH_B_HITTED)
        || (m_iBaseState == OC_SH_B_HITTED2) || (m_iBaseState == OC_SH_B_HITTED3) || (m_iBaseState == OC_SH_B_STUN)
        || (m_iBaseState == OC_SH_B_SHOCKED) || (m_iBaseState == OC_SH_B_DOWN))
        && !(_ColData.eColFlag == COL_PLAYERPARRYING))
    {
        //m_fHp -= 0.f;
        ToggleHPBar();

        DamagedByPlayer(_ColData, _szMyShapeTag);

        __super::ShowEffect(_ColData, _szMyShapeTag);
    }
}

void COrc_Shaman::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void COrc_Shaman::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void COrc_Shaman::EnableCollision(const char* _strShapeTag)
{
    m_pRigidBody->EnableCollision(_strShapeTag);
}

void COrc_Shaman::DisableCollision(const char* _strShapeTag)
{
    m_pRigidBody->DisableCollision(_strShapeTag);
}

void COrc_Shaman::ActiveFireBall()
{
    for (int i = 0; i < m_iFireBallCount; i++)
    {
        if (!(m_vecFireBall[i]->IsEnabled()))
        {
            _float3 m_vLookDir = m_pTransformCom->GetState(CTransform::STATE_LOOK);
            _float3 m_vRightDir = m_pTransformCom->GetState(CTransform::STATE_RIGHT);
            m_vecFireBall[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos + m_vLookDir * 1.f + m_vRightDir * -0.5f + _float3(0.f, 1.4f, 0.f));
            m_vecFireBall[i]->UpdateTargetPos();
            m_vecFireBall[i]->UpdateTargetDir(_float3(0.f, 1.5f, 0.f));
            m_vecFireBall[i]->UpdateDirection();
            m_vecFireBall[i]->GetTransform()->SetSpeed(10.f);
            m_vecFireBall[i]->SetProjectileType(PJ_DIRECT);
            m_vecFireBall[i]->SetEnable(true);
            m_vecFireBall[i]->OnCollisionProjectile();
            m_vecFireBall[i]->OnEffect();
            m_vecFireBall[i]->SetPower(2);
            m_vecFireBall[i]->SetAbnormal(PJ_A_SHORTHITTED);
            
            EFFECTMGR->PlayEffect("Mon_O_Fire", m_vecFireBall[i]);
            GAMEINSTANCE->PlaySoundW("OSH_Fire", 0.5f);
            break;
        }
    }
}

HRESULT COrc_Shaman::AddPartObjects(string _strModelKey)
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


void COrc_Shaman::UpdateDistanceState()
{
    // 위치 바로 못받아오는거 때문에 우선
    if (m_vTargetPos == _float3(0.f, 0.f, 0.f))
    {
        m_iDistanceState = OC_SH_D_BEFOREMEET;
        return;
    }

    if (m_iDistanceState == OC_SH_D_BEFOREMEET)
    {
        if (m_fDistanceToTarget < m_fMeetRange)
        {
            m_iDistanceState = OC_SH_D_MOVERANGE;
        }
        else
        {
            m_iDistanceState = OC_SH_D_BEFOREMEET;
        }
    }
    else
    {
        if (m_fDistanceToTarget < m_fAttack1Range)
        {
            m_iDistanceState = OC_SH_D_ATTACK1RANGE;
        }
        else if (m_fDistanceToTarget < m_fAttack2Range)
        {
            m_iDistanceState = OC_SH_D_ATTACK2RANGE;
        }
        else if (m_fDistanceToTarget >= m_fAttack2Range && m_fDistanceToTarget < m_fOutRange)
        {
            m_iDistanceState = OC_SH_D_MOVERANGE;
        }
        else
        {
            m_iDistanceState = OC_SH_D_OUTRANGE;
        }
    }
}

void COrc_Shaman::FSM_BeforeMeet(_float _fTimeDelta)
{
    UpdateDistanceState();

    // 가까워지면
    if (m_iDistanceState < OC_SH_D_OUTRANGE)
    {
        m_iBaseState = OC_SH_B_GENERATE;
        m_iActionState = OC_SH_A_COMBATSTART;
        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
    }
    // 멀때
    else
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            switch (GAMEINSTANCE->PickRandomInt(0, 5))
            {
            case 0:
                m_iActionState = OC_SH_A_SEARCH;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                break;
            case 1:
                m_iActionState = OC_SH_A_STDALT1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                break;
            case 2:
                m_iActionState = OC_SH_A_STDALT2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                break;
            case 3:
                m_iActionState = OC_SH_A_STDALT3;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                break;
            case 4:
                m_iActionState = OC_SH_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                break;
            }
        }
    }
}

void COrc_Shaman::FSM_Generate(_float _fTimeDelta)
{
    UpdateDirection();
    if (m_iActionState == OC_SH_A_COMBATSTART)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iBaseState = OC_SH_B_NORMAL;
            m_iActionState = OC_SH_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
            m_iDistanceState = OC_SH_D_MOVERANGE;
        }
    }
}

void COrc_Shaman::FSM_Normal(_float _fTimeDelta)
{
    UpdateDirection();

    switch (m_iDistanceState)
    {
    case OC_SH_D_MOVERANGE:
        if (m_iActionState == OC_SH_A_STDALT)
        {
            m_iActionState = OC_SH_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
        }
        else if (m_iActionState == OC_SH_A_RUN)
        {
            UpdateCoolTimeRange(_fTimeDelta);

            MoveToTarget(_fTimeDelta);
            UpdateDistanceState();
        }
        else
        {
            m_iActionState = OC_SH_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        break;
    case OC_SH_D_ATTACK1RANGE:
        if (m_iActionState == OC_SH_A_RUN)
        {
            MoveToTarget(_fTimeDelta);
            UpdateDistanceState();
            UpdateCoolTimeMelee(_fTimeDelta);

            for (int i = OC_SH_C_NORMALATTACK1; i < OC_SH_C_NORMALATTACK1 + 1; i++)
            {
                if (m_bAttackReady[i])
                {
                    m_iActionState = i + (OC_SH_A_ATTACK1 - OC_SH_C_NORMALATTACK1);
                    m_pModelCom->ChangeAnim(m_iActionState, 0.2f);
                    m_bAttackReady[i] = false;
                    m_iDistanceState = OC_SH_D_ATTACK1RANGE;
                    break;
                }
            }
        }
        if (m_iActionState == OC_SH_A_STDALT)
        {
            UpdateDistanceState();
            UpdateCoolTimeMelee(_fTimeDelta);

            for (int i = OC_SH_C_NORMALATTACK1; i < OC_SH_C_NORMALATTACK1 + 1; i++)
            {
                if (m_bAttackReady[i])
                {
                    m_iActionState = i + (OC_SH_A_ATTACK1 - OC_SH_C_NORMALATTACK1);
                    m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                    m_bAttackReady[i] = false;
                    m_iDistanceState = OC_SH_D_ATTACK1RANGE;

                    if (m_iActionState == OC_SH_A_ATTACK1)
                        SetWeaponRimLight(true);
                    break;
                }
            }
        }
        if (m_iActionState == OC_SH_A_ATTACK1)
        {
            if (m_bParried)
            {
                GAMEINSTANCE->PlaySoundW("OSH_Parried", m_fVoiceSize);
                SetWeaponRimLight(false);
                m_bParried = false;
                m_iActionState = OC_SH_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                m_iParriedPower = PARRIEDSTRONG;
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                SetWeaponRimLight(false);
                //OffCollisionWeapon();
                m_iActionState = OC_SH_A_STDALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.2f, true);
                UpdateDistanceState();
            }
        }
        if (m_iActionState == OC_SH_A_BLOCKED)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = OC_SH_A_STDALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.2f, true);
                UpdateDistanceState();
            }
        }
        break;

    case OC_SH_D_ATTACK2RANGE:
        if (m_iActionState == OC_SH_A_RUN)
        {
            MoveToTarget(_fTimeDelta);
            UpdateDistanceState();
            UpdateCoolTimeRange(_fTimeDelta);
            if(m_fDistanceToTarget < m_fAttack1CoolTimeRange)
                UpdateCoolTimeMelee(_fTimeDelta);

            for (int i = OC_SH_C_NORMALATTACK2; i < OC_SH_C_NORMALATTACK2 + 2; i++)
            {
                if (m_bAttackReady[i])
                {
                    m_iActionState = i + (OC_SH_A_ATTACK2 - OC_SH_C_NORMALATTACK2);
                    m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                    m_bAttackReady[i] = false;
                    m_iDistanceState = OC_SH_D_ATTACK2RANGE;
                    break;
                }
            }
        }
        if (m_iActionState == OC_SH_A_STDALT)
        {
            UpdateDistanceState();
            UpdateCoolTimeRange(_fTimeDelta);
            if (m_fDistanceToTarget < m_fAttack1CoolTimeRange)
                UpdateCoolTimeMelee(_fTimeDelta);

            for (int i = OC_SH_C_NORMALATTACK2; i < OC_SH_C_NORMALATTACK2 + 2; i++)
            {
                if (m_bAttackReady[i])
                {
                    m_fCalculateTime = 0.f;
                    m_iActionState = i + (OC_SH_A_ATTACK2 - OC_SH_C_NORMALATTACK2);
                    m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                    m_bAttackReady[i] = false;
                    m_iDistanceState = OC_SH_D_ATTACK2RANGE;

                    if (m_iActionState == OC_SH_A_ATTACK2)
                    {
                        m_bEffectReady = true;
                    }

                    break;
                }
            }
        }
        if (m_iActionState == OC_SH_A_ATTACK2)
        {
            if (m_bEffectReady)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > 0.5f)
                {
                    m_fCalculateTime = 0.f;
                    m_bEffectReady = false;
                    EFFECTMGR->PlayEffect("Mon_O_Buff", shared_from_this());
                }
            }
            m_fCalculateTime2 += _fTimeDelta;
            if (m_fCalculateTime2 > m_fBuffStartTime && !m_bBuffSoundOn)
            {
                GAMEINSTANCE->PlaySoundW("OSH_Buffea", m_fEffectSize);
                m_bBuffSoundOn = true;
            }

            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime2 = 0.f;
                m_bBuffSoundOn = false;
                m_iActionState = OC_SH_A_STDALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.4f, true);
                UpdateDistanceState();
            }
        }
        if (m_iActionState == OC_SH_A_ATTACK3)
        {
            m_fCalculateTime += _fTimeDelta;
            if (!m_bFireBallFire1)
            {
                if (m_fCalculateTime > m_fFireBallStartTime1)
                {
                    ActiveFireBall();
                    m_bFireBallFire1 = true;
                }
            }

            if (!m_bFireBallFire2)
            {
                if (m_fCalculateTime > m_fFireBallStartTime2)
                {
                    ActiveFireBall();
                    m_bFireBallFire2 = true;
                }
            }

            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_bFireBallFire1 = false;
                m_bFireBallFire2 = false;
                m_iActionState = OC_SH_A_STDALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.4f, true);
                UpdateDistanceState();
            }
        }
        break;
    case OC_SH_D_OUTRANGE:
        break;
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void COrc_Shaman::FSM_Hitted(_float _fTimeDelta)
{
    if (m_iActionState == OC_SH_A_FRONTBE)
    {
        if (m_bHitted)
        {
            UpdateDirection();
            m_iActionState = OC_SH_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
            m_bHitted = false;
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iBaseState = OC_SH_B_NORMAL;
            m_iActionState = OC_SH_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_bHitted = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void COrc_Shaman::FSM_Hitted2(_float _fTimeDelta)
{
    if (m_iActionState == OC_SH_A_FRONTBE)
    {
        if (m_bHitted2)
        {
            //UpdateDirection();
            m_iActionState = OC_SH_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
            m_bHitted2 = false;
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iBaseState = OC_SH_B_NORMAL;
            m_iActionState = OC_SH_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_bHitted2 = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void COrc_Shaman::FSM_Hitted3(_float _fTimeDelta)
{
    if (m_iActionState == OC_SH_A_FRONTBE)
    {
        if (m_bHitted3)
        {
            UpdateDirection();
            m_iActionState = OC_SH_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
            m_bHitted3 = false;
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iBaseState = OC_SH_B_NORMAL;
            m_iActionState = OC_SH_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_bHitted3 = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void COrc_Shaman::FSM_Stun(_float _fTimeDelta)
{
    if (m_iActionState == OC_SH_A_STUN)
    {
        if (m_bStun)
        {
            UpdateDirection();
            m_iActionState = OC_SH_A_STUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.3f, false);
            m_bStun = false;
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iBaseState = OC_SH_B_NORMAL;
            m_iActionState = OC_SH_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.3f, true);
            m_bStun = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void COrc_Shaman::FSM_Shocked(_float _fTimeDelta)
{
    if (m_iActionState == OC_SH_A_SHOCKED)
    {
        if (m_bShocked)
        {
            UpdateDirection();
            m_iActionState = OC_SH_A_SHOCKED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
            m_bShocked = false;
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iBaseState = OC_SH_B_NORMAL;
            m_iActionState = OC_SH_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_bShocked = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void COrc_Shaman::FSM_Down(_float _fTimeDelta)
{
    if (m_iActionState == OC_SH_A_DOWN1)
    {
        if (m_bDown)
        {
            UpdateDirection();
            m_fCalculateTime = 0.f;
            m_bDown = false;
            m_iActionState = OC_SH_A_DOWN1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iActionState = OC_SH_A_DOWN2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == OC_SH_A_DOWN2)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iActionState = OC_SH_A_DOWN3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == OC_SH_A_DOWN3)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iBaseState = OC_SH_B_NORMAL;
            m_iActionState = OC_SH_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_bDown = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void COrc_Shaman::FSM_Death(_float _fTimeDelta)
{
    if ((m_iActionState == OC_SH_A_DEATH) && (m_pModelCom->GetIsFinishedAnimation()))
    {
        m_iActionState = OC_SH_A_DEAD;
        m_pModelCom->ChangeAnim(m_iActionState, 0.01f, false);

        SetDissolve(true, true, m_fDeadDisolveTime);
        SetDissolveInfo(1.f, m_vDisolveColor, "Noise_3003");

        m_bDeadStart = true;
        m_fCalculateTime = 0.f;
    }
    if (m_bDeadStart)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_fCalculateTime > m_fDeadDisolveTime)
        {
            m_fCalculateTime = 0.f;
            m_bDeadStart = false;

            m_IsEnabled = false;
        }
    }
}

void COrc_Shaman::CheckDead()
{
    if (m_fHp <= 0.f)
    {
        GAMEINSTANCE->PlaySoundW("OSH_Dead", m_fEffectSize);
        SetWeaponRimLight(false);
        m_bDead = true;
        m_iBaseState = OC_SH_B_DEATH;
        m_iActionState = OC_SH_A_DEATH;
        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        m_pRigidBody->SimulationOff();

        QUESTMGR->CheckDeadMonster(shared_from_this());
    }
}

void COrc_Shaman::CheckAttacked()
{
    // 피격시
    if (m_bHitted)
    {
        GAMEINSTANCE->PlaySoundW("OSH_Hitted", m_fEffectSize);
        SetWeaponRimLight(false);
        m_iBaseState = OC_SH_B_HITTED;
        m_iActionState = OC_SH_A_FRONTBE;
    }

    // 짧은경직
    if (m_bHitted2)
    {
        GAMEINSTANCE->PlaySoundW("OSH_Hitted", m_fEffectSize);
        SetWeaponRimLight(false);
        m_iBaseState = OC_SH_B_HITTED2;
        m_iActionState = OC_SH_A_FRONTBE;
    }

    // 긴경직
    if (m_bHitted3)
    {
        GAMEINSTANCE->PlaySoundW("OSH_Hitted", m_fEffectSize);
        SetWeaponRimLight(false);
        m_iBaseState = OC_SH_B_HITTED3;
        m_iActionState = OC_SH_A_FRONTBE;
    }

    // 스턴
    if (m_bStun)
    {
        GAMEINSTANCE->PlaySoundW("OSH_Stun", m_fEffectSize);
        SetWeaponRimLight(false);
        m_iBaseState = OC_SH_B_STUN;
        m_iActionState = OC_SH_A_STUN;
    }

    // 감전
    if (m_bShocked)
    {
        GAMEINSTANCE->PlaySoundW("OSH_Shocked", m_fEffectSize);
        SetWeaponRimLight(false);
        m_iBaseState = OC_SH_B_SHOCKED;
        m_iActionState = OC_SH_A_SHOCKED;
    }

    // 넘어지기
    if (m_bDown)
    {
        GAMEINSTANCE->PlaySoundW("OSH_KnockDown", m_fEffectSize);
        SetWeaponRimLight(false);
        m_iBaseState = OC_SH_B_DOWN;
        m_iActionState = OC_SH_A_DOWN1;
    }
}

void COrc_Shaman::UpdateCoolTimeRange(_float _fTimeDelta)
{
    for (int i = OC_SH_C_NORMALATTACK2; i < OC_SH_C_NORMALATTACK2 + 2; i++)
    {
        if (!m_bAttackReady[i])
        {
            m_fAttackCoolTime[i] -= _fTimeDelta;
            if (m_fAttackCoolTime[i] < 0.f)
            {
                m_bAttackReady[i] = true;
                m_fAttackCoolTime[i] = m_fBAttackCoolTime[i];
                for (int j = OC_SH_C_NORMALATTACK2; j < OC_SH_C_NORMALATTACK2 + 2; j++)
                {
                    if (j != i)
                    {
                        m_fAttackCoolTime[j] += 1.f;
                    }
                }
                break;
            }
        }
    }
}

void COrc_Shaman::UpdateCoolTimeMelee(_float _fTimeDelta)
{
    for (int i = OC_SH_C_NORMALATTACK1; i < OC_SH_C_NORMALATTACK1 + 1; i++)
    {
        if (!m_bAttackReady[i])
        {
            m_fAttackCoolTime[i] -= _fTimeDelta;
            if (m_fAttackCoolTime[i] < 0.f)
            {
                m_bAttackReady[i] = true;
                m_fAttackCoolTime[i] = m_fBAttackCoolTime[i];
                for (int j = OC_SH_C_NORMALATTACK1; j < OC_SH_C_NORMALATTACK1 + 1; j++)
                {
                    if (j != i)
                    {
                        m_fAttackCoolTime[j] += 1.f;
                    }
                }
                break;
            }
        }
    }
}
