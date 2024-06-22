#include "Effect.h"
#include "Texture.h"
#include "Model.h"

CEffect::CEffect()
{
}

CEffect::CEffect(const CEffect& rhs)
{
}

CEffect::~CEffect()
{
}

HRESULT CEffect::Initialize(EffectDesc* pDesc)
{
    __super::Initialize(nullptr);

    m_pDevice = GAMEINSTANCE->GetDeviceInfo();
    m_pContext = GAMEINSTANCE->GetDeviceContextInfo();

    m_pTransformCom->SetTurnSpeed(1.f);
    m_iCurKeyFrameIdx = 0;

    m_eEffectDesc.iEffectType = pDesc->iEffectType;

    m_eEffectDesc.iShaderType = pDesc->iShaderType;
    m_eEffectDesc.iShaderPass = pDesc->iShaderPass;

    m_eEffectDesc.iRenderGroup = pDesc->iRenderGroup;

    m_eEffectDesc.fDuration = pDesc->fDuration;
    m_eEffectDesc.StartTrackPos = pDesc->StartTrackPos;

    m_eEffectDesc.bLoop = pDesc->bLoop;

    m_eEffectDesc.bUseDiffuse = pDesc->bUseDiffuse;

    m_eEffectDesc.strTexDiffuseKey = pDesc->strTexDiffuseKey;

    m_eShaderType = (ESHADER_TYPE)m_eEffectDesc.iShaderType;
    m_iShaderPass = m_eEffectDesc.iShaderPass;
    m_iRenderGroup = (CRenderer::ERENDER_GROUP)m_eEffectDesc.iRenderGroup;

    if (m_eEffectDesc.bUseDiffuse)
        m_strTextureKey[TEX_DIFFUSE] = m_eEffectDesc.strTexDiffuseKey;

    m_bPlaying = true;
    m_bStart = false;
    m_bFinished = false;
    m_eState = EFFECT_STATE_IDLE;

    return S_OK;
}

void CEffect::PriorityTick(_float _fTimeDelta)
{
}

void CEffect::Tick(_float _fTimeDelta)
{
    if (!m_bStart)
        return;

    if (m_bFinished)
        return;

    if (!m_bPlaying)
        return;

    m_fSpeedTime = _fTimeDelta * m_fSpeed;

    m_fActiveTime += m_fSpeedTime;

    // 루프인 애들은 반복해서 돌려주기? 이거 이펙트 시스템에서 관여해야하기 때문에
    // 추후 이펙트 시스템 만들고 나서 생각해보기
    if ((m_eEffectDesc.fDuration <= m_fActiveTime))
    {
        if (!m_eEffectDesc.bLoop)
        {
            m_eState = EFFECT_STATE_FINISH;
            m_bFinished = true;
        }

        else
        {
            // 언제든 다시 시작할 수 있도록
            m_eState = EFFECT_STATE_IDLE;
        }

        m_fActiveTime = 0.f;
        m_iCurKeyFrameIdx = 0;
    }
}

void CEffect::LateTick(_float _fTimeDelta)
{
    m_fSpeedTime = _fTimeDelta * m_fSpeed;

    if (CRenderer::RENDER_GLOW == m_eEffectDesc.iRenderGroup)
    {
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_BLEND, shared_from_this());
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_GLOW, shared_from_this());
    }
    else if (CRenderer::RENDER_BLUR == m_eEffectDesc.iRenderGroup)
    {
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_GLOW, shared_from_this());
    }
    else
    {
        GAMEINSTANCE->AddRenderGroup((CRenderer::ERENDER_GROUP)m_eEffectDesc.iRenderGroup, shared_from_this());
    }
}

HRESULT CEffect::Render()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);

    if (m_eEffectDesc.bUseDiffuse)
        GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTextureKey[TEX_DIFFUSE]);

    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    return S_OK;
}

HRESULT CEffect::SetOwner(shared_ptr<CGameObject> _pOwner)
{
    if (!_pOwner)
        return E_FAIL;

    m_pOwner = _pOwner;

    return S_OK;
}

void CEffect::Reset()
{
    m_fActiveTime = 0.f;
    m_fLifeTime = 0.f;
    m_fTimeDelta = 0.f;
    m_iCurKeyFrameIdx = 0;
    m_fFrameTime = 0.f;
    m_bFinished = false;
    m_bPlaying = true;
    m_bStart = false;
    m_eState = EFFECT_STATE_IDLE;
}

CEffect::EFFECT_STATE CEffect::CheckState(_double _TrackPos)
{
    m_ePrevState = m_eState;

    // 시작할 수 있는지 체크
    if (EFFECT_STATE_IDLE == m_eState
        && _TrackPos >= m_eEffectDesc.StartTrackPos)
    {
        m_bStart = true;
        m_eState = EFFECT_STATE_START;
    }
    // 시작하려는 상태가 아니면 상태를 반환해서 종료됐는지를 판단.
    else if (EFFECT_STATE_IDLE != m_eState)
        return m_eState;

    return m_eState;
}

shared_ptr<CEffect> CEffect::Create(EffectDesc* pDesc)
{
    shared_ptr<CEffect> pInstance = make_shared<CEffect>();

    if (FAILED(pInstance->Initialize(pDesc)))
        MSG_BOX("Failed to Create : CEffect");

    return pInstance;
}