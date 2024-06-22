#include "Chest.h"
#include "GameInstance.h"
#include "Model.h"

#include "Message.h"
#include "UIBackGround.h"

#include "UIMgr.h"
#include "DialogueMgr.h"

CChest::CChest()
{
}

HRESULT CChest::Initialize(_float3 _vPosition, _uint _iChestNum)
{
    CInteractionObj::Initialize(nullptr);
    m_iChestNum = _iChestNum;

    m_pModel = GAMEINSTANCE->GetModel("Chest2");

    m_eShaderType = ST_NONANIM;
    m_iShaderPass = 0;

    m_vUIDistance = _float3(0.4f, 0.5f, 0.f);
    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);
    m_pTransformCom->SetScaling(0.5f, 0.5f, 0.5f);


    /*UI: Message*/

    string strUIKey = "UI_Message" + to_string(m_iChestNum);
    string TextureKey = "Scroll_Texture_" + to_string(m_iChestNum);
    m_strUIKey = strUIKey;

    _float2 vUISize = _float2();

    if (1 == m_iChestNum) {
        vUISize = _float2(300.f, 300.f);
    }
    else if (2 == m_iChestNum) {

        vUISize = _float2(400.f, 400.f);
        m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, XMConvertToRadians(90.f));

    }
    else if (3 == m_iChestNum) {

        vUISize = _float2(300.f, 300.f);

        /*Complete Message*/
        CVIInstancing::InstanceDesc instanceDesc{};
        instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
        instanceDesc.vSize = _float2(700.f, 700.f);
        instanceDesc.fSizeY = 700.f;
        instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

        CUIBase::UIDesc uiDesc{};
        uiDesc.strTag = "UI_Message_Complete";
        uiDesc.eUIType = UI_BACKGROUND;
        uiDesc.eUITexType = UI_TEX_D;
        uiDesc.eUISlotType = SLOT_END;
        uiDesc.isMasked = false;
        uiDesc.isNoised = false;
        uiDesc.eUILayer = UI_LAYER_HUD;
        uiDesc.fZOrder = 0.1f;
        uiDesc.fDiscardAlpha = 0.f;
        uiDesc.iShaderPassIdx = 0;

        vector<string> tex;
        tex.emplace_back("UI_Message_Complete");

        shared_ptr<CUIBackGround> pUI = CUIBackGround::Create(tex, "", "", uiDesc, instanceDesc);
        if (pUI == nullptr)
            return E_FAIL;

        if (FAILED(UIMGR->AddUI("UI_Message_Complete", pUI)))
            return E_FAIL;

    }

    CVIInstancing::InstanceDesc instanceDesc{};
    instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
    instanceDesc.vSize = vUISize;
    instanceDesc.fSizeY = vUISize.y;
    instanceDesc.vCenter = _float3(0.f, 0.f, 0.f);

    CUIBase::UIDesc uiDesc{};
    uiDesc.strTag = strUIKey;
    uiDesc.eUIType = UI_BACKGROUND;
    uiDesc.eUITexType = UI_TEX_D;
    uiDesc.eUISlotType = SLOT_END;
    uiDesc.isMasked = false;
    uiDesc.isNoised = false;
    uiDesc.eUILayer = UI_LAYER_HUD;
    uiDesc.fZOrder = 0.1f;
    uiDesc.fDiscardAlpha = 0.1f;
    uiDesc.iShaderPassIdx = 0;

    vector<string> tex;
    tex.emplace_back(TextureKey);

    shared_ptr<CUIBackGround> pUI = CUIBackGround::Create(tex, "", "", uiDesc, instanceDesc);
    if (pUI == nullptr)
        return E_FAIL;

    if (FAILED(UIMGR->AddUI(strUIKey, pUI)))
        return E_FAIL;

    return S_OK;
}

void CChest::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);

}

void CChest::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    if (m_IsComplete) {
        m_fCurrentAccTime += _fTimeDelta;


        if (m_fCurrentAccTime > 2.f && !m_IsSwitchingDone) {
            UIMGR->DeactivateUI(m_strUIKey);
            UIMGR->ActivateUI("UI_Message_Complete");
            m_IsSwitchingDone = true;

            GAMEINSTANCE->SetFadeEffect(true, 0.4f);
    
        }
        else if (m_fCurrentAccTime > 2.f && m_IsSwitchingDone) {
            if (!m_IsFadeIn) {
                GAMEINSTANCE->SetFadeEffect(false, 0.4f);
                GAMEINSTANCE->PlaySoundW("SFX_Get_Complete_Message", 1.2f);
                m_IsFadeIn = true;
            }

            if (m_fCurrentAccTime >= m_fMaxAppearTime) {
                UIMGR->DeactivateUI("UI_Message_Complete");
                m_IsComplete = false;
                DIALOGUEMGR->SetCurDialogue(TEXT("Message0"));

            }
        }



    }

}

void CChest::LateTick(_float _fTimeDelta)
{
    __super::LateTick(_fTimeDelta);

    if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 3.0f))
    {
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
    }
}

HRESULT CChest::Render()
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

void CChest::InteractionOn()
{
    if (!m_IsMessageOn) {
        UIMGR->ActivateUI(m_strUIKey);
        GAMEINSTANCE->PlaySoundW("SFX_Get_Message", 1.2f);

        if (3 == m_iChestNum) {
            m_IsComplete = true;
        }
    }
    else {
        UIMGR->DeactivateUI(m_strUIKey);
    }

    m_IsMessageOn = !m_IsMessageOn;

}

shared_ptr<CChest> CChest::Create(_float3 _vPosition , _uint _iChestNum)
{
    shared_ptr<CChest> pChest = make_shared<CChest>();

    if (FAILED(pChest->Initialize(_vPosition, _iChestNum)))
        MSG_BOX("Failed to Create : CChest");

    return pChest;
}
