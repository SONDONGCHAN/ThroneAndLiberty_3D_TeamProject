#include "Orc_DarkCommander.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "Animation.h"
#include "EffectMgr.h"
#include "PlayerProjectile.h"
#include "Memory.h"
#include "Player.h"
#include "EventMgr.h"
#include "PointTrail.h"
#include "MotionTrail.h"
#include "EffectAttachObj.h"

COrc_DarkCommander::COrc_DarkCommander()
    : CMonster()
{
}

COrc_DarkCommander::~COrc_DarkCommander()
{
}

HRESULT COrc_DarkCommander::Initialize()
{
    shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
    
    pArg->fSpeedPerSec = 1.f;
    pArg->fRotationPerSec = XMConvertToRadians(1.0f);
    
    shared_ptr<MONSTER_DESC> pMonsterDesc = make_shared<CMonster::MONSTER_DESC>();
    pMonsterDesc->pTarget = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
    
    __super::Initialize(pMonsterDesc.get(), pArg.get());
    
    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_ANIM;
    m_iShaderPass = 0;
    
    m_pModelCom = GAMEINSTANCE->GetModel("M_Orc_LowDarkCommander");
    m_strModelName = "M_Orc_LowDarkCommander";
    AddComponent(L"Com_Model", m_pModelCom);
    m_pTransformCom->SetScaling(1.3f, 1.3f, 1.3f);
    m_pTransformCom->Rotation(_float3(0.f, 1.f, 0.f), 3.14f);

    m_pModelCom->ChangeAnim(m_iActionState);

    if (FAILED(AddPartObjects("IT_N_Blunt_OrcLowDarkCommander")))
        return E_FAIL;

    // 기억의 파편 넣어주기
    shared_ptr<CMemory> pMemory = CMemory::Create(_float3(165.2f, 10.5f, 82.f), 1);
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_Memory"), pMemory)))
        return E_FAIL;
    m_pMemory = pMemory;

    // 스탯
    m_fHp = 500.f;
    m_fMaxHp = 500.f;
    m_fSpeed = 1.5f;

    AddRigidBody("M_Orc_LowDarkCommander");
    m_pRigidBody->SimulationOff();

    m_vLockOnOffSetPosition = { 0.0f,2.f,0.0f };

    //PrepareMonsterHPBar(TEXT("블랙 오크 지휘관"));
    m_strDisplayName = TEXT("블랙 오크 지휘관");

    // 처음에 다른곳에 배치
    MovePos(_float3(20.f, -205.f, 435.f));

    CEventMgr::GetInstance()->BindBoss2(static_pointer_cast<COrc_DarkCommander>(shared_from_this()));

    AddWeaponTrail();
    AddMotionTrail(10);

    return S_OK;
}

void COrc_DarkCommander::PriorityTick(_float _fTimeDelta)
{
    /*if (GAMEINSTANCE->KeyDown(DIK_L))
    {
        m_fHp -= 50.f;
    }*/
    /*if (GAMEINSTANCE->KeyDown(DIK_7))
    {
        if (!m_bQuickBatch)
        {
            m_bQuickBatch = true;
            m_pRigidBody->SimulationOn();
            MovePos(_float3(165.2f, 12.5f, 82.f));
        }
        else
        {
            m_bQuickBatch = false;
            ResetBoss();

            m_pRigidBody->SimulationOff();
            MovePos(_float3(20.f, -205.f, 435.f));
        }
    }
    if (GAMEINSTANCE->KeyDown(DIK_9))
    {
        m_fHp -= 100.f;
    }*/

    __super::PriorityTick(_fTimeDelta);
}

void COrc_DarkCommander::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    // Base상태에 따른 FSM
    switch (m_iBaseState)
    {
    case OC_DC_B_BEFOREMEET:
        FSM_BeforeMeet(_fTimeDelta);
        break;
    case OC_DC_B_GENERATE:
        FSM_Generate(_fTimeDelta);
        break;
    case OC_DC_B_NORMAL:
        FSM_Normal(_fTimeDelta);
        break;
    case OC_DC_B_BLOCKED:
        FSM_Blocked(_fTimeDelta);
        break;
    case OC_DC_B_HITTED:
        FSM_Hitted(_fTimeDelta);
        break;
    case OC_DC_B_HITTED2:
        FSM_Hitted2(_fTimeDelta);
        break;
    case OC_DC_B_HITTED3:
        FSM_Hitted3(_fTimeDelta);
        break;
    case OC_DC_B_STUN:
        FSM_Stun(_fTimeDelta);
        break;
    case OC_DC_B_SHOCKED:
        FSM_Shocked(_fTimeDelta);
        break;
    case OC_DC_B_DOWN:
        FSM_Down(_fTimeDelta);
        break;
    case OC_DC_B_DEATH:
        FSM_Death(_fTimeDelta);
        break;
    }

    // 애니메이션 속도 조절
    for (int i = 0; i < 100; i++)
    {
        switch (i)
        {
        case OC_DC_A_BLOCKED:
            switch (m_iParriedPower)
            {
            case PARRIEDWEAK:
                m_fActionAnimSpeed[i] = 1.f;
                break;
            case PARRIEDNORMAL:
                m_fActionAnimSpeed[i] = 1.f;
                break;
            case PARRIEDSTRONG:
                m_fActionAnimSpeed[i] = 0.4f;
                break;
            }
            break;
        case OC_DC_A_FRONTBE:
            switch (m_iBaseState)
            {
            case OC_DC_B_HITTED:
                m_fActionAnimSpeed[i] = 1.f;
                break;
            case OC_DC_B_HITTED2:
                m_fActionAnimSpeed[i] = 0.6f;
                break;
            case OC_DC_B_HITTED3:
                m_fActionAnimSpeed[i] = 0.3f;
                break;
            }
            break;
        case OC_DC_A_FURY:
            m_fActionAnimSpeed[i] = 0.7f;
            break;
        case OC_DC_A_DEATH:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        default:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        }
    }

    m_pModelCom->PlayAnimation(m_fActionAnimSpeed[m_iActionState] * m_fAnimSpeed * _fTimeDelta);

    m_pTransformCom->Tick(_fTimeDelta);
}

void COrc_DarkCommander::LateTick(_float _fTimeDelta)
{
    m_IsEdgeRender = false;

    m_pModelCom->CheckDisableAnim();
    m_pTransformCom->LateTick(_fTimeDelta);
    __super::LateTick(_fTimeDelta);

    if (m_bDslv)
        UpdateDslv(_fTimeDelta);

    if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 4.0f))
    {
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
        GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
    }
}

HRESULT COrc_DarkCommander::Render()
{
    if (m_bRender)
    {
        if (!m_pMonsterParts.begin()->second->GetActiveOn())
            m_pMonsterParts.begin()->second->SetActiveOn(true);
        __super::Render();

        _bool bRimOff = false;
        GAMEINSTANCE->BindRawValue("g_IsUsingRim", &bRimOff, sizeof(bRimOff));

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
    }
    
    if (!m_bRender)
    {
        if (m_pMonsterParts.begin()->second->GetActiveOn())
            m_pMonsterParts.begin()->second->SetActiveOn(false);
    }

    _bool bRimOff = false;
    GAMEINSTANCE->BindRawValue("g_IsUsingRim", &bRimOff, sizeof(bRimOff));

    _bool bDslvFalse = false;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &bDslvFalse, sizeof(_bool))))
        return E_FAIL;

    return S_OK;
}

_float COrc_DarkCommander::GetHPRatio()
{
    _float ratio = m_fHp / m_fMaxHp;

    return ratio;
}

shared_ptr<COrc_DarkCommander> COrc_DarkCommander::Create(_float3 _vCreatePos)
{
    shared_ptr<COrc_DarkCommander> pInstance = make_shared<COrc_DarkCommander>();

    pInstance->SetCurrentPos(_vCreatePos); // 위치 지정
    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : COrc_DarkCommander");

    return pInstance;
}

HRESULT COrc_DarkCommander::AddRigidBody(string _strModelKey)
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
    CapsuleDesc.fHeight = 1.3f;
    CapsuleDesc.fRadius = 0.4f;
    _float3 vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
    CapsuleDesc.vOffSetPosition = { 0.0f, 1.1f, 0.0f };
    CapsuleDesc.vOffSetRotation = { 0.0f, 0.0f, 90.0f };
    CapsuleDesc.strShapeTag = "Orc_DarkCommander_Body";
    RigidBodyDesc.pGeometry = &CapsuleDesc;

    RigidBodyDesc.eThisColFlag = COL_MONSTER;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER | COL_PLAYERWEAPON | COL_PLAYERPROJECTILE | COL_PLAYERSKILL | COL_CHECKBOX;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

    m_pTransformCom->SetRigidBody(m_pRigidBody);
    m_pRigidBody->SetMass(3.0f);

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

void COrc_DarkCommander::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    // 피격시
    if (_ColData.eColFlag & COL_STATIC || _ColData.eColFlag & COL_PLAYER || _ColData.eColFlag & COL_CHECKBOX)
        return;

    // 일반피격
    if ((m_iActionState == OC_DC_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG))
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

        /*if ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_1") ||
            (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack"))
            EFFECTMGR->PlayEffect("Hit_Left", shared_from_this());

        if ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_2") ||
            (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_3") ||
            (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack2"))
            EFFECTMGR->PlayEffect("Hit_Right", shared_from_this());*/
    }

    // 짧은경직
    if ((m_iActionState == OC_DC_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == OC_DC_B_HITTED) || (m_iBaseState == OC_DC_B_HITTED2))
    {
        if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack3") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_CM_SW2_DashAttack") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_1") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_2") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_3")))
        {
            /*EFFECTMGR->PlayEffect("Hit_ShortStun_M", shared_from_this());

            if ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack3"))
                EFFECTMGR->PlayEffect("Hit_GlowHaze", shared_from_this());*/

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
    if ((m_iActionState == OC_DC_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == OC_DC_B_HITTED) || (m_iBaseState == OC_DC_B_HITTED2)
        || (m_iBaseState == OC_DC_B_HITTED3))
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
    if ((m_iActionState == OC_DC_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == OC_DC_B_HITTED) || (m_iBaseState == OC_DC_B_HITTED2)
        || (m_iBaseState == OC_DC_B_HITTED3) || (m_iBaseState == OC_DC_B_STUN))
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
    if ((m_iActionState == OC_DC_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == OC_DC_B_HITTED) || (m_iBaseState == OC_DC_B_HITTED2)
        || (m_iBaseState == OC_DC_B_HITTED3) || (m_iBaseState == OC_DC_B_STUN) || (m_iBaseState == OC_DC_B_SHOCKED))
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
    if ((m_iActionState == OC_DC_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == OC_DC_B_HITTED) || (m_iBaseState == OC_DC_B_HITTED2)
        || (m_iBaseState == OC_DC_B_HITTED3) || (m_iBaseState == OC_DC_B_STUN) || (m_iBaseState == OC_DC_B_SHOCKED) || (m_iBaseState == OC_DC_B_DOWN))
    {
        if ((_ColData.eColFlag & COL_PLAYERWEAPON) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_PowerAttack")))
        {
            m_bDown = true;
            //EFFECTMGR->PlayEffect("Hit_GlowHaze", shared_from_this());
        }
        if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_UpperAttack")))
        {
            m_bDown = true;
            //EFFECTMGR->PlayEffect("Hit_Plr_Sk_Upper", shared_from_this());
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
    if ((_szMyShapeTag == m_strATT1Collider) && (_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        m_bParried = true;
    }
    if ((_szMyShapeTag == m_strATT2Collider) && (_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        m_bParried = true;
    }
    if ((_szMyShapeTag == m_strATT3Collider) && (_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        m_bParried = true;
    }

    // 무적기일때는 제외하고 체력감소
    if (((m_iBaseState == OC_DC_B_NORMAL) || (m_iBaseState == OC_DC_B_BLOCKED) || (m_iBaseState == OC_DC_B_HITTED)
        || (m_iBaseState == OC_DC_B_HITTED2) || (m_iBaseState == OC_DC_B_HITTED3) || (m_iBaseState == OC_DC_B_STUN)
        || (m_iBaseState == OC_DC_B_SHOCKED) || (m_iBaseState == OC_DC_B_DOWN))
        && !(_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        //ToggleHPBar();

        DamagedByPlayer(_ColData, _szMyShapeTag);
        __super::ShowEffect(_ColData, _szMyShapeTag);
    }
}

void COrc_DarkCommander::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void COrc_DarkCommander::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void COrc_DarkCommander::EnableCollision(const char* _strShapeTag)
{
    m_pRigidBody->EnableCollision(_strShapeTag);
}

void COrc_DarkCommander::DisableCollision(const char* _strShapeTag)
{
    m_pRigidBody->DisableCollision(_strShapeTag);
}

void COrc_DarkCommander::ResetBoss()
{
    m_fHp = m_fMaxHp;
    ResetTime();
    m_iBaseState = OC_DC_B_BEFOREMEET;
    m_iDistanceState = OC_DC_D_BEFOREMEET;
    m_iActionState = OC_DC_A_STDIDLE;
    m_iSkillState = OC_DC_S_NOSKILL;
    for (int i = 0; i < 10; i++)
    {
        m_bSkill1State[i] = false;
        m_bSkill2State[i] = false;
        m_bSkill3State[i] = false;
        m_bSkill4State[i] = false;
    }
    m_fAnimSpeed = 1.f;
}

void COrc_DarkCommander::ActiveMemory()
{
    if (!m_pMemory->IsEnabled())
    {
        m_pMemory->SetEnable(true);
        m_pMemory->SimulationOn();
        m_pMemory->SetCreatePos(m_vCurrentPos + _float3(0.f, 2.5f, 0.f) + m_pTransformCom->GetState(CTransform::STATE_LOOK) * 2);
    }
}

HRESULT COrc_DarkCommander::AddPartObjects(string _strModelKey)
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

    m_pWeapon = pWeapon;

    shared_ptr<CPartObject> pPartObject = static_pointer_cast<CPartObject>(pWeapon);
    m_pMonsterParts.emplace("Part_Weapon", pPartObject);

    return S_OK;
}


void COrc_DarkCommander::UpdateDistanceState()
{
    // 위치 바로 못받아오는거 때문에 우선
    if (m_vTargetPos == _float3(0.f, 0.f, 0.f))
    {
        m_iDistanceState = OC_DC_D_BEFOREMEET;
        return;
    }

    if (m_iDistanceState == OC_DC_D_BEFOREMEET)
    {
        if (m_fDistanceToTarget < m_fMeetRange)
        {
            m_iDistanceState = OC_DC_D_MOVERANGE;
        }
        else
        {
            m_iDistanceState = OC_DC_D_BEFOREMEET;
        }
    }
    else
    {
        if (m_fDistanceToTarget < m_fAttack1Range)
        {
            m_iDistanceState = OC_DC_D_ATTACKRANGE;
        }
        else if (m_fDistanceToTarget >= m_fAttack1Range && m_fDistanceToTarget < m_fOutRange)
        {
            m_iDistanceState = OC_DC_D_MOVERANGE;
        }
        else
        {
            m_iDistanceState = OC_DC_D_OUTRANGE;
        }
    }
}

void COrc_DarkCommander::UpdateMoveSlide(_float _fSlideStart, _float _fSlideEnd, _float _fSpeed, _float _fTimeDelta)
{
    if ((m_fCalculateTime > (_fSlideStart / m_fAnimSpeed)) && (m_fCalculateTime < (_fSlideEnd / m_fAnimSpeed)))
    {
        MovePos(m_vNormLookDirXZ * _fTimeDelta * _fSpeed + m_vCurrentPos);
    }
}

_int COrc_DarkCommander::CheckParryingStack(_int _iParryingStack)
{
    switch (_iParryingStack)
    {
    case 1:
        m_iParryType = 0;
        break;
    case 2:
        m_iParryType = 1;
        break;
    case 3:
        m_iParryType = 2;
        break;
    }

    m_iParryingStack += _iParryingStack;
    if (m_iParryingStack >= m_iBParryingStack)
    {
        m_iParryingStack = 0;
        return PARRIEDSTRONG;
    }
    return PARRIEDWEAK;
}

void COrc_DarkCommander::FSM_BeforeMeet(_float _fTimeDelta)
{
    if (m_bCameraStart)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_fCalculateTime > 4.5f)
        {
            m_bBattleStart = true;
            m_fCalculateTime = 0.f;
        }
    }

    if (m_bBattleStart)
    {
        UpdateDirection();
        m_iBaseState = OC_DC_B_GENERATE;
        m_iActionState = OC_DC_A_FURY;
        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
        m_bEffectReady = true;
        //GAMEINSTANCE->SwitchingBGM();
    }
    // 멀때
    else
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            switch (GAMEINSTANCE->PickRandomInt(0, 3))
            {
            case 0:
                m_iActionState = OC_DC_A_SKTRFA1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                break;
            case 1:
                m_iActionState = OC_DC_A_SKTRFA2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                break;
            case 2:
                m_iActionState = OC_DC_A_STDIDLE;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                break;
            }
        }
    }
}

void COrc_DarkCommander::FSM_Generate(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_FURY)
    {
        if (m_bEffectReady)
        {
            m_fCalculateTime2 += _fTimeDelta;
            if (m_fCalculateTime2 > 0.1f)
            {
                m_fCalculateTime2 = 0.f;
                m_bEffectReady = false;
                EFFECTMGR->PlayEffect("Mon_O_Scream", shared_from_this());
            }
        }
        
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            UpdateDirection();
            m_iBaseState = OC_DC_B_NORMAL;
            m_iActionState = OC_DC_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
    }
}

void COrc_DarkCommander::FSM_Normal(_float _fTimeDelta)
{
    UpdateDir(_fTimeDelta);
    
    // 패턴 잡기
    if (m_iSkillState == OC_DC_S_NOSKILL)
    {
        switch (m_iSkillOrder[m_iPatternNum])
        {
        case OC_DC_S_SKILL1:
            m_iSkillState = OC_DC_S_SKILL1;
            m_bSkill1State[0] = true;
            break;
        case OC_DC_S_SKILL2:
            m_iSkillState = OC_DC_S_SKILL2;
            m_bSkill2State[0] = true;
            break;
        case OC_DC_S_SKILL3:
            m_iSkillState = OC_DC_S_SKILL3;
            m_bSkill3State[0] = true;
            break;
        case OC_DC_S_SKILL4:
            m_iSkillState = OC_DC_S_SKILL4;
            m_bSkill4State[0] = true;
            break;
        }
        m_iPatternNum++;
    }
    
    switch (m_iSkillState)
    {
    case OC_DC_S_SKILL1:
        // 달려서 박치기
        if (m_bSkill1State[0])
        {
            if (m_iActionState == OC_DC_A_STDALT)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fSkill1StandTime)
                {
                    ResetTime();
                    m_iActionState = OC_DC_A_DASH1;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == OC_DC_A_DASH1)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
                {
                    ResetTime();
                    m_iActionState = OC_DC_A_DASH2;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_fSpeed = m_fSkill1DashSpeed;
                    SetUseMotionTrail(true, true, 1.f);
                    EFFECTMGR->PlayEffect("Mon_O_Floor", shared_from_this());
                }
            }
            if (m_iActionState == OC_DC_A_DASH2)
            {
                UpdateDirection();
                m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);
                
                // 거리가 가까우면
                if (m_fDistanceToTarget < m_fDashEndRange)
                {
                    m_fSpeed = 1.f;
                    m_iActionState = OC_DC_A_DASH3;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == OC_DC_A_DASH3)
            {
                UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 7.f + 0.1f, m_fDash3SlideSpeed, _fTimeDelta);
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
                {
                    // 가까우면
                    if (m_fDistanceToTarget < m_fAttack1Range)
                    {
                        ResetTime();
                        m_iActionState = OC_DC_A_ATTACK2_C1;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        m_bSkill1State[0] = false;
                        m_bSkill1State[1] = true;
                        EFFECTMGR->StopEffect("Mon_O_Floor", 0);
                    }
                    // 멀면
                    else
                    {
                        UpdateDirection();
                        ResetTime();
                        m_fSpeed = m_fRunSpeed;
                        m_iActionState = OC_DC_A_RUN;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        EFFECTMGR->StopEffect("Mon_O_Floor", 0);
                    }
                }
            }
            if (m_iActionState == OC_DC_A_RUN)
            {
                m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);
                // 가까우면
                if (m_fDistanceToTarget < m_fAttack1Range)
                {
                    m_fSpeed = 1.f;
                    m_iActionState = OC_DC_A_ATTACK2_C1;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill1State[0] = false;
                    m_bSkill1State[1] = true;
                }
            }
        }
        // 돌아서 공격
        if (m_bSkill1State[1])
        {
            if (UpdateSAttackState(_fTimeDelta))
            {
                // 패링되었으면
                if (m_iBaseState == OC_DC_B_BLOCKED)
                {
                    m_bSkill1State[1] = false;
                }
                else
                {
                    m_iActionState = OC_DC_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                }
            }
            if (m_iActionState == OC_DC_A_STDALT)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fSkill1TimeBeforeFury)
                {
                    ResetTime();
                    m_iActionState = OC_DC_A_ATTACK2_B1;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill1State[1] = false;
                    m_bSkill1State[2] = true;
                }
            }
        }
        // 2연격
        if (m_bSkill1State[2])
        {
            if (Update2ComboState(_fTimeDelta))
            {
                m_iActionState = OC_DC_A_FURY;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                m_bSkill1State[2] = false;
                m_bSkill1State[3] = true;
                m_bEffectReady = true;
            }
        }
        // 포효(광역기)
        if (m_bSkill1State[3])
        {
            if (m_iActionState == OC_DC_A_FURY)
            {
                if (m_bEffectReady)
                {
                    m_fCalculateTime2 += _fTimeDelta;
                    if (m_fCalculateTime2 > 0.1f)
                    {
                        m_fCalculateTime2 = 0.f;
                        m_bEffectReady = false;
                        EFFECTMGR->PlayEffect("Mon_O_Scream", shared_from_this());
                    }
                }

                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
                {
                    //UpdateDirection();
                    ResetTime();
                    m_iActionState = OC_DC_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_bSkill1State[3] = false;
                    m_iSkillState = OC_DC_S_NOSKILL;
                }
            }
        }
        break;

    case OC_DC_S_SKILL2:
        // 달려서 박치기
        if (m_bSkill2State[0])
        {
            if (m_iActionState == OC_DC_A_STDALT)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fSkill2StandTime)
                {
                    ResetTime();
                    m_iActionState = OC_DC_A_DASH1;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == OC_DC_A_DASH1)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
                {
                    ResetTime();
                    m_iActionState = OC_DC_A_DASH2;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_fSpeed = m_fSkill2DashSpeed;
                    SetUseMotionTrail(true, true, 1.f);
                    EFFECTMGR->PlayEffect("Mon_O_Floor", shared_from_this());
                }
            }
            if (m_iActionState == OC_DC_A_DASH2)
            {
                UpdateDirection();
                m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);

                // 거리가 가까우면
                if (m_fDistanceToTarget < m_fDashEndRange)
                {
                    m_fSpeed = 1.f;
                    m_iActionState = OC_DC_A_DASH3;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == OC_DC_A_DASH3)
            {
                UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 7.f + 0.1f, m_fDash3SlideSpeed, _fTimeDelta);
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
                {
                    // 가까우면
                    if (m_fDistanceToTarget < m_fAttack1Range)
                    {
                        ResetTime();
                        m_iActionState = OC_DC_A_ATTACK3_A1;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        m_bSkill2State[0] = false;
                        m_bSkill2State[1] = true;

                        SetWeaponMeshTrail(true, _float4(0.25f, 0.459f, 0.f, 0.8f), true, 1.7f);
                        m_pWeaponTrail->StartTrail(_float4(0.25f, 0.459f, 0.f, 0.8f), _float4(0.761f, 0.878f, 0.129f, 1.f), true, true, true, 0.33f);
                        EFFECTMGR->StopEffect("Mon_O_Floor", 0);
                    }
                    // 멀면
                    else
                    {
                        UpdateDirection();
                        ResetTime();
                        m_fSpeed = m_fRunSpeed;
                        m_iActionState = OC_DC_A_RUN;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        EFFECTMGR->StopEffect("Mon_O_Floor", 0);
                    }
                }
            }
            if (m_iActionState == OC_DC_A_RUN)
            {
                m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);
                // 가까우면
                if (m_fDistanceToTarget < m_fAttack1Range)
                {
                    m_fSpeed = 1.f;
                    m_iActionState = OC_DC_A_ATTACK3_A1;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill2State[0] = false;
                    m_bSkill2State[1] = true;

                    SetWeaponMeshTrail(true, _float4(0.25f, 0.459f, 0.f, 0.8f), true, 1.7f);
                    m_pWeaponTrail->StartTrail(_float4(0.25f, 0.459f, 0.f, 0.8f), _float4(0.761f, 0.878f, 0.129f, 1.f), true, true, true, 0.33f);
                }
            }
        }
        // 특수연격
        if (m_bSkill2State[1])
        {
            if (UpdateSpecialState(_fTimeDelta))
            {
                // 패링되었으면
                if (m_iBaseState == OC_DC_B_BLOCKED)
                {
                    m_bSkill2State[1] = false;
                }
                else
                {
                    UpdateDirection();
                    m_iActionState = OC_DC_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                }
            }
            if (m_iActionState == OC_DC_A_STDALT)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fBeforeQuakeTime)
                {
                    ResetTime();
                    m_iActionState = OC_DC_A_ATTACK4;
                    m_fAnimSpeed = m_fAttack4AnimSpeed;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                    m_bSkill2State[1] = false;
                    m_bSkill2State[2] = true;
                    m_bEffectReady = true;
                }
            }
        }
        // 지면강타
        if (m_bSkill2State[2])
        {
            m_fCalculateTime += _fTimeDelta;
            if ((m_fCalculateTime > (0.f * m_fKeyFrameSpeed + 0.1f / m_fAnimSpeed)) && (m_fCalculateTime < (11.f * m_fKeyFrameSpeed + 0.1f / m_fAnimSpeed)))
            {
                m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            }
            if (m_fCalculateTime > 0.3f && m_bEffectReady)
            {
                m_bEffectReady = false;
                EFFECTMGR->PlayEffect("Mon_O_EarthQuake", shared_from_this());
                EFFECTMGR->PlayEffect("Orc_EarthQuake_Anim", shared_from_this());

                SetWeaponMeshTrail(true, _float4(0.149f, 0.278f, 0.5f, 0.8f), true, 1.7f);
                m_pWeaponTrail->StartTrail(_float4(0.149f, 0.278f, 0.5f, 0.8f), _float4(0.149f, 0.529f, 0.678f, 1.f), true, true, true, 0.33f);
            }

            if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
            {
                m_fAnimSpeed = 1.f;
                ResetTime();
                m_iActionState = OC_DC_A_STDALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_bSkill2State[2] = false;
                m_iSkillState = OC_DC_S_NOSKILL;
            }
        }

        break;

    case OC_DC_S_SKILL3:
        // 달려서 박치기
        if (m_bSkill3State[0])
        {
            if (m_iActionState == OC_DC_A_STDALT)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fSkill3StandTime)
                {
                    ResetTime();
                    m_iActionState = OC_DC_A_DASH1;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == OC_DC_A_DASH1)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
                {
                    ResetTime();
                    m_iActionState = OC_DC_A_DASH2;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_fSpeed = m_fSkill3DashSpeed;
                    SetUseMotionTrail(true, true, 1.f);
                    EFFECTMGR->PlayEffect("Mon_O_Floor", shared_from_this());
                }
            }
            if (m_iActionState == OC_DC_A_DASH2)
            {
                UpdateDirection();
                m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);

                // 거리가 가까우면
                if (m_fDistanceToTarget < m_fDashEndRange)
                {
                    m_fSpeed = 1.f;
                    m_iActionState = OC_DC_A_DASH3;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == OC_DC_A_DASH3)
            {
                UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 7.f + 0.1f, m_fDash3SlideSpeed, _fTimeDelta);
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
                {
                    ResetTime();
                    m_iActionState = OC_DC_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_bSkill3State[0] = false;
                    m_bSkill3State[7] = true;
                    EFFECTMGR->StopEffect("Mon_O_Floor", 0);
                }
            }
        }
        if (m_bSkill3State[7])
        {
            if (m_iActionState == OC_DC_A_STDALT)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fBeforeQuakeTime)
                {
                    ResetTime();
                    UpdateDirection();
                    m_iActionState = OC_DC_A_ATTACK4;
                    m_fAnimSpeed = m_fAttack4AnimSpeed;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                    m_bSkill3State[7] = false;
                    m_bSkill3State[1] = true;
                    m_bEffectReady = true;
                }
            }
        }
        // 지면강타
        if (m_bSkill3State[1])
        {
            if (m_iActionState == OC_DC_A_ATTACK4)
            {
                m_fCalculateTime += _fTimeDelta;
                if ((m_fCalculateTime > (0.f * m_fKeyFrameSpeed + 0.1f / m_fAnimSpeed)) && (m_fCalculateTime < (11.f * m_fKeyFrameSpeed + 0.1f / m_fAnimSpeed)))
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                }
                if (m_fCalculateTime > 0.3f && m_bEffectReady)
                {
                    m_bEffectReady = false;
                    EFFECTMGR->PlayEffect("Mon_O_EarthQuake", shared_from_this());
                    EFFECTMGR->PlayEffect("Orc_EarthQuake_Anim", shared_from_this());

                    SetWeaponMeshTrail(true, _float4(0.149f, 0.278f, 0.5f, 0.8f), true, 1.7f);
                    m_pWeaponTrail->StartTrail(_float4(0.149f, 0.278f, 0.5f, 0.8f), _float4(0.149f, 0.529f, 0.678f, 1.f), true, true, true, 0.33f);
                }
                if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
                {
                    m_fAnimSpeed = 1.f;
                    if (m_fDistanceToTarget < m_fAttack1Range)
                    {
                        ResetTime();
                        m_iActionState = OC_DC_A_ATTACK2_A1;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        m_bSkill3State[1] = false;
                        m_bSkill3State[2] = true;
                    }
                    // 멀면
                    else
                    {
                        UpdateDirection();
                        ResetTime();
                        m_fSpeed = m_fRunSpeed;
                        m_iActionState = OC_DC_A_RUN;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    }
                }
            }
            if (m_iActionState == OC_DC_A_RUN)
            {
                m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);
                // 가까우면
                if (m_fDistanceToTarget < m_fAttack1Range)
                {
                    m_fSpeed = 1.f;
                    m_iActionState = OC_DC_A_ATTACK2_A1;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill3State[1] = false;
                    m_bSkill3State[2] = true;
                }
            }
        }
        // 3연격
        if (m_bSkill3State[2])
        {
            if (Update3ComboState(_fTimeDelta))
            {
                // 패링되었으면
                if (m_iBaseState == OC_DC_B_BLOCKED)
                {
                    m_bSkill3State[2] = false;
                }
                else
                {
                    m_iActionState = OC_DC_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                }
            }
            if (m_iActionState == OC_DC_A_STDALT)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fSkill3TimeBeforeFury)
                {
                    ResetTime();
                    m_iActionState = OC_DC_A_FURY;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill3State[2] = false;
                    m_bSkill3State[3] = true;
                    m_bEffectReady = true;
                }
            }
        }
        // 포효(광역기)
        if (m_bSkill3State[3])
        {
            if (m_iActionState == OC_DC_A_FURY)
            {
                if (m_bEffectReady)
                {
                    m_fCalculateTime2 += _fTimeDelta;
                    if (m_fCalculateTime2 > 0.1f)
                    {
                        m_fCalculateTime2 = 0.f;
                        m_bEffectReady = false;
                        EFFECTMGR->PlayEffect("Mon_O_Scream", shared_from_this());
                    }
                }
                
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
                {
                    //UpdateDirection();
                    ResetTime();
                    m_iActionState = OC_DC_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_bSkill3State[3] = false;
                    m_iSkillState = OC_DC_S_NOSKILL;
                }
            }
        }
        break;

    case OC_DC_S_SKILL4:
        // 달려서 박치기
        if (m_bSkill4State[0])
        {
            if (m_iActionState == OC_DC_A_STDALT)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fSkill4StandTime)
                {
                    ResetTime();
                    m_iActionState = OC_DC_A_DASH1;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == OC_DC_A_DASH1)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
                {
                    ResetTime();
                    m_iActionState = OC_DC_A_DASH2;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_fSpeed = m_fSkill4DashSpeed;
                    SetUseMotionTrail(true, true, 1.f);
                    EFFECTMGR->PlayEffect("Mon_O_Floor", shared_from_this());
                }
            }
            if (m_iActionState == OC_DC_A_DASH2)
            {
                UpdateDirection();
                m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);

                // 거리가 가까우면
                if (m_fDistanceToTarget < m_fDashEndRange)
                {
                    m_fSpeed = 1.f;
                    m_iActionState = OC_DC_A_DASH3;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == OC_DC_A_DASH3)
            {
                UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 7.f + 0.1f, m_fDash3SlideSpeed, _fTimeDelta);
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
                {
                    // 가까우면
                    if (m_fDistanceToTarget < m_fAttack1Range)
                    {
                        ResetTime();
                        m_iActionState = OC_DC_A_ATTACK1_B;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        m_bSkill4State[0] = false;
                        m_bSkill4State[1] = true;
                        EFFECTMGR->StopEffect("Mon_O_Floor", 0);
                    }
                    // 멀면
                    else
                    {
                        UpdateDirection();
                        ResetTime();
                        m_fSpeed = m_fRunSpeed;
                        m_iActionState = OC_DC_A_RUN;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        EFFECTMGR->StopEffect("Mon_O_Floor", 0);
                    }
                }
            }
            if (m_iActionState == OC_DC_A_RUN)
            {
                m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);
                // 가까우면
                if (m_fDistanceToTarget < m_fAttack1Range)
                {
                    m_fSpeed = 1.f;
                    m_iActionState = OC_DC_A_ATTACK1_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill4State[0] = false;
                    m_bSkill4State[1] = true;
                }
            }
        }
        // 왼공격
        if (m_bSkill4State[1])
        {
            if (UpdateLAttackState(_fTimeDelta))
            {
                //UpdateDirection();
                m_iActionState = OC_DC_A_ATTACK3_A1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                m_bSkill4State[1] = false;
                m_bSkill4State[2] = true;

                SetWeaponMeshTrail(true, _float4(0.25f, 0.459f, 0.f, 0.8f), true, 1.7f);
                m_pWeaponTrail->StartTrail(_float4(0.25f, 0.459f, 0.f, 0.8f), _float4(0.761f, 0.878f, 0.129f, 1.f), true, true, true, 0.33f);
            }
        }
        // 특수연격
        if (m_bSkill4State[2])
        {
            if (UpdateSpecialState(_fTimeDelta))
            {
                // 패링되었으면
                if (m_iBaseState == OC_DC_B_BLOCKED)
                {
                    m_bSkill4State[2] = false;
                    m_bSkill4State[3] = true;
                }
                else
                {
                    if (m_fDistanceToTarget < m_fAttack1Range)
                    {
                        ResetTime();
                        m_iActionState = OC_DC_A_ATTACK2_B1;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        m_bSkill4State[2] = false;
                        m_bSkill4State[3] = true;
                    }
                    // 멀면
                    else
                    {
                        UpdateDirection();
                        ResetTime();
                        m_fSpeed = m_fRunSpeed;
                        m_iActionState = OC_DC_A_RUN;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    }
                }
            }
            if (m_iActionState == OC_DC_A_RUN)
            {
                m_pTransformCom->GoStraight(m_fSpeed* _fTimeDelta);
                // 가까우면
                if (m_fDistanceToTarget < m_fAttack1Range)
                {
                    ResetTime();
                    m_fSpeed = 1.f;
                    m_iActionState = OC_DC_A_ATTACK2_B1;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill4State[2] = false;
                    m_bSkill4State[3] = true;
                }
            }
        }
        // 2연격
        if (m_bSkill4State[3])
        {
            if (Update2ComboState(_fTimeDelta))
            {
                // UpdateDirection();
                ResetTime();
                m_iActionState = OC_DC_A_STDALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_bSkill4State[3] = false;
                m_iSkillState = OC_DC_S_NOSKILL;
            }
        }
        break;
    }

    // 사망
    CheckDead();
}

void COrc_DarkCommander::FSM_Blocked(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_BLOCKED)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iBaseState = OC_DC_B_NORMAL;
            m_iActionState = OC_DC_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = OC_DC_S_NOSKILL;
        }
    }

    // 긴 Blocked 상태에서 공격들어온 종류 체크
    if (m_iActionState == OC_DC_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG)
    {
        CheckAttacked();
    }
    
    // 사망
    CheckDead();
}

void COrc_DarkCommander::FSM_Hitted(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_FRONTBE)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bHitted)
        {
            ResetTime();
            UpdateDirection();
            m_bHitted = false;
            m_iActionState = OC_DC_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iBaseState = OC_DC_B_NORMAL;
            m_iActionState = OC_DC_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = OC_DC_S_NOSKILL;
            m_bHitted = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void COrc_DarkCommander::FSM_Hitted2(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_FRONTBE)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bHitted2)
        {
            ResetTime();
            //UpdateDirection();
            m_bHitted2 = false;
            m_iActionState = OC_DC_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iBaseState = OC_DC_B_NORMAL;
            m_iActionState = OC_DC_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = OC_DC_S_NOSKILL;
            m_bHitted2 = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void COrc_DarkCommander::FSM_Hitted3(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_FRONTBE)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bHitted3)
        {
            ResetTime();
            UpdateDirection();
            m_bHitted3 = false;
            m_iActionState = OC_DC_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iBaseState = OC_DC_B_NORMAL;
            m_iActionState = OC_DC_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = OC_DC_S_NOSKILL;
            m_bHitted3 = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void COrc_DarkCommander::FSM_Stun(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_STUN)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bStun)
        {
            ResetTime();
            UpdateDirection();
            m_bStun = false;
            m_iActionState = OC_DC_A_STUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iBaseState = OC_DC_B_NORMAL;
            m_iActionState = OC_DC_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = OC_DC_S_NOSKILL;
            m_bStun = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void COrc_DarkCommander::FSM_Shocked(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_SHOCKED)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bShocked)
        {
            ResetTime();
            UpdateDirection();
            m_bShocked = false;
            m_iActionState = OC_DC_A_SHOCKED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iBaseState = OC_DC_B_NORMAL;
            m_iActionState = OC_DC_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = OC_DC_S_NOSKILL;
            m_bShocked = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void COrc_DarkCommander::FSM_Down(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_DOWN1)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bDown)
        {
            UpdateDirection();
            ResetTime();
            m_bDown = false;
            m_iActionState = OC_DC_A_DOWN1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_DOWN2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == OC_DC_A_DOWN2)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_DOWN3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == OC_DC_A_DOWN3)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iBaseState = OC_DC_B_NORMAL;
            m_iActionState = OC_DC_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = OC_DC_S_NOSKILL;
            m_bDown = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void COrc_DarkCommander::FSM_Death(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_DEATH)
    {
        if (!m_bMemoryOut)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > m_fMemoryOutTime)
            {
                ActiveMemory();
                m_fCalculateTime = 0.f;
                m_bMemoryOut = true;
            }
        }
    }
    if ((m_iActionState == OC_DC_A_DEATH) && (m_pModelCom->GetIsFinishedAnimation()))
    {
        m_iActionState = OC_DC_A_DEAD;
        //m_pModelCom->ChangeAnim(m_iActionState, 0.01f, false);

        SetDissolve(true, true, 3.f);
        SetDissolveInfo(1.f, m_vDisolveColor, "Noise_3003");

        m_bDeadStart = true;
        m_fCalculateTime2 = 0.f;
    }
    if (m_bDeadStart)
    {
        m_fCalculateTime2 += _fTimeDelta;
        if (m_fCalculateTime2 > 3.f)
        {
            m_fCalculateTime2 = 0.f;
            m_bDeadStart = false;

            m_IsEnabled = false;
        }
    }
}

_bool COrc_DarkCommander::Update2ComboState(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_ATTACK2_B1)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK2_B2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);

            SetWeaponMeshTrail(true, _float4(0.25f, 0.459f, 0.f, 0.8f), true, 1.7f);
            m_pWeaponTrail->StartTrail(_float4(0.25f, 0.459f, 0.f, 0.8f), _float4(0.761f, 0.878f, 0.129f, 1.f), true, true, true, 0.33f);
        }
    }
    if (m_iActionState == OC_DC_A_ATTACK2_B2)
    {
        UpdateMoveSlide(m_fKeyFrameSpeed * 4.f + 0.1f, m_fKeyFrameSpeed * 11.f + 0.1f, m_f2Combo1SlideSpeed, _fTimeDelta);
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK2_B3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
        }
    }
    if (m_iActionState == OC_DC_A_ATTACK2_B3)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK2_B4;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);

            SetWeaponMeshTrail(true, _float4(0.25f, 0.459f, 0.f, 0.8f), true, 1.7f);
            m_pWeaponTrail->StartTrail(_float4(0.25f, 0.459f, 0.f, 0.8f), _float4(0.761f, 0.878f, 0.129f, 1.f), true, true, true, 0.33f);
        }
    }
    if (m_iActionState == OC_DC_A_ATTACK2_B4)
    {
        UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 5.f + 0.1f, m_f2Combo2SlideSpeed, _fTimeDelta);
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            return true;
        }
    }

    return false;
}

_bool COrc_DarkCommander::UpdateSAttackState(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_ATTACK2_C1)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            SetWeaponRimLight(true);
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK2_C2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);

            SetWeaponMeshTrail(true, _float4(0.761f, 0.231f, 0.2f, 0.8f), true, 1.7f);
            m_pWeaponTrail->StartTrail(_float4(0.761f, 0.231f, 0.2f, 0.8f), _float4(1.0f, 0.878f, 0.78f, 1.f), true, true, true, 0.33f);
        }
    }
    if (m_iActionState == OC_DC_A_ATTACK2_C2)
    {
        // 패링
        if (m_bParried)
        {
            SetWeaponRimLight(false);
            m_iParriedPower = CheckParryingStack(m_iSAttParryStack);
            if (m_iParriedPower == PARRIEDSTRONG)
            {
                GAMEINSTANCE->PlaySoundW("DC_Parried", m_fVoiceSize);
                m_iBaseState = OC_DC_B_BLOCKED;
                m_bParried = false;
                ResetTime();
                m_fAnimSpeed = 1.f;
                m_iActionState = OC_DC_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                return true;
            }
            m_bParried = false;
            ResetTime();
            m_fAnimSpeed = 1.f;
            m_iActionState = OC_DC_A_BLOCKED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
        }
        
        UpdateMoveSlide(m_fKeyFrameSpeed * 7.f + 0.1f, m_fKeyFrameSpeed * 12.f + 0.1f, m_fSAttackSlideSpeed, _fTimeDelta);
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            SetWeaponRimLight(false);
            ResetTime();
            return true;
        }
    }
    else if (m_iActionState == OC_DC_A_BLOCKED)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            ResetTime();
            m_iActionState = OC_DC_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            return true;
        }
    }

    return false;
}

_bool COrc_DarkCommander::UpdateRAttackState(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_ATTACK1_A)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK1_A2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f); 
            
            //SetWeaponGlow(true, true, _float4(0.25f, 0.459f, 0.f, 0.8f), true, 1.7f);
            SetWeaponMeshTrail(true, _float4(0.25f, 0.459f, 0.f, 0.8f), true, 1.7f);
            m_pWeaponTrail->StartTrail(_float4(0.25f, 0.459f, 0.f, 0.8f), _float4(0.761f, 0.878f, 0.129f, 1.f), true, true, true, 0.33f);
        }
    }
    if (m_iActionState == OC_DC_A_ATTACK1_A2)
    {
        UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 4.f + 0.1f, m_fRAttackSlideSpeed, _fTimeDelta);
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            return true;
        }
    }
    return false;
}

_bool COrc_DarkCommander::UpdateRMAttackState(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_ATTACK1_C)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK1_C2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);

            SetWeaponMeshTrail(true, _float4(0.25f, 0.459f, 0.f, 0.8f), true, 1.7f);
            m_pWeaponTrail->StartTrail(_float4(0.25f, 0.459f, 0.f, 0.8f), _float4(0.761f, 0.878f, 0.129f, 1.f), true, true, true, 0.33f);
        }
    }
    if (m_iActionState == OC_DC_A_ATTACK1_C2)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            return true;
        }
    }
    return false;
}

_bool COrc_DarkCommander::UpdateLAttackState(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_ATTACK1_B)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK1_B2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);

            SetWeaponMeshTrail(true, _float4(0.25f, 0.459f, 0.f, 0.8f), true, 1.7f);
            m_pWeaponTrail->StartTrail(_float4(0.25f, 0.459f, 0.f, 0.8f), _float4(0.761f, 0.878f, 0.129f, 1.f), true, true, true, 0.33f);
        }
    }
    if (m_iActionState == OC_DC_A_ATTACK1_B2)
    {
        UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 6.f + 0.1f, m_fLAttackSlideSpeed, _fTimeDelta);
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            return true;
        }
    }
    return false;
}

_bool COrc_DarkCommander::Update3ComboState(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_ATTACK2_A1)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK2_A2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);

            SetWeaponMeshTrail(true, _float4(0.25f, 0.459f, 0.f, 0.8f), true, 1.7f);
            m_pWeaponTrail->StartTrail(_float4(0.25f, 0.459f, 0.f, 0.8f), _float4(0.761f, 0.878f, 0.129f, 1.f), true, true, true, 0.33f);
        }
    }
    if (m_iActionState == OC_DC_A_ATTACK2_A2)
    {
        UpdateMoveSlide(m_fKeyFrameSpeed * 4.f + 0.1f, m_fKeyFrameSpeed * 11.f + 0.1f, m_f3Combo1SlideSpeed, _fTimeDelta);
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK2_A3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
        }
    }
    if (m_iActionState == OC_DC_A_ATTACK2_A3)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK2_A4;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);

            SetWeaponMeshTrail(true, _float4(0.25f, 0.459f, 0.f, 0.8f), true, 1.7f);
            m_pWeaponTrail->StartTrail(_float4(0.25f, 0.459f, 0.f, 0.8f), _float4(0.761f, 0.878f, 0.129f, 1.f), true, true, true, 0.33f);

        }
    }
    if (m_iActionState == OC_DC_A_ATTACK2_A4)
    {
        UpdateMoveSlide(m_fKeyFrameSpeed * 3.f + 0.1f, m_fKeyFrameSpeed * 7.f + 0.1f, m_f3Combo2SlideSpeed, _fTimeDelta);
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK2_A5;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);

        }
    }
    if (m_iActionState == OC_DC_A_ATTACK2_A5)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            SetWeaponRimLight(true);
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK2_A6;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);

            SetWeaponMeshTrail(true, _float4(0.761f, 0.231f, 0.2f, 0.8f), true, 1.7f);
            m_pWeaponTrail->StartTrail(_float4(0.761f, 0.231f, 0.2f, 0.8f), _float4(1.0f, 0.878f, 0.78f, 1.f), true, true, true, 0.33f);
        }
    }
    if (m_iActionState == OC_DC_A_ATTACK2_A6)
    {
        // 패링
        if (m_bParried)
        {
            SetWeaponRimLight(false);
            m_iParriedPower = CheckParryingStack(m_i3ComboAttParryStack);
            if (m_iParriedPower == PARRIEDSTRONG)
            {
                GAMEINSTANCE->PlaySoundW("DC_Parried", m_fVoiceSize);
                m_iBaseState = OC_DC_B_BLOCKED;
                m_bParried = false;
                ResetTime();
                m_fAnimSpeed = 1.f;
                m_iActionState = OC_DC_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                return true;
            }
            m_bParried = false;
            ResetTime();
            m_fAnimSpeed = 1.f;
            m_iActionState = OC_DC_A_BLOCKED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
        }
        
        UpdateMoveSlide(m_fKeyFrameSpeed * 3.f + 0.1f, m_fKeyFrameSpeed * 6.f + 0.1f, m_f3Combo3SlideSpeed, _fTimeDelta);
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            SetWeaponRimLight(false);
            ResetTime();
            return true;
        }
    }
    else if (m_iActionState == OC_DC_A_BLOCKED)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            ResetTime();
            m_iActionState = OC_DC_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            return true;
        }
    }

    return false;
}

_bool COrc_DarkCommander::UpdateSpecialState(_float _fTimeDelta)
{
    if (m_iActionState == OC_DC_A_ATTACK3_A1)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK3_A2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
        }
    }
    if (m_iActionState == OC_DC_A_ATTACK3_A2)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK3_A3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);

            SetWeaponMeshTrail(true, _float4(0.25f, 0.459f, 0.f, 0.8f), true, 1.7f);
            m_pWeaponTrail->StartTrail(_float4(0.25f, 0.459f, 0.f, 0.8f), _float4(0.761f, 0.878f, 0.129f, 1.f), true, true, true, 0.33f);
        }
    }
    // 휘두르며 전진 후 순간이동
    if (m_iActionState == OC_DC_A_ATTACK3_A3)
    {
        UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 8.f + 0.1f, m_fSCombo1SlideSpeed, _fTimeDelta);
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK3_A4;
            m_pModelCom->ChangeAnim(m_iActionState, 0.f);
            MovePos(m_vNormLookDirXZ * m_fSkill2MovePos + m_vCurrentPos);
        }
    }
    // 움직이며 도약준비
    if (m_iActionState == OC_DC_A_ATTACK3_A4)
    {
        UpdateDirection();
        UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 11.f + 0.1f, m_fSCombo2SlideSpeed, _fTimeDelta);
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK3_A5;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
        }
    }
    // 점프
    if (m_iActionState == OC_DC_A_ATTACK3_A5)
    {
        UpdateDirection();
        if(m_fDistanceToTarget > m_fAttack1Range)
            UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 8.f + 0.1f, m_fSCombo3SlideSpeed, _fTimeDelta);

        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK3_A6;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
        }
    }
    if (m_iActionState == OC_DC_A_ATTACK3_A6)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            SetWeaponRimLight(true);
            ResetTime();
            m_iActionState = OC_DC_A_ATTACK3_A7;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);

            SetWeaponMeshTrail(true, _float4(0.761f, 0.231f, 0.2f, 0.8f), true, 1.7f);
            m_pWeaponTrail->StartTrail(_float4(0.761f, 0.231f, 0.2f, 0.8f), _float4(1.0f, 0.878f, 0.78f, 1.f), true, true, true, 0.33f);
        }
    }
    if (m_iActionState == OC_DC_A_ATTACK3_A7)
    {
        // 패링
        if (m_bParried)
        {
            SetWeaponRimLight(false);
            m_iParriedPower = CheckParryingStack(m_iSpecialAttParryStack);
            if (m_iParriedPower == PARRIEDSTRONG)
            {
                GAMEINSTANCE->PlaySoundW("DC_Parried", m_fVoiceSize);
                m_iBaseState = OC_DC_B_BLOCKED;
                m_bParried = false;
                ResetTime();
                m_fAnimSpeed = 1.f;
                m_iActionState = OC_DC_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                return true;
            }
            m_bParried = false;
            ResetTime();
            m_fAnimSpeed = 1.f;
            m_iActionState = OC_DC_A_BLOCKED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
        }

        UpdateMoveSlide(m_fKeyFrameSpeed * 6.f + 0.1f, m_fKeyFrameSpeed * 9.f + 0.1f, m_fSCombo4SlideSpeed, _fTimeDelta);
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && CheckAnimFinish())
        {
            SetWeaponRimLight(false);
            ResetTime();
            return true;
        }
    }
    else if (m_iActionState == OC_DC_A_BLOCKED)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            ResetTime();
            m_iActionState = OC_DC_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            return true;
        }
    }

    return false;
}

void COrc_DarkCommander::ResetTime()
{
    m_fCalculateTime = 0.f;
    m_fCalculateTime2 = 0.f;
}

void COrc_DarkCommander::CheckDead()
{
    if (m_fHp <= 0.f)
    {
        GAMEINSTANCE->AddObject(LEVEL_FONOS, TEXT("Layer_Environment"), CEffectAttachObj::Create(_float3(183.6f, 10.07f, 92.54f), "Portal2"));

        EFFECTMGR->StopEffect("Mon_O_Floor", 0);
        GAMEINSTANCE->PlaySoundW("DC_Die", m_fVoiceSize);
        //GAMEINSTANCE->SwitchingBGM();
        SetWeaponRimLight(false);
        m_bDead = true;
        OffCollisionWeapon();
        m_iBaseState = OC_DC_B_DEATH;
        m_iActionState = OC_DC_A_DEATH;
        m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
        m_pRigidBody->SimulationOff();

        m_pMemory->SetLookDir(m_pTransformCom->GetState(CTransform::STATE_LOOK));

        static_pointer_cast<CPlayer>(m_pTarget)->LockOnChangeToWeapon();

        QUESTMGR->CheckDeadMonster(shared_from_this());
    }
}

void COrc_DarkCommander::CheckAttacked()
{
    // 피격시
    if (m_bHitted)
    {
        ResetTime();
        m_iBaseState = 	OC_DC_B_HITTED;
        m_iActionState = OC_DC_A_FRONTBE;
    }

    // 짧은경직
    if (m_bHitted2)
    {
        ResetTime();
        m_iBaseState = OC_DC_B_HITTED2;
        m_iActionState = OC_DC_A_FRONTBE;
    }
    
    // 긴경직
    if (m_bHitted3)
    {
        ResetTime();
        m_iBaseState = OC_DC_B_HITTED3;
        m_iActionState = OC_DC_A_FRONTBE;
    }

    // 스턴
    if (m_bStun)
    {
        ResetTime();
        m_iBaseState = OC_DC_B_STUN;
        m_iActionState = OC_DC_A_STUN;
    }

    // 감전
    if (m_bShocked)
    {
        ResetTime();
        m_iBaseState = OC_DC_B_SHOCKED;
        m_iActionState = OC_DC_A_SHOCKED;
    }

    // 넘어지기
    if (m_bDown)
    {
        ResetTime();
        m_iBaseState = OC_DC_B_DOWN;
        m_iActionState = OC_DC_A_DOWN1;
    }
}

_bool COrc_DarkCommander::CheckAnimFinish()
{
    if (m_fCalculateTime > 0.1f)
        return true;
    else
        return false;
}

HRESULT COrc_DarkCommander::AddWeaponTrail()
{
    // ==== PointTrail ====
    m_pWeaponTrail = CPointTrail::Create("Trail");

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, TEXT("Layer_Effect"), m_pWeaponTrail)))
        return E_FAIL;

    m_pWeaponTrail->ChangeWeapon(m_pWeapon, _float3(0.f, 2.3f, 0.f), _float3(0.f, 0.f, 0.f));

    // ==== MeshTrail ====
    m_pWeapon->AddTrail(7, 1);
}

void COrc_DarkCommander::SetWeaponGlow(_bool _bGlow, _bool _bUseGlowColor, _float4 _vGlowColor, _bool _bUseGlowTime, _float _fGlowTime)
{
    m_pWeapon->SetUseGlow(_bGlow, _bUseGlowTime, _fGlowTime);
    m_pWeapon->SetUseGlowColor(_bUseGlowColor, _vGlowColor);
}

void COrc_DarkCommander::SetWeaponMeshTrail(_bool _bUseMeshTrail, _float4 _vMeshTrailColor, _bool _bUseMeshTrailTime, _float _fMeshTrailTime)
{
    m_pWeapon->SetUseTrail(_bUseMeshTrail, _vMeshTrailColor, _bUseMeshTrailTime, _fMeshTrailTime);
}

HRESULT COrc_DarkCommander::AddMotionTrail(_int _iTrailCnt)
{
    if (0 >= _iTrailCnt)
        return E_FAIL;

    CMotionTrail::MOTIONTRAIL_DESC MotionTrail_Desc = {};
    MotionTrail_Desc.pModel = m_pModelCom;
    MotionTrail_Desc.iSize = _iTrailCnt;
    MotionTrail_Desc.fLifeTime = 0.3f;
    MotionTrail_Desc.vColor = _float3(0.25f, 0.459f, 0.f);
    MotionTrail_Desc.fTrailCntPerTick = 1.f;

    // Player
    m_pMotionTrail = CMotionTrail::Create();

    _int iExceptMeshIdxs[1] = { 0 };
    m_pMotionTrail->SettingMotionTrail(MotionTrail_Desc, shared_from_this(), 0, iExceptMeshIdxs);

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, TEXT("Layer_Effect"), m_pMotionTrail)))
        return E_FAIL;

    m_pMotionTrail->SetEnable(false);

    return S_OK;
}

HRESULT COrc_DarkCommander::SetUseMotionTrail(_bool _bUseMotionTrail, _bool _bUseTimer, _float _fTimer, _bool _bChangeColor, _float4 _vColor)
{
    m_pMotionTrail->ClearMotionTrail();

    if (!_bUseMotionTrail)
    {
        m_pMotionTrail->SetEnable(false);

        return S_OK;
    }

    if (_bUseTimer)
        m_pMotionTrail->SetTimer(true, _fTimer);

    if (_bChangeColor)
        m_pMotionTrail->SetColor(_vColor);

    m_pMotionTrail->SetEnable(true);

    return S_OK;
}

void COrc_DarkCommander::UpdateDir(_float _fTimeDelta)
{
    if (m_fDistanceToTarget < 3.5f)
    {
        switch (m_iActionState)
        {
        case OC_DC_A_RUN:
            m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK1_A:
            m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK1_B:
            m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK1_C:
            m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK2_A1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK2_A3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK2_A5:
            m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK2_B1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK2_B3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK2_C1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK3_A2:
            m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK3_A6:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        //case OC_DC_A_DASH2:
        //    m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
        //    break;
        case OC_DC_A_STDALT:
            m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
            break;
        case OC_DC_A_WALK:
            m_pTransformCom->TurnToDir(m_vTargetDir, 270.f * _fTimeDelta);
            break;
        }
    }
    else
    {
        switch (m_iActionState)
        {
        case OC_DC_A_RUN:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK1_A:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK1_B:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK1_C:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK2_A1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK2_A3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK2_A5:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK2_B1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK2_B3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK2_C1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK3_A2:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case OC_DC_A_ATTACK3_A6:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        /*case OC_DC_A_DASH2:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;*/
        case OC_DC_A_STDALT:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case OC_DC_A_WALK:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        }
    }
}
