#include "EffectGroup.h"
#include "EffectMesh.h"
#include "ParticleSystem.h"
#include "EffectAttachObj.h"

#include "Model.h"
#include "Animation.h"

CEffectGroup::CEffectGroup()
    : m_iEffectCnt(0)
{
}

CEffectGroup::CEffectGroup(const CEffectGroup& rhs)
    : m_strGroupID(rhs.m_strGroupID),
    m_bWithModel(rhs.m_bWithModel),
    m_bWithAnim(rhs.m_bWithAnim),
    m_strModelKey(rhs.m_strModelKey),
    m_strAnimKey(rhs.m_strAnimKey),
    m_StartTrackPos(rhs.m_StartTrackPos),
    m_iEffectCnt(rhs.m_iEffectCnt),
    m_strEffectID(rhs.m_strEffectID),
    m_fSpeedTime(rhs.m_fSpeedTime),
    m_fElapsedTime(rhs.m_fElapsedTime),
    m_bPlaying(rhs.m_bPlaying),
    m_fSpeed(rhs.m_fSpeed),
    m_bAllFinish(rhs.m_bAllFinish),
    m_eEffectStates(rhs.m_eEffectStates),
    m_pDevice(rhs.m_pDevice),
    m_pContext(rhs.m_pContext),
    m_eEffectAttachType(rhs.m_eEffectAttachType)
{
    for (const auto& effect : rhs.m_pEffects) {
        if (effect) {
            _int iEffectType = effect->GetEffectDesc().iEffectType;
            if (iEffectType == 0)
            {
                m_pEffects.push_back(make_shared<CEffectMesh>(*dynamic_pointer_cast<CEffectMesh>(effect))); // 깊은 복사
            }
            else if (iEffectType == 1)
            {
                m_pEffects.push_back(make_shared<CParticleSystem>(*dynamic_pointer_cast<CParticleSystem>(effect))); // 깊은 복사
            }
        }
        else {
            m_pEffects.push_back(nullptr);
        }
    }

    Reset();
}

CEffectGroup::~CEffectGroup()
{
}

HRESULT CEffectGroup::Initialize(EFFECT_ATTACH_TYPE _eEffectAttachType)
{
    //Reset();
    
    m_eEffectAttachType = _eEffectAttachType;
    m_fSpeedTime = 0.f;
    m_fElapsedTime = 0.f;
    m_bAllFinish = false;
    m_IsEnabled = false;

    m_pEffectAttachObj = CEffectAttachObj::Create(_float3(0.f, 0.f, 0.f), m_strGroupID, false);

    return S_OK;
}

void CEffectGroup::PriorityTick(_float _fTimeDelta)
{
}

void CEffectGroup::Tick(_float _fTimeDelta)
{
    if (!m_bPlaying)
        return;

    m_fSpeedTime = _fTimeDelta * m_fSpeed;

    if (m_bForTest)
        m_fElapsedTime += m_fSpeedTime * m_fTestStartFaster;
    
    else
        m_fElapsedTime += m_fSpeedTime;

    m_bAllFinish = true;

    if (EFFECT_ATTACH_ANIM == m_eEffectAttachType && !m_bForTest)
    {
        // 트랙 포지션 받아서 시작 체크
        for (_int i = 0; i < m_pEffects.size(); i++)
        {
            if (m_pOwnerModelCom.expired())
            {
                m_bAllFinish = true;
                return;
            }

            m_eEffectStates[i] = m_pEffects[i]->CheckState(m_pOwnerModelCom.lock()->GetCurAnimation()->GetTrackPosition());
            if (CEffect::EFFECT_STATE_START == m_eEffectStates[i])
                m_pEffects[i]->Tick(m_fSpeedTime);

            m_bAllFinish = (m_bAllFinish && (CEffect::EFFECT_STATE_FINISH == m_eEffectStates[i]));
        }
    }

    else if (EFFECT_ATTACH_NONANIM == m_eEffectAttachType || m_bForTest)
    {
        // 시작 타임 받아서 시작 체크
        for (_int i = 0; i < m_pEffects.size(); i++)
        {
            m_eEffectStates[i] = m_pEffects[i]->CheckState(m_fElapsedTime);
            if (CEffect::EFFECT_STATE_START == m_eEffectStates[i])
                m_pEffects[i]->Tick(m_fSpeedTime);

            m_bAllFinish = (m_bAllFinish && (CEffect::EFFECT_STATE_FINISH == m_eEffectStates[i]));
        }
    }

    else if (EFFECT_ATTACH_FREE == m_eEffectAttachType)
    {
        // 시작 타임 받아서 시작 체크
        for (_int i = 0; i < m_pEffects.size(); i++)
        {
            m_eEffectStates[i] = m_pEffects[i]->CheckState(m_fElapsedTime);
            if (CEffect::EFFECT_STATE_START == m_eEffectStates[i])
                m_pEffects[i]->Tick(m_fSpeedTime);

            m_bAllFinish = (m_bAllFinish && (CEffect::EFFECT_STATE_FINISH == m_eEffectStates[i]));
        }
    }
}

void CEffectGroup::LateTick(_float _fTimeDelta)
{
    if (m_bDurationControl)
    {
        m_fDuration -= _fTimeDelta;

        if (0.f >= m_fDuration)
        {
            Reset();
            return;
        }
    }

    if (m_bAllFinish)
    {
        Reset();
        return;
    }

    for (_int i = 0; i < m_pEffects.size(); i++)
    {
        if (CEffect::EFFECT_STATE_START == m_eEffectStates[i])
            m_pEffects[i]->LateTick(m_fSpeedTime);
    }
}

HRESULT CEffectGroup::Render()
{
    return S_OK;
}

void CEffectGroup::AddEffect(string _strEffectID, shared_ptr<CEffect> _pEffect)
{
    m_strEffectID.push_back(_strEffectID);
    m_pEffects.push_back(_pEffect);
    m_eEffectStates.push_back(CEffect::EFFECT_STATE_IDLE);

    //Reset();
    //m_bPlaying = true;

    // 툴이라서,,?
    //_pEffect->SetOwner(m_pOwner.lock());
}

void CEffectGroup::DeleteEffect(string _strEffectID)
{
    auto it = std::find(m_strEffectID.begin(), m_strEffectID.end(), _strEffectID);

    if (it != m_strEffectID.end())
    {
        _int iIdx = (_int)distance(m_strEffectID.begin(), it);

        m_strEffectID.erase(m_strEffectID.begin() + iIdx);
        m_pEffects.erase(m_pEffects.begin() + iIdx);
    }
}

void CEffectGroup::StopEffect()
{
    Reset();
}

void CEffectGroup::PauseEffect()
{
    m_bPlaying = false;
}

HRESULT CEffectGroup::Play(shared_ptr<CGameObject> _pOwner, _bool _bForTest, _float _fTestStartFaster)
{
    if (EFFECT_ATTACH_ANIM == m_eEffectAttachType || EFFECT_ATTACH_NONANIM == m_eEffectAttachType)
    {
        if (FAILED(SetOwner(_pOwner)))
            return E_FAIL;
    }

    m_IsEnabled = true;
    m_bForTest = _bForTest;
    m_fTestStartFaster = _fTestStartFaster;

    return S_OK;
}

HRESULT CEffectGroup::Play(_float3 _vPos, _bool _bLoop, _float _fDuration)
{
    m_pEffectAttachObj->GetTransform()->SetState(CTransform::STATE_POSITION, _vPos);
    m_fDuration = _fDuration;
    m_pEffectAttachObj->SetEnable(true);

    m_bDurationControl = !_bLoop;

    if (FAILED(SetOwner(m_pEffectAttachObj)))
        return E_FAIL;

    m_IsEnabled = true;

    return S_OK;
}

void CEffectGroup::Reset()
{
    m_pOwnerModelCom.reset();
    m_pOwnerAnimCom.reset();
    m_pOwner.reset();

    for (auto& iter : m_pEffects)
        iter->Reset();

    m_fSpeedTime = 0.f;
    m_fElapsedTime = 0.f;
    m_bAllFinish = false;
    m_IsEnabled = false;
    m_bForTest = false;
    m_fTestStartFaster = 1.f;

    m_pEffectAttachObj->SetEnable(false);
}

HRESULT CEffectGroup::SetOwner(shared_ptr<CGameObject> _pOwner)
{
    m_pOwner = _pOwner;

    // Owner가 없는 경우에 대해서 생각하기
    if (!m_pOwner.lock())
        return E_FAIL;
    
    m_pOwnerModelCom = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"));

    // Model이 없는 경우
    if (!m_pOwnerModelCom.lock())
    {
        if (EFFECT_ATTACH_ANIM == m_eEffectAttachType)
            return E_FAIL;
    }
    

    for (auto iter : m_pEffects)
    {
        iter->SetOwner(m_pOwner.lock());
    }

    return S_OK;
}

HRESULT CEffectGroup::SetModelKey(string _strModelKey)
{
    m_strModelKey = _strModelKey;

    return S_OK;
}


HRESULT CEffectGroup::SetAnimKey(string _strAnimKey)
{
    m_strAnimKey = _strAnimKey;

    return S_OK;
}

CEffectGroup::EffectGroupInfoDesc CEffectGroup::GetGroupInfo()
{
    EffectGroupInfoDesc groupInfoDesc;

    groupInfoDesc.strGroupID = "";
    groupInfoDesc.strModelKey = m_strModelKey;
    groupInfoDesc.strAnimKey = m_strAnimKey;
    groupInfoDesc.StartTrackPos = 0.0;

    return groupInfoDesc;
}

void CEffectGroup::SetGroupInfo(string _strGroupID, string _strModelKey, string _strAnimKey)
{
    m_strGroupID = _strGroupID;
    m_strModelKey = _strModelKey;
    m_strAnimKey = _strAnimKey;
}


shared_ptr<CEffectGroup> CEffectGroup::Create(EFFECT_ATTACH_TYPE _eEffectAttacjType)
{
    shared_ptr<CEffectGroup> pInstance = make_shared<CEffectGroup>();

    if (FAILED(pInstance->Initialize(_eEffectAttacjType)))
        MSG_BOX("Failed to Create : CEffectGroup");

    return pInstance;
}