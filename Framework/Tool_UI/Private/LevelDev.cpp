#include "pch.h"
#include "LevelDev.h"
#include "ToolFreeCamera.h"
#include "ToolUIMgr.h"
#include "ToolUIIcon.h"
#include "ToolUIButton.h"
#include "ToolUISlider.h"
#include "ToolUIBar.h"
#include "ToolUIScroll.h"
#include "ToolUISlot.h"
#include "ToolUICoolTime.h"
#include "ToolUIUV.h"
#include "ToolUIPanel.h"
#include "ToolUIBG.h"
#include "ToolUIBackGround.h"
#include "ToolUIHover.h"
#include "ToolUICover.h"
#include "ToolUIItemBackGround.h"
#include "ToolUIItemIcon.h"
#include "ToolUIItemHover.h"
#include "ToolUIItemTab.h"
#include "ToolUILockOn.h"
#include "ToolUIText.h"
#include "ToolUIInstance.h"
#include "ToolUIInstanceItemBG.h"
#include "ToolUIInstanceItemSlot.h"
#include "ToolUIInstanceItemIcon.h"
#include "ToolUIInstanceItemCoolTime.h"
#include "ToolUIInstanceItemHover.h"
#include "ToolUIInstanceQuickBG.h"
#include "ToolUIInstanceQuickSlot.h"
#include "ToolUIInstanceParring.h"
#include "ToolUIInstanceAbnormalSlot.h"
#include "ToolUIInstanceWorldHP.h"
#include "ToolUIInstanceBossHP.h"
#include "ToolUINormalText.h"
#include "ToolUIInstanceBossAbnormal.h"
#include "ToolUIInstanceQuest.h"
#include "ToolUIInstanceQuestIndicator.h"
#include "ToolUIInstanceInteraction.h"
#include "ToolUIInstanceDialogue.h"
#include "ToolUICraftBG.h"
#include "ToolUIInstanceCraftTab.h"
#include "ToolUICraftHover.h"
#include "ToolUICraftSlot.h"
#include "ToolUITradeBG.h"
#include "ToolUITradeSlot.h"
#include "ToolUITradeTab.h"
#include "ToolUITradeHover.h"
#include "ToolUIQuestComplete.h"
#include "ToolUIItemToolTip.h"
#include "ToolUISpeechBallon.h"
#include "ToolUIStarForce.h"
#include "ToolUIOptionBG.h"
#include "ToolUIOptionTab.h"
#include "ToolUIOptionHover.h"
#include "ToolUIOptionSlider.h"
#include "ToolUIOptionCheckBox.h"
#include "ToolUICraftResult.h"
#include "ToolUITutorial.h"

#include "GameInstance.h"
#include "Texture.h"
#include "GameObject.h"



Tool_UI::CLevelDev::CLevelDev()
{

}

Tool_UI::CLevelDev::~CLevelDev()
{

}

HRESULT Tool_UI::CLevelDev::Initialize()
{
    ReadyLight();

    if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(ReadyCamera(TEXT("Layer_Camera"))))
        return E_FAIL;

    if (FAILED(LoadTextureTags(TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/"))))
        return E_FAIL;

    m_pToolUIManager = UIMGR;
    if (m_pToolUIManager == nullptr)
        return E_FAIL;

    if (FAILED(m_pToolUIManager->Initialize()))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->AddFont("Font_Nanum", TEXT("../../Client/Bin/Resources/Level_Static/Textures/UI/Font/AsiaGothic.spritefont"))))
        return E_FAIL;

    m_allUIs = m_pToolUIManager->GetAllUIs();
    m_allActivatedUIs = m_pToolUIManager->GetAllActivatedUIs();

    return S_OK;
}

void Tool_UI::CLevelDev::Tick(_float _fTimeDelta)
{
    ImGui::Begin("Tool Widget");

    POINT mousePt;
    ::GetCursorPos(&mousePt);
    ::ScreenToClient(g_hWnd, &mousePt);

    _float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
    _float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

    ImGui::Text(std::format("{0}, {1}", mousePosX, mousePosY).c_str());

    if (ImGui::BeginTabBar("Select To Modify"), ImGuiTabBarFlags_FittingPolicyMask_)
    {
        if (ImGui::BeginTabItem("UI"))
        {
            ImGui::Text("This is UI Tab");

            if (ImGui::TreeNode("InstanceData"))
            {
                ImGui::InputFloat3("Center", m_UICenter.data());
                ImGui::InputFloat2("Size", m_UISize.data());
                ImGui::InputFloat2("Speed", m_UISpeed.data());
                ImGui::Checkbox("IsLoop", &m_isLoopUpper);
                ImGui::InputFloat4("Color", m_vUIColor.data());
                ImGui::InputFloat("Duration", &m_fUIDuration);
                ImGui::InputFloat("ScaleY", &m_fUIScaleY);
                ImGui::InputFloat2("UVs", m_UIuv.data());

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Create UI"))
            {
                /*if (ImGui::BeginListBox("Texture List"))
                {
                    for (_uint i = 0; i < m_texTags.size(); ++i)
                    {
                        ImGui::PushID(i);
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.f, 1.f));

                        ImVec2 size = ImVec2(32.0f, 32.0f);                         // Size of the image we want to make visible
                        ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
                        ImVec2 uv1 = ImVec2(1.f, 1.f);								// UV coordinates for (32,32) in our texture
                        ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
                        ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint

                        _tchar wTextureKey[MAX_PATH] = TEXT("");

                        MultiByteToWideChar(CP_ACP, 0, m_texTags[i].c_str(), static_cast<int>(strlen(m_texTags[i].c_str())), wTextureKey, MAX_PATH);

                        if (ImGui::ImageButton(m_texTags[i].c_str(), GAMEINSTANCE->GetTexture(m_texTags[i])->, size, uv0, uv1, bg_col, tint_col))
                        {
                            _particleTextureTag = m_texTags[i];
                        }

                        ImGui::PopStyleVar();

                        ImGui::PopID();
                        if (i == 0 || i % 5 != 0)
                            ImGui::SameLine();
                    }

                    ImGui::EndListBox();
                }*/

                ImGui::NewLine();

                ImGui::InputText("UITag", &m_strUITag);
                ImGui::InputInt("UILayer", &m_iUILayer);
                ImGui::InputInt("UIType", &m_iUIType);
                ImGui::InputInt("UITexType", &m_iUITexType);
                ImGui::InputInt("UISlotType", &m_iUISlotType);
                ImGui::InputText("UIDiffuseTag", &m_strUIDiffuseTag);

                if (ImGui::Button("Push Texture"))
                {
                    string strTexTag = m_strUIDiffuseTag;

                    auto it = find_if(m_createTexTags.begin(), m_createTexTags.end(), [&strTexTag](string createTexTag) {
                        if (createTexTag == strTexTag)
                            return true;

                        return false;
                        });

                    if (it == m_createTexTags.end())
                        m_createTexTags.emplace_back(m_strUIDiffuseTag);
                    else
                    {
                        MSG_BOX("Same Texture Already In");
                    }
                }

                ImGui::Checkbox("IsMasked", &m_isMasked);
                ImGui::InputText("UIMaskTag", &m_strUIMaskTag);
                ImGui::Checkbox("IsNoised", &m_isNoised);
                ImGui::InputText("UINoiseTag", &m_strUINoiseTag);
                ImGui::InputInt("NumUIInstance", &m_iNumUIInstance);
                ImGui::InputFloat("DiscardAlpha", &m_fUIDiscardAlpha);
                ImGui::InputFloat("ZOrder", &m_fZOrder);
                if (m_iUISlotType == TOOL_SLOT_INVEN)
                    ImGui::InputInt("UI ItemType", &m_iUIItemType);
                if (m_iUIType == TOOL_UI_SCROLLBG)
                    ImGui::InputFloat("UI VisibleSizeY", &m_fVisibleSizeY);
                if (m_iUIType == TOOL_UI_TEXT || m_iUIType == TOOL_UI_NORMALTEXT)
                    ImGui::InputText("Text Script", m_cTextScripts, MAX_PATH);

                if (ImGui::Button("CreateUI"))
                {
                    m_toolUIDesc.m_strTag = m_strUITag;
                    m_toolUIDesc.m_eUILayer = static_cast<ETOOL_UI_LAYER>(m_iUILayer);
                    m_toolUIDesc.m_eUIType = static_cast<ETOOL_UI_TYPE>(m_iUIType);
                    m_toolUIDesc.m_eUITexType = static_cast<ETOOL_UI_TEX_TYPE>(m_iUITexType);
                    m_toolUIDesc.m_eUISlotType = static_cast<ETOOL_SLOT_TYPE>(m_iUISlotType);
                    m_toolUIDesc.m_isMasked = m_isMasked;
                    m_toolUIDesc.isNoised = m_isNoised;
                    m_toolUIDesc.fDiscardAlpha = m_fUIDiscardAlpha;
                    m_toolUIDesc.fZOrder = m_fZOrder;

                    m_pUIInstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
                    m_pUIInstanceDesc.vCenter = _float3(m_UICenter[0], m_UICenter[1], m_UICenter[2]);
                    m_pUIInstanceDesc.vRange = _float3(0.f, 0.f, 0.f);
                    m_pUIInstanceDesc.vSize = _float2(m_UISize[0], m_UISize[1]);
                    m_pUIInstanceDesc.vSpeed = _float2(m_UISpeed[0], m_UISpeed[1]);
                    m_pUIInstanceDesc.isLoop = m_isLoopUpper;
                    m_pUIInstanceDesc.vColor = _float4(m_vUIColor[0], m_vUIColor[1], m_vUIColor[2], m_vUIColor[3]);
                    m_pUIInstanceDesc.fDuration = m_fUIDuration;
                    m_pUIInstanceDesc.fSizeY = m_fUIScaleY;

                    if (m_toolUIDesc.m_eUISlotType != TOOL_SLOT_MOUSE)
                    {
                        if (m_createTexTags.empty())
                        {
                            MSG_BOX("Please Push Texture");
                        }
                        else
                        {
                            CreateUI(m_createTexTags, m_strUIMaskTag, m_strUINoiseTag, m_toolUIDesc, m_pUIInstanceDesc);
                        }
                    }

                    else
                        CreateUI(m_createTexTags, m_strUIMaskTag, m_strUINoiseTag, m_toolUIDesc, m_pUIInstanceDesc);
                }

                ImGui::TreePop();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Modify UI"))
        {
            if (ImGui::BeginListBox("UI List"))
            {
                m_allActivatedUITags.clear();

                for (auto& pUI : *m_allActivatedUIs)
                {
                    m_allActivatedUITags.emplace_back(pUI.lock()->GetUITag());
                }

                static _int curUIIndex{ 0 };

                _uint uiListSize = static_cast<_uint>(m_allActivatedUITags.size());
                for (_uint i = 0; i < uiListSize; ++i)
                {
                    const bool isSelected = (curUIIndex == i);
                    if (ImGui::Selectable(m_allActivatedUITags[i].c_str(), isSelected))
                        curUIIndex = i;

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                        m_selectedUIIndex = i;

                        /* UI가 선택될 때 LatestActivatedUI가 갱신되면 안되지 않을까? -> 이유 : 부모 자식 설정 귀찮음 */
                        /* UI자체를 클릭할때만 LatestActivatedUI를 갱신 */
                        /* List를 클릭하면 DevLevel내의 SelectedUITag만 갱신하게끔? */
                        m_strSelUITag = m_allActivatedUITags[m_selectedUIIndex];
                    }
                }

                ImGui::EndListBox();
            }

            weak_ptr<CToolUIBase> pFoundUI = m_pToolUIManager->FindUI(m_strSelUITag);
            if (!pFoundUI.expired())
            {
                ImGui::Text("Cur UI Pos : x(%f), y(%f)", pFoundUI.lock()->GetUIPos().x, pFoundUI.lock()->GetUIPos().y);
            }

            ImGui::InputFloat2("UI Pos", m_UIPos.data());
            if (ImGui::Button("Adjust UI Pos"))
            {
                //shared_ptr<CToolUIBase> pFoundUI = m_pToolUIManager->FindUI(m_strSelUITag);
                if (!pFoundUI.expired())
                {
                    pFoundUI.lock()->SetUIPos(m_UIPos[0], m_UIPos[1]);
                }
            }

            if (!pFoundUI.expired())
                ImGui::Text("Cur UI Size : x(%f), y(%f)", pFoundUI.lock()->GetUISize().x, pFoundUI.lock()->GetUISize().y);

            ImGui::InputFloat2("UI Scale", m_UIScale.data());
            if (ImGui::Button("Adjust UI Scale"))
            {
                //shared_ptr<CToolUIBase> pFoundUI = m_pToolUIManager->FindUI(m_strSelUITag);
                if (!pFoundUI.expired())
                {
                    pFoundUI.lock()->SetBufferSize(m_UIScale[0], m_UIScale[1]);
                }
            }

            ImGui::InputFloat3("UI Rotation", m_UIRotation.data());
            if (ImGui::Button("Adjust UI Rotation"))
            {
                //shared_ptr<CToolUIBase> pFoundUI = m_pToolUIManager->FindUI(m_strSelUITag);
                if (!pFoundUI.expired())
                {
                    pFoundUI.lock()->RotateUI(m_UIRotation[0], m_UIRotation[1], m_UIRotation[2]);
                }
            }

            if (!pFoundUI.expired())
                ImGui::Text("Cur UI Color : %f, %f, %f, %f", pFoundUI.lock()->GetColor().x, pFoundUI.lock()->GetColor().y, pFoundUI.lock()->GetColor().z, pFoundUI.lock()->GetColor().w);

            ImGui::InputFloat4("UI Color", m_vModUIColor.data());
            if (ImGui::Button("Adjust UI Color"))
            {
                if (!pFoundUI.expired())
                {
                    pFoundUI.lock()->SetColor(_float4(m_vModUIColor[0], m_vModUIColor[1], m_vModUIColor[2], m_vModUIColor[3]));
                }
            }

            if (!pFoundUI.expired())
                ImGui::Text("Cur UI DiscardAlpha : %f", pFoundUI.lock()->GetDiscardAlpha());

            ImGui::InputFloat("UI DiscardAlpha", &m_fModUIDiscardAlpha);
            if (ImGui::Button("Adjust UI DiscardAlpha"))
            {
                if (!pFoundUI.expired())
                {
                    pFoundUI.lock()->SetDiscardAlpha(m_fModUIDiscardAlpha);
                }
            }

            if (!pFoundUI.expired())
                ImGui::Text("Cur UI ZOrder : %f", pFoundUI.lock()->GetZOrder());

            ImGui::InputFloat("ZOrder", &m_fzOrder);
            if (ImGui::Button("Adjust UI ZOrder"))
            {
                //shared_ptr<CToolUIBase> pFoundUI = m_pToolUIManager->FindUI(m_strSelUITag);
                if (!pFoundUI.expired())
                {
                    pFoundUI.lock()->SetZOrder(m_fzOrder);
                }
            }

            if (!pFoundUI.expired())
                ImGui::Text("Cur UI ShaderPassIndex : %d", pFoundUI.lock()->GetShaderPass());

            ImGui::InputInt("ShaderPass", &m_iShaderPassIndex);
            if (ImGui::Button("ADjust ShaderPass"))
            {
                if (!pFoundUI.expired())
                    pFoundUI.lock()->SetShaderPassIndex(m_iShaderPassIndex);
            }

            ImGui::InputText("Textures", &m_strSwapTexTag);
            if (ImGui::Button("Push Texture"))
            {
                string strTexTag = m_strSwapTexTag;

                auto it = find_if(m_swapTags.begin(), m_swapTags.end(), [&strTexTag](string createTexTag) {
                    if (createTexTag == strTexTag)
                        return true;

                    return false;
                    });

                if (it == m_swapTags.end())
                    m_swapTags.emplace_back(strTexTag);
                else
                {
                    MSG_BOX("Same Texture Already In");
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Swap Textures"))
            {
                if (m_swapTags.size() < 1)
                {
                    MSG_BOX("No Texture is in the Container");
                }

                else
                {
                    if (!pFoundUI.expired())
                    {
                        pFoundUI.lock()->SwapTextures(m_swapTags);

                        m_swapTags.clear();
                        m_swapTags.shrink_to_fit();
                    }
                }
            }

            ImGui::NewLine();

            weak_ptr<CToolUIBase> pLatestActivatedUI = m_pToolUIManager->GetLatestActivatedUI();
            if (!pLatestActivatedUI.expired())
            {
                if (pLatestActivatedUI.lock()->GetUITag() == m_strLatestSelUITag)
                {
                    ImGui::Text(pLatestActivatedUI.lock()->GetUITag().c_str());
                    ImGui::Text(m_strSelUITag.c_str());
                }
            }

            if (ImGui::Button("Add UI Child"))
            {
                if (pLatestActivatedUI.lock()->GetUITag() != m_strSelUITag)
                {
                    m_pToolUIManager->AddUIChild(m_strSelUITag);

                    string selUITag = m_strSelUITag;

                    auto it = find_if(m_strUIList.begin(), m_strUIList.end(), [&selUITag](string strUITag) {
                        if (strUITag == selUITag)
                            return true;

                        return false;
                        });

                    if (it != m_strUIList.end())
                        m_strUIList.erase(it);
                }

                else
                {
                    MSG_BOX("Parent UI & Child UI is the Same UI");
                }
            }

            //if (ImGui::Button("Add UI Parent"))
            //{
            //    m_pToolUIManager->AddUIParent(m_strSelUITag); // 필요한가?
            //}

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Parsing"))
        {
            /*if (ImGui::TreeNodeEx("Save UI"))
            {
                ImGui::InputText("UISaveFile", &m_strSaveFile);

                if (ImGui::Button("Save UI"))
                {
                    string finalFilePath = m_strSavePath + m_strSaveFile + ".data";

                    std::ofstream outFile(finalFilePath, std::ios::binary);

                    if (outFile.is_open())
                    {
                        SaveUI(outFile, m_strLatestSelUITag);
                    }
                }

                ImGui::TreePop();
            }*/

            if (ImGui::TreeNodeEx("Save UI JSON"))
            {
                ImGui::InputText("UISaveFile", &m_strSaveFile);

                if (ImGui::Button("Save UI"))
                {
                    string finalFilePath = m_strSavePath + m_strSaveFile + ".json";
                    
                    Json::Value root;

                    SaveUIJSON(root, m_strLatestSelUITag, m_strSaveFile);

                    _tchar tFinalPath[MAX_PATH] = TEXT("");
                    MultiByteToWideChar(CP_ACP, 0, finalFilePath.c_str(), static_cast<_uint>(strlen(finalFilePath.c_str())), tFinalPath, MAX_PATH);

                    wstring wstrFinalPath = tFinalPath;

                    GAMEINSTANCE->WriteJson(root, wstrFinalPath);
                }

                if (ImGui::Button("Save Instance Slot"))
                {
                    string finalFilePath = m_strSavePath + m_strSaveFile + ".json";

                    Json::Value root;

                    SaveUIInstanceJSON(root, m_strLatestSelUITag, m_strSaveFile);

                    _tchar tFinalPath[MAX_PATH] = TEXT("");
                    MultiByteToWideChar(CP_ACP, 0, finalFilePath.c_str(), static_cast<_uint>(strlen(finalFilePath.c_str())), tFinalPath, MAX_PATH);

                    wstring wstrFinalPath = tFinalPath;

                    GAMEINSTANCE->WriteJson(root, wstrFinalPath);
                }

                if (ImGui::Button("Save Instance Normal"))
                {
                    string finalFilePath = m_strSavePath + m_strSaveFile + ".json";

                    Json::Value root;

                    SaveUIInstanceNormal(root, m_strLatestSelUITag, m_strSaveFile);

                    _tchar tFinalPath[MAX_PATH] = TEXT("");
                    MultiByteToWideChar(CP_ACP, 0, finalFilePath.c_str(), static_cast<_uint>(strlen(finalFilePath.c_str())), tFinalPath, MAX_PATH);

                    wstring wstrFinalPath = tFinalPath;

                    GAMEINSTANCE->WriteJson(root, wstrFinalPath);
                }

                ImGui::TreePop();
            }

            /*if (ImGui::TreeNodeEx("Load UI"))
            {
                ImGui::InputText("UIDataFile", &m_strLoadFile);

                if (ImGui::Button("Load UI"))
                {
                    string finalFilePath = m_strSavePath + m_strLoadFile + ".data";

                    std::ifstream inFile(finalFilePath, std::ios::binary);

                    if (inFile.is_open())
                    {
                        m_uiChildrenTags.clear();

                        LoadUI(inFile);
                    }
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Load Noise UI"))
            {
                ImGui::InputText("UIDataFile", &m_strLoadNoiseFile);

                if (ImGui::Button("Load Noise UI"))
                {
                    string finalFilePath = m_strSavePath + m_strLoadNoiseFile + ".data";

                    std::ifstream inFile(finalFilePath, std::ios::binary);

                    if (inFile.is_open())
                    {
                        m_uiChildrenTags.clear();

                        LoadUIComplete(inFile);
                    }
                }

                ImGui::TreePop();
            }*/

            if (ImGui::TreeNodeEx("Load UI JSON"))
            {
                ImGui::InputText("UILoadFile", &m_strLoadJsonFile);

                if (ImGui::Button("Load UI"))
                {
                    string finalFilePath = m_strSavePath + m_strLoadJsonFile + ".json";

                    Json::Value root;

                    _tchar tFinalPath[MAX_PATH] = TEXT("");
                    MultiByteToWideChar(CP_ACP, 0, finalFilePath.c_str(), static_cast<_uint>(strlen(finalFilePath.c_str())), tFinalPath, MAX_PATH);

                    wstring wstrFinalPath = tFinalPath;

                    root = GAMEINSTANCE->ReadJson(wstrFinalPath);

                    m_uiChildrenTags.clear();

                    LoadUIJSON(root, m_strLoadJsonFile);
                }

                if (ImGui::Button("Load Instance UI"))
                {
                    string finalFilePath = m_strSavePath + m_strLoadJsonFile + ".json";

                    Json::Value root;

                    _tchar tFinalPath[MAX_PATH] = TEXT("");
                    MultiByteToWideChar(CP_ACP, 0, finalFilePath.c_str(), static_cast<_uint>(strlen(finalFilePath.c_str())), tFinalPath, MAX_PATH);

                    wstring wstrFinalPath = tFinalPath;

                    root = GAMEINSTANCE->ReadJson(wstrFinalPath);

                    m_uiChildrenTags.clear();

                    LoadUIInstanceJSON(root, m_strLoadJsonFile);
                }

                ImGui::TreePop();
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

    ImGui::Begin("Select UI List");

    if (ImGui::BeginTabBar("Select To Modify"), ImGuiTabBarFlags_FittingPolicyMask_)
    {
        if (ImGui::BeginTabItem("UI List"))
        {
            if (ImGui::BeginListBox("UI List"))
            {
                m_allActivatedUITags.clear();

                for (auto& pUI : *m_allActivatedUIs)
                {
                    m_allActivatedUITags.emplace_back(pUI.lock()->GetUITag());
                }

                static _int curSelUIIndex{ 0 };

                _uint uiListSize = static_cast<_uint>(m_allActivatedUITags.size());
                for (_uint i = 0; i < uiListSize; ++i)
                {
                    const bool isSelected = (curSelUIIndex == i);
                    if (ImGui::Selectable(m_allActivatedUITags[i].c_str(), isSelected))
                        curSelUIIndex = i;

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                        m_selUIIndex = i;

                        /* UI가 선택될 때 LatestActivatedUI가 갱신되면 안되지 않을까? -> 이유 : 부모 자식 설정 귀찮음 */
                        /* UI자체를 클릭할때만 LatestActivatedUI를 갱신 */
                        /* List를 클릭하면 DevLevel내의 SelectedUITag만 갱신하게끔? */
                        m_strLatestSelUITag = m_allActivatedUITags[m_selUIIndex];
                    }
                }

                ImGui::EndListBox();
            }

            if (ImGui::Button("Activate Selected UI"))
            {
                m_pToolUIManager->SelectUI(m_strLatestSelUITag);
            }

            if (ImGui::Button("Delete Selected UI"))
            {
                m_pToolUIManager->DeleteSelectedUI(m_strLatestSelUITag);

                string sTest = m_strLatestSelUITag;

                auto it = find_if(m_strUIList.begin(), m_strUIList.end(), [&sTest](string listTag) {
                    if (sTest == listTag)
                        return true;

                    return false;
                    });

                if (it != m_strUIList.end())
                {
                    m_strUIList.erase(it);
                }
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

    ImGui::Begin("Select UI Child List");

    if (ImGui::BeginTabBar("Select To Modify"), ImGuiTabBarFlags_FittingPolicyMask_)
    {
        if (ImGui::BeginTabItem("UI Child List"))
        {
            if (ImGui::BeginListBox("UI Child List"))
            {
                m_strChildUIList.clear();
                m_strChildUIList.shrink_to_fit();

                shared_ptr<CToolUIBase> pFoundUI = m_pToolUIManager->FindUI(m_strLatestSelUITag);
                if (pFoundUI != nullptr)
                {
                    unordered_map<string, weak_ptr<CToolUIBase>> pChildrenList = pFoundUI->GetChildrenList();
                    for (auto& pPair : pChildrenList)
                    {
                        if (!pPair.second.expired())
                        {
                            m_strChildUIList.emplace_back(pPair.second.lock()->GetUITag());
                        }
                    }
                }

                static _int curSelUIChildIndex{ 0 };

                _uint uiListSize = static_cast<_uint>(m_strChildUIList.size());
                for (_uint i = 0; i < uiListSize; ++i)
                {
                    const bool isSelected = (curSelUIChildIndex == i);
                    if (ImGui::Selectable(m_strChildUIList[i].c_str(), isSelected))
                        curSelUIChildIndex = i;

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                        m_selectedUIChildIndex = i;

                        /* UI가 선택될 때 LatestActivatedUI가 갱신되면 안되지 않을까? -> 이유 : 부모 자식 설정 귀찮음 */
                        /* UI자체를 클릭할때만 LatestActivatedUI를 갱신 */
                        /* List를 클릭하면 DevLevel내의 SelectedUITag만 갱신하게끔? */
                        m_strSelUIChildTag = m_strChildUIList[m_selectedUIChildIndex];
                    }
                }

                ImGui::EndListBox();
            }

            if (ImGui::Button("Remove UI Child"))
            {
                m_pToolUIManager->RemoveUIChild(m_strLatestSelUITag, m_strSelUIChildTag);

                string strChildTag = m_strSelUIChildTag;

                auto it = find_if(m_strUIList.begin(), m_strUIList.end(), [&strChildTag](string listTag) {
                    if (strChildTag == listTag)
                        return true;

                    return false;
                    });

                if (it == m_strUIList.end())
                    m_strUIList.emplace_back(strChildTag);
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

    ImGui::Begin("All UI List");

    if (ImGui::BeginTabBar("Select To Modify"), ImGuiTabBarFlags_FittingPolicyMask_)
    {
        if (ImGui::BeginTabItem("All UI List"))
        {
            if (ImGui::BeginListBox("UI Child List"))
            {
                m_allUITags.clear();

                for (auto& uiPair : *m_allUIs)
                {
                    if (uiPair.second != nullptr)
                    {
                        m_allUITags.emplace_back(uiPair.second->GetUITag());
                    }
                }

                static _int curSelAllUIIndex{ 0 };

                _uint uiListSize = static_cast<_uint>(m_allUITags.size());
                for (_uint i = 0; i < uiListSize; ++i)
                {
                    const bool isSelected = (curSelAllUIIndex == i);
                    if (ImGui::Selectable(m_allUITags[i].c_str(), isSelected))
                        curSelAllUIIndex = i;

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                        m_selectedAllUIIndex = i;

                        m_strAllUITag = m_allUITags[m_selectedAllUIIndex];
                    }
                }

                ImGui::EndListBox();
            }

            shared_ptr<CToolUIBase> pFoundUI = m_pToolUIManager->FindUI(m_strAllUITag);

            if (pFoundUI != nullptr)
                ImGui::Text(pFoundUI->GetUITag().c_str());
            ImGui::InputText("SwapText", &m_strSwapUITag);
            if (ImGui::Button("Swap UI Tag"))
            {
                m_pToolUIManager->DeleteSelectedUI(m_strAllUITag);

                if (pFoundUI != nullptr)
                    pFoundUI->SetUITag(m_strSwapUITag);

                m_pToolUIManager->AddUIAfterSwapTag(pFoundUI->GetUITag(), pFoundUI);

                pFoundUI.reset();
            }

            if (ImGui::Button("Activate Selected UI"))
                m_pToolUIManager->SelectUI(m_strAllUITag);

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

    m_pToolUIManager->Tick(_fTimeDelta);
}

void Tool_UI::CLevelDev::LateTick(_float _fTimeDelta)
{

}

HRESULT Tool_UI::CLevelDev::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("Dev레벨입니다."));
#endif

    //if FAILED(GAMEINSTANCE->RenderFont("Font_Nanum", TEXT("한글"), _float2(0.f, 0.f), CCustomFont::FA_CENTER, XMVectorSet(1.f, 1.f, 0.f, 1.f)))
    //    return E_FAIL;

    /*if (FAILED(GAMEINSTANCE->RenderFont("Font_Nanum", TEXT("한글 \n 한글"), _float2(100.f, 100.f), CCustomFont::FA_CENTER, _float4(1.f, 1.f, 0.f, 1.f))))
        return E_FAIL;*/

    return S_OK;
}

void Tool_UI::CLevelDev::CreateUI(vector<string>& _strTextureTags, const string& _strMaskTextureTag, const string& _strNoiseTextureTag, CToolUIBase::ToolUIDesc& _ToolUIDesc, CVIInstancing::InstanceDesc& _InstanceDesc)
{
    switch (_ToolUIDesc.m_eUIType)
    {
    case TOOL_UI_ICON:
    {
        shared_ptr<CToolUIIcon> pUIIcon = CToolUIIcon::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUIIcon == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUIIcon");

            pUIIcon.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUIIcon->GetUITag(), pUIIcon)))
            return;

        m_strUIList.emplace_back(pUIIcon->GetUITag());
        m_pToolUIManager->ActivateUI(pUIIcon->GetUITag());
    }
    break;
    case TOOL_UI_BUTTON:
    {
        shared_ptr<CToolUIButton> pUIButton = CToolUIButton::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUIButton == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUIButton");

            pUIButton.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUIButton->GetUITag(), pUIButton)))
            return;

        m_strUIList.emplace_back(pUIButton->GetUITag());
        m_pToolUIManager->ActivateUI(pUIButton->GetUITag());
    }
    break;
    case TOOL_UI_SLIDER:
    {
        shared_ptr<CToolUISlider> pUISlider = CToolUISlider::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUISlider == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUISlider");

            pUISlider.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUISlider->GetUITag(), pUISlider)))
            return;

        m_strUIList.emplace_back(pUISlider->GetUITag());
        m_pToolUIManager->ActivateUI(pUISlider->GetUITag());
    }
    break;
    case TOOL_UI_BAR:
    {
        shared_ptr<CToolUIBar> pUIBar = CToolUIBar::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUIBar == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUIBar");

            pUIBar.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUIBar->GetUITag(), pUIBar)))
            return;

        m_strUIList.emplace_back(pUIBar->GetUITag());
        m_pToolUIManager->ActivateUI(pUIBar->GetUITag());
    }
    break;
    case TOOL_UI_SCROLL:
    {
        shared_ptr<CToolUIScroll> pUIScroll = CToolUIScroll::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUIScroll == nullptr)
        {
            MSG_BOX("Failed to Create : pUIScroll");

            pUIScroll.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUIScroll->GetUITag(), pUIScroll)))
            return;

        m_strUIList.emplace_back(pUIScroll->GetUITag());
        m_pToolUIManager->ActivateUI(pUIScroll->GetUITag());
    }
    break;
    case TOOL_UI_SLOT:
    {
        shared_ptr<CToolUISlot> pUISlot = CToolUISlot::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUISlot == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUISlot");

            pUISlot.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUISlot->GetUITag(), pUISlot)))
            return;

        m_strUIList.emplace_back(pUISlot->GetUITag());
        m_pToolUIManager->ActivateUI(pUISlot->GetUITag());
        pUISlot->SetItemType(static_cast<ETOOL_ITEM_TYPE>(m_iUIItemType));
    }
    break;
    case TOOL_UI_COOLTIME:
    {
        shared_ptr<CToolUICoolTime> pUICoolTime = CToolUICoolTime::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUICoolTime == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUICoolTime");

            pUICoolTime.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUICoolTime->GetUITag(), pUICoolTime)))
            return;

        m_strUIList.emplace_back(pUICoolTime->GetUITag());
        m_pToolUIManager->ActivateUI(pUICoolTime->GetUITag());
    }
    break;
    case TOOL_UI_UV:
    {
        shared_ptr<CToolUIUV> pUIUV = CToolUIUV::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc, _float2(m_UIuv[0], m_UIuv[1]));
        if (pUIUV == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUIUV");

            pUIUV.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUIUV->GetUITag(), pUIUV)))
            return;

        m_strUIList.emplace_back(pUIUV->GetUITag());
        m_pToolUIManager->ActivateUI(pUIUV->GetUITag());
    }
    break;
    case TOOL_UI_SCROLLBG:
    {
        shared_ptr<CToolUIBG> pUIBG = CToolUIBG::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc, m_fVisibleSizeY);
        if (pUIBG == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUIBG");

            pUIBG.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUIBG->GetUITag(), pUIBG)))
            return;

        m_strUIList.emplace_back(pUIBG->GetUITag());
        m_pToolUIManager->ActivateUI(pUIBG->GetUITag());
    }
    break;
    case TOOL_UI_PANEL:
    {
        shared_ptr<CToolUIPanel> pUIPanel = CToolUIPanel::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUIPanel == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUIPanel");

            pUIPanel.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUIPanel->GetUITag(), pUIPanel)))
            return;

        m_strUIList.emplace_back(pUIPanel->GetUITag());
        m_pToolUIManager->ActivateUI(pUIPanel->GetUITag());
    }
    break;
    case TOOL_UI_BACKGROUND:
    {
        shared_ptr<CToolUIBackGround> pUIBackGround = CToolUIBackGround::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUIBackGround == nullptr)
        {
            MSG_BOX("Failed to Create : CUIBackGround");

            pUIBackGround.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUIBackGround->GetUITag(), pUIBackGround)))
            return;

        m_strUIList.emplace_back(pUIBackGround->GetUITag());
        m_pToolUIManager->ActivateUI(pUIBackGround->GetUITag());
    }
    break;
    case TOOL_UI_BACKGROUND_R:
    {
        shared_ptr<CToolUIBackGround> pUIBackGroundR = CToolUIBackGround::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUIBackGroundR == nullptr)
        {
            MSG_BOX("Failed to Create : pUIBackGroundR");

            pUIBackGroundR.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUIBackGroundR->GetUITag(), pUIBackGroundR)))
            return;

        m_strUIList.emplace_back(pUIBackGroundR->GetUITag());
        m_pToolUIManager->ActivateUI(pUIBackGroundR->GetUITag());
    }
    break;
    case TOOL_UI_HOVER:
    {
        shared_ptr<CToolUIHover> pUIHover = CToolUIHover::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUIHover == nullptr)
        {
            MSG_BOX("Failed to Create : pUIHover");
            
            pUIHover.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUIHover->GetUITag(), pUIHover)))
            return;

        m_strUIList.emplace_back(pUIHover->GetUITag());
        m_pToolUIManager->ActivateUI(pUIHover->GetUITag());
    }
    break;
    case TOOL_UI_COVER:
    {
        shared_ptr<CToolUICover> pUICover = CToolUICover::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUICover == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUICover");

            pUICover.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUICover->GetUITag(), pUICover)))
            return;

        m_strUIList.emplace_back(pUICover->GetUITag());
        m_pToolUIManager->ActivateUI(pUICover->GetUITag());
    }
    break;
    case TOOL_UI_ITEMBG:
    {
        shared_ptr<CToolUIItemBackGround> pUI = CToolUIItemBackGround::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUI == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUIItemBackGround");

            pUI.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        m_strUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_ITEMICON:
    {
        shared_ptr<CToolUIItemIcon> pUI = CToolUIItemIcon::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUI == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUIItemIcon");

            pUI.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        m_strUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_ITEMHOVER:
    {
        shared_ptr<CToolUIItemHover> pUI = CToolUIItemHover::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUI == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUIItemHover");

            pUI.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        m_strUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_ITEMTAB:
    {
        shared_ptr<CToolUIItemTab> pUI = CToolUIItemTab::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUI == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUIItemHover");

            pUI.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        m_strUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_COVER_RV:
    {
        shared_ptr<CToolUICover> pUI = CToolUICover::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUI == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUICover");

            pUI.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        m_strUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_BACKGROUND_NT:
    {
        shared_ptr<CToolUIBackGround> pUI = CToolUIBackGround::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUI == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUICover");

            pUI.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        m_strUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_LOCKON:
    {
        shared_ptr<CToolUILockOn> pUI = CToolUILockOn::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUI == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUICover");

            pUI.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        m_strUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_HL:
    {
        shared_ptr<CToolUIBackGround> pUI = CToolUIBackGround::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUI == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUIBackGround");

            pUI.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        m_strUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_HR:
    {
        shared_ptr<CToolUIBackGround> pUI = CToolUIBackGround::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc);
        if (pUI == nullptr)
        {
            MSG_BOX("Failed to Create : CToolUIBackGround");

            pUI.reset();

            return;
        }

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        m_strUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_TEXT:
    {
        weak_ptr<CCustomFont> pFont = GAMEINSTANCE->FindFont("Font_Nanum");
        if (!pFont.expired())
        {
            char cScript[MAX_PATH] = "";
            wchar_t wideString[MAX_PATH] = L"";

            MultiByteToWideChar(CP_UTF8, 0, m_cTextScripts, MAX_PATH, wideString, MAX_PATH);

            wstring wString = wideString;
            RECT measuredRect = pFont.lock()->GetFont()->MeasureDrawBounds(wString.c_str(), _float2(0.f, 0.f));
            _float fSizeX = (measuredRect.right - measuredRect.left) + 5.f;
            _float fSizeY = (measuredRect.bottom - measuredRect.top);

            _InstanceDesc.vSize = _float2(fSizeX, fSizeX);
            _InstanceDesc.fSizeY = fSizeY;

            shared_ptr<CToolUIText> pUI = CToolUIText::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc, pFont.lock(), wString);
            if (pUI == nullptr)
            {
                MSG_BOX("Failed to Create : CToolUIText");

                pUI.reset();

                return;
            }

            if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
                return;

            m_strUIList.emplace_back(pUI->GetUITag());
            m_pToolUIManager->ActivateUI(pUI->GetUITag());
        }
    }
    break;
    case TOOL_UI_NORMALTEXT:
    {
        weak_ptr<CCustomFont> pFont = GAMEINSTANCE->FindFont("Font_Nanum");
        if (!pFont.expired())
        {
            char cScript[MAX_PATH] = "";
            wchar_t wideString[MAX_PATH] = L"";

            MultiByteToWideChar(CP_UTF8, 0, m_cTextScripts, MAX_PATH, wideString, MAX_PATH);

            wstring wString = wideString;
            shared_ptr<CToolUINormalText> pUI = CToolUINormalText::Create(_strTextureTags, _strMaskTextureTag, _strNoiseTextureTag, _ToolUIDesc, _InstanceDesc, pFont.lock(), wString);
            if (pUI == nullptr)
            {
                MSG_BOX("Failed to Create : CToolUINormalText");

                pUI.reset();

                return;
            }

            if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
                return;

            m_strUIList.emplace_back(pUI->GetUITag());
            m_pToolUIManager->ActivateUI(pUI->GetUITag());
        }
    }
    break;
    }

    m_createTexTags.clear();
}

void Tool_UI::CLevelDev::SaveUI(std::ofstream& _of, const string& _strUITag)
{
    shared_ptr<CToolUIBase> pFoundUI = m_pToolUIManager->FindUI(_strUITag);
    if (pFoundUI == nullptr)
        return;

    CVIInstancing::InstanceDesc saveInstanceDesc = pFoundUI->GetInstanceDesc(); // 구조체째로 저장 가능
    CToolUIBase::ToolUIDesc saveUIDesc = pFoundUI->GetToolUIDesc(); // 구조체째로 저장 불가능
    _uint iPassIndex = pFoundUI->GetShaderPass();
    vector<string> strTexKeys = pFoundUI->GetTexKeys();
    string strMaskKey;
    string strNoiseKey;
    if (saveUIDesc.m_isMasked)
        strMaskKey = pFoundUI->GetMaskKey();
    if (saveUIDesc.isNoised)
        strNoiseKey = pFoundUI->GetNoiseKey();

    /* 가장 먼저 Write할 거 -> ToolUIDesc의 UITag */
    /* UI의 이름이자 식별자 */
    char cStrTag[MAX_PATH];
    strcpy_s(cStrTag, saveUIDesc.m_strTag.c_str());
    
    _of.write(reinterpret_cast<char*>(cStrTag), MAX_PATH);
    _of.write(reinterpret_cast<char*>(&saveUIDesc.m_eUILayer), sizeof(ETOOL_UI_LAYER));
    _of.write(reinterpret_cast<char*>(&saveUIDesc.m_eUIType), sizeof(ETOOL_UI_TYPE));
    _of.write(reinterpret_cast<char*>(&saveUIDesc.m_eUITexType), sizeof(ETOOL_UI_TEX_TYPE));
    _of.write(reinterpret_cast<char*>(&saveUIDesc.m_eUISlotType), sizeof(ETOOL_SLOT_TYPE));
    _of.write(reinterpret_cast<char*>(&saveUIDesc.m_isMasked), sizeof(_bool));
    _of.write(reinterpret_cast<char*>(&saveUIDesc.isNoised), sizeof(_bool));
    _of.write(reinterpret_cast<char*>(&iPassIndex), sizeof(_uint));

    _of.write(reinterpret_cast<char*>(&saveInstanceDesc), sizeof(CVIInstancing::InstanceDesc));

    //_float fDiscardAlpha = pFoundUI->GetDiscardAlpha();
    _of.write(reinterpret_cast<char*>(&saveUIDesc.fDiscardAlpha), sizeof(_float));

    _uint iNumTexKeys = static_cast<_uint>(strTexKeys.size());
    _of.write(reinterpret_cast<char*>(&iNumTexKeys), sizeof(_uint));
    for (auto& texKey : strTexKeys)
    {
        char cTexKey[MAX_PATH] = "";
        strcpy_s(cTexKey, texKey.c_str());
        _of.write(reinterpret_cast<char*>(cTexKey), MAX_PATH);
    }

    if (saveUIDesc.m_isMasked == true)
    {
        char cMaskKey[MAX_PATH] = "";
        strcpy_s(cMaskKey, strMaskKey.c_str());
        _of.write(reinterpret_cast<char*>(cMaskKey), MAX_PATH);
    }

    if (saveUIDesc.isNoised == true)
    {
        char cNoiseKey[MAX_PATH] = "";
        strcpy_s(cNoiseKey, strNoiseKey.c_str());
        _of.write(reinterpret_cast<char*>(cNoiseKey), MAX_PATH);
    }

    if (saveUIDesc.m_eUIType == TOOL_UI_UV)
    {
        vector<_float4> atlasUVs = pFoundUI->GetAtlasUVs();
        _uint iNumUVs = static_cast<_uint>(atlasUVs.size());

        _of.write(reinterpret_cast<char*>(&iNumUVs), sizeof(_uint));

        for (auto& atlasUV : atlasUVs)
        {
            _of.write(reinterpret_cast<char*>(&atlasUV), sizeof(_float4));
        }
    }

    _float2 uiPos = pFoundUI->GetUIPos();
    _of.write(reinterpret_cast<char*>(&uiPos), sizeof(_float2));

    _float2 uiSize = pFoundUI->GetUISize();
    _of.write(reinterpret_cast<char*>(&uiSize), sizeof(_float2));

    /*shared_ptr<CGameObject> pOwner = pFoundUI->GetUIOwner();
    if (pOwner != nullptr)
    {
        _bool hasOwner{ true };
        _of.write(reinterpret_cast<char*>(&hasOwner), sizeof(_bool));
        // 어떤 객체인지 Tag를 적어줄 수 있나?

    }

    else
    {
        _bool hasOwner{ false };
        _of.write(reinterpret_cast<char*>(&hasOwner), sizeof(_bool));
    }*/

    //_float fZOrder = pFoundUI->GetZOrder();
    _of.write(reinterpret_cast<char*>(&saveUIDesc.fZOrder), sizeof(_float));

    /*shared_ptr<CToolUIBase> pUIParent = pFoundUI->GetUIParent();
    if (pUIParent == nullptr)
    {
        char cStrNull[MAX_PATH] = "None";
        _of.write(reinterpret_cast<char*>(cStrNull), MAX_PATH);
    }
    else
    {
        string strParentTag = pUIParent->GetUITag();
        char cStrParentTag[MAX_PATH] = "";
        strcpy_s(cStrParentTag, strParentTag.c_str());
        _of.write(reinterpret_cast<char*>(cStrParentTag), MAX_PATH);
    }*/

    unordered_map<string, weak_ptr<CToolUIBase>> pUIChildren = pFoundUI->GetUIChildren();
    _uint iNumChildren = static_cast<_uint>(pUIChildren.size());
    
    _of.write(reinterpret_cast<char*>(&iNumChildren), sizeof(_uint));
    if (iNumChildren <= 0)
        return;

    for (auto& pPair : pUIChildren)
    {
        string strChildTag = pPair.second.lock()->GetUITag();
        char cStrChildTag[MAX_PATH] = "";
        strcpy_s(cStrChildTag, strChildTag.c_str());
        _of.write(reinterpret_cast<char*>(cStrChildTag), MAX_PATH);
    }

    for (auto& pPair : pUIChildren)
    {
        SaveUI(_of, pPair.second.lock()->GetUITag());
    }
}

void Tool_UI::CLevelDev::LoadUI(std::ifstream& _if)
{
    CToolUIBase::ToolUIDesc toolUIDesc{};
    CVIInstancing::InstanceDesc instanceDesc{};
    _uint iNumTexKeys{ 0 };
    vector<string> strKeys;
    string strMaskKey;
    _float2 uiPos{};
    _float2 uiSize{};
    vector<_float4> atlasUVs;
    string strUIParentTag;
    vector<string> strUIChildTags;

    char cStrTag[MAX_PATH] = "";
    _if.read(reinterpret_cast<char*>(cStrTag), MAX_PATH);
    toolUIDesc.m_strTag = cStrTag;

    _if.read(reinterpret_cast<char*>(&toolUIDesc.m_eUILayer), sizeof(ETOOL_UI_LAYER));
    _if.read(reinterpret_cast<char*>(&toolUIDesc.m_eUIType), sizeof(ETOOL_UI_TYPE));
    _if.read(reinterpret_cast<char*>(&toolUIDesc.m_eUITexType), sizeof(ETOOL_UI_TEX_TYPE));
    _if.read(reinterpret_cast<char*>(&toolUIDesc.m_eUISlotType), sizeof(ETOOL_SLOT_TYPE));
    _if.read(reinterpret_cast<char*>(&toolUIDesc.m_isMasked), sizeof(_bool));
    toolUIDesc.isNoised = false;
    _if.read(reinterpret_cast<char*>(&toolUIDesc.iShaderPassIdx), sizeof(_uint));

    _if.read(reinterpret_cast<char*>(&instanceDesc), sizeof(CVIInstancing::InstanceDesc));

    _if.read(reinterpret_cast<char*>(&toolUIDesc.fDiscardAlpha), sizeof(_float));

    /* Texture */
    {
        _if.read(reinterpret_cast<char*>(&iNumTexKeys), sizeof(_uint));
        for (_uint i = 0; i < iNumTexKeys; ++i)
        {
            string strTex;
            char cStrTex[MAX_PATH] = "";
            _if.read(reinterpret_cast<char*>(cStrTex), MAX_PATH);
            strTex = cStrTex;
            
            strKeys.emplace_back(strTex);
        }
    }
    /* Mask Texture */
    if (toolUIDesc.m_isMasked)
    {
        char cStrMaskKey[MAX_PATH] = "";
        _if.read(reinterpret_cast<char*>(cStrMaskKey), MAX_PATH);
        strMaskKey = cStrMaskKey;
    }
    /* Atlas UVs */
    if (toolUIDesc.m_eUIType == TOOL_UI_UV)
    {
        _uint iNumUVs{ 0 };
        _if.read(reinterpret_cast<char*>(&iNumUVs), sizeof(_uint));

        for (_uint i = 0; i < iNumUVs; ++i)
        {
            _float4 atlasUV{};
            _if.read(reinterpret_cast<char*>(&atlasUV), sizeof(_float4));
            atlasUVs.emplace_back(atlasUV);
        }
    }
    /* Pos */
    _if.read(reinterpret_cast<char*>(&uiPos), sizeof(_float2));
    /* Size */
    _if.read(reinterpret_cast<char*>(&uiSize), sizeof(_float2));
    /* ZOrder */
    _if.read(reinterpret_cast<char*>(&toolUIDesc.fZOrder), sizeof(_float));

    string noiseKey = "";

    switch (toolUIDesc.m_eUIType)
    {
    case TOOL_UI_HUD:
    {

    }
    break;
    case TOOL_UI_BAR:
    {
        shared_ptr<CToolUIBar> pUI = CToolUIBar::Create(strKeys, strMaskKey, noiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_ICON:
    {
        shared_ptr<CToolUIIcon> pUI = CToolUIIcon::Create(strKeys, strMaskKey, noiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_SLIDER:
    {
        shared_ptr<CToolUISlider> pUI = CToolUISlider::Create(strKeys, strMaskKey, noiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_BUTTON:
    {
        //shared_ptr<CToolUIButton> pUI = CToolUIButton::Create(strKeys, strMaskKey, toolUIDesc, instanceDesc);

    }
    break;
    case TOOL_UI_SCROLLBG:
    {
        //shared_ptr<CToolUIBG> pUI = CToolUIBG::Create(strKeys, strMaskKey, noiseKey, toolUIDesc, instanceDesc);

        //if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
        //    return;

        ////pUI->SetUIPos(uiPos.x, uiPos.y);
        //pUI->SetBGInitPos(uiPos.x, uiPos.y);
        //m_strUIList.emplace_back(pUI->GetUITag());
        //m_strLoadUIList.emplace_back(pUI->GetUITag());
        //m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_SLOT:
    {
        shared_ptr<CToolUISlot> pUI = CToolUISlot::Create(strKeys, strMaskKey, noiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_SCROLL:
    {
        shared_ptr<CToolUIScroll> pUI = CToolUIScroll::Create(strKeys, strMaskKey, noiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_COOLTIME:
    {
        shared_ptr<CToolUICoolTime> pUI = CToolUICoolTime::Create(strKeys, strMaskKey, noiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_UV:
    {

    }
    break;
    case TOOL_UI_PANEL:
    {
        shared_ptr<CToolUIPanel> pUI = CToolUIPanel::Create(strKeys, strMaskKey, noiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_BACKGROUND:
    {
        shared_ptr<CToolUIBackGround> pUI = CToolUIBackGround::Create(strKeys, strMaskKey, noiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_BACKGROUND_R:
    {
        shared_ptr<CToolUIBackGround> pUI = CToolUIBackGround::Create(strKeys, strMaskKey, noiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    }

    /* UIParent */
    /*{
        char cStrUIParentTag[MAX_PATH] = "";
        _if.read(reinterpret_cast<char*>(cStrUIParentTag), MAX_PATH);
        strUIParentTag = cStrUIParentTag;
    }*/

    /* UI 부모, 자식 관계 저장 */
    /* UIChildren */
    {
        _uint iNumChildren{ 0 };
        _if.read(reinterpret_cast<char*>(&iNumChildren), sizeof(_uint));

        if (iNumChildren <= 0)
        {
            m_uiChildrenTags.emplace(toolUIDesc.m_strTag, strUIChildTags);

            return;
        }

        for (_uint i = 0; i < iNumChildren; ++i)
        {
            char cStrChildTag[MAX_PATH] = "";
            string strChildTag;
            _if.read(reinterpret_cast<char*>(cStrChildTag), MAX_PATH);
            strChildTag = cStrChildTag;

            strUIChildTags.emplace_back(strChildTag);
        }

        m_uiChildrenTags.emplace(toolUIDesc.m_strTag, strUIChildTags);

        for (_uint i = 0; i < iNumChildren; ++i)
        {
            LoadUI(_if);
        }
    }

    for (auto& loadUI : m_strLoadUIList)
    {
        auto it = m_uiChildrenTags.find(loadUI);
        if (it != m_uiChildrenTags.end())
        {
            vector<string> childrenTags = it->second;
            if (!childrenTags.empty())
            {
                for (auto& childTag : childrenTags)
                {
                    m_pToolUIManager->AddUIChild(it->first, childTag);
#pragma region Only For Tool
                    auto it = find_if(m_strUIList.begin(), m_strUIList.end(), [&childTag](string strUITag) {
                        if (strUITag == childTag)
                            return true;

                        return false;
                        });

                    if (it != m_strUIList.end())
                        m_strUIList.erase(it);
#pragma endregion
                }
            }
        }
    }
}

void Tool_UI::CLevelDev::LoadUIComplete(std::ifstream& _if)
{
    CToolUIBase::ToolUIDesc toolUIDesc{};
    CVIInstancing::InstanceDesc instanceDesc{};
    _uint iNumTexKeys{ 0 };
    vector<string> strKeys;
    string strMaskKey;
    string strNoiseKey;
    _float2 uiPos{};
    _float2 uiSize{};
    vector<_float4> atlasUVs;
    string strUIParentTag;
    vector<string> strUIChildTags;

    char cStrTag[MAX_PATH] = "";
    _if.read(reinterpret_cast<char*>(cStrTag), MAX_PATH);
    toolUIDesc.m_strTag = cStrTag;

    _if.read(reinterpret_cast<char*>(&toolUIDesc.m_eUILayer), sizeof(ETOOL_UI_LAYER));
    _if.read(reinterpret_cast<char*>(&toolUIDesc.m_eUIType), sizeof(ETOOL_UI_TYPE));
    _if.read(reinterpret_cast<char*>(&toolUIDesc.m_eUITexType), sizeof(ETOOL_UI_TEX_TYPE));
    _if.read(reinterpret_cast<char*>(&toolUIDesc.m_eUISlotType), sizeof(ETOOL_SLOT_TYPE));
    _if.read(reinterpret_cast<char*>(&toolUIDesc.m_isMasked), sizeof(_bool));
    _if.read(reinterpret_cast<char*>(&toolUIDesc.isNoised), sizeof(_bool));
    _if.read(reinterpret_cast<char*>(&toolUIDesc.iShaderPassIdx), sizeof(_uint));

    _if.read(reinterpret_cast<char*>(&instanceDesc), sizeof(CVIInstancing::InstanceDesc));

    _if.read(reinterpret_cast<char*>(&toolUIDesc.fDiscardAlpha), sizeof(_float));

    /* Texture */
    {
        _if.read(reinterpret_cast<char*>(&iNumTexKeys), sizeof(_uint));
        for (_uint i = 0; i < iNumTexKeys; ++i)
        {
            string strTex;
            char cStrTex[MAX_PATH] = "";
            _if.read(reinterpret_cast<char*>(cStrTex), MAX_PATH);
            strTex = cStrTex;

            strKeys.emplace_back(strTex);
        }
    }
    /* Mask Texture */
    if (toolUIDesc.m_isMasked)
    {
        char cStrMaskKey[MAX_PATH] = "";
        _if.read(reinterpret_cast<char*>(cStrMaskKey), MAX_PATH);
        strMaskKey = cStrMaskKey;
    }
    /* Noise Texture */
    if (toolUIDesc.isNoised)
    {
        char cStrNoiseKey[MAX_PATH] = "";
        _if.read(reinterpret_cast<char*>(cStrNoiseKey), MAX_PATH);
        strNoiseKey = cStrNoiseKey;
    }
    /* Atlas UVs */
    if (toolUIDesc.m_eUIType == TOOL_UI_UV)
    {
        _uint iNumUVs{ 0 };
        _if.read(reinterpret_cast<char*>(&iNumUVs), sizeof(_uint));

        for (_uint i = 0; i < iNumUVs; ++i)
        {
            _float4 atlasUV{};
            _if.read(reinterpret_cast<char*>(&atlasUV), sizeof(_float4));
            atlasUVs.emplace_back(atlasUV);
        }
    }
    /* Pos */
    _if.read(reinterpret_cast<char*>(&uiPos), sizeof(_float2));
    /* Size */
    _if.read(reinterpret_cast<char*>(&uiSize), sizeof(_float2));
    /* ZOrder */
    _if.read(reinterpret_cast<char*>(&toolUIDesc.fZOrder), sizeof(_float));

    switch (toolUIDesc.m_eUIType)
    {
    case TOOL_UI_HUD:
    {

    }
    break;
    case TOOL_UI_BAR:
    {
        shared_ptr<CToolUIBar> pUI = CToolUIBar::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_ICON:
    {
        shared_ptr<CToolUIIcon> pUI = CToolUIIcon::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_SLIDER:
    {
        shared_ptr<CToolUISlider> pUI = CToolUISlider::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_BUTTON:
    {
        //shared_ptr<CToolUIButton> pUI = CToolUIButton::Create(strKeys, strMaskKey, toolUIDesc, instanceDesc);

    }
    break;
    case TOOL_UI_SCROLLBG:
    {
        //shared_ptr<CToolUIBG> pUI = CToolUIBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        //if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
        //    return;

        ////pUI->SetUIPos(uiPos.x, uiPos.y);
        //pUI->SetBGInitPos(uiPos.x, uiPos.y);
        //m_strUIList.emplace_back(pUI->GetUITag());
        //m_strLoadUIList.emplace_back(pUI->GetUITag());
        //m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_SLOT:
    {
        shared_ptr<CToolUISlot> pUI = CToolUISlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_SCROLL:
    {
        shared_ptr<CToolUIScroll> pUI = CToolUIScroll::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_COOLTIME:
    {
        shared_ptr<CToolUICoolTime> pUI = CToolUICoolTime::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_UV:
    {

    }
    break;
    case TOOL_UI_PANEL:
    {
        shared_ptr<CToolUIPanel> pUI = CToolUIPanel::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_BACKGROUND:
    {
        shared_ptr<CToolUIBackGround> pUI = CToolUIBackGround::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_BACKGROUND_R:
    {
        shared_ptr<CToolUIBackGround> pUI = CToolUIBackGround::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    }

    /* UIParent */
    /*{
        char cStrUIParentTag[MAX_PATH] = "";
        _if.read(reinterpret_cast<char*>(cStrUIParentTag), MAX_PATH);
        strUIParentTag = cStrUIParentTag;
    }*/

    /* UI 부모, 자식 관계 저장 */
    /* UIChildren */
    {
        _uint iNumChildren{ 0 };
        _if.read(reinterpret_cast<char*>(&iNumChildren), sizeof(_uint));

        if (iNumChildren <= 0)
        {
            m_uiChildrenTags.emplace(toolUIDesc.m_strTag, strUIChildTags);

            return;
        }

        for (_uint i = 0; i < iNumChildren; ++i)
        {
            char cStrChildTag[MAX_PATH] = "";
            string strChildTag;
            _if.read(reinterpret_cast<char*>(cStrChildTag), MAX_PATH);
            strChildTag = cStrChildTag;

            strUIChildTags.emplace_back(strChildTag);
        }

        m_uiChildrenTags.emplace(toolUIDesc.m_strTag, strUIChildTags);

        for (_uint i = 0; i < iNumChildren; ++i)
        {
            LoadUIComplete(_if);
        }
    }

    for (auto& loadUI : m_strLoadUIList)
    {
        auto it = m_uiChildrenTags.find(loadUI);
        if (it != m_uiChildrenTags.end())
        {
            vector<string> childrenTags = it->second;
            if (!childrenTags.empty())
            {
                for (auto& childTag : childrenTags)
                {
                    m_pToolUIManager->AddUIChild(it->first, childTag);
#pragma region Only For Tool
                    auto it = find_if(m_strUIList.begin(), m_strUIList.end(), [&childTag](string strUITag) {
                        if (strUITag == childTag)
                            return true;

                        return false;
                        });

                    if (it != m_strUIList.end())
                        m_strUIList.erase(it);
#pragma endregion
                }
            }
        }
    }
}

void Tool_UI::CLevelDev::SaveUIJSON(Json::Value& root, const string& _strUITag, const string& _strSaveFile)
{
    //Json::Value root;
    Json::Value subRoot;

    shared_ptr<CToolUIBase> pFoundUI = m_pToolUIManager->FindUI(_strUITag);
    if (pFoundUI == nullptr)
        return;

    CVIInstancing::InstanceDesc saveInstanceDesc = pFoundUI->GetInstanceDesc();
    CToolUIBase::ToolUIDesc saveUIDesc = pFoundUI->GetToolUIDesc();
    _uint iPassIndex = pFoundUI->GetShaderPass();
    vector<string> strTexKeys = pFoundUI->GetTexKeys();
    string strMaskKey;
    string strNoiseKey;
    if (saveUIDesc.m_isMasked)
        strMaskKey = pFoundUI->GetMaskKey();
    if (saveUIDesc.isNoised)
        strNoiseKey = pFoundUI->GetNoiseKey();

    Json::Value UIDesc;
    UIDesc["UITag"] = saveUIDesc.m_strTag;
    UIDesc["UILayer"] = saveUIDesc.m_eUILayer;
    UIDesc["UIType"] = saveUIDesc.m_eUIType;
    UIDesc["UITexType"] = saveUIDesc.m_eUITexType;
    UIDesc["UISlotType"] = saveUIDesc.m_eUISlotType;
    UIDesc["UIIsMasked"] = saveUIDesc.m_isMasked;
    UIDesc["UIIsNoised"] = saveUIDesc.isNoised;
    UIDesc["UIPassIndex"] = iPassIndex;
    UIDesc["UIZOrder"] = saveUIDesc.fZOrder;
    UIDesc["UIDiscardAlpha"] = saveUIDesc.fDiscardAlpha;
    if (saveUIDesc.m_eUISlotType == TOOL_SLOT_INVEN || saveUIDesc.m_eUIType == TOOL_UI_INSTANCEITEMSLOT)
    {
        shared_ptr<CToolUISlot> uiSlot = dynamic_pointer_cast<CToolUISlot>(pFoundUI);
        if (uiSlot != nullptr)
            UIDesc["UIItemType"] = uiSlot->GetItemType();
    }
    if (saveUIDesc.m_eUIType == TOOL_UI_SCROLLBG)
    {
        shared_ptr<CToolUIBG> uiScrollBG = dynamic_pointer_cast<CToolUIBG>(pFoundUI);
        if (uiScrollBG != nullptr)
            UIDesc["UIVisibleSizeY"] = uiScrollBG->GetOriginVisibleSizeY();
    }
    subRoot["UIDesc"] = UIDesc;

    Json::Value InstanceDesc;

    Json::Value vPivot(Json::arrayValue);
    vPivot.append(saveInstanceDesc.vPivot.x);
    vPivot.append(saveInstanceDesc.vPivot.y);
    vPivot.append(saveInstanceDesc.vPivot.z);
    InstanceDesc["Pivot"] = vPivot;

    Json::Value vCenter(Json::arrayValue);
    vCenter.append(saveInstanceDesc.vCenter.x);
    vCenter.append(saveInstanceDesc.vCenter.y);
    vCenter.append(saveInstanceDesc.vCenter.z);
    InstanceDesc["Center"] = vCenter;

    Json::Value vRange(Json::arrayValue);
    vRange.append(saveInstanceDesc.vRange.x);
    vRange.append(saveInstanceDesc.vRange.y);
    vRange.append(saveInstanceDesc.vRange.z);
    InstanceDesc["Range"] = vRange;

    Json::Value vSize(Json::arrayValue);
    vSize.append(saveInstanceDesc.vSize.x);
    vSize.append(saveInstanceDesc.vSize.y);
    InstanceDesc["Size"] = vSize;

    Json::Value vSpeed(Json::arrayValue);
    vSpeed.append(saveInstanceDesc.vSpeed.x);
    vSpeed.append(saveInstanceDesc.vSpeed.y);
    InstanceDesc["Speed"] = vSpeed;

    Json::Value vLTime(Json::arrayValue);
    vLTime.append(saveInstanceDesc.vLifeTime.x);
    vLTime.append(saveInstanceDesc.vLifeTime.y);
    InstanceDesc["LifeTime"] = vLTime;

    Json::Value vDescColor(Json::arrayValue);
    vDescColor.append(saveInstanceDesc.vColor.x);
    vDescColor.append(saveInstanceDesc.vColor.y);
    vDescColor.append(saveInstanceDesc.vColor.z);
    vDescColor.append(saveInstanceDesc.vColor.w);
    InstanceDesc["Color"] = vDescColor;

    InstanceDesc["SizeY"] = saveInstanceDesc.fSizeY;
    InstanceDesc["Duration"] = saveInstanceDesc.fDuration;
    InstanceDesc["IsLoop"] = saveInstanceDesc.isLoop;

    subRoot["InstanceDesc"] = InstanceDesc;

    Json::Value Textures;

    Json::Value Diffuse(Json::arrayValue);
    for (auto& texKey : strTexKeys)
    {
        Diffuse.append(texKey);
    }
    Textures["Diffuse"] = Diffuse;

    if (saveUIDesc.m_isMasked == true)
        Textures["Mask"] = strMaskKey;

    if (saveUIDesc.isNoised == true)
        Textures["Noise"] = strNoiseKey;

    subRoot["Textures"] = Textures;

    if (saveUIDesc.m_eUIType == TOOL_UI_UV)
    {
        Json::Value AtlasUVs(Json::arrayValue);

        vector<_float4> atlasUVs = pFoundUI->GetAtlasUVs();

        for (auto& atlasUVs : atlasUVs)
        {
            Json::Value AtlasUV(Json::arrayValue);
            AtlasUV.append(atlasUVs.x);
            AtlasUV.append(atlasUVs.y);
            AtlasUV.append(atlasUVs.z);
            AtlasUV.append(atlasUVs.w);

            AtlasUVs.append(AtlasUV);
        }

        subRoot["AtlasUVs"] = AtlasUVs;
    }

    Json::Value SRT; // Scale Translation

    _float2 uiPos = pFoundUI->GetUIPos();

    Json::Value Pos(Json::arrayValue);
    Pos.append(uiPos.x);
    Pos.append(uiPos.y);

    _float2 uiSize = pFoundUI->GetUISize();

    Json::Value Scale(Json::arrayValue);
    Scale.append(uiSize.x);
    Scale.append(uiSize.y);

    SRT["Pos"] = Pos;
    SRT["Scale"] = Scale;

    subRoot["SRT"] = SRT;

    if (saveUIDesc.m_eUIType == TOOL_UI_INSTANCEITEMBG || saveUIDesc.m_eUIType == TOOL_UI_INSTANCEITEMSLOT ||
        saveUIDesc.m_eUIType == TOOL_UI_INSTANCEITEMICON || saveUIDesc.m_eUIType == TOOL_UI_INSTANCEITEMCOOLTIME ||
        saveUIDesc.m_eUIType == TOOL_UI_INSTANCEITEMHOVER || saveUIDesc.m_eUIType == TOOL_UI_INSTANCEQUICKBG ||
        saveUIDesc.m_eUIType == TOOL_UI_INSTANCEQUICKMAINSLOT || saveUIDesc.m_eUIType == TOOL_UI_INSTANCEQUICKITEMSLOT ||
        saveUIDesc.m_eUIType == TOOL_UI_INSTANCEQUICKCOMBATSLOT || saveUIDesc.m_eUIType == TOOL_UI_INSTANCEQUICKFRAME ||
        saveUIDesc.m_eUIType == TOOL_UI_INSTANCEPARRING || saveUIDesc.m_eUIType == TOOL_UI_INSTANCEABNORMALSLOT ||
        saveUIDesc.m_eUIType == TOOL_UI_INSTANCEWORLDHP || saveUIDesc.m_eUIType == TOOL_UI_INSTANCEBOSSHP ||
        saveUIDesc.m_eUIType == TOOL_UI_INSTANCEBOSSABNORMAL || saveUIDesc.m_eUIType == TOOL_UI_INSTANCEQUEST ||
        saveUIDesc.m_eUIType == TOOL_UI_INSTANCEQUESTINDICATOR || saveUIDesc.m_eUIType == TOOL_UI_INSTANCEINTERACTION ||
        saveUIDesc.m_eUIType == TOOL_UI_INSTANCEDIALOGUE || saveUIDesc.m_eUIType == TOOL_UI_CRAFTBG ||
        saveUIDesc.m_eUIType == TOOL_UI_CRAFTHOVER || saveUIDesc.m_eUIType == TOOL_UI_CRAFTTAB ||
        saveUIDesc.m_eUIType == TOOL_UI_CRAFTSLOT || saveUIDesc.m_eUIType == TOOL_UI_TRADEBG || saveUIDesc.m_eUIType == TOOL_UI_TRADESLOT ||
        saveUIDesc.m_eUIType == TOOL_UI_TRADETAB || saveUIDesc.m_eUIType == TOOL_UI_TRADEHOVER ||
        saveUIDesc.m_eUIType == TOOL_UI_QUESTCOMPLETE || saveUIDesc.m_eUIType == TOOL_UI_ITEMTOOLTIP ||
        saveUIDesc.m_eUIType == TOOL_UI_SPEECHBALLON || saveUIDesc.m_eUIType == TOOL_UI_STARFORCE ||
        saveUIDesc.m_eUIType == TOOL_UI_OPTIONBG || saveUIDesc.m_eUIType == TOOL_UI_OPTIONTAB ||
        saveUIDesc.m_eUIType == TOOL_UI_OPTIONHOVER || saveUIDesc.m_eUIType == TOOL_UI_OPTIONSLIDER ||
        saveUIDesc.m_eUIType == TOOL_UI_OPTIONCHECKBOX || saveUIDesc.m_eUIType == TOOL_UI_CRAFTRESULT ||
        saveUIDesc.m_eUIType == TOOL_UI_TUTORIAL)
    {
        Json::Value InstanceUIValues;

        vector<_float2> instanceUIPos = pFoundUI->GetInstanceUIPos();
        vector<_float2> instanceUISize = pFoundUI->GetInstanceUISize();

        _uint iNumInstanceValue = static_cast<_uint>(instanceUIPos.size());
        for (_uint i = 0; i < iNumInstanceValue; ++i)
        {
            Json::Value InstanceUIValue; // Pos & Size

            Json::Value InstanceUIPos(Json::arrayValue);
            InstanceUIPos.append(instanceUIPos[i].x);
            InstanceUIPos.append(instanceUIPos[i].y);

            Json::Value InstanceUISize(Json::arrayValue);
            InstanceUISize.append(instanceUISize[i].x);
            InstanceUISize.append(instanceUISize[i].y);

            InstanceUIValue["InstancePos"] = InstanceUIPos;
            InstanceUIValue["InstanceSize"] = InstanceUISize;

            string strKey = "InstanceValue" + to_string(i);

            InstanceUIValues[strKey] = InstanceUIValue;
        }

        subRoot["InstanceUIValues"] = InstanceUIValues;
    }

    Json::Value Children;

    unordered_map<string, weak_ptr<CToolUIBase>> pUIChildren = pFoundUI->GetUIChildren();
    _uint iNumChildren = static_cast<_uint>(pUIChildren.size());

    Children["NumChildren"] = iNumChildren;

    if (iNumChildren <= 0)
    {
        subRoot["Children"] = Children;
        if (_strSaveFile == "")
            root[_strUITag] = subRoot;
        else
            root[_strSaveFile] = subRoot;

        return;
    }

    Json::Value ChildrenTag(Json::arrayValue);

    for (auto& pPair : pUIChildren)
    {
        string strChildTag = pPair.second.lock()->GetUITag();
        ChildrenTag.append(strChildTag);
    }

    Children["ChildrenTag"] = ChildrenTag;

    subRoot["Children"] = Children;
    if (_strSaveFile == "")
        root[_strUITag] = subRoot;
    else
        root[_strSaveFile] = subRoot;

    for (auto& pPair : pUIChildren)
        SaveUIJSON(root, pPair.second.lock()->GetUITag());
}

void Tool_UI::CLevelDev::SaveUIInstanceJSON(Json::Value& root, const string& _strUITag, const string& _strSaveFile)
{
    shared_ptr<CToolUIBase> pFoundUI = m_pToolUIManager->FindUI(_strUITag);
    if (pFoundUI == nullptr)
        return;

    vector<_float3> instanceUIPos;

    unordered_map<string, weak_ptr<CToolUIBase>> uiChildren = pFoundUI->GetChildrenList();

    shared_ptr<CToolUIBase> pFoundChild{ nullptr };
    for (auto& itPair : uiChildren)
    {
        if (!itPair.second.expired())
            pFoundChild = itPair.second.lock();

        break;
    }

    Json::Value subRoot;

    CVIInstancing::InstanceDesc saveInstanceDesc = pFoundChild->GetInstanceDesc();
    CToolUIBase::ToolUIDesc saveUIDesc = pFoundChild->GetToolUIDesc();
    _uint iPassIndex = pFoundChild->GetShaderPass();
    vector<string> strTexKeys = pFoundChild->GetTexKeys();
    string strMaskKey;
    string strNoiseKey;
    if (saveUIDesc.m_isMasked)
        strMaskKey = pFoundChild->GetMaskKey();
    if (saveUIDesc.isNoised)
        strNoiseKey = pFoundChild->GetNoiseKey();

    Json::Value UIDesc;
    UIDesc["UITag"] = saveUIDesc.m_strTag;
    UIDesc["UILayer"] = saveUIDesc.m_eUILayer;
    UIDesc["UIType"] = saveUIDesc.m_eUIType;
    UIDesc["UISlotType"] = saveUIDesc.m_eUISlotType;
    UIDesc["UIIsMasked"] = saveUIDesc.m_isMasked;
    UIDesc["UIIsNoised"] = saveUIDesc.isNoised;
    UIDesc["UIPassIndex"] = iPassIndex;
    UIDesc["UIZOrder"] = saveUIDesc.fZOrder;
    UIDesc["UIDiscardAlpha"] = saveUIDesc.fDiscardAlpha;
    if (saveUIDesc.m_eUISlotType == TOOL_SLOT_INVEN)
    {
        shared_ptr<CToolUISlot> uiSlot = dynamic_pointer_cast<CToolUISlot>(pFoundUI);
        if (uiSlot != nullptr)
            UIDesc["UIItemType"] = uiSlot->GetItemType();
    }
    if (saveUIDesc.m_eUIType == TOOL_UI_SCROLLBG)
    {
        shared_ptr<CToolUIBG> uiScrollBG = dynamic_pointer_cast<CToolUIBG>(pFoundUI);
        if (uiScrollBG != nullptr)
            UIDesc["UIVisibleSizeY"] = uiScrollBG->GetOriginVisibleSizeY();
    }
    subRoot["UIDesc"] = UIDesc;

    Json::Value InstanceDesc;

    Json::Value vCenter(Json::arrayValue);
    vCenter.append(saveInstanceDesc.vCenter.x);
    vCenter.append(saveInstanceDesc.vCenter.y);
    vCenter.append(saveInstanceDesc.vCenter.z);
    InstanceDesc["Center"] = vCenter;

    Json::Value vSize(Json::arrayValue);
    vSize.append(saveInstanceDesc.vSize.x);
    vSize.append(saveInstanceDesc.vSize.y);
    InstanceDesc["Size"] = vSize;

    Json::Value vSpeed(Json::arrayValue);
    vSpeed.append(saveInstanceDesc.vSpeed.x);
    vSpeed.append(saveInstanceDesc.vSpeed.y);
    InstanceDesc["Speed"] = vSpeed;

    Json::Value vLTime(Json::arrayValue);
    vLTime.append(saveInstanceDesc.vLifeTime.x);
    vLTime.append(saveInstanceDesc.vLifeTime.y);
    InstanceDesc["LifeTime"] = vLTime;

    Json::Value vDescColor(Json::arrayValue);
    vDescColor.append(saveInstanceDesc.vColor.x);
    vDescColor.append(saveInstanceDesc.vColor.y);
    vDescColor.append(saveInstanceDesc.vColor.z);
    vDescColor.append(saveInstanceDesc.vColor.w);
    InstanceDesc["Color"] = vDescColor;

    InstanceDesc["SizeY"] = saveInstanceDesc.fSizeY;
    InstanceDesc["Duration"] = saveInstanceDesc.fDuration;
    InstanceDesc["IsLoop"] = saveInstanceDesc.isLoop;

    subRoot["InstanceDesc"] = InstanceDesc;

    Json::Value Textures;

    Json::Value Diffuse(Json::arrayValue);
    for (auto& texKey : strTexKeys)
    {
        Diffuse.append(texKey);
    }
    Textures["Diffuse"] = Diffuse;

    if (saveUIDesc.m_isMasked == true)
        Textures["Mask"] = strMaskKey;

    if (saveUIDesc.isNoised == true)
        Textures["Noise"] = strNoiseKey;

    subRoot["Textures"] = Textures;

    if (saveUIDesc.m_eUIType == TOOL_UI_UV)
    {
        Json::Value AtlasUVs(Json::arrayValue);

        vector<_float4> atlasUVs = pFoundUI->GetAtlasUVs();

        for (auto& atlasUVs : atlasUVs)
        {
            Json::Value AtlasUV(Json::arrayValue);
            AtlasUV.append(atlasUVs.x);
            AtlasUV.append(atlasUVs.y);
            AtlasUV.append(atlasUVs.z);
            AtlasUV.append(atlasUVs.w);

            AtlasUVs.append(AtlasUV);
        }

        subRoot["AtlasUVs"] = AtlasUVs;
    }

    Json::Value SRTs;

    for (auto& pChildPair : uiChildren)
    {
        Json::Value SRT; // Scale Translation

        _float2 uiPos = pChildPair.second.lock()->GetUIPos();

        Json::Value Pos(Json::arrayValue);
        Pos.append(uiPos.x);
        Pos.append(uiPos.y);

        _float2 uiSize = pChildPair.second.lock()->GetUISize();

        Json::Value Scale(Json::arrayValue);
        Scale.append(uiSize.x);
        Scale.append(uiSize.y);

        SRT["Pos"] = Pos;
        SRT["Scale"] = Scale;

        string childUITag = pChildPair.second.lock()->GetUITag();

        stringstream ss;

        for (char c : childUITag)
        {
            if (std::isdigit(c))
                ss << c;
        }

        int foundNum{ 0 };
        ss >> foundNum;

        string strKey = "SRT" + to_string(foundNum);

        SRTs[strKey] = SRT;
    }

    subRoot["SRTs"] = SRTs;

    root[_strSaveFile] = subRoot;
}

void Tool_UI::CLevelDev::SaveUIInstanceNormal(Json::Value& root, const string& _strUITag, const string& _strSaveFile)
{
    shared_ptr<CToolUIBase> pFoundUI = m_pToolUIManager->FindUI(_strUITag);
    if (pFoundUI == nullptr)
        return;

    vector<_float3> instanceUIPos;

    unordered_map<string, weak_ptr<CToolUIBase>> uiChildren = pFoundUI->GetChildrenList();

    shared_ptr<CToolUIBase> pFoundChild{ nullptr };
    for (auto& itPair : uiChildren)
    {
        if (!itPair.second.expired())
            pFoundChild = itPair.second.lock();

        break;
    }

    Json::Value subRoot;

    CVIInstancing::InstanceDesc saveInstanceDesc = pFoundChild->GetInstanceDesc();
    CToolUIBase::ToolUIDesc saveUIDesc = pFoundChild->GetToolUIDesc();
    _uint iPassIndex = pFoundChild->GetShaderPass();
    vector<string> strTexKeys = pFoundChild->GetTexKeys();
    string strMaskKey;
    string strNoiseKey;
    if (saveUIDesc.m_isMasked)
        strMaskKey = pFoundChild->GetMaskKey();
    if (saveUIDesc.isNoised)
        strNoiseKey = pFoundChild->GetNoiseKey();

    Json::Value UIDesc;
    UIDesc["UITag"] = saveUIDesc.m_strTag;
    UIDesc["UILayer"] = saveUIDesc.m_eUILayer;
    UIDesc["UIType"] = saveUIDesc.m_eUIType;
    UIDesc["UISlotType"] = saveUIDesc.m_eUISlotType;
    UIDesc["UIIsMasked"] = saveUIDesc.m_isMasked;
    UIDesc["UIIsNoised"] = saveUIDesc.isNoised;
    UIDesc["UIPassIndex"] = iPassIndex;
    UIDesc["UIZOrder"] = saveUIDesc.fZOrder;
    UIDesc["UIDiscardAlpha"] = saveUIDesc.fDiscardAlpha;
    if (saveUIDesc.m_eUISlotType == TOOL_SLOT_INVEN)
    {
        shared_ptr<CToolUISlot> uiSlot = dynamic_pointer_cast<CToolUISlot>(pFoundUI);
        if (uiSlot != nullptr)
            UIDesc["UIItemType"] = uiSlot->GetItemType();
    }
    if (saveUIDesc.m_eUIType == TOOL_UI_SCROLLBG)
    {
        shared_ptr<CToolUIBG> uiScrollBG = dynamic_pointer_cast<CToolUIBG>(pFoundUI);
        if (uiScrollBG != nullptr)
            UIDesc["UIVisibleSizeY"] = uiScrollBG->GetOriginVisibleSizeY();
    }
    subRoot["UIDesc"] = UIDesc;

    Json::Value InstanceDesc;

    Json::Value vCenter(Json::arrayValue);
    vCenter.append(saveInstanceDesc.vCenter.x);
    vCenter.append(saveInstanceDesc.vCenter.y);
    vCenter.append(saveInstanceDesc.vCenter.z);
    InstanceDesc["Center"] = vCenter;

    Json::Value vSize(Json::arrayValue);
    vSize.append(saveInstanceDesc.vSize.x);
    vSize.append(saveInstanceDesc.vSize.y);
    InstanceDesc["Size"] = vSize;

    Json::Value vSpeed(Json::arrayValue);
    vSpeed.append(saveInstanceDesc.vSpeed.x);
    vSpeed.append(saveInstanceDesc.vSpeed.y);
    InstanceDesc["Speed"] = vSpeed;

    Json::Value vLTime(Json::arrayValue);
    vLTime.append(saveInstanceDesc.vLifeTime.x);
    vLTime.append(saveInstanceDesc.vLifeTime.y);
    InstanceDesc["LifeTime"] = vLTime;

    Json::Value vDescColor(Json::arrayValue);
    vDescColor.append(saveInstanceDesc.vColor.x);
    vDescColor.append(saveInstanceDesc.vColor.y);
    vDescColor.append(saveInstanceDesc.vColor.z);
    vDescColor.append(saveInstanceDesc.vColor.w);
    InstanceDesc["Color"] = vDescColor;

    InstanceDesc["SizeY"] = saveInstanceDesc.fSizeY;
    InstanceDesc["Duration"] = saveInstanceDesc.fDuration;
    InstanceDesc["IsLoop"] = saveInstanceDesc.isLoop;

    subRoot["InstanceDesc"] = InstanceDesc;

    Json::Value Textures;

    Json::Value Diffuse(Json::arrayValue);
    for (auto& texKey : strTexKeys)
    {
        Diffuse.append(texKey);
    }
    Textures["Diffuse"] = Diffuse;

    if (saveUIDesc.m_isMasked == true)
        Textures["Mask"] = strMaskKey;

    if (saveUIDesc.isNoised == true)
        Textures["Noise"] = strNoiseKey;

    subRoot["Textures"] = Textures;

    if (saveUIDesc.m_eUIType == TOOL_UI_UV)
    {
        Json::Value AtlasUVs(Json::arrayValue);

        vector<_float4> atlasUVs = pFoundUI->GetAtlasUVs();

        for (auto& atlasUVs : atlasUVs)
        {
            Json::Value AtlasUV(Json::arrayValue);
            AtlasUV.append(atlasUVs.x);
            AtlasUV.append(atlasUVs.y);
            AtlasUV.append(atlasUVs.z);
            AtlasUV.append(atlasUVs.w);

            AtlasUVs.append(AtlasUV);
        }

        subRoot["AtlasUVs"] = AtlasUVs;
    }

    Json::Value SRTs;

    _uint iCount{ 0 };

    for (auto& pChildPair : uiChildren)
    {
        Json::Value SRT; // Scale Translation

        _float2 uiPos = pChildPair.second.lock()->GetUIPos();

        Json::Value Pos(Json::arrayValue);
        Pos.append(uiPos.x);
        Pos.append(uiPos.y);

        _float2 uiSize = pChildPair.second.lock()->GetUISize();

        Json::Value Scale(Json::arrayValue);
        Scale.append(uiSize.x);
        Scale.append(uiSize.y);

        SRT["Pos"] = Pos;
        SRT["Scale"] = Scale;

        string strKey = "SRT" + to_string(iCount);

        SRTs[strKey] = SRT;

        iCount++;
    }

    subRoot["SRTs"] = SRTs;

    root[_strSaveFile] = subRoot;
}

void Tool_UI::CLevelDev::LoadUIJSON(Json::Value& root, const string& _strUITag)
{
    Json::Value subRoot = root[_strUITag];

    CToolUIBase::ToolUIDesc toolUIDesc{};
    ETOOL_ITEM_TYPE eItemType{ TOOL_ITEM_END };
    _float fVisibleSizeY{ 0.f };
    CVIInstancing::InstanceDesc instanceDesc{};
    _uint iNumTexKeys{ 0 };
    vector<string> strKeys;
    string strMaskKey;
    string strNoiseKey;
    _float2 uiPos{};
    _float2 uiSize{};
    vector<_float4> atlasUVs;
    string strUIParentTag;
    vector<string> strUIChildTags;
    vector<_float2> instanceUIPos;
    vector<_float2> instanceUISize;

    /* UIDesc */
    Json::Value UIDesc = subRoot["UIDesc"];

    toolUIDesc.m_strTag = UIDesc["UITag"].asString();
    toolUIDesc.m_eUILayer = static_cast<ETOOL_UI_LAYER>(UIDesc["UILayer"].asUInt());
    toolUIDesc.m_eUIType = static_cast<ETOOL_UI_TYPE>(UIDesc["UIType"].asUInt());
    toolUIDesc.m_eUITexType = static_cast<ETOOL_UI_TEX_TYPE>(UIDesc["UITexType"].asUInt());
    toolUIDesc.m_eUISlotType = static_cast<ETOOL_SLOT_TYPE>(UIDesc["UISlotType"].asUInt());
    toolUIDesc.m_isMasked = UIDesc["UIIsMasked"].asBool();
    toolUIDesc.isNoised = UIDesc["UIIsNoised"].asBool();
    toolUIDesc.iShaderPassIdx = UIDesc["UIPassIndex"].asUInt();
    toolUIDesc.fZOrder = UIDesc["UIZOrder"].asFloat();
    toolUIDesc.fDiscardAlpha = UIDesc["UIDiscardAlpha"].asFloat();

    if (toolUIDesc.m_eUISlotType == TOOL_SLOT_INVEN || toolUIDesc.m_eUIType == TOOL_UI_INSTANCEITEMSLOT)
        eItemType = static_cast<ETOOL_ITEM_TYPE>(UIDesc["UIItemType"].asUInt());

    if (toolUIDesc.m_eUIType == TOOL_UI_SCROLLBG)
        fVisibleSizeY = UIDesc["UIVisibleSizeY"].asFloat();

    /* Instance Desc */
    Json::Value InstanceDesc = subRoot["InstanceDesc"];
    {
        const Json::Value Pivot = InstanceDesc["Pivot"];
        if (Pivot.isArray())
            instanceDesc.vPivot = _float3(Pivot[0].asFloat(), Pivot[1].asFloat(), Pivot[2].asFloat());

        const Json::Value Center = InstanceDesc["Center"];
        if (Center.isArray())
            instanceDesc.vCenter = _float3(Center[0].asFloat(), Center[1].asFloat(), Center[2].asFloat());

        const Json::Value Range = InstanceDesc["Range"];
        if (Range.isArray())
            instanceDesc.vRange = _float3(Range[0].asFloat(), Range[1].asFloat(), Range[2].asFloat());

        const Json::Value Size = InstanceDesc["Size"];
        if (Size.isArray())
            instanceDesc.vSize = _float2(Size[0].asFloat(), Size[1].asFloat());

        const Json::Value Speed = InstanceDesc["Speed"];
        if (Speed.isArray())
            instanceDesc.vSpeed = _float2(Speed[0].asFloat(), Speed[1].asFloat());

        const Json::Value Time = InstanceDesc["LifeTime"];
        if (Time.isArray())
            instanceDesc.vLifeTime = _float2(Time[0].asFloat(), Time[1].asFloat());

        const Json::Value Color = InstanceDesc["Color"];
        if (Color.isArray())
            instanceDesc.vColor = _float4(Color[0].asFloat(), Color[1].asFloat(), Color[2].asFloat(), Color[3].asFloat());

        instanceDesc.fSizeY = InstanceDesc["SizeY"].asFloat();
        instanceDesc.fDuration = InstanceDesc["Duration"].asFloat();
        instanceDesc.isLoop = InstanceDesc["IsLoop"].asBool();
    }

    /* Textures */
    Json::Value Textures = subRoot["Textures"];
    {
        Json::Value Diffuse = Textures["Diffuse"];
        if (Diffuse.isArray())
        {
            for (const auto& strKey : Diffuse)
                strKeys.emplace_back(strKey.asString());
        }

        if (toolUIDesc.m_isMasked == true)
            strMaskKey = Textures["Mask"].asString();

        if (toolUIDesc.isNoised == true)
            strNoiseKey = Textures["Noise"].asString();
    }

    /* AtalsUVs */
    if (toolUIDesc.m_eUIType == TOOL_UI_UV)
    {
        Json::Value AtlasUVs = subRoot["AtlasUVs"];
        if (AtlasUVs.isArray())
        {
            for (auto& atlasValue : AtlasUVs)
            {
                if (atlasValue.isArray())
                {
                    _float4 UVs = _float4(atlasValue[0].asFloat(), atlasValue[1].asFloat(), atlasValue[2].asFloat(), atlasValue[3].asFloat());
                    atlasUVs.emplace_back(UVs);
                }
            }
        }
    }

    /* SRT */
    Json::Value SRT = subRoot["SRT"];
    {
        Json::Value Pos = SRT["Pos"];
        if (Pos.isArray())
            uiPos = _float2(Pos[0].asFloat(), Pos[1].asFloat());

        Json::Value Scale = SRT["Scale"];
        if (Scale.isArray())
            uiSize = _float2(Scale[0].asFloat(), Scale[1].asFloat());
    }

    /* InstanceUIValues */
    if (toolUIDesc.m_eUIType == TOOL_UI_INSTANCEITEMBG || toolUIDesc.m_eUIType == TOOL_UI_INSTANCEITEMSLOT ||
        toolUIDesc.m_eUIType == TOOL_UI_INSTANCEITEMICON || toolUIDesc.m_eUIType == TOOL_UI_INSTANCEITEMCOOLTIME ||
        toolUIDesc.m_eUIType == TOOL_UI_INSTANCEITEMHOVER || toolUIDesc.m_eUIType == TOOL_UI_INSTANCEQUICKBG ||
        toolUIDesc.m_eUIType == TOOL_UI_INSTANCEQUICKMAINSLOT || toolUIDesc.m_eUIType == TOOL_UI_INSTANCEQUICKITEMSLOT ||
        toolUIDesc.m_eUIType == TOOL_UI_INSTANCEQUICKCOMBATSLOT || toolUIDesc.m_eUIType == TOOL_UI_INSTANCEQUICKFRAME ||
        toolUIDesc.m_eUIType == TOOL_UI_INSTANCEPARRING || toolUIDesc.m_eUIType == TOOL_UI_INSTANCEABNORMALSLOT ||
        toolUIDesc.m_eUIType == TOOL_UI_INSTANCEWORLDHP || toolUIDesc.m_eUIType == TOOL_UI_INSTANCEBOSSHP ||
        toolUIDesc.m_eUIType == TOOL_UI_INSTANCEABNORMALSLOT || toolUIDesc.m_eUIType == TOOL_UI_INSTANCEQUEST ||
        toolUIDesc.m_eUIType == TOOL_UI_INSTANCEQUESTINDICATOR || toolUIDesc.m_eUIType == TOOL_UI_INSTANCEINTERACTION ||
        toolUIDesc.m_eUIType == TOOL_UI_INSTANCEDIALOGUE || toolUIDesc.m_eUIType == TOOL_UI_CRAFTBG ||
        toolUIDesc.m_eUIType == TOOL_UI_CRAFTHOVER || toolUIDesc.m_eUIType == TOOL_UI_CRAFTTAB ||
        toolUIDesc.m_eUIType == TOOL_UI_CRAFTSLOT || toolUIDesc.m_eUIType == TOOL_UI_TRADEBG || toolUIDesc.m_eUIType == TOOL_UI_TRADESLOT ||
        toolUIDesc.m_eUIType == TOOL_UI_TRADETAB || toolUIDesc.m_eUIType == TOOL_UI_TRADEHOVER ||
        toolUIDesc.m_eUIType == TOOL_UI_QUESTCOMPLETE || toolUIDesc.m_eUIType == TOOL_UI_ITEMTOOLTIP ||
        toolUIDesc.m_eUIType == TOOL_UI_SPEECHBALLON || toolUIDesc.m_eUIType == TOOL_UI_STARFORCE ||
        toolUIDesc.m_eUIType == TOOL_UI_OPTIONBG || toolUIDesc.m_eUIType == TOOL_UI_OPTIONTAB ||
        toolUIDesc.m_eUIType == TOOL_UI_OPTIONHOVER || toolUIDesc.m_eUIType == TOOL_UI_OPTIONSLIDER ||
        toolUIDesc.m_eUIType == TOOL_UI_OPTIONCHECKBOX || toolUIDesc.m_eUIType == TOOL_UI_CRAFTRESULT ||
        toolUIDesc.m_eUIType == TOOL_UI_TUTORIAL)
    {
        Json::Value InstanceUIValues = subRoot["InstanceUIValues"];

        _uint iNumValues{ 0 };
        for (Json::ValueIterator it = InstanceUIValues.begin(); it != InstanceUIValues.end(); ++it)
        {
            string strKey = "InstanceValue" + to_string(iNumValues);

            Json::Value InstanceValue = InstanceUIValues[strKey];
            {
                _float2 instanceValuePos{};
                _float2 instanceValueSize{};

                Json::Value InstancePos = InstanceValue["InstancePos"];
                if (InstancePos.isArray())
                    instanceValuePos = _float2(InstancePos[0].asFloat(), InstancePos[1].asFloat());

                Json::Value InstanceSize = InstanceValue["InstanceSize"];
                if (InstanceSize.isArray())
                    instanceValueSize = _float2(InstanceSize[0].asFloat(), InstanceSize[1].asFloat());

                instanceUIPos.emplace_back(instanceValuePos);
                instanceUISize.emplace_back(instanceValueSize);

                iNumValues++;
            }
        }
    }

    /* Create UI */
    switch (toolUIDesc.m_eUIType)
    {
    case TOOL_UI_HUD:
    {

    }
    break;
    case TOOL_UI_BAR:
    {
        shared_ptr<CToolUIBar> pUI = CToolUIBar::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_ICON:
    {
        shared_ptr<CToolUIIcon> pUI = CToolUIIcon::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_SLIDER:
    {
        shared_ptr<CToolUISlider> pUI = CToolUISlider::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_BUTTON:
    {
        shared_ptr<CToolUIButton> pUI = CToolUIButton::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_SCROLLBG:
    {
        shared_ptr<CToolUIBG> pUI = CToolUIBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, fVisibleSizeY);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        //pUI->SetUIPos(uiPos.x, uiPos.y);
        pUI->SetBGInitPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_SLOT:
    {
        shared_ptr<CToolUISlot> pUI = CToolUISlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
        pUI->SetItemType(eItemType);
    }
    break;
    case TOOL_UI_SCROLL:
    {
        shared_ptr<CToolUIScroll> pUI = CToolUIScroll::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_COOLTIME:
    {
        shared_ptr<CToolUICoolTime> pUI = CToolUICoolTime::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_UV:
    {

    }
    break;
    case TOOL_UI_PANEL:
    {
        shared_ptr<CToolUIPanel> pUI = CToolUIPanel::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_BACKGROUND:
    {
        shared_ptr<CToolUIBackGround> pUI = CToolUIBackGround::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_BACKGROUND_R:
    {
        shared_ptr<CToolUIBackGround> pUI = CToolUIBackGround::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_HOVER:
    {
        shared_ptr<CToolUIHover> pUI = CToolUIHover::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_COVER:
    {
        shared_ptr<CToolUICover> pUI = CToolUICover::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_ITEMBG:
    {
        shared_ptr<CToolUIItemBackGround> pUI = CToolUIItemBackGround::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_ITEMICON:
    {
        shared_ptr<CToolUIItemIcon> pUI = CToolUIItemIcon::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_ITEMHOVER:
    {
        shared_ptr<CToolUIItemHover> pUI = CToolUIItemHover::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_ITEMTAB:
    {
        shared_ptr<CToolUIItemTab> pUI = CToolUIItemTab::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_COVER_RV:
    {
        shared_ptr<CToolUICover> pUI = CToolUICover::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_BACKGROUND_NT:
    {
        shared_ptr<CToolUIBackGround> pUI = CToolUIBackGround::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_LOCKON:
    {
        shared_ptr<CToolUILockOn> pUI = CToolUILockOn::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_HL:
    {
        shared_ptr<CToolUIBackGround> pUI = CToolUIBackGround::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_HR:
    {
        shared_ptr<CToolUIBackGround> pUI = CToolUIBackGround::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEITEMBG:
    {
        shared_ptr<CToolUIInstanceItemBG> pUI = CToolUIInstanceItemBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEITEMSLOT:
    {
        shared_ptr<CToolUIInstanceItemSlot> pUI = CToolUIInstanceItemSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEITEMICON:
    {
        shared_ptr<CToolUIInstanceItemIcon> pUI = CToolUIInstanceItemIcon::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEITEMCOOLTIME:
    {
        shared_ptr<CToolUIInstanceItemCoolTime> pUI = CToolUIInstanceItemCoolTime::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEITEMHOVER:
    {
        shared_ptr<CToolUIInstanceItemHover> pUI = CToolUIInstanceItemHover::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEQUICKBG:
    {
        shared_ptr<CToolUIInstanceQuickBG> pUI = CToolUIInstanceQuickBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEQUICKMAINSLOT:
    {
        shared_ptr<CToolUIInstanceQuickSlot> pUI = CToolUIInstanceQuickSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEQUICKITEMSLOT:
    {
        shared_ptr<CToolUIInstanceQuickSlot> pUI = CToolUIInstanceQuickSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEQUICKCOMBATSLOT:
    {
        shared_ptr<CToolUIInstanceQuickSlot> pUI = CToolUIInstanceQuickSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEQUICKFRAME:
    {
        shared_ptr<CToolUIInstanceQuickBG> pUI = CToolUIInstanceQuickBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEPARRING:
    {
        shared_ptr<CToolUIInstanceParring> pUI = CToolUIInstanceParring::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEABNORMALSLOT:
    {
        shared_ptr<CToolUIInstanceAbnormalSlot> pUI = CToolUIInstanceAbnormalSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEWORLDHP:
    {
        shared_ptr<CToolUIInstanceWorldHP> pUI = CToolUIInstanceWorldHP::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEBOSSHP:
    {
        shared_ptr<CToolUIInstanceBossHP> pUI = CToolUIInstanceBossHP::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_NORMALTEXT:
    {
        weak_ptr<CCustomFont> pFont = GAMEINSTANCE->FindFont("Font_Nanum");
        if (!pFont.expired())
        {
            char cScript[MAX_PATH] = "TestWords";
            wchar_t wideString[MAX_PATH] = L"";

            MultiByteToWideChar(CP_UTF8, 0, cScript, MAX_PATH, wideString, MAX_PATH);

            wstring wString = wideString;
            shared_ptr<CToolUINormalText> pUI = CToolUINormalText::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, pFont.lock(), wString);
            
            if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
                return;

            pUI->SetUIPos(uiPos.x, uiPos.y);
            m_strUIList.emplace_back(pUI->GetUITag());
            m_strLoadUIList.emplace_back(pUI->GetUITag());
            m_pToolUIManager->ActivateUI(pUI->GetUITag());
        }
    }
    break;
    case TOOL_UI_INSTANCEBOSSABNORMAL:
    {
        shared_ptr<CToolUIInstanceBossAbnormal> pUI = CToolUIInstanceBossAbnormal::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEQUEST:
    {
        weak_ptr<CCustomFont> pFont = GAMEINSTANCE->FindFont("Font_Nanum");
        if (!pFont.expired())
        {
            shared_ptr<CToolUIInstanceQuest> pUI = CToolUIInstanceQuest::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize, pFont.lock());
            if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
                return;

            pUI->SetUIPos(uiPos.x, uiPos.y);
            m_strUIList.emplace_back(pUI->GetUITag());
            m_strLoadUIList.emplace_back(pUI->GetUITag());
            m_pToolUIManager->ActivateUI(pUI->GetUITag());
        }
    }
    break;
    case TOOL_UI_INSTANCEQUESTINDICATOR:
    {
        shared_ptr<CToolUIInstanceQuestIndicator> pUI = CToolUIInstanceQuestIndicator::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEINTERACTION:
    {
        shared_ptr<CToolUIInstanceInteraction> pUI = CToolUIInstanceInteraction::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEDIALOGUE:
    {
        shared_ptr<CToolUIInstanceDialogue> pUI = CToolUIInstanceDialogue::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_CRAFTBG:
    {
        shared_ptr<CToolUICraftBG> pUI = CToolUICraftBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_CRAFTHOVER:
    {
        shared_ptr<CToolUICraftHover> pUI = CToolUICraftHover::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_CRAFTTAB:
    {
        shared_ptr<CToolUIInstanceCraftTab> pUI = CToolUIInstanceCraftTab::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_CRAFTSLOT:
    {
        shared_ptr<CToolUICraftSlot> pUI = CToolUICraftSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_TRADEBG:
    {
        shared_ptr<CToolUITradeBG> pUI = CToolUITradeBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_TRADESLOT:
    {
        shared_ptr<CToolUITradeSlot> pUI = CToolUITradeSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_TRADETAB:
    {
        shared_ptr<CToolUITradeTab> pUI = CToolUITradeTab::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_TRADEHOVER:
    {
        shared_ptr<CToolUITradeHover> pUI = CToolUITradeHover::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_QUESTCOMPLETE:
    {
        shared_ptr<CToolUIQuestComplete> pUI = CToolUIQuestComplete::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_ITEMTOOLTIP:
    {
        shared_ptr<CToolUIItemToolTip> pUI = CToolUIItemToolTip::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_SPEECHBALLON:
    {
        shared_ptr<CToolUISpeechBallon> pUI = CToolUISpeechBallon::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_STARFORCE:
    {
        shared_ptr<CToolUIStarForce> pUI = CToolUIStarForce::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_OPTIONBG:
    {
        shared_ptr<CToolUIOptionBG> pUI = CToolUIOptionBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_OPTIONTAB:
    {
        shared_ptr<CToolUIOptionTab> pUI = CToolUIOptionTab::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_OPTIONHOVER:
    {
        shared_ptr<CToolUIOptionHover> pUI = CToolUIOptionHover::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_OPTIONSLIDER:
    {
        shared_ptr<CToolUIOptionSlider> pUI = CToolUIOptionSlider::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_OPTIONCHECKBOX:
    {
        shared_ptr<CToolUIOptionCheckBox> pUI = CToolUIOptionCheckBox::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_CRAFTRESULT:
    {
        shared_ptr<CToolUICraftResult> pUI = CToolUICraftResult::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_TUTORIAL:
    {
        shared_ptr<CToolUITutorial> pUI = CToolUITutorial::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    }
    
    /* Children */
    Json::Value Children = subRoot["Children"];

    _uint iNumChildren = Children["NumChildren"].asUInt();
    if (iNumChildren <= 0)
    {
        m_uiChildrenTags.emplace(toolUIDesc.m_strTag, strUIChildTags);

        return;
    }

    Json::Value ChildrenTag = Children["ChildrenTag"];
    if (ChildrenTag.isArray())
    {
        for (const auto& strTag : ChildrenTag)
            strUIChildTags.emplace_back(strTag.asString());
    }

    m_uiChildrenTags.emplace(toolUIDesc.m_strTag, strUIChildTags);

    for (auto& childTag : strUIChildTags)
        LoadUIJSON(root, childTag);

    for (auto& loadUI : m_strLoadUIList)
    {
        auto it = m_uiChildrenTags.find(loadUI);
        if (it != m_uiChildrenTags.end())
        {
            vector<string> childrenTags = it->second;
            if (!childrenTags.empty())
            {
                for (auto& childTag : childrenTags)
                {
                    m_pToolUIManager->AddUIChild(it->first, childTag);
#pragma region Only For Tool
                    auto it = find_if(m_strUIList.begin(), m_strUIList.end(), [&childTag](string strUITag) {
                        if (strUITag == childTag)
                            return true;

                        return false;
                        });

                    if (it != m_strUIList.end())
                        m_strUIList.erase(it);
#pragma endregion
                }
            }
        }
    }
}

void Tool_UI::CLevelDev::LoadUIInstanceJSON(Json::Value& root, const string& _strUITag)
{
    Json::Value subRoot = root[_strUITag];

    //CToolUIInstance::UIInstDesc toolUIDesc{};
    CToolUIBase::ToolUIDesc toolUIDesc{};
    ETOOL_ITEM_TYPE eItemType{ TOOL_ITEM_END };
    _float fVisibleSizeY{ 0.f };
    //InstanceUI instanceDesc{};
    CVIInstancing::InstanceDesc instanceDesc{};
    _uint iNumTexKeys;
    vector<string> strKeys;
    string strMaskKey;
    string strNoiseKey;
    _float2 uiPos{};
    _float2 uiSize{};
    vector<_float4> atlasUVs;
    string strUIParentTag;
    
    /* UIDesc */
    Json::Value UIDesc = subRoot["UIDesc"];

    toolUIDesc.m_strTag = UIDesc["UITag"].asString();
    toolUIDesc.m_eUILayer = static_cast<ETOOL_UI_LAYER>(UIDesc["UILayer"].asUInt());
    toolUIDesc.m_eUIType = static_cast<ETOOL_UI_TYPE>(UIDesc["UIType"].asUInt());
    toolUIDesc.m_eUITexType = TOOL_UI_TEX_END;
    toolUIDesc.m_eUISlotType = static_cast<ETOOL_SLOT_TYPE>(UIDesc["UISlotType"].asUInt());
    toolUIDesc.m_isMasked = UIDesc["UIIsMasked"].asBool();
    toolUIDesc.isNoised = UIDesc["UIIsNoised"].asBool();
    toolUIDesc.iShaderPassIdx = UIDesc["UIPassIndex"].asUInt();
    toolUIDesc.fZOrder = UIDesc["UIZOrder"].asFloat();
    toolUIDesc.fDiscardAlpha = UIDesc["UIDiscardAlpha"].asFloat();

    if (toolUIDesc.m_eUISlotType == TOOL_SLOT_INVEN)
        eItemType = static_cast<ETOOL_ITEM_TYPE>(UIDesc["UIItemType"].asUInt());

    if (toolUIDesc.m_eUIType == TOOL_UI_SCROLLBG)
        fVisibleSizeY = UIDesc["UIVisibleSizeY"].asFloat();

    /* Instance Desc */
    Json::Value InstanceDesc = subRoot["InstanceDesc"];
    {
        const Json::Value Center = InstanceDesc["Center"];
        if (Center.isArray())
            instanceDesc.vCenter = _float3(Center[0].asFloat(), Center[1].asFloat(), Center[2].asFloat());

        const Json::Value Speed = InstanceDesc["Speed"];
        if (Speed.isArray())
            instanceDesc.vSpeed = _float2(Speed[0].asFloat(), Speed[1].asFloat());

        const Json::Value Time = InstanceDesc["LifeTime"];
        if (Time.isArray())
            instanceDesc.vLifeTime = _float2(Time[0].asFloat(), Time[1].asFloat());

        const Json::Value Color = InstanceDesc["Color"];
        if (Color.isArray())
            instanceDesc.vColor = _float4(Color[0].asFloat(), Color[1].asFloat(), Color[2].asFloat(), Color[3].asFloat());

        const Json::Value Size = InstanceDesc["Size"];
        if (Size.isArray())
            instanceDesc.vSize = _float2(Size[0].asFloat(), Size[1].asFloat());

        instanceDesc.fSizeY = InstanceDesc["SizeY"].asFloat();
        instanceDesc.fDuration = InstanceDesc["Duration"].asFloat();
        instanceDesc.isLoop = InstanceDesc["IsLoop"].asBool();
    }

    /* Textures */
    Json::Value Textures = subRoot["Textures"];
    {
        Json::Value Diffuse = Textures["Diffuse"];
        if (Diffuse.isArray())
        {
            for (const auto& strKey : Diffuse)
                strKeys.emplace_back(strKey.asString());
        }

        if (toolUIDesc.m_isMasked == true)
            strMaskKey = Textures["Mask"].asString();

        if (toolUIDesc.isNoised == true)
            strNoiseKey = Textures["Noise"].asString();
    }

    /* AtalsUVs */
    if (toolUIDesc.m_eUIType == TOOL_UI_UV)
    {
        Json::Value AtlasUVs = subRoot["AtlasUVs"];
        if (AtlasUVs.isArray())
        {
            for (auto& atlasValue : AtlasUVs)
            {
                if (atlasValue.isArray())
                {
                    _float4 UVs = _float4(atlasValue[0].asFloat(), atlasValue[1].asFloat(), atlasValue[2].asFloat(), atlasValue[3].asFloat());
                    atlasUVs.emplace_back(UVs);
                }
            }
        }
    }

    vector<_float2> instanceUIPos;
    vector<_float2> instanceUISize;

    Json::Value SRTs = subRoot["SRTs"];

    _uint iNumSRTs{ 0 };

    for (Json::ValueIterator it = SRTs.begin(); it != SRTs.end(); ++it)
    {
        string strKey = "SRT" + to_string(iNumSRTs);

        Json::Value SRT = SRTs[strKey];
        {
            Json::Value Pos = SRT["Pos"];
            if (Pos.isArray())
                uiPos = _float2(Pos[0].asFloat(), Pos[1].asFloat());

            Json::Value Scale = SRT["Scale"];
            if (Scale.isArray())
                uiSize = _float2(Scale[0].asFloat(), Scale[1].asFloat());

            instanceUIPos.emplace_back(uiPos);
            instanceUISize.emplace_back(uiSize);

            iNumSRTs++;
        }
    }

    switch (toolUIDesc.m_eUIType)
    {
    case TOOL_UI_INSTANCEITEMBG:
    {
        shared_ptr<CToolUIInstanceItemBG> pUI = CToolUIInstanceItemBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEITEMSLOT:
    {
        shared_ptr<CToolUIInstanceItemSlot> pUI = CToolUIInstanceItemSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEITEMICON:
    {
        shared_ptr<CToolUIInstanceItemIcon> pUI = CToolUIInstanceItemIcon::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEITEMCOOLTIME:
    {
        shared_ptr<CToolUIInstanceItemCoolTime> pUI = CToolUIInstanceItemCoolTime::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEITEMHOVER:
    {
        shared_ptr<CToolUIInstanceItemHover> pUI = CToolUIInstanceItemHover::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEQUICKBG:
    {
        shared_ptr<CToolUIInstanceQuickBG> pUI = CToolUIInstanceQuickBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEQUICKMAINSLOT:
    {
        shared_ptr<CToolUIInstanceQuickSlot> pUI = CToolUIInstanceQuickSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEQUICKITEMSLOT:
    {
        shared_ptr<CToolUIInstanceQuickSlot> pUI = CToolUIInstanceQuickSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEQUICKCOMBATSLOT:
    {
        shared_ptr<CToolUIInstanceQuickSlot> pUI = CToolUIInstanceQuickSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEQUICKFRAME:
    {
        shared_ptr<CToolUIInstanceQuickBG> pUI = CToolUIInstanceQuickBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEPARRING:
    {
        shared_ptr<CToolUIInstanceParring> pUI = CToolUIInstanceParring::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEABNORMALSLOT:
    {
        shared_ptr<CToolUIInstanceAbnormalSlot> pUI = CToolUIInstanceAbnormalSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEWORLDHP:
    {
        shared_ptr<CToolUIInstanceWorldHP> pUI = CToolUIInstanceWorldHP::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEBOSSHP:
    {
        shared_ptr<CToolUIInstanceBossHP> pUI = CToolUIInstanceBossHP::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEBOSSABNORMAL:
    {
        shared_ptr<CToolUIInstanceBossAbnormal> pUI = CToolUIInstanceBossAbnormal::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (pUI == nullptr)
            return;

        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(0.f, 0.f);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEQUEST:
    {
        weak_ptr<CCustomFont> pFont = GAMEINSTANCE->FindFont("Font_Nanum");
        if (!pFont.expired())
        {
            shared_ptr<CToolUIInstanceQuest> pUI = CToolUIInstanceQuest::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize, pFont.lock());
            if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
                return;

            pUI->SetUIPos(uiPos.x, uiPos.y);
            m_strUIList.emplace_back(pUI->GetUITag());
            m_strLoadUIList.emplace_back(pUI->GetUITag());
            m_pToolUIManager->ActivateUI(pUI->GetUITag());
        }
    }
    break;
    case TOOL_UI_INSTANCEQUESTINDICATOR:
    {
        shared_ptr<CToolUIInstanceQuestIndicator> pUI = CToolUIInstanceQuestIndicator::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEINTERACTION:
    {
        shared_ptr<CToolUIInstanceInteraction> pUI = CToolUIInstanceInteraction::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_INSTANCEDIALOGUE:
    {
        shared_ptr<CToolUIInstanceDialogue> pUI = CToolUIInstanceDialogue::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_CRAFTBG:
    {
        shared_ptr<CToolUICraftBG> pUI = CToolUICraftBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_CRAFTHOVER:
    {
        shared_ptr<CToolUICraftHover> pUI = CToolUICraftHover::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_CRAFTTAB:
    {
        shared_ptr<CToolUIInstanceCraftTab> pUI = CToolUIInstanceCraftTab::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_CRAFTSLOT:
    {
        shared_ptr<CToolUICraftSlot> pUI = CToolUICraftSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_TRADEBG:
    {
        shared_ptr<CToolUITradeBG> pUI = CToolUITradeBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_TRADESLOT:
    {
        shared_ptr<CToolUITradeSlot> pUI = CToolUITradeSlot::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_TRADETAB:
    {
        shared_ptr<CToolUITradeTab> pUI = CToolUITradeTab::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_TRADEHOVER:
    {
        shared_ptr<CToolUITradeHover> pUI = CToolUITradeHover::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_QUESTCOMPLETE:
    {
        shared_ptr<CToolUIQuestComplete> pUI = CToolUIQuestComplete::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_ITEMTOOLTIP:
    {
        shared_ptr<CToolUIItemToolTip> pUI = CToolUIItemToolTip::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_SPEECHBALLON:
    {
        shared_ptr<CToolUISpeechBallon> pUI = CToolUISpeechBallon::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_STARFORCE:
    {
        shared_ptr<CToolUIStarForce> pUI = CToolUIStarForce::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_OPTIONBG:
    {
        shared_ptr<CToolUIOptionBG> pUI = CToolUIOptionBG::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_OPTIONTAB:
    {
        shared_ptr<CToolUIOptionTab> pUI = CToolUIOptionTab::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_OPTIONHOVER:
    {
        shared_ptr<CToolUIOptionHover> pUI = CToolUIOptionHover::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_OPTIONSLIDER:
    {
        shared_ptr<CToolUIOptionSlider> pUI = CToolUIOptionSlider::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_OPTIONCHECKBOX:
    {
        shared_ptr<CToolUIOptionCheckBox> pUI = CToolUIOptionCheckBox::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_CRAFTRESULT:
    {
        shared_ptr<CToolUICraftResult> pUI = CToolUICraftResult::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    case TOOL_UI_TUTORIAL:
    {
        shared_ptr<CToolUITutorial> pUI = CToolUITutorial::Create(strKeys, strMaskKey, strNoiseKey, toolUIDesc, instanceDesc, instanceUIPos, instanceUISize);
        if (FAILED(m_pToolUIManager->AddUI(pUI->GetUITag(), pUI)))
            return;

        pUI->SetUIPos(uiPos.x, uiPos.y);
        m_strUIList.emplace_back(pUI->GetUITag());
        m_strLoadUIList.emplace_back(pUI->GetUITag());
        m_pToolUIManager->ActivateUI(pUI->GetUITag());
    }
    break;
    }
}

HRESULT Tool_UI::CLevelDev::ReadyLayerBackGround(const wstring& _strLayerTag)
{

    return S_OK;
}

HRESULT Tool_UI::CLevelDev::ReadyLight()
{
    return S_OK;
}

HRESULT Tool_UI::CLevelDev::ReadyCamera(const wstring& _strLayerTag)
{
    /*shared_ptr<CGameObject> pCam;

    CToolFreeCamera::TOOLCAMERAFREE_DESC pCamFreeDesc = {};
    pCamFreeDesc.fMouseSensor = 0.05f;


    CToolFreeCamera::CAMERA_DESC pCamDesc = {};
    pCamDesc.vEye = _float4(0.0f, 30.f, -25.f, 1.f);
    pCamDesc.vAt = _float4(0.0f, 0.0f, 0.f, 1.f);
    pCamDesc.fFovy = XMConvertToRadians(60.0f);
    pCamDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    pCamDesc.fNear = 0.2f;
    pCamDesc.fFar = 1000.f;

    CTransform::TRANSFORM_DESC pTransDesc = {};
    pTransDesc.fSpeedPerSet = 20.f;
    pTransDesc.fRotationPerSet = XMConvertToRadians(90.0f);

    pCam = CToolFreeCamera::Create(true, &pCamFreeDesc, &pCamDesc, &pTransDesc);

    if (FAILED(CGameInstance::GetInstance()->AddObject(TOOL_LEVEL_STATIC, strLayerTag, pCam)))
        return E_FAIL;*/

    return S_OK;
}

HRESULT Tool_UI::CLevelDev::LoadTextureTags(const wstring& _strResourcePath)
{
    for (const fs::directory_entry& entry : fs::recursive_directory_iterator(_strResourcePath))
    {
        const fs::path& p = entry.path();

        if (fs::is_regular_file(p))
        {
            wstring absFilePath = fs::canonical(p);
            _tchar textureName[MAX_PATH] = TEXT("");
            _tchar textureExt[MAX_PATH] = TEXT("");
            _char cTextureName[MAX_PATH] = "";

            _wsplitpath_s(absFilePath.c_str(), nullptr, 0, nullptr, 0, textureName, MAX_PATH, textureExt, MAX_PATH);

            WideCharToMultiByte(CP_ACP, 0, textureName, static_cast<_uint>(lstrlenW(textureName)), cTextureName, MAX_PATH, nullptr, nullptr);

            string strTexTag;
            strTexTag.append(cTextureName);

            if (lstrcmp(textureExt, TEXT(".png")) == false)
            {
                m_texTags.emplace_back(strTexTag);
            }
        }
    }

    return S_OK;
}

shared_ptr<Tool_UI::CLevelDev> Tool_UI::CLevelDev::Create()
{
    shared_ptr<CLevelDev> pInstance = make_shared<CLevelDev>();

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CLevelDev");

    return pInstance;
}
