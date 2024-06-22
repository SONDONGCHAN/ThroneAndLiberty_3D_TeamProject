#include "PlayerRope.h"
#include "Model.h"
#include "RigidBody.h"

#include "EffectMgr.h"

CPlayerRope::CPlayerRope()
{
}

CPlayerRope::~CPlayerRope()
{
}

HRESULT CPlayerRope::Initialize(weak_ptr<CPlayer> _pPlayer)
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec =200.f;
	pArg->fRotationPerSec = XMConvertToRadians(90.0f);
	__super::Initialize(pArg);
    
    m_pOwnerObject = _pPlayer;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;

	m_pModelCom = GAMEINSTANCE->GetModel("PlayerArrow");

    m_pTransformCom->SetScaling(_float3(0.5f, 0.5f, 0.5f));
    AddRigidBody();
    DisableProjectile();
    
	return S_OK;
}

void CPlayerRope::PriorityTick(_float _fTimeDelta)
{
}

void CPlayerRope::Tick(_float _fTimeDelta)
{
    GoToTargetPos(_fTimeDelta);
    m_fLifeTime -= _fTimeDelta;

    m_pTransformCom->Tick(_fTimeDelta);
}

void CPlayerRope::LateTick(_float _fTimeDelta)
{
    if (m_isCollision || (m_fLifeTime <= 0.f) )
    {
        DisableProjectile();
        return;
    }

    if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 1.0f))
    {
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_GLOW, shared_from_this());
#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
    }
    
}

HRESULT CPlayerRope::Render()
{
    //__super::Render();

	return S_OK;
}

HRESULT CPlayerRope::RenderGlow()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, 4);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    _float fScale = 1.1f;
    _bool   bUseGlowColor = true;
    _float4 vGlowColor = { 0.31f, 0.98f, 0.89f, 1.f };

    GAMEINSTANCE->BindRawValue("g_bUseGlowColor", &bUseGlowColor, sizeof(_bool));
    GAMEINSTANCE->BindRawValue("g_vGlowColor", &vGlowColor, sizeof(_float4));
    GAMEINSTANCE->BindRawValue("g_fScale", &fScale, sizeof(_float));

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }

    return S_OK;
}

void CPlayerRope::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    if (_ColData.eColFlag == COL_STATIC)
    {
        m_isCollision = true;
    }
}

void CPlayerRope::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerRope::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPlayerRope::EnableProjectile(_float3 _vMyStartPos, _float3 _vTargetPos, PlayerProjAbnormal _ePlayerProjAbnormal, _bool isDistinguish, _uint _iIndex)
{
    m_pOwnerObject.lock()->SetRopeTargetPos(_float3(0.f, 0.f, 0.f));

    m_eMyAbNormal = _ePlayerProjAbnormal;

    _float3 vTargetDir = _vTargetPos - _vMyStartPos;
    vTargetDir.Normalize();

    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vMyStartPos);
    m_pTransformCom->LookAtDir(vTargetDir);

    m_iEffectIdx = EFFECTMGR->PlayEffect("Plr_Arrow_Basic", shared_from_this());

    m_pRigidBody->SimulationOn();
    SetEnable(true);
}

void CPlayerRope::DisableProjectile()
{
    if (m_isCollision)
    {
        _float3 vOwnerPos = m_pOwnerObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
        _float3 vMyPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
        
        if(vOwnerPos.y < vMyPos.y - 0.1f)
            m_pOwnerObject.lock()->SetRopeTargetPos(vMyPos);
        else
            m_pOwnerObject.lock()->SetRopeTargetPos(_float3(0.f, 0.f, 0.f));
    }
    else
        m_pOwnerObject.lock()->SetRopeTargetPos(_float3(0.f, 0.f, 0.f));

    m_fLifeTime = 0.2f;
    m_isCollision = false;
    m_pRigidBody->SimulationOff();
    SetEnable(false);
}

void CPlayerRope::GoToTargetPos(_float _fTimeDelta)
{
    m_pTransformCom->GoStraight(_fTimeDelta);
}

HRESULT CPlayerRope::AddRigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
    
    RigidBodyDesc.isStatic = false;
    RigidBodyDesc.isTrigger = true;
    RigidBodyDesc.isGravity = false;
    RigidBodyDesc.isInitCol = true;
    RigidBodyDesc.pTransform = m_pTransformCom;
    RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;
    
    RigidBodyDesc.pOwnerObject = shared_from_this();
    RigidBodyDesc.fStaticFriction = 1.0f;
    RigidBodyDesc.fDynamicFriction = 0.0f;
    RigidBodyDesc.fRestitution = 0.0f;
    
    GeometryBox RectDesc;
    RectDesc.vSize = _float3{ 0.2f, 0.2f, 2.f };
    RectDesc.vOffSetPosition = { 0.0f, 0.0f, -0.7f };
    RectDesc.strShapeTag = "Player_Rope";
    RigidBodyDesc.pGeometry = &RectDesc;
    
    RigidBodyDesc.eThisColFlag = COL_ROPE;
    RigidBodyDesc.eTargetColFlag = COL_STATIC;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
    
    m_pTransformCom->SetRigidBody(m_pRigidBody);
	return S_OK;
}

shared_ptr<CPlayerRope> CPlayerRope::Create(weak_ptr<CPlayer> _pPlayer)
{
	shared_ptr<CPlayerRope> pInstance = make_shared<CPlayerRope>();

	if (FAILED(pInstance->Initialize(_pPlayer)))
		MSG_BOX("Failed to Create : CPlayerRope");

	return pInstance;
}
