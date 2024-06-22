#include "NPC_Lottie.h"
#include "GameInstance.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "Animation.h"
#include "EffectMgr.h"

CNPC_Lottie::CNPC_Lottie()
    : CNPC()
{
}

HRESULT CNPC_Lottie::Initialize(_float3 _vCreatePos)
{
    // 설정된 위치에 생성
    __super::Initialize(_vCreatePos);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_ANIM;
    m_iShaderPass = 0;

    m_pModel = GAMEINSTANCE->GetModel("CT_Named_Lottie_SK");
    m_strModelName = "CT_Named_Lottie_SK";
    AddComponent(L"Com_Model", m_pModel);
    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

    m_pModel->ChangeAnim(m_iActionState, 0.1f, true);

    /*if (FAILED(AddPartObjects("")))
        return E_FAIL;*/

    m_fSpeed = 1.f;

    AddRigidBody("CT_Named_Lottie_SK");

    //m_pRigidBody->SimulationOn();

    //m_vLockOnOffSetPosition = { 0.0f,0.8f,0.0f };

    PrepareNPCBar(L"Lottie");
    m_strDisplayName = L"Lottie";

    return S_OK;
}

void CNPC_Lottie::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);
}

void CNPC_Lottie::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    // Base상태에 따른 FSM
    switch (m_iBaseState)
    {
    case LOTTIE_B_BEFOREMEET:
        FSM_BeforeMeet(_fTimeDelta);
        break;
    case LOTTIE_B_MEET:
        FSM_Meet(_fTimeDelta);
        break;
    case LOTTIE_B_INTERACT:
        FSM_Interact(_fTimeDelta);
        break;
    }

    m_pModel->PlayAnimation(_fTimeDelta);

    m_pTransformCom->Tick(_fTimeDelta);
}

void CNPC_Lottie::LateTick(_float _fTimeDelta)
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

HRESULT CNPC_Lottie::Render()
{
    /*if (!m_pNPCParts.begin()->second->GetActiveOn())
        m_pNPCParts.begin()->second->SetActiveOn(true);*/
    __super::Render();
    _uint iNumMeshes = m_pModel->GetNumMeshes();

    for (UINT i = 0; i < iNumMeshes; i++) {
        /*머리, 눈썹*/
        if (i == 4 /*|| i == 5*/) {

            //Pass 변경
            GAMEINSTANCE->ReadyShader(m_eShaderType, 1);
            GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

            //색 Bind
            GAMEINSTANCE->BindRawValue("g_vHairMainColor", &m_vHairMainColor, sizeof(_float3));
            GAMEINSTANCE->BindRawValue("g_vHairSubColor", &m_vHairSubColor, sizeof(_float3));

            GAMEINSTANCE->BeginAnimModel(m_pModel, i);
        }
        else if (i == 5)
        {
            continue;
        }
        // 망막
        else if (i == 6)
        {
            continue;
        }
        // 속눈썹
        else if (i == 7)
        {
            continue;
            ////Pass 변경
            //GAMEINSTANCE->ReadyShader(m_eShaderType, 1);
            //GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

            ////색 Bind
            //GAMEINSTANCE->BindRawValue("g_vHairMainColor", &m_vEyeBrowMainColor, sizeof(_float3));
            //GAMEINSTANCE->BindRawValue("g_vHairSubColor", &m_vEyeBrowSubColor, sizeof(_float3));

            //GAMEINSTANCE->BeginAnimModel(m_pModel, i);

            ////원상복구
            //GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
        }
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

shared_ptr<CNPC_Lottie> CNPC_Lottie::Create(_float3 _vCreatePos, _float3 _vInitLookPos, _float3 _vCheckPoint1, _float3 _vCheckPoint2, _float3 _vCheckPoint3)
{
    shared_ptr<CNPC_Lottie> pInstance = make_shared<CNPC_Lottie>();
    pInstance->SetInitLookPos(_vInitLookPos);
    if (FAILED(pInstance->Initialize(_vCreatePos)))
        MSG_BOX("Failed to Create : CNPC_Lottie");

    return pInstance;
}


HRESULT CNPC_Lottie::AddRigidBody(string _strModelKey)
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
    CapsuleDesc.strShapeTag = "NPC_Lottie_Body";
    RigidBodyDesc.pGeometry = &CapsuleDesc;

    RigidBodyDesc.eThisColFlag = COL_NPC;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

    m_pTransformCom->SetRigidBody(m_pRigidBody);
    m_pRigidBody->SetMass(20.0f);
    return S_OK;
}

void CNPC_Lottie::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CNPC_Lottie::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CNPC_Lottie::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

//HRESULT CNPC_Lottie::AddPartObjects(string _strModelKey)
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


void CNPC_Lottie::UpdateDistanceState()
{
    // 위치 바로 못받아오는거 때문에 우선
    if (m_PlayerPos == _float3(0.f, 0.f, 0.f))
    {
        m_iDistanceState = LOTTIE_D_FARRANGE;
        return;
    }

    if (m_fDistanceXZToPlayer < m_fMeetRange)
    {
        m_iDistanceState = LOTTIE_D_MEETRANGE;
    }
    else
    {
        m_iDistanceState = LOTTIE_D_FARRANGE;
    }
}

void CNPC_Lottie::FSM_BeforeMeet(_float _fTimeDelta)
{
    if (m_iActionState == LOTTIE_A_IDLE)
    {
        UpdateDistanceState();
        if (m_iDistanceState == LOTTIE_D_MEETRANGE)
        {
            m_iBaseState = LOTTIE_B_MEET;
            m_bInteractAble = true;
        }
    }
}

void CNPC_Lottie::FSM_Meet(_float _fTimeDelta)
{
    if (m_iActionState == LOTTIE_A_IDLE)
    {
        UpdateDistanceState();
        if (m_iDistanceState == LOTTIE_D_FARRANGE)
        {
            m_iBaseState = LOTTIE_B_BEFOREMEET;
            m_bInteractAble = false;
        }
    }
}

void CNPC_Lottie::FSM_Interact(_float _fTimeDelta)
{
    UpdateDistanceState();
    if (m_iDistanceState == LOTTIE_D_FARRANGE)
    {
        m_iActionState = LOTTIE_A_IDLE;
        m_pModel->ChangeAnim(m_iActionState, 0.5f, true);
        m_iBaseState = LOTTIE_B_BEFOREMEET;
        m_bInteractAble = false;
    }
}

void CNPC_Lottie::InteractionOn()
{
    if (m_iBaseState == LOTTIE_B_MEET)
    {
        m_iBaseState = LOTTIE_B_INTERACT;
        m_iActionState = LOTTIE_A_TALK1;
        m_pModel->ChangeAnim(m_iActionState, 0.5f, true);
    }
     
    if (TEXT("물건 전달해주기") == QUESTMGR->GetCurrentQuestKey(QUEST_SUB0) ||
        TEXT("물건 전달해주기") == QUESTMGR->GetCurrentQuestKey(QUEST_SUB1)) {
        QUESTMGR->CheckDialogNPC(shared_from_this());
        DIALOGUEMGR->SetCurDialogue(TEXT("Lottie0"));

    }
    else if((TEXT("사라진 약혼자") == QUESTMGR->GetCurrentQuestKey(QUEST_SUB0) ||
        TEXT("사라진 약혼자") == QUESTMGR->GetCurrentQuestKey(QUEST_SUB1)))
    {
        QUESTMGR->CheckDialogNPC(shared_from_this());
        DIALOGUEMGR->SetCurDialogue(TEXT("Lottie1"));

    }

}