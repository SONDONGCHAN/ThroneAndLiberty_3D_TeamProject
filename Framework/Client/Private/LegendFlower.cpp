#include "LegendFlower.h"
#include "QuestMgr.h"
#include "EffectMgr.h"

#include "Model.h"

CLegendFlower::CLegendFlower()
{
}

HRESULT CLegendFlower::Initialize(_float3 _vPosition)
{
    __super::Initialize(nullptr);
   
    m_pModel = GAMEINSTANCE->GetModel("BG_Flower_33_01_SM");

    m_eShaderType = ST_NONANIM;
    m_iShaderPass = 0;
    m_vUIDistance = _float3(0.4f, 0.5f, 0.f);

    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);

    return S_OK;
}

void CLegendFlower::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);
}

void CLegendFlower::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    m_bEffectOn = true;

    if (m_bEffectOn)
        EFFECTMGR->PlayEffect("Legend_Flower_3", shared_from_this());

    if (m_bDisappearOn)
    {
        m_fDisappearTime -= _fTimeDelta;

        if (0.f >= m_fDisappearTime)
            m_IsActive = false;
    }
}

void CLegendFlower::LateTick(_float _fTimeDelta)
{
    __super::LateTick(_fTimeDelta);

    if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 3.0f))
    {
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
    }
}

HRESULT CLegendFlower::Render()
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

void CLegendFlower::InteractionOn()
{

    QUESTMGR->ActiveEventTag(QUEST_SUB0, "Event_FindFlower");
    m_bInteractAble = false;

    EFFECTMGR->PlayEffect("Legend_Flower_Disappear_2", shared_from_this());
    m_fDisappearTime = 0.5f;
    m_bDisappearOn = true;

    GAMEINSTANCE->PlaySoundW("SFX_Get_LegendFlower", 1.3f);

}

shared_ptr<CLegendFlower> CLegendFlower::Create(_float3 _vPos)
{
    shared_ptr<CLegendFlower> pFlower = make_shared<CLegendFlower>();

    if (FAILED(pFlower->Initialize(_vPos)))
        MSG_BOX("Failed to Create : CLegendFlower");

    return pFlower;
}
