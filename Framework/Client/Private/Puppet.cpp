#include "Puppet.h"
#include "Model.h"
#include "RigidBody.h"
#include "EffectMgr.h"
#include "QuestMgr.h"

CPuppet::CPuppet()
{
}

CPuppet::~CPuppet()
{
}

HRESULT CPuppet::Initialize(shared_ptr<Puppet_Desc> _pPuppetDesc)
{
	shared_ptr<MONSTER_DESC> pMonsterDesc = make_shared<CMonster::MONSTER_DESC>();
	pMonsterDesc->pTarget = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));

	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 0.f;
	pArg->fRotationPerSec = XMConvertToRadians(90.0f);

	__super::Initialize(pMonsterDesc.get(), pArg.get());

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_ANIM;
	m_iShaderPass = 2;

	m_pModelCom = GAMEINSTANCE->GetModel("Puppet");
	AddComponent(L"Com_Model", m_pModelCom);
	AddRigidBody("Puppet");
	m_strModelName = "Puppet";
	m_pModelCom->ChangeAnim(1, 0.f, true);

	m_pTransformCom->SetState(CTransform::STATE_POSITION, _pPuppetDesc->vMyPos);
	m_pTransformCom->Tick(0.f);
	//m_pTransformCom->Rotation(_float3(0.f, 1.f, 0.f), 3.14f / 2.f);
	return S_OK;
}

void CPuppet::PriorityTick(_float _fTimeDelta)
{
}

void CPuppet::Tick(_float _fTimeDelta)
{
	if (m_pModelCom->GetIsFinishedAnimation())
	{
		m_pModelCom->ChangeAnim(1, 0.05f, true);
	}

	m_pModelCom->PlayAnimation(_fTimeDelta);
}

void CPuppet::LateTick(_float _fTimeDelta)
{
	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
	GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());

#ifdef _DEBUG
	if (m_pRigidBody)
		GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
}

HRESULT CPuppet::Render()
{
	__super::Render();

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		GAMEINSTANCE->BeginAnimModel(m_pModelCom, i);
	}

	if (m_bUsingRimLight)
	{
		_bool bRimOff = false;
		GAMEINSTANCE->BindRawValue("g_IsUsingRim", &bRimOff, sizeof(bRimOff));
	}

	return S_OK;
}

HRESULT CPuppet::RenderShadow()
{
	__super::RenderShadow();
	return S_OK;
}

void CPuppet::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.eColFlag == COL_PLAYERWEAPON ||
		_ColData.eColFlag == COL_PLAYERPROJECTILE ||
		_ColData.eColFlag == COL_PLAYERSKILL)
	{
		m_pModelCom->ChangeAnim(0, 0.05f);
		m_pModelCom->PlayAnimation(0.f);
	}
	__super::ShowEffect(_ColData, _szMyShapeTag);
}

void CPuppet::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CPuppet::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

HRESULT CPuppet::AddRigidBody(string _strModelKey)
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	
	//==º»Ã¼==
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;
	
	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 1.0f;
	RigidBodyDesc.fRestitution = 0.0f;
	
	GeometryCapsule CapsuleDesc;
	CapsuleDesc.fHeight = 1.5f;
	CapsuleDesc.fRadius = 0.4f;
	CapsuleDesc.vOffSetPosition = { 0.0f, 1.15f, 0.0f };
	CapsuleDesc.vOffSetRotation = { 0.0f, 0.0f, 90.0f };
	CapsuleDesc.strShapeTag = "Puppet_Body";
	RigidBodyDesc.pGeometry = &CapsuleDesc;
	
	RigidBodyDesc.eThisColFlag = COL_MONSTER;
	RigidBodyDesc.eTargetColFlag = COL_PLAYER | COL_PLAYERWEAPON | COL_PLAYERPROJECTILE | COL_PLAYERSKILL | COL_CHECKBOX;
	m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
	
	m_pTransformCom->SetRigidBody(m_pRigidBody);

	return S_OK;
}

shared_ptr<CPuppet> CPuppet::Create(shared_ptr<Puppet_Desc> _pPuppetDesc)
{
	shared_ptr<CPuppet> pInstance = make_shared<CPuppet>();

	if (FAILED(pInstance->Initialize(_pPuppetDesc)))
		MSG_BOX("Failed to Create : CPuppet");

	return pInstance;
}
