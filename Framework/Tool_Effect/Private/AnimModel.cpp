#include "AnimModel.h"
#include "Model.h"
#include "RigidBody.h"

CAnimModel::CAnimModel()
{
}

CAnimModel::~CAnimModel()
{
}

HRESULT CAnimModel::Initialize(shared_ptr<class CModel> _pModel)
{
	__super::Initialize(nullptr);

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_ANIM;
	m_iShaderPass = 0;

	m_pModelCom = _pModel;
	// m_pTransformCom->LookAtForLandObject(_float3{0.f, 0.f, -1.f});
	m_pModelCom->ChangeAnim(0, 0.1f, true);
	//m_pModelCom->ReadySeparate("Bip001-Spine");
	AddCollider();
	//AddWeapons();
	m_eCurrentWeapon = WEAPON_TYPE_SWORD;
	//m_eCurrentWeapon = WEAPON_TYPE_CROSSBOW;
	//m_eCurrentWeapon = WEAPON_TYPE_STAFF;
	//AddRigidBody();
	//m_pTransformCom->SetScaling(1.0f, 1.0f, 1.0f);
	m_pTransformCom->SetSpeed(3.f);
	m_pTransformCom->SetScaling(1.3f, 1.3f, 1.3f);
	//m_pTransformCom->SetScaling(1.f, 1.f, 1.f);
	m_pCurrentCollider = m_pBoxCollider;

	AddComponent(L"Com_Model", m_pModelCom);
	return S_OK;
}

void CAnimModel::PriorityTick(_float _fTimeDelta)
{
}

void CAnimModel::Tick(_float _fTimeDelta)
{
	m_pModelCom->PlayAnimation(_fTimeDelta);
	m_pTransformCom->SetScaling(1.f, 1.f, 1.f);
	//m_pTransformCom->Tick(_fTimeDelta);

	//m_pSphereCollider->Tick(m_pTransformCom->GetWorldMatrix());
	//m_pBoxCollider->Tick(m_pTransformCom->GetWorldMatrix());

	if (isColliderOn)
		m_pCurrentCollider->Tick(m_pTransformCom->GetWorldMatrix());

	if (GAMEINSTANCE->KeyPressing(DIK_K))
	{
		m_pTransformCom->RotationPlus({ 0.f, 1.f, 0.f }, _fTimeDelta);
	}

	if (GAMEINSTANCE->KeyPressing(DIK_I))
	{
		m_pTransformCom->GoStraight(_fTimeDelta);
	}

	if (GAMEINSTANCE->KeyPressing(DIK_J))
	{
		m_pTransformCom->GoLeft(_fTimeDelta);
	}

	if (GAMEINSTANCE->KeyPressing(DIK_L))
	{
		m_pTransformCom->GoRight(_fTimeDelta);
	}
}

void CAnimModel::LateTick(_float _fTimeDelta)
{
	//m_pTransformCom->LateTick(_fTimeDelta);

	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
	//RenderWeapon(_fTimeDelta);

#ifdef _DEBUG
	//if (m_pRigidBody)
	//	GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
	//GAMEINSTANCE->AddDebugComponent(m_pSphereCollider);
	//GAMEINSTANCE->AddDebugComponent(m_pBoxCollider);
#ifdef _DEBUG
	if (isColliderOn)
		GAMEINSTANCE->AddDebugComponent(m_pCurrentCollider);
#endif // _DEBUG
}

HRESULT CAnimModel::Render()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++) {
		GAMEINSTANCE->BeginAnimModel(m_pModelCom, (_uint)i);
	}

	return S_OK;
}

void CAnimModel::RenderWeapon(_float _fTimeDelta)
{
	//m_pPlayerWeapons.find("Weapon")->second->LateTick(_fTimeDelta);

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

void CAnimModel::SetCollider(CCollider::ECOL_TYPE _ColType)
{
	if (_ColType == CCollider::SPHERE_TYPE)
	{
		m_pCurrentCollider = m_pSphereCollider;
	}
	else if (_ColType == CCollider::OBB_TYPE)
	{
		m_pCurrentCollider = m_pBoxCollider;
	}
}

shared_ptr<CCollider> CAnimModel::GetCollider()
{
	return m_pCurrentCollider;
}

void CAnimModel::EnableCollision(const char* _strShapeTag)
{
	EnableCollider();
}

void CAnimModel::DisableCollision(const char* _strShapeTag)
{
	DisableCollider();
}


HRESULT CAnimModel::AddWeapons()
{
	shared_ptr<CWeapon::WEAPON_DESC> pWeaponDesc = make_shared<CWeapon::WEAPON_DESC>();
	pWeaponDesc->tPartObject.pParentTransform = m_pTransformCom;
	pWeaponDesc->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_RightHand");

	pWeaponDesc->strModelKey = "Sword_Sword_LivingArmor_Soldier";
	pWeaponDesc->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(-90.f)) *
		SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(-90.f));

	shared_ptr<CTransform::TRANSFORM_DESC> pTransformDesc = make_shared<CTransform::TRANSFORM_DESC>();

	pTransformDesc->fSpeedPerSec = 10.f;
	pTransformDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	shared_ptr<CWeapon> pWeapon = CWeapon::Create(pWeaponDesc.get(), pTransformDesc.get());
	if (nullptr == pWeapon)
		return E_FAIL;

	m_pPlayerWeapons.emplace("Weapon", pWeapon);

	/*shared_ptr<CWeapon::WEAPON_DESC> pWeaponDescSword = make_shared<CWeapon::WEAPON_DESC>();
	pWeaponDescSword->tPartObject.pParentTransform = m_pTransformCom;
	pWeaponDescSword->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_RightHand");
	pWeaponDescSword->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(90.f)) *
		SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(270.f));
	pWeaponDescSword->strModelKey = "Sword2h_1";

	shared_ptr<CTransform::TRANSFORM_DESC> pTransformDescSword = make_shared<CTransform::TRANSFORM_DESC>();
	pTransformDescSword->fSpeedPerSec = 1.f;
	pTransformDescSword->fRotationPerSec = XMConvertToRadians(90.0f);

	shared_ptr<CWeapon> pWeaponSword = CWeapon::Create(pWeaponDescSword.get(), pTransformDescSword.get());
	if (nullptr == pWeaponSword)
		return E_FAIL;


	m_pPlayerWeapons.emplace("Weapon", pWeaponSword);*/

	// Sword
	shared_ptr<CWeapon::WEAPON_DESC> pWeaponDescSword = make_shared<CWeapon::WEAPON_DESC>();
	pWeaponDescSword->tPartObject.pParentTransform = m_pTransformCom;
	pWeaponDescSword->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_RightHand");
	pWeaponDescSword->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(90.f)) *
		SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(270.f));
	pWeaponDescSword->strModelKey = "Sword2h_1";

	shared_ptr<CTransform::TRANSFORM_DESC> pTransformDescSword = make_shared<CTransform::TRANSFORM_DESC>();
	pTransformDescSword->fSpeedPerSec = 1.f;
	pTransformDescSword->fRotationPerSec = XMConvertToRadians(90.0f);

	shared_ptr<CWeapon> pWeaponSword = CWeapon::Create(pWeaponDescSword.get(), pTransformDescSword.get());
	if (nullptr == pWeaponSword)
		return E_FAIL;

	m_pPlayerWeapons.emplace("Part_Sword2h_1", pWeaponSword);


	// CrossBowL
	shared_ptr<CWeapon::WEAPON_DESC> pWeaponDescCrossBowL = make_shared<CWeapon::WEAPON_DESC>();
	pWeaponDescCrossBowL->tPartObject.pParentTransform = m_pTransformCom;
	pWeaponDescCrossBowL->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_LeftHand");
	pWeaponDescCrossBowL->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(90.f)) *
		SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(270.f));
	pWeaponDescCrossBowL->strModelKey = "CrossBow_1";

	shared_ptr<CTransform::TRANSFORM_DESC> pTransformDescCrossBowL = make_shared<CTransform::TRANSFORM_DESC>();
	pTransformDescCrossBowL->fSpeedPerSec = 1.f;
	pTransformDescCrossBowL->fRotationPerSec = XMConvertToRadians(90.0f);

	shared_ptr<CWeapon> pWeaponCrossBowL = CWeapon::Create(pWeaponDescCrossBowL.get(), pTransformDescCrossBowL.get());
	if (nullptr == pWeaponCrossBowL)
		return E_FAIL;

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

	m_pPlayerWeapons.emplace("Part_CrossBow_1_R", pWeaponCrossBowR);


	// Staff
	shared_ptr<CWeapon::WEAPON_DESC> pWeaponDescStaff = make_shared<CWeapon::WEAPON_DESC>();
	pWeaponDescStaff->tPartObject.pParentTransform = m_pTransformCom;
	pWeaponDescStaff->pSocketMatrix = m_pModelCom->GetCombinedBoneMatrixPtr("Bn_Action_LeftHand");
	pWeaponDescStaff->OffsetMatrixNormal = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(90.f)) *
		SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(270.f));
	pWeaponDescStaff->strModelKey = "Staff_1";

	shared_ptr<CTransform::TRANSFORM_DESC> pTransformDescStaff = make_shared<CTransform::TRANSFORM_DESC>();
	pTransformDescStaff->fSpeedPerSec = 1.f;
	pTransformDescStaff->fRotationPerSec = XMConvertToRadians(90.0f);

	shared_ptr<CWeapon> pWeaponStaff = CWeapon::Create(pWeaponDescStaff.get(), pTransformDescStaff.get());
	if (nullptr == pWeaponStaff)
		return E_FAIL;

	m_pPlayerWeapons.emplace("Part_Staff_1", pWeaponStaff);

	return S_OK;
}

HRESULT CAnimModel::AddRigidBody()
{
	//CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	//
	//RigidBodyDesc.isStatic = false;
	//RigidBodyDesc.isGravity = false;
	//RigidBodyDesc.isTrigger = true;
	//RigidBodyDesc.isInitCol = true;
	//RigidBodyDesc.pTransform = m_pTransformCom;
	//RigidBodyDesc.pOwnerObject = shared_from_this();
	//
	//GeometryBox BoxDesc;
	//BoxDesc.vSize = _float3{ 0.8f, 0.6f, 0.8f };
	//BoxDesc.vOffSetPosition = { 0.0f, 0.15f, 0.0f };
	//BoxDesc.strShapeTag = "Test";
	//RigidBodyDesc.pGeometry = &BoxDesc;
	//
	//RigidBodyDesc.eThisColFlag = COL_PLAYER;
	//RigidBodyDesc.eTargetColFlag = COL_STATIC;
	//m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

	return S_OK;
}

HRESULT CAnimModel::AddCollider()
{

	// Shpere 테스트 콜라이더 //
	CBounding_Sphere::SphereDesc		SphereDesc{};
	SphereDesc.fRadius = 1.f;
	SphereDesc.vCenter = _float3(0.f, 1.f, 0.f);

	m_pSphereCollider = CCollider::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), CCollider::SPHERE_TYPE, &SphereDesc);

	if (nullptr == m_pSphereCollider)
		return E_FAIL;

	// BoX 테스트 콜라이더 //
	CBounding_OBB::OBBDesc		BoxDesc{};
	BoxDesc.vExtents = { 1.f, 1.f, 1.f };
	BoxDesc.vRadians = { 0.f ,0.f, 0.f };
	BoxDesc.vCenter = _float3(0.f, 1.f, 0.f);

	m_pBoxCollider = CCollider::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), CCollider::OBB_TYPE, &BoxDesc);

	return S_OK;
}

shared_ptr<CAnimModel> CAnimModel::Create(shared_ptr<class CModel> _pModel)
{
	shared_ptr<CAnimModel> pInstance = make_shared<CAnimModel>();

	if (FAILED(pInstance->Initialize(_pModel)))
		MSG_BOX("Failed to Create : CAnimModel");

	return pInstance;
}
