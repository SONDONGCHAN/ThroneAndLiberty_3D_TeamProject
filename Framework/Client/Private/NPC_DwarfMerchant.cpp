#include "NPC_DwarfMerchant.h"
#include "GameInstance.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "Animation.h"
#include "EffectMgr.h"
#include "UIMgr.h"

CNPC_DwarfMerchant::CNPC_DwarfMerchant()
    : CNPC()
{
}

HRESULT CNPC_DwarfMerchant::Initialize(_float3 _vCreatePos)
{
    // 설정된 위치에 생성
    __super::Initialize(_vCreatePos);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_ANIM;
    m_iShaderPass = 0;

    m_pModel = GAMEINSTANCE->GetModel("CT_Named_GidroKoi_SK");
    m_strModelName = "CT_Named_GidroKoi_SK";
    AddComponent(L"Com_Model", m_pModel);
    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

    switch (m_iGenType)
    {
    case DWARFMERCHANT_G_IDLE:
        m_iActionState = DWARFMERCHANT_A_IDLE;
        m_vHairMainColor = _float3(0.02f, 0.02f, 0.07f);
        break;
    case DWARFMERCHANT_G_LAUGH:
        m_iActionState = DWARFMERCHANT_A_LAUGH;
        m_vHairMainColor = _float3(0.02f, 0.02f, 0.07f);
        break;
    case DWARFMERCHANT_G_TALK:
        m_iActionState = DWARFMERCHANT_A_TALK;
        m_vHairMainColor = _float3(0.02f, 0.02f, 0.07f);
        break;
    case DWARFMERCHANT_G_WALK:
        m_iActionState = DWARFMERCHANT_A_WALK;
        m_vHairMainColor = _float3(0.02f, 0.02f, 0.07f);
        break;
    }

    m_pModel->ChangeAnim(m_iActionState, 0.1f, true);

    /*if (FAILED(AddPartObjects("")))
        return E_FAIL;*/

    m_fSpeed = 1.f;
    m_pTransformCom->SetSpeed(2.f);

    AddRigidBody("CT_Named_GidroKoi_SK");

    //m_pRigidBody->SimulationOn();

    PrepareNPCBar(L"DwarfMerchant");
    m_strDisplayName = L"DwarfMerchant";

    return S_OK;
}

void CNPC_DwarfMerchant::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);
}

void CNPC_DwarfMerchant::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    // 생성상태에 따른 FSM
    switch (m_iGenType)
    {
    case DWARFMERCHANT_G_WALK:
        FSM_Walk(_fTimeDelta);
        break;
    default:
        FSM_BeforeMeet(_fTimeDelta);
        break;
    }

    m_pModel->PlayAnimation(_fTimeDelta);

    m_pTransformCom->Tick(_fTimeDelta);
}

void CNPC_DwarfMerchant::LateTick(_float _fTimeDelta)
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

HRESULT CNPC_DwarfMerchant::Render()
{
    /*if (!m_pNPCParts.begin()->second->GetActiveOn())
        m_pNPCParts.begin()->second->SetActiveOn(true);*/
    __super::Render();
    _uint iNumMeshes = m_pModel->GetNumMeshes();

    for (UINT i = 0; i < iNumMeshes; i++) {
        /*머리, 눈썹*/
        if (i == 3) {

            //Pass 변경
            GAMEINSTANCE->ReadyShader(m_eShaderType, 1);
            GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

            //색 Bind
            GAMEINSTANCE->BindRawValue("g_vHairMainColor", &m_vHairMainColor, sizeof(_float3));
            GAMEINSTANCE->BindRawValue("g_vHairSubColor", &m_vHairSubColor, sizeof(_float3));

            GAMEINSTANCE->BeginAnimModel(m_pModel, i);
        }
        // 망막
        else if (i == 0)
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

shared_ptr<CNPC_DwarfMerchant> CNPC_DwarfMerchant::Create(_float3 _vCreatePos, _float3 _vInitLookPos, _int _iGenType)
{
    shared_ptr<CNPC_DwarfMerchant> pInstance = make_shared<CNPC_DwarfMerchant>();
    pInstance->SetInitLookPos(_vInitLookPos);
    pInstance->SetPatrolPos(_vInitLookPos);
    pInstance->SetGenType(_iGenType);
    if (FAILED(pInstance->Initialize(_vCreatePos)))
        MSG_BOX("Failed to Create : CNPC_DwarfMerchant");

    return pInstance;
}

HRESULT CNPC_DwarfMerchant::AddRigidBody(string _strModelKey)
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
    CapsuleDesc.strShapeTag = "NPC_DwarfMerchant_Body";
    RigidBodyDesc.pGeometry = &CapsuleDesc;

    RigidBodyDesc.eThisColFlag = COL_NPC;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

    m_pTransformCom->SetRigidBody(m_pRigidBody);
    m_pRigidBody->SetMass(20.0f);
    return S_OK;
}

void CNPC_DwarfMerchant::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CNPC_DwarfMerchant::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CNPC_DwarfMerchant::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

//HRESULT CNPC_DwarfMerchant::AddPartObjects(string _strModelKey)
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

void CNPC_DwarfMerchant::FSM_BeforeMeet(_float _fTimeDelta)
{
}

void CNPC_DwarfMerchant::FSM_Meet(_float _fTimeDelta)
{
}

void CNPC_DwarfMerchant::FSM_Walk(_float _fTimeDelta)
{
    if (m_bPatrolDirection && (m_iActionState == DWARFMERCHANT_A_WALK))
    {
        m_pTransformCom->GoStraight(_fTimeDelta);
        _float2 vTargetPosXZ = _float2(m_vPatrolPos.x, m_vPatrolPos.z);
        _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
        _float m_fDistnce = (vTargetPosXZ - vCurrentPosXZ).Length();

        if (m_fDistnce < 0.5f)
        {
            m_iActionState = DWARFMERCHANT_A_IDLE;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, true);

            m_bPatrolDirection = false;
        }
    }
    if(!m_bPatrolDirection && (m_iActionState == DWARFMERCHANT_A_WALK))
    {
        m_pTransformCom->GoStraight(_fTimeDelta);
        _float2 vTargetPosXZ = _float2(m_vFirstCreatePos.x, m_vFirstCreatePos.z);
        _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
        _float m_fDistnce = (vTargetPosXZ - vCurrentPosXZ).Length();
        
        if (m_fDistnce < 0.5f)
        {
            m_iActionState = DWARFMERCHANT_A_IDLE; 
            m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
            
            m_bPatrolDirection = true;
        }
    }
    if (m_iActionState == DWARFMERCHANT_A_IDLE)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_fCalculateTime1 > 5.f)
        {
            m_fCalculateTime1 = 0.f;
            if (m_bPatrolDirection)
            {
                m_pTransformCom->LookAtDir(_float3(m_vPatrolPos.x, 0.f, m_vPatrolPos.z) -
                    _float3(m_vFirstCreatePos.x, 0.f, m_vFirstCreatePos.z));
                m_iActionState = DWARFMERCHANT_A_WALK;
                m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
            }
            else
            {
                m_pTransformCom->LookAtDir(_float3(m_vFirstCreatePos.x, 0.f, m_vFirstCreatePos.z) -
                    _float3(m_vPatrolPos.x, 0.f, m_vPatrolPos.z));
                m_iActionState = DWARFMERCHANT_A_WALK;
                m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
            }
        }
    }
}

void CNPC_DwarfMerchant::FSM_Interact(_float _fTimeDelta)
{
}

void CNPC_DwarfMerchant::InteractionOn()
{
}