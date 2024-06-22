#include "WindowMesh.h"
#include "GameInstance.h"

#include "TestEffectMesh.h"
#include "TestModel.h"
#include "Texture.h"

#include "Easing.h"

#include "ImGuiMgr.h"
#include "WindowModel.h"
#include "WindowPlay.h"

#include "AnimModel.h"
#include "Model.h"
#include "Bone.h"

#include "AnimEffect.h"

#include <filesystem>
#include <iostream>

CWindowMesh::CWindowMesh()
{
}

HRESULT CWindowMesh::Initialize()
{
	m_pDevice = GAMEINSTANCE->GetDeviceInfo();
	m_pContext = GAMEINSTANCE->GetDeviceContextInfo();

	wstring strEffectPath = TEXT("..\\..\\Tool_Effect\\Bin\\Resources\\Level_EffectTool");

	m_TextureList[TEX_DIFFUSE].resize(2000);
	//m_TextureList[TEX_MASK].resize(300);
	m_TextureList[TEX_NOISE].resize(300);

	m_pAnimEffect = CAnimEffect::Create();
	GAMEINSTANCE->AddObject(LEVEL_LOGO, L"Layer_AnimEffect", m_pAnimEffect);

	/*m_TextureList[TEX_DIFFUSE].reserve(100);
	m_TextureList[TEX_MASK].reserve(100);
	m_TextureList[TEX_NOISE].reserve(100);*/

	LoadResourceNames(strEffectPath, MODEL_END, TEX_END);

	return S_OK;
}

void CWindowMesh::Tick(_float _fTimeDelta)
{
	KeyInput();

	if (ImGui::TreeNode("0. Default"))
	{
		Properties_Default();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("1. Mesh"))
	{
		Properties_Mesh();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("2. Diffuse"))
	{
		Properties_Diffuse();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("3. Mask"))
	{
		Properties_Mask();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("4. Noise"))
	{
		Properties_Noise();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("5. Color"))
	{
		Properties_Color();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("6. Scale"))
	{
		Properties_Scale();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("7. Rotation"))
	{
		Properties_Rotation();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("8. Translation"))
	{
		Properties_Translation();

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("9. Shader"))
	{
		Properties_Shader();

		ImGui::TreePop();
	}
}

void CWindowMesh::Properties_Default()
{
	ImGui::Dummy(ImVec2(0.f, 5.f));

	//ImGui::Text("Shader Type  "); ImGui::SameLine();
	//ImGui::SetNextItemWidth(150.f);
	//const char* strShader[] = { "POSTEX", "NORTEX", "NONANIM", "ANIM", "EFFECT_MESH", "EFFECT_TEX", "UI", "DEFERRED" };
	//static int iShaderIdx = 0; // Here we store our selection data as an index.
	//const char* iShaderIdx_value = strShader[iShaderIdx];  // Pass in the preview value visible before opening the combo (it could be anything)
	//if (ImGui::BeginCombo("##ShaderType", iShaderIdx_value, ImGuiComboFlags_PopupAlignLeft))
	//{
	//	for (int n = 0; n < IM_ARRAYSIZE(strShader); n++)
	//	{
	//		const bool is_selected = (iShaderIdx == n);
	//		if (ImGui::Selectable(strShader[n], is_selected))
	//		{
	//			iShaderIdx = n;
	//			m_iShaderType = iShaderIdx;

	//			if (m_pTestEffectMesh)
	//				m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
	//		}

	//		// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
	//		if (is_selected)
	//			ImGui::SetItemDefaultFocus();
	//	}
	//	ImGui::EndCombo();
	//}

	//ImGui::Text("Shader Pass  "); ImGui::SameLine();
	//ImGui::SetNextItemWidth(150.f);
	//if (ImGui::InputInt("##ShaderPass", &m_iShaderPass, 0, 0))
	//{
	//	if (m_pTestEffectMesh)
	//		m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
	//}

	//ImGui::Text("Render Group "); ImGui::SameLine();
	//ImGui::SetNextItemWidth(150.f);
	//const char* strRenderGroup[] = { "PRIORITY", "NONBLEND", "EDGE", "NONLIGHT", "BLUR", "GLOW", "DISTORTION", "BLEND", "UI" };
	//static int iRenderIdx = 0; // Here we store our selection data as an index.
	//const char* iRenderIdx_value = strRenderGroup[iRenderIdx];  // Pass in the preview value visible before opening the combo (it could be anything)
	//if (ImGui::BeginCombo("##RenderGroup", iRenderIdx_value, ImGuiComboFlags_PopupAlignLeft))
	//{
	//	for (int n = 0; n < IM_ARRAYSIZE(strRenderGroup); n++)
	//	{
	//		const bool is_selected = (iRenderIdx == n);
	//		if (ImGui::Selectable(strRenderGroup[n], is_selected))
	//		{
	//			iRenderIdx = n;
	//			m_iRenderGroup = iRenderIdx;

	//			if (m_pTestEffectMesh)
	//				m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
	//		}

	//		// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
	//		if (is_selected)
	//			ImGui::SetItemDefaultFocus();
	//	}
	//	ImGui::EndCombo();
	//}

	//ImGui::Dummy(ImVec2(0.f, 5.f));

	//ImGui::Separator();

	ImGui::Dummy(ImVec2(0.f, 5.f));
	if (ImGui::InputFloat("Duration", &m_fDuration, 0.f, 0.f, "%.2f"))
	{
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
	}
	if (ImGui::InputFloat("StartTrackPos", &m_StartTrackPos, 0.f, 0.f, "%.2f"))
	{
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
	}

	ImGui::Dummy(ImVec2(0.f, 5.f));

	ImGui::Separator();

	ImGui::Dummy(ImVec2(0.f, 5.f));
	if (ImGui::Checkbox("Use BillBoard", &m_bBillBoard))
	{
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
	}
	if (ImGui::Checkbox("Loop", &m_bLoop))
	{
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
	}

	ImGui::Dummy(ImVec2(0.f, 5.f));
	ImGui::Separator();

	ImGui::Dummy(ImVec2(0.f, 5.f));
}

void CWindowMesh::Properties_Mesh()
{
	// 부모 창의 크기 설정
	//ImGui::SetNextWindowSizeConstraints(ImVec2(-FLT_MIN, -150), ImVec2(FLT_MAX, 150));
	//ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, -FLT_MIN), ImVec2(FLT_MAX, 150));

	ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 8);
	static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

	// 테이블 생성
	//if (ImGui::BeginTable("table_scrolly", 3, flags, outer_size))
	if (ImGui::BeginTable("Mesh List", 1, flags, outer_size))
		//if (ImGui::BeginTable("Mesh List", 1, ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_NoBordersInBody))
	{
		_bool isContinue = true;

		ImGuiTable* currentTable = ImGui::GetCurrentTable();
		if (currentTable != nullptr) {

			if (currentTable->HostSkipItems)
			{
				isContinue = false;
			}
		}

		if (isContinue)
		{

			// 열 설정
			ImGui::TableSetupColumn("Mesh Files", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			// 셀 생성
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			_int iChildIdx = 0;

			ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 0), ImVec2(FLT_MAX, 150));

			_bool bOpenNonAnimBasic = ImGui::TreeNodeEx("NonAnim/Basic", ImGuiTreeNodeFlags_None);
			if (bOpenNonAnimBasic)
			{
				iChildIdx = 0;
				static _int curModelIdx = 0;

				for (iChildIdx = 0; iChildIdx < m_ModelList[MODEL_BASIC].size(); iChildIdx++)
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					const bool isSelected = (curModelIdx == iChildIdx);
					if (ImGui::Selectable(m_ModelList[MODEL_BASIC][iChildIdx].c_str(), isSelected))
					{
						curModelIdx = iChildIdx;

						GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_TestEffect"));

						//m_pTestModel = CTestModel::Create(m_ModelList[MODEL_BASIC][iChildIdx]);
						m_pTestEffectMesh = CTestEffectMesh::Create(m_ModelList[MODEL_BASIC][iChildIdx]);
						m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
						m_pTestEffectMesh->SetOwner(CImGuiMgr::GetInstance()->GetWindowModel()->GetCurModelObj());
						m_strModelKey = m_ModelList[MODEL_BASIC][iChildIdx];

						/*if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_BackGround"), m_pTestModel)))
							return;*/
						if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_TestEffect"), m_pTestEffectMesh)))
							return;

						m_strDiffuseTexTag = "";
						m_strMaskTexTag = "";
						m_strNoiseTexTag = "";
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
						m_iSelectedMeshIdx = iChildIdx;
					}
				}
				ImGui::TreePop();
			}

			ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 0), ImVec2(FLT_MAX, 100));

			_bool bOpenNonAnimUnique = ImGui::TreeNodeEx("NonAnim/Unique", ImGuiTreeNodeFlags_SpanAllColumns);
			if (bOpenNonAnimUnique)
			{
				iChildIdx = 0;
				static _int curModelIdx = 0;

				for (iChildIdx = 0; iChildIdx < m_ModelList[MODEL_UNIQUE].size(); iChildIdx++)
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					const bool isSelected = (curModelIdx == iChildIdx);
					if (ImGui::Selectable(m_ModelList[MODEL_UNIQUE][iChildIdx].c_str(), isSelected))
					{
						curModelIdx = iChildIdx;

						GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_TestEffect"));

						//m_pTestModel = CTestModel::Create(m_ModelList[MODEL_BASIC][iChildIdx]);
						m_pTestEffectMesh = CTestEffectMesh::Create(m_ModelList[MODEL_UNIQUE][iChildIdx]);
						m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
						m_pTestEffectMesh->SetOwner(CImGuiMgr::GetInstance()->GetWindowModel()->GetCurModelObj());
						m_strModelKey = m_ModelList[MODEL_UNIQUE][iChildIdx];

						/*if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_BackGround"), m_pTestModel)))
							return;*/
						if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_TestEffect"), m_pTestEffectMesh)))
							return;

						/*GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_BackGround"));

						m_pTestModel = CTestModel::Create(m_ModelList[MODEL_UNIQUE][iChildIdx]);
						m_strModelKey = m_ModelList[MODEL_UNIQUE][iChildIdx];

						if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_BackGround"), m_pTestModel)))
							return;*/

						m_strDiffuseTexTag = "";
						m_strMaskTexTag = "";
						m_strNoiseTexTag = "";
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
						m_iSelectedMeshIdx = iChildIdx;
					}
				}
				ImGui::TreePop();
			}


			ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 0), ImVec2(FLT_MAX, 150));

			_bool bOpenOtehr = ImGui::TreeNodeEx("Other", ImGuiTreeNodeFlags_None);
			if (bOpenOtehr)
			{
				iChildIdx = 0;
				static _int curModelIdx = 0;

				for (iChildIdx = 0; iChildIdx < m_ModelList[MODEL_OTHER].size(); iChildIdx++)
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					const bool isSelected = (curModelIdx == iChildIdx);
					if (ImGui::Selectable(m_ModelList[MODEL_OTHER][iChildIdx].c_str(), isSelected))
					{
						curModelIdx = iChildIdx;

						GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_TestEffect"));

						//m_pTestModel = CTestModel::Create(m_ModelList[MODEL_BASIC][iChildIdx]);
						m_pTestEffectMesh = CTestEffectMesh::Create(m_ModelList[MODEL_OTHER][iChildIdx]);
						m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
						m_pTestEffectMesh->SetOwner(CImGuiMgr::GetInstance()->GetWindowModel()->GetCurModelObj());
						m_strModelKey = m_ModelList[MODEL_OTHER][iChildIdx];

						/*if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_BackGround"), m_pTestModel)))
							return;*/
						if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_TestEffect"), m_pTestEffectMesh)))
							return;

						m_strDiffuseTexTag = "";
						m_strMaskTexTag = "";
						m_strNoiseTexTag = "";
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
						m_iSelectedMeshIdx = iChildIdx;
					}
				}
				ImGui::TreePop();
			}

			ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 0), ImVec2(FLT_MAX, 150));

			_bool bOpenOther2 = ImGui::TreeNodeEx("Other2", ImGuiTreeNodeFlags_None);
			if (bOpenOther2)
			{
				iChildIdx = 0;
				static _int curModelIdx = 0;

				for (iChildIdx = 0; iChildIdx < m_ModelList[MODEL_OTHER2].size(); iChildIdx++)
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					const bool isSelected = (curModelIdx == iChildIdx);
					if (ImGui::Selectable(m_ModelList[MODEL_OTHER2][iChildIdx].c_str(), isSelected))
					{
						curModelIdx = iChildIdx;

						GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_TestEffect"));

						//m_pTestModel = CTestModel::Create(m_ModelList[MODEL_BASIC][iChildIdx]);
						m_pTestEffectMesh = CTestEffectMesh::Create(m_ModelList[MODEL_OTHER2][iChildIdx]);
						m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
						m_pTestEffectMesh->SetOwner(CImGuiMgr::GetInstance()->GetWindowModel()->GetCurModelObj());
						m_strModelKey = m_ModelList[MODEL_OTHER2][iChildIdx];

						/*if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_BackGround"), m_pTestModel)))
							return;*/
						if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_TestEffect"), m_pTestEffectMesh)))
							return;

						m_strDiffuseTexTag = "";
						m_strMaskTexTag = "";
						m_strNoiseTexTag = "";
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
						m_iSelectedMeshIdx = iChildIdx;
					}
				}
				ImGui::TreePop();
			}

			ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 0), ImVec2(FLT_MAX, 100));

			_bool bOpenAnim = ImGui::TreeNodeEx("Anim", ImGuiTreeNodeFlags_SpanAllColumns);
			if (bOpenAnim)
			{
				iChildIdx = 0;
				static _int curModelIdx = 0;

				for (iChildIdx = 0; iChildIdx < m_ModelList[MODEL_ANIM].size(); iChildIdx++)
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					const bool isSelected = (curModelIdx == iChildIdx);
					if (ImGui::Selectable(m_ModelList[MODEL_ANIM][iChildIdx].c_str(), isSelected))
					{
						curModelIdx = iChildIdx;

						GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_TestEffect"));

						//m_pTestModel = CTestModel::Create(m_ModelList[MODEL_BASIC][iChildIdx]);
						m_pTestEffectMesh = CTestEffectMesh::Create(m_ModelList[MODEL_ANIM][iChildIdx]);
						m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
						m_strModelKey = m_ModelList[MODEL_ANIM][iChildIdx];

						/*if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_BackGround"), m_pTestModel)))
							return;*/
						if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_TestEffect"), m_pTestEffectMesh)))
							return;

						/*GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_BackGround"));

						m_pTestModel = CTestModel::Create(m_ModelList[MODEL_ANIM][iChildIdx]);
						m_strModelKey = m_ModelList[MODEL_ANIM][iChildIdx];

						if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_BackGround"), m_pTestModel)))
							return;*/

						m_strDiffuseTexTag = "";
						m_strMaskTexTag = "";
						m_strNoiseTexTag = "";
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
						m_iSelectedMeshIdx = iChildIdx;
					}
				}
				ImGui::TreePop();
			}

		}

		ImGui::EndTable();
	}
}

void CWindowMesh::Properties_Diffuse()
{
	if (ImGui::Checkbox("Use Diffuse", &m_bUseDiffuse))
	{
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetDiffuseInfo(m_bUseDiffuse, m_strDiffuseTexTag, m_iDiffuseSamplerType, m_iDiffuseDiscardArea, m_fDiffuseDiscardValue, m_iDiffuseUVType, _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[1]), _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]), m_fDiffuseUVAtlasSpeed);
	}
	/*if (m_pTestModel)
		m_pTestModel->SetUseTexture(TEX_DIFFUSE, m_bUseDiffuse);*/

	if (!m_bUseDiffuse)
		return;

	ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 30), ImVec2(FLT_MAX, 100));
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
				/*if (m_pTestModel)
					m_pTestModel->SetTexture(TEX_DIFFUSE, m_strDiffuseTexTag);*/
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetDiffuseInfo(m_bUseDiffuse, m_strDiffuseTexTag, m_iDiffuseSamplerType, m_iDiffuseDiscardArea, m_fDiffuseDiscardValue, m_iDiffuseUVType, _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[1]), _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]), m_fDiffuseUVAtlasSpeed);

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

	ImGui::Text("Sampler"); ImGui::SameLine();

	const char* items[] = { "WRAP", "MIRROR", "CLAMP", "BORDER" };
	static int item_diffuse_current_idx = 0; // Here we store our selection data as an index.
	const char* combo_preview_value = items[item_diffuse_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo("##Sampler", combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			const bool is_selected = (item_diffuse_current_idx == n);
			if (ImGui::Selectable(items[n], is_selected))
			{
				item_diffuse_current_idx = n;
				m_iDiffuseSamplerType = item_diffuse_current_idx;

				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetDiffuseInfo(m_bUseDiffuse, m_strDiffuseTexTag, m_iDiffuseSamplerType, m_iDiffuseDiscardArea, m_fDiffuseDiscardValue, m_iDiffuseUVType, _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[1]), _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]), m_fDiffuseUVAtlasSpeed);
				/*if (m_pTestModel)
					m_pTestModel->SetTexSampler(TEX_MASK, (SAMPLE_TYPE)item_current_idx);*/
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (m_pTestModel)
		m_pTestModel->SetUseTexture(TEX_DIFFUSE, m_bUseDiffuse);

	ImGui::Text("Discard"); ImGui::SameLine();

	int iPrevDiscardArea = m_iDiffuseDiscardArea;
	ImGui::RadioButton("Less Than", &m_iDiffuseDiscardArea, 0); ImGui::SameLine();
	ImGui::RadioButton("More Than", &m_iDiffuseDiscardArea, 1); ImGui::SameLine();

	/*if ((m_iDiffuseDiscardArea != iPrevDiscardArea) && m_pTestModel)
		m_pTestModel->SetTexDiscardOpt(TEX_MASK, m_fDiffuseDiscardValue, m_iDiffuseDiscardArea);*/
	if ((m_iDiffuseDiscardArea != iPrevDiscardArea) && m_pTestEffectMesh)
		m_pTestEffectMesh->SetDiffuseInfo(m_bUseDiffuse, m_strDiffuseTexTag, m_iDiffuseSamplerType, m_iDiffuseDiscardArea, m_fDiffuseDiscardValue, m_iDiffuseUVType, _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[1]), _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]), m_fDiffuseUVAtlasSpeed);

	_float fMinDiscard = 0.f;
	_float fMaxDiscard = 1.f;

	ImGui::SetNextItemWidth(50);
	if (ImGui::InputFloat("##DiscardCriteria", &m_fDiffuseDiscardValue, 0.f, 0.0f, "%.2f"))
	{
		if (m_fDiffuseDiscardValue < fMinDiscard)
			m_fDiffuseDiscardValue = fMinDiscard;
		else if (m_fDiffuseDiscardValue > fMaxDiscard)
			m_fDiffuseDiscardValue = fMaxDiscard;

		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetDiffuseInfo(m_bUseDiffuse, m_strDiffuseTexTag, m_iDiffuseSamplerType, m_iDiffuseDiscardArea, m_fDiffuseDiscardValue, m_iDiffuseUVType, _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[1]), _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]), m_fDiffuseUVAtlasSpeed);

		/*if (m_pTestModel)
			m_pTestModel->SetTexDiscardOpt(TEX_MASK, m_fDiffuseDiscardValue, m_iDiffuseDiscardArea);*/
	}

	ImGui::Text("UV Type"); ImGui::SameLine();

	int iPrevUVType = m_iDiffuseUVType;
	ImGui::RadioButton("Fixed", &m_iDiffuseUVType, 0); ImGui::SameLine();
	ImGui::RadioButton("Scroll", &m_iDiffuseUVType, 1); ImGui::SameLine();
	ImGui::RadioButton("Atlas", &m_iDiffuseUVType, 2);

	/*if ((iPrevUVType != m_iDiffuseUVType) && m_pTestModel)
		m_pTestModel->SetTexUV(TEX_MASK, (UV_TYPE)m_iDiffuseUVType);*/

	if ((iPrevUVType != m_iDiffuseUVType) && m_pTestEffectMesh)
		m_pTestEffectMesh->SetDiffuseInfo(m_bUseDiffuse, m_strDiffuseTexTag, m_iDiffuseSamplerType, m_iDiffuseDiscardArea, m_fDiffuseDiscardValue, m_iDiffuseUVType, _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[1]), _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]), m_fDiffuseUVAtlasSpeed);


	// Fixed
	if (0 == m_iDiffuseUVType)
	{

	}
	// Scroll
	else if (1 == m_iDiffuseUVType)
	{
		ImGui::Dummy(ImVec2(0.f, 2.f));

		ImGui::Text("- Scroll"); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputFloat2("##ScrollDiffuse", m_fDiffuseUVScroll, "%.2f"))
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetDiffuseInfo(m_bUseDiffuse, m_strDiffuseTexTag, m_iDiffuseSamplerType, m_iDiffuseDiscardArea, m_fDiffuseDiscardValue, m_iDiffuseUVType, _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[1]), _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]), m_fDiffuseUVAtlasSpeed);

			//if (m_pTestModel)
			//    m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[0]), _float2(m_fDiffuseUVScroll[1], m_fDiffuseUVScroll[1]));
		} ImGui::SameLine();
		ImGui::Text("(MIN, MAX)");

		ImGui::SameLine();
		if (ImGui::Button("Reset##ResetScroll", ImVec2(45.f, 20.f)))
		{
			m_fDiffuseUVScroll[0] = 0.f;
			m_fDiffuseUVScroll[1] = 0.f;

			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetDiffuseInfo(m_bUseDiffuse, m_strDiffuseTexTag, m_iDiffuseSamplerType, m_iDiffuseDiscardArea, m_fDiffuseDiscardValue, m_iDiffuseUVType, _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[1]), _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]), m_fDiffuseUVAtlasSpeed);

			//if (m_pTestModel)
			//    m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[0]), _float2(m_fDiffuseUVScroll[1], m_fDiffuseUVScroll[1]));
			//m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(vecScrollDiffuseX[0], vecScrollDiffuseX[1]), _float2(vecScrollDiffuseY[0], vecScrollDiffuseY[1]));
		}

		// X, Y Range 버전
		/*ImGui::Dummy(ImVec2(0.f, 2.f));

		static float vecScrollDiffuseX[2] = { 0.0f, 0.0f };
		static float vecScrollDiffuseY[2] = { 0.0f, 0.0f };
		ImGui::Text("- Scroll X"); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputFloat2("##ScrollDiffuseX", vecScrollDiffuseX, "%.2f"))
		{
			if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(vecScrollDiffuseX[0], vecScrollDiffuseX[1]), _float2(vecScrollDiffuseY[0], vecScrollDiffuseY[1]));
		} ImGui::SameLine();
		ImGui::Text("(MIN, MAX)");

		ImGui::SameLine();
		if (ImGui::Button("Reset##ResetScrollX", ImVec2(45.f, 20.f)))
		{
			vecScrollDiffuseX[0] = 0.f;
			vecScrollDiffuseX[1] = 0.f;

			if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(vecScrollDiffuseX[0], vecScrollDiffuseX[1]), _float2(vecScrollDiffuseY[0], vecScrollDiffuseY[1]));
		}*/

		/*ImGui::Dummy(ImVec2(0.f, 2.f));

		ImGui::Text("- Scroll Y"); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputFloat2("##ScrollDiffuseY", vecScrollDiffuseY, "%.2f"))
		{
			if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(vecScrollDiffuseX[0], vecScrollDiffuseX[1]), _float2(vecScrollDiffuseY[0], vecScrollDiffuseY[1]));
		} ImGui::SameLine();
		ImGui::Text("(MIN, MAX)");

		ImGui::SameLine();
		if (ImGui::Button("Reset##ResetScrollY", ImVec2(45.f, 20.f)))
		{
			vecScrollDiffuseY[0] = 0.f;
			vecScrollDiffuseY[1] = 0.f;

			if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(vecScrollDiffuseX[0], vecScrollDiffuseX[1]), _float2(vecScrollDiffuseY[0], vecScrollDiffuseY[1]));
		}*/
	}
	// Atlas
	else if (2 == m_iDiffuseUVType)
	{
		ImGui::Dummy(ImVec2(0.f, 2.f));

		ImGui::Text("- Atlas"); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputInt2("##AtlasCnt", m_iDiffuseUVAtlasCnt))
		{
			if (m_iDiffuseUVAtlasCnt[0] < 1)
				m_iDiffuseUVAtlasCnt[0] = 1;
			if (m_iDiffuseUVAtlasCnt[1] < 1)
				m_iDiffuseUVAtlasCnt[1] = 1;

			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetDiffuseInfo(m_bUseDiffuse, m_strDiffuseTexTag, m_iDiffuseSamplerType, m_iDiffuseDiscardArea, m_fDiffuseDiscardValue, m_iDiffuseUVType, _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[1]), _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]), m_fDiffuseUVAtlasSpeed);

			//if (m_pTestModel)
			//    m_pTestModel->SetTexAtlasOpt(TEX_MASK, _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]), m_fDiffuseUVAtlasSpeed);
		} ImGui::SameLine();
		ImGui::Text("(X, Y)"); ImGui::SameLine();

		ImGui::SetNextItemWidth(40);
		if (ImGui::InputFloat("##AtlasSpeed", &m_fDiffuseUVAtlasSpeed, 0.f, 0.f, "%.2f"))
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetDiffuseInfo(m_bUseDiffuse, m_strDiffuseTexTag, m_iDiffuseSamplerType, m_iDiffuseDiscardArea, m_fDiffuseDiscardValue, m_iDiffuseUVType, _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[1]), _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]), m_fDiffuseUVAtlasSpeed);

			/*if (m_pTestModel)
				m_pTestModel->SetTexAtlasOpt(TEX_MASK, _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]), m_fDiffuseUVAtlasSpeed);*/
		} ImGui::SameLine();
		ImGui::Text("Speed");
	}
}

void CWindowMesh::Properties_Mask()
{
	if (ImGui::Checkbox("Use Mask", &m_bUseMask))
	{
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetMaskInfo(m_bUseMask, m_strMaskTexTag, m_iMaskSamplerType, m_iMaskDiscardArea, m_fMaskDiscardValue, m_iMaskUVType, _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[1]), _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]), m_fMaskUVAtlasSpeed);
	}
	/*if (m_pTestModel)
		m_pTestModel->SetUseTexture(TEX_MASK, m_bUseMask);*/

	if (!m_bUseMask)
		return;

	ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 30), ImVec2(FLT_MAX, 100));
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

			if (ImGui::ImageButton(m_TextureList[TEX_DIFFUSE][i].c_str(), GAMEINSTANCE->GetSRV(m_TextureList[TEX_DIFFUSE][i].c_str()).Get(), size, uv0, uv1, bg_col, tint_col))
			{
				m_strMaskTexTag = m_TextureList[TEX_DIFFUSE][i];
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetMaskInfo(m_bUseMask, m_strMaskTexTag, m_iMaskSamplerType, m_iMaskDiscardArea, m_fMaskDiscardValue, m_iMaskUVType, _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[1]), _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]), m_fMaskUVAtlasSpeed);
				/*if (m_pTestModel)
					m_pTestModel->SetTexture(TEX_MASK, m_strMaskTexTag);*/
			}

			if (m_strMaskTexTag == m_TextureList[TEX_DIFFUSE][i])
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

	ImGui::Text("Sampler"); ImGui::SameLine();

	const char* items[] = { "WRAP", "MIRROR", "CLAMP", "BORDER" };
	static int item_current_idx = 0; // Here we store our selection data as an index.
	const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo("##Sampler", combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(items[n], is_selected))
			{
				item_current_idx = n;
				m_iMaskSamplerType = item_current_idx;

				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetMaskInfo(m_bUseMask, m_strMaskTexTag, m_iMaskSamplerType, m_iMaskDiscardArea, m_fMaskDiscardValue, m_iMaskUVType, _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[1]), _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]), m_fMaskUVAtlasSpeed);
				/*if (m_pTestModel)
					m_pTestModel->SetTexSampler(TEX_MASK, (SAMPLE_TYPE)item_current_idx);*/
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (m_pTestModel)
		m_pTestModel->SetUseTexture(TEX_MASK, m_bUseMask);

	ImGui::Text("Discard"); ImGui::SameLine();

	int iPrevDiscardArea = m_iMaskDiscardArea;
	ImGui::RadioButton("Less Than", &m_iMaskDiscardArea, 0); ImGui::SameLine();
	ImGui::RadioButton("More Than", &m_iMaskDiscardArea, 1); ImGui::SameLine();

	/*if ((m_iMaskDiscardArea != iPrevDiscardArea) && m_pTestModel)
		m_pTestModel->SetTexDiscardOpt(TEX_MASK, m_fMaskDiscardValue, m_iMaskDiscardArea);*/
	if ((m_iMaskDiscardArea != iPrevDiscardArea) && m_pTestEffectMesh)
		m_pTestEffectMesh->SetMaskInfo(m_bUseMask, m_strMaskTexTag, m_iMaskSamplerType, m_iMaskDiscardArea, m_fMaskDiscardValue, m_iMaskUVType, _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[1]), _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]), m_fMaskUVAtlasSpeed);

	_float fMinDiscard = 0.f;
	_float fMaxDiscard = 1.f;

	ImGui::SetNextItemWidth(50);
	if (ImGui::InputFloat("##DiscardCriteria", &m_fMaskDiscardValue, 0.f, 0.0f, "%.2f"))
	{
		if (m_fMaskDiscardValue < fMinDiscard)
			m_fMaskDiscardValue = fMinDiscard;
		else if (m_fMaskDiscardValue > fMaxDiscard)
			m_fMaskDiscardValue = fMaxDiscard;

		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetMaskInfo(m_bUseMask, m_strMaskTexTag, m_iMaskSamplerType, m_iMaskDiscardArea, m_fMaskDiscardValue, m_iMaskUVType, _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[1]), _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]), m_fMaskUVAtlasSpeed);

		/*if (m_pTestModel)
			m_pTestModel->SetTexDiscardOpt(TEX_MASK, m_fMaskDiscardValue, m_iMaskDiscardArea);*/
	}

	ImGui::Text("UV Type"); ImGui::SameLine();

	int iPrevUVType = m_iMaskUVType;
	ImGui::RadioButton("Fixed", &m_iMaskUVType, 0); ImGui::SameLine();
	ImGui::RadioButton("Scroll", &m_iMaskUVType, 1); ImGui::SameLine();
	ImGui::RadioButton("Atlas", &m_iMaskUVType, 2);

	/*if ((iPrevUVType != m_iMaskUVType) && m_pTestModel)
		m_pTestModel->SetTexUV(TEX_MASK, (UV_TYPE)m_iMaskUVType);*/

	if ((iPrevUVType != m_iMaskUVType) && m_pTestEffectMesh)
		m_pTestEffectMesh->SetMaskInfo(m_bUseMask, m_strMaskTexTag, m_iMaskSamplerType, m_iMaskDiscardArea, m_fMaskDiscardValue, m_iMaskUVType, _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[1]), _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]), m_fMaskUVAtlasSpeed);


	// Fixed
	if (0 == m_iMaskUVType)
	{

	}
	// Scroll
	else if (1 == m_iMaskUVType)
	{
		ImGui::Dummy(ImVec2(0.f, 2.f));

		ImGui::Text("- Scroll"); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputFloat2("##ScrollMask", m_fMaskUVScroll, "%.2f"))
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetMaskInfo(m_bUseMask, m_strMaskTexTag, m_iMaskSamplerType, m_iMaskDiscardArea, m_fMaskDiscardValue, m_iMaskUVType, _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[1]), _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]), m_fMaskUVAtlasSpeed);

			//if (m_pTestModel)
			//    m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[0]), _float2(m_fMaskUVScroll[1], m_fMaskUVScroll[1]));
		} ImGui::SameLine();
		ImGui::Text("(MIN, MAX)");

		ImGui::SameLine();
		if (ImGui::Button("Reset##ResetScroll", ImVec2(45.f, 20.f)))
		{
			m_fMaskUVScroll[0] = 0.f;
			m_fMaskUVScroll[1] = 0.f;

			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetMaskInfo(m_bUseMask, m_strMaskTexTag, m_iMaskSamplerType, m_iMaskDiscardArea, m_fMaskDiscardValue, m_iMaskUVType, _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[1]), _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]), m_fMaskUVAtlasSpeed);

			//if (m_pTestModel)
			//    m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[0]), _float2(m_fMaskUVScroll[1], m_fMaskUVScroll[1]));
			//m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(vecScrollMaskX[0], vecScrollMaskX[1]), _float2(vecScrollMaskY[0], vecScrollMaskY[1]));
		}

		// X, Y Range 버전
		/*ImGui::Dummy(ImVec2(0.f, 2.f));

		static float vecScrollMaskX[2] = { 0.0f, 0.0f };
		static float vecScrollMaskY[2] = { 0.0f, 0.0f };
		ImGui::Text("- Scroll X"); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputFloat2("##ScrollMaskX", vecScrollMaskX, "%.2f"))
		{
			if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(vecScrollMaskX[0], vecScrollMaskX[1]), _float2(vecScrollMaskY[0], vecScrollMaskY[1]));
		} ImGui::SameLine();
		ImGui::Text("(MIN, MAX)");

		ImGui::SameLine();
		if (ImGui::Button("Reset##ResetScrollX", ImVec2(45.f, 20.f)))
		{
			vecScrollMaskX[0] = 0.f;
			vecScrollMaskX[1] = 0.f;

			if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(vecScrollMaskX[0], vecScrollMaskX[1]), _float2(vecScrollMaskY[0], vecScrollMaskY[1]));
		}*/

		/*ImGui::Dummy(ImVec2(0.f, 2.f));

		ImGui::Text("- Scroll Y"); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputFloat2("##ScrollMaskY", vecScrollMaskY, "%.2f"))
		{
			if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(vecScrollMaskX[0], vecScrollMaskX[1]), _float2(vecScrollMaskY[0], vecScrollMaskY[1]));
		} ImGui::SameLine();
		ImGui::Text("(MIN, MAX)");

		ImGui::SameLine();
		if (ImGui::Button("Reset##ResetScrollY", ImVec2(45.f, 20.f)))
		{
			vecScrollMaskY[0] = 0.f;
			vecScrollMaskY[1] = 0.f;

			if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(vecScrollMaskX[0], vecScrollMaskX[1]), _float2(vecScrollMaskY[0], vecScrollMaskY[1]));
		}*/
	}
	// Atlas
	else if (2 == m_iMaskUVType)
	{
		ImGui::Dummy(ImVec2(0.f, 2.f));

		ImGui::Text("- Atlas"); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputInt2("##AtlasCnt", m_iMaskUVAtlasCnt))
		{
			if (m_iMaskUVAtlasCnt[0] < 1)
				m_iMaskUVAtlasCnt[0] = 1;
			if (m_iMaskUVAtlasCnt[1] < 1)
				m_iMaskUVAtlasCnt[1] = 1;

			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetMaskInfo(m_bUseMask, m_strMaskTexTag, m_iMaskSamplerType, m_iMaskDiscardArea, m_fMaskDiscardValue, m_iMaskUVType, _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[1]), _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]), m_fMaskUVAtlasSpeed);

			//if (m_pTestModel)
			//    m_pTestModel->SetTexAtlasOpt(TEX_MASK, _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]), m_fMaskUVAtlasSpeed);
		} ImGui::SameLine();
		ImGui::Text("(X, Y)"); ImGui::SameLine();

		ImGui::SetNextItemWidth(40);
		if (ImGui::InputFloat("##AtlasSpeed", &m_fMaskUVAtlasSpeed, 0.f, 0.f, "%.2f"))
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetMaskInfo(m_bUseMask, m_strMaskTexTag, m_iMaskSamplerType, m_iMaskDiscardArea, m_fMaskDiscardValue, m_iMaskUVType, _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[1]), _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]), m_fMaskUVAtlasSpeed);

			/*if (m_pTestModel)
				m_pTestModel->SetTexAtlasOpt(TEX_MASK, _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]), m_fMaskUVAtlasSpeed);*/
		} ImGui::SameLine();
		ImGui::Text("Speed");
	}
}

void CWindowMesh::Properties_Noise()
{
	if (ImGui::Checkbox("Use Noise", &m_bUseNoise))
	{
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetNoiseInfo(m_bUseNoise, m_strNoiseTexTag, m_iNoiseSamplerType, m_iNoiseDiscardArea, m_fNoiseDiscardValue, m_iNoiseUVType, _float2(m_fNoiseUVScroll[0], m_fNoiseUVScroll[1]), _float2((_float)m_iNoiseUVAtlasCnt[0], (_float)m_iNoiseUVAtlasCnt[1]), m_fNoiseUVAtlasSpeed);
	}
	/*if (m_pTestModel)
		m_pTestModel->SetUseTexture(TEX_NOISE, m_bUseNoise);*/

	if (!m_bUseNoise)
		return;

	ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 30), ImVec2(FLT_MAX, 100));
	if (ImGui::BeginListBox(""))
	{
		for (_int i = 0; i < m_iTextureIdx[TEX_NOISE]; ++i)
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

			MultiByteToWideChar(CP_ACP, 0, m_TextureList[TEX_NOISE][i].c_str(), static_cast<int>(strlen(m_TextureList[TEX_NOISE][i].c_str())), wTextureKey, MAX_PATH);

			if (ImGui::ImageButton(m_TextureList[TEX_NOISE][i].c_str(), GAMEINSTANCE->GetSRV(m_TextureList[TEX_NOISE][i].c_str()).Get(), size, uv0, uv1, bg_col, tint_col))
			{
				m_strNoiseTexTag = m_TextureList[TEX_NOISE][i];
				/*if (m_pTestModel)
					m_pTestModel->SetTexture(TEX_NOISE, m_strNoiseTexTag);*/
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetNoiseInfo(m_bUseNoise, m_strNoiseTexTag, m_iNoiseSamplerType, m_iNoiseDiscardArea, m_fNoiseDiscardValue, m_iNoiseUVType, _float2(m_fNoiseUVScroll[0], m_fNoiseUVScroll[1]), _float2((_float)m_iNoiseUVAtlasCnt[0], (_float)m_iNoiseUVAtlasCnt[1]), m_fNoiseUVAtlasSpeed);
			}

			if (m_strNoiseTexTag == m_TextureList[TEX_NOISE][i])
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

	ImGui::Text("Sampler"); ImGui::SameLine();

	const char* items[] = { "WRAP", "MIRROR", "CLAMP", "BORDER" };
	const char* combo_preview_value = items[m_iNoiseSamplerType];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo("##Sampler", combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			const bool is_selected = (m_iNoiseSamplerType == n);
			if (ImGui::Selectable(items[n], is_selected))
			{
				m_iNoiseSamplerType = n;

				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetNoiseInfo(m_bUseNoise, m_strNoiseTexTag, m_iNoiseSamplerType, m_iNoiseDiscardArea, m_fNoiseDiscardValue, m_iNoiseUVType, _float2(m_fNoiseUVScroll[0], m_fNoiseUVScroll[1]), _float2((_float)m_iNoiseUVAtlasCnt[0], (_float)m_iNoiseUVAtlasCnt[1]), m_fNoiseUVAtlasSpeed);
				/*if (m_pTestModel)
					m_pTestModel->SetTexSampler(TEX_NOISE, (SAMPLE_TYPE)m_iNoiseSamplerType);*/
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Text("UV Type"); ImGui::SameLine();

	int iPrevUVType = m_iNoiseUVType;
	ImGui::RadioButton("Fixed", &m_iNoiseUVType, 0); ImGui::SameLine();
	ImGui::RadioButton("Scroll", &m_iNoiseUVType, 1); ImGui::SameLine();
	ImGui::RadioButton("Atlas", &m_iNoiseUVType, 2);

	/*if ((iPrevUVType != m_iNoiseUVType) && m_pTestModel)
		m_pTestModel->SetTexUV(TEX_NOISE, (UV_TYPE)m_iNoiseUVType);*/
	if ((iPrevUVType != m_iNoiseUVType) && m_pTestEffectMesh)
		m_pTestEffectMesh->SetNoiseInfo(m_bUseNoise, m_strNoiseTexTag, m_iNoiseSamplerType, m_iNoiseDiscardArea, m_fNoiseDiscardValue, m_iNoiseUVType, _float2(m_fNoiseUVScroll[0], m_fNoiseUVScroll[1]), _float2((_float)m_iNoiseUVAtlasCnt[0], (_float)m_iNoiseUVAtlasCnt[1]), m_fNoiseUVAtlasSpeed);

	// Fixed
	if (0 == m_iNoiseUVType)
	{

	}
	// Scroll
	else if (1 == m_iNoiseUVType)
	{
		ImGui::Dummy(ImVec2(0.f, 2.f));

		ImGui::Text("- Scroll"); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputFloat2("##ScrollNoise", m_fNoiseUVScroll, "%.2f"))
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetNoiseInfo(m_bUseNoise, m_strNoiseTexTag, m_iNoiseSamplerType, m_iNoiseDiscardArea, m_fNoiseDiscardValue, m_iNoiseUVType, _float2(m_fNoiseUVScroll[0], m_fNoiseUVScroll[1]), _float2((_float)m_iNoiseUVAtlasCnt[0], (_float)m_iNoiseUVAtlasCnt[1]), m_fNoiseUVAtlasSpeed);

			/*if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_NOISE, _float2(m_fNoiseUVScroll[0], m_fNoiseUVScroll[0]), _float2(m_fNoiseUVScroll[1], m_fNoiseUVScroll[1]));*/
		} ImGui::SameLine();
		ImGui::Text("(MIN, MAX)");

		ImGui::SameLine();
		if (ImGui::Button("Reset##ResetScrollNoise", ImVec2(45.f, 20.f)))
		{
			m_fNoiseUVScroll[0] = 0.f;
			m_fNoiseUVScroll[1] = 0.f;

			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetNoiseInfo(m_bUseNoise, m_strNoiseTexTag, m_iNoiseSamplerType, m_iNoiseDiscardArea, m_fNoiseDiscardValue, m_iNoiseUVType, _float2(m_fNoiseUVScroll[0], m_fNoiseUVScroll[1]), _float2((_float)m_iNoiseUVAtlasCnt[0], (_float)m_iNoiseUVAtlasCnt[1]), m_fNoiseUVAtlasSpeed);

			/*if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_NOISE, _float2(m_fNoiseUVScroll[0], m_fNoiseUVScroll[0]), _float2(m_fNoiseUVScroll[1], m_fNoiseUVScroll[1]));*/
				//m_pTestModel->SetTexScrollOpt(TEX_MASK, _float2(vecScrollMaskX[0], vecScrollMaskX[1]), _float2(vecScrollMaskY[0], vecScrollMaskY[1]));
		}


		/*static float vecScrollNoiseX[2] = { 0.0f, 0.0f };
		static float vecScrollNoiseY[2] = { 0.0f, 0.0f };

		ImGui::Text("- Scroll X"); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputFloat2("##ScrollNoiseX", vecScrollNoiseX, "%.2f"))
		{
			if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_NOISE, _float2(vecScrollNoiseX[0], vecScrollNoiseX[1]), _float2(vecScrollNoiseY[0], vecScrollNoiseY[1]));
		} ImGui::SameLine();
		ImGui::Text("(MIN, MAX)");

		ImGui::SameLine();
		if (ImGui::Button("Reset##ResetScrollX", ImVec2(45.f, 20.f)))
		{
			vecScrollNoiseX[0] = 0.f;
			vecScrollNoiseX[1] = 0.f;

			if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_NOISE, _float2(vecScrollNoiseX[0], vecScrollNoiseX[1]), _float2(vecScrollNoiseY[0], vecScrollNoiseY[1]));
		}

		ImGui::Dummy(ImVec2(0.f, 2.f));

		ImGui::Text("- Scroll Y"); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputFloat2("##ScrollNoiseY", vecScrollNoiseY, "%.2f"))
		{
			if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_NOISE, _float2(vecScrollNoiseX[0], vecScrollNoiseX[1]), _float2(vecScrollNoiseY[0], vecScrollNoiseY[1]));
		} ImGui::SameLine();
		ImGui::Text("(MIN, MAX)");

		ImGui::SameLine();
		if (ImGui::Button("Reset##ResetScrollY", ImVec2(45.f, 20.f)))
		{
			vecScrollNoiseY[0] = 0.f;
			vecScrollNoiseY[1] = 0.f;

			if (m_pTestModel)
				m_pTestModel->SetTexScrollOpt(TEX_NOISE, _float2(vecScrollNoiseX[0], vecScrollNoiseX[1]), _float2(vecScrollNoiseY[0], vecScrollNoiseY[1]));
		}*/
	}
	// Atlas
	else if (2 == m_iNoiseUVType)
	{
		ImGui::Dummy(ImVec2(0.f, 2.f));

		ImGui::Text("- Atlas"); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputInt2("##AtlasCntNoise", m_iNoiseUVAtlasCnt))
		{
			if (m_iNoiseUVAtlasCnt[0] < 1)
				m_iNoiseUVAtlasCnt[0] = 1;
			if (m_iNoiseUVAtlasCnt[1] < 1)
				m_iNoiseUVAtlasCnt[1] = 1;

			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetNoiseInfo(m_bUseNoise, m_strNoiseTexTag, m_iNoiseSamplerType, m_iNoiseDiscardArea, m_fNoiseDiscardValue, m_iNoiseUVType, _float2(m_fNoiseUVScroll[0], m_fNoiseUVScroll[1]), _float2((_float)m_iNoiseUVAtlasCnt[0], (_float)m_iNoiseUVAtlasCnt[1]), m_fNoiseUVAtlasSpeed);

			/*if (m_pTestModel)
				m_pTestModel->SetTexAtlasOpt(TEX_NOISE, _float2((_float)m_iNoiseUVAtlasCnt[0], (_float)m_iNoiseUVAtlasCnt[1]), m_fNoiseUVAtlasSpeed);*/

		} ImGui::SameLine();
		ImGui::Text("(X, Y)"); ImGui::SameLine();

		ImGui::SetNextItemWidth(40);
		if (ImGui::InputFloat("##AtlasSpeedNoise", &m_fNoiseUVAtlasSpeed, 0.f, 0.f, "%.2f"))
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetNoiseInfo(m_bUseNoise, m_strNoiseTexTag, m_iNoiseSamplerType, m_iNoiseDiscardArea, m_fNoiseDiscardValue, m_iNoiseUVType, _float2(m_fNoiseUVScroll[0], m_fNoiseUVScroll[1]), _float2((_float)m_iNoiseUVAtlasCnt[0], (_float)m_iNoiseUVAtlasCnt[1]), m_fNoiseUVAtlasSpeed);

			/*if (m_pTestModel)
				m_pTestModel->SetTexAtlasOpt(TEX_NOISE, _float2((_float)m_iNoiseUVAtlasCnt[0], (_float)m_iNoiseUVAtlasCnt[1]), m_fNoiseUVAtlasSpeed);*/

		} ImGui::SameLine();
		ImGui::Text("Speed");
	}
}

void CWindowMesh::Properties_Color()
{
	if (ImGui::Checkbox("Use Color", &m_bUseColor))
	{
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetColorInfo(m_bUseColor, m_iColorSplitArea, _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]),
				m_iColorSetCnt, m_fColorTime, m_iColorEase, m_vColor);
	}

	/*if (m_pTestModel)
		m_pTestModel->SetUseTexture(TEX_END, m_bUseColor);*/

	if (!m_bUseColor)
		return;

	string  strSplitValueLabel[MAX_COLORSPLIT] = {};
	string  strColorTimeLabel[MAX_TIMETABLE] = {};               // 컬러 바꿔줄 시간 input창 ID
	string  strEasingTypeLabel[MAX_TIMETABLE] = {};               // Easing 그래프 타입 input창 ID
	string  strSplitColorLabel[MAX_TIMETABLE][MAX_COLORSPLIT] = {};               // 색깔을 입력하는 input창 ID

	ImGui::Text("Split Num"); ImGui::SameLine();
	if (ImGui::SliderInt("##SplitNumSlider", &m_iColorSplitArea, 1, 4))
	{
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetColorInfo(m_bUseColor, m_iColorSplitArea, _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]),
				m_iColorSetCnt, m_fColorTime, m_iColorEase, m_vColor);
	}

	/*if (m_pTestModel)
		m_pTestModel->SetColorSplitNum(m_iColorSplitArea);*/

	ImGui::Text("Split Value");
	for (int iSplitCnt = 0; iSplitCnt < MAX_COLORSPLIT; ++iSplitCnt)
	{
		ImGui::SameLine();

		strSplitValueLabel[iSplitCnt] = "##SplitValue" + to_string(iSplitCnt);

		// 스플릿 기준 입력 창 Enable 조건
		if (iSplitCnt < m_iColorSplitArea)
		{
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::InputFloat(strSplitValueLabel[iSplitCnt].c_str(), &m_vColorSplitter[iSplitCnt]))
			{
				if (m_vColorSplitter[iSplitCnt] < 0.f)
					m_vColorSplitter[iSplitCnt] = 0.f;
				else if (m_vColorSplitter[iSplitCnt] > 1.f)
					m_vColorSplitter[iSplitCnt] = 1.f;

				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetColorInfo(m_bUseColor, m_iColorSplitArea, _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]),
						m_iColorSetCnt, m_fColorTime, m_iColorEase, m_vColor);
			}
		}

		else
		{
			ImGui::SetNextItemWidth(50.f);
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ImGui::InputFloat(strSplitValueLabel[iSplitCnt].c_str(), &m_vColorSplitter[iSplitCnt]);
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
	}

	/*if (m_pTestModel)
	{
		m_pTestModel->SetColorSplitter(m_vColorSplitter);
	}*/

	for (int iCnt = 0; iCnt < m_iColorSetCnt; ++iCnt)
	{
		ImGui::Separator();
		strColorTimeLabel[iCnt] = "##ColorTime" + to_string(iCnt);
		strEasingTypeLabel[iCnt] = "##EasingType" + to_string(iCnt);

		ImGui::Text("Time"); ImGui::SameLine();
		ImGui::Dummy(ImVec2(27.f, 0.f)); ImGui::SameLine();
		ImGui::SetNextItemWidth(50.f);
		if (ImGui::InputFloat(strColorTimeLabel[iCnt].c_str(), &m_fColorTime[iCnt], 0.f, 0.f, "%.2f", ImGuiInputTextFlags_CharsNoBlank))
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetColorInfo(m_bUseColor, m_iColorSplitArea, _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]),
					m_iColorSetCnt, m_fColorTime, m_iColorEase, m_vColor);
			/*if (m_pTestModel)
				m_pTestModel->SetColorChangeTime(iCnt, m_fColorTime[iCnt]);*/
		}

		ImGui::Text("Easing"); ImGui::SameLine();
		ImGui::Dummy(ImVec2(13.f, 0.f)); ImGui::SameLine();
		ImGui::SetNextItemWidth(161.f);
		const char* strEasingTypes[EASE_END] = {};
		memcpy(strEasingTypes, GAMEINSTANCE->GetEaseTypeKeys(), sizeof(strEasingTypes));
		const char* combo_preview_value = strEasingTypes[m_iColorEase[iCnt]];  // Pass in the preview value visible before opening the combo (it could be anything)
		if (ImGui::BeginCombo(strEasingTypeLabel[iCnt].c_str(), combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
		{
			for (int n = 0; n < IM_ARRAYSIZE(strEasingTypes); n++)
			{
				const bool is_selected = (m_iColorEase[iCnt] == n);
				if (ImGui::Selectable(strEasingTypes[n], is_selected))
				{
					m_iColorEase[iCnt] = n;

					if (m_pTestEffectMesh)
						m_pTestEffectMesh->SetColorInfo(m_bUseColor, m_iColorSplitArea, _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]),
							m_iColorSetCnt, m_fColorTime, m_iColorEase, m_vColor);
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

		if (ImGui::BeginTable("##ColorTable", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
		{
			ImGui::TableSetupColumn("Num", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			for (int row = 0; row < m_iColorSplitArea; row++)
			{
				strSplitColorLabel[iCnt][row] = "##SplitColor" + to_string(iCnt) + to_string(row);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("#%d", row);

				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(25.f);

				ImGui::Text("%.2f", m_vColorSplitter[row]);

				ImGui::TableSetColumnIndex(2);
				// Color
				ImGui::SetNextItemWidth(220.f);
				if (ImGui::ColorEdit4(strSplitColorLabel[iCnt][row].c_str(), (float*)&m_vColor[iCnt][row], ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float))
				{
					if (m_pTestEffectMesh)
						m_pTestEffectMesh->SetColorInfo(m_bUseColor, m_iColorSplitArea, _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]),
							m_iColorSetCnt, m_fColorTime, m_iColorEase, m_vColor);
				}

				/*if (m_pTestModel)
					m_pTestModel->SetColor(iCnt, row, m_vColor[iCnt][row]);*/

			}
			ImGui::EndTable();
		}
		ImGui::Separator();
	}

	if (ImGui::Button("Add Set"))
	{
		// 최대 컬러 개수는 4개
		if (m_iColorSetCnt <= 3)
			m_iColorSetCnt++;

		/*if (m_pTestModel)
			m_pTestModel->SetColorSetNum(true);*/

		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetColorInfo(m_bUseColor, m_iColorSplitArea, _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]),
				m_iColorSetCnt, m_fColorTime, m_iColorEase, m_vColor);

	} ImGui::SameLine();

	if (ImGui::Button("Delete Set"))
	{
		// 최소 컬러 개수는 1개
		if (m_iColorSetCnt >= 2)
			m_iColorSetCnt--;

		/*if (m_pTestModel)
			m_pTestModel->SetColorSetNum(false);*/
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetColorInfo(m_bUseColor, m_iColorSplitArea, _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]),
				m_iColorSetCnt, m_fColorTime, m_iColorEase, m_vColor);
	}
}

void CWindowMesh::Properties_Scale()
{
	static string strScaleTimeLabel[MAX_TIMETABLE];
	static string strScaleLabel[MAX_TIMETABLE];
	static string strScaleEasingTypeLabel[MAX_TIMETABLE];

	// Follow 체크박스
	if (ImGui::Checkbox("Use Follow", &m_bScaleFollow))
	{
		// Follow 여부 Set
		/*if (!m_bScaleFollow && m_pTestModel)
			m_pTestModel->SetFollowObj(0, -1, nullptr);*/
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetOwner(m_bScaleFollow, 0, m_pFollowObj);

		if (!m_bScaleFollow && m_pTestEffectMesh)
			m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);
	}

	// Object리스트나, Bone리스트 선택 가능
	if (m_bScaleFollow)
	{
		_int iPrevFollowType = m_iScaleFollowType;
		ImGui::RadioButton("Object", &m_iScaleFollowType, 0); ImGui::SameLine();
		ImGui::RadioButton("Bone", &m_iScaleFollowType, 1);

		// Follow 타입 Set
		if ((iPrevFollowType != m_iScaleFollowType))
		{
			// 오브젝트 가져오기
			m_pFollowObj = CImGuiMgr::GetInstance()->GetWindowModel()->GetCurModelObj(), CImGuiMgr::GetInstance()->GetWindowModel();

			if (!m_pFollowObj)
			{
				ImGui::Text("No Object To Follow");
				return;
			}

			m_pFollowObjModel = dynamic_pointer_cast<CModel>(m_pFollowObj->GetComponent(L"Com_Model"));

			// 본 가져오기
			if (1 == m_iScaleFollowType)
			{
				m_pFollowObjBones = m_pFollowObjModel->GetBones();
				m_vecFollowBoneNames.clear();

				for (auto pBones : m_pFollowObjBones)
				{
					const _char* pName = pBones->GetBoneName();
					m_vecFollowBoneNames.push_back(pName);
				}
			}

			else {
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);
			}
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetOwner(true, 1, m_pFollowObj);
		}

		if (-1 == m_iScaleFollowType)
			return;

		else if (!m_pFollowObj)
		{
			ImGui::Text("No Object To Follow");
			return;
		}

		// 오브젝트 Follow
		if (0 == m_iScaleFollowType)
		{
			/*if (m_pTestModel)
				m_pTestModel->SetFollowObj(0, m_iScaleFollowType, m_pFollowObj);*/
				//dynamic_pointer_cast<CModel>(CImGuiMgr::GetInstance()->GetWindowModel()->GetCurModelObj()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr("");
		}

		// 본 Follow
		else
		{
			if (ImGui::BeginListBox("##BoneList", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (int n = 0; n < m_vecFollowBoneNames.size(); n++)
				{
					const bool is_selected = (m_pScaleFollowBoneIdx == n);
					if (ImGui::Selectable(m_vecFollowBoneNames[n], is_selected))
					{
						m_pScaleFollowBoneIdx = n;
						m_strScaleFollowBoneKey = m_vecFollowBoneNames[n];

						if (m_pTestEffectMesh)
							m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);
						/*if (m_pTestModel)
							m_pTestModel->SetFollowObj(0, m_iScaleFollowType, m_pFollowObj, m_pFollowObjModel->GetCombinedBoneMatrixPtr(m_vecFollowBoneNames[m_pScaleFollowBoneIdx]));*/
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}
	}

	// TimeTable
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

		/*if (m_pTestModel)
			m_pTestModel->SetTransformSetNum(0, m_iScaleCnt);*/

		for (int iCnt = 0; iCnt < m_iScaleCnt; iCnt++)
		{
			strScaleTimeLabel[iCnt] = "##ScaleTime" + to_string(iCnt);
			strScaleLabel[iCnt] = "##Scale" + to_string(iCnt);
			strScaleEasingTypeLabel[iCnt] = "##ScaleEase" + to_string(iCnt);

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(40.f);
			if (ImGui::InputFloat(strScaleTimeLabel[iCnt].c_str(), &m_fScaleTime[iCnt], 0.f, 0.f, "%.2f"))
			{
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);
				/*if (m_pTestModel)
					m_pTestModel->SetTransformTime(0, iCnt, m_fScaleTime[iCnt]);*/
			}

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(140.f);

			if (ImGui::InputFloat3(strScaleLabel[iCnt].c_str(), m_fScale[iCnt], "%.2f"))
			{
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);
				/*if (m_pTestModel)
					m_pTestModel->SetTransformValue(0, iCnt, m_fScale[iCnt]);*/
			}

			ImGui::TableSetColumnIndex(2);
			ImGui::SetNextItemWidth(110.f);

			const char* strEasingTypes[EASE_END] = {};
			memcpy(strEasingTypes, GAMEINSTANCE->GetEaseTypeKeys(), sizeof(strEasingTypes));
			const char* combo_preview_value = strEasingTypes[m_iScaleEasing[iCnt]];  // Pass in the preview value visible before opening the combo (it could be anything)
			if (ImGui::BeginCombo(strScaleEasingTypeLabel[iCnt].c_str(), combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
			{
				for (int n = 0; n < IM_ARRAYSIZE(strEasingTypes); n++)
				{
					const bool is_selected = (m_iScaleEasing[iCnt] == n);
					if (ImGui::Selectable(strEasingTypes[n], is_selected))
					{
						m_iScaleEasing[iCnt] = n;

						if (m_pTestEffectMesh)
							m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);
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
		if (m_iScaleCnt <= 3)
			m_iScaleCnt++;
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);

	} ImGui::SameLine();

	if (ImGui::Button("Delete Set"))
	{
		// 최소 컬러 개수는 1개
		if (m_iScaleCnt >= 2)
			m_iScaleCnt--;
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);
	}
}

void CWindowMesh::Properties_Rotation()
{
	static string strRotationTimeLabel[MAX_TIMETABLE];
	static string strRotationLabel[MAX_TIMETABLE];
	static string strRotationEasingTypeLabel[MAX_TIMETABLE];

	_int iPrevFollowType = m_iRotationFollowType;
	ImGui::RadioButton("World", &m_iRotationFollowType, -1); ImGui::SameLine();
	ImGui::RadioButton("Object", &m_iRotationFollowType, 0); ImGui::SameLine();
	ImGui::RadioButton("Bone", &m_iRotationFollowType, 1);

	// Follow 타입 Set
	if ((iPrevFollowType != m_iRotationFollowType))
	{
		if (m_iRotationFollowType != -1)
		{
			// 오브젝트 가져오기
			m_pFollowObj = CImGuiMgr::GetInstance()->GetWindowModel()->GetCurModelObj(), CImGuiMgr::GetInstance()->GetWindowModel();

			if (!m_pFollowObj)
			{
				ImGui::Text("No Object To Follow");
				return;
			}

			m_pFollowObjModel = dynamic_pointer_cast<CModel>(m_pFollowObj->GetComponent(L"Com_Model"));

			// 본 가져오기
			if (1 == m_iRotationFollowType)
			{
				m_pFollowObjBones = m_pFollowObjModel->GetBones();
				m_vecFollowBoneNames.clear();

				for (auto pBones : m_pFollowObjBones)
				{
					const _char* pName = pBones->GetBoneName();
					m_vecFollowBoneNames.push_back(pName);
				}
			}
		}

		else {
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));
		}
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetOwner(true, 1, m_pFollowObj);
	}

	if (m_iRotationFollowType != -1)
	{
		if (!m_pFollowObj)
		{
			ImGui::Text("No Object To Follow");
				return;
		}

		// 오브젝트 Follow
		if (0 == m_iRotationFollowType)
		{
			/*if (m_pTestModel)
				m_pTestModel->SetFollowObj(1, m_iRotationFollowType, m_pFollowObj);*/
				//dynamic_pointer_cast<CModel>(CImGuiMgr::GetInstance()->GetWindowModel()->GetCurModelObj()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr("");
		}

		// 본 Follow
		else
		{
			if (ImGui::BeginListBox("##BoneList", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (int n = 0; n < m_vecFollowBoneNames.size(); n++)
				{
					const bool is_selected = (m_pRotationFollowBoneIdx == n);
					if (ImGui::Selectable(m_vecFollowBoneNames[n], is_selected))
					{
						m_pRotationFollowBoneIdx = n;
						m_strRotationFollowBoneKey = m_vecFollowBoneNames[n];

						if (m_pTestEffectMesh)
							m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));
						/*if (m_pTestModel)
							m_pTestModel->SetFollowObj(1, m_iRotationFollowType, m_pFollowObj, m_pFollowObjModel->GetCombinedBoneMatrixPtr(m_vecFollowBoneNames[m_pRotationFollowBoneIdx]));*/
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}

		// Follow 체크박스
		if (ImGui::Checkbox("Use Follow", &m_bRotationFollow))
		{
			// Follow 여부 Set
			/*if (!m_bRotationFollow && m_pTestModel)
				m_pTestModel->SetFollowObj(1, -1, nullptr);*/

			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetOwner(m_bRotationFollow, 1, m_pFollowObj);

			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));
		}
	}

	// TimeTable
	// Time, X, Y, Z, Easing
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
			ImGui::Text("Rotation");

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##Rotation R", &m_fRotate[0], 0.f, 0.f, "%.2f"))
			{
				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 0, m_fRotate);
				}*/
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));

			}

			ImGui::TableSetColumnIndex(2);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##Rotation U", &m_fRotate[1], 0.f, 0.f, "%.2f"))
			{
				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 0, m_fRotate);
				}*/
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));
			}

			ImGui::TableSetColumnIndex(3);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##Rotation L", &m_fRotate[2], 0.f, 0.f, "%.2f"))
			{
				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 0, m_fRotate);
				}*/
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));
			}

			ImGui::TableSetColumnIndex(4);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Degree");
		}

		ImGui::TableNextRow();

		{
			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Turn(vel)");

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnV R", &m_fTurnVel[0], 0.f, 0.f, "%.2f"))
			{
				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 1, m_fTurnVel);
				}*/
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));
			}

			ImGui::TableSetColumnIndex(2);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnV U", &m_fTurnVel[1], 0.f, 0.f, "%.2f"))
			{
				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 1, m_fTurnVel);
				}*/
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));
			}

			ImGui::TableSetColumnIndex(3);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnV L", &m_fTurnVel[2], 0.f, 0.f, "%.2f"))
			{
				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 1, m_fTurnVel);
				}*/
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));
			}

			ImGui::TableSetColumnIndex(4);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Degree/Sec");
		}

		ImGui::TableNextRow();

		{
			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Turn(Acc)");

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnAcc R", &m_fTurnAcc[0], 0.f, 0.f, "%.2f"))
			{
				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 2, m_fTurnAcc);
				}*/
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));
			}

			ImGui::TableSetColumnIndex(2);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnAcc U", &m_fTurnAcc[1], 0.f, 0.f, "%.2f"))
			{
				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 2, m_fTurnAcc);
				}*/
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));
			}

			ImGui::TableSetColumnIndex(3);
			ImGui::SetNextItemWidth(45.f);
			if (ImGui::InputFloat("##TurnAcc L", &m_fTurnAcc[2], 0.f, 0.f, "%.2f"))
			{
				/*if (m_pTestModel)
				{
					m_pTestModel->SetTransformValue(1, 2, m_fTurnAcc);
				}*/
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));
			}

			ImGui::TableSetColumnIndex(4);
			ImGui::SetNextItemWidth(40.f);
			ImGui::Text("Degree/Sec");
		}

		ImGui::EndTable();
	}
}

void CWindowMesh::Properties_Translation()
{
	static string strTranslationTimeLabel[MAX_TIMETABLE];
	static string strTranslationLabel[MAX_TIMETABLE];
	static string strTranslationEasingTypeLabel[MAX_TIMETABLE];

	_int iPrevFollowType = m_iTranslationFollowType;
	ImGui::RadioButton("World", &m_iTranslationFollowType, -1); ImGui::SameLine();
	ImGui::RadioButton("Object", &m_iTranslationFollowType, 0); ImGui::SameLine();
	ImGui::RadioButton("Bone", &m_iTranslationFollowType, 1);

	// Follow 타입 Set
	if ((iPrevFollowType != m_iTranslationFollowType))
	{
		// 오브젝트 가져오기
		m_pFollowObj = CImGuiMgr::GetInstance()->GetWindowModel()->GetCurModelObj(), CImGuiMgr::GetInstance()->GetWindowModel();

		if (!m_pFollowObj)
		{
			ImGui::Text("No Object To Follow");
			return;
		}

		m_pFollowObjModel = dynamic_pointer_cast<CModel>(m_pFollowObj->GetComponent(L"Com_Model"));

		// 본 가져오기
		if (1 == m_iTranslationFollowType)
		{
			m_pFollowObjBones = m_pFollowObjModel->GetBones();
			m_vecFollowBoneNames.clear();

			for (auto pBones : m_pFollowObjBones)
			{
				const _char* pName = pBones->GetBoneName();
				m_vecFollowBoneNames.push_back(pName);
			}
		}
		else
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetTransInfo(m_bTranslationFollow, m_iTranslationFollowType, m_strTranslationFollowBoneKey, m_iTranslationCnt, m_fTranslationTime, m_fTranslation, m_iTranslationEasing);
		}
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetOwner(true, 2, m_pFollowObj);
	}

	if (-1 != m_iTranslationFollowType)
	{
		if (!m_pFollowObj)
		{
			ImGui::Text("No Object To Follow");
			return;
		}

		// 오브젝트 Follow
		if (0 == m_iTranslationFollowType)
		{
			/*if (m_pTestModel)
				m_pTestModel->SetFollowObj(2, m_iTranslationFollowType, m_pFollowObj);*/
				//dynamic_pointer_cast<CModel>(CImGuiMgr::GetInstance()->GetWindowModel()->GetCurModelObj()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr("");
		}

		// 본 Follow
		else
		{
			if (ImGui::BeginListBox("##BoneList", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (int n = 0; n < m_vecFollowBoneNames.size(); n++)
				{
					const bool is_selected = (m_pTranslationFollowBoneIdx == n);
					if (ImGui::Selectable(m_vecFollowBoneNames[n], is_selected))
					{
						m_pTranslationFollowBoneIdx = n;
						m_strTranslationFollowBoneKey = m_vecFollowBoneNames[n];

						if (m_pTestEffectMesh)
							m_pTestEffectMesh->SetTransInfo(m_bTranslationFollow, m_iTranslationFollowType, m_strTranslationFollowBoneKey, m_iTranslationCnt, m_fTranslationTime, m_fTranslation, m_iTranslationEasing);
						/*if (m_pTestModel)
							m_pTestModel->SetFollowObj(2, m_iTranslationFollowType, m_pFollowObj, m_pFollowObjModel->GetCombinedBoneMatrixPtr(m_vecFollowBoneNames[m_pTranslationFollowBoneIdx]));*/
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}

		// Follow 체크박스
		if (ImGui::Checkbox("Use Follow", &m_bTranslationFollow))
		{
			// Follow 여부 Set
			/*if (!m_bTranslationFollow && m_pTestModel)
				m_pTestModel->SetFollowObj(2, -1, nullptr);*/

			
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetOwner(m_bTranslationFollow, 2, m_pFollowObj);

			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetTransInfo(m_bTranslationFollow, m_iTranslationFollowType, m_strTranslationFollowBoneKey, m_iTranslationCnt, m_fTranslationTime, m_fTranslation, m_iTranslationEasing);
		}
	}

	// TimeTable
	// Time, X, Y, Z, Easing
	if (ImGui::BeginTable("##TranslationTimeTable", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
	{
		ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Translation", ImGuiTableColumnFlags_WidthFixed);
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

		/*if (m_pTestModel)
			m_pTestModel->SetTransformSetNum(2, m_iTranslationCnt);*/

		for (int iCnt = 0; iCnt < m_iTranslationCnt; iCnt++)
		{
			strTranslationTimeLabel[iCnt] = "##TranslationTime" + to_string(iCnt);

			strTranslationLabel[iCnt] = "##Translation" + to_string(iCnt);

			strTranslationEasingTypeLabel[iCnt] = "##TranslationEase" + to_string(iCnt);

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::SetNextItemWidth(40.f);
			if (ImGui::InputFloat(strTranslationTimeLabel[iCnt].c_str(), &m_fTranslationTime[iCnt], 0.f, 0.f, "%.2f"))
			{
				/*if (m_pTestModel)
					m_pTestModel->SetTransformTime(2, iCnt, m_fTranslationTime[iCnt]);*/
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetTransInfo(m_bTranslationFollow, m_iTranslationFollowType, m_strTranslationFollowBoneKey, m_iTranslationCnt, m_fTranslationTime, m_fTranslation, m_iTranslationEasing);
			}

			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(140.f);

			if (ImGui::InputFloat3(strTranslationLabel[iCnt].c_str(), m_fTranslation[iCnt], "%.2f"))
			{
				/*if (m_pTestModel)
					m_pTestModel->SetTransformValue(2, iCnt, m_fTranslation[iCnt]);*/
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetTransInfo(m_bTranslationFollow, m_iTranslationFollowType, m_strTranslationFollowBoneKey, m_iTranslationCnt, m_fTranslationTime, m_fTranslation, m_iTranslationEasing);
			}

			ImGui::TableSetColumnIndex(2);
			ImGui::SetNextItemWidth(110.f);

			const char* strEasingTypes[EASE_END] = {};
			memcpy(strEasingTypes, GAMEINSTANCE->GetEaseTypeKeys(), sizeof(strEasingTypes));
			const char* combo_preview_value = strEasingTypes[m_iTranslationEasing[iCnt]];  // Pass in the preview value visible before opening the combo (it could be anything)
			if (ImGui::BeginCombo(strTranslationEasingTypeLabel[iCnt].c_str(), combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
			{
				for (int n = 0; n < IM_ARRAYSIZE(strEasingTypes); n++)
				{
					const bool is_selected = (m_iTranslationEasing[iCnt] == n);
					if (ImGui::Selectable(strEasingTypes[n], is_selected))
					{
						m_iTranslationEasing[iCnt] = n;
						if (m_pTestEffectMesh)
							m_pTestEffectMesh->SetTransInfo(m_bTranslationFollow, m_iTranslationFollowType, m_strTranslationFollowBoneKey, m_iTranslationCnt, m_fTranslationTime, m_fTranslation, m_iTranslationEasing);
						/*if (m_pTestModel)
							m_pTestModel->SetTransformEasing(2, iCnt, m_iTranslationEasing[iCnt]);*/
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
		if (m_iTranslationCnt <= 3)
			m_iTranslationCnt++;
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetTransInfo(m_bTranslationFollow, m_iTranslationFollowType, m_strTranslationFollowBoneKey, m_iTranslationCnt, m_fTranslationTime, m_fTranslation, m_iTranslationEasing);
	} ImGui::SameLine();

	if (ImGui::Button("Delete Set"))
	{
		// 최소 컬러 개수는 1개
		if (m_iTranslationCnt >= 2)
			m_iTranslationCnt--;
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetTransInfo(m_bTranslationFollow, m_iTranslationFollowType, m_strTranslationFollowBoneKey, m_iTranslationCnt, m_fTranslationTime, m_fTranslation, m_iTranslationEasing);
	}
}

void CWindowMesh::Properties_Shader()
{
	ImGui::Dummy(ImVec2(0.f, 2.f));
	ImGui::Separator();

	if (ImGui::Checkbox("Soft Effect", &m_bUseSoftEffect))
	{
		if (m_bUseSoftEffect)
			m_iRenderGroup = m_iSoftEffectType == 0 ? CRenderer::RENDER_GLOW : CRenderer::RENDER_BLUR;

		else
			m_iRenderGroup = CRenderer::RENDER_BLEND;

		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
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

			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetDefaultInfo(m_iShaderType, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
		}

		if (0 == m_iSoftEffectType)
		{
			ImGui::Dummy(ImVec2(2.f, 0.f)); ImGui::SameLine();

			if (ImGui::Checkbox("Use Color", &m_bGlowUseColor))
			{

			}

			if (m_bGlowUseColor)
			{
				ImGui::Dummy(ImVec2(2.f, 0.f)); ImGui::SameLine();
				if (ImGui::BeginTable("##GlowColorTimeTable", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
				{
					ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("GlowColor", ImGuiTableColumnFlags_WidthFixed); ImGui::TableHeadersRow();

					static string strGlowColorTimeLabel[MAX_TIMETABLE];
					static string strGlowColorLabel[MAX_TIMETABLE];

					for (int iCnt = 0; iCnt < m_iGlowColorSetCnt; iCnt++)
					{
						strGlowColorTimeLabel[iCnt] = "##GlowColorTime" + to_string(iCnt);

						strGlowColorLabel[iCnt] = "##GlowColor" + to_string(iCnt);

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::SetNextItemWidth(40.f);
						if (ImGui::InputFloat(strGlowColorTimeLabel[iCnt].c_str(), &m_fGlowColorTime[iCnt], 0.f, 0.f, "%.2f"))
						{
							if (m_pTestEffectMesh)
								m_pTestEffectMesh->SetShaderGlowInfo(m_bGlowUseColor, m_iGlowColorSetCnt, m_fGlowColorTime, m_fGlowColor);
						}

						ImGui::TableSetColumnIndex(1);

						ImGui::SetNextItemWidth(250.f);
						if (ImGui::ColorEdit4(strGlowColorLabel[iCnt].c_str(), m_fGlowColor[iCnt], ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float))
						{
							if (m_pTestEffectMesh)
								m_pTestEffectMesh->SetShaderGlowInfo(m_bGlowUseColor, m_iGlowColorSetCnt, m_fGlowColorTime, m_fGlowColor);
						}
					}
					ImGui::EndTable();
				}
				ImGui::Separator();

				// Add Set, Delete Set 버튼
				if (ImGui::Button("Add Set"))
				{
					// 최대 컬러 개수는 4개
					if (m_iGlowColorSetCnt <= 3)
						m_iGlowColorSetCnt++;
					if (m_pTestEffectMesh)
						m_pTestEffectMesh->SetShaderGlowInfo(m_bGlowUseColor, m_iGlowColorSetCnt, m_fGlowColorTime, m_fGlowColor);
				} ImGui::SameLine();

				if (ImGui::Button("Delete Set"))
				{
					// 최소 컬러 개수는 1개
					if (m_iGlowColorSetCnt >= 2)
						m_iGlowColorSetCnt--;
					if (m_pTestEffectMesh)
						m_pTestEffectMesh->SetShaderGlowInfo(m_bGlowUseColor, m_iGlowColorSetCnt, m_fGlowColorTime, m_fGlowColor);
				}

			}
		}
	}

	ImGui::Dummy(ImVec2(0.f, 2.f));

	ImGui::Separator();
	if (ImGui::Checkbox("Distortion", &m_bUseDistortion))
	{
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetDistiortion(m_bUseDistortion, m_strDistortionTexTag, m_fDistortionSpeed, m_fDistortionWeight);
	}

	if (m_bUseDistortion)
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 30), ImVec2(FLT_MAX, 100));
		if (ImGui::BeginListBox(""))
		{
			for (_int i = 0; i < m_iTextureIdx[TEX_NOISE]; ++i)
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

				MultiByteToWideChar(CP_ACP, 0, m_TextureList[TEX_NOISE][i].c_str(), static_cast<int>(strlen(m_TextureList[TEX_NOISE][i].c_str())), wTextureKey, MAX_PATH);

				if (ImGui::ImageButton(m_TextureList[TEX_NOISE][i].c_str(), GAMEINSTANCE->GetSRV(m_TextureList[TEX_NOISE][i].c_str()).Get(), size, uv0, uv1, bg_col, tint_col))
				{
					m_strDistortionTexTag = m_TextureList[TEX_NOISE][i];
					if (m_pTestEffectMesh)
						m_pTestEffectMesh->SetDistiortion(m_bUseDistortion, m_strDistortionTexTag, m_fDistortionSpeed, m_fDistortionWeight);
					/*if (m_pTestModel)
						m_pTestModel->SetTexture(TEX_MASK, m_strMaskTexTag);*/
				}

				if (m_strDistortionTexTag == m_TextureList[TEX_NOISE][i])
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

		if (ImGui::InputFloat("Speed", &m_fDistortionSpeed, 0.1f, 1.f, "%.3f"))
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetDistiortion(m_bUseDistortion, m_strDistortionTexTag, m_fDistortionSpeed, m_fDistortionWeight);
		}

		if (ImGui::InputFloat("Weight", &m_fDistortionWeight, 0.1f, 1.f, "%.3f"))
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetDistiortion(m_bUseDistortion, m_strDistortionTexTag, m_fDistortionSpeed, m_fDistortionWeight);
		}
	}

	ImGui::Dummy(ImVec2(0.f, 2.f));
	ImGui::Separator();
	if (ImGui::Checkbox("Dissolve", &m_bUseDslv))
	{
		if (m_pTestEffectMesh)
			m_pTestEffectMesh->SetDissolveT(m_bUseDslv, m_iDslvType, m_fDslvStartTime, m_fDslvDuration, m_fDslvThick, _float4(m_vDslvColor[0], m_vDslvColor[1], m_vDslvColor[2], m_vDslvColor[3]), m_strDslvTexKey, m_bDslvGlow, _float4(m_vDslvGlowColor[0], m_vDslvGlowColor[1], m_vDslvGlowColor[2], m_vDslvGlowColor[3]));
	}

	if (m_bUseDslv)
	{
		// Dissolve Type
		int iPrevDslvType = m_iDslvType;
		ImGui::RadioButton("Dissove In", &m_iDslvType, 0); ImGui::SameLine();
		ImGui::RadioButton("Dissolve Out", &m_iDslvType, 1);

		if (iPrevDslvType != m_iDslvType)
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetDissolveT(m_bUseDslv, m_iDslvType, m_fDslvStartTime, m_fDslvDuration, m_fDslvThick, _float4(m_vDslvColor[0], m_vDslvColor[1], m_vDslvColor[2], m_vDslvColor[3]), m_strDslvTexKey, m_bDslvGlow, _float4(m_vDslvGlowColor[0], m_vDslvGlowColor[1], m_vDslvGlowColor[2], m_vDslvGlowColor[3]));
		}

		if (1 == m_iDslvType)
		{
			// Dslv Start Time
			ImGui::SetNextItemWidth(100.f);
			if (ImGui::InputFloat("Dissolve Start Time", &m_fDslvStartTime))
			{
				if (m_fDuration < m_fDslvStartTime)
				{
					m_fDslvStartTime = m_fDuration;
				}

				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetDissolveT(m_bUseDslv, m_iDslvType, m_fDslvStartTime, m_fDslvDuration, m_fDslvThick, _float4(m_vDslvColor[0], m_vDslvColor[1], m_vDslvColor[2], m_vDslvColor[3]), m_strDslvTexKey, m_bDslvGlow, _float4(m_vDslvGlowColor[0], m_vDslvGlowColor[1], m_vDslvGlowColor[2], m_vDslvGlowColor[3]));
			}
		}

		// Dslv Duration
		ImGui::SetNextItemWidth(100.f);
		if (ImGui::InputFloat("Dissolve Duration", &m_fDslvDuration))
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetDissolveT(m_bUseDslv, m_iDslvType, m_fDslvStartTime, m_fDslvDuration, m_fDslvThick, _float4(m_vDslvColor[0], m_vDslvColor[1], m_vDslvColor[2], m_vDslvColor[3]), m_strDslvTexKey, m_bDslvGlow, _float4(m_vDslvGlowColor[0], m_vDslvGlowColor[1], m_vDslvGlowColor[2], m_vDslvGlowColor[3]));
		}

		// Dslv Thick
		ImGui::SetNextItemWidth(100.f);
		if (ImGui::InputFloat("Dissolve Thick", &m_fDslvThick))
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetDissolveT(m_bUseDslv, m_iDslvType, m_fDslvStartTime, m_fDslvDuration, m_fDslvThick, _float4(m_vDslvColor[0], m_vDslvColor[1], m_vDslvColor[2], m_vDslvColor[3]), m_strDslvTexKey, m_bDslvGlow, _float4(m_vDslvGlowColor[0], m_vDslvGlowColor[1], m_vDslvGlowColor[2], m_vDslvGlowColor[3]));
		}

		// Dslv Color
		if (ImGui::ColorEdit4("Dissolve Color", m_vDslvColor, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float))
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetDissolveT(m_bUseDslv, m_iDslvType, m_fDslvStartTime, m_fDslvDuration, m_fDslvThick, _float4(m_vDslvColor[0], m_vDslvColor[1], m_vDslvColor[2], m_vDslvColor[3]), m_strDslvTexKey, m_bDslvGlow, _float4(m_vDslvGlowColor[0], m_vDslvGlowColor[1], m_vDslvGlowColor[2], m_vDslvGlowColor[3]));
		}

		// Dslv Tex Key
		ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 30), ImVec2(FLT_MAX, 100));
		if (ImGui::BeginListBox("Dslv"))
		{
			for (_int i = 0; i < m_iTextureIdx[TEX_NOISE]; ++i)
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

				MultiByteToWideChar(CP_ACP, 0, m_TextureList[TEX_NOISE][i].c_str(), static_cast<int>(strlen(m_TextureList[TEX_NOISE][i].c_str())), wTextureKey, MAX_PATH);

				const char* strID = "Dslv" + i;

				if (ImGui::ImageButton(m_TextureList[TEX_NOISE][i].c_str(), GAMEINSTANCE->GetSRV(m_TextureList[TEX_NOISE][i].c_str()).Get(), size, uv0, uv1, bg_col, tint_col))
				{
					m_strDslvTexKey = m_TextureList[TEX_NOISE][i];
					if (m_pTestEffectMesh)
						m_pTestEffectMesh->SetDissolveT(m_bUseDslv, m_iDslvType, m_fDslvStartTime, m_fDslvDuration, m_fDslvThick, _float4(m_vDslvColor[0], m_vDslvColor[1], m_vDslvColor[2], m_vDslvColor[3]), m_strDslvTexKey, m_bDslvGlow, _float4(m_vDslvGlowColor[0], m_vDslvGlowColor[1], m_vDslvGlowColor[2], m_vDslvGlowColor[3]));
					/*if (m_pTestModel)
						m_pTestModel->SetTexture(TEX_MASK, m_strMaskTexTag);*/
				}

				if (m_strDslvTexKey == m_TextureList[TEX_NOISE][i])
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


		// Use Dslv Glow
		if (ImGui::Checkbox("Dissolve Glow", &m_bDslvGlow))
		{
			if (m_pTestEffectMesh)
				m_pTestEffectMesh->SetDissolveT(m_bUseDslv, m_iDslvType, m_fDslvStartTime, m_fDslvDuration, m_fDslvThick, _float4(m_vDslvColor[0], m_vDslvColor[1], m_vDslvColor[2], m_vDslvColor[3]), m_strDslvTexKey, m_bDslvGlow, _float4(m_vDslvGlowColor[0], m_vDslvGlowColor[1], m_vDslvGlowColor[2], m_vDslvGlowColor[3]));
		}

		if (m_bDslvGlow)
		{
			// Dslv Glow Color
			if (ImGui::ColorEdit4("Dissolve Glow Color", m_vDslvColor, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float))
			{
				if (m_pTestEffectMesh)
					m_pTestEffectMesh->SetDissolveT(m_bUseDslv, m_iDslvType, m_fDslvStartTime, m_fDslvDuration, m_fDslvThick, _float4(m_vDslvColor[0], m_vDslvColor[1], m_vDslvColor[2], m_vDslvColor[3]), m_strDslvTexKey, m_bDslvGlow, _float4(m_vDslvGlowColor[0], m_vDslvGlowColor[1], m_vDslvGlowColor[2], m_vDslvGlowColor[3]));
			}
		}
	}
}

CEffect::EffectDesc CWindowMesh::CreateEffectDesc()
{
	// 구조체에 다 담아주기
	m_CreateEffect.iEffectType = CEffect::EFFECT_MESH;
	m_CreateEffect.iShaderType = m_iShaderType;
	m_CreateEffect.iShaderPass = m_iShaderPass;

	m_CreateEffect.iRenderGroup = m_iRenderGroup;

	m_CreateEffect.fDuration = m_fDuration;
	m_CreateEffect.StartTrackPos = m_StartTrackPos;

	m_CreateEffect.bBillBoard = m_bBillBoard;
	m_CreateEffect.bLoop = m_bLoop;

	m_CreateEffect.strModelKey = m_strModelKey;

	m_CreateEffect.bUseDiffuse = m_bUseDiffuse;
	m_CreateEffect.strTexDiffuseKey = m_strDiffuseTexTag;

	m_CreateEffect.bUseMask = m_bUseMask;
	m_CreateEffect.strTexMaskKey = m_strMaskTexTag;

	m_CreateEffect.bUseNoise = m_bUseNoise;
	m_CreateEffect.strTexNoiseKey = m_strNoiseTexTag;

	m_CreateEffect.iDiffuseSamplerType = m_iDiffuseSamplerType;
	m_CreateEffect.iDiffuseUVType = m_iDiffuseUVType;

	m_CreateEffect.fDiffuseDiscardValue = m_fDiffuseDiscardValue;
	m_CreateEffect.iDiffuseDiscardArea = m_iDiffuseDiscardArea;

	m_CreateEffect.fDiffuseUVScroll = _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[1]);
	m_CreateEffect.iDiffuseUVAtlasCnt = _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]);
	m_CreateEffect.fDiffuseUVAtlasSpeed = m_fDiffuseUVAtlasSpeed;

	m_CreateEffect.iMaskSamplerType = m_iMaskSamplerType;
	m_CreateEffect.iMaskUVType = m_iMaskUVType;

	m_CreateEffect.fMaskDiscardValue = m_fMaskDiscardValue;
	m_CreateEffect.iMaskDiscardArea = m_iMaskDiscardArea;

	m_CreateEffect.fMaskUVScroll = _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[1]);
	m_CreateEffect.iMaskUVAtlasCnt = _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]);
	m_CreateEffect.fMaskUVAtlasSpeed = m_fMaskUVAtlasSpeed;

	m_CreateEffect.iNoiseSamplerType = m_iNoiseSamplerType;
	m_CreateEffect.iNoiseUVType = m_iNoiseUVType;

	m_CreateEffect.fNoiseDiscardValue = m_fNoiseDiscardValue;
	m_CreateEffect.iNoiseDiscardArea = m_iNoiseDiscardArea;

	m_CreateEffect.fNoiseUVScroll = _float2(m_fNoiseUVScroll[0], m_fNoiseUVScroll[1]);
	m_CreateEffect.iNoiseUVAtlasCnt = _float2((_float)m_iNoiseUVAtlasCnt[0], (_float)m_iNoiseUVAtlasCnt[1]);
	m_CreateEffect.fNoiseUVAtlasSpeed = m_fNoiseUVAtlasSpeed;

	m_CreateEffect.bUseGlowColor = m_bGlowUseColor;
	CEffectMesh::GLOWCOLOR_KEYFRAME		GlowColorKeyFrame;
	m_CreateEffect.KeyFrames.GlowColorKeyFrames.clear();
	m_CreateEffect.iGlowColorSetCnt = m_iGlowColorSetCnt;

	for (int i = 0; i < m_iGlowColorSetCnt; ++i)
	{
		GlowColorKeyFrame.fTime = m_fGlowColorTime[i];
		GlowColorKeyFrame.vGlowColor = _float4(m_fGlowColor[i][0], m_fGlowColor[i][1], m_fGlowColor[i][2], m_fGlowColor[i][3]);

		m_CreateEffect.KeyFrames.GlowColorKeyFrames.push_back(GlowColorKeyFrame);
	}

	m_CreateEffect.bUseDistortion = m_bUseDistortion;
	m_CreateEffect.fDistortionSpeed = m_fDistortionSpeed;
	m_CreateEffect.fDistortionWeight = m_fDistortionWeight;
	m_CreateEffect.strDistortionKey = m_strDistortionTexTag;

	m_CreateEffect.bUseDslv = m_bUseDslv;
	m_CreateEffect.bUseDslv = m_bUseDslv;
	m_CreateEffect.iDslvType = m_iDslvType;	// 0 : Dslv In, 1 : Dslv Out
	m_CreateEffect.bDslvGlow = m_bDslvGlow;
	m_CreateEffect.fDslvStartTime = m_fDslvStartTime;
	m_CreateEffect.fDslvDuration = m_fDslvDuration;
	m_CreateEffect.fDslvThick = m_fDslvThick;
	m_CreateEffect.vDslvColor = _float4(m_vDslvColor[0], m_vDslvColor[1], m_vDslvColor[2], m_vDslvColor[3]);
	m_CreateEffect.vDslvGlowColor = _float4(m_vDslvGlowColor[0], m_vDslvGlowColor[1], m_vDslvGlowColor[2], m_vDslvGlowColor[3]);
	m_CreateEffect.strDslvTexKey = m_strDslvTexKey;

	m_CreateEffect.bUseColor = m_bUseColor;
	m_CreateEffect.iColorSetCnt = m_iColorSetCnt;
	m_CreateEffect.iColorSplitArea = m_iColorSplitArea;
	m_CreateEffect.vColorSplitter = _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]);

	CEffectMesh::COLOR_KEYFRAME     ColorKeyFrame;
	m_CreateEffect.KeyFrames.ColorKeyFrames.clear();

	for (int i = 0; i < m_iColorSetCnt; ++i)
	{
		ColorKeyFrame.fTime = m_fColorTime[i];
		ColorKeyFrame.eEaseType = m_iColorEase[i];
		memcpy(ColorKeyFrame.vColor, m_vColor[i], sizeof(_float4[4]));

		m_CreateEffect.KeyFrames.ColorKeyFrames.push_back(ColorKeyFrame);
	}

	m_CreateEffect.iScaleFollowType = m_bScaleFollow ? m_iScaleFollowType : -1;
	m_CreateEffect.strScaleFollowBoneKey = m_strScaleFollowBoneKey;
	m_CreateEffect.iScaleSetCnt = m_iScaleCnt;

	// Scale KeyFrames 정보 저장
	CEffectMesh::SCALE_KEYFRAME     ScaleKeyFrame;
	m_CreateEffect.KeyFrames.ScaleKeyFrames.clear();

	for (int i = 0; i < m_iScaleCnt; ++i)
	{
		ScaleKeyFrame.fTime = m_fScaleTime[i];
		ScaleKeyFrame.eEaseType = m_iScaleEasing[i];
		ScaleKeyFrame.vScale.x = m_fScale[i][0];
		ScaleKeyFrame.vScale.y = m_fScale[i][1];
		ScaleKeyFrame.vScale.z = m_fScale[i][2];

		m_CreateEffect.KeyFrames.ScaleKeyFrames.push_back(ScaleKeyFrame);
	}

	m_CreateEffect.iRotFollowType = m_bRotationFollow ? m_iRotationFollowType : -1;
	m_CreateEffect.strRotFollowBoneKey = m_strRotationFollowBoneKey;

	m_CreateEffect.vRotation = _float3(XMConvertToRadians(m_fRotate[0]), XMConvertToRadians(m_fRotate[1]), XMConvertToRadians(m_fRotate[2]));
	m_CreateEffect.vTurnVel = _float3(XMConvertToRadians(m_fTurnVel[0]), XMConvertToRadians(m_fTurnVel[1]), XMConvertToRadians(m_fTurnVel[2]));
	m_CreateEffect.vTurnAcc = _float3(XMConvertToRadians(m_fTurnAcc[0]), XMConvertToRadians(m_fTurnAcc[1]), XMConvertToRadians(m_fTurnAcc[2]));

	m_CreateEffect.iTransFollowType = m_bTranslationFollow ? m_iTranslationFollowType : -1;
	m_CreateEffect.strTransFollowBoneKey = m_strTranslationFollowBoneKey;
	m_CreateEffect.iTransSetCnt = m_iTranslationCnt;

	// Scale KeyFrames 정보 저장
	CEffectMesh::TRANS_KEYFRAME     TransKeyFrame;
	m_CreateEffect.KeyFrames.TransKeyFrames.clear();

	for (int i = 0; i < m_iTranslationCnt; ++i)
	{
		TransKeyFrame.fTime = m_fTranslationTime[i];
		TransKeyFrame.eEaseType = m_iTranslationEasing[i];
		TransKeyFrame.vTranslation.x = m_fTranslation[i][0];
		TransKeyFrame.vTranslation.y = m_fTranslation[i][1];
		TransKeyFrame.vTranslation.z = m_fTranslation[i][2];

		m_CreateEffect.KeyFrames.TransKeyFrames.push_back(TransKeyFrame);
	}

	return m_CreateEffect;
}

shared_ptr<CEffectMesh> CWindowMesh::CreateEffect()
{
	CreateEffectDesc();

	shared_ptr<CEffectMesh>     pEffect = CEffectMesh::Create(&m_CreateEffect);

	/*if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_BackGround"), pEffect)))
		return nullptr;*/

	if (m_pTestModel)
		m_pTestModel->SetActive(false);

	return pEffect;
}

void CWindowMesh::CreateTestEffect(CEffectMesh::EffectMeshDesc* _pEffectDesc, shared_ptr<CGameObject> _pOwner)
{

	m_pFollowObj = dynamic_pointer_cast<CAnimModel>(_pOwner);
	m_pFollowObjModel = dynamic_pointer_cast<CModel>(_pOwner->GetComponent(L"Com_Model"));

	m_pFollowObjBones = m_pFollowObjModel->GetBones();
	m_vecFollowBoneNames.clear();

	for (auto pBones : m_pFollowObjBones)
	{
		const _char* pName = pBones->GetBoneName();
		m_vecFollowBoneNames.push_back(pName);
	}

	m_fDuration = _pEffectDesc->fDuration;
	m_StartTrackPos = _pEffectDesc->StartTrackPos;
	m_iShaderType = ESHADER_TYPE::ST_EFFECTMESH;
	m_iShaderPass = _pEffectDesc->iShaderPass;
	m_iRenderGroup = _pEffectDesc->iRenderGroup;
	m_bLoop = _pEffectDesc->bLoop;
	m_bBillBoard = _pEffectDesc->bBillBoard;


	m_pTestModel = nullptr;
	m_strModelKey = _pEffectDesc->strModelKey;

	string	strModelKey = m_strModelKey;
	_bool	bFindModel = false;

	auto it = std::find_if(m_ModelList[0].begin(), m_ModelList[0].end(), [strModelKey](string element) {
		return element == strModelKey;
		});

	if (it != m_ModelList[0].end()) {
		m_iSelectedMeshIdx = std::distance(m_ModelList[0].begin(), it);
		bFindModel = true;
	}
	else {
		bFindModel = false; // 일치하는 요소를 찾지 못한 경우 -1 반환
	}

	if (!bFindModel)
	{
		auto it = std::find_if(m_ModelList[1].begin(), m_ModelList[1].end(), [strModelKey](string element) {
			return element == strModelKey;
			});

		if (it != m_ModelList[1].end()) {
			m_iSelectedMeshIdx = std::distance(m_ModelList[1].begin(), it);
			bFindModel = true;
		}
		else {
			bFindModel = false; // 일치하는 요소를 찾지 못한 경우 -1 반환
		}
	}

	if (!bFindModel)
	{
		auto it = std::find_if(m_ModelList[2].begin(), m_ModelList[2].end(), [strModelKey](string element) {
			return element == strModelKey;
			});

		if (it != m_ModelList[2].end()) {
			m_iSelectedMeshIdx = std::distance(m_ModelList[2].begin(), it);
			bFindModel = true;
		}
		else {
			m_iSelectedMeshIdx = -1; // 일치하는 요소를 찾지 못한 경우 -1 반환
		}
	}

	m_bUseDiffuse = _pEffectDesc->bUseDiffuse;
	m_strDiffuseTexTag = _pEffectDesc->strTexDiffuseKey;

	m_iDiffuseSamplerType = _pEffectDesc->iDiffuseSamplerType;
	m_iDiffuseUVType = _pEffectDesc->iDiffuseUVType;

	m_fDiffuseDiscardValue = _pEffectDesc->fDiffuseDiscardValue;
	m_iDiffuseDiscardArea = _pEffectDesc->iDiffuseDiscardArea;

	m_fDiffuseUVScroll[0] = _pEffectDesc->fDiffuseUVScroll.x;
	m_fDiffuseUVScroll[1] = _pEffectDesc->fDiffuseUVScroll.y;

	m_iDiffuseUVAtlasCnt[0] = _pEffectDesc->iDiffuseUVAtlasCnt.x;
	m_iDiffuseUVAtlasCnt[1] = _pEffectDesc->iDiffuseUVAtlasCnt.y;

	m_fDiffuseUVAtlasSpeed = _pEffectDesc->fDiffuseUVAtlasSpeed;


	m_bUseMask = _pEffectDesc->bUseMask;
	m_strMaskTexTag = _pEffectDesc->strTexMaskKey;

	m_iMaskSamplerType = _pEffectDesc->iMaskSamplerType;
	m_iMaskUVType = _pEffectDesc->iMaskUVType;

	m_fMaskDiscardValue = _pEffectDesc->fMaskDiscardValue;
	m_iMaskDiscardArea = _pEffectDesc->iMaskDiscardArea;

	m_fMaskUVScroll[0] = _pEffectDesc->fMaskUVScroll.x;
	m_fMaskUVScroll[1] = _pEffectDesc->fMaskUVScroll.y;

	m_iMaskUVAtlasCnt[0] = _pEffectDesc->iMaskUVAtlasCnt.x;
	m_iMaskUVAtlasCnt[1] = _pEffectDesc->iMaskUVAtlasCnt.y;

	m_fMaskUVAtlasSpeed = _pEffectDesc->fMaskUVAtlasSpeed;


	m_bUseNoise = _pEffectDesc->bUseNoise;
	m_strNoiseTexTag = _pEffectDesc->strTexNoiseKey;

	m_iNoiseSamplerType = _pEffectDesc->iNoiseSamplerType;
	m_iNoiseUVType = _pEffectDesc->iNoiseUVType;

	m_fNoiseDiscardValue = _pEffectDesc->fNoiseDiscardValue;
	m_iNoiseDiscardArea = _pEffectDesc->iNoiseDiscardArea;

	m_fNoiseUVScroll[0] = _pEffectDesc->fNoiseUVScroll.x;
	m_fNoiseUVScroll[1] = _pEffectDesc->fNoiseUVScroll.y;

	m_iNoiseUVAtlasCnt[0] = _pEffectDesc->iNoiseUVAtlasCnt.x;
	m_iNoiseUVAtlasCnt[1] = _pEffectDesc->iNoiseUVAtlasCnt.y;
	m_fNoiseUVAtlasSpeed = _pEffectDesc->fNoiseUVAtlasSpeed;

	m_bUseColor = _pEffectDesc->bUseColor;
	m_iColorSetCnt = _pEffectDesc->iColorSetCnt;
	m_iColorSplitArea = _pEffectDesc->iColorSplitArea;
	m_vColorSplitter[0] = _pEffectDesc->vColorSplitter.x;
	m_vColorSplitter[1] = _pEffectDesc->vColorSplitter.y;
	m_vColorSplitter[2] = _pEffectDesc->vColorSplitter.z;
	m_vColorSplitter[3] = _pEffectDesc->vColorSplitter.w;

	for (int i = 0; i < m_iColorSetCnt; ++i)
	{
		m_fColorTime[i] = _pEffectDesc->KeyFrames.ColorKeyFrames[i].fTime;
		m_iColorEase[i] = _pEffectDesc->KeyFrames.ColorKeyFrames[i].eEaseType;
		memcpy(m_vColor[i], _pEffectDesc->KeyFrames.ColorKeyFrames[i].vColor, sizeof(_float4[4]));
	}

	m_bScaleFollow = (_pEffectDesc->iScaleFollowType != -1);
	m_iScaleFollowType = _pEffectDesc->iScaleFollowType;	// 0 : object / 1 : bone
	m_strScaleFollowBoneKey = _pEffectDesc->strScaleFollowBoneKey;

	string strScaleFollowBoneKey = m_strScaleFollowBoneKey;

	auto itScale = std::find_if(m_vecFollowBoneNames.begin(), m_vecFollowBoneNames.end(), [strScaleFollowBoneKey](const _char* element) {
		return strcmp(element, strScaleFollowBoneKey.c_str()) == 0;
		});

	if (itScale != m_vecFollowBoneNames.end()) {
		m_pScaleFollowBoneIdx = std::distance(m_vecFollowBoneNames.begin(), itScale);
	}

	else {
		m_pScaleFollowBoneIdx = -1; // 일치하는 요소를 찾지 못한 경우 -1 반환
	}

	m_iScaleCnt = _pEffectDesc->KeyFrames.ScaleKeyFrames.size();

	for (int i = 0; i < m_iScaleCnt; ++i)
	{
		m_fScaleTime[i] = _pEffectDesc->KeyFrames.ScaleKeyFrames[i].fTime;
		m_iScaleEasing[i] = _pEffectDesc->KeyFrames.ScaleKeyFrames[i].eEaseType;
		m_fScale[i][0] = _pEffectDesc->KeyFrames.ScaleKeyFrames[i].vScale.x;
		m_fScale[i][1] = _pEffectDesc->KeyFrames.ScaleKeyFrames[i].vScale.y;
		m_fScale[i][2] = _pEffectDesc->KeyFrames.ScaleKeyFrames[i].vScale.z;
	}

	m_bRotationFollow = (_pEffectDesc->iRotFollowType != -1);
	m_iRotationFollowType = _pEffectDesc->iRotFollowType;
	m_strRotationFollowBoneKey = _pEffectDesc->strRotFollowBoneKey;

	string strRotationFollowBoneKey = m_strRotationFollowBoneKey;

	auto itRotation = std::find_if(m_vecFollowBoneNames.begin(), m_vecFollowBoneNames.end(), [strRotationFollowBoneKey](const _char* element) {
		return strcmp(element, strRotationFollowBoneKey.c_str()) == 0;
		});

	if (itScale != m_vecFollowBoneNames.end()) {
		m_pRotationFollowBoneIdx = std::distance(m_vecFollowBoneNames.begin(), itRotation);
	}

	else {
		m_pRotationFollowBoneIdx = -1; // 일치하는 요소를 찾지 못한 경우 -1 반환
	}
	;
	m_fRotate[0] = XMConvertToDegrees(_pEffectDesc->vRotation.x);
	m_fRotate[1] = XMConvertToDegrees(_pEffectDesc->vRotation.y);
	m_fRotate[2] = XMConvertToDegrees(_pEffectDesc->vRotation.z);

	m_fTurnVel[0] = XMConvertToDegrees(_pEffectDesc->vTurnVel.x);
	m_fTurnVel[1] = XMConvertToDegrees(_pEffectDesc->vTurnVel.y);
	m_fTurnVel[2] = XMConvertToDegrees(_pEffectDesc->vTurnVel.z);

	m_fTurnAcc[0] = XMConvertToDegrees(_pEffectDesc->vTurnAcc.x);
	m_fTurnAcc[1] = XMConvertToDegrees(_pEffectDesc->vTurnAcc.y);
	m_fTurnAcc[2] = XMConvertToDegrees(_pEffectDesc->vTurnAcc.z);

	m_iTranslationCnt = _pEffectDesc->KeyFrames.TransKeyFrames.size();
	m_bTranslationFollow = (_pEffectDesc->iTransFollowType != -1);
	m_iTranslationFollowType = _pEffectDesc->iTransFollowType;

	m_strTranslationFollowBoneKey = _pEffectDesc->strTransFollowBoneKey;

	string strTranslationFollowBoneKey = m_strTranslationFollowBoneKey;

	auto itTrans = std::find_if(m_vecFollowBoneNames.begin(), m_vecFollowBoneNames.end(), [strTranslationFollowBoneKey](const _char* element) {
		return strcmp(element, strTranslationFollowBoneKey.c_str()) == 0;
		});

	if (itScale != m_vecFollowBoneNames.end()) {
		m_pTranslationFollowBoneIdx = std::distance(m_vecFollowBoneNames.begin(), itRotation);
	}

	else {
		m_pTranslationFollowBoneIdx = -1; // 일치하는 요소를 찾지 못한 경우 -1 반환
	}

	for (int i = 0; i < m_iTranslationCnt; ++i)
	{
		m_fTranslationTime[i] = _pEffectDesc->KeyFrames.TransKeyFrames[i].fTime;
		m_iTranslationEasing[i] = _pEffectDesc->KeyFrames.TransKeyFrames[i].eEaseType;
		m_fTranslation[i][0] = _pEffectDesc->KeyFrames.TransKeyFrames[i].vTranslation.x;
		m_fTranslation[i][1] = _pEffectDesc->KeyFrames.TransKeyFrames[i].vTranslation.y;
		m_fTranslation[i][2] = _pEffectDesc->KeyFrames.TransKeyFrames[i].vTranslation.z;
	}

	m_bUseSoftEffect = (_pEffectDesc->iRenderGroup == CRenderer::RENDER_BLUR || _pEffectDesc->iRenderGroup == CRenderer::RENDER_GLOW);;
	m_iSoftEffectType = _pEffectDesc->iRenderGroup == CRenderer::RENDER_BLUR ? 1 : 0;	// 0 : Glow, 1 : Blur

	m_bGlowUseColor = _pEffectDesc->bUseGlowColor;

	m_iGlowColorSetCnt = _pEffectDesc->iGlowColorSetCnt;

	for (int i = 0; i < m_iGlowColorSetCnt; ++i)
	{
		m_fGlowColorTime[i] = _pEffectDesc->KeyFrames.GlowColorKeyFrames[i].fTime;
		m_fGlowColor[i][0] = _pEffectDesc->KeyFrames.GlowColorKeyFrames[i].vGlowColor.x;
		m_fGlowColor[i][1] = _pEffectDesc->KeyFrames.GlowColorKeyFrames[i].vGlowColor.y;
		m_fGlowColor[i][2] = _pEffectDesc->KeyFrames.GlowColorKeyFrames[i].vGlowColor.z;
		m_fGlowColor[i][3] = _pEffectDesc->KeyFrames.GlowColorKeyFrames[i].vGlowColor.w;
	}

	m_bUseDistortion = _pEffectDesc->bUseDistortion;
	m_strDistortionTexTag = _pEffectDesc->strDistortionKey;
	m_fDistortionSpeed = _pEffectDesc->fDistortionSpeed;
	m_fDistortionWeight = _pEffectDesc->fDistortionWeight;

	/*m_bUseDissolve = false;
	m_strDissolveTexTag = "";
	m_fDissolveStartTime = 0.f;
	m_fDissolveSpeed = 0.f;
	m_bDissolveGlow = false;
	m_vDissolveColor1 = { 0.f, 0.f, 0.f, 0.f };
	m_vDissolveColor2 = { 0.f, 0.f, 0.f, 0.f };*/

	GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_TestEffect"));

	m_pTestEffectMesh = CTestEffectMesh::Create(_pEffectDesc->strModelKey);

	if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_TestEffect"), m_pTestEffectMesh)))
		return;

	m_pTestEffectMesh->SetOwner(_pOwner);

	m_pTestEffectMesh->SetDefaultInfo(ST_EFFECTMESH, m_iShaderPass, m_iRenderGroup, m_fDuration, m_StartTrackPos, m_bBillBoard, m_bLoop);
	m_pTestEffectMesh->SetDiffuseInfo(m_bUseDiffuse, m_strDiffuseTexTag, m_iDiffuseSamplerType, m_iDiffuseDiscardArea, m_fDiffuseDiscardValue, m_iDiffuseUVType, _float2(m_fDiffuseUVScroll[0], m_fDiffuseUVScroll[1]), _float2((_float)m_iDiffuseUVAtlasCnt[0], (_float)m_iDiffuseUVAtlasCnt[1]), m_fDiffuseUVAtlasSpeed);

	m_pTestEffectMesh->SetColorInfo(m_bUseColor, m_iColorSplitArea, _float4(m_vColorSplitter[0], m_vColorSplitter[1], m_vColorSplitter[2], m_vColorSplitter[3]),
		m_iColorSetCnt, m_fColorTime, m_iColorEase, m_vColor);	m_pTestEffectMesh->SetMaskInfo(m_bUseMask, m_strMaskTexTag, m_iMaskSamplerType, m_iMaskDiscardArea, m_fMaskDiscardValue, m_iMaskUVType, _float2(m_fMaskUVScroll[0], m_fMaskUVScroll[1]), _float2((_float)m_iMaskUVAtlasCnt[0], (_float)m_iMaskUVAtlasCnt[1]), m_fMaskUVAtlasSpeed);
	m_pTestEffectMesh->SetNoiseInfo(m_bUseNoise, m_strNoiseTexTag, m_iNoiseSamplerType, m_iNoiseDiscardArea, m_fNoiseDiscardValue, m_iNoiseUVType, _float2(m_fNoiseUVScroll[0], m_fNoiseUVScroll[1]), _float2((_float)m_iNoiseUVAtlasCnt[0], (_float)m_iNoiseUVAtlasCnt[1]), m_fNoiseUVAtlasSpeed);


	m_pTestEffectMesh->SetScaleInfo(m_bScaleFollow, m_iScaleFollowType, m_strScaleFollowBoneKey, m_iScaleCnt, m_fScaleTime, m_fScale, m_iScaleEasing);
	m_pTestEffectMesh->SetRotInfo(m_bRotationFollow, m_iRotationFollowType, m_strRotationFollowBoneKey, _float3(m_fRotate[0], m_fRotate[1], m_fRotate[2]), _float3(m_fTurnVel[0], m_fTurnVel[1], m_fTurnVel[2]), _float3(m_fTurnAcc[0], m_fTurnAcc[1], m_fTurnAcc[2]));

	m_pTestEffectMesh->SetTransInfo(m_bTranslationFollow, m_iTranslationFollowType, m_strTranslationFollowBoneKey, m_iTranslationCnt, m_fTranslationTime, m_fTranslation, m_iTranslationEasing);

	m_pTestEffectMesh->SetUseSoftShader(m_bUseSoftEffect);
	m_pTestEffectMesh->SetShaderGlowInfo(m_bGlowUseColor, m_iGlowColorSetCnt, m_fGlowColorTime, m_fGlowColor);
	m_pTestEffectMesh->SetDistiortion(m_bUseDistortion, m_strDistortionTexTag, m_fDistortionSpeed, m_fDistortionWeight);

}

void CWindowMesh::KeyInput()
{
	if (GAMEINSTANCE->KeyDown(DIK_9))
	{
		//CAnimEffect::ANIMEFFECT_DESC eAnimEffectDesc;

		//eAnimEffectDesc.strModelKey = "Crash_Rock";
		////eAnimEffectDesc.strModelKey = "Gaia_Crush";
		//eAnimEffectDesc.vRotation = { 0.f, 0.f, 0.f };
		//eAnimEffectDesc.bUseDistortion = false;
		//eAnimEffectDesc.strDistortionKey = "";
		//eAnimEffectDesc.fDistortionSpeed = 1.f;
		//eAnimEffectDesc.fDistortionWeight = 1.f;
		//eAnimEffectDesc.bUseGlow = false;
		//eAnimEffectDesc.bUseGlowColor = false;
		//eAnimEffectDesc.vGlowColor = { 0.f, 0.f, 0.f, 0.f };
		//eAnimEffectDesc.bUseDslv = false;
		//eAnimEffectDesc.iDslvType = 0;  // 0 : Dslv In, 1 : Dslv Out
		//eAnimEffectDesc.fDslvStartTime = 0.f;
		//eAnimEffectDesc.fDslvDuration = 0.f;
		//eAnimEffectDesc.fDslvThick = 1.f;
		//eAnimEffectDesc.vDslvColor = { 1.f, 1.f, 1.f, 1.f };
		//eAnimEffectDesc.strDslvTexKey = "";

		//m_pAnimEffect->SetAnimEffectDesc(eAnimEffectDesc);
		//m_pAnimEffect->SetEnable(true);

		CAnimEffect::ANIMEFFECT_DESC eAnimEffectDesc;

		eAnimEffectDesc.strModelKey = "Gaia_Crush";
		//eAnimEffectDesc.strModelKey = "Gaia_Crush";
		eAnimEffectDesc.vRotation = { 0.f, -1.57f, 0.f };
		eAnimEffectDesc.bUseDistortion = false;
		eAnimEffectDesc.strDistortionKey = "";
		eAnimEffectDesc.fDistortionSpeed = 1.f;
		eAnimEffectDesc.fDistortionWeight = 1.f;
		eAnimEffectDesc.bUseGlow = false;
		eAnimEffectDesc.bUseGlowColor = false;
		eAnimEffectDesc.vGlowColor = { 0.f, 0.f, 0.f, 0.f };
		eAnimEffectDesc.bUseDslv = false;
		eAnimEffectDesc.iDslvType = 0;  // 0 : Dslv In, 1 : Dslv Out
		eAnimEffectDesc.fDslvStartTime = 0.f;
		eAnimEffectDesc.fDslvDuration = 0.f;
		eAnimEffectDesc.fDslvThick = 1.f;
		eAnimEffectDesc.vDslvColor = { 1.f, 1.f, 1.f, 1.f };
		eAnimEffectDesc.strDslvTexKey = "";
		
		m_pAnimEffect->SetAnimEffectDesc(eAnimEffectDesc);
		m_pAnimEffect->SetEnable(true);
		m_pAnimEffect->GetTransform()->SetScaling({ 7.f, 3.f, 4.f });
		m_pAnimEffect->GetTransform()->SetState(CTransform::STATE_POSITION, { 3.5f, 0.f, 0.f });
	}
}

HRESULT CWindowMesh::LoadResourceNames(const wstring& _strResourcePath, MODEL_TYPE _eModelType, TEX_TYPE _eTexType)
{
	filesystem::path basePath(_strResourcePath);

	for (const auto& entry : filesystem::directory_iterator(basePath)) {
		/*if. File : .png 라면 texture Create*/
		if (entry.is_regular_file()) {
			if (".png" == entry.path().extension()) {
				if (MODEL_ANIM == _eModelType)
				{
					m_TextureList[TEX_DIFFUSE][m_iTextureIdx[TEX_DIFFUSE]] = entry.path().filename().stem().string();
					m_iTextureIdx[TEX_DIFFUSE]++;
					continue;
				}
				m_TextureList[_eTexType][m_iTextureIdx[_eTexType]] = entry.path().filename().stem().string();
				m_iTextureIdx[_eTexType]++;
			}

			else if (".dat" == entry.path().extension()) {
				m_ModelList[_eModelType].push_back(entry.path().filename().stem().string());
			}
		}
		/*if. Directory : 계속 다음 폴더를 순회*/
		else if (entry.is_directory()) {

			const wstring& strDirName = entry.path().filename();

			if (strDirName == L"." || strDirName == L"..")
				continue;

			else if (L"Mask" == strDirName)
				LoadResourceNames(entry.path(), MODEL_END, TEX_MASK);

			else if (L"Noise" == strDirName)
				LoadResourceNames(entry.path(), MODEL_END, TEX_NOISE);

			else if (L"Diffuse" == strDirName)
				LoadResourceNames(entry.path(), MODEL_END, TEX_DIFFUSE);

			else if (L"Mesh" == strDirName || L"NonAnim" == strDirName)
				LoadResourceNames(entry.path(), MODEL_END, TEX_END);

			else if (L"Basic" == strDirName)
				LoadResourceNames(entry.path(), MODEL_BASIC, TEX_END);

			else if (L"Unique" == strDirName)
				LoadResourceNames(entry.path(), MODEL_UNIQUE, TEX_END);

			else if (L"Anim" == strDirName)
				LoadResourceNames(entry.path(), MODEL_ANIM, TEX_END);

			else if (L"Other" == strDirName)
				LoadResourceNames(entry.path(), MODEL_OTHER, TEX_END);

			else if (L"Other2" == strDirName)
				LoadResourceNames(entry.path(), MODEL_OTHER2, TEX_END);
		}
	}

	return S_OK;
}