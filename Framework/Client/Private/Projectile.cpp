#include "Projectile.h"
#include "Model.h"
#include "RigidBody.h"

CProjectile::CProjectile()
{
}

CProjectile::~CProjectile()
{
}

HRESULT CProjectile::Initialize(PROJECTILE_DESC* _pProjectileDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    __super::Initialize(_pTransformDesc);

    m_pTarget = _pProjectileDesc->pTarget;
    
    return S_OK;
}

void CProjectile::PriorityTick(_float _fTimeDelta)
{
    m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
    
    m_fDistanceToTarget = (m_vTargetPos - m_vCurrentPos).Length();
}

void CProjectile::Tick(_float _fTimeDelta)
{
    if (m_vTargetPos == _float3(0.f, 0.f, 0.f))
        return;

    switch (m_iProjectileType)
    {
    case PJ_DIRECT:
        MoveToDir(_fTimeDelta);
        break;

    case PJ_READYDIRECT:
        if (m_bReadyFire)
        {
            m_fCalculateTime1 += _fTimeDelta;
            if (m_fCalculateTime1 > m_fReadyTime)
            {
                m_fCalculateTime1 = 0.f;
                m_bReadyFire = false;
            }
        }
        if(!m_bReadyFire)
            MoveToDir(_fTimeDelta);
        break;

    case PJ_GUIDED:
        UpdateTargetPos();
        UpdateDirection();
        MoveToTarget(_fTimeDelta);
        break;

    case PJ_READYGUIDED:
        if (m_bReadyFire)
        {
            m_fCalculateTime1 += _fTimeDelta;
            if (m_fCalculateTime1 > m_fReadyTime)
            {
                m_fCalculateTime1 = 0.f;
                m_bReadyFire = false;
            }
        }
        if (!m_bReadyFire)
        {
            UpdateTargetPos();
            UpdateDirection();
            MoveToTarget(_fTimeDelta);
        }
        break;

    case PJ_FALL:
        break;
        
    case PJ_SUMMON:
        break;

    case PJ_NONE:
        break;
    }
}

void CProjectile::LateTick(_float _fTimeDelta)
{
    if (m_bUseGlow)
    {
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_GLOW, shared_from_this());
    }
}

HRESULT CProjectile::Render()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    return S_OK;
}

HRESULT CProjectile::RenderGlow()
{
    return E_NOTIMPL;
}

void CProjectile::OnCollisionProjectile()
{
    SimulationOn();
    m_bCollisionActive = true;
}

void CProjectile::OffCollisionProjectile()
{
    SimulationOff();
    m_bCollisionActive = false;
}

void CProjectile::CheckCollisionActive()
{
    if (m_strShapeTag != "")
    {
        if (m_bPrevCollisionActive)
        {
            if (!m_bCollisionActive)
            {
                SimulationOff();
                m_bCollisionActive = false;
                m_bPrevCollisionActive = false;
                m_pRigidBody->DisableCollision(m_strShapeTag);
            }
        }
        else
        {
            if (!m_bPrevCollisionActive)
            {
                if (m_bCollisionActive)
                {
                    SimulationOn();
                    m_bCollisionActive = true;
                    m_bPrevCollisionActive = true;
                    m_pRigidBody->EnableCollision(m_strShapeTag);
                }
            }
        }
    }
}

void CProjectile::UpdateTargetPos()
{
    m_vTargetPos = m_pTarget.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) + _float3(0.f, 1.5f, 0.f);
}

void CProjectile::UpdateTargetDir(_float3 _vUp)
{
    m_vTargetDir = m_pTarget.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) + _vUp -
        _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
}

void CProjectile::UpdateDirection()
{
    m_pTransformCom->LookAt(m_vTargetPos);
}

void CProjectile::MoveToTarget(_float _fTimeDelta)
{
    m_pTransformCom->MoveTo(m_vTargetPos, 0.f, _fTimeDelta);
}

void CProjectile::MoveToDir(_float _fTimeDelta)
{
    m_pTransformCom->GoDir(_fTimeDelta, m_vTargetDir);
}

void CProjectile::MovePos(_float3 _vPos)
{
    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPos);
    m_vCurrentPos = _vPos;
}
