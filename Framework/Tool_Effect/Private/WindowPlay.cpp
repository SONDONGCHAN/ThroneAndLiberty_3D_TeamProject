#include "WindowPlay.h"
#include "GameInstance.h"

#include "TestModel.h"
#include "Effect.h"
#include "Texture.h"

#include "Easing.h"

#include <filesystem>
#include <iostream>

CWindowPlay::CWindowPlay()
{
}

HRESULT CWindowPlay::Initialize()
{
    m_pDevice = GAMEINSTANCE->GetDeviceInfo();
    m_pContext = GAMEINSTANCE->GetDeviceContextInfo();

    return S_OK;
}

void CWindowPlay::Tick(_float _fTimeDelta)
{
    KeyInput();

    static string strPlayButtonKey = "Play";

    if (ImGui::ImageButton("##EffectPlayButton", GAMEINSTANCE->GetSRV(strPlayButtonKey).Get(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1)))
    {
        m_bPlay = !m_bPlay;

        if (m_bPlay && m_fElapsedTime == m_fTotalDuration)
        {
            m_fElapsedTime = 0.f;
        }
    } ImGui::SameLine();

    if (ImGui::ImageButton("##EffectStopButton", GAMEINSTANCE->GetSRV("Stop").Get(), ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1)))
    {
        m_bPlay = false;
        m_fElapsedTime = 0.f;
    } ImGui::SameLine();

    if (ImGui::Checkbox("Loop", &m_bLoop))
    {

    } ImGui::SameLine();

    ImGui::SetNextItemWidth(100.f);
    if (ImGui::InputFloat("Speed", &m_fTotalSpeed, 0.01f, 1.0f, "%.2f"))
    {

    }

    if (m_bPlay)
    {
        m_fElapsedTime += _fTimeDelta * m_fTotalSpeed;
        if (m_fElapsedTime >= m_fTotalDuration)
        {
            if (m_bLoop)
                m_fElapsedTime = 0.f;

            else
            {
                m_fElapsedTime = m_fTotalDuration;
                m_bPlay = false;
            }
        }

        strPlayButtonKey = "Pause";
    }

    else
        strPlayButtonKey = "Play";

    if (ImGui::BeginTable("##PlayTable", 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupColumn("Visible", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 50);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow(); ImGui::SameLine();
        ImGui::Dummy({ 30.f, 0.f }); ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 50.f);
        if (ImGui::SliderFloat("##PlayEffects", &m_fElapsedTime, 0.f, m_fTotalDuration, "%.1f"))
        {
            for (auto& pEffect : m_pEffects)
            {
                //pEffect->SetTrackPosition(m_fElapsedTime - pEffect->GetStartTrackPos());  // ver.1
                //pEffect->SetTrackPosition(m_fElapsedTime);                            // ver.2
            }
        } ImGui::SameLine();

        ImGui::Text("%.2f", m_fTotalDuration);

        static string   strVisibleLabel[MAX_EFFECT_NUM] = {};
        static _bool    bVisible[MAX_EFFECT_NUM] = { true };

        for (int row = 0; row < 1; row++)
        {
            strVisibleLabel[row] = "##Visible" + to_string(row);

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::SetNextItemWidth(20.f);
            ImGui::Checkbox(strVisibleLabel[row].c_str(), &bVisible[row]);

            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(50.f);
            //ImGui::Text(m_pEffects[row]->GetName());

            ImGui::TableSetColumnIndex(2);
            ImGui::SetNextItemWidth(200.f);
            // 이펙트마다 재생바

        }
        ImGui::EndTable();
    }
}

void CWindowPlay::KeyInput()
{
}