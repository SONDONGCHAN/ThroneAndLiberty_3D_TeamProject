#include "RedKnight.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "Pillar.h"
#include "Slash.h"
#include "BindMagic.h"
#include "RedKnight_Shadow.h"
#include "Midget_Berserker_Shadow.h"
#include "Orc_DarkCommander_Shadow.h"
#include "Animation.h"
#include "EffectMgr.h"
#include "PointTrail.h"
#include "MotionTrail.h"
#include "PlayerProjectile.h"
#include "EffectAttachObj.h"
#include "CameraMgr.h"

CRedKnight::CRedKnight()
    : CMonster()
{
}

CRedKnight::~CRedKnight()
{
}

HRESULT CRedKnight::Initialize()
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
    
    m_pModelCom = GAMEINSTANCE->GetModel("M_UndeadArmy_Knight");
    m_strModelName = "M_UndeadArmy_Knight";
    AddComponent(L"Com_Model", m_pModelCom);
    m_pTransformCom->SetScaling(1.3f, 1.3f, 1.3f);
    m_pTransformCom->Rotation(_float3(0.f, 1.f, 0.f), 3.14f);
    m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
    
    AddWeaponTrail();
    AddMotionTrail(5, _float3(1.f, 1.f, 1.f), 0.3f);
    if (FAILED(AddPartObjects("N_Sword2h_UndeadArmyKnight")))
        return E_FAIL;
    
    // 상하체 분리
    //m_pModelCom->ReadySeparate("Bip001-Spine1");

    /*m_pModelCom->ChangeUpperAnim(RK_A_ALT, 0.f, true);
    m_pModelCom->ChangeLowerAnim(RK_A_WALKFL, 0.f, true);*/

    // 참격 종류별로 생성후 집어넣기
    shared_ptr<CSlash::SLASH_DESC> pSlashDesc = make_shared<CSlash::SLASH_DESC>();
    pSlashDesc->tProjectile.pTarget = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));

    shared_ptr<CTransform::TRANSFORM_DESC> pTransformDesc = make_shared<CTransform::TRANSFORM_DESC>();
    pTransformDesc->fSpeedPerSec = 10.f;
    pTransformDesc->fRotationPerSec = XMConvertToRadians(90.0f);
    
    for (int i = SLASHLDOWN; i <= SLASHBIG; i++)
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
            case SLASHBIG:
                m_vecSlashBig.push_back(pSlash);
                break;
            }
        }
    }

    // 속박마법 생성후 넣어주기
    shared_ptr<CBindMagic::BINDMAGIC_DESC> pBindMagicDesc = make_shared<CBindMagic::BINDMAGIC_DESC>();
    pBindMagicDesc->tProjectile.pTarget = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
    pBindMagicDesc->pOwner = shared_from_this();
    
    pTransformDesc->fSpeedPerSec = 10.f;
    pTransformDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    for (int j = 0; j < m_iBindMagicCount; j++)
    {
        shared_ptr<CBindMagic> pBindMagic = CBindMagic::Create(pBindMagicDesc.get(), pTransformDesc.get());
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Projectile"), pBindMagic)))
            return E_FAIL;
        m_vecBindMagic.push_back(pBindMagic);
    }
    
    // 기둥 생성후 넣어주기
    shared_ptr<CPillar::PILLAR_DESC> pPillarDesc = make_shared<CPillar::PILLAR_DESC>();
    pPillarDesc->tProjectile.pTarget = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
    pTransformDesc->fRotationPerSec = XMConvertToRadians(360.0f);

    for (int j = 1; j <= 5; j++)
    {
        for (int i = 0; i < 2; i++)
        {
            pPillarDesc->iType = j;
            shared_ptr<CPillar> pPillar = CPillar::Create(pPillarDesc.get(), pTransformDesc.get());
            if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Projectile"), pPillar)))
                return E_FAIL;

            m_vecPillar.push_back(pPillar);
        }
    }

    // 검 생성후 넣어주기
    shared_ptr<CFallingSword::FALLINGSWORD_DESC> pFallingSwordDesc = make_shared<CFallingSword::FALLINGSWORD_DESC>();
    pFallingSwordDesc->tProjectile.pTarget = shared_from_this();
    pTransformDesc->fSpeedPerSec = 1.f;
    pTransformDesc->fRotationPerSec = XMConvertToRadians(360.0f);

    for (int i = 0; i < m_iFallingSwordCount; i++)
    {
        shared_ptr<CFallingSword> pFallingSword = CFallingSword::Create(pFallingSwordDesc.get(), pTransformDesc.get());
        if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Projectile"), pFallingSword)))
            return E_FAIL;

        m_vecFallingSword.push_back(pFallingSword);
    }

    pFallingSwordDesc.reset();

    // 움직일수 있는 존 관련 위치 설정
    m_vCenterPos = _float3(20.f, -25.f, 235.f) - _float3(0.f, 5.f, 0.f); // 처음에 프레임으로 인해 5만큼 올려주기때문에
    m_vCenterPos.y = m_fGroundPosY;
    m_vRightTopPos = _float3(m_vCenterPos.x + m_fZHLength, m_vCenterPos.y, m_vCenterPos.z + m_fZHLength);
    m_vRightBottomPos = _float3(m_vCenterPos.x + m_fZHLength, m_vCenterPos.y, m_vCenterPos.z - m_fZHLength);
    m_vLeftTopPos = _float3(m_vCenterPos.x - m_fZHLength, m_vCenterPos.y, m_vCenterPos.z + m_fZHLength);
    m_vLeftBottomPos = _float3(m_vCenterPos.x - m_fZHLength, m_vCenterPos.y, m_vCenterPos.z - m_fZHLength);
    // 움직일수 있는 존의 각각 경계선 설정 
    m_fRightEnd = m_vCenterPos.x + m_fZHLength;
    m_fLeftEnd = m_vCenterPos.x - m_fZHLength;
    m_fTopEnd = m_vCenterPos.z + m_fZHLength;
    m_fBottomEnd = m_vCenterPos.z - m_fZHLength;
    // 검들 떨어질 위치 설정
    m_vSwordFallPos1 = _float3(46.f, -30.8f, 260.f);
    m_vSwordFallPos2 = _float3(-4.f, -30.8f, 260.f);
    m_vSwordFallPos3 = _float3(-4.f, -30.8f, 210.f);
    m_vSwordFallPos4 = _float3(46.f, -30.8f, 210.f);
    m_vSwordFallPos5 = _float3(21.f, -30.8f, 235.f);
    // 왕좌 위치 설정
    m_vFrontThronePos = m_vCenterPos + _float3(0.f, 2.6f, m_fZHLength + 20.f);
    m_vFrontThronePos.x = 21.2f;
    m_vFrontThronePos.y = -27.43f;

    // 분신 생성후 넣어주기
    // redknight 그림자
    shared_ptr<CRedKnight_Shadow> pModelShadowRedKnight1 = CRedKnight_Shadow::Create(m_vFrontThronePos + _float3(-9.f, 0.f, 5.f), m_vLeftTopPos + _float3(m_fZHLength, 0.f, -5.f));
    shared_ptr<CRedKnight_Shadow> pModelShadowRedKnight2 = CRedKnight_Shadow::Create(m_vFrontThronePos + _float3(-5.8f, 0.f, 5.f), m_vLeftTopPos + _float3(5.f, 0.f, -m_fZHLength));
    shared_ptr<CRedKnight_Shadow> pModelShadowRedKnight3 = CRedKnight_Shadow::Create(m_vFrontThronePos + _float3(5.8f, 0.f, 5.f), m_vRightBottomPos + _float3(-5.f, 0.f, m_fZHLength));
    shared_ptr<CRedKnight_Shadow> pModelShadowRedKnight4 = CRedKnight_Shadow::Create(m_vFrontThronePos + _float3(9.f, 0.f, 5.f), m_vRightBottomPos + _float3(-m_fZHLength, 0.f, 5.f));
    shared_ptr<CRedKnight_Shadow> pModelShadowRedKnight5 = CRedKnight_Shadow::Create(m_vFrontThronePos + _float3(6.f, 0.f, 5.f), m_vRightBottomPos + _float3(-m_fZHLength, 0.f, 5.f));
    shared_ptr<CRedKnight_Shadow> pModelShadowRedKnight6 = CRedKnight_Shadow::Create(m_vFrontThronePos + _float3(6.f, 0.f, 5.f), m_vRightBottomPos + _float3(-m_fZHLength, 0.f, 5.f));
    shared_ptr<CRedKnight_Shadow> pModelShadowRedKnight7 = CRedKnight_Shadow::Create(m_vFrontThronePos + _float3(6.f, 0.f, 5.f), m_vRightBottomPos + _float3(-m_fZHLength, 0.f, 5.f));
    shared_ptr<CRedKnight_Shadow> pModelShadowRedKnight8 = CRedKnight_Shadow::Create(m_vFrontThronePos + _float3(6.f, 0.f, 5.f), m_vRightBottomPos + _float3(-m_fZHLength, 0.f, 5.f));
    shared_ptr<CRedKnight_Shadow> pModelShadowRedKnight9 = CRedKnight_Shadow::Create(m_vFrontThronePos + _float3(6.f, 0.f, 5.f), m_vRightBottomPos + _float3(-m_fZHLength, 0.f, 5.f));
    shared_ptr<CRedKnight_Shadow> pModelShadowRedKnight10 = CRedKnight_Shadow::Create(m_vFrontThronePos + _float3(6.f, 0.f, 5.f), m_vRightBottomPos + _float3(-m_fZHLength, 0.f, 5.f));
    shared_ptr<CRedKnight_Shadow> pModelShadowRedKnight11 = CRedKnight_Shadow::Create(m_vFrontThronePos + _float3(6.f, 0.f, 5.f), m_vRightBottomPos + _float3(-m_fZHLength, 0.f, 5.f));
    shared_ptr<CRedKnight_Shadow> pModelShadowRedKnight12 = CRedKnight_Shadow::Create(m_vFrontThronePos + _float3(6.f, 0.f, 5.f), m_vRightBottomPos + _float3(-m_fZHLength, 0.f, 5.f));
    
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelShadowRedKnight1)))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelShadowRedKnight2)))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelShadowRedKnight3)))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelShadowRedKnight4)))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelShadowRedKnight5)))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelShadowRedKnight6)))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelShadowRedKnight7)))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelShadowRedKnight8)))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelShadowRedKnight9)))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelShadowRedKnight10)))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelShadowRedKnight11)))
        return E_FAIL;
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelShadowRedKnight12)))
        return E_FAIL;

    m_vecRedKnightShadow.push_back(pModelShadowRedKnight1);
    m_vecRedKnightShadow.push_back(pModelShadowRedKnight2);
    m_vecRedKnightShadow.push_back(pModelShadowRedKnight3);
    m_vecRedKnightShadow.push_back(pModelShadowRedKnight4);
    m_vecRedKnightShadow.push_back(pModelShadowRedKnight5);
    m_vecRedKnightShadow.push_back(pModelShadowRedKnight6);
    m_vecRedKnightShadow.push_back(pModelShadowRedKnight7);
    m_vecRedKnightShadow.push_back(pModelShadowRedKnight8);
    m_vecRedKnightShadow.push_back(pModelShadowRedKnight9);
    m_vecRedKnightShadow.push_back(pModelShadowRedKnight10);
    m_vecRedKnightShadow.push_back(pModelShadowRedKnight11);
    m_vecRedKnightShadow.push_back(pModelShadowRedKnight12);

    // Midget_Berserker 그림자
    shared_ptr<CMidget_Berserker_Shadow> pModelMidget_Berserker_Shadow = CMidget_Berserker_Shadow::Create(_float3(2.f, 2.f, 2.f));
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelMidget_Berserker_Shadow)))
        return E_FAIL;
    m_vecMidgetBerserkerShadow.push_back(pModelMidget_Berserker_Shadow);

    // Ork_Commander_Shadow 그림자
    shared_ptr<COrc_DarkCommander_Shadow> pModelOrc_Commander_Shadow = COrc_DarkCommander_Shadow::Create(_float3(-5.f, -5.f, -5.f));
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Monster"), pModelOrc_Commander_Shadow)))
        return E_FAIL;
    m_vecOrcDarkCommanderShadow.push_back(pModelOrc_Commander_Shadow);
    
    // 스탯
    m_fMaxHp = 2000.f;
    m_fHp = m_fMaxHp;
    m_fSpeed = 1.5f;
    
    AddRigidBody("M_UndeadArmy_Knight");
    //m_pRigidBody->SimulationOff();

    m_vLockOnOffSetPosition = { 0.0f,1.5f,0.0f };

    //PrepareMonsterHPBar(TEXT("적기사"));
    m_strDisplayName = TEXT("적기사");

    // 처음에 다른곳에 배치
    MovePos(_float3(21.f, -27.f, 310.f));
    
    return S_OK;
}

void CRedKnight::PriorityTick(_float _fTimeDelta)
{
    //if (GAMEINSTANCE->KeyDown(DIK_M))
    //{
    //    m_fHp -= 200.f;
    //}
    //if (GAMEINSTANCE->KeyDown(DIK_N))
    //{
    //    m_vecFallingSword[1]->AttackOn();
    //    m_vecFallingSword[3]->AttackOn();
    //    m_vecFallingSword[4]->AttackOn();
    //}
    //if (GAMEINSTANCE->KeyDown(DIK_7))
    //{
    //    if (!m_bQuickBatch)
    //    {
    //        m_bQuickBatch = true;
    //        m_pRigidBody->SimulationOn();
    //        MovePos(_float3(20.f, -25.f, 235.f));
    //    }
    //    else
    //    {
    //        m_bQuickBatch = false;
    //        ResetBoss();

    //        m_pRigidBody->SimulationOff();
    //        MovePos(_float3(20.f, -205.f, 435.f));
    //    }
    //}
    //if (GAMEINSTANCE->KeyDown(DIK_P))
    //{
    //    m_fHp = m_fFury1Hp;
    //    m_iBaseState = RK_B_WHILEFURY;
    //    m_iActionState = RK_A_ALT;
    //    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
    //    ResetTimer();
    //    m_i2ComboType = 10;
    //    m_i3ComboType = 10;
    //    m_fAnimSpeed = 1.f;
    //    m_bFury = true;
    //    m_iPhase = RK_P_TOFURY1;
    //    m_bFastComboSlash = false;
    //}
    //if (GAMEINSTANCE->KeyDown(DIK_O))
    //{
    //    m_bJumpAttack1EffectOn = false;
    //    m_fHp = m_fGimicPattern2Hp;
    //    m_iBaseState = RK_B_GIMICPATTERN2;
    //    m_iActionState = RK_A_ALT;
    //    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
    //    ResetTimer();
    //    m_i2ComboType = 10;
    //    m_i3ComboType = 10;
    //    m_fAnimSpeed = 1.f;
    //    m_bGimicPattern2 = true;
    //    m_bGP2SkillState[0] = true;
    //    m_iPhase = RK_P_GIMIC2;
    //    m_bFastComboSlash = false;

    //    //EFFECTMGR->PlayEffect("Boss_Aura", shared_from_this());
    //    //SetUseMotionTrail(true, true, 2.f, true, _float4(1.f, 1.f, 1.f, 1.f));
    //}

    __super::PriorityTick(_fTimeDelta);

    // 범위안에서 할수있게 하기위해서
    if ((m_iBaseState != RK_B_BEFOREMEET && m_iBaseState != RK_B_GIMICPATTERN1) && m_bQuickBatch)
    {
        m_vCurrentPos = CalculateBindary(m_vCurrentPos);
    }
    //_float3 vLookXZ = CalculateBindary(_float3(m_pTransformCom->GetState(CTransform::STATE_LOOK).x, 0.f,
    //    m_pTransformCom->GetState(CTransform::STATE_LOOK).z));
    //vLookXZ.Normalize(m_vNormLookDirXZ);
    m_vTargetPos = CalculateBindary(m_vTargetPos);

    _float2 vTargetPosXZ = _float2(m_vTargetPos.x, m_vTargetPos.z);
    _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
    m_fDistanceToTarget = (vTargetPosXZ - vCurrentPosXZ).Length();

    m_vTargetDir = m_vTargetPos - m_vCurrentPos;
    _float3 vTargetDirXZ = _float3(m_vTargetDir.x, 0.f, m_vTargetDir.z);
    vTargetDirXZ.Normalize(m_vNormTargetDirXZ);
}

void CRedKnight::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    // Base상태에 따른 FSM
    switch (m_iBaseState)
    {
    case RK_B_BEFOREMEET:
        FSM_BeforeMeet(_fTimeDelta);
        break;
    case RK_B_BEALERT:
        FSM_BeAlert(_fTimeDelta);
        break;
    case RK_B_ALERT:
        FSM_Alert(_fTimeDelta);
        break;
    case RK_B_NORMAL:
        FSM_Normal(_fTimeDelta);
        break;
    case RK_B_GIMICPATTERN1:
        FSM_GimicPattern1(_fTimeDelta);
        break;
    case RK_B_WHILEFURY:
        FSM_WhileFury(_fTimeDelta);
        break;
    case RK_B_FURY:
        FSM_Fury(_fTimeDelta);
        break;
    case RK_B_GIMICPATTERN2:
        FSM_GimicPattern2(_fTimeDelta);
        break;
    case RK_B_WHILEFURY2:
        FSM_WhileFury2(_fTimeDelta);
        break;
    case RK_B_FURY2:
        FSM_Fury2(_fTimeDelta);
        break;
    case RK_B_BLOCKED:
        FSM_Blocked(_fTimeDelta);
        break;
    case RK_B_HITTED:
        FSM_Hitted(_fTimeDelta);
        break;
    case RK_B_HITTED2:
        FSM_Hitted2(_fTimeDelta);
        break;
    case RK_B_HITTED3:
        FSM_Hitted3(_fTimeDelta);
        break;
    case RK_B_STUN:
        FSM_Stun(_fTimeDelta);
        break;
    case RK_B_SHOCKED:
        FSM_Shocked(_fTimeDelta);
        break;
    case RK_B_DOWN:
        FSM_Down(_fTimeDelta);
        break;
    case RK_B_DEATH:
        FSM_Death(_fTimeDelta);
        break;
    }

    // 애니메이션 속도 조절
    for (int i = 0; i < 100; i++)
    {
        switch (i)
        {
        case RK_A_BLOCKED:
            switch (m_iParriedPower)
            {
            case PARRIEDWEAK:
                m_fActionAnimSpeed[i] = 1.5f;
                break;
            case PARRIEDNORMAL:
                m_fActionAnimSpeed[i] = 1.f;
                break;
            case PARRIEDSTRONG:
                m_fActionAnimSpeed[i] = 0.6f;
                break;
            }
            break;
        case RK_A_SHORTJUMP:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        case RK_A_SPECIAL2:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        case RK_A_RUNFL:
            if(m_iBaseState == RK_B_NORMAL)
                m_fActionAnimSpeed[i] = 2.f;
            else if (m_iBaseState == RK_B_FURY)
                m_fActionAnimSpeed[i] = 4.f;
            break;
        case RK_A_RUNFR:
            if (m_iBaseState == RK_B_NORMAL)
                m_fActionAnimSpeed[i] = 4.f;
            else if (m_iBaseState == RK_B_FURY)
                m_fActionAnimSpeed[i] = 4.f;
            break;
        case RK_A_MAGIC:
            m_fActionAnimSpeed[i] = 2.f;
            break;
        case RK_A_CHARGESLASH:
            m_fActionAnimSpeed[i] = 2.f;
            break;
        case RK_A_RUN:
            if (m_iBaseState == RK_B_NORMAL)
                m_fActionAnimSpeed[i] = 1.f;
            else if (m_iBaseState == RK_B_FURY)
                m_fActionAnimSpeed[i] = 2.f;
            break;
        case RK_A_DEATH:
            m_fActionAnimSpeed[i] = 0.5f;
            break;
        case RK_A_HITTED:
            switch (m_iBaseState)
            {
            case RK_B_HITTED:
                m_fActionAnimSpeed[i] = 1.f;
                break;
            case RK_B_HITTED2:
                m_fActionAnimSpeed[i] = 0.5f;
                break;
            case RK_B_HITTED3:
                m_fActionAnimSpeed[i] = 0.5f;
                break;
            }
            break;
        case RK_A_SHOCKED:
            m_fActionAnimSpeed[i] = 0.5f;
            break;
        case RK_A_GROUNDMAGIC:
            m_fActionAnimSpeed[i] = 1.2f;
            break;
        case RK_A_SUMMONSHADOW:
            m_fActionAnimSpeed[i] = 2.f;
            break;
        case RK_A_TELEPORT:
            switch (m_iBaseState)
            {
            case RK_B_NORMAL:
                m_fActionAnimSpeed[i] = 1.5f;
                break;
            case RK_B_FURY:
                m_fActionAnimSpeed[i] = 1.5f;
                break;
            case RK_B_FURY2:
                m_fActionAnimSpeed[i] = 3.5f;
                break;
            }
            break;
        case RK_A_SHOTSWORD:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        case RK_A_ATT_D1:
            m_fActionAnimSpeed[i] = 0.8f;
            break;
        case RK_A_ATT_D2:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        case RK_A_ATT_D3:
            if(m_i2ComboType == 3 || m_i3ComboType == 3)
                m_fActionAnimSpeed[i] = 1.f;
            else
                m_fActionAnimSpeed[i] = 0.6f;
            break;
        case RK_A_ATT_D4:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        case RK_A_ATT_L1:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        case RK_A_ATT_L2:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        case RK_A_ATT_L3:
            m_fActionAnimSpeed[i] = 0.6f;
            break;
        case RK_A_ATT_L4:
            if (m_i2ComboType == 2 || m_i3ComboType == 2 || m_i2ComboType == 3)
                m_fActionAnimSpeed[i] = 1.f;
            else
                m_fActionAnimSpeed[i] = 1.f;
            break;
        case RK_A_ATT_LM1:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        case RK_A_ATT_LM2:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        case RK_A_ATT_U1:
            m_fActionAnimSpeed[i] = 0.8f;
            break;
        case RK_A_ATT_U2:
            m_fActionAnimSpeed[i] = 0.8f;
            break;
        case RK_A_ATT_U3:
            m_fActionAnimSpeed[i] = 0.5f;
            break;
        case RK_A_ATT_U4:
            if (m_i2ComboType == 2 || m_i3ComboType == 2)
                m_fActionAnimSpeed[i] = 1.f;
            else
                m_fActionAnimSpeed[i] = 0.6f;
            break;
        case RK_A_ATT_U5:
            if (m_i2ComboType == 3 || m_i3ComboType == 3)
                m_fActionAnimSpeed[i] = 1.f;
            else
                m_fActionAnimSpeed[i] = 1.f;
            break;
        case RK_A_SUMMON:
            m_fActionAnimSpeed[i] = 0.5f;
            //m_fActionAnimSpeed[i] = 10.f;
            break;
        case RK_A_MAGIC2:
            m_fActionAnimSpeed[i] = 0.5f;
            break;
        case RK_A_STUN:
            m_fActionAnimSpeed[i] = 0.5f;
            break;
        default:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        }
    }

    if (m_bAnimStop)
    {
    }
    else
        m_pModelCom->PlayAnimation(m_fActionAnimSpeed[m_iActionState] * m_fAnimSpeed * _fTimeDelta);

    _float3 vCurrentPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);

    m_pTransformCom->Tick(_fTimeDelta);
}

void CRedKnight::LateTick(_float _fTimeDelta)
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

        
#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CRedKnight::Render()
{
    if (m_bRender)
    {
        if(!m_pMonsterParts.begin()->second->GetActiveOn())
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

shared_ptr<CRedKnight> CRedKnight::Create(_float3 _vCreatePos)
{
    shared_ptr<CRedKnight> pInstance = make_shared<CRedKnight>();

    pInstance->SetCurrentPos(_vCreatePos); // 위치 지정
    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CRedKnight");
    
    return pInstance;
}

void CRedKnight::ActivePillar()
{
    _float3 vPos = _float3(m_vCenterPos.x, m_vCurrentPos.y - 0.1f , m_vCenterPos.z);
    for (int i = 0; i < m_iPillarCount; i++)
    {
        if (!(m_vecPillar[i]->IsEnabled()))
        {
            m_vecPillar[i]->SetCreatePos(vPos);
            switch (i)
            {
                // 크기 1
            case 0:
                m_vecPillar[i]->GetTransform()->SetState(CTransform::STATE_POSITION, vPos + _float3(0.f, 0.f, m_fZHLength2 / 2.f) - _float3(0.f, 6.f * ((i / 2) + 1), 0.f));
                break;
            case 1:
                m_vecPillar[i]->GetTransform()->SetState(CTransform::STATE_POSITION, vPos + _float3(0.f, 0.f, -m_fZHLength2 / 2.f) - _float3(0.f, 6.f * ((i / 2) + 1), 0.f));
                break;
                // 크기 2
            case 2:
                m_vecPillar[i]->GetTransform()->SetState(CTransform::STATE_POSITION, vPos + _float3(m_fZHLength2 / 2.f, 0.f, 0.f) - _float3(0.f, 6.f * ((i / 2) + 1), 0.f));
                break;
            case 3:
                m_vecPillar[i]->GetTransform()->SetState(CTransform::STATE_POSITION, vPos + _float3(-m_fZHLength2 / 2.f, 0.f, 0.f) - _float3(0.f, 6.f * ((i / 2) + 1), 0.f));
                break;
                // 크기 3
            case 4:
                m_vecPillar[i]->GetTransform()->SetState(CTransform::STATE_POSITION, vPos + _float3(-m_fZHLength2, 0.f, m_fZHLength2 / 2.f * 1.5f) - _float3(0.f, 6.f * ((i / 2) + 1), 0.f));
                break;
            case 5:
                m_vecPillar[i]->GetTransform()->SetState(CTransform::STATE_POSITION, vPos + _float3(m_fZHLength2, 0.f, -m_fZHLength2 / 2.f * 1.5f) - _float3(0.f, 6.f * ((i / 2) + 1), 0.f));
                break;
                // 크기 4
            case 6:
                m_vecPillar[i]->GetTransform()->SetState(CTransform::STATE_POSITION, vPos + _float3(0.f, 0.f, m_fZHLength2) - _float3(0.f, 6.f * ((i / 2) + 1), 0.f));
                break;
            case 7:
                m_vecPillar[i]->GetTransform()->SetState(CTransform::STATE_POSITION, vPos + _float3(0.f, 0.f, -m_fZHLength2) - _float3(0.f, 6.f * ((i / 2) + 1), 0.f));
                break;
                // 크기 5
            case 8:
                m_vecPillar[i]->GetTransform()->SetState(CTransform::STATE_POSITION, vPos + _float3(m_fZHLength2, 0.f, m_fZHLength2 / 2.f * 1.5f) - _float3(0.f, 6.f * ((i / 2) + 1), 0.f));
                break;
            case 9:
                m_vecPillar[i]->GetTransform()->SetState(CTransform::STATE_POSITION, vPos + _float3(-m_fZHLength2, 0.f, -m_fZHLength2 / 2.f * 1.5f) - _float3(0.f, 6.f * ((i / 2) + 1), 0.f));
                break;
            }
            
            m_vecPillar[i]->SetProjectileType(PJ_FALL);
            m_vecPillar[i]->SetEnable(true);
        }
    }
}

void CRedKnight::ActivePillarMoveUp()
{
    for (int i = 0; i < m_iPillarCount; i++)
    {
        m_vecPillar[i]->SetMoveUp(true);
        m_vecPillar[i]->PlayMoveUpEffect();
    }
}

void CRedKnight::ActivePillarMoveDown()
{
    for (int i = 0; i < m_iPillarCount; i++)
    {
        m_vecPillar[i]->SetMoveDown(true);
    }
}

void CRedKnight::ActiveSword()
{
    for (int i = 0; i < m_iFallingSwordCount; i++)
    {
        if (!(m_vecFallingSword[i]->IsEnabled()))
        {
            switch (i)
            {
            case 0: // 우상단
                m_vecFallingSword[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vSwordFallPos1 + _float3(0.f, 90.f, 0.f));
                break;
            case 1: // 좌상단
                m_vecFallingSword[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vSwordFallPos2 + _float3(0.f, 90.f, 0.f));
                m_vecFallingSword[i]->SetFallPos(m_vFrontThronePos + _float3(-8.f, 26.f, 0.f));
                m_vecFallingSword[i]->SetSwordType(1);
                break;
            case 2: // 좌하단
                m_vecFallingSword[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vSwordFallPos3 + _float3(0.f, 90.f, 0.f));
                break;
            case 3: // 우하단
                m_vecFallingSword[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vSwordFallPos4 + _float3(0.f, 90.f, 0.f));
                m_vecFallingSword[i]->SetFallPos(m_vFrontThronePos + _float3(0.f, 30.f, 0.f));
                m_vecFallingSword[i]->SetSwordType(3);
                break;
            case 4: // 중앙
                m_vecFallingSword[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vSwordFallPos5 + _float3(0.f, 100.f, 0.f));
                m_vecFallingSword[i]->SetFallPos(m_vFrontThronePos + _float3(8.f, 26.f, 0.f));
                m_vecFallingSword[i]->SetSwordType(2);
                break;
            }
            m_vecFallingSword[i]->GetTransform()->SetSpeed(15.f);
            m_vecFallingSword[i]->SetProjectileType(PJ_FALL);
            m_vecFallingSword[i]->OnCollisionProjectile();
            m_vecFallingSword[i]->FallStart();
            m_vecFallingSword[i]->GravityOff();
            m_vecFallingSword[i]->SetEnable(true);
        }
    }
}

void CRedKnight::ActiveGimicShadow4()
{
    for (int i = 0; i < m_iRedKnightGimicShadowCount; i++)
    {
        if (!(m_vecRedKnightShadow[i]->IsEnabled()))
        {
            m_vecRedKnightShadow[i]->SetEnable(true);
            m_vecRedKnightShadow[i]->StartPattern(RKS_P_GIMIC1);
        }
    }
}

void CRedKnight::Active2ComboShadow(_int _iNum)
{
    _int iCount = 0;
    for (int i = m_iRedKnightGimicShadowCount; i < m_iRedKnightShadowCount; i++)
    {
        if (!(m_vecRedKnightShadow[i]->IsEnabled()))
        {
            iCount++;
            m_vecRedKnightShadow[i]->SetEnable(true);
            m_vecRedKnightShadow[i]->StartPattern(RKS_P_2COMBO);
            if (iCount >= _iNum)
                break;
        }
    }
}

void CRedKnight::Active3ComboShadow(_int _iNum)
{
    _int iCount = 0;
    for (int i = m_iRedKnightGimicShadowCount; i < m_iRedKnightShadowCount; i++)
    {
        if (!(m_vecRedKnightShadow[i]->IsEnabled()))
        {
            iCount++;
            m_vecRedKnightShadow[i]->SetEnable(true);
            m_vecRedKnightShadow[i]->StartPattern(RKS_P_3COMBO);
            if (iCount >= _iNum)
                break;
        }
    }
}

void CRedKnight::ActiveJumpAttackShadow(_int _iNum, _int _iType)
{
    _int iCount = 0;
    for (int i = m_iRedKnightGimicShadowCount; i < m_iRedKnightShadowCount; i++)
    {
        if (!(m_vecRedKnightShadow[i]->IsEnabled()))
        {
            iCount++;
            m_vecRedKnightShadow[i]->SetEnable(true);
            m_vecRedKnightShadow[i]->StartPattern(RKS_P_JUMPATTACK, _iType);
            m_vecRedKnightShadow[i]->SetGenPos(_float3(m_vCurrentPos.x, m_vCenterPos.y, m_vCurrentPos.z));
            if (iCount >= _iNum)
                break;
        }
    }
}

void CRedKnight::ActiveSlashShadow(_int _iNum)
{
    _int iCount = 0;
    for (int i = m_iRedKnightGimicShadowCount; i < m_iRedKnightShadowCount; i++)
    {
        if (!(m_vecRedKnightShadow[i]->IsEnabled()))
        {
            iCount++;
            m_vecRedKnightShadow[i]->SetEnable(true);
            m_vecRedKnightShadow[i]->StartPattern(RKS_P_SLASH);
            m_vecRedKnightShadow[i]->SetGenPos(_float3(m_vCurrentPos.x, m_vCenterPos.y, m_vCurrentPos.z));
            if (iCount >= _iNum)
                break;
        }
    }
}

void CRedKnight::ActiveDashThroughShadow(_int _iNum, _int _iType)
{
    _int iCount = 0;
    for (int i = m_iRedKnightGimicShadowCount; i < m_iRedKnightShadowCount; i++)
    {
        if (!(m_vecRedKnightShadow[i]->IsEnabled()))
        {
            iCount++;
            m_vecRedKnightShadow[i]->SetEnable(true);
            m_vecRedKnightShadow[i]->StartPattern(RKS_P_DASHTHROUGH, _iType);
            m_vecRedKnightShadow[i]->SetGenPos(_float3(m_vCurrentPos.x, m_vCenterPos.y, m_vCurrentPos.z));
            if (iCount >= _iNum)
                break;
        }
    }
}

void CRedKnight::ActiveBindGroundShadow(_int _iNum, _int _iType)
{
    _int iCount = 0;
    for (int i = m_iRedKnightGimicShadowCount; i < m_iRedKnightShadowCount; i++)
    {
        if (!(m_vecRedKnightShadow[i]->IsEnabled()))
        {
            iCount++;
            m_vecRedKnightShadow[i]->SetEnable(true);
            m_vecRedKnightShadow[i]->StartPattern(RKS_P_BINDGROUND);
            m_vecRedKnightShadow[i]->SetGenPos(_float3(m_vCurrentPos.x, m_vCenterPos.y, m_vCurrentPos.z));
            if (iCount >= _iNum)
                break;
        }
    }
}

void CRedKnight::ActiveQuakeShadow(_int _iNum, _int _iType)
{
    _int iCount = 0;
    for (int i = m_iRedKnightGimicShadowCount; i < m_iRedKnightShadowCount; i++)
    {
        if (!(m_vecRedKnightShadow[i]->IsEnabled()))
        {
            iCount++;
            m_vecRedKnightShadow[i]->SetEnable(true);
            m_vecRedKnightShadow[i]->StartPattern(RKS_P_QUAKE, _iType);
            m_vecRedKnightShadow[i]->SetGenPos(_float3(m_vCurrentPos.x, m_vCenterPos.y, m_vCurrentPos.z));
            if (iCount >= _iNum)
                break;
        }
    }
}

void CRedKnight::ActiveMidgetBerserkerShadow()
{
    for (int i = 0; i < m_iMidgetBerserkerShadowCount; i++)
    {
        if (!(m_vecMidgetBerserkerShadow[i]->IsEnabled()))
        {
            m_vecMidgetBerserkerShadow[i]->SetEnable(true);
            m_vecMidgetBerserkerShadow[i]->SimulationOn();
            m_vecMidgetBerserkerShadow[i]->SetShadowActiveOn(true);
        }
    }
}

void CRedKnight::ActiveDarkOrcShadow()
{
    for (int i = 0; i < m_iOrcDarkCommanderShadowCount; i++)
    {
        if (!(m_vecOrcDarkCommanderShadow[i]->IsEnabled()))
        {
            m_vecOrcDarkCommanderShadow[i]->SetEnable(true);
            m_vecOrcDarkCommanderShadow[i]->SimulationOn();
            m_vecOrcDarkCommanderShadow[i]->SetShadowActiveOn(true);
        }
    }
}

HRESULT CRedKnight::AddRigidBody(string _strModelKey)
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
    //RigidBodyDesc.isGravity = false;
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
    CapsuleDesc.strShapeTag = "RedKnight_Body";
    RigidBodyDesc.pGeometry = &CapsuleDesc;

    RigidBodyDesc.eThisColFlag = COL_MONSTER;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER | COL_PLAYERWEAPON | COL_PLAYERPROJECTILE | COL_PLAYERSKILL | COL_CHECKBOX;
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

void CRedKnight::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    // 피격시
    if (_ColData.eColFlag & COL_STATIC || _ColData.eColFlag & COL_PLAYER || _ColData.eColFlag & COL_CHECKBOX)
        return;
    
    // 일반피격
    if ((m_iActionState == RK_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG))
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
    if ((m_iActionState == RK_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == RK_B_HITTED) || (m_iBaseState == RK_B_HITTED2))
    {
        if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack3") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_CM_SW2_DashAttack") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_1") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_2") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_3")))
        {
            //EFFECTMGR->PlayEffect("Hit_ShortStun_M", shared_from_this());

            //if ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack3"))
            //    EFFECTMGR->PlayEffect("Hit_GlowHaze", shared_from_this());

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
    if ((m_iActionState == RK_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == RK_B_HITTED) || (m_iBaseState == RK_B_HITTED2)
        || (m_iBaseState == RK_B_HITTED3))
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
    if ((m_iActionState == RK_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == RK_B_HITTED) || (m_iBaseState == RK_B_HITTED2)
        || (m_iBaseState == RK_B_HITTED3) || (m_iBaseState == RK_B_STUN))
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
    if ((m_iActionState == RK_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == RK_B_HITTED) || (m_iBaseState == RK_B_HITTED2)
        || (m_iBaseState == RK_B_HITTED3) || (m_iBaseState == RK_B_STUN) || (m_iBaseState == RK_B_SHOCKED))
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
    if ((m_iActionState == RK_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG) || (m_iBaseState == RK_B_HITTED) || (m_iBaseState == RK_B_HITTED2)
        || (m_iBaseState == RK_B_HITTED3) || (m_iBaseState == RK_B_STUN) || (m_iBaseState == RK_B_SHOCKED) || (m_iBaseState == RK_B_DOWN))
    {
        if ((_ColData.eColFlag & COL_PLAYERWEAPON) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_PowerAttack")))
        {
            //EFFECTMGR->PlayEffect("Hit_GlowHaze", shared_from_this());
            m_bDown = true;
        }
        if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_UpperAttack")))
        {
            //EFFECTMGR->PlayEffect("Hit_Plr_Sk_Upper", shared_from_this());
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
    switch (m_iBaseState)
    {
    case RK_B_NORMAL:
        if ((_szMyShapeTag == m_strATTCollider2) && (_ColData.eColFlag & COL_PLAYERPARRYING) &&
            (m_bRaSkill1State[4] || m_bSkill2State[6] || m_bSkill1State[3] || m_bRaSkill2State[4]))
        {
            m_bParried = true;
        }
        if ((_szMyShapeTag == m_strATTCollider6) && (_ColData.eColFlag & COL_PLAYERPARRYING) &&
            (m_bSkill1State[3] || m_bRaSkill2State[4]))
        {
            m_bParried = true;
        }
        if ((_szMyShapeTag == m_strATTCollider10) && (_ColData.eColFlag & COL_PLAYERPARRYING) &&
            (m_bSkill1State[3] || m_bRaSkill2State[4]))
        {
            m_bParried = true;
        }
        if ((_szMyShapeTag == m_strATTCollider12) && (_ColData.eColFlag & COL_PLAYERPARRYING) &&
            (m_bRaSkill1State[4] || m_bSkill2State[6] || m_bSkill1State[3] || m_bRaSkill2State[4]))
        {
            m_bParried = true;
        }
        if ((_szMyShapeTag == m_strDashAttCollider) && (_ColData.eColFlag == COL_PLAYERPARRYING) &&
            (m_bRaSkill1State[3] || m_bRaSkill2State[3]))
        {
            m_bParried = true;
        }
        break;
    case RK_B_FURY:
        if ((_szMyShapeTag == m_strATTCollider2) && (_ColData.eColFlag & COL_PLAYERPARRYING) &&
            (m_bRaSkill2State[5] || m_bSkill1State[1] || m_bRaSkill1State[5]))
        {
            m_bParried = true;
        }
        if ((_szMyShapeTag == m_strATTCollider6) && (_ColData.eColFlag & COL_PLAYERPARRYING) &&
            (m_bRaSkill1State[5]))
        {
            m_bParried = true;
        }
        if ((_szMyShapeTag == m_strATTCollider10) && (_ColData.eColFlag & COL_PLAYERPARRYING) &&
            (m_bRaSkill1State[5]))
        {
            m_bParried = true;
        }
        if ((_szMyShapeTag == m_strATTCollider12) && (_ColData.eColFlag & COL_PLAYERPARRYING) &&
            (m_bRaSkill2State[5] || m_bSkill1State[1] || m_bRaSkill1State[5]))
        {
            m_bParried = true;
        }
        if ((_szMyShapeTag == m_strDashAttCollider) && (_ColData.eColFlag == COL_PLAYERPARRYING) &&
            (m_bRaSkill1State[4] || m_bRaSkill2State[4]))
        {
            m_bParried = true;
        }
        break;
    case RK_B_FURY2:
        break;
    }

    // 무적기일때는 제외하고 체력감소
    if (((m_iBaseState == RK_B_NORMAL) || (m_iBaseState == RK_B_FURY) || (m_iBaseState == RK_B_GIMICPATTERN1)
        || (m_iBaseState == RK_B_WHILEFURY2) || (m_iBaseState == RK_B_FURY2) || (m_iBaseState == RK_B_BLOCKED) || (m_iBaseState == RK_B_HITTED)
        || (m_iBaseState == RK_B_HITTED2) || (m_iBaseState == RK_B_HITTED3) || (m_iBaseState == RK_B_STUN)
        || (m_iBaseState == RK_B_SHOCKED) || (m_iBaseState == RK_B_DOWN))
        && !(_ColData.eColFlag & COL_PLAYERPARRYING))
    {
        switch (m_iPhase)
        {
        // 1페이즈
        case RK_P_NORMAL:
            if (m_fHp >= m_fGimicPattern1Hp)
            {
                DamagedByPlayer(_ColData, _szMyShapeTag);
                if (m_fHp <= m_fGimicPattern1Hp)
                    m_fHp = m_fGimicPattern1Hp;
            }
            break;
        // 전멸기 이후
        case RK_P_GIMIC1:
            if (m_iActionState == RK_A_STUN)
            {
                if (m_fHp >= m_fFury1Hp)
                {
                    DamagedByPlayer(_ColData, _szMyShapeTag);
                    if (m_fHp <= m_fFury1Hp)
                        m_fHp = m_fFury1Hp;
                }
            }
            break;
        // 2페이즈
        case RK_P_FURY1:
            if (m_fHp >= m_fGimicPattern2Hp)
            {
                DamagedByPlayer(_ColData, _szMyShapeTag);
                if (m_fHp <= m_fGimicPattern2Hp)
                    m_fHp = m_fGimicPattern2Hp;
            }
            break;
        // 단상에서 떨어진 뒤
        case RK_P_FURY2:
            if (m_fHp >= 0.f)
            {
                DamagedByPlayer(_ColData, _szMyShapeTag);

                if (m_fHp <= 0.f)
                    m_fHp = 0.f;
            }
            break;
        }

        //ToggleHPBar();
        
        __super::ShowEffect(_ColData, _szMyShapeTag);
    }
}

void CRedKnight::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CRedKnight::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CRedKnight::EnableCollision(const char* _strShapeTag)
{
    m_pRigidBody->EnableCollision(_strShapeTag);
}

void CRedKnight::DisableCollision(const char* _strShapeTag)
{
    m_pRigidBody->DisableCollision(_strShapeTag);
}

HRESULT CRedKnight::AddPartObjects(string _strModelKey)
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

    m_pWeaponTrail->ChangeWeapon(pWeapon, _float3(0.f, 2.0f, 0.f), _float3(0.f, 0.f, 0.f));

    shared_ptr<CPartObject> pPartObject = static_pointer_cast<CPartObject>(pWeapon);
    m_pMonsterParts.emplace("Part_Weapon", pPartObject);

    return S_OK;
}


void CRedKnight::UpdateDistanceState()
{
    // 위치 바로 못받아오는거 때문에 우선
    if (m_vTargetPos == _float3(0.f, 0.f, 0.f))
    {
        m_iDistanceState = RK_D_BEFOREMEET;
        return;
    }

    if (m_iDistanceState == RK_D_BEFOREMEET)
    {
        if (m_fDistanceToTarget < m_fMeetRange)
        {
            m_iDistanceState = RK_D_IN2RANGE;
        }
        else
        {
            m_iDistanceState = RK_D_BEFOREMEET;
        }
    }
    else if (m_iBaseState > RK_B_BEFOREMEET)
    {
        if (m_fDistanceToTarget < m_fShortRange)
        {
            m_iDistanceState = RK_D_IN1RANGE;
        }
        else if (m_fDistanceToTarget < m_fInRange)
        {
            m_iDistanceState = RK_D_IN2RANGE;
        }
        else
        {
            m_iDistanceState = RK_D_OUTRANGE;
        }
    }
}

void CRedKnight::FSM_BeforeMeet(_float _fTimeDelta)
{
    // 아직 조우하지 않았고 플레이어가 가운데 라인을 넘어오면 첫번째 연출 시작
    if (!m_bFirstCinematic && m_pTarget->GetTransform()->GetState(CTransform::STATE_POSITION).z > (m_vCenterPos.z - 30.f))
    {
        m_bFirstCinematic = true;
        m_iActionState = RK_A_WALK;
        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        vector<string> vecEncounterFilmCamera;
        vecEncounterFilmCamera.emplace_back("FinalBossEncounter1");
        vecEncounterFilmCamera.emplace_back("FinalBossEncounter2");
        vecEncounterFilmCamera.emplace_back("FinalBossEncounter3");
        CCameraMgr::GetInstance()->FilmCameraTurnOn(vecEncounterFilmCamera);

        //GAMEINSTANCE->LevelBGMOff();
        //GAMEINSTANCE->SwitchingBGM();
    }

    if (m_bFirstCinematic)
    {
        if (m_iActionState == RK_A_WALK)
        {
            m_pTransformCom->GoStraight(2.f * _fTimeDelta);
            if (m_vCurrentPos.z < m_vCenterPos.z + 40.f)
            {
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_bFirstCinematicEnd = true;
            }
        }
    }

    if(m_bFirstCinematicEnd)
    {
        m_iBaseState = RK_B_NORMAL;
        m_iDistanceState = RK_D_OUTRANGE;
        //GAMEINSTANCE->SwitchingBGM();
    }
}

void CRedKnight::FSM_BeAlert(_float _fTimeDelta)
{
    if (m_iActionState == RK_A_WAITTOALERT)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iBaseState = RK_B_ALERT;
            m_iActionState = RK_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
    }
    if (m_iActionState == RK_A_ALERTTOWAIT)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_iBaseState = RK_B_BEFOREMEET;
            m_iActionState = RK_A_WAIT;
            m_iDistanceState = RK_D_BEFOREMEET;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        }
    }
}

void CRedKnight::FSM_Alert(_float _fTimeDelta)
{
    //m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
    if (m_iActionState == RK_A_ALT)
    {
        if (m_fDistanceToTarget < m_fFightRange)
        {
            UpdateDistanceState();
            m_iBaseState = RK_B_NORMAL;
        }
        else if (m_fDistanceToTarget > m_fMeetRange)
        {
            m_iBaseState = RK_B_BEALERT;
            m_iActionState = RK_A_ALERTTOWAIT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
        }
        else
        {
            
        }
    }
}

void CRedKnight::FSM_Normal(_float _fTimeDelta)
{
    switch (m_iDistanceState)
    {
    case RK_D_IN1RANGE:
        if (m_iActionState == RK_A_ALT)
        {
            // 패턴 잡기
            if (m_iSkillState == RK_S_NOSKILL)
            {
                switch (m_iMeleeSkillOrder[m_iMeleePatternNum])
                {
                case RK_S_INSKILL1:
                    m_iSkillState = RK_S_INSKILL1;
                    m_bSkill1State[0] = true;
                    break;
                case RK_S_INSKILL2:
                    m_iSkillState = RK_S_INSKILL2;
                    m_bSkill4State[0] = true;
                    break;
                }
                m_iMeleePatternNum++;
                CheckGimicPattern1();
            }
        }
        switch (m_iSkillState)
        {
        case RK_S_INSKILL1:
            // 서있다가 점멸해서 3콤보로
            if (m_bSkill1State[0])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > m_fIn1Skill1StandTime)
                    {
                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_TELEPORT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        EFFECTMGR->PlayEffect("Boss_Teleport", shared_from_this());
                        GAMEINSTANCE->PlaySoundW("Final_SFX_Teleport", m_fEffectSize);
                    }
                }
                if (m_iActionState == RK_A_TELEPORT)
                {
                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        MovePos(RandomBlinkPos());
                        m_fCalculateTime = 0.f;
                        m_bSkill1State[0] = false;
                        m_bSkill1State[3] = true;
                        m_fAnimSpeed = m_fIn1Skill1Combo3Speed;
                        m_iActionState = RandomNormal3ComboStartAttack();
                        m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNormalAttackStartTime);
                    }
                }
            }
            // 3콤보
            if (m_bSkill1State[3])
            {
                if (Update3ComboAttackState(_fTimeDelta))
                {
                    // 패링되었으면
                    if (m_iBaseState == RK_B_BLOCKED)
                    {
                        m_bSkill1State[3] = false;
                    }
                    m_fAnimSpeed = 1.f;
                }

                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    //플레이어 방향으로 대쉬공격
                    if (m_fCalculateTime > m_fIn1ToDashTime)
                    {
                        m_fCalculateTime = 0.f;
                        UpdateDirection();
                        m_iActionState = RK_A_DASHSTART;
                        m_fAnimSpeed = 0.5f;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                        m_bSkill1State[3] = false;
                        m_bSkill1State[4] = true;
                    }
                }
            }
            // 관통대쉬 공격
            if (m_bSkill1State[4])
            {
                if (UpdateThroughDashState(_fTimeDelta))
                {
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > m_fIn1Skill1StandTime)
                    {
                        m_fCalculateTime = 0.f;
                        UpdateDirection();
                        m_fAnimSpeed = m_fIn1Skill1SlashSpeed;
                        m_iActionState = RandomSlowSlash();
                        m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                        m_bSkill1State[4] = false;
                        m_bSkill1State[5] = true;
                    }
                }
            }
            // 관통대쉬 이후에 긴 참격
            if (m_bSkill1State[5])
            {
                if (UpdateAttackState(_fTimeDelta))
                {
                    ResetTimer();
                    m_bSlashGen = false;
                    m_fAnimSpeed = 1.f;
                    m_bSkill1State[5] = false;
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNormalAltChangeTime, true);
                    UpdateDirection();
                    UpdateDistanceState();
                    m_iSkillState = RK_S_NOSKILL;
                }
            }
            break;
        case RK_S_INSKILL2:
            // 서있다가 2 콤보 그림자
            if (m_bSkill4State[0])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.5f)
                    {
                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_SUMMONSHADOW;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        GAMEINSTANCE->PlaySoundW("Final_SummonShadow", m_fVoiceSize);
                    }
                }
                if (m_iActionState == RK_A_SUMMONSHADOW)
                {
                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        Active2ComboShadow(1);
                        m_bSummonShadow[0] = false;
                        m_bSkill4State[0] = false;
                        m_bSkill4State[1] = true;

                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    }
                }
            }
            // 그림자 소환이후에 얼마후 2콤보 그림자1, 땅찍는 그림자1 소환
            if (m_bSkill4State[1])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime > 2.f)
                    {
                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_SUMMONSHADOW;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        GAMEINSTANCE->PlaySoundW("Final_SummonShadow", m_fVoiceSize);
                    }
                }
                if (m_iActionState == RK_A_SUMMONSHADOW)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.2f && !m_bSummonShadow[0])
                    {
                        ActiveJumpAttackShadow(1);
                        m_bSummonShadow[0] = true;
                    }

                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        //Active2ComboShadow(1);
                        m_bSummonShadow[0] = false;

                        m_bSkill4State[1] = false;
                        m_bSkill4State[2] = true;

                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    }
                }
            }
            // 이후에 플레이어 방향으로 관통대쉬
            if (m_bSkill4State[2])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    //플레이어 방향으로 대쉬공격
                    if (m_fCalculateTime > m_fIn1ToDashTime)
                    {
                        m_fCalculateTime = 0.f;
                        UpdateDirection();
                        m_iActionState = RK_A_DASHSTART;
                        m_fAnimSpeed = 0.5f;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                        m_bSkill4State[2] = false;
                        m_bSkill4State[3] = true;
                    }
                }
            }
            // 관통대쉬 공격
            if (m_bSkill4State[3])
            {
                if (UpdateThroughDashState(_fTimeDelta))
                {
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > m_fIn1Skill1StandTime)
                    {
                        m_fCalculateTime = 0.f;
                        UpdateDirection();
                        m_fAnimSpeed = m_fIn1Skill1SlashSpeed;
                        m_iActionState = RandomSlowSlash();
                        m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                        m_bSkill4State[3] = false;
                        m_bSkill4State[4] = true;
                    }
                }
            }
            // 관통대쉬 이후에 긴참격
            if (m_bSkill4State[4])
            {
                if (UpdateAttackState(_fTimeDelta))
                {
                    ResetTimer();
                    m_bSlashGen = false;
                    m_fAnimSpeed = 1.f;
                    m_bSkill4State[4] = false;
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNormalAltChangeTime, true);
                    UpdateDirection();
                    UpdateDistanceState();
                    m_iSkillState = RK_S_NOSKILL;
                }
            }
            break;
        }
        break;

    case RK_D_IN2RANGE:
        if (m_iActionState == RK_A_ALT)
        {
            // 패턴 잡기
            if (m_iSkillState == RK_S_NOSKILL)
            {
                switch (m_iMiddleRangeSkillOrder[m_iMiddleRangePatternNum])
                {
                case RK_S_MIDDLESKILL1:
                    m_iSkillState = RK_S_MIDDLESKILL1;
                    m_bSkill2State[0] = true;
                    break;
                case RK_S_MIDDLESKILL2:
                    m_iSkillState = RK_S_MIDDLESKILL2;
                    m_bSkill5State[0] = true;
                    break;
                }
                m_iMiddleRangePatternNum++;
                CheckGimicPattern1();
            }
        }
        switch (m_iSkillState)
        {
        case RK_S_MIDDLESKILL1:
            // 서있다가 속박마법
            if (m_bSkill2State[0])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > m_fIn2Skill2StandTime)
                    {
                        m_fCalculateTime = 0.f;

                        m_iActionState = RK_A_MAGIC;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    }
                }
                if (m_iActionState == RK_A_MAGIC)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > m_fBindMagicStartTime && !m_bBindMagicGen)
                    {
                        ActiveBindMagic(BM_T_UPGUIDED);
                        m_bBindMagicGen = true;
                    }

                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_bBindMagicGen = false;
                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);

                        m_bSkill2State[0] = false;
                        m_bSkill2State[1] = true;
                    }
                }
            }
            // 장판찍기
            if (m_bSkill2State[1])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime > 0.5f)
                    {
                        m_fCalculateTime = 0.f;
                        m_bSlashGen = false;

                        m_iActionState = RK_A_SPECIAL1;
                        m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNomalJumpChangeTime);
                        m_bSkill2State[1] = false;
                        m_bSkill2State[2] = true;

                        UpdateDirection();
                        // 점프 방향, 속도 설정
                        m_vMovePos = m_vTargetPos;
                        UpdateSpecial1DirSpeed();
                    }
                }
            }
            // 장판찍기 이후 텔레포트
            if (m_bSkill2State[2])
            {
                if (m_iActionState == RK_A_SPECIAL1)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.5f && !m_bJumpAttack1EffectOn)
                    {
                        GAMEINSTANCE->PlaySoundW("Final_JumpAttack", m_fVoiceSize);
                        EFFECTMGR->PlayEffect("Boss_Special1", shared_from_this());
                        m_bJumpAttack1EffectOn = true;
                    }

                    if (m_fCalculateTime > m_fJumpAttackEndTime && !m_bJumpAttackSoundStart)
                    {
                        GAMEINSTANCE->PlaySoundW("Final_SFX_JumpAtt1", m_fEffectSize);
                        m_bJumpAttackSoundStart = true;
                    }

                    if ((m_fCalculateTime > (m_fSpecial1JumpStartTime + m_fIn1FSkill2SpecialAnimTime)) && (m_fCalculateTime < (m_fSpecial1JumpEndTime + m_fIn1FSkill2SpecialAnimTime)))
                    {
                        MovePos(m_vJumpNormalMoveDir * _fTimeDelta * m_fJumpMoveSpeed + m_vCurrentPos);
                    }

                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_bJumpAttackSoundStart = false;
                        m_bJumpAttack1EffectOn = false;
                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    }
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 1.f)
                    {
                        m_bSkill2State[2] = false;
                        m_bSkill2State[3] = true;
                        m_iActionState = RK_A_TELEPORT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        EFFECTMGR->PlayEffect("Boss_Teleport", shared_from_this());
                        GAMEINSTANCE->PlaySoundW("Final_SFX_Teleport", m_fEffectSize);
                    }
                }
            }
            // 텔레포트 이후에 2콤보로
            if (m_bSkill2State[3])
            {
                if (m_iActionState == RK_A_TELEPORT)
                {
                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        MovePos(RandomBlinkPos());
                        m_fCalculateTime = 0.f;
                        m_bSkill2State[3] = false;
                        m_bSkill2State[6] = true;
                        m_fAnimSpeed = m_fIn1Skill2Combo2Speed;
                        m_iActionState = RandomNormal2ComboStartAttack();
                        m_pModelCom->ChangeAnim(m_iActionState, 0.2f);
                    }
                }
            }

            // 2콤보 이후 회피점프
            if (m_bSkill2State[6])
            {
                if (Update2ComboAttackState(_fTimeDelta))
                {
                    // 패링되었으면
                    if (m_iBaseState == RK_B_BLOCKED)
                    {
                        m_bSkill2State[6] = false;
                    }
                    
                    m_fAnimSpeed = 1.f;
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.5f)
                    {
                        m_fAnimSpeed = 1.f;
                        m_fCalculateTime = 0.f;
                        m_bSkill2State[6] = false;
                        m_bSkill2State[9] = true;
                        m_iActionState = RK_A_SHORTJUMP;
                        m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNomalJumpChangeTime);
                        //UpdateDirection();
                        // 점프 방향, 속도 설정
                        m_vMovePos = CalculateDodge2Pos();
                        UpdateJumpDirSpeed();
                    }
                }
            }
            // 회피 점프
            if (m_bSkill2State[9])
            {
                m_fCalculateTime += _fTimeDelta;
                if ((m_fCalculateTime > (m_fFastJumpStartTime + m_fAnimNomalJumpChangeTime)) && (m_fCalculateTime < (m_fFastJumpEndTime + m_fAnimNomalJumpChangeTime)))
                {
                    MovePos(m_vJumpNormalMoveDir * _fTimeDelta * m_fJumpMoveSpeed + m_vCurrentPos);
                }
                if (m_fCalculateTime > m_fFastJumpEndTime)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                }
                if (m_pModelCom->GetIsFinishedAnimation())
                {
                    m_fCalculateTime = 0.f;
                    m_bSkill2State[9] = false;
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNormalAltChangeTime, true);
                    UpdateDirection();
                    UpdateDistanceState();
                    m_iSkillState = RK_S_NOSKILL;
                }
            }
            break;
        case RK_S_MIDDLESKILL2:
            // 서있기
            if (m_bSkill5State[0])
            {
                m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > 0.5f)
                {
                    m_fCalculateTime = 0.f;

                    m_iActionState = RK_A_SUMMONSHADOW;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_bSkill5State[0] = false;
                    m_bSkill5State[1] = true;
                    GAMEINSTANCE->PlaySoundW("Final_SummonShadow", m_fVoiceSize);
                }
            }
            if (m_bSkill5State[1])
            {
                if (m_iActionState == RK_A_SUMMONSHADOW)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.2f && !m_bSummonShadow[0])
                    {
                        ActiveSlashShadow(1);
                        m_bSummonShadow[0] = true;
                    }

                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_fCalculateTime = 0.f;
                        ActiveSlashShadow(1);
                        m_bSummonShadow[0] = false;
                        
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    }
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime > 3.f)
                    {
                        m_fCalculateTime = 0.f;
                        m_bSkill5State[1] = false;
                        m_bSkill5State[2] = true;

                        m_iActionState = RK_A_SUMMONSHADOW;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                        GAMEINSTANCE->PlaySoundW("Final_SummonShadow", m_fVoiceSize);
                    }
                }
            }
            // 점프그림자1, 2콤보 그림자 소환, 좀있다가 회피점프
            if (m_bSkill5State[2])
            {
                if (m_iActionState == RK_A_SUMMONSHADOW)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.2f && !m_bSummonShadow[0])
                    {
                        Active2ComboShadow(1);
                        m_bSummonShadow[0] = true;
                    }

                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_bSummonShadow[0] = false;
                        m_fCalculateTime = 0.f;
                        ActiveJumpAttackShadow(1);
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    }
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.5f)
                    {
                        m_iActionState = RK_A_DASHSTART;
                        m_fAnimSpeed = 0.5f;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                        m_fCalculateTime = 0.f;
                        m_bSkill5State[2] = false;
                        m_bSkill5State[3] = true;
                    }
                }
            }
            // 관통대쉬
            if (m_bSkill5State[3])
            {
                if (UpdateThroughDashState(_fTimeDelta))
                {
                    m_fAnimSpeed = 1.f;
                    m_fCalculateTime = 0.f;
                    m_bSkill5State[3] = false;
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    UpdateDirection();
                    UpdateDistanceState();
                    m_iSkillState = RK_S_NOSKILL;
                }
            }

            break;
        }
        break;

    case RK_D_OUTRANGE:
        if (m_iActionState == RK_A_ALT)
        {
            // 패턴 잡기
            if (m_iSkillState == RK_S_NOSKILL)
            {
                switch (m_iRangeSkillOrder[m_iRangePatternNum])
                {
                case RK_S_OUTSKILL1:
                    m_iSkillState = RK_S_OUTSKILL1;
                    m_bRaSkill1State[0] = true;
                    break;
                case RK_S_OUTSKILL2:
                    m_iSkillState = RK_S_OUTSKILL2;
                    m_bRaSkill2State[0] = true;
                    break;
                }
                m_iRangePatternNum++;
                CheckGimicPattern1();
            }
        }
        switch (m_iSkillState)
        {
        case RK_S_OUTSKILL1:
            // 서있기
            if (m_bRaSkill1State[0])
            {
                //UpdateDirection();
                m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fOutSkill1StandTime)
                {
                    m_bRaSkill1State[0] = false;
                    m_bRaSkill1State[1] = true;
                    m_fCalculateTime = 0.f;

                    m_bFastComboSlash = true;
                    m_iActionState = RandomNormal3ComboStartAttack();
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_fAnimSpeed = m_fOutSkill1SlashSpeed;
                }
            }
            // 참격 3회
            if (m_bRaSkill1State[1])
            {
                if (Update3ComboAttackState(_fTimeDelta))
                {
                    m_fAnimSpeed = 1.f;
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_bFastComboSlash = false;
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.5f)
                    {
                        m_fCalculateTime = 0.f;
                        // 위로 점프
                        m_fAnimSpeed = 1.f;
                        m_bRaSkill1State[1] = false;
                        m_bRaSkill1State[2] = true;
                        m_iActionState = RK_A_GOUP;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                    }
                }
            }
            // 위로 점프해서 플레이어 위치로 관통대쉬해서 찌르기 
            if (m_bRaSkill1State[2])
            {
                if (UpdateJumpDashState(_fTimeDelta))
                {
                    UpdateDirection();
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_bRaSkill1State[2] = false;
                    m_bRaSkill1State[3] = true;
                }
            }
            // 프론트대쉬
            if (m_bRaSkill1State[3])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.1f)
                    {
                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_DASHSTART;
                        m_fAnimSpeed = 0.5f;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                        m_bRaSkill1State[2] = false;
                        m_bRaSkill1State[3] = true;
                        SetWeaponRimLight(true);
                    }
                }
                if (UpdateFrontDashState(_fTimeDelta, 100.f))
                {
                    // 패링되었으면
                    if (m_iBaseState == RK_B_BLOCKED)
                    {
                        m_bRaSkill1State[3] = false;
                        m_fAnimSpeed = 1.f;
                    }
                    else
                    {
                        m_fAnimSpeed = m_fOutSkill1Combo2Speed;
                        m_iActionState = RandomNormal2ComboStartAttack();
                        m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                        m_bRaSkill1State[3] = false;
                        m_bRaSkill1State[4] = true;
                    }
                }
            }
            // 찌르기 이후에2콤보
            if (m_bRaSkill1State[4])
            {
                if (Update2ComboAttackState(_fTimeDelta))
                {
                    // 패링되었으면
                    if (m_iBaseState == RK_B_BLOCKED)
                    {
                        m_bRaSkill1State[4] = false;
                    }

                    m_fAnimSpeed = 1.f;
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_bRaSkill1State[4] = false;
                    UpdateDistanceState();
                    m_iSkillState = RK_S_NOSKILL;
                }
            }

            break;
        case RK_S_OUTSKILL2:
            // 서있기
            if (m_bRaSkill2State[0])
            {
                m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fOutSkill1StandTime)
                {
                    m_bRaSkill2State[0] = false;
                    m_bRaSkill2State[1] = true;
                    m_fCalculateTime = 0.f;

                    m_iActionState = RK_A_SUMMONSHADOW;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    GAMEINSTANCE->PlaySoundW("Final_SummonShadow", m_fVoiceSize);
                }
            }
            // 대쉬하는 그림자 시간차로 2마리 소환
            if (m_bRaSkill2State[1])
            {
                if (m_iActionState == RK_A_SUMMONSHADOW)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.5f && !m_bSummonShadow[0])
                    {
                        ActiveDashThroughShadow(1);
                        m_bSummonShadow[0] = true;
                    }
                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        ActiveDashThroughShadow(1);
                        m_bSummonShadow[0] = false;
                        m_fCalculateTime = 0.f;

                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    }
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.5f)
                    {
                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_SHORTJUMP;
                        m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNomalJumpChangeTime);
                        //UpdateDirection();
                        // 점프 방향, 속도 설정
                        m_vMovePos = JumpFarBackPos();
                        UpdateJumpDirSpeed();

                        m_bRaSkill2State[1] = false;
                        m_bRaSkill2State[2] = true;
                    }
                }
            }
            // 배후로 점프해서 프론트대쉬준비
            if (m_bRaSkill2State[2])
            {
                m_fCalculateTime += _fTimeDelta;
                if ((m_fCalculateTime > (m_fFastJumpStartTime + m_fAnimNomalJumpChangeTime)) && (m_fCalculateTime < (m_fFastJumpEndTime + m_fAnimNomalJumpChangeTime)))
                {
                    MovePos(m_vJumpNormalMoveDir * _fTimeDelta * m_fJumpMoveSpeed + m_vCurrentPos);
                }
                if (m_fCalculateTime > m_fFastJumpEndTime)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                }
                if (m_pModelCom->GetIsFinishedAnimation())
                {
                    UpdateDirection();
                    m_fCalculateTime = 0.f;
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                }
                if (m_iActionState == RK_A_ALT)
                {
                    //m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime2 += _fTimeDelta;
                    if (m_fCalculateTime2 > m_fOutSkill4StandTime)
                    {
                        m_fCalculateTime = 0.f;
                        m_fCalculateTime2 = 0.f;
                        m_bRaSkill2State[2] = false;
                        m_bRaSkill2State[3] = true;
                        m_iActionState = RK_A_DASHSTART;
                        m_fAnimSpeed = 0.5f;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        UpdateDirection();
                        SetWeaponRimLight(true);
                    }
                }
            }
            // 프론트대쉬해서 찌르기
            if (m_bRaSkill2State[3])
            {
                if (UpdateFrontDashState(_fTimeDelta))
                {
                    // 패링되었으면
                    if (m_iBaseState == RK_B_BLOCKED)
                    {
                        m_bRaSkill2State[3] = false;
                        m_fAnimSpeed = 1.f;
                    }
                    else
                    {
                        m_iActionState = RandomNormal3ComboStartAttack();
                        m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNormalAttackStartTime);
                        m_bRaSkill2State[3] = false;
                        m_bRaSkill2State[4] = true;
                        m_fAnimSpeed = m_fOutSkill2Combo3Speed;
                        UpdateDirection();
                    }
                }
            }
            // 빠른 3콤보
            if (m_bRaSkill2State[4])
            {
                if (Update3ComboAttackState(_fTimeDelta))
                {
                    // 패링되었으면
                    if (m_iBaseState == RK_B_BLOCKED)
                    {
                        m_bRaSkill2State[4] = false;
                    }
                    m_fAnimSpeed = 1.f;
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_bRaSkill2State[4] = false;
                    UpdateDistanceState();
                    m_iSkillState = RK_S_NOSKILL;
                }
            }
            break;
        }
        break;
    }
}

void CRedKnight::FSM_GimicPattern1(_float _fTimeDelta)
{
    // 텔레포트로 맵 중앙으로 가기
    if (m_bGP1SkillState[0])
    {
        if (m_iActionState == RK_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;

            if (m_fCalculateTime > 1.f)
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_TELEPORT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                EFFECTMGR->PlayEffect("Boss_Teleport", shared_from_this());
                GAMEINSTANCE->PlaySoundW("Final_SFX_Teleport", m_fEffectSize);
            }
        }
        if (m_iActionState == RK_A_TELEPORT)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                MovePos(_float3(m_vCenterPos.x, m_vCurrentPos.y, m_vCenterPos.z));
                m_fCalculateTime = 0.f;
                m_bGP1SkillState[0] = false;
                m_bGP1SkillState[1] = true;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.2f, true);
                m_pTransformCom->LookAtDir(_float3(m_vCenterPos.x, 0.f, m_vCenterPos.z) -
                    _float3(m_vCurrentPos.x, 0.f, m_vCurrentPos.z + 1.f));
            }
        }
    }
    // 기둥 솟아오르게 하기
    if (m_bGP1SkillState[1])
    {
        if (m_iActionState == RK_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > 1.f)
            {
                vector<string> vecPillarUp;
                vecPillarUp.emplace_back("FinalBossPillarUp1");
                vecPillarUp.emplace_back("FinalBossPillarUp2");
                CCameraMgr::GetInstance()->FilmCameraTurnOn(vecPillarUp);
                m_fCalculateTime = 0.f;
                ActivePillar();
                m_iActionState = RK_A_MAGIC2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                GAMEINSTANCE->PlaySoundW("Final_SummonShadow", m_fVoiceSize);
            }
        }
        if (m_iActionState == RK_A_MAGIC2)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                ActivePillarMoveUp();
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_bGP1SkillState[1] = false;
                m_bGP1SkillState[2] = true;
                GAMEINSTANCE->PlaySoundL("Final_SFX_Quake", m_fEffectSize);
            }
        }
    }
    // 시작
    if (m_bGP1SkillState[2])
    {
        if (m_iActionState == RK_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > m_fGimic2ReadyTime)
            {
                //GAMEINSTANCE->StopSound("Final_SFX_Quake");
                m_bGimic2SlashType = 0;
                m_bGP1SkillState[2] = false;
                m_bGP1SkillState[3] = true;
                m_fCalculateTime3 = 0.f;
            }
        }
    }
    // 랜덤기술들 난사
    if (m_bGP1SkillState[3])
    {
        m_fCalculateTime4 += _fTimeDelta;
        if (m_fCalculateTime4 > m_fGimic2TimeLimit && m_iActionState == RK_A_ALT)
        {
            m_fCalculateTime4 = 0.f;
            ResetTimer();
            m_bGimic2SlashType = 10;
            m_bGP1SkillState[3] = false;
            m_bGP1SkillState[4] = true;
            m_pTransformCom->LookAtDir(_float3(m_vCenterPos.x, 0.f, m_vCenterPos.z) -
                _float3(m_vCurrentPos.x, 0.f, m_vCurrentPos.z + 1.f));
        }

        if (m_iActionState == RK_A_ALT)
        {
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
        }

        if (m_bGimic2SlashType == 0)
        {
            m_fCalculateTime2 += _fTimeDelta;
            if (m_fCalculateTime2 > 0.5f)
            {
                m_bGimic2SlashType = GAMEINSTANCE->PickRandomInt(1, 5);
                switch (m_bGimic2SlashType)
                {
                case 1:
                    m_bFastComboSlash = true;
                    m_fAnimSpeed = m_fGimic23SlashSpeed;
                    m_iActionState = RandomNormal3ComboStartAttack();
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    break;
                case 2:
                    m_iActionState = RK_A_CHARGESLASH;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    GAMEINSTANCE->PlaySoundW("Final_ChargeSlash", m_fVoiceSize);
                    break;
                case 3:
                    UpdateDirection();
                    m_iActionState = RK_A_MAGIC;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    break;
                case 4:
                    m_iActionState = RK_A_CHARGESLASH;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    GAMEINSTANCE->PlaySoundW("Final_ChargeSlash2", m_fVoiceSize);
                    break;
                }
                m_fCalculateTime = 0.f;
                m_fCalculateTime2 = 0.f;
            }
        }
        else
        {
            switch (m_bGimic2SlashType)
            {
                // 3연참
            case 1:
                if (Update3ComboAttackState(_fTimeDelta))
                {
                    m_fAnimSpeed = 1.f;
                    m_fCalculateTime = 0.f;
                    m_bGimic2SlashType = 0;
                    UpdateDirection();
                    m_bFastComboSlash = false;
                }
                break;
                // 차징참격
            case 2:
                if (m_iActionState == RK_A_CHARGESLASH)
                {
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime < m_fChargeSlashStartSlashTime + 0.f)
                    {
                        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    }

                    // 거대 참격 소환
                    if (m_fCalculateTime > (m_fChargeSlashStartSlashTime + 0.f) && !m_bSlashGen)
                    {
                        m_bSlashGen = true;
                        ActiveSlash(SLASHBIG);
                    }

                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_fCalculateTime = 0.f;
                        m_fAnimSpeed = 1.f;
                        m_bSlashGen = false;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        UpdateDirection();
                        m_bGimic2SlashType = 0;
                    }
                }
                break;
                // 속박마법
            case 3:
                if (m_iActionState == RK_A_MAGIC)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > m_fBindMagicStartTime && !m_bBindMagicGen)
                    {
                        ActiveBindMagic(BM_T_UPGUIDED);
                        m_bBindMagicGen = true;
                    }

                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_fCalculateTime = 0.f;
                        m_bBindMagicGen = false;
                        m_fAnimSpeed = 1.f;
                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                        m_bGimic2SlashType = 0;
                    }
                }
                break;
                // 차징참격
            case 4:
                if (m_iActionState == RK_A_CHARGESLASH)
                {
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime < m_fChargeSlashStartSlashTime + 0.f)
                    {
                        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    }

                    // 거대 참격 소환
                    if (m_fCalculateTime > (m_fChargeSlashStartSlashTime + 0.f) && !m_bSlashGen)
                    {
                        m_bSlashGen = true;
                        ActiveSlash(SLASHBIG);
                    }

                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_fCalculateTime = 0.f;
                        m_fAnimSpeed = 1.f;
                        m_bSlashGen = false;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        UpdateDirection();
                        m_bGimic2SlashType = 0;
                    }
                }
                break;
            }
        }
    }
    // 분노
    if (m_bGP1SkillState[4])
    {
        if (m_iActionState == RK_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > 1.f)
            {
                vector<string> vecFury;
                vecFury.emplace_back("FinalBossFury1");
                vecFury.emplace_back("FinalBossFury2");
                vecFury.emplace_back("FinalBossFury3");
                CCameraMgr::GetInstance()->FilmCameraTurnOn(vecFury);
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_TOFURY;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                m_iEffectIdx = EFFECTMGR->PlayEffect("Boss_Aura", shared_from_this());
            }
        }
        if (m_iActionState == RK_A_TOFURY)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_bGimic2SlashType = 0;
                m_bGP1SkillState[4] = false;
                m_bGP1SkillState[5] = true;
                m_fCalculateTime3 = 0.f;
            }
        }
    }
    // 2중찍기
    if (m_bGP1SkillState[5])
    {
        if (m_iActionState == RK_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > 1.f)
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_SPECIAL2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                GAMEINSTANCE->PlaySoundW("Final_Ultimate1", m_fVoiceSize);
                EFFECTMGR->PlayEffect("Boss_Special2", shared_from_this());
                GAMEINSTANCE->PlaySoundW("Final_SFX_Ultimate2", m_fEffectSize);
                //GAMEINSTANCE->PlaySoundW("Final_SFX_Ultimate1", m_fEffectSize);
            }
        }
        if (m_iActionState == RK_A_SPECIAL2)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > m_fSpecial2FirstTime && !m_bSpecial2FirstGround)
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Ultimate3", m_fEffectSize);
                m_bSpecial2FirstGround = true;
            }

            if (m_fCalculateTime > m_fSpecial2SecondTime && !m_bSpecial2SecondGround)
            {
                GAMEINSTANCE->PlaySoundW("Final_Ultimate3", m_fVoiceSize);
                GAMEINSTANCE->PlaySoundW("Final_SFX_Ultimate3", m_fEffectSize);
                m_bSpecial2SecondGround = true;
            }

            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_STUN;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
                m_bGimic2SlashType = 0;
                m_bGP1SkillState[5] = false;
                m_bGP1SkillState[7] = true;
                ResetTimer();
                SimulationOn();
            }
        }
    }
    // 대기했다가 기둥 내려가기
    if (m_bGP1SkillState[7])
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_fCalculateTime > m_fGimic2BeforePillarDownTime)
        {
            m_fCalculateTime = 0.f;
            // 기둥 내려가게 하기
            ActivePillarMoveDown();
            m_bGP1SkillState[7] = false;
            m_bGP1SkillState[6] = true;
        }
    }

    // 피 60퍼 이하 될때까지 스턴 지속
    if (m_bGP1SkillState[6])
    {
        if (m_iActionState == RK_A_STUN)
        {
            if (m_fHp <= m_fFury1Hp)
            {
                m_fCalculateTime = 0.f;
                m_fHp = m_fFury1Hp;
                m_iBaseState = RK_B_WHILEFURY;
                m_iActionState = RK_A_DOWN1;
                //m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                ResetTimer();
                m_i2ComboType = 10;
                m_i3ComboType = 10;
                m_fAnimSpeed = 1.f;
                m_bFury = true;
                m_iPhase = RK_P_TOFURY1;
                m_bGP1SkillState[6] = false;
                GAMEINSTANCE->PlaySoundW("Final_HardHitted", m_fVoiceSize);

                //GAMEINSTANCE->LevelBGMOff();
                //GAMEINSTANCE->SwitchingBGM2();
            }
        }
    }
}

void CRedKnight::FSM_WhileFury(_float _fTimeDelta)
{
    if (m_iActionState == RK_A_DOWN1)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iActionState = RK_A_DOWN2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == RK_A_DOWN2)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iActionState = RK_A_DOWN3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == RK_A_DOWN3)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iActionState = RK_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            GAMEINSTANCE->PlaySoundW("Final_Roar", m_fVoiceSize);
        }
    }

    if (m_iActionState == RK_A_ALT)
    {
        m_fCalculateTime += _fTimeDelta;
        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);

        if (m_fCalculateTime > 1.f)
        {
            m_fCalculateTime = 0.f;
            //GAMEINSTANCE->SwitchingBGM2();
            m_iBaseState = RK_B_FURY;

            UpdateDistanceState();
            m_iSkillState = RK_S_NOSKILL;
            m_fAnimSpeed = 1.f;
            m_iMeleePatternNum = 0;
            m_iMiddleRangePatternNum = 0;
            m_iRangePatternNum = 0;
            m_iPhase = RK_P_FURY1;
        }
    }
}

void CRedKnight::FSM_Fury(_float _fTimeDelta)
{
    switch (m_iDistanceState)
    {
    case RK_D_IN1RANGE:
        if (m_iActionState == RK_A_ALT)
        {
            // 패턴 잡기
            if (m_iSkillState == RK_S_NOSKILL)
            {
                switch (m_iMeleeSkillOrder[m_iMeleePatternNum])
                {
                case RK_S_INSKILL1:
                    m_iSkillState = RK_S_INSKILL1;
                    m_bSkill1State[0] = true;
                    break;
                case RK_S_INSKILL2:
                    m_iSkillState = RK_S_INSKILL2;
                    m_bSkill4State[0] = true;
                    break;
                }
                m_iMeleePatternNum++;
                CheckGimicPattern2();
            }
        }
        switch (m_iSkillState)
        {
        case RK_S_INSKILL1:
            // 서있다가 전방 지진공격
            if (m_bSkill1State[0])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime > m_fF1In1StandTime)
                    {
                        UpdateDirection();
                        ResetTimer();
                        m_iActionState = RK_A_FRONTQUAKE;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                        GAMEINSTANCE->PlaySoundW("Final_Quake", m_fVoiceSize);
                        EFFECTMGR->PlayEffect("Boss_EarthQuake_Anim", shared_from_this());
                        EFFECTMGR->PlayEffect("Boss_EarthQuake", shared_from_this());
                    }
                }
                if (m_iActionState == RK_A_FRONTQUAKE)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > m_fQuakeStartTime && !m_bQuakeSoundStart)
                    {
                        GAMEINSTANCE->PlaySoundW("Final_SFX_Quake", m_fEffectSize);
                        m_bQuakeSoundStart = true;
                    }
                    if (m_fCalculateTime > m_fQuakeStartTime2 && !m_bQuakeCameraStart)
                    {
                        ROT_DESC RotDesc{};
                        RotDesc.eShakeFunc = SHAKE_WAVE;
                        RotDesc.isOffSetRand = false;
                        RotDesc.vRotAmplitude = { 0.02618f,0.014f,0.014f };
                        RotDesc.vRotFrequency = { 10.0f,6.0f,2.0f };
                        RotDesc.vBlendInOut = { 0.1f,0.2f };
                        RotDesc.vInOutWeight = { 1.5f,1.5f };
                        LOC_DESC LocDesc{};
                        LocDesc.eShakeFunc = SHAKE_LINEAR;
                        LocDesc.isOffSetRand = false;
                        LocDesc.vLocAmplitude = { 0.3f,0.5f,0.2f };
                        LocDesc.vLocFrequency = { 3.5f,5.5f,2.0f };
                        LocDesc.vBlendInOut = { 0.1f,0.2f };
                        LocDesc.vInOutWeight = { 1.5f,1.5f };
                        CCameraMgr::GetInstance()->CameraShake(SHAKE_ROT | SHAKE_LOC, 0.4f, &RotDesc, &LocDesc);
                        m_bQuakeCameraStart = true;
                    }
                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_fCalculateTime = 0.f;
                        m_bQuakeCameraStart = false;
                        m_bQuakeSoundStart = false;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_bSkill1State[0] = false;
                        m_bSkill1State[1] = true;
                    }
                }
            }
            // 순간이동하여 2콤보공격
            if (m_bSkill1State[1])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.5f)
                    {
                        ResetTimer();
                        m_iActionState = RK_A_TELEPORT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                        EFFECTMGR->PlayEffect("Boss_Teleport", shared_from_this());
                        GAMEINSTANCE->PlaySoundW("Final_SFX_Teleport", m_fEffectSize);
                    }
                }

                if (m_iActionState == RK_A_TELEPORT)
                {
                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        MovePos(RandomBlinkPos());
                        UpdateDirection();
                        ResetTimer();
                        m_fAnimSpeed = m_fF1In1Combo2Speed;
                        m_iActionState = RandomNormal2ComboStartAttack();
                        m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNormalAttackStartTime);
                    }
                }
                if (Update2ComboAttackState(_fTimeDelta))
                {
                    // 패링되었으면
                    if (m_iBaseState == RK_B_BLOCKED)
                    {
                        m_bSkill1State[1] = false;
                    }
                    m_fAnimSpeed = 1.f;
                    m_bSkill1State[1] = false;
                    m_bSkill1State[2] = true;
                }
            }
            // 관통대쉬
            if (m_bSkill1State[2])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    //플레이어 방향으로 관통대쉬
                    if (m_fCalculateTime > 0.1f)
                    {
                        ResetTimer();
                        UpdateDirection();
                        m_iActionState = RK_A_DASHSTART;
                        m_fAnimSpeed = 0.5f;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                    }
                }
                if (UpdateThroughDashState(_fTimeDelta))
                {
                    m_fAnimSpeed = 0.5f;
                    m_iActionState = RK_A_DASHSTART;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.3f);

                    m_vDashOutPos = RandomDashOutPos(20.f);
                    m_vDashOutDir = m_vDashOutPos - m_vCurrentPos;

                    m_bSkill1State[2] = false;
                    m_bSkill1State[3] = true;
                }
            }
            // 빠지는 대쉬
            if (m_bSkill1State[3])
            {
                if (UpdatePositionDashState(_fTimeDelta))
                {
                    m_fDashDistanceXZ = m_fDistanceToTarget + 15.f;
                    m_fDashTime = m_fDashDistanceXZ / 100.f;
                    m_vDashDir = m_vNormTargetDirXZ;
                    SetUseMotionTrail(true, true, m_fDashTime, true, m_vThroughDashColor);
                    
                    m_bSkill1State[3] = false;
                    m_bSkill1State[4] = true;
                    GAMEINSTANCE->PlaySoundW("Final_SFX_Dash2", m_fEffectSize);
                }
            }
            // 관통대쉬
            if (m_bSkill1State[4])
            {
                if (UpdateAThroughDashState(_fTimeDelta))
                {
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.1f)
                    {
                        m_fCalculateTime = 0.f;
                        m_fAnimSpeed = 1.f;
                        m_bSkill1State[4] = false;
                        UpdateDistanceState();
                        m_iSkillState = RK_S_NOSKILL;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    }
                }
            }
            
            break;
        case RK_S_INSKILL2:
            // 서있다가 땅찍는 그림자 3마리 시간차 소환
            if (m_bSkill4State[0])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime > m_fF1In1StandTime)
                    {
                        UpdateDirection();
                        ResetTimer();
                        m_iActionState = RK_A_SUMMONSHADOW;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                        ActiveJumpAttackShadow(1);
                        GAMEINSTANCE->PlaySoundW("Final_SummonShadow2", m_fVoiceSize);
                    }
                }
                if (m_iActionState == RK_A_SUMMONSHADOW)
                {
                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        ActiveJumpAttackShadow(1);
                        m_fAnimSpeed = 1.f;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_bSkill4State[0] = false;
                        m_bSkill4State[1] = true;
                    }
                }
            }
            // 전방지진
            if (m_bSkill4State[1])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime > 0.5f)
                    {
                        ActiveJumpAttackShadow(1);
                        UpdateDirection();
                        ResetTimer();
                        m_iActionState = RK_A_FRONTQUAKE;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                        GAMEINSTANCE->PlaySoundW("Final_Quake", m_fVoiceSize);
                        EFFECTMGR->PlayEffect("Boss_EarthQuake_Anim", shared_from_this());
                        EFFECTMGR->PlayEffect("Boss_EarthQuake", shared_from_this());
                    }
                }
                if (m_iActionState == RK_A_FRONTQUAKE)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > m_fQuakeStartTime && !m_bQuakeSoundStart)
                    {
                        m_fCalculateTime = 0.f;
                        GAMEINSTANCE->PlaySoundW("Final_SFX_Quake", m_fEffectSize);
                        m_bQuakeSoundStart = true;
                    }
                    if (m_fCalculateTime > m_fQuakeStartTime2 && !m_bQuakeCameraStart)
                    {
                        ROT_DESC RotDesc{};
                        RotDesc.eShakeFunc = SHAKE_WAVE;
                        RotDesc.isOffSetRand = false;
                        RotDesc.vRotAmplitude = { 0.02618f,0.014f,0.014f };
                        RotDesc.vRotFrequency = { 10.0f,6.0f,2.0f };
                        RotDesc.vBlendInOut = { 0.1f,0.2f };
                        RotDesc.vInOutWeight = { 1.5f,1.5f };
                        LOC_DESC LocDesc{};
                        LocDesc.eShakeFunc = SHAKE_LINEAR;
                        LocDesc.isOffSetRand = false;
                        LocDesc.vLocAmplitude = { 0.3f,0.5f,0.2f };
                        LocDesc.vLocFrequency = { 3.5f,5.5f,2.0f };
                        LocDesc.vBlendInOut = { 0.1f,0.2f };
                        LocDesc.vInOutWeight = { 1.5f,1.5f };
                        CCameraMgr::GetInstance()->CameraShake(SHAKE_ROT | SHAKE_LOC, 0.4f, &RotDesc, &LocDesc);
                        m_bQuakeCameraStart = true;
                    }
                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_bQuakeCameraStart = false;
                        m_bQuakeSoundStart = false;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_bSkill4State[1] = false;
                        m_bSkill4State[2] = true;
                    }
                }
            }
            // 차징 참격
            if (m_bSkill4State[2])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.1f)
                    {
                        UpdateDirection();
                        ResetTimer();
                        m_iActionState = RK_A_CHARGESLASH;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                        GAMEINSTANCE->PlaySoundW("Final_ChargeSlash", m_fVoiceSize);
                    }
                }

                if (m_iActionState == RK_A_CHARGESLASH)
                {
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime < m_fChargeSlashStartSlashTime + 0.f)
                    {
                        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    }

                    // 거대 참격 소환
                    if (m_fCalculateTime > (m_fChargeSlashStartSlashTime + 0.f) && !m_bSlashGen)
                    {
                        m_bSlashGen = true;
                        ActiveSlash(SLASHBIG);
                    }

                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        ResetTimer();
                        m_bSlashGen = false;
                        m_bSkill4State[2] = false;
                        m_bSkill4State[3] = true;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        UpdateDirection();
                    }
                }
            }
            // 관통대쉬
            if (m_bSkill4State[3])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime > m_fF1In2StandTime)
                    {
                        UpdateDirection();
                        ResetTimer();
                        m_fAnimSpeed = 0.5f;
                        m_iActionState = RK_A_DASHSTART;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                    }
                }
                if (UpdateThroughDashState(_fTimeDelta), 100.f, 25.f)
                {
                    m_fAnimSpeed = 1.f;
                    m_fCalculateTime = 0.f;
                    m_bSkill4State[3] = false;
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNormalAltChangeTime, true);
                    UpdateDirection();
                    UpdateDistanceState();
                    m_iSkillState = RK_S_NOSKILL;
                }
            }
            break;
        }
        break;
    case RK_D_IN2RANGE:
        if (m_iActionState == RK_A_ALT)
        {
            // 패턴 잡기
            if (m_iSkillState == RK_S_NOSKILL)
            {
                switch (m_iMiddleRangeSkillOrder[m_iMiddleRangePatternNum])
                {
                case RK_S_MIDDLESKILL1:
                    m_iSkillState = RK_S_MIDDLESKILL1;
                    m_bSkill2State[0] = true;
                    break;
                case RK_S_MIDDLESKILL2:
                    m_iSkillState = RK_S_MIDDLESKILL2;
                    m_bSkill5State[0] = true;
                    break;
                }
                m_iMiddleRangePatternNum++;
                CheckGimicPattern2();
            }
        }
        switch (m_iSkillState)
        {
        case RK_S_MIDDLESKILL1:
            // 서있다가 속박장판 그림자 2마리 시간차 소환
            if (m_bSkill2State[0])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    
                    if (m_fCalculateTime > m_fF1Middle1StandTime)
                    {
                        ResetTimer();
                        m_iActionState = RK_A_SUMMONSHADOW;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.2f, false);
                        GAMEINSTANCE->PlaySoundW("Final_SummonShadow2", m_fVoiceSize);
                    }
                }
                if (m_iActionState == RK_A_SUMMONSHADOW)
                {
                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_fCalculateTime = 0.f;
                        UpdateDirection();
                        ActiveBindGroundShadow(1);
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_bSkill2State[0] = false;
                        m_bSkill2State[1] = true;
                    }
                }
            }
            // 속박마법
            if (m_bSkill2State[1])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime > 0.5f)
                    {
                        UpdateDirection();
                        ResetTimer();
                        m_iActionState = RK_A_MAGIC;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                    }
                }
                if (m_iActionState == RK_A_MAGIC)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > m_fBindMagicStartTime && !m_bBindMagicGen)
                    {
                        ActiveBindMagic(BM_T_UPGUIDED);
                        m_bBindMagicGen = true;
                    }

                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_fCalculateTime = 0.f;
                        m_bBindMagicGen = false;
                        m_fAnimSpeed = 1.f;
                        //ActiveBindGroundShadow(1);
                        ResetTimer();
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                        m_bSkill2State[1] = false;
                        m_bSkill2State[2] = true;
                    }
                }
            }
            // 차징 참격
            if (m_bSkill2State[2])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    
                    if (m_fCalculateTime > 0.1f)
                    {
                        UpdateDirection();
                        ResetTimer();
                        m_iActionState = RK_A_CHARGESLASH;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                        GAMEINSTANCE->PlaySoundW("Final_ChargeSlash2", m_fVoiceSize);
                    }
                }

                if (m_iActionState == RK_A_CHARGESLASH)
                {
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime > 1.f && !m_bF1Middle1FirstSummon)
                    {
                        ActiveBindGroundShadow(1);
                        m_bF1Middle1FirstSummon = true;
                    }

                    if (m_fCalculateTime < m_fChargeSlashStartSlashTime + 0.f)
                    {
                        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    }
                    
                    // 거대 참격 소환
                    if (m_fCalculateTime > (m_fChargeSlashStartSlashTime + 0.f) && !m_bSlashGen)
                    {
                        m_bSlashGen = true;
                        ActiveSlash(SLASHBIG);
                    }

                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_bF1Middle1FirstSummon = false;
                        ResetTimer();
                        m_bSlashGen = false;
                        m_bSkill2State[2] = false;
                        m_bSkill2State[3] = true;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        UpdateDirection();
                    }
                }
            }
            // 서있다가 빠지는 관통대쉬
            if (m_bSkill2State[3])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime > m_fF1Middle1StandTime)
                    {
                        UpdateDirection();
                        ResetTimer();
                        m_fAnimSpeed = 0.5f;
                        m_iActionState = RK_A_DASHSTART;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);

                        m_vDashOutPos = RandomDashOutPos(20.f);
                        m_vDashOutDir = m_vDashOutPos - m_vCurrentPos;

                        m_bSkill2State[3] = false;
                        m_bSkill2State[4] = true;
                    }
                }
            }
            // 빠지는 관통대쉬
            if (m_bSkill2State[4])
            {
                if (UpdatePositionDashState(_fTimeDelta))
                {
                    m_fDashDistanceXZ = m_fDistanceToTarget + 25.f;
                    m_fDashTime = m_fDashDistanceXZ / 100.f;
                    m_vDashDir = m_vNormTargetDirXZ;
                    SetUseMotionTrail(true, true, m_fDashTime, true, m_vThroughDashColor);

                    m_bSkill2State[4] = false;
                    m_bSkill2State[5] = true;
                    GAMEINSTANCE->PlaySoundW("Final_SFX_Dash2", m_fEffectSize);
                }
            }
            // 관통대쉬
            if (m_bSkill2State[5])
            {
                if (UpdateAThroughDashState(_fTimeDelta))
                {
                    m_bSkill2State[5] = false;
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    UpdateDistanceState();
                    m_iSkillState = RK_S_NOSKILL;
                }
            }
            break;
        case RK_S_MIDDLESKILL2:
            if (m_bSkill5State[0])
            {
                m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fF1Middle2StandTime)
                {
                    m_bSkill5State[0] = false;
                    m_bSkill5State[1] = true;
                    m_fCalculateTime = 0.f;

                    m_iActionState = RK_A_GROUNDMAGIC;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    //GAMEINSTANCE->PlaySoundW("Final_BindZone", m_fVoiceSize);

                    EFFECTMGR->PlayEffect("Boss_GroundMagic", shared_from_this());
                    GAMEINSTANCE->PlaySoundW("Final_SFX_GoundMagic", m_fEffectSize);
                }
            }
            // 속박장판마법
            if (m_bSkill5State[1])
            {
                if (m_iActionState = RK_A_GROUNDMAGIC)
                {
                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_bSkill5State[1] = false;
                        m_bSkill5State[2] = true;
                        m_fCalculateTime2 = 0.f;

                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    }
                }
            }
            // 서있기
            if (m_bSkill5State[2])
            {
                m_fCalculateTime2 += _fTimeDelta;

                if (m_fCalculateTime2 > m_fSummonIntervel)
                {
                    m_fCalculateTime2 = 0.f;
                    ActiveDashThroughShadow(1);
                }
                //UpdateDirection();
                m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                m_fCalculateTime += _fTimeDelta;
                if (m_fCalculateTime > m_fF1Middle2StandTime)
                {
                    m_bSkill5State[2] = false;
                    m_bSkill5State[3] = true;
                    ResetTimer();
                    m_fCalculateTime2 = 0.f;

                    m_bFastComboSlash = true;
                    m_iActionState = RandomNormal3ComboStartAttack();
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                    m_fAnimSpeed = m_fF1Middle2SlashSpeed;
                }
            }
            // 참격 3회
            if (m_bSkill5State[3])
            {
                //m_fCalculateTime2 += _fTimeDelta;

                //if (m_fCalculateTime2 > m_fSummonIntervel)
                //{
                //    m_fCalculateTime2 = 0.f;
                //    //ActiveDashThroughShadow(1);
                //    m_fAnimSpeed = 2.f;
                //}
                //
                if (Update3ComboAttackState(_fTimeDelta))
                {
                    m_fAnimSpeed = 1.f;
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_bFastComboSlash = false;
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.5f)
                    {
                        ResetTimer();
                        // 위로 점프
                        m_fAnimSpeed = 1.f;
                        m_bSkill5State[3] = false;
                        m_bSkill5State[4] = true;
                        m_iActionState = RK_A_SPECIAL1;
                        m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNomalJumpChangeTime);
                        
                        UpdateDirection();
                        // 점프 방향, 속도 설정
                        m_vMovePos = m_vTargetPos;
                        UpdateSpecial1DirSpeed();
                    }
                }
            }
            // 장판찍기 이후 차징 참격
            if (m_bSkill5State[4])
            {
                if (m_iActionState == RK_A_SPECIAL1)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.5f && !m_bJumpAttack1EffectOn)
                    {
                        GAMEINSTANCE->PlaySoundW("Final_JumpAttack", m_fVoiceSize);
                        EFFECTMGR->PlayEffect("Boss_Special1", shared_from_this());
                        m_bJumpAttack1EffectOn = true;
                    }

                    if (m_fCalculateTime > m_fJumpAttackEndTime && !m_bJumpAttackSoundStart)
                    {
                        GAMEINSTANCE->PlaySoundW("Final_SFX_JumpAtt1", m_fEffectSize);
                        m_bJumpAttackSoundStart = true;
                    }

                    if ((m_fCalculateTime > (m_fSpecial1JumpStartTime + m_fIn1FSkill2SpecialAnimTime)) && (m_fCalculateTime < (m_fSpecial1JumpEndTime + m_fIn1FSkill2SpecialAnimTime)))
                    {
                        MovePos(m_vJumpNormalMoveDir * _fTimeDelta * m_fJumpMoveSpeed + m_vCurrentPos);
                    }

                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_bJumpAttackSoundStart = false;
                        m_bJumpAttack1EffectOn = false;
                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    }
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.1f)
                    {
                        UpdateDirection();
                        ResetTimer();
                        m_iActionState = RK_A_CHARGESLASH;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                        GAMEINSTANCE->PlaySoundW("Final_ChargeSlash", m_fVoiceSize);
                    }
                }
                
                if (m_iActionState == RK_A_CHARGESLASH)
                {
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime < m_fChargeSlashStartSlashTime + 0.f)
                    {
                        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    }

                    // 거대 참격 소환
                    if (m_fCalculateTime > (m_fChargeSlashStartSlashTime + 0.f) && !m_bSlashGen)
                    {
                        m_bSlashGen = true;
                        ActiveSlash(SLASHBIG);
                    }

                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        ResetTimer();
                        m_bSlashGen = false;
                        m_bSkill5State[4] = false;
                        m_bSkill5State[5] = true;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        UpdateDirection();
                    }
                }
            }
            // 점프이동
            if (m_bSkill5State[5])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;

                    if (m_fCalculateTime > 0.2f)
                    {
                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_SHORTJUMP;
                        m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNomalJumpChangeTime);
                        // 점프 방향, 속도 설정
                        m_vMovePos = CalculateDodgePos();
                        UpdateJumpDirSpeed();
                    }
                }
                if (m_iActionState == RK_A_SHORTJUMP)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if ((m_fCalculateTime > (m_fFastJumpStartTime + m_fAnimNomalJumpChangeTime)) && (m_fCalculateTime < (m_fFastJumpEndTime + m_fAnimNomalJumpChangeTime)))
                    {
                        MovePos(m_vJumpNormalMoveDir * _fTimeDelta * m_fJumpMoveSpeed + m_vCurrentPos);
                    }
                    if (m_fCalculateTime > m_fFastJumpEndTime)
                    {
                        m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    }
                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        m_fCalculateTime = 0.f;
                        m_bSkill5State[5] = false;
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNormalAltChangeTime, true);
                        UpdateDirection();
                        UpdateDistanceState();
                        m_iSkillState = RK_S_NOSKILL;
                    }
                }
            }
            break;
        }
        break;
    case RK_D_OUTRANGE:
        if (m_iActionState == RK_A_ALT)
        {
            // 패턴 잡기
            if (m_iSkillState == RK_S_NOSKILL)
            {
                switch (m_iRangeFurySkillOrder[m_iRangePatternNum])
                {
                case RK_S_OUTSKILL1:
                    m_iSkillState = RK_S_OUTSKILL1;
                    m_bRaSkill1State[0] = true;
                    break;
                case RK_S_OUTSKILL2:
                    m_iSkillState = RK_S_OUTSKILL2;
                    m_bRaSkill2State[0] = true;
                    break;
                }
                m_iRangePatternNum++;
                CheckGimicPattern2();
            }
        }
        switch (m_iSkillState)
        {
        case RK_S_OUTSKILL1:
            // 공중으로 점프 후 마법 시전
            if (m_bRaSkill1State[0])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.5f)
                    {
                        SimulationOff();
                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_TELEPORT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        EFFECTMGR->PlayEffect("Boss_Teleport", shared_from_this());
                        GAMEINSTANCE->PlaySoundW("Final_SFX_Teleport", m_fEffectSize);
                    }
                }
                if (m_iActionState == RK_A_TELEPORT)
                {
                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        MovePos(m_vCenterPos);
                        UpdateDirection();
                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                        m_bRaSkill1State[0] = false;
                        m_bRaSkill1State[6] = true;
                    }
                }
            }
            if (m_bRaSkill1State[6])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 1.f)
                    {
                        m_fCalculateTime = 0.f;
                        m_iActionState = RK_A_GOUP;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.f);
                    }
                }
                if (m_iActionState == RK_A_GOUP)
                {
                    if (m_pModelCom->GetIsFinishedAnimation())
                    {
                        UpdateDirection();
                        SimulationOff();
                        m_pTransformCom->SetPositionParam(1, m_fDashUpDistance + m_fGroundPosY);
                        m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
                        m_iActionState = RK_A_WAIT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.f, true);
                        m_bRaSkill1State[6] = false;
                        m_bRaSkill1State[1] = true;
                        m_fCalculateTime2 = 0.f;
                    }
                }
            }
            // 등 뒤를 도는 마법구 시전, 주기적으로 그림자 소환
            if (m_bRaSkill1State[1])
            {
                if (m_iActionState == RK_A_WAIT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 1.f && !m_bBindCircleMagicStart)
                    {
                        m_bBindCircleMagicStart = true;
                        m_fCalculateTime = 0.f;
                    }
                    if (m_fCalculateTime > m_fBindIntervel && !m_bBindCircleMagicStop && m_bBindCircleMagicStart)
                    {
                        m_fCalculateTime = 0.f;
                        ActiveBindMagic(BM_T_SPINBACK, m_iBindMagicGenNum, m_fOutRange2ReadyTime - m_fBindIntervel * (m_iBindMagicGenNum + 1) + 0.5f * m_iBindMagicGenNum);
                        m_iBindMagicGenNum++;
                        if (m_iBindMagicGenNum >= 5)
                        {
                            m_bBindCircleMagicStop = true;
                            m_iBindMagicGenNum = 0;
                        }
                    }

                    //m_fCalculateTime2 += _fTimeDelta;

                    if (m_bBindCircleMagicStop && !m_bOutSkill4ShadowStart)
                    {
                        m_bOutSkill4ShadowStart = true;
                    }

                    if (m_bOutSkill4ShadowStart)
                    {
                        m_fCalculateTime2 += _fTimeDelta;
                    }

                    // 1차 그림자 조합
                    if (m_fCalculateTime2 > 2.f && !m_bOutSkill4Summon[0])
                    {
                        m_bOutSkill4Summon[0] = true;
                        Active2ComboShadow(1);
                        ActiveJumpAttackShadow(1);
                    }
                    if (m_fCalculateTime2 > 3.f && !m_bOutSkill4Summon[1])
                    {
                        m_bOutSkill4Summon[1] = true;
                        ActiveDashThroughShadow(1);
                    }
                    // 2차 그림자 조합
                    if (m_fCalculateTime2 > 7.f && !m_bOutSkill4Summon[2])
                    {
                        m_bOutSkill4Summon[2] = true;
                        ActiveDashThroughShadow(1);
                        ActiveBindGroundShadow(1);
                    }
                    if (m_fCalculateTime2 > 9.f && !m_bOutSkill4Summon[3])
                    {
                        m_bOutSkill4Summon[3] = true;
                        ActiveQuakeShadow(1);
                    }
                    // 3차 그림자 조합
                    if (m_fCalculateTime2 > 12.f && !m_bOutSkill4Summon[4])
                    {
                        m_bOutSkill4Summon[4] = true;
                        ActiveSlashShadow(2);
                    }
                    if (m_fCalculateTime2 > 14.f && !m_bOutSkill4Summon[5])
                    {
                        m_bOutSkill4Summon[5] = true;
                        ActiveBindGroundShadow(1);
                    }
                    // 마법 발사
                    if (m_fCalculateTime2 > 16.3f)
                    {
                        m_iActionState = RK_A_MAGIC;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                        m_bOutSkill4ShadowStart = false;
                        m_bBindCircleMagicStop = false;
                        GAMEINSTANCE->PlaySoundW("Final_BindMagic", m_fVoiceSize);
                    }
                }
                if (m_iActionState == RK_A_MAGIC)
                {
                    m_fCalculateTime3 += _fTimeDelta;
                    if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime3 > 0.1f)
                    {
                        ResetTimer();
                        m_bBindCircleMagicStart = false;
                        for (int i = 0; i < 6; i++)
                            m_bOutSkill4Summon[i] = false;

                        m_iActionState = RK_A_ALT;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    }
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 3.f)
                    {
                        ResetTimer();
                        m_fDashBeforeY = m_pTarget->GetTransform()->GetState(CTransform::STATE_POSITION).y;
                        m_fDashDistanceY = m_fDashUpDistance + m_fGroundPosY - m_fDashBeforeY;
                        m_pTransformCom->SetPositionParam(1, m_fDashUpDistance + m_fGroundPosY);
                        m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
                        m_iActionState = RK_A_DASHSTART;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                        m_fAnimSpeed = 0.5f;
                        m_bRaSkill1State[1] = false;
                        m_bRaSkill1State[2] = true;
                        SimulationOn();
                    }
                }
            }
            // 공중대쉬
            if (m_bRaSkill1State[2])
            {
                if(UpdateJumpUpDashState(_fTimeDelta))
                { 
                    UpdateDirection();
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                    m_bRaSkill1State[2] = false;
                    m_bRaSkill1State[3] = true;
                }
            }
            // 프론트대쉬준비
            if (m_bRaSkill1State[3])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    //m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime2 += _fTimeDelta;
                    if (m_fCalculateTime2 > m_fOutSkill4StandTime)
                    {
                        m_fCalculateTime = 0.f;
                        m_fCalculateTime2 = 0.f;
                        m_bRaSkill1State[3] = false;
                        m_bRaSkill1State[4] = true;
                        m_iActionState = RK_A_DASHSTART;
                        m_fAnimSpeed = 0.5f;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        UpdateDirection();
                        SetWeaponRimLight(true);
                    }
                }
            }
            // 프론트대쉬해서 찌르기
            if (m_bRaSkill1State[4])
            {
                if (UpdateFrontDashState(_fTimeDelta))
                {
                    // 패링되었으면
                    if (m_iBaseState == RK_B_BLOCKED)
                    {
                        m_bRaSkill1State[4] = false;
                        m_fAnimSpeed = 1.f;
                    }
                    else
                    {
                        m_iActionState = RandomNormal3ComboStartAttack();
                        m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNormalAttackStartTime);
                        m_bRaSkill1State[4] = false;
                        m_bRaSkill1State[5] = true;
                        m_fAnimSpeed = 2.f;
                        UpdateDirection();
                    }
                }
            }
            if (m_bRaSkill1State[5])
            {
                if (Update3ComboAttackState(_fTimeDelta))
                {
                    // 패링되었으면
                    if (m_iBaseState == RK_B_BLOCKED)
                    {
                        m_bRaSkill1State[5] = false;
                    }
                    m_fAnimSpeed = 1.f;
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_bRaSkill1State[5] = false;
                    UpdateDistanceState();
                    m_iSkillState = RK_S_NOSKILL;
                }
            }
            break;
        case RK_S_OUTSKILL2:
            // 빠지는 대쉬
            if (m_bRaSkill2State[0])
            {
                if (m_iActionState == RK_A_ALT)
                {
                    m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.1f)
                    {
                        ResetTimer();
                        UpdateDirection();
                        m_iActionState = RK_A_DASHSTART;
                        m_fAnimSpeed = 0.5f;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.3f);

                        m_vDashOutPos = RandomDashOutPos(20.f);
                        m_vDashOutDir = m_vDashOutPos - m_vCurrentPos;
                    }
                }
                if (UpdatePositionDashState(_fTimeDelta))
                {
                    m_fDashDistanceXZ = m_fDistanceToTarget + 25.f;
                    m_fDashTime = m_fDashDistanceXZ / 100.f;
                    m_vDashDir = m_vNormTargetDirXZ;

                    m_bRaSkill2State[0] = false;
                    m_bRaSkill2State[1] = true;
                    GAMEINSTANCE->PlaySoundW("Final_SFX_Dash2", m_fEffectSize);
                }
            }
            // 관통대쉬
            if (m_bRaSkill2State[1])
            {
                if (UpdateAThroughDashState(_fTimeDelta))
                {
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.1f)
                    {
                        m_fCalculateTime = 0.f;
                        m_bRaSkill2State[1] = false;
                        m_bRaSkill2State[2] = true;
                        m_iActionState = RK_A_DASHSTART;
                        m_fAnimSpeed = 0.5f;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        UpdateDirection();

                        m_vDashOutPos = RandomDashOutPos(20.f);
                        m_vDashOutDir = m_vDashOutPos - m_vCurrentPos;
                    }
                }
            }
            // 빠지는 대쉬
            if (m_bRaSkill2State[2])
            {
                if (UpdatePositionDashState(_fTimeDelta))
                {
                    m_fDashDistanceXZ = m_fDistanceToTarget + 25.f;
                    m_fDashTime = m_fDashDistanceXZ / 100.f;
                    m_vDashDir = m_vNormTargetDirXZ;
                    SetUseMotionTrail(true, true, m_fDashTime, true, m_vThroughDashColor);

                    m_bRaSkill2State[2] = false;
                    m_bRaSkill2State[3] = true;
                    GAMEINSTANCE->PlaySoundW("Final_SFX_Dash2", m_fEffectSize);
                }
            }
            // 관통대쉬
            if (m_bRaSkill2State[3])
            {
                if (UpdateAThroughDashState(_fTimeDelta))
                {
                    m_iActionState = RK_A_ALT;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_fCalculateTime += _fTimeDelta;
                    if (m_fCalculateTime > 0.1f)
                    {
                        m_fCalculateTime = 0.f;
                        m_bRaSkill2State[3] = false;
                        m_bRaSkill2State[4] = true;
                        m_iActionState = RK_A_DASHSTART;
                        m_fAnimSpeed = 0.5f;
                        m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                        UpdateDirection();
                        SetWeaponRimLight(true);
                    }
                }
            }
            // 프론트대쉬해서 찌르기
            if (m_bRaSkill2State[4])
            {
                if (UpdateFrontDashState(_fTimeDelta, 100.f, 0.f))
                {
                    // 패링되었으면
                    if (m_iBaseState == RK_B_BLOCKED)
                    {
                        m_bRaSkill2State[4] = false;
                        m_fAnimSpeed = 1.f;
                    }
                    else
                    {
                        m_iActionState = RandomNormal2ComboStartAttack();
                        m_pModelCom->ChangeAnim(m_iActionState, m_fAnimNormalAttackStartTime);
                        m_bRaSkill2State[4] = false;
                        m_bRaSkill2State[5] = true;
                        m_fAnimSpeed = 2.f;
                        UpdateDirection();
                        ResetTimer();
                    }
                }
            }
            // 2콤보
            if (m_bRaSkill2State[5])
            {
                if (Update2ComboAttackState(_fTimeDelta))
                {
                    // 패링되었으면
                    if (m_iBaseState == RK_B_BLOCKED)
                    {
                        m_bRaSkill2State[5] = false;
                    }

                    m_fAnimSpeed = 1.f;
                }
                if (m_iActionState == RK_A_ALT)
                {
                    m_bRaSkill2State[4] = false;
                    UpdateDistanceState();
                    m_iSkillState = RK_S_NOSKILL;
                }
            }
            break;
        }
        break;
    }
}

void CRedKnight::FSM_GimicPattern2(_float _fTimeDelta)
{
    // 텔레포트로 단상 위로 가기
    if (m_bGP2SkillState[0])
    {
        if (m_iActionState == RK_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;

            if (m_fCalculateTime > 1.f)
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_TELEPORT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                EFFECTMGR->PlayEffect("Boss_Teleport", shared_from_this());
                GAMEINSTANCE->PlaySoundW("Final_SFX_Teleport", m_fEffectSize);
            }
        }
        if (m_iActionState == RK_A_TELEPORT)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                MovePos(m_vFrontThronePos);
                m_fCalculateTime = 0.f;
                m_bGP2SkillState[0] = false;
                //m_bGP2SkillState[9] = true;
                m_bGP2SkillState[1] = true;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.2f, true);
                m_pTransformCom->LookAtDir(_float3(m_vCenterPos.x, 0.f, m_vCenterPos.z) -
                    _float3(m_vCurrentPos.x, 0.f, m_vCurrentPos.z));
                vector<string> vecSpawnSword;
                vecSpawnSword.emplace_back("FinalBossSpawnSword1");
                vecSpawnSword.emplace_back("FinalBossSpawnSword2");
                vecSpawnSword.emplace_back("FinalBossSpawnSword3");
                vecSpawnSword.emplace_back("FinalBossSpawnSword4");
                CCameraMgr::GetInstance()->FilmCameraTurnOn(vecSpawnSword);
            }
        }
    }
    // 오크, 고블린 필드보스 그림자 소환
    //if (m_bGP2SkillState[9])
    //{
    //    if (m_iActionState == RK_A_ALT)
    //    {
    //        m_fCalculateTime += _fTimeDelta;

    //        if (m_fCalculateTime > 5.f)
    //        {
    //            m_fAnimSpeed = 0.5f;
    //            m_iActionState = RK_A_SUMMONSHADOW;
    //            m_pModelCom->ChangeAnim(m_iActionState, 0.5f, false);
    //            m_fCalculateTime = 0.f;
    //        }
    //    }
    //    if (m_iActionState == RK_A_SUMMONSHADOW)
    //    {
    //        if (m_pModelCom->GetIsFinishedAnimation())
    //        {
    //            m_fAnimSpeed = 1.f;

    //            ActiveMidgetBerserkerShadow();
    //            ActiveDarkOrcShadow();

    //            m_iActionState = RK_A_WAIT;
    //            m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
    //        }
    //    }
    //    if (m_iActionState == RK_A_WAIT)
    //    {
    //        // 필드보스 유령 필드보스들이 다 죽으면
    //        if (m_vecMidgetBerserkerShadow[0]->GetDead() && m_vecOrcDarkCommanderShadow[0]->GetDead())
    //        {
    //            m_iActionState = RK_A_ALT;
    //            m_pModelCom->ChangeAnim(m_iActionState, 0.2f, true);
    //            m_bGP2SkillState[9] = false;
    //            m_bGP2SkillState[1] = true;
    //        }
    //    }
    //}
    // 단상위에서 검 떨어뜨리기 모션후에 검 떨어지기
    if (m_bGP2SkillState[1])
    {
        if (m_iActionState == RK_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;

            if (m_fCalculateTime > 3.f)
            {
                m_fCalculateTime = 0.f;
                m_fAnimSpeed = 0.5f;
                m_iActionState = RK_A_SUMMON;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
                EFFECTMGR->PlayEffect("Boss_BeforeSword", shared_from_this());
            }
        }
        if (m_iActionState == RK_A_SUMMON)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > m_fFinalRoarEndTime && !m_bFinalRoarSoundStart)
            {
                m_bFinalRoarSoundStart = true;
                GAMEINSTANCE->PlaySoundW("Final_Roar", m_fVoiceSize);
            }
            
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_bFinalRoarSoundStart = false;
                m_fAnimSpeed = 1.f;
                // 검떨어뜨리기
                ActiveSword();
                m_fCalculateTime = 0.f;
                m_bGP2SkillState[1] = false;
                m_bGP2SkillState[2] = true;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
            }
        }
    }
    // 검떨어질때 까지 가만히 있다가 분신소환 모션과 함께 분신소환
    if (m_bGP2SkillState[2])
    {
        if (m_iActionState == RK_A_ALT)
        {
            m_fCalculateTime += _fTimeDelta;

            if (m_fCalculateTime > 5.f)
            {
                m_fAnimSpeed = 0.5f;
                m_iActionState = RK_A_SUMMONSHADOW;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
                GAMEINSTANCE->PlaySoundW("Final_SummonShadow3", m_fVoiceSize);
                m_fCalculateTime = 0.f;
            }
        }
        if (m_iActionState == RK_A_SUMMONSHADOW)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fAnimSpeed = 1.f;
                ActiveGimicShadow4();

                m_iActionState = RK_A_WAIT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
                m_bGP2SkillState[2] = false;
                m_bGP2SkillState[3] = true;
            }
        }
    }
    // 서서 검 하나 빛나게하기
    if (m_bGP2SkillState[3])
    {
        // 주기적으로 방해그림자 소환
        m_fCalculateTime2 += _fTimeDelta;
        if (m_fCalculateTime2 > m_fGimicDisruptShadowTime)
        {
            m_fCalculateTime2 = 0.f;
            switch (m_iGimic1ShadowOrder[m_iGimic1ShadowNum])
            {
            case 0:
                ActiveQuakeShadow(1, 1);
                break;
            case 1:
                ActiveJumpAttackShadow(1, 1);
                break;
            case 2:
                ActiveDashThroughShadow(1, 1);
                break;
            }
            m_iGimic1ShadowNum++;
        }

        if (m_bBeforeTargetSword)
        {
            m_fCalculateTime += _fTimeDelta;
            if (m_fCalculateTime > 3.f)
            {
                m_fCalculateTime = 0.f;
                switch (m_iTargetCount)
                {
                    // 중앙
                case 0:
                    m_iTargetSwordIndex = 4;
                    break;
                    // 좌상단
                case 1:
                    m_iTargetSwordIndex = 1;
                    break;
                    // 우하단
                case 2:
                    m_iTargetSwordIndex = 3;
                    break;
                }
                m_vecFallingSword[m_iTargetSwordIndex]->SetTargetOn(true);
                //m_vecFallingSword[m_iTargetSwordIndex]->UseGlow();
                m_vecFallingSword[m_iTargetSwordIndex]->PlayEffect("Boss_FallingSword");
                m_bBeforeTargetSword = false;
            }
        }
        else
        {
            //  빛나는 검이 체력이 다 닳으면
            if (m_vecFallingSword[m_iTargetSwordIndex]->GetTargetOut())
            {
                m_iTargetCount++;
                // 3번 다 채우면 스턴으로
                if (m_iTargetCount >= m_iBTargetCount)
                {
                    m_bGP2SkillState[3] = false;
                    m_bGP2SkillState[4] = true;
                    m_iTargetCount = 0;
                    m_iActionState = RK_A_STUN;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.3f, true);
                }
                // 아직 못채웠으면
                else
                {
                    m_bBeforeTargetSword = true;
                }
            }
        }
    }
    // 활성화된 칼들 들어올리기, 다른 검들은 올라가버리게 그림자들 사망
    if (m_bGP2SkillState[4])
    {
        vector<string> vecFallingSword;
        vecFallingSword.emplace_back("FinalBossFallingSword1");
        vecFallingSword.emplace_back("FinalBossFallingSword2");
        vecFallingSword.emplace_back("FinalBossFallingSword3");
        vecFallingSword.emplace_back("FinalBossFallingSword4");
        vecFallingSword.emplace_back("FinalBossFallingSword5");
        CCameraMgr::GetInstance()->FilmCameraTurnOn(vecFallingSword);
        for (int i = 0; i < m_iRedKnightGimicShadowCount; i++)
        {
            m_vecRedKnightShadow[i]->SetDead(true);
        }

        for (int i = 0; i < 5; i++)
        {
            if (m_vecFallingSword[i]->GetTargetOut())
            {
                m_vecFallingSword[i]->AttackOn();
            }
            else
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_SwordUp", m_fEffectSize);
                m_vecFallingSword[i]->SetRise(true);
            }
        }
        m_bGP2SkillState[4] = false;
        m_bGP2SkillState[5] = true;
    }
    // 칼들에 데미지 받으면 피해받기, 마지막으로 맞은중앙 칼에 앞으로 넘어지기
    if (m_bGP2SkillState[5])
    {
        for (int i = 0; i < 5; i++)
        {
            if (m_vecFallingSword[i]->GetFallGround())
            {
                // 3방 맞아서 40퍼에서 10퍼되기
                m_fHp -= m_fSwordPower;
                m_vecFallingSword[i]->SetFallGround(false);
                m_vecFallingSword[i]->SetTargetOut(false);
                GAMEINSTANCE->PlaySoundW("Final_SFX_SwordHit", m_fEffectSize);
                GAMEINSTANCE->PlaySoundW("Final_HardHitted", m_fVoiceSize);
            }
        }
        // 날라가기
        if (m_fHp <= m_fFury2Hp)
        {
            m_iActionState = RK_A_FRONTDOWN1;
            m_fAnimSpeed = 1.f;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
            m_bGP2SkillState[5] = false;
            m_bGP2SkillState[6] = true;
            SimulationOn();
        }
    }
    // 앞으로 날라 왕좌 단상에서 떨어지기
    if (m_bGP2SkillState[6])
    {
        if (m_iActionState == RK_A_FRONTDOWN1)
        {
            // 앞으로 떨어지기
            MovePos(_fTimeDelta * 20.f * _float3(0.f, 0.f, -1.f) + m_vCurrentPos);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = RK_A_FRONTDOWN2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            }
        }
        if (m_iActionState == RK_A_FRONTDOWN2)
        {
            m_fCalculateTime += _fTimeDelta;

            if (m_fCalculateTime > m_fGP1DownTime)
            {
                m_iActionState = RK_A_FRONTDOWN3;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
            }
        }
        // 떨어지고 나서
        if (m_iActionState == RK_A_FRONTDOWN3)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fHp = m_fFury2Hp;
                m_iBaseState = RK_B_WHILEFURY2;
                m_iActionState = RK_A_STUN;
                m_pModelCom->ChangeAnim(m_iActionState, 0.5f, true);
                ResetTimer();
                m_i2ComboType = 10;
                m_i3ComboType = 10;
                m_fAnimSpeed = 1.f;
                m_bBeforeDead = true;
                m_bGP2SkillState[6] = false;
                m_iPhase = RK_P_FURY2;
            }
        }
    }
}

void CRedKnight::FSM_WhileFury2(_float _fTimeDelta)
{
    CheckDead();
}

void CRedKnight::FSM_Fury2(_float _fTimeDelta)
{
}

void CRedKnight::FSM_Blocked(_float _fTimeDelta)
{
    if (m_iActionState == RK_A_BLOCKED)
    {
        m_fCalculateTime2 += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime2 > 0.3f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            if (m_bFury)
            {
                m_iBaseState = RK_B_FURY;
            }
            else
                m_iBaseState = RK_B_NORMAL;
            m_iActionState = RK_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            UpdateDistanceState();
            //UpdateDirection();
            m_iSkillState = RK_S_NOSKILL;
        }
    }

    // 긴 Blocked 상태에서 공격들어온 종류 체크
    if (m_iActionState == RK_A_BLOCKED && m_iParriedPower == PARRIEDSTRONG)
    {
        CheckAttacked();
    }

    // 사망
    CheckDead();
}

void CRedKnight::FSM_Hitted(_float _fTimeDelta)
{
    if (m_iActionState == RK_A_HITTED)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bHitted)
        {
            m_fCalculateTime = 0.f;
            UpdateDirection();
            m_bHitted = false;
            m_iActionState = RK_A_HITTED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.2f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            if (m_bFury)
            {
                m_iBaseState = RK_B_FURY;
            }
            else
                m_iBaseState = RK_B_NORMAL;
            m_iActionState = RK_A_ALT;
            UpdateDistanceState();
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = RK_S_NOSKILL;
            m_bHitted = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void CRedKnight::FSM_Hitted2(_float _fTimeDelta)
{
    if (m_iActionState == RK_A_HITTED)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bHitted2)
        {
            m_fCalculateTime = 0.f;
            //UpdateDirection();
            m_bHitted2 = false;
            m_iActionState = RK_A_HITTED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.2f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            if (m_bFury)
            {
                m_iBaseState = RK_B_FURY;
            }
            else
                m_iBaseState = RK_B_NORMAL;
            m_iActionState = RK_A_ALT;
            UpdateDistanceState();
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = RK_S_NOSKILL;
            m_bHitted2 = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void CRedKnight::FSM_Hitted3(_float _fTimeDelta)
{
    if (m_iActionState == RK_A_HITTED)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bHitted3)
        {
            m_fCalculateTime = 0.f;
            UpdateDirection();
            m_bHitted3 = false;
            m_iActionState = RK_A_HITTED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.2f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            if (m_bFury)
            {
                m_iBaseState = RK_B_FURY;
            }
            else
                m_iBaseState = RK_B_NORMAL;
            m_iActionState = RK_A_ALT;
            UpdateDistanceState();
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = RK_S_NOSKILL;
            m_bHitted3 = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void CRedKnight::FSM_Stun(_float _fTimeDelta)
{
    if (m_iActionState == RK_A_STUN)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bStun)
        {
            m_fCalculateTime = 0.f;
            UpdateDirection();
            m_bStun = false;
            m_iActionState = RK_A_STUN;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.2f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            if (m_bFury)
            {
                m_iBaseState = RK_B_FURY;
            }
            else
                m_iBaseState = RK_B_NORMAL;
            m_iActionState = RK_A_ALT;
            UpdateDistanceState();
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = RK_S_NOSKILL;
            m_bStun = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void CRedKnight::FSM_Shocked(_float _fTimeDelta)
{
    if (m_iActionState == RK_A_SHOCKED)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bShocked)
        {
            m_fCalculateTime = 0.f;
            UpdateDirection();
            m_bShocked = false;
            m_iActionState = RK_A_SHOCKED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.2f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            if (m_bFury)
            {
                m_iBaseState = RK_B_FURY;
            }
            else
                m_iBaseState = RK_B_NORMAL;
            m_iActionState = RK_A_ALT;
            UpdateDistanceState();
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = RK_S_NOSKILL;
            m_bShocked = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void CRedKnight::FSM_Down(_float _fTimeDelta)
{
    if (m_iActionState == RK_A_DOWN1)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_bDown)
        {
            UpdateDirection();
            m_fCalculateTime = 0.f;
            m_bDown = false;
            m_iActionState = RK_A_DOWN1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iActionState = RK_A_DOWN2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == RK_A_DOWN2)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_iActionState = RK_A_DOWN3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == RK_A_DOWN3)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            if (m_bFury)
            {
                m_iBaseState = RK_B_FURY;
            }
            else
                m_iBaseState = RK_B_NORMAL;
            m_iActionState = RK_A_ALT;
            UpdateDistanceState();
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            m_iSkillState = RK_S_NOSKILL;
            m_bDown = false;
        }
    }

    // 공격들어온 종류 체크
    CheckAttacked();
    // 사망
    CheckDead();
}

void CRedKnight::FSM_Death(_float _fTimeDelta)
{
    if ((m_iActionState == RK_A_DEATH) && (m_pModelCom->GetIsFinishedAnimation()))
    {
        m_iActionState = RK_A_DIE;
        //m_pModelCom->ChangeAnim(m_iActionState, 0.01f, false);

        SetDissolve(true, true, 5.f);
        SetDissolveInfo(1.f, m_vDisolveColor, "Noise_3003");

        m_bDeadStart = true;
        m_fCalculateTime2 = 0.f;
    }
    if (m_bDeadStart)
    {
        m_fCalculateTime2 += _fTimeDelta;
        if (m_fCalculateTime2 > 5.f)
        {
            m_fCalculateTime2 = 0.f;
            m_bDeadStart = false;

            m_IsEnabled = false;
        }
    }
}

HRESULT CRedKnight::AddWeaponTrail()
{
    m_pWeaponTrail = CPointTrail::Create("Trail");
    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_Effect"), m_pWeaponTrail)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRedKnight::AddMotionTrail(_int _iTrailCnt, _float3 _vTrailColor, _float _fTrailCntPerTick)
{
    if (0 >= _iTrailCnt)
        return E_FAIL;

    CMotionTrail::MOTIONTRAIL_DESC MotionTrail_Desc = {};
    MotionTrail_Desc.pModel = m_pModelCom;
    MotionTrail_Desc.iSize = _iTrailCnt;
    MotionTrail_Desc.fLifeTime = 0.3f;
    MotionTrail_Desc.vColor = _vTrailColor;
    MotionTrail_Desc.fTrailCntPerTick = _fTrailCntPerTick;

    m_pMotionTrail = CMotionTrail::Create();

    m_pMotionTrail->SettingMotionTrail(MotionTrail_Desc, shared_from_this());

    if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, TEXT("Layer_Effect"), m_pMotionTrail)))
        return E_FAIL;

    m_pMotionTrail->SetEnable(false);

    return S_OK;
}

HRESULT CRedKnight::SetUseMotionTrail(_bool _bUseMotionTrail, _bool _bUseTimer, _float _fTimer, _bool _bChangeColor, _float4 _vColor)
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

HRESULT CRedKnight::ChangeTrailType()
{
    return E_NOTIMPL;
}

void CRedKnight::UpdateAttackDir(_float _fTimeDelta)
{
    if (m_fDistanceToTarget < 10.f)
    {
        switch (m_iActionState)
        {
        case RK_A_ATT_D1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 720.f * _fTimeDelta);
            break;
        case RK_A_ATT_D3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 720.f * _fTimeDelta);
            break;
        case RK_A_ATT_L1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 360.f * _fTimeDelta);
            break;
        case RK_A_ATT_L3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 360.f * _fTimeDelta);
            break;
        case RK_A_ATT_LM1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 360.f * _fTimeDelta);
            break;
        case RK_A_ATT_U1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 360.f * _fTimeDelta);
            break;
        case RK_A_ATT_U4:
            m_pTransformCom->TurnToDir(m_vTargetDir, 360.f * _fTimeDelta);
            break;
        }
    }
    else
    {
        switch (m_iActionState)
        {
        case RK_A_ATT_D1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RK_A_ATT_D3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RK_A_ATT_L1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RK_A_ATT_L3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RK_A_ATT_LM1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RK_A_ATT_U1:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RK_A_ATT_U4:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        }
    }
}

void CRedKnight::UpdateSlashDir(_float _fTimeDelta)
{
    if (m_fDistanceToTarget < 20.f)
    {
        switch (m_iActionState)
        {
        case RK_A_ATT_D3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case RK_A_ATT_D4:
            //m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RK_A_ATT_L3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case RK_A_ATT_L4:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RK_A_ATT_U4:
            m_pTransformCom->TurnToDir(m_vTargetDir, 180.f * _fTimeDelta);
            break;
        case RK_A_ATT_U5:
            //m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        }
    }
    else
    {
        switch (m_iActionState)
        {
        case RK_A_ATT_D3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RK_A_ATT_D4:
            //m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RK_A_ATT_L3:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RK_A_ATT_L4:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RK_A_ATT_U4:
            m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        case RK_A_ATT_U5:
            //m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
            break;
        }
    }
}

void CRedKnight::UpdateMoveSlide(_float _fSlideStart, _float _fSlideEnd, _float _fSpeed, _float _fTimeDelta)
{
    if (m_iSkillState == RK_S_OUTSKILL3 || m_bFastComboSlash)
    {
        return;
    }
    if ((m_fCalculateTime > (_fSlideStart / m_fAnimSpeed)) && (m_fCalculateTime < (_fSlideEnd / m_fAnimSpeed)))
    {
        MovePos(m_vNormLookDirXZ * _fTimeDelta * _fSpeed + m_vCurrentPos);
    }
}

void CRedKnight::UpdateCirclePos(_bool _bDir, _float _fTimeDelta)
{
    _float fPosX = 0.f;
    _float fPosZ = 0.f;
    
    if (_bDir)
    {
        fPosX = m_fCircleRadius * cos(m_fIn2Skill2CircleRadian + m_fSpeed / (m_fCircleRadius)*m_fCalculateTime2);
        fPosZ = m_fCircleRadius * sin(m_fIn2Skill2CircleRadian + m_fSpeed / (m_fCircleRadius)*m_fCalculateTime2);
        MovePos(_float3(fPosX, 0.f, fPosZ) + m_vCenterPos);
    }
    else
    {
        fPosX = m_fCircleRadius * cos(m_fIn2Skill2CircleRadian - m_fSpeed / (m_fCircleRadius)*m_fCalculateTime2);
        fPosZ = m_fCircleRadius * sin(m_fIn2Skill2CircleRadian - m_fSpeed / (m_fCircleRadius)*m_fCalculateTime2);
        MovePos(_float3(fPosX, 0.f, fPosZ) + m_vCenterPos);
    }
}

_float3 CRedKnight::CalculateTeleportFrontPos()
{
    _float3 vTargetDir = m_vTargetPos - m_vCurrentPos;
    vTargetDir.Normalize();
    _float3 vPos = m_vTargetPos - vTargetDir * m_fMelee2Range;
    _float3 vReturnPos = CalculateBindary(vPos);
    
    return vReturnPos;
}

_float3 CRedKnight::CalculateTeleportBackPos()
{
    _float3 vTargetDir = m_vTargetPos - m_vCurrentPos;
    vTargetDir.Normalize();
    _float3 vPos = m_vTargetPos + vTargetDir * m_fMelee2Range;
    _float3 vReturnPos = CalculateBindary(vPos);

    return vReturnPos;
}

_float3 CRedKnight::CalculateTeleportFarBackPos()
{
    _float3 vTargetDir = m_vTargetPos - m_vCurrentPos;
    vTargetDir.Normalize();
    _float3 vPos = m_vTargetPos + vTargetDir * m_fMelee3Range;
    _float3 vReturnPos = CalculateBindary(vPos);

    return vReturnPos;
}

_float3 CRedKnight::CalculateCircleRunPos()
{
    _float fPosX = 0.f;
    _float fPosZ = 0.f;
    fPosX = m_fCircleRadius * cos(m_fIn2Skill2CircleRadian);
    fPosZ = m_fCircleRadius * sin(m_fIn2Skill2CircleRadian);
    _float3 fReturnPos = _float3(fPosX, 0.f, fPosZ) + m_vCenterPos;
    
    return fReturnPos;
}

_float3 CRedKnight::CalculateDodgePos()
{
    _float3 vGap = m_vTargetPos - m_vCurrentPos;
    _float3 vGapXZ = _float3(vGap.x, 0.f, vGap.z);
    vGapXZ.Normalize();
    _float3 vDodgePos = vGapXZ * m_vDodgeRange1 + m_vTargetPos;

    _float3 vReturnPos = CalculateBindary(vDodgePos);

    return vReturnPos;
}

_float3 CRedKnight::CalculateDodge2Pos()
{
    _float3 vGap = m_vTargetPos - m_vCurrentPos;
    _float3 vGapXZ = _float3(vGap.x, 0.f, vGap.z);
    vGapXZ.Normalize();
    _float3 vDodgePos = vGapXZ * m_vDodgeRange2 + m_vTargetPos;

    _float3 vReturnPos = CalculateBindary(vDodgePos);

    return vDodgePos;
}

void CRedKnight::UpdateJumpDirSpeed()
{
    _float3 vTargetDirXZ = _float3(m_vTargetDir.x, 0.f, m_vTargetDir.z);
    vTargetDirXZ.Normalize(m_vJumpNormalMoveDir);

    _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
    _float2 vMovePosXZ = _float2(m_vMovePos.x, m_vMovePos.z);
    _float fFastJumpDistance = (vCurrentPosXZ - vMovePosXZ).Length();

    //_float fFastJumpDistance = (m_vCurrentPos - m_vMovePos).Length();
    m_fJumpMoveSpeed = fFastJumpDistance / (m_fKeyFrameSpeed * 8);
}

_float CRedKnight::GetHPRatio()
{
    _float ratio = m_fHp / m_fMaxHp;

    return ratio;
}

void CRedKnight::UpdateJumpToThroneDirSpeed()
{
    _float3 vTargetDirXZ = _float3(m_vFrontThronePos.x, 0.f, m_vFrontThronePos.z) -
        _float3(m_vCurrentPos.x, 0.f, m_vCurrentPos.z);
    vTargetDirXZ.Normalize(m_vJumpNormalMoveDir);

    _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
    _float2 vMovePosXZ = _float2(m_vMovePos.x, m_vMovePos.z);
    _float fFastJumpDistance = (vCurrentPosXZ - vMovePosXZ).Length();
    
    //_float fFastJumpDistance = (m_vCurrentPos - m_vMovePos).Length();
    m_fJumpMoveSpeed = fFastJumpDistance / (m_fKeyFrameSpeed * 8);
}

void CRedKnight::UpdateSpecial1DirSpeed()
{
    _float3 vTargetDirXZ = _float3(m_vTargetDir.x, 0.f, m_vTargetDir.z);
    vTargetDirXZ.Normalize(m_vJumpNormalMoveDir);

    _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
    _float2 vMovePosXZ = _float2(m_vMovePos.x, m_vMovePos.z);
    _float fFastJumpDistance = (vCurrentPosXZ - vMovePosXZ).Length();

    //_float fFastJumpDistance = (m_vCurrentPos - m_vMovePos).Length();
    m_fJumpMoveSpeed = fFastJumpDistance / (m_fKeyFrameSpeed * 20);
}

void CRedKnight::UpdateSpecial2DirSpeed()
{
    _float3 vTargetDirXZ = _float3(m_vTargetCenterDir.x, 0.f, m_vTargetCenterDir.z);
    vTargetDirXZ.Normalize(m_vJumpNormalMoveDir);

    _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
    _float2 vMovePosXZ = _float2(m_vMovePos.x, m_vMovePos.z);
    _float fFastJumpDistance = (vCurrentPosXZ - vMovePosXZ).Length();

    //_float fFastJumpDistance = (m_vCurrentPos - m_vMovePos).Length();
    m_fJumpMoveSpeed = fFastJumpDistance / (m_fKeyFrameSpeed * 20);
}

_float3 CRedKnight::CalculateBindary(_float3 _vPos)
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

_int CRedKnight::CheckParryingStack(_int _iParryingStack)
{
    switch (_iParryingStack)
    {
    case 1:
        m_iParryType = 1;
        break;
    case 2:
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

_float3 CRedKnight::RandomJumpFrontOrBackPos()
{
    _int iRandom = GAMEINSTANCE->PickRandomInt(0, 2);
    switch (iRandom)
    {
    case 0:
        return CalculateTeleportFrontPos();
        break;
    case 1:
        return CalculateTeleportBackPos();
        break;
    }
    return { 0.0f,0.0f,0.0f };
}

_float3 CRedKnight::JumpFarBackPos()
{
    return CalculateTeleportFarBackPos();
}

_float3 CRedKnight::RandomBlinkPos()
{
    _float3 vTargetDir = _float3((float)GAMEINSTANCE->PickRandomInt(-100, 100), 0.f, (float)GAMEINSTANCE->PickRandomInt(-100, 100));
    vTargetDir.Normalize();

    _float3 vPos = m_vTargetPos + vTargetDir * m_fMelee2Range;
    _float3 vReturnPos = CalculateBindary(vPos);

    return vReturnPos;
}

_float3 CRedKnight::RandomBlinkSlashPos()
{
    _float3 vTargetDir = _float3((float)GAMEINSTANCE->PickRandomInt(-100, 100), 0.f, (float)GAMEINSTANCE->PickRandomInt(-100, 100));
    vTargetDir.Normalize();

    _float3 vPos = m_vTargetPos + vTargetDir * m_fBlinkSlashRange;
    _float3 vReturnPos = CalculateBindary(vPos);

    return vReturnPos;
}

_float3 CRedKnight::RandomBlinkDashPos()
{
    _float3 vTargetDir = _float3((float)GAMEINSTANCE->PickRandomInt(-100, 100), 0.f, (float)GAMEINSTANCE->PickRandomInt(-100, 100));
    vTargetDir.Normalize();

    _float3 vPos = m_vTargetPos + vTargetDir * m_fBlinkDashRange;
    _float3 vReturnPos = CalculateBindary(vPos);

    return vReturnPos;
}

_float3 CRedKnight::RandomDashOutPos(_float _fDashOutRange, _int _iType)
{
    _float3 vTargetDir;
    _float3 vPos;
    _float3 vReturnPos;
    _float  fDistance = 1000.f;
    m_fDashOutRange = _fDashOutRange;

    vTargetDir = _float3((float)GAMEINSTANCE->PickRandomInt(-100, 100), 0.f, (float)GAMEINSTANCE->PickRandomInt(-100, 100));
    vTargetDir.Normalize();
    if (_iType == 0)
    {
        // 플레이어와의 거리보다 짧은곳이어야 함
        while (fDistance > m_fDistanceToTarget)
        {
            vTargetDir = _float3((float)GAMEINSTANCE->PickRandomInt(-100, 100), 0.f, (float)GAMEINSTANCE->PickRandomInt(-100, 100));
            vTargetDir.Normalize();
            if (m_fDistanceToTarget > m_fDashOutRange)
            {
                vPos = m_vTargetPos + vTargetDir * m_fDashOutRange;
                vReturnPos = CalculateBindary(vPos);
                _float2 vPos1 = _float2(vReturnPos.x, vReturnPos.z);
                _float2 vPos2 = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
                fDistance = (vPos1 - vPos2).Length();
                // 너무 짧으면 안됨
                if (fDistance < 15.f)
                    fDistance = 1000.f;
            }
            else
            {
                vPos = m_vTargetPos + vTargetDir * 10.f;
                vReturnPos = CalculateBindary(vPos);
                _float2 vPos1 = _float2(vReturnPos.x, vReturnPos.z);
                _float2 vPos2 = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
                fDistance = (vPos1 - vPos2).Length();
                break;
            }
        }
    }
    else
    {
        while (1)
        {
            vTargetDir = _float3((float)GAMEINSTANCE->PickRandomInt(-100, 100), 0.f, (float)GAMEINSTANCE->PickRandomInt(-100, 100));
            vTargetDir.Normalize();
            vPos = m_vTargetPos + vTargetDir * m_fDashOutRange;
            vReturnPos = CalculateBindary(vPos);
            _float2 vPos1 = _float2(vReturnPos.x, vReturnPos.z);
            _float2 vPos2 = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
            fDistance = (vPos1 - vPos2).Length();
            if (fDistance > m_fDistanceToTarget)
                break;
        }
    }

    m_fDistanceToDashOut = fDistance;

    return vReturnPos;
}

_int CRedKnight::RandomSlowSlash()
{
    _int iReturnState = 0;
    switch (GAMEINSTANCE->PickRandomInt(0, 4))
    {
    case 0:
        iReturnState = RK_A_ATT_D3;
        break;
    case 1:
        iReturnState = RK_A_ATT_L3;
        break;
    case 2:
        iReturnState = RK_A_ATT_U4;
        break;
    case 3:
        iReturnState = RK_A_ATT_LM1;
        break;
    }
    return iReturnState;
}

_int CRedKnight::RandomNormal2ComboStartAttack()
{
    _int iReturnState = 0;
    switch (GAMEINSTANCE->PickRandomInt(0, 3))
    {
    case 0:
        iReturnState = RK_A_ATT_LM1;
        m_i2ComboType = 0;
        break;
    case 1:
        iReturnState = RK_A_ATT_L1;
        m_i2ComboType = 1;
        break;
    case 2:
        iReturnState = RK_A_ATT_U4;
        m_i2ComboType = 2;
        break;
    }
    return iReturnState;
}

_int CRedKnight::RandomNormal3ComboStartAttack()
{
    _int iReturnState = 0;
    switch (GAMEINSTANCE->PickRandomInt(0, 4))
    {
    case 0:
        iReturnState = RK_A_ATT_LM1;
        m_i3ComboType = 0;
        break;
    case 1:
        iReturnState = RK_A_ATT_L1;
        m_i3ComboType = 1;
        break;
    case 2:
        iReturnState = RK_A_ATT_U4;
        m_i3ComboType = 2;
        break;
    case 3:
        iReturnState = RK_A_ATT_D3;
        m_i3ComboType = 3;
        break;
    }
    return iReturnState;
}

_bool CRedKnight::Update2ComboAttackState(float _fTimeDelta)
{
    if ((m_iActionState == RK_A_ATT_L1 && m_i2ComboType == 1) || (m_iActionState == RK_A_ATT_LM1 && m_i2ComboType == 0)
        || (m_iActionState == RK_A_ATT_D3 && m_i2ComboType == 3) || (m_iActionState == RK_A_ATT_U4 && m_i2ComboType == 2))
    {
        m_fCalculateTime3 += _fTimeDelta;
    }
    UpdateAttackDir(_fTimeDelta);
    switch (m_i2ComboType)
    {
    case 0:
        if ((m_iActionState == RK_A_ATT_LM1) && (m_fCalculateTime3 > 0.05f))
        {
            m_fCalculateTime3 = 0.f;
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack1", m_fEffectSize);
                m_iActionState = RK_A_ATT_LM2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            }
        }
        else if (m_iActionState == RK_A_ATT_LM2)
        {
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 2.f + 0.1f, m_fKeyFrameSpeed * 8.f + 0.1f, m_fLM2SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ATT_U1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                SetWeaponRimLight(true);
            }
        }
        else if (m_iActionState == RK_A_ATT_U1)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack2", m_fEffectSize);
                m_iActionState = RK_A_ATT_U5;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            }
        }
        else if (m_iActionState == RK_A_ATT_U5)
        {
            // 패링
            if (m_bParried)
            { 
                SetWeaponRimLight(false);
                m_iParriedPower = CheckParryingStack(m_iAttParryStack);
                if (m_iParriedPower == PARRIEDSTRONG)
                {
                    m_iBaseState = RK_B_BLOCKED;
                    GAMEINSTANCE->PlaySoundW("Final_Blocked", m_fVoiceSize);
                    m_bParried = false;
                    ResetTimer();
                    m_fAnimSpeed = 1.f;
                    m_iActionState = RK_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                    m_i2ComboType = 10;
                    return true;
                }
                m_bParried = false;
                ResetTimer();
                m_fAnimSpeed = 1.f;
                m_iActionState = RK_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
            }
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 10.f + 0.1f, m_fU5SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                SetWeaponRimLight(false);
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i2ComboType = 10;
                return true;
            }
        }
        else if (m_iActionState == RK_A_BLOCKED)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i2ComboType = 10;
                return true;
            }
        }
        break;
    case 1:
        if ((m_iActionState == RK_A_ATT_L1) && (m_fCalculateTime3 > 0.05f))
        {
            m_fCalculateTime3 = 0.f;
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack1", m_fEffectSize);
                m_iActionState = RK_A_ATT_L2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            }
        }
        else if (m_iActionState == RK_A_ATT_L2)
        {
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 2.f + 0.1f, m_fKeyFrameSpeed * 8.f + 0.1f, m_fL2SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ATT_D1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                SetWeaponRimLight(true);
            }
        }
        else if (m_iActionState == RK_A_ATT_D1)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack2", m_fEffectSize);
                m_iActionState = RK_A_ATT_D2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            }
        }
        else if (m_iActionState == RK_A_ATT_D2)
        {
            // 패링
            if (m_bParried)
            {
                SetWeaponRimLight(false);
                m_iParriedPower = CheckParryingStack(m_iAttParryStack);
                if (m_iParriedPower == PARRIEDSTRONG)
                {
                    m_iBaseState = RK_B_BLOCKED;
                    GAMEINSTANCE->PlaySoundW("Final_Blocked", m_fVoiceSize);
                    m_bParried = false;
                    ResetTimer();
                    m_fAnimSpeed = 1.f;
                    m_iActionState = RK_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                    m_i2ComboType = 10;
                    return true;
                }
                m_bParried = false;
                ResetTimer();
                m_fAnimSpeed = 1.f;
                m_iActionState = RK_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                SetWeaponRimLight(false);
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i2ComboType = 10;
                return true;
            }
        }
        else if (m_iActionState == RK_A_BLOCKED)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i2ComboType = 10;
                return true;
            }
        }
        break;
    case 2:
        if (m_iActionState == RK_A_ATT_U4 && (m_fCalculateTime3 > 0.05f))
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack2", m_fEffectSize);
                m_fCalculateTime3 = 0.f;
                m_iActionState = RK_A_ATT_U3;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            }
        }
        else if (m_iActionState == RK_A_ATT_U3)
        {
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 10.f + 0.1f, m_fU3SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ATT_D1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                SetWeaponRimLight(true);
            }
        }
        else if (m_iActionState == RK_A_ATT_D1)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack1", m_fEffectSize);
                m_iActionState = RK_A_ATT_D2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            }
        }
        else if (m_iActionState == RK_A_ATT_D2)
        {
            // 패링
            if (m_bParried)
            {
                SetWeaponRimLight(false);
                m_iParriedPower = CheckParryingStack(m_iAttParryStack);
                if (m_iParriedPower == PARRIEDSTRONG)
                {
                    m_iBaseState = RK_B_BLOCKED;
                    GAMEINSTANCE->PlaySoundW("Final_Blocked", m_fVoiceSize);
                    m_bParried = false;
                    ResetTimer();
                    m_fAnimSpeed = 1.f;
                    m_iActionState = RK_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                    m_i2ComboType = 10;
                    return true;
                }
                m_bParried = false;
                ResetTimer();
                m_fAnimSpeed = 1.f;
                m_iActionState = RK_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                SetWeaponRimLight(false);
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i2ComboType = 10;
                return true;
            }
        }
        else if (m_iActionState == RK_A_BLOCKED)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i2ComboType = 10;
                return true;
            }
        }
        break;
    //case 3:
    //    if (m_iActionState == RK_A_ATT_D3 && (m_fCalculateTime3 > 0.05f))
    //    {
    //        if (m_pModelCom->GetIsFinishedAnimation())
    //        {
    //            m_fCalculateTime3 = 0.f;
    //            m_iActionState = RK_A_ATT_D2;
    //            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
    //        }
    //    }
    //    else if (m_iActionState == RK_A_ATT_D2)
    //    {
    //        if (m_pModelCom->GetIsFinishedAnimation())
    //        {
    //            m_iActionState = RK_A_ATT_L1;
    //            m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
    //        }
    //    }
    //    else if ((m_iActionState == RK_A_ATT_L1))
    //    {
    //        if (m_pModelCom->GetIsFinishedAnimation())
    //        {
    //            m_iActionState = RK_A_ATT_L4;
    //            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
    //        }
    //    }
    //    else if (m_iActionState == RK_A_ATT_L4)
    //    {
    //        m_fCalculateTime += _fTimeDelta;
    //        UpdateMoveSlide(m_fKeyFrameSpeed * 2.f + 0.1f, m_fKeyFrameSpeed * 8.f + 0.1f, m_fL42SlideSpeed, _fTimeDelta);
    //        if (m_pModelCom->GetIsFinishedAnimation())
    //        {
    //            m_fCalculateTime = 0.f;
    //            /*m_iActionState = RandomSlowSlash();
    //            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
    //            m_i2ComboType = 10;*/

    //            m_iActionState = RK_A_ALT;
    //            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
    //            m_i2ComboType = 10;
    //            //OffCollisionWeapon();
    //            return true;
    //        }
    //    }
    //    break;
    }
    return false;
}

_bool CRedKnight::Update3ComboAttackState(float _fTimeDelta)
{
    if ((m_iActionState == RK_A_ATT_L1 && m_i3ComboType == 1) || (m_iActionState == RK_A_ATT_LM1 && m_i3ComboType == 0)
        || (m_iActionState == RK_A_ATT_D3 && m_i3ComboType == 3) || (m_iActionState == RK_A_ATT_U4 && m_i3ComboType == 2))
    {
        m_fCalculateTime3 += _fTimeDelta;
    }
    UpdateAttackDir(_fTimeDelta);
    switch (m_i3ComboType)
    {
    case 0:
        if ((m_iActionState == RK_A_ATT_LM1) && (m_fCalculateTime3 > 0.05f))
        {
            m_fCalculateTime3 = 0.f;
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack1", m_fEffectSize);
                m_iActionState = RK_A_ATT_LM2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                if (m_bFastComboSlash)
                {
                    ActiveSlash(SLASHLRIGHTDOWN);
                }
            }
        }
        else if (m_iActionState == RK_A_ATT_LM2)
        {
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 2.f + 0.1f, m_fKeyFrameSpeed * 8.f + 0.1f, m_fLM2SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ATT_U1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
            }
        }
        else if (m_iActionState == RK_A_ATT_U1)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack2", m_fEffectSize);
                m_iActionState = RK_A_ATT_U2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                if (m_bFastComboSlash)
                {
                    ActiveSlash(SLASHUP);
                }
            }
        }
        else if (m_iActionState == RK_A_ATT_U2)
        {
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 10.f + 0.1f, m_fU2SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ATT_D1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                if(!m_bFastComboSlash)
                    SetWeaponRimLight(true);
            }
        }
        else if (m_iActionState == RK_A_ATT_D1)
        { 
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack1", m_fEffectSize);
                m_iActionState = RK_A_ATT_D2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                if (m_bFastComboSlash)
                {
                    GAMEINSTANCE->PlaySoundW("Final_BigAttack2", m_fVoiceSize);
                    ActiveSlash(SLASHDOWN);
                }
            }
        }
        else if (m_iActionState == RK_A_ATT_D2)
        {
            // 패링
            if (m_bParried)
            {
                if (!m_bFastComboSlash)
                    SetWeaponRimLight(false);
                m_iParriedPower = CheckParryingStack(m_iAttParryStack);
                if (m_iParriedPower == PARRIEDSTRONG)
                {
                    m_iBaseState = RK_B_BLOCKED;
                    GAMEINSTANCE->PlaySoundW("Final_Blocked", m_fVoiceSize);
                    m_bParried = false;
                    ResetTimer();
                    m_fAnimSpeed = 1.f;
                    m_iActionState = RK_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                    m_i3ComboType = 10;
                    return true;
                }
                m_bParried = false;
                ResetTimer();
                m_fAnimSpeed = 1.f;
                m_iActionState = RK_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
            }
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                if (!m_bFastComboSlash)
                    SetWeaponRimLight(false);
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i3ComboType = 10;
                return true;
            }
        }
        else if (m_iActionState == RK_A_BLOCKED)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i3ComboType = 10;
                return true;
            }
        }
        break;
    case 1:
        if ((m_iActionState == RK_A_ATT_L1) && (m_fCalculateTime3 > 0.05f))
        {
            m_fCalculateTime3 = 0.f;
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack2", m_fEffectSize);
                m_iActionState = RK_A_ATT_L2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                if (m_bFastComboSlash)
                    ActiveSlash(SLASHRIGHT);
            }
        }
        else if (m_iActionState == RK_A_ATT_L2)
        {
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 2.f + 0.1f, m_fKeyFrameSpeed * 8.f + 0.1f, m_fL2SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ATT_D1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
            }
        }
        else if (m_iActionState == RK_A_ATT_D1)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack1", m_fEffectSize);
                m_iActionState = RK_A_ATT_D2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                if (m_bFastComboSlash)
                    ActiveSlash(SLASHDOWN);
            }
        }
        else if (m_iActionState == RK_A_ATT_D2)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = RK_A_ATT_U1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                if (!m_bFastComboSlash)
                    SetWeaponRimLight(true);
            }
        }
        else if (m_iActionState == RK_A_ATT_U1)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack2", m_fEffectSize);
                m_iActionState = RK_A_ATT_U2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                if (m_bFastComboSlash)
                {
                    GAMEINSTANCE->PlaySoundW("Final_BigAttack1", m_fVoiceSize);
                    ActiveSlash(SLASHUP);
                }
            }
        }
        else if (m_iActionState == RK_A_ATT_U2)
        {
            // 패링
            if (m_bParried)
            {
                if (!m_bFastComboSlash)
                    SetWeaponRimLight(false);
                m_iParriedPower = CheckParryingStack(m_iAttParryStack);
                if (m_iParriedPower == PARRIEDSTRONG)
                {
                    m_iBaseState = RK_B_BLOCKED;
                    GAMEINSTANCE->PlaySoundW("Final_Blocked", m_fVoiceSize);
                    m_bParried = false;
                    ResetTimer();
                    m_fAnimSpeed = 1.f;
                    m_iActionState = RK_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                    m_i3ComboType = 10;
                    return true;
                }
                m_bParried = false;
                ResetTimer();
                m_fAnimSpeed = 1.f;
                m_iActionState = RK_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
            }
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 10.f + 0.1f, m_fU2SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                if (!m_bFastComboSlash)
                    SetWeaponRimLight(false);
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i3ComboType = 10;
                return true;
            }
        }
        else if (m_iActionState == RK_A_BLOCKED)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i3ComboType = 10;
                return true;
            }
        }
        break;
    case 2:
        if (m_iActionState == RK_A_ATT_U4 && (m_fCalculateTime3 > 0.05f))
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack1", m_fEffectSize);
                m_fCalculateTime3 = 0.f;
                m_iActionState = RK_A_ATT_U3;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                if (m_bFastComboSlash)
                    ActiveSlash(SLASHUP);
            }
        }
        else if (m_iActionState == RK_A_ATT_U3)
        {
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 10.f + 0.1f, m_fU3SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ATT_D1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
            }
        }
        else if (m_iActionState == RK_A_ATT_D1)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack2", m_fEffectSize);
                m_iActionState = RK_A_ATT_D2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                if (m_bFastComboSlash)
                    ActiveSlash(SLASHDOWN);
            }
        }
        else if (m_iActionState == RK_A_ATT_D2)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = RK_A_ATT_L1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                if (!m_bFastComboSlash)
                    SetWeaponRimLight(true);
            }
        }
        else if (m_iActionState == RK_A_ATT_L1)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack1", m_fEffectSize);
                m_iActionState = RK_A_ATT_L4;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                if (m_bFastComboSlash)
                {
                    GAMEINSTANCE->PlaySoundW("Final_BigAttack2", m_fVoiceSize);
                    ActiveSlash(SLASHLRIGHT);
                }
            }
        }
        else if (m_iActionState == RK_A_ATT_L4)
        {
            // 패링
            if (m_bParried)
            {
                if (!m_bFastComboSlash)
                    SetWeaponRimLight(false);
                m_iParriedPower = CheckParryingStack(m_iAttParryStack);
                if (m_iParriedPower == PARRIEDSTRONG)
                {
                    m_iBaseState = RK_B_BLOCKED;
                    GAMEINSTANCE->PlaySoundW("Final_Blocked", m_fVoiceSize);
                    m_bParried = false;
                    ResetTimer();
                    m_fAnimSpeed = 1.f;
                    m_iActionState = RK_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                    m_i3ComboType = 10;
                    return true;
                }
                m_bParried = false;
                ResetTimer();
                m_fAnimSpeed = 1.f;
                m_iActionState = RK_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
            }
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 2.f + 0.1f, m_fKeyFrameSpeed * 8.f + 0.1f, m_fL42SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                if (!m_bFastComboSlash)
                    SetWeaponRimLight(false);
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i3ComboType = 10;
                return true;
            }
        }
        else if (m_iActionState == RK_A_BLOCKED)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i3ComboType = 10;
                return true;
            }
        }
        break;
    case 3:
        if (m_iActionState == RK_A_ATT_D3 && (m_fCalculateTime3 > 0.05f))
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack2", m_fEffectSize);
                m_fCalculateTime3 = 0.f;
                m_iActionState = RK_A_ATT_D2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                if (m_bFastComboSlash)
                    ActiveSlash(SLASHDOWN);
            }
        }
        else if (m_iActionState == RK_A_ATT_D2)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_iActionState = RK_A_ATT_LM1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
            }
        }
        else if ((m_iActionState == RK_A_ATT_LM1))
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack1", m_fEffectSize);
                m_iActionState = RK_A_ATT_LM2;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                if (m_bFastComboSlash)
                    ActiveSlash(SLASHLRIGHTDOWN);
            }
        }
        else if (m_iActionState == RK_A_ATT_LM2)
        {
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 2.f + 0.1f, m_fKeyFrameSpeed * 8.f + 0.1f, m_fLM2SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ATT_U1;
                m_pModelCom->ChangeAnim(m_iActionState, 0.3f);
                if (!m_bFastComboSlash)
                    SetWeaponRimLight(true);
            }
        }
        else if (m_iActionState == RK_A_ATT_U1)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                GAMEINSTANCE->PlaySoundW("Final_SFX_Attack2", m_fEffectSize);
                m_iActionState = RK_A_ATT_U5;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
                if (m_bFastComboSlash)
                {
                    GAMEINSTANCE->PlaySoundW("Final_BigAttack1", m_fVoiceSize);
                    ActiveSlash(SLASHLUP);
                }
            }
        }
        else if (m_iActionState == RK_A_ATT_U5)
        {
            // 패링
            if (m_bParried)
            {
                if (!m_bFastComboSlash)
                    SetWeaponRimLight(false);
                m_iParriedPower = CheckParryingStack(m_iAttParryStack);
                if (m_iParriedPower == PARRIEDSTRONG)
                {
                    m_iBaseState = RK_B_BLOCKED;
                    GAMEINSTANCE->PlaySoundW("Final_Blocked", m_fVoiceSize);
                    m_bParried = false;
                    ResetTimer();
                    m_fAnimSpeed = 1.f;
                    m_iActionState = RK_A_BLOCKED;
                    m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                    m_i3ComboType = 10;
                    return true;
                }
                m_bParried = false;
                ResetTimer();
                m_fAnimSpeed = 1.f;
                m_iActionState = RK_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
            }
            m_fCalculateTime += _fTimeDelta;
            UpdateMoveSlide(m_fKeyFrameSpeed * 0.f + 0.1f, m_fKeyFrameSpeed * 10.f + 0.1f, m_fU5SlideSpeed, _fTimeDelta);
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                if (!m_bFastComboSlash)
                    SetWeaponRimLight(false);
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i3ComboType = 10;
                return true;
            }
        }
        else if (m_iActionState == RK_A_BLOCKED)
        {
            if (m_pModelCom->GetIsFinishedAnimation())
            {
                m_fCalculateTime = 0.f;
                m_iActionState = RK_A_ALT;
                m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
                m_i3ComboType = 10;
                return true;
            }
        }
        break;
    }
    return false;
}

_bool CRedKnight::UpdateAttackState(float _fTimeDelta)
{
    UpdateSlashDir(_fTimeDelta);
    m_fCalculateTime += _fTimeDelta;
    if (m_iActionState == RK_A_ATT_D3 && (m_fCalculateTime > 0.1f))
    {
        m_pWeaponTrail->StartTrail(_float4(1.f, 0.8f, 0.7f, 1.0f), _float4(1.f, 0.f, 0.3f, 0.9f), false, false, true, 0.5f);

        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            m_iActionState = RK_A_ATT_D4;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            //m_bSlashGen = true;
            GAMEINSTANCE->PlaySoundW("Final_BigAttack1", m_fVoiceSize);
            ActiveSlash(SLASHLDOWN);
        }
    }
    if (m_iActionState == RK_A_ATT_D4 && (m_fCalculateTime > 0.1f))
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            /*m_iActionState = RK_A_ATT_L3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);*/
            m_iActionState = RK_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            return true;
        }
    }
    if (m_iActionState == RK_A_ATT_L3 && (m_fCalculateTime > 0.1f))
    {
        m_pWeaponTrail->StartTrail(_float4(1.f, 0.8f, 0.7f, 1.0f), _float4(1.f, 0.f, 0.3f, 0.9f), false, false, true, 0.5f);

        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            m_iActionState = RK_A_ATT_L4;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            GAMEINSTANCE->PlaySoundW("Final_BigAttack2", m_fVoiceSize);
            ActiveSlash(SLASHLRIGHT);
        }
    }
    if (m_iActionState == RK_A_ATT_L4 && (m_fCalculateTime > 0.1f))
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            /*m_iActionState = RK_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);*/
            m_iActionState = RK_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            return true;
        }
    }
    if (m_iActionState == RK_A_ATT_U4 && (m_fCalculateTime > 0.1f))
    {
        m_pWeaponTrail->StartTrail(_float4(1.f, 0.8f, 0.7f, 1.0f), _float4(1.f, 0.f, 0.3f, 0.9f), false, false, true, 0.5f);

        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            m_iActionState = RK_A_ATT_U5;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            ActiveSlash(SLASHLUP);
            GAMEINSTANCE->PlaySoundW("Final_BigAttack1", m_fVoiceSize);
        }
    }
    if (m_iActionState == RK_A_ATT_U5 && (m_fCalculateTime > 0.1f))
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            /*m_iActionState = RK_A_ATT_LM1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);*/
            m_iActionState = RK_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
            return true;
        }
    }
    if (m_iActionState == RK_A_ATT_LM1 && (m_fCalculateTime > 0.1f))
    {
        m_pWeaponTrail->StartTrail(_float4(1.f, 0.8f, 0.7f, 1.0f), _float4(1.f, 0.f, 0.3f, 0.9f), false, false, true, 0.5f);

        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            m_iActionState = RK_A_ATT_LM2;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            ActiveSlash(SLASHLRIGHTDOWN);
            GAMEINSTANCE->PlaySoundW("Final_BigAttack2", m_fVoiceSize);
        }
    }
    if (m_iActionState == RK_A_ATT_LM2 && (m_fCalculateTime > 0.1f))
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fCalculateTime = 0.f;
            /*m_iActionState = RK_A_ATT_D3;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f, false);*/
            m_iActionState = RK_A_ALT;
            m_pModelCom->ChangeAnim(m_iActionState, 0.3f, true);
            return true;
        }
    }
    return false;
}

_bool CRedKnight::UpdateThroughDashState(float _fTimeDelta, _float _fSpeed, _float _fDistance)
{
    if (m_iActionState == RK_A_DASHSTART)
    {
        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
        
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fAnimSpeed = 10.f;
            m_fDashDistanceXZ = m_fDistanceToTarget + _fDistance;
            m_fDashTime = m_fDashDistanceXZ / _fSpeed;
            m_iActionState = RK_A_DASHATT1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
            m_vDashDir = m_vNormTargetDirXZ;
            UpdateDirection();
            SetUseMotionTrail(true, true, m_fDashTime, true, m_vThroughDashColor);
            GAMEINSTANCE->PlaySoundW("Final_SFX_Dash1", m_fEffectSize);
        }
    }
    if (m_iActionState == RK_A_DASHATT1)
    {
        m_fCalculateTime += _fTimeDelta;
        MovePos(CalculateBindary(m_vCurrentPos + m_vDashDir * _fSpeed * _fTimeDelta));
        // 시간지나면
        if (m_fCalculateTime > m_fDashTime)
        {
            m_fCalculateTime = 0.f;
            m_fAnimSpeed = 1.f;
            UpdateDirection();

            return true;
        }
    }
    return false;
}

_bool CRedKnight::UpdateAThroughDashState(float _fTimeDelta, _float _fSpeed, _float _fDistance)
{
    if (m_iActionState == RK_A_DASHATT1)
    {
        m_fCalculateTime += _fTimeDelta;
        MovePos(CalculateBindary(m_vCurrentPos + m_vDashDir * _fSpeed * _fTimeDelta));
        // 시간지나면
        if (m_fCalculateTime > m_fDashTime)
        {
            m_fCalculateTime = 0.f;
            m_fAnimSpeed = 1.f;
            UpdateDirection();

            return true;
        }
    }
    return false;
}

_bool CRedKnight::UpdateFrontDashState(float _fTimeDelta, _float _fSpeed, _float _fFrontDistance)
{
    if (m_iActionState == RK_A_DASHSTART)
    {
        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fAnimSpeed = 10.f;
            m_fDashDistanceXZ = m_fDistanceToTarget - _fFrontDistance;
            m_fDashTime = m_fDashDistanceXZ / _fSpeed;
            m_iActionState = RK_A_DASHATT1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
            m_vDashDir = m_vNormTargetDirXZ;
            UpdateDirection();
            m_fCalculateTime = 0.f;
            SetUseMotionTrail(true, true, m_fDashTime, true, m_vFrontDashColor);
            GAMEINSTANCE->PlaySoundW("Final_SFX_Dash2", m_fEffectSize);
        }
    }
    if (m_iActionState == RK_A_DASHATT1)
    {
        // 패링
        if (m_bParried)
        {
            SetWeaponRimLight(false);
            m_iParriedPower = CheckParryingStack(m_iDashParryStack);
            if (m_iParriedPower == PARRIEDSTRONG)
            {
                m_iBaseState = RK_B_BLOCKED;
                GAMEINSTANCE->PlaySoundW("Final_Blocked", m_fVoiceSize);
                m_bParried = false;
                ResetTimer();
                m_fAnimSpeed = 1.f;
                m_iActionState = RK_A_BLOCKED;
                m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
                return true;
            }
            m_bParried = false;
            ResetTimer();
            m_fAnimSpeed = 1.f;
            m_iActionState = RK_A_BLOCKED;
            m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
        }

        if (m_iActionState != RK_A_BLOCKED)
        {
            m_fCalculateTime += _fTimeDelta;
            MovePos(m_vCurrentPos + m_vDashDir * _fSpeed * _fTimeDelta);
        }

        // 시간지나면
        if (m_fCalculateTime > m_fDashTime && (m_iActionState != RK_A_BLOCKED))
        {
            SetWeaponRimLight(false);
            m_fCalculateTime = 0.f;
            m_fAnimSpeed = 1.f;
            UpdateDirection();

            return true;
        }
    }
    if (m_iActionState == RK_A_BLOCKED)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_pModelCom->GetIsFinishedAnimation() && m_fCalculateTime > 0.1f)
        {
            m_fCalculateTime = 0.f;
            m_fAnimSpeed = 1.f;
            return true;
        }
    }
    return false;
}

_bool CRedKnight::UpdateJumpUpDashState(float _fTimeDelta, _float _fSpeed, _float _fDistance)
{
    if (m_iActionState == RK_A_DASHSTART)
    {
        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fDashDistanceXZ = m_fDistanceToTarget + _fDistance;
            m_fDashDistanceXZGround = m_fDistanceToTarget - 3.f;
            m_fDashTime = m_fDashDistanceXZ / _fSpeed;
            m_fDashYTime = m_fDashDistanceXZGround / _fSpeed;
            m_fDashSpeedY = m_fDashDistanceY / m_fDashYTime;
            m_iActionState = RK_A_DASHATT1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
            m_fAnimSpeed = 10.f;
            m_vDashDir = m_vNormTargetDirXZ;
            UpdateDirection();
            SetUseMotionTrail(true, true, m_fDashTime, true, m_vThroughDashColor);
            GAMEINSTANCE->PlaySoundW("Final_JumpDashStart", m_fVoiceSize);
            GAMEINSTANCE->PlaySoundW("Final_SFX_Dash2", m_fEffectSize);
        }
    }
    if (m_iActionState == RK_A_DASHATT1)
    {
        m_fCalculateTime += _fTimeDelta;
        MovePos(CalculateBindary(m_vCurrentPos + m_vDashDir * _fSpeed * _fTimeDelta));

        // y값 내려가는 시간
        if (m_fCalculateTime < m_fDashYTime)
        {
            if (m_vCurrentPos.y > m_fDashBeforeY)
            {
                MovePos(m_vCurrentPos - _float3(0.f, 1.f, 0.f) * m_fDashSpeedY * _fTimeDelta);
            }
            else
            {
                m_pTransformCom->SetPositionParam(1, m_fDashBeforeY);
            }
        }

        // 시간지나면
        if (m_fCalculateTime > m_fDashTime)
        {
            m_fCalculateTime = 0.f;
            UpdateDirection();
            m_fAnimSpeed = 1.f;
            return true;
        }
    }
    return false;
}

_bool CRedKnight::UpdateJumpDashState(float _fTimeDelta, _float _fSpeed, _float _fDistance)
{
    if (m_iActionState == RK_A_GOUP)
    {
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fDashBeforeY = m_pTarget->GetTransform()->GetState(CTransform::STATE_POSITION).y;
            m_fDashDistanceY = m_fDashUpDistance + m_fGroundPosY - m_fDashBeforeY;
            m_pTransformCom->SetPositionParam(1, m_fDashUpDistance + m_fGroundPosY);
            m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
            m_iActionState = RK_A_DASHSTART;
            m_pModelCom->ChangeAnim(m_iActionState, 0.1f);
            m_fAnimSpeed = 0.5f;
            GAMEINSTANCE->PlaySoundW("Final_JumpDashStart", m_fVoiceSize);
            GAMEINSTANCE->PlaySoundW("Final_SFX_Dash2", m_fEffectSize);
        }
    }
    if (m_iActionState == RK_A_DASHSTART)
    {
        m_pTransformCom->TurnToDir(m_vTargetDir, 90.f * _fTimeDelta);
        if (m_pModelCom->GetIsFinishedAnimation())
        {
            m_fDashDistanceXZ = m_fDistanceToTarget + _fDistance;
            m_fDashDistanceXZGround = m_fDistanceToTarget - 3.f;
            m_fDashTime = m_fDashDistanceXZ / _fSpeed;
            m_fDashYTime = m_fDashDistanceXZGround / _fSpeed;
            m_fDashSpeedY = m_fDashDistanceY / m_fDashYTime;
            m_iActionState = RK_A_DASHATT1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
            m_fAnimSpeed = 10.f;
            m_vDashDir = m_vNormTargetDirXZ;
            UpdateDirection();
            SetUseMotionTrail(true, true, m_fDashTime, true, m_vThroughDashColor);
        }
    }
    if (m_iActionState == RK_A_DASHATT1)
    {
        m_fCalculateTime += _fTimeDelta;
        MovePos(CalculateBindary(m_vCurrentPos + m_vDashDir * _fSpeed * _fTimeDelta));
        
        // y값 내려가는 시간
        if (m_fCalculateTime < m_fDashYTime)
        {
            if (m_vCurrentPos.y > m_fDashBeforeY)
            {
                MovePos(m_vCurrentPos - _float3(0.f, 1.f, 0.f) * m_fDashSpeedY * _fTimeDelta);
            }
            else
            {
                m_pTransformCom->SetPositionParam(1, m_fDashBeforeY);
            }
        }

        // 시간지나면
        if (m_fCalculateTime > m_fDashTime)
        {
            m_fCalculateTime = 0.f;
            UpdateDirection();
            m_fAnimSpeed = 1.f;
            return true;
        }
    }
    return false;
}

_bool CRedKnight::UpdatePositionDashState(float _fTimeDelta, _float _fSpeed)
{
    if (m_iActionState == RK_A_DASHSTART)
    {
        m_pTransformCom->TurnToDir(m_vDashOutDir, 90.f * _fTimeDelta);
        m_fCalculateTime += _fTimeDelta;

        if (m_fCalculateTime > 1.f)
        {
            m_fAnimSpeed = 10.f;
            m_fCalculateTime = 0.f;
            m_fDashDistanceXZ = m_fDistanceToDashOut;
            m_fDashTime = m_fDashDistanceXZ / _fSpeed;
            m_iActionState = RK_A_DASHATT1;
            m_pModelCom->ChangeAnim(m_iActionState, 0.f, false);
            m_vDashOutDir.Normalize(m_vDashOutDir);
            m_pTransformCom->LookAt(_float3(m_vDashOutPos.x, m_vCurrentPos.y, m_vDashOutPos.z));
            SetUseMotionTrail(true, true, m_fDashTime, true, m_vOutDashColor);
            GAMEINSTANCE->PlaySoundW("Final_SFX_DashPos", m_fEffectSize);
        }
    }
    if (m_iActionState == RK_A_DASHATT1)
    {
        m_fCalculateTime += _fTimeDelta;
        MovePos(CalculateBindary(m_vCurrentPos + m_vDashOutDir * _fSpeed * _fTimeDelta));
        // 시간지나면 플레이어 관통대쉬
        if (m_fCalculateTime > m_fDashTime)
        {
            m_fCalculateTime = 0.f;
            UpdateDirection();

            return true;
        }
    }

    return false;
}

void CRedKnight::CheckGimicPattern1()
{
    if ((m_fHp <= m_fGimicPattern1Hp) && !m_bGimicPattern1)
    {
        m_bJumpAttack1EffectOn = false;
        m_fHp = m_fGimicPattern1Hp;
        m_iBaseState = RK_B_GIMICPATTERN1;
        m_iActionState = RK_A_ALT;
        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        ResetTimer();
        m_i2ComboType = 10;
        m_i3ComboType = 10;
        m_fAnimSpeed = 1.f;
        m_bGimicPattern1 = true;
        m_bGP1SkillState[0] = true;
        m_iPhase = RK_P_GIMIC1;
        SimulationOff();

        ResetSkillState();
    }
}

void CRedKnight::CheckGimicPattern2()
{
    if ((m_fHp <= m_fGimicPattern2Hp) && !m_bGimicPattern2)
    {
        m_bJumpAttack1EffectOn = false;
        m_fHp = m_fGimicPattern2Hp;
        m_iBaseState = RK_B_GIMICPATTERN2;
        m_iActionState = RK_A_ALT;
        m_pModelCom->ChangeAnim(m_iActionState, 0.1f, true);
        ResetTimer();
        m_i2ComboType = 10;
        m_i3ComboType = 10;
        m_fAnimSpeed = 1.f;
        m_bGimicPattern2 = true;
        m_bGP2SkillState[0] = true;
        m_iPhase = RK_P_GIMIC2;
        SimulationOff();

        ResetSkillState();
    }
}

void CRedKnight::CheckDead()
{
    if (m_fHp <= 0.f)
    {
        //GAMEINSTANCE->LevelBGMOff();
        m_bDead = true;
        m_fHp = 0.f;
        GAMEINSTANCE->PlaySoundW("Final_Die", m_fVoiceSize);
        m_iBaseState = RK_B_DEATH;
        m_iActionState = RK_A_DEATH;
        m_pModelCom->ChangeAnim(m_iActionState, 0.5f);
        m_pRigidBody->SimulationOff();

        EFFECTMGR->StopEffect("Boss_Aura", m_iEffectIdx);
    }
}

void CRedKnight::CheckAttacked()
{
    // 피격시
    if (m_bHitted)
    {
        ResetTimer();
        m_iBaseState = RK_B_HITTED;
        m_iActionState = RK_A_HITTED;
    }

    // 짧은경직
    if (m_bHitted2)
    {
        ResetTimer();
        m_iBaseState = RK_B_HITTED2;
        m_iActionState = RK_A_HITTED;
    }

    // 긴경직
    if (m_bHitted3)
    {
        ResetTimer();
        m_iBaseState = RK_B_HITTED3;
        m_iActionState = RK_A_HITTED;
    }
    
    // 스턴
    if (m_bStun)
    {
        ResetTimer();
        m_iBaseState = RK_B_STUN;
        m_iActionState = RK_A_STUN;
    }

    // 감전
    if (m_bShocked)
    {
        ResetTimer();
        m_iBaseState = RK_B_SHOCKED;
        m_iActionState = RK_A_SHOCKED;
    }

    // 넘어지기
    if (m_bDown)
    {
        ResetTimer();
        m_iBaseState = RK_B_DOWN;
        m_iActionState = RK_A_DOWN1;
    }
}

void CRedKnight::ResetBoss()
{
    m_fHp = m_fMaxHp;
    m_iBaseState = RK_B_BEFOREMEET;
    m_iDistanceState = RK_D_BEFOREMEET;
    m_iActionState = RK_A_WAIT;
    m_iSkillState = RK_S_NOSKILL;
    ResetSkillState();
    m_fAnimSpeed = 1.f;
    m_i2ComboType = 10;
    m_i3ComboType = 10;
    SetWeaponRimLight(false);
    m_iMeleePatternNum = 0;
    m_iMiddleRangePatternNum = 0;
    m_iRangePatternNum = 0;
    ResetTimer();
    m_bSlashGen = false;
    m_bFastComboSlash = false;
}

void CRedKnight::ResetTimer()
{
    m_fCalculateTime = 0.f;
    m_fCalculateTime2 = 0.f;
    m_fCalculateTime3 = 0.f;
}

void CRedKnight::ResetSkillState()
{
    for (int i = 0; i < 10; i++)
    {
        m_bSkill1State[i] = false;
        m_bSkill2State[i] = false;
        m_bSkill3State[i] = false;
        m_bSkill4State[i] = false;
        m_bSkill5State[i] = false;
        m_bRaSkill1State[i] = false;
        m_bRaSkill2State[i] = false;
    }
}

void CRedKnight::ActiveSlash(_int _iSlashType)
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
                m_vecSlashD[i]->GetTransform()->SetSpeed(m_fFastSlashSpeed);
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
                m_vecSlashR[i]->GetTransform()->SetSpeed(m_fFastSlashSpeed);
                m_vecSlashR[i]->SetProjectileType(PJ_DIRECT);
                m_vecSlashR[i]->SetEnable(true);
                m_vecSlashR[i]->OnCollisionProjectile();
                m_vecSlashR[i]->OnEffect();
                m_vecSlashR[i]->SetPower(1);
                m_vecSlashR[i]->SetAbnormal(PJ_A_SHORTHITTED);
                break;
            }
        }
        else if (_iSlashType == SLASHBIG)
        {
            if (!(m_vecSlashBig[i]->IsEnabled()))
            {
                m_vecSlashBig[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos + _float3(0.f, 2.5f, 0.f));
                m_vecSlashBig[i]->UpdateTargetPos();
                m_vecSlashBig[i]->UpdateTargetDir(_float3(0.f, 2.5f, 0.f));
                m_vecSlashBig[i]->UpdateDirection();
                m_vecSlashBig[i]->GetTransform()->SetSpeed(m_fBigSlashSpeed);
                m_vecSlashBig[i]->SetProjectileType(PJ_READYDIRECT);
                m_vecSlashBig[i]->SetEnable(true);
                m_vecSlashBig[i]->OnCollisionProjectile();
                m_vecSlashBig[i]->OnEffect();
                m_vecSlashBig[i]->SetPower(1);
                m_vecSlashBig[i]->SetAbnormal(PJ_A_GOOUT);
                m_vecSlashBig[i]->SetReady(1.f);
                break;
            }
        }
    }
}

void CRedKnight::ActiveBindMagic(_int _iBindMagicType, _int _iCircleMagicType, _float _fReadyTime)
{
    for (int i = 0; i < m_iBindMagicCount; i++)
    {
        if (_iBindMagicType == BM_T_GUIDED)
        {
            if (!(m_vecBindMagic[i]->IsEnabled()))
            {
                m_vecBindMagic[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos + _float3(0.f, 2.f, 0.f));
                m_vecBindMagic[i]->UpdateTargetPos();
                m_vecBindMagic[i]->UpdateTargetDir(_float3(0.f, 1.5f, 0.f));
                m_vecBindMagic[i]->UpdateDirection();
                m_vecBindMagic[i]->GetTransform()->SetSpeed(m_fBindGuidedMagicSpeed);
                m_vecBindMagic[i]->SetProjectileType(PJ_GUIDED);
                m_vecBindMagic[i]->SetEnable(true);
                m_vecBindMagic[i]->OnCollisionProjectile();
                m_vecBindMagic[i]->OnEffect();
                m_vecBindMagic[i]->SetPower(1);
                m_vecBindMagic[i]->SetAbnormal(PJ_A_BIND);
                m_vecBindMagic[i]->SetMagicType(_iBindMagicType);
                break;
            }
        }
        else if (_iBindMagicType == BM_T_UPGUIDED)
        {
            if (!(m_vecBindMagic[i]->IsEnabled()))
            {
                m_vecBindMagic[i]->GetTransform()->SetState(CTransform::STATE_POSITION, m_vCurrentPos + _float3(0.f, 4.f, 0.f));
                m_vecBindMagic[i]->UpdateTargetPos();
                m_vecBindMagic[i]->UpdateTargetDir(_float3(0.f, 1.5f, 0.f));
                m_vecBindMagic[i]->UpdateDirection();
                m_vecBindMagic[i]->GetTransform()->SetSpeed(m_fBindGuidedMagicSpeed);
                m_vecBindMagic[i]->SetProjectileType(PJ_READYGUIDED);
                m_vecBindMagic[i]->SetEnable(true);
                m_vecBindMagic[i]->OnCollisionProjectile();
                m_vecBindMagic[i]->OnEffect();
                m_vecBindMagic[i]->SetPower(1);
                m_vecBindMagic[i]->SetAbnormal(PJ_A_BIND);
                m_vecBindMagic[i]->SetReady(m_fBindMagicFireTime);
                m_vecBindMagic[i]->SetMagicType(_iBindMagicType);
                break;
            }
        }
        else if (_iBindMagicType == BM_T_SPINBACK)
        {
            if (!(m_vecBindMagic[i]->IsEnabled()))
            {
                m_vecBindMagic[i]->UpdateTargetPos();
                m_vecBindMagic[i]->UpdateTargetDir(_float3(0.f, 1.5f, 0.f));
                m_vecBindMagic[i]->UpdateDirection();
                m_vecBindMagic[i]->GetTransform()->SetSpeed(m_fBindGuidedMagicSpeed2);
                m_vecBindMagic[i]->SetProjectileType(PJ_READYGUIDED);
                m_vecBindMagic[i]->SetEnable(true);
                m_vecBindMagic[i]->OnCollisionProjectile();
                m_vecBindMagic[i]->OnEffect();
                m_vecBindMagic[i]->SetPower(1);
                m_vecBindMagic[i]->SetAbnormal(PJ_A_BIND);
                m_vecBindMagic[i]->SetReady(_fReadyTime);
                m_vecBindMagic[i]->SetMagicType(_iBindMagicType);
                m_vecBindMagic[i]->SetCircleMagicType(_iCircleMagicType);
                break;
            }
        }
    }
}
