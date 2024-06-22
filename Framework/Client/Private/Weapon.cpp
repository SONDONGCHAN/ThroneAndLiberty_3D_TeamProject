#include "Weapon.h"
#include "Model.h"
#include "RigidBody.h"
#include "MeshTrail.h"

CWeapon::CWeapon()
	: CPartObject()
{
}

CWeapon::~CWeapon()
{
}

HRESULT CWeapon::Initialize(WEAPON_DESC* _pWeaponDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
	WEAPON_DESC* pWeaponDesc = _pWeaponDesc;

	m_pSocketMatrix = pWeaponDesc->pSocketMatrix;
	m_strModelKey = pWeaponDesc->strModelKey;
	m_OffsetMatrixNormal = pWeaponDesc->OffsetMatrixNormal;
	m_OffsetMatrixSituation = pWeaponDesc->OffsetMatrixSituation;

	PARTOBJ_DESC* pPartObjDesc = (PARTOBJ_DESC*)_pWeaponDesc;

	__super::Initialize(pPartObjDesc, _pTransformDesc);

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_NONANIM;
	m_iShaderPass = 0;

	m_pModelCom = GAMEINSTANCE->GetModel(m_strModelKey);

	shared_ptr<GeometryCapsule> CapsuleDesc = make_shared<GeometryCapsule>();
	/*if (m_strModelKey == "Sword2h_1")
	{
		CapsuleDesc->fHeight = 1.f;
		CapsuleDesc->fRadius = 0.2f;
		CapsuleDesc->vOffSetPosition = { 0.0f, 0.75f, 0.0f };
		CapsuleDesc->vOffSetRotation = { 0.0f, 0.0f, 90.0f };
		CapsuleDesc->strShapeTag = "Player_Sword2h_1_Body";
		m_strShapeTag = "Player_Sword2h_1_Body";
		AddRigidBody(CapsuleDesc, COL_PLAYERWEAPON, COL_MONSTER | COL_MONSTERWEAPON | COL_PROJECTILE);
		m_bCollisionActive = true;
		m_bPrevCollisionActive = true;
	}*/
	// NPC_DAVID 등뒤 검
	if (m_strModelKey == "Sword2h_Sample2")
	{
		m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(0.15f, -0.2f, 0.f));
	}
	// NPC_Henry Torch
	if (m_strModelKey == "IT_Torch_00001_SK")
	{
		m_pTransformCom->SetScaling(0.5f, 0.5f, 0.5f);
	}
	// Knight Shield
	if (m_strModelKey == "IT_P_Shield_00009_SK")
	{
		m_pTransformCom->SetScaling(0.8f, 0.8f, 0.8f);
	}
	// RedKnight
	if (m_strModelKey == "N_Sword2h_UndeadArmyKnight")
	{
		CapsuleDesc->fHeight = 2.6f;
		CapsuleDesc->fRadius = 0.5f;
		CapsuleDesc->vOffSetPosition = { 0.0f, 1.55f, 0.0f };
		CapsuleDesc->vOffSetRotation = { 0.0f, 0.0f, 90.0f };
		CapsuleDesc->strShapeTag = "RedKnight_Weapon_Body";
		m_strShapeTag = "RedKnight_Weapon_Body";
		AddRigidBody(CapsuleDesc, COL_MONSTERWEAPON, COL_PLAYER | COL_PLAYERPARRYING);
		m_bCollisionActive = false;
		m_bPrevCollisionActive = false;
	}
	// 고블린 필드보스
	if (m_strModelKey == "IT_N_Sword_DemonGoat_Butcher")
	{
		CapsuleDesc->fHeight = 2.6f;
		CapsuleDesc->fRadius = 0.5f;
		CapsuleDesc->vOffSetPosition = { 0.0f, 1.95f, 0.0f };
		CapsuleDesc->vOffSetRotation = { 0.0f, 0.0f, 90.0f };
		CapsuleDesc->strShapeTag = "IT_N_Sword_DemonGoat_Butcher_Body";
		m_strShapeTag = "IT_N_Sword_DemonGoat_Butcher_Body";
		AddRigidBody(CapsuleDesc, COL_MONSTERWEAPON, COL_PLAYER | COL_PLAYERPARRYING);
		m_pTransformCom->SetScaling(1.f, 1.5f, 1.f);
		m_bCollisionActive = false;
		m_bPrevCollisionActive = false;
	}
	// 오크 필드보스
	if (m_strModelKey == "IT_N_Blunt_OrcLowDarkCommander")
	{
		CapsuleDesc->fHeight = 1.8f;
		CapsuleDesc->fRadius = 0.5f;
		CapsuleDesc->vOffSetPosition = { 0.0f, 1.55f, 0.0f };
		CapsuleDesc->vOffSetRotation = { 0.0f, 0.0f, 90.0f };
		CapsuleDesc->strShapeTag = "IT_N_Blunt_OrcLowDarkCommander";
		m_strShapeTag = "IT_N_Blunt_OrcLowDarkCommander";
		AddRigidBody(CapsuleDesc, COL_MONSTERWEAPON, COL_PLAYER | COL_PLAYERPARRYING);
		m_pTransformCom->SetScaling(1.3f, 1.3f, 1.3f);
		m_bCollisionActive = false;
		m_bPrevCollisionActive = false;
	}
	// 오크
	if (m_strModelKey == "Sword_OrcWarrior")
	{
		CapsuleDesc->fHeight = 1.f;
		CapsuleDesc->fRadius = 0.2f;
		CapsuleDesc->vOffSetPosition = { 0.0f, 0.75f, 0.0f };
		CapsuleDesc->vOffSetRotation = { 0.0f, 0.0f, 90.0f };
		CapsuleDesc->strShapeTag = "Sword_OrcWarrior_Body";
		m_strShapeTag = "Sword_OrcWarrior_Body";
		AddRigidBody(CapsuleDesc, COL_MONSTERWEAPON, COL_PLAYER | COL_PLAYERPARRYING);
		m_bCollisionActive = false;
		m_bPrevCollisionActive = false;
	}
	if (m_strModelKey == "IT_N_OrcShaman")
	{
		CapsuleDesc->fHeight = 2.f;
		CapsuleDesc->fRadius = 0.2f;
		CapsuleDesc->vOffSetPosition = { 0.0f, 0.1f, 0.0f };
		CapsuleDesc->vOffSetRotation = { 0.0f, 0.0f, 90.0f };
		CapsuleDesc->strShapeTag = "IT_N_OrcShaman_Body";
		m_strShapeTag = "IT_N_OrcShaman_Body";
		AddRigidBody(CapsuleDesc, COL_MONSTERWEAPON, COL_PLAYER | COL_PLAYERPARRYING);
		m_bCollisionActive = false;
		m_bPrevCollisionActive = false;
	}
	// 고블린
	if (m_strModelKey == "Axe_L1MidgetFootman")
	{
		CapsuleDesc->fHeight = 0.8f;
		CapsuleDesc->fRadius = 0.2f;
		CapsuleDesc->vOffSetPosition = { 0.0f, 0.f, 0.0f };
		CapsuleDesc->vOffSetRotation = { 0.0f, 0.0f, 90.0f };
		CapsuleDesc->strShapeTag = "Axe_L1MidgetFootman_Body";
		m_strShapeTag = "Axe_L1MidgetFootman_Body";
		AddRigidBody(CapsuleDesc, COL_MONSTERWEAPON, COL_PLAYER | COL_PLAYERPARRYING);
		m_bCollisionActive = false;
		m_bPrevCollisionActive = false;
	}
	if (m_strModelKey == "N_Staff_KoboldMiddleShaman")
	{
		CapsuleDesc->fHeight = 2.f;
		CapsuleDesc->fRadius = 0.2f;
		CapsuleDesc->vOffSetPosition = { 0.0f, 0.1f, 0.0f };
		CapsuleDesc->vOffSetRotation = { 0.0f, 0.0f, 90.0f };
		CapsuleDesc->strShapeTag = "N_Staff_KoboldMiddleShaman_Body";
		m_strShapeTag = "N_Staff_KoboldMiddleShaman_Body";
		AddRigidBody(CapsuleDesc, COL_MONSTERWEAPON, COL_PLAYER | COL_PLAYERPARRYING);
		m_bCollisionActive = false;
		m_bPrevCollisionActive = false;
	}
	// 리빙아머
	if (m_strModelKey == "Sword_Sword_LivingArmor_Soldier")
	{
		CapsuleDesc->fHeight = 0.8f;
		CapsuleDesc->fRadius = 0.2f;
		CapsuleDesc->vOffSetPosition = { 0.0f, 0.5f, 0.0f };
		CapsuleDesc->vOffSetRotation = { 0.0f, 0.0f, 90.0f };
		CapsuleDesc->strShapeTag = "Sword_Sword_LivingArmor_Soldier_Body";
		m_strShapeTag = "Sword_Sword_LivingArmor_Soldier_Body";
		AddRigidBody(CapsuleDesc, COL_MONSTERWEAPON, COL_PLAYER | COL_PLAYERPARRYING);
		m_bCollisionActive = false;
		m_bPrevCollisionActive = false;
	}

	m_CurrentOffset = m_OffsetMatrixNormal;

	return S_OK;
}

void CWeapon::PriorityTick(_float _fTimeDelta)
{
}

void CWeapon::Tick(_float _fTimeDelta)
{
	if (m_strShapeTag != "")
	{
		if (m_bPrevCollisionActive)
		{
			if (!m_bCollisionActive)
			{
				m_bCollisionActive = false;
				m_bPrevCollisionActive = false;
				m_pRigidBody->DisableCollision(m_strShapeTag);
			}
		}
		else
		{
			if (!m_bPrevCollisionActive)
			{
				if (m_bCollisionActive)
				{
					m_bCollisionActive = true;
					m_bPrevCollisionActive = true;
					m_pRigidBody->EnableCollision(m_strShapeTag);
				}
			}
		}
	}

	//UpdateTickWeapon("Sword2h_1");
	//UpdateTickWeapon("N_Sword2h_UndeadArmyKnight");
	UpdateTickWeapon("IT_N_Sword_DemonGoat_Butcher");
	/*UpdateTickWeapon("Sword_OrcWarrior");
	UpdateTickWeapon("IT_N_OrcShaman");
	UpdateTickWeapon("Axe_L1MidgetFootman");*/
	//UpdateTickWeapon("N_Staff_KoboldMiddleShaman");
	//UpdateTickWeapon("Sword_Sword_LivingArmor_Soldier");

	if (m_bUseTrail)
	{
		m_pMeshTrail->Tick(_fTimeDelta);

		if (m_bUseTrailTimer)
		{
			if (m_fTrailTimer <= 0.f)
				m_bUseTrail = false;

			m_fTrailTimer -= _fTimeDelta;
		}
	}

	if (m_bUseGlow && m_bUseGlowTime)
	{
		m_fGlowTime -= _fTimeDelta;
		if (0 >= m_fGlowTime)
			m_bUseGlow = false;
	}

	// RimLight 서서히 빛나게
	if (m_bActiveRimLight)
	{
		m_fCalculateTime1 += _fTimeDelta;

		if (m_fCalculateTime1 > 1.f)
		{
		}
		else
		{
			m_fRimPow += _fTimeDelta * 3.3f;
		}
	}
}

void CWeapon::LateTick(_float _fTimeDelta)
{
	_float4x4 BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	BoneMatrix.Right(XMVector3Normalize(BoneMatrix.Right()));
	BoneMatrix.Up(XMVector3Normalize(BoneMatrix.Up()));
	BoneMatrix.Forward(XMVector3Normalize(BoneMatrix.Forward()));

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMatrix() * m_CurrentOffset * BoneMatrix * m_pParentTransform->GetWorldMatrix());

	if (GAMEINSTANCE->isInWorldSpace(m_WorldMatrix.Translation(), 3.0f))
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

		if (m_bUseGlow)
			GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_GLOW, shared_from_this());

		if (m_bUseTrail)
			m_pMeshTrail->LateTick(_fTimeDelta);

	//if (m_IsEdgeRender)
		//GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_EDGE, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody && m_bCollisionActive)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG

		GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());
	}
}

HRESULT CWeapon::Render()
{

	if (m_bActiveOn)
	{
		GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
		GAMEINSTANCE->BindWVPMatrixPerspective(m_WorldMatrix);

		_uint iNumMeshes = m_pModelCom->GetNumMeshes();

		if (m_bActiveRimLight)
		{
			UsingRimLight(m_vRimLight, m_fRimPow);
		}

		for (size_t i = 0; i < iNumMeshes; i++) {
			GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
		}
		
		/*RimLight Off*/
		_bool bRimOff = false;
		GAMEINSTANCE->BindRawValue("g_IsUsingRim", &bRimOff, sizeof(bRimOff));
	}

	if (m_bUseTrail)
	{
		if (0 == m_iGeneratedTrail % 1)
		{
			m_iGeneratedTrail = 0;
			CMeshTrail::MESHTRAIL_ELEMENT_DESC MeshTrail_Element_Desc = {};
			MeshTrail_Element_Desc.fLifeTime = 0.3f;
			MeshTrail_Element_Desc.vColor = { 1.f, 1.f, 1.f };
			MeshTrail_Element_Desc.WorldMatrix = m_WorldMatrix;

			m_pMeshTrail->AddMeshTrail(MeshTrail_Element_Desc);
		}

		m_iGeneratedTrail++;
	}

	return S_OK;
}

HRESULT CWeapon::RenderShadow()
{
	if (m_bActiveOn)
	{
		GAMEINSTANCE->ReadyShader(m_eShaderType, 1);
		GAMEINSTANCE->BindWVPMatrixPerspective(m_WorldMatrix);

		GAMEINSTANCE->BindLightProjMatrix();

		_uint iNumMeshes = m_pModelCom->GetNumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++) {
			GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
		}
	}

	return S_OK;
}

HRESULT CWeapon::RenderGlow()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 4);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_WorldMatrix);

	_float fScale = 1.f;

	/*m_bUseGlowColor = true;
	m_vGlowColor = { 1.f, 0.f, 0.f, 0.8f };*/

	GAMEINSTANCE->BindRawValue("g_bUseGlowColor", &m_bUseGlowColor, sizeof(_bool));
	GAMEINSTANCE->BindRawValue("g_vGlowColor", &m_vGlowColor, sizeof(_float4));
	GAMEINSTANCE->BindRawValue("g_fScale", &fScale, sizeof(_float));

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++) {
		GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
	}

	return S_OK;
}

HRESULT CWeapon::RenderEdgeDepth()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 2);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_WorldMatrix);

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++) {
		GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
	}

	return S_OK;
}

HRESULT CWeapon::RenderEdge()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 6);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_WorldMatrix);

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++) {
		GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
	}

	return S_OK;
}

void CWeapon::OnRimLight(_float3 _fRimLight, _float _fPow)
{
	m_bActiveRimLight = true;
	m_vRimLight = _fRimLight;
	//m_fRimPow = _fPow;
}

void CWeapon::OffRimLight()
{
	m_bActiveRimLight = false;
	m_fCalculateTime1 = 0.f;
	m_fRimPow = 0.f;
}

void CWeapon::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	// 보스 무기 패링
	if ((_szMyShapeTag == "RedKnight_Weapon_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = true;
	}
	// 고블린 필드보스 무기 패링
	if ((_szMyShapeTag == "IT_N_Sword_DemonGoat_Butcher_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = true;
	}
	// 오크 전사 패링
	if ((_szMyShapeTag == "Sword_OrcWarrior_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = true;
	}
	// 오크 샤먼 패링
	if ((_szMyShapeTag == "IT_N_OrcShaman_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = true;
	}
	// 리빙아머 전사 패링
	if ((_szMyShapeTag == "Sword_Sword_LivingArmor_Soldier_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = true;
	}
	// 고블린 전사 패링
	if ((_szMyShapeTag == "Axe_L1MidgetFootman_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = true;
	}
	// 고블린 샤먼 패링
	if ((_szMyShapeTag == "N_Staff_KoboldMiddleShaman_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = true;
	}
}

void CWeapon::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CWeapon::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	// 보스 무기 패링감지 취소
	if ((_szMyShapeTag == "RedKnight_Weapon_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = false;
	}
	// 고블린 필드보스 무기 패링감지 취소
	if ((_szMyShapeTag == "IT_N_Sword_DemonGoat_Butcher_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = false;
	}
	// 오크 전사 패링감지 취소
	if ((_szMyShapeTag == "Sword_OrcWarrior_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = false;
	}
	// 오크 샤먼 패링감지 취소
	if ((_szMyShapeTag == "IT_N_OrcShaman_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = false;
	}
	// 리빙아머 전사 패링감지 취소
	if ((_szMyShapeTag == "Sword_Sword_LivingArmor_Soldier_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = false;
	}
	// 고블린 전사 패링감지 취소
	if ((_szMyShapeTag == "Axe_L1MidgetFootman_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = false;
	}
	// 고블린 샤먼 패링감지 취소
	if ((_szMyShapeTag == "N_Staff_KoboldMiddleShaman_Body") && (_ColData.eColFlag == COL_PLAYERPARRYING))
	{
		m_bParried = false;
	}
}

void CWeapon::UpdateTickWeapon(const char* _szModelKey)
{
	_float3 scale;
	_quaternion quat;
	_float3 pos;
	if (m_strModelKey == _szModelKey)
	{
		m_WorldMatrix.Decompose(scale, quat, pos);
		m_pRigidBody->SetGlobalPos(m_WorldMatrix.Translation(), quat);
	}
}

HRESULT CWeapon::AddRigidBody(shared_ptr<Geometry> _pGeometry, ECOLLISIONFLAG _iThisCol, UINT _iTargetCol)
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestitution = 0.0f;

	//GeometryCapsule CapsuleDesc;
	//CapsuleDesc.fHeight = 1.f;
	//CapsuleDesc.fRadius = 0.25f;
	//CapsuleDesc.vOffSetPosition = { 0.0f, 0.75f, 0.0f };
	//CapsuleDesc.vOffSetRotation = { 0.0f, 0.0f, 90.0f };
	//CapsuleDesc.strShapeTag = "RedKnight_Weapon_Body";
	RigidBodyDesc.pGeometry = _pGeometry.get();

	RigidBodyDesc.eThisColFlag = _iThisCol;
	RigidBodyDesc.eTargetColFlag = _iTargetCol;
	m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

	m_pTransformCom->SetRigidBody(m_pRigidBody);

	return S_OK;
}

void CWeapon::SetUseTrail(_bool _bUseTrail, _float4 _vMeshTrailColor, _bool _bUseMeshTrailTime, _float _fMeshTrailTime)
{
	m_bUseTrail = _bUseTrail;
	m_pMeshTrail->ChangeColor(_vMeshTrailColor);
	m_bUseTrailTimer = _bUseMeshTrailTime;
	m_fTrailTimer = _fMeshTrailTime;
	m_pMeshTrail->ClearMeshTrail();
}

void CWeapon::SetUseGlowColor(_bool _bUseGlowColor, _float4 _vGlowColor)
{
	m_bUseGlowColor = _bUseGlowColor;
	m_vGlowColor = _vGlowColor;
	if (m_pMeshTrail)
		m_pMeshTrail->ChangeColor(m_vGlowColor);
}

void CWeapon::AddTrail(_uint _iTrailCnt, _uint _iTickPerTrailGen)
{
	if (0 >= _iTrailCnt)
		return;

	// 몇 틱 마다 생성할건지?
	m_iTickPerTrailGen = _iTickPerTrailGen;

	CMeshTrail::MESHTRAIL_DESC MeshTrail_Desc = {};
	MeshTrail_Desc.pModel = m_pModelCom;
	MeshTrail_Desc.iSize = _iTrailCnt;
	MeshTrail_Desc.fLifeTime = 0.3f;
	MeshTrail_Desc.vColor = { 0.98f, 0.576f, 0.31f };
	MeshTrail_Desc.ePassType = CMeshTrail::MESH_COLOR_ALPHA;

	m_pMeshTrail = CMeshTrail::Create();
	m_pMeshTrail->SettingMeshTrail(MeshTrail_Desc);
	m_bUseTrail = false;
}

shared_ptr<CWeapon> CWeapon::Create(WEAPON_DESC* _pWeaponDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
	shared_ptr<CWeapon> pInstance = make_shared<CWeapon>();

	if (FAILED(pInstance->Initialize(_pWeaponDesc, _pTransformDesc)))
		MSG_BOX("Failed to Create : Weapon");

	return pInstance;
}
