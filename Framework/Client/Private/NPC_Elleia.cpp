#include "NPC_Elleia.h"
#include "GameInstance.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "Animation.h"
#include "EffectMgr.h"
#include "UIMgr.h"
#include "UIPanel.h"
#include "GameMgr.h"

CNPC_Elleia::CNPC_Elleia()
    : CNPC()
{
}

HRESULT CNPC_Elleia::Initialize(_float3 _vCreatePos)
{
    // 설정된 위치에 생성
    __super::Initialize(_vCreatePos);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_ANIM;
    m_iShaderPass = 0;

    m_pModel = GAMEINSTANCE->GetModel("CT_Named_Elleia_SK");
    m_strModelName = "CT_Named_Elleia_SK";
    AddComponent(L"Com_Model", m_pModel);
    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

    m_pModel->ChangeAnim(m_iActionState, 0.1f, true);

    /*if (FAILED(AddPartObjects("")))
        return E_FAIL;*/

    m_fSpeed = 1.f;

    AddRigidBody("CT_Named_Elleia_SK");

    //m_pRigidBody->SimulationOn();

    PrepareNPCBar(L"Elleia");
    m_strDisplayName = L"Elleia";

    return S_OK;
}

void CNPC_Elleia::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);
}

void CNPC_Elleia::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    // Base상태에 따른 FSM
    switch (m_iBaseState)
    {
    case ELLEIA_B_BEFOREMEET:
        FSM_BeforeMeet(_fTimeDelta);
        break;
    case ELLEIA_B_MEET:
        FSM_Meet(_fTimeDelta);
        break;
    case ELLEIA_B_INTERACT:
        FSM_Interact(_fTimeDelta);
        break;
    }

    m_pModel->PlayAnimation(_fTimeDelta);

    m_pTransformCom->Tick(_fTimeDelta);
}

void CNPC_Elleia::LateTick(_float _fTimeDelta)
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

HRESULT CNPC_Elleia::Render()
{
    /*if (!m_pNPCParts.begin()->second->GetActiveOn())
        m_pNPCParts.begin()->second->SetActiveOn(true);*/
    __super::Render();
    _uint iNumMeshes = m_pModel->GetNumMeshes();

    for (UINT i = 0; i < iNumMeshes; i++) {
        /*머리, 눈썹*/
        if (i == 3 || i == 6) {

            //Pass 변경
            GAMEINSTANCE->ReadyShader(m_eShaderType, 1);
            GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

            //색 Bind
            GAMEINSTANCE->BindRawValue("g_vHairMainColor", &m_vHairMainColor, sizeof(_float3));
            GAMEINSTANCE->BindRawValue("g_vHairSubColor", &m_vHairSubColor, sizeof(_float3));

            GAMEINSTANCE->BeginAnimModel(m_pModel, i);
        }
        else if (i == 1)
        {
            continue;
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

shared_ptr<CNPC_Elleia> CNPC_Elleia::Create(_float3 _vCreatePos, _float3 _vInitLookPos)
{
    shared_ptr<CNPC_Elleia> pInstance = make_shared<CNPC_Elleia>();
    pInstance->SetInitLookPos(_vInitLookPos);
    if (FAILED(pInstance->Initialize(_vCreatePos)))
        MSG_BOX("Failed to Create : CNPC_Elleia");

    return pInstance;
}

HRESULT CNPC_Elleia::AddRigidBody(string _strModelKey)
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
    CapsuleDesc.strShapeTag = "NPC_Elleia_Body";
    RigidBodyDesc.pGeometry = &CapsuleDesc;

    RigidBodyDesc.eThisColFlag = COL_NPC;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

    m_pTransformCom->SetRigidBody(m_pRigidBody);
    m_pRigidBody->SetMass(20.0f);
    return S_OK;
}

void CNPC_Elleia::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CNPC_Elleia::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CNPC_Elleia::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

//HRESULT CNPC_Elleia::AddPartObjects(string _strModelKey)
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


void CNPC_Elleia::UpdateDistanceState()
{
    // 위치 바로 못받아오는거 때문에 우선
    if (m_PlayerPos == _float3(0.f, 0.f, 0.f))
    {
        m_iDistanceState = ELLEIA_D_FARRANGE;
        return;
    }

    if (m_fDistanceXZToPlayer < m_fMeetRange)
    {
        m_iDistanceState = ELLEIA_D_MEETRANGE;
    }
    else
    {
        m_iDistanceState = ELLEIA_D_FARRANGE;
    }
}

void CNPC_Elleia::FSM_BeforeMeet(_float _fTimeDelta)
{
    // 돌고나서 돌려주기
    if (m_iActionState == ELLEIA_A_TURNR80)
    {
        if (m_pModel->GetIsFinishedAnimation())
        {
            m_iActionState = ELLEIA_A_SOCLOOPINGS;
            m_pModel->ChangeAnim(m_iActionState, 0.f, false);
            m_pTransformCom->RotationPlus(_float3(0.f, 1.f, 0.f), 3.14f / 180.f * 80.f);
        }
    }
    if (m_iActionState == ELLEIA_A_TURNL80)
    {
        if (m_pModel->GetIsFinishedAnimation())
        {
            m_iActionState = ELLEIA_A_SOCLOOPINGS;
            m_pModel->ChangeAnim(m_iActionState, 0.f, false);
            m_pTransformCom->RotationPlus(_float3(0.f, 1.f, 0.f), 3.14f / 180.f * -80.f);
        }
    }
    if (m_iActionState == ELLEIA_A_TURNR130)
    {
        if (m_pModel->GetIsFinishedAnimation())
        {
            m_iActionState = ELLEIA_A_SOCLOOPINGS;
            m_pModel->ChangeAnim(m_iActionState, 0.f, false);
            m_pTransformCom->RotationPlus(_float3(0.f, 1.f, 0.f), 3.14f / 180.f * 130.f);
        }
    }
    if (m_iActionState == ELLEIA_A_TURNL130)
    {
        if (m_pModel->GetIsFinishedAnimation())
        {
            m_iActionState = ELLEIA_A_SOCLOOPINGS;
            m_pModel->ChangeAnim(m_iActionState, 0.f, false);
            m_pTransformCom->RotationPlus(_float3(0.f, 1.f, 0.f), 3.14f / 180.f * -130.f);
        }
    }

    if (m_iActionState == ELLEIA_A_SOCLOOPINGS)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_pModel->GetIsFinishedAnimation() && m_fCalculateTime1 > 0.1f)
        {
            m_fCalculateTime1 = 0.f;
            m_iActionState = ELLEIA_A_SOCLOOPINGL;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
        }
    }

    if (m_iActionState == ELLEIA_A_SOCLOOPINGL)
    {
        UpdateDistanceState();
        if (m_iDistanceState == ELLEIA_D_MEETRANGE)
        {
            // 보는 각도 36도 안에 플레이어가 있다면
            if (IsInPlayerDegree(m_fDegree1))
            {
                m_iBaseState = ELLEIA_B_MEET;
                m_bInteractAble = true;
            }
            // 108도 보다 안이면
            else if (IsInPlayerDegree(m_fDegree2))
            {
                // 플레이어가 오른쪽에 있으면
                if (IsPlayerRight())
                {
                    m_iActionState = ELLEIA_A_TURNR80;
                    m_pModel->ChangeAnim(m_iActionState, 0.5f, false);
                    m_iBaseState = ELLEIA_B_MEET;
                    m_bTurnRight = true;
                }
                else
                {
                    m_iActionState = ELLEIA_A_TURNL80;
                    m_pModel->ChangeAnim(m_iActionState, 0.5f, false);
                    m_iBaseState = ELLEIA_B_MEET;
                    m_bTurnLeft = true;
                }
            }
            // 179도 보다 안이면
            else if (IsInPlayerDegree(m_fDegree3))
            {
                // 플레이어가 오른쪽에 있으면
                if (IsPlayerRight())
                {
                    m_iActionState = ELLEIA_A_TURNR130;
                    m_pModel->ChangeAnim(m_iActionState, 0.5f, false);
                    m_iBaseState = ELLEIA_B_MEET;
                    m_bTurnRight130 = true;
                }
                else
                {
                    m_iActionState = ELLEIA_A_TURNL130;
                    m_pModel->ChangeAnim(m_iActionState, 0.5f, false);
                    m_iBaseState = ELLEIA_B_MEET;
                    m_bTurnLeft130 = true;
                }
            }
        }
    }
}

void CNPC_Elleia::FSM_Meet(_float _fTimeDelta)
{
    // 돌고나서 돌려주기
    if (m_iActionState == ELLEIA_A_TURNR80)
    {
        if (m_pModel->GetIsFinishedAnimation())
        {
            m_iActionState = ELLEIA_A_IDLE;
            m_pModel->ChangeAnim(m_iActionState, 0.f, true);
            m_pTransformCom->RotationPlus(_float3(0.f, 1.f, 0.f), 3.14f / 180.f * 80.f);
            m_bInteractAble = true;
        }
    }
    if (m_iActionState == ELLEIA_A_TURNL80)
    {
        if (m_pModel->GetIsFinishedAnimation())
        {
            m_iActionState = ELLEIA_A_IDLE;
            m_pModel->ChangeAnim(m_iActionState, 0.f, true);
            m_pTransformCom->RotationPlus(_float3(0.f, 1.f, 0.f), 3.14f / 180.f * -80.f);
            m_bInteractAble = true;
        }
    }
    if (m_iActionState == ELLEIA_A_TURNR130)
    {
        if (m_pModel->GetIsFinishedAnimation())
        {
            m_iActionState = ELLEIA_A_IDLE;
            m_pModel->ChangeAnim(m_iActionState, 0.f, true);
            m_pTransformCom->RotationPlus(_float3(0.f, 1.f, 0.f), 3.14f / 180.f * 130.f);
            m_bInteractAble = true;
        }
    }
    if (m_iActionState == ELLEIA_A_TURNL130)
    {
        if (m_pModel->GetIsFinishedAnimation())
        {
            m_iActionState = ELLEIA_A_IDLE;
            m_pModel->ChangeAnim(m_iActionState, 0.f, true);
            m_pTransformCom->RotationPlus(_float3(0.f, 1.f, 0.f), 3.14f / 180.f * -130.f);
            m_bInteractAble = true;
        }
    }

    if (m_iActionState == ELLEIA_A_IDLE)
    {
        UpdateDistanceState();
        // 다시 멀어지면
        if (m_iDistanceState == ELLEIA_D_FARRANGE)
        {
            if (m_bTurnRight)
            {
                m_iActionState = ELLEIA_A_TURNL80;
                m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
                m_iBaseState = ELLEIA_B_BEFOREMEET;
                m_bTurnRight = false;
                m_bInteractAble = false;
            }
            else if (m_bTurnLeft)
            {
                m_iActionState = ELLEIA_A_TURNR80;
                m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
                m_iBaseState = ELLEIA_B_BEFOREMEET;
                m_bTurnLeft = false;
                m_bInteractAble = false;
            }
            else if (m_bTurnRight130)
            {
                m_iActionState = ELLEIA_A_TURNL130;
                m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
                m_iBaseState = ELLEIA_B_BEFOREMEET;
                m_bTurnRight130 = false;
                m_bInteractAble = false;
            }
            else if (m_bTurnLeft130)
            {
                m_iActionState = ELLEIA_A_TURNR130;
                m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
                m_iBaseState = ELLEIA_B_BEFOREMEET;
                m_bTurnLeft130 = false;
                m_bInteractAble = false;
            }
            else
            {
                m_iBaseState = ELLEIA_B_BEFOREMEET;
                m_bInteractAble = false;
            }
        }
        // 가까우면
        else
        {
        }
    }
}

void CNPC_Elleia::FSM_Interact(_float _fTimeDelta)
{
    UpdateDistanceState();
    if (m_iDistanceState == ELLEIA_D_FARRANGE)
    {
        if (m_bTurnRight)
        {
            m_iActionState = ELLEIA_A_TURNL80;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
            m_iBaseState = ELLEIA_B_BEFOREMEET;
            m_bTurnRight = false;
            m_bInteractAble = false;
        }
        else if (m_bTurnLeft)
        {
            m_iActionState = ELLEIA_A_TURNR80;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
            m_iBaseState = ELLEIA_B_BEFOREMEET;
            m_bTurnLeft = false;
            m_bInteractAble = false;
        }
        else if (m_bTurnRight130)
        {
            m_iActionState = ELLEIA_A_TURNL130;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
            m_iBaseState = ELLEIA_B_BEFOREMEET;
            m_bTurnRight130 = false;
            m_bInteractAble = false;
        }
        else if (m_bTurnLeft130)
        {
            m_iActionState = ELLEIA_A_TURNR130;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
            m_iBaseState = ELLEIA_B_BEFOREMEET;
            m_bTurnLeft130 = false;
            m_bInteractAble = false;
        }
        else
        {
            m_iActionState = ELLEIA_A_SOCLOOPINGS;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
            m_iBaseState = ELLEIA_B_BEFOREMEET;
            m_bInteractAble = false;
        }
    }
}

void CNPC_Elleia::InteractionOn()
{
    if (m_iBaseState == ELLEIA_B_MEET)
    {
        m_iBaseState = ELLEIA_B_INTERACT;
        m_iActionState = ELLEIA_A_BASE;
        m_pModel->ChangeAnim(m_iActionState, 0.5f, true);
    }

    shared_ptr<CUIBase> foundUI = UIMGR->FindUI("CraftAllPanel");
    if (foundUI != nullptr)
    {
        if (foundUI->IsActivated() == false)
        {
            UIMGR->ActivateUI("CraftAllPanel");
            UIMGR->DeactivateHUD();
            CGameMgr::GetInstance()->ChangeToFreeMouse(1);
        }
    }
}