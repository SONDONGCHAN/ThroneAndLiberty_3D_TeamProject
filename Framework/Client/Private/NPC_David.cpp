#include "NPC_David.h"
#include "GameInstance.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "Animation.h"
#include "EffectMgr.h"
#include "PlayerProjectile.h"
#include "NPC_MonsterDavid.h"
#include "Player.h"

#include "QuestMgr.h"
#include "DialogueMgr.h"
#include "UIMgr.h"

CNPC_David::CNPC_David()
    : CNPC()
{
}

HRESULT CNPC_David::Initialize(_float3 _vCreatePos)
{
    // 설정된 위치에 생성
    __super::Initialize(_vCreatePos);
    m_vCreatePos = m_vTrainPos;
    //static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->SetNPCPos(m_vCreatePos);
    static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->SetNPCPos(m_vCreatePos);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_ANIM;
    m_iShaderPass = 0;

    m_pModel = GAMEINSTANCE->GetModel("CT_Named_ArthurTalon_SK");
    m_strModelName = "CT_Named_ArthurTalon_SK";
    AddComponent(L"Com_Model", m_pModel);
    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);
    //m_pTransformCom->Rotation(_float3(0.f, 1.f, 0.f), 3.14f);
    
    m_pModel->ChangeAnim(m_iActionState, 0.1f, true);

    if (FAILED(AddPartObjects("Sword2h_Sample")))       // 손
        return E_FAIL;

    if (FAILED(AddPartObjects("Sword2h_Sample2")))      // 등뒤
        return E_FAIL;

    m_pNPCParts.find("Part_Weapon")->second->SetActiveOn(false);
    m_bHandWeaponRender = false;
    m_pNPCParts.find("Part_InWeapon")->second->SetActiveOn(true);
    m_bBackWeaponRender = true;

    m_fSpeed = 1.f;
    m_fAnimSpeed = 1.f;
    m_bInteractAble = true;

    m_pTransformCom->SetSpeed(1.f);

    AddRigidBody("CT_Named_ArthurTalon_SK");

    //m_pRigidBody->SimulationOn();

    // 생성시 Dialog 상태
    m_iDialogOrder = DAVID_DI_FIRSTMEET;

    PrepareNPCBar(L"David");
    m_strDisplayName = L"David";

    return S_OK;
}

void CNPC_David::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);
    
    if (DIALOGUEMGR->GetCurDialogueKey() == L"David0" && DIALOGUEMGR->GetDialogueState() == DIALOGUE_COMPLETE)
    {
        m_bMoveToTraining1Trigger = true;
        m_iActionState = DAVID_A_IDLE;
        m_pModel->ChangeAnim(m_iActionState, m_fToIdle, true);
    }
    if (DIALOGUEMGR->GetCurDialogueKey() == L"David1" && DIALOGUEMGR->GetDialogueState() == DIALOGUE_COMPLETE)
    {
        m_iActionState = DAVID_A_IDLE;
        m_pModel->ChangeAnim(m_iActionState, m_fToIdle, true);
    }
    if (DIALOGUEMGR->GetCurDialogueKey() == L"David2" && DIALOGUEMGR->GetDialogueState() == DIALOGUE_COMPLETE)
    {
        m_iActionState = DAVID_A_IDLE;
        m_pModel->ChangeAnim(m_iActionState, m_fToIdle, true);
        m_bTrainStartTiming = true;
    }
    if (DIALOGUEMGR->GetCurDialogueKey() == L"David3" && DIALOGUEMGR->GetDialogueState() == DIALOGUE_COMPLETE)
    {
        m_iActionState = DAVID_A_IDLE;
        m_pModel->ChangeAnim(m_iActionState, m_fToIdle, true);
        //dynamic_pointer_cast<CPlayer>(m_pPlayer.lock())->RestoreHp();
        m_bTrainStartTiming = true;
    }
    if (DIALOGUEMGR->GetCurDialogueKey() == L"David4" && DIALOGUEMGR->GetDialogueState() == DIALOGUE_COMPLETE)
    {
        m_iActionState = DAVID_A_IDLE;
        m_pModel->ChangeAnim(m_iActionState, m_fToIdle, true);
        //dynamic_pointer_cast<CPlayer>(m_pPlayer.lock())->RestoreHp();
        m_bTrainStartTiming = true;
    }
    if (DIALOGUEMGR->GetCurDialogueKey() == L"David5" && DIALOGUEMGR->GetDialogueState() == DIALOGUE_COMPLETE)
    {
        //dynamic_pointer_cast<CPlayer>(m_pPlayer.lock())->RestoreHp();
        m_bMoveToBack1 = true;
        m_pTransformCom->LookAtDir(_float3(m_vMovePos.x, m_vCurrentPos.y, m_vMovePos.z) -
            _float3(m_vCurrentPos.x, m_vCurrentPos.y, m_vCurrentPos.z));
        m_iActionState = DAVID_A_RUN;
        m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
        m_iDialogOrder = DAVID_DI_MOVEBACKTO;
    }
}

void CNPC_David::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    // Base상태와 대화순서에 따른 상태 조절
    if (m_iBaseState == DAVID_B_NORMAL)
    {
        switch (m_iDialogOrder)
        {
        case DAVID_DI_FIRSTMEET:
            FSM_FirstMeet(_fTimeDelta);
            break;
        case DAVID_DI_MOVETOTRAINING:
            FSM_MoveToTraining(_fTimeDelta);
            break;
        case DAVID_DI_PRACTICEMEET1:
            FSM_PracticeMeet1(_fTimeDelta);
            break;
        case DAVID_DI_PRACTICEMEET2:
            FSM_PracticeMeet2(_fTimeDelta);
            break;
        case DAVID_DI_PRACTICEMEET3:
            FSM_PracticeMeet3(_fTimeDelta);
            break;
        case DAVID_DI_PRACTICEMEET4:
            FSM_PracticeMeet4(_fTimeDelta);
            break;
        case DAVID_DI_PRACTICEMEET5:
            FSM_PracticeMeet5(_fTimeDelta);
            break;
        case DAVID_DI_MOVEBACKTO:
            FSM_MoveBackTo(_fTimeDelta);
            break;
        }
    }

    // 애니메이션 속도 조절
    for (int i = 0; i < 100; i++)
    {
        switch (i)
        {
        case DAVID_A_ATT1_2:
            m_fActionAnimSpeed[i] = 0.5f;
            break;
        case DAVID_A_ATT2_2:
            m_fActionAnimSpeed[i] = 0.5f;
            break;
        default:
            m_fActionAnimSpeed[i] = 1.f;
            break;
        }
    }

    m_pModel->PlayAnimation(m_fActionAnimSpeed[m_iActionState] * m_fAnimSpeed * _fTimeDelta);

    m_pTransformCom->Tick(_fTimeDelta);
}

void CNPC_David::LateTick(_float _fTimeDelta)
{
    //m_pModel->CheckDisableAnim();

    m_pTransformCom->LateTick(_fTimeDelta);

    __super::LateTick(_fTimeDelta);

    // frustrum
	if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 3.f))
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CNPC_David::Render()
{
    __super::Render();
    _uint iNumMeshes = m_pModel->GetNumMeshes();

    for (UINT i = 0; i < iNumMeshes; i++) {
        /*머리, 눈썹*/
        if (i == 4) {

            //Pass 변경
            GAMEINSTANCE->ReadyShader(m_eShaderType, 1);
            GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

            //색 Bind
            GAMEINSTANCE->BindRawValue("g_vHairMainColor", &m_vHairMainColor, sizeof(_float3));
            GAMEINSTANCE->BindRawValue("g_vHairSubColor", &m_vHairSubColor, sizeof(_float3));

            GAMEINSTANCE->BeginAnimModel(m_pModel, i);
        }
        else if (i == 2)
        {
            continue;
        }
        else {
            GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
            GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

            GAMEINSTANCE->BeginAnimModel(m_pModel, i);
        }
    }

    return S_OK;
}

shared_ptr<CNPC_David> CNPC_David::Create(_float3 _vCreatePos, _float3 _vInitLookPos, _float3 _vMovePos, _float3 _vSecondPos)
{
    shared_ptr<CNPC_David> pInstance = make_shared<CNPC_David>();
    pInstance->SetInitPos(_vCreatePos);
    pInstance->SetInitLookPos(_vInitLookPos);
    pInstance->SetMovePos(_vMovePos);
    pInstance->SetSecondPos(_vSecondPos);


    if (FAILED(pInstance->Initialize(_vCreatePos)))
        MSG_BOX("Failed to Create : CNPC_David");
    
    return pInstance;
}

HRESULT CNPC_David::AddRigidBody(string _strModelKey)
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
    CapsuleDesc.fHeight = 0.8f;
    CapsuleDesc.fRadius = 0.6f;
    _float3 vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
    CapsuleDesc.vOffSetPosition = { 0.0f, 1.f, 0.0f };
    CapsuleDesc.vOffSetRotation = { 0.0f, 0.0f, 90.0f };
    CapsuleDesc.strShapeTag = "NPC_David_Body";
    RigidBodyDesc.pGeometry = &CapsuleDesc;
    
    RigidBodyDesc.eThisColFlag = COL_NPC;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

    m_pTransformCom->SetRigidBody(m_pRigidBody);
    m_pRigidBody->SetMass(20.0f);
    
    return S_OK;
}

void CNPC_David::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CNPC_David::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CNPC_David::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

HRESULT CNPC_David::AddPartObjects(string _strModelKey)
{
    if (_strModelKey == "Sword2h_Sample")
    {
        shared_ptr<CWeapon::WEAPON_DESC> pWeaponDesc = make_shared<CWeapon::WEAPON_DESC>();
        pWeaponDesc->tPartObject.pParentTransform = m_pTransformCom;
        pWeaponDesc->pSocketMatrix = m_pModel->GetCombinedBoneMatrixPtr("Bn_Action_RightHand");
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
        pPartObject->SetActive(false);
        m_pNPCParts.emplace("Part_Weapon", pPartObject);
    }

    // 등뒤 검
    else if (_strModelKey == "Sword2h_Sample2")
    {
        shared_ptr<CWeapon::WEAPON_DESC> pWeaponDesc2 = make_shared<CWeapon::WEAPON_DESC>();
        pWeaponDesc2->tPartObject.pParentTransform = m_pTransformCom;
        pWeaponDesc2->pSocketMatrix = m_pModel->GetCombinedBoneMatrixPtr("Bip001-Neck");
        pWeaponDesc2->strModelKey = _strModelKey;
        pWeaponDesc2->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(-180.f)) *
            SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(-105.f));

        shared_ptr<CTransform::TRANSFORM_DESC> pTransformDesc2 = make_shared<CTransform::TRANSFORM_DESC>();

        pTransformDesc2->fSpeedPerSec = 10.f;
        pTransformDesc2->fRotationPerSec = XMConvertToRadians(90.0f);

        shared_ptr<CWeapon> pWeapon2 = CWeapon::Create(pWeaponDesc2.get(), pTransformDesc2.get());
        if (nullptr == pWeapon2)
            return E_FAIL;

        shared_ptr<CPartObject> pPartObject2 = static_pointer_cast<CPartObject>(pWeapon2);
        pPartObject2->SetActive(false);
        m_pNPCParts.emplace("Part_InWeapon", pPartObject2);
    }

    return S_OK;
}


void CNPC_David::UpdateDistanceState()
{
    // 위치 바로 못받아오는거 때문에 우선
    if (m_PlayerPos == _float3(0.f, 0.f, 0.f))
    {
        m_iDistanceState = DAVID_D_FARRANGE;
        return;
    }

    if (m_fDistanceXZToPlayer < m_fCloseRange)
    {
        m_iDistanceState = DAVID_D_CLOSE;
    }
    else if (m_fDistanceXZToPlayer < m_fMeetRange)
    {
        m_iDistanceState = DAVID_D_MIDDLE;
    }
    else
    {
        m_iDistanceState = DAVID_D_FARRANGE;
    }
}

void CNPC_David::RenderBackWeapon()
{
    m_pNPCParts.find("Part_Weapon")->second->SetActiveOn(false);
    m_pNPCParts.find("Part_InWeapon")->second->SetActiveOn(true);
}

void CNPC_David::RenderHandWeapon()
{
    m_pNPCParts.find("Part_Weapon")->second->SetActiveOn(true);
    m_pNPCParts.find("Part_InWeapon")->second->SetActiveOn(false);
}

void CNPC_David::ResetTimer()
{
    m_fCalculateTime1 = 0.f;
    m_fCalculateTime2 = 0.f;
    m_fCalculateTime3 = 0.f;
}

void CNPC_David::FSM_FirstMeet(float _fTimeDelta)
{
    // 기본 거리를 기반으로 매우 가까워지면 자동으로 
    //UpdateDistanceState();
}

void CNPC_David::FSM_MoveToTraining(float _fTimeDelta)
{
    if (m_bMoveToTraining1Trigger)
    {
        m_fCalculateTime1 += _fTimeDelta;
        //if (m_fCalculateTime1 > 20.f)
        if (m_fCalculateTime1 > m_fQuestSkipTime) // Default = 20.f -> QuestSkip = 1.f
        {
            m_fCalculateTime1 = 0.f;
            m_bMoveToTraining1Trigger = false;
            m_bMoveToTraining1 = true;
            m_pTransformCom->LookAtDir(_float3(m_vMovePos.x, m_vCurrentPos.y, m_vMovePos.z) -
                _float3(m_vCurrentPos.x, m_vCurrentPos.y, m_vCurrentPos.z));
            m_iActionState = DAVID_A_RUN;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
        }
    }
    // 경유지로 이동
    if (m_bMoveToTraining1)
    {
        if (!m_bMoveStart)
        {
            m_fCalculateTime1 += _fTimeDelta;
            if (m_fCalculateTime1 > 0.1f)
            {
                m_bMoveStart = true;
            }
        }
        if (m_bMoveStart)
        {
            m_pTransformCom->GoStraight(_fTimeDelta * 5.f);
            _float2 vTargetPosXZ = _float2(m_vMovePos.x, m_vMovePos.z);
            _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
            _float m_fDistnce = (vTargetPosXZ - vCurrentPosXZ).Length();
            if (m_fDistnce < 0.5f)
            {
                m_bMoveToTraining1 = false;
                m_bMoveToTraining2 = true;
                m_pTransformCom->LookAtDir(_float3(m_vTrainPos.x, m_vCurrentPos.y, m_vTrainPos.z) -
                    _float3(m_vCurrentPos.x, m_vCurrentPos.y, m_vCurrentPos.z));
            }
        }
    }
    // 훈련위치로 이동
    if (m_bMoveToTraining2)
    {
        m_pTransformCom->GoStraight(_fTimeDelta * 5.f);
        _float2 vTargetPosXZ = _float2(m_vTrainPos.x, m_vTrainPos.z);
        _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
        _float m_fDistnce = (vTargetPosXZ - vCurrentPosXZ).Length();
        if (m_fDistnce < 0.5f)
        {
            m_bMoveToTraining2 = false;
            m_pTransformCom->LookAtDir(_float3(m_vMovePos.x, m_vCurrentPos.y, m_vMovePos.z) -
                _float3(m_vTrainPos.x, m_vCurrentPos.y, m_vTrainPos.z));
            m_iActionState = DAVID_A_IDLE;
            m_pModel->ChangeAnim(m_iActionState, 0.5f, true);
            m_bInteractAble = true;
        }
    }
}

void CNPC_David::FSM_PracticeMeet1(float _fTimeDelta)
{
    // 기본상태에서 말걸면 행동이 바뀌기
    //if (m_iActionState == DAVID_A_IDLE)
    //{
    //    
    //}
}

void CNPC_David::FSM_PracticeMeet2(float _fTimeDelta)
{
    if (m_bTrainStartTiming)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_fCalculateTime1 > m_fToIdle)
        {
            m_fCalculateTime1 = 0.f;
            m_bTrainStartTiming = false;
            static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->SetDialogState(DAVIDMONSTER_DI_PRACTICEMEET2);
            static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->MovePos(m_vCurrentPos);
            static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->SimulationOn();
            MovePos(m_vCreatePos + _float3(0.f, 100.f, 0.f));
            SimulationOff();
        }
    }
    // Monster ver의 David이 행동이 끝나면 switch
    AfterSwitch();
}

void CNPC_David::FSM_PracticeMeet3(float _fTimeDelta)
{
    if (m_bTrainStartTiming)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_fCalculateTime1 > m_fToIdle)
        {
            m_fCalculateTime1 = 0.f;
            m_bTrainStartTiming = false;
            static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->SetDialogState(DAVIDMONSTER_DI_PRACTICEMEET3);
            static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->MovePos(m_vCurrentPos);
            static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->SimulationOn();
            MovePos(m_vCreatePos + _float3(0.f, 100.f, 0.f));
            SimulationOff();
        }
    }
    // Monster ver의 David이 행동이 끝나면 switch
    AfterSwitch();
}

void CNPC_David::FSM_PracticeMeet4(float _fTimeDelta)
{
    if (m_bTrainStartTiming)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_fCalculateTime1 > m_fToIdle)
        {
            m_fCalculateTime1 = 0.f;
            m_bTrainStartTiming = false;
            static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->SetDialogState(DAVIDMONSTER_DI_PRACTICEMEET4);
            static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->MovePos(m_vCurrentPos);
            static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->SimulationOn();
            MovePos(m_vCreatePos + _float3(0.f, 100.f, 0.f));
            SimulationOff();
        }
    }
    // Monster ver의 David이 행동이 끝나면 switch
    AfterSwitch();
}

void CNPC_David::FSM_PracticeMeet5(float _fTimeDelta)
{

}

void CNPC_David::FSM_MoveBackTo(float _fTimeDelta)
{
    // 경유지로 이동
    if (m_bMoveToBack1)
    {
        if (!m_bMoveStart)
        {
            m_fCalculateTime1 += _fTimeDelta;
            if (m_fCalculateTime1 > 0.1f)
            {
                m_bMoveStart = true;
            }
        }
        if (m_bMoveStart)
        {
            m_pTransformCom->GoStraight(_fTimeDelta * 5.f);
            _float2 vTargetPosXZ = _float2(m_vMovePos.x, m_vMovePos.z);
            _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
            _float m_fDistnce = (vTargetPosXZ - vCurrentPosXZ).Length();
            if (m_fDistnce < 0.5f)
            {
                m_bMoveToBack1 = false;
                m_bMoveToBack2 = true;
                m_pTransformCom->LookAtDir(_float3(m_vInitPos.x, m_vCurrentPos.y, m_vInitPos.z) -
                    _float3(m_vCurrentPos.x, m_vCurrentPos.y, m_vCurrentPos.z));
            }
        }
    }
    // 원래위치로 이동
    if (m_bMoveToBack2)
    {
        m_pTransformCom->GoStraight(_fTimeDelta * 5.f);
        _float2 vTargetPosXZ = _float2(m_vInitPos.x, m_vInitPos.z);
        _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
        _float m_fDistnce = (vTargetPosXZ - vCurrentPosXZ).Length();
        if (m_fDistnce < 0.5f)
        {
            m_bMoveToBack2 = false;
            m_pTransformCom->LookAtDir(_float3(m_vInitLookPos.x, m_vCurrentPos.y, m_vInitLookPos.z) -
                _float3(m_vInitPos.x, m_vCurrentPos.y, m_vInitPos.z));
            m_iActionState = DAVID_A_IDLE;
            m_pModel->ChangeAnim(m_iActionState, 0.5f, true);
            m_bInteractAble = true;
        }
    }
}

void CNPC_David::AfterSwitch()
{
    if (static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->GetSwitch())
    {
        m_bInteractAble = true;
        MovePos(static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->GetCurrentPos());
        SimulationOn();
        static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->GoSwitchPosition();
        static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->SetSwitch(false);

        // 역할끝난 몬스터 데이빗 꺼주기
        if (static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->GetDoingEnd())
        {
            static_pointer_cast<CNPC_MonsterDavid>(GAMEINSTANCE->GetGameObject(LEVEL_VILLAGE, TEXT("Layer_MonsterNPC")))->SetEnable(false);
        }
    }
}


void CNPC_David::InteractionOn()
{
    if (TEXT("잃어버린 기억") == QUESTMGR->GetCurrentQuestKey(QUEST_MAIN)) {
        DIALOGUEMGR->SetCurDialogue(L"David0");
        //Dialog Event Fire
        QUESTMGR->CheckDialogNPC(shared_from_this());
    }
    else if (TEXT("시작이 반이다") == QUESTMGR->GetCurrentQuestKey(QUEST_MAIN)) {
        wstring wstrDialogueKey = L"David" + to_wstring(m_iDialogOrder);
        m_iDialogOrder++;
        DIALOGUEMGR->SetCurDialogue(wstrDialogueKey);
        QUESTMGR->CheckDialogNPC(shared_from_this());
        m_bInteractAble = false;
        switch (m_iDialogOrder)
        {
        case DAVID_DI_PRACTICEMEET1:
            m_bInteractAble = true;
            m_iActionState = DAVID_A_TALK2;
            m_pModel->ChangeAnim(m_iActionState, m_fToTalk, true);
            break;
        case DAVID_DI_PRACTICEMEET2:
            m_iActionState = DAVID_A_TALK1;
            m_pModel->ChangeAnim(m_iActionState, m_fToTalk, true);
            break;
        case DAVID_DI_PRACTICEMEET3:
            m_iActionState = DAVID_A_TALK3;
            m_pModel->ChangeAnim(m_iActionState, m_fToTalk, true);
            break;
        case DAVID_DI_PRACTICEMEET4:
            m_iActionState = DAVID_A_TALK1;
            m_pModel->ChangeAnim(m_iActionState, m_fToTalk, true);
            break;
        case DAVID_DI_PRACTICEMEET5:
            m_iActionState = DAVID_A_TALK4;
            m_pModel->ChangeAnim(m_iActionState, m_fToTalk, true);
            /*Quest Clear*/
            QUESTMGR->ActiveEventTag(QUEST_MAIN, "Event_Training");
            break;
        }
    }
    else if ((TEXT("풋내기 모험가") == QUESTMGR->GetCurrentQuestKey(QUEST_SUB0) ||
        TEXT("풋내기 모험가") == QUESTMGR->GetCurrentQuestKey(QUEST_SUB1)))
    {
        QUESTMGR->CheckDialogNPC(shared_from_this());
        DIALOGUEMGR->SetCurDialogue(L"David6");
    }
    
    if (m_iDialogOrder == DAVID_DI_FIRSTMEET)
    {
        m_bInteractAble = false;
        m_iDialogOrder++;
        m_iActionState = DAVID_A_TALK1;
        m_pModel->ChangeAnim(m_iActionState, m_fToTalk, true);
    }
}