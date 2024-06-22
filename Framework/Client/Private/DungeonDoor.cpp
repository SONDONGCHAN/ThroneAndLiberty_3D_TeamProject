#include "DungeonDoor.h"
#include "GameInstance.h"

#include "Model.h"

#include "EnterStatue.h"
#include "CameraMgr.h"

CDungeonDoor::CDungeonDoor()
{
}

HRESULT CDungeonDoor::Initialize()
{

    CTransform::TRANSFORM_DESC desc;
    desc.fRotationPerSec = -0.6f;
    desc.fSpeedPerSec = 0.f;

    __super::Initialize(&desc);

    desc.fRotationPerSec = 0.6f;

    m_pDoorModel1 = GAMEINSTANCE->GetModel("BG_OneDoor_13_01_SM");
    m_pDoorModel2 = GAMEINSTANCE->GetModel("BG_OneDoor_13_02_SM");
    m_pDoorLockModel = GAMEINSTANCE->GetModel("BG_Monument_15_01_SM");

    m_pDoorTransform2 = CTransform::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), &desc);
    m_pDoorLockTransform = CTransform::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), nullptr);

    m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(496.f, -69.f, 350.98f));
    m_pDoorTransform2->SetState(CTransform::STATE_POSITION, _float3(496.f, -69.f, 358.34f));
    
    m_pDoorLockTransform->SetState(CTransform::STATE_POSITION, _float3(495.5f, -63.5f, 354.74f));
    m_pDoorLockTransform->SetScaling(_float3(3.f, 3.f, 3.f));

    m_iShaderPass = 0;
    m_eShaderType = ST_NONANIM;


    /*Create EnterStatue*/
 
    _float3 vStatuePos[6] = { {464.01f, -67.f, 328.09f},
                              {431.81f, -67.f, 342.79f} ,
                              {453.78f, -67.f, 374.14f} ,
                              {446.98f, -67.f, 350.98f} ,
                              {477.24f, -67.f, 369.2f},
                              {485.04f, -67.f, 354.89f} };


    //135.f
    _float fAnswerDegree[6] = {295.f, 35.5f, 198.f, 59.f, 150.f , 90.4f };

    //vStatuePos
    for (_int i = 0; i < 6; ++i) {

        shared_ptr<CEnterStatue> pEnterStatue = CEnterStatue::Create(vStatuePos[i], fAnswerDegree[i], i);
        GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_InteractionObj"), pEnterStatue);
      //  GAMEINSTANCE->AddStaticObject(pEnterStatue, _float3(vStatuePos[i]), 80.f);
        m_Statues.push_back(pEnterStatue);
        m_Statues[i].lock()->SetDungeonDoor(shared_from_this());
    }

    //SetDungeonDoor

    return S_OK;
}

void CDungeonDoor::PriorityTick(_float _fTimeDelta)
{
}

void CDungeonDoor::Tick(_float _fTimeDelta)
{

    if (!m_IsOpenDone && m_IsDoorOpen && !m_bDslv) {
        if (!m_IsSoundTriggerOn) {

            GAMEINSTANCE->PlaySoundW("SFX_DungeonDoorClosed", 1.4f);
            ROT_DESC RotDesc = CCameraMgr::GetInstance()->RotSetting({ 0.02f, 0.04f, 0.0f }, { 8.0f,14.0f,0.0f }, { 0.0f, 2.f });
            CCameraMgr::GetInstance()->CameraShake(SHAKE_ROT, 2.3f, &RotDesc);
            //소리 
            //카메라 셰이킹

            m_IsSoundTriggerOn = true;
        }


        DoorOpen(_fTimeDelta);

    }

    if (m_IsDoorOpen)
        return;

    _bool IsAllEnergyOn = true;

    for (auto& iter : m_Statues) {
        if (!iter.lock()->IsConnected()) {
            IsAllEnergyOn = false;
            break;
        }
    }

    if (IsAllEnergyOn) {
        /*던전 문 열리기*/
        m_IsDoorOpen = true;
        CCameraMgr::GetInstance()->CameraTurnOn("StaticCamera", _float3(487.5f, -63.5f, 354.74f), _float3(495.5f, -63.5f, 354.74f));
        m_bDslv = true;

        GAMEINSTANCE->PlaySoundW("SFX_DungeonLuneBreak", 1.f);

        SetDissolve(true, true, 1.5f);
        SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");

    }



}

void CDungeonDoor::LateTick(_float _fTimeDelta)
{
    if (m_bDslv)
        UpdateDslv(0.016f);



    GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

}

HRESULT CDungeonDoor::Render()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    /*Door1*/

    _uint iNumMeshes = m_pDoorModel1->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++) {

        GAMEINSTANCE->BeginNonAnimModel(m_pDoorModel1, (_uint)i);
    }


    if (m_bDslv)
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &m_bDslv, sizeof(_bool))))
            return E_FAIL;

        _float g_fDslvValue = m_fDslvTime / m_fMaxDslvTime;


        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDslvValue", &g_fDslvValue, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDslvThick", &m_fDslvThick, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_vDslvColor", &m_vDslvColor, sizeof(_float4))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DslvTexture", m_strDslvTexKey)))
            return E_FAIL;
    }


    if (!m_IsDoorOpen || (m_IsDoorOpen && m_bDslv)) {
        /*DoorLock*/
        GAMEINSTANCE->BindWVPMatrixPerspective(m_pDoorLockTransform->GetWorldMatrix());
        _uint iNumMeshes3 = m_pDoorLockModel->GetNumMeshes();

        for (size_t i = 0; i < iNumMeshes3; i++) {

            GAMEINSTANCE->BeginNonAnimModel(m_pDoorLockModel, (_uint)i);
        }
    }

    if (m_bDslv)
    {
        bool bDslvFalse = false;
        if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &bDslvFalse, sizeof(bool))))
            return E_FAIL;
    }

    GAMEINSTANCE->BindWVPMatrixPerspective(m_pDoorTransform2->GetWorldMatrix());

    /*Door2*/
    _uint iNumMeshes2 = m_pDoorModel1->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes2; i++) {

        GAMEINSTANCE->BeginNonAnimModel(m_pDoorModel2, (_uint)i);

    }


    return S_OK;
}

void CDungeonDoor::EnergyConnect(_int _iNextIdx)
{
    if (_iNextIdx >= m_Statues.size())
        return;

    m_Statues[_iNextIdx].lock()->SetEnergyOn(true);

}

void CDungeonDoor::EnergyDisConnect(_int _iNextIdx)
{

    if (_iNextIdx >= m_Statues.size())
        return;

    m_Statues[_iNextIdx].lock()->SetEnergyOn(false);

}

void CDungeonDoor::DoorOpen(_float _fTimeDelta)
{

    m_fDoorOpenTime += 0.016f;

    if (m_fDoorOpenTime <= 2.3f) {
        m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), _fTimeDelta);
        m_pDoorTransform2->Turn(_float3(0.f, 1.f, 0.f), _fTimeDelta);
    }

    if (m_fDoorOpenTime >= 4.f) {
        m_IsOpenDone = true;

        GAMEINSTANCE->LevelBGMOff();
        GAMEINSTANCE->PlayBGM("AMB_DungeonEntrance", 1.2f);
        GAMEINSTANCE->PlaySoundW("SFX_DungeonEnterOpen", 1.f);
        CCameraMgr::GetInstance()->CameraTurnOn("PlayerCamera");
    }
}

shared_ptr<CDungeonDoor> CDungeonDoor::Create()
{
    shared_ptr<CDungeonDoor> pDoor = make_shared<CDungeonDoor>();

    if (FAILED(pDoor->Initialize()))
        MSG_BOX("Failed to Create : CDungeonDoor");

    return pDoor;
}
