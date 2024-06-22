#include "pch.h"
#include "LevelEditor.h"
#include "Map.h"
#include "TestObject.h"
#include "CameraKeyFrame.h"

CLevelEditor::CLevelEditor()
{
}

CLevelEditor::~CLevelEditor()
{
}

shared_ptr<CLevelEditor> CLevelEditor::Create()
{
	shared_ptr<CLevelEditor> pInstance = make_shared<CLevelEditor>();

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CLevelEditor::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CLevelEditor::Initialize()
{
	/*
	    case LEVEL_VILLAGE:
        hr = LoadingForVillage();
		GAMEINSTANCE->CreateOctree({ 250.0f,0.0f,250.0f }, 512.0f, 4);
        m_pNextLevel = CLevelVillage::Create();
        m_IsFinished = true;
        break;

    case LEVEL_DUNGEON_MAIN:
        LoadingForDungeonMain();
		GAMEINSTANCE->CreateOctree({ 21.15f,0.0f,40.0f }, 160.0f, 4);
        m_pNextLevel = CLevelDungeonMain::Create();
        m_IsFinished = true;
        break;

    case LEVEL_DUNGEON_FINAL:
        LoadingForDungeonFinal();
        GAMEINSTANCE->CreateOctree({ 21.15f,-16.0f,220.0f }, 176.0f, 4);
        m_pNextLevel = CLevelDungeonFinal::Create();
        m_IsFinished = true;
        break;

    case LEVEL_FONOS:
        LoadingForFonos();
		GAMEINSTANCE->CreateOctree({ 128,0.0f,128.0f }, 224.0f, 4);
        m_pNextLevel = CLevelFonos::Create();
        m_IsFinished = true;
        break;

	*/
	GAMEINSTANCE->CreateOctree({ 128,0.0f,128.0f }, 224.0f, 4);

	Init_Camera();
	Init_Map();
	Init_TestObj();

	LIGHT_DESC		LightDesc = {};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.85f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(GAMEINSTANCE->AddLight(LightDesc)))
		return E_FAIL;

	return S_OK;
}

void CLevelEditor::Tick(_float _fTimeDelta)
{
	if (m_pDirectionCamera->isDirection())
	{
		m_pFreeCamera->CameraTurnOff();
		m_pCurrentCamera = m_pDirectionCamera;
	}
	else
	{
		m_pFreeCamera->CameraTurnOn();
		m_pCurrentCamera = m_pFreeCamera;
	}


	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	Render_Collider_Window(_fTimeDelta);
	Render_Camera_Window(_fTimeDelta);
	RenderLightWindow(_fTimeDelta);

	ImgZmoUpdate();

	if (GAMEINSTANCE->KeyDown(DIK_T))
	{
		CameraDirection();
	}

	if (GAMEINSTANCE->KeyDown(DIK_Y))
	{
		m_pDirectionCamera->EndDirection();
	}

	if (GAMEINSTANCE->KeyDown(DIK_G))
	{
		m_isRenderCamBox = !m_isRenderCamBox;
	}
	/*if (GAMEINSTANCE->KeyDown(DIK_NUMPAD1))
	{
		CameraDirection();
	}
	if (GAMEINSTANCE->KeyDown(DIK_NUMPAD2))
	{
		m_pDirectionCamera->EndDirection();
	}*/
	if (m_isRenderCamBox)
	{
		if (iSelectPath < (int)m_CameraKeyFrames.size())
		{
			for (auto KeyFrame : m_CameraKeyFrames[iSelectPath].second)
			{
				KeyFrame->LateTick(_fTimeDelta);
			}
		}
	}

	static UINT iRenderType = 1;
	if (GAMEINSTANCE->KeyDown(DIK_F))
	{
		if (1 == iRenderType)
			iRenderType = 2;
		else
			iRenderType = 1;
	}
	if (!m_Colliders[m_iSelectColVector].empty())
	{
		for (auto ColliderFrame : m_Colliders[m_iSelectColVector])
		{
			ColliderFrame->LateTick(_fTimeDelta, iRenderType);
		}
	}

	if (GAMEINSTANCE->MouseDown(DIM_RB))
	{
		if (m_iPickType == 0)
		{
			if (!ColliderPicking(&m_pPickCol, &m_vPickingPosition, m_iSelectColLine))
			{
				m_pPickCol.reset();
			}
		}
		else if (m_iPickType == 1)
		{
			if (!m_CameraKeyFrames.empty())
			{
				if (!CameraPicking(&m_pPickCamera, &m_vPickingPosition, m_CameraKeyFrames[iSelectPath].first))
				{
					m_pPickCamera.reset();
				}
			}
		}
	}


}


void CLevelEditor::LateTick(_float _fTimeDelta)
{
}

HRESULT CLevelEditor::Render()
{
	return S_OK;
}

HRESULT CLevelEditor::Init_Camera()
{
	m_pFreeCamera = CCamera_Direction::Create();
	m_pFreeCamera->CameraTurnOn();
	m_pCurrentCamera = m_pFreeCamera;

	if (FAILED(GAMEINSTANCE->AddObject(LEVEL_EDITOR, L"Layer_Camera", m_pFreeCamera)))
		return E_FAIL;

	m_pDirectionCamera = CCamera_Direction::Create();

	if (FAILED(GAMEINSTANCE->AddObject(LEVEL_EDITOR, L"Layer_Camera", m_pDirectionCamera)))
		return E_FAIL;

	m_fCamVelocity = m_pCurrentCamera->GetTransform()->GetSpeed();

	return S_OK;
}

HRESULT CLevelEditor::Init_Map()
{
	//string strMapPath = "..\\..\\Client\\Bin\\DataFiles\\MapData\\Dungeon_Main_Test7.dat";
	//string strMapPath = "..\\..\\Client\\Bin\\DataFiles\\MapData\\Village.dat";
	string strMapPath = "..\\..\\Client\\Bin\\DataFiles\\MapData\\Fonos.dat";
	//string strMapPath = "..\\..\\Client\\Bin\\DataFiles\\MapData\\Dungeon_Final9.dat";
	shared_ptr<CMap> pMap = CMap::Create(strMapPath, false);
	if (nullptr == pMap)
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->AddObject(LEVEL_EDITOR, TEXT("Layer_Map"), pMap)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevelEditor::Init_TestObj()
{
	shared_ptr<CTestObject> pTestObj = CTestObject::Create();

	if (FAILED(GAMEINSTANCE->AddObject(LEVEL_EDITOR, L"Layer_Test", pTestObj)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevelEditor::ImgZmoUpdate()
{
	if (m_iPickType == 0)
	{
		if (m_pPickCol.lock())
		{
			shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
			ImGuizmo::BeginFrame();
			ImGuizmo::Enable(true);

			ImGuiIO& io = ImGui::GetIO();

			_float4x4 ViewMat = pGameInstance->GetTransformMatrix(CPipeLine::D3DTS_VIEW);
			_float4x4 ProjMat = pGameInstance->GetTransformMatrix(CPipeLine::D3DTS_PROJ);

			shared_ptr<CColliderFrame> pColFrame = m_pPickCol.lock();
			float* WorldMat = (float*)pColFrame->GetTransform()->GetWorldMatrixPtr();

			ImGui::InputFloat3("Position", &WorldMat[12]);

			float* View = (float*)ViewMat.m;
			float* Proj = (float*)ProjMat.m;
			float* Iden = (float*)XMMatrixIdentity().r;

			/*static int GizmoOperation = 0;
			static _float3 m_ObjTranslation;
			static _float3 m_ObjRotation;
			static _float3 m_ObjScale;*/

			//SetRect < 꼭 넣어줘야함! 뭔가 그리기 전에 캔버스? 같은 역할을 하나봄
			ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

			//그리드 생성
			//ImGuizmo::DrawGrid(View, Proj, Iden, 100.f);

			//실질적인 기능을 해주는 function
			ImGuizmo::Manipulate(View, Proj, m_CurrentGizmoOperation, m_CurrentGizmoMode, WorldMat);

			//ImGuizmo::DecomposeMatrixToComponents(WorldMat, (float*)&m_ObjTranslation, (float*)&m_ObjRotation, (float*)&m_ObjScale);

			string GeometryTag;
			EGEOMETRY_TYPE GeometryType = pColFrame->GetGeometry()->GetType();
			switch (GeometryType)
			{
			case GT_SPHERE:
			{
				GeometryTag = "SPHERE";
			}
			break;
			case GT_BOX:
			{
				GeometryTag = "BOX";
			}
			break;
			case GT_CAPSULE:
			{
				GeometryTag = "CAPSULE";
			}
			break;
			}

			ImGui::Text(GeometryTag.c_str());
			ImGui::Separator();
			switch (GeometryType)
			{
			case GT_SPHERE:
			{
				ImGui::Text(" R:%.3f, Radius", static_pointer_cast<GeometrySphere>(pColFrame->GetGeometry())->fRadius);
			}
			break;
			case GT_BOX:
			{
				_float3 vSize = static_pointer_cast<GeometryBox>(pColFrame->GetGeometry())->vSize;
				ImGui::Text(" X:%.3f, Y:%.3f, Z:%.3f, Size", vSize.x, vSize.y, vSize.z);
			}
			break;
			case GT_CAPSULE:
			{
				float fHeight = static_pointer_cast<GeometryCapsule>(pColFrame->GetGeometry())->fHeight;
				float fRadius = static_pointer_cast<GeometryCapsule>(pColFrame->GetGeometry())->fRadius;
				ImGui::Text(" H:%.3f, R:%.3f, Height Ridius", fHeight, fRadius);
			}
			break;
			}
			ImGui::Text(" X:%.3f, Y:%.3f, Z:%.3f, Translation", WorldMat[12], WorldMat[13], WorldMat[14]);
		}
	}
	else if (m_iPickType == 1)
	{
		if (m_pPickCamera.lock())
		{
			shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
			ImGuizmo::BeginFrame();
			ImGuizmo::Enable(true);

			static int iEyeAt = 0;
			ImGui::RadioButton("Eye", &iEyeAt, 0); ImGui::SameLine();
			ImGui::RadioButton("At", &iEyeAt, 1); ImGui::SameLine();

			ImGuiIO& io = ImGui::GetIO();

			_float4x4 ViewMat = pGameInstance->GetTransformMatrix(CPipeLine::D3DTS_VIEW);
			_float4x4 ProjMat = pGameInstance->GetTransformMatrix(CPipeLine::D3DTS_PROJ);

			shared_ptr<CCameraKeyFrame> pKeyFrame = m_pPickCamera.lock();

			float* WorldMat = nullptr;;
			if (iEyeAt == 0)
			{
				WorldMat = (float*)pKeyFrame->GetTransform()->GetWorldMatrixPtr();
			}
			else
			{
				WorldMat = (float*)pKeyFrame->GetTransformAt()->GetWorldMatrixPtr();
			}

			ImGui::InputFloat3("Position", &WorldMat[12]);

			float* View = (float*)ViewMat.m;
			float* Proj = (float*)ProjMat.m;
			float* Iden = (float*)XMMatrixIdentity().r;

			static int GizmoOperation = 0;

			//SetRect < 꼭 넣어줘야함! 뭔가 그리기 전에 캔버스? 같은 역할을 하나봄
			ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

			////그리드 생성
			//ImGuizmo::DrawGrid(View, Proj, Iden, 100.f);

			//실질적인 기능을 해주는 function
			ImGuizmo::Manipulate(View, Proj, m_CurrentGizmoOperation2, m_CurrentGizmoMode, WorldMat);

			ImGui::Text(" X:%.3f, Y:%.3f, Z:%.3f, Right", WorldMat[0], WorldMat[1], WorldMat[2]);
			ImGui::Text(" X:%.3f, Y:%.3f, Z:%.3f, Up", WorldMat[4], WorldMat[5], WorldMat[6]);
			ImGui::Text(" X:%.3f, Y:%.3f, Z:%.3f, Look", WorldMat[8], WorldMat[9], WorldMat[10]);

		}
	}
	
	return S_OK;
}

HRESULT CLevelEditor::Render_Collider_Window(float _fTimeDelta)
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
	ImGuiIO& io = ImGui::GetIO();

	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, main_viewport->Size.y * 0.5f), ImGuiCond_Once);
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

	/* Begin */
	ImGui::Begin("Collider", nullptr, window_flags);                          // Create a window called "Hello, world!" and append into it.
	ImGui::Separator();
	ImGui::RadioButton("Pick Col", &m_iPickType, 0); ImGui::SameLine();
	ImGui::RadioButton("Pick Cam", &m_iPickType, 1);
	ImGui::Separator();
	ImGui::Text("Green: Box, Red: Sphere, Yellow: Capsule");
	ImGui::Separator();
	if (ImGui::InputInt("Index (0~9)", &m_iSelectColVector))
	{
		m_iSelectColLine = 0;
		m_pPickCol.reset();
		if (m_iSelectColVector >= 10)
		{
			m_iSelectColVector = 9;
		}
		else if (m_iSelectColVector < 0)
		{
			m_iSelectColVector = 0;
		}
	}
	if (ImGui::TreeNode("Data Handle"))
	{
		ImGui::InputText("FileName", &m_strFileNameCol);
		if (ImGui::Button("Save"))
		{
			if (FAILED(SaveColliderData()))
			{
				MessageBox(nullptr, L"저장 실패", L"system", MB_OK);
			}
			else
			{
				MessageBox(nullptr, L"저장 성공", L"system", MB_OK);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			if (FAILED(LoadColliderData()))
			{
				MessageBox(nullptr, L"불러오기 실패", L"system", MB_OK);
			}
			else
			{
				MessageBox(nullptr, L"불러오기 성공", L"system", MB_OK);
			}
		}

		if (ImGui::TreeNode("Optional Function"))
		{
			static int iAB[2]{};
			if (ImGui::InputInt2("A, B", iAB))
			{
				if (iAB[0] >= 10)
				{
					iAB[0] = 9;
				}
				else if (iAB[0] < 0)
				{
					iAB[0] = 0;
				}

				if (iAB[1] >= 10)
				{
					iAB[1] = 9;
				}
				else if (iAB[1] < 0)
				{
					iAB[1] = 0;
				}
			}
			if (ImGui::Button("Combine"))
			{
				CombineColliderData(iAB[0], iAB[1]);
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				m_Colliders[m_iSelectColVector].clear();
			}

			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	
	ImGui::Separator();
	ImGui::RadioButton("BOX", &m_iColType, COL_BOX); ImGui::SameLine();
	ImGui::RadioButton("CAPSULE", &m_iColType, COL_CAPSULE); ImGui::SameLine();
	ImGui::RadioButton("SPHERE", &m_iColType, COL_SPHERE);
	if (ImGui::Button("Create"))
	{
		shared_ptr<Geometry> pGeomertry = nullptr;
		if (m_iColType == COL_BOX)
		{
			shared_ptr<GeometryBox>  GeoBox = make_shared<GeometryBox>();
			GeoBox->vSize = m_vBoxSize;;
			pGeomertry = GeoBox;
		}
		else if (m_iColType == COL_CAPSULE)
		{
			shared_ptr<GeometryCapsule> GeoCap = make_shared<GeometryCapsule>();
			GeoCap->fHeight = m_fCapHeight;
			GeoCap->fRadius = m_fCapRadius;
			pGeomertry = GeoCap;
		}
		else if (m_iColType == COL_SPHERE)
		{
			shared_ptr<GeometrySphere> GeoSph = make_shared<GeometrySphere>();
			GeoSph->fRadius = m_fSphRadius;
			pGeomertry = GeoSph;
		}
		shared_ptr<CColliderFrame> pColliderFrame = CColliderFrame::Create(pGeomertry, m_pCurrentCamera->GetTransform()->GetState(CTransform::STATE_POSITION));
		if (pColliderFrame)
		{
			m_Colliders[m_iSelectColVector].push_back(pColliderFrame);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		if (m_iSelectColLine < (int)m_Colliders[m_iSelectColVector].size())
		{
			m_Colliders[m_iSelectColVector].erase(m_Colliders[m_iSelectColVector].begin() + m_iSelectColLine);
		}
	}
	ImGui::Separator();
	if (m_iColType == COL_BOX)
	{
		ImGui::InputFloat3("BoxSize1", (float*)&m_vBoxSize);
		ImGui::SliderFloat3("BoxSize2", (float*)&m_vBoxSize, 0.1f, 100.0f);
	}
	else if (m_iColType == COL_CAPSULE)
	{
		ImGui::InputFloat("CapHeight1", &m_fCapHeight);
		ImGui::SliderFloat("CapHeight2", &m_fCapHeight, 0.0f, 100.0f);
		ImGui::InputFloat("CapRadius1", &m_fCapRadius);
		ImGui::SliderFloat("CapRadius2", &m_fCapRadius, 0.1f, 100.0f);
	}
	else if (m_iColType == COL_SPHERE)
	{
		ImGui::InputFloat("SphRadius1", &m_fSphRadius);
		ImGui::SliderFloat("SphRadius2", &m_fSphRadius, 0.1f, 100.0f);
	}
	ImGui::Separator();

	if (!m_Colliders[m_iSelectColVector].empty())
	{
		UINT iNumCollider = (UINT)m_Colliders[m_iSelectColVector].size();
		vector<string> Colliders;
		Colliders.reserve(iNumCollider);
		int iShphere = 0;
		int iBox = 0;
		int iCapsule = 0;
		for (UINT i = 0; i < iNumCollider; i++)
		{
			string Name;
			switch (m_Colliders[m_iSelectColVector][i]->GetGeometry()->GetType())
			{
			case GT_SPHERE:
			{
				Name = "Sphere_" + to_string(iShphere++);
			}
			break;
			case GT_BOX:
			{
				Name = "Box_" + to_string(iBox++);
			}
			break;
			case GT_CAPSULE:
			{
				Name = "Capsule_" + to_string(iCapsule++);
			}
			break;
			}
			Colliders.push_back(Name);
		}
		if (ImGui::BeginListBox("ColliderList"))
		{
			for (UINT i = 0; i < iNumCollider; i++)
			{
				const bool item_selected = (i == m_iSelectColLine);
				if (ImGui::Selectable(Colliders[i].c_str(), item_selected))
				{
					m_iSelectColLine = i;
					m_pPickCol = m_Colliders[m_iSelectColVector][i];
				}
				if (item_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndListBox();
		}
	}

	if (m_iPickType == 0)
	{
		if (m_pPickCol.lock())
		{
			ImGui::Separator();
			ImGui::Text("PickCol Setting");
			shared_ptr<CColliderFrame> pColFrame = m_pPickCol.lock();

			shared_ptr<Geometry> pGeometry = pColFrame->GetGeometry();
			int iColType = pGeometry->GetType();

			ImGui::InputFloat3("OffSetRot1", (float*)&pGeometry->vOffSetRotation);
			ImGui::SliderFloat3("OffSetRot2", (float*)&pGeometry->vOffSetRotation, 0.0f, 360.0f);

			if (iColType == GT_BOX)
			{
				shared_ptr<GeometryBox> pGeoBox = static_pointer_cast<GeometryBox>(pGeometry);
				ImGui::InputFloat3("BoxSize1", (float*)&pGeoBox->vSize);
				ImGui::SliderFloat3("BoxSize2", (float*)&pGeoBox->vSize, 0.1f, 100.0f);
			}
			else if (iColType == GT_CAPSULE)
			{
				shared_ptr<GeometryCapsule> pGeoCap = static_pointer_cast<GeometryCapsule>(pGeometry);
				ImGui::InputFloat("CapHeight1", &pGeoCap->fHeight);
				ImGui::SliderFloat("CapHeight2", &pGeoCap->fHeight, 0.0f, 100.0f);
				ImGui::InputFloat("CapRadius1", &pGeoCap->fRadius);
				ImGui::SliderFloat("CapRadius2", &pGeoCap->fRadius, 0.1f, 100.0f);
			}
			else if (iColType == GT_SPHERE)
			{
				shared_ptr<GeometrySphere> pGeoSph = static_pointer_cast<GeometrySphere>(pGeometry);
				ImGui::InputFloat("SphRadius1", &pGeoSph->fRadius);
				ImGui::SliderFloat("SphRadius2", &pGeoSph->fRadius, 0.1f, 100.0f);
			}

		}
	}
	else if (m_iPickType == 1)
	{
		if (m_pPickCamera.lock())
		{
			shared_ptr<CCameraKeyFrame> pPickCamera = m_pPickCamera.lock();

			int FadeEnum{ pPickCamera->GetFade() };
			float fCurrentLerpTime{ pPickCamera->GetLerpTime() };
			if (ImGui::InputFloat("LerpTime", &fCurrentLerpTime))
			{
				if (0.0f > fCurrentLerpTime)
					fCurrentLerpTime = 0.0f;
				else if (20.0f < fCurrentLerpTime)
					fCurrentLerpTime = 20.0f;
				pPickCamera->SetLerpTime(fCurrentLerpTime);
			}

			ImGui::SeparatorText("FADE");
			float fCurrentFadeInOutStartTime{ pPickCamera->GetFadeInOutTime() };
			float fCurrentFadeInOutDuration{ pPickCamera->GetFadeInOutDurtaion() };
			if (ImGui::Button("NONE"))
			{
				FadeEnum = 0;
				pPickCamera->SetFade(FadeEnum);
			}
			ImGui::SameLine();
			if (ImGui::Button("FADE IN"))
			{
				FadeEnum = 1;
				pPickCamera->SetFade(FadeEnum);
			}
			ImGui::SameLine();
			if (ImGui::Button("FADE OUT"))
			{
				FadeEnum = 2;
				pPickCamera->SetFade(FadeEnum);
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset"))
			{
				FadeEnum = 0;
				fCurrentFadeInOutStartTime = 0.0f;
				pPickCamera->ResetFade();
			}
			if (ImGui::InputFloat("Fade StartTime", &fCurrentFadeInOutStartTime))
			{
				if (fCurrentFadeInOutStartTime < 0.0f)
				{
					fCurrentFadeInOutStartTime = 0.0f;
					pPickCamera->SetFadeInOutStartTime(fCurrentFadeInOutStartTime);
				}
				else if (fCurrentFadeInOutStartTime > fCurrentLerpTime)
				{
					fCurrentFadeInOutStartTime = fCurrentLerpTime;
					pPickCamera->SetFadeInOutStartTime(fCurrentFadeInOutStartTime);
				}
				else
				{
					pPickCamera->SetFadeInOutStartTime(fCurrentFadeInOutStartTime);
				}
			}
			if (ImGui::InputFloat("Fade Duration", &fCurrentFadeInOutDuration))
			{
				if (fCurrentFadeInOutDuration < 0.0f)
				{
					fCurrentFadeInOutDuration = 0.0f;
					pPickCamera->SetFadeInOutDuration(fCurrentFadeInOutDuration);
				}
				else
				{
					pPickCamera->SetFadeInOutDuration(fCurrentFadeInOutDuration);
				}
			}

			ImGui::Text("LerpTime : %.2f", fCurrentLerpTime);
			string strFadeType{};
			switch (FadeEnum)
			{
			case 0:
				strFadeType = "FADE NONE, Start: %.2f, Dura: %.2f";
				break;
			case 1:
				strFadeType = "FADE IN, Start: %.2f, Dura: %.2f";
				break;
			case 2:
				strFadeType = "FADE OUT, Start: %.2f, Dura: %.2f";
				break;
			}
			ImGui::Text(strFadeType.c_str(), fCurrentFadeInOutStartTime, fCurrentFadeInOutDuration);
			ImGui::SeparatorText("Shake");
			if (ImGui::Button("Add Shake"))
			{
				pPickCamera->SetCameraShake(m_fShakeStartTiem, m_eShakeFlag, m_fShakeDuration, &m_RotDesc, &m_LocDesc, &m_FovDesc);
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset Shake"))
			{
				pPickCamera->ResetShake();
			}
			m_fShakeStartTiem = pPickCamera->GetShakeTime();
			if (ImGui::InputFloat("shake time", &m_fShakeStartTiem))
			{
				pPickCamera->SetShakeTime(m_fShakeStartTiem);
			}

			if (pPickCamera->GetisCameraShake())
			{
				UINT ShakeFlags{ 0 };
				float fShakeStartTime{ 0.0f };
				float fShakeDuration{ 0.0f };
				ROT_DESC RotDesc{};
				LOC_DESC LocDesc{};
				FOV_DESC FovDesc{};
				pPickCamera->GetShakeInfo(&ShakeFlags, &fShakeStartTime, &fShakeDuration, &RotDesc, &LocDesc, &FovDesc);

				ImGui::Text("StartTime: %.2f, Duration: %.2f", fShakeStartTime, fShakeDuration);
				string strShakeFlag{ "ShakeFlags =" };
				if (SHAKE_ROT & ShakeFlags)
				{
					strShakeFlag += " ROT |";
				}
				if (SHAKE_LOC & ShakeFlags)
				{
					strShakeFlag += " LOC |";
				}
				if (SHAKE_FOV & ShakeFlags)
				{
					strShakeFlag += " FOV";
				}
				ImGui::Text(strShakeFlag.c_str());

				if (SHAKE_ROT & ShakeFlags)
				{
					ImGui::SeparatorText("ROT");
					ImGui::Text("Amplitude ( %.4f, %.4f, %.4f )", RotDesc.vRotAmplitude.x, RotDesc.vRotAmplitude.y, RotDesc.vRotAmplitude.z);
					ImGui::Text("Frequency ( %.2f, %.2f, %.2f )", RotDesc.vRotFrequency.x, RotDesc.vRotFrequency.y, RotDesc.vRotFrequency.z);
					ImGui::Text("BlendInOut ( %.3f, %.3f )", RotDesc.vBlendInOut.x, RotDesc.vBlendInOut.y);
					ImGui::Text("InOutWeight ( %.3f, %.3f )", RotDesc.vInOutWeight.x, RotDesc.vInOutWeight.y);
					if (RotDesc.eShakeFunc == SHAKE_WAVE)
						ImGui::Text("ShakeFunc : SHAKE_WAVE");
					else
						ImGui::Text("ShakeFunc : SHAKE_LINEAR");
					if (RotDesc.isOffSetRand)
						ImGui::Text("OffSetRand : True");
					else
						ImGui::Text("OffSetRand : False");
				}
				if (SHAKE_LOC & ShakeFlags)
				{
					ImGui::SeparatorText("LOC");
					ImGui::Text("Amplitude ( %.4f, %.4f, %.4f )", LocDesc.vLocAmplitude.x, LocDesc.vLocAmplitude.y, LocDesc.vLocAmplitude.z);
					ImGui::Text("Frequency ( %.2f, %.2f, %.2f )", LocDesc.vLocFrequency.x, LocDesc.vLocFrequency.y, LocDesc.vLocFrequency.z);
					ImGui::Text("BlendInOut ( %.3f, %.3f )", LocDesc.vBlendInOut.x, LocDesc.vBlendInOut.y);
					ImGui::Text("InOutWeight ( %.3f, %.3f )", LocDesc.vInOutWeight.x, LocDesc.vInOutWeight.y);
					if (LocDesc.eShakeFunc == SHAKE_WAVE)
						ImGui::Text("ShakeFunc : SHAKE_WAVE");
					else
						ImGui::Text("ShakeFunc : SHAKE_LINEAR");
					if (LocDesc.isOffSetRand)
						ImGui::Text("OffSetRand : True");
					else
						ImGui::Text("OffSetRand : False");
				}
				if (SHAKE_FOV & ShakeFlags)
				{
					ImGui::SeparatorText("FOV");
					ImGui::Text("Amplitude ( %.4f )", FovDesc.fFovyAmplitude);
					ImGui::Text("Frequency ( %.2f )", FovDesc.fFovyFrequency);
					ImGui::Text("BlendInOut ( %.3f, %.3f )", FovDesc.vBlendInOut.x, FovDesc.vBlendInOut.y);
					ImGui::Text("InOutWeight ( %.3f, %.3f )", FovDesc.vInOutWeight.x, FovDesc.vInOutWeight.y);
					if (FovDesc.eShakeFunc == SHAKE_WAVE)
						ImGui::Text("ShakeFunc : SHAKE_WAVE");
					else
						ImGui::Text("ShakeFunc : SHAKE_LINEAR");
				}
			}
		}
	}

	ImGui::End();


	return S_OK;
}

HRESULT CLevelEditor::Render_Camera_Window(float _fTimeDelta)
{
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x - 300.0f, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, main_viewport->Size.y), ImGuiCond_Once);
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

	/* Begin */
	ImGui::Begin("Camera", nullptr, window_flags);

	if (ImGui::TreeNode("Camera Info"))
	{
		if (nullptr != m_pCurrentCamera)
		{
			shared_ptr<CTransform> vCamTranform = m_pCurrentCamera->GetTransform();

			_float4x4 vCamWorldMat = vCamTranform->GetWorldMatrix();

			ImGui::Text(" X:%.3f, Y:%.3f, Z:%.3f, Cam Right", vCamWorldMat._11, vCamWorldMat._12, vCamWorldMat._13);
			ImGui::Text(" X:%.3f, Y:%.3f, Z:%.3f, Cam Up", vCamWorldMat._21, vCamWorldMat._22, vCamWorldMat._23);
			ImGui::Text(" X:%.3f, Y:%.3f, Z:%.3f, Cam Look", vCamWorldMat._31, vCamWorldMat._32, vCamWorldMat._33);
			ImGui::Text(" X:%.3f, Y:%.3f, Z:%.3f, Cam Pos", vCamWorldMat._41, vCamWorldMat._42, vCamWorldMat._43);
			ImGui::Text(" V:%.1f, Cam Speed", m_fCamVelocity);
			if (ImGui::InputFloat("Cam Speed", &m_fCamVelocity, 10.0f))
			{
				m_pFreeCamera->GetTransform()->SetSpeed(m_fCamVelocity);
			}
			ImGui::Separator();
		}

		ImGui::TreePop();
	}


	if (ImGui::TreeNode("Camera KeyFrames"))
	{
		if (ImGui::InputInt("Path Num", &iSelectPath))
		{
			int iMaxSize = (int)m_CameraKeyFrames.size();
			if ((iSelectPath > iMaxSize))
				iSelectPath = iMaxSize;
			else if (iSelectPath < 0)
				iSelectPath = 0;
		}

		if (ImGui::TreeNode("Data Handle"))
		{
			ImGui::InputText("FileName", &m_strFileNameCam);
			if (ImGui::Button("Save"))
			{
				if (iSelectPath < (int)m_CameraKeyFrames.size())
				{
					if (FAILED(SaveCameraData()))
					{
						MessageBox(nullptr, L"저장 실패", L"system", MB_OK);
					}
					else
					{
						MessageBox(nullptr, L"저장 성공", L"system", MB_OK);
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Load"))
			{
				if (FAILED(LoadCameraData()))
				{
					MessageBox(nullptr, L"불러오기 실패", L"system", MB_OK);
				}
				else
				{
					MessageBox(nullptr, L"불러오기 성공", L"system", MB_OK);
				}
			}

			if (ImGui::TreeNode("Optional Function"))
			{
				if (ImGui::Button("Clear"))
				{
					m_CameraKeyFrames[iSelectPath].second.clear();
				}

				ImGui::TreePop();
			}
			ImGui::TreePop();
		}

		if (ImGui::Button("Create"))
		{
			m_CameraKeyFrames.push_back({ 0, vector<shared_ptr<CCameraKeyFrame>>() });
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			if (iSelectPath < (int)m_CameraKeyFrames.size())
			{
				m_CameraKeyFrames.erase(m_CameraKeyFrames.begin() + iSelectPath);
			}
		}

		//////////////////////////////////////////////////////////////////////////

		UINT NumPath = (UINT)m_CameraKeyFrames.size();
		for (UINT i = 0; i < NumPath; i++)
		{
			string label = "Path_" + to_string(i);
			if (ImGui::TreeNode(label.c_str()))
			{
				if (ImGui::Button("Add"))
				{
					_float3 vLook = m_pFreeCamera->GetTransform()->GetState(CTransform::STATE_LOOK);
					_float3 vPos = m_pFreeCamera->GetTransform()->GetState(CTransform::STATE_POSITION);
					shared_ptr<CCameraKeyFrame> pKeyFrame = CCameraKeyFrame::Create(vPos, vLook);
					if (m_CameraKeyFrames[i].first + 2 > (UINT)m_CameraKeyFrames[i].second.size())
					{
						m_CameraKeyFrames[i].second.emplace_back(pKeyFrame);
					}
					else
					{
						m_CameraKeyFrames[i].second.insert(m_CameraKeyFrames[i].second.begin() + (m_CameraKeyFrames[i].first + 1), pKeyFrame);
					}

					m_CameraKeyFrames[i].first = (UINT)m_CameraKeyFrames[i].second.size() - 1;
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					if (m_CameraKeyFrames[i].first < m_CameraKeyFrames[i].second.size())
					{
						m_CameraKeyFrames[i].second.erase(m_CameraKeyFrames[i].second.begin() + m_CameraKeyFrames[i].first);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Direction"))
				{
					CameraDirection();
				}
				ImGui::SameLine();
				if (ImGui::Button("Stop"))
				{
					m_pDirectionCamera->EndDirection();
				}
				UINT iNumKeyFrames = (UINT)m_CameraKeyFrames[i].second.size();
				vector<string> KeyFrames;
				KeyFrames.reserve(iNumKeyFrames);
				for (UINT j = 0; j < iNumKeyFrames; j++)
				{
					string Name = "KeyFrame_" + to_string(j);
					KeyFrames.push_back(Name);
				}
				if (ImGui::BeginListBox("KeyFrame"))
				{
					for (UINT j = 0; j < iNumKeyFrames; j++)
					{
						const bool item_selected = (j == m_CameraKeyFrames[i].first);
						if (ImGui::Selectable(KeyFrames[j].c_str(), item_selected))
						{
							m_CameraKeyFrames[i].first = j;
							m_pPickCamera = m_CameraKeyFrames[i].second[j];
						}
						if (item_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndListBox();
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}


	if (ImGui::TreeNode("Camera Shake"))
	{
		{
			ImGui::InputFloat("Duration1", &m_fShakeDuration);
			ImGui::SliderFloat("Duration2", &m_fShakeDuration, 0.0f, 10.0f);
			ImGui::Checkbox("Rot", &m_isRot); ImGui::SameLine();
			ImGui::Checkbox("Loc", &m_isLoc); ImGui::SameLine();
			ImGui::Checkbox("Fov", &m_isFov);
			ImGui::Separator();
			ImGui::SeparatorText("Rotation");
			ImGui::InputFloat3("Amp1(Degree)", m_fRotAmplitude);
			ImGui::SliderFloat3("Amp2(Degree)", m_fRotAmplitude, 0.0f, 90.0f);
			ImGui::InputFloat3("Fre1", m_fRotFrequency);
			ImGui::SliderFloat3("Fre2", m_fRotFrequency, 0.0f, 20.0f);
			ImGui::InputFloat2("InOut1", (float*)&m_vRotBlendInOut);
			ImGui::SliderFloat2("InOut2", (float*)&m_vRotBlendInOut, 0.0f, m_fShakeDuration);
			ImGui::InputFloat2("Weight1", (float*)&m_vRotBlendWeight);
			ImGui::SliderFloat2("Weight2", (float*)&m_vRotBlendWeight, 0.0f, 5.0f);
			ImGui::RadioButton("WAVE0", (int*)&m_eShakeFunc[0], SHAKE_WAVE); ImGui::SameLine();
			ImGui::RadioButton("LINEAR0", (int*)&m_eShakeFunc[0], SHAKE_LINEAR); ImGui::SameLine();
			ImGui::Checkbox("Rand0", &m_isRotRand);
			ImGui::Separator();

			ImGui::SeparatorText("Location");
			ImGui::InputFloat3("Amp3", m_fLocAmplitude);
			ImGui::SliderFloat3("Amp4", m_fLocAmplitude, 0.0f, 5.0f);
			ImGui::InputFloat3("Fre3", m_fLocFrequency);
			ImGui::SliderFloat3("Fre4", m_fLocFrequency, 0.0f, 20.0f);
			ImGui::InputFloat2("InOut3", (float*)&m_vLocBlendInOut);
			ImGui::SliderFloat2("InOut4", (float*)&m_vLocBlendInOut, 0.0f, m_fShakeDuration);
			ImGui::InputFloat2("Weight3", (float*)&m_vLocBlendWeight);
			ImGui::SliderFloat2("Weight4", (float*)&m_vLocBlendWeight, 0.0f, 5.0f);
			ImGui::RadioButton("WAVE1", (int*)&m_eShakeFunc[1], SHAKE_WAVE); ImGui::SameLine();
			ImGui::RadioButton("LINEAR1", (int*)&m_eShakeFunc[1], SHAKE_LINEAR); ImGui::SameLine();
			ImGui::Checkbox("Rand1", &m_isLocRand);
			ImGui::Separator();

			ImGui::SeparatorText("Fovy");
			ImGui::InputFloat("Amp5", &m_fFovyAmplitude);
			ImGui::SliderFloat("Amp6", &m_fFovyAmplitude, 0.0f, 90.0f);
			ImGui::InputFloat("Fre5", &m_fFovyFrequency);
			ImGui::SliderFloat("Fre6", &m_fFovyFrequency, 0.0f, 20.0f);
			ImGui::InputFloat2("InOut5", (float*)&m_vFovyBlendInOut);
			ImGui::SliderFloat2("InOut6", (float*)&m_vFovyBlendInOut, 0.0f, m_fShakeDuration);
			ImGui::InputFloat2("Weight5", (float*)&m_vFovyBlendWeight);
			ImGui::SliderFloat2("Weight6", (float*)&m_vFovyBlendWeight, 0.0f, 5.0f);
			ImGui::RadioButton("WAVE2", (int*)&m_eShakeFunc[2], SHAKE_WAVE); ImGui::SameLine();
			ImGui::RadioButton("LINEAR2", (int*)&m_eShakeFunc[2], SHAKE_LINEAR);
			ImGui::Separator();
		}

		if (GAMEINSTANCE->KeyDown(DIK_Q) || ImGui::Button("Shake"))
		{
			m_eShakeFlag = 0;
			if (m_isRot)
			{
				m_eShakeFlag |= SHAKE_ROT;
				m_RotDesc.vRotAmplitude = { XMConvertToRadians(m_fRotAmplitude[0]),XMConvertToRadians(m_fRotAmplitude[1]),XMConvertToRadians(m_fRotAmplitude[2]) };
				m_RotDesc.vRotFrequency = { m_fRotFrequency[0],m_fRotFrequency[1],m_fRotFrequency[2] };
				m_RotDesc.vBlendInOut = m_vRotBlendInOut;
				m_RotDesc.vInOutWeight = m_vRotBlendWeight;
				m_RotDesc.eShakeFunc = m_eShakeFunc[0];
				m_RotDesc.isOffSetRand = m_isRotRand;
			}
			if (m_isLoc)
			{
				m_eShakeFlag |= SHAKE_LOC;
				m_LocDesc.vLocAmplitude = { m_fLocAmplitude[0],m_fLocAmplitude[1],m_fLocAmplitude[2] };
				m_LocDesc.vLocFrequency = { m_fLocFrequency[0],m_fLocFrequency[1],m_fLocFrequency[2] };
				m_LocDesc.vBlendInOut = m_vLocBlendInOut;
				m_LocDesc.vInOutWeight = m_vLocBlendWeight;
				m_LocDesc.eShakeFunc = m_eShakeFunc[1];
				m_LocDesc.isOffSetRand = m_isLocRand;
			}
			if (m_isFov)
			{
				m_eShakeFlag |= SHAKE_FOV;
				m_FovDesc.fFovyAmplitude = XMConvertToRadians(m_fFovyAmplitude);
				m_FovDesc.fFovyFrequency = m_fFovyFrequency;
				m_FovDesc.vBlendInOut = m_vFovyBlendInOut;
				m_FovDesc.vInOutWeight = m_vFovyBlendWeight;
				m_FovDesc.eShakeFunc = m_eShakeFunc[2];
			}

			if (m_fShakeDuration > 0.0f && m_eShakeFlag && !m_pCurrentCamera->isCamShake())
			{
				m_pCurrentCamera->CamShake(m_eShakeFlag, m_fShakeDuration, &m_RotDesc, &m_LocDesc, &m_FovDesc);
			}
		}
		ImGui::SameLine();
		if (GAMEINSTANCE->KeyDown(DIK_E) || ImGui::Button("Stop"))
		{
			if (m_pCurrentCamera->isCamShake())
			{
				m_pCurrentCamera->CamShakeStop();
			}
		}

		ImGui::TreePop();
	}


	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

	ImGui::End();

	return S_OK;
}

HRESULT CLevelEditor::RenderLightWindow(float _fTimeDelta)
{
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(0, main_viewport->Size.y * 0.5f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, main_viewport->Size.y * 0.5f), ImGuiCond_Once);
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

	/* Begin */
	ImGui::Begin("Light", nullptr, window_flags);

	ImGui::Separator();

	ImGui::InputFloat3("Position", &m_vLightPosition.x);
	ImGui::InputFloat4("Diffuse", &m_vLightDiffuse.x);
	ImGui::InputFloat4("Ambient", &m_vLightAmbient.x);
	ImGui::InputFloat("Range", &m_fLightRange);
	ImGui::InputFloat("Culling Range", &m_fLightCullRange);

	ImGui::Checkbox("Using NoiseTex", &m_IsUsingNoise);

	ImGui::Separator();

	/*Create Light*/
	if (ImGui::Button("Create")) {


		LIGHT_DESC desc;
		desc.eType = LIGHT_DESC::TYPE_POINT;
		desc.vPosition = _float4(m_vLightPosition.x, m_vLightPosition.y, m_vLightPosition.z, 1.f);
		desc.vDiffuse = m_vLightDiffuse;
		desc.vAmbient = m_vLightAmbient;
		desc.vSpecular = m_vLightDiffuse;
		desc.fRange = m_fLightRange;

		desc.fCullDistance = m_fLightCullRange;
		desc.bUsingNoise = m_IsUsingNoise;

		GAMEINSTANCE->AddLight(desc);
		m_LightDescs.push_back(desc);

		m_iCurrentLightIdx = m_LightDescs.size() - 1;

	}

	ImGui::Separator();

	std::vector<std::string> LightVector;

	for (auto& iter : m_LightDescs) {
	
		std::string PosX = to_string(iter.vPosition.x);
		std::string PosY = to_string(iter.vPosition.y);
		std::string PosZ = to_string(iter.vPosition.z);

		PosX = PosX.substr(0, PosX.size() - 3);
		PosY =PosY.substr(0, PosY.size() - 3);
		PosZ = PosZ.substr(0, PosZ.size() - 3);

		std::string strPosInfo = PosX + " , " + PosY + " , " + PosZ;
		LightVector.push_back(strPosInfo);
	}


	if (ImGui::BeginListBox("Light List"))
	{
		for (UINT i = 0; i < LightVector.size(); i++)
		{
			const bool item_selected = (i == m_iCurrentLightIdx);
			if (ImGui::Selectable(LightVector[i].c_str(), item_selected))
			{
				m_iCurrentLightIdx = i;

				m_IsUsingNoise = m_LightDescs[m_iCurrentLightIdx].bUsingNoise;
				m_fLightCullRange = m_LightDescs[m_iCurrentLightIdx].fCullDistance;
				m_fLightRange = m_LightDescs[m_iCurrentLightIdx].fRange;
				m_vLightAmbient = m_LightDescs[m_iCurrentLightIdx].vAmbient;
				m_vLightDiffuse = m_LightDescs[m_iCurrentLightIdx].vDiffuse;
				m_vLightPosition = _float3(m_LightDescs[m_iCurrentLightIdx].vPosition.x, m_LightDescs[m_iCurrentLightIdx].vPosition.y, m_LightDescs[m_iCurrentLightIdx].vPosition.z);

			}
			if (item_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}


	if (ImGui::Button("Edit")) {

		LIGHT_DESC* pDesc =  GAMEINSTANCE->GetLightDesc(m_iCurrentLightIdx + 1);

		pDesc->vPosition = _float4(m_vLightPosition.x, m_vLightPosition.y, m_vLightPosition.z, 1.f);
		pDesc->vDiffuse = m_vLightDiffuse;
		pDesc->vAmbient = m_vLightAmbient;
		pDesc->vSpecular = m_vLightDiffuse;
		pDesc->fRange = m_fLightRange;
		pDesc->bUsingNoise = m_IsUsingNoise;
		pDesc->fCullDistance = m_fLightCullRange;

		m_LightDescs[m_iCurrentLightIdx] = *pDesc;

	}
	
	ImGui::SameLine();

	if (ImGui::Button("Delete")) {

		auto iter = m_LightDescs.begin();
		for (_int i = 0; i < m_iCurrentLightIdx; ++i) {
			++iter;
		}
		m_LightDescs.erase(iter);
		GAMEINSTANCE->DeleteLight(m_iCurrentLightIdx + 1);
	}
	
	//SaveLightData

	ImGui::Separator();

	if (ImGui::Button("Save")) {
		if(!m_LightDescs.empty())
			SaveLightData();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load")) {
		LoadLightData();
	}

	ImGui::End();

	return S_OK;
}

HRESULT CLevelEditor::RenderAnimation(float _fTimeDelta)
{

	return S_OK;
}

bool CLevelEditor::ColliderPicking(weak_ptr<CColliderFrame>* _ColOut, _float3* _vColPosOut, int& _pIndex)
{
	if (m_Colliders[m_iSelectColVector].empty())
		return false;

	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	XMVECTOR vNDCPosition = XMVectorSet((2.0f * (float)ptMouse.x) / (float)g_iWinSizeX - 1.0f, (-2.0f * (float)ptMouse.y) / (float)g_iWinSizeY + 1.0f, 0.0f, 1.0f);

	XMMATRIX matInverse_Proj = pGameInstance->GetTransformMatrixInverse(CPipeLine::D3DTS_PROJ);
	XMMATRIX matInverse_View = pGameInstance->GetTransformMatrixInverse(CPipeLine::D3DTS_VIEW);

	XMVECTOR Origin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR Dir = XMVector3TransformCoord(vNDCPosition, matInverse_Proj);

	Origin = XMVector3TransformCoord(Origin, matInverse_View);
	Dir = XMVector3TransformNormal(Dir, matInverse_View);
	
	XMVECTOR vPickPos;
	float fNear = 1000.f;
	float dist = 0.0f;
	shared_ptr<CColliderFrame> pPickedCollider = nullptr;
	bool Picked{ false };

	_float3 vVerticePos[8] = { {-0.5f,0.5f,-0.5f},{0.5f, 0.5f, -0.5f},{0.5f, -0.5f, -0.5f},{-0.5f, -0.5f, -0.5f},
		{-0.5f, 0.5f, 0.5f},{0.5f, 0.5f, 0.5f},{0.5f, -0.5f, 0.5f},{-0.5f, -0.5f, 0.5f} };
	UINT pIndices[36]{};
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;
	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;
	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;
	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;
	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;
	int index = 0;
	for (vector<shared_ptr<CColliderFrame>>::iterator iter = m_Colliders[m_iSelectColVector].begin(); iter != m_Colliders[m_iSelectColVector].end(); iter++, index++)
	{
		if (nullptr == (*iter))
			continue;

		_float4x4 matInverse_World = XMMatrixInverse(nullptr, (*iter)->GetWorldMat());
		XMVECTOR vLocalOrigin = XMVector3TransformCoord(Origin, matInverse_World);
		XMVECTOR vLocalDir = XMVector3TransformNormal(Dir, matInverse_World);
		vLocalDir = XMVector3Normalize(vLocalDir);
		UINT iIndicesNum = 0;

		for (UINT i = 0; i < 12; i++)
		{
			if (DirectX::TriangleTests::Intersects(vLocalOrigin, vLocalDir, vVerticePos[pIndices[iIndicesNum++]], vVerticePos[pIndices[iIndicesNum++]], vVerticePos[pIndices[iIndicesNum++]], dist))
			{
				if (fNear > dist)
				{
					Picked = true;
					vPickPos = vLocalOrigin + vLocalDir * dist;
					fNear = dist;
					pPickedCollider = (*iter);
					_pIndex = index;
				}
			}
		}
	}

	if (Picked)
	{
		if (nullptr != _vColPosOut)
		{
			if (nullptr != pPickedCollider)
			{
				*_vColPosOut = XMVector4Transform(vPickPos, pPickedCollider->GetWorldMat());
			}
		}
		(*_ColOut) = pPickedCollider;
	}

	return Picked;
}

bool CLevelEditor::CameraPicking(weak_ptr<CCameraKeyFrame>* _CameraOut, _float3* _vCameraPosOut, UINT& _pIndex)
{
	if (m_CameraKeyFrames[iSelectPath].second.empty())
		return false;

	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	XMVECTOR vNDCPosition = XMVectorSet((2.0f * (float)ptMouse.x) / (float)g_iWinSizeX - 1.0f, (-2.0f * (float)ptMouse.y) / (float)g_iWinSizeY + 1.0f, 0.0f, 1.0f);

	XMMATRIX matInverse_Proj = pGameInstance->GetTransformMatrixInverse(CPipeLine::D3DTS_PROJ);
	XMMATRIX matInverse_View = pGameInstance->GetTransformMatrixInverse(CPipeLine::D3DTS_VIEW);

	XMVECTOR Origin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR Dir = XMVector4Transform(vNDCPosition, matInverse_Proj);

	Origin = XMVector4Transform(Origin, matInverse_View);
	Dir = XMVector3TransformNormal(Dir, matInverse_View);

	XMVECTOR vPickPos;
	float fNear = 1000.f;
	float dist = 0.0f;
	shared_ptr<CCameraKeyFrame> pPickedKeyFrame = nullptr;
	bool Picked{ false };

	_float3 vVerticePos[8] = { {-0.5f,0.5f,-0.5f},{0.5f, 0.5f, -0.5f},{0.5f, -0.5f, -0.5f},{-0.5f, -0.5f, -0.5f},
		{-0.5f, 0.5f, 0.5f},{0.5f, 0.5f, 0.5f},{0.5f, -0.5f, 0.5f},{-0.5f, -0.5f, 0.5f} };
	UINT pIndices[36]{};
	{
		pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
		pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;
		pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
		pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;
		pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
		pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;
		pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
		pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;
		pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
		pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;
		pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
		pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;
	}
	UINT index = 0;
	for (vector<shared_ptr<CCameraKeyFrame>>::iterator iter = m_CameraKeyFrames[iSelectPath].second.begin(); iter != m_CameraKeyFrames[iSelectPath].second.end(); iter++, index++)
	{
		if (nullptr == (*iter))
			continue;

		_float4x4 matInverse_World = (*iter)->GetTransform()->GetWorldMatrixInverse();
		XMVECTOR vLocalOrigin = XMVector4Transform(Origin, matInverse_World);
		XMVECTOR vLocalDir = XMVector3TransformNormal(Dir, matInverse_World);
		vLocalDir = XMVector3Normalize(vLocalDir);
		UINT iIndicesNum = 0;

		for (UINT i = 0; i < 12; i++)
		{
			if (DirectX::TriangleTests::Intersects(vLocalOrigin, vLocalDir, vVerticePos[pIndices[iIndicesNum++]], vVerticePos[pIndices[iIndicesNum++]], vVerticePos[pIndices[iIndicesNum++]], dist))
			{
				if (fNear > dist)
				{
					Picked = true;
					vPickPos = vLocalOrigin + vLocalDir * dist;
					fNear = dist;
					pPickedKeyFrame = (*iter);
					_pIndex = index;
				}
			}
		}
	}

	if (Picked)
	{
		if (nullptr != _vCameraPosOut)
		{
			if (nullptr != pPickedKeyFrame)
			{
				*_vCameraPosOut = XMVector4Transform(vPickPos, pPickedKeyFrame->GetTransform()->GetWorldMatrix());
			}
		}
		(*_CameraOut) = pPickedKeyFrame;
	}

	return Picked;
}

HRESULT CLevelEditor::SaveColliderData()                                                             
{
	if (m_Colliders[m_iSelectColVector].empty())
	{
		MessageBox(nullptr, L"Vector Empty", L"system", MB_OK);
		return E_FAIL;
	}

	string strFilePath = ColliderSaveLoadPath + m_strFileNameCol + ".dat";

	// 파일 경로
	filesystem::path SaveFilePath = strFilePath.c_str();

	ofstream outFile(SaveFilePath, ios::out | ios::binary);

	if (outFile.is_open())
	{
		int iColliderNum = (int)m_Colliders[m_iSelectColVector].size();
		outFile.write(reinterpret_cast<const char*>(&iColliderNum), sizeof(int));

		for (int i = 0; i < iColliderNum; i++)
		{
			// World Position
			_float3 vPos = m_Colliders[m_iSelectColVector][i]->GetTransform()->GetState(CTransform::STATE_POSITION);
			outFile.write(reinterpret_cast<const char*>(&vPos), sizeof(_float3));

			shared_ptr<Geometry> Geo = m_Colliders[m_iSelectColVector][i]->GetGeometry();

			// EGEOMETRY_TYPE
			EGEOMETRY_TYPE GeometryType = Geo->GetType();
			outFile.write(reinterpret_cast<const char*>(&GeometryType), sizeof(EGEOMETRY_TYPE));

			// OffSetRotation
			_float3 vOffSetRot = Geo->vOffSetRotation;
			outFile.write(reinterpret_cast<const char*>(&vOffSetRot), sizeof(_float3));

			// Geometry Data
			switch (GeometryType)
			{
			case GT_SPHERE:
			{
				shared_ptr<GeometrySphere> GeoSph = static_pointer_cast<GeometrySphere>(Geo);
				float fRadius = GeoSph->fRadius;
				outFile.write(reinterpret_cast<const char*>(&fRadius), sizeof(float));
			}
			break;
			case GT_BOX:
			{
				shared_ptr<GeometryBox> GeoBox = static_pointer_cast<GeometryBox>(Geo);
				_float3 vSize = GeoBox->vSize;
				outFile.write(reinterpret_cast<const char*>(&vSize), sizeof(_float3));
			}
			break;
			case GT_CAPSULE:
			{
				shared_ptr<GeometryCapsule> GeoCap = static_pointer_cast<GeometryCapsule>(Geo);
				float fRadius = GeoCap->fRadius;
				float fHeight = GeoCap->fHeight;
				outFile.write(reinterpret_cast<const char*>(&fRadius), sizeof(float));
				outFile.write(reinterpret_cast<const char*>(&fHeight), sizeof(float));
			}
			break;
			}
		}

		outFile.close();
	}
	else
	{
		MessageBox(nullptr, L"File Open Failed", L"system", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevelEditor::LoadColliderData()
{
	if (!m_Colliders[m_iSelectColVector].empty())
	{
		MessageBox(nullptr, L"Vector Not Empty\nPlease Load into Empty Vector", L"system", MB_OK);
		return E_FAIL;
	}

	string strFilePath = ColliderSaveLoadPath + m_strFileNameCol + ".dat";

	// 파일 경로
	filesystem::path LoadFilePath = strFilePath.c_str();

	ifstream inFile(LoadFilePath, ios::in | ios::binary);

	if (inFile.is_open())
	{
		int iColliderNum = 0;
		inFile.read(reinterpret_cast<char*>(&iColliderNum), sizeof(int));
		
		m_Colliders[m_iSelectColVector].reserve(iColliderNum);

		for (int i = 0; i < iColliderNum; i++)
		{
			// World Position
			_float3 vPos;
			inFile.read(reinterpret_cast<char*>(&vPos), sizeof(_float3));

			// EGEOMETRY_TYPE
			EGEOMETRY_TYPE GeometryType{};
			inFile.read(reinterpret_cast<char*>(&GeometryType), sizeof(EGEOMETRY_TYPE));

			// OffSetRotation
			_float3 vOffSetRot;
			inFile.read(reinterpret_cast<char*>(&vOffSetRot), sizeof(_float3));

			shared_ptr<Geometry> pGeomertry = nullptr;

			// Geometry Data
			switch (GeometryType)
			{
			case GT_SPHERE:
			{
				float fRadius = 0.0f;
				inFile.read(reinterpret_cast<char*>(&fRadius), sizeof(float));
				shared_ptr<GeometrySphere> GeoSph = make_shared<GeometrySphere>();
				GeoSph->fRadius = fRadius;
				pGeomertry = GeoSph;
			}
			break;
			case GT_BOX:
			{
				_float3 vSize;
				inFile.read(reinterpret_cast<char*>(&vSize), sizeof(_float3));
				shared_ptr<GeometryBox> GeoBox = make_shared<GeometryBox>();
				GeoBox->vSize = vSize;
				pGeomertry = GeoBox;
			}
			break;
			case GT_CAPSULE:
			{
				float fRadius = 0.0f;
				float fHeight = 0.0f;
				inFile.read(reinterpret_cast<char*>(&fRadius), sizeof(float));
				inFile.read(reinterpret_cast<char*>(&fHeight), sizeof(float));
				shared_ptr<GeometryCapsule> GeoCap = make_shared<GeometryCapsule>();
				GeoCap->fRadius = fRadius;
				GeoCap->fHeight = fHeight;
				pGeomertry = GeoCap;
			}
			break;
			}

			pGeomertry->vOffSetRotation = vOffSetRot;

			shared_ptr<CColliderFrame> pColliderFrame = CColliderFrame::Create(pGeomertry, vPos);
			m_Colliders[m_iSelectColVector].push_back(pColliderFrame);
		}

		inFile.close();
	}
	else
	{
		MessageBox(nullptr, L"File Open Failed", L"System", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevelEditor::CombineColliderData(int _iA, int _iB)
{
	if (_iA == _iB)
	{
		MessageBox(nullptr, L"A and B are Same Index", L"system", MB_OK);
		return E_FAIL;
	}
	else if (m_iSelectColVector == _iA || m_iSelectColVector == _iB)
	{
		MessageBox(nullptr, L"Don't choose the same number like Index", L"system", MB_OK);
		return E_FAIL;
	}
	else if (m_Colliders[_iA].empty() && m_Colliders[_iB].empty())
	{
		MessageBox(nullptr, L"Container A, B are empty", L"system", MB_OK);
		return E_FAIL;
	}
	else if (!m_Colliders[m_iSelectColVector].empty())
	{
		MessageBox(nullptr, L"Not Empty Container", L"system", MB_OK);
		return E_FAIL;
	}

	UINT ColliderNumA = (UINT)m_Colliders[_iA].size();
	UINT ColliderNumB = (UINT)m_Colliders[_iB].size();
	m_Colliders[m_iSelectColVector].reserve(ColliderNumA + ColliderNumB);

	for (UINT i = 0; i < ColliderNumA; i++)
	{
		m_Colliders[m_iSelectColVector].push_back(m_Colliders[_iA][i]);
	}
	for (UINT i = 0; i < ColliderNumB; i++)
	{
		m_Colliders[m_iSelectColVector].push_back(m_Colliders[_iB][i]);
	}

	return S_OK;
}

HRESULT CLevelEditor::SaveCameraData()
{
	if (m_CameraKeyFrames[iSelectPath].second.empty())
	{
		MessageBox(nullptr, L"Vector Empty", L"system", MB_OK);
		return E_FAIL;
	}

	string strFilePath = CameraSaveLoadPath + m_strFileNameCam + ".json";

	int iNumKeyFrames = (int)m_CameraKeyFrames[iSelectPath].second.size();
	shared_ptr<CTransform> pEyeTransform = nullptr;
	shared_ptr<CTransform> pAtTransform = nullptr;
	
	UINT iFadeFlag{ 0 };
	float fFadeStartTime{ 0.0f };
	float fFadeDuration{ 0.0f };

	UINT iShakeFlags{ 0 };
	float fStartTime{ 0.0f };
	float fDuration{ 0.0f };
	ROT_DESC RotDesc{};
	LOC_DESC LocDesc{};
	FOV_DESC FovDesc{};

	Json::Value KeyFrame;
	for (int i = 0; i < iNumKeyFrames; i++)
	{
		Json::Value Channel;

		pEyeTransform = m_CameraKeyFrames[iSelectPath].second[i]->GetTransform();
		if (!pEyeTransform)
			return E_FAIL;
		pAtTransform = m_CameraKeyFrames[iSelectPath].second[i]->GetTransformAt();
		if (!pAtTransform)
			return E_FAIL;

		_float3 vPos = pEyeTransform->GetState(CTransform::STATE_POSITION);
		_float3 vAt = pAtTransform->GetState(CTransform::STATE_POSITION);

		Channel["Eye"].append(vPos.x);
		Channel["Eye"].append(vPos.y);
		Channel["Eye"].append(vPos.z);

		Channel["At"].append(vAt.x);
		Channel["At"].append(vAt.y);
		Channel["At"].append(vAt.z);

		Channel["LerpTime"] = m_CameraKeyFrames[iSelectPath].second[i]->GetLerpTime();
		
		if (m_CameraKeyFrames[iSelectPath].second[i]->GetFadeInfo(&iFadeFlag, &fFadeStartTime, &fFadeDuration))
		{
			Channel["FadeInfo"]["FadeFlag"] = iFadeFlag;
			Channel["FadeInfo"]["FadeTime"] = fFadeStartTime;
			Channel["FadeInfo"]["FadeDuration"] = fFadeDuration;
		}

		if (m_CameraKeyFrames[iSelectPath].second[i]->GetShakeInfo(&iShakeFlags, &fStartTime, &fDuration, &RotDesc, &LocDesc, &FovDesc))
		{
			Channel["ShakeInfo"]["ShakeFlags"] = iShakeFlags;
			Channel["ShakeInfo"]["StartTime"] = fStartTime;
			Channel["ShakeInfo"]["Duration"] = fDuration;

			if (iShakeFlags & SHAKE_ROT)
			{
				Channel["ShakeInfo"]["RotDesc"]["Amplitude"].append(RotDesc.vRotAmplitude.x);
				Channel["ShakeInfo"]["RotDesc"]["Amplitude"].append(RotDesc.vRotAmplitude.y);
				Channel["ShakeInfo"]["RotDesc"]["Amplitude"].append(RotDesc.vRotAmplitude.z);

				Channel["ShakeInfo"]["RotDesc"]["Frequency"].append(RotDesc.vRotFrequency.x);
				Channel["ShakeInfo"]["RotDesc"]["Frequency"].append(RotDesc.vRotFrequency.y);
				Channel["ShakeInfo"]["RotDesc"]["Frequency"].append(RotDesc.vRotFrequency.z);

				Channel["ShakeInfo"]["RotDesc"]["BlendInOut"].append(RotDesc.vBlendInOut.x);
				Channel["ShakeInfo"]["RotDesc"]["BlendInOut"].append(RotDesc.vBlendInOut.y);

				Channel["ShakeInfo"]["RotDesc"]["InOutWeight"].append(RotDesc.vInOutWeight.x);
				Channel["ShakeInfo"]["RotDesc"]["InOutWeight"].append(RotDesc.vInOutWeight.y);

				Channel["ShakeInfo"]["RotDesc"]["ShakeFunc"] = (UINT)RotDesc.eShakeFunc;

				Channel["ShakeInfo"]["RotDesc"]["isOffSetRand"] = RotDesc.isOffSetRand;
			}
			if (iShakeFlags & SHAKE_LOC)
			{
				Channel["ShakeInfo"]["LocDesc"]["Amplitude"].append(LocDesc.vLocAmplitude.x);
				Channel["ShakeInfo"]["LocDesc"]["Amplitude"].append(LocDesc.vLocAmplitude.y);
				Channel["ShakeInfo"]["LocDesc"]["Amplitude"].append(LocDesc.vLocAmplitude.z);

				Channel["ShakeInfo"]["LocDesc"]["Frequency"].append(LocDesc.vLocFrequency.x);
				Channel["ShakeInfo"]["LocDesc"]["Frequency"].append(LocDesc.vLocFrequency.y);
				Channel["ShakeInfo"]["LocDesc"]["Frequency"].append(LocDesc.vLocFrequency.z);

				Channel["ShakeInfo"]["LocDesc"]["BlendInOut"].append(LocDesc.vBlendInOut.x);
				Channel["ShakeInfo"]["LocDesc"]["BlendInOut"].append(LocDesc.vBlendInOut.y);

				Channel["ShakeInfo"]["LocDesc"]["InOutWeight"].append(LocDesc.vInOutWeight.x);
				Channel["ShakeInfo"]["LocDesc"]["InOutWeight"].append(LocDesc.vInOutWeight.y);

				Channel["ShakeInfo"]["LocDesc"]["ShakeFunc"] = (UINT)LocDesc.eShakeFunc;

				Channel["ShakeInfo"]["LocDesc"]["isOffSetRand"] = LocDesc.isOffSetRand;
			}
			if (iShakeFlags & SHAKE_FOV)
			{
				Channel["ShakeInfo"]["FovDesc"]["Amplitude"] = FovDesc.fFovyAmplitude;

				Channel["ShakeInfo"]["FovDesc"]["Frequency"] = FovDesc.fFovyFrequency;

				Channel["ShakeInfo"]["FovDesc"]["BlendInOut"].append(FovDesc.vBlendInOut.x);
				Channel["ShakeInfo"]["FovDesc"]["BlendInOut"].append(FovDesc.vBlendInOut.y);

				Channel["ShakeInfo"]["FovDesc"]["InOutWeight"].append(FovDesc.vInOutWeight.x);
				Channel["ShakeInfo"]["FovDesc"]["InOutWeight"].append(FovDesc.vInOutWeight.y);

				Channel["ShakeInfo"]["FovDesc"]["ShakeFunc"] = (UINT)FovDesc.eShakeFunc;
			}
		}

		KeyFrame[to_string(i)] = Channel;
	}

	GAMEINSTANCE->WriteJson(KeyFrame, wstring().assign(strFilePath.begin(), strFilePath.end()));
	
	return S_OK;
}

HRESULT CLevelEditor::LoadCameraData()
{
	if (!m_CameraKeyFrames.empty() && iSelectPath < (int)m_CameraKeyFrames.size() && !m_CameraKeyFrames[iSelectPath].second.empty())
	{
		MessageBox(nullptr, L"Vector Not Empty\nPlease Load into Empty Vector", L"system", MB_OK);
		return E_FAIL;
	}

	string strFilePath = CameraSaveLoadPath + m_strFileNameCam + ".json";

	int iNumKeyFrames{ 0 };
	shared_ptr<CTransform> pEyeTransform{ nullptr };
	shared_ptr<CTransform> pAtTransform{ nullptr };

	Json::Value KeyFrames = GAMEINSTANCE->ReadJson(wstring().assign(strFilePath.begin(), strFilePath.end()));
	UINT iSize = (UINT)KeyFrames.size();
	m_CameraKeyFrames[iSelectPath].second.reserve(iSize);
	_float3 vEye;
	_float3 vAt;
	float fLerpTime{ 0.0f };

	UINT iFadeFlag{ 0 };
	float fFadeStartTime{ 0.0f };
	float fFadeDuration{ 0.0f };

	UINT iShakeFlags{ 0 };
	float fStartTime{ 0.0f };
	float fDuration{ 0.0f };
	ROT_DESC RotDesc{};
	LOC_DESC LocDesc{};
	FOV_DESC FovDesc{};

	for (UINT i = 0; i < iSize; i++)
	{
		string strNum = to_string(i);

		vEye.x = KeyFrames[strNum]["Eye"][0].asFloat();
		vEye.y = KeyFrames[strNum]["Eye"][1].asFloat();
		vEye.z = KeyFrames[strNum]["Eye"][2].asFloat();
		vAt.x = KeyFrames[strNum]["At"][0].asFloat();
		vAt.y = KeyFrames[strNum]["At"][1].asFloat();
		vAt.z = KeyFrames[strNum]["At"][2].asFloat();
		fLerpTime = KeyFrames[strNum]["LerpTime"].asFloat();

		shared_ptr<CCameraKeyFrame> pLoadKeyFrame = CCameraKeyFrame::Create(vEye, { 0.0f,0.0f,-1.0f });
		pLoadKeyFrame->GetTransformAt()->SetState(CTransform::STATE_POSITION, vAt);
		pLoadKeyFrame->SetLerpTime(fLerpTime);

		if (KeyFrames[strNum].isMember("FadeInfo"))
		{
			iFadeFlag = KeyFrames[strNum]["FadeInfo"]["FadeFlag"].asUInt();
			fFadeStartTime = KeyFrames[strNum]["FadeInfo"]["FadeTime"].asFloat();
			fFadeDuration = KeyFrames[strNum]["FadeInfo"]["FadeDuration"].asFloat();
			
			pLoadKeyFrame->SetFade(iFadeFlag);
			pLoadKeyFrame->SetFadeInOutStartTime(fFadeStartTime);
			pLoadKeyFrame->SetFadeInOutDuration(fFadeDuration);
		}

		if (KeyFrames[strNum].isMember("ShakeInfo"))
		{
			iShakeFlags = KeyFrames[strNum]["ShakeInfo"]["ShakeFlags"].asUInt();
			fStartTime = KeyFrames[strNum]["ShakeInfo"]["StartTime"].asFloat();
			fDuration = KeyFrames[strNum]["ShakeInfo"]["Duration"].asFloat();

			if (iShakeFlags & SHAKE_ROT)
			{
				RotDesc.vRotAmplitude.x = KeyFrames[strNum]["ShakeInfo"]["RotDesc"]["Amplitude"][0].asFloat();
				RotDesc.vRotAmplitude.y = KeyFrames[strNum]["ShakeInfo"]["RotDesc"]["Amplitude"][1].asFloat();
				RotDesc.vRotAmplitude.z = KeyFrames[strNum]["ShakeInfo"]["RotDesc"]["Amplitude"][2].asFloat();

				RotDesc.vRotFrequency.x = KeyFrames[strNum]["ShakeInfo"]["RotDesc"]["Frequency"][0].asFloat();
				RotDesc.vRotFrequency.y = KeyFrames[strNum]["ShakeInfo"]["RotDesc"]["Frequency"][1].asFloat();
				RotDesc.vRotFrequency.z = KeyFrames[strNum]["ShakeInfo"]["RotDesc"]["Frequency"][2].asFloat();

				RotDesc.vBlendInOut.x = KeyFrames[strNum]["ShakeInfo"]["RotDesc"]["BlendInOut"][0].asFloat();
				RotDesc.vBlendInOut.y = KeyFrames[strNum]["ShakeInfo"]["RotDesc"]["BlendInOut"][1].asFloat();

				RotDesc.vInOutWeight.x = KeyFrames[strNum]["ShakeInfo"]["RotDesc"]["InOutWeight"][0].asFloat();
				RotDesc.vInOutWeight.y = KeyFrames[strNum]["ShakeInfo"]["RotDesc"]["InOutWeight"][1].asFloat();

				RotDesc.eShakeFunc = (ESHAKEFUNC)KeyFrames[strNum]["ShakeInfo"]["RotDesc"]["ShakeFunc"].asUInt();

				RotDesc.isOffSetRand = KeyFrames[strNum]["ShakeInfo"]["RotDesc"]["isOffSetRand"].asBool();
			}
			if (iShakeFlags & SHAKE_LOC)
			{
				LocDesc.vLocAmplitude.x = KeyFrames[strNum]["ShakeInfo"]["LocDesc"]["Amplitude"][0].asFloat();
				LocDesc.vLocAmplitude.y = KeyFrames[strNum]["ShakeInfo"]["LocDesc"]["Amplitude"][1].asFloat();
				LocDesc.vLocAmplitude.z = KeyFrames[strNum]["ShakeInfo"]["LocDesc"]["Amplitude"][2].asFloat();

				LocDesc.vLocFrequency.x = KeyFrames[strNum]["ShakeInfo"]["LocDesc"]["Frequency"][0].asFloat();
				LocDesc.vLocFrequency.y = KeyFrames[strNum]["ShakeInfo"]["LocDesc"]["Frequency"][1].asFloat();
				LocDesc.vLocFrequency.z = KeyFrames[strNum]["ShakeInfo"]["LocDesc"]["Frequency"][2].asFloat();

				LocDesc.vBlendInOut.x = KeyFrames[strNum]["ShakeInfo"]["LocDesc"]["BlendInOut"][0].asFloat();
				LocDesc.vBlendInOut.y = KeyFrames[strNum]["ShakeInfo"]["LocDesc"]["BlendInOut"][1].asFloat();

				LocDesc.vInOutWeight.x = KeyFrames[strNum]["ShakeInfo"]["LocDesc"]["InOutWeight"][0].asFloat();
				LocDesc.vInOutWeight.y = KeyFrames[strNum]["ShakeInfo"]["LocDesc"]["InOutWeight"][1].asFloat();

				LocDesc.eShakeFunc = (ESHAKEFUNC)KeyFrames[strNum]["ShakeInfo"]["LocDesc"]["ShakeFunc"].asUInt();

				LocDesc.isOffSetRand = KeyFrames[strNum]["ShakeInfo"]["LocDesc"]["isOffSetRand"].asBool();
			}
			if (iShakeFlags & SHAKE_FOV)
			{
				FovDesc.fFovyAmplitude = KeyFrames[strNum]["ShakeInfo"]["FovDesc"]["Amplitude"].asFloat();

				FovDesc.fFovyFrequency = KeyFrames[strNum]["ShakeInfo"]["FovDesc"]["Frequency"].asFloat();

				FovDesc.vBlendInOut.x = KeyFrames[strNum]["ShakeInfo"]["FovDesc"]["BlendInOut"][0].asFloat();
				FovDesc.vBlendInOut.y = KeyFrames[strNum]["ShakeInfo"]["FovDesc"]["BlendInOut"][1].asFloat();

				FovDesc.vInOutWeight.x = KeyFrames[strNum]["ShakeInfo"]["FovDesc"]["InOutWeight"][0].asFloat();
				FovDesc.vInOutWeight.y = KeyFrames[strNum]["ShakeInfo"]["FovDesc"]["InOutWeight"][1].asFloat();

				FovDesc.eShakeFunc = (ESHAKEFUNC)KeyFrames[strNum]["ShakeInfo"]["FovDesc"]["ShakeFunc"].asUInt();
			}

			pLoadKeyFrame->SetCameraShake(fStartTime, iShakeFlags, fDuration, &RotDesc, &LocDesc, &FovDesc);
		}

		m_CameraKeyFrames[iSelectPath].second.emplace_back(pLoadKeyFrame);
	}

	return S_OK;
}

HRESULT CLevelEditor::CameraDirection()
{
	if (!m_CameraKeyFrames.empty() && iSelectPath < (int)m_CameraKeyFrames.size() && !m_CameraKeyFrames[iSelectPath].second.empty())
	{
		m_pFreeCamera->CameraTurnOff();
		m_pDirectionCamera->CameraTurnOn();
		m_pDirectionCamera->StartDirection(m_CameraKeyFrames[iSelectPath].second);
	}

	return S_OK;
}

HRESULT CLevelEditor::SaveLightData()
{
	Json::Value root;

	for (_int i = 0; i < m_LightDescs.size(); ++i) {

		Json::Value info;

		/*Light Position*/
		info["Position"].append(m_LightDescs[i].vPosition.x);
		info["Position"].append(m_LightDescs[i].vPosition.y);
		info["Position"].append(m_LightDescs[i].vPosition.z);

		info["Diffuse"].append(m_LightDescs[i].vDiffuse.x);
		info["Diffuse"].append(m_LightDescs[i].vDiffuse.y);
		info["Diffuse"].append(m_LightDescs[i].vDiffuse.z);
		info["Diffuse"].append(m_LightDescs[i].vDiffuse.w);

		info["Ambient"].append(m_LightDescs[i].vDiffuse.x);
		info["Ambient"].append(m_LightDescs[i].vDiffuse.y);
		info["Ambient"].append(m_LightDescs[i].vDiffuse.z);
		info["Ambient"].append(m_LightDescs[i].vDiffuse.w);

		info["Range"] = m_LightDescs[i].fRange;
		info["CullDistance"] = m_LightDescs[i].fCullDistance;
		info["UsingNoise"] = m_LightDescs[i].bUsingNoise;


		string strNum = to_string(i);
		root[strNum] = info;
	}
	
	wstring Path = m_strLightSavePath + TEXT("Light_Dungeon.json");
	GAMEINSTANCE->WriteJson(root, Path);


	return S_OK;
}

HRESULT CLevelEditor::LoadLightData()
{
	Json::Value root;

	wstring Path = m_strLightSavePath + TEXT("Light_Dungeon.json");
	root = GAMEINSTANCE->ReadJson(Path);

	Json::ValueIterator iter;
	_int iCount = 0;

	for (auto iter = root.begin(); iter != root.end(); ++iter) {

		LIGHT_DESC desc;

		string strIndex = to_string(iCount);
		Json::Value CountValue = root[strIndex];


		_float4 vPosition = _float4(CountValue["Position"][0].asFloat(),
			CountValue["Position"][1].asFloat(),
			CountValue["Position"][2].asFloat(),
			1.f);

		_float4 vAmbient = _float4(CountValue["Ambient"][0].asFloat(),
			CountValue["Ambient"][1].asFloat(),
			CountValue["Ambient"][2].asFloat(),
			CountValue["Ambient"][3].asFloat());

		_float4 vDiffuse = _float4(CountValue["Diffuse"][0].asFloat(),
			CountValue["Diffuse"][1].asFloat(),
			CountValue["Diffuse"][2].asFloat(),
			CountValue["Diffuse"][3].asFloat());

		_float fRange = CountValue["Range"].asFloat();

		_bool UsingNoise = CountValue["UsingNoise"].asBool();

		_float fCullDist = CountValue["CullDistance"].asFloat();


		desc.eType = LIGHT_DESC::TYPE_POINT;
		desc.fRange = fRange;
		desc.vAmbient = vAmbient;
		desc.vDiffuse = vDiffuse;
		desc.vPosition = vPosition;
		desc.vSpecular = vDiffuse;
		desc.bUsingNoise = UsingNoise;
		desc.fCullDistance = fCullDist;

		if (FAILED(GAMEINSTANCE->AddLight(desc)))
			return E_FAIL;

		m_LightDescs.push_back(desc);

		++iCount;
	}

	GAMEINSTANCE->WriteJson(root, Path);

	return S_OK;
}
