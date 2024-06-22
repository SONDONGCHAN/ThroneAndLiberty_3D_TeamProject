#include "Player.h"
#include "Model.h"
#include "RigidBody.h"
#include "EffectMgr.h"
#include "CameraMgr.h"
#include "CameraSpherical.h"
#include "CameraFilm.h"
#include "Animation.h"
#include "Projectile.h"
#include "UIMgr.h"
#include "UIUVBar.h"
#include "UIInstanceQuickSlot.h"
#include "ItemMgr.h"
#include "ItemEquip.h"
#include "PointTrail.h"
#include "CharacterMgr.h"
#include "PlayerProjectilePool.h"
#include "QuestMgr.h"
#include "MotionTrail.h"
#include "PlayerRope.h"
#include "EventMgr.h"
#include "Monster.h"
#include "GameMgr.h"
#include "FonosAmitoy.h"

#include "LevelLoading.h"

CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Initialize()
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 5.f;
	pArg->fRotationPerSec = XMConvertToRadians(90.0f);
	__super::Initialize(pArg.get());
	
	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_ANIM;
	m_iShaderPass = 0;

	m_pModelCom = GAMEINSTANCE->GetModel("Player_Base");
	m_pModelCom->ReadySeparate("Bip001-Spine");
	AddComponent(L"Com_Model", m_pModelCom);
	ChangeAnim(WEAPON_SWORD_IDLE, 0.1f, true);
	
	/* ItemMgr Init for Initial Items */
	ITEMMGR->Initialize();

	AddRigidBody("Player_Base");
	AddWeapons();
	AddEquipments();
	MakeAnimationSet();
	SettingSkillCoolTime();
	AddSeparateAnims();
	AddMovementAnims();
	AddSkillAnims();
	AddIgnoreAbnormalAnims();
	AddWeaponTrail();
	AddMotionTrail(4);
	AddProjectilePool();
	AddProjectileTriggers("Player_Base");
	SetAnimOwner();
	AddRopeLine();
	m_pSeparating = m_pModelCom->GetSeparating();

	shared_ptr<CCameraSpherical> pCamera = CCameraSpherical::Create();
	if (pCamera)
	{
		pCamera->SetOwner(shared_from_this());
		m_pCamera = pCamera;
		if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, L"Layer_Camera", pCamera)))
			return E_FAIL;
		CCameraMgr::GetInstance()->AddCamera("PlayerCamera", pCamera);
		CCameraMgr::GetInstance()->CameraTurnOn("PlayerCamera");
		CHARACTERMGR->SetCamera(pCamera);
	}
	CreateCamera();

	/* Temp, Might Change Later */
	/* Bind Player to ItemMgr */
	ITEMMGR->BindPlayer(static_pointer_cast<CPlayer>(shared_from_this()));
	UIMGR->BindPlayer(static_pointer_cast<CPlayer>(shared_from_this()));
	CEventMgr::GetInstance()->BindPlayer(static_pointer_cast<CPlayer>(shared_from_this()));
	CGameMgr::GetInstance()->BindPlayer(static_pointer_cast<CPlayer>(shared_from_this()));

	return S_OK;
}

void CPlayer::PriorityTick(_float _fTimeDelta)
{

	if (GAMEINSTANCE->KeyDown(DIK_6) && GAMEINSTANCE->KeyPressing(DIK_RSHIFT)) {

		CHARACTERMGR->SetInDungeon(false);
		SetInDungeon(false);
		UIMGR->UITickOff();

		GAMEINSTANCE->StopAll();

		GAMEINSTANCE->SetLoadingLevel(CLevelLoading::Create(LEVEL_VILLAGE));
		GAMEINSTANCE->SetChangeLevelSignal(LEVEL_VILLAGE);
		m_bSceneChange = true;
		m_iChangeScene = LEVEL_VILLAGE;
	}
	if (GAMEINSTANCE->KeyDown(DIK_7) && GAMEINSTANCE->KeyPressing(DIK_RSHIFT)) {

		CHARACTERMGR->SetInDungeon(false);
		SetInDungeon(false);
		UIMGR->UITickOff();
		
		GAMEINSTANCE->StopAll();

		GAMEINSTANCE->SetLoadingLevel(CLevelLoading::Create(LEVEL_FONOS));
		GAMEINSTANCE->SetChangeLevelSignal(LEVEL_FONOS);
		m_bSceneChange = true;
		m_iChangeScene = LEVEL_FONOS;
	}
	if (GAMEINSTANCE->KeyDown(DIK_8) && GAMEINSTANCE->KeyPressing(DIK_RSHIFT)) {

		CHARACTERMGR->SetInDungeon(false);
		SetInDungeon(false);
		UIMGR->UITickOff();
		GAMEINSTANCE->StopAll();


		GAMEINSTANCE->SetLoadingLevel(CLevelLoading::Create(LEVEL_DUNGEON_MAIN));
		GAMEINSTANCE->SetChangeLevelSignal(LEVEL_DUNGEON_MAIN);
		m_bSceneChange = true;
		m_iChangeScene = LEVEL_DUNGEON_MAIN;
	}
	if (GAMEINSTANCE->KeyDown(DIK_9) && GAMEINSTANCE->KeyPressing(DIK_RSHIFT)) {

		CHARACTERMGR->SetInDungeon(false);
		SetInDungeon(false);
		UIMGR->UITickOff();

		GAMEINSTANCE->StopAll();

		GAMEINSTANCE->SetLoadingLevel(CLevelLoading::Create(LEVEL_DUNGEON_FINAL));
		GAMEINSTANCE->SetChangeLevelSignal(LEVEL_DUNGEON_FINAL);
		m_bSceneChange = true;
		m_iChangeScene = LEVEL_DUNGEON_FINAL;
	}
	if (GAMEINSTANCE->KeyDown(DIK_H)) {
		RestoreHp();
	}

	PolymorphControl(_fTimeDelta);
}

void CPlayer::Tick(_float _fTimeDelta)
{
#ifdef _DEBUG
	if (GAMEINSTANCE->KeyDown(DIK_COMMA)) 
	{
		SetPlayerHp(-500.f);
		weak_ptr<CUIUVBar> hpPanel = dynamic_pointer_cast<CUIUVBar>(UIMGR->FindUI("PCInfoGaugeHP"));
		if (hpPanel.expired())
			return;
		hpPanel.lock()->SetBar(m_fCurrentHp, m_fMaxHp);
	}
#endif // _DEBUG

	if (m_bSceneChange)
	{
		m_fCalculateTime += _fTimeDelta;
		if (m_fCalculateTime > 1.f)
		{
			switch (m_iChangeScene)
			{
			case LEVEL_VILLAGE:
				m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(104.f, -10.6f, 371.6f));
				m_pTransformCom->LookAtDir(_float3(97.8f, -10.f, 301.f) -
					_float3(94.47f, -10.f, 305.f));
				break;
			case LEVEL_FONOS:
				m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(81.6f, 1.2f, 6.5f));
				m_pTransformCom->LookAtDir(_float3(81.3f, -10.f, 10.1f) -
					_float3(81.6f, -10.f, 6.5f));
				break;
			case LEVEL_DUNGEON_MAIN:
				m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(20.7f, 0.2f, 13.f));
				break;
			case LEVEL_DUNGEON_FINAL:
				m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(21.3f, -9.2f, 86.1f));
				m_pTransformCom->LookAtDir(_float3(21.3f, -10.f, 93.5f) -
					_float3(21.3f, -10.f, 86.1f));
				break;
			}
			m_fCalculateTime = 0.f;
			m_bSceneChange = false;
		}
	}

	if (!m_isChanged)
	{
		SetDissolve(true, false, 0.8f);
		SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");

		for (auto pEquipment : m_pPlayerEquipments)
		{
			pEquipment->SetDissolve(true, false, 0.8f);
			pEquipment->SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
		}
		m_isChanged = true;
	}

	_float3 vCurrentPos = m_pTransformCom->GetState(CTransform:: STATE_POSITION); // À§Ä¡Ã¼Å© µð¹ö±ë ¿ëµµ

	//console("%f %f %f\n", vCurrentPos.x, vCurrentPos.y, vCurrentPos.z);
	//console("RuffaloRice\n");
	//console("%f,%d \n", m_fCurrentHp, m_bHealing);
	TickWeapon(_fTimeDelta);

	if (GAMEINSTANCE->KeyDown(DIK_0))
	{
		LEVEL eCurrentLevel = (LEVEL)GAMEINSTANCE->GetCurrentLevel();

		if (eCurrentLevel == LEVEL_DUNGEON_MAIN)
			m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3{ 10.f, 10.f, 10.f });
		else if (eCurrentLevel == LEVEL_VILLAGE)
			m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3{ 490.f, -65.f, 355.78f });
		else if (eCurrentLevel == LEVEL_FONOS)
			m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3{ 81.05f, 1.6f, 9.f });
		else if (eCurrentLevel == LEVEL_DUNGEON_FINAL)
			m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3{ 21.3f, -9.2f, 86.1f });
	}
		//LEVEL_DUNGEON_MAIN,
		//LEVEL_DUNGEON_FINAL,
		//LEVEL_VILLAGE,
		//LEVEL_FONOS,
	
	CommonTick(_fTimeDelta);
	LockOnControl();

	switch (m_eCurrentBaseState)
	{
	case CPlayer::STATE_LOOP:
		LoopStateFSM(_fTimeDelta);
		break;

	case CPlayer::STATE_ACTIVE:
		ActiveStateFSM(_fTimeDelta);
		break;

	case CPlayer::STATE_ISDAMAGED:
		IsDamagedFSM(_fTimeDelta);
		break;

	default:
		break;
	}
	m_pModelCom->PlayAnimation(_fTimeDelta);
	TickEquipment(_fTimeDelta);
	m_pTransformCom->Tick(_fTimeDelta);

	UpdateLightDesc();
}

void CPlayer::LateTick(_float _fTimeDelta)
{
	if (m_bDslv)
		UpdateDslv(_fTimeDelta);

	m_pModelCom->CheckDisableAnim();
	m_pTransformCom->LateTick(_fTimeDelta);

	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
	//GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_SHADOW, shared_from_this());
	GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());

	LateTickWeapon(_fTimeDelta);

	//GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_EDGE, shared_from_this());


	if(m_pRigidBody)
		GAMEINSTANCE->AddDebugComponent(m_pRigidBody);


	LateTickEquipment(_fTimeDelta);

	if(m_bRopeRend)
		m_pRopeLine->LateTick(_fTimeDelta);
}

HRESULT CPlayer::Render()
{
	//string a = to_string(m_eCurrentAnimation);
	//a = a + ";";
	//console(a.c_str());

	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

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

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (0 == i) 
		{
		}
		/*¸Ó¸®*/
		else if (6 == i){
			if (m_bRenderHair)
			{
				//Pass º¯°æ
				GAMEINSTANCE->ReadyShader(m_eShaderType, 1);

				//»ö Bind
				GAMEINSTANCE->BindRawValue("g_vHairMainColor", &m_vHairMainColor, sizeof(_float3));
				GAMEINSTANCE->BindRawValue("g_vHairSubColor", &m_vHairSubColor, sizeof(_float3));

				GAMEINSTANCE->BeginAnimModel(m_pModelCom, i);

				//¿ø»óº¹±¸
				GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
			}
		}
		else {

			GAMEINSTANCE->BeginAnimModel(m_pModelCom, i);
		}
	}

	_bool bDslvFalse = false;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &bDslvFalse, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

void CPlayer::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.eColFlag & COL_TRAP)
	{
		CancleAnimationSet();

		if (_ColData.szShapeTag == "StoneTrap")
		{
			GAMEINSTANCE->PlaySoundW("Player_Hit_Stiff_Short", 0.5f);
			SetPlayerHp(-50.f);

			switch (m_eCurrentWeapon)
			{
			case CPlayer::WEAPON_TYPE_SWORD:AssaultAnim(WEAPON_SWORD_DAMAGED_STUN, 0.05f, 0.8f); break;
			case CPlayer::WEAPON_TYPE_CROSSBOW:AssaultAnim(WEAPON_CROSSBOW_DAMAGED_STUN, 0.05f, 0.8f); break;
			case CPlayer::WEAPON_TYPE_STAFF:AssaultAnim(WEAPON_STAFF_DAMAGED_STUN, 0.05f, 0.8f); break;
			default: break;
			}
		}
		if (_ColData.szShapeTag == "ElectricTrap")
		{
			GAMEINSTANCE->PlaySoundW("Player_Hit_Electric", 0.5f);
			SetPlayerHp(-30.f);
			switch (m_eCurrentWeapon)
			{
			case CPlayer::WEAPON_TYPE_SWORD:AssaultAnim(WEAPON_SWORD_DAMAGED_SHOCK, 0.05f, 0.8f); break;
			case CPlayer::WEAPON_TYPE_CROSSBOW:AssaultAnim(WEAPON_CROSSBOW_DAMAGED_SHOCK, 0.05f, 0.8f); break;
			case CPlayer::WEAPON_TYPE_STAFF:AssaultAnim(WEAPON_STAFF_DAMAGED_SHOCK, 0.05f, 0.8f); break;
			default: break;
			}
		}
		m_pModelCom->PlayAnimation(0.f);
	}

	if (_ColData.eColFlag & COL_MONSTER)
	{	
		if (  (_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_1")
			||(_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_2")
			||(_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_3") )
		{
			ROT_DESC RotDesc = CCameraMgr::GetInstance()->RotSetting({ 0.04f, 0.04f, 0.0f }, { 4.0f,7.0f,0.0f }, { 0.0f, 0.05f });
			CCameraMgr::GetInstance()->CameraShake(SHAKE_ROT, 0.1f, &RotDesc);
			GAMEINSTANCE->SetMotionBlur(0.1f, 0.025f);
			GAMEINSTANCE->PlaySoundW("Player_Sword_Hit_2", 0.5f);
			AddSupremeGauge(3.f);
		}

		else if (_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_PowerAttack")
		{
			ROT_DESC RotDesc = CCameraMgr::GetInstance()->RotSetting({ 0.06f, 0.06f, 0.0f }, { 4.0f,7.0f,0.0f }, { 0.0f, 0.1f });
			CCameraMgr::GetInstance()->CameraShake(SHAKE_ROT, 0.2f, &RotDesc);
			GAMEINSTANCE->SetMotionBlur(0.2f, 0.04f);
			GAMEINSTANCE->PlaySoundW("Player_Sword_Hit_3", 0.5f);
			AddSupremeGauge(3.f);
		}

		else if (  (_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack")
				||(_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack2")
				||(_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack3") )
		{
			ROT_DESC RotDesc = CCameraMgr::GetInstance()->RotSetting({ 0.04f, 0.04f, 0.0f }, { 4.0f,7.0f,0.0f }, { 0.0f, 0.1f });
			CCameraMgr::GetInstance()->CameraShake(SHAKE_ROT, 0.15f, &RotDesc);
			GAMEINSTANCE->SetMotionBlur(0.15f, 0.03f);
			GAMEINSTANCE->PlaySoundW("Player_Sword_Hit_2", 0.5f);
		}

		else if (_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_CM_SW2_DashAttack")
		{
			ROT_DESC RotDesc = CCameraMgr::GetInstance()->RotSetting({ 0.04f, 0.04f, 0.0f }, { 4.0f,7.0f,0.0f }, { 0.0f, 0.1f });
			CCameraMgr::GetInstance()->CameraShake(SHAKE_ROT, 0.15f, &RotDesc);
			GAMEINSTANCE->SetMotionBlur(0.15f, 0.035f);
			GAMEINSTANCE->PlaySoundW("Player_Sword_Hit_1", 0.5f);
		}

		else if (_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_GaiaCrash_Legend")
		{
			ROT_DESC RotDesc = CCameraMgr::GetInstance()->RotSetting({ 0.06f, 0.06f, 0.0f }, { 4.0f,7.0f,0.0f }, { 0.0f, 0.1f });
			CCameraMgr::GetInstance()->CameraShake(SHAKE_ROT, 0.3f, &RotDesc);
			GAMEINSTANCE->SetMotionBlur(0.3f, 0.04f);
		}

		else if (_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_UpperAttack")
		{
			ROT_DESC RotDesc = CCameraMgr::GetInstance()->RotSetting({ 0.05f, 0.05f, 0.0f }, { 4.0f,7.0f,0.0f }, { 0.0f, 0.1f });
			CCameraMgr::GetInstance()->CameraShake(SHAKE_ROT, 0.2f, &RotDesc);
			GAMEINSTANCE->SetMotionBlur(0.2f, 0.035f);
			GAMEINSTANCE->PlaySoundW("Player_Sword_Hit_4", 0.5f);
		}

		else if ( (_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_1")
				||(_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_2")
				||(_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_3") ) 
		{
			ROT_DESC RotDesc = CCameraMgr::GetInstance()->RotSetting({ 0.05f, 0.05f, 0.0f }, { 4.0f,7.0f,0.0f }, { 0.0f, 0.1f });
			CCameraMgr::GetInstance()->CameraShake(SHAKE_ROT, 0.2f, &RotDesc);
			GAMEINSTANCE->SetMotionBlur(0.2f, 0.025f);
			GAMEINSTANCE->PlaySoundW("Player_Sword_Hit_3", 0.5f);
		}	

		else if (_szMyShapeTag == "Player_Body")
		{
			if ((_ColData.pGameObject.lock()->IsEnabled()))
			{
				m_pTouchedTargets.push_back(_ColData.pGameObject);
			}
			return;
		}	
	}
	
	if (_szMyShapeTag == "LockOn_Check_Sword" || _szMyShapeTag == "LockOn_Check_CrossBow" || _szMyShapeTag == "LockOn_Check_Staff" || _szMyShapeTag == "LockOn_Check_Boss")
	{
		if ((_ColData.pGameObject.lock()->IsEnabled()))
		{
			m_pLockOnTargets.push_back(_ColData.pGameObject);
			if (m_pLockOnTargets.size() == 1) {
				m_iCurrentLockOnCursor = 0;
				m_pCurrentLockOnTarget = m_pLockOnTargets[m_iCurrentLockOnCursor];
				//dynamic_pointer_cast<CMonster>(m_pCurrentLockOnTarget.lock())->OnRimLight();
				m_pCurrentLockOnTarget.lock()->SetEdgeRender(true);
				UIMGR->SetLockOnTarget(m_pCurrentLockOnTarget.lock());

				if (m_isSeparateLockOn)
				{
					UIMGR->SetLockOnIcon(true);
					m_pCamera.lock()->LockOn(m_pCurrentLockOnTarget.lock());
				}
				else
					UIMGR->SetLockOnIcon(false);
			}		
		}
		return;
	}

	if (_ColData.eColFlag & COL_STATIC)
	{
		if (((_ColData.szShapeTag == "HeightField") || (_ColData.szShapeTag == "Static_Collider") || (_ColData.szShapeTag == "Pillar_Body")) && (_szMyShapeTag == "Land_Check") && m_isJumpState && (!m_isJumpMinActive))
		{
			CancleAnimationSet();
			switch (m_eCurrentWeapon)
			{
			case CPlayer::WEAPON_TYPE_SWORD:
				ChangeAnim(WEAPON_SWORD_JUMP_LAND, 0.05f, false, 0.5f);
				break;
			case CPlayer::WEAPON_TYPE_CROSSBOW:
				ChangeAnim(WEAPON_CROSSBOW_JUMP_LAND, 0.05f, false, 0.5f);
				break;
			case CPlayer::WEAPON_TYPE_STAFF:
				ChangeAnim(WEAPON_STAFF_JUMP_LAND, 0.05f, false, 0.5f);
				break;
			default:
				break;
			}
			m_isJumpState = false;
			m_isLanding = true;

			m_pModelCom->PlayAnimation(0.f);
		}
		//else if (((_ColData.szShapeTag == "HeightField") || (_ColData.szShapeTag == "Static_Collider")) && (_szMyShapeTag == "Jump_Check") && m_isJumpState)
		//	m_isLanding = true;

		return;
	}

	if ( (	m_eCurrentAnimation == WEAPON_SWORD_ROLLING) || (m_eCurrentAnimation == WEAPON_CROSSBOW_ROLLING) || (m_eCurrentAnimation == WEAPON_STAFF_ROLLING) ||
			(m_eCurrentAnimation == WEAPON_CROSSBOW_BACKSTEP) || (m_eCurrentAnimation == WEAPON_SWORD_SUPREME_SKILL_READY) || 
			(m_eCurrentAnimation == WEAPON_SWORD_SUPREME_SKILL_ATTACK))
		return;

	else if ( (_ColData.eColFlag & COL_MONSTERWEAPON) || (_ColData.eColFlag & COL_MONSTERPROJECTILE) || (_ColData.eColFlag & COL_MONSTERSKILL) )
	{
		if (_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_ActionSkill" || _szMyShapeTag == "Armature|P_Hu_M_ST_Skl_S_CounterMove_ActionSkill")
		{
			if (_szMyShapeTag == "Armature|P_Hu_M_SW2_Skl_S_ActionSkill")
			{
				EFFECTMGR->PlayEffect("Plr_Parrying_Sword", shared_from_this());
				GAMEINSTANCE->PlaySoundW("Player_Parrying_Sword", 0.5f);
			}
			else if (_szMyShapeTag == "Armature|P_Hu_M_ST_Skl_S_CounterMove_ActionSkill")
			{
				EFFECTMGR->PlayEffect("Plr_Parrying_Staff", shared_from_this());
				GAMEINSTANCE->PlaySoundW("Player_Parrying_Staff", 0.5f);

			}
			if (_ColData.eColFlag & COL_MONSTERPROJECTILE)
			{
				_int iPower = dynamic_pointer_cast<CProjectile>(_ColData.pGameObject.lock())->GetPower();

				m_pTransformCom->LookAtForLandObject(_ColData.pGameObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION));

				if (iPower == 1)
					m_fDashSpeed = -7.f;
				else if (iPower == 2)
					m_fDashSpeed = -12.f;
				else if (iPower == 3)
					m_fDashSpeed = -17.f;
			}
			else
			{
				if (_ColData.szShapeTag == "M_L1Midget_Footman_SK.ao|M_L1Midget_Footman_SkdmgA" ||
					_ColData.szShapeTag == "M_L1Midget_Shaman_SK.ao|M_L1Midget_Shaman_SkdmgC" ||
					_ColData.szShapeTag == "M_L1Midget_Sling_SK.ao|M_L1Midget_Sling_USkdmgA" ||
					_ColData.szShapeTag == "M_LivingArmor_Soldier_SK.ao|M_LivingArmor_Soldier_SkdmgC" ||
					_ColData.szShapeTag == "M_LivingArmor_Soldier_SK.ao|M_LivingArmor_Soldier_SkdmgB" ||
					_ColData.szShapeTag == "M_LivingArmor_Mage_SK.ao|M_LivingArmor_Mage_SkdmgD" ||
					_ColData.szShapeTag == "M_LivingArmor_Mage_SK.ao|M_LivingArmor_Mage_SkdmgE" ||
					_ColData.szShapeTag == "M_Orc_Warrior_001_SK.ao|M_Orc_Warrior_USkdmgA" ||
					_ColData.szShapeTag == "M_Orc_Warrior_001_SK.ao|M_Orc_Warrior_USkdmgB" ||
					_ColData.szShapeTag == "M_Orc_Archer_SK.ao|M_Orc_Archer_USkdmgA" ||
					_ColData.szShapeTag == "M_Orc_Shaman_SK.ao|M_Orc_Shaman_USkdmgA" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA.002" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA.004" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB.002" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB.004" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB_M" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB_M.002" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.002" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.003" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.005" )
				{
					m_vecParrieds.push_back(_ColData.szShapeTag);
					m_fDashSpeed = -7.f;
				}
				else if (	_ColData.szShapeTag == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkdmgA.001" ||
							_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgA.001" ||
							_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgA_2.001" || 
							_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgA_M.001" || 
							_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB.001" || 
							_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB.003" || 
							_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB_12.001" || 
							_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD" || 
							_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD.002" || 
							_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingA.001" ||
							_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingB.001")
				{
					m_vecParrieds.push_back(_ColData.szShapeTag);
					m_fDashSpeed = -12.f;
				}
				else if (	_ColData.szShapeTag == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkdmgD.001" ||
							_ColData.szShapeTag == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkmovBE" ||
							_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_DashAttack_E.001" ||
							_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB_12.003" ||
							_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB.005" || 
							_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB_3.001" || 
							_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD.006" ||
							_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_Dash" )
				{
					m_vecParrieds.push_back(_ColData.szShapeTag);
					m_fDashSpeed = -17.f;
				}
			}
		}
		else if (_szMyShapeTag == "Armature|P_Hu_M_CR_CR_Act_DefenseAction_Stdalt")
		{
			EFFECTMGR->PlayEffect("Plr_Parrying_CrossBow", shared_from_this());
			GAMEINSTANCE->PlaySoundW("Player_Parrying_CrossBow", 0.5f);
			if (_ColData.eColFlag == COL_MONSTERPROJECTILE)
			{
				_int iPower = dynamic_pointer_cast<CProjectile>(_ColData.pGameObject.lock())->GetPower();

				m_pTransformCom->LookAtForLandObject(_ColData.pGameObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION));
				ChangeAnim(WEAPON_CROSSBOW_BACKSTEP, 0.05f, false, 0.6f);
				m_pModelCom->PlayAnimation(0.f);
				m_fDashSpeed = -12.f;
			}
			else
			{
				if (_ColData.szShapeTag == "M_L1Midget_Footman_SK.ao|M_L1Midget_Footman_SkdmgA" ||
					_ColData.szShapeTag == "M_L1Midget_Shaman_SK.ao|M_L1Midget_Shaman_SkdmgC" ||
					_ColData.szShapeTag == "M_L1Midget_Sling_SK.ao|M_L1Midget_Sling_USkdmgA" ||
					_ColData.szShapeTag == "M_LivingArmor_Soldier_SK.ao|M_LivingArmor_Soldier_SkdmgC" ||
					_ColData.szShapeTag == "M_LivingArmor_Soldier_SK.ao|M_LivingArmor_Soldier_SkdmgB" ||
					_ColData.szShapeTag == "M_LivingArmor_Mage_SK.ao|M_LivingArmor_Mage_SkdmgD" ||
					_ColData.szShapeTag == "M_LivingArmor_Mage_SK.ao|M_LivingArmor_Mage_SkdmgE" ||
					_ColData.szShapeTag == "M_Orc_Warrior_001_SK.ao|M_Orc_Warrior_USkdmgA" ||
					_ColData.szShapeTag == "M_Orc_Warrior_001_SK.ao|M_Orc_Warrior_USkdmgB" ||
					_ColData.szShapeTag == "M_Orc_Archer_SK.ao|M_Orc_Archer_USkdmgA" ||
					_ColData.szShapeTag == "M_Orc_Shaman_SK.ao|M_Orc_Shaman_USkdmgA" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA.002" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA.004" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB.002" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB.004" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB_M" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB_M.002" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.002" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.003" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.005" ||
					_ColData.szShapeTag == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkdmgA.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgA.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgA_2.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgA_M.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB.003" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB_12.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD.002" ||
					_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingA.001" ||
					_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingB.001" ||
					_ColData.szShapeTag == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkdmgD.001" ||
					_ColData.szShapeTag == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkmovBE" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_DashAttack_E.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB_12.003" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB.005" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB_3.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD.006" ||
					_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_Dash")
				{
					m_vecParrieds.push_back(_ColData.szShapeTag);
					ChangeAnim(WEAPON_CROSSBOW_BACKSTEP, 0.05f, false, 0.6f);
					m_pModelCom->PlayAnimation(0.f);
					m_fDashSpeed = -12.f;
				}
			}
		}

		else if (_szMyShapeTag == "Player_Body")
		{
			if (_ColData.eColFlag & COL_MONSTERPROJECTILE)
			{
				ProjectileAbnormal eAbnormal = (ProjectileAbnormal)dynamic_pointer_cast<CProjectile>(_ColData.pGameObject.lock())->GetAbnormal();

				if(_ColData.szShapeTag == "Axe_Body") SetPlayerHp(-50.f);
				else if (_ColData.szShapeTag == "Thunder1_Body") SetPlayerHp(-200.f);
				else if (_ColData.szShapeTag == "BindMagic2_Body") SetPlayerHp(-50.f);
				else if (_ColData.szShapeTag == "Stone_Body") SetPlayerHp(-50.f);
				else if (_ColData.szShapeTag == "Arrow_Body")
				{
					if(eAbnormal == PJ_A_NORMAL) SetPlayerHp(-50.f);
					else if(eAbnormal == PJ_A_SHORTHITTED) SetPlayerHp(-100.f);
					else if (eAbnormal == PJ_A_BIND) SetPlayerHp(-200.f);
				}
				else if (_ColData.szShapeTag == "ElectricBall_Body")
				{
					if (eAbnormal == PJ_A_NORMAL) SetPlayerHp(-30.f);
					else if (eAbnormal == PJ_A_BIND) SetPlayerHp(-200.f);
				}
				else if (_ColData.szShapeTag == "FireBall_Body") SetPlayerHp(-150.f);
				else if (_ColData.szShapeTag == "Slash_Body") SetPlayerHp(-50.f);
				else if (_ColData.szShapeTag == "LSlash_Body") SetPlayerHp(-100.f);
				else if (_ColData.szShapeTag == "BigSlash_Body") SetPlayerHp(-200.f);
				else if (_ColData.szShapeTag == "BindMagic_Body") SetPlayerHp(-50.f);

				if (eAbnormal == PJ_A_NORMAL)
				{
					GAMEINSTANCE->PlaySoundW("Player_Hit_Normal", 0.5f);

					if (CheckIgnoreAbnormalAnim(m_eCurrentAnimation))
						return;

					if (CheckSkillAnim(m_eCurrentAnimation))
						return;

					CancleAnimationSet();

					switch (m_eCurrentWeapon)
					{
					case CPlayer::WEAPON_TYPE_SWORD:AssaultAnim(WEAPON_SWORD_DAMAGED_NORMAL_B, 0.05f, 0.7f); break;
					case CPlayer::WEAPON_TYPE_CROSSBOW:AssaultAnim(WEAPON_CROSSBOW_DAMAGED_NORMAL_B, 0.05f, 0.7f); break;
					case CPlayer::WEAPON_TYPE_STAFF:AssaultAnim(WEAPON_STAFF_DAMAGED_NORMAL_B, 0.05f, 0.7f); break;
					default: break;
					}
					
					m_pModelCom->PlayAnimation(0.f);
					return;
				}
				else if (eAbnormal == PJ_A_SHORTHITTED)
				{
					GAMEINSTANCE->PlaySoundW("Player_Hit_Stiff_Short", 0.5f);

					if (CheckIgnoreAbnormalAnim(m_eCurrentAnimation))
						return;

					CancleAnimationSet();

					switch (m_eCurrentWeapon)
					{
					case CPlayer::WEAPON_TYPE_SWORD:AssaultAnim(WEAPON_SWORD_DAMAGED_STIFF_SHORT, 0.05f, 0.8f); break;
					case CPlayer::WEAPON_TYPE_CROSSBOW:AssaultAnim(WEAPON_CROSSBOW_DAMAGED_STIFF_SHORT, 0.05f, 0.8f); break;
					case CPlayer::WEAPON_TYPE_STAFF: AssaultAnim(WEAPON_STAFF_DAMAGED_STIFF_SHORT, 0.05f, 0.8f); break;
					default: break;
					}
				}
				else if (eAbnormal == PJ_A_LONGHITTED)
				{
					GAMEINSTANCE->PlaySoundW("Player_Hit_Stiff_Short", 0.5f);

					if (CheckIgnoreAbnormalAnim(m_eCurrentAnimation))
						return;
					
					CancleAnimationSet();

					switch (m_eCurrentWeapon)
					{							
					case CPlayer::WEAPON_TYPE_SWORD: TriggerAssaultAnimSet("Stiff_Long_Sword",1); break;
					case CPlayer::WEAPON_TYPE_CROSSBOW:TriggerAssaultAnimSet("Stiff_Long_CrossBow", 1); break;
					case CPlayer::WEAPON_TYPE_STAFF: TriggerAssaultAnimSet("Stiff_Long_CrossBow", 1); break;
					default: break;
					}
				}
				else if (eAbnormal == PJ_A_BIND)
				{
					GAMEINSTANCE->PlaySoundW("Player_Hit_Stiff_Short", 0.5f);

					if (CheckIgnoreAbnormalAnim(m_eCurrentAnimation))
						return;

					CancleAnimationSet();

					switch (m_eCurrentWeapon)
					{
					case CPlayer::WEAPON_TYPE_SWORD:AssaultAnim(WEAPON_SWORD_DAMAGED_STUN, 0.05f, 0.8f); break;
					case CPlayer::WEAPON_TYPE_CROSSBOW:AssaultAnim(WEAPON_CROSSBOW_DAMAGED_STUN, 0.05f, 0.8f); break;
					case CPlayer::WEAPON_TYPE_STAFF:AssaultAnim(WEAPON_STAFF_DAMAGED_STUN, 0.05f, 0.8f); break;
					default: break;
					}
				}
				else if (eAbnormal == PJ_A_SHOCK)
				{
					GAMEINSTANCE->PlaySoundW("Player_Hit_Electric", 0.5f);

					if (CheckIgnoreAbnormalAnim(m_eCurrentAnimation))
						return;

					CancleAnimationSet();

					switch (m_eCurrentWeapon)
					{
					case CPlayer::WEAPON_TYPE_SWORD:AssaultAnim(WEAPON_SWORD_DAMAGED_SHOCK, 0.05f, 0.8f); break;
					case CPlayer::WEAPON_TYPE_CROSSBOW:AssaultAnim(WEAPON_CROSSBOW_DAMAGED_SHOCK, 0.05f, 0.8f); break;
					case CPlayer::WEAPON_TYPE_STAFF:AssaultAnim(WEAPON_STAFF_DAMAGED_SHOCK, 0.05f, 0.8f); break;
					default: break;
					}
				}
				else if (eAbnormal == PJ_A_GOOUT)
				{	
					GAMEINSTANCE->PlaySoundW("Player_Hit_Stiff_Long", 0.5f);

					if (CheckIgnoreAbnormalAnim(m_eCurrentAnimation))
						return;
					
					CancleAnimationSet();
					
					m_fDashSpeed = -22.f;
					switch (m_eCurrentWeapon)
					{
					case CPlayer::WEAPON_TYPE_SWORD: TriggerAssaultAnimSet("Down_Long_Sword", 1); break;
					case CPlayer::WEAPON_TYPE_CROSSBOW:TriggerAssaultAnimSet("Down_Long_CrossBow", 1); break;
					case CPlayer::WEAPON_TYPE_STAFF: TriggerAssaultAnimSet("Down_Long_CrossBow", 1); break;
					default: break;
					}
				}

				m_pModelCom->PlayAnimation(0.f);
				return;
			}

			else if (_ColData.eColFlag & COL_MONSTERSKILL)
			{
				//CancleAnimationSet();

				// ÀÏ¹ÝÇÇ°Ý
				if (_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_BladeDance" ||
					_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_BladeDance.001" )
				{
					m_mapAbnormals.emplace(_ColData.szShapeTag, ABNORMAL_NORMAL);
				}

				// ÂªÀº°æÁ÷
				else if (_ColData.szShapeTag == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkdmgA2.001" || 
					_ColData.szShapeTag == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkdmgA.001"  ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgA.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgA_2.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgA_M.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB.003" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB_12.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD.002" ||
					_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingA" ||
					_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingB" )
				{
					m_mapAbnormals.emplace(_ColData.szShapeTag, ABNORMAL_SHORTHITTED);
				}

				// ±ä°æÁ÷
				else if (_ColData.szShapeTag == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkdmgD.001" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_DashAttack_E.001" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB_12.003" || 
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB.005" || 
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB_3.001" ||
					_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_Dash" )
				{
					m_mapAbnormals.emplace(_ColData.szShapeTag, ABNORMAL_LONGHITTED);
				}

				// ³¯¾Æ°¡±â(¾à)
				else if (_ColData.szShapeTag == "M_LivingArmor_Mage_SK.ao|M_LivingArmor_Mage_SkdmgC" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_DirectDamage" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD.007" ||
					_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingB.001")
				{
					m_mapAbnormals.emplace(_ColData.szShapeTag, ABNORMAL_GOOUT);

					m_pTransformCom->LookAtForLandObject(_ColData.pGameObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION));
				}
				 
				// ³¯¾Æ°¡±â(°­)
				else if (_ColData.szShapeTag == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkmovAE" ||
					_ColData.szShapeTag == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkmovBE" || 
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_DirectDamage_Plus" ||
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkmovAE" || 
					_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD.006" ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_SkmovD" ||
					_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_BladeDance.002")
				{
					m_mapAbnormals.emplace(_ColData.szShapeTag, ABNORMAL_GOOUT_STRONG);

					m_pTransformCom->LookAtForLandObject(_ColData.pGameObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION));
				}

				// °¨Àü
				else if (_ColData.szShapeTag == "M_LivingArmor_Mage_SK.ao|M_LivingArmor_Mage_SkdmgD" ||
					_ColData.szShapeTag == "M_LivingArmor_Mage_SK.ao|M_LivingArmor_Mage_SkdmgE" || 
					_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_Dash.001" )
				{
					m_mapAbnormals.emplace(_ColData.szShapeTag, ABNORMAL_SHOCK);
				}

				// ½ºÅÏ(¼Ó¹Ú)
				else if (_ColData.szShapeTag == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkbufA"  ||
					_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkbufA" ||
					_ColData.szShapeTag == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingA.001" )				
				{
					m_mapAbnormals.emplace(_ColData.szShapeTag, ABNORMAL_BIND);
				}

				return;
			}

			else if (_ColData.eColFlag & COL_MONSTERWEAPON)
			{
				if (_ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA" ||
					     _ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA.002" ||
					     _ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA.004" ||
					     _ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB" ||
					     _ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB.002" ||
					     _ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB.004" ||
					     _ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB_M" ||
					     _ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB_M.002" ||
					     _ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC" ||
					     _ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.002" ||
					     _ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.003" ||
					     _ColData.szShapeTag == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.005")
				{
					m_mapAbnormals.emplace(_ColData.szShapeTag, ABNORMAL_SHORTHITTED);
				}
				else
				{
					m_mapAbnormals.emplace(_ColData.szShapeTag, ABNORMAL_NORMAL);
				}

				//if (_ColData.szShapeTag == "M_L1Midget_Footman_SK.ao|M_L1Midget_Footman_SkdmgA" ||
				//	_ColData.szShapeTag == "M_L1Midget_Shaman_SK.ao|M_L1Midget_Shaman_SkdmgC" ||
				//	_ColData.szShapeTag == "M_L1Midget_Sling_SK.ao|M_L1Midget_Sling_USkdmgA" ||
				//	_ColData.szShapeTag == "M_LivingArmor_Soldier_SK.ao|M_LivingArmor_Soldier_SkdmgC" ||
				//	_ColData.szShapeTag == "M_LivingArmor_Soldier_SK.ao|M_LivingArmor_Soldier_SkdmgB" ||
				//	_ColData.szShapeTag == "M_Orc_Warrior_001_SK.ao|M_Orc_Warrior_USkdmgA" ||
				//	_ColData.szShapeTag == "M_Orc_Warrior_001_SK.ao|M_Orc_Warrior_USkdmgB" ||
				//	_ColData.szShapeTag == "M_Orc_Archer_SK.ao|M_Orc_Archer_USkdmgA" ||
				//	_ColData.szShapeTag == "M_Orc_Shaman_SK.ao|M_Orc_Shaman_USkdmgA")
		

				return;
			}
			 

		}
	}

}

void CPlayer::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.eColFlag & COL_STATIC)
	{
		if (((_ColData.szShapeTag == "HeightField") || (_ColData.szShapeTag == "Static_Collider") || (_ColData.szShapeTag == "Pillar_Body")) && (_szMyShapeTag == "Jump_Check"))
		{
			m_isLanding = true;
		}

		else if (((_ColData.szShapeTag == "HeightField") || (_ColData.szShapeTag == "Static_Collider") || (_ColData.szShapeTag == "Pillar_Body")) && (_szMyShapeTag == "Land_Check"))
		{
			m_isLanding = true;
		}
	}

}

void CPlayer::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_szMyShapeTag == "LockOn_Check_Sword" || _szMyShapeTag == "LockOn_Check_CrossBow" || _szMyShapeTag == "LockOn_Check_Staff" || _szMyShapeTag == "LockOn_Check_Boss")
	{
		for (_int i = 0 ;  i < m_pLockOnTargets.size() ; i++)
		{
			if (_ColData.pGameObject.lock() == m_pLockOnTargets[i].lock())
			{
				//dynamic_pointer_cast<CMonster>(m_pLockOnTargets[i].lock())->OffRimLight();
				m_pLockOnTargets[i].lock()->SetEdgeRender(false);
				m_pLockOnTargets.erase(m_pLockOnTargets.begin()+i);
				
				if (m_pLockOnTargets.size() == 0)
				{
					m_iCurrentLockOnCursor = -1;
					m_pCurrentLockOnTarget.reset();
					m_isSeparateLockOn = false;
					m_pCamera.lock()->LockOff();
					UIMGR->SetLockOnTarget(nullptr);
					UIMGR->SetLockOnIcon(false);
				}
				else
				{
					if ( (i <= m_iCurrentLockOnCursor) && (m_iCurrentLockOnCursor!= 0))
					{
						m_iCurrentLockOnCursor--;
					}
					m_pCurrentLockOnTarget = m_pLockOnTargets[m_iCurrentLockOnCursor];
					//dynamic_pointer_cast<CMonster>(m_pCurrentLockOnTarget.lock())->OnRimLight();
					m_pCurrentLockOnTarget.lock()->SetEdgeRender(true);
					UIMGR->SetLockOnTarget(m_pCurrentLockOnTarget.lock());
					if (m_isSeparateLockOn)
					{
						m_pCamera.lock()->LockOn(m_pCurrentLockOnTarget.lock());
						UIMGR->SetLockOnIcon(true);
					}

				}
				break;
			}
		}
	}

	if (_ColData.eColFlag & COL_MONSTER)
	{
		if (_szMyShapeTag == "Player_Body")
		{
			for (_int i = 0; i < m_pTouchedTargets.size(); i++)
			{
				if (_ColData.pGameObject.lock() == m_pTouchedTargets[i].lock())
				{
					m_pTouchedTargets.erase(m_pTouchedTargets.begin() + i);
				}
			}
			return;
		}
	}

	//if (_ColData.eColFlag == COL_STATIC)
	//{
	//	if (((_ColData.szShapeTag == "HeightField") || (_ColData.szShapeTag == "Static_Collider")) && (_szMyShapeTag == "Jump_Check") && !m_isJumpState)
	//	{
	//		switch (m_eCurrentWeapon)
	//		{
	//		case CPlayer::WEAPON_TYPE_SWORD:
	//			ChangeAnim(WEAPON_SWORD_JUMP_LOOP_RUN, 0.1f, true);
	//			break;
	//		case CPlayer::WEAPON_TYPE_CROSSBOW:
	//			ChangeAnim(WEAPON_CROSSBOW_JUMP_LOOP_RUN, 0.1f, true);
	//			break;
	//		case CPlayer::WEAPON_TYPE_STAFF:
	//			ChangeAnim(WEAPON_STAFF_JUMP_LOOP_RUN, 0.1f, true);
	//			break;
	//		default:
	//			break;
	//		}
	//		m_isJumpState = true;
	//		m_eCurrentBaseState = STATE_ACTIVE;
	//		m_pModelCom->PlayAnimation(0.f);
	//	}
	//}
}

void CPlayer::EnableCollision(const char* _strShapeTag)
{
	__super::EnableCollision(_strShapeTag);
}

void CPlayer::DisableCollision(const char* _strShapeTag)
{
	__super::DisableCollision(_strShapeTag);
}

void CPlayer::PlayEffect(string _strGroupID)
{
	m_EffectGroupIDX = EFFECTMGR->PlayEffect(_strGroupID, shared_from_this());
	m_strEffectGroupID = _strGroupID;
}

HRESULT CPlayer::RenderShadow()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 3);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	GAMEINSTANCE->BindLightProjMatrix();

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{

		if (i == 0 || !m_bRenderHair) {
			continue;
		}


		GAMEINSTANCE->BeginAnimModel(m_pModelCom, i);
	}

	return S_OK;
}

void CPlayer::SimulationOff()
{
	m_pRigidBody->SimulationOff();
}

void CPlayer::LockOnChangeToBoss()
{
	for (auto pLockOnTargets : m_pLockOnTargets)
	{
		//dynamic_pointer_cast<CMonster>(pLockOnTargets.lock())->OffRimLight();
		pLockOnTargets.lock()->SetEdgeRender(false);
	}
	m_pLockOnTargets.clear();
	m_pRigidBody->DisableCollision("LockOn_Check_Sword");
	m_pRigidBody->DisableCollision("LockOn_Check_CrossBow");
	m_pRigidBody->DisableCollision("LockOn_Check_Staff");
	m_pRigidBody->DisableCollision("LockOn_Check_Boss");

	m_pRigidBody->EnableCollision("LockOn_Check_Boss");

	m_iLockOnMode = 1;
}

void CPlayer::LockOnChangeToWeapon()
{
	for (auto pLockOnTargets : m_pLockOnTargets)
	{
		//dynamic_pointer_cast<CMonster>(pLockOnTargets.lock())->OffRimLight();
		pLockOnTargets.lock()->SetEdgeRender(false);
	}
	m_pLockOnTargets.clear();
	m_pRigidBody->DisableCollision("LockOn_Check_Sword");
	m_pRigidBody->DisableCollision("LockOn_Check_CrossBow");
	m_pRigidBody->DisableCollision("LockOn_Check_Staff");
	m_pRigidBody->DisableCollision("LockOn_Check_Boss");
	
	if(m_eCurrentWeapon == WEAPON_TYPE_SWORD)
		m_pRigidBody->EnableCollision("LockOn_Check_Sword");
	else if (m_eCurrentWeapon == WEAPON_TYPE_CROSSBOW)
		m_pRigidBody->EnableCollision("LockOn_Check_CrossBow");
	else if (m_eCurrentWeapon == WEAPON_TYPE_STAFF)
		m_pRigidBody->EnableCollision("LockOn_Check_Staff");

	m_iLockOnMode = 0;
}

void CPlayer::LockOnOff()
{
	for (auto pLockOnTargets : m_pLockOnTargets)
	{
		//dynamic_pointer_cast<CMonster>(pLockOnTargets.lock())->OffRimLight();
		pLockOnTargets.lock()->SetEdgeRender(false);
	}
	m_pLockOnTargets.clear();
	m_pRigidBody->DisableCollision("LockOn_Check_Sword");
	m_pRigidBody->DisableCollision("LockOn_Check_CrossBow");
	m_pRigidBody->DisableCollision("LockOn_Check_Staff");
	m_pRigidBody->DisableCollision("LockOn_Check_Boss");

	m_iLockOnMode = 2;
}

void CPlayer::AddSupremeGauge(_float _AddGauge)
{
	m_fCurrentSupremeGauge += _AddGauge;

	if (m_fCurrentSupremeGauge >= m_fSupremeGaugeMax)
	{
		m_fCurrentSupremeGauge = m_fSupremeGaugeMax;
		m_bCanUseSupreme = true;
	}
	else if (m_fCurrentSupremeGauge <= 0.f)
		m_fCurrentSupremeGauge = 0.f;

	weak_ptr<CUIUVBar> supremeBar = dynamic_pointer_cast<CUIUVBar>(UIMGR->FindUI("PCInfoGaugeMP"));
	if (supremeBar.expired())
		return;

	supremeBar.lock()->SetBar(m_fCurrentSupremeGauge, m_fSupremeGaugeMax);
}

void CPlayer::LoopStateFSM(_float _fTimeDelta)
{
	m_fDashSpeed = 0.f;

	if (PlayAnimationSet())
		return;

	if (m_isReserved)
	{
		PlayReservedAnim();
		return;
	}

	if (RopeActionControl())
		return;

	if (CheckWeaponChange())
		return;

	if (SkillControl())
		return;
	
	if (ComboAttackControl(_fTimeDelta))
		return;

	if (MoveControlNormal(_fTimeDelta))
		return;

	// effect test
	if (TestKeyInput())
		return;
	
}

void CPlayer::ActiveStateFSM(_float _fTimeDelta)
{
	if (CheckCanMovement(m_eCurrentAnimation))
		MoveControlSkill(_fTimeDelta);

	if (m_isSeparateLockOn && CheckCanSeparate(m_eCurrentAnimation))
		MoveControlSeparate(_fTimeDelta);
	
	AnimationActive(_fTimeDelta);

	if (m_pModelCom->GetCanCancelAnimation())
	{
		if (PlayAnimationSet())
			return;

		if (SkillControl())
			return;
	}
	if ((m_iComboCount == 0) && (m_pModelCom->GetCanCancelAnimation()))
	{
		if (ComboAttackControl(_fTimeDelta))
			return;
	}
	if (m_isReserved && (m_pModelCom->GetCanCancelAnimation()))
	{
		PlayReservedAnim();
		return;
	}
	
	if (m_iComboCount > 0)
	{
		if (ComboAttackControl(_fTimeDelta))
			return;
	}
	if (m_pModelCom->GetIsFinishedAnimation())
	{
		if (m_isJumpState)
			return;

		m_pWeaponTrail->EndTrail();
		m_eCurrentBaseState = STATE_LOOP;
		LoopStateFSM(_fTimeDelta);
		return;
	}
	if (m_pModelCom->GetCanCancelAnimation() && MoveControlNormal(_fTimeDelta, false) && (!(m_isSeparateLockOn && (CheckCanSeparate(m_eCurrentAnimation) ) ) ) )
	{
		m_pWeaponTrail->EndTrail();
		m_eCurrentBaseState = STATE_LOOP;
		return;
	}
}

void CPlayer::IsDamagedFSM(_float _fTimeDelta)
{
	 AnimationActive(_fTimeDelta);

	if (m_pModelCom->GetCanCancelAnimation())
	{
		if (PlayAnimationSet())
			return;
	}

	if (m_pModelCom->GetIsFinishedAnimation())
	{
		m_pWeaponTrail->EndTrail();
		m_eCurrentBaseState = STATE_LOOP;
		LoopStateFSM(_fTimeDelta);
		return;
	}
	if (m_pModelCom->GetCanCancelAnimation() && MoveControlNormal(_fTimeDelta, false))
	{
		m_pWeaponTrail->EndTrail();
		m_eCurrentBaseState = STATE_LOOP;
		LoopStateFSM(_fTimeDelta);
		return;
	}
}

_bool CPlayer::MoveControlNormal(_float _fTimeDelta, _bool _bMove)
{
	if (m_isAllInputStop)
	{	
		return false;
	}

	_bool isMove = false;
	_bool isDash = false;
	_bool isJump = false;

	if (GAMEINSTANCE->KeyPressing(DIK_W) || GAMEINSTANCE->KeyPressing(DIK_A) || GAMEINSTANCE->KeyPressing(DIK_S) || GAMEINSTANCE->KeyPressing(DIK_D))
	{
		isMove = true;

		if (!_bMove)
			return isMove;

		_float fDgree = 0.f;

		if (GAMEINSTANCE->KeyDown(DIK_SPACE))
			isDash = true;
		else if (GAMEINSTANCE->KeyDown(DIK_E))
			isJump = true;

		if (GAMEINSTANCE->KeyPressing(DIK_W) && GAMEINSTANCE->KeyPressing(DIK_A))
			fDgree = 315.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_W) && GAMEINSTANCE->KeyPressing(DIK_D))
			fDgree = 45.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_S) && GAMEINSTANCE->KeyPressing(DIK_A))
			fDgree = 225.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_S) && GAMEINSTANCE->KeyPressing(DIK_D))
			fDgree = 135.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_W))
			fDgree = 0.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_A))
			fDgree = 270.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_S))
			fDgree = 180.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_D))
			fDgree = 90.f;

		if (isDash)
		{
			switch (m_eCurrentWeapon)
			{
			case CPlayer::WEAPON_TYPE_SWORD:
				SetDirectionFromCamera(fDgree, false);
				m_fDashSpeed = 15.f;
				ChangeAnim(WEAPON_SWORD_ROLLING, 0.05f, false, 0.7f);

				SetUseMotionTrail(true, true, 0.5f, true, _float4(1.f, 0.58f, 0.1f, 0.7f));
				break;

			case CPlayer::WEAPON_TYPE_CROSSBOW:
				SetDirectionFromCamera(fDgree, false);
				m_fDashSpeed = 15.f;
				ChangeAnim(WEAPON_CROSSBOW_ROLLING, 0.05f, false, 0.7f);

				SetUseMotionTrail(true, true, 0.5f, true, _float4(0.f, 1.f, 0.8f, 0.7f));
				break;

			case CPlayer::WEAPON_TYPE_STAFF:
				SetDirectionFromCamera(fDgree, false);
				m_fDashSpeed = 20.f;
				ChangeAnim(WEAPON_STAFF_ROLLING, 0.05f, false, 0.8f);

				SetUseMotionTrail(true, true, 0.5f, true, _float4(0.4f, 0.259f, 1.f, 0.9f));
				break;

			default:
				break;
			}

			return isMove;
		}
		else if(isJump)
		{
			m_pTransformCom->Jump(420.0f);
			m_isJumpState = true;
			m_isJumpMinActive = true;

			switch (m_eCurrentWeapon)
			{
			case CPlayer::WEAPON_TYPE_SWORD:
				TriggerAnimationSet("Jump_Run_Sword", 2);
				break;

			case CPlayer::WEAPON_TYPE_CROSSBOW:
				TriggerAnimationSet("Jump_Run_CrossBow", 2);
				break;

			case CPlayer::WEAPON_TYPE_STAFF:
				TriggerAnimationSet("Jump_Run_Staff", 2);
				break;

			default:
				break;
			}			
			return !isMove;
		}
		else
			SetDirectionFromCamera(fDgree);

		m_pTransformCom->GoStraight(_fTimeDelta);

		switch (m_eCurrentWeapon)
		{
		case CPlayer::WEAPON_TYPE_SWORD:
			ChangeAnim(WEAPON_SWORD_RUN, 0.1f, true);
			break;

		case CPlayer::WEAPON_TYPE_CROSSBOW:
			ChangeAnim(WEAPON_CROSSBOW_RUN, 0.1f, true);
			break;

		case CPlayer::WEAPON_TYPE_STAFF:
			ChangeAnim(WEAPON_STAFF_RUN, 0.1f, true);
			break;

		default:
			break;
		}					
	}

	else
	{
		if (!_bMove)
			return isMove;

		if (GAMEINSTANCE->KeyDown(DIK_E))
			isJump = true;

		if (isJump)
		{
			m_pTransformCom->Jump(420.0f);
			m_isJumpState = true;
			m_isJumpMinActive = true;

			switch (m_eCurrentWeapon)
			{
			case CPlayer::WEAPON_TYPE_SWORD:				
				TriggerAnimationSet("Jump_Normal_Sword", 2);
				break;

			case CPlayer::WEAPON_TYPE_CROSSBOW:
				TriggerAnimationSet("Jump_Normal_CrossBow", 2);
				break;

			case CPlayer::WEAPON_TYPE_STAFF:
				TriggerAnimationSet("Jump_Normal_Staff", 2);
				break;

			default:
				break;
			}
			return !isMove;
		}

		switch (m_eCurrentWeapon)
		{
		case CPlayer::WEAPON_TYPE_SWORD:
			ChangeAnim(WEAPON_SWORD_IDLE, 0.1f, true);
			break;
		
		case CPlayer::WEAPON_TYPE_CROSSBOW:
			ChangeAnim(WEAPON_CROSSBOW_IDLE, 0.1f, true);
			break;
		
		case CPlayer::WEAPON_TYPE_STAFF:
			ChangeAnim(WEAPON_STAFF_IDLE, 0.1f, true);
			break;
		
		default:
			break;
		}	
	}

	return isMove;
}

_bool CPlayer::MoveControlSeparate(_float _fTimeDelta)
{
	if (m_isAllInputStop)
		return false;

	_bool isMove = false; 

	if (GAMEINSTANCE->KeyPressing(DIK_W) || GAMEINSTANCE->KeyPressing(DIK_A) || GAMEINSTANCE->KeyPressing(DIK_S) || GAMEINSTANCE->KeyPressing(DIK_D))
	{
		isMove = true;

		_float fDgree = 0.f;

		if (GAMEINSTANCE->KeyPressing(DIK_W) && GAMEINSTANCE->KeyPressing(DIK_A))
			fDgree = 315.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_W) && GAMEINSTANCE->KeyPressing(DIK_D))
			fDgree = 45.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_S) && GAMEINSTANCE->KeyPressing(DIK_A))
			fDgree = 225.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_S) && GAMEINSTANCE->KeyPressing(DIK_D))
			fDgree = 135.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_W))
			fDgree = 0.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_A))
			fDgree = 270.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_S))
			fDgree = 180.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_D))
			fDgree = 90.f;

		SetSeparateDir(_fTimeDelta, fDgree);		
	}
	return isMove;
}

_bool CPlayer::MoveControlSkill(_float _fTimeDelta)
{
	if (m_isAllInputStop)
		return false;

	_bool isMove = false;

	if (GAMEINSTANCE->KeyPressing(DIK_W) || GAMEINSTANCE->KeyPressing(DIK_A) || GAMEINSTANCE->KeyPressing(DIK_S) || GAMEINSTANCE->KeyPressing(DIK_D))
	{
		isMove = true;

		_float fDgree = 0.f;

		if (GAMEINSTANCE->KeyPressing(DIK_W) && GAMEINSTANCE->KeyPressing(DIK_A))
			fDgree = 315.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_W) && GAMEINSTANCE->KeyPressing(DIK_D))
			fDgree = 45.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_S) && GAMEINSTANCE->KeyPressing(DIK_A))
			fDgree = 225.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_S) && GAMEINSTANCE->KeyPressing(DIK_D))
			fDgree = 135.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_W))
			fDgree = 0.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_A))
			fDgree = 270.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_S))
			fDgree = 180.f;
		else if (GAMEINSTANCE->KeyPressing(DIK_D))
			fDgree = 90.f;

		SetDirectionFromCamera(fDgree, true);
		m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 6.f);

		//m_pTransformCom->GoDirFreeSpeed(_fTimeDelta, SetDirectionFromCamera(fDgree, false, true), 5.f);
	}
	return isMove;
}

_bool CPlayer::ComboAttackControl(_float _fTimeDelta)
{
	if (m_isAllInputStop)
		return false;

	if (m_isUIInputStop)
		return false;

	_bool isAttack = false;

	if (GAMEINSTANCE->MouseDown(DIM_LB) )
	{
		switch (m_eCurrentWeapon)
		{
		case CPlayer::WEAPON_TYPE_SWORD:
			if (m_iComboCount == 0)
			{
				isAttack = true;
				m_iComboCount++;
				m_fDashSpeed = 7.f;
				m_iComboTimer = 1.f;
				SetToLockOnDir();
				ChangeAnim(WEAPON_SWORD_ATTACK_COMBO_1, 0.05f, false, 0.35f);
			}
			else if (m_iComboCount == 1)
			{
				if (ReserveAnim(WEAPON_SWORD_ATTACK_COMBO_2, 7.f, 0.4f))
				{
					isAttack = true;
					m_iComboTimer = 1.f;
				}
			}
			else if (m_iComboCount == 2)
			{
				if (ReserveAnim(WEAPON_SWORD_ATTACK_COMBO_3, 7.f, 0.5f))
				{
					isAttack = true;
					m_iComboTimer = 1.f;
				}
			}
			else if (m_iComboCount == 3)
			{
				if (ReserveAnim(WEAPON_SWORD_ATTACK_COMBO_4, 3.f, 0.8f))
				{
					isAttack = true;
					m_iComboTimer = 1.f;
				}
			}
			break;

		case CPlayer::WEAPON_TYPE_CROSSBOW:
			if (m_iComboCount == 0)
			{
				isAttack = true;
				m_iComboCount++;
				m_iComboTimer = 1.f;
				SetToLockOnDir();
				if(ChangeAnim(WEAPON_CROSSBOW_ATTACK_COMBO_1, 0.05f, false, 0.4f) && m_isSeparateLockOn )
				{ 
					MoveControlSeparate(_fTimeDelta);
				}
				if ((*m_pSeparating))
					ChangeAnim(WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_1, 0.05f, false, 0.35f);
			}
			else if (m_iComboCount == 1)
			{
				if ((*m_pSeparating))
				{
					if (ReserveAnim(WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_2, 0.f, 0.35f))
					{
						isAttack = true;
						m_iComboTimer = 1.f;
					}
				}
				else
				{
					if (ReserveAnim(WEAPON_CROSSBOW_ATTACK_COMBO_2, 0.f, 0.4f))
					{
						isAttack = true;
						m_iComboTimer = 1.f;
					}
				}
			}
			else if (m_iComboCount == 2)
			{
				if ((*m_pSeparating))
				{
					if (ReserveAnim(WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_3, 0.f, 0.45f))
					{
						isAttack = true;
						m_iComboTimer = 1.f;
					}
				}
				else
				{
					if (ReserveAnim(WEAPON_CROSSBOW_ATTACK_COMBO_3, 0.f, 0.4f))
					{
						isAttack = true;
						m_iComboTimer = 1.f;
					}
				}
			}
			else if (m_iComboCount == 3)
			{
				if ((*m_pSeparating))
				{
					if (ReserveAnim(WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_4, 0.f, 0.6f))
					{
						isAttack = true;
						m_iComboTimer = 0.6f;
					}
				}
				else
				{
					if (ReserveAnim(WEAPON_CROSSBOW_ATTACK_COMBO_4, -10.f, 0.7f))
					{
						isAttack = true;
						m_iComboTimer = 1.f;
					}
				}
			}
			break;

		case CPlayer::WEAPON_TYPE_STAFF:
			if (m_iComboCount == 0)
			{
				isAttack = true;
				m_iComboCount++;
				m_iComboTimer = 1.f;
				SetToLockOnDir();
				ChangeAnim(WEAPON_STAFF_ATTACK_COMBO_1, 0.05f, false, 0.4f);
			}
			else if (m_iComboCount == 1)
			{
				if (ReserveAnim(WEAPON_STAFF_ATTACK_COMBO_2, 0.f, 0.33f))
				{
					isAttack = true;
					m_iComboTimer = 1.f;
				}
			}
			else if (m_iComboCount == 2)
			{
				if (ReserveAnim(WEAPON_STAFF_ATTACK_COMBO_3, 0.f, 0.7f))
				{
					isAttack = true;
					m_iComboTimer = 1.f;
				}
			}
			break;

		default:
			break;
		}

		shared_ptr<CUIInstanceQuickSlot> pQuick = dynamic_pointer_cast<CUIInstanceQuickSlot>(UIMGR->FindUI("QuickCombatSlot0"));
		if (pQuick)
			pQuick->SetQuickSlotScaling(1);
	}

	return isAttack;
}

_bool CPlayer::SkillControl()
{
	if (m_isAllInputStop)
		return false;

	if (GAMEINSTANCE->KeyDown(DIK_Q))
	{
		PlaySkill(0);
		shared_ptr<CUIInstanceQuickSlot> pQuick = dynamic_pointer_cast<CUIInstanceQuickSlot>(UIMGR->FindUI("QuickCombatSlot0"));
		if (pQuick)
			pQuick->SetQuickSlotScaling(0);

		UIMGR->ActivateCoolTimeEffect({ -271.f, -258.f });

		return true;
	}
	if (GAMEINSTANCE->KeyDown(DIK_1))
	{
		PlaySkill(1);
		/* Test */
		shared_ptr<CUIInstanceQuickSlot> pQuick = dynamic_pointer_cast<CUIInstanceQuickSlot>(UIMGR->FindUI("QuickMainSlot0"));
		if (pQuick)
			pQuick->SetQuickSlotScaling(0);

		UIMGR->ActivateCoolTimeEffect({ -272.f, -303.5f });

		return true;
	}
	if (GAMEINSTANCE->KeyDown(DIK_2))
	{
		PlaySkill(2);
		shared_ptr<CUIInstanceQuickSlot> pQuick = dynamic_pointer_cast<CUIInstanceQuickSlot>(UIMGR->FindUI("QuickMainSlot0"));
		if (pQuick)
			pQuick->SetQuickSlotScaling(1);

		UIMGR->ActivateCoolTimeEffect({ -231.f, -303.5f });

		return true;
	}
	if (GAMEINSTANCE->KeyDown(DIK_3))
	{
		PlaySkill(3);
		shared_ptr<CUIInstanceQuickSlot> pQuick = dynamic_pointer_cast<CUIInstanceQuickSlot>(UIMGR->FindUI("QuickMainSlot0"));
		if (pQuick)
			pQuick->SetQuickSlotScaling(2);

		UIMGR->ActivateCoolTimeEffect({ -190.f, -303.5f });

		return true;
	}
	if (GAMEINSTANCE->KeyDown(DIK_4))
	{
		PlaySkill(4);
		shared_ptr<CUIInstanceQuickSlot> pQuick = dynamic_pointer_cast<CUIInstanceQuickSlot>(UIMGR->FindUI("QuickMainSlot0"));
		if (pQuick)
			pQuick->SetQuickSlotScaling(3);

		UIMGR->ActivateCoolTimeEffect({ -149.f, -303.5f });

		return true;
	}
	if (GAMEINSTANCE->KeyDown(DIK_5))
	{
		PlaySkill(5);
		shared_ptr<CUIInstanceQuickSlot> pQuick = dynamic_pointer_cast<CUIInstanceQuickSlot>(UIMGR->FindUI("QuickMainSlot0"));
		if (pQuick)
			pQuick->SetQuickSlotScaling(4);

		UIMGR->ActivateCoolTimeEffect({ -108.0f, -303.5f });
		
		return true;
	}
	if (GAMEINSTANCE->KeyDown(DIK_6))
	{
		return PlaySupremeSkill();
	}
	
	return false;
}

_bool CPlayer::CheckWeaponChange()
{
	if (m_isAllInputStop)
		return false;

	_bool isChange = { false };

	if(GAMEINSTANCE->KeyDown(DIK_Z))
	{ 
		isChange = true;
		if (m_eCurrentWeapon == WEAPON_TYPE_SWORD)
			return true;
		m_eCurrentWeapon = WEAPON_TYPE_SWORD;
		m_pCurrentWeapon = m_pPlayerWeapons.find("Part_Sword2h_1")->second;
		m_pCurrentWeapon->SetEdgeRender(true);
		ChangeAnim(WEAPON_SWORD_PUTOUT, 0.05f);
		ChangeTrailType();
		m_bCrossBowBuff = false;
		m_fCrossBowBuffPersistTime = 0.f;

		if (m_iLockOnMode == 0)
		{
			for (auto pLockOnTargets : m_pLockOnTargets)
			{
				//dynamic_pointer_cast<CMonster>(pLockOnTargets.lock())->OffRimLight();
				pLockOnTargets.lock()->SetEdgeRender(false);
			}
			m_pLockOnTargets.clear();

			m_pRigidBody->EnableCollision("LockOn_Check_Sword");
			m_pRigidBody->DisableCollision("LockOn_Check_CrossBow");
			m_pRigidBody->DisableCollision("LockOn_Check_Staff");
			m_pRigidBody->DisableCollision("LockOn_Check_Boss");
		}
	}
	else if (GAMEINSTANCE->KeyDown(DIK_X))
	{
		isChange = true;
		if (m_eCurrentWeapon == WEAPON_TYPE_CROSSBOW)
			return true;
		m_eCurrentWeapon = WEAPON_TYPE_CROSSBOW;
		m_pCurrentWeapon = m_pPlayerWeapons.find("Part_CrossBow_1_L")->second;
		m_pCurrentWeapon->SetEdgeRender(true);
		m_pPlayerWeapons.find("Part_CrossBow_1_R")->second->SetEdgeRender(true); //
		ChangeAnim(WEAPON_CROSSBOW_PUTOUT, 0.05f);
		ChangeTrailType();
		
		if (m_iLockOnMode == 0)
		{
			for (auto pLockOnTargets : m_pLockOnTargets)
			{
				//dynamic_pointer_cast<CMonster>(pLockOnTargets.lock())->OffRimLight();
				pLockOnTargets.lock()->SetEdgeRender(false);
			}
			m_pLockOnTargets.clear();

			m_pRigidBody->DisableCollision("LockOn_Check_Sword");
			m_pRigidBody->EnableCollision("LockOn_Check_CrossBow");
			m_pRigidBody->DisableCollision("LockOn_Check_Staff");
			m_pRigidBody->DisableCollision("LockOn_Check_Boss");
		}
	}
	else if (GAMEINSTANCE->KeyDown(DIK_C))
	{
		isChange = true;
		if (m_eCurrentWeapon == WEAPON_TYPE_STAFF)
			return true;
		m_eCurrentWeapon = WEAPON_TYPE_STAFF;
		m_pCurrentWeapon = m_pPlayerWeapons.find("Part_Staff_1")->second;
		m_pCurrentWeapon->SetEdgeRender(true);
		ChangeAnim(WEAPON_STAFF_PUTOUT, 0.05f);
		ChangeTrailType();
		m_bCrossBowBuff = false;
		m_fCrossBowBuffPersistTime = 0.f;

		if (m_iLockOnMode == 0)
		{
			for (auto pLockOnTargets : m_pLockOnTargets)
			{
				//dynamic_pointer_cast<CMonster>(pLockOnTargets.lock())->OffRimLight();
				pLockOnTargets.lock()->SetEdgeRender(false);
			}
			m_pLockOnTargets.clear();
		
			m_pRigidBody->DisableCollision("LockOn_Check_Sword");
			m_pRigidBody->DisableCollision("LockOn_Check_CrossBow");
			m_pRigidBody->EnableCollision("LockOn_Check_Staff");
			m_pRigidBody->DisableCollision("LockOn_Check_Boss");
		}
	}

	if (isChange)
		m_iComboCount = 0;

	return isChange;
}

void CPlayer::CommonTick(_float _fTimeDelta)
{
	if (m_bHealing)
	{
		m_fHealingDelay += _fTimeDelta;

		if (m_fHealingDelay >= 1.f)
		{
			SetPlayerHp(50.f);
			m_fHealingDelay = 0.f;

#pragma region HP Ratio Update
			weak_ptr<CUIUVBar> hpPanel = dynamic_pointer_cast<CUIUVBar>(UIMGR->FindUI("PCInfoGaugeHP"));
			if (hpPanel.expired())
				return;

			hpPanel.lock()->SetBar(m_fCurrentHp, m_fMaxHp);
#pragma endregion
		}
	}

	CheckTouched();
	LandingControl();
	DamagedStateControl();

	if (m_isJumpMinActive)
	{
		m_fJumpMinActiveTime += _fTimeDelta;

		if (m_fJumpMinActiveTime >= 0.1f)
		{
			m_isJumpMinActive = false;
			m_fJumpMinActiveTime = 0.f;
		}
	}
	if (GAMEINSTANCE->KeyDown(DIK_R))
	{
		if (m_isSeparateLockOn == true)
		{
			m_isSeparateLockOn = false;
			m_pCamera.lock()->LockOff();
			UIMGR->SetLockOnIcon(false);
		}
		else
		{
			if (m_pCurrentLockOnTarget.lock() != nullptr)
			{
				m_isSeparateLockOn = true;
				m_pCamera.lock()->LockOn(m_pCurrentLockOnTarget.lock());
				/* LockOn Icon è«›ë¶½?€æ¹?*/
				UIMGR->SetLockOnIcon(true);
			}
		}
	}

	// ??‰ë¹Ÿ ?â‘¥??
	if (m_fReservationTimer > 0.f)
		m_fReservationTimer -= _fTimeDelta;
	else
		m_isReserved = false;

	// ?„ã…»???¥ë‡ë¦??
	if (m_iComboTimer > 0.f)
		m_iComboTimer -= _fTimeDelta;
	else
		m_iComboCount = 0;

	// Å©·Î½º º¸¿ì ¹öÇÁ Áö¼Ó½Ã°£
	if (m_bCrossBowBuff)
	{
		m_fCrossBowBuffPersistTime -= _fTimeDelta;

		if (m_fCrossBowBuffPersistTime <= 0.f)
			m_bCrossBowBuff = false;
	}
	/* For QuickSlot UI */
	ITEMMGR->BindCurrentWeaponIndex(static_cast<_uint>(m_eCurrentWeapon));

	// ÄðÅ¸ÀÓ
	for (_uint i = 0; i < WEAPON_TYPE_END; i++)
	{
		for (_uint j = 0; j < SWORD_SKILL_END; j++)
		{
			m_fCurrentCoolTime[i][j] -= _fTimeDelta;

			if (static_cast<_uint>(m_eCurrentWeapon) == i)
			{
				if (j == static_cast<_uint>(SWORD_SKILL_PARRYING))
				{
					ITEMMGR->GetSkillCoolTime("QuickCombatSlot0CoolTime", m_fSKillCoolTime[i][j]);
					ITEMMGR->GetCurrentSkillCoolTime("QuickCombatSlot0CoolTime", m_fCurrentCoolTime[i][j]);
				}

				else if (j == static_cast<_uint>(SWORD_SKILL_BRAVE_STRIKE))
				{
					ITEMMGR->GetSkillCoolTime("QuickMainSlot0CoolTime", m_fSKillCoolTime[i][j]);
					ITEMMGR->GetCurrentSkillCoolTime("QuickMainSlot0CoolTime", m_fCurrentCoolTime[i][j]);
				}

				else if (j == static_cast<_uint>(SWORD_SKILL_RUSH_ATTACK))
				{
					ITEMMGR->GetSkillCoolTime("QuickMainSlot1CoolTime", m_fSKillCoolTime[i][j]);
					ITEMMGR->GetCurrentSkillCoolTime("QuickMainSlot1CoolTime", m_fCurrentCoolTime[i][j]);
				}

				else if (j == static_cast<_uint>(SWORD_SKILL_GAIA_CRASH))
				{
					ITEMMGR->GetSkillCoolTime("QuickMainSlot2CoolTime", m_fSKillCoolTime[i][j]);
					ITEMMGR->GetCurrentSkillCoolTime("QuickMainSlot2CoolTime", m_fCurrentCoolTime[i][j]);
				}

				else if (j == static_cast<_uint>(SWORD_SKILL_UPPER_ATTACK))
				{
					ITEMMGR->GetSkillCoolTime("QuickMainSlot3CoolTime", m_fSKillCoolTime[i][j]);
					ITEMMGR->GetCurrentSkillCoolTime("QuickMainSlot3CoolTime", m_fCurrentCoolTime[i][j]);
				}

				else if (j == static_cast<_uint>(SWORD_SKILL_SPIN_ATTACK))
				{
					ITEMMGR->GetSkillCoolTime("QuickMainSlot4CoolTime", m_fSKillCoolTime[i][j]);
					ITEMMGR->GetCurrentSkillCoolTime("QuickMainSlot4CoolTime", m_fCurrentCoolTime[i][j]);
				}
			}

			if ((m_fCurrentCoolTime[i][j] <= 0) && (m_bCanUseSkill[i][j] == false))
			{
				m_fCurrentCoolTime[i][j] = 0;
				m_bCanUseSkill[i][j] = true;

				if ((i == WEAPON_TYPE_CROSSBOW) && (j == SWORD_SKILL_SPIN_ATTACK))
					m_bFuriousFire = false;
			}
		}
	}

	//ÇÊ»ì±â °ÔÀÌÁö °ü·Ã
	//if ((m_fSupremeGaugeMax <= m_fCurrentSupremeGauge) && (!m_bCanUseSupreme))
	//{
	//	m_fCurrentSupremeGauge = m_fSupremeGaugeMax;
	//	m_bCanUseSupreme = true;
	//}

	// Æä¸µ, ÇÇ°Ý °ü·Ã
	m_vecParrieds.clear();
	m_mapAbnormals.clear();
	m_isLanding = false;
}

void CPlayer::TickWeapon(_float _fTimeDelta)
{
	if (m_eCurrentWeapon == WEAPON_TYPE_SWORD)
	{
		m_pPlayerWeapons.find("Part_Sword2h_1")->second->Tick(_fTimeDelta);
	}
	else if (m_eCurrentWeapon == WEAPON_TYPE_CROSSBOW)
	{
		m_pPlayerWeapons.find("Part_CrossBow_1_L")->second->Tick(_fTimeDelta);
		m_pPlayerWeapons.find("Part_CrossBow_1_R")->second->Tick(_fTimeDelta);
	}
	else if (m_eCurrentWeapon == WEAPON_TYPE_STAFF)
	{
		m_pPlayerWeapons.find("Part_Staff_1")->second->Tick(_fTimeDelta);
	}
}

void CPlayer::LateTickWeapon(_float _fTimeDelta)
{
	if ( (ROPE_ACTION_ALL != m_eCurrentAnimation) &&
		(POLYMORPH_HUMAN_TO_WOLF_RUN != m_eCurrentAnimation) &&
		(POLYMORPH_WOLF_TO_HUMAN_RUN != m_eCurrentAnimation))
	{
		if (m_eCurrentWeapon == WEAPON_TYPE_SWORD)
		{
			m_pPlayerWeapons.find("Part_Sword2h_1")->second->LateTick(_fTimeDelta);
		}
		else if (m_eCurrentWeapon == WEAPON_TYPE_CROSSBOW)
		{
			m_pPlayerWeapons.find("Part_CrossBow_1_L")->second->LateTick(_fTimeDelta);
			m_pPlayerWeapons.find("Part_CrossBow_1_R")->second->LateTick(_fTimeDelta);
		}
		else if (m_eCurrentWeapon == WEAPON_TYPE_STAFF)
		{
			m_pPlayerWeapons.find("Part_Staff_1")->second->LateTick(_fTimeDelta);
		}
	}
}

void CPlayer::TickEquipment(_float _fTimeDelta)
{
	for (auto pEquipment : m_pPlayerEquipments)
	{
		pEquipment->Tick(_fTimeDelta);
	}
}

void CPlayer::LateTickEquipment(_float _fTimeDelta)
{
	for (auto pEquipment : m_pPlayerEquipments)
	{
		pEquipment->LateTick(_fTimeDelta);
	}
}

void CPlayer::ChangeEquipment(ITEM_EQUIP_TYPE _eparts, string _ModelName)
{
	m_pPlayerEquipments[_eparts]->SetCurrentModel(_ModelName);

	if (ITEM_EQUIP_HELMET == _eparts)
		m_bRenderHair = false;

	if (ITEM_EQUIP_GLOVE > _eparts)
		m_pMotionTrail[_eparts + 1]->ChangeModel(dynamic_pointer_cast<CModel>(m_pPlayerEquipments[_eparts]->GetComponent(L"Com_Model")));
	else if (ITEM_EQUIP_GLOVE < _eparts)
		m_pMotionTrail[_eparts]->ChangeModel(dynamic_pointer_cast<CModel>(m_pPlayerEquipments[_eparts]->GetComponent(L"Com_Model")));
}

void CPlayer::NoneEquipment(ITEM_EQUIP_TYPE _eparts)
{
	m_pPlayerEquipments[_eparts]->SetNoneModel();
	
	if (ITEM_EQUIP_HELMET == _eparts)
		m_bRenderHair = true;
}

void CPlayer::SetPlayerHp(_float _fHpVariation)
{
	m_fCurrentHp += _fHpVariation;

	if (m_fCurrentHp >= 1000.f)
		m_fCurrentHp = 1000.f;
	else if (m_fCurrentHp <= 0.f)
		m_fCurrentHp = 0.f;

	if (_fHpVariation > 0.f) PlayEffect("Plr_Heal");

	weak_ptr<CUIUVBar> hpPanel = dynamic_pointer_cast<CUIUVBar>(UIMGR->FindUI("PCInfoGaugeHP"));
	if (hpPanel.expired())
		return;

	hpPanel.lock()->SetBar(m_fCurrentHp, m_fMaxHp);
}

void CPlayer::SetAllInputState(_bool _isInputStop)
{
	m_isAllInputStop = _isInputStop;

	if (_isInputStop == true)
	{
		switch (m_eCurrentWeapon)
		{
		case CPlayer::WEAPON_TYPE_SWORD:
			ChangeAnim(WEAPON_SWORD_IDLE, 0.1f, true);
			break;

		case CPlayer::WEAPON_TYPE_CROSSBOW:
			ChangeAnim(WEAPON_CROSSBOW_IDLE, 0.1f, true);
			break;

		case CPlayer::WEAPON_TYPE_STAFF:
			ChangeAnim(WEAPON_STAFF_IDLE, 0.1f, true);
			break;

		default:
			break;
		}
	}
}

void CPlayer::SetRopeTargetPos(_float3 _vRopeTargetPos)
{
	_float3 vMyPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_float3 vRopeTargetPos = _vRopeTargetPos;
	vRopeTargetPos.y += 2.f;
	if (_vRopeTargetPos == _float3(0.f, 0.f, 0.f))
	{
		m_fRopeAnimDur = 0.f;
		m_isRopeTarget = false;
		m_vRopeTargetDir = _float3(0.f, 0.f, 0.f);
	}
	else
	{
		m_fRopeMoveSpeed = 2.5f;
		m_isRopeTarget = true;
		m_vRopeTargetDir = vRopeTargetPos - vMyPos;
	}
}

void CPlayer::SetHealing()
{
	m_bHealing = !m_bHealing;
	m_fHealingDelay = 0.f;
}

void CPlayer::DashMove(_float _fTimeDelta, _float _fRatio)
{
	m_fDashSpeed = GAMEINSTANCE->Lerp(m_fDashSpeed, 0.f, _fRatio);
	m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, m_fDashSpeed);
}

void CPlayer::AnimationActive(_float _fTimeDelta)
{
	switch (m_eCurrentAnimation) 
	{
	case CPlayer::WEAPON_SWORD_ROLLING: DashMove(_fTimeDelta, 0.05f); break;		
	case CPlayer::WEAPON_SWORD_ATTACK_COMBO_1: m_iComboCount = 1; if (m_isTouched)break; DashMove(_fTimeDelta, 0.1f); break;
	case CPlayer::WEAPON_SWORD_ATTACK_COMBO_2: m_iComboCount = 2; if (m_isTouched)break; DashMove(_fTimeDelta, 0.1f); break;
	case CPlayer::WEAPON_SWORD_ATTACK_COMBO_3: m_iComboCount = 3; if (m_isTouched)break; DashMove(_fTimeDelta, 0.1f); break;
	case CPlayer::WEAPON_SWORD_ATTACK_COMBO_4: m_iComboCount = 4; if (m_isTouched)break; DashMove(_fTimeDelta, 0.04f); break;
				
	case CPlayer::WEAPON_CROSSBOW_ROLLING: DashMove(_fTimeDelta, 0.05f); break;
	case CPlayer::WEAPON_CROSSBOW_BACKSTEP: DashMove(_fTimeDelta, 0.05f); break;
	case CPlayer::WEAPON_CROSSBOW_ATTACK_COMBO_1: m_iComboCount = 1; break;
	case CPlayer::WEAPON_CROSSBOW_ATTACK_COMBO_2: m_iComboCount = 2; break;
	case CPlayer::WEAPON_CROSSBOW_ATTACK_COMBO_3: m_iComboCount = 3; break;
	case CPlayer::WEAPON_CROSSBOW_ATTACK_COMBO_4: m_iComboCount = 4; DashMove(_fTimeDelta, 0.1f); break;
		
	case CPlayer::WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_1: m_iComboCount = 1; break;
	case CPlayer::WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_2: m_iComboCount = 2; break;
	case CPlayer::WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_3: m_iComboCount = 3; break;
	case CPlayer::WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_4: m_iComboCount = 4; break;
		
	case CPlayer::WEAPON_STAFF_ROLLING: DashMove(_fTimeDelta, 0.05f); break;			
	case CPlayer::WEAPON_STAFF_ATTACK_COMBO_1: m_iComboCount = 1; break;		
	case CPlayer::WEAPON_STAFF_ATTACK_COMBO_2: m_iComboCount = 2; break;		
	case CPlayer::WEAPON_STAFF_ATTACK_COMBO_3: m_iComboCount = 3; break;
		
	case CPlayer::WEAPON_SWORD_SKILL_RUSH_ATTACK: if (m_isTouched)break; DashMove(_fTimeDelta, 0.07f); break;
	case CPlayer::WEAPON_SWORD_SKILL_GAIA_CRASH: if (m_isTouched)break; DashMove(_fTimeDelta, 0.04f); break;
	case CPlayer::WEAPON_SWORD_SKILL_UPPER_ATTACK: if (m_isTouched)break; DashMove(_fTimeDelta, 0.1f); break;
				
	case CPlayer::WEAPON_SWORD_JUMP_READY_RUN: m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 5.f); break;
	case CPlayer::WEAPON_SWORD_JUMP_MAIN_RUN: m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 5.f); break;
	case CPlayer::WEAPON_SWORD_JUMP_LOOP_RUN: m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 5.f); break;
	
	case CPlayer::WEAPON_CROSSBOW_JUMP_READY: m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 5.f); break;
	case CPlayer::WEAPON_CROSSBOW_JUMP_MAIN_RUN: m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 5.f); break;
	case CPlayer::WEAPON_CROSSBOW_JUMP_LOOP_RUN: m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 5.f); break;
	
	case CPlayer::WEAPON_STAFF_JUMP_READY_RUN: m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 5.f); break;
	case CPlayer::WEAPON_STAFF_JUMP_MAIN_RUN: m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 5.f); break;
	case CPlayer::WEAPON_STAFF_JUMP_LOOP_RUN: m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 5.f); break;

	case CPlayer::WEAPON_SWORD_DAMAGED_DOWN_START: DashMove(_fTimeDelta, 0.07f); break;
	case CPlayer::WEAPON_SWORD_DAMAGED_DOWN_LOOP: DashMove(_fTimeDelta, 0.07f); break;
	case CPlayer::WEAPON_SWORD_DAMAGED_DOWN_END: DashMove(_fTimeDelta, 0.07f); break;
	case CPlayer::WEAPON_SWORD_DAMAGED_DOWN_LONG_START: DashMove(_fTimeDelta, 0.07f); break;
	case CPlayer::WEAPON_SWORD_DAMAGED_DOWN_LONG_LOOP: DashMove(_fTimeDelta, 0.07f); break;
	case CPlayer::WEAPON_SWORD_DAMAGED_DOWN_LONG_END: DashMove(_fTimeDelta, 0.07f); break;
	
	case CPlayer::WEAPON_CROSSBOW_DAMAGED_DOWN_START: DashMove(_fTimeDelta, 0.07f); break;
	case CPlayer::WEAPON_CROSSBOW_DAMAGED_DOWN_LOOP: DashMove(_fTimeDelta, 0.07f); break;
	case CPlayer::WEAPON_CROSSBOW_DAMAGED_DOWN_END: DashMove(_fTimeDelta, 0.07f); break;
	case CPlayer::WEAPON_CROSSBOW_DAMAGED_DOWN_LONG_START: DashMove(_fTimeDelta, 0.07f); break;
	case CPlayer::WEAPON_CROSSBOW_DAMAGED_DOWN_LONG_LOOP: DashMove(_fTimeDelta, 0.07f); break;
	case CPlayer::WEAPON_CROSSBOW_DAMAGED_DOWN_LONG_END: DashMove(_fTimeDelta, 0.07f); break;

	case CPlayer::WEAPON_SWORD_PARRYING: DashMove(_fTimeDelta, 0.1f); break;
	case CPlayer::WEAPON_STAFF_PARRYING: DashMove(_fTimeDelta, 0.1f); break;

	case CPlayer::ROPE_ACTION_ALL: 
		m_fRopeAnimDur += _fTimeDelta;
		if ((!m_isRopeTarget) && (0.2f < m_fRopeAnimDur) && (!m_isGravity))
		{
			m_pRigidBody->GravityOn();
			m_isGravity = true;
			m_eCurrentBaseState = STATE_LOOP;
			LoopStateFSM(_fTimeDelta);
			m_pModelCom->PlayAnimation(0.f);
			m_fRopeAnimDur = 0.f;
			m_bRopeRend = false;
		}
		else if (m_isRopeTarget && (0.2f <= m_fRopeAnimDur) && (m_fRopeAnimDur <= 1.f))
		{
			if (0.8f <= m_fRopeAnimDur)
				m_bRopeRend = false;

			GAMEINSTANCE->SetZoomBlur(0.02f, 0.4f, m_pTransformCom->GetState(CTransform::STATE_POSITION));
			m_pTransformCom->GoDirNotNormalize(_fTimeDelta, m_vRopeTargetDir, m_fRopeMoveSpeed);
			m_fRopeMoveSpeed -= ((2.5f / 0.8f) * _fTimeDelta);
		}
		else if ((1.f < m_fRopeAnimDur) && (!m_isGravity))
		{
			m_pRigidBody->GravityOn();
			m_isGravity = true;
		}
		else if (m_isRopeTarget && (1.f < m_fRopeAnimDur) && (m_fRopeAnimDur <= 1.6f))
			m_pTransformCom->GoStraightFreeSpeed(_fTimeDelta, 1.f);
		
		break;

	default:
		break;
	}
}

_float3 CPlayer::SetDirectionFromCamera(_float _fDgree, _bool _isSoft, _bool _bGetDir)
{
	_float3	CamDir = GAMEINSTANCE->GetCamLook();
	CamDir.y = 0.f;
	CamDir.Normalize();

	_float3 ResultDir;
	SimpleMath::Vector3::TransformNormal(CamDir, SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(_fDgree)), ResultDir);

	if (_bGetDir)
		return ResultDir;

	if (!_isSoft)
	{
		m_pTransformCom->LookAtDir(ResultDir);
		return ResultDir;
	}

	_float fRatio = { 1.f };
	if (0 > XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pTransformCom->GetState(CTransform::STATE_LOOK)), ResultDir)))
		fRatio = 0.499f;
	else
		fRatio = 0.1f;

	ResultDir = SimpleMath::Vector3::Lerp(XMVector3Normalize(m_pTransformCom->GetState(CTransform::STATE_LOOK)), ResultDir, fRatio);
	ResultDir.Normalize();

	m_pTransformCom->LookAtDir(ResultDir);

	return ResultDir;
}

void CPlayer::SetSeparateDir(_float _fTimeDelta , _float _fDgree)
{
	m_pTransformCom->LookAtForLandObject(m_pCurrentLockOnTarget.lock()->GetTransform()->GetState(CTransform::STATE_POSITION));

	_float3	vCamLook = GAMEINSTANCE->GetCamLook();
	vCamLook.y = 0.f;
	vCamLook.Normalize();
	_float3 vMoveDir;
	SimpleMath::Vector3::TransformNormal(vCamLook, SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(_fDgree)), vMoveDir);

	m_pTransformCom->GoDirFreeSpeed(_fTimeDelta, vMoveDir, 4.f);

	_float3 vPlayerLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);	

	_float fAngle;

	if (XMVectorGetY(XMVector3Cross(vPlayerLook, vMoveDir)) > 0)
	{
		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLook, vMoveDir));
		fAngle = acos(fDot);
		fAngle = XMConvertToDegrees(fAngle);
	}
	else if (XMVectorGetY(XMVector3Cross(vPlayerLook, vMoveDir)) < 0)
	{
		_float fDot = -(XMVectorGetX(XMVector3Dot(vPlayerLook, vMoveDir)));
		fAngle = acos(fDot);
		fAngle = XMConvertToDegrees(fAngle) + 180.f;
	}
	else
	{
		_float fDot = (XMVectorGetX(XMVector3Dot(vPlayerLook, vMoveDir)));
		if (fDot > 0)
			fAngle = 0.f;
		else
			fAngle = 180.f;
	}

	if(( 337.5f <= fAngle && fAngle <= 360.f ) || ( 0.f <= fAngle && fAngle < 22.5f ) )
		m_pModelCom->ChangeLowerAnim(LOCK_ON_MOVE_F, 0.05f, true);
	else if (22.5f <= fAngle && fAngle < 67.5f)
		m_pModelCom->ChangeLowerAnim(LOCK_ON_MOVE_FR, 0.05f, true);
	else if (67.5f <= fAngle && fAngle < 112.5f)
		m_pModelCom->ChangeLowerAnim(LOCK_ON_MOVE_R, 0.05f, true);
	else if (112.5f <= fAngle && fAngle < 157.5f)
		m_pModelCom->ChangeLowerAnim(LOCK_ON_MOVE_BR, 0.05f, true);
	else if (157.5f <= fAngle && fAngle < 202.5f)
		m_pModelCom->ChangeLowerAnim(LOCK_ON_MOVE_B, 0.05f, true);
	else if (202.5f <= fAngle && fAngle < 247.5f)
		m_pModelCom->ChangeLowerAnim(LOCK_ON_MOVE_BL, 0.05f, true);
	else if (247.5f <= fAngle && fAngle < 292.5f)
		m_pModelCom->ChangeLowerAnim(LOCK_ON_MOVE_L, 0.05f, true);
	else if (292.5f <= fAngle && fAngle < 337.5f)
		m_pModelCom->ChangeLowerAnim(LOCK_ON_MOVE_FL, 0.05f, true);

}

void CPlayer::SetToLockOnDir()
{
	if (m_pCurrentLockOnTarget.lock() == nullptr)
		return;

	m_pTransformCom->LookAtForLandObject(m_pCurrentLockOnTarget.lock()->GetTransform()->GetState(CTransform::STATE_POSITION));
}

_bool CPlayer::ChangeAnim(_uint _iAnimIndex, _float _fChangingDelay, _bool _isLoop, _float _fCancleRatio)
{
	if (!_isLoop)
		m_eCurrentBaseState = STATE_ACTIVE;

	_bool bSeparate = false;

	if (MoveControlNormal(0.f, false))
	{
		bSeparate = CheckCanSeparate(_iAnimIndex);
	}

	m_pModelCom->ChangeAnim(_iAnimIndex, _fChangingDelay, _isLoop, _fCancleRatio);
	m_eCurrentAnimation = (PLAYER_ANIMATION)_iAnimIndex;
	
	return bSeparate;
}

_bool CPlayer::AssaultAnim(_uint _iAnimIndex, _float _fChangingDelay, _float _fCancleRatio)
{
	m_eCurrentBaseState = STATE_ISDAMAGED;
	m_pWeaponTrail->EndTrail();
	m_pModelCom->ChangeAnim(_iAnimIndex, _fChangingDelay, false, _fCancleRatio);
	m_eCurrentAnimation = (PLAYER_ANIMATION)_iAnimIndex;
	
	return false;
}

void CPlayer::TriggerAssaultAnimSet(string _strSetName, _uint _SetFlag)
{
	m_iSetTrigger = _SetFlag;
	m_vecAnimationSet = m_AnimationSets.find(_strSetName)->second;
	AssaultAnim(get<0>(m_vecAnimationSet[0]), get<1>(m_vecAnimationSet[0]), get<2>(m_vecAnimationSet[0]));
}

void CPlayer::DamagedStateControl()
{
	_int iAbnormal = { -1 };

	for (auto Abnormal : m_mapAbnormals)
	{
		auto iter = find(m_vecParrieds.begin(), m_vecParrieds.end(), Abnormal.first);
		
		if (iter == m_vecParrieds.end())
		{
			if(Abnormal.first == "M_L1Midget_Footman_SK.ao|M_L1Midget_Footman_SkdmgA")	SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_L1Midget_Shaman_SK.ao|M_L1Midget_Shaman_SkdmgC") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_L1Midget_Sling_SK.ao|M_L1Midget_Sling_USkdmgA") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkdmgD.001") SetPlayerHp(-200.f);
			else if (Abnormal.first == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkdmgA2.001") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkdmgA.001") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkmovAE") SetPlayerHp(-300.f);
			else if (Abnormal.first == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkmovBE") SetPlayerHp(-400.f);
			else if (Abnormal.first == "IT_N_Sword_DemonGoat_Butcher_Body") SetPlayerHp(-50.f);
			else if (Abnormal.first == "M_LivingArmor_Soldier_SK.ao|M_LivingArmor_Soldier_SkdmgC") SetPlayerHp(-50.f);
			else if (Abnormal.first == "M_LivingArmor_Soldier_SK.ao|M_LivingArmor_Soldier_SkdmgB") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_LivingArmor_Mage_SK.ao|M_LivingArmor_Mage_SkdmgC") SetPlayerHp(-300.f);
			else if (Abnormal.first == "M_LivingArmor_Mage_SK.ao|M_LivingArmor_Mage_SkdmgE") SetPlayerHp(-200.f);
			else if (Abnormal.first == "M_Orc_Warrior_001_SK.ao|M_Orc_Warrior_USkdmgA") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_Orc_Warrior_001_SK.ao|M_Orc_Warrior_USkdmgB") SetPlayerHp(-150.f);
			else if (Abnormal.first == "M_Orc_Archer_SK.ao|M_Orc_Archer_USkdmgA") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_Orc_Shaman_SK.ao|M_Orc_Shaman_USkdmgA") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_L1Midget_Berserker_SK.ao|M_L1Midget_Berserker_SkdmgA.001") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgA.001") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgA_2.001") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgA_M.001") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB.001") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB.003") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB_12.001") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD.002") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB_12.003") SetPlayerHp(-200.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB.005") SetPlayerHp(-200.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkmovAE") SetPlayerHp(-300.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgB_3.001") SetPlayerHp(-300.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD.007") SetPlayerHp(-200.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkdmgD.006") SetPlayerHp(-300.f);
			else if (Abnormal.first == "M_Orc_LowDarkCommander_SK.ao|M_Orc_LowDarkCommander_SkbufA") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA.002") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgA.004") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB.002") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB.004") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB_M") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgB_M.002") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.002") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.003") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkdmgC.005") SetPlayerHp(-100.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_DashAttack_E.001") SetPlayerHp(-200.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_USkbufA") SetPlayerHp(-50.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_DirectDamage") SetPlayerHp(-200.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_DirectDamage_Plus") SetPlayerHp(-1000.f);
			else if (Abnormal.first == "M_UndeadArmy_Knight_SK.ao|M_UndeadArmy_Knight_SkmovD") SetPlayerHp(-300.f);
			else if (Abnormal.first == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_BladeDance") SetPlayerHp(-50.f);
			else if (Abnormal.first == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_BladeDance.001") SetPlayerHp(-50.f);
			else if (Abnormal.first == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_BladeDance.002") SetPlayerHp(-100.f);
			else if (Abnormal.first == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_Dash") SetPlayerHp(-100.f);
			else if (Abnormal.first == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_Dash.001") SetPlayerHp(-100.f);
			else if (Abnormal.first == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingA") SetPlayerHp(-50.f);
			else if (Abnormal.first == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingA.001") SetPlayerHp(-50.f);
			else if (Abnormal.first == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingB") SetPlayerHp(-50.f);
			else if (Abnormal.first == "CT_Named_ArthurTalon_SK.ao|CT_Named_ArthurTalon_SwingB.001") SetPlayerHp(-50.f);

			//if (Abnormal.second == PLAYER_ABNORMAL::ABNORMAL_NORMAL)
			//	SetPlayerHp(-10.f);
			//else if (Abnormal.second == PLAYER_ABNORMAL::ABNORMAL_SHORTHITTED)
			//	SetPlayerHp(-20.f);
			//else if (Abnormal.second == PLAYER_ABNORMAL::ABNORMAL_LONGHITTED)
			//	SetPlayerHp(-30.f);
			//else if (Abnormal.second == PLAYER_ABNORMAL::ABNORMAL_BIND)
			//	SetPlayerHp(-40.f);
			//else if (Abnormal.second == PLAYER_ABNORMAL::ABNORMAL_SHOCK)
			//	SetPlayerHp(-50.f);
			//else if (Abnormal.second == PLAYER_ABNORMAL::ABNORMAL_GOOUT)
			//	SetPlayerHp(-60.f);
			//else if (Abnormal.second == PLAYER_ABNORMAL::ABNORMAL_GOOUT_STRONG)
			//	SetPlayerHp(-70.f);

			if (iAbnormal < Abnormal.second)
				iAbnormal = Abnormal.second;
		}
	}

	if (-1 != iAbnormal)
	{
		if (CheckIgnoreAbnormalAnim(m_eCurrentAnimation))
			return;

		CancleAnimationSet();

		// æ¹²ê³•????¨êº½
		if (iAbnormal == PLAYER_ABNORMAL::ABNORMAL_NORMAL)
		{
			GAMEINSTANCE->PlaySoundW("Player_Hit_Normal",0.5f);

			if (CheckSkillAnim(m_eCurrentAnimation))
				return;
			
			EFFECTMGR->StopEffect(m_strEffectGroupID, m_EffectGroupIDX);

			switch (m_eCurrentWeapon)
			{
			case CPlayer::WEAPON_TYPE_SWORD:AssaultAnim(WEAPON_SWORD_DAMAGED_NORMAL_B, 0.05f, 0.7f); break;
			case CPlayer::WEAPON_TYPE_CROSSBOW:AssaultAnim(WEAPON_CROSSBOW_DAMAGED_NORMAL_B, 0.05f, 0.7f); break;
			case CPlayer::WEAPON_TYPE_STAFF:AssaultAnim(WEAPON_STAFF_DAMAGED_NORMAL_B, 0.05f, 0.7f); break;
			default: break;
			}
		}
		// ÂªÀº°æÁ÷
		else if (iAbnormal == PLAYER_ABNORMAL::ABNORMAL_SHORTHITTED)
		{
			GAMEINSTANCE->PlaySoundW("Player_Hit_Stiff_Short", 0.5f);

			EFFECTMGR->StopEffect(m_strEffectGroupID, m_EffectGroupIDX);

			switch (m_eCurrentWeapon)
			{
			case CPlayer::WEAPON_TYPE_SWORD:AssaultAnim(WEAPON_SWORD_DAMAGED_STIFF_SHORT, 0.05f, 0.8f); break;
			case CPlayer::WEAPON_TYPE_CROSSBOW:AssaultAnim(WEAPON_CROSSBOW_DAMAGED_STIFF_SHORT, 0.05f, 0.8f); break;
			case CPlayer::WEAPON_TYPE_STAFF: AssaultAnim(WEAPON_STAFF_DAMAGED_STIFF_SHORT, 0.05f, 0.8f); break;
			default: break;
			}
		}

		// ±ä°æÁ÷
		else if (iAbnormal == PLAYER_ABNORMAL::ABNORMAL_LONGHITTED)
		{
			GAMEINSTANCE->PlaySoundW("Player_Hit_Stiff_Short", 0.5f);

			EFFECTMGR->StopEffect(m_strEffectGroupID, m_EffectGroupIDX);

			switch (m_eCurrentWeapon)
			{
			case CPlayer::WEAPON_TYPE_SWORD: TriggerAssaultAnimSet("Stiff_Long_Sword", 1); break;
			case CPlayer::WEAPON_TYPE_CROSSBOW:TriggerAssaultAnimSet("Stiff_Long_CrossBow", 1); break;
			case CPlayer::WEAPON_TYPE_STAFF: TriggerAssaultAnimSet("Stiff_Long_CrossBow", 1); break;
			default: break;
			}
		}
		
		// ¼Ó¹Ú(½ºÅÏ)
		else if (iAbnormal == PLAYER_ABNORMAL::ABNORMAL_BIND)
		{
			GAMEINSTANCE->PlaySoundW("Player_Hit_Stiff_Short", 0.5f);

			EFFECTMGR->StopEffect(m_strEffectGroupID, m_EffectGroupIDX);

			switch (m_eCurrentWeapon)
			{
			case CPlayer::WEAPON_TYPE_SWORD:AssaultAnim(WEAPON_SWORD_DAMAGED_STUN, 0.05f, 0.8f); break;
			case CPlayer::WEAPON_TYPE_CROSSBOW:AssaultAnim(WEAPON_CROSSBOW_DAMAGED_STUN, 0.05f, 0.8f); break;
			case CPlayer::WEAPON_TYPE_STAFF:AssaultAnim(WEAPON_STAFF_DAMAGED_STUN, 0.05f, 0.8f); break;
			default: break;
			}
		}

		// °¨Àü
		else if (iAbnormal == PLAYER_ABNORMAL::ABNORMAL_SHOCK)
		{
			GAMEINSTANCE->PlaySoundW("Player_Hit_Electric", 0.5f);

			EFFECTMGR->StopEffect(m_strEffectGroupID, m_EffectGroupIDX);

			switch (m_eCurrentWeapon)
			{
			case CPlayer::WEAPON_TYPE_SWORD:AssaultAnim(WEAPON_SWORD_DAMAGED_SHOCK, 0.05f, 0.8f); break;
			case CPlayer::WEAPON_TYPE_CROSSBOW:AssaultAnim(WEAPON_CROSSBOW_DAMAGED_SHOCK, 0.05f, 0.8f); break;
			case CPlayer::WEAPON_TYPE_STAFF:AssaultAnim(WEAPON_STAFF_DAMAGED_SHOCK, 0.05f, 0.8f); break;
			default: break;
			}
		}

		// ³¯¾Æ°¡±â(¾à)
		else if (iAbnormal == PLAYER_ABNORMAL::ABNORMAL_GOOUT)
		{
			GAMEINSTANCE->PlaySoundW("Player_Hit_Stiff_Long", 0.5f);

			EFFECTMGR->StopEffect(m_strEffectGroupID, m_EffectGroupIDX);

			m_fDashSpeed = -15.f;
			switch (m_eCurrentWeapon)
			{
			case CPlayer::WEAPON_TYPE_SWORD: TriggerAssaultAnimSet("Down_Sword", 1); break;
			case CPlayer::WEAPON_TYPE_CROSSBOW:TriggerAssaultAnimSet("Down_CrossBow", 1); break;
			case CPlayer::WEAPON_TYPE_STAFF: TriggerAssaultAnimSet("Down_CrossBow", 1); break;
			default: break;
			}
		}

		// ³¯¾Æ°¡±â(°­)
		else if (iAbnormal == PLAYER_ABNORMAL::ABNORMAL_GOOUT_STRONG)
		{
			GAMEINSTANCE->PlaySoundW("Player_Hit_Stiff_Long", 0.5f);

			EFFECTMGR->StopEffect(m_strEffectGroupID, m_EffectGroupIDX);

			m_fDashSpeed = -22.f;
			switch (m_eCurrentWeapon)
			{
			case CPlayer::WEAPON_TYPE_SWORD: TriggerAssaultAnimSet("Down_Long_Sword", 1); break;
			case CPlayer::WEAPON_TYPE_CROSSBOW:TriggerAssaultAnimSet("Down_Long_CrossBow", 1); break;
			case CPlayer::WEAPON_TYPE_STAFF: TriggerAssaultAnimSet("Down_Long_CrossBow", 1); break;
			default: break;
			}
		}
		m_pModelCom->PlayAnimation(0.f);
	}
	else
	{

	}
}

_bool CPlayer::RopeActionControl()
{
	if(m_isAllInputStop)
		return false;

	if(m_isUIInputStop || m_isSeparateLockOn)
		return false;

	if (GAMEINSTANCE->MouseDown(DIM_RB))
	{
		UIMGR->ActivateCrossHair();
		m_pCamera.lock()->CameraZoom(true);
		m_bCrossHair = true;
	}
	if (GAMEINSTANCE->MouseUp(DIM_RB))
	{
		UIMGR->DeactivateCrossHair();
		m_pCamera.lock()->CameraZoom(false);
		m_bCrossHair = false;
	}
	if (GAMEINSTANCE->KeyDown(DIK_V) && m_bCrossHair)
	{
		UIMGR->DeactivateCrossHair();
		m_pCamera.lock()->CameraZoom(false);
		m_bCrossHair = false;

		_float3 vCamDir = GAMEINSTANCE->GetCamLook();
		m_pTransformCom->LookAtDirForLandObject(vCamDir);
		ChangeAnim(ROPE_ACTION_ALL, 0.05f, false);

		return true;
	}
	return false;
}

_bool CPlayer::ReserveAnim(PLAYER_ANIMATION _eReservationAnim, _float _fDashSpeed, _float _fCancelRatio)
{
	if (m_isReserved)
		return false;

	m_isReserved = true;
	m_fReservationTimer = 0.5f;
	m_eReservedAnimation = _eReservationAnim;
	m_fReserveDashSpeed = _fDashSpeed;
	m_fReserveCancelRatio = _fCancelRatio;

	return true;
}

void CPlayer::PlayReservedAnim()
{
	m_fDashSpeed = m_fReserveDashSpeed; 
	SetToLockOnDir();
	ChangeAnim(m_eReservedAnimation, 0.05f, false, m_fReserveCancelRatio);
	m_fReservationTimer = 0.f;
	m_isReserved = false;
}

void CPlayer::TriggerAnimationSet(string _strSetName, _uint _SetFlag)
{
	m_iSetTrigger = _SetFlag;
	m_vecAnimationSet = m_AnimationSets.find(_strSetName)->second;
	ChangeAnim( get<0>(m_vecAnimationSet[0]), get<1>(m_vecAnimationSet[0]),false, get<2>(m_vecAnimationSet[0]));
}


_bool CPlayer::PlaySkill(_uint _iSkillIndex)
{
	if (m_bCanUseSkill[m_eCurrentWeapon][_iSkillIndex])
	{
		if(0 != _iSkillIndex)
			SetToLockOnDir();

		switch (m_eCurrentWeapon)
		{
		case CPlayer::WEAPON_TYPE_SWORD:
			switch (_iSkillIndex)
			{
			case CPlayer::SWORD_SKILL_PARRYING:
				m_pWeaponTrail->StartTrail(_float4(0.98f, 0.576f, 0.31f, 1.f), _float4(0.98f, 0.89f, 0.48f, 1.f), true, true, true, 1.f);
				ChangeAnim(WEAPON_SWORD_PARRYING, 0.05f, false, 0.7f);
				break;

			case CPlayer::SWORD_SKILL_BRAVE_STRIKE:
				TriggerAnimationSet("Brave_Strike",1);
				SetWeaponGlow(true, true, _float4(0.98f, 0.592f, 0.318f, 0.8f), true, 1.5f);
				SetWeaponMeshTrail(true, _float4(0.98f, 0.592f, 0.318f, 1.0f), true, 1.5f);
				m_pWeaponTrail->StartTrail(_float4(0.98f, 0.576f, 0.31f, 1.f), _float4(0.98f, 0.89f, 0.48f, 1.f), true, true, true, 0.5f);
				AddSupremeGauge(10.f);
				break;

			case CPlayer::SWORD_SKILL_RUSH_ATTACK:
				m_fDashSpeed = 25.f;
				ChangeAnim(WEAPON_SWORD_SKILL_RUSH_ATTACK, 0.05f, false, 0.7f);
				SetWeaponGlow(true, true, _float4(0.98f, 0.592f, 0.318f, 0.8f), true, 1.f);
				SetWeaponMeshTrail(true, _float4(0.98f, 0.592f, 0.318f, 1.0f), true, 1.f);
				m_pWeaponTrail->StartTrail(_float4(0.98f, 0.576f, 0.31f, 1.f), _float4(0.98f, 0.89f, 0.48f, 1.f), true, true, true, 1.5f);
				AddSupremeGauge(10.f);
				break;

			case CPlayer::SWORD_SKILL_GAIA_CRASH:
				m_fDashSpeed = 15.f;
				ChangeAnim(WEAPON_SWORD_SKILL_GAIA_CRASH, 0.05f, false, 0.8f);
				PlayEffect("Plr_GaiaCrash");
				SetWeaponGlow(true, true, _float4(0.98f, 0.592f, 0.318f, 0.8f), true, 1.8f);
				SetWeaponMeshTrail(true, _float4(0.98f, 0.592f, 0.318f, 1.0f), true, 2.f);
				m_pWeaponTrail->StartTrail(_float4(0.98f, 0.576f, 0.31f, 1.f), _float4(0.98f, 0.89f, 0.48f, 1.f), true, true, true, 0.4f);
				AddSupremeGauge(10.f);
				break;

			case CPlayer::SWORD_SKILL_UPPER_ATTACK:
				m_fDashSpeed = 15.f;
				ChangeAnim(WEAPON_SWORD_SKILL_UPPER_ATTACK, 0.05f, false, 0.8f);
				SetWeaponGlow(true, true, _float4(0.98f, 0.592f, 0.318f, 0.8f), true, 0.9f);
				SetWeaponMeshTrail(true, _float4(0.98f, 0.592f, 0.318f, 1.0f), true, 0.9f);
				m_pWeaponTrail->StartTrail(_float4(0.98f, 0.592f, 0.318f, 1.0f), _float4(1.f, 1.f, 1.f, 1.f), true, true, true, 1.5f);
				AddSupremeGauge(10.f);
				break;

			case CPlayer::SWORD_SKILL_SPIN_ATTACK:
				TriggerAnimationSet("Spin_Attack",1);
				SetWeaponGlow(true, true, _float4(0.98f, 0.592f, 0.318f, 0.8f), true, 4.3f);
				SetWeaponMeshTrail(true, _float4(0.98f, 0.592f, 0.318f, 1.0f), true, 4.5f);
				m_pWeaponTrail->StartTrail(_float4(0.98f, 0.576f, 0.31f, 1.f), _float4(0.98f, 0.89f, 0.48f, 1.f), true, true, false, 1.f);
				AddSupremeGauge(10.f);
				break;

			default:
				break;
			}

			break;

		case CPlayer::WEAPON_TYPE_CROSSBOW:
			switch (_iSkillIndex)
			{
			case CPlayer::CROSSBOW_SKILL_PARRYING:
				ChangeAnim(WEAPON_CROSSBOW_PARRYING, 0.05f, false, 0.7f);
				//SetWeaponGlow(true, true, _float4(0.31f, 0.98f, 0.89f, 0.8f), true, 1.5f);
				SetWeaponMeshTrail(true, _float4(0.31f, 0.98f, 0.89f, 1.0f), true, 1.5f);
				//m_pWeaponTrail->StartTrail(_float4(0.31f, 0.98f, 0.89f, 1.f), _float4(0.48f, 0.98f, 0.89f, 1.f), true, true, true, 1.f);				
				break;

			case CPlayer::CROSSBOW_SKILL_RAPIDSHOT:
				ChangeAnim(WEAPON_CROSSBOW_SKILL_RAPIDSHOT, 0.05f, false, 0.6f);
				SetWeaponMeshTrail(true, _float4(0.31f, 0.98f, 0.89f, 1.0f), true, 1.5f);
				//m_pWeaponTrail->StartTrail(_float4(0.31f, 0.98f, 0.89f, 1.f), _float4(0.48f, 0.98f, 0.89f, 1.f), true, true, true, 1.f);
				break;
			
			case CPlayer::CROSSBOW_SKILL_MULTIPLESHOT:
				ChangeAnim(WEAPON_CROSSBOW_SKILL_MULTIPLESHOT, 0.05f, false, 0.5f);
				SetWeaponMeshTrail(true, _float4(0.31f, 0.98f, 0.89f, 1.0f), true, 1.5f);
				//m_pWeaponTrail->StartTrail(_float4(0.31f, 0.98f, 0.89f, 1.f), _float4(0.48f, 0.98f, 0.89f, 1.f), true, true, true, 1.f);			
				AddSupremeGauge(10.f);
				break;
			
			case CPlayer::CROSSBOW_SKILL_INDOMITABLESPIRIT:
				ChangeAnim(WEAPON_CROSSBOW_SKILL_INDOMITABLESPIRIT, 0.05f, false, 0.3f);
				m_fCrossBowBuffPersistTime = 10.f;
				m_bCrossBowBuff = true;
				SetWeaponMeshTrail(true, _float4(0.31f, 0.98f, 0.89f, 1.0f), true, 1.5f);
				//m_pWeaponTrail->StartTrail(_float4(0.31f, 0.98f, 0.89f, 1.f), _float4(0.48f, 0.98f, 0.89f, 1.f), true, true, true, 1.f);
				AddSupremeGauge(10.f);
				break;
			
			case CPlayer::CROSSBOW_SKILL_ELECTRICSHOT:
				ChangeAnim(WEAPON_CROSSBOW_SKILL_ELECTRICSHOT, 0.05f, false, 0.7f);
				SetWeaponMeshTrail(true, _float4(0.31f, 0.98f, 0.89f, 1.0f), true, 1.5f);
				//m_pWeaponTrail->StartTrail(_float4(0.31f, 0.98f, 0.89f, 1.f), _float4(0.48f, 0.98f, 0.89f, 1.f), true, true, true, 1.f);
				AddSupremeGauge(10.f);
				break;
			
			case CPlayer::CROSSBOW_SKILL_FURIOUSFIRE:
			{
				TriggerAnimationSet("Furious_Fire", 1);
				SetWeaponMeshTrail(true, _float4(0.31f, 0.98f, 0.89f, 1.0f), true, 2.5f);
				//m_pWeaponTrail->StartTrail(_float4(0.31f, 0.98f, 0.89f, 1.f), _float4(0.48f, 0.98f, 0.89f, 1.f), true, true, true, 1.f);
				CCameraMgr::GetInstance()->FilmCameraTurnOn("Supreme_CrossBow", m_pTransformCom->GetState(CTransform::STATE_LOOK), m_pTransformCom->GetState(CTransform::STATE_POSITION));
				AddSupremeGauge(10.f);
			}
			break;

			default:
				break;
			}
			break;


		case CPlayer::WEAPON_TYPE_STAFF:
			switch (_iSkillIndex)
			{
			case CPlayer::STAFF_SKILL_PARRYING:
				ChangeAnim(WEAPON_STAFF_PARRYING, 0.05f, false, 0.7f);
				//SetWeaponGlow(true, true, _float4(0.31f, 0.98f, 0.89f, 0.8f), true, 1.5f);
				SetWeaponMeshTrail(true, _float4(0.31f, 0.31f, 0.89f, 1.0f), true, 1.5f);
				//m_pWeaponTrail->StartTrail(_float4(0.31f, 0.98f, 0.89f, 1.f), _float4(0.48f, 0.98f, 0.89f, 1.f), true, true, true, 1.f);
				break;

			case CPlayer::STAFF_SKILL_FIREBALL:
				ChangeAnim(WEAPON_STAFF_SKILL_FIREBALL, 0.05f, false, 0.6f);
				SetWeaponMeshTrail(true, _float4(0.31f, 0.31f, 0.89f, 1.0f), true, 1.5f);
				//m_pWeaponTrail->StartTrail(_float4(0.31f, 0.98f, 0.89f, 1.f), _float4(0.48f, 0.98f, 0.89f, 1.f), true, true, true, 1.f);
				AddSupremeGauge(10.f);
				break;

			case CPlayer::STAFF_SKILL_ICESPEAR:
				ChangeAnim(WEAPON_STAFF_SKILL_ICESPEAR, 0.05f, false, 0.6f);
				SetWeaponMeshTrail(true, _float4(0.31f, 0.31f, 0.89f, 1.0f), true, 1.5f);
				//m_pWeaponTrail->StartTrail(_float4(0.31f, 0.98f, 0.89f, 1.f), _float4(0.48f, 0.98f, 0.89f, 1.f), true, true, true, 1.f);			
				AddSupremeGauge(10.f);
				break;

			case CPlayer::STAFF_SKILL_FIREBLAST:
				ChangeAnim(WEAPON_STAFF_SKILL_FIREBLAST, 0.05f, false, 0.6f);
				SetWeaponMeshTrail(true, _float4(0.31f, 0.31f, 0.89f, 1.0f), true, 1.5f);
				//m_pWeaponTrail->StartTrail(_float4(0.31f, 0.98f, 0.89f, 1.f), _float4(0.48f, 0.98f, 0.89f, 1.f), true, true, true, 1.f);
				AddSupremeGauge(10.f);
				break;

			case CPlayer::STAFF_SKILL_LIGHTNIGJUDGMENT:
				ChangeAnim(WEAPON_STAFF_SKILL_LIGHTNIGJUDGMENT, 0.05f, false, 0.6f);
				SetWeaponMeshTrail(true, _float4(0.31f, 0.31f, 0.89f, 1.0f), true, 1.5f);
				//m_pWeaponTrail->StartTrail(_float4(0.31f, 0.98f, 0.89f, 1.f), _float4(0.48f, 0.98f, 0.89f, 1.f), true, true, true, 1.f);
				AddSupremeGauge(10.f);
				break;

			case CPlayer::STAFF_SKILL_METEOR:
				ChangeAnim(WEAPON_STAFF_SKILL_METEOR, 0.05f, false, 0.8f);
				SetWeaponMeshTrail(true, _float4(0.31f, 0.31f, 0.89f, 1.0f), true, 2.5f);
				//m_pWeaponTrail->StartTrail(_float4(0.31f, 0.98f, 0.89f, 1.f), _float4(0.48f, 0.98f, 0.89f, 1.f), true, true, true, 1.f);
				CCameraMgr::GetInstance()->FilmCameraTurnOn("Supreme_Meteor", m_pTransformCom->GetState(CTransform::STATE_LOOK), m_pTransformCom->GetState(CTransform::STATE_POSITION));
				AddSupremeGauge(10.f);
				break;

			default:
				break;
			}

			break;

		default:
			break;
		}
		m_bCanUseSkill[m_eCurrentWeapon][_iSkillIndex] = false;
		m_fCurrentCoolTime[m_eCurrentWeapon][_iSkillIndex] = m_fSKillCoolTime[m_eCurrentWeapon][_iSkillIndex];

		/* Bind CurrentWeapon */
		/* Bind Cooltime */
		ITEMMGR->BindCurrentWeaponIndex(static_cast<_uint>(m_eCurrentWeapon));

		return true;
	}

	return false;
}

_bool CPlayer::PlaySupremeSkill()
{
	if (m_bCanUseSupreme && (m_eCurrentWeapon== WEAPON_TYPE_SWORD) )
	{
		m_fCurrentSupremeGauge = 0.f;
		m_bCanUseSupreme = false;
		SetToLockOnDir();

		weak_ptr<CUIUVBar> supremeBar = dynamic_pointer_cast<CUIUVBar>(UIMGR->FindUI("PCInfoGaugeMP"));
		if (supremeBar.expired())
			return true;

		supremeBar.lock()->SetBar(m_fCurrentSupremeGauge, m_fSupremeGaugeMax);

		TriggerAnimationSet("Supreme_Skill_Sword", 1);
		//SetWeaponGlow(true, true, _float4(0.98f, 0.592f, 0.318f, 0.8f), true, 1.5f);
		//SetWeaponMeshTrail(true, _float4(0.98f, 0.592f, 0.318f, 1.0f), true, 1.5f);
		//m_pWeaponTrail->StartTrail(_float4(0.98f, 0.576f, 0.31f, 1.f), _float4(0.98f, 0.89f, 0.48f, 1.f), true, true, true, 0.5f);

		_float3 vOffSetPos = m_pTransformCom->GetState(CTransform::STATE_POSITION) + _float3{0.0f, 1.0f, 0.0f};
		CCameraMgr::GetInstance()->FilmCameraTurnOn("Supreme_Sword", m_pTransformCom->GetState(CTransform::STATE_LOOK), vOffSetPos);

		return true;
	}
	return false;
}

_bool CPlayer::CheckCanSeparate(_uint _iAnimIndex)
{
	_bool bSeparate = false;

	for (auto Anim : m_SeparateAnims)
	{
		if (_iAnimIndex == Anim)
		{
			bSeparate = true;
			break;
		}
	}	
	return bSeparate;
}

_bool CPlayer::CheckCanMovement(_uint _iAnimIndex)
{
	_bool bMovement = false;

	for (auto Anim : m_MovementAnims)
	{
		if (_iAnimIndex == Anim)
		{
			bMovement = true;
			break;
		}
	}
	return bMovement;
}

_bool CPlayer::CheckSkillAnim(_uint _iAnimIndex)
{
	_bool bSkillAnim = false;

	for (auto Anim : m_SKillAnims)
	{
		if (_iAnimIndex == Anim)
		{
			bSkillAnim = true;
			break;
		}
	}
	return bSkillAnim;
}

_bool CPlayer::CheckIgnoreAbnormalAnim(_uint _iAnimIndex)
{
	_bool bIgnoreAbnormalAnim = false;
	
	for (auto Anim : m_IgnoreAbnormalAnims)
	{
		if (_iAnimIndex == Anim)
		{
			bIgnoreAbnormalAnim = true;
			break;
		}
	}
	return bIgnoreAbnormalAnim;
}

void CPlayer::LockOnControl()
{
	for (_int i = 0; i < m_pLockOnTargets.size(); i++)
	{	
		if ( !(m_pLockOnTargets[i].lock()->IsEnabled()) )
		{
			//dynamic_pointer_cast<CMonster>(m_pLockOnTargets[i].lock())->OffRimLight();
			m_pLockOnTargets[i].lock()->SetEdgeRender(false);
			m_pLockOnTargets.erase(m_pLockOnTargets.begin() + i);

			if (m_pLockOnTargets.size() == 0)
			{
				m_iCurrentLockOnCursor = -1;
				m_pCurrentLockOnTarget.reset();
				m_isSeparateLockOn = false;
				m_pCamera.lock()->LockOff();
				UIMGR->SetLockOnTarget(nullptr);
			}
			else
			{
				if ((i <= m_iCurrentLockOnCursor) && (m_iCurrentLockOnCursor != 0))
				{
					m_iCurrentLockOnCursor--;
				}
				//dynamic_pointer_cast<CMonster>(m_pCurrentLockOnTarget.lock())->OffRimLight();
				m_pCurrentLockOnTarget.lock()->SetEdgeRender(false);
				m_pCurrentLockOnTarget = m_pLockOnTargets[m_iCurrentLockOnCursor];
				//dynamic_pointer_cast<CMonster>(m_pCurrentLockOnTarget.lock())->OnRimLight();
				m_pCurrentLockOnTarget.lock()->SetEdgeRender(true);
				UIMGR->SetLockOnTarget(m_pCurrentLockOnTarget.lock());

				if (m_isSeparateLockOn)
					m_pCamera.lock()->LockOn(m_pCurrentLockOnTarget.lock());
			}
			break;
		}
	}

	if (m_pLockOnTargets.size() == 0)
	{
		UIMGR->SetLockOnTarget(nullptr);

		return;
	}

	if (GAMEINSTANCE->KeyDown(DIK_TAB))
	{
		m_iCurrentLockOnCursor++;

		if (m_pLockOnTargets.size() <= m_iCurrentLockOnCursor)
			m_iCurrentLockOnCursor = 0;
		//dynamic_pointer_cast<CMonster>(m_pCurrentLockOnTarget.lock())->OffRimLight();
		m_pCurrentLockOnTarget.lock()->SetEdgeRender(false);
		m_pCurrentLockOnTarget = m_pLockOnTargets[m_iCurrentLockOnCursor];
		//dynamic_pointer_cast<CMonster>(m_pCurrentLockOnTarget.lock())->OnRimLight();
		m_pCurrentLockOnTarget.lock()->SetEdgeRender(true);
		UIMGR->SetLockOnTarget(m_pCurrentLockOnTarget.lock());

		if(m_isSeparateLockOn)
			m_pCamera.lock()->LockOn(m_pCurrentLockOnTarget.lock());
	}
}

void CPlayer::LandingControl()
{

	if ((!m_isJumpState) && (!m_isLanding) && ( m_pModelCom->GetCanCancelAnimation() )/*|| m_pModelCom->GetIsFinishedAnimation())*/)
	{
		CancleAnimationSet();
		m_pWeaponTrail->EndTrail();
		switch (m_eCurrentWeapon)
		{
		case CPlayer::WEAPON_TYPE_SWORD:
			ChangeAnim(WEAPON_SWORD_JUMP_LOOP_RUN, 0.1f, true);
			break;
		case CPlayer::WEAPON_TYPE_CROSSBOW:
			ChangeAnim(WEAPON_CROSSBOW_JUMP_LOOP_RUN, 0.1f, true);
			break;
		case CPlayer::WEAPON_TYPE_STAFF:
			ChangeAnim(WEAPON_STAFF_JUMP_LOOP_RUN, 0.1f, true);
			break;
		default:
			break;
		}

		m_isJumpState = true;
		m_eCurrentBaseState = STATE_ACTIVE;
		m_pModelCom->PlayAnimation(0.f);
	}
}

void CPlayer::CheckTouched()
{
	m_isTouched = false;

	for (_int i = 0; i < m_pTouchedTargets.size(); i++)
	{
		if (m_pTouchedTargets[i].lock() == m_pCurrentLockOnTarget.lock())
		{
			m_isTouched = true;	
			break;
		}
	}
}

void CPlayer::ShootBuffProjectile(_float3 _vStartPos, _float3 _vTargetPos)
{
	if (m_bCrossBowBuff && (nullptr != m_pCurrentLockOnTarget.lock() ) )
		m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_ARROW_CURVE, _vStartPos, _vTargetPos);
}



void CPlayer::PolymorphControl(_float _fTimeDelta)
{
	if (m_isAllInputStop || m_isUIInputStop)
		return;

	if (GAMEINSTANCE->KeyDown(DIK_LSHIFT) && m_isChanged)
	{
		if (m_isJumpState)
		{	
			ChangeAnim(POLYMORPH_HUMAN_TO_WOLF_RUN, 0.05f, false, 0.95f);
			m_pModelCom->PlayAnimation(0);
			m_isEagleChanging = true;	
			SetDissolve(true, true, 0.5f);
			SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
			for (auto pEquipment : m_pPlayerEquipments)
			{
				pEquipment->SetDissolve(true, true, 0.5f);
				pEquipment->SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
			}
			return;
		}

		else if(m_eCurrentAnimation == WEAPON_SWORD_IDLE	||
				m_eCurrentAnimation == WEAPON_SWORD_RUN		||
				m_eCurrentAnimation == WEAPON_CROSSBOW_IDLE ||
				m_eCurrentAnimation == WEAPON_CROSSBOW_RUN	||
				m_eCurrentAnimation == WEAPON_STAFF_IDLE	||
				m_eCurrentAnimation == WEAPON_STAFF_RUN		)
		{
			ChangeAnim(POLYMORPH_HUMAN_TO_WOLF_RUN, 0.05f, false, 0.95f);
			m_pModelCom->PlayAnimation(0);
			// Dissolve
			SetDissolve(true, true, 0.62f);
			SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");

			for (auto pEquipment : m_pPlayerEquipments)
			{
				pEquipment->SetDissolve(true, true, 0.62f);
				pEquipment->SetDissolveInfo(1.f, _float4(0.427f, 0.227f, 0.573f, 1.f), "Noise_3003");
			}
		}
	}	
	if (m_isEagleChanging)
	{
		m_fEagleFormDuringTime -= _fTimeDelta;

		if (m_fEagleFormDuringTime <= 0 && m_isJumpState)
		{
			m_isJumpState = false;
			ChangeAnim(POLYMORPH_WOLF_TO_HUMAN_RUN, 0.05f, false, 0.95f);
			m_pModelCom->PlayAnimation(0);
			m_isEagleChanging = false;
			m_fEagleFormDuringTime = 0.5f;
			m_isChanged = false;
			SetDissolve(false, false, 0.0f);
			CHARACTERMGR->ChangePolymorph("Form_Eagle");
			for (auto pEquipment : m_pPlayerEquipments)
			{
				pEquipment->SetDissolve(false, false, 0.0f);
			}
			return;
		}
		else if (m_fEagleFormDuringTime <= 0 && (!m_isJumpState))
		{
			m_isEagleChanging = false;
			m_fEagleFormDuringTime = 0.5f;
			SetDissolve(false, false, 0.0f);
			for (auto pEquipment : m_pPlayerEquipments)
			{
				pEquipment->SetDissolve(false, false, 0.0f);
			}
		}
	}

	if (m_eCurrentAnimation == POLYMORPH_HUMAN_TO_WOLF_RUN &&
		m_pModelCom->GetCanCancelAnimation() && m_isChanged)
	{
		ChangeAnim(POLYMORPH_WOLF_TO_HUMAN_RUN, 0.05f, false, 0.95f);
		m_pModelCom->PlayAnimation(0);
		m_isChanged = false;
		SetDissolve(false, false, 0.0f);
		CHARACTERMGR->ChangePolymorph("Form_Wolf");
		for (auto pEquipment : m_pPlayerEquipments)
		{
			pEquipment->SetDissolve(false, false, 0.0f);
		}
		return;
	}
}

void CPlayer::ShootProjectile(_int _eProjectileOwner)
{
	if (_eProjectileOwner == FRONT)
	{
		if (m_eCurrentAnimation == WEAPON_CROSSBOW_SKILL_MULTIPLESHOT)
		{
			_float4x4 Worldmat = m_pTransformCom->GetWorldMatrix();
			Worldmat = (*pRHandMatrix) * Worldmat;
			_float3 vStartPos = _float3(Worldmat.m[3][0], Worldmat.m[3][1], Worldmat.m[3][2]);
			_float3 vTargetPos;
			_float3 MyLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);
			_float3 MyRight = m_pTransformCom->GetState(CTransform::STATE_RIGHT);
			vTargetPos = vStartPos + (MyLook*6) - (MyRight*3);

			for(_uint i = 0; i < 7 ;++i)
			{ 
				m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_ARROW_BASIC, vStartPos, vTargetPos);
				vTargetPos += MyRight;
			}
		}

		else if (m_eCurrentAnimation == WEAPON_STAFF_SKILL_FIREBLAST)
		{
			_float3 vMyPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
			_float3 MyLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);

			_float3 vTargetPos = vMyPos + MyLook * 3.f;

			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_SPELL_FIREBLAST, vMyPos, vTargetPos, PLAYER_PJ_A_STUN, true, 0);
		}
	}
	else if (_eProjectileOwner == TARGET)
	{
		//if (m_eCurrentAnimation == WEAPON_STAFF_SKILL_FIREBLAST)
		//{
		//	_float3 vMyPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
		//	_float3 vTargetPos;
		//	if (nullptr != m_pCurrentLockOnTarget.lock())
		//	{
		//		vTargetPos = m_pCurrentLockOnTarget.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
		//	}
		//	else
		//	{
		//		_float3 MyLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);
		//		vTargetPos = vMyPos + MyLook * 3.f;
		//	}

		//	m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_SPELL_FIREBLAST, vMyPos, vTargetPos, PLAYER_PJ_A_STUN);
		//}
		if (m_eCurrentAnimation == WEAPON_STAFF_SKILL_LIGHTNIGJUDGMENT)
		{
			_float3 vMyPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
			_float3 vTargetPos;
			if (nullptr != m_pCurrentLockOnTarget.lock())
			{
				vTargetPos = m_pCurrentLockOnTarget.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
			}
			else
			{
				_float3 MyLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);
				vTargetPos = vMyPos + MyLook * 4.f;
			}
			
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_SPELL_LIGHTINGJUDGMENT, vMyPos, vTargetPos, PLAYER_PJ_A_SHOCK);
		}
		else if (m_eCurrentAnimation == WEAPON_STAFF_SKILL_METEOR)
		{
			_float3 vMyPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
			_float3 vTargetPos;
			if (nullptr != m_pCurrentLockOnTarget.lock())
			{
				vTargetPos = m_pCurrentLockOnTarget.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
			}
			else
			{
				_float3 MyLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);
				vTargetPos = vMyPos + MyLook * 6.f;
			}
		
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_SPELL_METEOR, vMyPos, vTargetPos, PLAYER_PJ_A_DOWN);
		}

		
	}
	else
	{
		_float4x4 Worldmat = m_pTransformCom->GetWorldMatrix();
		_float4x4 MatPosOffset;

		switch (_eProjectileOwner)
		{
		case CROSSBOW_L:
			Worldmat = (*pLHandMatrix) * Worldmat;
			break;
		case CROSSBOW_R:
			Worldmat = (*pRHandMatrix) * Worldmat;
			break;
		case STAFF:
			MatPosOffset = SimpleMath::Matrix::CreateTranslation(_float3(0.f, 0.f, 1.f));
			Worldmat = (*pLHandMatrix)* MatPosOffset * Worldmat ;
			break;
		case STAFF_HAND:
			Worldmat = (*pRHandMatrix) * Worldmat;
			break;
		default:
			break;
		}
		
		_float3 vStartPos = _float3(Worldmat.m[3][0], Worldmat.m[3][1], Worldmat.m[3][2]);
		_float3 vTargetPos;
		
		if (m_eCurrentAnimation == ROPE_ACTION_ALL)
		{
			_float3 CamLook = GAMEINSTANCE->GetCamLook();
			vTargetPos = vStartPos + (CamLook * 10.f);
		}
		else if (nullptr != m_pCurrentLockOnTarget.lock())
		{
			vTargetPos = m_pCurrentLockOnTarget.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
			vTargetPos.y += 1.1f;
		}
		else
		{
			_float3 MyLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);
			vTargetPos = vStartPos + MyLook*10.f;
		}
		
		
		if (m_eCurrentAnimation == WEAPON_CROSSBOW_SKILL_ELECTRICSHOT)
		{
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_ARROW_ELECTRIC, vStartPos, vTargetPos, PLAYER_PJ_A_SHOCK);
		}
		else if (m_eCurrentAnimation == WEAPON_CROSSBOW_SKILL_FURIOUSFIRE_LOOP)
		{
			if (m_bFuriousFire == false)
			{
				m_bFuriousFire = true;
				m_vFuriousFireTargetPos = vTargetPos;
			}
			m_vFuriousFireStartPos = vStartPos;
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_ARROW_FURIOUSFIRE, m_vFuriousFireStartPos, m_vFuriousFireTargetPos);
			ShootBuffProjectile(vStartPos, vTargetPos);
		}
		else if (m_eCurrentAnimation == WEAPON_CROSSBOW_ATTACK_COMBO_4)
		{
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_ARROW_BASIC, vStartPos, vTargetPos, PLAYER_PJ_A_STIFF_SHORT);
			ShootBuffProjectile(vStartPos, vTargetPos);
		}
		//else if (m_eCurrentAnimation == WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_4)
		//{
		//	m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_ARROW_BASIC, vStartPos, vTargetPos, PLAYER_PJ_A_STIFF_SHORT);
		//	ShootBuffProjectile(vStartPos, vTargetPos);
		//}
		else if (m_eCurrentAnimation == WEAPON_STAFF_ATTACK_COMBO_1 ||
				 m_eCurrentAnimation == WEAPON_STAFF_ATTACK_COMBO_2)
		{
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_MAGIC_BULLET, vStartPos, vTargetPos);
		}
		else if (m_eCurrentAnimation == WEAPON_STAFF_ATTACK_COMBO_3)
		{
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_MAGIC_BULLET, vStartPos, vTargetPos, PLAYER_PJ_A_STIFF_SHORT);
		}
		else if (m_eCurrentAnimation == WEAPON_STAFF_SKILL_FIREBALL)
		{
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_SPELL_FIREBALL, vStartPos, vTargetPos, PLAYER_PJ_A_STIFF_SHORT, true, 0);
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_SPELL_FIREBALL, vStartPos, vTargetPos, PLAYER_PJ_A_STIFF_SHORT, true, 1);
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_SPELL_FIREBALL, vStartPos, vTargetPos, PLAYER_PJ_A_STIFF_SHORT, true, 2);
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_SPELL_FIREBALL, vStartPos, vTargetPos, PLAYER_PJ_A_STIFF_SHORT, true, 3);
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_SPELL_FIREBALL, vStartPos, vTargetPos, PLAYER_PJ_A_STIFF_SHORT, true, 4);

		}
		else if (m_eCurrentAnimation == WEAPON_STAFF_SKILL_ICESPEAR)
		{
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_SPELL_ICESPEAR, vStartPos, vTargetPos, PLAYER_PJ_A_STIFF_SHORT);
		}
		else if (m_eCurrentAnimation == ROPE_ACTION_ALL)
		{
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_ROPE_ACTION_BULLET, vStartPos, vTargetPos, PLAYER_PJ_NONE);
			m_pRigidBody->GravityOff();
			m_isGravity = false;
			m_bRopeRend = true;
		}
		else
		{
			m_pProjectilePool->AwakeProjectile(CPlayerProjectilePool::PLAYER_ARROW_BASIC, vStartPos, vTargetPos);
			ShootBuffProjectile(vStartPos, vTargetPos);
		}
	}
}

_bool CPlayer::PlayAnimationSet()
{
	if (m_iSetTrigger != 0)
	{
		m_iAnimSetCount++;

		if (m_iAnimSetCount > m_vecAnimationSet.size()-1)
		{
			m_iSetTrigger = 0;
			m_iAnimSetCount = 0;
			m_vecAnimationSet.clear();

			return false;
		}
		if( (m_iSetTrigger == 2) && ( m_iAnimSetCount == (m_vecAnimationSet.size()-1) ) )
			ChangeAnim(get<0>(m_vecAnimationSet[m_iAnimSetCount]), get<1>(m_vecAnimationSet[m_iAnimSetCount]), true, get<2>(m_vecAnimationSet[m_iAnimSetCount]));
		else		
			ChangeAnim(get<0>(m_vecAnimationSet[m_iAnimSetCount]), get<1>(m_vecAnimationSet[m_iAnimSetCount]), false, get<2>(m_vecAnimationSet[m_iAnimSetCount]));
		
		return true;
	}
	return false;
}

void CPlayer::CancleAnimationSet()
{
	m_iSetTrigger = 0;
	m_iAnimSetCount = 0;
	m_vecAnimationSet.clear();
}

HRESULT CPlayer::MakeAnimationSet()
{
	// BraveStrike=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecSkillBraveStrike;
	
	tuple<PLAYER_ANIMATION, _float, _float> tpBraveStrike1(WEAPON_SWORD_SKILL_BRAVE_STRIKE_1, 0.02f, 0.2f);
	vecSkillBraveStrike.push_back(tpBraveStrike1);
	tuple<PLAYER_ANIMATION, _float, _float> tpBraveStrike2(WEAPON_SWORD_SKILL_BRAVE_STRIKE_2, 0.02f, 0.2f);
	vecSkillBraveStrike.push_back(tpBraveStrike2);
	tuple<PLAYER_ANIMATION, _float, _float> tpBraveStrike3(WEAPON_SWORD_SKILL_BRAVE_STRIKE_3, 0.02f, 0.5f);
	vecSkillBraveStrike.push_back(tpBraveStrike3);
	
	m_AnimationSets.emplace("Brave_Strike", vecSkillBraveStrike);
	// ===================================================================================
	
	// SpinAttack=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecSkillSpinAttack;

	tuple<PLAYER_ANIMATION, _float, _float> tpSpinAttack1(WEAPON_SWORD_SKILL_SPIN_ATTACK_1, 0.02f, 0.5f);
	vecSkillSpinAttack.push_back(tpSpinAttack1);
	tuple<PLAYER_ANIMATION, _float, _float> tpSpinAttack2(WEAPON_SWORD_SKILL_SPIN_ATTACK_2, 0.02f, 0.5f);
	vecSkillSpinAttack.push_back(tpSpinAttack2);
	tuple<PLAYER_ANIMATION, _float, _float> tpSpinAttack3(WEAPON_SWORD_SKILL_SPIN_ATTACK_3, 0.02f, 0.5f);
	vecSkillSpinAttack.push_back(tpSpinAttack3);
	tuple<PLAYER_ANIMATION, _float, _float> tpSpinAttack4(WEAPON_SWORD_SKILL_SPIN_ATTACK_1, 0.02f, 0.5f);
	vecSkillSpinAttack.push_back(tpSpinAttack4);
	tuple<PLAYER_ANIMATION, _float, _float> tpSpinAttack5(WEAPON_SWORD_SKILL_SPIN_ATTACK_2, 0.02f, 0.7f);
	vecSkillSpinAttack.push_back(tpSpinAttack5);

	m_AnimationSets.emplace("Spin_Attack", vecSkillSpinAttack);
	// =================================================================================

	// FuriousFire=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecSkillFuriousFire;

	tuple<PLAYER_ANIMATION, _float, _float> tpFuriousFire1(WEAPON_CROSSBOW_SKILL_FURIOUSFIRE_READY, 0.02f, 0.9f);
	vecSkillFuriousFire.push_back(tpFuriousFire1);
	tuple<PLAYER_ANIMATION, _float, _float> tpFuriousFire2(WEAPON_CROSSBOW_SKILL_FURIOUSFIRE_LOOP, 0.0f, 1.0f);
	vecSkillFuriousFire.push_back(tpFuriousFire2);
	tuple<PLAYER_ANIMATION, _float, _float> tpFuriousFire3(WEAPON_CROSSBOW_SKILL_FURIOUSFIRE_LOOP, 0.0f, 1.0f);
	vecSkillFuriousFire.push_back(tpFuriousFire3);
	tuple<PLAYER_ANIMATION, _float, _float> tpFuriousFire4(WEAPON_CROSSBOW_SKILL_FURIOUSFIRE_LOOP, 0.0f, 1.0f);
	vecSkillFuriousFire.push_back(tpFuriousFire4);
	tuple<PLAYER_ANIMATION, _float, _float> tpFuriousFire5(WEAPON_CROSSBOW_SKILL_FURIOUSFIRE_LOOP, 0.0f, 1.0f);
	vecSkillFuriousFire.push_back(tpFuriousFire5);
	tuple<PLAYER_ANIMATION, _float, _float> tpFuriousFire6(WEAPON_CROSSBOW_SKILL_FURIOUSFIRE_END, 0.0f, 0.6f);
	vecSkillFuriousFire.push_back(tpFuriousFire6);

	m_AnimationSets.emplace("Furious_Fire", vecSkillFuriousFire);
	// ===================================================================================
	
	// NormalJump_Sword=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecNormalJumpSword;
	
	tuple<PLAYER_ANIMATION, _float, _float> tpNormalJumpSword2(WEAPON_SWORD_JUMP_MAIN, 0.05f, 1.f);
	vecNormalJumpSword.push_back(tpNormalJumpSword2);
	tuple<PLAYER_ANIMATION, _float, _float> tpNormalJumpSword3(WEAPON_SWORD_JUMP_LOOP, 0.f, 1.f);
	vecNormalJumpSword.push_back(tpNormalJumpSword3);
	
	m_AnimationSets.emplace("Jump_Normal_Sword", vecNormalJumpSword);
	// =================================================================================
	
	// RunJump_Sword=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecRunJumpSword;
	
	tuple<PLAYER_ANIMATION, _float, _float> tpRunJumpSword2(WEAPON_SWORD_JUMP_MAIN_RUN, 0.05f, 1.f);
	vecRunJumpSword.push_back(tpRunJumpSword2);
	tuple<PLAYER_ANIMATION, _float, _float> tpRunJumpSword3(WEAPON_SWORD_JUMP_LOOP_RUN, 0.f, 1.f);
	vecRunJumpSword.push_back(tpRunJumpSword3);
	
	m_AnimationSets.emplace("Jump_Run_Sword", vecRunJumpSword);
	// =================================================================================

	// NormalJump_CrossBow=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecNormalJumpCrossBow;

	tuple<PLAYER_ANIMATION, _float, _float> tpNormalJumpCrossBow2(WEAPON_CROSSBOW_JUMP_MAIN, 0.05f, 1.f);
	vecNormalJumpCrossBow.push_back(tpNormalJumpCrossBow2);
	tuple<PLAYER_ANIMATION, _float, _float> tpNormalJumpCrossBow3(WEAPON_CROSSBOW_JUMP_LOOP, 0.f, 1.f);
	vecNormalJumpCrossBow.push_back(tpNormalJumpCrossBow3);

	m_AnimationSets.emplace("Jump_Normal_CrossBow", vecNormalJumpCrossBow);
	// =================================================================================
	
	// RunJump_CrossBow=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecRunJumpCrossBow;

	tuple<PLAYER_ANIMATION, _float, _float> tpRunJumpCrossBow2(WEAPON_CROSSBOW_JUMP_MAIN_RUN, 0.05f, 1.f);
	vecRunJumpCrossBow.push_back(tpRunJumpCrossBow2);
	tuple<PLAYER_ANIMATION, _float, _float> tpRunJumpCrossBow3(WEAPON_CROSSBOW_JUMP_LOOP_RUN, 0.f, 1.f);
	vecRunJumpCrossBow.push_back(tpRunJumpCrossBow3);

	m_AnimationSets.emplace("Jump_Run_CrossBow", vecRunJumpCrossBow);
	// =================================================================================

	// NormalJump_Staff=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecNormalJumpStaff;

	tuple<PLAYER_ANIMATION, _float, _float> tpNormalJumpStaff2(WEAPON_STAFF_JUMP_MAIN, 0.05f, 1.f);
	vecNormalJumpStaff.push_back(tpNormalJumpStaff2);
	tuple<PLAYER_ANIMATION, _float, _float> tpNormalJumpStaff3(WEAPON_STAFF_JUMP_LOOP, 0.f, 1.f);
	vecNormalJumpStaff.push_back(tpNormalJumpStaff3);

	m_AnimationSets.emplace("Jump_Normal_Staff", vecNormalJumpStaff);
	// =================================================================================
	
	// RunJump_Staff=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecRunJumpStaff;

	tuple<PLAYER_ANIMATION, _float, _float> tpRunJumpStaff2(WEAPON_STAFF_JUMP_MAIN_RUN, 0.05f, 1.f);
	vecRunJumpStaff.push_back(tpRunJumpStaff2);
	tuple<PLAYER_ANIMATION, _float, _float> tpRunJumpStaff3(WEAPON_STAFF_JUMP_LOOP_RUN, 0.05f, 1.f);
	vecRunJumpStaff.push_back(tpRunJumpStaff3);

	m_AnimationSets.emplace("Jump_Run_Staff", vecRunJumpStaff);
	// =================================================================================

	// Stiff_Long_Sword=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecStiffLongSword;
	
	tuple<PLAYER_ANIMATION, _float, _float> tpStiffLongSword1(WEAPON_SWORD_DAMAGED_STIFF_LONG_S, 0.05f, 1.f);
	vecStiffLongSword.push_back(tpStiffLongSword1);
	tuple<PLAYER_ANIMATION, _float, _float> tpStiffLongSword2(WEAPON_SWORD_DAMAGED_STIFF_LONG_L, 0.05f, 1.f);
	vecStiffLongSword.push_back(tpStiffLongSword2);
	tuple<PLAYER_ANIMATION, _float, _float> tpStiffLongSword3(WEAPON_SWORD_DAMAGED_STIFF_LONG_E, 0.05f, 0.8f);
	vecStiffLongSword.push_back(tpStiffLongSword3);
	
	m_AnimationSets.emplace("Stiff_Long_Sword", vecStiffLongSword);
	// =================================================================================

	// Down_Sword=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecDownSword;

	tuple<PLAYER_ANIMATION, _float, _float> tpDownSword1(WEAPON_SWORD_DAMAGED_DOWN_START, 0.05f, 1.f);
	vecDownSword.push_back(tpDownSword1);
	tuple<PLAYER_ANIMATION, _float, _float> tpDownSword2(WEAPON_SWORD_DAMAGED_DOWN_LOOP, 0.05f, 1.f);
	vecDownSword.push_back(tpDownSword2);
	tuple<PLAYER_ANIMATION, _float, _float> tpDownSword3(WEAPON_SWORD_DAMAGED_DOWN_END, 0.05f, 0.9f);
	vecDownSword.push_back(tpDownSword3);

	m_AnimationSets.emplace("Down_Sword", vecDownSword);
	// =================================================================================

	// Down_Long_Sword=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecDownLongSword;

	tuple<PLAYER_ANIMATION, _float, _float> tpDownLongSword1(WEAPON_SWORD_DAMAGED_DOWN_LONG_START, 0.05f, 1.f);
	vecDownLongSword.push_back(tpDownLongSword1);
	tuple<PLAYER_ANIMATION, _float, _float> tpDownLongSword2(WEAPON_SWORD_DAMAGED_DOWN_LONG_LOOP, 0.05f, 1.f);
	vecDownLongSword.push_back(tpDownLongSword2);
	tuple<PLAYER_ANIMATION, _float, _float> tpDownLongSword3(WEAPON_SWORD_DAMAGED_DOWN_LONG_END, 0.05f, 1.0f);
	vecDownLongSword.push_back(tpDownLongSword3);

	m_AnimationSets.emplace("Down_Long_Sword", vecDownLongSword);
	// =================================================================================

	// Stiff_Long_CrossBow=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecStiffLongCrossBow;

	tuple<PLAYER_ANIMATION, _float, _float> tpStiffLongCrossBow1(WEAPON_CROSSBOW_DAMAGED_STIFF_LONG_S, 0.05f, 1.f);
	vecStiffLongCrossBow.push_back(tpStiffLongCrossBow1);
	tuple<PLAYER_ANIMATION, _float, _float> tpStiffLongCrossBow2(WEAPON_CROSSBOW_DAMAGED_STIFF_LONG_L, 0.05f, 1.f);
	vecStiffLongCrossBow.push_back(tpStiffLongCrossBow2);
	tuple<PLAYER_ANIMATION, _float, _float> tpStiffLongCrossBow3(WEAPON_CROSSBOW_DAMAGED_STIFF_LONG_E, 0.05f, 0.8f);
	vecStiffLongCrossBow.push_back(tpStiffLongCrossBow3);

	m_AnimationSets.emplace("Stiff_Long_CrossBow", vecStiffLongCrossBow);
	// =================================================================================

	// Down_CrossBow=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecDownCrossBow;

	tuple<PLAYER_ANIMATION, _float, _float> tpDownCrossBow1(WEAPON_CROSSBOW_DAMAGED_DOWN_START, 0.05f, 1.f);
	vecDownCrossBow.push_back(tpDownCrossBow1);
	tuple<PLAYER_ANIMATION, _float, _float> tpDownCrossBow2(WEAPON_CROSSBOW_DAMAGED_DOWN_LOOP, 0.05f, 1.f);
	vecDownCrossBow.push_back(tpDownCrossBow2);
	tuple<PLAYER_ANIMATION, _float, _float> tpDownCrossBow3(WEAPON_CROSSBOW_DAMAGED_DOWN_END, 0.05f, 0.9f);
	vecDownCrossBow.push_back(tpDownCrossBow3);

	m_AnimationSets.emplace("Down_CrossBow", vecDownCrossBow);
	// =================================================================================

	// Down_Long_CrossBow=========================================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecDownLongCrossBow;

	tuple<PLAYER_ANIMATION, _float, _float> tpDownLongCrossBow1(WEAPON_CROSSBOW_DAMAGED_DOWN_LONG_START, 0.05f, 1.f);
	vecDownLongCrossBow.push_back(tpDownLongCrossBow1);
	tuple<PLAYER_ANIMATION, _float, _float> tpDownLongCrossBow2(WEAPON_CROSSBOW_DAMAGED_DOWN_LONG_LOOP, 0.05f, 1.f);
	vecDownLongCrossBow.push_back(tpDownLongCrossBow2);
	tuple<PLAYER_ANIMATION, _float, _float> tpDownLongCrossBow3(WEAPON_CROSSBOW_DAMAGED_DOWN_LONG_END, 0.05f, 1.f);
	vecDownLongCrossBow.push_back(tpDownLongCrossBow3);
	
	m_AnimationSets.emplace("Down_Long_CrossBow", vecDownLongCrossBow);
	// ===================================================================================

	// Supreme_Skill_Sword==============================================================
	vector< tuple<PLAYER_ANIMATION, _float, _float> > vecSupremeSkill;
	
	tuple<PLAYER_ANIMATION, _float, _float> tpSupremeSkillSword1(WEAPON_SWORD_SUPREME_SKILL_READY, 0.05f, 0.63f);
	vecSupremeSkill.push_back(tpSupremeSkillSword1);
	tuple<PLAYER_ANIMATION, _float, _float> tpSupremeSkillSword2(WEAPON_SWORD_SUPREME_SKILL_ATTACK, 0.05f, 1.f);
	vecSupremeSkill.push_back(tpSupremeSkillSword2);
	
	m_AnimationSets.emplace("Supreme_Skill_Sword", vecSupremeSkill);
	// ====================================================================================

	return S_OK;
}

HRESULT CPlayer::SettingSkillCoolTime()
{
	m_fSKillCoolTime[WEAPON_TYPE_SWORD][SWORD_SKILL_PARRYING]		= 2.f;
	m_fSKillCoolTime[WEAPON_TYPE_SWORD][SWORD_SKILL_BRAVE_STRIKE]	= 3.f;
	m_fSKillCoolTime[WEAPON_TYPE_SWORD][SWORD_SKILL_RUSH_ATTACK]	= 4.f;
	m_fSKillCoolTime[WEAPON_TYPE_SWORD][SWORD_SKILL_GAIA_CRASH]		= 5.f;
	m_fSKillCoolTime[WEAPON_TYPE_SWORD][SWORD_SKILL_UPPER_ATTACK]	= 5.f;
	m_fSKillCoolTime[WEAPON_TYPE_SWORD][SWORD_SKILL_SPIN_ATTACK]	= 5.f;

	m_fSKillCoolTime[WEAPON_TYPE_CROSSBOW][CROSSBOW_SKILL_PARRYING]			= 2.f;
	m_fSKillCoolTime[WEAPON_TYPE_CROSSBOW][CROSSBOW_SKILL_RAPIDSHOT]		= 3.f;
	m_fSKillCoolTime[WEAPON_TYPE_CROSSBOW][CROSSBOW_SKILL_MULTIPLESHOT]		= 4.f;
	m_fSKillCoolTime[WEAPON_TYPE_CROSSBOW][CROSSBOW_SKILL_INDOMITABLESPIRIT]= 5.f;
	m_fSKillCoolTime[WEAPON_TYPE_CROSSBOW][CROSSBOW_SKILL_ELECTRICSHOT]		= 5.f;
	m_fSKillCoolTime[WEAPON_TYPE_CROSSBOW][CROSSBOW_SKILL_FURIOUSFIRE]		= 5.f;

	m_fSKillCoolTime[WEAPON_TYPE_STAFF][STAFF_SKILL_PARRYING]			= 2.f;
	m_fSKillCoolTime[WEAPON_TYPE_STAFF][STAFF_SKILL_FIREBALL]			= 3.f;
	m_fSKillCoolTime[WEAPON_TYPE_STAFF][STAFF_SKILL_ICESPEAR]			= 4.f;
	m_fSKillCoolTime[WEAPON_TYPE_STAFF][STAFF_SKILL_FIREBLAST]			= 5.f;
	m_fSKillCoolTime[WEAPON_TYPE_STAFF][STAFF_SKILL_LIGHTNIGJUDGMENT]	= 5.f;
	m_fSKillCoolTime[WEAPON_TYPE_STAFF][STAFF_SKILL_METEOR]				= 5.f;

	return S_OK;
}

HRESULT CPlayer::AddSeparateAnims()
{
	m_SeparateAnims.push_back(WEAPON_CROSSBOW_ATTACK_COMBO_1);
	m_SeparateAnims.push_back(WEAPON_CROSSBOW_ATTACK_COMBO_2);
	m_SeparateAnims.push_back(WEAPON_CROSSBOW_ATTACK_COMBO_3);
	m_SeparateAnims.push_back(WEAPON_CROSSBOW_ATTACK_COMBO_4);
	m_SeparateAnims.push_back(WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_1);
	m_SeparateAnims.push_back(WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_2);
	m_SeparateAnims.push_back(WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_3);
	m_SeparateAnims.push_back(WEAPON_CROSSBOW_ATTACK_LOCK_ON_COMBO_4);
	m_SeparateAnims.push_back(WEAPON_CROSSBOW_SKILL_RAPIDSHOT);
	m_SeparateAnims.push_back(WEAPON_CROSSBOW_SKILL_MULTIPLESHOT);
	m_SeparateAnims.push_back(WEAPON_CROSSBOW_SKILL_INDOMITABLESPIRIT);
	//m_SeparateAnims.push_back(WEAPON_STAFF_ATTACK_COMBO_1);
	//m_SeparateAnims.push_back(WEAPON_STAFF_ATTACK_COMBO_2);
	//m_SeparateAnims.push_back(WEAPON_STAFF_ATTACK_COMBO_3);

	return S_OK;
}

HRESULT CPlayer::AddMovementAnims()
{
	m_MovementAnims.push_back(WEAPON_SWORD_SKILL_SPIN_ATTACK_1);
	m_MovementAnims.push_back(WEAPON_SWORD_SKILL_SPIN_ATTACK_2);
	m_MovementAnims.push_back(WEAPON_SWORD_SKILL_SPIN_ATTACK_3);

	m_MovementAnims.push_back(POLYMORPH_HUMAN_TO_WOLF_RUN);
	m_MovementAnims.push_back(POLYMORPH_WOLF_TO_HUMAN_RUN);


	return S_OK;
}

HRESULT CPlayer::AddSkillAnims()
{
	m_SKillAnims.push_back(WEAPON_SWORD_SKILL_BRAVE_STRIKE_1);
	m_SKillAnims.push_back(WEAPON_SWORD_SKILL_BRAVE_STRIKE_2);
	m_SKillAnims.push_back(WEAPON_SWORD_SKILL_BRAVE_STRIKE_3);
	m_SKillAnims.push_back(WEAPON_SWORD_SKILL_RUSH_ATTACK);
	m_SKillAnims.push_back(WEAPON_SWORD_SKILL_GAIA_CRASH);
	m_SKillAnims.push_back(WEAPON_SWORD_SKILL_UPPER_ATTACK);
	m_SKillAnims.push_back(WEAPON_SWORD_SKILL_SPIN_ATTACK_1);
	m_SKillAnims.push_back(WEAPON_SWORD_SKILL_SPIN_ATTACK_2);
	m_SKillAnims.push_back(WEAPON_SWORD_SKILL_SPIN_ATTACK_3);

	return S_OK;
}

HRESULT CPlayer::AddIgnoreAbnormalAnims()
{
	m_IgnoreAbnormalAnims.push_back(ROPE_ACTION_ALL);

	m_IgnoreAbnormalAnims.push_back(WEAPON_SWORD_SKILL_GAIA_CRASH);

	m_IgnoreAbnormalAnims.push_back(WEAPON_SWORD_SKILL_SPIN_ATTACK_1);
	m_IgnoreAbnormalAnims.push_back(WEAPON_SWORD_SKILL_SPIN_ATTACK_2);
	m_IgnoreAbnormalAnims.push_back(WEAPON_SWORD_SKILL_SPIN_ATTACK_3);

	m_IgnoreAbnormalAnims.push_back(WEAPON_CROSSBOW_SKILL_FURIOUSFIRE_READY);
	m_IgnoreAbnormalAnims.push_back(WEAPON_CROSSBOW_SKILL_FURIOUSFIRE_LOOP);
	m_IgnoreAbnormalAnims.push_back(WEAPON_CROSSBOW_SKILL_FURIOUSFIRE_END);

	m_IgnoreAbnormalAnims.push_back(WEAPON_STAFF_SKILL_METEOR);

	return S_OK;
}

 HRESULT CPlayer::SetAnimOwner()
{
	vector<shared_ptr<class CAnimation>> vecAnims = m_pModelCom->GetAnimations();
	for (auto pAnim : vecAnims)
	{
		pAnim->SetOwnerObject(shared_from_this());
	}
	return S_OK;
}

 HRESULT CPlayer::CreateCamera()
 {
	 shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
	 shared_ptr<CCameraMgr> pCameraMgr = CCameraMgr::GetInstance();

	 CCamera::CAMERA_DESC pCamDesc = {};
	 pCamDesc.fFovy = XMConvertToRadians(60.0f);
	 pCamDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	 pCamDesc.fNear = 0.1f;
	 pCamDesc.fFar = 1000.f;

	 shared_ptr<CCameraFilm> pSkillCam = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/TestSupreme.json");
	 if (FAILED(pGameInstance->AddObject(LEVEL_STATIC, L"Layer_Camera", pSkillCam)))
		 return E_FAIL;

	 pCameraMgr->AddFilmCamera("Supreme_Sword", pSkillCam);

	 shared_ptr<CCameraFilm> pSkillCam1 = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/TestCrossbow.json");
	 if (FAILED(pGameInstance->AddObject(LEVEL_STATIC, L"Layer_Camera", pSkillCam1)))
		 return E_FAIL;

	 pCameraMgr->AddFilmCamera("Supreme_CrossBow", pSkillCam1);

	 shared_ptr<CCameraFilm> pSkillCam2 = CCameraFilm::Create(&pCamDesc, L"../../Tool_CC/Bin/DataFiles/Camera/TestMeteor.json");
	 if (FAILED(pGameInstance->AddObject(LEVEL_STATIC, L"Layer_Camera", pSkillCam2)))
		 return E_FAIL;

	 pCameraMgr->AddFilmCamera("Supreme_Meteor", pSkillCam2);

	 return S_OK;
 }

HRESULT CPlayer::AddWeapons()
{
	// Sword
	shared_ptr<CWeapon::WEAPON_DESC> pWeaponDescSword = make_shared<CWeapon::WEAPON_DESC>();
	pWeaponDescSword->tPartObject.pParentTransform = m_pTransformCom;
	pWeaponDescSword->pSocketMatrix = pRHandMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_RightHand");
	pWeaponDescSword->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(90.f)) *
										   SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(270.f));
	pWeaponDescSword->strModelKey = "Sword2h_1";
	
	shared_ptr<CTransform::TRANSFORM_DESC> pTransformDescSword = make_shared<CTransform::TRANSFORM_DESC>();
	pTransformDescSword->fSpeedPerSec = 1.f;
	pTransformDescSword->fRotationPerSec = XMConvertToRadians(90.0f);

	shared_ptr<CWeapon> pWeaponSword = CWeapon::Create(pWeaponDescSword.get(), pTransformDescSword.get());
	if (nullptr == pWeaponSword)
		return E_FAIL;

	pWeaponSword->AddTrail(7, 1);

	m_pCurrentWeapon = pWeaponSword;
	m_pCurrentWeapon->SetEdgeRender(true);
	m_pPlayerWeapons.emplace("Part_Sword2h_1", pWeaponSword);

	// CrossBowL
	shared_ptr<CWeapon::WEAPON_DESC> pWeaponDescCrossBowL = make_shared<CWeapon::WEAPON_DESC>();
	pWeaponDescCrossBowL->tPartObject.pParentTransform = m_pTransformCom;
	pWeaponDescCrossBowL->pSocketMatrix = pLHandMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_LeftHand");
	pWeaponDescCrossBowL->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(90.f))*
											   SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(270.f));
	pWeaponDescCrossBowL->strModelKey = "CrossBow_1";
	
	shared_ptr<CTransform::TRANSFORM_DESC> pTransformDescCrossBowL = make_shared<CTransform::TRANSFORM_DESC>();
	pTransformDescCrossBowL->fSpeedPerSec = 1.f;
	pTransformDescCrossBowL->fRotationPerSec = XMConvertToRadians(90.0f);
	
	shared_ptr<CWeapon> pWeaponCrossBowL = CWeapon::Create(pWeaponDescCrossBowL.get(), pTransformDescCrossBowL.get());
	if (nullptr == pWeaponCrossBowL)
		return E_FAIL;

	pWeaponCrossBowL->AddTrail(4, 1);
	
	m_pPlayerWeapons.emplace("Part_CrossBow_1_L", pWeaponCrossBowL);


	// CrossBowR
	shared_ptr<CWeapon::WEAPON_DESC> pWeaponDescCrossBowR = make_shared<CWeapon::WEAPON_DESC>();
	pWeaponDescCrossBowR->tPartObject.pParentTransform = m_pTransformCom;
	pWeaponDescCrossBowR->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_RightHand");
	pWeaponDescCrossBowR->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(90.f)) *
											   SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(270.f));
	pWeaponDescCrossBowR->strModelKey = "CrossBow_1";
	
	shared_ptr<CTransform::TRANSFORM_DESC> pTransformDescCrossBowR = make_shared<CTransform::TRANSFORM_DESC>();
	pTransformDescCrossBowR->fSpeedPerSec = 1.f;
	pTransformDescCrossBowR->fRotationPerSec = XMConvertToRadians(90.0f);
	
	shared_ptr<CWeapon> pWeaponCrossBowR = CWeapon::Create(pWeaponDescCrossBowR.get(), pTransformDescCrossBowR.get());
	if (nullptr == pWeaponCrossBowR)
		return E_FAIL;

	pWeaponCrossBowR->AddTrail(4, 1);
	
	m_pPlayerWeapons.emplace("Part_CrossBow_1_R", pWeaponCrossBowR);


	// Staff
	shared_ptr<CWeapon::WEAPON_DESC> pWeaponDescStaff = make_shared<CWeapon::WEAPON_DESC>();
	pWeaponDescStaff->tPartObject.pParentTransform = m_pTransformCom;
	pWeaponDescStaff->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_LeftHand");
	pWeaponDescStaff->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(90.f)) *
										   SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(270.f));
	pWeaponDescStaff->strModelKey = "Staff_1";
	
	shared_ptr<CTransform::TRANSFORM_DESC> pTransformDescStaff= make_shared<CTransform::TRANSFORM_DESC>();
	pTransformDescStaff->fSpeedPerSec = 1.f;
	pTransformDescStaff->fRotationPerSec = XMConvertToRadians(90.0f);

	shared_ptr<CWeapon> pWeaponStaff= CWeapon::Create(pWeaponDescStaff.get(), pTransformDescStaff.get());
	if (nullptr == pWeaponStaff)
		return E_FAIL;

	pWeaponStaff->AddTrail(7, 1);

	m_pPlayerWeapons.emplace("Part_Staff_1", pWeaponStaff);

	return S_OK;
}

HRESULT CPlayer::AddEquipments()
{
	// Helmet
	shared_ptr<CEquipment::EQUIPMENT_DESC> pEquipmentDescHelmet = make_shared<CEquipment::EQUIPMENT_DESC>();
	pEquipmentDescHelmet->tPartObject.pParentTransform = m_pTransformCom;
	pEquipmentDescHelmet->ePartsType = ITEM_EQUIP_HELMET;
	
	shared_ptr<CEquipment> pEquipmentHelmet = CEquipment::Create(pEquipmentDescHelmet, m_pModelCom);
	if (nullptr == pEquipmentHelmet)
		return E_FAIL;
	
	m_pPlayerEquipments.push_back(pEquipmentHelmet);
	pEquipmentHelmet->SetEdgeRender(true);
	
	// Upper
	shared_ptr<CEquipment::EQUIPMENT_DESC> pEquipmentDescUpper = make_shared<CEquipment::EQUIPMENT_DESC>();
	pEquipmentDescUpper->tPartObject.pParentTransform = m_pTransformCom;
	pEquipmentDescUpper->ePartsType = ITEM_EQUIP_UPPER;
	
	shared_ptr<CEquipment> pEquipmentUpper = CEquipment::Create(pEquipmentDescUpper, m_pModelCom);
	if (nullptr == pEquipmentUpper)
		return E_FAIL;

	m_pPlayerEquipments.push_back(pEquipmentUpper);
	pEquipmentUpper->SetEdgeRender(true);

	// Lower
	shared_ptr<CEquipment::EQUIPMENT_DESC> pEquipmentDescLower = make_shared<CEquipment::EQUIPMENT_DESC>();
	pEquipmentDescLower->tPartObject.pParentTransform = m_pTransformCom;
	pEquipmentDescLower->ePartsType = ITEM_EQUIP_LOWER;
	
	shared_ptr<CEquipment> pEquipmentLower = CEquipment::Create(pEquipmentDescLower, m_pModelCom);
	if (nullptr == pEquipmentLower)
		return E_FAIL;
	
	m_pPlayerEquipments.push_back(pEquipmentLower);
	pEquipmentLower->SetEdgeRender(true);
	
	// Cape
	shared_ptr<CEquipment::EQUIPMENT_DESC> pEquipmentDescCape = make_shared<CEquipment::EQUIPMENT_DESC>();
	pEquipmentDescCape->tPartObject.pParentTransform = m_pTransformCom;
	pEquipmentDescCape->ePartsType = ITEM_EQUIP_CAPE;
	
	shared_ptr<CEquipment> pEquipmentCape = CEquipment::Create(pEquipmentDescCape, m_pModelCom);
	if (nullptr == pEquipmentCape)
		return E_FAIL;
	
	m_pPlayerEquipments.push_back(pEquipmentCape);
	pEquipmentCape->SetEdgeRender(true);
	
	// Glove
	shared_ptr<CEquipment::EQUIPMENT_DESC> pEquipmentDescGlove = make_shared<CEquipment::EQUIPMENT_DESC>();
	pEquipmentDescGlove->tPartObject.pParentTransform = m_pTransformCom;
	pEquipmentDescGlove->ePartsType = ITEM_EQUIP_GLOVE;
	
	shared_ptr<CEquipment> pEquipmentGlove = CEquipment::Create(pEquipmentDescGlove, m_pModelCom);
	if (nullptr == pEquipmentGlove)
		return E_FAIL;
	
	m_pPlayerEquipments.push_back(pEquipmentGlove);
	pEquipmentGlove->SetEdgeRender(true);

	// Boots
	shared_ptr<CEquipment::EQUIPMENT_DESC> pEquipmentDescBoots = make_shared<CEquipment::EQUIPMENT_DESC>();
	pEquipmentDescBoots->tPartObject.pParentTransform = m_pTransformCom;
	pEquipmentDescBoots->ePartsType = ITEM_EQUIP_BOOTS;

	shared_ptr<CEquipment> pEquipmentBoots = CEquipment::Create(pEquipmentDescBoots, m_pModelCom);
	if (nullptr == pEquipmentBoots)
		return E_FAIL;

	m_pPlayerEquipments.push_back(pEquipmentBoots);
	pEquipmentBoots->SetEdgeRender(true);

	return S_OK;
}

HRESULT CPlayer::AddRigidBody(string _strModelKey)
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	//==º»Ã¼==
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = true;
	//RigidBodyDesc.isGravity = false;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestitution = 0.0f;

	GeometryCapsule CapsuleDesc;
	CapsuleDesc.fHeight = 1.5f;
	CapsuleDesc.fRadius = 0.25f;
	CapsuleDesc.vOffSetPosition = { 0.0f, 1.0f, 0.0f };
	CapsuleDesc.vOffSetRotation = { 0.0f, 0.0f, 90.0f };
	CapsuleDesc.strShapeTag = "Player_Body";
	RigidBodyDesc.pGeometry = &CapsuleDesc;

	RigidBodyDesc.eThisColFlag = COL_PLAYER;
	RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_MONSTER | COL_NPC | COL_MONSTERWEAPON | COL_MONSTERPROJECTILE | COL_MONSTERSKILL | COL_TRIGGER | COL_INTERACTIONOBJ | COL_TRAP;
	m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

	m_pTransformCom->SetRigidBody(m_pRigidBody);

	//==ÇÇÁ÷½º ³«ÇÏÆÇÁ¤ Ã¼Å©¿ë==
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.isInitCol = true;

	GeometryBox BoxDesc;
	BoxDesc.vSize = _float3{1.2f, 0.8f, 1.2f};
	BoxDesc.vOffSetPosition = { 0.0f, 0.05f, 0.0f };
	BoxDesc.strShapeTag = "Jump_Check";
	RigidBodyDesc.pGeometry = &BoxDesc;

	RigidBodyDesc.eThisColFlag = COL_PLAYER;
	RigidBodyDesc.eTargetColFlag = COL_STATIC;
	m_pRigidBody->CreateShape(&RigidBodyDesc);

	//==ÇÇÁ÷½º ÂøÁöÆÇÁ¤ Ã¼Å©¿ë==
	BoxDesc.vSize = _float3{ 0.05f, 0.4f, 0.05f };
	BoxDesc.vOffSetPosition = { 0.f, 0.f, 0.0f };
	BoxDesc.strShapeTag = "Land_Check";
	RigidBodyDesc.pGeometry = &BoxDesc;
	
	RigidBodyDesc.eThisColFlag = COL_PLAYER;
	RigidBodyDesc.eTargetColFlag = COL_STATIC;
	m_pRigidBody->CreateShape(&RigidBodyDesc);

	//==·Ï¿Â´ë»ó Å½»ö¿ë==
	// ·Ï¿Â Sword
	GeometrySphere SphereDesc;
	SphereDesc.fRadius = 10.f;
	SphereDesc.vOffSetPosition = { 0.0f, 1.f, 0.f };
	SphereDesc.strShapeTag = "LockOn_Check_Sword";
	RigidBodyDesc.pGeometry = &SphereDesc;
	RigidBodyDesc.eThisColFlag = COL_CHECKBOX;
	RigidBodyDesc.eTargetColFlag = COL_MONSTER | COL_MONSTERPROJECTILE;
	m_pRigidBody->CreateShape(&RigidBodyDesc);

	// ·Ï¿Â CrossBow
	SphereDesc.fRadius = 20.f;
	SphereDesc.vOffSetPosition = { 0.0f, 1.f, 0.f };
	SphereDesc.strShapeTag = "LockOn_Check_CrossBow";
	RigidBodyDesc.pGeometry = &SphereDesc;
	RigidBodyDesc.eThisColFlag = COL_CHECKBOX;
	RigidBodyDesc.eTargetColFlag = COL_MONSTER | COL_MONSTERPROJECTILE;
	m_pRigidBody->CreateShape(&RigidBodyDesc);
	m_pRigidBody->DisableCollision("LockOn_Check_CrossBow");

	// ·Ï¿Â Staff
	SphereDesc.fRadius = 15.f;
	SphereDesc.vOffSetPosition = { 0.0f, 1.f, 0.f };
	SphereDesc.strShapeTag = "LockOn_Check_Staff";
	RigidBodyDesc.pGeometry = &SphereDesc;
	RigidBodyDesc.eThisColFlag = COL_CHECKBOX;
	RigidBodyDesc.eTargetColFlag = COL_MONSTER | COL_MONSTERPROJECTILE;
	m_pRigidBody->CreateShape(&RigidBodyDesc);
	m_pRigidBody->DisableCollision("LockOn_Check_Staff");

	// ·Ï¿Â Boss
	SphereDesc.fRadius = 75.f;
	SphereDesc.vOffSetPosition = { 0.0f, 1.f, 0.f };
	SphereDesc.strShapeTag = "LockOn_Check_Boss";
	RigidBodyDesc.pGeometry = &SphereDesc;
	RigidBodyDesc.eThisColFlag = COL_CHECKBOX;
	RigidBodyDesc.eTargetColFlag = COL_MONSTER | COL_MONSTERPROJECTILE;
	m_pRigidBody->CreateShape(&RigidBodyDesc);
	m_pRigidBody->DisableCollision("LockOn_Check_Boss");
	//=========================================
	//=============JSONÀÐ¾î¼­ ÀúÀå==============

	wstring wstrModelName(_strModelKey.begin(), _strModelKey.end());
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

		vector<shared_ptr<class CAnimation>> vecAnims = m_pModelCom->GetAnimations();
		vecAnims[iAnimIndex]->SetOwnerObject(shared_from_this());
		vecAnims[iAnimIndex]->SetColStartTrackPos(Model[strAnimIndex]["TrackPos"]["StartPos"].asFloat());
		vecAnims[iAnimIndex]->SetColEndTrackPos(Model[strAnimIndex]["TrackPos"]["EndPos"].asFloat());
	}
	//=========================================
	//=========================================

	return S_OK;
}

HRESULT CPlayer::AddProjectilePool()
{
	m_pProjectilePool = CPlayerProjectilePool::Create(dynamic_pointer_cast<CPlayer>(shared_from_this()));

	if (!m_pProjectilePool)
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::AddProjectileTriggers(string _strModelKey)
{
	wstring wstrModelName(_strModelKey.begin(), _strModelKey.end());
	wstring strCompletePath = L"..\\..\\Tool_Animation\\Bin\\DataFiles\\ProjectileTrigger\\" + wstrModelName + TEXT("_Proj") + TEXT(".json");

	Json::Value Model;

	filesystem::path path(strCompletePath);
	if (!(filesystem::exists(path) && filesystem::is_regular_file(path)))
		return S_OK;

	Model = GAMEINSTANCE->ReadJson(strCompletePath);

	vector<shared_ptr<class CAnimation>> vecAnims = m_pModelCom->GetAnimations();

	_int			 iAnimationIndex;
	//vector<_float>	 fProjectileTriggerTrackPoss;
	//vector<_int>	 eProjectileTriggerOwners;

	auto iter = Model.begin();

	for (iter; iter != Model.end(); ++iter)
	{
		//fProjectileTriggerTrackPoss.clear();
		//eProjectileTriggerOwners.clear();

		string strAnimIndex = iter.key().asString();
		_uint iAnimIndex = atoi(strAnimIndex.c_str());

		iAnimationIndex = iAnimIndex;

		vecAnims[iAnimIndex]->SetAnimIndex(iAnimationIndex);
		vecAnims[iAnimIndex]->SetOwnerObject(shared_from_this());
		for (_uint i = 0; i < Model[strAnimIndex]["ProjOwner"].size(); ++i)
		{
			vecAnims[iAnimIndex]->AddProjectileTrigger(Model[strAnimIndex]["TrackPos"][i].asFloat(), Model[strAnimIndex]["ProjOwner"][i].asInt());
		}
	}

	return S_OK;
}

HRESULT CPlayer::AddRopeLine()
{

	shared_ptr<CPlayerRopeLine::ROPELINE_DESC> pRopeLineDesc = make_shared<CPlayerRopeLine::ROPELINE_DESC>();
	pRopeLineDesc->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_RightHand");

	shared_ptr<CPlayerProjectile> pPlayerRope = m_pProjectilePool->GetProjectile(CPlayerProjectilePool::PLAYER_ROPE_ACTION_BULLET, 0);		
	pRopeLineDesc->pTargetMatrix = pPlayerRope->GetTransform()->GetWorldMatrixPtr();
	
	pRopeLineDesc->pParentTransform = m_pTransformCom;

	m_pRopeLine = CPlayerRopeLine::Create(pRopeLineDesc);

	return S_OK;
}

HRESULT CPlayer::ReadyPlayerLight()
{
	/*Point Light*/
	LIGHT_DESC		LightDesc = {};

	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4();
	LightDesc.fRange = 5.f;

	LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vAmbient = _float4(0.9f, 0.9f, 0.9f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	GAMEINSTANCE->AddLight(LightDesc, &m_iPlayerLightNum);


	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4();
	LightDesc.fRange = 5.f;

	LightDesc.vDiffuse = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	GAMEINSTANCE->AddLight(LightDesc, &m_iPlayerLightNum2);


	return S_OK;
}

HRESULT CPlayer::AddWeaponTrail()
{
	m_pWeaponTrail = CPointTrail::Create("Trail");

	if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, TEXT("Layer_Effect"), m_pWeaponTrail)))
		return E_FAIL;

	ChangeTrailType();

	return S_OK;
}

HRESULT CPlayer::ChangeTrailType()
{
	shared_ptr<CWeapon> pWeapon;

	switch (m_eCurrentWeapon)
	{
	case CPlayer::WEAPON_TYPE_SWORD:
		m_pWeaponTrail->ChangeWeapon(m_pCurrentWeapon, _float3(0.f, 1.5f, 0.f), _float3(0.f, 0.f, 0.f));
		break;
	case CPlayer::WEAPON_TYPE_CROSSBOW:
		break;
	case CPlayer::WEAPON_TYPE_STAFF:
		break;
	
	default:
		break;
	}

	return S_OK;
}

void CPlayer::SetWeaponGlow(_bool _bGlow, _bool _bUseGlowColor, _float4 _vGlowColor, _bool _bUseGlowTime, _float _fGlowTime)
{
	m_pCurrentWeapon->SetUseGlow(_bGlow, _bUseGlowTime, _fGlowTime);
	m_pCurrentWeapon->SetUseGlowColor(_bUseGlowColor, _vGlowColor);
}

void CPlayer::SetWeaponMeshTrail(_bool _bUseMeshTrail, _float4 _vMeshTrailColor, _bool _bUseMeshTrailTime, _float _fMeshTrailTime)
{
	m_pCurrentWeapon->SetUseTrail(_bUseMeshTrail, _vMeshTrailColor, _bUseMeshTrailTime, _fMeshTrailTime);
	if(m_pCurrentWeapon == m_pPlayerWeapons.find("Part_CrossBow_1_L")->second)
		m_pPlayerWeapons.find("Part_CrossBow_1_R")->second ->SetUseTrail(_bUseMeshTrail, _vMeshTrailColor, _bUseMeshTrailTime, _fMeshTrailTime);
}

_bool CPlayer::TestKeyInput()
{
	//if (GAMEINSTANCE->KeyDown(DIK_9))
	//{
	//	PlayEffect("Plr_Parrying_Staff");
	//	return true;
	//}
	//else if (GAMEINSTANCE->KeyDown(DIK_8))
	//{
	//	PlayEffect("Boss_Special1");
	//	return true;
	//}
	//if (GAMEINSTANCE->KeyDown(DIK_7))
	//{
	//	//PlayEffect("Plr_Parrying_Sword");
	//	PlayEffect("Env_Fire");
	//	return true;
	//}
	//if (GAMEINSTANCE->KeyDown(DIK_8))
	//{
	//	//PlayEffect("Boss_Special2");
	//	PlayEffect("Boss_Teleport");
	//	return true;
	//}
	//if (GAMEINSTANCE->KeyDown(DIK_7))
	//{
	//	/*EFFECTMGR->PlayEffect("Boss_Special_Large2", shared_from_this(), true, 10.f);
	//	EFFECTMGR->PlayEffect("Boss_Special2_Large_Sphere", shared_from_this(), true, 10.f);
	//	EFFECTMGR->PlayEffect("Boss_Special2_Sphere", shared_from_this(), true, 10.f);*/
	//	EFFECTMGR->PlayEffect("Boss_Special2", shared_from_this(), true, 20.f);
	//	return true;
	//}
	/*if (GAMEINSTANCE->KeyDown(DIK_7))
	{
		ChangeAnim(68, 0.05f, false, 0.8f);
		PlayEffect("Plr_Sk_MultiShot");
		return true;
	}

	if (GAMEINSTANCE->KeyDown(DIK_9))
	{
		ChangeAnim(82, 0.05f, false, 0.8f);
		PlayEffect("Plr_Sk_FuriousFire2");
		return true;
	}*/

	/*if (GAMEINSTANCE->KeyDown(DIK_8))
	{
		ChangeAnim(WEAPON_SWORD_DAMAGED_NORMAL_B, 0.05f, false, 0.8f);
		_int iRand = rand() % 2;
		if (iRand)
			PlayEffect("Hit_Left");
		else
			PlayEffect("Hit_Right");
		return true;
	}*/

	/*if (GAMEINSTANCE->KeyDown(DIK_9))
	{
		ChangeAnim(WEAPON_SWORD_DAMAGED_STIFF_SHORT, 0.05f, false, 0.8f);
		PlayEffect("Hit_Blood_Small");
		return true;
	}*/

	/*if (GAMEINSTANCE->KeyDown(DIK_9))
	{
		ChangeAnim(WEAPON_SWORD_DAMAGED_STIFF_SHORT, 0.05f, false, 0.8f);
		PlayEffect("Hit_Center_Big");
		return true;
	}*/

	if (GAMEINSTANCE->KeyDown(DIK_N))
	{
		SetPlayerHp(-50.f);
	}

	if (GAMEINSTANCE->KeyDown(DIK_M))
	{
		SetPlayerHp(50.f);
	}

	if (GAMEINSTANCE->KeyDown(DIK_LCONTROL))
	{
		m_fCurrentSupremeGauge += 100.f;
		weak_ptr<CUIUVBar> supremeBar = dynamic_pointer_cast<CUIUVBar>(UIMGR->FindUI("PCInfoGaugeMP"));
		if (supremeBar.expired())
			return true;

		supremeBar.lock()->SetBar(m_fCurrentSupremeGauge, m_fSupremeGaugeMax);
		return true;
	}

	//if (GAMEINSTANCE->KeyDown(DIK_HOME))
	//{
	//	shared_ptr<CGameObject> pAmitoy = GAMEINSTANCE->GetGameObject(LEVEL_FONOS, TEXT("Layer_Amitoy"));
	//	//pAmitoy->SetEnable(true);
	//	dynamic_pointer_cast<CFonosAmitoy>(pAmitoy)->SetTriggerOn(true);

	//	//CCameraMgr::GetInstance()->CameraTurnOn("FonosAmitoyCamera");

	//	//m_IsEnabled = false;
	//}

}

HRESULT CPlayer::AddMotionTrail(_int _iTrailCnt)
{
	if (0 >= _iTrailCnt)
		return E_FAIL;

	CMotionTrail::MOTIONTRAIL_DESC MotionTrail_Desc = {};
	MotionTrail_Desc.pModel = m_pModelCom;
	MotionTrail_Desc.iSize = _iTrailCnt;
	MotionTrail_Desc.fLifeTime = 0.3f;
	MotionTrail_Desc.vColor = _float3(0.427f, 0.227f, 0.573f);
	MotionTrail_Desc.fTrailCntPerTick = 0.1f;

	// Player
	m_pMotionTrail.push_back(CMotionTrail::Create());

	_int iExceptMeshIdxs[1] = {0};
	m_pMotionTrail[0]->SettingMotionTrail(MotionTrail_Desc, shared_from_this(), 1, iExceptMeshIdxs);

	if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, TEXT("Layer_Effect"), m_pMotionTrail[0])))
		return E_FAIL;

	m_pMotionTrail[0]->SetEnable(false);

	_int iIndex = 1;

	// Equipment
	for (auto& iter : m_pPlayerEquipments)
	{
		if (ITEM_EQUIP_GLOVE == iter->GetEquipType())
			continue;

		m_pMotionTrail.push_back(CMotionTrail::Create());

		MotionTrail_Desc.pModel = dynamic_pointer_cast<CModel>(iter->GetComponent(L"Com_Model"));

		m_pMotionTrail[iIndex]->SettingMotionTrail(MotionTrail_Desc, shared_from_this());

		if (FAILED(GAMEINSTANCE->AddObject(LEVEL_STATIC, TEXT("Layer_Effect"), m_pMotionTrail[iIndex])))
			return E_FAIL;

		m_pMotionTrail[iIndex]->SetEnable(false);

		iIndex++;
	}
	return S_OK;
}

HRESULT CPlayer::SetUseMotionTrail(_bool _bUseMotionTrail, _bool _bUseTimer, _float _fTimer, _bool _bChangeColor, _float4 _vColor)
{
	for (size_t i = 0; i < m_pMotionTrail.size(); i++)
	{
		m_pMotionTrail[i]->ClearMotionTrail();

		if (!_bUseMotionTrail)
		{
			m_pMotionTrail[i]->SetEnable(false);

			return S_OK;
		}

		if (_bUseTimer)
			m_pMotionTrail[i]->SetTimer(true, _fTimer);

		if (_bChangeColor)
			m_pMotionTrail[i]->SetColor(_vColor);

		m_pMotionTrail[i]->SetEnable(true);
	}

	return S_OK;
}

shared_ptr<CPlayer> CPlayer::Create()
{
	shared_ptr<CPlayer> pInstance = make_shared<CPlayer>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CPlayer");

	return pInstance;
}
