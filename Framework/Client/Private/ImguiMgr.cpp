#include "ImguiMgr.h"
#include "GameInstance.h"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_impl_dx11.h"

#include "CameraMgr.h"
#include "GameMgr.h"


CImguiMgr::CImguiMgr()
{
}

CImguiMgr::~CImguiMgr()
{
    ShutDown();
}

HRESULT CImguiMgr::Initialize()
{

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui_ImplWin32_Init(g_hWnd);

    ImGui_ImplDX11_Init(GAMEINSTANCE->GetDeviceInfo().Get(),
        GAMEINSTANCE->GetDeviceContextInfo().Get());

    return S_OK;
}

void CImguiMgr::Tick(_float _fTimeDelta)
{
    m_fDeltaTime = _fTimeDelta;
    KeyInput();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

HRESULT CImguiMgr::Render()
{
//#ifdef _DEBUG
    RenderMainWindow();
    ShaderOptionWindow();
    CamShakeWindow();
//#endif

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return S_OK;
}

//#ifdef _DEBUG
void CImguiMgr::RenderMainWindow()
{
    ImGui::Begin("Debug Option");

    ImGui::Text("Cam Pos : (%.2f, %.2f, %.2f)", GAMEINSTANCE->GetCamPosition().x, GAMEINSTANCE->GetCamPosition().y, GAMEINSTANCE->GetCamPosition().z);
 
    if (GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player")))
    {
        _float3 vPlrPos = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"))->GetTransform()->GetState(CTransform::STATE_POSITION);
        ImGui::Text("Plr Pos : (%.2f, %.2f, %.2f)", vPlrPos.x, vPlrPos.y, vPlrPos.z);
    }
    ImGui::InputFloat3("Move Pos", &m_vMovePoint.x);

    if (ImGui::Button("Move")) {

        shared_ptr<CGameObject> pPlr = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));

        if (pPlr)
            pPlr->GetTransform()->SetState(CTransform::STATE_POSITION, m_vMovePoint);

    }



    ImGui::Text("Debug Option On/Off : `");

    /*렌더 타겟 On&Off : 라디오 버튼*/
    /*
    *렌더 타겟 그룹 지정
    1: 기본 렌더(디퓨즈 노말)
    2: 포스트 프로세스 렌더 타겟()
    */

    ImGui::SeparatorText("0. Render Target");

    _bool prevTargetVisible = m_IsTargetVisible;

    ImGui::Checkbox("Render Target On", &m_IsTargetVisible);

    if (m_IsTargetVisible != prevTargetVisible) {
        GAMEINSTANCE->DebugRenderSwitch();
    }

    _int prevGroupIdx = m_iTargetNum;
    
    ImGui::RadioButton("Group1(Deferred)", &m_iTargetNum, 0);
    ImGui::RadioButton("Group2(PostProcess)", &m_iTargetNum, 1);

    if (prevGroupIdx != m_iTargetNum) {
        GAMEINSTANCE->SetRenderGourpIdx(m_iTargetNum);
    }

    ImGui::SeparatorText("0-1 . Collider");

    if (ImGui::Checkbox("Collider On", &m_IsColliderVisible))
    {
        GAMEINSTANCE->DebugColliderSwitch();
    }



    /*
      카메라 옵션
    : 카메라 타입 선택(자유시점 카메라, 기본 카메라(3인칭 카메라))
      카메라 커서 락 On/Off
      회전 On/Off
    */

    ImGui::SeparatorText("1. Camera");

    UINT iPrevCamIndex = m_iCamIndex;

    auto* Cameras = CCameraMgr::GetInstance()->GetCameras();
    UINT iIndex = 0;
    string CamTag = "";
	for (auto iter : *Cameras)
	{
		ImGui::RadioButton(iter.first.c_str(), &m_iCamIndex, iIndex);
        if (m_iCamIndex == iIndex)
        {
            CamTag = iter.first;
        }
        iIndex++;
	}

	if (iPrevCamIndex != m_iCamIndex)
	{
        //CCameraMgr::GetInstance()->CameraTurnOn(CamTag, 1.0f);
		CCameraMgr::GetInstance()->CameraTurnOn(CamTag);
        //CGameMgr::GetInstance()->ChangeToNormalMouse();
	}

    ImGui::Dummy(ImVec2(2.f, 2.f));

    ImGui::Text("Inventory ON/OFF : I");
    ImGui::Text("---Player Control---");
    ImGui::Text("Jump : E");
    ImGui::Text("Weapon:[Z:Sword/X:CrossBow/C:Staff]");
    ImGui::Text("Camera LockOn : R");
    ImGui::Text("LockOn Target Change : Tab");
    ImGui::Text("Parrying : Q");
    ImGui::Text("WolfForm ON/OFF : L Shift");
    ImGui::Text("WolfForm Sprint : SpaceBar");
    ImGui::Text("Warp To Fonos Basin : 8");


    ImGui::End();
}

void CImguiMgr::ShaderOptionWindow()
{
    ImGui::Begin("Shader Option");

    _bool PrevToneMap = m_IsToneMappingOn;

    ImGui::Checkbox("Tone Mapping", &m_IsToneMappingOn); // 얘 빼고

    if (PrevToneMap != m_IsToneMappingOn) {

        GAMEINSTANCE->SwitchingToneMappingOption(m_IsToneMappingOn);

    }

    _bool PrevPBRMap = m_IsPBROn;

    ImGui::Checkbox("PBR", &m_IsPBROn);

    if (PrevPBRMap != m_IsPBROn) {

        GAMEINSTANCE->SwitchingPBROption(m_IsPBROn);

    }

    _bool PrevAOMap = m_IsAOOn;

    ImGui::Checkbox("AO", &m_IsAOOn);

    if (PrevAOMap != m_IsAOOn) {

        GAMEINSTANCE->SwitchingAOOptions(m_IsAOOn);

    }

    
    if (ImGui::SliderFloat("AO Power", &m_fAOPower, 1.f, 50.f)) {
        GAMEINSTANCE->ChangeAOPower(m_fAOPower);
    }

    _bool PrevShadowMap = m_IsShadowOn;

    ImGui::Checkbox("Shadow", &m_IsShadowOn);

    if (PrevShadowMap != m_IsShadowOn) {

        GAMEINSTANCE->SwitchingShadowOption(m_IsShadowOn);
    }

    //SwitchingDOF
    
    _bool PrevDOFOption = m_IsDOFOn;

    ImGui::Checkbox("Depth Of Field", &m_IsDOFOn);

    if (PrevDOFOption != m_IsDOFOn) {

        GAMEINSTANCE->SwitchingDOF(m_IsDOFOn);
    }

    //SwitchingFog

    _bool PrevFogOption = m_IsFogOn;

    ImGui::Checkbox("Fog", &m_IsFogOn);

    if (PrevFogOption != m_IsFogOn) {

        GAMEINSTANCE->SwitchingFog(m_IsFogOn);
    }


    if (ImGui::Checkbox("HDR", &m_IsHDROn))
    {
        GAMEINSTANCE->SwitchingHDROption(m_IsHDROn);
    }


    if (ImGui::SliderFloat("Constrast", &m_fConstrast, 0.f, 1.f)) {
        GAMEINSTANCE->SetToneMappingProperties(m_fSaturate, m_fConstrast, m_fMiddleGray);
    }

    if (ImGui::SliderFloat("MiddleGray", &m_fMiddleGray, 0.f, 1.f)) {
        GAMEINSTANCE->SetToneMappingProperties(m_fSaturate, m_fConstrast, m_fMiddleGray);
    }

    if (ImGui::SliderFloat("Saturate", &m_fSaturate, 0.f, 1.f)) {
        GAMEINSTANCE->SetToneMappingProperties(m_fSaturate, m_fConstrast, m_fMiddleGray);
    }

    ImGui::End();

}
void CImguiMgr::CamShakeWindow()
{
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x - 300.0f, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, main_viewport->Size.y), ImGuiCond_Once);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    /* Begin */
    ImGui::Begin("Camera", nullptr, window_flags);

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
        ImGui::InputText("FileName", &m_strFileNameShake);
        if (ImGui::Button("Shake"))
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

            if (m_fShakeDuration > 0.0f && m_eShakeFlag)
            {
                CCameraMgr::GetInstance()->CameraShake(m_eShakeFlag, m_fShakeDuration, &m_RotDesc, &m_LocDesc, &m_FovDesc);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            CCameraMgr::GetInstance()->CameraShakeStop();
        }
        ImGui::SameLine();
        if (ImGui::Button("Save"))
		{
			if (FAILED(SaveShakeData()))
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
			if (FAILED(LoadShakeData()))
			{
				MessageBox(nullptr, L"불러오기 실패", L"system", MB_OK);
			}
			else
			{
				MessageBox(nullptr, L"불러오기 성공", L"system", MB_OK);
			}
		}
        

        ImGui::TreePop();
    }

    ImGui::End();

}
HRESULT CImguiMgr::SaveShakeData()
{
    string strFilePath = CameraSaveLoadPath + m_strFileNameShake + ".json";

	Json::Value Shake;

	Shake["ShakeInfo"]["ShakeFlags"] = m_eShakeFlag;
	Shake["ShakeInfo"]["Duration"] = m_fShakeDuration;

	if (m_eShakeFlag & SHAKE_ROT)
	{
		Shake["ShakeInfo"]["RotDesc"]["Amplitude"].append(m_RotDesc.vRotAmplitude.x);
		Shake["ShakeInfo"]["RotDesc"]["Amplitude"].append(m_RotDesc.vRotAmplitude.y);
		Shake["ShakeInfo"]["RotDesc"]["Amplitude"].append(m_RotDesc.vRotAmplitude.z);

		Shake["ShakeInfo"]["RotDesc"]["Frequency"].append(m_RotDesc.vRotFrequency.x);
		Shake["ShakeInfo"]["RotDesc"]["Frequency"].append(m_RotDesc.vRotFrequency.y);
		Shake["ShakeInfo"]["RotDesc"]["Frequency"].append(m_RotDesc.vRotFrequency.z);

		Shake["ShakeInfo"]["RotDesc"]["BlendInOut"].append(m_RotDesc.vBlendInOut.x);
		Shake["ShakeInfo"]["RotDesc"]["BlendInOut"].append(m_RotDesc.vBlendInOut.y);

		Shake["ShakeInfo"]["RotDesc"]["InOutWeight"].append(m_RotDesc.vInOutWeight.x);
		Shake["ShakeInfo"]["RotDesc"]["InOutWeight"].append(m_RotDesc.vInOutWeight.y);

		Shake["ShakeInfo"]["RotDesc"]["ShakeFunc"] = (UINT)m_RotDesc.eShakeFunc;

		Shake["ShakeInfo"]["RotDesc"]["isOffSetRand"] = m_RotDesc.isOffSetRand;
	}
	if (m_eShakeFlag & SHAKE_LOC)
	{
		Shake["ShakeInfo"]["LocDesc"]["Amplitude"].append(m_LocDesc.vLocAmplitude.x);
		Shake["ShakeInfo"]["LocDesc"]["Amplitude"].append(m_LocDesc.vLocAmplitude.y);
		Shake["ShakeInfo"]["LocDesc"]["Amplitude"].append(m_LocDesc.vLocAmplitude.z);

		Shake["ShakeInfo"]["LocDesc"]["Frequency"].append(m_LocDesc.vLocFrequency.x);
		Shake["ShakeInfo"]["LocDesc"]["Frequency"].append(m_LocDesc.vLocFrequency.y);
		Shake["ShakeInfo"]["LocDesc"]["Frequency"].append(m_LocDesc.vLocFrequency.z);

		Shake["ShakeInfo"]["LocDesc"]["BlendInOut"].append(m_LocDesc.vBlendInOut.x);
		Shake["ShakeInfo"]["LocDesc"]["BlendInOut"].append(m_LocDesc.vBlendInOut.y);

		Shake["ShakeInfo"]["LocDesc"]["InOutWeight"].append(m_LocDesc.vInOutWeight.x);
		Shake["ShakeInfo"]["LocDesc"]["InOutWeight"].append(m_LocDesc.vInOutWeight.y);

		Shake["ShakeInfo"]["LocDesc"]["ShakeFunc"] = (UINT)m_LocDesc.eShakeFunc;

		Shake["ShakeInfo"]["LocDesc"]["isOffSetRand"] = m_LocDesc.isOffSetRand;
	}
	if (m_eShakeFlag & SHAKE_FOV)
	{
		Shake["ShakeInfo"]["FovDesc"]["Amplitude"] = m_FovDesc.fFovyAmplitude;

		Shake["ShakeInfo"]["FovDesc"]["Frequency"] = m_FovDesc.fFovyFrequency;

		Shake["ShakeInfo"]["FovDesc"]["BlendInOut"].append(m_FovDesc.vBlendInOut.x);
		Shake["ShakeInfo"]["FovDesc"]["BlendInOut"].append(m_FovDesc.vBlendInOut.y);

		Shake["ShakeInfo"]["FovDesc"]["InOutWeight"].append(m_FovDesc.vInOutWeight.x);
		Shake["ShakeInfo"]["FovDesc"]["InOutWeight"].append(m_FovDesc.vInOutWeight.y);

		Shake["ShakeInfo"]["FovDesc"]["ShakeFunc"] = (UINT)m_FovDesc.eShakeFunc;
	}

	GAMEINSTANCE->WriteJson(Shake, wstring().assign(strFilePath.begin(), strFilePath.end()));

	return S_OK;
}

HRESULT CImguiMgr::LoadShakeData()
{
    string strFilePath = CameraSaveLoadPath + m_strFileNameShake + ".json";

    Json::Value KeyFrames = GAMEINSTANCE->ReadJson(wstring().assign(strFilePath.begin(), strFilePath.end()));

	if (KeyFrames.isMember("ShakeInfo"))
	{
        m_eShakeFlag = KeyFrames["ShakeInfo"]["ShakeFlags"].asUInt();
        m_fShakeDuration = KeyFrames["ShakeInfo"]["Duration"].asFloat();

		if (m_eShakeFlag & SHAKE_ROT)
		{
            m_RotDesc.vRotAmplitude.x = KeyFrames["ShakeInfo"]["RotDesc"]["Amplitude"][0].asFloat();
			m_RotDesc.vRotAmplitude.y = KeyFrames["ShakeInfo"]["RotDesc"]["Amplitude"][1].asFloat();
			m_RotDesc.vRotAmplitude.z = KeyFrames["ShakeInfo"]["RotDesc"]["Amplitude"][2].asFloat();

			m_RotDesc.vRotFrequency.x = KeyFrames["ShakeInfo"]["RotDesc"]["Frequency"][0].asFloat();
            m_RotDesc.vRotFrequency.y = KeyFrames["ShakeInfo"]["RotDesc"]["Frequency"][1].asFloat();
            m_RotDesc.vRotFrequency.z = KeyFrames["ShakeInfo"]["RotDesc"]["Frequency"][2].asFloat();

            m_RotDesc.vBlendInOut.x = KeyFrames["ShakeInfo"]["RotDesc"]["BlendInOut"][0].asFloat();
            m_RotDesc.vBlendInOut.y = KeyFrames["ShakeInfo"]["RotDesc"]["BlendInOut"][1].asFloat();

            m_RotDesc.vInOutWeight.x = KeyFrames["ShakeInfo"]["RotDesc"]["InOutWeight"][0].asFloat();
            m_RotDesc.vInOutWeight.y = KeyFrames["ShakeInfo"]["RotDesc"]["InOutWeight"][1].asFloat();

            m_RotDesc.eShakeFunc = (ESHAKEFUNC)KeyFrames["ShakeInfo"]["RotDesc"]["ShakeFunc"].asUInt();

            m_RotDesc.isOffSetRand = KeyFrames["ShakeInfo"]["RotDesc"]["isOffSetRand"].asBool();
		}
        else
        {
            m_RotDesc = ROT_DESC();
        }
		if (m_eShakeFlag & SHAKE_LOC)
		{
			m_LocDesc.vLocAmplitude.x = KeyFrames["ShakeInfo"]["LocDesc"]["Amplitude"][0].asFloat();
			m_LocDesc.vLocAmplitude.y = KeyFrames["ShakeInfo"]["LocDesc"]["Amplitude"][1].asFloat();
			m_LocDesc.vLocAmplitude.z = KeyFrames["ShakeInfo"]["LocDesc"]["Amplitude"][2].asFloat();

			m_LocDesc.vLocFrequency.x = KeyFrames["ShakeInfo"]["LocDesc"]["Frequency"][0].asFloat();
			m_LocDesc.vLocFrequency.y = KeyFrames["ShakeInfo"]["LocDesc"]["Frequency"][1].asFloat();
			m_LocDesc.vLocFrequency.z = KeyFrames["ShakeInfo"]["LocDesc"]["Frequency"][2].asFloat();

			m_LocDesc.vBlendInOut.x = KeyFrames["ShakeInfo"]["LocDesc"]["BlendInOut"][0].asFloat();
			m_LocDesc.vBlendInOut.y = KeyFrames["ShakeInfo"]["LocDesc"]["BlendInOut"][1].asFloat();

			m_LocDesc.vInOutWeight.x = KeyFrames["ShakeInfo"]["LocDesc"]["InOutWeight"][0].asFloat();
			m_LocDesc.vInOutWeight.y = KeyFrames["ShakeInfo"]["LocDesc"]["InOutWeight"][1].asFloat();

            m_LocDesc.eShakeFunc = (ESHAKEFUNC)KeyFrames["ShakeInfo"]["LocDesc"]["ShakeFunc"].asUInt();

            m_LocDesc.isOffSetRand = KeyFrames["ShakeInfo"]["LocDesc"]["isOffSetRand"].asBool();
		}
        else
        {
            m_LocDesc = LOC_DESC();
        }
		if (m_eShakeFlag & SHAKE_FOV)
		{
            m_FovDesc.fFovyAmplitude = KeyFrames["ShakeInfo"]["FovDesc"]["Amplitude"].asFloat();

            m_FovDesc.fFovyFrequency = KeyFrames["ShakeInfo"]["FovDesc"]["Frequency"].asFloat();

            m_FovDesc.vBlendInOut.x = KeyFrames["ShakeInfo"]["FovDesc"]["BlendInOut"][0].asFloat();
            m_FovDesc.vBlendInOut.y = KeyFrames["ShakeInfo"]["FovDesc"]["BlendInOut"][1].asFloat();

            m_FovDesc.vInOutWeight.x = KeyFrames["ShakeInfo"]["FovDesc"]["InOutWeight"][0].asFloat();
            m_FovDesc.vInOutWeight.y = KeyFrames["ShakeInfo"]["FovDesc"]["InOutWeight"][1].asFloat();

            m_FovDesc.eShakeFunc = (ESHAKEFUNC)KeyFrames["ShakeInfo"]["FovDesc"]["ShakeFunc"].asUInt();
		}
        else
        {
            m_FovDesc = FOV_DESC();
        }
	}

    return S_OK;
}
//#endif

void CImguiMgr::KeyInput()
{

}

void CImguiMgr::ShutDown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

shared_ptr<CImguiMgr> CImguiMgr::Create()
{
    shared_ptr<CImguiMgr> pInstance = make_shared<CImguiMgr>();

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create");

    return pInstance;
}
