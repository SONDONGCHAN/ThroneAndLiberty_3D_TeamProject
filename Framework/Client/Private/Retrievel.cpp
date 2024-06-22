#include "Retrievel.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "Animation.h"
#include "EffectMgr.h"

CRetrievel::CRetrievel()
    : CNPC()
{
}

HRESULT CRetrievel::Initialize(_float3 _vCreatePos)
{
    // 설정된 위치에 생성
    __super::Initialize(_vCreatePos);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_ANIM;
    m_iShaderPass = 10;

    m_pModel = GAMEINSTANCE->GetModel("BD_Dog_Retriever_SK");
    m_strModelName = "BD_Dog_Retriever_SK";
    AddComponent(L"Com_Model", m_pModel);
    
    switch (m_iGenType)
    {
        // 대형
    case 0:
        m_pTransformCom->SetScaling(2.f, 2.f, 2.f);
        break;
        // 소형
    case 1:
        m_pTransformCom->SetScaling(1.f, 1.f, 1.f);
        break;
    }

    m_pModel->ChangeAnim(m_iActionState, 0.1f, true);

    //m_pTransformCom->Rotation(_float3(0.f, 1.f, 0.f), m_fInitRotation);

    AddRigidBody("BD_Dog_Retriever_SK");

    m_vUIDistance = _float3(0.f, 0.5f, 0.f);

    //m_pRigidBody->SimulationOff();
    
    //PrepareNPCBar(L"Retrievel");
    
    m_IsEnabled = false;

    m_fInteractDistance = 2.f;
    m_bInteractAble = true;

    m_pTransformCom->SetSpeed(m_fWalkSpeed);

    return S_OK;
}

void CRetrievel::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);
}

void CRetrievel::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);
    
    // Base상태에 따른 FSM
    switch (m_iBaseState)
    {
    case RETRIEVEL_B_BEFOREMEET:
        FSM_BeforeMeet(_fTimeDelta);
        break;
    case RETRIEVEL_B_MEET:
        FSM_Meet(_fTimeDelta);
        break;
    case RETRIEVEL_B_INTERACT:
        FSM_Interact(_fTimeDelta);
        break;
    }

    m_pModel->PlayAnimation(_fTimeDelta);

    m_pTransformCom->Tick(_fTimeDelta);
}

void CRetrievel::LateTick(_float _fTimeDelta)
{
    //m_pModelCom->CheckDisableAnim();

    m_pTransformCom->LateTick(_fTimeDelta);

    __super::LateTick(_fTimeDelta);

    // frustrum
	if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 4.f))
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CRetrievel::Render()
{
    __super::Render();

    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    _uint iNumMeshes = m_pModel->GetNumMeshes();

    for (UINT i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->BeginAnimModel(m_pModel, i);
    }

    return S_OK;
}

shared_ptr<CRetrievel> CRetrievel::Create(_float3 _vCreatePos, _float3 _vInitLookPos, _int _iGenType)
{
    shared_ptr<CRetrievel> pInstance = make_shared<CRetrievel>();
    pInstance->SetInitLookPos(_vInitLookPos);
    pInstance->SetGenType(_iGenType);
    if (FAILED(pInstance->Initialize(_vCreatePos)))
        MSG_BOX("Failed to Create : CRetrievel");

    return pInstance;
}

HRESULT CRetrievel::AddRigidBody(string _strModelKey)
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
    RectDesc.vSize.x = 1.f;
    RectDesc.vSize.y = 1.6f;
    RectDesc.vSize.z = 1.f;
    RectDesc.vOffSetPosition = { 0.f, 0.8f, 0.f };
    RectDesc.vOffSetRotation = { 0.0f, 0.0f, 0.0f };
    RectDesc.strShapeTag = "Retrievel_Body";
    RigidBodyDesc.pGeometry = &RectDesc;

    RigidBodyDesc.eThisColFlag = COL_NPC;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

    m_pTransformCom->SetRigidBody(m_pRigidBody);
    m_pRigidBody->SetMass(1.0f);
    return S_OK;
}

void CRetrievel::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    if (_ColData.eColFlag & COL_STATIC || _ColData.eColFlag & COL_PLAYER)
        return;
}

void CRetrievel::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CRetrievel::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CRetrievel::UpdateDistanceState()
{
    // 위치 바로 못받아오는거 때문에 우선
    if (m_PlayerPos == _float3(0.f, 0.f, 0.f))
    {
        m_iDistanceState = RETRIEVEL_D_FARRANGE;
        return;
    }

    if (m_fDistanceXZToPlayer < m_fMeetRange)
    {
        m_iDistanceState = RETRIEVEL_D_MEETRANGE;
    }
    else
    {
        m_iDistanceState = RETRIEVEL_D_FARRANGE;
    }
}

void CRetrievel::FSM_BeforeMeet(_float _fTimeDelta)
{   
    if (m_iActionState == RETRIEVEL_A_TALK && m_bFirstInit)
    {
        m_bFirstInit = false;
        m_iActionState = RETRIEVEL_A_IDLE;
        m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
    }

    if (m_iActionState == RETRIEVEL_A_IDLE && m_bInteractDog)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_fCalculateTime1 > 2.f)
        {
            m_bInteractAble = true;
            m_bInteractDog = false;
            // 방향 원래로 전환
            //m_pTransformCom->Rotation(_float3(0.f, 1.f, 0.f), m_fInitRotation);
        }
    }
}

void CRetrievel::FSM_Meet(_float _fTimeDelta)
{
    if (m_iActionState == RETRIEVEL_A_WALK)
    {
        m_pTransformCom->GoStraight(_fTimeDelta);
        m_fCalculateTime1 += _fTimeDelta;
        if (m_fCalculateTime1 > 30.f)
        {
            m_fCalculateTime1 = 0.f;
            SimulationOff();
            m_IsEnabled = false;
        }
    }

    if (m_iActionState == RETRIEVEL_A_IDLE)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_fCalculateTime1 > 2.f)
        {
            // 방향 가던길로 전환하기
            m_pTransformCom->Rotation(_float3(0.f, 1.f, 0.f), m_fInitRotation);
            m_iActionState = RETRIEVEL_A_WALK;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
        }
    }
}

void CRetrievel::FSM_Interact(_float _fTimeDelta)
{
    if (m_iActionState == RETRIEVEL_A_CHARM1)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_pModel->GetIsFinishedAnimation() && m_fCalculateTime1 > 0.1f)
        {
            m_fCalculateTime1 = 0.f;
            m_iActionState = RETRIEVEL_A_CHARM2;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == RETRIEVEL_A_CHARM2)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_pModel->GetIsFinishedAnimation() && m_fCalculateTime1 > 0.1f)
        {
            m_fCalculateTime1 = 0.f;
            m_iActionState = RETRIEVEL_A_CHARM3;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
        }
    }
    if (m_iActionState == RETRIEVEL_A_CHARM3)
    {
        m_fCalculateTime1 += _fTimeDelta;
        if (m_pModel->GetIsFinishedAnimation() && m_fCalculateTime1 > 0.1f)
        {
            m_fCalculateTime1 = 0.f;
            m_iBaseState = RETRIEVEL_B_BEFOREMEET;
            m_iActionState = RETRIEVEL_A_IDLE;
            m_pModel->ChangeAnim(m_iActionState, 0.1f, true);
        }
    }
}

void CRetrievel::InteractionOn()
{
    if (m_iBaseState == RETRIEVEL_B_BEFOREMEET && !m_bInteractDog)
    {
        m_bInteractAble = false;
        m_bInteractDog = true;
        GAMEINSTANCE->PlaySoundW("DogSound", 0.5f);
        UpdateDirection();
        m_iBaseState = RETRIEVEL_B_INTERACT;
        m_iActionState = RETRIEVEL_A_CHARM1;
        m_pModel->ChangeAnim(m_iActionState, 0.1f, false);
    }
}