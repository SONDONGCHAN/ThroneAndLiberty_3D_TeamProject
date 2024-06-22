#include "pch.h"
#include "Amitoy.h"
#include "Model.h"
#include "Animation.h"
#include "RigidBody.h"
#include "CameraMgr.h"
#include "CameraSpherical.h"
#include "CameraFilm.h"
#include "EffectMgr.h"
#include "QuestMgr.h"

CAmitoy::CAmitoy()
	: CGameObject()
{
}

CAmitoy::~CAmitoy()
{
}

shared_ptr<CAmitoy> CAmitoy::Create()
{
	shared_ptr<CAmitoy> pInstance = make_shared<CAmitoy>();

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CAmitoy::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CAmitoy::Initialize()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	m_pTransformCom = CTransform::Create(pGameInstance->GetDeviceInfo(), pGameInstance->GetDeviceContextInfo());
	m_pTransformCom->SetSpeed(1.5f);
	m_pTransformCom->SetState(CTransform::STATE_POSITION, { 48.435f,-10.0f,332.547f });
	m_pTransformCom->Tick(0.0f);

	string ModelTag = "Amitoy";
	m_pModelCom = pGameInstance->GetModel(ModelTag);
	m_pModelCom->ChangeAnim(ACT_IDLE, 0.1f, true);

	AddComponent(L"Com_Model", m_pModelCom);

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_ANIM;
	m_iShaderPass = 0;

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = true;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestitution = 0.0f;

	GeometrySphere SphereDesc;
	SphereDesc.fRadius = 0.5f;
	SphereDesc.strShapeTag = "Amitoy_Body";
	SphereDesc.vOffSetPosition = { 0.0f,0.5f,0.0f };
	RigidBodyDesc.pGeometry = &SphereDesc;

	RigidBodyDesc.eThisColFlag = COL_AMITOY;
	RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_NPC | COL_TRAP;
	m_pRigidBody = pGameInstance->CloneRigidBody(&RigidBodyDesc);
	m_pRigidBody->SetMass(3.0f);

	m_pTransformCom->SetRigidBody(m_pRigidBody);

	m_szIdleSoundTag[0] = "AmitoyIdle1";
	m_szIdleSoundTag[1] = "AmitoyIdle2";
	m_szIdleSoundTag[2] = "AmitoyIdle3";
	m_fSoundWeight[0] = 0.3333333f;
	m_fSoundWeight[1] = 0.3333333f;
	m_fSoundWeight[2] = 0.3333333f;


	GeometrySphere GrabSphere;
	GrabSphere.fRadius = 0.2f;
	GrabSphere.strShapeTag = "AmitoyHands";
	GrabSphere.vOffSetPosition = { 0.0f,0.5f,0.5f };

	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.isInitCol = false;
	RigidBodyDesc.pGeometry = &GrabSphere;

	RigidBodyDesc.eThisColFlag = COL_AMITOY;
	RigidBodyDesc.eTargetColFlag = COL_STATIC;

	m_pRigidBody->CreateShape(&RigidBodyDesc);

	if (FAILED(ReadyJsonShape()))
		return E_FAIL;

	if (FAILED(ReadyCamera()))
		return E_FAIL;

	m_pRigidBody->SimulationOff();
	m_IsActive = false;

	m_pPlayer = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));

	return S_OK;
}

void CAmitoy::PriorityTick(_float _fTimeDelta)
{
	if (m_isInteraction && m_pModelCom->GetIsFinishedAnimation())
	{
		m_isInteraction = false;
	}
}

void CAmitoy::Tick(_float _fTimeDelta)
{
	KeyInput(_fTimeDelta);
	/*if (m_fIdleTime > 3.0f)
	{
		if (GetRandomChance(m_fSoundWeight[0]))
		{
			GAMEINSTANCE->PlaySoundW(m_szIdleSoundTag[0], 0.35f);
			m_fSoundWeight[0] -= 0.15f;
			m_fSoundWeight[1] += 0.075f;
			m_fSoundWeight[2] += 0.075f;
		}
		else if (GetRandomChance(m_fSoundWeight[1]))
		{
			GAMEINSTANCE->PlaySoundW(m_szIdleSoundTag[1], 0.35f);
			m_fSoundWeight[0] += 0.075f;
			m_fSoundWeight[1] -= 0.15f;
			m_fSoundWeight[2] += 0.075f;
		}
		else if (GetRandomChance(m_fSoundWeight[2]))
		{
			GAMEINSTANCE->PlaySoundW(m_szIdleSoundTag[2], 0.35f);
			m_fSoundWeight[0] += 0.075f;
			m_fSoundWeight[1] += 0.075f;
			m_fSoundWeight[2] -= 0.15f;
		}
		memset(&m_fIdleTime, 0, sizeof(float));
	}*/
	if (m_isAllAcquired)
	{
		if (XMVector3Length(m_pPlayer.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) - m_pTransformCom->GetState(CTransform::STATE_POSITION)).m128_f32[0] < 3.0f)
		{
			QUESTMGR->ActiveEventTag(QUEST_SUB1, "Event_FindMinigame");
			m_IsEnabled = false;
		}
	}

	m_pModelCom->PlayAnimation(_fTimeDelta);
	m_pTransformCom->Tick(_fTimeDelta);
}

void CAmitoy::LateTick(_float _fTimeDelta)
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	m_pModelCom->CheckDisableAnim();
	m_pTransformCom->LateTickFragment(_fTimeDelta);

	if (m_isAttactObject && m_isInteraction)
	{
		pGameInstance->PlaySoundW("FlyAwayObject", 0.4f);
		m_isAttactObject = false;
	}   

	if (m_isUsingGrab)
	{
		m_pRigidBody->DisableCollision("AmitoyHands");
		m_isUsingGrab = false;
	}

	if (m_isAttactedBoom)
	{
		m_pModelCom->ChangeAnim(ACT_STUN, 0.1f, false);
		pGameInstance->PlaySoundW("AmitoyStun", 0.5f);
		m_isAttactedBoom = false;
		m_isUsingGrab = false;
		m_pRigidBody->DisableCollision("AmitoyHands");
		EFFECTMGR->PlayEffect("Amitoy_Stun", shared_from_this());
	}

	// frustrum
	if (pGameInstance->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 2.f))
	{
		pGameInstance->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
		GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody)
			pGameInstance->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CAmitoy::Render()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	UINT iNumMeshes = m_pModelCom->GetNumMeshes();

	pGameInstance->ReadyShader(m_eShaderType, m_iShaderPass);

	for (UINT i = 0; i < iNumMeshes; i++)
	{
		pGameInstance->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
		pGameInstance->BeginAnimModel(m_pModelCom, i);
	}

	return S_OK;
}

HRESULT CAmitoy::RenderShadow()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 3);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
	GAMEINSTANCE->BindLightProjMatrix();

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (UINT i = 0; i < iNumMeshes; i++) {

		GAMEINSTANCE->BeginAnimModel(m_pModelCom, i);

	}
	return S_OK;
}

void CAmitoy::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (0 == strcmp(_szMyShapeTag.c_str(), "MD_IceGolem_LV5_SK.ao|MD_IceGolem_Und_act_Heal") && m_isInteraction)
	{
		m_isAttactObject = true;
	}

	if (_ColData.eColFlag & COL_TRAP)
	{
		m_isAttactedBoom = true;
	}
}

void CAmitoy::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CAmitoy::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CAmitoy::EnableCollision(const char* _strShapeTag)
{
	m_pRigidBody->EnableCollision(_strShapeTag);
}

void CAmitoy::DisableCollision(const char* _strShapeTag)
{
	m_pRigidBody->DisableCollision(_strShapeTag);
}

void CAmitoy::SpawnAmitoy()
{
	shared_ptr<CTransform> pPlayerTransform = m_pPlayer.lock()->GetTransform();
	m_pTransformCom->SetState(CTransform::STATE_POSITION, pPlayerTransform->GetState(CTransform::STATE_POSITION) + pPlayerTransform->GetState(CTransform::STATE_RIGHT));
	m_pTransformCom->Tick(0.0f);
	m_IsActive = true;
	m_pRigidBody->SimulationOn();
	CCameraMgr::GetInstance()->CameraTurnOn("AmitoyCamera");
}

void CAmitoy::DespawnAmitoy()
{
	m_IsActive = false;
	m_pRigidBody->SimulationOff();
	CCameraMgr::GetInstance()->CameraTurnOn("PlayerCamera", 0.5f, 0.1f);
}

void CAmitoy::AllAcquired()
{
	m_isAllAcquired = true;
}

void CAmitoy::KeyInput(float _fTimeDelta)
{
	if (!m_isInteraction)
	{
		shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

		// Skill Control
		if (pGameInstance->KeyDown(DIK_F) && !m_isUsingGrab)
		{
			m_pModelCom->ChangeAnim(ACT_GETITEM, 0.1f, false);
			m_isInteraction = true;
			CCameraMgr::GetInstance()->FilmCameraTurnOn("AmitoyGetItem", m_pTransformCom->GetState(CTransform::STATE_LOOK), m_pTransformCom->GetState(CTransform::STATE_POSITION), false);
			return;
		}
		if (pGameInstance->KeyDown(DIK_SPACE) && !m_isUsingGrab)
		{
			m_pModelCom->ChangeAnim(ACT_ATTACK, 0.1f, false);
			EFFECTMGR->PlayEffect("Amitoy_Attack2", shared_from_this());
			m_isInteraction = true;
			return;
		}
		if (pGameInstance->KeyDown(DIK_E))
		{
			m_pTransformCom->Jump(1100.0f);
			pGameInstance->PlaySoundW("AmitoyJump", 0.3f);
		}

		if (pGameInstance->MouseDown(DIM_LB))
		{
			m_isUsingGrab = true;
			m_pRigidBody->EnableCollision("AmitoyHands");
		}

		UINT KeyFlag{ 0 };

		KeyFlag |= pGameInstance->KeyPressing(DIK_W) ? 1 : 0;
		KeyFlag |= pGameInstance->KeyPressing(DIK_S) ? 2 : 0;
		KeyFlag |= pGameInstance->KeyPressing(DIK_A) ? 4 : 0;
		KeyFlag |= pGameInstance->KeyPressing(DIK_D) ? 8 : 0;

		KeyFlag |= pGameInstance->KeyPressing(DIK_LSHIFT) ? 16 : 0;

		// Move Control
		switch (KeyFlag)
		{
			// Walk
		case 1:
		{
			m_pTransformCom->LookAtDirForLandObject(pGameInstance->GetCamLook());
			m_pModelCom->ChangeAnim(ACT_WALK, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 2:
		{
			m_pTransformCom->LookAtDirForLandObject(-pGameInstance->GetCamLook());
			m_pModelCom->ChangeAnim(ACT_WALK, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 4:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 1.5f)));
			m_pModelCom->ChangeAnim(ACT_WALK, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 5:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 1.75f)));
			m_pModelCom->ChangeAnim(ACT_WALK, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 6:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 1.25f)));
			m_pModelCom->ChangeAnim(ACT_WALK, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 7:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 1.5f)));
			m_pModelCom->ChangeAnim(ACT_WALK, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 8:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 0.5f)));
			m_pModelCom->ChangeAnim(ACT_WALK, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 9:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 0.25f)));
			m_pModelCom->ChangeAnim(ACT_WALK, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 10:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 0.75f)));
			m_pModelCom->ChangeAnim(ACT_WALK, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 11:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 0.5f)));
			m_pModelCom->ChangeAnim(ACT_WALK, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 13:
		{
			m_pTransformCom->LookAtDirForLandObject(pGameInstance->GetCamLook());
			m_pModelCom->ChangeAnim(ACT_WALK, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 14:
		{
			m_pTransformCom->LookAtDirForLandObject(-pGameInstance->GetCamLook());
			m_pModelCom->ChangeAnim(ACT_WALK, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		// Run
		case 17:
		{
			m_pTransformCom->LookAtDirForLandObject(pGameInstance->GetCamLook());
			m_pModelCom->ChangeAnim(ACT_RUN, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta * 2.0f);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 18:
		{
			m_pTransformCom->LookAtDirForLandObject(-pGameInstance->GetCamLook());
			m_pModelCom->ChangeAnim(ACT_RUN, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta * 2.0f);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 20:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 1.5f)));
			m_pModelCom->ChangeAnim(ACT_RUN, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta * 2.0f);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 21:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 1.75f)));
			m_pModelCom->ChangeAnim(ACT_RUN, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta * 2.0f);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 22:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 1.25f)));
			m_pModelCom->ChangeAnim(ACT_RUN, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta * 2.0f);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 23:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 1.5f)));
			m_pModelCom->ChangeAnim(ACT_RUN, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta * 2.0f);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 24:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 0.5f)));
			m_pModelCom->ChangeAnim(ACT_RUN, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta * 2.0f);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 25:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 0.25f)));
			m_pModelCom->ChangeAnim(ACT_RUN, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta * 2.0f);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 26:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 0.75f)));
			m_pModelCom->ChangeAnim(ACT_RUN, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta * 2.0f);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 27:
		{
			m_pTransformCom->LookAtDirForLandObject(XMVector3TransformNormal(pGameInstance->GetCamLook(), XMMatrixRotationY(XM_PI * 0.5f)));
			m_pModelCom->ChangeAnim(ACT_RUN, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta * 2.0f);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 29:
		{
			m_pTransformCom->LookAtDirForLandObject(pGameInstance->GetCamLook());
			m_pModelCom->ChangeAnim(ACT_RUN, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta * 2.0f);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		case 30:
		{
			m_pTransformCom->LookAtDirForLandObject(-pGameInstance->GetCamLook());
			m_pModelCom->ChangeAnim(ACT_RUN, 0.1f, true);
			m_pTransformCom->GoStraight(_fTimeDelta * 2.0f);
			memset(&m_fIdleTime, 0, sizeof(float));
		}
		break;
		default:
		{
			m_pModelCom->ChangeAnim(ACT_IDLE, 0.1f, true);
			m_fIdleTime += _fTimeDelta;
		}
		break;
		}

	}
}

HRESULT CAmitoy::ReadyJsonShape()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.pOwnerObject = shared_from_this();

	string ModelTag = "Amitoy";
	wstring wstrModelName(ModelTag.begin(), ModelTag.end());
	wstring strCompletePath = L"..\\..\\Tool_Animation\\Bin\\DataFiles\\" + wstrModelName + TEXT(".json");

	Json::Value Model;

	filesystem::path path(strCompletePath);
	if (!(filesystem::exists(path) && filesystem::is_regular_file(path)))
		return S_OK;

	Model = GAMEINSTANCE->ReadJson(strCompletePath);

	auto iter = Model.begin();
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.isInitCol = false;

	GeometryBox		TriggerBoxDesc;
	GeometrySphere  TriggerSphereDesc;
	GeometryCapsule TriggerCapsuleDesc;

	for (iter; iter != Model.end(); ++iter)
	{
		string strAnimIndex = iter.key().asString();
		_uint iAnimIndex = atoi(strAnimIndex.c_str());

		_float3 vOffsetPos = { Model[strAnimIndex]["ShapeInfo"]["OffSetPos"][0].asFloat(),
								Model[strAnimIndex]["ShapeInfo"]["OffSetPos"][1].asFloat(),
								Model[strAnimIndex]["ShapeInfo"]["OffSetPos"][2].asFloat() };
		_float3 vOffsetRot = { Model[strAnimIndex]["ShapeInfo"]["OffSetRot"][0].asFloat(),
								Model[strAnimIndex]["ShapeInfo"]["OffSetRot"][1].asFloat(),
								Model[strAnimIndex]["ShapeInfo"]["OffSetRot"][2].asFloat() };

		const char* strShapeTag = Model[strAnimIndex]["AnimName"].asCString();

		_uint iGeoType = Model[strAnimIndex]["ShapeInfo"]["Type"].asInt();

		if (iGeoType == 1)
		{
			TriggerBoxDesc.vSize = _float3(Model[strAnimIndex]["ShapeInfo"]["Size"]["BoxSize"][0].asFloat(),
				Model[strAnimIndex]["ShapeInfo"]["Size"]["BoxSize"][1].asFloat(),
				Model[strAnimIndex]["ShapeInfo"]["Size"]["BoxSize"][2].asFloat());

			TriggerBoxDesc.strShapeTag = strShapeTag;

			TriggerBoxDesc.vOffSetPosition = vOffsetPos;
			TriggerBoxDesc.vOffSetRotation = vOffsetRot;

			RigidBodyDesc.pGeometry = &TriggerBoxDesc;
		}

		else if (iGeoType == 2)
		{
			TriggerSphereDesc.fRadius = Model[strAnimIndex]["ShapeInfo"]["Size"]["SphereRadious"].asFloat();

			TriggerSphereDesc.strShapeTag = strShapeTag;

			TriggerSphereDesc.vOffSetPosition = vOffsetPos;
			TriggerSphereDesc.vOffSetRotation = vOffsetRot;

			RigidBodyDesc.pGeometry = &TriggerSphereDesc;
		}

		else if (iGeoType == 3)
		{
			TriggerCapsuleDesc.fHeight = Model[strAnimIndex]["ShapeInfo"]["Size"]["CapsuleHeight"].asFloat();
			TriggerCapsuleDesc.fRadius = Model[strAnimIndex]["ShapeInfo"]["Size"]["CapsuleRadius"].asFloat();

			TriggerCapsuleDesc.strShapeTag = strShapeTag;

			TriggerCapsuleDesc.vOffSetPosition = vOffsetPos;
			TriggerCapsuleDesc.vOffSetRotation = vOffsetRot;

			RigidBodyDesc.pGeometry = &TriggerCapsuleDesc;
		}

		RigidBodyDesc.eThisColFlag = (ECOLLISIONFLAG)Model[strAnimIndex]["ShapeInfo"]["MyFlag"].asInt();
		RigidBodyDesc.eTargetColFlag = (ECOLLISIONFLAG)Model[strAnimIndex]["ShapeInfo"]["TargetFlag"].asInt();

		m_pRigidBody->CreateShape(&RigidBodyDesc);

		vector<shared_ptr<CAnimation>> vecAnims = m_pModelCom->GetAnimations();
		vecAnims[iAnimIndex]->SetOwnerObject(shared_from_this());
		vecAnims[iAnimIndex]->SetColStartTrackPos(Model[strAnimIndex]["TrackPos"]["StartPos"].asFloat());
		vecAnims[iAnimIndex]->SetColEndTrackPos(Model[strAnimIndex]["TrackPos"]["EndPos"].asFloat());
	}

	return S_OK;
}

HRESULT CAmitoy::ReadyCamera()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	shared_ptr<CCameraSpherical> pCamera = CCameraSpherical::Create();
	if (pCamera)
	{
		pCamera->SetOwner(shared_from_this());
		pCamera->SetOffSetPosition({ 0.0f,0.8f,0.0f });
		pCamera->SetRadius(2.0f);
		pCamera->SetOwnerLerpRatio(0.15f);
		if (FAILED(pGameInstance->AddObject(LEVEL_VILLAGE, L"Layer_Camera", pCamera)))
			return E_FAIL;
		CCameraMgr::GetInstance()->AddCamera("AmitoyCamera", pCamera);
		m_pCamera = pCamera;
	}

	CCamera::CAMERA_DESC pCamDesc{};
	pCamDesc.vEye = _float3(0.0f, 30.f, -25.f);
	pCamDesc.vAt = _float3(0.0f, 0.0f, 0.f);
	pCamDesc.fFovy = XMConvertToRadians(60.0f);
	pCamDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	pCamDesc.fNear = 0.1f;
	pCamDesc.fFar = 1000.f;

	shared_ptr<CCameraFilm> AmitoyGetItem = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/AmitoyGetItem.json");
	if (FAILED(CGameInstance::GetInstance()->AddObject(LEVEL_VILLAGE, L"Layer_Camera", AmitoyGetItem)))
		return E_FAIL;

	CCameraMgr::GetInstance()->AddFilmCamera("AmitoyGetItem", AmitoyGetItem);

	return S_OK;
}
