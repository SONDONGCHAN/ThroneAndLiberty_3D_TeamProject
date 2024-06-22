#include "AnimToolImguiManager.h"
#include "GameInstance.h"
#include "AnimObject.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CAnimToolImguiManager)

CAnimToolImguiManager::CAnimToolImguiManager()
{
}

CAnimToolImguiManager::~CAnimToolImguiManager()
{
    ShutDown();
}

HRESULT CAnimToolImguiManager::Initialize()
{
	m_pDevice = GAMEINSTANCE->GetDeviceInfo();
	m_pContext = GAMEINSTANCE->GetDeviceContextInfo();

	ImGuiSetUp();

    wstring strStaticPath = TEXT("../../Client/Bin/Resources/Level_Static/Models/");
    LoadAllAnimModelName(strStaticPath);
    GAMEINSTANCE->DebugRenderSwitch();
    LoadAllShapesJSON();
    LoadAllProjTriggersJSON();
    LoadAllSoundJSON();
    FindAllSoundPath("../../Client/Bin/Sounds/", ".wav");
	return S_OK;
}

void CAnimToolImguiManager::ImGuiSetUp()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get());
}

void CAnimToolImguiManager::ImGuiTick()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    ShowGui();
   
    ImGui::EndFrame();
}

void CAnimToolImguiManager::ImGuiRender()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CAnimToolImguiManager::ShowGui()
{
    static string CurrentModelName;

    static int ColTypeIndex = 1;
    
    static float BoxSize[3] = { 1.f, 1.f, 1.f };
    static float Radius = 1.000f;
    static float CapRadius = 1.000f;
    static float Height = 1.000f;
    
    static float OffsetPosition[3] = { 0.f, 0.f, 0.f };
    static float OffsetRotation[3] = { 0.f, 0.f, 0.f };
    
    static int MyFlagIndex = 0;
    
    static bool COL_STATIC              = false;
    static bool COL_PLAYER              = false;
    static bool COL_PLAYERWEAPON        = false;
    static bool COL_PLAYERSKILL         = false;
    static bool COL_PLAYERPROJECTILE    = false;
    static bool COL_PLAYERPARRYING      = false;
    static bool COL_MONSTER             = false;
    static bool COL_MONSTERWEAPON       = false;
    static bool COL_MONSTERSKILL        = false;
    static bool COL_MONSTERPROJECTILE   = false;

    static float fEnablePos = { 0.f };
    static float fDisablePos = { 0.f };

    ///////////////////////////

    static int MyProjectileOwner = 0;

    static ProjectileInfo PtInfo;

    ImGui::Begin("Model_Loader");

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

            ImGui::Separator();

            if (ImGui::Button("Load_Model"))
            {
                m_pCurrentAnimModel = GAMEINSTANCE->GetModel(m_vecAnimModelName[m_iCurrentModelIndex]);
                CurrentModelName = m_vecAnimModelName[m_iCurrentModelIndex];

                if (!m_bObjectCreated)
                {
                    m_bObjectCreated = true;
                    m_pAnimObject = CAnimObject::Create(m_pCurrentAnimModel);
                    GAMEINSTANCE->AddObject(LEVEL_EDIT, TEXT("Layer_AnimObject"), m_pAnimObject);
                }
                else
                    m_pAnimObject->SetModel(m_pCurrentAnimModel);

                if (nullptr != m_pCurrentAnimModel)
                {
                    m_CurrentModelAnimations = m_pCurrentAnimModel->GetAnimations();

                    m_vecAnimationNames.clear();

                    for (auto pAnimation : m_CurrentModelAnimations)
                    {
                        const _char* pName = pAnimation->GetName();
                        m_vecAnimationNames.push_back(pName);

                        pAnimation->SetOwnerObject(m_pAnimObject);                           
                    }

                    m_iCurrentAnimationIndex = 0;
                    m_pCurrentAnimModel->ChangeAnim(m_iCurrentAnimationIndex, 0.1f, true);

                }
                if (m_ModelLoadDatas.find(m_vecAnimModelName[m_iCurrentModelIndex]) != m_ModelLoadDatas.end())
                {
                    m_CurrentShapes = m_ModelLoadDatas.find(m_vecAnimModelName[m_iCurrentModelIndex])->second;

                    if (m_ProjTriggerLoadDatas.find(m_vecAnimModelName[m_iCurrentModelIndex]) != m_ProjTriggerLoadDatas.end())
                        m_CurrentProjectileDatas = m_ProjTriggerLoadDatas.find(m_vecAnimModelName[m_iCurrentModelIndex])->second;

                    //if (m_ModelSoundDatas.find(CurrentModelName) != m_ModelSoundDatas.end())
                    //{
                    //    m_CurrentModelSoundDatas = m_ModelSoundDatas.find(CurrentModelName)->second;
                    //}
                    //else
                    //{
                    //    m_CurrentModelSoundDatas.clear();
                    //}

                }
                if (m_ModelSoundDatas.find(m_vecAnimModelName[m_iCurrentModelIndex]) != m_ModelSoundDatas.end())
                {
                    m_CurrentModelSoundDatas = m_ModelSoundDatas.find(m_vecAnimModelName[m_iCurrentModelIndex])->second;
                }
                else
                    m_CurrentModelSoundDatas.clear();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Monster"))
        {
            ImGui::Separator();

            if (ImGui::Button("Load_Model"))
            {
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::Separator();

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

                    //=======================For Projectile Trigger======================
                    if (m_CurrentProjectileDatas.size() != 0)
                    {
                        _bool bHaveData = { false };

                        for(auto pData : m_CurrentProjectileDatas)
                        {
                            if (pData.iAnimationIndex == m_iCurrentAnimationIndex)
                            {
                                bHaveData = true;
                                PtInfo = pData;
                                break;
                            }                          
                        }

                        if (!bHaveData)
                        {
                            PtInfo.iAnimationIndex = m_iCurrentAnimationIndex;
                            PtInfo.fProjectileTriggerTrackPoss.clear();
                            PtInfo.eProjectileTriggerOwners.clear();
                        }
                    }
                    else
                    {
                        PtInfo.iAnimationIndex = m_iCurrentAnimationIndex;
                        PtInfo.fProjectileTriggerTrackPoss.clear();
                        PtInfo.eProjectileTriggerOwners.clear();
                    }
                    //===================================================================

                    //=========================For Sound Trigger=========================
                    if (m_CurrentModelSoundDatas.find(m_CurrentModelAnimations[m_iCurrentAnimationIndex]->GetName()) != m_CurrentModelSoundDatas.end())
                    {
                        m_CurrnetAnimSoundDatas = m_CurrentModelSoundDatas.find(m_CurrentModelAnimations[m_iCurrentAnimationIndex]->GetName())->second;
                    }
                    else
                        m_CurrnetAnimSoundDatas.clear();

                    //===================================================================

                    m_pCurrentAnimModel->ChangeAnim(m_iCurrentAnimationIndex, 0.1f, true);
                    m_CurrentModelAnimations[m_iCurrentAnimationIndex]->SetTrackPosition(0.f);
                    m_pAnimObject->DisableCollider();

                    auto iter = m_CurrentShapes.find(m_vecAnimationNames[m_iCurrentAnimationIndex]);

                    if(iter != m_CurrentShapes.end())
                    {
                        ShapeInfo SI = iter->second;

                        ColTypeIndex = SI.iGeometryType;

                        if (ColTypeIndex == 1)
                        {
                            m_pAnimObject->SetCollider(CCollider::OBB_TYPE);
                            BoxSize[0] = SI.vBoxSize.x;
                            BoxSize[1] = SI.vBoxSize.y;
                            BoxSize[2] = SI.vBoxSize.z;
                            m_pAnimObject->GetCollider()->Set_Extents(_float3(BoxSize[0] * 0.5f, BoxSize[1] * 0.5f, BoxSize[2] * 0.5f));
                        }
                        else if (ColTypeIndex == 2) 
                        {
                            m_pAnimObject->SetCollider(CCollider::SPHERE_TYPE);
                            Radius = SI.fSphereRadius;
                            m_pAnimObject->GetCollider()->Set_Radius(Radius);
                        }
                        else if (ColTypeIndex == 3)
                        {
                            m_pAnimObject->SetCollider(CCollider::OBB_TYPE);
                            CapRadius = SI.fCapsuleRadius;
                            Height = SI.fCapsuleHeight;
                            m_pAnimObject->GetCollider()->Set_Extents(_float3(CapRadius + (Height * 0.5f), CapRadius, CapRadius));
                        }

                        OffsetPosition[0] = SI.vOffSetPosition.x;
                        OffsetPosition[1] = SI.vOffSetPosition.y;
                        OffsetPosition[2] = SI.vOffSetPosition.z;
                        m_pAnimObject->GetCollider()->Set_Center(_float3(OffsetPosition[0], OffsetPosition[1], OffsetPosition[2]));
                        
                        OffsetRotation[0] = SI.vOffSetRotation.x;
                        OffsetRotation[1] = SI.vOffSetRotation.y;
                        OffsetRotation[2] = SI.vOffSetRotation.z;
                        m_pAnimObject->GetCollider()->Set_Rotation(_float3(OffsetRotation[0], OffsetRotation[1], OffsetRotation[2]));

                        _uint MyFlag = SI.iMyFlag;
                        
                        _uint MFlag = (1 << 0);
                        if (MyFlag & MFlag) MyFlagIndex = 0;
                        MFlag = (1 << 1);
                        if (MyFlag & MFlag) MyFlagIndex = 1;
                        MFlag = (1 << 2);
                        if (MyFlag & MFlag) MyFlagIndex = 2;
                        MFlag = (1 << 3);
                        if (MyFlag & MFlag) MyFlagIndex = 3;
                        MFlag = (1 << 4);
                        if (MyFlag & MFlag) MyFlagIndex = 4;
                        MFlag = (1 << 5);
                        if (MyFlag & MFlag) MyFlagIndex = 5;
                        MFlag = (1 << 10);
                        if (MyFlag & MFlag) MyFlagIndex = 10;
                        MFlag = (1 << 11);
                        if (MyFlag & MFlag) MyFlagIndex = 11;
                        MFlag = (1 << 12);
                        if (MyFlag & MFlag) MyFlagIndex = 12;
                        MFlag = (1 << 13);
                        if (MyFlag & MFlag) MyFlagIndex = 13;


                        _uint TargetFlag = SI.iTargetFlag;

                        COL_STATIC          = false;
                        COL_PLAYER          = false;
                        COL_PLAYERWEAPON    = false;
                        COL_PLAYERSKILL     = false;
                        COL_PLAYERPROJECTILE = false;
                        COL_PLAYERPARRYING = false;
                        COL_MONSTER         = false;
                        COL_MONSTERWEAPON   = false;
                        COL_MONSTERSKILL    = false;
                        COL_MONSTERPROJECTILE = false;

                        _uint TFlag = (1 << 0);
                        if (TargetFlag & TFlag) COL_STATIC = true;
                        TFlag = (1 << 1);
                        if (TargetFlag & TFlag) COL_PLAYER = true;
                        TFlag = (1 << 2);
                        if (TargetFlag & TFlag) COL_PLAYERWEAPON = true;
                        TFlag = (1 << 3);
                        if (TargetFlag & TFlag) COL_PLAYERSKILL = true;
                        TFlag = (1 << 4);
                        if (TargetFlag & TFlag) COL_PLAYERPROJECTILE = true;
                        TFlag = (1 << 5);
                        if (TargetFlag & TFlag) COL_PLAYERPARRYING = true;
                        TFlag = (1 << 10);
                        if (TargetFlag & TFlag) COL_MONSTER = true;
                        TFlag = (1 << 11);
                        if (TargetFlag & TFlag) COL_MONSTERWEAPON = true;
                        TFlag = (1 << 12);
                        if (TargetFlag & TFlag) COL_MONSTERSKILL = true;
                        TFlag = (1 << 13);
                        if (TargetFlag & TFlag) COL_MONSTERPROJECTILE = true;

                        fEnablePos = SI.fColStartTrackPos;
                        fDisablePos = SI.fColEndTrackPos;
                        m_CurrentModelAnimations[m_iCurrentAnimationIndex]->SetColStartTrackPos(fEnablePos);
                        m_CurrentModelAnimations[m_iCurrentAnimationIndex]->SetColEndTrackPos(fDisablePos);
                    }
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
        _double	TrackPosition = m_CurrentModelAnimations[m_iCurrentAnimationIndex]->GetTrackPosition();
        _float  fCurrentTime = (_float)TrackPosition;

        //"Playing" ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())
        if (ImGui::SliderFloat(to_string(AnimationDuration).c_str(), &fCurrentTime, 0.f, AnimationDuration, "%.3f"))
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
    ImGui::Begin("Event_Importer");

    //ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
    //ImGui::SeparatorText("Model Name :");
    //ImGui::Text("dear imgui says hello! (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);

    ImGui::BulletText("Model");
    ImGui::Text(CurrentModelName.c_str());

    ImGui::BulletText("Animation");
    if(m_iCurrentAnimationIndex!= -1)
        ImGui::Text(m_vecAnimationNames[m_iCurrentAnimationIndex].c_str());

    ImGui::Separator();

    ImGui::BeginTabBar("tabs");
    {
        if (ImGui::BeginTabItem("Collider"))
        {
            ImGui::SeparatorText("Geometry Type");

            _int prevCOlTypeIndex = ColTypeIndex;
           
            ImGui::RadioButton("BOX", &ColTypeIndex, 1); ImGui::SameLine();
            ImGui::RadioButton("SPHERE", &ColTypeIndex, 2); ImGui::SameLine();
            ImGui::RadioButton("CAPSULE", &ColTypeIndex, 3);

            if (ColTypeIndex != prevCOlTypeIndex)
            {
                m_pAnimObject->EnableCollider();

                if (ColTypeIndex == 1)
                    m_pAnimObject->SetCollider(CCollider::OBB_TYPE);
                else if (ColTypeIndex == 2)
                    m_pAnimObject->SetCollider(CCollider::SPHERE_TYPE);
                else if (ColTypeIndex == 3)
                    m_pAnimObject->SetCollider(CCollider::OBB_TYPE);
            }

            if (ImGui::CollapsingHeader("Shape Info"))
            {
                ImGui::SeparatorText("Size");
                
                if (ColTypeIndex == 1)
                {
                    ImGui::InputFloat3("Box Size", BoxSize);

                    m_pAnimObject->GetCollider()->Set_Extents(_float3(BoxSize[0] * 0.5f, BoxSize[1] * 0.5f, BoxSize[2] * 0.5f));
                }
                else if (ColTypeIndex == 2)
                {
                    ImGui::InputFloat("Radius", &Radius, 0.1f, 1.0f, "%.3f");

                    m_pAnimObject->GetCollider()->Set_Radius(Radius);
                }
                else if (ColTypeIndex == 3)
                {
                    ImGui::InputFloat("Radius", &CapRadius, 0.1f, 1.0f, "%.3f");
                    ImGui::InputFloat("Height", &Height, 0.1f, 1.0f, "%.3f");

                    m_pAnimObject->GetCollider()->Set_Extents(_float3(CapRadius+(Height*0.5f), CapRadius, CapRadius));
                }

                ImGui::SeparatorText("Offset Position");
                ImGui::InputFloat3("Offset Pos", OffsetPosition);
                m_pAnimObject->GetCollider()->Set_Center(_float3(OffsetPosition[0], OffsetPosition[1], OffsetPosition[2]) );


                ImGui::SeparatorText("Offset Rotation");
                ImGui::InputFloat3("Offset Rot", OffsetRotation);
                m_pAnimObject->GetCollider()->Set_Rotation(_float3(OffsetRotation[0], OffsetRotation[1], OffsetRotation[2]));
            }

            ImGui::SeparatorText("Input Trigger");

            ImGui::BulletText("Track Position");  ImGui::SameLine();

            _double	TrackPosition = 0.f;

            if (m_iCurrentAnimationIndex != -1)
            {
                TrackPosition = m_CurrentModelAnimations[m_iCurrentAnimationIndex]->GetTrackPosition();
                ImGui::Text(to_string(TrackPosition).c_str());
            }

            if (ImGui::Button("Insert Enable Trigger"))  
            {
                fEnablePos = (_float)TrackPosition;
            }

            ImGui::InputFloat("Enable", &fEnablePos, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);

            ImGui::Separator();

            if (ImGui::Button("Insert Disable Trigger"))
            {
                fDisablePos = (_float)TrackPosition;
            }
            
            ImGui::InputFloat("Disable", &fDisablePos, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);

            ImGui::Separator();

            if (ImGui::Button("Apply Collider Trigger"))
            {
                m_CurrentModelAnimations[m_iCurrentAnimationIndex]->SetColStartTrackPos(fEnablePos);
                m_CurrentModelAnimations[m_iCurrentAnimationIndex]->SetColEndTrackPos(fDisablePos);
            }

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Col Flag"))
            {
                ImGui::SeparatorText("My Col");
            
                ImGui::RadioButton("MCOL_STATIC",           &MyFlagIndex, 0);
                ImGui::RadioButton("MCOL_PLAYER",           &MyFlagIndex, 1);
                ImGui::RadioButton("MCOL_PLAYERWEAPON",     &MyFlagIndex, 2);
                ImGui::RadioButton("MCOL_PLAYERSKILL",      &MyFlagIndex, 3);
                ImGui::RadioButton("MCOL_PLAYERPROJECTILE", &MyFlagIndex, 4);
                ImGui::RadioButton("MCOL_PLAYERPARRYING",   &MyFlagIndex, 5);
                ImGui::RadioButton("MCOL_MONSTER",          &MyFlagIndex, 10);
                ImGui::RadioButton("MCOL_MONSTERWEAPON",    &MyFlagIndex, 11);
                ImGui::RadioButton("MCOL_MONSTERSKILL",     &MyFlagIndex, 12);
                ImGui::RadioButton("MCOL_MONSTERPROJECTILE",&MyFlagIndex, 13);


                ImGui::SeparatorText("Target Col");
            
                ImGui::TableNextColumn(); ImGui::Checkbox("TCOL_STATIC",            &COL_STATIC);
                ImGui::TableNextColumn(); ImGui::Checkbox("TCOL_PLAYER",            &COL_PLAYER);
                ImGui::TableNextColumn(); ImGui::Checkbox("TCOL_PLAYERWEAPON",      &COL_PLAYERWEAPON);
                ImGui::TableNextColumn(); ImGui::Checkbox("TCOL_PLAYERSKILL",       &COL_PLAYERSKILL);
                ImGui::TableNextColumn(); ImGui::Checkbox("TCOL_PLAYERPROJECTILE",  &COL_PLAYERPROJECTILE);
                ImGui::TableNextColumn(); ImGui::Checkbox("TCOL_PLAYERPARRYING",    &COL_PLAYERPARRYING);
                ImGui::TableNextColumn(); ImGui::Checkbox("TCOL_MONSTER",           &COL_MONSTER);
                ImGui::TableNextColumn(); ImGui::Checkbox("TCOL_MONSTERWEAPON",     &COL_MONSTERWEAPON);
                ImGui::TableNextColumn(); ImGui::Checkbox("TCOL_MONSTERSKILL",      &COL_MONSTERSKILL);
                ImGui::TableNextColumn(); ImGui::Checkbox("TCOL_MONSTERPROJECTILE", &COL_MONSTERPROJECTILE);

            }     

            ImGui::Separator();

            _uint iMyFlag = (1 << MyFlagIndex) ;
            _uint iTargetFlag = { 0 };

            if (COL_STATIC)             iTargetFlag += (1 << 0);
            if (COL_PLAYER)             iTargetFlag += (1 << 1);
            if (COL_PLAYERWEAPON)       iTargetFlag += (1 << 2);
            if (COL_PLAYERSKILL)        iTargetFlag += (1 << 3);
            if (COL_PLAYERPROJECTILE)   iTargetFlag += (1 << 4);
            if (COL_PLAYERPARRYING)     iTargetFlag += (1 << 5);
            if (COL_MONSTER)            iTargetFlag += (1 << 10);
            if (COL_MONSTERWEAPON)      iTargetFlag += (1 << 11);
            if (COL_MONSTERSKILL)       iTargetFlag += (1 << 12);
            if (COL_MONSTERPROJECTILE)  iTargetFlag += (1 << 13);

            ImGui::SeparatorText("Save Shape");
            
            if (ImGui::Button("Save Shape Info"))
            {
                ShapeInfo stShapeInfo;
                stShapeInfo.iAnimationIndex = m_iCurrentAnimationIndex;
                stShapeInfo.iGeometryType = ColTypeIndex;

                if (ColTypeIndex == 1)
                    stShapeInfo.vBoxSize = _float3(BoxSize[0], BoxSize[1], BoxSize[2]);
                else if (ColTypeIndex == 2)
                    stShapeInfo.fSphereRadius = Radius;
                else if (ColTypeIndex == 3) {
                    stShapeInfo.fCapsuleHeight = Height;
                    stShapeInfo.fCapsuleRadius = CapRadius;
                };

                stShapeInfo.vOffSetPosition = _float3(OffsetPosition[0], OffsetPosition[1], OffsetPosition[2]);
                stShapeInfo.vOffSetRotation = _float3(OffsetRotation[0], OffsetRotation[1], OffsetRotation[2]);
                stShapeInfo.iMyFlag = iMyFlag;
                stShapeInfo.iTargetFlag = iTargetFlag;
                stShapeInfo.fColStartTrackPos = fEnablePos;
                stShapeInfo.fColEndTrackPos = fDisablePos;

                auto iter = m_CurrentShapes.find(m_vecAnimationNames[m_iCurrentAnimationIndex]);

                if (iter != m_CurrentShapes.end())
                {
                    iter->second = stShapeInfo;
                }
                else 
                {
                    m_CurrentShapes.emplace(m_vecAnimationNames[m_iCurrentAnimationIndex], stShapeInfo);
                }
                
            }

            ImGui::SeparatorText("Save Model");

            if (ImGui::Button("Make Model JSON"))
            {
                SaveShapesToJSON(CurrentModelName, m_CurrentShapes);

                auto iter = m_ModelLoadDatas.find(CurrentModelName);

                if (iter != m_ModelLoadDatas.end())
                {
                    iter->second = m_CurrentShapes;
                }
                else
                {
                    m_ModelLoadDatas.emplace(CurrentModelName, m_CurrentShapes);
                }
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Projectile"))
        {  
            ImGui::SeparatorText("Projectile Owner");
            ImGui::RadioButton("CROSSBOW_L", &MyProjectileOwner, 0);
            ImGui::RadioButton("CROSSBOW_R", &MyProjectileOwner, 1);
            ImGui::RadioButton("STAFF",      &MyProjectileOwner, 2);
            ImGui::RadioButton("STAFF_HAND", &MyProjectileOwner, 3);
            ImGui::RadioButton("TARGET",     &MyProjectileOwner, 4);
            ImGui::RadioButton("FRONT",      &MyProjectileOwner, 5);

            ImGui::SeparatorText("Input Trigger");
            ImGui::BulletText("Track Position");  ImGui::SameLine();           

            _double	TrackPosition = 0.f;          
            if (m_iCurrentAnimationIndex != -1)
            {
                TrackPosition = m_CurrentModelAnimations[m_iCurrentAnimationIndex]->GetTrackPosition();
                ImGui::Text(to_string(TrackPosition).c_str());
            }

            if (ImGui::Button("Insert Projectile Trigger"))
            {
                if (m_iCurrentAnimationIndex != -1)
                {
                    PtInfo.fProjectileTriggerTrackPoss.push_back(TrackPosition);
                    PtInfo.eProjectileTriggerOwners.push_back(MyProjectileOwner);;
                }
            }

            if (ImGui::BeginListBox("Proj Trigger List"))
            {
                static _int iCurrentTriggerIndex{ 0 };
                _uint i = 0;
                for (auto TrackPos : PtInfo.fProjectileTriggerTrackPoss)
                {
                    const bool isSelected = (iCurrentTriggerIndex == i);

                    _int Owner = PtInfo.eProjectileTriggerOwners[i];
                    string strOwner;
                    if (Owner == 0) strOwner = "CROSSBOW_L";
                    else if (Owner == 1) strOwner = "CROSSBOW_R";
                    else if (Owner == 2) strOwner = "STAFF";
                    else if (Owner == 3) strOwner = "STAFF_HAND";
                    else if (Owner == 4) strOwner = "TARGET";
                    else if (Owner == 5) strOwner = "FRONT";

                    string strTriggerTrack = to_string(TrackPos) + " : " + strOwner ;

                    if (ImGui::Selectable(strTriggerTrack.c_str(), isSelected))
                        iCurrentTriggerIndex = i;

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                    i++;
                }

                ImGui::EndListBox();
                
                if (ImGui::Button("Delete Trigger"))
                {
                    if (PtInfo.fProjectileTriggerTrackPoss.size() != 0)
                    {
                        PtInfo.fProjectileTriggerTrackPoss.erase(PtInfo.fProjectileTriggerTrackPoss.begin() + iCurrentTriggerIndex);
                        PtInfo.eProjectileTriggerOwners.erase(PtInfo.eProjectileTriggerOwners.begin() + iCurrentTriggerIndex);
                    }
                }
            }

            if (ImGui::Button("Save This Anim Projectile"))
            {
                if (m_CurrentProjectileDatas.size() != 0)
                {
                    _bool bHaveData = { false };
                
                    for (_uint i = 0 ; i< m_CurrentProjectileDatas.size() ; ++i)
                    {
                        if ((m_CurrentProjectileDatas[i].iAnimationIndex) == (PtInfo.iAnimationIndex))
                        {
                            m_CurrentProjectileDatas[i] = PtInfo;
                            bHaveData = true;
                            break;
                        }            
                    }
                
                    if (!bHaveData)
                    {
                        m_CurrentProjectileDatas.push_back(PtInfo);
                    }
                }
                else
                    m_CurrentProjectileDatas.push_back(PtInfo);
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Sound"))
        {
            static _int iCurrentSoundIndex{ 0 };

            if (ImGui::BeginListBox("AllSound"))
            {             
                _uint i = 0;
                
                for (auto SoundData : m_vecAllSound)
                {
                    const bool isSelected = (iCurrentSoundIndex == i);
                
                    string strAnim = SoundData;
                
                    if (ImGui::Selectable(strAnim.c_str(), isSelected))
                        iCurrentSoundIndex = i;
                
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                    i++;
                }
                ImGui::EndListBox();
            }

            ImGui::Separator();

            static _float fVolume = { 0.5f } ;

            ImGui::SliderFloat(to_string(fVolume).c_str(), &fVolume, 0.f, 1.f, "%.2f");

            ImGui::Separator();

            if (ImGui::Button("Play Sound"))
            {
                GAMEINSTANCE->StopAll();
                GAMEINSTANCE->PlaySoundW(m_vecAllSound[iCurrentSoundIndex], fVolume);
            }

            ImGui::Separator();

            ImGui::BulletText("Track Position");  ImGui::SameLine();
            _double	TrackPosition = 0.f;
            if (m_iCurrentAnimationIndex != -1)
            {
                TrackPosition = m_CurrentModelAnimations[m_iCurrentAnimationIndex]->GetTrackPosition();
                ImGui::Text(to_string(TrackPosition).c_str());
            }

            ImGui::Separator();

            if (ImGui::Button("Insert Sound Trigger"))
            {
                SoundInfo stSoundInfo;
                stSoundInfo.iAnimationIndex = m_iCurrentAnimationIndex;
                stSoundInfo.strSoundName = m_vecAllSound[iCurrentSoundIndex];
                stSoundInfo.fSoundTrackPoss = TrackPosition;
                stSoundInfo.fSoundVolume = fVolume;
                m_CurrnetAnimSoundDatas.push_back(stSoundInfo);
            }

            ImGui::Separator();

            static _int iCurrentAnimSoundIndex{ 0 };

            if (ImGui::BeginListBox("AnimSound"))
            {
                _uint i = 0;

                for (auto AnimSoundData : m_CurrnetAnimSoundDatas)
                {
                    const bool isSelected = (iCurrentAnimSoundIndex == i);

                    std::ostringstream oss;
                    oss << std::fixed << std::setprecision(1) << AnimSoundData.fSoundTrackPoss;
                    std::string result = oss.str();

                    std::ostringstream Vol;
                    Vol << std::fixed << std::setprecision(1) << AnimSoundData.fSoundVolume;
                    std::string volResult = Vol.str();

                    string strAnim = AnimSoundData.strSoundName + "/" + result+ "/" + volResult;

                    if (ImGui::Selectable(strAnim.c_str(), isSelected))
                    {
                        iCurrentAnimSoundIndex = i;
                        fVolume = m_CurrnetAnimSoundDatas[iCurrentAnimSoundIndex].fSoundVolume;
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                    i++;
                }          
                ImGui::EndListBox();
            }

            ImGui::Separator();

            if (ImGui::Button("Delete Sound"))
            {
                if (m_CurrnetAnimSoundDatas.size() != 0)
                {
                    m_CurrnetAnimSoundDatas.erase(m_CurrnetAnimSoundDatas.begin() + iCurrentAnimSoundIndex);
                }
            }

            ImGui::Separator();

            if (ImGui::Button("Save Anim Sound"))
            {
                if (m_CurrnetAnimSoundDatas.size() != 0)
                {
                    string AnimName = m_CurrentModelAnimations[m_iCurrentAnimationIndex]->GetName();

                    if (m_CurrentModelSoundDatas.find(AnimName) != m_CurrentModelSoundDatas.end() )
                    {                      
                        m_CurrentModelSoundDatas.erase(AnimName);
                        m_CurrentModelSoundDatas.emplace(AnimName, m_CurrnetAnimSoundDatas);

                        
                        m_CurrentModelAnimations[m_CurrnetAnimSoundDatas[0].iAnimationIndex]->SetSoundCursor();
                        m_CurrentModelAnimations[m_CurrnetAnimSoundDatas[0].iAnimationIndex]->ResetSound();
                        m_CurrentModelAnimations[m_CurrnetAnimSoundDatas[0].iAnimationIndex]->SetIsSoundAnim();

                        for (_uint i = 0; i < m_CurrnetAnimSoundDatas.size(); ++i)
                        {
                            m_CurrentModelAnimations[m_CurrnetAnimSoundDatas[0].iAnimationIndex]->AddSound( m_CurrnetAnimSoundDatas[i].strSoundName, 
                                                                                                            m_CurrnetAnimSoundDatas[i].fSoundTrackPoss, 
                                                                                                            m_CurrnetAnimSoundDatas[i].fSoundVolume);
                        }
                    }
                    else
                    {
                        m_CurrentModelSoundDatas.emplace(AnimName, m_CurrnetAnimSoundDatas);

                        m_CurrentModelAnimations[m_CurrnetAnimSoundDatas[0].iAnimationIndex]->SetIsSoundAnim();

                        for (_uint i = 0; i < m_CurrnetAnimSoundDatas.size(); ++i)
                        {
                            m_CurrentModelAnimations[m_CurrnetAnimSoundDatas[0].iAnimationIndex]->AddSound(m_CurrnetAnimSoundDatas[i].strSoundName, 
                                                                                                            m_CurrnetAnimSoundDatas[i].fSoundTrackPoss, 
                                                                                                            m_CurrnetAnimSoundDatas[i].fSoundVolume);
                        }                      
                    }
                }
                else
                {
                    string AnimName = m_CurrentModelAnimations[m_iCurrentAnimationIndex]->GetName();
                    
                    if (m_CurrentModelSoundDatas.find(AnimName) != m_CurrentModelSoundDatas.end())
                    {
                        m_CurrentModelSoundDatas.erase(AnimName);
                                     
                        m_CurrentModelAnimations[m_iCurrentAnimationIndex]->SetSoundCursor();
                        m_CurrentModelAnimations[m_iCurrentAnimationIndex]->ResetSound();
                    }
                }
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

    //========================================================================

    ImGui::Begin("Event_List");

    ImGui::BeginTabBar("tabs");
    {
        if (ImGui::BeginTabItem("Collider"))
        {
            if (ImGui::BeginListBox("Collider_List"))
            {
                static _int iCurrentShapeIndex{ 0 };
                _uint i = 0;
                for (const auto& pair : m_CurrentShapes)
                {
                    const bool isSelected = (iCurrentShapeIndex == i);

                    _uint iIndex = pair.second.iAnimationIndex;                  
                    string strAnim = to_string(iIndex) + " : " + pair.first;

                    if (ImGui::Selectable(strAnim.c_str(), isSelected))
                        iCurrentShapeIndex = i;
                    
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                    i++;
                }

                ImGui::EndListBox();
            }

            ImGui::Separator();

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Projectile"))
        {
            if (ImGui::BeginListBox("Proj Trigger List"))
            {
                static _int iCurrentAnimIndex{ 0 };
                _uint i = 0;
                for (auto ProjData : m_CurrentProjectileDatas)
                {
                    const bool isSelected = (iCurrentAnimIndex == i);

                    _uint iIndex = ProjData.iAnimationIndex;
                    _uint iTriggerNum = ProjData.fProjectileTriggerTrackPoss.size();

                    string strAnim = to_string(iIndex) + " : " + to_string(iTriggerNum) + " EA";

                    if (ImGui::Selectable(strAnim.c_str(), isSelected))
                        iCurrentAnimIndex = i;

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                    i++;
                }
                ImGui::EndListBox();

                if (ImGui::Button("Delete Anim Trigger"))
                {
                    if (m_CurrentProjectileDatas.size() != 0)
                    {
                        m_CurrentProjectileDatas.erase(m_CurrentProjectileDatas.begin() + iCurrentAnimIndex);
                    }
                }
                if (ImGui::Button("Make Model Proj JSON"))
                {
                    SaveProjTriggersToJSON(CurrentModelName, m_CurrentProjectileDatas);
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Sound"))
        {
            if (ImGui::BeginListBox("Sound_List"))
            {
                static _int iCurrentAnimIndex{ 0 };
                static string strCurrentAnim{""};

                _uint i = 0;
                for (auto SoundData : m_CurrentModelSoundDatas)
                {
                    const bool isSelected = (iCurrentAnimIndex == i);
                
                    _uint iIndex = (SoundData.second)[0].iAnimationIndex;
                    _uint iTriggerNum = (SoundData.second).size();
                
                    string strAnim = to_string(iIndex) + " : " + to_string(iTriggerNum) + " Sounds";
                
                    if (ImGui::Selectable(strAnim.c_str(), isSelected))
                    {
                        iCurrentAnimIndex = i;
                        strCurrentAnim = SoundData.first;
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                    i++;
                }
                ImGui::EndListBox();

                ImGui::Separator();

                if (ImGui::Button("Delete Anim Sound"))
                {
                    if (m_CurrentModelSoundDatas.size() != 0)
                    {
                        m_CurrentModelSoundDatas.erase(strCurrentAnim);
                    }
                }

                ImGui::Separator();

                if (ImGui::Button("Make Model Sound JSON"))
                {
                    SaveSoundToJSON(CurrentModelName, m_CurrentModelSoundDatas);
                    MessageBox(nullptr, L"사운드 저장 성공", L"system", MB_OK);
                }
            }

            ImGui::Separator();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();

}

void CAnimToolImguiManager::ShutDown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(); 
}

bool CAnimToolImguiManager::FileExists(const wstring& filePath)
{
    filesystem::path path(filePath);
    return std::filesystem::exists(path) && filesystem::is_regular_file(path);
}

HRESULT CAnimToolImguiManager::LoadAllAnimModelName(const wstring& _strModelPath)
{
    filesystem::path basePath(_strModelPath);
    
    for (const auto& entry : filesystem::directory_iterator(basePath)) 
    {
        if (entry.is_regular_file()) 
        {
            if (".dat" == entry.path().extension()) 
            {
                string strFileName = entry.path().filename().stem().string();

                if(GAMEINSTANCE->GetModel(strFileName)->GetModelType() == CModel::TYPE_ANIM)
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


void CAnimToolImguiManager::SaveShapesToJSON(string _strModelKey, map<string, ShapeInfo> _shapes)
{
    Json::Value Model;

    for (auto pair : _shapes)
    {
        //===================================
        Json::Value ShapeInfo;

        ShapeInfo["Type"] = pair.second.iGeometryType;

        Json::Value SizeInfo;
        if (pair.second.iGeometryType == 1)
        {
            Json::Value vBoxSize(Json::arrayValue);
            vBoxSize.append(pair.second.vBoxSize.x);
            vBoxSize.append(pair.second.vBoxSize.y);
            vBoxSize.append(pair.second.vBoxSize.z);
            SizeInfo["BoxSize"] = vBoxSize;
        } 
        else if (pair.second.iGeometryType == 2)
        {
            SizeInfo["SphereRadious"] = pair.second.fSphereRadius;
        }
        else if (pair.second.iGeometryType == 3)
        {
            SizeInfo["CapsuleHeight"] = pair.second.fCapsuleHeight;
            SizeInfo["CapsuleRadius"] = pair.second.fCapsuleRadius;
        }
        ShapeInfo["Size"] = SizeInfo;

        Json::Value vOffsetPos(Json::arrayValue);
        vOffsetPos.append(pair.second.vOffSetPosition.x);
        vOffsetPos.append(pair.second.vOffSetPosition.y);
        vOffsetPos.append(pair.second.vOffSetPosition.z);
        ShapeInfo["OffSetPos"] = vOffsetPos;

        Json::Value vOffsetRot(Json::arrayValue);
        vOffsetRot.append(pair.second.vOffSetRotation.x);
        vOffsetRot.append(pair.second.vOffSetRotation.y);
        vOffsetRot.append(pair.second.vOffSetRotation.z);
        ShapeInfo["OffSetRot"] = vOffsetRot;
         
        ShapeInfo["MyFlag"] = pair.second.iMyFlag;
        ShapeInfo["TargetFlag"] = pair.second.iTargetFlag;

        //===================================
        Json::Value TrackPosition;

        TrackPosition["StartPos"] = pair.second.fColStartTrackPos;
        TrackPosition["EndPos"] = pair.second.fColEndTrackPos;

        //===================================
        Json::Value Animation;
        Animation["AnimName"] = m_vecAnimationNames[pair.second.iAnimationIndex];
        Animation["ShapeInfo"] = ShapeInfo;
        Animation["TrackPos"] = TrackPosition;

        string index = to_string(pair.second.iAnimationIndex);
        Model[index] = Animation;
    }
    std::wstring wstrModelKey(_strModelKey.begin(), _strModelKey.end());
    wstring strCompletePath = m_strSavePath + wstrModelKey + TEXT(".json");
    
    GAMEINSTANCE->WriteJson(Model, strCompletePath);
}

void CAnimToolImguiManager::SaveProjTriggersToJSON(string _strModelKey, vector<ProjectileInfo> _Triggers)
{
    Json::Value Model;

    for (auto AnimTriggers : _Triggers)
    {
        Json::Value TriggerInfo;

        Json::Value fTrackPos(Json::arrayValue);
        for (auto TrackPoss : AnimTriggers.fProjectileTriggerTrackPoss)
        {
            fTrackPos.append(TrackPoss);
        }
        TriggerInfo["TrackPos"] = fTrackPos;

        Json::Value eProjOwner(Json::arrayValue);
        for (auto ProjOwners : AnimTriggers.eProjectileTriggerOwners)
        {
            eProjOwner.append(ProjOwners);
        }
        TriggerInfo["ProjOwner"] = eProjOwner;

       string index = to_string(AnimTriggers.iAnimationIndex);

       Model[index] = TriggerInfo;
    }
    std::wstring wstrModelKey(_strModelKey.begin(), _strModelKey.end());
    wstring strCompletePath = m_strProjTriggersSavePath + wstrModelKey + TEXT("_Proj") + TEXT(".json");
    
    GAMEINSTANCE->WriteJson(Model, strCompletePath);
}

void CAnimToolImguiManager::SaveSoundToJSON(string _strModelKey, map<string, vector<SoundInfo>> _Sounds)
{
    Json::Value Model;

    for (auto AnimSound: _Sounds)
    {
        _uint jiAnimationIndex;

        Json::Value AnimSoundInfo;
    
        AnimSoundInfo["AnimName"] = AnimSound.first;

        Json::Value SoundName(Json::arrayValue);
        for (auto pSoundInfo : AnimSound.second)
        {
            string strSoundName = pSoundInfo.strSoundName;
            SoundName.append(strSoundName);
        }
        AnimSoundInfo["SoundName"] = SoundName;
    
        Json::Value TrackPos(Json::arrayValue);
        for (auto pSoundInfo : AnimSound.second)
        {
            _float fTrackPos = pSoundInfo.fSoundTrackPoss;
            TrackPos.append(fTrackPos);
        }
        AnimSoundInfo["TrackPos"] = TrackPos;

        Json::Value Volume(Json::arrayValue);
        for (auto pSoundInfo : AnimSound.second)
        {
            _float fVolume= pSoundInfo.fSoundVolume;
            Volume.append(fVolume);
        }
        AnimSoundInfo["Volume"] = Volume;

        jiAnimationIndex = AnimSound.second[0].iAnimationIndex;
    
        string index = to_string(jiAnimationIndex);
    
        Model[index] = AnimSoundInfo;
    }

    std::wstring wstrModelKey(_strModelKey.begin(), _strModelKey.end());
    wstring strCompletePath = m_strSoundSavePath + wstrModelKey + TEXT("_Sound") + TEXT(".json");

    GAMEINSTANCE->WriteJson(Model, strCompletePath);
}

void CAnimToolImguiManager::LoadAllShapesJSON()
{
    for (auto name : m_vecAnimModelName)
    {
        wstring wstrModelName(name.begin(), name.end());
        wstring strCompletePath = m_strSavePath + wstrModelName + TEXT(".json");

        Json::Value Model;

        if (!FileExists(strCompletePath))
            continue;

        Model = GAMEINSTANCE->ReadJson(strCompletePath);

        map<string, ShapeInfo> Shapes;

        auto iter = Model.begin();

        for (iter; iter != Model.end(); ++iter)
        {
            string strAnimIndex = iter.key().asString();
            _uint iAnimIndex = atoi(strAnimIndex.c_str());
            
            //_uint iAnimIndex = iter.key().asInt();

            ShapeInfo MyShapeInfo;

            MyShapeInfo.iAnimationIndex = iAnimIndex;

            MyShapeInfo.iMyFlag = Model[strAnimIndex]["ShapeInfo"]["MyFlag"].asInt();
            MyShapeInfo.iTargetFlag = Model[strAnimIndex]["ShapeInfo"]["TargetFlag"].asInt();


            MyShapeInfo.vOffSetPosition = _float3(Model[strAnimIndex]["ShapeInfo"]["OffSetPos"][0].asFloat(),
                                                    Model[strAnimIndex]["ShapeInfo"]["OffSetPos"][1].asFloat(),
                                                    Model[strAnimIndex]["ShapeInfo"]["OffSetPos"][2].asFloat());

            MyShapeInfo.vOffSetRotation = _float3(Model[strAnimIndex]["ShapeInfo"]["OffSetRot"][0].asFloat(),
                                                    Model[strAnimIndex]["ShapeInfo"]["OffSetRot"][1].asFloat(),
                                                    Model[strAnimIndex]["ShapeInfo"]["OffSetRot"][2].asFloat());

            MyShapeInfo.iGeometryType = Model[strAnimIndex]["ShapeInfo"]["Type"].asInt();

            if (MyShapeInfo.iGeometryType == 1)
            {
                MyShapeInfo.vBoxSize = _float3(Model[strAnimIndex]["ShapeInfo"]["Size"]["BoxSize"][0].asFloat(),
                                                Model[strAnimIndex]["ShapeInfo"]["Size"]["BoxSize"][1].asFloat(),
                                                Model[strAnimIndex]["ShapeInfo"]["Size"]["BoxSize"][2].asFloat());
            }
            else if (MyShapeInfo.iGeometryType == 2)
            {
                MyShapeInfo.fSphereRadius = Model[strAnimIndex]["ShapeInfo"]["Size"]["SphereRadious"].asFloat();

            }
            else if (MyShapeInfo.iGeometryType == 3)
            {
                MyShapeInfo.fCapsuleHeight = Model[strAnimIndex]["ShapeInfo"]["Size"]["CapsuleHeight"].asFloat();
                MyShapeInfo.fCapsuleRadius = Model[strAnimIndex]["ShapeInfo"]["Size"]["CapsuleRadius"].asFloat();
            }


            MyShapeInfo.fColStartTrackPos = Model[strAnimIndex]["TrackPos"]["StartPos"].asFloat();
            MyShapeInfo.fColEndTrackPos = Model[strAnimIndex]["TrackPos"]["EndPos"].asFloat();


            Shapes.emplace(Model[strAnimIndex]["AnimName"].asString(), MyShapeInfo);
        }

        string ModelName(wstrModelName.begin(), wstrModelName.end());

        m_ModelLoadDatas.emplace(ModelName, Shapes);
    }

}

void CAnimToolImguiManager::LoadAllProjTriggersJSON()
{
    for (auto name : m_vecAnimModelName)
    {
        wstring wstrModelName(name.begin(), name.end());
        wstring strCompletePath = m_strProjTriggersSavePath + wstrModelName + TEXT("_Proj") + TEXT(".json");

        Json::Value Model;

        if (!FileExists(strCompletePath))
            continue;

        Model = GAMEINSTANCE->ReadJson(strCompletePath);

        vector<ProjectileInfo> ProjTriggers;

        auto iter = Model.begin();

        for (iter; iter != Model.end(); ++iter)
        {
            string strAnimIndex = iter.key().asString();
            _uint iAnimIndex = atoi(strAnimIndex.c_str());

            ProjectileInfo MyProjInfo;

            MyProjInfo.iAnimationIndex = iAnimIndex;

            for (_uint i = 0 ; i < Model[strAnimIndex]["ProjOwner"].size(); ++i)
            {
                MyProjInfo.fProjectileTriggerTrackPoss.push_back(Model[strAnimIndex]["TrackPos"][i].asFloat());
                MyProjInfo.eProjectileTriggerOwners.push_back(Model[strAnimIndex]["ProjOwner"][i].asInt());
            }

            ProjTriggers.push_back(MyProjInfo);
        }

        string ModelName(wstrModelName.begin(), wstrModelName.end());
        m_ProjTriggerLoadDatas.emplace(ModelName, ProjTriggers);
    }
}

void CAnimToolImguiManager::LoadAllSoundJSON()
{
    for (auto name : m_vecAnimModelName)
    {
        wstring wstrModelName(name.begin(), name.end());
        wstring strCompletePath = m_strSoundSavePath + wstrModelName + TEXT("_Sound") + TEXT(".json");
    
        Json::Value Model;
    
        if (!FileExists(strCompletePath))
            continue;
    
        Model = GAMEINSTANCE->ReadJson(strCompletePath);
    
        map<string, vector<SoundInfo>>      ModelSoundDatas;

        auto iter = Model.begin();
    
        for (iter; iter != Model.end(); ++iter)
        {
            string strAnimIndex = iter.key().asString();
            _uint iAnimIndex = atoi(strAnimIndex.c_str());
    
            vector<SoundInfo> AnimSoundInfo;
            
            string strAnimName = Model[strAnimIndex]["AnimName"].asString();

            for (_uint i = 0; i < Model[strAnimIndex]["TrackPos"].size(); ++i)
            {
                SoundInfo stSoundInfo;
                
                stSoundInfo.iAnimationIndex = iAnimIndex;
                stSoundInfo.strSoundName    = Model[strAnimIndex]["SoundName"][i].asString();
                stSoundInfo.fSoundTrackPoss = Model[strAnimIndex]["TrackPos"][i].asFloat();
                stSoundInfo.fSoundVolume    = Model[strAnimIndex]["Volume"][i].asFloat();

                AnimSoundInfo.push_back(stSoundInfo);
            }
            
            ModelSoundDatas.emplace(strAnimName, AnimSoundInfo);
        }
    
        string ModelName(wstrModelName.begin(), wstrModelName.end());
        m_ModelSoundDatas.emplace(ModelName, ModelSoundDatas);
    }


}

void CAnimToolImguiManager::StoreAllSoundPath(const filesystem::path& CurrentPath, const string& FileExtension)
{
    for (const auto& entry : filesystem::directory_iterator(CurrentPath))
    {
        if (filesystem::is_regular_file(entry) && entry.path().extension() == FileExtension)
        {
            string strPath = entry.path().stem().string();

            m_vecAllSound.push_back(strPath);
        }

        else if (filesystem::is_directory(entry))
        {
            StoreAllSoundPath(entry.path(), FileExtension);
        }
    }
}

void CAnimToolImguiManager::FindAllSoundPath(const string& _strBasePath, const string& _FileExtension)
{
    filesystem::path fullPath = _strBasePath;
    StoreAllSoundPath(fullPath, _FileExtension);
}
