#include "LivingArmor_Soldier.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "Animation.h"
#include "EffectMgr.h"
#include "PlayerProjectile.h"
#include "QuestMgr.h"

CLivingArmor_Soldier::CLivingArmor_Soldier()
    : CMonster()
{
}

CLivingArmor_Soldier::~CLivingArmor_Soldier()
{
}

HRESULT CLivingArmor_Soldier::Initialize()
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

    m_pModelCom = GAMEINSTANCE->GetModel("LivingArmor_Soldier");
    m_strModelName = "LivingArmor_Soldier";
    AddComponent(L"Com_Model", m_pModelCom);
    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

    m_pModelCom->ChangeAnim(m_iActionState);

    if (FAILED(AddPartObjects("Sword_Sword_LivingArmor_Soldier")))
        return E_FAIL;

    // 스탯
    m_fHp = 150.f;
    m_fMaxHp = 150.f;
    m_fSpeed = 2.f;

    AddRigidBody("LivingArmor_Soldier");
    m_pRigidBody->SimulationOff();

    m_vLockOnOffSetPosition = { 0.0f,1.5f,0.0f };

    PrepareMonsterHPBar(TEXT("리빙아머 전사"));

    return S_OK;
}

void CLivingArmor_Soldier::PriorityTick(_float _fTimeDelta)
{

    __super::PriorityTick(_fTimeDelta);
}

void CLivingArmor_Soldier::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    // Base상태에 따른 FSM
    switch (m_iBaseState)
    {
    case LA_S_B_BEFOREMEET:
        FSM_BeforeMeet(_fTimeDelta);
        break;
    case LA_S_B_GENERATE:
        FSM_Generate(_fTimeDelta);
        break;
    case LA_S_B_NORMAL:
        FSM_Normal(_fTimeDelta);
        break;
    case LA_S_B_WHILEFURY:
        FSM_WhileFury(_fTimeDelta);
        break;
    case LA_S_B_FURY:
        FSM_Fury(_fTimeDelta);
        break;
    case LA_S_B_HITTED:
        FSM_Hitted(_fTimeDelta);
        break;
    case LA_S_B_HITTED2:
        FSM_Hitted2(_fTimeDelta);
        break;
    case LA_S_B_HITTED3:
        FSM_Hitted3(_fTimeDelta);
        break;
    case LA_S_B_STUN:
        FSM_Stun(_fTimeDelta);
        break;
    case LA_S_B_SHOCKED:
        FSM_Shocked(_fTimeDelta);
        break;
    case LA_S_B_DOWN:
        FSM_Down(_fTimeDelta);
        break;
    case LA_S_B_DEATH:
        FSM_Death(_fTimeDelta);
        break;
    }

    if ((m_iBaseState == LA_S_B_FURY) && (m_iActionState != LA_S_A_BLOCKED) && (m_iActionState != LA_S_A_FRONTBE))
    {
        m_pModelCom->PlayAnimation(m_fFuryActionSpeed * _fTimeDelta);
    }
    else if (m_iActionState == LA_S_A_APPEAR)
    {
        m_pModelCom->PlayAnimation(1.5f * _fTimeDelta);
    }
    else if (m_iActionState == LA_S_A_DEATH)
    {
        m_pModelCom->PlayAnimation(1.5f * _fTimeDelta);
    }
    else if (m_iActionState == LA_S_A_BLOCKED)
    {
        switch (m_iParriedPower)
        {
        case PARRIEDWEAK:
            m_pModelCom->PlayAnimation(2.f * _fTimeDelta);
            break;
        case PARRIEDNORMAL:
            m_pModelCom->PlayAnimation(1.f * _fTimeDelta);
            break;
        case PARRIEDSTRONG:
            m_pModelCom->PlayAnimation(1.f * _fTimeDelta);
            break;
        }
    }
    else if (m_iActionState == LA_S_A_FRONTBE)
    {
        switch (m_iBaseState)
        {
        case LA_S_B_HITTED:
            m_pModelCom->PlayAnimation(1.f * _fTimeDelta);
            break;
        case LA_S_B_HITTED2:
            m_pModelCom->PlayAnimation(0.6f * _fTimeDelta);
            break;
        case LA_S_B_HITTED3:
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

void CLivingArmor_Soldier::LateTick(_float _fTimeDelta)
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

HRESULT CLivingArmor_Soldier::Render()
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

shared_ptr<CLivingArmor_Soldier> CLivingArmor_Soldier::Create(_float3 _vCreatePos)
{
    shared_ptr<CLivingArmor_Soldier> pInstance = make_shared<CLivingArmor_Soldier>();

    pInstance->SetCurrentPos(_vCreatePos); // 위치 지정
    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CLivingArmor_Soldier");

    return pInstance;
}

HRESULT CLivingArmor_Soldier::AddRigidBody(string _strModelKey)
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
    CapsuleDesc.vOffSetPosition = { 0.0f, 1.0f, 0.0f };
    CapsuleDesc.vOffSetRotation = { 0.0f, 0.0f, 90.0f };
    CapsuleDesc.strShapeTag = "LivingArmor_Soldier_Body";
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

void CLivingArmor_Soldier::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    // 피격시
    if (_ColData.eColFlag & COL_STATIC || _ColData.eColFlag & COL_PLAYER || _ColData.eColFlag & COL_CHECKBOX || _ColData.eColFlag & COL_MONSTER)
        return;

    // 일반피격
    if ((m_iActionState == LA_S_A_STDALT) || (m_iActionState == LA_S_A_RUN) || (m_iActionState == LA_S_A_BLOCKED)
        || ((m_iActionState == LA_S_A_FRONTBE) && ((m_iBaseState != LA_S_B_HITTED2) && (m_iBaseState != LA_S_B_HITTED3))))
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
    if ((m_iBaseState == LA_S_B_NORMAL) || (m_iBaseState == LA_S_B_FURY) || (m_iBaseState == LA_S_B_HITTED) || (m_iBaseState == LA_S_B_HITTED2))
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
    if ((m_iBaseState == LA_S_B_NORMAL) || (m_iBaseState == LA_S_B_FURY) || (m_iBaseState == LA_S_B_HITTED)
        || (m_iBaseState == LA_S_B_HITTED2) || (m_iBaseState == LA_S_B_HITTED3))
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
    if ((m_iBaseState == LA_S_B_NORMAL) || (m_iBaseState == LA_S_B_FURY) || (m_iBaseState == LA_S_B_HITTED)
        || (m_iBaseState == LA_S_B_HITTED2) || (m_iBaseState == LA_S_B_HITTED3) || (m_iBaseState == LA_S_B_STUN))
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
    if ((m_iBaseState == LA_S_B_NORMAL) || (m_iBaseState == LA_S_B_FURY) || (m_iBaseState == LA_S_B_HITTED)
        || (m_iBaseState == LA_S_B_HITTED2) || (m_iBaseState == LA_S_B_HITTED3) || (m_iBaseState == LA_S_B_STUN)
        || (m_iBaseState == LA_S_B_SHOCKED))
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
    if ((m_iBaseState == LA_S_B_NORMAL) || (m_iBaseState == LA_S_B_FURY) || (m_iBaseState == LA_S_B_HITTED)
        || (m_iBaseState == LA_S_B_HITTED2) || (m_iBaseState == LA_S_B_HITTED3) || (m_iBaseState == LA_S_B_STUN)
        || (m_iBaseState == LA_S_B_SHOCKED) || (m_iBaseState == LA_S_B_DOWN))
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
    
    // 패링
    if ((_szMyShapeTag == m_strATT1Collider1) && (_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        m_bParried = true;
    }
    if ((_szMyShapeTag == m_strATT2Collider1) && (_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        m_bParried = true;
    }

    // 무적기일때는 제외하고 체력감소
    if (((m_iBaseState == LA_S_B_NORMAL) || (m_iBaseState == LA_S_B_FURY) || (m_iBaseState == LA_S_B_HITTED)
        || (m_iBaseState == LA_S_B_HITTED2) || (m_iBaseState == LA_S_B_HITTED3) || (m_iBaseState == LA_S_B_STUN)
        || (m_iBaseState == LA_S_B_SHOCKED) || (m_iBaseState == LA_S_B_DOWN))
        && !(_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        ToggleHPBar();
        
        DamagedByPlayer(_ColData, _szMyShapeTag);
        __super::ShowEffect(_ColData, _szMyShapeTag);
    }
}

void CLivingArmor_Soldier::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CLivingArmor_Soldier::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CLivingArmor_Soldier::EnableCollision(const char* _strShapeTag)
{
    m_pRigidBody->EnableCollision(_strShapeTag);
}

void CLivingArmor_Soldier::DisableCollision(const char* _strShapeTag)
{
    m_pRigidBody->DisableCollision(_strShapeTag);
}

HRESULT CLivingArmor_Soldier::AddPartObjects(string _strModelKey)
{
    shared_ptr<CWeapon::WEAPON_DESC> pWeaponDesc = make_shared<CWeapon::WEAPON_DESC>();
    pWeaponDesc->tPartObject.pParentTransform = m_pTransformCom;
    pWeaponDesc->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_RightHand");

    pWeaponDesc->strModelKey = _strModelKey;
    pWeaponDesc->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(-90.f));

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


void CLivingArmor_Soldier::UpdateDistanceState()
{
    // 위치 바로 못받아오는거 때문에 우선
    if (m_vTargetPos == _float3(0.f, 0.f, 0.f))
    {
        m_iDistanceState = LA_S_D_BEFOREMEET;
        return;
    }

    if (m_iDistanceState == LA_S_D_BEFOREMEET)
    {
        if (m_fDistanceToTarget < m_fMeetRange)
        {
            m_iDistanceState = LA_S_D_MOVERANGE;
        }
        else
        {
            m_iDistanceState = LA_S_D_BEFOREMEET;
        }
    }
    else
    {
        if (m_fDistanceToTarget < m_fAttack1Range)
        {
            m_iDistanceState = LA_S_D_ATTACKRANGE;
        }
        else if (m_fDistanceToTarget >= m_fAttack1Range && m_fDistanceToTarget < m_fOutRange)
        {
            m_iDistanceState = LA_S_D_MOVERANGE;
        }
        else
        {
            m_iDistanceState = LA_S_D_OUTRANGE;
        }
    }
}

void CLivingArmor_Soldier::FSM_BeforeMeet(_float _fTimeDelta)
{
    UpdateDistanceState();

    // 가까워지면
    if (m_iDistanceState < LA_S_D_OUTRANGE)
    {
        m_iBaseState = LA_S_B_GENERATE;
        m_iActionState = LA_S_A_APPEAR;
        m_pModelCom->ChangeAnim(m_iActionState, 2.f);
        m_iEffectIdx = EFFECTMGR->PlayEffect("LivingArmor_Aura_Blue", shared_from_this());
    }
    // 멀때
    else
    {
    }
}

void CLivingArmor_Soldier::FSM_Generate(_float _fTimeDelta)
{
    if (m_iActionState == LA_S_A_APPEAR)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iActionState = LA_S_A_COMBATSTART;
            m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
        }
    }
    if (m_iActionState == LA_S_A_COMBATSTART)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            UpdateDistanceState();
            if (m_iDistanceState == LA_S_D_ATTACKRANGE)
            {
                m_iBaseState = LA_S_B_NORMAL;
                m_iActionState = LA_S_A_STDALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
            }
            else
            {
                m_iBaseState = LA_S_B_NORMAL;
                m_iActionState = LA_S_A_RUN;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
            }
        }
    }
}

void CLivingArmor_Soldier::FSM_Normal(_float _fTimeDelta)
{
    if(m_iActionState != LA_S_A_BLOCKED)
        UpdateDirection();

    switch (m_iDistanceState)
    {
    case LA_S_D_MOVERANGE:
        if (m_iActionState == LA_S_A_STDALT)
        {
            m_iActionState = LA_S_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        else if (m_iActionState == LA_S_A_RUN)
        {
            UpdateCoolTime(_fTimeDelta);

            MoveToTarget(_fTimeDelta);
            UpdateDistanceState();
            if (m_fDistanceToTarget < m_fAttack1Range)
            {
                m_iActionState = LA_S_A_STDALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            }
        }
        else
        {
            m_iActionState = LA_S_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        break;
    case LA_S_D_ATTACKRANGE:
        //if (m_iActionState == LA_S_A_RUN)
        //{
        //    m_iActionState = LA_S_A_STDIDL;
        //    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        //}
        if (m_iActionState == LA_S_A_RUN || m_iActionState == LA_S_A_STDALT)
        {
            UpdateDistanceState();
            UpdateCoolTime(_fTimeDelta);

            for (int i = LA_S_C_NORMALATTACK1; i < LA_S_C_NORMALATTACK1 + m_NormalAttackCount; i++)
            {
                if (m_bAttackReady[i])
                {
                    m_iActionState = i + (LA_S_A_ATTACK1 - LA_S_C_NORMALATTACK1);
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bAttackReady[i] = false;
                    m_iDistanceState = LA_S_D_ATTACKRANGE;
                    if (m_iActionState == LA_S_A_ATTACK1)
                    {
                        SetWeaponRimLight(true);
                    }
                    break;
                }
            }
        }
        if (m_iActionState == LA_S_A_ATTACK1)
        {
            if (m_bParried)
            {
                GAMEINSTANCE->PlaySoundW("LS_Parried", m_fVoiceSize);
                SetWeaponRimLight(false);
                m_bParried = false;
                m_iActionState = LA_S_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                m_iParriedPower = PARRIEDSTRONG;
            }

            if (m_pModelCom->GetIsFinishedAnimation())
            {
                SetWeaponRimLight(false);
                //OffCollisionWeapon();
                UpdateDistanceState();
                if (m_iDistanceState == LA_S_D_ATTACKRANGE)
                {
                    m_iActionState = LA_S_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                }
                else
                {
                    m_iActionState = LA_S_A_RUN;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                }
            }
        }
        if (m_iActionState == LA_S_A_ATTACK2)
        {
            if (m_bParried)
            {
                m_bParried = false;
                m_iActionState = LA_S_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                m_iParriedPower = PARRIEDWEAK;
            }

            if (m_pModelCom->GetIsFinishedAnimation())
            {
                UpdateDistanceState();
                if (m_iDistanceState == LA_S_D_ATTACKRANGE)
                {
                    m_iActionState = LA_S_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                }
                else
                {
                    m_iActionState = LA_S_A_RUN;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                }
            }
        }
        if (m_iActionState == LA_S_A_BLOCKED)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                UpdateDistanceState();
                if (m_iDistanceState == LA_S_D_ATTACKRANGE)
                {
                    m_iActionState = LA_S_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                }
                else
                {
                    m_iActionState = LA_S_A_RUN;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                }
            }
        }

        break;
    case LA_S_D_OUTRANGE:
        break;
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 광폭화상태
    CheckFury();
}

void CLivingArmor_Soldier::FSM_WhileFury(_float _fTimeDelta)
{
    if (m_iActionState == LA_S_A_FURY)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            UpdateDistanceState();
            if (m_iDistanceState == LA_S_D_ATTACKRANGE)
            {
                m_iBaseState = LA_S_B_FURY;
                m_iActionState = LA_S_A_STDALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
            }
            else
            {
                m_iBaseState = LA_S_B_FURY;
                m_iActionState = LA_S_A_RUN;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
            }
            m_fSpeed = m_fFurySpeed;
        }
    }
}

void CLivingArmor_Soldier::FSM_Fury(_float _fTimeDelta)
{
    if (m_iActionState != LA_S_A_BLOCKED)
        UpdateDirection();

    switch (m_iDistanceState)
    {
    case LA_S_D_MOVERANGE:
        if (m_iActionState == LA_S_A_STDALT)
        {
            m_iActionState = LA_S_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        else if (m_iActionState == LA_S_A_RUN)
        {
            UpdateCoolTime(_fTimeDelta);

            MoveToTarget(_fTimeDelta);
            UpdateDistanceState();
            if (m_fDistanceToTarget < m_fAttack1Range)
            {
                m_iActionState = LA_S_A_STDALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            }
        }
        else
        {
            m_iActionState = LA_S_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        break;
    case LA_S_D_ATTACKRANGE:
        //if (m_iActionState == LA_S_A_RUN)
        //{
        //    m_iActionState = LA_S_A_STDIDL;
        //    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        //}
        if (m_iActionState == LA_S_A_RUN || m_iActionState == LA_S_A_STDALT)
        {
            UpdateDistanceState();
            UpdateCoolTime(_fTimeDelta);

            for (int i = LA_S_C_FURYATTACK1; i < LA_S_C_FURYATTACK1 + m_FuryAttackCount; i++)
            {
                if (m_bAttackReady[i])
                {
                    m_iActionState = i + (LA_S_A_ATTACK1 - LA_S_C_FURYATTACK1);
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bAttackReady[i] = false;
                    m_iDistanceState = LA_S_D_ATTACKRANGE;
                    if (m_iActionState == LA_S_A_ATTACK1)
                        SetWeaponRimLight(true);
                    break;
                }
            }
        }
        if (m_iActionState == LA_S_A_ATTACK1)
        {
            if (m_bParried)
            {
                SetWeaponRimLight(false);
                m_bParried = false;
                m_iActionState = LA_S_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                m_iParriedPower = PARRIEDSTRONG;
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                SetWeaponRimLight(false);
                //OffCollisionWeapon();
                UpdateDistanceState();
                if (m_iDistanceState == LA_S_D_ATTACKRANGE)
                {
                    m_iActionState = LA_S_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                }
                else
                {
                    m_iActionState = LA_S_A_RUN;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                }
            }
        }
        if (m_iActionState == LA_S_A_ATTACK2)
        {
            if (m_bParried)
            {
                GAMEINSTANCE->PlaySoundW("LS_Parried", m_fVoiceSize);
                m_bParried = false;
                m_iActionState = LA_S_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                m_iParriedPower = PARRIEDWEAK;
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                UpdateDistanceState();
                if (m_iDistanceState == LA_S_D_ATTACKRANGE)
                {
                    m_iActionState = LA_S_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                }
                else
                {
                    m_iActionState = LA_S_A_RUN;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                }
            }
        }
        if (m_iActionState == LA_S_A_BLOCKED)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                UpdateDistanceState();
                if (m_iDistanceState == LA_S_D_ATTACKRANGE)
                {
                    m_iActionState = LA_S_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                }
                else
                {
                    m_iActionState = LA_S_A_RUN;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                }
            }
        }
        break;
    case LA_S_D_OUTRANGE:
        break;
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void CLivingArmor_Soldier::FSM_Hitted(_float _fTimeDelta)
{
    if (m_iActionState == LA_S_A_FRONTBE)
    {
        if (m_bHitted)
        {
            UpdateDirection();
            m_bHitted = false;
            m_iActionState = LA_S_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < m_fFuryHp)
            {
                m_iBaseState = LA_S_B_FURY;
            }
            else
                m_iBaseState = LA_S_B_NORMAL;
            m_iActionState = LA_S_A_STDALT;
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

void CLivingArmor_Soldier::FSM_Hitted2(_float _fTimeDelta)
{
    if (m_iActionState == LA_S_A_FRONTBE)
    {
        if (m_bHitted2)
        {
            //UpdateDirection();
            m_bHitted2 = false;
            m_iActionState = LA_S_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < m_fFuryHp)
            {
                m_iBaseState = LA_S_B_FURY;
            }
            else
                m_iBaseState = LA_S_B_NORMAL;
            m_iActionState = LA_S_A_STDALT;
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

void CLivingArmor_Soldier::FSM_Hitted3(_float _fTimeDelta)
{
    if (m_iActionState == LA_S_A_FRONTBE)
    {
        if (m_bHitted3)
        {
            UpdateDirection();
            m_bHitted3 = false;
            m_iActionState = LA_S_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < m_fFuryHp)
            {
                m_iBaseState = LA_S_B_FURY;
            }
            else
                m_iBaseState = LA_S_B_NORMAL;
            m_iActionState = LA_S_A_STDALT;
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

void CLivingArmor_Soldier::FSM_Stun(_float _fTimeDelta)
{
    if (m_iActionState == LA_S_A_STUN)
    {
        if (m_bStun)
        {
            UpdateDirection();
            m_bStun = false;
            m_iActionState = LA_S_A_STUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < m_fFuryHp)
            {
                m_iBaseState = LA_S_B_FURY;
            }
            else
                m_iBaseState = LA_S_B_NORMAL;
            m_iActionState = LA_S_A_STDALT;
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

void CLivingArmor_Soldier::FSM_Shocked(_float _fTimeDelta)
{
    if (m_iActionState == LA_S_A_SHOCKED)
    {
        if (m_bShocked)
        {
            UpdateDirection();
            m_bShocked = false;
            m_iActionState = LA_S_A_SHOCKED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < m_fFuryHp)
            {
                m_iBaseState = LA_S_B_FURY;
            }
            else
                m_iBaseState = LA_S_B_NORMAL;
            m_iActionState = LA_S_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
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

void CLivingArmor_Soldier::FSM_Down(_float _fTimeDelta)
{
    if (m_iActionState == LA_S_A_KDWN1)
    {
        if (m_bDown)
        {
            UpdateDirection();
            m_fCalculateTime = 0.f;
            m_bDown = false;
            m_iActionState = LA_S_A_KDWN1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iActionState = LA_S_A_KDWN2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == LA_S_A_KDWN2)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iActionState = LA_S_A_KDWN3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == LA_S_A_KDWN3)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            if (m_fHp < m_fFuryHp)
            {
                m_iBaseState = LA_S_B_FURY;
            }
            else
                m_iBaseState = LA_S_B_NORMAL;
            m_iActionState = LA_S_A_STDALT;
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

void CLivingArmor_Soldier::FSM_Death(_float _fTimeDelta)
{
    if ((m_iActionState == LA_S_A_DEATH) && (m_pModelCom->GetIsFinishedAnimation()))
    {
        m_iActionState = LA_S_A_DEAD;
        //m_pModelCom->ChangeAnim(m_iActionState, 0.01f, false);

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

void CLivingArmor_Soldier::CheckFury()
{
    if ((m_fHp < m_fFuryHp) && !m_bFury)
    {
        m_bHitted = false;
        m_bHitted2 = false;
        m_bHitted3 = false;
        m_bStun = false;
        m_bShocked = false;
        m_bDown = false;
        
        m_fHp = m_fFuryHp;
        m_bFury = true;
        m_iBaseState = LA_S_B_WHILEFURY;
        m_iActionState = LA_S_A_FURY;
        m_pModelCom->ChangeAnim(m_iActionState, 0.5f, false);
        SetWeaponRimLight(false);

        EFFECTMGR->StopEffect("LivingArmor_Aura_Blue", m_iEffectIdx);
        m_iEffectIdx = EFFECTMGR->PlayEffect("LivingArmor_Aura_Red", shared_from_this());
    }
}

void CLivingArmor_Soldier::CheckDead()
{
    // 사망
    if (m_fHp <= 0.f)
    {
        GAMEINSTANCE->PlaySoundW("LS_Die", m_fVoiceSize);
        m_bDead = true;
        m_iBaseState = LA_S_B_DEATH;
        m_iActionState = LA_S_A_DEATH;
        m_pModelCom->ChangeAnim(m_iActionState, 0.5f, false);
        m_pRigidBody->SimulationOff();
        EFFECTMGR->StopEffect("LivingArmor_Aura_Red", m_iEffectIdx);
        QUESTMGR->CheckDeadMonster(shared_from_this());
    }
}

void CLivingArmor_Soldier::CheckAttacked()
{
    // 피격시
    if (m_bHitted)
    {
        GAMEINSTANCE->PlaySoundW("LS_Hitted", m_fVoiceSize);
        SetWeaponRimLight(false);
        m_iBaseState = LA_S_B_HITTED;
        m_iActionState = LA_S_A_FRONTBE;
    }

    // 짧은경직
    if (m_bHitted2)
    {
        GAMEINSTANCE->PlaySoundW("LS_Hitted", m_fVoiceSize);
        SetWeaponRimLight(false);
        m_iBaseState = LA_S_B_HITTED2;
        m_iActionState = LA_S_A_FRONTBE;
    }

    // 긴경직
    if (m_bHitted3)
    {
        GAMEINSTANCE->PlaySoundW("LS_Hitted", m_fVoiceSize);
        SetWeaponRimLight(false);
        m_iBaseState = LA_S_B_HITTED3;
        m_iActionState = LA_S_A_FRONTBE;
    }

    // 스턴
    if (m_bStun)
    {
        GAMEINSTANCE->PlaySoundW("LS_Stun", m_fVoiceSize);
        SetWeaponRimLight(false);
        m_iBaseState = LA_S_B_STUN;
        m_iActionState = LA_S_A_STUN;
    }

    // 감전
    if (m_bShocked)
    {
        GAMEINSTANCE->PlaySoundW("LS_Shocked", m_fVoiceSize);
        SetWeaponRimLight(false);
        m_iBaseState = LA_S_B_SHOCKED;
        m_iActionState = LA_S_A_SHOCKED;
    }

    // 넘어지기
    if (m_bDown)
    {
        GAMEINSTANCE->PlaySoundW("LS_KnockDown", m_fVoiceSize);
        SetWeaponRimLight(false);
        m_iBaseState = LA_S_B_DOWN;
        m_iActionState = LA_S_A_KDWN1;
    }
}

void CLivingArmor_Soldier::UpdateCoolTime(_float _fTimeDelta)
{
    switch (m_iBaseState)
    {
    case LA_S_B_NORMAL:
        for (int i = LA_S_C_NORMALATTACK1; i < LA_S_C_NORMALATTACK1 + m_NormalAttackCount; i++)
        {
            if (!m_bAttackReady[i])
            {
                m_fAttackCoolTime[i] -= _fTimeDelta;
                if (m_fAttackCoolTime[i] < 0.f)
                {
                    m_bAttackReady[i] = true;
                    m_fAttackCoolTime[i] = m_fBAttackCoolTime[i];
                    for (int j = LA_S_C_NORMALATTACK1; j < LA_S_C_NORMALATTACK1 + m_NormalAttackCount; j++)
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
    case LA_S_B_FURY:
        for (int i = LA_S_C_FURYATTACK1; i < LA_S_C_FURYATTACK1 + m_FuryAttackCount; i++)
        {
            if (!m_bAttackReady[i])
            {
                m_fAttackCoolTime[i] -= _fTimeDelta;
                if (m_fAttackCoolTime[i] < 0.f)
                {
                    m_bAttackReady[i] = true;
                    m_fAttackCoolTime[i] = m_fBAttackCoolTime[i];
                    for (int j = LA_S_C_FURYATTACK1; j < LA_S_C_FURYATTACK1 + m_FuryAttackCount; j++)
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