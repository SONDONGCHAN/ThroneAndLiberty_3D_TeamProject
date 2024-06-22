#include "NPC_Soldier.h"
#include "GameInstance.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "Animation.h"
#include "EffectMgr.h"
#include "UIMgr.h"

CNPC_Soldier::CNPC_Soldier()
    : CNPC()
{
}

HRESULT CNPC_Soldier::Initialize(_float3 _vCreatePos)
{
    // 설정된 위치에 생성
    __super::Initialize(_vCreatePos);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_ANIM;
    m_iShaderPass = 0;

    m_pModel = GAMEINSTANCE->GetModel("PatrolSoldier1");
    m_strModelName = "PatrolSoldier1";
    AddComponent(L"Com_Model", m_pModel);
    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

    switch (m_iGenType)
    {
    case SOLDIER_G_WALK200:
        m_iActionState = SOLDIER_A_IDLE;
        m_pTransformCom->SetSpeed(1.5f);
        break;
    case SOLDIER_G_WALK50:
        m_iActionState = SOLDIER_A_IDLE;
        m_pTransformCom->SetSpeed(0.5f);
        break;
    case SOLDIER_G_PRACTICE:
        m_iActionState = SOLDIER_A_ALT;
        break;
    case SOLDIER_G_TALK:
        m_iActionState = SOLDIER_A_TALK2;
        break;
    case SOLDIER_G_LAUGH:
        m_iActionState = SOLDIER_A_LAUGH2;
        break;
    case SOLDIER_G_IDLE:
        m_iActionState = SOLDIER_A_IDLE;
        break;
    }

    m_pModel->ChangeAnim(m_iActionState, 0.f, true);

    if (FAILED(AddPartObjects("IT_N_Spear_00007_SK")))
        return E_FAIL;

    m_fSpeed = 1.f;

    AddRigidBody("PatrolSoldier1");

    //m_pRigidBody->SimulationOn();

    PrepareNPCBar(L"Soldier");
    m_strDisplayName = L"Soldier";

    return S_OK;
}

void CNPC_Soldier::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);
}

void CNPC_Soldier::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    // 생성상태에 따른 FSM
    switch (m_iGenType)
    {
    case SOLDIER_G_WALK200:
        FSM_Walk1(_fTimeDelta);
        break;
    case SOLDIER_G_WALK50:
        FSM_Walk2(_fTimeDelta);
        break;
    case SOLDIER_G_PRACTICE:
        FSM_Practice(_fTimeDelta);
        break;
    case SOLDIER_G_TALK:
        FSM_Talk(_fTimeDelta);
        break;
    case SOLDIER_G_LAUGH:
        FSM_Laugh(_fTimeDelta);
        break;
    case SOLDIER_G_IDLE:
        FSM_Idle(_fTimeDelta);
        break;
    }

    m_pModel->PlayAnimation(_fTimeDelta);

    m_pTransformCom->Tick(_fTimeDelta);
}

void CNPC_Soldier::LateTick(_float _fTimeDelta)
{
    //m_pModelCom->CheckDisableAnim();

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

HRESULT CNPC_Soldier::Render()
{
    /*if (!m_pNPCParts.begin()->second->GetActiveOn())
        m_pNPCParts.begin()->second->SetActiveOn(true);*/
    __super::Render();
    _uint iNumMeshes = m_pModel->GetNumMeshes();

    for (UINT i = 0; i < iNumMeshes; i++) {
        /*머리, 눈썹*/
        //if (i == 3) {

        //    //Pass 변경
        //    GAMEINSTANCE->ReadyShader(m_eShaderType, 1);
        //    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

        //    //색 Bind
        //    GAMEINSTANCE->BindRawValue("g_vHairMainColor", &m_vHairMainColor, sizeof(_float3));
        //    GAMEINSTANCE->BindRawValue("g_vHairSubColor", &m_vHairSubColor, sizeof(_float3));

        //    GAMEINSTANCE->BeginAnimModel(m_pModel, i);
        //}
        // 망막, 몸
        if (i == 5 || i == 6)
        {
            continue;
        }
        // 안경 앞
        /*else if (i == 9)
        {
            continue;
        }*/
        else {
            GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
            GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
            GAMEINSTANCE->BeginAnimModel(m_pModel, i);
        }
    }

    /*if (m_pNPCParts.begin()->second->GetActiveOn())
        m_pNPCParts.begin()->second->SetActiveOn(false);*/

    return S_OK;
}

shared_ptr<CNPC_Soldier> CNPC_Soldier::Create(_float3 _vCreatePos, _float3 _vInitLookPos, _int _iGenType)
{
    shared_ptr<CNPC_Soldier> pInstance = make_shared<CNPC_Soldier>();
    pInstance->SetInitLookPos(_vInitLookPos);
    pInstance->SetPatrolPos(_vInitLookPos);
    pInstance->SetGenType(_iGenType);
    if (FAILED(pInstance->Initialize(_vCreatePos)))
        MSG_BOX("Failed to Create : CNPC_Soldier");

    return pInstance;
}

HRESULT CNPC_Soldier::AddRigidBody(string _strModelKey)
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
    CapsuleDesc.strShapeTag = "NPC_Soldier_Body";
    RigidBodyDesc.pGeometry = &CapsuleDesc;

    RigidBodyDesc.eThisColFlag = COL_NPC;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

    m_pTransformCom->SetRigidBody(m_pRigidBody);
    m_pRigidBody->SetMass(20.0f);
    return S_OK;
}

void CNPC_Soldier::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CNPC_Soldier::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CNPC_Soldier::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

HRESULT CNPC_Soldier::AddPartObjects(string _strModelKey)
{
    shared_ptr<CWeapon::WEAPON_DESC> pWeaponDesc = make_shared<CWeapon::WEAPON_DESC>();
    pWeaponDesc->tPartObject.pParentTransform = m_pTransformCom;
    pWeaponDesc->pSocketMatrix = m_pModel->GetCombinedBoneMatrixPtr("Bn_Action_LeftHand");
    pWeaponDesc->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(-90.f)) *
        SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(-90.f));
    pWeaponDesc->strModelKey = _strModelKey;

    shared_ptr<CTransform::TRANSFORM_DESC> pTransformDesc = make_shared<CTransform::TRANSFORM_DESC>();

    pTransformDesc->fSpeedPerSec = 10.f;
    pTransformDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    shared_ptr<CWeapon> pWeapon = CWeapon::Create(pWeaponDesc.get(), pTransformDesc.get());
    if (nullptr == pWeapon)
        return E_FAIL;

    shared_ptr<CPartObject> pPartObject = static_pointer_cast<CPartObject>(pWeapon);
    m_pNPCParts.emplace("Part_Weapon", pPartObject);

    return S_OK;
}

//HRESULT CNPC_Soldier::AddPartObjects(string _strModelKey)
//{
//    shared_ptr<CWeapon::WEAPON_DESC> pWeaponDesc = make_shared<CWeapon::WEAPON_DESC>();
//    pWeaponDesc->tPartObject.pParentTransform = m_pTransformCom;
//    pWeaponDesc->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_RightHand");
//    pWeaponDesc->strModelKey = _strModelKey;
//    pWeaponDesc->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(90.f)) *
//        SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(90.f));
//    
//    shared_ptr<CTransform::TRANSFORM_DESC> pTransformDesc = make_shared<CTransform::TRANSFORM_DESC>();
//
//    pTransformDesc->fSpeedPerSec = 10.f;
//    pTransformDesc->fRotationPerSec = XMConvertToRadians(90.0f);
//
//    shared_ptr<CWeapon> pWeapon = CWeapon::Create(pWeaponDesc.get(), pTransformDesc.get());
//    if (nullptr == pWeapon)
//        return E_FAIL;
//
//    shared_ptr<CPartObject> pPartObject = static_pointer_cast<CPartObject>(pWeapon);
//    pPartObject->SetActive(false);
//    m_pNPCParts.emplace("Part_Weapon", pPartObject);
//
//    return S_OK;
//}

void CNPC_Soldier::FSM_Walk1(_float _fTimeDelta)
{
    if (m_bPatrolDirection && (m_iActionState == SOLDIER_A_WALK200))
    {
        m_pTransformCom->GoStraight(_fTimeDelta);
        _float2 vTargetPosXZ = _float2(m_vPatrolPos.x, m_vPatrolPos.z);
        _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
        _float m_fDistnce = (vTargetPosXZ - vCurrentPosXZ).Length();

        if (m_fDistnce < 0.5f)
        {
            m_iActionState = SOLDIER_A_IDLE;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, true);

            m_bPatrolDirection = false;
        }
    }
    if (!m_bPatrolDirection && (m_iActionState == SOLDIER_A_WALK200))
    {
        m_pTransformCom->GoStraight(_fTimeDelta);
        _float2 vTargetPosXZ = _float2(m_vFirstCreatePos.x, m_vFirstCreatePos.z);
        _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
        _float m_fDistnce = (vTargetPosXZ - vCurrentPosXZ).Length();

        if (m_fDistnce < 0.5f)
        {
            m_iActionState = SOLDIER_A_IDLE;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, true);

            m_bPatrolDirection = true;
        }
    }
    if (m_iActionState == SOLDIER_A_IDLE)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_fCalculateTime1 > 3.f)
        {
            m_fCalculateTime1 = 0.f;
            if (m_bPatrolDirection)
            {
                m_pTransformCom->LookAtDir(_float3(m_vPatrolPos.x, 0.f, m_vPatrolPos.z) -
                    _float3(m_vFirstCreatePos.x, 0.f, m_vFirstCreatePos.z));
                m_iActionState = SOLDIER_A_WALK200;
                m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
            }
            else
            {
                m_pTransformCom->LookAtDir(_float3(m_vFirstCreatePos.x, 0.f, m_vFirstCreatePos.z) -
                    _float3(m_vPatrolPos.x, 0.f, m_vPatrolPos.z));
                m_iActionState = SOLDIER_A_WALK200;
                m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
            }
        }
    }
}

void CNPC_Soldier::FSM_Walk2(_float _fTimeDelta)
{
    if (m_bPatrolDirection && (m_iActionState == SOLDIER_A_WALK50))
    {
        m_pTransformCom->GoStraight(_fTimeDelta);
        _float2 vTargetPosXZ = _float2(m_vPatrolPos.x, m_vPatrolPos.z);
        _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
        _float m_fDistnce = (vTargetPosXZ - vCurrentPosXZ).Length();

        if (m_fDistnce < 0.5f)
        {
            m_iActionState = SOLDIER_A_IDLE;
            m_pModel->ChangeAnim(m_iActionState, 0.5f, true);

            m_bPatrolDirection = false;
        }
    }
    if (!m_bPatrolDirection && (m_iActionState == SOLDIER_A_WALK50))
    {
        m_pTransformCom->GoStraight(_fTimeDelta);
        _float2 vTargetPosXZ = _float2(m_vFirstCreatePos.x, m_vFirstCreatePos.z);
        _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
        _float m_fDistnce = (vTargetPosXZ - vCurrentPosXZ).Length();

        if (m_fDistnce < 0.5f)
        {
            m_iActionState = SOLDIER_A_IDLE;
            m_pModel->ChangeAnim(m_iActionState, 0.5f, true);

            m_bPatrolDirection = true;
        }
    }
    if (m_iActionState == SOLDIER_A_IDLE)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_fCalculateTime1 > 3.f)
        {
            m_fCalculateTime1 = 0.f;
            if (m_bPatrolDirection)
            {
                m_pTransformCom->LookAtDir(_float3(m_vPatrolPos.x, 0.f, m_vPatrolPos.z) -
                    _float3(m_vFirstCreatePos.x, 0.f, m_vFirstCreatePos.z));
                m_iActionState = SOLDIER_A_WALK50;
                m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
            }
            else
            {
                m_pTransformCom->LookAtDir(_float3(m_vFirstCreatePos.x, 0.f, m_vFirstCreatePos.z) -
                    _float3(m_vPatrolPos.x, 0.f, m_vPatrolPos.z));
                m_iActionState = SOLDIER_A_WALK50;
                m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
            }
        }
    }
}

void CNPC_Soldier::FSM_Practice(_float _fTimeDelta)
{
    if (m_iActionState == SOLDIER_A_ALT)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_fCalculateTime1 > 1.f)
        {
            m_fCalculateTime1 = 0.f;
            switch (GAMEINSTANCE->PickRandomInt(0, 3))
            {
            case 0:
                m_iActionState = SOLDIER_A_ATT1;
                break;
            case 1:
                m_iActionState = SOLDIER_A_ATT2;
                break;
            case 2:
                m_iActionState = SOLDIER_A_ATT3;
                break;
            }
            m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == SOLDIER_A_ATT1)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_pModel->GetIsFinishedAnimation() && m_fCalculateTime1>0.1f)
        {
            m_fCalculateTime1 = 0.f;
            m_iActionState = SOLDIER_A_ATT2;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == SOLDIER_A_ATT2)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_pModel->GetIsFinishedAnimation() && m_fCalculateTime1 > 0.1f)
        {
            m_fCalculateTime1 = 0.f;
            m_iActionState = SOLDIER_A_ATT3;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == SOLDIER_A_ATT3)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_pModel->GetIsFinishedAnimation() && m_fCalculateTime1 > 0.1f)
        {
            m_fCalculateTime1 = 0.f;
            m_iActionState = SOLDIER_A_ATT1;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
}

void CNPC_Soldier::FSM_Talk(_float _fTimeDelta)
{
}

void CNPC_Soldier::FSM_Laugh(_float _fTimeDelta)
{
}

void CNPC_Soldier::FSM_Idle(_float _fTimeDelta)
{
}

void CNPC_Soldier::InteractionOn()
{
}