#include "LivingArmor_Archer.h"
#include "Model.h"
#include "Weapon.h"
#include "Arrow.h"
#include "RigidBody.h"
#include "Animation.h"
#include "EffectMgr.h"
#include "PlayerProjectile.h"
#include "QuestMgr.h"

CLivingArmor_Archer::CLivingArmor_Archer()
    : CMonster()
{
}

CLivingArmor_Archer::~CLivingArmor_Archer()
{
}

HRESULT CLivingArmor_Archer::Initialize()
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

    m_pModelCom = GAMEINSTANCE->GetModel("LivingArmor_Archer");
    m_strModelName = "LivingArmor_Archer";
    AddComponent(L"Com_Model", m_pModelCom);
    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

    m_pModelCom->ChangeAnim(m_iActionState);

    if (FAILED(AddPartObjects("Bow_LivingArmor_Archer")))
        return E_FAIL;
    if (FAILED(AddPartObjects("Arrow_00001")))
        return E_FAIL;

    // 화살 생성후 집어넣기
    shared_ptr<CArrow::ARROW_DESC> pArrowDesc = make_shared<CArrow::ARROW_DESC>();
    pArrowDesc->tProjectile.pTarget = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
    pArrowDesc->m_iArrowType = 0;

    shared_ptr<CTransform::TRANSFORM_DESC> pTransformDesc = make_shared<CTransform::TRANSFORM_DESC>();
    pTransformDesc->fSpeedPerSec = 10.f;
    pTransformDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    for (int i = 0; i < m_iArrowCount; i++)
    {
        shared_ptr<CArrow> pArrow = CArrow::Create(pArrowDesc.get(), pTransformDesc.get());
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_MAIN, TEXT("Layer_Projectile"), pArrow)))
            return E_FAIL;
        m_vecArrow.push_back(pArrow);
    }

    // 스탯
    m_fHp = 100.f;
    m_fMaxHp = 100.f;
    m_fSpeed = 1.f;

    AddRigidBody("LivingArmor_Archer");
    m_pRigidBody->SimulationOff();

    m_vLockOnOffSetPosition = { 0.0f,1.5f,0.0f };

    PrepareMonsterHPBar(TEXT("리빙아머 궁수"));

    return S_OK;
}

void CLivingArmor_Archer::PriorityTick(_float _fTimeDelta)
{

    __super::PriorityTick(_fTimeDelta);
}

void CLivingArmor_Archer::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    // Base상태에 따른 FSM
    switch (m_iBaseState)
    {
    case LA_AC_B_BEFOREMEET:
        FSM_BeforeMeet(_fTimeDelta);
        break;
    case LA_AC_B_GENERATE:
        FSM_Generate(_fTimeDelta);
        break;
    case LA_AC_B_NORMAL:
        FSM_Normal(_fTimeDelta);
        break;
    case LA_AC_B_WHILEFURY:
        FSM_WhileFury(_fTimeDelta);
        break;
    case LA_AC_B_FURY:
        FSM_Fury(_fTimeDelta);
        break;
    case LA_AC_B_HITTED:
        FSM_Hitted(_fTimeDelta);
        break;
    case LA_AC_B_HITTED2:
        FSM_Hitted2(_fTimeDelta);
        break;
    case LA_AC_B_HITTED3:
        FSM_Hitted3(_fTimeDelta);
        break;
    case LA_AC_B_STUN:
        FSM_Stun(_fTimeDelta);
        break;
    case LA_AC_B_SHOCKED:
        FSM_Shocked(_fTimeDelta);
        break;
    case LA_AC_B_DOWN:
        FSM_Down(_fTimeDelta);
        break;
    case LA_AC_B_DEATH:
        FSM_Death(_fTimeDelta);
        break;
    }

    if (m_iBaseState == LA_AC_B_FURY && (m_iActionState != LA_AC_A_FRONTBE))
    {
        m_pModelCom->PlayAnimation(m_fFuryActionSpeed * _fTimeDelta);
    }
    else if (m_iActionState == LA_AC_A_FRONTBE)
    {
        switch (m_iBaseState)
        {
        case LA_AC_B_HITTED:
            m_pModelCom->PlayAnimation(1.f * _fTimeDelta);
            break;
        case LA_AC_B_HITTED2:
            m_pModelCom->PlayAnimation(0.6f * _fTimeDelta);
            break;
        case LA_AC_B_HITTED3:
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

void CLivingArmor_Archer::LateTick(_float _fTimeDelta)
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

HRESULT CLivingArmor_Archer::Render()
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

shared_ptr<CLivingArmor_Archer> CLivingArmor_Archer::Create(_float3 _vCreatePos)
{
    shared_ptr<CLivingArmor_Archer> pInstance = make_shared<CLivingArmor_Archer>();

    pInstance->SetCurrentPos(_vCreatePos); // 위치 지정
    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CLivingArmor_Archer");

    return pInstance;
}

HRESULT CLivingArmor_Archer::AddRigidBody(string _strModelKey)
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
    CapsuleDesc.strShapeTag = "LivingArmor_Archer_Body";
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

void CLivingArmor_Archer::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    // 피격시
    if (_ColData.eColFlag & COL_STATIC || _ColData.eColFlag & COL_PLAYER || _ColData.eColFlag & COL_CHECKBOX || _ColData.eColFlag & COL_MONSTER)
        return;

    // 일반피격
    if ((m_iActionState == LA_AC_A_RUN) || (m_iActionState == LA_AC_A_STDIDL)
        || ((m_iActionState == LA_AC_A_FRONTBE) && ((m_iBaseState != LA_AC_B_HITTED2) && (m_iBaseState != LA_AC_B_HITTED3))))
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

            if (iAbnormal == PLAYER_PJ_A_NORMAL)
            {
                m_bHitted = true;
            }
        }
    }

    // 짧은경직
    if ((m_iBaseState == LA_AC_B_NORMAL) || (m_iBaseState == LA_AC_B_FURY) || (m_iBaseState == LA_AC_B_HITTED) || (m_iBaseState == LA_AC_B_HITTED2))
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
    if ((m_iBaseState == LA_AC_B_NORMAL) || (m_iBaseState == LA_AC_B_FURY) || (m_iBaseState == LA_AC_B_HITTED)
        || (m_iBaseState == LA_AC_B_HITTED2) || (m_iBaseState == LA_AC_B_HITTED3))
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
    if ((m_iBaseState == LA_AC_B_NORMAL) || (m_iBaseState == LA_AC_B_FURY) || (m_iBaseState == LA_AC_B_HITTED)
        || (m_iBaseState == LA_AC_B_HITTED2) || (m_iBaseState == LA_AC_B_HITTED3) || (m_iBaseState == LA_AC_B_STUN))
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
    if ((m_iBaseState == LA_AC_B_NORMAL) || (m_iBaseState == LA_AC_B_FURY) || (m_iBaseState == LA_AC_B_HITTED)
        || (m_iBaseState == LA_AC_B_HITTED2) || (m_iBaseState == LA_AC_B_HITTED3) || (m_iBaseState == LA_AC_B_STUN)
        || (m_iBaseState == LA_AC_B_SHOCKED))
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
    if ((m_iBaseState == LA_AC_B_NORMAL) || (m_iBaseState == LA_AC_B_FURY) || (m_iBaseState == LA_AC_B_HITTED)
        || (m_iBaseState == LA_AC_B_HITTED2) || (m_iBaseState == LA_AC_B_HITTED3) || (m_iBaseState == LA_AC_B_STUN)
        || (m_iBaseState == LA_AC_B_SHOCKED) || (m_iBaseState == LA_AC_B_DOWN))
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
    if (((m_iBaseState == LA_AC_B_NORMAL) || (m_iBaseState == LA_AC_B_FURY) || (m_iBaseState == LA_AC_B_HITTED)
        || (m_iBaseState == LA_AC_B_HITTED2) || (m_iBaseState == LA_AC_B_HITTED3) || (m_iBaseState == LA_AC_B_STUN)
        || (m_iBaseState == LA_AC_B_SHOCKED) || (m_iBaseState == LA_AC_B_DOWN))
        && !(_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        ToggleHPBar();

        DamagedByPlayer(_ColData, _szMyShapeTag);
        __super::ShowEffect(_ColData, _szMyShapeTag);
    }
}

void CLivingArmor_Archer::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CLivingArmor_Archer::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CLivingArmor_Archer::EnableCollision(const char* _strShapeTag)
{
    m_pRigidBody->EnableCollision(_strShapeTag);
}

void CLivingArmor_Archer::DisableCollision(const char* _strShapeTag)
{
    m_pRigidBody->DisableCollision(_strShapeTag);
}

HRESULT CLivingArmor_Archer::AddPartObjects(string _strModelKey)
{
    shared_ptr<CWeapon::WEAPON_DESC> pWeaponDesc = make_shared<CWeapon::WEAPON_DESC>();
    pWeaponDesc->tPartObject.pParentTransform = m_pTransformCom;
    if (_strModelKey == "Bow_LivingArmor_Archer")
    {
        pWeaponDesc->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_LeftHand");
    }
    if (_strModelKey == "Arrow_00001")
    {
        pWeaponDesc->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_RightHand");
    }
    pWeaponDesc->strModelKey = _strModelKey;
    pWeaponDesc->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(90.f)) *
        SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(90.f));

    shared_ptr<CTransform::TRANSFORM_DESC> pTransformDesc = make_shared<CTransform::TRANSFORM_DESC>();

    pTransformDesc->fSpeedPerSec = 10.f;
    pTransformDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    shared_ptr<CWeapon> pWeapon = CWeapon::Create(pWeaponDesc.get(), pTransformDesc.get());
    if (nullptr == pWeapon)
        return E_FAIL;

    if (_strModelKey == "Bow_LivingArmor_Archer")
    {
        shared_ptr<CPartObject> pPartObject = static_pointer_cast<CPartObject>(pWeapon);
        m_pMonsterParts.emplace("Part_Weapon", pPartObject);
    }
    if (_strModelKey == "Arrow_00001")
    {
        shared_ptr<CPartObject> pPartObject = static_pointer_cast<CPartObject>(pWeapon);
        pPartObject->SetActive(false);
        m_pMonsterParts.emplace("Part_Arrow", pPartObject);
    }

    return S_OK;
}


void CLivingArmor_Archer::UpdateDistanceState()
{
    // 위치 바로 못받아오는거 때문에 우선
    if (m_vTargetPos == _float3(0.f, 0.f, 0.f))
    {
        m_iDistanceState = LA_AC_D_BEFOREMEET;
        return;
    }

    if (m_iDistanceState == LA_AC_D_BEFOREMEET)
    {
        if (m_fDistanceToTarget < m_fMeetRange)
        {
            m_iDistanceState = LA_AC_D_MOVERANGE;
        }
        else
        {
            m_iDistanceState = LA_AC_D_BEFOREMEET;
        }
    }
    else
    {
        if (m_fDistanceToTarget < m_fAttack1Range)
        {
            m_iDistanceState = LA_AC_D_ATTACKRANGE;
        }
        else if (m_fDistanceToTarget >= m_fAttack1Range && m_fDistanceToTarget < m_fOutRange)
        {
            m_iDistanceState = LA_AC_D_MOVERANGE;
        }
        else
        {
            m_iDistanceState = LA_AC_D_OUTRANGE;
        }
    }
}

void CLivingArmor_Archer::FSM_BeforeMeet(_float _fTimeDelta)
{
    UpdateDistanceState();

    // 가까워지면
    if (m_iDistanceState < LA_AC_D_OUTRANGE)
    {
        m_iBaseState = LA_AC_B_GENERATE;
        m_iActionState = LA_AC_A_APPEAR;
        m_pModelCom->ChangeAnim(m_iActionState, 2.f);
        m_iEffectIdx = EFFECTMGR->PlayEffect("LivingArmor_Aura_Blue", shared_from_this());
    }
    // 멀때
    else
    {
    }
}

void CLivingArmor_Archer::FSM_Generate(_float _fTimeDelta)
{
    if (m_iActionState == LA_AC_A_APPEAR)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iActionState = LA_AC_A_COMBATSTART;
            m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
        }
    }
    if (m_iActionState == LA_AC_A_COMBATSTART)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iBaseState = LA_AC_B_NORMAL;
            m_iActionState = LA_AC_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
            UpdateDistanceState();
        }
    }
}

void CLivingArmor_Archer::FSM_Normal(_float _fTimeDelta)
{
    UpdateDirection();

    switch (m_iDistanceState)
    {
    case LA_AC_D_MOVERANGE:
        if (m_iActionState == LA_AC_A_STDIDL)
        {
            m_iActionState = LA_AC_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        else if (m_iActionState == LA_AC_A_RUN)
        {
            UpdateCoolTime(_fTimeDelta);

            MoveToTarget(_fTimeDelta);
            UpdateDistanceState();
        }
        else
        {
            m_iActionState = LA_AC_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        break;
    case LA_AC_D_ATTACKRANGE:
        if (m_iActionState == LA_AC_A_STDIDL)
        {
            UpdateDistanceState();
            UpdateCoolTime(_fTimeDelta);

            for (int i = LA_AC_C_NORMALATTACK1; i < LA_AC_C_NORMALATTACK1 + m_NormalAttackCount; i++)
            {
                if (m_bAttackReady[i])
                {
                    m_iActionState = i + (LA_AC_A_ATTACK1 - LA_AC_C_NORMALATTACK1);
                    m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                    m_bAttackReady[i] = false;
                    m_iDistanceState = LA_AC_D_ATTACKRANGE;
                    break;
                }
            }
        }
        if (m_iActionState == LA_AC_A_RUN)
        {
            m_iActionState = LA_AC_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        if (m_iActionState == LA_AC_A_ATTACK1)
        {
            m_fCalculateTime += _fTimeDelta;

            if ((m_fCalculateTime > m_fAttack1ArrowGenTime) && !m_bPartArrowGen
                && (m_fCalculateTime < m_fAttack1ArrowGenTime + 0.1f))
            {
                (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(true);
                m_bPartArrowGen = true;
            }
            if ((m_fCalculateTime > m_fAttack1ArrowOutTime) && m_bPartArrowGen)
            {
                (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(false);
                m_bPartArrowGen = false;
                ActiveNormalArrow();
            }

            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = LA_AC_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.2f, true);
                m_fCalculateTime = 0.f;
                UpdateDistanceState();
            }
        }
        if (m_iActionState == LA_AC_A_ATTACK2)
        {
            m_fCalculateTime += _fTimeDelta;

            if ((m_fCalculateTime > m_fAttack2ArrowGenTime) && !m_bPartArrowGen
                && (m_fCalculateTime < m_fAttack2ArrowGenTime + 0.1f))
            {
                (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(true);
                m_bPartArrowGen = true;
            }
            if ((m_fCalculateTime > m_fAttack2ArrowOutTime) && m_bPartArrowGen)
            {
                (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(false);
                m_bPartArrowGen = false;
                ActivePowerArrow();
            }

            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = LA_AC_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.4f, true);
                m_fCalculateTime = 0.f;
                UpdateDistanceState();
            }
        }

        break;
    case LA_AC_D_OUTRANGE:
        break;
    }

    // 피격시
    CheckAttacked();
    // 광폭화여부
    CheckFury();
}

void CLivingArmor_Archer::FSM_WhileFury(_float _fTimeDelta)
{
    if (m_iActionState == LA_AC_A_FURY)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iBaseState = LA_AC_B_FURY;
            m_iActionState = LA_AC_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);

            UpdateDistanceState();

            m_fSpeed = m_fFurySpeed;
        }
    }
}

void CLivingArmor_Archer::FSM_Fury(_float _fTimeDelta)
{
    UpdateDirection();

    switch (m_iDistanceState)
    {
    case LA_AC_D_MOVERANGE:
        if (m_iActionState == LA_AC_A_STDIDL)
        {
            m_iActionState = LA_AC_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        else if (m_iActionState == LA_AC_A_RUN)
        {
            UpdateCoolTime(_fTimeDelta);

            MoveToTarget(_fTimeDelta);
            UpdateDistanceState();
        }
        else
        {
            m_iActionState = LA_AC_A_RUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        break;
    case LA_AC_D_ATTACKRANGE:
        if (m_iActionState == LA_AC_A_STDIDL)
        {
            UpdateDistanceState();
            UpdateCoolTime(_fTimeDelta);

            for (int i = LA_AC_C_FURYATTACK1; i < LA_AC_C_FURYATTACK1 + m_FuryAttackCount; i++)
            {
                if (m_bAttackReady[i])
                {
                    m_iActionState = i + (LA_AC_A_ATTACK1 - LA_AC_C_FURYATTACK1);
                    m_pModelCom->ChangeAnim(m_iActionState, 0.25f);
                    m_bAttackReady[i] = false;
                    m_iDistanceState = LA_AC_D_ATTACKRANGE;
                    break;
                }
            }
        }
        if (m_iActionState == LA_AC_A_RUN)
        {
            m_iActionState = LA_AC_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
        if (m_iActionState == LA_AC_A_ATTACK1)
        {
            m_fCalculateTime += _fTimeDelta;

            if ((m_fCalculateTime > m_fFAttack1ArrowGenTime) && !m_bPartArrowGen
                && (m_fCalculateTime < m_fFAttack1ArrowGenTime + 0.1f))
            {
                (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(true);
                m_bPartArrowGen = true;
            }
            if ((m_fCalculateTime > m_fFAttack1ArrowOutTime) && m_bPartArrowGen)
            {
                (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(false);
                m_bPartArrowGen = false;
                ActiveNormalArrow();
            }

            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = LA_AC_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_fCalculateTime = 0.f;
                UpdateDistanceState();
            }
        }
        if (m_iActionState == LA_AC_A_ATTACK2)
        {
            m_fCalculateTime += _fTimeDelta;

            if ((m_fCalculateTime > m_fFAttack2ArrowGenTime) && !m_bPartArrowGen
                && (m_fCalculateTime < m_fFAttack2ArrowGenTime + 0.1f))
            {
                (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(true);
                m_bPartArrowGen = true;
            }
            if ((m_fCalculateTime > m_fFAttack2ArrowOutTime) && m_bPartArrowGen)
            {
                (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(false);
                m_bPartArrowGen = false;
                ActivePowerArrow();
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = LA_AC_A_STDIDL;
                m_pModelCom->ChangeAnim(m_iActionState, 0.2f, true);
                m_fCalculateTime = 0.f;
                UpdateDistanceState();
            }
        }

        break;
    case LA_AC_D_OUTRANGE:
        break;
    }

    // 피격시
    CheckAttacked();
    // 사망
    CheckDead();
}

void CLivingArmor_Archer::FSM_Hitted(_float _fTimeDelta)
{
    if (m_iActionState == LA_AC_A_FRONTBE)
    {
        if (m_bHitted)
        {
            UpdateDirection();
            m_bHitted = false;
            m_iActionState = LA_AC_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < m_fFuryHp)
            {
                m_iBaseState = LA_AC_B_FURY;
            }
            else
                m_iBaseState = LA_AC_B_NORMAL;
            m_iActionState = LA_AC_A_STDIDL;
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

void CLivingArmor_Archer::FSM_Hitted2(_float _fTimeDelta)
{
    if (m_iActionState == LA_AC_A_FRONTBE)
    {
        if (m_bHitted2)
        {
            //UpdateDirection();
            m_bHitted2 = false;
            m_iActionState = LA_AC_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < m_fFuryHp)
            {
                m_iBaseState = LA_AC_B_FURY;
            }
            else
                m_iBaseState = LA_AC_B_NORMAL;
            m_iActionState = LA_AC_A_STDIDL;
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

void CLivingArmor_Archer::FSM_Hitted3(_float _fTimeDelta)
{
    if (m_iActionState == LA_AC_A_FRONTBE)
    {
        if (m_bHitted3)
        {
            UpdateDirection();
            m_bHitted3 = false;
            m_iActionState = LA_AC_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < m_fFuryHp)
            {
                m_iBaseState = LA_AC_B_FURY;
            }
            else
                m_iBaseState = LA_AC_B_NORMAL;
            m_iActionState = LA_AC_A_STDIDL;
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

void CLivingArmor_Archer::FSM_Stun(_float _fTimeDelta)
{
    if (m_iActionState == LA_AC_A_STUN)
    {
        if (m_bStun)
        {
            UpdateDirection();
            m_bStun = false;
            m_iActionState = LA_AC_A_STUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.3f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < m_fFuryHp)
            {
                m_iBaseState = LA_AC_B_FURY;
            }
            else
                m_iBaseState = LA_AC_B_NORMAL;
            m_iActionState = LA_AC_A_STDIDL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.3f, true);
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

void CLivingArmor_Archer::FSM_Shocked(_float _fTimeDelta)
{
    if (m_iActionState == LA_AC_A_SHOCKED)
    {
        if (m_bShocked)
        {
            UpdateDirection();
            m_bShocked = false;
            m_iActionState = LA_AC_A_SHOCKED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            if (m_fHp < m_fFuryHp)
            {
                m_iBaseState = LA_AC_B_FURY;
            }
            else
                m_iBaseState = LA_AC_B_NORMAL;
            m_iActionState = LA_AC_A_STDIDL;
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

void CLivingArmor_Archer::FSM_Down(_float _fTimeDelta)
{
    if (m_iActionState == LA_AC_A_DOWN1)
    {
        if (m_bDown)
        {
            UpdateDirection();
            m_fCalculateTime = 0.f;
            m_bDown = false;
            m_iActionState = LA_AC_A_DOWN1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iActionState = LA_AC_A_DOWN2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == LA_AC_A_DOWN2)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iActionState = LA_AC_A_DOWN3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == LA_AC_A_DOWN3)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            if (m_fHp < m_fFuryHp)
            {
                m_iBaseState = LA_AC_B_FURY;
            }
            else
                m_iBaseState = LA_AC_B_NORMAL;
            m_iActionState = LA_AC_A_STDIDL;
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

void CLivingArmor_Archer::FSM_Death(_float _fTimeDelta)
{
    if ((m_iActionState == LA_AC_A_DEAD) && (m_pModelCom->GetIsFinishedAnimation()))
    {
        m_iActionState = LA_AC_A_COMBATSTART;
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

void CLivingArmor_Archer::CheckFury()
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
        m_iBaseState = LA_AC_B_WHILEFURY;
        m_iActionState = LA_AC_A_FURY;
        m_pModelCom->ChangeAnim(m_iActionState, 0.5f, false);

        m_fCalculateTime = 0.f;
        m_bPartArrowGen = false;
        (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(false);

        EFFECTMGR->StopEffect("LivingArmor_Aura_Blue", m_iEffectIdx);
        m_iEffectIdx = EFFECTMGR->PlayEffect("LivingArmor_Aura_Red", shared_from_this());
    }
}

void CLivingArmor_Archer::CheckDead()
{
    if (m_fHp <= 0.f)
    {
        GAMEINSTANCE->PlaySoundW("LA_Die", m_fVoiceSize);
        m_bDead = true;
        m_iBaseState = LA_AC_B_DEATH;
        m_iActionState = LA_AC_A_DEAD;
        m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);

        m_fCalculateTime = 0.f;
        m_bPartArrowGen = false;
        (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(false);
        m_pRigidBody->SimulationOff();
        EFFECTMGR->StopEffect("LivingArmor_Aura_Red", m_iEffectIdx);

        /* Quest */
        QUESTMGR->CheckDeadMonster(shared_from_this());
    }
}

void CLivingArmor_Archer::CheckAttacked()
{
    // 피격시
    if (m_bHitted)
    {
        GAMEINSTANCE->PlaySoundW("LA_Hitted", m_fVoiceSize);
        m_iBaseState = LA_AC_B_HITTED;
        m_iActionState = LA_AC_A_FRONTBE;

        m_fCalculateTime = 0.f;
        m_bPartArrowGen = false;
        (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(false);
    }

    // 짧은경직
    if (m_bHitted2)
    {
        GAMEINSTANCE->PlaySoundW("LA_Hitted", m_fVoiceSize);
        m_iBaseState = LA_AC_B_HITTED2;
        m_iActionState = LA_AC_A_FRONTBE;

        m_fCalculateTime = 0.f;
        m_bPartArrowGen = false;
        (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(false);
    }

    // 긴경직
    if (m_bHitted3)
    {
        GAMEINSTANCE->PlaySoundW("LA_Hitted", m_fVoiceSize);
        m_iBaseState = LA_AC_B_HITTED3;
        m_iActionState = LA_AC_A_FRONTBE;

        m_fCalculateTime = 0.f;
        m_bPartArrowGen = false;
        (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(false);
    }

    // 스턴
    if (m_bStun)
    {
        GAMEINSTANCE->PlaySoundW("LA_Stun", m_fVoiceSize);
        m_iBaseState = LA_AC_B_STUN;
        m_iActionState = LA_AC_A_STUN;

        m_fCalculateTime = 0.f;
        m_bPartArrowGen = false;
        (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(false);
    }

    // 감전
    if (m_bShocked)
    {
        GAMEINSTANCE->PlaySoundW("LA_Shocked", m_fVoiceSize);
        m_iBaseState = LA_AC_B_SHOCKED;
        m_iActionState = LA_AC_A_SHOCKED;

        m_fCalculateTime = 0.f;
        m_bPartArrowGen = false;
        (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(false);
    }

    // 넘어지기
    if (m_bDown)
    {
        GAMEINSTANCE->PlaySoundW("LA_KnockDown", m_fVoiceSize);
        m_iBaseState = LA_AC_B_DOWN;
        m_iActionState = LA_AC_A_DOWN1;

        m_fCalculateTime = 0.f;
        m_bPartArrowGen = false;
        (m_pMonsterParts.find("Part_Arrow"))->second->SetActiveOn(false);
    }
}

void CLivingArmor_Archer::UpdateCoolTime(_float _fTimeDelta)
{
    switch (m_iBaseState)
    {
    case LA_AC_B_NORMAL:
        for (int i = LA_AC_C_NORMALATTACK1; i < LA_AC_C_NORMALATTACK1 + m_NormalAttackCount; i++)
        {
            if (!m_bAttackReady[i])
            {
                m_fAttackCoolTime[i] -= _fTimeDelta;
                if (m_fAttackCoolTime[i] < 0.f)
                {
                    m_bAttackReady[i] = true;
                    m_fAttackCoolTime[i] = m_fBAttackCoolTime[i];
                    for (int j = LA_AC_C_NORMALATTACK1; j < LA_AC_C_NORMALATTACK1 + m_NormalAttackCount; j++)
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
    case LA_AC_B_FURY:
        for (int i = LA_AC_C_FURYATTACK1; i < LA_AC_C_FURYATTACK1 + m_FuryAttackCount; i++)
        {
            if (!m_bAttackReady[i])
            {
                m_fAttackCoolTime[i] -= _fTimeDelta;
                if (m_fAttackCoolTime[i] < 0.f)
                {
                    m_bAttackReady[i] = true;
                    m_fAttackCoolTime[i] = m_fBAttackCoolTime[i];
                    for (int j = LA_AC_C_FURYATTACK1; j < LA_AC_C_FURYATTACK1 + m_FuryAttackCount; j++)
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

void CLivingArmor_Archer::ActiveNormalArrow()
{
    for (int i = 0; i < m_iArrowCount; i++)
    {
        if (!(m_vecArrow[i]->IsEnabled()))
        {
            m_vecArrow[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos + _float3(0.f, 1.7f, 0.f));
            m_vecArrow[i]->UpdateTargetPos();
            m_vecArrow[i]->UpdateTargetDir(_float3(0.f, 1.5f, 0.f));
            m_vecArrow[i]->UpdateDirection();
            m_vecArrow[i]->GetTransform()->SetSpeed(15.f);
            m_vecArrow[i]->SetProjectileType(PJ_DIRECT);
            m_vecArrow[i]->SetEnable(true);
            m_vecArrow[i]->SetPower(1);
            m_vecArrow[i]->SetAbnormal(PJ_A_NORMAL);
            m_vecArrow[i]->OnCollisionProjectile();

            _int iIndex = EFFECTMGR->PlayEffect("Mon_Arrow_Basic", m_vecArrow[i]);
            m_vecArrow[i]->SetDeleteKey(iIndex);
            break;
        }
        // 다 활성상태면
        if (i == m_iArrowCount - 1)
        {
            m_vecArrow[i-1]->SetOnlyRender(false);
            m_vecArrow[i-1]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos + _float3(0.f, 1.7f, 0.f));
            m_vecArrow[i-1]->UpdateTargetPos();
            m_vecArrow[i-1]->UpdateTargetDir(_float3(0.f, 1.5f, 0.f));
            m_vecArrow[i-1]->UpdateDirection();
            m_vecArrow[i-1]->GetTransform()->SetSpeed(15.f);
            m_vecArrow[i-1]->SetProjectileType(PJ_DIRECT);
            m_vecArrow[i-1]->SetPower(1);
            m_vecArrow[i-1]->SetAbnormal(PJ_A_NORMAL);
            m_vecArrow[i-1]->OnCollisionProjectile();

            _int iIndex = EFFECTMGR->PlayEffect("Mon_Arrow_Basic", m_vecArrow[i - 1]);
            m_vecArrow[i-1]->SetDeleteKey(iIndex);
        }
    }
}

void CLivingArmor_Archer::ActivePowerArrow()
{
    for (int i = 0; i < m_iArrowCount; i++)
    {
        if (!(m_vecArrow[i]->IsEnabled()))
        {
            m_vecArrow[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos + _float3(0.f, 1.7f, 0.f));
            m_vecArrow[i]->UpdateTargetPos();
            m_vecArrow[i]->UpdateTargetDir(_float3(0.f, 1.5f, 0.f));
            m_vecArrow[i]->UpdateDirection();
            m_vecArrow[i]->GetTransform()->SetSpeed(4.f);
            m_vecArrow[i]->SetProjectileType(PJ_GUIDED);
            m_vecArrow[i]->SetEnable(true);
            m_vecArrow[i]->SetPower(3);
            m_vecArrow[i]->SetAbnormal(PJ_A_BIND);
            m_vecArrow[i]->OnCollisionProjectile();

            _int iIndex = EFFECTMGR->PlayEffect("Mon_Arrow_Enhanced", m_vecArrow[i]);
            m_vecArrow[i]->SetDeleteKey(iIndex);
            break;
        }
        // 다 활성상태면
        if (i == m_iArrowCount - 1)
        {
            m_vecArrow[i-1]->SetOnlyRender(false);
            m_vecArrow[i-1]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos + _float3(0.f, 1.7f, 0.f));
            m_vecArrow[i-1]->UpdateTargetPos();
            m_vecArrow[i-1]->UpdateTargetDir(_float3(0.f, 1.5f, 0.f));
            m_vecArrow[i-1]->UpdateDirection();
            m_vecArrow[i-1]->GetTransform()->SetSpeed(4.f);
            m_vecArrow[i-1]->SetProjectileType(PJ_GUIDED);
            m_vecArrow[i-1]->SetPower(3);
            m_vecArrow[i-1]->SetAbnormal(PJ_A_BIND);
            m_vecArrow[i-1]->OnCollisionProjectile();

            _int iIndex = EFFECTMGR->PlayEffect("Mon_Arrow_Enhanced", m_vecArrow[i - 1]);
            m_vecArrow[i-1]->SetDeleteKey(iIndex);
        }
    }
}
