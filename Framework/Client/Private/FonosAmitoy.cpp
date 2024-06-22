#include "pch.h"
#include "FonosAmitoy.h"
#include "Model.h"
#include "Animation.h"
#include "RigidBody.h"
#include "CameraMgr.h"
#include "CameraSpherical.h"
#include "ItemMisc.h"

#include "EffectMgr.h"
#include "DialogueMgr.h"
#include "EventMgr.h"
#include "UIMgr.h"
#include "ItemMgr.h"


CFonosAmitoy::CFonosAmitoy()
	: CInteractionObj()
{
}

CFonosAmitoy::~CFonosAmitoy()
{
}

shared_ptr<CFonosAmitoy> CFonosAmitoy::Create()
{
	shared_ptr<CFonosAmitoy> pInstance = make_shared<CFonosAmitoy>();

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CFonosAmitoy::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CFonosAmitoy::Initialize()
{
	__super::Initialize(nullptr);

	m_pTransformCom = CTransform::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo());
	m_pTransformCom->SetSpeed(6.f);

	string ModelTag = "MD_Golem_Skin_001";
	m_pModelCom = GAMEINSTANCE->GetModel(ModelTag);
	AddComponent(L"Com_Model", m_pModelCom);

	m_pTransformCom->LookAtDirForLandObject({ 0.f, 0.f, -1.f });

	m_pModelCom->ChangeAnim(AMITOY_STATE_APPEAR, 0.f, false);
	//m_pModelCom->ChangeAnim(AMITOY_STATE_IDLE, 0.1f, true);
	m_pModelCom->PlayAnimation(0.016f);

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_ANIM;
	m_iShaderPass = 0;

	AddRigidBody("Fonos_Amitoy");

	shared_ptr<CCameraSpherical> pCamera = CCameraSpherical::Create();
	if (pCamera)
	{
		pCamera->SetOwner(shared_from_this());
		pCamera->SetOffSetPosition({ 0.0f,0.5f,0.0f });
		pCamera->SetRadius(2.0f);
		pCamera->SetOwnerLerpRatio(0.15f);
		if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Layer_Camera", pCamera)))
			return E_FAIL;
		CCameraMgr::GetInstance()->AddCamera("FonosAmitoyCamera", pCamera);
		m_pCamera = pCamera;
	}

	CEventMgr::GetInstance()->BindFonosAmitoy(static_pointer_cast<CFonosAmitoy>(shared_from_this()));

	EFFECTMGR->PlayEffect("Amitoy_Hair", shared_from_this());

	m_vUIDistance = { 0.f, 0.6f, 0.2f };

	return S_OK;
}

void CFonosAmitoy::PriorityTick(_float _fTimeDelta)
{
	__super::PriorityTick(_fTimeDelta);

	if (!m_bTriggerOn)
		return;

	TestKeyInput();

	if (m_isInteraction && m_pModelCom->GetIsFinishedAnimation())
	{
		m_isInteraction = false;
	}
}

void CFonosAmitoy::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	// ÀÎÅÍ·¢¼Ç Àü
	if (!m_bTriggerOn)
	{
		m_pTransformCom->SetState(CTransform::STATE_POSITION, { 54.76f, 15.29f, 112.22f });
		m_pRigidBody->SetPosition(m_pTransformCom->GetState(CTransform::STATE_POSITION));
		m_pTransformCom->Tick(_fTimeDelta);
		return;
	}

	// °ÔÀÓ ³¡³­ ÈÄ
	if (m_bGameEnd)
	{
		m_pModelCom->PlayAnimation(_fTimeDelta);
		m_pTransformCom->Tick(_fTimeDelta);
		return;
	}

	// ·¹ÀÌ½Ì Áß
	if (m_bRunStart && UIMGR->GetIsStartSignal())
	{
		m_fRaceTime -= _fTimeDelta;
		UIMGR->UpdateCountDown(m_fRaceTime);
		// console("%f\n", m_fRaceTime);
	}

	m_ePrevState = m_eCurState;
	State_Tick(_fTimeDelta);
	
	if (m_eCurState != m_ePrevState)
	{
		State_Out(_fTimeDelta);
		State_In(_fTimeDelta);
	}

	m_pModelCom->PlayAnimation(_fTimeDelta);
	m_pTransformCom->Tick(_fTimeDelta);
}

void CFonosAmitoy::LateTick(_float _fTimeDelta)
{
	__super::LateTick(_fTimeDelta);

	m_pModelCom->CheckDisableAnim();
	m_pTransformCom->LateTick(_fTimeDelta);

	// frustrum
	if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 2.f))
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
		GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());


#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CFonosAmitoy::Render()
{
	UINT iNumMeshes = m_pModelCom->GetNumMeshes();

	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);

	for (UINT i = 0; i < iNumMeshes; i++)
	{
		GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
		GAMEINSTANCE->BeginAnimModel(m_pModelCom, i);
	}

	return S_OK;
}

HRESULT CFonosAmitoy::RenderShadow()
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

void CFonosAmitoy::TestKeyInput()
{

	if (GAMEINSTANCE->KeyDown(DIK_LEFT))
	{
		m_eCurState = (AMITOY_STATE)(m_eCurState - 1 < 0 ? m_pModelCom->GetAnimations().size() - 1 : m_eCurState - 1);
		m_pModelCom->ChangeAnim(m_eCurState, 0.1f, true);
	}

	if (GAMEINSTANCE->KeyDown(DIK_RIGHT))
	{
		m_eCurState = (AMITOY_STATE)(m_eCurState + 1 >= m_pModelCom->GetAnimations().size() ? 0 : m_eCurState + 1);
		m_pModelCom->ChangeAnim(m_eCurState, 0.1f, true);
	}

	if (GAMEINSTANCE->KeyDown(DIK_R))
		m_pTransformCom->SetState(CTransform::STATE_POSITION, { 55.7f, 15.37f, 104.62f });

	if (GAMEINSTANCE->KeyDown(DIK_PGUP))
	{
		m_iEffectIdx = EFFECTMGR->PlayEffect("Trap_Stone_Target2", m_pTransformCom->GetState(CTransform::STATE_POSITION), false, 3.f);
	}

	if (GAMEINSTANCE->KeyDown(DIK_PGDN))
	{
		m_bGameStart = true;
		CCameraMgr::GetInstance()->CameraTurnOn("FonosAmitoyCamera");
		m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(39.29f, 17.17f, 122.96f));
		m_pRigidBody->SetPosition(m_pTransformCom->GetState(CTransform::STATE_POSITION));
		m_pTransformCom->LookAtDir(_float3(-1.f, 0.f, 0.f));
		UIMGR->DeactivateHUD();
		shared_ptr<CGameObject> pPlr = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
		pPlr->SetEnable(false);
	}

	if (DIALOGUEMGR->GetCurDialogueKey() == L"Sloti0" && DIALOGUEMGR->GetDialogueState() == DIALOGUE_COMPLETE)
	{
		m_bGameStart = true;
		CCameraMgr::GetInstance()->CameraTurnOn("FonosAmitoyCamera");
		m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(39.29f, 17.17f, 122.96f));
		m_pRigidBody->SetPosition(m_pTransformCom->GetState(CTransform::STATE_POSITION));
		m_pTransformCom->LookAtDir(_float3(-1.f, 0.f, 0.f));
		UIMGR->DeactivateHUD();
		shared_ptr<CGameObject> pPlr = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
		pPlr->SetEnable(false);
		vector<string> vecMiniGameFilmCamera;
		vecMiniGameFilmCamera.emplace_back("FonosMiniGame1");
		vecMiniGameFilmCamera.emplace_back("FonosMiniGame2");
		CCameraMgr::GetInstance()->FilmCameraTurnOn(vecMiniGameFilmCamera, false);
		GAMEINSTANCE->LevelBGMOff();
		//GAMEINSTANCE->PlayBGM("FA_BGM_2", 0.3f);
	}

	/*CItemBase::ItemDesc itemDesc2{};
	itemDesc2.strItemTag = "Èñ±ÍÇÑ ¸¶¸°µå";
	itemDesc2.eItemType = ITEM_MISC;
	itemDesc2.strTexTag = "I_SkillGrowth_Polymorph_Sapphire_004";
	itemDesc2.strModelTag = "";
	itemDesc2.numItem = 2;

	shared_ptr<CItemMisc> pItemMisc2 = CItemMisc::Create(itemDesc2);*/

	if (DIALOGUEMGR->GetCurDialogueKey() == L"Sloti1" && DIALOGUEMGR->GetDialogueState() == DIALOGUE_COMPLETE)
	{
		
	}
}

void CFonosAmitoy::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.eColFlag & COL_TRAP)
	{
		if (m_bInvincible)
			return;

		if (_ColData.szShapeTag == "Fonos_Trap_Shock")
			m_bShock = true;

		if (_ColData.szShapeTag == "Fonos_Trap_Move")
			m_bShock = true;

		if (_ColData.szShapeTag == "Jump_Mushroom_Head")
			m_bSuperJump = true;
	}
}

void CFonosAmitoy::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.szShapeTag == "Fonos_Trap_Slow")
		m_bSlow = true;
}

void CFonosAmitoy::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.szShapeTag == "Fonos_Trap_Slow")
		m_bSlow = false;
}

void CFonosAmitoy::EnableCollision(const char* _strShapeTag)
{
	m_pRigidBody->EnableCollision(_strShapeTag);
}

void CFonosAmitoy::DisableCollision(const char* _strShapeTag)
{
	m_pRigidBody->DisableCollision(_strShapeTag);
}

_bool CFonosAmitoy::AbnormalCheck(float _fTimeDelta)
{
	if (m_bInvincible)
	{
		m_fInvTime -= _fTimeDelta;

		if (0.f >= m_fInvTime)
			m_bInvincible = false;

		return false;
	}

	_bool bAbnormal = true;

	if (m_bShock)
		m_eCurState = AMITOY_STATE_SHOCK;
	/*else if (m_bJumpBack)
		m_eCurState = AMITOY_STATE_KNOCKBACK;*/
	else if (m_bSuperJump)
		m_eCurState = AMITOY_STATE_SUPERJUMP;
	else
		bAbnormal = false;

	return bAbnormal;
}

CFonosAmitoy::AMITOY_STATE CFonosAmitoy::KeyInput(float _fTimeDelta)
{
	if (GAMEINSTANCE->KeyDown(DIK_E))
	{
		m_pTransformCom->Jump(1100.0f);
	}

	_uint	iMoveType = 0;
	_bool	bValidMove = false;

	if (GAMEINSTANCE->KeyPressing(DIK_W))
		iMoveType |= 1;
	if (GAMEINSTANCE->KeyPressing(DIK_A))
		iMoveType |= 2;
	if (GAMEINSTANCE->KeyPressing(DIK_S))
		iMoveType |= 4;
	if (GAMEINSTANCE->KeyPressing(DIK_D))
		iMoveType |= 8;

	if (0 == iMoveType)
		return m_eCurState == AMITOY_STATE_SUPERJUMP ? AMITOY_STATE_SUPERJUMP : AMITOY_STATE_IDLE;

	_float3 vLook(0.f, 0.f, 0.f);

	// Move Control
	switch (iMoveType)
	{
	case 1: // »ó
		vLook = GAMEINSTANCE->GetCamLook();
		bValidMove = true;
		break;
	case 4:	// ÇÏ
		vLook = GAMEINSTANCE->GetCamLook() * -1.f;
		bValidMove = true;
		break;
	case 2:	// ÁÂ
		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(270.f)));
		bValidMove = true;
		break;
	case 8: // ¿ì
		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(90.f)));
		bValidMove = true;
		break;
	case 3: // »óÁÂ
		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(315.f)));
		bValidMove = true;
		break;
	case 9: // »ó¿ì
		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(45.f)));
		bValidMove = true;
		break;
	case 6:	// ÇÏÁÂ
		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(225.f)));
		bValidMove = true;
		break;
	case 12:// ÇÏ¿ì
		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(135.f)));
		bValidMove = true;
		break;
	case 11:// ÁÂ»ó¿ì
		vLook = GAMEINSTANCE->GetCamLook();
		bValidMove = true;
		break;
	case 14:// ÁÂÇÏ¿ì
		vLook = GAMEINSTANCE->GetCamLook() * -1.f;
		bValidMove = true;
		break;
	case 7:	// »óÁÂÇÏ
		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(270.f)));
		bValidMove = true;
		break;
	case 13:// »ó¿ìÇÏ
		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(90.f)));
		bValidMove = true;
		break;
	}

	if (!bValidMove)
		return m_eCurState == AMITOY_STATE_SUPERJUMP ? AMITOY_STATE_SUPERJUMP : AMITOY_STATE_IDLE;

	_float fRadian = XMVectorGetX(XMVector3Dot(m_pTransformCom->GetState(CTransform::STATE_LOOK), vLook));

	if (-0.7 > fRadian)
	{
		vLook = XMVectorLerp(m_pTransformCom->GetState(CTransform::STATE_LOOK), vLook, 0.5f);
	}
	else if (-0.1 > fRadian)
	{
		vLook = XMVectorLerp(m_pTransformCom->GetState(CTransform::STATE_LOOK), vLook, 0.5f);
	}
	else
	{
		vLook = XMVectorLerp(m_pTransformCom->GetState(CTransform::STATE_LOOK), vLook, 0.5f);
	}

	m_pTransformCom->LookAtDirForLandObject(vLook);

	if (AMITOY_STATE_SUPERJUMP == m_eCurState)
	{
		m_pTransformCom->GoStraight(_fTimeDelta);
		return AMITOY_STATE_SUPERJUMP;
	}

	if (m_bSlow)
		return	AMITOY_STATE_WALK;
	else
		return	AMITOY_STATE_RUN;
}

void CFonosAmitoy::InteractionOn()
{
	if (m_bIsInteractedOnce == false)
	{
		DIALOGUEMGR->SetCurDialogue(L"Sloti0");
		m_bTriggerOn = true;
		m_bIsInteractedOnce = true;

		//GAMEINSTANCE->Play
		// ("FA_Hair", 0.5f);
		GAMEINSTANCE->LevelBGMOff();
		EFFECTMGR->StopEffect("Amitoy_Hair", 0);
	}

	else
		DIALOGUEMGR->SetCurDialogue(L"Sloti1");
}

void CFonosAmitoy::MiniGameCeremony()
{
	//if (0.f >= m_fRaceTime)
	//{
	//	m_pModelCom->ChangeAnim(AMITOY_STATE_DISAPPOINT, 0.1f, true);
	//	m_pModelCom->ChangeAnim(AMITOY_STATE_CEREMONY, 0.1f, true);
	//	m_pTransformCom->LookAtDir({ 1.f, 0.f, -0.5f });
	//	m_eCurState = AMITOY_STATE_DISAPPOINT;
	//	m_ePrevState = AMITOY_STATE_DISAPPOINT;
	//	m_fCeremonyTime = 5.f;
	//	m_bWinGame = false;
	//}

	//else
	//{

	GAMEINSTANCE->PlaySoundW("FA_Shock3", 2.5f);
	GAMEINSTANCE->StopSound("FA_BGM_2");
	//GAMEINSTANCE->PlayBGM("FA_BGM_Ceremony", 0.8f);
	GAMEINSTANCE->PlaySoundW("FA_FX_Firework", 1.0f);

	UIMGR->DeactivateCountDown();
	UIMGR->UpdateCountDown(-1.f);

		m_pModelCom->ChangeAnim(AMITOY_STATE_CEREMONY, 0.1f, true);
		m_pTransformCom->LookAtDir({ 1.0f, 0.0f, -1.05f });
		m_eCurState = AMITOY_STATE_CEREMONY;
		m_ePrevState = AMITOY_STATE_CEREMONY;
		m_fCeremonyTime = 7.6f;
		m_bWinGame = true;
		vector<string> vecMiniGameFilmCamera;
		vecMiniGameFilmCamera.emplace_back("FonosMiniGameCeremony1");
		vecMiniGameFilmCamera.emplace_back("FonosMiniGameCeremony2");
		CCameraMgr::GetInstance()->FilmCameraTurnOn(vecMiniGameFilmCamera, false);

		EFFECTMGR->PlayEffect("Firework_Red", shared_from_this());
		EFFECTMGR->PlayEffect("Firework_Blue", shared_from_this());
		EFFECTMGR->PlayEffect("Firework_Purple", shared_from_this());
		EFFECTMGR->PlayEffect("Firework_Green", shared_from_this());

		EFFECTMGR->PlayEffect("Firework_Red_2", shared_from_this());
		EFFECTMGR->PlayEffect("Firework_Blue_2", shared_from_this());
		EFFECTMGR->PlayEffect("Firework_Purple_2", shared_from_this());
		EFFECTMGR->PlayEffect("Firework_Green_2", shared_from_this());
	//}
}

void CFonosAmitoy::MiniGameEnd()
{
	EFFECTMGR->StopEffect("Amitoy_Leave", 0);

	shared_ptr<CGameObject> pPlr = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
	pPlr->SetEnable(true);
	GAMEINSTANCE->StopAll();
	//GAMEINSTANCE->LevelBGMOn();
	UIMGR->ActivateHUD();
	m_bWinGame = true;

	if (m_bWinGame)
	{
		m_bGameEnd = true;
		m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(89.63f, 20.25f, 119.45f));
		m_pRigidBody->SetPosition(m_pTransformCom->GetState(CTransform::STATE_POSITION));
		m_pTransformCom->LookAtDir(_float3(-1.f, 0.f, 0.f));

		m_eCurState = AMITOY_STATE_LAUGH;
	}

	else
	{
		m_bGameEnd = false;
		m_bGameStart = false;
		m_bRunStart = false;
		m_bTriggerOn = false;
		m_bRunStart = false;

		m_pTransformCom->LookAtDirForLandObject({ 0.f, 0.f, -1.f });

		m_pModelCom->ChangeAnim(AMITOY_STATE_APPEAR, 0.f, false);
		m_pModelCom->PlayAnimation(0.016f);
	}
	//m_pModelCom->ChangeAnim(AMITOY_STATE_LAUGH, 0.f, true);

	CCameraMgr::GetInstance()->CameraTurnOn("PlayerCamera");
}

HRESULT CFonosAmitoy::AddRigidBody(string _strModelKey)
{
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
	SphereDesc.fRadius = 0.25f;
	SphereDesc.strShapeTag = _strModelKey.c_str();
	SphereDesc.vOffSetPosition = { 0.0f, 0.1f,0.0f };
	RigidBodyDesc.pGeometry = &SphereDesc;

	RigidBodyDesc.eThisColFlag = COL_AMITOY;
	RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_TRIGGER | COL_TRAP | COL_AMITOY;
	m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
	m_pRigidBody->SetMass(3.0f);

	m_pTransformCom->SetRigidBody(m_pRigidBody);

	return S_OK;
}

void CFonosAmitoy::State_In(_float _fTimeDelta)
{
	switch (m_eCurState)
	{
	case AMITOY_STATE_IDLE:
		m_pModelCom->ChangeAnim(AMITOY_STATE_IDLE, 0.1f, true);
		break;
	case AMITOY_STATE_SPIN:
		m_pModelCom->ChangeAnim(AMITOY_STATE_SPIN, 0.1f, true);
		break;
	case AMITOY_STATE_SHOCK:
		m_fAbnormalTime = 1.5f;
		m_bShock = false;
		m_pModelCom->ChangeAnim(AMITOY_STATE_SHOCK, 0.1f, false, 0.7f);
		EFFECTMGR->PlayEffect("P_Hu_M_ST_Abn_shocked", shared_from_this());
		GAMEINSTANCE->PlaySoundW("FA_Shock2", 1.5f);
		GAMEINSTANCE->PlaySoundW("FA_FX_Shock", 1.5f);
		break;
	case AMITOY_STATE_SUPERJUMP:
		m_bSuperJump = false;
		m_pModelCom->ChangeAnim(AMITOY_STATE_SUPERJUMP, 0.1f, false, 0.7f);
		m_pModelCom->GetCurAnimation()->SetTrackPosition(4.f);
		m_pRigidBody->SetLinearVelocity({ 0.0f,0.0f,0.0f });
		m_pTransformCom->AddForce({ 0.0f, 15.0f,0.0f }, PxForceMode::eVELOCITY_CHANGE);
		GAMEINSTANCE->PlaySoundW("FA_SuperJump", 2.f);
		GAMEINSTANCE->PlaySoundW("FA_Jump_Obj", 1.0f);
		break;
	case AMITOY_STATE_APPEAR:
		m_pModelCom->ChangeAnim(AMITOY_STATE_APPEAR, 0.1f, true);
		break;
	case AMITOY_STATE_DISAPPEAR:
		m_pModelCom->ChangeAnim(AMITOY_STATE_DISAPPEAR, 0.1f, true);
		break;
	case AMITOY_STATE_CEREMONY:
		m_pModelCom->ChangeAnim(AMITOY_STATE_CEREMONY, 0.1f, true);
		break;
	case AMITOY_STATE_LAUGH:
		m_pModelCom->ChangeAnim(AMITOY_STATE_LAUGH, 0.1f, true);
		break;
	case AMITOY_STATE_DISAPPOINT:
		m_pModelCom->ChangeAnim(AMITOY_STATE_DISAPPOINT, 0.1f, true);
		break;
	/*case AMITOY_STATE_KNOCKBACK:
		m_pModelCom->ChangeAnim(AMITOY_STATE_KNOCKBACK, 0.1f, true);*/
		break;
	case AMITOY_STATE_RUN:
		m_pModelCom->ChangeAnim(AMITOY_STATE_RUN, 0.1f, true);
		break;
	case AMITOY_STATE_WALK:
		m_pModelCom->ChangeAnim(AMITOY_STATE_WALK, 0.1f, true);
		GAMEINSTANCE->PlaySoundW("FA_Slow", 2.f);
		GAMEINSTANCE->PlaySoundW("FA_FX_Slow2", 1.0f);
		break;
	case AMITOY_STATE_TALK:
		m_pModelCom->ChangeAnim(AMITOY_STATE_TALK, 0.1f, true);
		break;
	}
}

void CFonosAmitoy::State_Tick(_float _fTimeDelta)
{
	switch (m_eCurState)
	{
	case AMITOY_STATE_IDLE:
		if (AbnormalCheck(_fTimeDelta))
			return;
		m_eCurState = KeyInput(_fTimeDelta);
		break;
	case AMITOY_STATE_SHOCK:
		m_fAbnormalTime -= _fTimeDelta;
		m_bInvincible = true;
		if (0.f >= m_fAbnormalTime)
		{
			m_eCurState = AMITOY_STATE_IDLE;
			m_bInvincible = true;
			m_fInvTime = 0.5f;
		}
		break;
	case AMITOY_STATE_SUPERJUMP:
		if (m_pModelCom->GetCanCancelAnimation())
		{
			AbnormalCheck(_fTimeDelta);
			m_eCurState = AMITOY_STATE_IDLE;
			return;
		}
		m_eCurState = KeyInput(_fTimeDelta);
		break;
	case AMITOY_STATE_APPEAR:
		if (m_pModelCom->GetCanCancelAnimation())
			m_eCurState = AMITOY_STATE_TALK;
		break;
	case AMITOY_STATE_DISAPPEAR:
		break;
	case AMITOY_STATE_CEREMONY:
		m_fCeremonyTime -= _fTimeDelta;

		if (0.f >= m_fCeremonyTime)
		{
			GAMEINSTANCE->StopSound("FA_BGM_Ceremony");
			MiniGameEnd();
		}

		break;
	case AMITOY_STATE_LAUGH:
		break;
	case AMITOY_STATE_SPIN:
		//console("%f\n", m_fGameStartWaitTime);

		m_fGameStartWaitTime -= _fTimeDelta;

		if (1.f >= m_fGameStartWaitTime && 0 < m_iCountSound)
		{
			GAMEINSTANCE->PlaySoundW("FA_FX_Count", 1.f);
			m_iCountSound--;
		}
		else if (2.f >= m_fGameStartWaitTime && 1 < m_iCountSound)
		{
			GAMEINSTANCE->PlaySoundW("FA_FX_Count", 1.f);
			m_iCountSound--;
		}
		else if (3.f >= m_fGameStartWaitTime && 2 < m_iCountSound)
		{
			GAMEINSTANCE->PlaySoundW("FA_FX_Count", 1.f);
			m_iCountSound--;
			UIMGR->ActivateCountDown();
		}
		else if (4.f >= m_fGameStartWaitTime && 3 < m_iCountSound)
		{
			m_iCountSound--;
		}

		if (0.f >= m_fGameStartWaitTime)
		{
			m_eCurState = AMITOY_STATE_IDLE;
			m_fRaceTime = 40.f;
			m_bRunStart = true;
			EFFECTMGR->PlayEffect("Amitoy_Leave", shared_from_this());
			GAMEINSTANCE->PlaySoundW("FA_FX_Go", 1.f);
		}
		break;
	case AMITOY_STATE_DISAPPOINT:
		if (m_bGameStart)
			m_eCurState = AMITOY_STATE_SPIN;
		if (m_pModelCom->GetCanCancelAnimation())
		{
			m_eCurState = AMITOY_STATE_TALK;
		}
		break;
	case AMITOY_STATE_TALK:
		if (m_bGameStart)
			m_eCurState = AMITOY_STATE_SPIN;
		if (m_pModelCom->GetCanCancelAnimation())
		{
			m_eCurState = AMITOY_STATE_DISAPPOINT;
		}
		break;
	/*case AMITOY_STATE_KNOCKBACK:
		break;*/
	case AMITOY_STATE_RUN:
		if (AbnormalCheck(_fTimeDelta))
			return;
		m_eCurState = KeyInput(_fTimeDelta);
		m_pTransformCom->GoStraight(_fTimeDelta);
		m_fRunSoundTimer -= _fTimeDelta;
		if (0.f >= m_fRunSoundTimer)
		{
			//GAMEINSTANCE->PlaySoundW("", 1.f);
			m_fRunSoundTimer = 1.f;
		}
		break;
	case AMITOY_STATE_WALK:
		if (AbnormalCheck(_fTimeDelta))
			return;
		m_eCurState = KeyInput(_fTimeDelta);
		m_pTransformCom->GoStraight(_fTimeDelta / 5.f);
		break;
	}
}

void CFonosAmitoy::State_Out(_float _fTimeDelta)
{
	int a;
	switch (m_ePrevState)
	{
	case AMITOY_STATE_IDLE:
		break;
	case AMITOY_STATE_SPIN:
		break;
	case AMITOY_STATE_SHOCK:
		break;
	case AMITOY_STATE_SUPERJUMP:
		break;
	case AMITOY_STATE_APPEAR:
		break;
	case AMITOY_STATE_DISAPPEAR:
		break;
	case AMITOY_STATE_CEREMONY:
		break;
	case AMITOY_STATE_LAUGH:
		break;
	case AMITOY_STATE_DISAPPOINT:
		break;
	/*case AMITOY_STATE_KNOCKBACK:
		break;*/
	case AMITOY_STATE_RUN:
		break;
	case AMITOY_STATE_WALK:
		GAMEINSTANCE->StopSound("FA_FX_Slow2");
		break;
	}
}
