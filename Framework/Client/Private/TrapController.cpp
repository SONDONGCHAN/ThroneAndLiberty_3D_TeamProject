#include "pch.h"
#include "TrapController.h"
#include "RigidBody.h"
#include "CameraMgr.h"
#include "UIMgr.h"

CTrapController::CTrapController()
{
}

CTrapController::~CTrapController()
{
    Reset();
}

shared_ptr<CTrapController> CTrapController::Create(const _float3& _vPosition)
{
    shared_ptr<CTrapController> pInstance = make_shared<CTrapController>();

    if (FAILED(pInstance->Initialize(_vPosition)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CTrapController::Create", MB_OK);
        pInstance.reset();
        return nullptr;
    }

    return pInstance;
}

HRESULT CTrapController::Initialize(const _float3& _vPosition)
{
    shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

    m_pTransformCom = CTransform::Create(pGameInstance->GetDeviceInfo(), pGameInstance->GetDeviceContextInfo());
	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);

    CRigidBody::RIGIDBODYDESC RigidBodyDesc;

    RigidBodyDesc.isStatic = true;
    RigidBodyDesc.isTrigger = true;
    RigidBodyDesc.isGravity = false;
    RigidBodyDesc.isInitCol = true;
    RigidBodyDesc.pTransform = m_pTransformCom;
    RigidBodyDesc.eLock_Flag = CRigidBody::NONE;

    RigidBodyDesc.pOwnerObject = shared_from_this();
    RigidBodyDesc.fStaticFriction = 0.0f;
    RigidBodyDesc.fDynamicFriction = 0.0f;
    RigidBodyDesc.fRestitution = 0.0f;

    GeometryBox boxDesc1{};
    boxDesc1.strShapeTag = "TrapActive";
    boxDesc1.vSize = { 2.0f,5.0f,10.0f };

    RigidBodyDesc.pGeometry = &boxDesc1;

    RigidBodyDesc.eThisColFlag = COL_TRIGGER;
    RigidBodyDesc.eTargetColFlag = COL_PLAYER;
    m_pRigidBody = pGameInstance->CloneRigidBody(&RigidBodyDesc);

    GeometryBox boxDesc2{};
    boxDesc2.strShapeTag = "DoorActive";
    boxDesc2.vSize = { 2.0f,5.0f,5.0f };
    boxDesc2.vOffSetPosition = { 10.0f,0.0f,0.0f };

    RigidBodyDesc.pGeometry = &boxDesc2;

    m_pRigidBody->CreateShape(&RigidBodyDesc);

    //boxDesc2.strShapeTag = "TrapStop";
    //boxDesc2.vSize = { 2.0f,5.0f,5.0f };
    //boxDesc2.vOffSetPosition = {-51.0f,0.0f,0.0f };

    //RigidBodyDesc.pGeometry = &boxDesc2;

    //m_pRigidBody->CreateShape(&RigidBodyDesc);

    CreateDoor();

    CreateMirror();

    CreateElectric();

    CreateStone();

    return S_OK;
}

void CTrapController::PriorityTick(_float _fTimeDelta)
{
    for (auto& pMirror : m_vecMirror)
    {
        if (pMirror->IsActive())
        {
            pMirror->PriorityTick(_fTimeDelta);
        }
    }
}

void CTrapController::Tick(_float _fTimeDelta)
{
    if (m_isRun)
    {
        m_fTimeAcc += _fTimeDelta;
        m_fTimeAccElectric += _fTimeDelta;
        m_fTimeAccStone += _fTimeDelta;
        UIMGR->UpdateCountDown(m_fTimeAcc);
        if (m_fTimeAcc >= 120.0f)
        {
            UIMGR->DeactivateCountDown();
            CCameraMgr::GetInstance()->FilmCameraTurnOn("TrapDoorOpen");
            m_pTrapDoors[0]->DoorOpen();
            m_pTrapDoors[1]->DoorOpen();
            GAMEINSTANCE->PlaySoundW("TrapDoorOpen", 0.5f);
            m_isRun = false;
            m_pRigidBody->SimulationOff();
            StopElectric();
            StopMirror();
            //m_isCollision3 = false;
        }
		if (m_iElectricPhase < 11 && m_fTimeAccElectric >= 10.0f)
        {
            SpawnElectric();
            GAMEINSTANCE->PlaySoundW("SpawnElectric", 0.4f);
			memset(&m_fTimeAccElectric, 0, sizeof(float));
        }
		if (m_fTimeAcc < 115.0f && m_fTimeAccStone >= 3.5f - m_fTimeAcc * 0.022f)
        {
            SpawnStone(5.5f);
            if (m_fTimeAcc > 60.0f)
                SpawnStone(9.0f);
            GAMEINSTANCE->PlaySoundW("SpawnStone", 0.35f);
			memset(&m_fTimeAccStone, 0, sizeof(float));
        }
    }
	
    m_pTrapDoors[0]->Tick(_fTimeDelta);
    m_pTrapDoors[1]->Tick(_fTimeDelta);

    for (auto& pMirror : m_vecMirror)
    {
        if (pMirror->IsActive())
        {
            pMirror->Tick(_fTimeDelta);
        }
    }

    for (auto& pElectric : m_vecElectric)
    {
        if (pElectric->IsActive())
        {
            pElectric->Tick(_fTimeDelta);
        }
    }

    for (auto& pStone : m_vecStone)
    {
        if (pStone->IsActive())
        {
            pStone->Tick(_fTimeDelta);
        }
    }
}

void CTrapController::LateTick(_float _fTimeDelta)
{
    for (auto& pMirror : m_vecMirror)
    {
        if (pMirror->IsActive())
        {
            pMirror->LateTick(_fTimeDelta);
        }
    }

    for (auto& pElectric : m_vecElectric)
    {
        if (pElectric->IsActive())
        {
            pElectric->LateTick(_fTimeDelta);
        }
    }

    for (auto& pStone : m_vecStone)
    {
        if (pStone->IsActive())
        {
            pStone->LateTick(_fTimeDelta);
        }
    }
   
    m_pTrapDoors[0]->LateTick(_fTimeDelta);
    m_pTrapDoors[1]->LateTick(_fTimeDelta);

	if (m_isCollision1)
	{
		m_pRigidBody->DisableCollision("DoorActive");
        CCameraMgr::GetInstance()->FilmCameraTurnOn("TrapDoorShake");
		m_isCollision1 = false;
        m_pTrapDoors[0]->DoorClose();
        m_pTrapDoors[1]->DoorClose();
        GAMEINSTANCE->PlaySoundW("TrapDoorClose", 0.5f);
	}

	if (m_isCollision2)
	{
		m_pRigidBody->DisableCollision("TrapActive");
        CCameraMgr::GetInstance()->FilmCameraTurnOn("TrapRoom");
        SpawnMirror();
		m_isRun = true;
		m_isCollision2 = false;
        UIMGR->ActivateCountDown();
	}

    //if (m_isCollision3)
    //{
    //    m_pRigidBody->DisableCollision("TrapStop");
    //    m_pRigidBody->SimulationOff();
    //    m_isCollision3 = false;
    //    StopElectric();
    //    StopMirror();
    //}

#ifdef _DEBUG
    if (m_pRigidBody)
        GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG

}

HRESULT CTrapController::Render()
{
    return S_OK;
}

void CTrapController::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    if (0 == _szMyShapeTag.compare("DoorActive"))
    {
        CCameraMgr::GetInstance()->FilmCameraTurnOn("TrapDoorShake");
        m_isCollision1 = true;
        GAMEINSTANCE->PlaySoundW("TrapDoorClosing", 0.4f);
        return;
    }
    if (0 == _szMyShapeTag.compare("TrapActive"))
    {
        CCameraMgr::GetInstance()->FilmCameraTurnOn("TrapRoom");
        m_isCollision2 = true;
        GAMEINSTANCE->PlaySoundW("TrapStart", 0.5f);
        return;
    }
    //if (0 == _szMyShapeTag.compare("TrapStop"))
    //{
    //    m_isCollision3 = true;
    //    //GAMEINSTANCE->PlaySoundW("TrapStart", 0.5f);
    //    return;
    //}
}

void CTrapController::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CTrapController::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CTrapController::CreateDoor()
{
	m_pTrapDoors[0] = CTrapDoor::Create({ 43.411f, 7.5f, 103.702f });
	m_pTrapDoors[1] = CTrapDoor::Create({ 110.942f, 7.5f, 103.702f });
}

void CTrapController::CreateMirror()
{
    m_vecMirror.reserve(10);

    _float3 vColor{ 1.0f,0.0f,0.0f };
    _float4x4 WorldMatrix = XMMatrixMultiply(XMMatrixRotationY(XM_PI), XMMatrixTranslation(58.7f, 0.2f, 87.7f));
    shared_ptr<CMirror> pMirror1 = CMirror::Create(WorldMatrix, vColor, CMirror::M_FRONT);
    WorldMatrix = XMMatrixMultiply(XMMatrixRotationY(XM_PIDIV2), XMMatrixTranslation(104.2f, 0.2f, 110.6f));
    shared_ptr<CMirror> pMirror2 = CMirror::Create(WorldMatrix, vColor, CMirror::M_LEFT);

    pMirror1->LinkMirror(pMirror2);
    pMirror2->LinkMirror(pMirror1);

    m_vecMirror.emplace_back(pMirror1);
    m_vecMirror.emplace_back(pMirror2);


    vColor = { 0.0f,1.0f,0.0f };
    WorldMatrix = XMMatrixMultiply(XMMatrixRotationY(XM_PI * 1.5f), XMMatrixTranslation(50.3f, 0.2f, 110.6f));
    pMirror1 = CMirror::Create(WorldMatrix, vColor, CMirror::M_RIGHT);
    WorldMatrix = XMMatrixTranslation(95.5f, 0.2f, 119.5f);
    pMirror2 = CMirror::Create(WorldMatrix, vColor, CMirror::M_BACK);

    pMirror1->LinkMirror(pMirror2);
    pMirror2->LinkMirror(pMirror1);

    m_vecMirror.emplace_back(pMirror1);
    m_vecMirror.emplace_back(pMirror2);


    vColor = { 0.0f,0.0f,1.0f };
    WorldMatrix = XMMatrixTranslation(58.7f, 0.2f, 119.5f);
    pMirror1 = CMirror::Create(WorldMatrix, vColor, CMirror::M_BACK);
    WorldMatrix = XMMatrixMultiply(XMMatrixRotationY(XM_PI), XMMatrixTranslation(95.5f, 0.2f, 87.7f));
    pMirror2 = CMirror::Create(WorldMatrix, vColor, CMirror::M_FRONT);

    pMirror1->LinkMirror(pMirror2);
    pMirror2->LinkMirror(pMirror1);

    m_vecMirror.emplace_back(pMirror1);
    m_vecMirror.emplace_back(pMirror2);


	vColor = { 1.0f,1.0f,0.0f };
	WorldMatrix = XMMatrixTranslation(77.2f, 0.2f, 119.5f);
	pMirror1 = CMirror::Create(WorldMatrix, vColor, CMirror::M_BACK);
	WorldMatrix = XMMatrixMultiply(XMMatrixRotationY(XM_PIDIV2), XMMatrixTranslation(104.2f, 0.2f, 96.6f));
	pMirror2 = CMirror::Create(WorldMatrix, vColor, CMirror::M_LEFT);

	pMirror1->LinkMirror(pMirror2);
	pMirror2->LinkMirror(pMirror1);

	m_vecMirror.emplace_back(pMirror1);
	m_vecMirror.emplace_back(pMirror2);


    vColor = { 1.0f,0.0f,1.0f };
    WorldMatrix = XMMatrixMultiply(XMMatrixRotationY(XM_PI * 1.5f), XMMatrixTranslation(50.3f, 0.2f, 96.6f));
    pMirror1 = CMirror::Create(WorldMatrix, vColor, CMirror::M_RIGHT);
    WorldMatrix = XMMatrixMultiply(XMMatrixRotationY(XM_PI), XMMatrixTranslation(77.2f, 0.2f, 87.7f));
    pMirror2 = CMirror::Create(WorldMatrix, vColor, CMirror::M_FRONT);

    pMirror1->LinkMirror(pMirror2);
    pMirror2->LinkMirror(pMirror1);

    m_vecMirror.emplace_back(pMirror1);
    m_vecMirror.emplace_back(pMirror2);
}

void CTrapController::CreateElectric()
{
    m_vecElectric.reserve(40);
    for (UINT i = 0; i < 40; i++)
    {
        m_vecElectric.emplace_back(CTrapElectric::Create());
    }
}

void CTrapController::CreateStone()
{
    m_vecStone.reserve(16);
    for (UINT i = 0; i < 16; i++)
    {
        m_vecStone.emplace_back(CTrapStone::Create());
    }
}

void CTrapController::SpawnMirror()
{
    for (auto& pMirror : m_vecMirror)
    {
        pMirror->SpawnMirror();
    }
}

void CTrapController::SpawnElectric()
{
    shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	switch (m_iElectricPhase)
    {
    case 0:
    {
        shared_ptr<CTrapElectric> arrElectric[3]{ nullptr };
        UINT iCnt{ 0 };
		for (auto& pElectric : m_vecElectric)
        {
            if (iCnt == 3)
                break;

            if (!pElectric->IsActive())
            {
                arrElectric[iCnt] = pElectric;
                iCnt++;
            }
		}
		if (iCnt == 3)
		{
			_float4x4 WorldMatrix = XMMatrixRotationY(XM_PIDIV2) * XMMatrixTranslation(49.0f, 0.5f, 103.75f);
			for (UINT i = 0; i < 3; i++)
			{
				arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 7.2f);
                WorldMatrix.m[3][1] += 2.0f;
			}
		}
    }
    break;
    case 1:
    {
        shared_ptr<CTrapElectric> arrElectric[3]{ nullptr };
        UINT iCnt{ 0 };
        for (auto& pElectric : m_vecElectric)
        {
            if (iCnt == 3)
                break;

            if (!pElectric->IsActive())
            {
                arrElectric[iCnt] = pElectric;
                iCnt++;
            }
        }
        if (iCnt == 3)
        {
            _float4x4 WorldMatrix = XMMatrixTranslation(77.3f, 0.5f, 86.8f);
            for (UINT i = 0; i < 3; i++)
            {
                arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 4.5f);
                WorldMatrix.m[3][1] += 2.0f;
            }
        }
    }
    break;
    case 2:
    {
        shared_ptr<CTrapElectric> arrElectric[3]{ nullptr };
        UINT iCnt{ 0 };
        for (auto& pElectric : m_vecElectric)
        {
            if (iCnt == 3)
                break;

            if (!pElectric->IsActive())
            {
                arrElectric[iCnt] = pElectric;
                iCnt++;
            }
        }
        if (iCnt == 3)
        {
			_float4x4 WorldMatrix = XMMatrixRotationY(XM_PIDIV2) * XMMatrixTranslation(77.3f, 0.5f, 103.75f);
            float fSpeed{ XM_PI / 11.0f };
            for (UINT i = 0; i < 3; i++)
            {
				arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_SPIN, WorldMatrix, fSpeed);
                WorldMatrix.m[3][1] += 2.0f;
            }
        }
    }
    break;
    case 3:
    {
        shared_ptr<CTrapElectric> arrElectric[10]{ nullptr };
        UINT iCnt{ 0 };
        for (auto& pElectric : m_vecElectric)
        {
            if (iCnt == 10)
                break;

            if (!pElectric->IsActive())
            {
                arrElectric[iCnt] = pElectric;
                iCnt++;
            }
        }
        if (iCnt == 10)
        {
			_float4x4 WorldMatrix;
            WorldMatrix.Right({0.0f,1.0f,0.0f});
            WorldMatrix.Up({ 0.0f,0.0f,-1.0f });
            WorldMatrix.Forward({ -1.0f,0.0f,0.0f });
            WorldMatrix.Translation({ 105.6f, 10.0f, 92.575f });

            for (UINT i = 0; i < 5; i++)
            {
				arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 7.2f);
                WorldMatrix.m[3][2] += 1.0f;
            }
            WorldMatrix.m[3][2] = 110.925f;
            for (UINT i = 5; i < 10; i++)
            {
                arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 7.2f);
                WorldMatrix.m[3][2] += 1.0f;
            }
        }
    }
    break;
    case 4:
    {
        shared_ptr<CTrapElectric> arrElectric[4]{ nullptr };
        UINT iCnt{ 0 };
        for (auto& pElectric : m_vecElectric)
        {
            if (iCnt == 4)
                break;

            if (!pElectric->IsActive())
            {
                arrElectric[iCnt] = pElectric;
                iCnt++;
            }
        }
        if (iCnt == 4)
        {
			_float4x4 WorldMatrix = XMMatrixRotationY(XM_PI) * XMMatrixTranslation(77.3f, 0.5f, 120.7f);
            for (UINT i = 0; i < 3; i++)
            {
                arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 4.5f);
                WorldMatrix.m[3][1] += 2.0f;
            }
			WorldMatrix = XMMatrixRotationY(XM_PIDIV2) * XMMatrixTranslation(49.0f, 0.5f, 103.75f);
            arrElectric[3]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 10.0f);
        }
    }
    break;
    case 5:
    {
        shared_ptr<CTrapElectric> arrElectric[10]{ nullptr };
        UINT iCnt{ 0 };
        for (auto& pElectric : m_vecElectric)
        {
            if (iCnt == 10)
                break;

            if (!pElectric->IsActive())
            {
                arrElectric[iCnt] = pElectric;
                iCnt++;
            }
        }
        if (iCnt == 10)
        {
            _float4x4 WorldMatrix;
            WorldMatrix.Right({ 0.0f,1.0f,0.0f });
            WorldMatrix.Up({ 0.0f,0.0f,-1.0f });
            WorldMatrix.Forward({ -1.0f,0.0f,0.0f });
            WorldMatrix.Translation({ 105.6f, 10.0f, 85.8f });
            for (UINT i = 0; i < 10; i++)
            {
                arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 7.2f);
                WorldMatrix.m[3][2] += 3.6f;
            }
        }
    }
    break;
    case 6:
    {
        shared_ptr<CTrapElectric> arrElectric[6]{ nullptr };
        UINT iCnt{ 0 };
        for (auto& pElectric : m_vecElectric)
        {
            if (iCnt == 6)
                break;

            if (!pElectric->IsActive())
            {
                arrElectric[iCnt] = pElectric;
                iCnt++;
            }
        }
        if (iCnt == 6)
        {
            _float4x4 WorldMatrix = XMMatrixRotationY(XM_PIDIV2) * XMMatrixTranslation(77.3f, 0.5f, 103.75f);
			float fSpeed{ XM_PI / 12.0f };
            for (UINT i = 0; i < 3; i++)
            {
                arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_SPIN, WorldMatrix, fSpeed);
                WorldMatrix.m[3][1] += 2.0f;
            }
            WorldMatrix = XMMatrixTranslation(77.3f, 0.5f, 103.75f);;
            for (UINT i = 3; i < 6; i++)
            {
                arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_SPIN, WorldMatrix, fSpeed);
                WorldMatrix.m[3][1] += 2.0f;
            }
        }
    }
    break;
    case 7:
    {
        shared_ptr<CTrapElectric> arrElectric[9]{ nullptr };
        UINT iCnt{ 0 };
        for (auto& pElectric : m_vecElectric)
        {
            if (iCnt == 9)
                break;

            if (!pElectric->IsActive())
            {
                arrElectric[iCnt] = pElectric;
                iCnt++;
            }
        }
        if (iCnt == 9)
        {
            _float4x4 WorldMatrix = XMMatrixTranslation(77.3f, 0.5f, 86.8f);
            arrElectric[0]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 4.5f);
			WorldMatrix = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(49.0f, 10.0f, 86.8f);
            for (UINT i = 1; i < 9; i++)
            {
                arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 4.5f);
                WorldMatrix.m[3][0] += 8.27f;
            }
        }
    }
    break;
    case 8:
    {
        shared_ptr<CTrapElectric> arrElectric[8]{ nullptr };
        UINT iCnt{ 0 };
        for (auto& pElectric : m_vecElectric)
        {
            if (iCnt == 8)
                break;

            if (!pElectric->IsActive())
            {
                arrElectric[iCnt] = pElectric;
                iCnt++;
            }
		}
		if (iCnt == 8)
		{
			_float4x4 WorldMatrix = XMMatrixRotationY(XM_PI) * XMMatrixTranslation(77.3f, 0.5f, 120.7f);
			for (UINT i = 0; i < 3; i++)
			{
				arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 4.5f);
				WorldMatrix.m[3][1] += 2.0f;
			}
            WorldMatrix.Right({ 0.0f,1.0f,0.0f });
            WorldMatrix.Up({ 0.0f,0.0f,-1.0f });
            WorldMatrix.Forward({ -1.0f,0.0f,0.0f });
            WorldMatrix.Translation({ 105.6f, 10.0f, 91.516f });
			for (UINT i = 3; i < 8; i++)
			{
				arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 10.2f);
                WorldMatrix.m[3][2] += 6.117f;
			}
		}
    }
    break;
	case 9:
	{
		shared_ptr<CTrapElectric> arrElectric[13]{ nullptr };
		UINT iCnt{ 0 };
		for (auto& pElectric : m_vecElectric)
		{
			if (iCnt == 13)
				break;

            if (!pElectric->IsActive())
            {
                arrElectric[iCnt] = pElectric;
                iCnt++;
            }
		}
		if (iCnt == 13)
		{
            _float4x4 WorldMatrix;
            WorldMatrix.Right({ 0.0f,1.0f,0.0f });
            WorldMatrix.Up({ 1.0f,0.0f,0.0f });
            WorldMatrix.Forward({ 0.0f,0.0f,-1.0f });
            WorldMatrix.Translation({ 48.0f, 10.0f, 120.7f });
			for (UINT i = 0; i < 8; i++)
			{
				arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 4.5f);
				WorldMatrix.m[3][0] += 6.51f;
			}
			WorldMatrix.Right({ 0.0f,1.0f,0.0f });
			WorldMatrix.Up({ 0.0f,0.0f,1.0f });
			WorldMatrix.Forward({ 1.0f,0.0f,0.0f });
			WorldMatrix.Translation({ 49.0f, 10.0f, 91.516f });
			for (UINT i = 8; i < 13; i++)
			{
				arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 7.2f);
                WorldMatrix.m[3][2] += 6.117f;
            }
        }
    }
    break;
    case 10:
    {
        shared_ptr<CTrapElectric> arrElectric[9]{ nullptr };
        UINT iCnt{ 0 };
        for (auto& pElectric : m_vecElectric)
        {
            if (iCnt == 9)
                break;

            if (!pElectric->IsActive())
            {
                arrElectric[iCnt] = pElectric;
                iCnt++;
            }
        }
		if (iCnt == 9)
		{
            _float4x4 WorldMatrix = XMMatrixRotationY(XM_PI) * XMMatrixTranslation(77.3f, 0.5f, 120.7f);
			for (UINT i = 0; i < 9; i++)
			{
				arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 0.77f);
				WorldMatrix.m[3][1] += 2.0f;
			}
			WorldMatrix = XMMatrixRotationY(XM_PI * 1.5f) * XMMatrixTranslation(105.6f, 0.5f, 103.75f);
			for (UINT i = 3; i < 6; i++)
			{
				arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 2.3f);
				WorldMatrix.m[3][1] += 2.0f;
			}
			WorldMatrix = XMMatrixTranslation(77.3f, 0.5f, 86.8f);
			for (UINT i = 6; i < 9; i++)
			{
				arrElectric[i]->SettingElectric(CTrapElectric::EELECTRICMODE::ELEC_GO, WorldMatrix, 0.77f);
				WorldMatrix.m[3][1] += 2.0f;;
			}
		}
    }
    break;
    }
    m_iElectricPhase++;
}

void CTrapController::SpawnStone(float _fSpeed)
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	_float3 vPos{ pGameInstance->PickRandomFloat(50.0f,104.6f),20.0f,pGameInstance->PickRandomFloat(87.8f,119.7f) };

	for (auto& pStone : m_vecStone)
	{
		if (!pStone->IsActive())
		{
			pStone->SettingStone(vPos, _fSpeed);
			break;
		}
	}
}

void CTrapController::StopMirror()
{
    for (auto& pMirror : m_vecMirror)
    {
        pMirror->DespawnMiorror();
    }
}

void CTrapController::StopElectric()
{
    for (auto& pElectric : m_vecElectric)
    {
        pElectric->TrapStop();
    }
}

void CTrapController::Reset()
{
    m_pTrapDoors[0].reset();
    m_pTrapDoors[1].reset();
    m_vecMirror.clear();
    m_vecElectric.clear();
    m_vecStone.clear();
}

