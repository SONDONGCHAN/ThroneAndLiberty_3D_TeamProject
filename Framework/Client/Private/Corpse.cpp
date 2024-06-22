#include "Corpse.h"
#include "GameInstance.h"
#include "Model.h"
#include "QuestMgr.h"

CCorpse::CCorpse()
{
}

HRESULT CCorpse::Initialize(_float3 _vPosition)
{
    CInteractionObj::Initialize(nullptr);

    m_pModel = GAMEINSTANCE->GetModel("BG_Bone_03_02_SM");

    m_eShaderType = ST_NONANIM;
    m_iShaderPass = 0;

    m_fInteractDistance = 1.f;
    m_vUIDistance = _float3(0.4f, 0.5f, 0.f);
    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);

    return S_OK;
}

void CCorpse::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);

}

void CCorpse::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

}

void CCorpse::LateTick(_float _fTimeDelta)
{
    __super::LateTick(_fTimeDelta);

    if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 3.0f))
    {
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
    }
}

HRESULT CCorpse::Render()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    _uint iNumMeshes = m_pModel->GetNumMeshes();

    UsingRimLight(_float3(0.2f, 0.2f, 0.2f));

    for (size_t i = 0; i < iNumMeshes; i++) {

        GAMEINSTANCE->BeginNonAnimModel(m_pModel, (_uint)i);
    }

    /*RimLight Off*/
    _bool bRimOff = false;
    GAMEINSTANCE->BindRawValue("g_IsUsingRim", &bRimOff, sizeof(bRimOff));

    return S_OK;
}

void CCorpse::InteractionOn()
{
    QUESTMGR->ActiveEventTag(QUEST_SUB0, "Event_FindRing");
    m_IsInteractionDone = true;

}

shared_ptr<CCorpse> CCorpse::Create(_float3 _vPosition)
{
    shared_ptr<CCorpse> pCorpse = make_shared<CCorpse>();
    
    if (FAILED(pCorpse->Initialize(_vPosition)))
        MSG_BOX("Failed to Create : CCorpse");

    return pCorpse;
}
