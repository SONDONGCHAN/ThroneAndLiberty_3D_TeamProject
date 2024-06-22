#include "ImGuiMgr.h"
#include "GameInstance.h"

#include "WindowMesh.h"
#include "WindowPlay.h"
#include "WindowModel.h"
#include "WindowParticle.h"

#include "EffectGroup.h"

#include "Effect.h"

IMPLEMENT_SINGLETON(CImGuiMgr)

CImGuiMgr::CImGuiMgr()
{
}

CImGuiMgr::~CImGuiMgr()
{
	ShutDown();
}

HRESULT CImGuiMgr::Initialize()
{
	m_pDevice = GAMEINSTANCE->GetDeviceInfo();
	m_pContext = GAMEINSTANCE->GetDeviceContextInfo();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsClassic();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get());

	m_pWindowMesh = make_shared<CWindowMesh>();
	m_pWindowMesh->Initialize();

	m_pWindowPlay = make_shared<CWindowPlay>();
	m_pWindowPlay->Initialize();

	m_pWindowModel = make_shared<CWindowModel>();
	m_pWindowModel->Initialize();

	m_pWindowParticle = make_shared<CWindowParticle>();
	m_pWindowParticle->Initialize();

	// 이펙트 그룹 목록 읽어오기
	LoadEffectGroupList();

	// 툴 로드용 이펙트 그룹
	m_pLoadEffectGroup = CEffectGroup::Create(EFFECT_ATTACH_FREE);
	m_pLoadEffectGroup->SetPlaying(false);

	if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_Effect"), m_pLoadEffectGroup)))
		return E_FAIL;

	return S_OK;
}

void CImGuiMgr::Tick(_float _fTimeDelta)
{
	KeyInput(_fTimeDelta);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	TickImGui(_fTimeDelta);

	ImGui::EndFrame();
}

HRESULT CImGuiMgr::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

void CImGuiMgr::TickImGui(_float _fTimeDelta)
{
	if (m_bShowSetting)
		TickSetting(_fTimeDelta);

	if (m_bShowPlayWindow)
		TickPlayWindow(_fTimeDelta);

	TickModelWindow(_fTimeDelta);
	TickMainWindow(_fTimeDelta);
}

void CImGuiMgr::TickSetting(_float _fTimeDelta)
{
	ImGui::SetNextWindowSizeConstraints(ImVec2(400, 30), ImVec2(400, 700));

	if (ImGui::Begin("Setting", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Cam Pos : (%g, %g, %g)", GAMEINSTANCE->GetCamPosition().x, GAMEINSTANCE->GetCamPosition().y, GAMEINSTANCE->GetCamPosition().z);
		ImGui::Text("Cam Look : (%g, %g, %g)", GAMEINSTANCE->GetCamLook().x, GAMEINSTANCE->GetCamLook().y, GAMEINSTANCE->GetCamLook().z);

		ImGuiIO& io = ImGui::GetIO();

		ImGui::Text("Mouse Pos  : (%g, %g)", io.MousePos.x, io.MousePos.y);

		if (ImGui::CollapsingHeader("Style"))
		{
			ImGui::ShowStyleEditor();
		}

	} ImGui::End();

	if (ImGui::Begin("Debug Option"))
	{
		ImGui::Text("Cam Pos : (%.2g, %.2g, %.2g)", GAMEINSTANCE->GetCamPosition().x, GAMEINSTANCE->GetCamPosition().y, GAMEINSTANCE->GetCamPosition().z);

		ImGui::Text("Debug Option On/Off : `");

		ImGui::SeparatorText("0. Render Target");

		_bool prevTargetVisible = m_bTargetVisible;

		ImGui::Checkbox("Render Target On", &m_bTargetVisible);

		if (m_bTargetVisible != prevTargetVisible) {
			GAMEINSTANCE->DebugRenderSwitch();
		}

		_int prevGroupIdx = m_iTargetNum;

		ImGui::RadioButton("Group1(Deferred)", &m_iTargetNum, 0);
		ImGui::RadioButton("Group2(PostProcess)", &m_iTargetNum, 1);

		if (prevGroupIdx != m_iTargetNum) {
			GAMEINSTANCE->SetRenderGourpIdx(m_iTargetNum);
		}

		if (ImGui::Checkbox("HDR On", &m_IsHDROn))
		{
			GAMEINSTANCE->SwitchingHDROption(m_IsHDROn);
		}

		ImGui::Dummy(ImVec2(2.f, 2.f));
	} ImGui::End();
}

void CImGuiMgr::TickMainWindow(_float _fTimeDelta)
{
	ImGui::SetNextWindowSizeConstraints(ImVec2(370, 100), ImVec2(370, 700));

	if (ImGui::Begin("Effect Tool", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::CollapsingHeader("Effect List"))
		{
			ImVec2 child_size = ImVec2(0, 0.f);
			ImGui::BeginChild("##ScrollingRegion", child_size, ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY);

			TickListWindow(_fTimeDelta);

			ImGui::EndChild();
		}

		if (ImGui::CollapsingHeader("Create Effect"))
		{
			/*_int prevAttachType = m_iEffectAttachType;

			ImGui::RadioButton("Attach AnimModel", &m_iEffectAttachType, 0);
			ImGui::RadioButton("Attach NonAnimModel", &m_iEffectAttachType, 1);
			ImGui::RadioButton("No Attach", &m_iEffectAttachType, 2);

			if (prevAttachType != m_iEffectAttachType)
			{
				if (m_pWindowModel->GetCurModelObj())
				{
					m_pLoadEffectGroup->SetAttachType((EFFECT_ATTACH_TYPE)m_iEffectAttachType);
				}
				else
					m_pLoadEffectGroup->SetAttachType(EFFECT_ATTACH_FREE);
			}*/

			ImGui::BeginGroup();
			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar("EffectTabBar", tab_bar_flags))
			{
				if (ImGui::BeginTabItem("Mesh"))
				{
					ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 0.f), ImVec2(ImGui::GetContentRegionAvail().x, 500.f));
					ImGui::BeginChild("Properties_Mesh", ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar);

					TickMesh(_fTimeDelta);

					m_iCreateEffectType = CEffect::EFFECT_MESH;

					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Texture"))
				{
					ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 0.f), ImVec2(ImGui::GetContentRegionAvail().x, 500.f));
					//ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeightWithSpacing() * 1), ImVec2(ImGui::GetContentRegionAvail().x, 550.f));
					ImGui::BeginChild("Properties_Texture", ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar);

					TickTexture(_fTimeDelta);

					m_iCreateEffectType = -1;

					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Particle"))
				{
					ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, 0.f), ImVec2(ImGui::GetContentRegionAvail().x, 500.f));
					//ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeightWithSpacing() * 1), ImVec2(ImGui::GetContentRegionAvail().x, 550.f));
					ImGui::BeginChild("Properties_Particle", ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar);

					TickParticle(_fTimeDelta);

					m_iCreateEffectType = CEffect::EFFECT_PARTICLE;

					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();

				ImGui::BeginChild("Create_Effect", ImVec2(ImGui::GetContentRegionAvail().x, 75), ImGuiChildFlags_Border);

				ImGui::Dummy(ImVec2(0.f, 2.f));
				ImGui::Dummy(ImVec2(2.f, 0.f)); ImGui::SameLine();
				ImGui::Text("Effect Key : "); ImGui::SameLine();
				ImGui::SetNextItemWidth(220.f);
				ImGui::InputText("##Effect Name", m_strEffectName, IM_ARRAYSIZE(m_strEffectName));

				ImGui::Dummy(ImVec2(0.f, 3.f));

				_float  fWidth = ImGui::GetContentRegionAvail().x;
				_float  fBlank = 10.f;
				_uint   iNumButton = 3;

				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4.f / 7.0f, 0.6f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4.f / 7.0f, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4.f / 7.0f, 0.8f, 0.8f));
				if (ImGui::Button("Add to Play", ImVec2((fWidth - fBlank * 2.f) / _float(iNumButton), 20.f)))
				{
					AddToPlay();
				}
				ImGui::PopStyleColor(3);

				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(5.f / 7.0f, 0.6f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(5.f / 7.0f, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(5.f / 7.0f, 0.8f, 0.8f));
				ImGui::SameLine();
				//ImGui::SameLine((fWidth - fBlank * 2.f) / _float(iNumButton) + fBlank, 0.f);
				if (ImGui::Button("Add to Effect", ImVec2((fWidth - fBlank * 2.f) / _float(iNumButton), 20.f)))
				{
					AddToEffect();
				}
				ImGui::PopStyleColor(3);

				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(6.f / 7.0f, 0.6f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(6.f / 7.0f, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(6.f / 7.0f, 0.8f, 0.8f));
				ImGui::SameLine();
				//ImGui::SameLine(((fWidth - fBlank * 2.f) / _float(iNumButton)) * 2.f + fBlank * 2.f, 0.f);
				if (ImGui::Button("Reset", ImVec2((fWidth - fBlank * 2.f) / _float(iNumButton), 20.f)))
				{
					int a = 3;
				}
				ImGui::PopStyleColor(3);

				TextModel();

				ImGui::EndChild();
			}
			ImGui::EndGroup();
		}

	} ImGui::End();
}

void CImGuiMgr::TickListWindow(_float _fTimeDelta)
{
	// 이펙트 시스템 목록
	if (ImGui::BeginListBox("이펙트 그룹", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int n = 0; n < m_strEffectGroupList.size(); n++)
		{
			const bool is_selected = (m_iSelectEffectGroupIdx == n);
			if (ImGui::Selectable(m_strEffectGroupList[n].c_str(), is_selected))
			{
				m_iSelectEffectGroupIdx = n;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndListBox();
	}

	// 버튼들
	_float  fWidth = ImGui::GetContentRegionAvail().x;
	_float  fBlank = 10.f;
	_uint   iNumButton = 3;
	{
		ImGui::BeginChild("##Effect System Buttons", ImVec2(ImGui::GetContentRegionAvail().x, 37), ImGuiChildFlags_Border);

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.8f, 0.8f));
		if (ImGui::Button("Load", ImVec2((fWidth - fBlank * 2.f) / _float(iNumButton), 20.f)))
		{
			// 해당 json 파일에 포함된 key값을 가진 이펙트들의 정보 json을 읽어서 m_pLoadEffectList에 넣어준다.
			if (m_strEffectGroupList.size() > m_iSelectEffectGroupIdx)
				LoadEffectJson(m_strEffectGroupList[m_iSelectEffectGroupIdx]);
		}
		ImGui::PopStyleColor(3);

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.f / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1.f / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1.f / 7.0f, 0.8f, 0.8f));
		ImGui::SameLine();
		//ImGui::SameLine((fWidth - fBlank * 2.f) / _float(iNumButton) + fBlank, 0.f);
		if (ImGui::Button("Delete", ImVec2((fWidth - fBlank * 2.f) / _float(iNumButton), 20.f)))
		{
			// 실제 윈도우에서 지우고
			// 성공하면 // 지우기
			m_strEffectGroupList[m_iSelectEffectGroupIdx];
		}
		ImGui::PopStyleColor(3);

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2.f / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2.f / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(2.f / 7.0f, 0.8f, 0.8f));
		ImGui::SameLine();
		//ImGui::SameLine(((fWidth - fBlank * 2.f) / _float(iNumButton)) * 2.f + fBlank * 2.f, 0.f);
		if (ImGui::Button("ReLoad", ImVec2((fWidth - fBlank * 2.f) / _float(iNumButton), 20.f)))
		{
			LoadEffectGroupList();
		}
		ImGui::PopStyleColor(3);

		ImGui::EndChild();
	}

	// 이펙트 목록
	if (ImGui::BeginListBox("이펙트 목록", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int n = 0; n < m_strEffectList.size(); n++)
		{
			const bool is_selected = (m_iSelectLoadEffectList == n);
			if (ImGui::Selectable(m_strEffectList[n].c_str(), is_selected))
			{
				m_iSelectLoadEffectList = n;
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	{
		// 버튼들
		ImGui::BeginChild("##Save Buttons", ImVec2(ImGui::GetContentRegionAvail().x, 50), ImGuiChildFlags_Border);

		iNumButton = 2;

		ImGui::Text("AttachType"); ImGui::SameLine();

		const char* items[] = { "EFFECT_ATTACH_ANIM", "EFFECT_ATTACH_NONANIM", "CEFFECT_ATTACH_FREE" };
		const char* combo_preview_value = items[m_iEffectAttachType];  // Pass in the preview value visible before opening the combo (it could be anything)
		if (ImGui::BeginCombo("##Effect_Attach_Type", combo_preview_value, ImGuiComboFlags_PopupAlignLeft))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (m_iEffectAttachType == n);
				if (ImGui::Selectable(items[n], is_selected))
				{
					m_iEffectAttachType = n;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Text("Clone :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(30.f);
		ImGui::InputInt("##CloneCnt", &m_iGroupCloneCnt, 0, 0);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(150.f);
		ImGui::InputText("##GroupName", m_strGroupFileName, IM_ARRAYSIZE(m_strGroupFileName));


		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(6.f / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(6.f / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(6.f / 7.0f, 0.8f, 0.8f));
		ImGui::SameLine();
		//ImGui::SameLine(((fWidth - fBlank * 2.f) / _float(iNumButton)) * 2.f + fBlank * 2.f, 0.f);
		if (ImGui::Button("Save", ImVec2(70.f, 20.f)))
		{
			SaveEffectJson();

			// 파싱하고 나서 reload 기능 자동 호출해서 리스트박스에 생성되도록
			LoadEffectGroupList();
		}
		ImGui::PopStyleColor(3);

		ImGui::EndChild();
	}

	{
		// 버튼들
		ImGui::BeginChild("##Effect Buttons", ImVec2(ImGui::GetContentRegionAvail().x, 37), ImGuiChildFlags_Border);

		iNumButton = 2;

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4.f / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4.f / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4.f / 7.0f, 0.8f, 0.8f));
		if (ImGui::Button("Edit", ImVec2((fWidth - fBlank) / _float(iNumButton), 20.f)))
		{
			// 그룹 재생중인거 치우고, 테스트 어쩌고에 넣어주기
			GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_Effect"));

			_int iEffectType = m_pLoadEffectGroup->GetEffects()[m_iSelectLoadEffectList]->GetEffectDesc().iEffectType;

			if (CEffect::EFFECT_MESH == iEffectType)
			{
				shared_ptr<CEffectMesh> pMeshEffect = dynamic_pointer_cast<CEffectMesh>(m_pLoadEffectGroup->GetEffects()[m_iSelectLoadEffectList]);
				CEffectMesh::EffectMeshDesc* pEffectDesc = pMeshEffect->GetMeshDesc();

				m_pWindowMesh->CreateTestEffect(pEffectDesc, m_pWindowModel->GetCurModelObj());
			}
			else if (CEffect::EFFECT_PARTICLE == iEffectType)
			{
				shared_ptr<CParticleSystem> pParticle = dynamic_pointer_cast<CParticleSystem>(m_pLoadEffectGroup->GetEffects()[m_iSelectLoadEffectList]);
				CParticleSystem::PARTICLE_SYSTEM* pEffectDesc = pParticle->GetParticleDesc();

				m_pWindowParticle->CreateTestEffect(pEffectDesc, m_pWindowModel->GetCurModelObj());
			}
		}
		ImGui::PopStyleColor(3);

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(5.f / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(5.f / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(5.f / 7.0f, 0.8f, 0.8f));
		ImGui::SameLine();
		//ImGui::SameLine((fWidth - fBlank * 2.f) / _float(iNumButton) + fBlank, 0.f);
		if (ImGui::Button("Delete", ImVec2((fWidth - fBlank) / _float(iNumButton), 20.f)))
		{
			if (m_strEffectList.size() > m_iSelectLoadEffectList)
			{
				m_pLoadEffectGroup->DeleteEffect(m_strEffectList[m_iSelectLoadEffectList].c_str());
				m_strEffectList.erase(m_strEffectList.begin() + m_iSelectLoadEffectList);
			}
		}
		ImGui::PopStyleColor(3);

		ImGui::EndChild();
	}
}

void CImGuiMgr::TickPlayWindow(_float _fTimeDelta)
{
	ImGui::SetNextWindowSizeConstraints(ImVec2(700, 200), ImVec2(700, 200));

	if (ImGui::Begin("Play Window", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		m_pWindowPlay->Tick(_fTimeDelta);

	} ImGui::End();
}

void CImGuiMgr::TickModelWindow(_float _fTimeDelta)
{
	m_pWindowModel->Tick(_fTimeDelta);
}

void CImGuiMgr::TickMesh(_float _fTimeDelta)
{
	m_pWindowMesh->Tick(_fTimeDelta);
	/*for (int n = 0; n < 100; n++)
		ImGui::Text("Line %04d", n);*/
}

void CImGuiMgr::TickTexture(_float _fTimeDelta)
{
	for (int n = 0; n < 20; n++)
		ImGui::Text("Line %04d", n);
}

void CImGuiMgr::TickParticle(_float _fTimeDelta)
{
	m_pWindowParticle->Tick(_fTimeDelta);
}

void CImGuiMgr::KeyInput(_float _fTimeDelta)
{
	if (GAMEINSTANCE->KeyDown(DIK_PGUP))
		m_bShowSetting = !m_bShowSetting;

	if (GAMEINSTANCE->KeyDown(DIK_PGDN))
		m_bShowPlayWindow = !m_bShowPlayWindow;
}

void CImGuiMgr::LoadEffectGroupList()
{
	m_strEffectGroupList.clear();

	filesystem::path basePath(m_strSavePath);

	for (const auto& entry : filesystem::directory_iterator(basePath)) {
		/*if. File : .png 라면 texture Create*/
		if (entry.is_regular_file()) {
			if (".json" == entry.path().extension()) {
				m_strEffectGroupList.push_back(entry.path().filename().stem().string());
			}
		}
	}
}

void CImGuiMgr::LoadEffectList(string _strEffectGroupKey)
{

}

HRESULT CImGuiMgr::AddToEffect()
{
	// 선택된 이펙트 시스템이 없으면 추가 안되도록

	if (CEffect::EFFECT_MESH == m_iCreateEffectType)
	{
		if (strcmp(m_strEffectName, "") == 0)
		{
			m_chContent = "Enter the Effect ID";
			ImGui::OpenPopup(m_chTitle);
			return S_OK;
		}

		m_strEffectList.push_back(m_strEffectName);
		m_pLoadEffectGroup->AddEffect(m_strEffectName, m_pWindowMesh->CreateEffect());
		//m_pEffectList.push_back(m_pWindowMesh->CreateEffect());
	}

	/*else if (1 == m_iCreateEffectType)
	{
		if (strcmp(m_strEffectName, "") == 0)
		{
			m_chContent = "Enter the Effect ID";
			ImGui::OpenPopup(m_chTitle);
			return S_OK;
		}
	}*/

	else if (CEffect::EFFECT_PARTICLE == m_iCreateEffectType)
	{
		if (strcmp(m_strEffectName, "") == 0)
		{
			m_chContent = "Enter the Effect ID";
			ImGui::OpenPopup(m_chTitle);
			return S_OK;
		}

		m_strEffectList.push_back(m_strEffectName);
		m_pLoadEffectGroup->AddEffect(m_strEffectName, m_pWindowParticle->CreateEffect());

		//m_pEffectList.push_back(m_pWindowParticle->CreateEffect());
	}

	return S_OK;
}

HRESULT CImGuiMgr::AddToPlay()
{
	if (CEffect::EFFECT_MESH == m_iCreateEffectType)
	{
		if (strcmp(m_strEffectName, "") == 0)
		{
			m_chContent = "Enter the Effect ID";
			ImGui::OpenPopup(m_chTitle);
			return S_OK;
		}

		m_pPlayEffectList.emplace(m_strEffectName, m_pWindowMesh->CreateEffect());
	}

	/*else if (1 == m_iCreateEffectType)
	{
		if (strcmp(m_strEffectName, "") == 0)
		{
			m_chContent = "Enter the Effect ID";
			ImGui::OpenPopup(m_chTitle);
			return S_OK;
		}
	}*/

	else if (CEffect::EFFECT_PARTICLE == m_iCreateEffectType)
	{
		if (strcmp(m_strEffectName, "") == 0)
		{
			m_chContent = "Enter the Effect ID";
			ImGui::OpenPopup(m_chTitle);
			return S_OK;
		}

		m_pPlayEffectList.emplace(m_strEffectName, m_pWindowParticle->CreateEffect());
	}

	return S_OK;
}

void CImGuiMgr::ShutDown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void CImGuiMgr::TextModel()
{
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(m_chTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(m_chContent);
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

void CImGuiMgr::SetModelObject(shared_ptr<CGameObject> _pGameObject, string _strModelKey, string _strAnimKey)
{
	m_pLoadEffectGroup->SetOwner(_pGameObject);
	if ("" != _strModelKey)
		m_pLoadEffectGroup->SetModelKey(_strModelKey);
	if ("" != _strAnimKey)
		m_pLoadEffectGroup->SetAnimKey(_strAnimKey);
}

HRESULT CImGuiMgr::SaveEffectJson()
{
	// Info File Save (Anim에 붙는 경우만 Info파일 생성하기)
	if (EFFECT_ATTACH_ANIM == m_iEffectAttachType)
	{
		string strGroupInfoFile = m_strSavePath + "Info/" + m_strGroupFileName + "_Info.json";

		Json::Value root;

		if (!m_pLoadEffectGroup)
			return E_FAIL;

		if (m_pLoadEffectGroup->GetEffects().size() <= 0)
			return E_FAIL;

		Json::Value GroupInfo;
		GroupInfo["GroupID"] = m_strGroupFileName;
		GroupInfo["ModelKey"] = m_pLoadEffectGroup->GetGroupInfo().strModelKey;
		GroupInfo["AnimKey"] = m_pLoadEffectGroup->GetGroupInfo().strAnimKey;

		root[m_strGroupFileName] = GroupInfo;

		_tchar tFinalPath[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, 0, strGroupInfoFile.c_str(), static_cast<_uint>(strlen(strGroupInfoFile.c_str())), tFinalPath, MAX_PATH);

		wstring wstrFinalPath = tFinalPath;

		GAMEINSTANCE->WriteJson(root, wstrFinalPath);
	}

	{
		string strGroupInfoFile = m_strSavePath + m_strGroupFileName + ".json";

		Json::Value root;

		if (!m_pLoadEffectGroup)
			return E_FAIL;

		if (m_pLoadEffectGroup->GetEffects().size() <= 0)
			return E_FAIL;

		Json::Value EffectsList;

		// 다 하고나서 이거 위로 올리기
		{
			for (_int i = 0; i < m_pLoadEffectGroup->GetEffects().size(); ++i)
				SaveEffectListJson(EffectsList, m_pLoadEffectGroup->GetEffects()[i], i);
		}

		Json::Value GroupInfo;

		GroupInfo["GroupID"] = m_strGroupFileName;
		GroupInfo["EffectCnt"] = m_pLoadEffectGroup->GetEffects().size();
		GroupInfo["Effects"] = EffectsList;
		GroupInfo["CloneCnt"] = m_iGroupCloneCnt;
		GroupInfo["EffectAttachType"] = m_iEffectAttachType;

		root[m_strGroupFileName] = GroupInfo;

		_tchar tFinalPath[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, 0, strGroupInfoFile.c_str(), static_cast<_uint>(strlen(strGroupInfoFile.c_str())), tFinalPath, MAX_PATH);

		wstring wstrFinalPath = tFinalPath;

		GAMEINSTANCE->WriteJson(root, wstrFinalPath);
	}

	return S_OK;
}

HRESULT CImGuiMgr::SaveEffectListJson(Json::Value& _EffectListRoot, shared_ptr<CEffect> _pEffect, _int _iIdx)
{
	_int iEffectType = _pEffect->GetEffectDesc().iEffectType;

	Json::Value EffectInfo;
	EffectInfo["EffectID"] = m_strEffectList[_iIdx];
	EffectInfo["Type"] = iEffectType;

	Json::Value EffectDesc;

	EffectDesc["StartTrackPos"] = _pEffect->GetEffectDesc().StartTrackPos;
	EffectDesc["ShaderType"] = _pEffect->GetEffectDesc().iShaderType;
	EffectDesc["ShaderPass"] = _pEffect->GetEffectDesc().iShaderPass;
	EffectDesc["RenderGroup"] = _pEffect->GetEffectDesc().iRenderGroup;
	EffectDesc["Duration"] = _pEffect->GetEffectDesc().fDuration;
	EffectDesc["Loop"] = _pEffect->GetEffectDesc().bLoop;
	EffectDesc["UseDiffuse"] = _pEffect->GetEffectDesc().bUseDiffuse;

	if (CEffect::EFFECT_MESH == iEffectType)
	{
		shared_ptr<CEffectMesh> pMeshEffect = dynamic_pointer_cast<CEffectMesh>(_pEffect);
		CEffectMesh::EffectMeshDesc* pEffectDesc = pMeshEffect->GetMeshDesc();

		EffectDesc["TexDiffuseKey"] = _pEffect->GetEffectDesc().strTexDiffuseKey;
		if (_pEffect->GetEffectDesc().strTexDiffuseKey != "")
			TextureToClient(TEX_DIFFUSE, _pEffect->GetEffectDesc().strTexDiffuseKey);

		EffectDesc["ModelKey"] = pEffectDesc->strModelKey;
		ModelToClient(m_strOriginPath + "Mesh/", pEffectDesc->strModelKey);

		EffectDesc["BillBoard"] = pEffectDesc->bBillBoard;

		EffectDesc["UseMask"] = pEffectDesc->bUseMask;
		EffectDesc["TexMaskKey"] = pEffectDesc->strTexMaskKey;
		if (pEffectDesc->bUseMask && pEffectDesc->strTexMaskKey != "")
			TextureToClient(TEX_MASK, pEffectDesc->strTexMaskKey);

		EffectDesc["UseNoise"] = pEffectDesc->bUseNoise;
		EffectDesc["TexNoiseKey"] = pEffectDesc->strTexNoiseKey;
		if (pEffectDesc->bUseNoise && pEffectDesc->strTexNoiseKey != "")
			TextureToClient(TEX_NOISE, pEffectDesc->strTexNoiseKey);

		EffectDesc["UseColor"] = pEffectDesc->bUseColor;

		EffectDesc["DiffuseSamplerType"] = pEffectDesc->iDiffuseSamplerType;
		EffectDesc["DiffuseUVType"] = pEffectDesc->iDiffuseUVType;
		EffectDesc["DiffuseDiscardValue"] = pEffectDesc->fDiffuseDiscardValue;
		EffectDesc["DiffuseDiscardArea"] = pEffectDesc->iDiffuseDiscardArea;

		Json::Value DiffuseUVScroll(Json::arrayValue);
		DiffuseUVScroll.append(pEffectDesc->fDiffuseUVScroll.x);
		DiffuseUVScroll.append(pEffectDesc->fDiffuseUVScroll.y);
		EffectDesc["DiffuseUVScroll"] = DiffuseUVScroll;

		Json::Value DiffuseUVAtlasCnt(Json::arrayValue);
		DiffuseUVAtlasCnt.append(pEffectDesc->iDiffuseUVAtlasCnt.x);
		DiffuseUVAtlasCnt.append(pEffectDesc->iDiffuseUVAtlasCnt.y);
		EffectDesc["DiffuseUVAtlasCnt"] = DiffuseUVAtlasCnt;

		EffectDesc["DiffuseUVAtlasSpeed"] = pEffectDesc->fDiffuseUVAtlasSpeed;

		EffectDesc["MaskSamplerType"] = pEffectDesc->iMaskSamplerType;
		EffectDesc["MaskUVType"] = pEffectDesc->iMaskUVType;
		EffectDesc["MaskDiscardValue"] = pEffectDesc->fMaskDiscardValue;
		EffectDesc["MaskDiscardArea"] = pEffectDesc->iMaskDiscardArea;

		Json::Value MaskUVScroll(Json::arrayValue);
		MaskUVScroll.append(pEffectDesc->fMaskUVScroll.x);
		MaskUVScroll.append(pEffectDesc->fMaskUVScroll.y);
		EffectDesc["MaskUVScroll"] = MaskUVScroll;

		Json::Value MaskUVAtlasCnt(Json::arrayValue);
		MaskUVAtlasCnt.append(pEffectDesc->iMaskUVAtlasCnt.x);
		MaskUVAtlasCnt.append(pEffectDesc->iMaskUVAtlasCnt.y);
		EffectDesc["MaskUVAtlasCnt"] = MaskUVAtlasCnt;

		EffectDesc["MaskUVAtlasSpeed"] = pEffectDesc->fMaskUVAtlasSpeed;

		EffectDesc["NoiseSamplerType"] = pEffectDesc->iNoiseSamplerType;
		EffectDesc["NoiseUVType"] = pEffectDesc->iNoiseUVType;
		EffectDesc["NoiseDiscardValue"] = pEffectDesc->fNoiseDiscardValue;
		EffectDesc["NoiseDiscardArea"] = pEffectDesc->iNoiseDiscardArea;

		Json::Value NoiseUVScroll(Json::arrayValue);
		NoiseUVScroll.append(pEffectDesc->fNoiseUVScroll.x);
		NoiseUVScroll.append(pEffectDesc->fNoiseUVScroll.y);
		EffectDesc["NoiseUVScroll"] = NoiseUVScroll;

		Json::Value NoiseUVAtlasCnt(Json::arrayValue);
		NoiseUVAtlasCnt.append(pEffectDesc->iNoiseUVAtlasCnt.x);
		NoiseUVAtlasCnt.append(pEffectDesc->iNoiseUVAtlasCnt.y);
		EffectDesc["NoiseUVAtlasCnt"] = NoiseUVAtlasCnt;

		EffectDesc["NoiseUVAtlasSpeed"] = pEffectDesc->fNoiseUVAtlasSpeed;

		EffectDesc["ColorSetCnt"] = pEffectDesc->iColorSetCnt;
		EffectDesc["ColorSplitArea"] = pEffectDesc->iColorSplitArea;

		Json::Value ColorSplitter(Json::arrayValue);
		ColorSplitter.append(pEffectDesc->vColorSplitter.x);
		ColorSplitter.append(pEffectDesc->vColorSplitter.y);
		ColorSplitter.append(pEffectDesc->vColorSplitter.z);
		ColorSplitter.append(pEffectDesc->vColorSplitter.w);
		EffectDesc["ColorSplitter"] = ColorSplitter;

		EffectDesc["ScaleFollowType"] = pEffectDesc->iScaleFollowType;
		EffectDesc["ScaleFollowBoneKey"] = pEffectDesc->strScaleFollowBoneKey;
		EffectDesc["ScaleSetCnt"] = pEffectDesc->iScaleSetCnt;
		EffectDesc["RotFollowType"] = pEffectDesc->iRotFollowType;
		EffectDesc["RotFollowBoneKey"] = pEffectDesc->strRotFollowBoneKey;

		EffectDesc["GlowColorSetCnt"] = pEffectDesc->iGlowColorSetCnt;
		EffectDesc["UseGlowColor"] = pEffectDesc->bUseGlowColor;

		Json::Value Rotation(Json::arrayValue);
		Rotation.append(pEffectDesc->vRotation.x);
		Rotation.append(pEffectDesc->vRotation.y);
		Rotation.append(pEffectDesc->vRotation.z);
		EffectDesc["Rotation"] = Rotation;

		Json::Value TurnVel(Json::arrayValue);
		TurnVel.append(pEffectDesc->vTurnVel.x);
		TurnVel.append(pEffectDesc->vTurnVel.y);
		TurnVel.append(pEffectDesc->vTurnVel.z);
		EffectDesc["TurnVel"] = TurnVel;

		Json::Value TurnAcc(Json::arrayValue);
		TurnAcc.append(pEffectDesc->vTurnAcc.x);
		TurnAcc.append(pEffectDesc->vTurnAcc.y);
		TurnAcc.append(pEffectDesc->vTurnAcc.z);
		EffectDesc["TurnAcc"] = TurnAcc;

		EffectDesc["TransFollowType"] = pEffectDesc->iTransFollowType;
		EffectDesc["TransFollowBoneKey"] = pEffectDesc->strTransFollowBoneKey;
		EffectDesc["TransSetCnt"] = pEffectDesc->iTransSetCnt;

		EffectDesc["DistortionKey"] = pEffectDesc->strDistortionKey;
		if (pEffectDesc->strDistortionKey != "")
			TextureToClient(TEX_NOISE, pEffectDesc->strDistortionKey);

		EffectDesc["UseDistortion"] = pEffectDesc->bUseDistortion;
		EffectDesc["DistortionSpeed"] = pEffectDesc->fDistortionSpeed;
		EffectDesc["DistortionWeight"] = pEffectDesc->fDistortionWeight;

		EffectDesc["UseDslv"] = pEffectDesc->bUseDslv;
		EffectDesc["DslvType"] = pEffectDesc->iDslvType;
		EffectDesc["DslvGlow"] = pEffectDesc->bDslvGlow;
		EffectDesc["DslvStartTime"] = pEffectDesc->fDslvStartTime;
		EffectDesc["DslvDuration"] = pEffectDesc->fDslvDuration;
		EffectDesc["DslvThick"] = pEffectDesc->fDslvThick;
		EffectDesc["strDslvTexKey"] = pEffectDesc->strDslvTexKey;

		Json::Value DslvColor(Json::arrayValue);
		TurnAcc.append(pEffectDesc->vDslvColor.x);
		TurnAcc.append(pEffectDesc->vDslvColor.y);
		TurnAcc.append(pEffectDesc->vDslvColor.z);
		TurnAcc.append(pEffectDesc->vDslvColor.w);
		EffectDesc["DslvColor"] = TurnAcc;

		Json::Value DslvGlowColor(Json::arrayValue);
		TurnAcc.append(pEffectDesc->vDslvGlowColor.x);
		TurnAcc.append(pEffectDesc->vDslvGlowColor.y);
		TurnAcc.append(pEffectDesc->vDslvGlowColor.z);
		TurnAcc.append(pEffectDesc->vDslvGlowColor.w);
		EffectDesc["DslvGlowColor"] = TurnAcc;

		Json::Value KeyFrame;

		Json::Value ScaleKeyFrame;

		Json::Value ScaleKeyFrameTime(Json::arrayValue);
		Json::Value ScaleKeyFrameEase(Json::arrayValue);
		Json::Value ScaleKeyFrameScale(Json::arrayValue);

		for (auto& iter : pEffectDesc->KeyFrames.ScaleKeyFrames)
		{
			Json::Value ScaleKeyFrameScaleArray(Json::arrayValue);

			ScaleKeyFrameTime.append(iter.fTime);
			ScaleKeyFrameEase.append(iter.eEaseType);
			ScaleKeyFrameScaleArray.append(iter.vScale.x);
			ScaleKeyFrameScaleArray.append(iter.vScale.y);
			ScaleKeyFrameScaleArray.append(iter.vScale.z);
			ScaleKeyFrameScale.append(ScaleKeyFrameScaleArray);
		}

		ScaleKeyFrame["Time"] = ScaleKeyFrameTime;
		ScaleKeyFrame["EaseType"] = ScaleKeyFrameEase;
		ScaleKeyFrame["Scale"] = ScaleKeyFrameScale;

		Json::Value TransKeyFrame;

		Json::Value TransKeyFrameTime(Json::arrayValue);
		Json::Value TransKeyFrameEase(Json::arrayValue);
		Json::Value TransKeyFrameTrans(Json::arrayValue);

		for (auto& iter : pEffectDesc->KeyFrames.TransKeyFrames)
		{
			Json::Value TransKeyFrameTransArray(Json::arrayValue);

			TransKeyFrameTime.append(iter.fTime);
			TransKeyFrameEase.append(iter.eEaseType);
			TransKeyFrameTransArray.append(iter.vTranslation.x);
			TransKeyFrameTransArray.append(iter.vTranslation.y);
			TransKeyFrameTransArray.append(iter.vTranslation.z);
			TransKeyFrameTrans.append(TransKeyFrameTransArray);
		}

		TransKeyFrame["Time"] = TransKeyFrameTime;
		TransKeyFrame["EaseType"] = TransKeyFrameEase;
		TransKeyFrame["Trans"] = TransKeyFrameTrans;

		Json::Value GlowColorKeyFrame;

		Json::Value GlowColorKeyFrameTime(Json::arrayValue);
		Json::Value GlowColorKeyFrameColor(Json::arrayValue);

		for (auto& iter : pEffectDesc->KeyFrames.GlowColorKeyFrames)
		{
			Json::Value GlowColorKeyFrameColorArrays(Json::arrayValue);

			GlowColorKeyFrameTime.append(iter.fTime);
			GlowColorKeyFrameColorArrays.append(iter.vGlowColor.x);
			GlowColorKeyFrameColorArrays.append(iter.vGlowColor.y);
			GlowColorKeyFrameColorArrays.append(iter.vGlowColor.z);
			GlowColorKeyFrameColorArrays.append(iter.vGlowColor.w);
			GlowColorKeyFrameColor.append(GlowColorKeyFrameColorArrays);
		}

		GlowColorKeyFrame["Time"] = GlowColorKeyFrameTime;
		GlowColorKeyFrame["GlowColor"] = GlowColorKeyFrameColor;

		Json::Value ColorKeyFrame;

		Json::Value ColorKeyFrameTime(Json::arrayValue);
		Json::Value ColorKeyFrameEase(Json::arrayValue);
		Json::Value ColorKeyFrameColor(Json::arrayValue);

		for (auto& iter : pEffectDesc->KeyFrames.ColorKeyFrames)
		{
			Json::Value ColorKeyFrameColorArray(Json::arrayValue);

			ColorKeyFrameTime.append(iter.fTime);
			ColorKeyFrameEase.append(iter.eEaseType);

			for (_int i = 0; i < pEffectDesc->iColorSplitArea; i++)
			{
				Json::Value Color(Json::arrayValue);

				Color.append(iter.vColor[i].x);
				Color.append(iter.vColor[i].y);
				Color.append(iter.vColor[i].z);
				Color.append(iter.vColor[i].w);

				ColorKeyFrameColorArray.append(Color);
			}
			ColorKeyFrameColor.append(ColorKeyFrameColorArray);
		}

		ColorKeyFrame["Time"] = ColorKeyFrameTime;
		ColorKeyFrame["EaseType"] = ColorKeyFrameEase;
		ColorKeyFrame["Color"] = ColorKeyFrameColor;

		KeyFrame["Scale"] = ScaleKeyFrame;
		KeyFrame["Trans"] = TransKeyFrame;
		KeyFrame["Color"] = ColorKeyFrame;
		KeyFrame["GlowColor"] = GlowColorKeyFrame;

		EffectDesc["KeyFrames"] = KeyFrame;
	}

	/*else if (1 == iEffectType)
	{

	}*/

	else if (CEffect::EFFECT_PARTICLE == iEffectType)
	{
		shared_ptr<CParticleSystem> pParticle = dynamic_pointer_cast<CParticleSystem>(_pEffect);
		CParticleSystem::PARTICLE_SYSTEM* pEffectDesc = pParticle->GetParticleDesc();

		EffectDesc["TexDiffuseKey"] = _pEffect->GetEffectDesc().strTexDiffuseKey;
		if (_pEffect->GetEffectDesc().strTexDiffuseKey != "")
			TextureToClient(TEX_END, _pEffect->GetEffectDesc().strTexDiffuseKey);

		/*EffectDesc["StartTrackPos"] = pEffectDesc->StartTrackPos;
		EffectDesc["ShaderType"] = pEffectDesc->iShaderType;
		EffectDesc["ShaderPass"] = pEffectDesc->iShaderPass;
		EffectDesc["RenderGroup"] = pEffectDesc->iRenderGroup;
		EffectDesc["Duration"] = pEffectDesc->fDuration;
		EffectDesc["Loop"] = pEffectDesc->bLoop;
		EffectDesc["UseDiffuse"] = pEffectDesc->bUseDiffuse;
		EffectDesc["TexDiffuseKey"] = pEffectDesc->strTexDiffuseKey;*/

		EffectDesc["MaxParticles"] = pEffectDesc->iMaxParticles;

		Json::Value LifeTime(Json::arrayValue);
		LifeTime.append(pEffectDesc->vLifeTime.x);
		LifeTime.append(pEffectDesc->vLifeTime.y);
		EffectDesc["LifeTime"] = LifeTime;

		EffectDesc["SpawnType"] = pEffectDesc->iSpawnType;
		EffectDesc["SpawnNumPerSec"] = pEffectDesc->fSpawnNumPerSec;
		EffectDesc["RenderType"] = pEffectDesc->iRenderType;
		EffectDesc["BillBoardType"] = pEffectDesc->iBillBoardType;

		EffectDesc["PosType"] = pEffectDesc->iPosType;
		EffectDesc["PosFollow"] = pEffectDesc->bPosFollow;
		EffectDesc["PosFollowBoneKey"] = pEffectDesc->strPosFollowBoneKey;

		if (pEffectDesc->bUsePosKeyFrame)
			EffectDesc["UsePosKeyFrame"] = pEffectDesc->bUsePosKeyFrame;

		EffectDesc["UseHazeEffect"] = pEffectDesc->bUseHazeEffect;
		EffectDesc["RelativeOwner"] = pEffectDesc->bRelativeOwner;

		Json::Value HazeSpeedMin(Json::arrayValue);
		HazeSpeedMin.append(pEffectDesc->vHazeSpeedMin.x);
		HazeSpeedMin.append(pEffectDesc->vHazeSpeedMin.y);
		HazeSpeedMin.append(pEffectDesc->vHazeSpeedMin.z);
		EffectDesc["HazeSpeedMin"] = HazeSpeedMin;

		Json::Value HazeSpeedMax(Json::arrayValue);
		HazeSpeedMax.append(pEffectDesc->vHazeSpeedMax.x);
		HazeSpeedMax.append(pEffectDesc->vHazeSpeedMax.y);
		HazeSpeedMax.append(pEffectDesc->vHazeSpeedMax.z);
		EffectDesc["HazeSpeedMax"] = HazeSpeedMax;
		
		EffectDesc["UseGlowColor"] = pEffectDesc->bUseGlowColor;

		Json::Value GlowColor(Json::arrayValue);
		GlowColor.append(pEffectDesc->vGlowColor.x);
		GlowColor.append(pEffectDesc->vGlowColor.y);
		GlowColor.append(pEffectDesc->vGlowColor.z);
		GlowColor.append(pEffectDesc->vGlowColor.w);
		EffectDesc["GlowColor"] = GlowColor;

		if (!pEffectDesc->bUsePosKeyFrame)
		{
			Json::Value PosMin(Json::arrayValue);
			PosMin.append(pEffectDesc->vPosMin.x);
			PosMin.append(pEffectDesc->vPosMin.y);
			PosMin.append(pEffectDesc->vPosMin.z);
			EffectDesc["PosMin"] = PosMin;

			Json::Value PosMax(Json::arrayValue);
			PosMax.append(pEffectDesc->vPosMax.x);
			PosMax.append(pEffectDesc->vPosMax.y);
			PosMax.append(pEffectDesc->vPosMax.z);
			EffectDesc["PosMax"] = PosMax;
		}

		EffectDesc["Shape"] = pEffectDesc->iShape;
		EffectDesc["SpeedMode"] = pEffectDesc->bSpeedMode;

		Json::Value StartSpeedMin(Json::arrayValue);
		StartSpeedMin.append(pEffectDesc->vStartSpeedMin.x);
		StartSpeedMin.append(pEffectDesc->vStartSpeedMin.y);
		StartSpeedMin.append(pEffectDesc->vStartSpeedMin.z);
		EffectDesc["StartSpeedMin"] = StartSpeedMin;

		Json::Value StartSpeedMax(Json::arrayValue);
		StartSpeedMax.append(pEffectDesc->vStartSpeedMax.x);
		StartSpeedMax.append(pEffectDesc->vStartSpeedMax.y);
		StartSpeedMax.append(pEffectDesc->vStartSpeedMax.z);
		EffectDesc["StartSpeedMax"] = StartSpeedMax;

		Json::Value SpeedAccMin(Json::arrayValue);
		SpeedAccMin.append(pEffectDesc->vSpeedAccMin.x);
		SpeedAccMin.append(pEffectDesc->vSpeedAccMin.y);
		SpeedAccMin.append(pEffectDesc->vSpeedAccMin.z);
		EffectDesc["SpeedAccMin"] = SpeedAccMin;

		Json::Value SpeedAccMax(Json::arrayValue);
		SpeedAccMax.append(pEffectDesc->vSpeedAccMax.x);
		SpeedAccMax.append(pEffectDesc->vSpeedAccMax.y);
		SpeedAccMax.append(pEffectDesc->vSpeedAccMax.z);
		EffectDesc["SpeedAccMax"] = SpeedAccMax;



		Json::Value RotationMin(Json::arrayValue);
		RotationMin.append(pEffectDesc->vRotationMin.x);
		RotationMin.append(pEffectDesc->vRotationMin.y);
		RotationMin.append(pEffectDesc->vRotationMin.z);
		EffectDesc["RotationMin"] = RotationMin;

		Json::Value RotationMax(Json::arrayValue);
		RotationMax.append(pEffectDesc->vRotationMax.x);
		RotationMax.append(pEffectDesc->vRotationMax.y);
		RotationMax.append(pEffectDesc->vRotationMax.z);
		EffectDesc["RotationMax"] = RotationMax;

		Json::Value TurnVelMin(Json::arrayValue);
		TurnVelMin.append(pEffectDesc->vTurnVelMin.x);
		TurnVelMin.append(pEffectDesc->vTurnVelMin.y);
		TurnVelMin.append(pEffectDesc->vTurnVelMin.z);
		EffectDesc["TurnVelMin"] = TurnVelMin;

		Json::Value TurnVelMax(Json::arrayValue);
		TurnVelMax.append(pEffectDesc->vTurnVelMax.x);
		TurnVelMax.append(pEffectDesc->vTurnVelMax.y);
		TurnVelMax.append(pEffectDesc->vTurnVelMax.z);
		EffectDesc["TurnVelMax"] = TurnVelMax;

		Json::Value TurnAccMin(Json::arrayValue);
		TurnAccMin.append(pEffectDesc->vTurnAccMin.x);
		TurnAccMin.append(pEffectDesc->vTurnAccMin.y);
		TurnAccMin.append(pEffectDesc->vTurnAccMin.z);
		EffectDesc["TurnAccMin"] = TurnAccMin;

		Json::Value TurnAccMax(Json::arrayValue);
		TurnAccMax.append(pEffectDesc->vTurnAccMax.x);
		TurnAccMax.append(pEffectDesc->vTurnAccMax.y);
		TurnAccMax.append(pEffectDesc->vTurnAccMax.z);
		EffectDesc["TurnAccMax"] = TurnAccMax;

		EffectDesc["UseColor"] = pEffectDesc->bUseColor;

		EffectDesc["UsePivot"] = pEffectDesc->bUsePivot;

		Json::Value PivotMin(Json::arrayValue);
		PivotMin.append(pEffectDesc->vPivotMin.x);
		PivotMin.append(pEffectDesc->vPivotMin.y);
		PivotMin.append(pEffectDesc->vPivotMin.z);
		EffectDesc["PivotMin"] = PivotMin;

		Json::Value PivotMax(Json::arrayValue);
		PivotMax.append(pEffectDesc->vPivotMax.x);
		PivotMax.append(pEffectDesc->vPivotMax.y);
		PivotMax.append(pEffectDesc->vPivotMax.z);
		EffectDesc["PivotMax"] = PivotMax;

		Json::Value KeyFrame;

		Json::Value ScaleKeyFrame;

		Json::Value ScaleKeyFrameTime(Json::arrayValue);
		Json::Value ScaleKeyFrameEase(Json::arrayValue);
		Json::Value ScaleKeyFrameScaleMin(Json::arrayValue);
		Json::Value ScaleKeyFrameScaleMax(Json::arrayValue);

		EffectDesc["ScaleSetCnt"] = pEffectDesc->KeyFrames.ScaleKeyFrames.size();

		for (auto& iter : pEffectDesc->KeyFrames.ScaleKeyFrames)
		{
			Json::Value ScaleKeyFrameScaleMinArray(Json::arrayValue);
			Json::Value ScaleKeyFrameScaleMaxArray(Json::arrayValue);

			ScaleKeyFrameTime.append(iter.fTime);
			ScaleKeyFrameEase.append(iter.eEaseType);

			ScaleKeyFrameScaleMinArray.append(iter.vScaleMin.x);
			ScaleKeyFrameScaleMinArray.append(iter.vScaleMin.y);
			ScaleKeyFrameScaleMinArray.append(iter.vScaleMin.z);
			ScaleKeyFrameScaleMin.append(ScaleKeyFrameScaleMinArray);

			ScaleKeyFrameScaleMaxArray.append(iter.vScaleMax.x);
			ScaleKeyFrameScaleMaxArray.append(iter.vScaleMax.y);
			ScaleKeyFrameScaleMaxArray.append(iter.vScaleMax.z);
			ScaleKeyFrameScaleMax.append(ScaleKeyFrameScaleMaxArray);
		}

		ScaleKeyFrame["Time"] = ScaleKeyFrameTime;
		ScaleKeyFrame["EaseType"] = ScaleKeyFrameEase;
		ScaleKeyFrame["ScaleMin"] = ScaleKeyFrameScaleMin;
		ScaleKeyFrame["ScaleMax"] = ScaleKeyFrameScaleMax;


		if (pEffectDesc->bUsePosKeyFrame)
		{
			Json::Value PosKeyFrame;

			Json::Value PosKeyFrameTime(Json::arrayValue);
			Json::Value PosKeyFrameEase(Json::arrayValue);
			Json::Value PosKeyFramePosMin(Json::arrayValue);
			Json::Value PosKeyFramePosMax(Json::arrayValue);

			EffectDesc["PosSetCnt"] = pEffectDesc->KeyFrames.PosKeyFrames.size();

			for (auto& iter : pEffectDesc->KeyFrames.PosKeyFrames)
			{
				Json::Value PosKeyFramePosMinArray(Json::arrayValue);
				Json::Value PosKeyFramePosMaxArray(Json::arrayValue);

				PosKeyFrameTime.append(iter.fTime);
				PosKeyFrameEase.append(iter.eEaseType);

				PosKeyFramePosMinArray.append(iter.vPosMin.x);
				PosKeyFramePosMinArray.append(iter.vPosMin.y);
				PosKeyFramePosMinArray.append(iter.vPosMin.z);
				PosKeyFramePosMin.append(PosKeyFramePosMinArray);

				PosKeyFramePosMaxArray.append(iter.vPosMax.x);
				PosKeyFramePosMaxArray.append(iter.vPosMax.y);
				PosKeyFramePosMaxArray.append(iter.vPosMax.z);
				PosKeyFramePosMax.append(PosKeyFramePosMaxArray);
			}

			PosKeyFrame["Time"] = PosKeyFrameTime;
			PosKeyFrame["EaseType"] = PosKeyFrameEase;
			PosKeyFrame["PosMin"] = PosKeyFramePosMin;
			PosKeyFrame["PosMax"] = PosKeyFramePosMax;

			KeyFrame["Pos"] = PosKeyFrame;
		}

		Json::Value SpeedKeyFrame;

		Json::Value SpeedKeyFrameTime(Json::arrayValue);
		Json::Value SpeedKeyFrameEase(Json::arrayValue);
		Json::Value SpeedKeyFrameSpeedMin(Json::arrayValue);
		Json::Value SpeedKeyFrameSpeedMax(Json::arrayValue);

		EffectDesc["SpeedSetCnt"] = pEffectDesc->KeyFrames.SpeedKeyFrames.size();

		for (auto& iter : pEffectDesc->KeyFrames.SpeedKeyFrames)
		{
			Json::Value SpeedKeyFrameSpeedMinArray(Json::arrayValue);
			Json::Value SpeedKeyFrameSpeedMaxArray(Json::arrayValue);

			SpeedKeyFrameTime.append(iter.fTime);
			SpeedKeyFrameEase.append(iter.eEaseType);

			SpeedKeyFrameSpeedMinArray.append(iter.vSpeedMin.x);
			SpeedKeyFrameSpeedMinArray.append(iter.vSpeedMin.y);
			SpeedKeyFrameSpeedMinArray.append(iter.vSpeedMin.z);
			SpeedKeyFrameSpeedMin.append(SpeedKeyFrameSpeedMinArray);

			SpeedKeyFrameSpeedMaxArray.append(iter.vSpeedMax.x);
			SpeedKeyFrameSpeedMaxArray.append(iter.vSpeedMax.y);
			SpeedKeyFrameSpeedMaxArray.append(iter.vSpeedMax.z);
			SpeedKeyFrameSpeedMax.append(SpeedKeyFrameSpeedMaxArray);
		}

		SpeedKeyFrame["Time"] = SpeedKeyFrameTime;
		SpeedKeyFrame["EaseType"] = SpeedKeyFrameEase;
		SpeedKeyFrame["SpeedMin"] = SpeedKeyFrameSpeedMin;
		SpeedKeyFrame["SpeedMax"] = SpeedKeyFrameSpeedMax;

		Json::Value ColorKeyFrame;

		Json::Value ColorKeyFrameTime(Json::arrayValue);
		Json::Value ColorKeyFrameEase(Json::arrayValue);
		Json::Value ColorKeyFrameColorMin(Json::arrayValue);
		Json::Value ColorKeyFrameColorMax(Json::arrayValue);

		EffectDesc["ColorSetCnt"] = pEffectDesc->KeyFrames.ColorKeyFrames.size();

		for (auto& iter : pEffectDesc->KeyFrames.ColorKeyFrames)
		{
			Json::Value ColorKeyFrameColorMinArray(Json::arrayValue);
			Json::Value ColorKeyFrameColorMaxArray(Json::arrayValue);

			ColorKeyFrameTime.append(iter.fTime);
			ColorKeyFrameEase.append(iter.eEaseType);

			ColorKeyFrameColorMinArray.append(iter.vColorMin.x);
			ColorKeyFrameColorMinArray.append(iter.vColorMin.y);
			ColorKeyFrameColorMinArray.append(iter.vColorMin.z);
			ColorKeyFrameColorMinArray.append(iter.vColorMin.w);
			ColorKeyFrameColorMin.append(ColorKeyFrameColorMinArray);

			ColorKeyFrameColorMaxArray.append(iter.vColorMax.x);
			ColorKeyFrameColorMaxArray.append(iter.vColorMax.y);
			ColorKeyFrameColorMaxArray.append(iter.vColorMax.z);
			ColorKeyFrameColorMaxArray.append(iter.vColorMax.w);
			ColorKeyFrameColorMax.append(ColorKeyFrameColorMaxArray);
		}

		ColorKeyFrame["Time"] = ColorKeyFrameTime;
		ColorKeyFrame["EaseType"] = ColorKeyFrameEase;
		ColorKeyFrame["ColorMin"] = ColorKeyFrameColorMin;
		ColorKeyFrame["ColorMax"] = ColorKeyFrameColorMax;

		KeyFrame["Scale"] = ScaleKeyFrame;
		KeyFrame["Speed"] = SpeedKeyFrame;
		KeyFrame["Color"] = ColorKeyFrame;

		EffectDesc["KeyFrames"] = KeyFrame;
	}

	EffectInfo["Desc"] = EffectDesc;

	_EffectListRoot[m_strEffectList[_iIdx]] = EffectInfo;

	return S_OK;
}

HRESULT CImGuiMgr::LoadEffectJson(const string& _strGroupKey)
{
	m_strEffectList.clear();
	m_pEffectList.clear();

	// 이펙트 그룹 Info json 로드

	string strFilePath = m_strSavePath + "Info/" + _strGroupKey + "_Info.json";

	Json::Value root;

	_tchar tFinalPath[MAX_PATH] = TEXT("");
	MultiByteToWideChar(CP_ACP, 0, strFilePath.c_str(), static_cast<_uint>(strlen(strFilePath.c_str())), tFinalPath, MAX_PATH);

	wstring wstrFinalPath = tFinalPath;

	root = GAMEINSTANCE->ReadJson(wstrFinalPath);

	GAMEINSTANCE->ClearLayer(LEVEL_LOGO, TEXT("Layer_Effect"));

	Json::Value GroupInfo = root[_strGroupKey];

	// 몰라
	// 모델이랑 애님키로 모델 생성해서 걔를 넣어줘야한다아아아아아앙
	//m_pLoadEffectGroup->SetOwner();


	// 이펙트들 json 로드
	{
		string strFilePath = m_strSavePath + _strGroupKey + ".json";

		Json::Value root;

		_tchar tFinalPath[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, 0, strFilePath.c_str(), static_cast<_uint>(strlen(strFilePath.c_str())), tFinalPath, MAX_PATH);

		wstring wstrFinalPath = tFinalPath;

		root = GAMEINSTANCE->ReadJson(wstrFinalPath);

		Json::Value EffectInfo = root[_strGroupKey];
		EFFECT_ATTACH_TYPE eEffectAttachType = (EFFECT_ATTACH_TYPE)EffectInfo["EffectAttachType"].asInt();
		m_pLoadEffectGroup = CEffectGroup::Create(eEffectAttachType);
		m_pLoadEffectGroup->SetPlaying(false);

		if (FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, TEXT("Layer_Effect"), m_pLoadEffectGroup)))
			return E_FAIL;

		m_pLoadEffectGroup->SetGroupInfo(GroupInfo["GroupID"].asString(), GroupInfo["ModelKey"].asString(), GroupInfo["AnimKey"].asString());
		// 위의 정보로 모델 생성해서 띄워주기이이이이
		m_pLoadEffectGroup->SetOwner(m_pWindowModel->ChangeModelAndAnim(GroupInfo["ModelKey"].asString(), GroupInfo["AnimKey"].asString()));

		m_iGroupCloneCnt = EffectInfo["CloneCnt"].asInt();
		m_iEffectAttachType = EffectInfo["EffectAttachType"].asInt();

		_int _iEffectCnt = EffectInfo["EffectCnt"].asInt();

		Json::Value Element = EffectInfo["Effects"];

		Json::ValueIterator iter = Element.begin();

		for (Json::ValueIterator iter = Element.begin(); iter != Element.end(); ++iter)
		{
			string	strEffectID = iter.key().asString();
			LoadEffectListJson(Element[strEffectID]);
			m_pLoadEffectGroup->SetEnable(true);
		}
	}


	return S_OK;
}

HRESULT CImGuiMgr::LoadEffectListJson(Json::Value& Element)
{
	_int	iEffectType = Element["Type"].asInt();

	Json::Value& Desc = Element["Desc"];

	if (CEffect::EFFECT_MESH == iEffectType)
	{
		// 메쉬
		CEffectMesh::EffectMeshDesc eEffectMeshDesc = {};

		eEffectMeshDesc.iEffectType = iEffectType;
		eEffectMeshDesc.iShaderType = ST_EFFECTMESH;
		eEffectMeshDesc.iShaderPass = Desc["ShaderPass"].asInt();
		eEffectMeshDesc.iRenderGroup = Desc["RenderGroup"].asInt();

		eEffectMeshDesc.fDuration = Desc["Duration"].asFloat();
		eEffectMeshDesc.StartTrackPos = Desc["StartTrackPos"].asDouble();

		eEffectMeshDesc.bLoop = Desc["Loop"].asBool();
		eEffectMeshDesc.bUseDiffuse = Desc["UseDiffuse"].asBool();
		eEffectMeshDesc.strTexDiffuseKey = Desc["TexDiffuseKey"].asString();

		eEffectMeshDesc.strModelKey = Desc["ModelKey"].asString();

		eEffectMeshDesc.bBillBoard = Desc["BillBoard"].asBool();
		eEffectMeshDesc.bUseMask = Desc["UseMask"].asBool();
		eEffectMeshDesc.strTexMaskKey = Desc["TexMaskKey"].asString();
		eEffectMeshDesc.bUseNoise = Desc["UseNoise"].asBool();
		eEffectMeshDesc.strTexNoiseKey = Desc["TexNoiseKey"].asString();
		eEffectMeshDesc.bUseColor = Desc["UseColor"].asBool();

		eEffectMeshDesc.iDiffuseSamplerType = Desc["DiffuseSamplerType"].asInt();
		eEffectMeshDesc.iDiffuseUVType = Desc["DiffuseUVType"].asInt();
		eEffectMeshDesc.fDiffuseDiscardValue = Desc["DiffuseDiscardValue"].asFloat();
		eEffectMeshDesc.iDiffuseDiscardArea = Desc["DiffuseDiscardArea"].asInt();

		eEffectMeshDesc.bUseDistortion = Desc["UseDistortion"].asBool();
		eEffectMeshDesc.strDistortionKey = Desc["DistortionKey"].asString();
		eEffectMeshDesc.fDistortionSpeed = Desc["DistortionSpeed"].asFloat();
		eEffectMeshDesc.fDistortionWeight = Desc["DistortionWeight"].asFloat();

		if (Desc.isMember("UseDslv"))
		{
			eEffectMeshDesc.bUseDslv = Desc["UseDslv"].asBool();
			eEffectMeshDesc.iDslvType = Desc["DslvType"].asInt();
			eEffectMeshDesc.bDslvGlow = Desc["DslvGlow"].asBool();
			eEffectMeshDesc.fDslvStartTime = Desc["DslvStartTime"].asFloat();
			eEffectMeshDesc.fDslvDuration = Desc["DslvDuration"].asFloat();
			eEffectMeshDesc.fDslvThick = Desc["DslvThick"].asFloat();
			eEffectMeshDesc.strDslvTexKey = Desc["strDslvTexKey"].asString();

			if (Desc["DslvColor"].isArray())
			{
				Json::Value DslvColor = Desc["DslvColor"];
				eEffectMeshDesc.vDslvColor = _float4(DslvColor[0].asFloat(), DslvColor[1].asFloat(), DslvColor[2].asFloat(), DslvColor[3].asFloat());
			}

			if (Desc["DslvGlowColor"].isArray())
			{
				Json::Value DslvGlowColor = Desc["DslvGlowColor"];
				eEffectMeshDesc.vDslvGlowColor = _float4(DslvGlowColor[0].asFloat(), DslvGlowColor[1].asFloat(), DslvGlowColor[2].asFloat(), DslvGlowColor[3].asFloat());
			}
		}

		else
		{
			eEffectMeshDesc.bUseDslv = false;
			eEffectMeshDesc.iDslvType = 0;
			eEffectMeshDesc.bDslvGlow = false;
			eEffectMeshDesc.fDslvStartTime = 0.f;
			eEffectMeshDesc.fDslvDuration = 0.f;
			eEffectMeshDesc.fDslvThick = 0.f;
			eEffectMeshDesc.strDslvTexKey = "";

			eEffectMeshDesc.vDslvColor = { 1.f, 1.f, 1.f, 1.f };
			eEffectMeshDesc.vDslvGlowColor = { 1.f, 1.f, 1.f, 1.f };
		}

		if (Desc["DiffuseUVScroll"].isArray())
		{
			Json::Value DiffuseUVScroll = Desc["DiffuseUVScroll"];
			eEffectMeshDesc.fDiffuseUVScroll = _float2(DiffuseUVScroll[0].asFloat(), DiffuseUVScroll[1].asFloat());
		}

		if (Desc["DiffuseUVAtlasCnt"].isArray())
		{
			Json::Value DiffuseUVAtlasCnt = Desc["DiffuseUVAtlasCnt"];
			eEffectMeshDesc.iDiffuseUVAtlasCnt = _float2(DiffuseUVAtlasCnt[0].asFloat(), DiffuseUVAtlasCnt[1].asFloat());
		}

		eEffectMeshDesc.fDiffuseUVAtlasSpeed = Desc["DiffuseUVAtlasSpeed"].asFloat();

		eEffectMeshDesc.iMaskSamplerType = Desc["MaskSamplerType"].asInt();
		eEffectMeshDesc.iMaskUVType = Desc["MaskUVType"].asInt();
		eEffectMeshDesc.fMaskDiscardValue = Desc["MaskDiscardValue"].asFloat();
		eEffectMeshDesc.iMaskDiscardArea = Desc["MaskDiscardArea"].asInt();

		if (Desc.isMember("MaskUVScroll") && Desc["MaskUVScroll"].isArray() && Desc["MaskUVScroll"].size() == 2) {
			Json::Value maskUVScroll = Desc["MaskUVScroll"];
			eEffectMeshDesc.fMaskUVScroll = _float2(maskUVScroll[0].asFloat(), maskUVScroll[1].asFloat());
		}

		if (Desc["MaskUVAtlasCnt"].isArray())
		{
			Json::Value MaskUVAtlasCnt = Desc["MaskUVAtlasCnt"];
			eEffectMeshDesc.iMaskUVAtlasCnt = _float2(MaskUVAtlasCnt[0].asFloat(), MaskUVAtlasCnt[1].asFloat());
		}

		eEffectMeshDesc.fMaskUVAtlasSpeed = Desc["MaskUVAtlasSpeed"].asFloat();

		eEffectMeshDesc.iNoiseSamplerType = Desc["NoiseSamplerType"].asInt();
		eEffectMeshDesc.iNoiseUVType = Desc["NoiseUVType"].asInt();
		eEffectMeshDesc.fNoiseDiscardValue = Desc["NoiseDiscardValue"].asFloat();
		eEffectMeshDesc.iNoiseDiscardArea = Desc["NoiseDiscardArea"].asInt();

		if (Desc["NoiseUVScroll"].isArray())
		{
			Json::Value NoiseUVScroll = Desc["NoiseUVScroll"];
			eEffectMeshDesc.fNoiseUVScroll = _float2(NoiseUVScroll[0].asFloat(), NoiseUVScroll[1].asFloat());
		}

		if (Desc["NoiseUVAtlasCnt"].isArray())
		{
			Json::Value NoiseUVAtlasCnt = Desc["NoiseUVAtlasCnt"];
			eEffectMeshDesc.iNoiseUVAtlasCnt = _float2(NoiseUVAtlasCnt[0].asFloat(), NoiseUVAtlasCnt[1].asFloat());
		}

		eEffectMeshDesc.fNoiseUVAtlasSpeed = Desc["NoiseUVAtlasSpeed"].asFloat();

		eEffectMeshDesc.iColorSetCnt = Desc["ColorSetCnt"].asInt();
		eEffectMeshDesc.iColorSplitArea = Desc["ColorSplitArea"].asInt();

		if (Desc["ColorSplitter"].isArray())
		{
			Json::Value ColorSplitter = Desc["ColorSplitter"];
			eEffectMeshDesc.vColorSplitter = _float4(ColorSplitter[0].asFloat(), ColorSplitter[1].asFloat(), ColorSplitter[2].asFloat(), ColorSplitter[3].asFloat());
		}

		eEffectMeshDesc.iScaleFollowType = Desc["ScaleFollowType"].asInt();
		eEffectMeshDesc.strScaleFollowBoneKey = Desc["ScaleFollowBoneKey"].asString();
		eEffectMeshDesc.iScaleSetCnt = Desc["ScaleSetCnt"].asInt();
		eEffectMeshDesc.iRotFollowType = Desc["RotFollowType"].asInt();
		eEffectMeshDesc.strRotFollowBoneKey = Desc["RotFollowBoneKey"].asString();

		if (Desc["Rotation"].isArray())
		{
			Json::Value Rotation = Desc["Rotation"];
			eEffectMeshDesc.vRotation = _float3(Rotation[0].asFloat(), Rotation[1].asFloat(), Rotation[2].asFloat());
		}

		if (Desc["TurnVel"].isArray())
		{
			Json::Value TurnVel = Desc["TurnVel"];
			eEffectMeshDesc.vTurnVel = _float3(TurnVel[0].asFloat(), TurnVel[1].asFloat(), TurnVel[2].asFloat());
		}

		if (Desc["TurnAcc"].isArray())
		{
			Json::Value TurnAcc = Desc["TurnAcc"];
			eEffectMeshDesc.vTurnAcc = _float3(TurnAcc[0].asFloat(), TurnAcc[1].asFloat(), TurnAcc[2].asFloat());
		}

		eEffectMeshDesc.iTransFollowType = Desc["TransFollowType"].asInt();
		eEffectMeshDesc.strTransFollowBoneKey = Desc["TransFollowBoneKey"].asString();
		eEffectMeshDesc.iTransSetCnt = Desc["TransSetCnt"].asInt();

		eEffectMeshDesc.iGlowColorSetCnt = Desc["GlowColorSetCnt"].asInt();
		eEffectMeshDesc.bUseGlowColor = Desc["UseGlowColor"].asBool();

		Json::Value& KeyFrames = Desc["KeyFrames"];

		Json::Value& ScaleKeyFrame = KeyFrames["Scale"];
		Json::Value& TransKeyFrame = KeyFrames["Trans"];
		Json::Value& ColorKeyFrame = KeyFrames["Color"];
		Json::Value& GlowColorKeyFrame = KeyFrames["GlowColor"];

		{
			Json::Value& ScaleKeyFrameTime = ScaleKeyFrame["Time"];
			Json::Value& ScaleKeyFrameEase = ScaleKeyFrame["EaseType"];
			Json::Value& ScaleKeyFrameScale = ScaleKeyFrame["Scale"];

			_float	fScaleTime[MAX_TIMETABLE] = {};
			_int	iScaleEase[MAX_TIMETABLE] = {};
			_float3	fScale[MAX_TIMETABLE] = {};

			if (ScaleKeyFrameTime.isArray())
			{
				_int iScaleIdx = 0;

				for (const auto& ScaleTime : ScaleKeyFrameTime)
				{
					fScaleTime[iScaleIdx++] = ScaleTime.asFloat();
				}

				iScaleIdx = 0;

				for (const auto& ScaleEase : ScaleKeyFrameEase)
				{
					iScaleEase[iScaleIdx++] = ScaleEase.asInt();
				}

				iScaleIdx = 0;

				for (const auto& ScaleValue : ScaleKeyFrameScale)
				{
					fScale[iScaleIdx++] = _float3(ScaleValue[0].asFloat(), ScaleValue[1].asFloat(), ScaleValue[2].asFloat());
				}
			}

			CEffectMesh::SCALE_KEYFRAME scaleKeyFrame;

			for (_int i = 0; i < eEffectMeshDesc.iScaleSetCnt; ++i)
			{
				scaleKeyFrame.fTime = fScaleTime[i];
				scaleKeyFrame.eEaseType = iScaleEase[i];
				scaleKeyFrame.vScale = fScale[i];

				eEffectMeshDesc.KeyFrames.ScaleKeyFrames.push_back(scaleKeyFrame);
			}
		}

		{
			Json::Value& TransKeyFrameTime = TransKeyFrame["Time"];
			Json::Value& TransKeyFrameEase = TransKeyFrame["EaseType"];
			Json::Value& TransKeyFrameTrans = TransKeyFrame["Trans"];

			_float	fTransTime[MAX_TIMETABLE] = {};
			_int	iTransEase[MAX_TIMETABLE] = {};
			_float3	fTrans[MAX_TIMETABLE] = {};

			if (TransKeyFrameTime.isArray())
			{
				_int iTransIdx = 0;

				for (const auto& TransTime : TransKeyFrameTime)
				{
					fTransTime[iTransIdx++] = TransTime.asFloat();
				}

				iTransIdx = 0;

				for (const auto& TransEase : TransKeyFrameEase)
				{
					iTransEase[iTransIdx++] = TransEase.asInt();
				}

				iTransIdx = 0;

				for (const auto& TransValue : TransKeyFrameTrans)
				{
					fTrans[iTransIdx++] = _float3(TransValue[0].asFloat(), TransValue[1].asFloat(), TransValue[2].asFloat());
				}
			}

			CEffectMesh::TRANS_KEYFRAME transKeyFrame;

			for (_int i = 0; i < eEffectMeshDesc.iTransSetCnt; ++i)
			{
				transKeyFrame.fTime = fTransTime[i];
				transKeyFrame.eEaseType = iTransEase[i];
				transKeyFrame.vTranslation = fTrans[i];

				eEffectMeshDesc.KeyFrames.TransKeyFrames.push_back(transKeyFrame);
			}
		}

		{
			Json::Value& GlowColorKeyFrameTime = GlowColorKeyFrame["Time"];
			Json::Value& GlowColorKeyFrameColor = GlowColorKeyFrame["GlowColor"];

			_float	fGlowTime[MAX_TIMETABLE] = {};
			_float4	fGlowColor[MAX_TIMETABLE] = {};

			if (GlowColorKeyFrameTime.isArray())
			{
				_int iTransIdx = 0;

				for (const auto& fTime : GlowColorKeyFrameTime)
				{
					fGlowTime[iTransIdx++] = fTime.asFloat();
				}

				iTransIdx = 0;

				for (const auto& GlowValue : GlowColorKeyFrameColor)
				{
					fGlowColor[iTransIdx++] = _float4(GlowValue[0].asFloat(), GlowValue[1].asFloat(), GlowValue[2].asFloat(), GlowValue[3].asFloat());
				}
			}

			CEffectMesh::GLOWCOLOR_KEYFRAME glowColorKeyFrame;

			for (_int i = 0; i < eEffectMeshDesc.iGlowColorSetCnt; ++i)
			{
				glowColorKeyFrame.fTime = fGlowTime[i];
				glowColorKeyFrame.vGlowColor = fGlowColor[i];

				eEffectMeshDesc.KeyFrames.GlowColorKeyFrames.push_back(glowColorKeyFrame);
			}
		}

		{
			Json::Value& ColorKeyFrameTime = ColorKeyFrame["Time"];
			Json::Value& ColorKeyFrameEase = ColorKeyFrame["EaseType"];
			Json::Value& ColorKeyFrameColor = ColorKeyFrame["Color"];

			_float	fTime[MAX_TIMETABLE] = {};
			_int	iEase[MAX_TIMETABLE] = {};
			_float4	vColor[MAX_TIMETABLE][4] = {};

			if (ColorKeyFrameTime.isArray())
			{
				_int iIdx = 0;

				for (const auto& ColorTime : ColorKeyFrameTime)
				{
					fTime[iIdx++] = ColorTime.asFloat();
				}

				iIdx = 0;

				for (const auto& ColorEase : ColorKeyFrameEase)
				{
					iEase[iIdx++] = ColorEase.asInt();
				}

				iIdx = 0;

				for (const auto& ColorArray : ColorKeyFrameColor)
				{
					_int iSplit = 0;

					for (const auto& Color : ColorArray)
					{
						vColor[iIdx][iSplit++] = _float4(Color[0].asFloat(), Color[1].asFloat(), Color[2].asFloat(), Color[3].asFloat());
					}
					iIdx++;
				}
			}

			CEffectMesh::COLOR_KEYFRAME colorKeyFrame;

			for (_int i = 0; i < eEffectMeshDesc.iColorSetCnt; ++i)
			{
				colorKeyFrame.fTime = fTime[i];
				colorKeyFrame.eEaseType = iEase[i];
				for (_int j = 0; j < eEffectMeshDesc.iColorSplitArea; j++)
					colorKeyFrame.vColor[j] = vColor[i][j];

				eEffectMeshDesc.KeyFrames.ColorKeyFrames.push_back(colorKeyFrame);
			}
		}

		shared_ptr<CEffectMesh> pEffectMesh = CEffectMesh::Create(&eEffectMeshDesc);
		m_pEffectList.push_back(pEffectMesh);
		m_strEffectList.push_back(Element["EffectID"].asString());

		m_pLoadEffectGroup->AddEffect(Element["EffectID"].asString(), pEffectMesh);
	}

	/*else if (1 == iEffectType)
	{

	}*/

	else if (CEffect::EFFECT_PARTICLE == iEffectType)
	{
		// 파티클
		CParticleSystem::PARTICLE_SYSTEM eParticleDesc = {};

		eParticleDesc.iEffectType = iEffectType;
		eParticleDesc.iShaderType = ST_PARTICLE;
		eParticleDesc.iShaderPass = Desc["ShaderPass"].asInt();
		eParticleDesc.iRenderGroup = Desc["RenderGroup"].asInt();

		eParticleDesc.fDuration = Desc["Duration"].asFloat();
		eParticleDesc.StartTrackPos = Desc["StartTrackPos"].asDouble();

		eParticleDesc.bLoop = Desc["Loop"].asBool();
		eParticleDesc.bUseDiffuse = Desc["UseDiffuse"].asBool();
		eParticleDesc.strTexDiffuseKey = Desc["TexDiffuseKey"].asString();

		eParticleDesc.iMaxParticles = Desc["MaxParticles"].asInt();
		eParticleDesc.iSpawnType = Desc["SpawnType"].asInt();

		if (Desc.isMember("LifeTime") && Desc["LifeTime"].isArray() && Desc["LifeTime"].size() == 2) {
			Json::Value LifeTime = Desc["LifeTime"];
			eParticleDesc.vLifeTime = _float2(LifeTime[0].asFloat(), LifeTime[1].asFloat());
		}

		eParticleDesc.fSpawnNumPerSec = Desc["SpawnNumPerSec"].asFloat();
		eParticleDesc.iRenderType = Desc["RenderType"].asInt();
		eParticleDesc.iBillBoardType = Desc["BillBoardType"].asInt();

		eParticleDesc.iPosType = Desc["PosType"].asInt();
		eParticleDesc.bPosFollow = Desc["PosFollow"].asBool();
		eParticleDesc.bUsePosKeyFrame = false;
		if (Desc.isMember("UsePosKeyFrame"))
		{
			eParticleDesc.bUsePosKeyFrame = Desc["UsePosKeyFrame"].asBool();
		}
		eParticleDesc.strPosFollowBoneKey = Desc["PosFollowBoneKey"].asString();

		eParticleDesc.bUseHazeEffect = Desc["UseHazeEffect"].asBool();
		if (Desc.isMember("RelativeOwner"))
			eParticleDesc.bRelativeOwner = Desc["RelativeOwner"].asBool();
		else
			eParticleDesc.bRelativeOwner = false;

		if (Desc["HazeSpeedMin"].isArray())
		{
			Json::Value HazeSpeedMin = Desc["HazeSpeedMin"];
			eParticleDesc.vHazeSpeedMin = _float3(HazeSpeedMin[0].asFloat(), HazeSpeedMin[1].asFloat(), HazeSpeedMin[2].asFloat());
		}

		if (Desc["HazeSpeedMax"].isArray())
		{
			Json::Value HazeSpeedMax = Desc["HazeSpeedMax"];
			eParticleDesc.vHazeSpeedMax = _float3(HazeSpeedMax[0].asFloat(), HazeSpeedMax[1].asFloat(), HazeSpeedMax[2].asFloat());
		}

		eParticleDesc.bUseGlowColor = Desc["UseGlowColor"].asBool();

		if (Desc["GlowColor"].isArray())
		{
			Json::Value GlowColor = Desc["GlowColor"];
			eParticleDesc.vGlowColor = _float4(GlowColor[0].asFloat(), GlowColor[1].asFloat(), GlowColor[2].asFloat(), GlowColor[3].asFloat());
		}

		if (!eParticleDesc.bUsePosKeyFrame)
		{
			if (Desc["PosMin"].isArray())
			{
				Json::Value PosMin = Desc["PosMin"];
				eParticleDesc.vPosMin = _float3(PosMin[0].asFloat(), PosMin[1].asFloat(), PosMin[2].asFloat());
			}

			if (Desc["PosMax"].isArray())
			{
				Json::Value PosMax = Desc["PosMax"];
				eParticleDesc.vPosMax = _float3(PosMax[0].asFloat(), PosMax[1].asFloat(), PosMax[2].asFloat());
			}
		}

		eParticleDesc.bUsePivot = Desc["UsePivot"].asBool();

		if (Desc["PivotMin"].isArray())
		{
			Json::Value PivotMin = Desc["PivotMin"];
			eParticleDesc.vPivotMin = _float3(PivotMin[0].asFloat(), PivotMin[1].asFloat(), PivotMin[2].asFloat());
		}

		if (Desc["PivotMax"].isArray())
		{
			Json::Value PivotMax = Desc["PivotMax"];
			eParticleDesc.vPivotMax = _float3(PivotMax[0].asFloat(), PivotMax[1].asFloat(), PivotMax[2].asFloat());
		}

		eParticleDesc.iShape = Desc["Shape"].asInt();
		eParticleDesc.bSpeedMode = Desc["SpeedMode"].asBool();

		if (Desc["StartSpeedMin"].isArray())
		{
			Json::Value StartSpeedMin = Desc["StartSpeedMin"];
			eParticleDesc.vStartSpeedMin = _float3(StartSpeedMin[0].asFloat(), StartSpeedMin[1].asFloat(), StartSpeedMin[2].asFloat());
		}

		if (Desc["StartSpeedMax"].isArray())
		{
			Json::Value StartSpeedMax = Desc["StartSpeedMax"];
			eParticleDesc.vStartSpeedMax = _float3(StartSpeedMax[0].asFloat(), StartSpeedMax[1].asFloat(), StartSpeedMax[2].asFloat());
		}

		if (Desc["SpeedAccMin"].isArray())
		{
			Json::Value SpeedAccMin = Desc["SpeedAccMin"];
			eParticleDesc.vSpeedAccMin = _float3(SpeedAccMin[0].asFloat(), SpeedAccMin[1].asFloat(), SpeedAccMin[2].asFloat());
		}

		if (Desc["SpeedAccMax"].isArray())
		{
			Json::Value SpeedAccMax = Desc["SpeedAccMax"];
			eParticleDesc.vSpeedAccMax = _float3(SpeedAccMax[0].asFloat(), SpeedAccMax[1].asFloat(), SpeedAccMax[2].asFloat());
		}


		if (Desc["RotationMin"].isArray())
		{
			Json::Value RotationMin = Desc["RotationMin"];
			eParticleDesc.vRotationMin = _float3(RotationMin[0].asFloat(), RotationMin[1].asFloat(), RotationMin[2].asFloat());
		}

		if (Desc["RotationMax"].isArray())
		{
			Json::Value RotationMax = Desc["RotationMax"];
			eParticleDesc.vRotationMax = _float3(RotationMax[0].asFloat(), RotationMax[1].asFloat(), RotationMax[2].asFloat());
		}

		if (Desc["TurnVelMin"].isArray())
		{
			Json::Value TurnVelMin = Desc["TurnVelMin"];
			eParticleDesc.vTurnVelMin = _float3(TurnVelMin[0].asFloat(), TurnVelMin[1].asFloat(), TurnVelMin[2].asFloat());
		}

		if (Desc["TurnVelMax"].isArray())
		{
			Json::Value TurnVelMax = Desc["TurnVelMax"];
			eParticleDesc.vTurnVelMax = _float3(TurnVelMax[0].asFloat(), TurnVelMax[1].asFloat(), TurnVelMax[2].asFloat());
		}

		if (Desc["TurnAccMin"].isArray())
		{
			Json::Value TurnAccMin = Desc["TurnAccMin"];
			eParticleDesc.vTurnAccMin = _float3(TurnAccMin[0].asFloat(), TurnAccMin[1].asFloat(), TurnAccMin[2].asFloat());
		}

		if (Desc["TurnAccMax"].isArray())
		{
			Json::Value TurnAccMax = Desc["TurnAccMax"];
			eParticleDesc.vTurnAccMax = _float3(TurnAccMax[0].asFloat(), TurnAccMax[1].asFloat(), TurnAccMax[2].asFloat());
		}

		eParticleDesc.bUseColor = Desc["UseColor"].asBool();

		Json::Value& KeyFrames = Desc["KeyFrames"];

		Json::Value& ScaleKeyFrame = KeyFrames["Scale"];
		Json::Value& SpeedKeyFrame = KeyFrames["Speed"];
		Json::Value& ColorKeyFrame = KeyFrames["Color"];
		Json::Value& PosKeyFrame = KeyFrames["Pos"];

		{
			Json::Value& ScaleKeyFrameTime = ScaleKeyFrame["Time"];
			Json::Value& ScaleKeyFrameEase = ScaleKeyFrame["EaseType"];
			Json::Value& ScaleKeyFrameScaleMin = ScaleKeyFrame["ScaleMin"];
			Json::Value& ScaleKeyFrameScaleMax = ScaleKeyFrame["ScaleMax"];

			_float	fTime[MAX_TIMETABLE] = {};
			_int	iEase[MAX_TIMETABLE] = {};
			_float3	vMin[MAX_TIMETABLE] = {};
			_float3	vMax[MAX_TIMETABLE] = {};

			if (ScaleKeyFrameTime.isArray())
			{
				_int iScaleIdx = 0;

				for (const auto& ScaleTime : ScaleKeyFrameTime)
				{
					fTime[iScaleIdx++] = ScaleTime.asFloat();
				}

				iScaleIdx = 0;

				for (const auto& ScaleEase : ScaleKeyFrameEase)
				{
					iEase[iScaleIdx++] = ScaleEase.asInt();
				}

				iScaleIdx = 0;

				for (const auto& ScaleValue : ScaleKeyFrameScaleMin)
				{
					vMin[iScaleIdx++] = _float3(ScaleValue[0].asFloat(), ScaleValue[1].asFloat(), ScaleValue[2].asFloat());
				}

				iScaleIdx = 0;

				for (const auto& ScaleValue : ScaleKeyFrameScaleMax)
				{
					vMax[iScaleIdx++] = _float3(ScaleValue[0].asFloat(), ScaleValue[1].asFloat(), ScaleValue[2].asFloat());
				}
			}

			CParticleSystem::SCALE_KEYFRAME_PARTICLE scaleKeyFrame;

			for (_int i = 0; i < Desc["ScaleSetCnt"].asInt(); ++i)
			{
				scaleKeyFrame.fTime = fTime[i];
				scaleKeyFrame.eEaseType = iEase[i];
				scaleKeyFrame.vScaleMin = vMin[i];
				scaleKeyFrame.vScaleMax = vMax[i];

				eParticleDesc.KeyFrames.ScaleKeyFrames.push_back(scaleKeyFrame);
			}
		}

		if (eParticleDesc.bUsePosKeyFrame)
		{
			Json::Value& PosKeyFrameTime = PosKeyFrame["Time"];
			Json::Value& PosKeyFrameEase = PosKeyFrame["EaseType"];
			Json::Value& PosKeyFramePosMin = PosKeyFrame["PosMin"];
			Json::Value& PosKeyFramePosMax = PosKeyFrame["PosMax"];

			_float	fTime[MAX_TIMETABLE] = {};
			_int	iEase[MAX_TIMETABLE] = {};
			_float3	vMin[MAX_TIMETABLE] = {};
			_float3	vMax[MAX_TIMETABLE] = {};

			if (PosKeyFrameTime.isArray())
			{
				_int iPosIdx = 0;

				for (const auto& PosTime : PosKeyFrameTime)
				{
					fTime[iPosIdx++] = PosTime.asFloat();
				}

				iPosIdx = 0;

				for (const auto& PosEase : PosKeyFrameEase)
				{
					iEase[iPosIdx++] = PosEase.asInt();
				}

				iPosIdx = 0;

				for (const auto& PosValue : PosKeyFramePosMin)
				{
					vMin[iPosIdx++] = _float3(PosValue[0].asFloat(), PosValue[1].asFloat(), PosValue[2].asFloat());
				}

				iPosIdx = 0;

				for (const auto& PosValue : PosKeyFramePosMax)
				{
					vMax[iPosIdx++] = _float3(PosValue[0].asFloat(), PosValue[1].asFloat(), PosValue[2].asFloat());
				}
			}

			CParticleSystem::POSITION_KEYFRAME_PARTICLE PosKeyFrame;

			for (_int i = 0; i < Desc["PosSetCnt"].asInt(); ++i)
			{
				PosKeyFrame.fTime = fTime[i];
				PosKeyFrame.eEaseType = iEase[i];
				PosKeyFrame.vPosMin = vMin[i];
				PosKeyFrame.vPosMax = vMax[i];

				eParticleDesc.KeyFrames.PosKeyFrames.push_back(PosKeyFrame);
			}
		}

		{
			Json::Value& SpeedKeyFrameTime = SpeedKeyFrame["Time"];
			Json::Value& SpeedKeyFrameEase = SpeedKeyFrame["EaseType"];
			Json::Value& SpeedKeyFrameMin = SpeedKeyFrame["SpeedMin"];
			Json::Value& SpeedKeyFrameMax = SpeedKeyFrame["SpeedMax"];

			_float	fTime[MAX_TIMETABLE] = {};
			_int	iEase[MAX_TIMETABLE] = {};
			_float3	vMin[MAX_TIMETABLE] = {};
			_float3	vMax[MAX_TIMETABLE] = {};

			if (SpeedKeyFrameTime.isArray())
			{
				_int iIdx = 0;

				for (const auto& Time : SpeedKeyFrameTime)
				{
					fTime[iIdx++] = Time.asFloat();
				}

				iIdx = 0;

				for (const auto& Ease : SpeedKeyFrameEase)
				{
					iEase[iIdx++] = Ease.asInt();
				}

				iIdx = 0;

				for (const auto& Min : SpeedKeyFrameMin)
				{
					vMin[iIdx++] = _float3(Min[0].asFloat(), Min[1].asFloat(), Min[2].asFloat());
				}

				iIdx = 0;

				for (const auto& Max : SpeedKeyFrameMax)
				{
					vMax[iIdx++] = _float3(Max[0].asFloat(), Max[1].asFloat(), Max[2].asFloat());
				}
			}

			CParticleSystem::SPEED_KEYFRAME_PARTICLE speedKeyFrame;

			for (_int i = 0; i < Desc["SpeedSetCnt"].asInt(); ++i)
			{
				speedKeyFrame.fTime = fTime[i];
				speedKeyFrame.eEaseType = iEase[i];
				speedKeyFrame.vSpeedMin = vMin[i];
				speedKeyFrame.vSpeedMax = vMax[i];

				eParticleDesc.KeyFrames.SpeedKeyFrames.push_back(speedKeyFrame);
			}
		}

		{
			Json::Value& ColorKeyFrameTime = ColorKeyFrame["Time"];
			Json::Value& ColorKeyFrameEase = ColorKeyFrame["EaseType"];
			Json::Value& ColorKeyFrameMin = ColorKeyFrame["ColorMin"];
			Json::Value& ColorKeyFrameMax = ColorKeyFrame["ColorMax"];

			_float	fTime[MAX_TIMETABLE] = {};
			_int	iEase[MAX_TIMETABLE] = {};
			_float4	vMin[MAX_TIMETABLE] = {};
			_float4	vMax[MAX_TIMETABLE] = {};

			if (ColorKeyFrameTime.isArray())
			{
				_int iIdx = 0;

				for (const auto& Time : ColorKeyFrameTime)
				{
					fTime[iIdx++] = Time.asFloat();
				}

				iIdx = 0;

				for (const auto& Ease : ColorKeyFrameEase)
				{
					iEase[iIdx++] = Ease.asInt();
				}

				iIdx = 0;

				for (const auto& Min : ColorKeyFrameMin)
				{
					vMin[iIdx++] = _float4(Min[0].asFloat(), Min[1].asFloat(), Min[2].asFloat(), Min[3].asFloat());
				}

				iIdx = 0;

				for (const auto& Max : ColorKeyFrameMax)
				{
					vMax[iIdx++] = _float4(Max[0].asFloat(), Max[1].asFloat(), Max[2].asFloat(), Max[3].asFloat());
				}
			}

			CParticleSystem::COLOR_KEYFRAME_PARTICLE colorKeyFrame;

			for (_int i = 0; i < Desc["ColorSetCnt"].asInt(); ++i)
			{
				colorKeyFrame.fTime = fTime[i];
				colorKeyFrame.eEaseType = iEase[i];
				colorKeyFrame.vColorMin = vMin[i];
				colorKeyFrame.vColorMax = vMax[i];

				eParticleDesc.KeyFrames.ColorKeyFrames.push_back(colorKeyFrame);
			}
		}

		shared_ptr<CParticleSystem> pParticle = CParticleSystem::Create(&eParticleDesc);
		m_pEffectList.push_back(pParticle);
		m_strEffectList.push_back(Element["EffectID"].asString());

		m_pLoadEffectGroup->AddEffect(Element["EffectID"].asString(), pParticle);
	}

	return S_OK;
}

HRESULT CImGuiMgr::ModelToClient(string _strModelPath, string _strModelKey)
{
	filesystem::path basePath(_strModelPath);

	for (const auto& entry : filesystem::directory_iterator(basePath))
	{
		if (entry.is_directory())
			ModelToClient(entry.path().string(), _strModelKey);
		else if (entry.is_regular_file())
		{
			if (_strModelKey == entry.path().filename().stem().string())
			{
				string relativePath = entry.path().string().substr((m_strOriginPath + "Mesh/").length());

				filesystem::path CopyModelPath(m_strClientRscPth + "Model/" + relativePath);
				filesystem::copy(entry.path(), CopyModelPath, filesystem::copy_options::overwrite_existing);
			}
		}
	}

	return S_OK;
}

HRESULT CImGuiMgr::TextureToClient(TEX_TYPE _iTexType, string _strTextureKey)
{
	filesystem::path OriginPath;
	filesystem::path CopyPath;

	switch (_iTexType)
	{
	case Tool_Effect::TEX_DIFFUSE:
		OriginPath = m_strOriginPath + "Diffuse/" + _strTextureKey + ".png";
		CopyPath = m_strClientRscPth + "Diffuse/" + _strTextureKey + ".png";
		break;
	case Tool_Effect::TEX_MASK:
		OriginPath = m_strOriginPath + "Diffuse/" + _strTextureKey + ".png";
		CopyPath = m_strClientRscPth + "Mask/" + _strTextureKey + ".png";
		break;
	case Tool_Effect::TEX_NOISE:
		OriginPath = m_strOriginPath + "Noise/" + _strTextureKey + ".png";
		CopyPath = m_strClientRscPth + "Noise/" + _strTextureKey + ".png";
		break;
	case Tool_Effect::TEX_END:
		OriginPath = m_strOriginPath + "Particle/" + _strTextureKey + ".png";
		CopyPath = m_strClientRscPth + "Particle/" + _strTextureKey + ".png";
		break;
	default:
		break;
	}

	filesystem::copy(OriginPath, CopyPath, filesystem::copy_options::overwrite_existing);

	return S_OK;
}
