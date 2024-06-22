#include "UIPanel.h"
#include "UIItemBackGround.h"
#include "UIItemIcon.h"
#include "UIInstanceItemBG.h"
#include "UIMgr.h"
#include "UIQuestComplete.h"
#include "ItemMgr.h"
#include "UICraftResult.h"

#include "VIInstancePoint.h"

CUIPanel::CUIPanel()
{

}

CUIPanel::~CUIPanel()
{

}

HRESULT CUIPanel::Initialize(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    if (FAILED(Super::Initialize()))
        return E_FAIL;

    m_UIDesc = _UIDesc;

    if (FAILED(AddComponent(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _InstanceDesc)))
        return E_FAIL;

    m_fX = _InstanceDesc.vCenter.x;
    m_fY = _InstanceDesc.vCenter.y;
    m_fSizeX = _InstanceDesc.vSize.x;
    m_fSizeY = _InstanceDesc.fSizeY;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_UI;
    m_iShaderPass = m_UIDesc.iShaderPassIdx;

    if (m_UIDesc.strTag == "InstanceParringPanel" || m_UIDesc.strTag == "BossHPPanel" || m_UIDesc.strTag == "MainQuestIndicatorPanel" ||
        m_UIDesc.strTag == "SubQuestIndicatorPanel0" || m_UIDesc.strTag == "SubQuestIndicatorPanel1" ||
        m_UIDesc.strTag == "InteractionPanel" || m_UIDesc.strTag == "TargetIndicatorPanel" ||
        m_UIDesc.strTag == "TargetIndicatorPanel_0" || m_UIDesc.strTag == "TargetIndicatorPanel_1" ||
        m_UIDesc.strTag == "TargetIndicatorPanel_2" || m_UIDesc.strTag == "RotationArrowPanel" ||
        m_UIDesc.strTag == "StarForceBGPanel"|| m_UIDesc.strTag == "MainQuestIndicatorPanel_0" ||
        m_UIDesc.strTag == "SubQuestIndicatorPanel2" || m_UIDesc.strTag == "SubQuestIndicatorPanel3" ||
		m_UIDesc.strTag == "CraftResultPanel" || m_UIDesc.strTag == "SubQuestIndicatorPanel4" ||
        m_UIDesc.strTag == "SubQuestIndicatorPanel5"|| m_UIDesc.strTag == "SubQuestIndicatorPanel6" ||
        m_UIDesc.strTag == "LoadingBGs" || m_UIDesc.strTag == "InstanceParringPanel_0" ||
        m_UIDesc.strTag == "InteractionPopUpPanel")
    {
        m_isRender = false;
        m_isPickable = false;
    }

    return S_OK;
}

void CUIPanel::PriorityTick(_float _fDeltaTime)
{

}

void CUIPanel::Tick(_float _fDeltaTime)
{
    if (m_isLoadingDone)
    {
        /* 부모를 따라다니게끔 */
        if (!m_pUIParent.expired() && (m_pUIParent.lock()->GETPOS != m_vPrevParentPos))
        {
            _float3 moveDir = m_pUIParent.lock()->GETPOS - m_vPrevParentPos;
            moveDir.Normalize();
            _float moveDist = SimpleMath::Vector3::Distance(m_pUIParent.lock()->GETPOS, m_vPrevParentPos);

            _float3 newPos = _float3(m_fX, m_fY, 0.f) + (moveDir * moveDist);
            SetUIPos(newPos.x, newPos.y);

            m_BindWorldMat = m_pTransformCom->GetWorldMatrix();
        }
        else
            m_BindWorldMat = m_pTransformCom->GetWorldMatrix();

        if (m_pUIChildren.empty())
            m_iRenderState = 0;
        else
            m_iRenderState = 1;

        if (m_UIDesc.strTag == "MonsterHPPanel" && m_isRender)
        {
            m_fTimeAcc += _fDeltaTime;
            if (m_fTimeAcc >= 4.f)
                m_isRender = false;
        }

        if (m_UIDesc.strTag == "QuestCompleteBGs" && m_isRender)
        {
            if (m_fTimeAcc <= 0.f)
            {
                UIMGR->ActivateQuestCompleteEffect();
                UIMGR->ActivateQuestCompleteIconEffect();
                GAMEINSTANCE->PlaySoundW("UI_Text_Mission_Success_02_A", 0.3f);
            }

            m_fTimeAcc += _fDeltaTime;
            if (m_fTimeAcc >= 4.f)
            {
                m_isRender = false;
                shared_ptr<CUIQuestComplete> pUI = dynamic_pointer_cast<CUIQuestComplete>(UIMGR->FindUI("QuestCompleteInstance"));
                if (pUI != nullptr)
                    pUI->ResetQuestReward();

                ITEMMGR->IncreaseMoney(1000);

                UIMGR->DeactivateQuestCompleteSlotEffect();
                UIMGR->DeactivateQuestCompleteEffect();
                UIMGR->DeactivateQuestCompleteIconEffect();
            }
        }

        if (m_UIDesc.strTag == "SpeechBallonBGs" && m_isRender)
        {
            m_fTimeAcc += _fDeltaTime;
            if (m_fTimeAcc >= 3.f)
            {
                m_isRender = false;
                UIMGR->SetCurNPC(nullptr);
                m_fTimeAcc = 0.f;
            }
        }

        if (m_UIDesc.strTag == "InteractionPopUpPanel" && m_isRender)
        {
            m_fTimeAcc += _fDeltaTime;
            if (m_fTimeAcc >= 3.f)
            {
                m_isRender = false;
                UIMGR->DeactivateInteractionPopUp();
                m_fTimeAcc = 0.f;
            }
        }

        if (m_UIDesc.strTag == "CraftResultPanel" && m_isRender && !m_bIsFailed)
        {
            if (m_fTimeAcc <= 0.f)
            {
                weak_ptr<CUICraftResult> pUI = dynamic_pointer_cast<CUICraftResult>(UIMGR->FindUI("CraftResultInstance"));
                pUI.lock()->ResetRatio();
                pUI.lock()->SetIsRender(true);
                GAMEINSTANCE->PlaySoundW("UI_Text_upgrade_Ready_01_A", 1.f);
            }

            m_fTimeAcc += _fDeltaTime;
            weak_ptr<CUICraftResult> pUI = dynamic_pointer_cast<CUICraftResult>(UIMGR->FindUI("CraftResultInstance"));
            if (!pUI.expired())
            {
                pUI.lock()->SetCur(m_fTimeAcc);
            }

            if (m_fTimeAcc >= 1.2f)
            {
                //m_isRender = false;
                //m_fTimeAcc = 0.f;
                //pUI.lock()->ResetRatio();
                pUI.lock()->RenderScript();
                if (m_fTimeAcc < 1.5f && m_fTimeAcc >= 1.2f)
                {
                    UIMGR->ActivateCraftResultEffect();
                }
            }

            if (m_fTimeAcc >= 4.f)
            {
                m_isRender = false;
                m_fTimeAcc = 0.f;
                //pUI.lock()->ResetRatio();
                pUI.lock()->SetIsRender(false);
                UIMGR->DeactivateStarForce();
            }
        }

        if (m_UIDesc.strTag == "CraftResultPanel" && m_isRender && m_bIsFailed)
        {
            if (m_fTimeAcc <= 0.f)
            {
                weak_ptr<CUICraftResult> pUI = dynamic_pointer_cast<CUICraftResult>(UIMGR->FindUI("CraftResultInstance"));
                pUI.lock()->SetIsRender(true);
                pUI.lock()->RenderScript();
            }

            weak_ptr<CUICraftResult> pUI = dynamic_pointer_cast<CUICraftResult>(UIMGR->FindUI("CraftResultInstance"));

            m_fTimeAcc += 0.016f;

            if (m_fTimeAcc >= 2.f)
            {
                m_isRender = false;
                m_fTimeAcc = 0.f;
                pUI.lock()->SetIsRender(false);
                UIMGR->DeactivateCraftResult();
                m_bIsFailed = false;
            }
        }

        if (!m_pUIChildren.empty())
        {
            for (auto& childPair : m_pUIChildren)
            {
                if (childPair.second.lock() != nullptr)
                    childPair.second.lock()->Tick(_fDeltaTime);
            }
        }

        if (!m_pUIParent.expired())
            m_vPrevParentPos = m_pUIParent.lock()->GETPOS;
    }
}

void CUIPanel::LateTick(_float _fDeltaTime)
{
    if (m_isLoadingDone)
    {
        if (m_isOnce && m_isWorldUI && !m_pUIChildren.empty())
        {
            m_isOnce = false;

            for (auto& childPair : m_pUIChildren)
            {
                if (childPair.second.lock() != nullptr)
                    childPair.second.lock()->SetIsWorldUI(true);
            }
        }

        if (m_isWorldUI)
        {
            if (m_iRenderState == 0)
            {
                if (m_isRender)
                {
                    if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_WORLDUI, shared_from_this())))
                        return;
                }
            }
        }

        else
        {
            if (m_iRenderState == 0)
            {
                if (m_isRender)
                {
                    /* 키 값에 따라 나눠서 넣어준다 */
                    /*if (FAILED(AddUIRenderGroup()))
                        return;*/
                    if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_UI, shared_from_this())))
                        return;
                }
            }
        }

        if (!m_pUIChildren.empty())
        {
            for (auto& childPair : m_pUIChildren)
            {
                if (childPair.second.lock() != nullptr)
                {
                    if (m_isRender)
                    {
                        if (childPair.second.lock()->GetUIType() == UI_ITEMBG)
                        {
                            if (dynamic_pointer_cast<CUIItemBackGround>(childPair.second.lock())->IsRender(m_fVisibleTop, m_fVisibleBottom))
                                childPair.second.lock()->LateTick(_fDeltaTime);
                        }

                        else if (childPair.second.lock()->GetUIType() == UI_ITEMICON)
                        {
                            if (dynamic_pointer_cast<CUIItemIcon>(childPair.second.lock())->IsRender(m_fVisibleTop, m_fVisibleBottom))
                                childPair.second.lock()->LateTick(_fDeltaTime);
                        }

                        /*else if (childPair.second.lock()->GetUIType() == UI_LOCKON)
                        {
                            if (m_isRender)
                                childPair.second.lock()->LateTick(_fDeltaTime);
                        }*/

                        else if (childPair.second.lock()->GetUIType() == UI_INSTANCEITEMBG)
                        {
                            dynamic_pointer_cast<CUIInstanceItemBG>(childPair.second.lock())->IsRender(m_fVisibleTop, m_fVisibleBottom);
                            childPair.second.lock()->LateTick(_fDeltaTime);
                        }

                        else
                            childPair.second.lock()->LateTick(_fDeltaTime);
                    }
                }
            }
        }
    }
}

HRESULT CUIPanel::Render()
{
    if (!m_pUIChildren.empty())
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iRenderState", &m_iRenderState, sizeof(_uint))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BeginShader()))
            return E_FAIL;

        if (FAILED(m_pBuffer->BindBuffers()))
            return E_FAIL;

        if (FAILED(m_pBuffer->Render()))
            return E_FAIL;
    }

    else
    {
        if (FAILED(GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_BindWorldMat)))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDiscardAlpha", &m_UIDesc.fDiscardAlpha, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iRenderState", &m_iRenderState, sizeof(_uint))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_iTexType", &m_UIDesc.eUITexType, sizeof(UI_TEX_TYPE))))
            return E_FAIL;

        if (!m_strTexKeys.empty())
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTexKeys[m_iTexIndex])))
                return E_FAIL;
        }

        if (m_UIDesc.isMasked)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_MaskTexture", m_strMaskKey)))
                return E_FAIL;
        }

        if (m_UIDesc.isNoised)
        {
            if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strNoiseKey)))
                return E_FAIL;
        }

        if (FAILED(GAMEINSTANCE->BeginShader()))
            return E_FAIL;

        if (FAILED(m_pBuffer->BindBuffers()))
            return E_FAIL;

        if (FAILED(m_pBuffer->Render()))
            return E_FAIL;
    }

    return S_OK;
}

void CUIPanel::SetVisibleRange(_float _fVisibleTop, _float _fVisibleBottom)
{
    m_fVisibleTop = _fVisibleTop;
    m_fVisibleBottom = _fVisibleBottom;
}

HRESULT CUIPanel::AddUIRenderGroup()
{
    /*if (m_UIDesc.strTag == "BossHPPanel")
    {
        
    }*/

    return S_OK;
}

HRESULT CUIPanel::AddComponent(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    shared_ptr<CVIInstancePoint> pBufferCom = dynamic_pointer_cast<CVIInstancePoint>(GAMEINSTANCE->GetBuffer("Buffer_InstancePoint"));
    if (pBufferCom == nullptr)
        return E_FAIL;

    m_pBuffer = dynamic_pointer_cast<CVIInstancePoint>(pBufferCom->Clone(1, &_InstanceDesc));
    if (m_pBuffer == nullptr)
        return E_FAIL;

    m_strTexKeys = _strTextureTags;

    if (m_UIDesc.isMasked)
        m_strMaskKey = _strMaskTextureTag;

    if (m_UIDesc.isNoised)
        m_strNoiseKey = _strNoiseTextureTag;

    return S_OK;
}

shared_ptr<CUIPanel> CUIPanel::Create(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CUIBase::UIDesc& _UIDesc, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    shared_ptr<CUIPanel> pInstance = make_shared<CUIPanel>();

    if (FAILED(pInstance->Initialize(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _UIDesc, _InstanceDesc)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CUIPanel::Create", MB_OK);

        pInstance.reset();

        return nullptr;
    }

    return pInstance;
}
