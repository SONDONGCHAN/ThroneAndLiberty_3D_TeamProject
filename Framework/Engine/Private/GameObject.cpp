#include "GameObject.h"
#include "ShaderHandler.h"
#include "Component.h"

CGameObject::CGameObject()
{
}

HRESULT CGameObject::Initialize(CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    m_pTransformCom = CTransform::Create(CGameInstance::GetInstance()->GetDeviceInfo(), CGameInstance::GetInstance()->GetDeviceContextInfo(), _pTransformDesc);

    if (!m_pTransformCom)
        return E_FAIL;

    m_Components.emplace(TEXT("Com_Transform"), m_pTransformCom);

    return S_OK;
}

void CGameObject::PriorityTick(_float _fTimeDelta)
{
}

void CGameObject::Tick(_float _fTimeDelta)
{
}

void CGameObject::LateTick(_float _fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
    return S_OK;
}

_float3 CGameObject::GetLockOnOffSetPosition()
{
    return m_vLockOnOffSetPosition;
}

HRESULT CGameObject::RenderDepth()
{
    if (ST_ANIM == m_eShaderType) {

    }
    else if (ST_NONANIM == m_eShaderType) {

    }


    return S_OK;
}


void CGameObject::AddComponent(const wstring& _strComponentTag, shared_ptr<class CComponent> _pComponent)
{
    m_Components.emplace(_strComponentTag, _pComponent);
}

shared_ptr<CComponent> CGameObject::GetComponent(const wstring& _strComponentTag)
{
    auto iter = m_Components.find(_strComponentTag);
    if (iter == m_Components.end())
        return nullptr;

    return iter->second;
}

shared_ptr<CTransform> CGameObject::GetTransform()
{
	return m_pTransformCom;
}

void CGameObject::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CGameObject::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CGameObject::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

HRESULT CGameObject::UsingRimLight(_float3 _vRimColor, _float _fRimPow)
{
    _bool IsUsingRim = true;
    GAMEINSTANCE->BindRawValue("g_IsUsingRim", &IsUsingRim, sizeof(IsUsingRim));
    GAMEINSTANCE->BindRawValue("g_vRimColor", &_vRimColor, sizeof(_vRimColor));
    GAMEINSTANCE->BindRawValue("g_fRimPow", &_fRimPow, sizeof(_fRimPow));

    _float4 vCamPos = CGameInstance::GetInstance()->GetCamPosition();
    if (FAILED(GAMEINSTANCE->BindRawValue("g_vCamPos", &vCamPos, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

_float CGameObject::GetDistanceFromCam()
{
	return XMVector3Length(CGameInstance::GetInstance()->GetCamPosition() - m_pTransformCom->GetState(CTransform::STATE_POSITION)).m128_f32[0];
}

void CGameObject::ComputeCascadeNum()
{
    _float4x4* pCascadeMatrices = GAMEINSTANCE->GetShadowProjMat();



}

HRESULT CGameObject::UpdateDslv(_float _fTimeDelta)
{	
    // Dissolve Out
    if (m_bDslvType)
    {
        m_fDslvTime -= _fTimeDelta;
        if (m_fDslvTime < 0.0f)
        {
            m_fDslvTime = 0.f;
            m_bDslv = false;

            // Dissolve In End
        }
    }
    // Dissolve In
    else
    {
        m_fDslvTime += _fTimeDelta;
        if (m_fDslvTime > m_fMaxDslvTime)
        {
            m_fDslvTime = m_fMaxDslvTime;
            m_bDslv = false;

            // Dissolve Out End
        }
    }

    return S_OK;
}