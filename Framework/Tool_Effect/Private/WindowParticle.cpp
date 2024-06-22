#include "WindowParticle.h"
#include "GameInstance.h"

#include "ParticleSystem.h"
#include "Texture.h"

#include "Easing.h"

#include "ImGuiMgr.h"
#include "WindowModel.h"
#include "WindowPlay.h"

#include "TestParticle.h"

#include "AnimModel.h"
#include "Model.h"
#include "Bone.h"

#include <filesystem>
#include <iostream>
#include "WindowParticle.h"

CWindowParticle::CWindowParticle()
{
}

HRESULT CWindowParticle::Initialize()
{
	m_pDevice = GAMEINSTANCE->GetDeviceInfo();
	m_pContext = GAMEINSTANCE->GetDeviceContextInfo();

	wstring strEffectPath = TEXT("..\\..\\Tool_Effect\\Bin\\Resources\\Level_EffectTool");

	m_TextureList[TEX_DIFFUSE].resize(300);

	/*m_TextureList[TEX_DIFFUSE].reserve(100);
	m_TextureList[TEX_MASK].reserve(100);
	m_TextureList[TEX_NOISE].reserve(100);*/

	LoadResourceNames(strEffectPath, TEX_END);

	return S_OK;
}

void CWindowParticle::Tick(_float _fTimeDelta)
{
	//if (m_isTested)
	//{
	//	CParticleSystem::PARTICLE_SYSTEM eParticle;
	//	eParticle.bUseDiffuse = true;
	//	eParticle.bSpeedMode = 1;
	//	eParticle.strTexDiffuseKey = "Blood_Green";

	//	CParticleSystem::COLOR_KEYFRAME_PARTICLE colorKeyFrame;
	//	colorKeyFrame.fTime = 0.f;
	//	colorKeyFrame.eEaseType = 0;
	//	colorKeyFrame.vColorMin = { 0.6f, 0.5f, 0.6f, 1.f };
	//	colorKeyFrame.vColorMax = { 0.8f, 0.6f, 0.7f, 1.f };

	//	eParticle.KeyFrames.ColorKeyFrames.push_back(colorKeyFrame);

	//	CParticleSystem::SCALE_KEYFRAME_PARTICLE scaleKeyFrame;
	//	scaleKeyFrame.fTime = 0.f;
	//	scaleKeyFrame.eEaseType = 0;
	//	scaleKeyFrame.vScaleMin = { 1.f, 1.f, 1.f };
	//	scaleKeyFrame.vScaleMax = { 2.f, 2.f, 2.f };

	//	eParticle.KeyFrames.ScaleKeyFrames.push_back(scaleKeyFrame);

	//	eParticle.iShaderType = ESHADER_TYPE::ST_PARTICLE;
	//	eParticle.iRenderGroup = CRenderer::RENDER_NONLIGHT;

	//	//eParticle.iMaxParticles 
	//	//m_pTestParticle->Initialize();

	//	m_pTestParticle = CParticleSystem::Create(&eParticle);

	//	if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_TestEffect"), m_pTestParticle)))
	//		return;

	//	m_isTested = true;
	//}

	//KeyInput();

	/*if (ImGui::TreeNode("0. Default"))
	{
		Properties_Default();

		ImGui::TreePop();
	}*/

	if (ImGui::TreeNode("1. Main"))
	{
		Properties_Main();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("2. Position"))
	{
		Properties_Position();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("3. Speed"))
	{
		Properties_Speed();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("4. Scale"))
	{
		Properties_Scale();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("5. Rotation"))
	{
		Properties_Rotation();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("6. Color"))
	{
		Properties_Color();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("7. Shader"))
	{
		Properties_Shader();

		ImGui::TreePop();
	}

	//if (ImGui::TreeNode("6. Scale"))
	//{
	//	Properties_Scale();

	//	ImGui::TreePop();
	//}

	//if (ImGui::TreeNode("7. Rotation"))
	//{
	//	Properties_Rotation();

	//	ImGui::TreePop();
	//}

	//if (ImGui::TreeNode("8. Translation"))
	//{
	//	Properties_Translation();

	//	ImGui::TreePop();
	//}
}

void CWindowParticle::Properties_Default()
{
	ImGui::Dummy(ImVec2(0.f, 5.f));

	ImGui::Text("Shader Type  "); ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	const char* strShader[] = { "POSTEX", "NORTEX", "NONANIM", "ANIM", "EFFECT_MESH", "EFFECT_TEX", "EFFECT_PARTICLE", "UI", "DEFERRED" };
	static int iShaderIdx = 6; // Here we store our selection data as an index.
	const char* iShaderIdx_value = strShader[iShaderIdx];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo("##ShaderType", iShaderIdx_value, ImGuiComboFlags_PopupAlignLeft))
	{
		for (int n = 0; n < IM_ARRAYSIZE(strShader); n++)
		{
			const bool is_selected = (iShaderIdx == n);
			if (ImGui::Selectable(strShader[n], is_selected))
			{
				iShaderIdx = n;
				m_iShaderType = iShaderIdx;

				if (m_pTestParticle)
					m_pTestParticle->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup);
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Text("Shader Pass  "); ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	if (ImGui::InputInt("##ShaderPass", &m_iShaderPass, 0, 0))
	{
		// here

		if (m_pTestParticle)
			m_pTestParticle->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup);
	}

	ImGui::Text("Render Group "); ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	const char* strRenderGroup[] = { "PRIORITY", "NONBLEND", "NONLIGHT", "BLEND", "UI" };
	static int iRenderIdx = 2; // Here we store our selection data as an index.
	const char* iRenderIdx_value = strRenderGroup[iRenderIdx];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo("##RenderGroup", iRenderIdx_value, ImGuiComboFlags_PopupAlignLeft))
	{
		for (int n = 0; n < IM_ARRAYSIZE(strRenderGroup); n++)
		{
			const bool is_selected = (iRenderIdx == n);
			if (ImGui::Selectable(strRenderGroup[n], is_selected))
			{
				iRenderIdx = n;
				m_iRenderGroup = iRenderIdx;

				// here

				if (m_pTestParticle)
					m_pTestParticle->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup);
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Dummy(ImVec2(0.f, 5.f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.f, 5.f));
}

void CWindowParticle::Properties_Main()
{
	ImGui::Dummy(ImVec2(0.f, 3.f));
	ImGui::SetNextItemWidth(230.f);
	if (ImGui::DragInt("Num Particle", &m_iNumInstance, 2.f, 1, MAX_PARTICLE))
	{
		// here
		if (m_pTestParticle)
			m_pTestParticle->SetMainInfo(m_iNumInstance, m_fDuration, m_fStartTrackPos, _float2(m_fLifeTime[0], m_fLifeTime[1]), m_bLoop, m_iBllBoardType, m_strDiffuseTexTag, m_iSpawnType, m_iSpawnNumPerSec);

	}

	ImGui::Dummy(ImVec2(0.f, 3.f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.f, 3.f));

	ImGui::Text("Start TrackPos  "); ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	if (ImGui::InputFloat("##TrackPos", &m_fStartTrackPos, 0.f, 0.f, "%.2f"))
	{
		// here
		if (m_pTestParticle)
			m_pTestParticle->SetMainInfo(m_iNumInstance, m_fDuration, m_fStartTrackPos, _float2(m_fLifeTime[0], m_fLifeTime[1]), m_bLoop, m_iBllBoardType, m_strDiffuseTexTag, m_iSpawnType, m_iSpawnNumPerSec);

	}

	ImGui::Text("Duration  "); ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	if (ImGui::InputFloat("sec", &m_fDuration, 0.f, 0.f, "%.2f"))
	{
		// here
		if (m_pTestParticle)
			m_pTestParticle->SetMainInfo(m_iNumInstance, m_fDuration, m_fStartTrackPos, _float2(m_fLifeTime[0], m_fLifeTime[1]), m_bLoop, m_iBllBoardType, m_strDiffuseTexTag, m_iSpawnType, m_iSpawnNumPerSec);

	}

	ImGui::Text("LifeTime  "); ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	if (ImGui::InputFloat2("(min, max)", m_fLifeTime, "%.2f"))
	{
		// here
		if (m_pTestParticle)
			m_pTestParticle->SetMainInfo(m_iNumInstance, m_fDuration, m_fStartTrackPos, _float2(m_fLifeTime[0], m_fLifeTime[1]), m_bLoop, m_iBllBoardType, m_strDiffuseTexTag, m_iSpawnType, m_iSpawnNumPerSec);

	}

	ImGui::Text("Loop      "); ImGui::SameLine();
	if (ImGui::Checkbox("##Loop", &m_bLoop))
	{
		// here
		if (m_pTestParticle)
			m_pTestParticle->SetMainInfo(m_iNumInstance, m_fDuration, m_fStartTrackPos, _float2(m_fLifeTime[0], m_fLifeTime[1]), m_bLoop, m_iBllBoardType, m_strDiffuseTexTag, m_iSpawnType, m_iSpawnNumPerSec);

	}
	ImGui::Dummy(ImVec2(0.f, 3.f));

	ImGui::Separator();

	ImGui::Dummy(ImVec2(0.f, 3.f));
	ImGui::Text("Render Type  ");
	ImGui::Dummy(ImVec2(0.f, 3.f));

	/*int iPrevBillBoardType = m_iBllBoardType;
	ImGui::RadioButton("None", &m_iBllBoardType, 0); ImGui::SameLine();
	ImGui::RadioButton("BillBoard", &m_iBllBoardType, 1); ImGui::SameLine();
	ImGui::RadioButton("Directional", &m_iBllBoardType, 2);

	if ((iPrevBillBoardType != m_iBllBoardType) && m_pTestParticle)
		m_pTestParticle->SetMainInfo(m_iNumInstance, m_fDuration, m_fStartTrackPos, _float2(m_fLifeTime[0], m_fLifeTime[1]), m_bLoop, m_iBllBoardType, m_strDiffuseTexTag, m_iSpawnType, m_iSpawnNumPerSec);*/

	ImGui::Dummy(ImVec2(0.f, 3.f));

	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.f, 3.f));
	ImGui::Text("Texture");
	ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 30), ImVec2(FLT_MAX, 108));
	if (ImGui::BeginListBox(""))
	{
		for (_int i = 0; i < m_iTextureIdx[TEX_DIFFUSE]; ++i)
		{
			ImGui::PushID(i);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.f, 2.f));

			ImVec2 size = ImVec2(45.0f, 45.0f);                         // Size of the image we want to make visible
			ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
			ImVec2 uv1 = ImVec2(1.f, 1.f);                              // UV coordinates for (32,32) in our texture
			ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint

			_tchar wTextureKey[MAX_PATH] = TEXT("");

			ImGuiIO& io = ImGui::GetIO();

			MultiByteToWideChar(CP_ACP, 0, m_TextureList[TEX_DIFFUSE][i].c_str(), static_cast<int>(strlen(m_TextureList[TEX_DIFFUSE][i].c_str())), wTextureKey, MAX_PATH);

			if (ImGui::ImageButton(m_TextureList[TEX_DIFFUSE][i].c_str(), GAMEINSTANCE->GetSRV(m_TextureList[TEX_DIFFUSE][i]).Get(), size, uv0, uv1, bg_col, tint_col))
			{
				m_strDiffuseTexTag = m_TextureList[TEX_DIFFUSE][i];

				GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_TestEffect"));
				m_pTestParticle = nullptr;

				// here
				m_pTestParticle = CTestParticle::Create(m_iNumInstance, m_strDiffuseTexTag);

				if (m_pTestParticle)
				{
					m_pTestParticle->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup);
					m_pTestParticle->SetMainInfo(m_iNumInstance, m_fDuration, m_fStartTrackPos, _float2(m_fLifeTime[0], m_fLifeTime[1]), m_bLoop, m_iBllBoardType, m_strDiffuseTexTag, m_iSpawnType, m_iSpawnNumPerSec);

					if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_TestEffect"), m_pTestParticle)))
						return;
				}
			}

			if (m_strDiffuseTexTag == m_TextureList[TEX_DIFFUSE][i])
			{
				ImVec2 pos = ImGui::GetItemRectMin();
				ImVec2 size = ImGui::GetItemRectSize();
				ImGui::GetWindowDrawList()->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 0.3f, 0.3f, 0.7f)), 0.0f, 0, 2.f);
			}

			ImGui::PopStyleVar();

			ImGui::PopID();
			if ((i + 1) % 5 != 0)
				ImGui::SameLine();
		}

		ImGui::EndListBox();
	}

	ImGui::Dummy(ImVec2(0.f, 3.f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.f, 3.f));

	ImGui::Text("Spawn Type  "); ImGui::SameLine();

	int iPrevSpawnType = m_iSpawnType;
	ImGui::RadioButton("Continuous", &m_iSpawnType, 0); ImGui::SameLine();
	ImGui::RadioButton("Burst", &m_iSpawnType, 1);

	if ((iPrevSpawnType != m_iSpawnType) && m_pTestParticle)
		m_pTestParticle->SetMainInfo(m_iNumInstance, m_fDuration, m_fStartTrackPos, _float2(m_fLifeTime[0], m_fLifeTime[1]), m_bLoop, m_iBllBoardType, m_strDiffuseTexTag, m_iSpawnType, m_iSpawnNumPerSec);


	// Fixed
	if (0 == m_iSpawnType)
	{
		ImGui::Text("Spawn Num  "); ImGui::SameLine();
		ImGui::SetNextItemWidth(150.f);
		if (ImGui::InputInt("##SpawnNum", &m_iSpawnNumPerSec, 1, 100))
		{
			if (0 >= m_iSpawnNumPerSec)
				m_iSpawnNumPerSec = 1;
			else if (m_iNumInstance < m_iSpawnNumPerSec)
				m_iSpawnNumPerSec = m_iNumInstance;

			// here
			if (m_pTestParticle)
				m_pTestParticle->SetMainInfo(m_iNumInstance, m_fDuration, m_fStartTrackPos, _float2(m_fLifeTime[0], m_fLifeTime[1]), m_bLoop, m_iBllBoardType, m_strDiffuseTexTag, m_iSpawnType, m_iSpawnNumPerSec);

		} ImGui::SameLine();

		ImGui::Text("per Sec");
	}
	ImGui::Dummy(ImVec2(0.f, 3.f));
}

// 발생 위치
void CWindowParticle::Properties_Position()
{
	static string strPositionTimeLabel[MAX_TIMETABLE];
	static string strPositionLabel[MAX_TIMETABLE];
	static string strPositionEasingTypeLabel[MAX_TIMETABLE];

	int iPrevPosType = m_iPosType;
	ImGui::RadioButton("World", &m_iPosType, 0); ImGui::SameLine();
	ImGui::RadioButton("Object", &m_iPosType, 1); ImGui::SameLine();
	ImGui::RadioButton("Bone", &m_iPosType, 2);

	if (iPrevPosType != m_iPosType)
	{
		// here
		if (m_pTestParticle)
			m_pTestParticle->SetPosInfo(m_iPosType, m_bPosFollow, m_strPosFollowBoneKey, m_iPosSetCnt, m_fPosTime, m_vPosMin, m_vPosMax, m_iPosEasing, m_bUsePosKeyFrame);

		if (1 == m_iPosType)
		{
			// 오브젝트 가져오기
			m_pFollowObj = CImGuiMgr::GetInstance()->GetWindowModel()->GetCurModelObj(), CImGuiMgr::GetInstance()->GetWindowModel();
		}

		else if (2 == m_iPosType)
		{
			// 오브젝트 가져오기
			m_pFollowObj = CImGuiMgr::GetInstance()->GetWindowModel()->GetCurModelObj(), CImGuiMgr::GetInstance()->GetWindowModel();

			if (!m_pFollowObj)
				return;

			m_pFollowObjModel = dynamic_pointer_cast<CModel>(m_pFollowObj->GetComponent(L"Com_Model"));

			if (!m_pFollowObjModel)
				return;

			m_pFollowObjBones = m_pFollowObjModel->GetBones();
			m_vecFollowBoneNames.clear();

			for (auto pBones : m_pFollowObjBones)
			{
				const _char* pName = pBones->GetBoneName();
				m_vecFollowBoneNames.push_back(pName);
			}
		}
	}

	ImGui::Dummy(ImVec2(0.f, 3.f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0.f, 3.f));

	if (0 == m_iPosType)
	{
		// 절대 좌표 기준
		ImGui::Text("World Pos");
	}

	else if (1 == m_iPosType)
	{
		if (!m_pFollowObj)
		{
			ImGui::Text("No Object To Follow");
			return;
		}

		if (m_bPosFollow)
		{
		}

		// 상대 좌표 기준
		ImGui::Text("Offset Pos");
	}

	else if (2 == m_iPosType)
	{
		if (!m_pFollowObj)
		{
			ImGui::Text("No Object To Follow");
			return;
		}

		if (0 == m_vecFollowBoneNames.size())
		{
			ImGui::Text("No Bones To Follow");
			return;
		}

		if (ImGui::BeginListBox("##BoneList", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (int n = 0; n < m_vecFollowBoneNames.size(); n++)
			{
				const bool is_selected = (m_pPosFollowBoneIdx == n);
				if (ImGui::Selectable(m_vecFollowBoneNames[n], is_selected))
				{
					m_pPosFollowBoneIdx = n;
					m_strPosFollowBoneKey = m_vecFollowBoneNames[n];

					if (m_pTestParticle)
						m_pTestParticle->SetPosInfo(m_iPosType, m_bPosFollow, m_strPosFollowBoneKey, m_iPosSetCnt, m_fPosTime, m_vPosMin, m_vPosMax, m_iPosEasing, m_bUsePosKeyFrame);
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		// 상대 좌표 기준
		ImGui::Text("Offset Pos");
	}

	ImGui::Checkbox("Use Pos KeyFrames", &m_bUsePosKeyFrame);

	if (m_bUsePosKeyFrame)
	{
		// Pos Time Table
		// Time, X, Y, Z, Easing
		if (ImGui::BeginTable("##PosTimeTable", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
		{
			ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Pos", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Easing", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow(); ImGui::SameLine();
			ImGui::Dummy({ 33.f, 2.f }); ImGui::SameLine();
			ImGui::Button("Graph", ImVec2(45, 17));
			if (ImGui::BeginItemTooltip())
			{
				ImGui::Text("Easing Graphs");
				ImGui::Image(GAMEINSTANCE->GetSRV("Easing").Get(), ImVec2(700.f, 600.f), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::EndTooltip();
			}

			for (int iCnt = 0; iCnt < m_iPosSetCnt; iCnt++)
			{
				m_strPosTimeLabel[iCnt] = "##PosTime" + to_string(iCnt);
				m_strPosMinLabel[iCnt] = "##PosMin" + to_string(iCnt);
				m_strPosMaxLabel[iCnt] = "##PosMax" + to_string(iCnt);
				m_strPosEaseTypeLabel[iCnt] = "##PosEase" + to_string(iCnt);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::SetNextItemWidth(40.f);
				if (ImGui::InputFloat(m_strPosTimeLabel[iCnt].c_str(), &m_fPosTime[iCnt], 0.f, 0.f, "%.2f"))
				{
					if (0.f > m_fPosTime[iCnt])
						m_fPosTime[iCnt] = 0.f;

					else if (m_fLifeTime[1] < m_fPosTime[iCnt])
						m_fPosTime[iCnt] = m_fLifeTime[1];

					if (m_pTestParticle)
						m_pTestParticle->SetPosInfo(m_iPosType, m_bPosFollow, m_strPosFollowBoneKey, m_iPosSetCnt, m_fPosTime, m_vPosMin, m_vPosMax, m_iPosEasing, m_bUsePosKeyFrame);


					// here

					/*if (m_pTestEffectMesh)
						m_pTestEffectMesh->SetPosInfo(m_bPosFollow, m_iPosFollowType, m_strPosFollowBoneKey, m_iPosCnt, m_fPosTime, m_fPos, m_iPosEasing);*/
						/*if (m_pTestModel)
							m_pTestModel->SetTransformTime(0, iCnt, m_fPosTime[iCnt]);*/
				}

				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(140.f);

				if (ImGui::InputFloat3(m_strPosMinLabel[iCnt].c_str(), m_vPosMin[iCnt], "%.2f"))
				{

					if (m_pTestParticle)
						m_pTestParticle->SetPosInfo(m_iPosType, m_bPosFollow, m_strPosFollowBoneKey, m_iPosSetCnt, m_fPosTime, m_vPosMin, m_vPosMax, m_iPosEasing, m_bUsePosKeyFrame);

					/*if (m_pTestEffectMesh)
						m_pTestEffectMesh->SetPosInfo(m_bPosFollow, m_iPosFollowType, m_strPosFollowBoneKey, m_iPosCnt, m_fPosTime, m_fPos, m_iPosEasing);*/
						/*if (m_pTestModel)
							m_pTestModel->SetTransformValue(0, iCnt, m_fPos[iCnt]);*/
				}

				ImGui::SetNextItemWidth(140.f);
				if (ImGui::InputFloat3(m_strPosMaxLabel[iCnt].c_str(), m_vPosMax[iCnt], "%.2f"))
				{

					if (m_pTestParticle)
						m_pTestParticle->SetPosInfo(m_iPosType, m_bPosFollow, m_strPosFollowBoneKey, m_iPosSetCnt, m_fPosTime, m_vPosMin, m_vPosMax, m_iPosEasing, m_bUsePosKeyFrame);

					/*if (m_pTestEffectMesh)
						m_pTestEffectMesh->SetPosInfo(m_bPosFollow, m_iPosFollowType, m_strPosFollowBoneKey, m_iPosCnt, m_fPosTime, m_fPos, m_iPosEasing);*/
						/*if (m_pTestModel)
							m_pTestModel->SetTransformValue(0, iCnt, m_fPos[iCnt]);*/
				}

				ImGui::TableSetColumnIndex(2);
				ImGui::SetNextItemWidth(100.f);

				const char* strEasingTypes[EASE_END] = {};
				memcpy(strEasingTypes, GAMEINSTANCE->GetEaseTypeKeys(), sizeof(strEasingTypes));
				const char* combo_preview_value = strEasingTypes[m_iPosEasing[iCnt]];  // Pass in the preview value visible before opening the combo (it could be anything)
				if (ImGui::BeginCombo(m_strPosEaseTypeLabel[iCnt].c_str(), combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
				{
					for (int n = 0; n < IM_ARRAYSIZE(strEasingTypes); n++)
					{
						const bool is_selected = (m_iPosEasing[iCnt] == n);
						if (ImGui::Selectable(strEasingTypes[n], is_selected))
						{
							m_iPosEasing[iCnt] = n;

							if (m_pTestParticle)
								m_pTestParticle->SetPosInfo(m_iPosType, m_bPosFollow, m_strPosFollowBoneKey, m_iPosSetCnt, m_fPosTime, m_vPosMin, m_vPosMax, m_iPosEasing, m_bUsePosKeyFrame);

							// here

							/*if (m_pTestEffectMesh)
								m_pTestEffectMesh->SetPosInfo(m_bPosFollow, m_iPosFollowType, m_strPosFollowBoneKey, m_iPosCnt, m_fPosTime, m_fPos, m_iPosEasing);*/
								/*if (m_pTestModel)
									m_pTestModel->SetTransformEasing(0, iCnt, m_iPosEasing[iCnt]);*/
						}

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
			ImGui::EndTable();
		}
		ImGui::Separator();

		// Add Set, Delete Set 버튼
		if (ImGui::Button("Add Set"))
		{
			// 최대 컬러 개수는 4개
			if (m_iPosSetCnt <= 3)
				m_iPosSetCnt++;

			if (m_pTestParticle)
				m_pTestParticle->SetPosInfo(m_iPosType, m_bPosFollow, m_strPosFollowBoneKey, m_iPosSetCnt, m_fPosTime, m_vPosMin, m_vPosMax, m_iPosEasing, m_bUsePosKeyFrame);

			/*if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetPosInfo(m_bPosFollow, m_iPosFollowType, m_strPosFollowBoneKey, m_iPosCnt, m_fPosTime, m_fPos, m_iPosEasing);*/

		} ImGui::SameLine();

		if (ImGui::Button("Delete Set"))
		{
			// 최소 컬러 개수는 1개
			if (m_iPosSetCnt >= 2)
				m_iPosSetCnt--;

			if (m_pTestParticle)
				m_pTestParticle->SetPosInfo(m_iPosType, m_bPosFollow, m_strPosFollowBoneKey, m_iPosSetCnt, m_fPosTime, m_vPosMin, m_vPosMax, m_iPosEasing, m_bUsePosKeyFrame);

			/*if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetPosInfo(m_bPosFollow, m_iPosFollowType, m_strPosFollowBoneKey, m_iPosCnt, m_fPosTime, m_fPos, m_iPosEasing);*/
		}
	}
	else
	{
		if (ImGui::InputFloat3("Min", m_vPosMin[0], "%.2f"))
		{
			if (m_pTestParticle)
				m_pTestParticle->SetPosInfo(m_iPosType, m_bPosFollow, m_strPosFollowBoneKey, m_iPosSetCnt, m_fPosTime, m_vPosMin, m_vPosMax, m_iPosEasing, m_bUsePosKeyFrame);
		}

		if (ImGui::InputFloat3("Max", m_vPosMax[0], "%.2f"))
		{
			if (m_pTestParticle)
				m_pTestParticle->SetPosInfo(m_iPosType, m_bPosFollow, m_strPosFollowBoneKey, m_iPosSetCnt, m_fPosTime, m_vPosMin, m_vPosMax, m_iPosEasing, m_bUsePosKeyFrame);
		}
	}

	if (1 == m_iPosType || 2 == m_iPosType)
	{
		// Follow 체크박스
		if (ImGui::Checkbox("Follow", &m_bPosFollow))
		{
			if (m_pTestParticle)
				m_pTestParticle->SetPosInfo(m_iPosType, m_bPosFollow, m_strPosFollowBoneKey, m_iPosSetCnt, m_fPosTime, m_vPosMin, m_vPosMax, m_iPosEasing, m_bUsePosKeyFrame);


			/* if (!m_bTranslationFollow && m_pTestEffectMesh)
				m_pTestEffectMesh->SetTransInfo(m_bTranslationFollow, m_iTranslationFollowType, m_strTranslationFollowBoneKey, m_iTranslationCnt, m_fTranslationTime, m_fTranslation, m_iTranslationEasing);*/
		}
	}
	ImGui::Separator();
	if (ImGui::Checkbox("Use Pivot", &m_bUsePivot))
	{
		if (m_pTestParticle)
			m_pTestParticle->SetPivotInfo(m_bUsePivot, m_vPivotMin, m_vPivotMax);
	}

	if (m_bUsePivot)
	{
		if (ImGui::InputFloat3("Pivot Min", m_vPivotMin))
		{
			if (m_pTestParticle)
				m_pTestParticle->SetPivotInfo(m_bUsePivot, m_vPivotMin, m_vPivotMax);
		}

		if (ImGui::InputFloat3("Pivot Max", m_vPivotMax))
		{
			if (m_pTestParticle)
				m_pTestParticle->SetPivotInfo(m_bUsePivot, m_vPivotMin, m_vPivotMax);
		}
	}

	ImGui::Separator();

	if (ImGui::Checkbox("Implosion Mode", &m_bImplosion))
	{
		m_iBllBoardType = m_bImplosion;

		if (m_bImplosion)
			m_bUsePivot = true;

		if (m_pTestParticle)
			m_pTestParticle->SetMainInfo(m_iNumInstance, m_fDuration, m_fStartTrackPos, _float2(m_fLifeTime[0], m_fLifeTime[1]), m_bLoop, m_iBllBoardType, m_strDiffuseTexTag, m_iSpawnType, m_iSpawnNumPerSec);

		if (m_pTestParticle)
			m_pTestParticle->SetPivotInfo(m_bUsePivot, m_vPivotMin, m_vPivotMax);
	}
}

void CWindowParticle::Properties_Speed()
{
	ImGui::Dummy(ImVec2(0.f, 3.f));

	int iPrevSpeedType = m_iSpeedType;
	ImGui::RadioButton("Time Table", &m_iSpeedType, 0); ImGui::SameLine();
	ImGui::RadioButton("Acceleration", &m_iSpeedType, 1);

	if (iPrevSpeedType != m_iSpeedType)
	{
		// here

		if (0 == m_iSpeedType)
		{
			if (m_pTestParticle)
				m_pTestParticle->SetSpeedInfo(m_iSpeedType, m_iSpeedSetCnt, m_fSpeedTime, m_fSpeedMin, m_fSpeedMax, m_iSpeedEasing);
		}

		else if (1 == m_iSpeedType)
		{
			if (m_pTestParticle)
				m_pTestParticle->SetSpeedInfo(m_iSpeedType, _float3(m_vStartSpeedMin[0], m_vStartSpeedMin[1], m_vStartSpeedMin[2]), _float3(m_vStartSpeedMax[0], m_vStartSpeedMax[1], m_vStartSpeedMax[2]), _float3(m_vSpeedAccMin[0], m_vSpeedAccMin[1], m_vSpeedAccMin[2]), _float3(m_vSpeedAccMax[0], m_vSpeedAccMax[1], m_vSpeedAccMax[2]));
		}
	}

	// Time Table Type
	if (0 == m_iSpeedType)
	{
		//// TimeTable
		// Time, X, Y, Z, Easing
		if (ImGui::BeginTable("##SpeedTimeTable", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
		{
			ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Speed", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Easing", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow(); ImGui::SameLine();
			ImGui::Dummy({ 33.f, 2.f }); ImGui::SameLine();
			ImGui::Button("Graph", ImVec2(45, 17));
			if (ImGui::BeginItemTooltip())
			{
				ImGui::Text("Easing Graphs");
				ImGui::Image(GAMEINSTANCE->GetSRV("Easing").Get(), ImVec2(700.f, 600.f), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::EndTooltip();
			}

			for (int iCnt = 0; iCnt < m_iSpeedSetCnt; iCnt++)
			{
				m_strSpeedTimeLabel[iCnt] = "##SpeedTime" + to_string(iCnt);
				m_strSpeedMinLabel[iCnt] = "##SpeedMin" + to_string(iCnt);
				m_strSpeedMaxLabel[iCnt] = "##SpeedMax" + to_string(iCnt);
				m_strSpeedEaseTypeLabel[iCnt] = "##SpeedEase" + to_string(iCnt);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::SetNextItemWidth(40.f);
				if (ImGui::InputFloat(m_strSpeedTimeLabel[iCnt].c_str(), &m_fSpeedTime[iCnt], 0.f, 0.f, "%.2f"))
				{
					if (0.f > m_fSpeedTime[iCnt])
						m_fSpeedTime[iCnt] = 0.f;

					else if (m_fLifeTime[1] < m_fSpeedTime[iCnt])
						m_fSpeedTime[iCnt] = m_fLifeTime[1];

					if (m_pTestParticle)
						m_pTestParticle->SetSpeedInfo(m_iSpeedType, m_iSpeedSetCnt, m_fSpeedTime, m_fSpeedMin, m_fSpeedMax, m_iSpeedEasing);

					// here

					/*if (m_pTestEffectMesh)
						m_pTestEffectMesh->SetSpeedInfo(m_bSpeedFollow, m_iSpeedFollowType, m_strSpeedFollowBoneKey, m_iSpeedCnt, m_fSpeedTime, m_fSpeed, m_iSpeedEasing);*/
						/*if (m_pTestModel)
							m_pTestModel->SetTransformTime(0, iCnt, m_fSpeedTime[iCnt]);*/
				}

				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(140.f);

				if (ImGui::InputFloat3(m_strSpeedMinLabel[iCnt].c_str(), m_fSpeedMin[iCnt], "%.2f"))
				{
					if (m_pTestParticle)
						m_pTestParticle->SetSpeedInfo(m_iSpeedType, m_iSpeedSetCnt, m_fSpeedTime, m_fSpeedMin, m_fSpeedMax, m_iSpeedEasing);

					/*if (m_pTestEffectMesh)
						m_pTestEffectMesh->SetSpeedInfo(m_bSpeedFollow, m_iSpeedFollowType, m_strSpeedFollowBoneKey, m_iSpeedCnt, m_fSpeedTime, m_fSpeed, m_iSpeedEasing);*/
						/*if (m_pTestModel)
							m_pTestModel->SetTransformValue(0, iCnt, m_fSpeed[iCnt]);*/
				}

				ImGui::SetNextItemWidth(140.f);
				if (ImGui::InputFloat3(m_strSpeedMaxLabel[iCnt].c_str(), m_fSpeedMax[iCnt], "%.2f"))
				{
					if (m_pTestParticle)
						m_pTestParticle->SetSpeedInfo(m_iSpeedType, m_iSpeedSetCnt, m_fSpeedTime, m_fSpeedMin, m_fSpeedMax, m_iSpeedEasing);

					/*if (m_pTestEffectMesh)
						m_pTestEffectMesh->SetSpeedInfo(m_bSpeedFollow, m_iSpeedFollowType, m_strSpeedFollowBoneKey, m_iSpeedCnt, m_fSpeedTime, m_fSpeed, m_iSpeedEasing);*/
						/*if (m_pTestModel)
							m_pTestModel->SetTransformValue(0, iCnt, m_fSpeed[iCnt]);*/
				}

				ImGui::TableSetColumnIndex(2);
				ImGui::SetNextItemWidth(100.f);

				const char* strEasingTypes[EASE_END] = {};
				memcpy(strEasingTypes, GAMEINSTANCE->GetEaseTypeKeys(), sizeof(strEasingTypes));
				const char* combo_preview_value = strEasingTypes[m_iSpeedEasing[iCnt]];  // Pass in the preview value visible before opening the combo (it could be anything)
				if (ImGui::BeginCombo(m_strSpeedEaseTypeLabel[iCnt].c_str(), combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
				{
					for (int n = 0; n < IM_ARRAYSIZE(strEasingTypes); n++)
					{
						const bool is_selected = (m_iSpeedEasing[iCnt] == n);
						if (ImGui::Selectable(strEasingTypes[n], is_selected))
						{
							m_iSpeedEasing[iCnt] = n;

							if (m_pTestParticle)
								m_pTestParticle->SetSpeedInfo(m_iSpeedType, m_iSpeedSetCnt, m_fSpeedTime, m_fSpeedMin, m_fSpeedMax, m_iSpeedEasing);

							// here

							/*if (m_pTestEffectMesh)
								m_pTestEffectMesh->SetSpeedInfo(m_bSpeedFollow, m_iSpeedFollowType, m_strSpeedFollowBoneKey, m_iSpeedCnt, m_fSpeedTime, m_fSpeed, m_iSpeedEasing);*/
								/*if (m_pTestModel)
									m_pTestModel->SetTransformEasing(0, iCnt, m_iSpeedEasing[iCnt]);*/
						}

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
			ImGui::EndTable();
		}
		ImGui::Separator();

		// Add Set, Delete Set 버튼
		if (ImGui::Button("Add Set"))
		{
			// 최대 컬러 개수는 4개
			if (m_iSpeedSetCnt <= 3)
				m_iSpeedSetCnt++;
			if (m_pTestParticle)
				m_pTestParticle->SetSpeedInfo(m_iSpeedType, m_iSpeedSetCnt, m_fSpeedTime, m_fSpeedMin, m_fSpeedMax, m_iSpeedEasing);
			/*if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetSpeedInfo(m_bSpeedFollow, m_iSpeedFollowType, m_strSpeedFollowBoneKey, m_iSpeedCnt, m_fSpeedTime, m_fSpeed, m_iSpeedEasing);*/

		} ImGui::SameLine();

		if (ImGui::Button("Delete Set"))
		{
			// 최소 컬러 개수는 1개
			if (m_iSpeedSetCnt >= 2)
				m_iSpeedSetCnt--;
			if (m_pTestParticle)
				m_pTestParticle->SetSpeedInfo(m_iSpeedType, m_iSpeedSetCnt, m_fSpeedTime, m_fSpeedMin, m_fSpeedMax, m_iSpeedEasing);
			/*if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetSpeedInfo(m_bSpeedFollow, m_iSpeedFollowType, m_strSpeedFollowBoneKey, m_iSpeedCnt, m_fSpeedTime, m_fSpeed, m_iSpeedEasing);*/
		}
	}

	else
	{
		if (ImGui::InputFloat3("Start Speed Min", m_vStartSpeedMin, "%.3f"))
		{
			if (m_pTestParticle)
				m_pTestParticle->SetSpeedInfo(m_iSpeedType, _float3(m_vStartSpeedMin[0], m_vStartSpeedMin[1], m_vStartSpeedMin[2]), _float3(m_vStartSpeedMax[0], m_vStartSpeedMax[1], m_vStartSpeedMax[2]), _float3(m_vSpeedAccMin[0], m_vSpeedAccMin[1], m_vSpeedAccMin[2]), _float3(m_vSpeedAccMax[0], m_vSpeedAccMax[1], m_vSpeedAccMax[2]));
		}
		if (ImGui::InputFloat3("Start Speed Max", m_vStartSpeedMax, "%.3f"))
		{
			if (m_pTestParticle)
				m_pTestParticle->SetSpeedInfo(m_iSpeedType, _float3(m_vStartSpeedMin[0], m_vStartSpeedMin[1], m_vStartSpeedMin[2]), _float3(m_vStartSpeedMax[0], m_vStartSpeedMax[1], m_vStartSpeedMax[2]), _float3(m_vSpeedAccMin[0], m_vSpeedAccMin[1], m_vSpeedAccMin[2]), _float3(m_vSpeedAccMax[0], m_vSpeedAccMax[1], m_vSpeedAccMax[2]));
		}


		ImGui::Separator();

		if (ImGui::InputFloat3("Speed Acc Min", m_vSpeedAccMin, "%.3f"))
		{
			if (m_pTestParticle)
				m_pTestParticle->SetSpeedInfo(m_iSpeedType, _float3(m_vStartSpeedMin[0], m_vStartSpeedMin[1], m_vStartSpeedMin[2]), _float3(m_vStartSpeedMax[0], m_vStartSpeedMax[1], m_vStartSpeedMax[2]), _float3(m_vSpeedAccMin[0], m_vSpeedAccMin[1], m_vSpeedAccMin[2]), _float3(m_vSpeedAccMax[0], m_vSpeedAccMax[1], m_vSpeedAccMax[2]));
		}
		if (ImGui::InputFloat3("Speed Acc Max", m_vSpeedAccMax, "%.3f"))
		{
			if (m_pTestParticle)
				m_pTestParticle->SetSpeedInfo(m_iSpeedType, _float3(m_vStartSpeedMin[0], m_vStartSpeedMin[1], m_vStartSpeedMin[2]), _float3(m_vStartSpeedMax[0], m_vStartSpeedMax[1], m_vStartSpeedMax[2]), _float3(m_vSpeedAccMin[0], m_vSpeedAccMin[1], m_vSpeedAccMin[2]), _float3(m_vSpeedAccMax[0], m_vSpeedAccMax[1], m_vSpeedAccMax[2]));
		}
	}

	if (ImGui::Checkbox("Relative To Owner", &m_bRelativeOwner))
	{
		if (m_pTestParticle)
			m_pTestParticle->SetHazeInfo(m_bUseHazeEffect, m_vHazeSpeedMin, m_vHazeSpeedMax, m_bRelativeOwner);
	}

	if (ImGui::Checkbox("Use Haze Effect", &m_bUseHazeEffect))
	{
		if (m_pTestParticle)
			m_pTestParticle->SetHazeInfo(m_bUseHazeEffect, m_vHazeSpeedMin, m_vHazeSpeedMax, m_bRelativeOwner);
	}

	if (m_bUseHazeEffect)
	{
		if (ImGui::InputFloat3("Haze Min", m_vHazeSpeedMin, "%.3f"))
		{
			if (m_pTestParticle)
				m_pTestParticle->SetHazeInfo(m_bUseHazeEffect, m_vHazeSpeedMin, m_vHazeSpeedMax, m_bRelativeOwner);
		}
		if (ImGui::InputFloat3("Haze Max", m_vHazeSpeedMax, "%.3f"))
		{
			if (m_pTestParticle)
				m_pTestParticle->SetHazeInfo(m_bUseHazeEffect, m_vHazeSpeedMin, m_vHazeSpeedMax, m_bRelativeOwner);
		}
	}
}

void CWindowParticle::Properties_Scale()
{
	ImGui::Dummy(ImVec2(0.f, 3.f));

	//// TimeTable
	// Time, X, Y, Z, Easing
	if (ImGui::BeginTable("##ScaleTimeTable", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
	{
		ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Scale", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Easing", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow(); ImGui::SameLine();
		ImGui::Dummy({ 33.f, 2.f }); ImGui::SameLine();
		ImGui::Button("Graph", ImVec2(45, 17));
		if (ImGui::BeginItemTooltip())
		{
			ImGui::Text("Easing Graphs");
			ImGui::Image(GAMEINSTANCE->GetSRV("Easing").Get(), ImVec2(700.f, 600.f), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::EndTooltip();
		}

		for (int iCnt = 0; iCnt < m_iScaleSetCnt; iCnt++)
		{
			m_strScaleTimeLabel[iCnt] = "##ScaleTime" + to_string(iCnt);
			m_strScaleMinLabel[iCnt] = "##ScaleMin" + to_string(iCnt);
			m_strScaleMaxLabel[iCnt] = "##ScaleMax" + to_string(iCnt);
			m_strScaleEaseTypeLabel[iCnt] = "##ScaleEase" + to_string(iCnt);

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(40.f);
			if (ImGui::InputFloat(m_strScaleTimeLabel[iCnt].c_str(), &m_fScaleTime[iCnt], 0.f, 0.f, "%.2f"))
			{
				if (0.f > m_fScaleTime[iCnt])
					m_fScaleTime[iCnt] = 0.f;

				else if (m_fLifeTime[1] < m_fScaleTime[iCnt])
					m_fScaleTime[iCnt] = m_fLifeTime[1];

				if (m_pTestParticle)
					m_pTestParticle->SetScaleInfo(m_iScaleSetCnt, m_fScaleTime, m_fScaleMin, m_fScaleMax, m_iScaleEasing);


				// here

				/*if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);*/
					/*if (m_pTestModel)
						m_pTestModel->SetTransformTime(0, iCnt, m_fScaleTime[iCnt]);*/
			}

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(140.f);

			if (ImGui::InputFloat3(m_strScaleMinLabel[iCnt].c_str(), m_fScaleMin[iCnt], "%.2f"))
			{

				if (m_pTestParticle)
					m_pTestParticle->SetScaleInfo(m_iScaleSetCnt, m_fScaleTime, m_fScaleMin, m_fScaleMax, m_iScaleEasing);

				/*if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);*/
					/*if (m_pTestModel)
						m_pTestModel->SetTransformValue(0, iCnt, m_fScale[iCnt]);*/
			}

			ImGui::SetNextItemWidth(140.f);
			if (ImGui::InputFloat3(m_strScaleMaxLabel[iCnt].c_str(), m_fScaleMax[iCnt], "%.2f"))
			{

				if (m_pTestParticle)
					m_pTestParticle->SetScaleInfo(m_iScaleSetCnt, m_fScaleTime, m_fScaleMin, m_fScaleMax, m_iScaleEasing);

				/*if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);*/
					/*if (m_pTestModel)
						m_pTestModel->SetTransformValue(0, iCnt, m_fScale[iCnt]);*/
			}

			ImGui::TableSetColumnIndex(2);
			ImGui::SetNextItemWidth(100.f);

			const char* strEasingTypes[EASE_END] = {};
			memcpy(strEasingTypes, GAMEINSTANCE->GetEaseTypeKeys(), sizeof(strEasingTypes));
			const char* combo_preview_value = strEasingTypes[m_iScaleEasing[iCnt]];  // Pass in the preview value visible before opening the combo (it could be anything)
			if (ImGui::BeginCombo(m_strScaleEaseTypeLabel[iCnt].c_str(), combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
			{
				for (int n = 0; n < IM_ARRAYSIZE(strEasingTypes); n++)
				{
					const bool is_selected = (m_iScaleEasing[iCnt] == n);
					if (ImGui::Selectable(strEasingTypes[n], is_selected))
					{
						m_iScaleEasing[iCnt] = n;

						if (m_pTestParticle)
							m_pTestParticle->SetScaleInfo(m_iScaleSetCnt, m_fScaleTime, m_fScaleMin, m_fScaleMax, m_iScaleEasing);

						// here

						/*if (m_pTestEffectMesh)
							m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);*/
							/*if (m_pTestModel)
								m_pTestModel->SetTransformEasing(0, iCnt, m_iScaleEasing[iCnt]);*/
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		ImGui::EndTable();
	}
	ImGui::Separator();

	// Add Set, Delete Set 버튼
	if (ImGui::Button("Add Set"))
	{
		// 최대 컬러 개수는 4개
		if (m_iScaleSetCnt <= 3)
			m_iScaleSetCnt++;

		if (m_pTestParticle)
			m_pTestParticle->SetScaleInfo(m_iScaleSetCnt, m_fScaleTime, m_fScaleMin, m_fScaleMax, m_iScaleEasing);
		/*if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);*/

	} ImGui::SameLine();

	if (ImGui::Button("Delete Set"))
	{
		// 최소 컬러 개수는 1개
		if (m_iScaleSetCnt >= 2)
			m_iScaleSetCnt--;

		if (m_pTestParticle)
			m_pTestParticle->SetScaleInfo(m_iScaleSetCnt, m_fScaleTime, m_fScaleMin, m_fScaleMax, m_iScaleEasing);
		/*if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);*/
	}
}

void CWindowParticle::Properties_Rotation()
{
	if (ImGui::BeginTable("##RotateTable", 5, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
	{
		ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Up", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Look", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("unit", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();

		{
			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Rotation Min");

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##NRotation Min R", &m_vRotationMin[0], 0.f, 0.f, "%.2f"))
			{
				// here
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 0, m_fRotate);
				}*/
				/*if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));*/

			}

			ImGui::TableSetColumnIndex(2);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##Rotation Min U", &m_vRotationMin[1], 0.f, 0.f, "%.2f"))
			{
				// here
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);


				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 0, m_fRotate);
				}*/
			}

			ImGui::TableSetColumnIndex(3);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##Rotation Min L", &m_vRotationMin[2], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 0, m_fRotate);
				}*/
			}

			ImGui::TableSetColumnIndex(4);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Deg");
		}

		ImGui::TableNextRow();

		{
			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Rotation Max");

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##NRotation Max R", &m_vRotationMax[0], 0.f, 0.f, "%.2f"))
			{
				// here

				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 0, m_fRotate);
				}*/
				/*if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));*/

			}

			ImGui::TableSetColumnIndex(2);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##Rotation Max U", &m_vRotationMax[1], 0.f, 0.f, "%.2f"))
			{
				// here
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);


				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 0, m_fRotate);
				}*/
			}

			ImGui::TableSetColumnIndex(3);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##Rotation Max L", &m_vRotationMax[2], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 0, m_fRotate);
				}*/
			}

			ImGui::TableSetColumnIndex(4);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Deg");
		}

		ImGui::TableNextRow();

		{
			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Turn(vel) Min");

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnV Min R", &m_vTurnVelMin[0], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 1, m_fTurnVel);
				}*/
			}

			ImGui::TableSetColumnIndex(2);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnV Min U", &m_vTurnVelMin[1], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 1, m_fTurnVel);
				}*/
				
			}

			ImGui::TableSetColumnIndex(3);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnV Min L", &m_vTurnVelMin[2], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 1, m_fTurnVel);
				}*/
				
			}

			ImGui::TableSetColumnIndex(4);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Deg/Sec");
		}

		ImGui::TableNextRow();

		{
			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Turn(vel) Max");

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnV Max R", &m_vTurnVelMax[0], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 1, m_fTurnVel);
				}*/
			}

			ImGui::TableSetColumnIndex(2);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnV Max U", &m_vTurnVelMax[1], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 1, m_fTurnVel);
				}*/

			}

			ImGui::TableSetColumnIndex(3);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnV Max L", &m_vTurnVelMax[2], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 1, m_fTurnVel);
				}*/

			}

			ImGui::TableSetColumnIndex(4);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Deg/Sec");
		}

		ImGui::TableNextRow();

		{
			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Turn(Acc) Min");

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnAcc Min R", &m_vTurnAccMin[0], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 2, m_fTurnAcc);
				}*/
				
			}

			ImGui::TableSetColumnIndex(2);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnAcc Min U", &m_vTurnAccMin[1], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 2, m_fTurnAcc);
				}*/
				
			}

			ImGui::TableSetColumnIndex(3);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnAcc Min L", &m_vTurnAccMin[2], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 2, m_fTurnAcc);
				}*/
				
			}

			ImGui::TableSetColumnIndex(4);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Deg/Sec");
		}

		ImGui::TableNextRow();

		{
			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Turn(Acc) Max");

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnAcc Max R", &m_vTurnAccMax[0], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 2, m_fTurnAcc);
				}*/

			}

			ImGui::TableSetColumnIndex(2);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnAcc Max U", &m_vTurnAccMax[1], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 2, m_fTurnAcc);
				}*/

			}

			ImGui::TableSetColumnIndex(3);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnAcc Max L", &m_vTurnAccMax[2], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);

				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 2, m_fTurnAcc);
				}*/

			}

			ImGui::TableSetColumnIndex(4);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Deg/Sec");
		}

		ImGui::EndTable();
	}
}

void CWindowParticle::Properties_Color()
{
	ImGui::Dummy(ImVec2(0.f, 3.f));

	if (ImGui::Checkbox("Use Color", &m_bUseColor))
	{
		if (m_pTestParticle)
			m_pTestParticle->SetColorInfo(m_bUseColor, m_iColorSetCnt, m_fColorTime, m_vColorMin, m_vColorMax, m_iColorEasing);

	}

	if (!m_bUseColor)
		return;

	ImGui::Dummy(ImVec2(0.f, 3.f));

	for (int iCnt = 0; iCnt < m_iColorSetCnt; ++iCnt)
	{
		ImGui::Separator();
		m_strColorTimeLabel[iCnt] = "##ColorTime" + to_string(iCnt);
		m_strColorEaseTypeLabel[iCnt] = "##ColorEasingType" + to_string(iCnt);
		m_strColorMinLabel[iCnt] = "##ColorMin" + to_string(iCnt);
		m_strColorMaxLabel[iCnt] = "##ColorMax" + to_string(iCnt);

		ImGui::Text("Time"); ImGui::SameLine();
		ImGui::Dummy(ImVec2(27.f, 0.f)); ImGui::SameLine();
		ImGui::SetNextItemWidth(50.f);
		if (ImGui::InputFloat(m_strColorTimeLabel[iCnt].c_str(), &m_fColorTime[iCnt], 0.f, 0.f, "%.2f", ImGuiInputTextFlags_CharsNoBlank))
		{
			// here
			if (m_pTestParticle)
				m_pTestParticle->SetColorInfo(m_bUseColor, m_iColorSetCnt, m_fColorTime, m_vColorMin, m_vColorMax, m_iColorEasing);

			/*if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetColorInfo(m_bUseColor, m_iColorSplitArea, _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]),
					m_iColorSetCnt, m_fColorTime, m_iColorEase, m_vColor);*/
			/*if (m_pTestModel)
				m_pTestModel->SetColorChangeTime(iCnt, m_fColorTime[iCnt]);*/
		}

		ImGui::Text("Easing"); ImGui::SameLine();
		ImGui::Dummy(ImVec2(13.f, 0.f)); ImGui::SameLine();
		ImGui::SetNextItemWidth(161.f);
		const char* strEasingTypes[EASE_END] = {};
		memcpy(strEasingTypes, GAMEINSTANCE->GetEaseTypeKeys(), sizeof(strEasingTypes));
		const char* combo_preview_value = strEasingTypes[m_iColorEasing[iCnt]];  // Pass in the preview value visible before opening the combo (it could be anything)
		if (ImGui::BeginCombo(m_strColorEaseTypeLabel[iCnt].c_str(), combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
		{
			for (int n = 0; n < IM_ARRAYSIZE(strEasingTypes); n++)
			{
				const bool is_selected = (m_iColorEasing[iCnt] == n);
				if (ImGui::Selectable(strEasingTypes[n], is_selected))
				{
					m_iColorEasing[iCnt] = n;

					// here
					if (m_pTestParticle)
						m_pTestParticle->SetColorInfo(m_bUseColor, m_iColorSetCnt, m_fColorTime, m_vColorMin, m_vColorMax, m_iColorEasing);

					/*if (m_pTestEffectMesh)
						m_pTestEffectMesh->SetColorInfo(m_bUseColor, m_iColorSplitArea, _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]),
							m_iColorSetCnt, m_fColorTime, m_iColorEase, m_vColor);*/
					/*if (m_pTestModel)
						m_pTestModel->SetColorSetEasing(iCnt, m_iColorEase[iCnt]);*/
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		} ImGui::SameLine();

		ImGui::Button("Graphs", ImVec2(60.f, 20.f));
		if (ImGui::BeginItemTooltip())
		{
			ImGui::Text("Easing Graphs");
			ImGui::Image(GAMEINSTANCE->GetSRV("Easing").Get(), ImVec2(700.f, 600.f), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::EndTooltip();
		}

		if (ImGui::BeginTable("##ColorTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
		{
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Min");

			ImGui::TableSetColumnIndex(1);
			// Color
			ImGui::SetNextItemWidth(250.f);
			if (ImGui::ColorEdit4(m_strColorMinLabel[iCnt].c_str(), m_vColorMin[iCnt], ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float))
			{
				// here
				if (m_pTestParticle)
					m_pTestParticle->SetColorInfo(m_bUseColor, m_iColorSetCnt, m_fColorTime, m_vColorMin, m_vColorMax, m_iColorEasing);

				/*if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetColorInfo(m_bUseColor, m_iColorSplitArea, _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]),
						m_iColorSetCnt, m_fColorTime, m_iColorEase, m_vColor);*/
			}

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Max");

			ImGui::TableSetColumnIndex(1);
			// Color
			ImGui::SetNextItemWidth(250.f);
			if (ImGui::ColorEdit4(m_strColorMaxLabel[iCnt].c_str(), m_vColorMax[iCnt], ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float))
			{
				// here
				if (m_pTestParticle)
					m_pTestParticle->SetColorInfo(m_bUseColor, m_iColorSetCnt, m_fColorTime, m_vColorMin, m_vColorMax, m_iColorEasing);

				/*if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetColorInfo(m_bUseColor, m_iColorSplitArea, _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]),
						m_iColorSetCnt, m_fColorTime, m_iColorEase, m_vColor);*/
			}
			if (m_pTestParticle)
				m_pTestParticle->SetColorInfo(m_bUseColor, m_iColorSetCnt, m_fColorTime, m_vColorMin, m_vColorMax, m_iColorEasing);

			/*if (m_pTestModel)
				m_pTestModel->SetColor(iCnt, row, m_vColor[iCnt][row]);*/

			ImGui::EndTable();
		}
		ImGui::Separator();
	}

	// Add Set, Delete Set 버튼
	if (ImGui::Button("Add Set"))
	{
		// 최대 컬러 개수는 4개
		if (m_iColorSetCnt <= 3)
			m_iColorSetCnt++;
		if (m_pTestParticle)
			m_pTestParticle->SetColorInfo(m_bUseColor, m_iColorSetCnt, m_fColorTime, m_vColorMin, m_vColorMax, m_iColorEasing);
		/*if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetColorInfo(m_bColorFollow, m_iColorFollowType, m_strColorFollowBoneKey, m_iColorCnt, m_fColorTime, m_fColor, m_iColorEasing);*/

	} ImGui::SameLine();

	if (ImGui::Button("Delete Set"))
	{
		// 최소 컬러 개수는 1개
		if (m_iColorSetCnt >= 2)
			m_iColorSetCnt--;
		if (m_pTestParticle)
			m_pTestParticle->SetColorInfo(m_bUseColor, m_iColorSetCnt, m_fColorTime, m_vColorMin, m_vColorMax, m_iColorEasing);
		/*if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetColorInfo(m_bColorFollow, m_iColorFollowType, m_strColorFollowBoneKey, m_iColorCnt, m_fColorTime, m_fColor, m_iColorEasing);*/
	}
}

void CWindowParticle::Properties_Shader()
{
	ImGui::Dummy(ImVec2(0.f, 2.f));
	ImGui::Separator();

	if (ImGui::Checkbox("Soft Effect", &m_bUseSoftEffect))
	{
		if (m_bUseSoftEffect)
			m_iRenderGroup = m_iSoftEffectType == 0 ? CRenderer::RENDER_GLOW : CRenderer::RENDER_BLUR;

		else
			m_iRenderGroup = CRenderer::RENDER_BLEND;

		if (m_pTestParticle)
			m_pTestParticle->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup);
	}

	if (m_bUseSoftEffect)
	{
		int iPrevSoftEffectType = m_iSoftEffectType;
		ImGui::RadioButton("Glow", &m_iSoftEffectType, 0); ImGui::SameLine();
		ImGui::RadioButton("Blur", &m_iSoftEffectType, 1);

		ImGui::Dummy(ImVec2(0.f, 3.f));

		if (m_iSoftEffectType != iPrevSoftEffectType)
		{
			m_iRenderGroup = m_iSoftEffectType == 0 ? CRenderer::RENDER_GLOW : CRenderer::RENDER_BLUR;

			if (m_pTestParticle)
				m_pTestParticle->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup);
		}

		if (0 == m_iSoftEffectType)
		{
			ImGui::Dummy(ImVec2(2.f, 0.f)); ImGui::SameLine();

			if (ImGui::Checkbox("Use Color", &m_bUseGlowColor))
			{
				if (m_pTestParticle)
					m_pTestParticle->SetShaderInfo(m_bUseGlowColor, m_fGlowColor);
			}

			if (m_bUseGlowColor)
			{
				if (ImGui::ColorEdit4("##GlowColor", m_fGlowColor, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float))
				{
					if (m_pTestParticle)
						m_pTestParticle->SetShaderInfo(m_bUseGlowColor, m_fGlowColor);
				}
			}
		}
	}
}

shared_ptr<CParticleSystem> CWindowParticle::CreateEffect()
{
	// Default
	m_CreateParticle.iShaderType = (ESHADER_TYPE)m_iShaderType;
	m_CreateParticle.iShaderPass = m_iShaderPass;
	m_CreateParticle.iRenderGroup = m_iRenderGroup;

	// Main
	{
		m_CreateParticle.iMaxParticles = m_iNumInstance;
		m_CreateParticle.StartTrackPos = (_double)m_fStartTrackPos;
		m_CreateParticle.fDuration = m_fDuration;
		m_CreateParticle.vLifeTime = _float2(m_fLifeTime[0], m_fLifeTime[1]);
		m_CreateParticle.bLoop = m_bLoop;
		m_CreateParticle.iBillBoardType = m_iBllBoardType;
		m_CreateParticle.bUseDiffuse = true;
		m_CreateParticle.strTexDiffuseKey = m_strDiffuseTexTag;
		m_CreateParticle.iSpawnType = m_iSpawnType;
		m_CreateParticle.fSpawnNumPerSec = (_float)m_iSpawnNumPerSec;
	}

	// Pos
	{
		m_CreateParticle.iPosType = m_iPosType;
		m_CreateParticle.bPosFollow = m_bPosFollow;
		m_CreateParticle.strPosFollowBoneKey = m_strPosFollowBoneKey;
		m_CreateParticle.bUsePosKeyFrame = m_bUsePosKeyFrame;

		if (!m_bUsePosKeyFrame)
		{
			m_CreateParticle.vPosMin = _float3(m_vPosMin[0][0], m_vPosMin[0][1], m_vPosMin[0][2]);
			m_CreateParticle.vPosMax = _float3(m_vPosMax[0][0], m_vPosMax[0][1], m_vPosMax[0][2]);
		}
	}

	// GlowColor
	{
		m_CreateParticle.bUseGlowColor = m_bUseGlowColor;
		m_CreateParticle.vGlowColor = _float4(m_fGlowColor[0], m_fGlowColor[1], m_fGlowColor[2], m_fGlowColor[3]);
	}

	// Haze
	{
		m_CreateParticle.bUseHazeEffect = m_bUseHazeEffect;
		m_CreateParticle.vHazeSpeedMin = _float3(m_vHazeSpeedMin[0], m_vHazeSpeedMin[1], m_vHazeSpeedMin[2]);
		m_CreateParticle.vHazeSpeedMax = _float3(m_vHazeSpeedMax[0], m_vHazeSpeedMax[1], m_vHazeSpeedMax[2]);
	}

	// Pivot
	{
		m_CreateParticle.bUsePivot = m_bUsePivot;
		m_CreateParticle.vPivotMin = _float3(m_vPivotMin[0], m_vPivotMin[1], m_vPivotMin[2]);
		m_CreateParticle.vPivotMax = _float3(m_vPivotMax[0], m_vPivotMax[1], m_vPivotMax[2]);
	}

	// Speed
	{
		m_CreateParticle.bSpeedMode = m_iSpeedType;

		if (!m_CreateParticle.bSpeedMode)
		{
			// Speed KeyFrames 정보 저장
			CParticleSystem::SPEED_KEYFRAME_PARTICLE     SpeedKeyFrames;
			m_CreateParticle.KeyFrames.SpeedKeyFrames.clear();

			for (int i = 0; i < m_iSpeedSetCnt; ++i)
			{
				SpeedKeyFrames.fTime = m_fSpeedTime[i];
				SpeedKeyFrames.eEaseType = m_iSpeedEasing[i];
				SpeedKeyFrames.vSpeedMin = _float3(m_fSpeedMin[i][0], m_fSpeedMin[i][1], m_fSpeedMin[i][2]);
				SpeedKeyFrames.vSpeedMax = _float3(m_fSpeedMax[i][0], m_fSpeedMax[i][1], m_fSpeedMax[i][2]);

				m_CreateParticle.KeyFrames.SpeedKeyFrames.push_back(SpeedKeyFrames);
			}
		}

		// Acc Mode
		else
		{

			m_CreateParticle.vStartSpeedMin = _float3(m_vStartSpeedMin[0], m_vStartSpeedMin[1], m_vStartSpeedMin[2]);
			m_CreateParticle.vStartSpeedMax = _float3(m_vStartSpeedMax[0], m_vStartSpeedMax[1], m_vStartSpeedMax[2]);

			m_CreateParticle.vSpeedAccMin = _float3(m_vSpeedAccMin[0], m_vSpeedAccMin[1], m_vSpeedAccMin[2]);
			m_CreateParticle.vSpeedAccMax = _float3(m_vSpeedAccMax[0], m_vSpeedAccMax[1], m_vSpeedAccMax[2]);
		}

		m_CreateParticle.bRelativeOwner = m_bRelativeOwner;
	}

	// Pos
	if (m_bUsePosKeyFrame)
	{
		// Pos KeyFrames 정보 저장
		CParticleSystem::POSITION_KEYFRAME_PARTICLE     PosKeyFrames;
		m_CreateParticle.KeyFrames.PosKeyFrames.clear();

		for (int i = 0; i < m_iPosSetCnt; ++i)
		{
			PosKeyFrames.fTime = m_fPosTime[i];
			PosKeyFrames.eEaseType = m_iPosEasing[i];
			PosKeyFrames.vPosMin = _float3(m_vPosMin[i][0], m_vPosMin[i][1], m_vPosMin[i][2]);
			PosKeyFrames.vPosMax = _float3(m_vPosMax[i][0], m_vPosMax[i][1], m_vPosMax[i][2]);

			m_CreateParticle.KeyFrames.PosKeyFrames.push_back(PosKeyFrames);
		}
	}

	// Scale
	{
		// Scale KeyFrames 정보 저장
		CParticleSystem::SCALE_KEYFRAME_PARTICLE     ScaleKeyFrames;
		m_CreateParticle.KeyFrames.ScaleKeyFrames.clear();

		for (int i = 0; i < m_iScaleSetCnt; ++i)
		{
			ScaleKeyFrames.fTime = m_fScaleTime[i];
			ScaleKeyFrames.eEaseType = m_iScaleEasing[i];
			ScaleKeyFrames.vScaleMin = _float3(m_fScaleMin[i][0], m_fScaleMin[i][1], m_fScaleMin[i][2]);
			ScaleKeyFrames.vScaleMax = _float3(m_fScaleMax[i][0], m_fScaleMax[i][1], m_fScaleMax[i][2]);

			m_CreateParticle.KeyFrames.ScaleKeyFrames.push_back(ScaleKeyFrames);
		}
	}

	// Rotation
	{
		m_CreateParticle.vRotationMin = _float3(m_vRotationMin[0], m_vRotationMin[1], m_vRotationMin[2]);
		m_CreateParticle.vRotationMax = _float3(m_vRotationMax[0], m_vRotationMax[1], m_vRotationMax[2]);

		m_CreateParticle.vTurnVelMin = _float3(m_vTurnVelMin[0], m_vTurnVelMin[1], m_vTurnVelMin[2]);
		m_CreateParticle.vTurnVelMax = _float3(m_vTurnVelMax[0], m_vTurnVelMax[1], m_vTurnVelMax[2]);

		m_CreateParticle.vTurnAccMin = _float3(m_vTurnAccMin[0], m_vTurnAccMin[1], m_vTurnAccMin[2]);
		m_CreateParticle.vTurnAccMax = _float3(m_vTurnAccMax[0], m_vTurnAccMax[1], m_vTurnAccMax[2]);
	}

	// Color
	{
		m_CreateParticle.bUseColor = m_bUseColor;

		// Scale KeyFrames 정보 저장
		CParticleSystem::COLOR_KEYFRAME_PARTICLE     ColorKeyFrames;
		m_CreateParticle.KeyFrames.ColorKeyFrames.clear();

		for (int i = 0; i < m_iColorSetCnt; ++i)
		{
			ColorKeyFrames.fTime = m_fColorTime[i];
			ColorKeyFrames.eEaseType = m_iColorEasing[i];
			ColorKeyFrames.vColorMin = _float4(m_vColorMin[i][0], m_vColorMin[i][1], m_vColorMin[i][2], m_vColorMin[i][3]);
			ColorKeyFrames.vColorMax = _float4(m_vColorMax[i][0], m_vColorMax[i][1], m_vColorMax[i][2], m_vColorMax[i][3]);

			m_CreateParticle.KeyFrames.ColorKeyFrames.push_back(ColorKeyFrames);
		}
	}

	shared_ptr<CParticleSystem>     pEffect = CParticleSystem::Create(&m_CreateParticle);

	/*if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_BackGround"), pEffect)))
		return nullptr;*/

	if (m_pTestParticle)
		m_pTestParticle->SetActive(false);

	return pEffect;
}

void CWindowParticle::CreateTestEffect(CParticleSystem::PARTICLE_SYSTEM* _pEffectDesc, shared_ptr<CGameObject> _pOwner)
{
	// 한 번씩 세팅은 해주기
	//this->Tick(0.016f);

	m_pFollowObj = dynamic_pointer_cast<CAnimModel>(_pOwner);
	m_pFollowObjModel = dynamic_pointer_cast<CModel>(_pOwner->GetComponent(L"Com_Model"));

	m_pFollowObjBones = m_pFollowObjModel->GetBones();
	m_vecFollowBoneNames.clear();

	for (auto pBones : m_pFollowObjBones)
	{
		const _char* pName = pBones->GetBoneName();
		m_vecFollowBoneNames.push_back(pName);
	}
	
	m_iShaderType = ESHADER_TYPE::ST_PARTICLE;
	m_iShaderPass = _pEffectDesc->iShaderPass;
	m_iRenderGroup = CRenderer::RENDER_BLEND;

	m_iNumInstance = _pEffectDesc->iMaxParticles;
	m_fStartTrackPos = _pEffectDesc->StartTrackPos;
	m_fDuration = _pEffectDesc->fDuration;
	m_fLifeTime[0] = { _pEffectDesc->vLifeTime.x };
	m_fLifeTime[1] = { _pEffectDesc->vLifeTime.y };

	m_bLoop = _pEffectDesc->bLoop;
	m_iBllBoardType = _pEffectDesc->iBillBoardType; // 0 : 기본, 1 : 빌보드, 2 : 방향성
	m_strDiffuseTexTag = _pEffectDesc->strTexDiffuseKey;
	m_iSpawnType = _pEffectDesc->iSpawnType;	// 0 : Continuous, 1 : Burst
	m_iSpawnNumPerSec = _pEffectDesc->fSpawnNumPerSec;


	m_iPosType = _pEffectDesc->iPosType;		// 0 : World, 1 : Object, 2 : Bone
	m_bPosFollow = _pEffectDesc->bPosFollow;
	m_strPosFollowBoneKey = _pEffectDesc->strPosFollowBoneKey;
	m_bUsePosKeyFrame = _pEffectDesc->bUsePosKeyFrame;

	if (m_bUsePosKeyFrame)
	{
		m_iPosSetCnt = 1;

		for (int i = 0; i < m_iPosSetCnt; ++i)
		{
			m_fPosTime[i] = _pEffectDesc->KeyFrames.PosKeyFrames[i].fTime;
			m_iPosEasing[i] = _pEffectDesc->KeyFrames.PosKeyFrames[i].eEaseType;

			m_vPosMin[i][0] = _pEffectDesc->KeyFrames.PosKeyFrames[i].vPosMin.x;
			m_vPosMin[i][1] = _pEffectDesc->KeyFrames.PosKeyFrames[i].vPosMin.y;
			m_vPosMin[i][2] = _pEffectDesc->KeyFrames.PosKeyFrames[i].vPosMin.z;

			m_vPosMax[i][0] = _pEffectDesc->KeyFrames.PosKeyFrames[i].vPosMax.x;
			m_vPosMax[i][1] = _pEffectDesc->KeyFrames.PosKeyFrames[i].vPosMax.y;
			m_vPosMax[i][2] = _pEffectDesc->KeyFrames.PosKeyFrames[i].vPosMax.z;
		}
	}
	else
	{
		m_vPosMin[0][0] = _pEffectDesc->vPosMin.x;
		m_vPosMin[0][1] = _pEffectDesc->vPosMin.y;
		m_vPosMin[0][2] = _pEffectDesc->vPosMin.z;

		m_vPosMax[0][0] = _pEffectDesc->vPosMax.x;
		m_vPosMax[0][1] = _pEffectDesc->vPosMax.y;
		m_vPosMax[0][2] = _pEffectDesc->vPosMax.z;
	}


	string strKey = m_strPosFollowBoneKey;

	auto it = std::find_if(m_vecFollowBoneNames.begin(), m_vecFollowBoneNames.end(), [strKey](string element) {
		return element == strKey;
		});

	if (it != m_vecFollowBoneNames.end()) {
		m_pPosFollowBoneIdx = std::distance(m_vecFollowBoneNames.begin(), it);
	}
	else {
		m_pPosFollowBoneIdx = -1; // 일치하는 요소를 찾지 못한 경우 -1 반환
	}

	m_bUsePivot = _pEffectDesc->bUsePivot;
	m_vPivotMin[0] = _pEffectDesc->vPivotMin.x;
	m_vPivotMin[1] = _pEffectDesc->vPivotMin.y;
	m_vPivotMin[2] = _pEffectDesc->vPivotMin.z;

	m_vPivotMax[0] = _pEffectDesc->vPivotMax.x;
	m_vPivotMax[1] = _pEffectDesc->vPivotMax.y;
	m_vPivotMax[2] = _pEffectDesc->vPivotMax.z;

	m_bRelativeOwner = _pEffectDesc->bRelativeOwner;

	m_bUseHazeEffect = _pEffectDesc->bUseHazeEffect;
	m_vHazeSpeedMin[0] = _pEffectDesc->vHazeSpeedMin.x;
	m_vHazeSpeedMin[1] = _pEffectDesc->vHazeSpeedMin.y;
	m_vHazeSpeedMin[2] = _pEffectDesc->vHazeSpeedMin.z;

	m_vHazeSpeedMax[0] = _pEffectDesc->vHazeSpeedMax.x;
	m_vHazeSpeedMax[1] = _pEffectDesc->vHazeSpeedMax.y;
	m_vHazeSpeedMax[2] = _pEffectDesc->vHazeSpeedMax.z;

	m_iSpeedType = _pEffectDesc->bSpeedMode;		// 0 : TimeTable, 1 : Acceleration

	m_iSpeedSetCnt = _pEffectDesc->KeyFrames.SpeedKeyFrames.size();

	for (int i = 0; i < m_iSpeedSetCnt; ++i)
	{
		m_fSpeedTime[i] = _pEffectDesc->KeyFrames.SpeedKeyFrames[i].fTime;
		m_iSpeedEasing[i] = _pEffectDesc->KeyFrames.SpeedKeyFrames[i].eEaseType;

		m_fSpeedMin[i][0] = _pEffectDesc->KeyFrames.SpeedKeyFrames[i].vSpeedMin.x;
		m_fSpeedMin[i][1] = _pEffectDesc->KeyFrames.SpeedKeyFrames[i].vSpeedMin.y;
		m_fSpeedMin[i][2] = _pEffectDesc->KeyFrames.SpeedKeyFrames[i].vSpeedMin.z;

		m_fSpeedMax[i][0] = _pEffectDesc->KeyFrames.SpeedKeyFrames[i].vSpeedMax.x;
		m_fSpeedMax[i][1] = _pEffectDesc->KeyFrames.SpeedKeyFrames[i].vSpeedMax.y;
		m_fSpeedMax[i][2] = _pEffectDesc->KeyFrames.SpeedKeyFrames[i].vSpeedMax.z;
	}

	m_vStartSpeedMin[0] = _pEffectDesc->vStartSpeedMin.x;
	m_vStartSpeedMin[1] = _pEffectDesc->vStartSpeedMin.y;
	m_vStartSpeedMin[2] = _pEffectDesc->vStartSpeedMin.z;

	m_vStartSpeedMax[0] = _pEffectDesc->vStartSpeedMax.x;
	m_vStartSpeedMax[1] = _pEffectDesc->vStartSpeedMax.y;
	m_vStartSpeedMax[2] = _pEffectDesc->vStartSpeedMax.z;

	m_vSpeedAccMin[0] = _pEffectDesc->vSpeedAccMin.x;
	m_vSpeedAccMin[1] = _pEffectDesc->vSpeedAccMin.y;
	m_vSpeedAccMin[2] = _pEffectDesc->vSpeedAccMin.z;

	m_vSpeedAccMax[0] = _pEffectDesc->vSpeedAccMax.x;
	m_vSpeedAccMax[1] = _pEffectDesc->vSpeedAccMax.y;
	m_vSpeedAccMax[2] = _pEffectDesc->vSpeedAccMax.z;

	m_iScaleSetCnt = _pEffectDesc->KeyFrames.ScaleKeyFrames.size();

	for (int i = 0; i < m_iScaleSetCnt; ++i)
	{
		m_fScaleTime[i] = _pEffectDesc->KeyFrames.ScaleKeyFrames[i].fTime;
		m_iScaleEasing[i] = _pEffectDesc->KeyFrames.ScaleKeyFrames[i].eEaseType;

		m_fScaleMin[i][0] = _pEffectDesc->KeyFrames.ScaleKeyFrames[i].vScaleMin.x;
		m_fScaleMin[i][1] = _pEffectDesc->KeyFrames.ScaleKeyFrames[i].vScaleMin.y;
		m_fScaleMin[i][2] = _pEffectDesc->KeyFrames.ScaleKeyFrames[i].vScaleMin.z;

		m_fScaleMax[i][0] = _pEffectDesc->KeyFrames.ScaleKeyFrames[i].vScaleMax.x;
		m_fScaleMax[i][1] = _pEffectDesc->KeyFrames.ScaleKeyFrames[i].vScaleMax.y;
		m_fScaleMax[i][2] = _pEffectDesc->KeyFrames.ScaleKeyFrames[i].vScaleMax.z;
	}

	m_vRotationMin[0] = _pEffectDesc->vRotationMin.x;
	m_vRotationMin[1] = _pEffectDesc->vRotationMin.y;
	m_vRotationMin[2] = _pEffectDesc->vRotationMin.z;

	m_vRotationMax[0] = _pEffectDesc->vRotationMax.x;
	m_vRotationMax[1] = _pEffectDesc->vRotationMax.y;
	m_vRotationMax[2] = _pEffectDesc->vRotationMax.z;

	m_vTurnVelMin[0] = _pEffectDesc->vTurnVelMin.x;
	m_vTurnVelMin[1] = _pEffectDesc->vTurnVelMin.y;
	m_vTurnVelMin[2] = _pEffectDesc->vTurnVelMin.z;

	m_vTurnVelMax[0] = _pEffectDesc->vTurnVelMax.x;
	m_vTurnVelMax[1] = _pEffectDesc->vTurnVelMax.y;
	m_vTurnVelMax[2] = _pEffectDesc->vTurnVelMax.z;

	m_vTurnAccMin[0] = _pEffectDesc->vTurnAccMin.x;
	m_vTurnAccMin[1] = _pEffectDesc->vTurnAccMin.y;
	m_vTurnAccMin[2] = _pEffectDesc->vTurnAccMin.z;

	m_vTurnAccMax[0] = _pEffectDesc->vTurnAccMax.x;
	m_vTurnAccMax[1] = _pEffectDesc->vTurnAccMax.y;
	m_vTurnAccMax[2] = _pEffectDesc->vTurnAccMax.z;

	m_bUseColor = _pEffectDesc->bUseColor;

	_pEffectDesc->KeyFrames.ColorKeyFrames;

	m_iColorSetCnt = _pEffectDesc->KeyFrames.ColorKeyFrames.size();

	for (int i = 0; i < m_iColorSetCnt; ++i)
	{
		m_fColorTime[i] = _pEffectDesc->KeyFrames.ColorKeyFrames[i].fTime;
		m_iColorEasing[i] = _pEffectDesc->KeyFrames.ColorKeyFrames[i].eEaseType;

		m_vColorMin[i][0] = _pEffectDesc->KeyFrames.ColorKeyFrames[i].vColorMin.x;
		m_vColorMin[i][1] = _pEffectDesc->KeyFrames.ColorKeyFrames[i].vColorMin.y;
		m_vColorMin[i][2] = _pEffectDesc->KeyFrames.ColorKeyFrames[i].vColorMin.z;
		m_vColorMin[i][3] = _pEffectDesc->KeyFrames.ColorKeyFrames[i].vColorMin.w;

		m_vColorMax[i][0] = _pEffectDesc->KeyFrames.ColorKeyFrames[i].vColorMax.x;
		m_vColorMax[i][1] = _pEffectDesc->KeyFrames.ColorKeyFrames[i].vColorMax.y;
		m_vColorMax[i][2] = _pEffectDesc->KeyFrames.ColorKeyFrames[i].vColorMax.z;
		m_vColorMax[i][3] = _pEffectDesc->KeyFrames.ColorKeyFrames[i].vColorMax.w;
	}

	m_bUseSoftEffect = (_pEffectDesc->iRenderGroup == CRenderer::RENDER_BLUR || _pEffectDesc->iRenderGroup == CRenderer::RENDER_GLOW);

	m_iSoftEffectType = _pEffectDesc->iRenderGroup == CRenderer::RENDER_BLUR ? 1 : 0;	// 0 : Glow, 1 : Blur
	m_bUseGlowColor = _pEffectDesc->bUseGlowColor;
	m_fGlowColor[0] = _pEffectDesc->vGlowColor.x;
	m_fGlowColor[1] = _pEffectDesc->vGlowColor.y;
	m_fGlowColor[2] = _pEffectDesc->vGlowColor.z;
	m_fGlowColor[3] = _pEffectDesc->vGlowColor.w;

	m_bUseBlur = (_pEffectDesc->iRenderGroup == CRenderer::RENDER_BLUR);

	GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_TestEffect"));
	m_pTestParticle = nullptr;

	m_pTestParticle = CTestParticle::Create(m_iNumInstance, m_strDiffuseTexTag);

	if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_TestEffect"), m_pTestParticle)))
		return;

	m_pTestParticle->SetOwner(_pOwner);

	m_pTestParticle->SetDefaultInfo(ST_PARTICLE, m_iShaderPass, m_iRenderGroup);
	m_pTestParticle->SetMainInfo(m_iNumInstance, m_fDuration, m_fStartTrackPos, _float2(m_fLifeTime[0], m_fLifeTime[1]), m_bLoop, m_iBllBoardType, m_strDiffuseTexTag, m_iSpawnType, m_iSpawnNumPerSec);
	m_pTestParticle->SetPosInfo(m_iPosType, m_bPosFollow, m_strPosFollowBoneKey, m_iPosSetCnt, m_fPosTime, m_vPosMin, m_vPosMax, m_iPosEasing, m_bUsePosKeyFrame);

	if (0 == m_iSpeedType)
		m_pTestParticle->SetSpeedInfo(m_iSpeedType, m_iSpeedSetCnt, m_fSpeedTime, m_fSpeedMin, m_fSpeedMax, m_iSpeedEasing);
	
	else
		m_pTestParticle->SetSpeedInfo(m_iSpeedType, _float3(m_vStartSpeedMin[0], m_vStartSpeedMin[1], m_vStartSpeedMin[2]), _float3(m_vStartSpeedMax[0], m_vStartSpeedMax[1], m_vStartSpeedMax[2]), _float3(m_vSpeedAccMin[0], m_vSpeedAccMin[1], m_vSpeedAccMin[2]), _float3(m_vSpeedAccMax[0], m_vSpeedAccMax[1], m_vSpeedAccMax[2]));

	m_pTestParticle->SetScaleInfo(m_iScaleSetCnt, m_fScaleTime, m_fScaleMin, m_fScaleMax, m_iScaleEasing);
	m_pTestParticle->SetRotationInfo(m_vRotationMin, m_vRotationMax, m_vTurnVelMin, m_vTurnVelMax, m_vTurnAccMin, m_vTurnAccMax);
	m_pTestParticle->SetColorInfo(m_bUseColor, m_iColorSetCnt, m_fColorTime, m_vColorMin, m_vColorMax, m_iColorEasing);
	m_pTestParticle->SetShaderInfo(m_bUseGlowColor, m_fGlowColor);
	m_pTestParticle->SetHazeInfo(m_bUseHazeEffect, m_vHazeSpeedMin, m_vHazeSpeedMax, m_bRelativeOwner);
	m_pTestParticle->SetPivotInfo(m_bUsePivot, m_vPivotMin, m_vPivotMax);
}

void CWindowParticle::KeyInput()
{
}

HRESULT CWindowParticle::LoadResourceNames(const wstring& _strResourcePath, TEX_TYPE _eTexType)
{
	filesystem::path basePath(_strResourcePath);

	for (const auto& entry : filesystem::directory_iterator(basePath)) {
		/*if. File : .png 라면 texture Create*/
		if (entry.is_regular_file()) {
			if (".png" == entry.path().extension()) {
				m_TextureList[_eTexType][m_iTextureIdx[_eTexType]] = entry.path().filename().stem().string();
				m_iTextureIdx[_eTexType]++;
			}
		}
		/*if. Directory : 계속 다음 폴더를 순회*/
		else if (entry.is_directory()) {

			const wstring& strDirName = entry.path().filename();

			if (strDirName == L"." || strDirName == L"..")
				continue;

			else if (L"Particle" == strDirName)
				LoadResourceNames(entry.path(), TEX_DIFFUSE);

			else continue;
		}
	}

	return S_OK;
}