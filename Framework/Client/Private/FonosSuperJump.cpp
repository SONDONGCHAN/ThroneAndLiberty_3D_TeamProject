#include "FonosSuperJump.h"
#include "Model.h"
#include "Animation.h"
#include "RigidBody.h"

CFonosSuperJump::CFonosSuperJump()
{
}

CFonosSuperJump::~CFonosSuperJump()
{
}

shared_ptr<CFonosSuperJump> CFonosSuperJump::Create(TRAP_TYPE _eTrapType, _float3 _vPos, _float3 _vLook)
{
	shared_ptr<CFonosSuperJump> pInstance = make_shared<CFonosSuperJump>();

	if (FAILED(pInstance->Initialize(_eTrapType, _vPos, _vLook)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CFonosSuperJump::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CFonosSuperJump::Initialize(TRAP_TYPE _eTrapType, _float3 _vPos, _float3 _vLook)
{
	m_pTransformCom = CTransform::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo());
	m_pTransformCom->SetSpeed(4.f);

	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPos);
	m_pTransformCom->LookAtDirForLandObject(_vLook);

	string ModelTag = "Mushroom";
	m_pModelCom = GAMEINSTANCE->GetModel(ModelTag);
	AddComponent(L"Com_Model", m_pModelCom);

	m_pModelCom->ChangeAnim(0, 0.0f, true);
	m_pModelCom->PlayAnimation(0);
	//m_pModelCom->ChangeAnim(AMITOY_STATE_IDLE, 0.1f, true);

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_ANIM;
	m_iShaderPass = 0;

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	// Body (For. Move Block)
	{
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.isGravity = false;
		RigidBodyDesc.isInitCol = true;
		RigidBodyDesc.pTransform = m_pTransformCom;
		RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;

		RigidBodyDesc.pOwnerObject = shared_from_this();
		RigidBodyDesc.fStaticFriction = 1.0f;
		RigidBodyDesc.fDynamicFriction = 0.0f;
		RigidBodyDesc.fRestitution = 0.0f;

		GeometryBox	BodyBoxDesc;
		BodyBoxDesc.vSize = { 1.7f, 1.1f, 1.7f };
		BodyBoxDesc.strShapeTag = "SuperJump_Body";
		BodyBoxDesc.vOffSetPosition = { 0.f, 1.f, 0.f };
		BodyBoxDesc.vOffSetRotation = { 0.f, 0.f, 0.f };

		RigidBodyDesc.pGeometry = &BodyBoxDesc;

		RigidBodyDesc.eThisColFlag = COL_STATIC;
		RigidBodyDesc.eTargetColFlag = COL_AMITOY;
		m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

		m_pTransformCom->SetRigidBody(m_pRigidBody);
	}

	// Head (For. Jump)
	{
		GeometrySphere SphereDesc;
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = true;
		RigidBodyDesc.isGravity = false;
		RigidBodyDesc.isInitCol = true;

		SphereDesc.fRadius = 0.7f;
		SphereDesc.strShapeTag = "Jump_Mushroom_Head";
		SphereDesc.vOffSetPosition = { 0.0f, 1.f,0.0f };
		SphereDesc.vOffSetRotation = { 0.f, 0.f, 0.f };
		RigidBodyDesc.pGeometry = &SphereDesc;

		RigidBodyDesc.eThisColFlag = COL_TRAP;
		RigidBodyDesc.eTargetColFlag = COL_AMITOY;

		m_pRigidBody->CreateShape(&RigidBodyDesc);
	}

	return S_OK;
}

void CFonosSuperJump::PriorityTick(_float _fTimeDelta)
{
}

void CFonosSuperJump::Tick(_float _fTimeDelta)
{
	m_pModelCom->PlayAnimation(_fTimeDelta);
	m_pTransformCom->Tick(_fTimeDelta);
}

void CFonosSuperJump::LateTick(_float _fTimeDelta)
{
	m_pModelCom->CheckDisableAnim();
	m_pTransformCom->LateTick(_fTimeDelta);

	// frustrum
	if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 2.f))
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
	}

	if (m_bJump)
	{
		if (m_pModelCom->GetCanCancelAnimation())
		{
			m_bJump = false;
			m_pModelCom->ChangeAnim(0, 0.1f, true);
		}
	}
}

HRESULT CFonosSuperJump::Render()
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

void CFonosSuperJump::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if ((_ColData.eColFlag & COL_AMITOY) && (_szMyShapeTag == "Jump_Mushroom_Head"))
	{
		m_pModelCom->ChangeAnim(1, 0.05f, false, 0.4f);
		m_pModelCom->GetCurAnimation()->SetTrackPosition(40.f);
		m_pModelCom->PlayAnimation(0);
		m_bJump = true;
	}
}

void CFonosSuperJump::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CFonosSuperJump::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}
