#include "WallStone.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"

CWallStone::CWallStone()
{
}

HRESULT CWallStone::Initialize(_float3 _vStartPos, _float3 _vScale, _uint _StoneIndex)
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 1.f;
	pArg->fRotationPerSec = XMConvertToRadians(10.0f);
	__super::Initialize(pArg.get());

	m_iMyStoneIndex = _StoneIndex;

	string MyModelName = { "" };
	_float3 vMyColSize = { 1.f ,1.f, 1.f };
	_float3 vMyOffsetPos = { 0.f, 0.f, 0.f };

	if (_StoneIndex == 0)
	{
		MyModelName = "IT_N_Stone_Manticus_Decman_SK";
		vMyColSize = _float3{3.6f, 1.f, 2.1f };
	}
	else if (_StoneIndex == 1)
	{
		MyModelName = "M_Golem_Talus_Stone01";
		vMyColSize = _float3{ 1.5f, 1.65f, 1.65f };
	}
	else if (_StoneIndex == 2)
	{
		MyModelName = "M_StoneGolem_Smallrock_SK";
		vMyColSize = _float3{ 1.1f, 1.4f, 1.f };
		vMyOffsetPos = _float3{ 0.05f, 0.7f, 0.f };
	}

	vMyColSize *= _vScale;
	vMyOffsetPos *= _vScale;

	m_pModel = GAMEINSTANCE->GetModel(MyModelName);

	m_eShaderType = ST_NONANIM;
	m_iShaderPass = 0;

	_float3 MyStartPos	= _vStartPos;
	_float3 MyScale		= _vScale;

	m_pTransformCom->SetScaling(MyScale);
	m_pTransformCom->SetState(CTransform::STATE_POSITION, MyStartPos);
	m_pTransformCom->Tick(0.f);

	AddRigidBody(MyModelName, vMyColSize, vMyOffsetPos);

	m_pRigidBody->SetMass(0.1f);

	return S_OK;
}

void CWallStone::PriorityTick(_float _fTimeDelta)
{
}

void CWallStone::Tick(_float _fTimeDelta)
{
}

void CWallStone::LateTick(_float _fTimeDelta)
{
	m_pTransformCom->LateTickFragment(_fTimeDelta);

	if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 5.f))
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
		GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());
	}

#ifdef _DEBUG
	if (m_pRigidBody)
		GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
}

HRESULT CWallStone::Render()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	_uint iNumMeshes = m_pModel->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		GAMEINSTANCE->BeginNonAnimModel(m_pModel, i);
	}

	return S_OK;
}

HRESULT CWallStone::RenderShadow()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 1);
	GAMEINSTANCE->BindLightProjMatrix();
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	_uint iNumMeshes = m_pModel->GetNumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
		GAMEINSTANCE->BeginNonAnimModel(m_pModel, (_uint)i);

	return S_OK;
}

void CWallStone::DisableStone()
{
	m_pRigidBody->DisableCollision("WallStone_Body");
	m_pRigidBody->SimulationOff();
	shared_from_this()->SetEnable(false);
}

void CWallStone::StopStone()
{
	m_pRigidBody->SetLinearVelocity({ 0.f,0.f,0.f });
	m_pRigidBody->SetAngularVelocity({ 0.f,0.f,0.f });
}

void CWallStone::AddRigidBody(string _strModelKey, _float3 _vColScale, _float3 _OffSetPos)
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	
	//==º»Ã¼==
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = true;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::NONE;
	
	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 1.0f;
	RigidBodyDesc.fRestitution = 0.f;
	
	GeometryBox BoxDesc;
	BoxDesc.vSize = _vColScale ;
	BoxDesc.vOffSetPosition = _OffSetPos;
	BoxDesc.vOffSetRotation = { 0.f, 0.f, 0.f };
	BoxDesc.strShapeTag = "WallStone_Body";
	
	RigidBodyDesc.pGeometry = &BoxDesc;
	
	RigidBodyDesc.eThisColFlag = COL_INTERACTIONOBJ;
	RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER | COL_INTERACTIONOBJ;
	m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
	
	m_pTransformCom->SetRigidBody(m_pRigidBody);
}

shared_ptr<CWallStone> CWallStone::Create(_float3 _vStartPos, _float3 _vScale, _uint _StoneIndex)
{
	shared_ptr<CWallStone> pTargetDirection = make_shared<CWallStone>();

	if (FAILED(pTargetDirection->Initialize(_vStartPos, _vScale, _StoneIndex)))
		MSG_BOX("Failed to Create : CWallStone");

	return pTargetDirection;
}
