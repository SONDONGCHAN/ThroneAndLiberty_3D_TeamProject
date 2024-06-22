#include "Midget_Berserker.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "Animation.h"
#include "EffectMgr.h"
#include "PlayerProjectile.h"
#include "UIInstanceParring.h"
#include "Memory.h"
#include "Player.h"
#include "QuestMgr.h"
#include "EventMgr.h"

CMidget_Berserker::CMidget_Berserker()
    : CMonster()
{
}

CMidget_Berserker::~CMidget_Berserker()
{
}

HRESULT CMidget_Berserker::Initialize()
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

    m_pModelCom = GAMEINSTANCE->GetModel("M_L1Midget_Berserker");
    m_strModelName = "M_L1Midget_Berserker";
    AddComponent(L"Com_Model", m_pModelCom);
    m_pTransformCom->SetScaling(2.f, 2.f, 2.f);
    m_pTransformCom->Rotation(_float3(0.f, 1.f, 0.f), 3.14f);

    m_pModelCom->ChangeAnim(m_iActionState);

    if (FAILED(AddPartObjects("IT_N_Sword_DemonGoat_Butcher"))) 
        return E_FAIL;
    
    // 기억의 파편 넣어주기
    shared_ptr<CMemory> pMemory = CMemory::Create(_float3(144.6f, -71.f, 55.5f), 0);
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_Memory"), pMemory)))
        return E_FAIL;
    m_pMemory = pMemory;

    // 스탯
    m_fHp = 500.f;
    //m_fHp = 100.f;

    m_fMaxHp = 500.f;
    m_fSpeed = 1.5f;

    AddRigidBody("M_L1Midget_Berserker");
    m_pRigidBody->SimulationOff();

    m_vLockOnOffSetPosition = { 0.0f,2.f,0.0f };

    //PrepareMonsterHPBar(TEXT("대왕 고블린 버서커"));
    m_strDisplayName = TEXT("대왕 고블린 버서커");

    // 처음에 다른곳에 배치
    MovePos(_float3(20.f, -205.f, 435.f));

    CEventMgr::GetInstance()->BindBoss1(static_pointer_cast<CMidget_Berserker>(shared_from_this()));

    return S_OK;
}

void CMidget_Berserker::PriorityTick(_float _fTimeDelta)
{
    /*if (GAMEINSTANCE->KeyDown(DIK_N))
    {
        m_fHp -= 50.f;
    }*/
    /*if (GAMEINSTANCE->KeyDown(DIK_7))
    {
        if (!m_bQuickBatch)
        {
            m_bQuickBatch = true;
            m_pRigidBody->SimulationOn();
            MovePos(_float3(147.6f, -70.f, 55.5f));
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

void CMidget_Berserker::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    // Base상태에 따른 FSM
    switch (m_iBaseState)
    {
    case MI_B_B_BEFOREMEET:
        FSM_BeforeMeet(_fTimeDelta);
        break;
    case MI_B_B_GENERATE:
        FSM_Generate(_fTimeDelta);
        break;
    case MI_B_B_NORMAL:
        FSM_Normal(_fTimeDelta);
        break;
    case MI_B_B_BLOCKED:
        FSM_Blocked(_fTimeDelta);
        break;
    case MI_B_B_HITTED:
        FSM_Hitted(_fTimeDelta);
        break;
    case MI_B_B_HITTED2:
        FSM_Hitted2(_fTimeDelta);
        break;
    case MI_B_B_HITTED3:
        FSM_Hitted3(_fTimeDelta);
        break;
    case MI_B_B_STUN:
        FSM_Stun(_fTimeDelta);
        break;
    case MI_B_B_SHOCKED:
        FSM_Shocked(_fTimeDelta);
        break;
    case MI_B_B_DOWN:
        FSM_Down(_fTimeDelta);
        break;
    case MI_B_B_DEATH:
        FSM_Death(_fTimeDelta);
        break;
    }

    // 애니메이션 속도 조절
    for (int i = 0; i < 100; i++)
    {
        switch (i)
        {
        case MI_B_A_BLOCKED:
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
        case MI_B_A_FRONTBE:
            switch (m_iBaseState)
            {
            case MI_B_B_HITTED:
                m_fActionAnimSpeed[i] = 0.8f;
                break;
            case MI_B_B_HITTED2:
                m_fActionAnimSpeed[i] = 0.4f;
                break;
            case MI_B_B_HITTED3:
                m_fActionAnimSpeed[i] = 0.4f;
                break;
            }
            break;
        case MI_B_A_DEATH:
            m_fActionAnimSpeed[i] = 0.5f;
            break;
        default:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        }
    }

    m_pModelCom->PlayAnimation(m_fActionAnimSpeed[m_iActionState] * m_fAnimSpeed * _fTimeDelta);

    m_pTransformCom->Tick(_fTimeDelta);

    //if (m_pParringStack != nullptr)
    //{
    //    m_pParringStack->Tick(_fTimeDelta);
    //}
}

void CMidget_Berserker::LateTick(_float _fTimeDelta)
{
    m_IsEdgeRender = false;

    m_pModelCom->CheckDisableAnim();
    m_pTransformCom->LateTick(_fTimeDelta);
    __super::LateTick(_fTimeDelta);

    if (m_bDslv)
        UpdateDslv(_fTimeDelta);

    if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 5.f))
    {
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
        GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());

    }
#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
}

HRESULT CMidget_Berserker::Render()
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

    _bool bDslvFalse = false;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &bDslvFalse, sizeof(_bool))))
        return E_FAIL;

    return S_OK;
}

_float CMidget_Berserker::GetHPRatio()
{
    _float ratio = m_fHp / m_fMaxHp;

    return ratio;
}

shared_ptr<CMidget_Berserker> CMidget_Berserker::Create(_float3 _vCreatePos)
{
    shared_ptr<CMidget_Berserker> pInstance = make_shared<CMidget_Berserker>();

    pInstance->SetCurrentPos(_vCreatePos); // 위치 지정
    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CMidget_Berserker");

    return pInstance;
}

HRESULT CMidget_Berserker::AddRigidBody(string _strModelKey)
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
    CapsuleDesc.fHeight = 1.8f;
    CapsuleDesc.fRadius = 0.6f;
    _float3 vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
    CapsuleDesc.vOffSetPosition = { 0.0f, 1.5f, 0.0f };
    CapsuleDesc.vOffSetRotation = { 0.0f, 0.0f, 90.0f };
    CapsuleDesc.strShapeTag = "Midget_Berserker_Body";
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

void CMidget_Berserker::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    // 피격시
    if (_ColData.eColFlag & COL_STATIC || _ColData.eColFlag & COL_PLAYER || _ColData.eColFlag & COL_CHECKBOX)
        return;

    // 일반피격
    if ((m_iActionState == MI_B_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG))
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
    if ((m_iActionState == MI_B_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == MI_B_B_HITTED) || (m_iBaseState == MI_B_B_HITTED2))
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
    if ((m_iActionState == MI_B_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == MI_B_B_HITTED) || (m_iBaseState == MI_B_B_HITTED2)
        || (m_iBaseState == MI_B_B_HITTED3))
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
    if ((m_iActionState == MI_B_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == MI_B_B_HITTED) || (m_iBaseState == MI_B_B_HITTED2)
        || (m_iBaseState == MI_B_B_HITTED3) || (m_iBaseState == MI_B_B_STUN) || m_bWhileStun)
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
    if ((m_iActionState == MI_B_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == MI_B_B_HITTED) || (m_iBaseState == MI_B_B_HITTED2)
        || (m_iBaseState == MI_B_B_HITTED3) || (m_iBaseState == MI_B_B_STUN) || (m_iBaseState == MI_B_B_SHOCKED) || m_bWhileStun)
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
    if ((m_iActionState == MI_B_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == MI_B_B_HITTED) || (m_iBaseState == MI_B_B_HITTED2)
        || (m_iBaseState == MI_B_B_HITTED3) || (m_iBaseState == MI_B_B_STUN) || (m_iBaseState == MI_B_B_SHOCKED) || (m_iBaseState == MI_B_B_DOWN) || m_bWhileStun)
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
    if ((_szMyShapeTag == m_strATT5Collider) && (_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        m_bParried = true;
    }
    if ((_szMyShapeTag == m_strATT1Collider) && (_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        m_bParried = true;
    }
    if ((_szMyShapeTag == m_strJumpATT2Collider) && (_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        m_bParried = true;
    }

    // 무적기일때는 제외하고 체력감소
    if (((m_iBaseState == MI_B_B_NORMAL) || (m_iBaseState == MI_B_B_BLOCKED) || (m_iBaseState == MI_B_B_HITTED)
        || (m_iBaseState == MI_B_B_HITTED2) || (m_iBaseState == MI_B_B_HITTED3) || (m_iBaseState == MI_B_B_STUN)
        || (m_iBaseState == MI_B_B_SHOCKED) || (m_iBaseState == MI_B_B_DOWN))
        && !(_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        //ToggleHPBar();

        DamagedByPlayer(_ColData, _szMyShapeTag);

        __super::ShowEffect(_ColData, _szMyShapeTag);
    }
}

void CMidget_Berserker::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CMidget_Berserker::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CMidget_Berserker::EnableCollision(const char* _strShapeTag)
{
    m_pRigidBody->EnableCollision(_strShapeTag);
}

void CMidget_Berserker::DisableCollision(const char* _strShapeTag)
{
    m_pRigidBody->DisableCollision(_strShapeTag);
}

void CMidget_Berserker::ResetBoss()
{
    m_fHp = m_fMaxHp;
    m_fCalculateTime = 0.f;
    m_fCalculateTime2 = 0.f;
    m_iBaseState = MI_B_B_BEFOREMEET;
    m_iDistanceState = MI_B_D_BEFOREMEET;
    m_iActionState = MI_B_A_STDIDLE;
    m_iSkillState = MI_B_S_NOSKILL;
    for (int i = 0; i < 10; i++)
    {
        m_bSkill1State[i] = false;
        m_bSkill2State[i] = false;
        m_bSkill3State[i] = false;
        m_bSkill4State[i] = false;
    }
    m_fAnimSpeed = 1.f;
}

void CMidget_Berserker::ActiveMemory()
{
    if (!m_pMemory->IsEnabled())
    {
        m_pMemory->SetEnable(true);
        m_pMemory->SimulationOn();
        m_pMemory->SetCreatePos(m_vCurrentPos + _float3(0.f, 2.5f, 0.f));
    }
}

HRESULT CMidget_Berserker::AddPartObjects(string _strModelKey)
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


void CMidget_Berserker::UpdateDistanceState()
{
    // 위치 바로 못받아오는거 때문에 우선
    if (m_vTargetPos == _float3(0.f, 0.f, 0.f))
    {
        m_iDistanceState = MI_B_D_BEFOREMEET;
        return;
    }

    if (m_iDistanceState == MI_B_D_BEFOREMEET)
    {
        if (m_fDistanceToTarget < m_fMeetRange)
        {
            m_iDistanceState = MI_B_D_MOVERANGE;
        }
        else
        {
            m_iDistanceState = MI_B_D_BEFOREMEET;
        }
    }
    else
    {
        if (m_fDistanceToTarget < m_fAttack1Range)
        {
            m_iDistanceState = MI_B_D_ATTACKRANGE;
        }
        else if (m_fDistanceToTarget >= m_fAttack1Range && m_fDistanceToTarget < m_fOutRange)
        {
            m_iDistanceState = MI_B_D_MOVERANGE;
        }
        else
        {
            m_iDistanceState = MI_B_D_OUTRANGE;
        }
    }
}

void CMidget_Berserker::UpdateMoveSlide(_float _fSlideStart, _float _fSlideEnd, _float _fSpeed, _float _fTimeDelta)
{
    if ((m_fCalculateTime > (_fSlideStart / m_fAnimSpeed)) && (m_fCalculateTime < (_fSlideEnd / m_fAnimSpeed)))
    {
        MovePos(m_vNormLookDirXZ * _fTimeDelta * _fSpeed + m_vCurrentPos);
    }
}

void CMidget_Berserker::UpdateAttack2MoveSlide(_float _fTimeDelta)
{
    UpdateMoveSlide(0.f + 0.1f, m_fAttack2Move1EndTime + 0.1f, m_fA2SlideSpeed, _fTimeDelta);
    UpdateMoveSlide(m_fAttack2Move2StartTime + 0.1f, m_fAttack2Move2EndTime + 0.1f, m_fA2SlideSpeed, _fTimeDelta);
    UpdateMoveSlide(m_fAttack2Move3StartTime + 0.1f, m_fAttack2Move3EndTime + 0.1f, m_fA2SlideSpeed, _fTimeDelta);
}

_int CMidget_Berserker::CheckParryingStack(_int _iParryingStack)
{
    switch (_iParryingStack)
    {
    case 1:
        m_iParryType = 1;
        break;
    case 2:
        m_iParryType = 0;
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

void CMidget_Berserker::FSM_BeforeMeet(_float _fTimeDelta)
{
    //UpdateDistanceState();

    if (m_bCameraStart)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_fCalculateTime > 10.7f)
        {
            m_bBattleStart = true;
            m_fCalculateTime = 0.f;
        }
    }

    if (m_bBattleStart)
    {
        UpdateDirection();
        OnCollisionWeapon();
        m_iBaseState = MI_B_B_GENERATE;
        m_iActionState = MI_B_A_COMBATSTART;

        GAMEINSTANCE->PlaySoundW("MB_BattleStart", m_fVoiceSize);
        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
        //GAMEINSTANCE->SwitchingBGM2();
    }
    // 멀때
    else
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            switch (GAMEINSTANCE->PickRandomInt(0, 6))
            {
            case 0:
                m_iActionState = MI_B_A_STDALT1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                break;
            case 1:
                m_iActionState = MI_B_A_STDALT2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                break;
            case 2:
                m_iActionState = MI_B_A_DANCE1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                break;
            case 3:
                m_iActionState = MI_B_A_DANCE2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                break;
            case 4:
                m_iActionState = MI_B_A_STDIDLE;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                break;
            case 5:
                m_iActionState = MI_B_A_STDIDLE;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                break;
            }
        }
    }
}

void CMidget_Berserker::FSM_Generate(_float _fTimeDelta)
{
    if (m_iActionState == MI_B_A_COMBATSTART)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            OffCollisionWeapon();
            m_iBaseState = MI_B_B_NORMAL;
            m_iActionState = MI_B_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
    }
}

void CMidget_Berserker::FSM_Normal(_float _fTimeDelta)
{
    UpdateDir(_fTimeDelta);
    
    // 패턴 잡기
    if (m_iSkillState == MI_B_S_NOSKILL)
    {
        switch (m_iSkillOrder[m_iPatternNum])
        {
        case MI_B_S_SKILL1:
            m_iSkillState = MI_B_S_SKILL1;
            m_bSkill1State[0] = true;
            break;
        case MI_B_S_SKILL2:
            m_iSkillState = MI_B_S_SKILL2;
            m_bSkill2State[0] = true;
            break;
        case MI_B_S_SKILL3:
            m_iSkillState = MI_B_S_SKILL3;
            m_bSkill3State[0] = true;
            break;
        case MI_B_S_SKILL4:
            m_iSkillState = MI_B_S_SKILL4;
            m_bSkill4State[0] = true;
            break;
        }
        m_iPatternNum++;
    }
    
    switch (m_iSkillState)
    {
    case MI_B_S_SKILL1:
        // 서있다가 점프 내려찍기1
        if (m_bSkill1State[0])
        {
            if (m_iActionState == MI_B_A_STDALT)
            {
                m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fSkill1StandTime)
                {
                    m_fCalculateTime = 0.f;

                    m_iActionState = MI_B_A_JUMPATTACK1_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    GAMEINSTANCE->PlaySoundW("MB_JumpAtt1", m_fVoiceSize);
                }
            }
            if (m_iActionState == MI_B_A_JUMPATTACK1_A)
            {
                if (m_pModelCom->GetIsFinishedAnimation())
                {
                    m_iActionState = MI_B_A_JUMPATTACK1_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_vMovePos = m_vTargetPos;
                    UpdateJumpDirSpeed1();
                }
            }
            if (m_iActionState == MI_B_A_JUMPATTACK1_B)
            {
                m_fCalculateTime += _fTimeDelta;

                MovePosXZ(m_vJumpNormalMoveDir * _fTimeDelta * m_fJumpMoveSpeed + m_vCurrentPos);
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.3f)
                {
                    //GAMEINSTANCE->PlaySoundW("MB_SFX_AttackGound", m_fEffectSize);
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_JUMPATTACK1_C;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill1State[0] = false; 
                    m_bSkill1State[1] = true;
                }
            }
        }
        // 점프 내려찍기1 이후 내려찍기1
        if (m_bSkill1State[1])
        {
            if (m_iActionState == MI_B_A_JUMPATTACK1_C)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.3f)
                {
                    m_fCalculateTime = 0.f;
                    // 거리가 가까우면
                    if (m_fDistanceToTarget < m_fAttack1Range)
                    {
                        m_iActionState = MI_B_A_ATTACK5_A;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    }
                    // 거리가 멀면
                    else
                    {
                        m_iActionState = MI_B_A_RUN;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_fSpeed = m_fSkill1RunSpeed;
                    }
                }
            }
            if (m_iActionState == MI_B_A_RUN)
            {
                m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);
                // 거리가 가까우면
                if (m_fDistanceToTarget < m_fAttack1Range)
                {
                    m_fSpeed = 1.f;
                    m_iActionState = MI_B_A_ATTACK5_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == MI_B_A_ATTACK5_A)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    //OnCollisionWeapon();
                    m_iActionState = MI_B_A_ATTACK5_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    SetWeaponRimLight(true);
                    GAMEINSTANCE->PlaySoundW("MB_SpecialAtt1", m_fVoiceSize);
                }
            }
            if (m_iActionState == MI_B_A_ATTACK5_B)
            {
                // 패링
                if (m_bParried)
                {
                    SetWeaponRimLight(false);
                    m_iParriedPower = CheckParryingStack(m_iAtt5ParryStack);
                    if (m_iParriedPower == PARRIEDSTRONG)
                    {
                        GAMEINSTANCE->PlaySoundW("MB_Parried", m_fVoiceSize);
                        m_iBaseState = MI_B_B_BLOCKED;
                        m_bSkill1State[1] = false;
                    }
                    m_bParried = false;
                    m_fCalculateTime = 0.f;
                    m_fAnimSpeed = 1.f;
                    m_iActionState = MI_B_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                }
                m_fCalculateTime += _fTimeDelta;
                UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 21.f + 0.1f, m_fA5SlideSpeed, _fTimeDelta);
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    SetWeaponRimLight(false);
                    m_fCalculateTime = 0.f;
                    //OffCollisionWeapon();
                    m_iActionState = MI_B_A_ATTACK5_C;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                    m_bSkill1State[1] = false;
                    m_bSkill1State[2] = true;
                }
            }
            if (m_iActionState == MI_B_A_BLOCKED)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    //OffCollisionWeapon();
                    m_iActionState = MI_B_A_ATTACK3_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                    m_bSkill1State[1] = false;
                    m_bSkill1State[2] = true;
                }
            }
        }
        // 내려찍기 이후 회전공격
        if (m_bSkill1State[2])
        {
            if (m_iActionState == MI_B_A_ATTACK5_C)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_ATTACK3_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    GAMEINSTANCE->PlaySoundW("MB_SpinStart", m_fVoiceSize);
                }
            }
            if (m_iActionState == MI_B_A_ATTACK3_A)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    OnCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_ATTACK3_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_fSpeed = m_fSkill1SpinSpeed;
                }
            }
            if (m_iActionState == MI_B_A_ATTACK3_B)
            {
                m_fCalculateTime += _fTimeDelta;

                if ((m_fCalculateTime > m_fSpin1SoundStartTime + 0.1f) && !m_bWhirlWindSoundStart)
                {
                    GAMEINSTANCE->PlaySoundL("MB_SFX_WhirlWind", m_fEffectSize);
                    m_bWhirlWindSoundStart = true;
                }
                if ((m_fCalculateTime > m_fSpin1SoundEndTime + 0.1f) && !m_bWhirlWindSoundEnd)
                {
                    GAMEINSTANCE->StopSound("MB_SFX_WhirlWind");
                    m_bWhirlWindSoundEnd = true;
                }


                if (m_fCalculateTime < m_fSpin1EndTime)
                {
                    m_pTransformCom->GoStraight(m_fSpeed* _fTimeDelta);
                }
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_bWhirlWindSoundStart = false;
                    m_bWhirlWindSoundEnd = false;
                    OffCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_fSpeed = 1.f;
                    m_iActionState = MI_B_A_STUNL;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                    m_bWhileStun = true;
                }
            }
            if (m_iActionState == MI_B_A_STUNL)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_STUNE;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == MI_B_A_STUNE)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_bWhileStun = false;
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_bSkill1State[2] = false;
                    m_iSkillState = MI_B_S_NOSKILL;
                }
            }
        }
        break;

    case MI_B_S_SKILL2:
        // 서있다가 점프 내려찍기2
        if (m_bSkill2State[0])
        {
            if (m_iActionState == MI_B_A_STDALT)
            {
                m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fSkill2StandTime)
                {
                    m_fCalculateTime = 0.f;

                    m_iActionState = MI_B_A_JUMPATTACK2_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == MI_B_A_JUMPATTACK2_A)
            {
                if (m_pModelCom->GetIsFinishedAnimation())
                {
                    m_iActionState = MI_B_A_JUMPATTACK2_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_vMovePos = m_vTargetPos;
                    UpdateJumpDirSpeed2();
                    GAMEINSTANCE->PlaySoundW("MB_JumpAtt2", m_fVoiceSize);
                }
            }
            if (m_iActionState == MI_B_A_JUMPATTACK2_B)
            {
                m_fCalculateTime += _fTimeDelta;
                MovePosXZ(m_vJumpNormalMoveDir * _fTimeDelta * m_fJumpMoveSpeed + m_vCurrentPos);
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.3f)
                {
                    GAMEINSTANCE->PlaySoundW("MB_SFX_AttackGound", m_fEffectSize);
                    //OnCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_JUMPATTACK2_C;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill2State[0] = false;
                    m_bSkill2State[1] = true;
                    SetWeaponRimLight(true);
                }
            }
        }
        // 점프내려찍기2 이후 내려찍기1
        if (m_bSkill2State[1])
        {
            if (m_iActionState == MI_B_A_JUMPATTACK2_C)
            {
                // 패링
                if (m_bParried)
                {
                    SetWeaponRimLight(false);
                    m_iParriedPower = CheckParryingStack(m_iJumpAtt2ParryStack);
                    if (m_iParriedPower == PARRIEDSTRONG)
                    {
                        GAMEINSTANCE->PlaySoundW("MB_Parried", m_fVoiceSize);
                        m_iBaseState = MI_B_B_BLOCKED;
                        m_bSkill1State[1] = false;
                    }
                    m_bParried = false;
                    m_fCalculateTime = 0.f;
                    m_fAnimSpeed = 1.f;
                    m_iActionState = MI_B_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                }
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.3f)
                {
                    SetWeaponRimLight(false);
                    //OffCollisionWeapon();
                    UpdateDirection();
                    m_fCalculateTime = 0.f;
                    // 거리가 가까우면
                    if (m_fDistanceToTarget < m_fAttack1Range)
                    {
                        m_iActionState = MI_B_A_ATTACK5_A;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    }
                    // 거리가 멀면
                    else
                    {
                        m_iActionState = MI_B_A_RUN;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_fSpeed = m_fSkill2RunSpeed;
                    }
                }
            }
            if (m_iActionState == MI_B_A_BLOCKED && (m_iParryType == 2))
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    // 거리가 가까우면
                    if (m_fDistanceToTarget < m_fAttack1Range)
                    {
                        m_iActionState = MI_B_A_ATTACK5_A;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    }
                    // 거리가 멀면
                    else
                    {
                        m_iActionState = MI_B_A_RUN;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_fSpeed = m_fSkill2RunSpeed;
                    }
                }
            }
            if (m_iActionState == MI_B_A_RUN)
            {
                m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);
                // 거리가 가까우면
                if (m_fDistanceToTarget < m_fAttack1Range)
                {
                    m_fSpeed = 1.f;
                    m_iActionState = MI_B_A_ATTACK5_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == MI_B_A_ATTACK5_A)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    //OnCollisionWeapon();
                    m_iActionState = MI_B_A_ATTACK5_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    SetWeaponRimLight(true);
                    GAMEINSTANCE->PlaySoundW("MB_SpecialAtt1", m_fVoiceSize);
                }
            }
            if (m_iActionState == MI_B_A_ATTACK5_B)
            {
                // 패링
                if (m_bParried)
                {
                    SetWeaponRimLight(false);
                    m_iParriedPower = CheckParryingStack(m_iAtt5ParryStack);
                    if (m_iParriedPower == PARRIEDSTRONG)
                    {
                        GAMEINSTANCE->PlaySoundW("MB_Parried", m_fVoiceSize);
                        m_iBaseState = MI_B_B_BLOCKED;
                        m_bSkill2State[1] = false;
                    }
                    m_bParried = false;
                    m_fCalculateTime = 0.f;
                    m_fAnimSpeed = 1.f;
                    m_iActionState = MI_B_A_BLOCKED;
                    m_bBug = true;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                }
                m_fCalculateTime += _fTimeDelta;
                UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 21.f + 0.1f, m_fA5SlideSpeed, _fTimeDelta);
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    SetWeaponRimLight(false);
                    m_fCalculateTime = 0.f;
                    //OffCollisionWeapon();
                    m_iActionState = MI_B_A_ATTACK5_C;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                    m_bSkill2State[1] = false;
                    m_bSkill2State[2] = true;
                }
                if (m_iActionState == MI_B_A_BLOCKED && m_bBug)
                {
                    m_fCalculateTime2 += _fTimeDelta;
                    if (m_pModelCom->GetIsFinishedAnimation() && (m_fCalculateTime2 > 0.2f))
                    {
                        m_bBug = false;
                        m_fCalculateTime2 = 0.f;
                        // 거리가 가까우면
                        if (m_fDistanceToTarget < m_fMeleeRange)
                        {
                            m_iActionState = MI_B_A_ATTACK1_A;
                            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        }
                        // 거리가 멀면
                        else
                        {
                            m_iActionState = MI_B_A_RUN;
                            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                            m_fSpeed = m_fSkill2RunSpeed;
                        }
                        m_bSkill2State[1] = false;
                        m_bSkill2State[2] = true;
                    }
                }
            }
        }
        // 내려찍기 이후 멀면 뛰다가 일반공격
        if (m_bSkill2State[2])
        {
            if (m_iActionState == MI_B_A_ATTACK5_C)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    // 거리가 가까우면
                    if (m_fDistanceToTarget < m_fMeleeRange)
                    {
                        m_iActionState = MI_B_A_ATTACK1_A;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    }
                    // 거리가 멀면
                    else
                    {
                        m_iActionState = MI_B_A_RUN;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_fSpeed = m_fSkill2RunSpeed;
                    }
                }
            }
            if (m_iActionState == MI_B_A_RUN)
            {
                m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);
                // 거리가 가까우면
                if (m_fDistanceToTarget < m_fMeleeRange)
                {
                    m_fSpeed = 1.f;
                    m_iActionState = MI_B_A_ATTACK1_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == MI_B_A_ATTACK1_A)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    //OnCollisionWeapon();
                    m_iActionState = MI_B_A_ATTACK1_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    GAMEINSTANCE->PlaySoundW("MB_NomalAtt", m_fVoiceSize);

                    SetWeaponRimLight(true);
                }
            }
            if (m_iActionState == MI_B_A_ATTACK1_B)
            {
                m_fCalculateTime += _fTimeDelta;
                // 패링
                if (m_bParried)
                {
                    SetWeaponRimLight(false);
                    m_iParriedPower = CheckParryingStack(m_iAtt1ParryStack);
                    if (m_iParriedPower == PARRIEDSTRONG)
                    {
                        GAMEINSTANCE->PlaySoundW("MB_Parried", m_fVoiceSize);
                        m_iBaseState = MI_B_B_BLOCKED;
                        m_bSkill2State[2] = false;
                    }
                    m_bParried = false;
                    m_fCalculateTime = 0.f;
                    m_fAnimSpeed = 1.f;
                    m_iActionState = MI_B_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                }
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    SetWeaponRimLight(false);
                    //OffCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_ATTACK1_C;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == MI_B_A_BLOCKED)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_bSkill2State[2] = false;
                    m_iSkillState = MI_B_S_NOSKILL;
                }
            }
            if (m_iActionState == MI_B_A_ATTACK1_C)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_bSkill2State[2] = false;
                    m_iSkillState = MI_B_S_NOSKILL;
                }
            }
        }
        break;

    case MI_B_S_SKILL3:
        // 서있다가 회전공격2
        if (m_bSkill3State[0])
        {
            if (m_iActionState == MI_B_A_STDALT)
            {
                m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fSkill3StandTime)
                {
                    m_fCalculateTime = 0.f;

                    m_iActionState = MI_B_A_ATTACK4_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    GAMEINSTANCE->PlaySoundW("MB_SpinStart", m_fVoiceSize);
                }
            }
            if (m_iActionState == MI_B_A_ATTACK4_A)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    OnCollisionWeapon();
                    m_iActionState = MI_B_A_ATTACK4_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_fSpeed = m_fSkill3SpinSpeed;
                }
            }
            if (m_iActionState == MI_B_A_ATTACK4_B)
            {
                m_fCalculateTime += _fTimeDelta;

                if ((m_fCalculateTime > m_fSpin2SoundStartTime + 0.1f) && !m_bWhirlWindSoundStart)
                {
                    GAMEINSTANCE->PlaySoundL("MB_SFX_WhirlWind", m_fEffectSize);
                    m_bWhirlWindSoundStart = true;
                }
                if ((m_fCalculateTime > m_fSpin2SoundEndTime + 0.1f) && !m_bWhirlWindSoundEnd)
                {
                    GAMEINSTANCE->StopSound("MB_SFX_WhirlWind");
                    m_bWhirlWindSoundEnd = true;
                }

                m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_bWhirlWindSoundStart = false;
                    m_bWhirlWindSoundEnd = false;
                    OffCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_fSpeed = 1.f;

                    m_bWhileStun = true;
                    m_iActionState = MI_B_A_STUNL;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                }
            }
            if (m_iActionState == MI_B_A_STUNL)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_STUNE;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == MI_B_A_STUNE)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_bWhileStun = false;
                    m_fCalculateTime = 0.f;
                    // 거리가 가까우면
                    if (m_fDistanceToTarget < m_fAttack1Range)
                    {
                        GAMEINSTANCE->PlaySoundW("MB_SpecialAtt2Ready", m_fVoiceSize);
                        m_iActionState = MI_B_A_ATTACK2_A;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        m_bSkill3State[0] = false;
                        m_bSkill3State[1] = true;
                    }
                    // 거리가 멀면
                    else
                    {
                        m_iActionState = MI_B_A_WALK;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_fSpeed = m_fSkill3WalkSpeed;
                    }
                }
            }
            if (m_iActionState == MI_B_A_WALK)
            {
                m_fCalculateTime += _fTimeDelta;
                m_pTransformCom->GoStraight(m_fSpeed* _fTimeDelta);
                // 거리가 가까우면
                if (m_fDistanceToTarget < m_fAttack1Range)
                {
                    m_fSpeed = 1.f;
                    GAMEINSTANCE->PlaySoundW("MB_SpecialAtt2Ready", m_fVoiceSize);
                    m_iActionState = MI_B_A_ATTACK2_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill3State[0] = false;
                    m_bSkill3State[1] = true;
                }
                // 걷다가 계속 멀면
                if (m_fCalculateTime > m_fSkill3WalkTime)
                {
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_RUN;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_fSpeed = m_fSkill3RunSpeed;
                }
            }
            if (m_iActionState == MI_B_A_RUN)
            {
                m_pTransformCom->GoStraight(m_fSpeed* _fTimeDelta);
                // 거리가 가까우면
                if (m_fDistanceToTarget < m_fAttack1Range)
                {
                    m_fSpeed = 1.f;
                    GAMEINSTANCE->PlaySoundW("MB_SpecialAtt2Ready", m_fVoiceSize);
                    m_iActionState = MI_B_A_ATTACK2_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill3State[0] = false;
                    m_bSkill3State[1] = true;
                }
            }
        }
        // 내려찍기2
        if (m_bSkill3State[1])
        {
            if (m_iActionState == MI_B_A_ATTACK2_A)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    //OnCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_ATTACK2_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    GAMEINSTANCE->PlaySoundW("MB_SpecialAtt2", m_fVoiceSize);
                }
            }
            if (m_iActionState == MI_B_A_ATTACK2_B)
            {
                m_fCalculateTime += _fTimeDelta;
                UpdateAttack2MoveSlide(_fTimeDelta);

                if (m_fCalculateTime < m_fAttack2Move3StartTime)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 120.f * _fTimeDelta);
                }
                
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    //OffCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_JUMPATTACK2_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill3State[1] = false;
                    m_bSkill3State[2] = true;
                }
            }
        }
        // 점프내려찍기2
        if (m_bSkill3State[2])
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_iActionState == MI_B_A_JUMPATTACK2_A)
            {
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_JUMPATTACK2_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_vMovePos = m_vTargetPos;
                    UpdateJumpDirSpeed2();
                    GAMEINSTANCE->PlaySoundW("MB_JumpAtt2", m_fVoiceSize);
                }
            }
            if (m_iActionState == MI_B_A_JUMPATTACK2_B)
            {
                m_fCalculateTime += _fTimeDelta;
                MovePosXZ(m_vJumpNormalMoveDir * _fTimeDelta * m_fJumpMoveSpeed + m_vCurrentPos);
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.3f)
                {
                    GAMEINSTANCE->PlaySoundW("MB_SFX_AttackGound", m_fEffectSize);
                    //OnCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_JUMPATTACK2_C;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill3State[2] = false;
                    m_bSkill3State[3] = true;
                    SetWeaponRimLight(true);
                }
            }
        }
        // 점프내려찍기 이후 가까우면 일반공격, 아니면 뛰기 하다가 일반공격
        if (m_bSkill3State[3])
        {
            if (m_iActionState == MI_B_A_JUMPATTACK2_C)
            {
                // 패링
                if (m_bParried)
                {
                    SetWeaponRimLight(false);
                    m_iParriedPower = CheckParryingStack(m_iJumpAtt2ParryStack);
                    if (m_iParriedPower == PARRIEDSTRONG)
                    {
                        GAMEINSTANCE->PlaySoundW("MB_Parried", m_fVoiceSize);
                        m_iBaseState = MI_B_B_BLOCKED;
                        m_bSkill3State[3] = false;
                    }
                    m_bParried = false;
                    m_fCalculateTime = 0.f;
                    m_fAnimSpeed = 1.f;
                    m_iActionState = MI_B_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                }
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.3f)
                {
                    SetWeaponRimLight(false);
                    //OffCollisionWeapon();
                    UpdateDirection();
                    m_fCalculateTime = 0.f;
                    // 거리가 가까우면
                    if (m_fDistanceToTarget < m_fMeleeRange)
                    {
                        m_iActionState = MI_B_A_ATTACK1_A;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    }
                    // 거리가 멀면
                    else
                    {
                        m_iActionState = MI_B_A_RUN;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_fSpeed = m_fSkill3RunSpeed;
                    }
                }
            }
            if (m_iActionState == MI_B_A_BLOCKED && (m_iParryType == 2))
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    UpdateDirection();
                    m_fCalculateTime = 0.f;
                    // 거리가 가까우면
                    if (m_fDistanceToTarget < m_fMeleeRange)
                    {
                        m_iActionState = MI_B_A_ATTACK1_A;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    }
                    // 거리가 멀면
                    else
                    {
                        m_iActionState = MI_B_A_RUN;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_fSpeed = m_fSkill3RunSpeed;
                    }
                }
            }
            if (m_iActionState == MI_B_A_RUN)
            {
                m_pTransformCom->GoStraight(m_fSpeed* _fTimeDelta);
                // 거리가 가까우면
                if (m_fDistanceToTarget < m_fMeleeRange)
                {
                    m_fSpeed = 1.f;
                    m_iActionState = MI_B_A_ATTACK1_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == MI_B_A_ATTACK1_A)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    //OnCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_ATTACK1_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    SetWeaponRimLight(true);
                    GAMEINSTANCE->PlaySoundW("MB_NomalAtt", m_fVoiceSize);
                }
            }
            if (m_iActionState == MI_B_A_ATTACK1_B)
            {
                m_fCalculateTime += _fTimeDelta;
                // 패링
                if (m_bParried)
                {
                    SetWeaponRimLight(false);
                    m_iParriedPower = CheckParryingStack(m_iAtt1ParryStack);
                    if (m_iParriedPower == PARRIEDSTRONG)
                    {
                        GAMEINSTANCE->PlaySoundW("MB_Parried", m_fVoiceSize);
                        m_iBaseState = MI_B_B_BLOCKED;
                        m_bSkill3State[3] = false;
                    }
                    m_bParried = false;
                    m_fCalculateTime = 0.f;
                    m_fAnimSpeed = 1.f;
                    m_iActionState = MI_B_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                }
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    SetWeaponRimLight(false);
                    //OffCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_ATTACK1_C;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == MI_B_A_BLOCKED && (m_iParryType == 1))
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_bSkill3State[3] = false;
                    m_iSkillState = MI_B_S_NOSKILL;
                }
            }
            if (m_iActionState == MI_B_A_ATTACK1_C)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_bSkill3State[3] = false;
                    m_iSkillState = MI_B_S_NOSKILL;
                }
            }
        }
        break;

    case MI_B_S_SKILL4:
        // 서있다가 점프 내려찍기2 1
        if (m_bSkill4State[0])
        {
            if (m_iActionState == MI_B_A_STDALT)
            {
                m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fSkill4StandTime)
                {
                    m_fCalculateTime = 0.f;

                    m_iActionState = MI_B_A_JUMPATTACK2_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == MI_B_A_JUMPATTACK2_A)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_JUMPATTACK2_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_vMovePos = m_vTargetPos;
                    UpdateJumpDirSpeed2();
                    GAMEINSTANCE->PlaySoundW("MB_JumpAtt2", m_fVoiceSize);
                }
            }
            if (m_iActionState == MI_B_A_JUMPATTACK2_B)
            {
                m_fCalculateTime += _fTimeDelta;
                MovePosXZ(m_vJumpNormalMoveDir * _fTimeDelta * m_fJumpMoveSpeed + m_vCurrentPos);
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.3f)
                {
                    GAMEINSTANCE->PlaySoundW("MB_SFX_AttackGound", m_fEffectSize);
                    //OnCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_JUMPATTACK2_C;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill4State[0] = false;
                    m_bSkill4State[1] = true;
                    SetWeaponRimLight(true);
                }
            }
        }
        // 다음 점프 내려찍기1 2
        if (m_bSkill4State[1])
        {
            if (m_iActionState == MI_B_A_JUMPATTACK2_C)
            {
                // 패링
                if (m_bParried)
                {
                    SetWeaponRimLight(false);
                    m_iParriedPower = CheckParryingStack(m_iJumpAtt2ParryStack);
                    if (m_iParriedPower == PARRIEDSTRONG)
                    {
                        GAMEINSTANCE->PlaySoundW("MB_Parried", m_fVoiceSize);
                        m_iBaseState = MI_B_B_BLOCKED;
                        m_bSkill4State[1] = false;
                    }
                    m_bParried = false;
                    m_fCalculateTime = 0.f;
                    m_fAnimSpeed = 1.f;
                    m_iActionState = MI_B_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                }
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.3f)
                {
                    SetWeaponRimLight(false);
                    //OffCollisionWeapon();
                    UpdateDirection();
                    m_fCalculateTime = 0.f;

                    m_iActionState = MI_B_A_JUMPATTACK1_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == MI_B_A_BLOCKED && (m_iParryType == 2))
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    UpdateDirection();
                    m_fCalculateTime = 0.f;

                    m_iActionState = MI_B_A_JUMPATTACK1_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    GAMEINSTANCE->PlaySoundW("MB_JumpAtt1", m_fVoiceSize);
                }
            }
            if (m_iActionState == MI_B_A_JUMPATTACK1_A)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_JUMPATTACK1_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_vMovePos = m_vTargetPos;
                    UpdateJumpDirSpeed1();
                }
            }
            if (m_iActionState == MI_B_A_JUMPATTACK1_B)
            {
                m_fCalculateTime += _fTimeDelta;
                MovePosXZ(m_vJumpNormalMoveDir * _fTimeDelta * m_fJumpMoveSpeed + m_vCurrentPos);
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.3f)
                {
                    //GAMEINSTANCE->PlaySoundW("MB_SFX_AttackGound", m_fEffectSize);
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_JUMPATTACK1_C;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill4State[1] = false;
                    m_bSkill4State[2] = true;
                }
            }
        }
        // 다음 점프 내려찍기2 3
        if (m_bSkill4State[2])
        {
            if (m_iActionState == MI_B_A_JUMPATTACK1_C)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.3f)
                {
                    m_fCalculateTime = 0.f;

                    m_iActionState = MI_B_A_JUMPATTACK2_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == MI_B_A_JUMPATTACK2_A)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_JUMPATTACK2_B;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_vMovePos = m_vTargetPos;
                    UpdateJumpDirSpeed2();
                    GAMEINSTANCE->PlaySoundW("MB_JumpAtt2", m_fVoiceSize);
                }
            }
            if (m_iActionState == MI_B_A_JUMPATTACK2_B)
            {
                m_fCalculateTime += _fTimeDelta;
                MovePosXZ(m_vJumpNormalMoveDir * _fTimeDelta * m_fJumpMoveSpeed + m_vCurrentPos);
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.3f)
                {
                    GAMEINSTANCE->PlaySoundW("MB_SFX_AttackGound", m_fEffectSize);
                    //OnCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_JUMPATTACK2_C;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill4State[2] = false;
                    m_bSkill4State[3] = true;
                    SetWeaponRimLight(true);
                }
            }
        }
        // 멀면 뛰다가 내려찍기2
        if (m_bSkill4State[3])
        {
            if (m_iActionState == MI_B_A_JUMPATTACK2_C)
            {
                // 패링
                if (m_bParried)
                {
                    SetWeaponRimLight(false);
                    m_iParriedPower = CheckParryingStack(m_iJumpAtt2ParryStack);
                    if (m_iParriedPower == PARRIEDSTRONG)
                    {
                        GAMEINSTANCE->PlaySoundW("MB_Parried", m_fVoiceSize);
                        m_iBaseState = MI_B_B_BLOCKED;
                        m_bSkill4State[3] = false;
                    }
                    m_bParried = false;
                    m_fCalculateTime = 0.f;
                    m_fAnimSpeed = 1.f;
                    m_iActionState = MI_B_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                }
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.3f)
                {
                    SetWeaponRimLight(false);
                    //OffCollisionWeapon();
                    UpdateDirection();
                    m_fCalculateTime = 0.f;

                    // 거리가 가까우면
                    if (m_fDistanceToTarget < m_fAttack1Range)
                    {
                        GAMEINSTANCE->PlaySoundW("MB_SpecialAtt2Ready", m_fVoiceSize);
                        m_iActionState = MI_B_A_ATTACK2_A;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    }
                    // 거리가 멀면
                    else
                    {
                        m_iActionState = MI_B_A_RUN;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_fSpeed = m_fSkill4RunSpeed;
                    }
                }
            }
            if (m_iActionState == MI_B_A_BLOCKED && (m_iParryType == 2))
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    UpdateDirection();
                    m_fCalculateTime = 0.f;

                    // 거리가 가까우면
                    if (m_fDistanceToTarget < m_fAttack1Range)
                    {
                        GAMEINSTANCE->PlaySoundW("MB_SpecialAtt2Ready", m_fVoiceSize);
                        m_iActionState = MI_B_A_ATTACK2_A;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    }
                    // 거리가 멀면
                    else
                    {
                        m_iActionState = MI_B_A_RUN;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_fSpeed = m_fSkill4RunSpeed;
                    }
                }
            }
            if (m_iActionState == MI_B_A_RUN)
            {
                m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);
                // 거리가 가까우면
                if (m_fDistanceToTarget < m_fAttack1Range)
                {
                    GAMEINSTANCE->PlaySoundW("MB_SpecialAtt2Ready", m_fVoiceSize);
                    m_fSpeed = 1.f;
                    m_iActionState = MI_B_A_ATTACK2_A;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                }
            }
            if (m_iActionState == MI_B_A_ATTACK2_A)
            {
                m_fCalculateTime += _fTimeDelta;
                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    //OnCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_ATTACK2_B;

                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    GAMEINSTANCE->PlaySoundW("MB_SpecialAtt2", m_fVoiceSize);
                }
            }
            if (m_iActionState == MI_B_A_ATTACK2_B)
            {
                m_fCalculateTime += _fTimeDelta;
                UpdateAttack2MoveSlide(_fTimeDelta);

                if (m_fCalculateTime < m_fAttack2Move3StartTime)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 120.f * _fTimeDelta);
                }

                if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
                {
                    //OffCollisionWeapon();
                    m_fCalculateTime = 0.f;
                    m_iActionState = MI_B_A_STDALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_bSkill4State[3] = false;
                    m_iSkillState = MI_B_S_NOSKILL;
                }
            }
        }
        break;
    }

    if (m_bWhileStun)
    {
        CheckAttacked();
    }

    // 사망
    CheckDead();
}

void CMidget_Berserker::FSM_Blocked(_float _fTimeDelta)
{
    if (m_iActionState == MI_B_A_BLOCKED)
    {
        m_fCalculateTime2 += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime2 > 0.3f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            m_iBaseState = MI_B_B_NORMAL;
            m_iActionState = MI_B_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = MI_B_S_NOSKILL;
        }
    }

    // 긴 Blocked 상태에서 공격들어온 종류 체크
    if (m_iActionState == MI_B_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG)
    {
        CheckAttacked();
    }
    
    // 사망
    CheckDead();
}

void CMidget_Berserker::FSM_Hitted(_float _fTimeDelta)
{
    if (m_iActionState == MI_B_A_FRONTBE)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bHitted)
        {
            m_fCalculateTime = 0.f;
            UpdateDirection();
            m_bHitted = false;
            m_iActionState = MI_B_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.2f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            m_iBaseState = MI_B_B_NORMAL;
            m_iActionState = MI_B_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = MI_B_S_NOSKILL;
            m_bHitted = false;
            m_bWhileStun = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void CMidget_Berserker::FSM_Hitted2(_float _fTimeDelta)
{
    if (m_iActionState == MI_B_A_FRONTBE)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bHitted2)
        {
            m_fCalculateTime = 0.f;
            //UpdateDirection();
            m_bHitted2 = false;
            m_iActionState = MI_B_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.2f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            m_iBaseState = MI_B_B_NORMAL;
            m_iActionState = MI_B_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = MI_B_S_NOSKILL;
            m_bHitted2 = false;
            m_bWhileStun = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void CMidget_Berserker::FSM_Hitted3(_float _fTimeDelta)
{
    if (m_iActionState == MI_B_A_FRONTBE)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bHitted3)
        {
            m_fCalculateTime = 0.f;
            UpdateDirection();
            m_bHitted3 = false;
            m_iActionState = MI_B_A_FRONTBE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.2f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            m_iBaseState = MI_B_B_NORMAL;
            m_iActionState = MI_B_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = MI_B_S_NOSKILL;
            m_bHitted3 = false;
            m_bWhileStun = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void CMidget_Berserker::FSM_Stun(_float _fTimeDelta)
{
    if (m_iActionState == MI_B_A_STUNS)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bStun)
        {
            UpdateDirection();
            m_fCalculateTime = 0.f;
            m_bStun = false;
            m_iActionState = MI_B_A_STUNS;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iActionState = MI_B_A_STUNL;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == MI_B_A_STUNL)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iActionState = MI_B_A_STUNE;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == MI_B_A_STUNE)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            m_iBaseState = MI_B_B_NORMAL;
            m_iActionState = MI_B_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = MI_B_S_NOSKILL;
            m_bStun = false;
            m_bWhileStun = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void CMidget_Berserker::FSM_Shocked(_float _fTimeDelta)
{
    if (m_iActionState == MI_B_A_SHOCKED)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bShocked)
        {
            m_fCalculateTime = 0.f;
            UpdateDirection();
            m_bShocked = false;
            m_iActionState = MI_B_A_SHOCKED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.2f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            m_iBaseState = MI_B_B_NORMAL;
            m_iActionState = MI_B_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = MI_B_S_NOSKILL;
            m_bShocked = false;
            m_bWhileStun = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void CMidget_Berserker::FSM_Down(_float _fTimeDelta)
{
    if (m_iActionState == MI_B_A_DOWN1)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bDown)
        {
            UpdateDirection();
            m_fCalculateTime = 0.f;
            m_bDown = false;
            m_iActionState = MI_B_A_DOWN1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iActionState = MI_B_A_DOWN2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == MI_B_A_DOWN2)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iActionState = MI_B_A_DOWN3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == MI_B_A_DOWN3)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            m_iBaseState = MI_B_B_NORMAL;
            m_iActionState = MI_B_A_STDALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = MI_B_S_NOSKILL;
            m_bDown = false;
            m_bWhileStun = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void CMidget_Berserker::FSM_Death(_float _fTimeDelta)
{
    if (m_iActionState == MI_B_A_DEATH)
    {
        if (!m_bMemoryOut)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > m_fMemoryOutTime * 2.f)
            {
                ActiveMemory();
                m_fCalculateTime = 0.f;
                m_bMemoryOut = true;
            }
        }
    }
    if ((m_iActionState == MI_B_A_DEATH) && (m_pModelCom->GetIsFinishedAnimation()))
    {
        m_iActionState = MI_B_A_DEAD;
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

void CMidget_Berserker::CheckDead()
{
    if (m_fHp <= 0.f)
    {
        GAMEINSTANCE->StopSound("MB_SFX_WhirlWind");
        GAMEINSTANCE->PlaySoundW("MB_Die", m_fVoiceSize);
        //GAMEINSTANCE->SwitchingBGM2();
        SetWeaponRimLight(false);
        m_bDead = true;
        OffCollisionWeapon();
        m_iBaseState = MI_B_B_DEATH;
        m_iActionState = MI_B_A_DEATH;
        m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
        m_pRigidBody->SimulationOff();

        m_pMemory->SetLookDir(m_pTransformCom->GetState(CTransform::STATE_LOOK));

        static_pointer_cast<CPlayer>(m_pTarget)->LockOnChangeToWeapon();

        QUESTMGR->CheckDeadMonster(shared_from_this());
        QUESTMGR->ActiveEventTag(QUEST_SUB0, "Event_FindChild");
    }
}

void CMidget_Berserker::CheckAttacked()
{
    // 피격시
    if (m_bHitted)
    {
        m_fCalculateTime = 0.f;
        m_fCalculateTime2 = 0.f;
        m_iBaseState = 	MI_B_B_HITTED;
        m_iActionState = MI_B_A_FRONTBE;
    }

    // 짧은경직
    if (m_bHitted2)
    {
        m_fCalculateTime = 0.f;
        m_fCalculateTime2 = 0.f;
        m_iBaseState = MI_B_B_HITTED2;
        m_iActionState = MI_B_A_FRONTBE;
    }

    // 긴경직
    if (m_bHitted3)
    {
        m_fCalculateTime = 0.f;
        m_fCalculateTime2 = 0.f;
        m_iBaseState = MI_B_B_HITTED3;
        m_iActionState = MI_B_A_FRONTBE;
    }

    // 스턴
    if (m_bStun)
    {
        m_fCalculateTime = 0.f;
        m_fCalculateTime2 = 0.f;
        m_iBaseState = MI_B_B_STUN;
        m_iActionState = MI_B_A_STUNS;
    }

    // 감전
    if (m_bShocked)
    {
        m_fCalculateTime = 0.f;
        m_fCalculateTime2 = 0.f;
        m_iBaseState = MI_B_B_SHOCKED;
        m_iActionState = MI_B_A_SHOCKED;
    }

    // 넘어지기
    if (m_bDown)
    {
        m_fCalculateTime = 0.f;
        m_fCalculateTime2 = 0.f;
        m_iBaseState = MI_B_B_DOWN;
        m_iActionState = MI_B_A_DOWN1;
    }
}

void CMidget_Berserker::UpdateJumpDirSpeed1()
{
    _float3 vTargetDirXZ = _float3(m_vTargetDir.x, 0.f, m_vTargetDir.z);
    vTargetDirXZ.Normalize(m_vJumpNormalMoveDir);

    _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
    _float2 vMovePosXZ = _float2(m_vMovePos.x, m_vMovePos.z);
    _float fFastJumpDistance = (vCurrentPosXZ - vMovePosXZ).Length();

    m_fJumpMoveSpeed = fFastJumpDistance / (m_fKeyFrameSpeed * 16);
}

void CMidget_Berserker::UpdateJumpDirSpeed2()
{
    _float3 vTargetDirXZ = _float3(m_vTargetDir.x, 0.f, m_vTargetDir.z);
    vTargetDirXZ.Normalize(m_vJumpNormalMoveDir);

    _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
    _float2 vMovePosXZ = _float2(m_vMovePos.x, m_vMovePos.z);
    _float fFastJumpDistance = (vCurrentPosXZ - vMovePosXZ).Length() - m_fAttack1Range;

    m_fJumpMoveSpeed = fFastJumpDistance / (m_fKeyFrameSpeed * 21);
}

void CMidget_Berserker::UpdateDir(_float _fTimeDelta)
{
    switch (m_iActionState)
    {
    case MI_B_A_RUN:
        m_pTransformCom->TurnToDir(m_vTargetDir, 120.f * _fTimeDelta);
        break;
    case MI_B_A_ATTACK1_A:
        m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
        break;
    case MI_B_A_ATTACK2_A:
        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
        break;
    /*case MI_B_A_ATTACK2_B:
        m_pTransformCom->TurnToDir(m_vTargetDir, 45.f * _fTimeDelta);
        break;*/
    case MI_B_A_ATTACK3_A:
        m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
        break;
    case MI_B_A_ATTACK3_B:
        m_pTransformCom->TurnToDir(m_vTargetDir, 30.f * _fTimeDelta);
        break;
    case MI_B_A_ATTACK4_A:
        m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
        break;
    case MI_B_A_ATTACK4_B:
        m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
        break;
    case MI_B_A_JUMPATTACK1_A:
        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
        break;
    case MI_B_A_JUMPATTACK2_A:
        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
        break;
    case MI_B_A_JUMPATTACK1_C:
        m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
        break;
    case MI_B_A_WALK:
        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
        break;
    }
}
