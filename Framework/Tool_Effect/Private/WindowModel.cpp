#include "WindowModel.h"
#include "GameInstance.h"

#include "Model.h"
#include "Animation.h"
#include "AnimModel.h"

#include "ImGuiMgr.h"

CWindowModel::CWindowModel()
{
}

HRESULT CWindowModel::Initialize()
{
    m_pDevice = GAMEINSTANCE->GetDeviceInfo();
    m_pContext = GAMEINSTANCE->GetDeviceContextInfo();

    wstring strStaticPath = TEXT("../../Client/Bin/Resources/Level_Static/Models/");
    LoadAllAnimModelName(strStaticPath);

    return S_OK;
}

void CWindowModel::Tick(_float _fTimeDelta)
{
    KeyInput();

    if (!m_bShowEditWindow)
        return;

    ImGui::Begin("Model_Loader");

    if (ImGui::CollapsingHeader("Model_List"))
    {

        ImGui::BeginTabBar("tabs");
        {
            if (ImGui::BeginTabItem("Player"))
            {
                if (ImGui::BeginListBox("Model_List"))
                {
                    static _int iCurrentModelIndex{ 0 };

                    for (_int i = 0; i < m_vecAnimModelName.size(); ++i)
                    {
                        const bool isSelected = (iCurrentModelIndex == i);
                        if (ImGui::Selectable(m_vecAnimModelName[i].c_str(), isSelected))
                            iCurrentModelIndex = i;

                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                            m_iCurrentModelIndex = i;
                        }
                    }

                    ImGui::EndListBox();
                }

                if (ImGui::Button("Load_Model"))
                {
                    m_pCurrentAnimModel = GAMEINSTANCE->GetModel(m_vecAnimModelName[m_iCurrentModelIndex]);

                    GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_AnimObject"));

                    m_pAnimModel = CAnimModel::Create(m_pCurrentAnimModel);
                    GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_AnimObject"), m_pAnimModel);

                    CImGuiMgr::GetInstance()->SetModelObject(m_pAnimModel, m_vecAnimModelName[m_iCurrentModelIndex], "");

                    if (nullptr != m_pCurrentAnimModel)
                    {
                        m_CurrentModelAnimations = m_pCurrentAnimModel->GetAnimations();

                        m_vecAnimationNames.clear();

                        for (auto pAnimation : m_CurrentModelAnimations)
                        {
                            const _char* pName = pAnimation->GetName();
                            m_vecAnimationNames.push_back(pName);
                        }

                        m_iCurrentAnimationIndex = 0;
                        m_pCurrentAnimModel->ChangeAnim(m_iCurrentAnimationIndex, 0.1f, true);
                        CImGuiMgr::GetInstance()->SetModelObject(m_pAnimModel, m_vecAnimModelName[m_iCurrentModelIndex], m_vecAnimationNames[m_iCurrentAnimationIndex]);

                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Monster"))
            {
                if (ImGui::Button("Load_Model"))
                {
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::Separator();

    }

    ImGui::End();

    //=====================================================

    ImGui::Begin("Animation_Player");
    if (-1 != m_iCurrentAnimationIndex)
    {
        if (ImGui::BeginListBox("Animation_List"))
        {
            for (_int i = 0; i < m_vecAnimationNames.size(); ++i)
            {
                std::string itemLabel = std::to_string(i) + " : " + m_vecAnimationNames[i];
                const bool isSelected = (m_iCurrentAnimationIndex == i);

                if (ImGui::Selectable(itemLabel.c_str(), isSelected))
                {
                    m_iCurrentAnimationIndex = i;

                    m_pCurrentAnimModel->ChangeAnim(m_iCurrentAnimationIndex, 0.1f, true);
                    CImGuiMgr::GetInstance()->SetModelObject(m_pAnimModel, m_vecAnimModelName[m_iCurrentModelIndex], m_vecAnimationNames[m_iCurrentAnimationIndex]);
                    m_CurrentModelAnimations[m_iCurrentAnimationIndex]->SetTrackPosition(0.f);
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndListBox();
        }

        ImGui::SameLine();
        if (ImGui::Checkbox("Animation Play", &m_isAnimationPlay))
            m_pCurrentAnimModel->SetIsPlayAnimation(m_isAnimationPlay);

        _double	AnimationDuration = m_CurrentModelAnimations[m_iCurrentAnimationIndex]->GetDuration();
        _double	StartTrackPosition = m_CurrentModelAnimations[m_iCurrentAnimationIndex]->GetTrackPosition();
        _float  fCurrentTime = (_float)StartTrackPosition;

        //"Playing"
        if (ImGui::SliderFloat(to_string(AnimationDuration).c_str(), &fCurrentTime, 0.f, AnimationDuration, "%.1f"))
        {
            m_CurrentModelAnimations[m_iCurrentAnimationIndex]->SetTrackPosition(fCurrentTime);
        }
    }

    else
    {
        const char* items[] = { " " };
        _float empty = 0.f;
        ImGui::ListBox("Animation_List", &m_iCurrentAnimationIndex, items, IM_ARRAYSIZE(items), 5);
        ImGui::SliderFloat("Playing", &empty, 0.f, 1.f, "%.1f");
    }

    ImGui::End();

    //=======================================================

    ImGui::Begin("Event_Impoter");

    //ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
    //ImGui::SeparatorText("Model Name :");
    //ImGui::Text("dear imgui says hello! (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);

    ImGui::BulletText("Model");
    ImGui::Text(m_vecAnimModelName[m_iCurrentModelIndex].c_str());

    ImGui::BulletText("Animation");
    if (m_iCurrentAnimationIndex != -1)
        ImGui::Text(m_vecAnimationNames[m_iCurrentAnimationIndex]);

    ImGui::BulletText("Track Position");
    if (m_iCurrentAnimationIndex != -1)
    {
        _double	StartTrackPosition = m_CurrentModelAnimations[m_iCurrentAnimationIndex]->GetTrackPosition();
        ImGui::Text(to_string(StartTrackPosition).c_str());
    }

    ImGui::Separator();

    ImGui::BeginTabBar("tabs");
    {

        if (ImGui::BeginTabItem("Effect"))
        {
            if (ImGui::BeginListBox(""))
            {
                ImGui::EndListBox();
            }

            if (ImGui::Button("Insert Effect Trigger"))
            {
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Collider"))
        {
            ImGui::SeparatorText("Collier Type");

            static int RadioIndex = 0;
            ImGui::RadioButton("SPHERE", &RadioIndex, 0); ImGui::SameLine();
            ImGui::RadioButton("OBB", &RadioIndex, 1); ImGui::SameLine();
            ImGui::RadioButton("AABB", &RadioIndex, 2);

            if (ImGui::Button("Insert Collider Trigger"))
            {
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Sound"))
        {
            if (ImGui::BeginListBox(""))
            {
                ImGui::EndListBox();
            }
            if (ImGui::Button("Insert Sound Trigger"))
            {
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void CWindowModel::KeyInput()
{
    if (GAMEINSTANCE->KeyDown(DIK_HOME))
        m_bShowEditWindow = !m_bShowEditWindow;
}

HRESULT CWindowModel::LoadAllAnimModelName(const wstring& _strModelPath)
{
    filesystem::path basePath(_strModelPath);

    for (const auto& entry : filesystem::directory_iterator(basePath))
    {
        if (entry.is_regular_file())
        {
            if (".dat" == entry.path().extension())
            {
                string strFileName = entry.path().filename().stem().string();

                if (GAMEINSTANCE->GetModel(strFileName)->GetModelType() == CModel::TYPE_ANIM)
                    m_vecAnimModelName.push_back(strFileName);
            }
        }
        else if (entry.is_directory())
        {
            LoadAllAnimModelName(entry.path());
        }
    }

    return S_OK;
}

shared_ptr<CAnimModel> CWindowModel::ChangeModelAndAnim(string _strModelKey, string _strAnimKey)
{
    // 모델이 필요없는 이펙트그룹이면, 그냥 Player_Base 띄우기
    if ("" == _strModelKey)
        _strModelKey = "Player_Base";

    if ("" == _strAnimKey)
        _strAnimKey = "Armature|P_Hu_F_CR_BA_D_BasicShot_PowerUp";

    m_pCurrentAnimModel = GAMEINSTANCE->GetModel(_strModelKey);

    GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_AnimObject"));

    m_pAnimModel = CAnimModel::Create(m_pCurrentAnimModel);
    GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_AnimObject"), m_pAnimModel);
    
    CImGuiMgr::GetInstance()->SetModelObject(m_pAnimModel, _strModelKey, "");

    if (nullptr != m_pCurrentAnimModel)
    {
        m_CurrentModelAnimations = m_pCurrentAnimModel->GetAnimations();

        m_vecAnimationNames.clear();

        for (auto pAnimation : m_CurrentModelAnimations)
        {
            const _char* pName = pAnimation->GetName();
            m_vecAnimationNames.push_back(pName);
        }

        m_iCurrentAnimationIndex = 0;
        m_pCurrentAnimModel->ChangeAnim(m_iCurrentAnimationIndex, 0.1f, true);
        CImGuiMgr::GetInstance()->SetModelObject(m_pAnimModel, _strModelKey, m_vecAnimationNames[m_iCurrentAnimationIndex]);
    }

    auto it = find(m_vecAnimationNames.begin(), m_vecAnimationNames.end(), _strAnimKey);
    if (it == m_vecAnimationNames.end())
        return nullptr;

    m_iCurrentAnimationIndex = it - m_vecAnimationNames.begin();

    m_pCurrentAnimModel->ChangeAnim(m_iCurrentAnimationIndex, 0.1f, true);
    CImGuiMgr::GetInstance()->SetModelObject(m_pAnimModel, m_vecAnimModelName[m_iCurrentModelIndex], m_vecAnimationNames[m_iCurrentAnimationIndex]);
    m_CurrentModelAnimations[m_iCurrentAnimationIndex]->SetTrackPosition(0.f);

    return m_pAnimModel;
}
