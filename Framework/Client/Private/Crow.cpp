#include "Crow.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "Animation.h"
#include "EffectMgr.h"

CCrow::CCrow()
    : CNPC()
{
}

HRESULT CCrow::Initialize(_float3 _vCreatePos)
{
    // 설정된 위치에 생성
    __super::Initialize(_vCreatePos);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_ANIM;
    m_iShaderPass = 0;

    m_pModel = GAMEINSTANCE->GetModel("N_Crow_001_SK");
    m_strModelName = "N_Crow_001_SK";
    AddComponent(L"Com_Model", m_pModel);
    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

    m_pModel->ChangeAnim(m_iActionState, 0.1f, false);

    AddRigidBody("N_Crow_001_SK");

    m_vUIDistance = _float3(0.f, 1.f, 0.f);

    m_pRigidBody->SimulationOff();

    m_pTransformCom->SetSpeed(5.f);

    //PrepareNPCBar(L"Crow");
    
    m_IsEnabled = false;

    return S_OK;
}

void CCrow::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);
}

void CCrow::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);
    
    // Base상태에 따른 FSM
    switch (m_iBaseState)
    {
    case CROW_B_BEFOREMEET:
        FSM_BeforeMeet(_fTimeDelta);
        break;
    case CROW_B_MEET:
        FSM_Meet(_fTimeDelta);
        break;
    case CROW_B_INTERACT:
        FSM_Interact(_fTimeDelta);
        break;
    }

    m_pModel->PlayAnimation(_fTimeDelta);

    m_pTransformCom->Tick(_fTimeDelta);
}

void CCrow::LateTick(_float _fTimeDelta)
{
    //m_pModelCom->CheckDisableAnim();

    m_pTransformCom->LateTick(_fTimeDelta);

    __super::LateTick(_fTimeDelta);

    // frustrum
	if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 1.f))
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CCrow::Render()
{
    __super::Render();

    _uint iNumMeshes = m_pModel->GetNumMeshes();

    for (UINT i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
        GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
        GAMEINSTANCE->BeginAnimModel(m_pModel, i);
    }

    return S_OK;
}

shared_ptr<CCrow> CCrow::Create(_float3 _vCreatePos, _float3 _vInitLookPos, _int _iGenType)
{
    shared_ptr<CCrow> pInstance = make_shared<CCrow>();
    pInstance->SetInitLookPos(_vInitLookPos);
    pInstance->SetGenType(_iGenType);
    if (FAILED(pInstance->Initialize(_vCreatePos)))
        MSG_BOX("Failed to Create : CCrow");

    return pInstance;
}

HRESULT CCrow::AddRigidBody(string _strModelKey)
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

    GeometryBox RectDesc;
    RectDesc.vSize.x = 0.3f;
    RectDesc.vSize.y = 0.4f;
    RectDesc.vSize.z = 0.3f;
    RectDesc.vOffSetPosition = { 0.f, 0.2f, 0.f };
    RectDesc.vOffSetRotation = { 0.0f, 0.0f, 0.0f };
    RectDesc.strShapeTag = "Crow_Body";
    RigidBodyDesc.pGeometry = &RectDesc;
    
    RigidBodyDesc.eThisColFlag = COL_NPC;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

    m_pTransformCom->SetRigidBody(m_pRigidBody);
    m_pRigidBody->SetMass(10.0f);
    return S_OK;
}

void CCrow::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CCrow::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CCrow::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CCrow::UpdateDistanceState()
{
    // 위치 바로 못받아오는거 때문에 우선
    if (m_PlayerPos == _float3(0.f, 0.f, 0.f))
    {
        m_iDistanceState = CROW_D_FARRANGE;
        return;
    }

    if (m_fDistanceXZToPlayer < m_fMeetRange)
    {
        m_iDistanceState = CROW_D_MEETRANGE;
    }
    else
    {
        m_iDistanceState = CROW_D_FARRANGE;
    }
}

void CCrow::FSM_BeforeMeet(_float _fTimeDelta)
{
    UpdateDistanceState();
    if (m_iDistanceState == CROW_D_MEETRANGE)
    {
        m_iActionState = CROW_A_STARTFLY;
        m_pModel->ChangeAnim(m_iActionState, 0.5f, false);
        m_iBaseState = CROW_B_MEET;
        GAMEINSTANCE->PlaySoundW("CrowSound", 0.5f);
    }
    else if (m_iDistanceState == CROW_D_FARRANGE)
    {
        if (m_iActionState == CROW_A_EATLAND)
        {
            if (m_pModel->GetIsFinishedAnimation())
            {
                switch (GAMEINSTANCE->PickRandomInt(0, 2))
                {
                case 0:
                    m_iActionState = CROW_A_IDLE;
                    m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
                    break;
                case 1:
                    m_iActionState = CROW_A_IDLEMOTION;
                    m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
                    break;
                }
            }
        }
        if (m_iActionState == CROW_A_IDLE)
        {
            if (m_pModel->GetIsFinishedAnimation())
            {
                switch (GAMEINSTANCE->PickRandomInt(0, 2))
                {
                case 0:
                    m_iActionState = CROW_A_IDLEMOTION;
                    m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
                    break;
                case 1:
                    m_iActionState = CROW_A_EATLAND;
                    m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
                    break;
                }
            }
        }
        if (m_iActionState == CROW_A_IDLEMOTION)
        {
            if (m_pModel->GetIsFinishedAnimation())
            {
                switch (GAMEINSTANCE->PickRandomInt(0, 2))
                {
                case 0:
                    m_iActionState = CROW_A_IDLE;
                    m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
                    break;
                case 1:
                    m_iActionState = CROW_A_EATLAND;
                    m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
                    break;
                }
            }
        }
    }
}

void CCrow::FSM_Meet(_float _fTimeDelta)
{
    if (m_iActionState == CROW_A_STARTFLY)
    {
        if (m_pModel->GetIsFinishedAnimation())
        {
            m_iActionState = CROW_A_STARTFLY2;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == CROW_A_STARTFLY2)
    {
        if (m_pModel->GetIsFinishedAnimation())
        {
            m_pRigidBody->GravityOff();
            m_iActionState = CROW_A_FLY;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
        }
    }
    if (m_iActionState == CROW_A_FLY)
    {
        MovePos(m_vCurrentPos + m_pTransformCom->GetState(CTransform::STATE_LOOK) * 4.f * _fTimeDelta + _float3(0.f, 8.f, 0.f) * _fTimeDelta);
        m_fCalculateTime1 += _fTimeDelta;
        if (m_fCalculateTime1 > 5.f)
        {
            m_fCalculateTime1 = 0.f;
            m_iBaseState = CROW_B_BEFOREMEET;
            m_iDistanceState = CROW_D_FARRANGE;
            m_iActionState = CROW_A_EATLAND;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
            m_pRigidBody->GravityOn();
            SimulationOff();
            m_IsEnabled = false;
        }
    }
}

void CCrow::FSM_Interact(_float _fTimeDelta)
{

}

void CCrow::InteractionOn()
{
}