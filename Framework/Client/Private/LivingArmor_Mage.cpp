#include "LivingArmor_Mage.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "Animation.h"
#include "EffectMgr.h"
#include "PlayerProjectile.h"
#include "ElectricBall.h"
#include "QuestMgr.h"

CLivingArmor_Mage::CLivingArmor_Mage()
    : CMonster()
{
}

CLivingArmor_Mage::~CLivingArmor_Mage()
{
}

HRESULT CLivingArmor_Mage::Initialize()
{
    shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
    
    pArg->fSpeedPerSec = 1.f;
    pArg->fRotationPerSec = XMConvertToRadians(1.f);

    shared_ptr<MONSTER_DESC> pMonsterDesc = make_shared<CMonster::MONSTER_DESC>();
    pMonsterDesc->pTarget = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));

    __super::Initialize(pMonsterDesc.get(), pArg.get());

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_ANIM;
    m_iShaderPass = 0;

    m_pModelCom = GAMEINSTANCE->GetModel("LivingArmor_Mage");
    m_strModelName = "LivingArmor_Mage";
    AddComponent(L"Com_Model", m_pModelCom);
    m_pTransformCom->SetScaling(1.5f, 1.5f, 1.5f);

    m_pModelCom->ChangeAnim(m_iActionState);

    // 스탯
    m_fHp = 300.f;
    m_fMaxHp = 300.f;
    m_fSpeed = 1.f;

    shared_ptr<CTransform::TRANSFORM_DESC> pTransformDesc = make_shared<CTransform::TRANSFORM_DESC>();

    // 화염구마법 생성후 넣어주기
    shared_ptr<CElectricBall::ELECTRICMAGIC_DESC> pElectricBallDesc = make_shared<CElectricBall::ELECTRICMAGIC_DESC>();
    pElectricBallDesc->tProjectile.pTarget = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
    pElectricBallDesc->pOwner = shared_from_this();

    pTransformDesc->fSpeedPerSec = 10.f;
    pTransformDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    for (int j = 0; j < m_iElectricBallCount; j++)
    {
        shared_ptr<CElectricBall> pElectricBall = CElectricBall::Create(pElectricBallDesc.get(), pTransformDesc.get(), 0);
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_Projectile"), pElectricBall)))
            return E_FAIL;
        m_vecElectricBall.push_back(pElectricBall);
    }

    for (int j = 0; j < m_iElectricBallCount2; j++)
    {
        shared_ptr<CElectricBall> pElectricBall = CElectricBall::Create(pElectricBallDesc.get(), pTransformDesc.get(), 1);
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_Projectile"), pElectricBall)))
            return E_FAIL;
        m_vecElectricBall2.push_back(pElectricBall);
    }

    AddRigidBody("LivingArmor_Mage");
    m_pRigidBody->SimulationOff();
    //m_pRigidBody->SimulationOn();

    m_vLockOnOffSetPosition = { 0.0f,2.f,0.0f };

    PrepareMonsterHPBar(TEXT("리빙아머 대마법사"));

    return S_OK;
}

void CLivingArmor_Mage::PriorityTick(_float _fTimeDelta)
{
    /*if (GAMEINSTANCE->KeyDown(DIK_I))
    {
        m_fHp -= 30.f;
    }*/

    __super::PriorityTick(_fTimeDelta);
}

void CLivingArmor_Mage::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    // Base상태에 따른 FSM
    switch (m_iBaseState)
    {
    case LA_MG_B_BEFOREMEET:
        FSM_BeforeMeet(_fTimeDelta);
        break;
    case LA_MG_B_GENERATE:
        FSM_Generate(_fTimeDelta);
        break;
    case LA_MG_B_NORMAL:
        FSM_Normal(_fTimeDelta);
        break;
    case LA_MG_B_WHILEFURY:
        FSM_WhileFury(_fTimeDelta);
        break;
    case LA_MG_B_FURY:
        FSM_Fury(_fTimeDelta);
        break;
    case LA_MG_B_HITTED:
        FSM_Hitted(_fTimeDelta);
        break;
    case LA_MG_B_HITTED2:
        FSM_Hitted2(_fTimeDelta);
        break;
    case LA_MG_B_HITTED3:
        FSM_Hitted3(_fTimeDelta);
        break;
    case LA_MG_B_STUN:
        FSM_Stun(_fTimeDelta);
        break;
    case LA_MG_B_SHOCKED:
        FSM_Shocked(_fTimeDelta);
        break;
    case LA_MG_B_DOWN:
        FSM_Down(_fTimeDelta);
        break;
    case LA_MG_B_DEATH:
        FSM_Death(_fTimeDelta);
        break;
    }
    
    if (m_iBaseState == LA_MG_B_FURY && (m_iActionState != LA_MG_A_FRONTBE))
    {
        m_pModelCom->PlayAnimation(m_fFuryActionSpeed * _fTimeDelta);
    }
    else if (m_iActionState == LA_MG_A_FRONTBE)
    {
        switch (m_iBaseState)
        {
        case LA_MG_B_HITTED:
            m_pModelCom->PlayAnimation(1.f * _fTimeDelta);
            break;
        case LA_MG_B_HITTED2:
            m_pModelCom->PlayAnimation(0.6f * _fTimeDelta);
            break;
        case LA_MG_B_HITTED3:
            m_pModelCom->PlayAnimation(0.3f * _fTimeDelta);
            break;
        }
    }
    else
    {
        m_pModelCom->PlayAnimation(_fTimeDelta);
    }
    
    m_pTransformCom->Tick(_fTimeDelta);
}

void CLivingArmor_Mage::LateTick(_float _fTimeDelta)
{
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

HRESULT CLivingArmor_Mage::Render()
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

shared_ptr<CLivingArmor_Mage> CLivingArmor_Mage::Create(_float3 _vCreatePos)
{
    shared_ptr<CLivingArmor_Mage> pInstance = make_shared<CLivingArmor_Mage>();

    pInstance->SetCurrentPos(_vCreatePos); // 위치 지정
    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CLivingArmor_Mage");

    return pInstance;
}

HRESULT CLivingArmor_Mage::AddRigidBody(string _strModelKey)
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
    CapsuleDesc.fHeight = 2.3f;
    CapsuleDesc.fRadius = 0.4f;
    _float3 vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
    CapsuleDesc.vOffSetPosition = { 0.0f, 1.7f, 0.0f };
    CapsuleDesc.vOffSetRotation = { 0.0f, 0.0f, 90.0f };
    CapsuleDesc.strShapeTag = "LivingArmor_Mage_Body";
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

void CLivingArmor_Mage::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    // 피격시
    if (_ColData.eColFlag & COL_STATIC || _ColData.eColFlag & COL_PLAYER || _ColData.eColFlag & COL_CHECKBOX || _ColData.eColFlag & COL_MONSTER)
        return;

    // 일반피격
    if ((m_iActionState == LA_MG_A_STDIDL) || (m_iActionState == LA_MG_A_RUN) || ((m_iActionState == LA_MG_A_FRONTBE)
        && ((m_iBaseState != LA_MG_B_HITTED2) && (m_iBaseState != LA_MG_B_HITTED3))))
    {
        if ((_ColData.eColFlag & COL_PLAYERWEAPON) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_1") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_2") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_3")))
        {
            m_bHitted = true;
        }
        if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack2")))
        {
            m_bHitted = true;
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

    // 짧은경직
    if ((m_iBaseState == LA_MG_B_NORMAL) || (m_iBaseState == LA_MG_B_FURY) || (m_iBaseState == LA_MG_B_HITTED) || (m_iBaseState == LA_MG_B_HITTED2))
    {
        if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
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
    if ((m_iBaseState == LA_MG_B_NORMAL) || (m_iBaseState == LA_MG_B_FURY) || (m_iBaseState == LA_MG_B_HITTED)
        || (m_iBaseState == LA_MG_B_HITTED2) || (m_iBaseState == LA_MG_B_HITTED3))
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
    if ((m_iBaseState == LA_MG_B_NORMAL) || (m_iBaseState == LA_MG_B_FURY) || (m_iBaseState == LA_MG_B_HITTED)
        || (m_iBaseState == LA_MG_B_HITTED2) || (m_iBaseState == LA_MG_B_HITTED3) || (m_iBaseState == LA_MG_B_STUN))
    {
        if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
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
    if ((m_iBaseState == LA_MG_B_NORMAL) || (m_iBaseState == LA_MG_B_FURY) || (m_iBaseState == LA_MG_B_HITTED)
        || (m_iBaseState == LA_MG_B_HITTED2) || (m_iBaseState == LA_MG_B_HITTED3) || (m_iBaseState == LA_MG_B_STUN)
        || (m_iBaseState == LA_MG_B_SHOCKED))
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
    if ((m_iBaseState == LA_MG_B_NORMAL) || (m_iBaseState == LA_MG_B_FURY) || (m_iBaseState == LA_MG_B_HITTED)
        || (m_iBaseState == LA_MG_B_HITTED2) || (m_iBaseState == LA_MG_B_HITTED3) || (m_iBaseState == LA_MG_B_STUN)
        || (m_iBaseState == LA_MG_B_SHOCKED) || (m_iBaseState == LA_MG_B_DOWN))
    {
        if ((_ColData.eColFlag & COL_PLAYERWEAPON) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_PowerAttack")))
        {
            m_bDown = true;
        }
        if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
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

    // 무적기일때는 제외하고 체력감소
    if (((m_iBaseState == LA_MG_B_NORMAL) || (m_iBaseState == LA_MG_B_FURY) || (m_iBaseState == LA_MG_B_HITTED)
        || (m_iBaseState == LA_MG_B_HITTED2) || (m_iBaseState == LA_MG_B_HITTED3) || (m_iBaseState == LA_MG_B_STUN)
        || (m_iBaseState == LA_MG_B_SHOCKED) || (m_iBaseState == LA_MG_B_DOWN))
        && !(_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        ToggleHPBar();

        DamagedByPlayer(_ColData, _szMyShapeTag);
        __super::ShowEffect(_ColData, _szMyShapeTag);
    }
}

void CLivingArmor_Mage::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CLivingArmor_Mage::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CLivingArmor_Mage::EnableCollision(const char* _strShapeTag)
{
    m_pRigidBody->EnableCollision(_strShapeTag);
}

void CLivingArmor_Mage::DisableCollision(const char* _strShapeTag)
{
    m_pRigidBody->DisableCollision(_strShapeTag);
}

void CLivingArmor_Mage::ActiveElectricBall()
{
    for (int i = 0; i < m_iElectricBallCount; i++)
    {
        if (!(m_vecElectricBall[i]->IsEnabled()))
        {
            _float3 m_vLookDir = m_pTransformCom->GetState(CTransform::STATE_LOOK);
            _float3 m_vRightDir = m_pTransformCom->GetState(CTransform::STATE_RIGHT);
            m_vecElectricBall[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos + m_vLookDir * 0.3f + m_vRightDir * 0.34f + _float3(0.f, 2.7f, 0.f));
            m_vecElectricBall[i]->UpdateTargetPos();
            m_vecElectricBall[i]->UpdateTargetDir(_float3(0.f, 1.5f, 0.f));
            m_vecElectricBall[i]->UpdateDirection();
            m_vecElectricBall[i]->GetTransform()->SetSpeed(20.f);
            m_vecElectricBall[i]->SetProjectileType(PJ_DIRECT);
            m_vecElectricBall[i]->SetEnable(true);
            m_vecElectricBall[i]->OnCollisionProjectile();
            m_vecElectricBall[i]->OnEffect();
            m_vecElectricBall[i]->SetPower(3);
            m_vecElectricBall[i]->SetAbnormal(PJ_A_SHOCK);
            GAMEINSTANCE->PlaySoundW("LM_SFX_ElectricBall1", m_fEffectSize);

            EFFECTMGR->PlayEffect("Mon_L_ElecBall_0", m_vecElectricBall[i]);
            break;
        }
    }
}

void CLivingArmor_Mage::ActiveElectricBall2(_float3 _vCreatePos, _int _iCount)
{
    _float3 m_vLookDir = m_pTransformCom->GetState(CTransform::STATE_LOOK);
    _float3 m_vRightDir = m_pTransformCom->GetState(CTransform::STATE_RIGHT);
    _float3 m_vUpDir = m_pTransformCom->GetState(CTransform::STATE_UP);
    for (int i = 0; i < m_iElectricBallCount2; i++)
    {
        if (!(m_vecElectricBall2[i]->IsEnabled()))
        {
            switch (m_iElectricBallFireCount)
            {
            case 0:
                m_vecElectricBall2[i]->GetTransform()->SetState(CTransform::STATE_POSITION, _vCreatePos + m_vUpDir * 2.f * cos(((18.f - 90.f) / 180.f) * 3.14f) +
                    m_vRightDir * 2.f * sin(((18.f - 90.f) / 180.f) * 3.14f));
                break;
            case 1:
                m_vecElectricBall2[i]->GetTransform()->SetState(CTransform::STATE_POSITION, _vCreatePos + m_vUpDir * 2.f * cos(((90.f - 90.f) / 180.f) * 3.14f) +
                    m_vRightDir * 2.f * sin(((90.f - 90.f) / 180.f) * 3.14f));
                break;
            case 2:
                m_vecElectricBall2[i]->GetTransform()->SetState(CTransform::STATE_POSITION, _vCreatePos + m_vUpDir * 2.f * cos(((162.f - 90.f) / 180.f) * 3.14f) +
                    m_vRightDir * 2.f * sin(((162.f - 90.f) / 180.f) * 3.14f));
                break;
            case 3:
                m_vecElectricBall2[i]->GetTransform()->SetState(CTransform::STATE_POSITION, _vCreatePos + m_vUpDir * 2.f * cos(((234.f - 90.f) / 180.f) * 3.14f) +
                    m_vRightDir * 2.f * sin(((234.f - 90.f) / 180.f) * 3.14f));
                break;
            case 4:
                m_vecElectricBall2[i]->GetTransform()->SetState(CTransform::STATE_POSITION, _vCreatePos + m_vUpDir * 2.f * cos(((306.f - 90.f) / 180.f) * 3.14f) +
                    m_vRightDir * 2.f * sin(((306.f - 90.f) / 180.f) * 3.14f));
                break;
            }
            m_vecElectricBall2[i]->UpdateTargetPos();
            m_vecElectricBall2[i]->UpdateTargetDir(_float3(0.f, 1.5f, 0.f));
            m_vecElectricBall2[i]->UpdateDirection();
            m_vecElectricBall2[i]->GetTransform()->SetSpeed(4.f);
            m_vecElectricBall2[i]->SetProjectileType(PJ_READYGUIDED);
            m_vecElectricBall2[i]->SetEnable(true);
            m_vecElectricBall2[i]->OnCollisionProjectile();
            m_vecElectricBall2[i]->OnEffect();
            m_vecElectricBall2[i]->SetPower(1);
            //m_vecElectricBall2[i]->SetAbnormal(PJ_A_SHORTHITTED);
            m_vecElectricBall2[i]->SetReady(5.f);

            EFFECTMGR->PlayEffect("Mon_L_ElecBall_1", m_vecElectricBall2[i]);
            GAMEINSTANCE->PlaySoundW("LM_SFX_ElectricBall2", m_fEffectSize);

            m_iElectricBallFireCount++;
            if (m_iElectricBallFireCount == _iCount)
            {
                m_iElectricBallFireCount = 0;
                break;
            }
        }
    }
}

void CLivingArmor_Mage::DisActiveElectricBall()
{
    for (int i = 0; i < m_iElectricBallCount; i++)
    {
        if ((m_vecElectricBall[i]->IsEnabled()))
        {
            m_vecElectricBall[i]->SetEnable(false);
        }
    }
    for (int i = 0; i < m_iElectricBallCount2; i++)
    {
        if ((m_vecElectricBall2[i]->IsEnabled()))
        {
            m_vecElectricBall2[i]->SetEnable(false);
        }
    }
}

HRESULT CLivingArmor_Mage::AddPartObjects(string _strModelKey)
{
    shared_ptr<CWeapon::WEAPON_DESC> pWeaponDesc = make_shared<CWeapon::WEAPON_DESC>();
    pWeaponDesc->tPartObject.pParentTransform = m_pTransformCom;
    pWeaponDesc->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_RightHand");
    pWeaponDesc->strModelKey = _strModelKey;

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


void CLivingArmor_Mage::UpdateDistanceState()
{
    // 위치 바로 못받아오는거 때문에 우선
    if (m_vTargetPos == _float3(0.f, 0.f, 0.f))
    {
        m_iDistanceState = LA_MG_D_BEFOREMEET;
        return;
    }

    if (m_iDistanceState == LA_MG_D_BEFOREMEET)
    {
        if (m_fDistanceToTarget < m_fMeetRange)
        {
            m_iDistanceState = LA_MG_D_MOVERANGE;
        }
        else
        {
            m_iDistanceState = LA_MG_D_BEFOREMEET;
        }
    }
    else
    {
        if (m_fDistanceToTarget < m_fAttack1Range)
        {
            m_iDistanceState = LA_MG_D_ATTACKRANGE1;
        }
        else if (m_fDistanceToTarget < m_fAttack2Range)
        {
            m_iDistanceState = LA_MG_D_ATTACKRANGE2;
        }
        else if (m_fDistanceToTarget >= m_fAttack2Range && m_fDistanceToTarget < m_fOutRange)
        {
            m_iDistanceState = LA_MG_D_MOVERANGE;
        }
        else
        {
            m_iDistanceState = LA_MG_D_OUTRANGE;
        }
    }
}

void CLivingArmor_Mage::FSM_BeforeMeet(_float _fTimeDelta)
{
    UpdateDistanceState();

    // 가까워지면
    if (m_iDistanceState < LA_MG_D_OUTRANGE)
    {
        //GAMEINSTANCE->SwitchingBGM();
        m_iBaseState = LA_MG_B_GENERATE;
        m_iActionState = LA_MG_A_APPEAR;
        m_pModelCom->ChangeAnim(m_iActionState, 2.f);
        m_iEffectIdx = EFFECTMGR->PlayEffect("LivingArmor_Aura_Blue_Big", shared_from_this());
    }
    // 멀때
    else
    {
    }
}

void CLivingArmor_Mage::FSM_Generate(_float _fTimeDelta)
{
    if (m_iActionState == LA_MG_A_APPEAR)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iActionState = LA_MG_A_COMBATSTART;
            m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
        }
    }
    if (m_iActionState == LA_MG_A_COMBATSTART)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iBaseState = LA_MG_B_NORMAL;
            m_iActionState = LA_MG_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
            UpdateDistanceState();
        }
    }
}

void CLivingArmor_Mage::FSM_Normal(_float _fTimeDelta)
{ 
    if (m_iActionState != LA_MG_A_ATTACK2)
    {
        UpdateDirection();
    }
    else
    {
        m_pTransformCom->TurnToDir(m_vTargetDir, 10.f * _fTimeDelta);
    }

    m_fCalculateSummonTime += _fTimeDelta;
    if (m_fCalculateSummonTime > m_fBallSummonIntervel1)
    {
        m_fCalculateSummonTime = 0.f;
        ActiveElectricBall2(RandomBindMagic2Pos(), 3);
    }

    switch (m_iDistanceState)
    {
    case LA_MG_D_MOVERANGE:
        if (m_iActionState == LA_MG_A_STDIDL)
        {
            m_iActionState = LA_MG_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        else if (m_iActionState == LA_MG_A_RUN)
        {
            UpdateCoolTime(_fTimeDelta);

            MoveToTarget(_fTimeDelta);
            UpdateDistanceState();
        }
        else
        {
            m_iActionState = LA_MG_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        break;

    case LA_MG_D_ATTACKRANGE1:
        if (m_iActionState == LA_MG_A_STDIDL)
        {
            UpdateDistanceState();
            UpdateCoolTime(_fTimeDelta);

            for (int i = LA_MG_C_NORMALATTACK2; i < LA_MG_C_NORMALATTACK2 + 2; i++)
            {
                if (m_bAttackReady[i])
                {
                    m_iActionState = i + (LA_MG_A_ATTACK2 - LA_MG_C_NORMALATTACK2);
                    if (m_iActionState == LA_MG_A_ATTACK3)
                        m_iActionState = LA_MG_A_ATTACK4;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                    m_bAttackReady[i] = false;
                    m_iDistanceState = LA_MG_D_ATTACKRANGE1;

                    break;
                }
            }
        }
        if (m_iActionState == LA_MG_A_RUN)
        {
            m_iActionState = LA_MG_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        if (m_iActionState == LA_MG_A_ATTACK2)
        {
            m_fCalculateTime2 += _fTimeDelta;
            if (m_fCalculateTime2 > m_fEnergyStartTime && !m_bEnergyStart)
            {
                GAMEINSTANCE->PlaySoundL("LM_SFX_BEAM", m_fEffectSize);
                m_bEnergyStart = true;
            }
            if (m_fCalculateTime2 > m_fEnergyEndTime && !m_bEnergyEnd)
            {
                GAMEINSTANCE->StopSound("LM_SFX_BEAM");
                m_bEnergyEnd = true;
            }

            if (m_bEnergyGen)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > 0.5f)
                {
                    m_bEnergyGen = false;
                    m_iEffectIdx2 = EFFECTMGR->PlayEffect("Mon_L_EnergyWave", shared_from_this());
                    m_fCalculateTime = 0.f;
                }
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime2 = 0.f;
                m_bEnergyStart = false;
                m_bEnergyEnd = false;
                m_bEnergyGen = true;
                m_iActionState = LA_MG_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.4f, true);
                UpdateDistanceState();
            }
        }
        if (m_iActionState == LA_MG_A_ATTACK3)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = LA_MG_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.4f, true);
                UpdateDistanceState();
            }
        }
        if (m_iActionState == LA_MG_A_ATTACK4)
        {
            if (m_bEnergyGen)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > 0.5f)
                {
                    m_bEnergyGen = false;
                    EFFECTMGR->PlayEffect("Mon_L_FloorMagic", shared_from_this());
                    m_fCalculateTime = 0.f;
                }
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_bEnergyGen = true;
                m_iActionState = LA_MG_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.4f, true);
                UpdateDistanceState();
            }
        }
        break;

    case LA_MG_D_ATTACKRANGE2:
        if (m_iActionState == LA_MG_A_STDIDL)
        {
            UpdateDistanceState();
            UpdateCoolTime(_fTimeDelta);

            for (int i = LA_MG_C_NORMALATTACK1; i < LA_MG_C_NORMALATTACK1 + 2; i++)
            {
                if (m_bAttackReady[i])
                {
                    m_iActionState = i + (LA_MG_A_ATTACK1 - LA_MG_C_NORMALATTACK1);
                    m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                    m_bAttackReady[i] = false;
                    m_iDistanceState = LA_MG_D_ATTACKRANGE2;
                    break;
                }
            }
        }
        if (m_iActionState == LA_MG_A_RUN)
        {
            m_iActionState = LA_MG_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        if (m_iActionState == LA_MG_A_ATTACK1)
        {
            if (!m_bElectricBallFire)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fElectricBallStartTime < m_fCalculateTime)
                {
                    m_fCalculateTime = 0.f;
                    m_bElectricBallFire = true;
                    ActiveElectricBall();
                }
            }

            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_bElectricBallFire = false;
                m_iActionState = LA_MG_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.2f, true);
                UpdateDistanceState();
            }
        }
        if (m_iActionState == LA_MG_A_ATTACK2)
        {
            m_fCalculateTime2 += _fTimeDelta;
            if (m_fCalculateTime2 > m_fEnergyStartTime && !m_bEnergyStart)
            {
                GAMEINSTANCE->PlaySoundL("LM_SFX_BEAM", m_fEffectSize);
                m_bEnergyStart = true;
            }
            if (m_fCalculateTime2 > m_fEnergyEndTime && !m_bEnergyEnd)
            {
                GAMEINSTANCE->StopSound("LM_SFX_BEAM");
                m_bEnergyEnd = true;
            }

            if (m_bEnergyGen)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > 0.5f)
                {
                    m_bEnergyGen = false;
                    m_iEffectIdx2 = EFFECTMGR->PlayEffect("Mon_L_EnergyWave", shared_from_this());
                    m_fCalculateTime = 0.f;
                }
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime2 = 0.f;
                m_bEnergyStart = false;
                m_bEnergyEnd = false;
                m_bEnergyGen = true;
                m_iActionState = LA_MG_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.4f, true);
                UpdateDistanceState();
                //EFFECTMGR->PlayEffect("Mon_L_EnergyWave", shared_from_this());
            }
        }
        break;

    case LA_MG_D_OUTRANGE:
        break;
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 광폭화상태
    CheckFury();
}

void CLivingArmor_Mage::FSM_WhileFury(_float _fTimeDelta)
{
    if (m_iActionState == LA_MG_A_FURY)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateSummonTime = 0.f;
            m_fCalculateTime = 0.f;
            m_bElectricBallFire = false;
            m_iBaseState = LA_MG_B_FURY;
            m_iActionState = LA_MG_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
            UpdateDistanceState();

            m_fSpeed = m_fFurySpeed;
        }
    }
}

void CLivingArmor_Mage::FSM_Fury(_float _fTimeDelta)
{
    if (m_iActionState != LA_MG_A_ATTACK2)
    {
        UpdateDirection();
    }
    else
    {
        m_pTransformCom->TurnToDir(m_vTargetDir, 20.f * _fTimeDelta);
    }
    
    m_fCalculateSummonTime += _fTimeDelta;
    if (m_fCalculateSummonTime > m_fBallSummonIntervel2)
    {
        m_fCalculateSummonTime = 0.f;
        ActiveElectricBall2(RandomBindMagic2Pos(), 5);
    }

    switch (m_iDistanceState)
    {
    case LA_MG_D_MOVERANGE:
        if (m_iActionState == LA_MG_A_STDIDL)
        {
            m_iActionState = LA_MG_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        else if (m_iActionState == LA_MG_A_RUN)
        {
            UpdateCoolTime(_fTimeDelta);

            MoveToTarget(_fTimeDelta);
            UpdateDistanceState();
        }
        else
        {
            m_iActionState = LA_MG_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        break;

    case LA_MG_D_ATTACKRANGE1:
        if (m_iActionState == LA_MG_A_STDIDL)
        {
            UpdateDistanceState();
            UpdateCoolTime(_fTimeDelta);

            for (int i = LA_MG_C_FURYATTACK2; i < LA_MG_C_FURYATTACK2 + 3; i++)
            {
                if (m_bAttackReady[i])
                {
                    m_iActionState = i + (LA_MG_A_ATTACK2 - LA_MG_C_FURYATTACK2);
                    if (m_iActionState == LA_MG_A_ATTACK3)
                        m_iActionState = LA_MG_A_ATTACK4;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.25f);
                    m_bAttackReady[i] = false;
                    m_iDistanceState = LA_MG_D_ATTACKRANGE1;
                    break;
                }
            }
        }
        if (m_iActionState == LA_MG_A_RUN)
        {
            m_iActionState = LA_MG_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        if (m_iActionState == LA_MG_A_ATTACK2)
        {
            m_fCalculateTime2 += _fTimeDelta;
            if (m_fCalculateTime2 > m_fFEnergyStartTime && !m_bEnergyStart)
            {
                GAMEINSTANCE->PlaySoundL("LM_SFX_BEAM", m_fEffectSize);
                m_bEnergyStart = true;
            }
            if (m_fCalculateTime2 > m_fFEnergyEndTime && !m_bEnergyEnd)
            {
                GAMEINSTANCE->StopSound("LM_SFX_BEAM");
                m_bEnergyEnd = true;
            }

            if (m_bEnergyGen)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > 0.25f)
                {
                    m_bEnergyGen = false;
                    m_iEffectIdx2 = EFFECTMGR->PlayEffect("Mon_L_EnergyWave", shared_from_this());
                    m_fCalculateTime = 0.f;
                }
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime2 = 0.f;
                m_bEnergyStart = false;
                m_bEnergyEnd = false;

                m_bEnergyGen = true;
                m_iActionState = LA_MG_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                UpdateDistanceState();
                //EFFECTMGR->PlayEffect("Mon_L_EnergyWave", shared_from_this());
            }
        }
        if (m_iActionState == LA_MG_A_ATTACK3)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = LA_MG_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.2f, true);
                UpdateDistanceState();
            }
        }
        if (m_iActionState == LA_MG_A_ATTACK4)
        {
            if (m_bEnergyGen)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > 0.25f)
                {
                    m_bEnergyGen = false;
                    EFFECTMGR->PlayEffect("Mon_L_FloorMagic", shared_from_this());
                    m_fCalculateTime = 0.f;
                }
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_bEnergyGen = true;
                m_iActionState = LA_MG_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.2f, true);
                UpdateDistanceState();
            }
        }

        break;

    case LA_MG_D_ATTACKRANGE2:
        if (m_iActionState == LA_MG_A_STDIDL)
        {
            UpdateDistanceState();
            UpdateCoolTime(_fTimeDelta);

            for (int i = LA_MG_C_FURYATTACK1; i < LA_MG_C_FURYATTACK1 + 4; i++)
            {
                if (i == LA_MG_C_FURYATTACK1 + 2)
                {
                    continue;
                }
                if (m_bAttackReady[i])
                {
                    m_iActionState = i + (LA_MG_A_ATTACK1 - LA_MG_C_FURYATTACK1);
                    m_pModelCom->ChangeAnim(m_iActionState, 0.25f);
                    m_bAttackReady[i] = false;
                    m_iDistanceState = LA_MG_D_ATTACKRANGE2;
                    break;
                }
            }
        }
        if (m_iActionState == LA_MG_A_RUN)
        {
            m_iActionState = LA_MG_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        if (m_iActionState == LA_MG_A_ATTACK1)
        {
            if (!m_bElectricBallFire)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fElectricBallFStartTime < m_fCalculateTime)
                {
                    m_fCalculateTime = 0.f;
                    m_bElectricBallFire = true;
                    ActiveElectricBall();
                }
            }
            
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_bElectricBallFire = false;
                m_iActionState = LA_MG_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                UpdateDistanceState();
            }
        }
        if (m_iActionState == LA_MG_A_ATTACK2)
        {
            if (m_fCalculateTime2 > m_fEnergyStartTime && !m_bEnergyStart)
            {
                GAMEINSTANCE->PlaySoundL("LM_SFX_BEAM", m_fEffectSize);
                m_bEnergyStart = true;
            }
            if (m_fCalculateTime2 > m_fEnergyEndTime && !m_bEnergyEnd)
            {
                GAMEINSTANCE->StopSound("LM_SFX_BEAM");
                m_bEnergyEnd = true;
            }

            if (m_bEnergyGen)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > 0.25f)
                {
                    m_bEnergyGen = false;
                    m_iEffectIdx2 = EFFECTMGR->PlayEffect("Mon_L_EnergyWave", shared_from_this());
                    m_fCalculateTime = 0.f;
                }
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime2 = 0.f;
                m_bEnergyStart = false;
                m_bEnergyEnd = false;
                m_bEnergyGen = true;
                m_iActionState = LA_MG_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.2f, true);
                UpdateDistanceState();
                //EFFECTMGR->PlayEffect("Mon_L_EnergyWave", shared_from_this());
            }
        }
        if (m_iActionState == LA_MG_A_ATTACK4)
        {
            if (m_bEnergyGen)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > 0.25f)
                {
                    m_bEnergyGen = false;
                    EFFECTMGR->PlayEffect("Mon_L_FloorMagic", shared_from_this());
                    m_fCalculateTime = 0.f;
                }
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_bEnergyGen = true;
                m_iActionState = LA_MG_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.2f, true);
                UpdateDistanceState();
            }
        }

        break;
    case LA_MG_D_OUTRANGE:
        break;
    }

    // 공격들어온 종류 체크
    //CheckAttacked();
    // 사망상태
    CheckDead();
}

void CLivingArmor_Mage::FSM_Hitted(_float _fTimeDelta)
{
    if (m_iActionState == LA_MG_A_FRONTBE)
    {
        if (m_bHitted)
        {
            UpdateDirection();
            m_bHitted = false;
            m_iActionState = LA_MG_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }

        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < 50.f)
            {
                m_iBaseState = LA_MG_B_FURY;
            }
            else
                m_iBaseState = LA_MG_B_NORMAL;
            m_iActionState = LA_MG_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_bHitted = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 광폭화상태
    CheckFury();
    // 사망
    CheckDead();
}

void CLivingArmor_Mage::FSM_Hitted2(_float _fTimeDelta)
{
    if (m_iActionState == LA_MG_A_FRONTBE)
    {
        if (m_bHitted2)
        {
            //UpdateDirection();
            m_bHitted2 = false;
            m_iActionState = LA_MG_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }

        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < 50.f)
            {
                m_iBaseState = LA_MG_B_FURY;
            }
            else
                m_iBaseState = LA_MG_B_NORMAL;
            m_iActionState = LA_MG_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_bHitted2 = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 광폭화상태
    CheckFury();
    // 사망
    CheckDead();
}

void CLivingArmor_Mage::FSM_Hitted3(_float _fTimeDelta)
{
    if (m_iActionState == LA_MG_A_FRONTBE)
    {
        if (m_bHitted3)
        {
            UpdateDirection();
            m_bHitted3 = false;
            m_iActionState = LA_MG_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }

        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < 50.f)
            {
                m_iBaseState = LA_MG_B_FURY;
            }
            else
                m_iBaseState = LA_MG_B_NORMAL;
            m_iActionState = LA_MG_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_bHitted3 = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 광폭화상태
    CheckFury();
    // 사망
    CheckDead();
}

void CLivingArmor_Mage::FSM_Stun(_float _fTimeDelta)
{
    if (m_iActionState == LA_MG_A_STUN)
    {
        if (m_bStun)
        {
            UpdateDirection();
            m_bStun = false;
            m_iActionState = LA_MG_A_STUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }

        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < 50.f)
            {
                m_iBaseState = LA_MG_B_FURY;
            }
            else
                m_iBaseState = LA_MG_B_NORMAL;
            m_iActionState = LA_MG_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_bStun = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 광폭화상태
    CheckFury();
    // 사망
    CheckDead();
}

void CLivingArmor_Mage::FSM_Shocked(_float _fTimeDelta)
{
    if (m_iActionState == LA_MG_A_SHOCKED)
    {
        if (m_bShocked)
        {
            UpdateDirection();
            m_bShocked = false;
            m_iActionState = LA_MG_A_SHOCKED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.3f, false);
        }

        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < 50.f)
            {
                m_iBaseState = LA_MG_B_FURY;
            }
            else
                m_iBaseState = LA_MG_B_NORMAL;
            m_iActionState = LA_MG_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.3f, true);
            m_bShocked = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 광폭화상태
    CheckFury();
    // 사망
    CheckDead();
}

void CLivingArmor_Mage::FSM_Down(_float _fTimeDelta)
{
    if (m_iActionState == LA_MG_A_DOWN1)
    {
        if (m_bDown)
        {
            UpdateDirection();
            m_fCalculateTime = 0.f;
            m_bDown = false;
            m_iActionState = LA_MG_A_DOWN1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iActionState = LA_MG_A_DOWN2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == LA_MG_A_DOWN2)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iActionState = LA_MG_A_DOWN3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == LA_MG_A_DOWN3)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            if (m_fHp < m_fFuryHp)
            {
                m_iBaseState = LA_MG_B_FURY;
            }
            else
                m_iBaseState = LA_MG_B_NORMAL;
            m_iActionState = LA_MG_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_bDown = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 광폭화상태
    CheckFury();
    // 사망
    CheckDead();
}

void CLivingArmor_Mage::FSM_Death(_float _fTimeDelta)
{
    if ((m_iActionState == LA_MG_A_DEAD) && (m_pModelCom->GetIsFinishedAnimation()))
    {
        m_iActionState = LA_MG_A_COMBATSTART;
        //m_pModelCom->ChangeAnim(m_iActionState, 0.01f, false);

        SetDissolve(true, true, 3.f);
        SetDissolveInfo(1.f, m_vDisolveColor, "Noise_3003");

        m_bDeadStart = true;
        m_fCalculateTime = 0.f;
    }
    if (m_bDeadStart)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_fCalculateTime > 3.f)
        {
            m_fCalculateTime = 0.f;
            m_bDeadStart = false;

            m_IsEnabled = false;
        }
    }
}

void CLivingArmor_Mage::CheckFury()
{
    if ((m_fHp < m_fFuryHp) && !m_bFury)
    {
        m_bEnergyStart = false;
        m_bEnergyEnd = false;
        GAMEINSTANCE->StopSound("LM_SFX_BEAM");
        m_bHitted = false;
        m_bHitted2 = false;
        m_bHitted3 = false;
        m_bStun = false;
        m_bShocked = false;
        m_bDown = false;

        m_fHp = m_fFuryHp;
        m_bFury = true;
        m_iBaseState = LA_MG_B_WHILEFURY;
        m_iActionState = LA_MG_A_FURY;
        m_pModelCom->ChangeAnim(m_iActionState, 0.5f, false);

        EFFECTMGR->StopEffect("LivingArmor_Aura_Blue_Big", m_iEffectIdx);
        EFFECTMGR->StopEffect("Mon_L_EnergyWave", m_iEffectIdx2);
        m_iEffectIdx = EFFECTMGR->PlayEffect("LivingArmor_Aura_Red_Big", shared_from_this());

        EFFECTMGR->StopEffect("LivingArmor_Aura_Blue_Big", m_iEffectIdx2);
    }
}

void CLivingArmor_Mage::CheckDead()
{
    if (m_fHp <= 0.f)
    {
        //GAMEINSTANCE->SwitchingBGM();
        m_bEnergyStart = false;
        m_bEnergyEnd = false;
        GAMEINSTANCE->StopSound("LM_SFX_BEAM");
        GAMEINSTANCE->PlaySoundW("LM_Die", m_fVoiceSize);
        m_bDead = true;
        m_iBaseState = LA_MG_B_DEATH;
        m_iActionState = LA_MG_A_DEAD;
        m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
        m_pRigidBody->SimulationOff();
        
        DisActiveElectricBall();
        
        EFFECTMGR->StopEffect("Mon_L_EnergyWave", m_iEffectIdx2);
        QUESTMGR->CheckDeadMonster(shared_from_this());
    }
}

void CLivingArmor_Mage::CheckAttacked()
{
    if (m_bHitted)
    {
        m_bEnergyStart = false;
        m_bEnergyEnd = false;
        GAMEINSTANCE->StopSound("LM_SFX_BEAM");
        EFFECTMGR->StopEffect("Mon_L_EnergyWave", m_iEffectIdx2);
        m_iBaseState = LA_MG_B_HITTED;
        m_iActionState = LA_MG_A_FRONTBE;
    }

    if (m_bHitted2)
    {
        m_bEnergyStart = false;
        m_bEnergyEnd = false;
        GAMEINSTANCE->StopSound("LM_SFX_BEAM");
        EFFECTMGR->StopEffect("Mon_L_EnergyWave", m_iEffectIdx2);
        m_iBaseState = LA_MG_B_HITTED2;
        m_iActionState = LA_MG_A_FRONTBE;
    }

    if (m_bHitted3)
    {
        m_bEnergyStart = false;
        m_bEnergyEnd = false;
        GAMEINSTANCE->StopSound("LM_SFX_BEAM");
        EFFECTMGR->StopEffect("Mon_L_EnergyWave", m_iEffectIdx2);
        m_iBaseState = LA_MG_B_HITTED3;
        m_iActionState = LA_MG_A_FRONTBE;
    }

    if (m_bStun)
    {
        m_bEnergyStart = false;
        m_bEnergyEnd = false;
        GAMEINSTANCE->StopSound("LM_SFX_BEAM");
        EFFECTMGR->StopEffect("Mon_L_EnergyWave", m_iEffectIdx2);
        m_iBaseState = LA_MG_B_STUN;
        m_iActionState = LA_MG_A_STUN;
    }

    if (m_bShocked)
    {
        m_bEnergyStart = false;
        m_bEnergyEnd = false;
        GAMEINSTANCE->StopSound("LM_SFX_BEAM");
        EFFECTMGR->StopEffect("Mon_L_EnergyWave", m_iEffectIdx2);
        m_iBaseState = LA_MG_B_SHOCKED;
        m_iActionState = LA_MG_A_SHOCKED;
    }

    if (m_bDown)
    {
        m_bEnergyStart = false;
        m_bEnergyEnd = false;
        GAMEINSTANCE->StopSound("LM_SFX_BEAM");
        EFFECTMGR->StopEffect("Mon_L_EnergyWave", m_iEffectIdx2);
        m_iBaseState = LA_MG_B_DOWN;
        m_iActionState = LA_MG_A_DOWN1;
    }
}

_float3 CLivingArmor_Mage::RandomBindMagic2Pos()
{
    _float3 m_vPos;
    _float3 m_vLookDir = m_pTransformCom->GetState(CTransform::STATE_LOOK);
    _float3 m_vRightDir = m_pTransformCom->GetState(CTransform::STATE_RIGHT);
    _float3 m_vUpDir = m_pTransformCom->GetState(CTransform::STATE_UP);
    switch (GAMEINSTANCE->PickRandomInt(0, 3))
    {
    case 0:
        m_vPos = m_vCurrentPos + m_vRightDir * 4.f + _float3(0.f, 5.f, 0.f);
        break;
    case 1:
        m_vPos = m_vCurrentPos - m_vRightDir * 4.f + _float3(0.f, 5.f, 0.f);
        break;
    case 2:
        m_vPos = m_vCurrentPos + _float3(0.f, 8.f, 0.f);
        break;
    }

    return m_vPos;
}

void CLivingArmor_Mage::UpdateCoolTime(_float _fTimeDelta)
{
    switch (m_iBaseState)
    {
    case LA_MG_B_NORMAL:
        for (int i = LA_MG_C_NORMALATTACK1; i < LA_MG_C_NORMALATTACK1 + m_NormalAttackCount; i++)
        {
            if (!m_bAttackReady[i])
            {
                m_fAttackCoolTime[i] -= _fTimeDelta;
                if (m_fAttackCoolTime[i] < 0.f)
                {
                    m_bAttackReady[i] = true;
                    m_fAttackCoolTime[i] = m_fBAttackCoolTime[i];
                    for (int j = LA_MG_C_NORMALATTACK1; j < LA_MG_C_NORMALATTACK1 + m_NormalAttackCount; j++)
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
        break;
    case LA_MG_B_FURY:
        for (int i = LA_MG_C_FURYATTACK1; i < LA_MG_C_FURYATTACK1 + m_FuryAttackCount; i++)
        {
            if (!m_bAttackReady[i])
            {
                m_fAttackCoolTime[i] -= _fTimeDelta;
                if (m_fAttackCoolTime[i] < 0.f)
                {
                    m_bAttackReady[i] = true;
                    m_fAttackCoolTime[i] = m_fBAttackCoolTime[i];
                    for (int j = LA_MG_C_FURYATTACK1; j < LA_MG_C_FURYATTACK1 + m_FuryAttackCount; j++)
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
        break;
    }
}