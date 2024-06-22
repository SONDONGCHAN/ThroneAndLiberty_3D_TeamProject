#include "EffectAttachObj.h"
#include "GameInstance.h"
#include "EffectMgr.h"

CEffectAttachObj::CEffectAttachObj()
{
}

HRESULT CEffectAttachObj::Initialize(_float3 _vPos, string _strEffectKey, _bool _bStart)
{
    __super::Initialize(nullptr);

    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPos);
    m_strEffectKey = _strEffectKey;
    m_bLoop = true;

    if (_bStart)
        m_iEffectIdx = EFFECTMGR->PlayEffect(_strEffectKey, shared_from_this());

    return S_OK;
}

void CEffectAttachObj::PriorityTick(_float _fTimeDelta)
{
    if (!m_bLoop)
    {
        m_fDuration -= _fTimeDelta;

        if (0.f >= m_fDuration)
            m_IsEnabled = false;
    }
}

void CEffectAttachObj::Tick(_float _fTimeDelta)
{
}

void CEffectAttachObj::LateTick(_float _fTimeDelta)
{
}

HRESULT CEffectAttachObj::Render()
{
    return S_OK;
}

HRESULT CEffectAttachObj::PlayEffect()
{
    m_iEffectIdx = EFFECTMGR->PlayEffect(m_strEffectKey, shared_from_this());

    if (-1 == m_iEffectIdx)
        return E_FAIL;

    return S_OK;
}

void CEffectAttachObj::PlayEffect(string _strEffectKey)
{
    m_strEffectKey = _strEffectKey;
    m_iEffectIdx = EFFECTMGR->PlayEffect(_strEffectKey, shared_from_this());
}

HRESULT CEffectAttachObj::StopEffect()
{
    if (-1 == m_iEffectIdx)
        return E_FAIL;

    EFFECTMGR->StopEffect(m_strEffectKey, m_iEffectIdx);

    return S_OK;
}

void CEffectAttachObj::SetLifeTime(_bool _bLoop, _float _fDuration)
{
    m_bLoop = _bLoop;
    m_fDuration = _fDuration;
}

shared_ptr<CEffectAttachObj> CEffectAttachObj::Create(_float3 _vPos, string _strEffectKey, _bool _bStart)
{
    shared_ptr<CEffectAttachObj> pObj = make_shared<CEffectAttachObj>();

    if (FAILED(pObj->Initialize(_vPos, _strEffectKey, _bStart)))
        MSG_BOX("Failed to Create : CEffectAttachObj");

    return pObj;
}
