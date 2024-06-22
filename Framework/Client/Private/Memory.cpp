#include "Memory.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "EffectMgr.h"
#include "UIMgr.h"
#include "UIBackGround.h"

#include "CameraMgr.h"

CMemory::CMemory()
{
}

HRESULT CMemory::Initialize(_float3 _vPosition)
{
    CInteractionObj::Initialize(nullptr);

    switch (m_iGenType)
    {
    case MEMORY_MIDGET:
        m_fMoveTime = 1.5f;
        break;
    case MEMORY_ORC:
        m_fMoveTime = 1.f;
        break;
    }

    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;
    
    m_fInteractDistance = 2.f;
    m_vUIDistance = _float3(0.f, 0.8f, 0.f);

    m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

    m_bInteractAble = false;

    AddRigidBody();

    SimulationOff();

    m_IsEnabled = false;
    
    return S_OK;
}

void CMemory::PriorityTick(_float _fTimeDelta)
{
    m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
    
    __super::PriorityTick(_fTimeDelta);
}

void CMemory::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    // 튀어나옴
    if (m_bUp)
    {
        m_pTransformCom->Jump(400.0f);
        m_bUp = false;
        m_bMove = true;
        m_pTransformCom->SetState(CTransform::STATE_POSITION, m_vCreatePos);
        m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));

        EFFECTMGR->PlayEffect("Memory3", shared_from_this());
        EFFECTMGR->PlayEffect("Memory_P", shared_from_this());
    }
    
    // 움직이다가 땅에 부딪치면 움직이기 멈춤
    if (m_bMove)
    {
        m_fCalculateTime += _fTimeDelta;
        if (m_fCalculateTime > m_fMoveTime)
        {
            m_fCalculateTime = 0.f;
            m_bMove = false;
            m_bInteractAble = true;
        }

        m_pTransformCom->SetPositionParam(0, m_vLookDir.x * 1.5f * _fTimeDelta + m_vCurrentPos.x);
        m_pTransformCom->SetPositionParam(2, m_vLookDir.z * 1.5f * _fTimeDelta + m_vCurrentPos.z);
    }

    m_pTransformCom->Tick(_fTimeDelta);

    if (m_IsInteractionDone) {
        m_fEventAccTime += _fTimeDelta;

        if (m_fEventAccTime >= 3.f && !m_IsNoiseDone) {

            weak_ptr<CCamera> pCamera =  CCameraMgr::GetInstance()->FindCamera("PlayerCamera");

            if (pCamera.lock()) {
                pCamera.lock()->LerpFovy(XMConvertToRadians(2.f), 1.9f);
            }

            GAMEINSTANCE->SetScreenNoise(false);
            GAMEINSTANCE->SetZoomBlur(1.6f, 0.4f, m_pTransformCom->GetState(CTransform::STATE_POSITION));

            m_IsNoiseDone = true;
        }


        if (m_fEventAccTime >= 4.6f && !m_IsZoomDone) {

            GAMEINSTANCE->SetFadeEffect(true, 2.f);
            m_IsZoomDone = true;
        }

        if (m_fEventAccTime >= 6.6f && !m_IsFadeEvent) {

            GAMEINSTANCE->StopAll();

            if (MEMORY_MIDGET == m_iGenType) 
            {
                GAMEINSTANCE->PlayVideo(TEXT("..\\..\\Client\\Bin\\Videos\\기억조각_고블린필드.wmv"));
            }
            else if (MEMORY_ORC == m_iGenType) 
            {
                GAMEINSTANCE->PlayVideo(TEXT("..\\..\\Client\\Bin\\Videos\\기억조각_오크필드.wmv"));
            }

            GAMEINSTANCE->LevelBGMOn();

            GAMEINSTANCE->PlaySoundW("SFX_After_Get_Memory", 1.2f);

            GAMEINSTANCE->SetChromaticAberration(false);
            GAMEINSTANCE->SetFadeEffect(false, 2.f);

            weak_ptr<CCamera> pCamera = CCameraMgr::GetInstance()->FindCamera("PlayerCamera");

            if (pCamera.lock()) {
                CCameraMgr::GetInstance()->CameraTurnOn("PlayerCamera");
                pCamera.lock()->LerpFovy(XMConvertToRadians(60.f), 0.1f);

            }

            m_IsFadeEvent = true;
            m_IsActive = false;
        }


    }

}

void CMemory::LateTick(_float _fTimeDelta)
{
    m_pTransformCom->LateTick(_fTimeDelta);

    __super::LateTick(_fTimeDelta);

    if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 2.0f))
    {
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
#ifdef _DEBUG
        if (m_pRigidBody)
            GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
    }
}

HRESULT CMemory::Render()
{
    if(!m_IsEnabled)
        return S_OK;

    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    return S_OK;
}

void CMemory::InteractionOn()
{
    m_bInteractAble = false;

    //GAMEINSTANCE->SetZoomBlur()

    GAMEINSTANCE->LevelBGMOff();
    GAMEINSTANCE->PlaySoundW("SFX_Get_Memory", 1.3f);

    m_IsInteractionDone = true;

   // GAMEINSTANCE->SetZoomBlur(3.f, 0.4f, m_pTransformCom->GetState(CTransform::STATE_POSITION));
    GAMEINSTANCE->SetChromaticAberration(true);
    GAMEINSTANCE->SetScreenNoise(true);
    //if (MEMORY_MIDGET == m_iGenType) 
    //{
    //    GAMEINSTANCE->PlayVideo(TEXT("..\\..\\Client\\Bin\\Videos\\기억조각_고블린필드.wmv"));
    //}
    //else if (MEMORY_ORC == m_iGenType) 
    //{
    //    GAMEINSTANCE->PlayVideo(TEXT("..\\..\\Client\\Bin\\Videos\\기억조각_오크필드.wmv"));
    //}

    weak_ptr<CCamera> pCamera = CCameraMgr::GetInstance()->FindCamera("PlayerCamera");

    if (pCamera.lock()) {
        pCamera.lock()->LerpFovy(XMConvertToRadians(85.f), 3.f);
    }

    //GAMEINSTANCE->LevelBGMOn();

    if (m_iGenType == MEMORY_MIDGET)
    {
        weak_ptr<CUIBackGround> pUI = dynamic_pointer_cast<CUIBackGround>(UIMGR->FindUI("PCInfoIconBG"));
        if (!pUI.expired())
        {
            pUI.lock()->SetTexIndex(1);
        }
    }

    else if (m_iGenType == MEMORY_ORC)
    {
        weak_ptr<CUIBackGround> pUI = dynamic_pointer_cast<CUIBackGround>(UIMGR->FindUI("PCInfoIconBG"));
        if (!pUI.expired())
        {
            pUI.lock()->SetTexIndex(2);
        }
    }
}

HRESULT CMemory::AddRigidBody()
{
    CRigidBody::RIGIDBODYDESC RigidBodyDesc;

    RigidBodyDesc.isStatic = false;
    RigidBodyDesc.isTrigger = false;
    RigidBodyDesc.isGravity = true;
    RigidBodyDesc.isInitCol = true;
    RigidBodyDesc.pTransform = m_pTransformCom;
    RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;

    RigidBodyDesc.pOwnerObject = shared_from_this();
    RigidBodyDesc.fStaticFriction = 1.0f;
    RigidBodyDesc.fDynamicFriction = 0.0f;
    RigidBodyDesc.fRestitution = 0.7f;
    
    GeometryBox RectDesc;
    RectDesc.vSize.x = 0.6f;
    RectDesc.vSize.y = 0.6f;
    RectDesc.vSize.z = 0.6f;
    RectDesc.vOffSetPosition = { 0.f, 0.3f, 0.f };
    RectDesc.vOffSetRotation = { 0.0f, 0.0f, 0.f };
    RectDesc.strShapeTag = "Memory_Body";

    RigidBodyDesc.pGeometry = &RectDesc;

    RigidBodyDesc.eThisColFlag = COL_NPC;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

    m_pTransformCom->SetRigidBody(m_pRigidBody);
    m_pRigidBody->SetMass(10.0f);

    return S_OK;
}

void CMemory::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    if ((_ColData.eColFlag & COL_STATIC) && !m_bFallGround)
    {
        m_bFallGround = true;
    }
}

void CMemory::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CMemory::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

shared_ptr<CMemory> CMemory::Create(_float3 _vPosition, _int _iGenType)
{
    shared_ptr<CMemory> pMemory = make_shared<CMemory>();

    pMemory->SetGenType(_iGenType);
    
    if (FAILED(pMemory->Initialize(_vPosition)))
        MSG_BOX("Failed to Create : CMemory");

    return pMemory;
}
