#include "TestTerrain.h"
#include "VITerrain.h"
#include "Texture.h"
#include "RigidBody.h"

CTestTerrain::CTestTerrain()
{
}

CTestTerrain::~CTestTerrain()
{
	int a = 1;
}

HRESULT CTestTerrain::Initialize(string _strTexKey, string _strFilePath)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	m_eShaderType = ESHADER_TYPE::ST_NORTEX;
	m_iShaderPass = 0;

	m_pTextures[TYPE_DIFFUSE] = _strTexKey;

	//m_pVIBufferCom = CVITerrain::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), _strFilePath);
	m_Components.emplace(TEXT("Com_VIBuffer"), m_pVIBufferCom);

	m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestitution = 0.0f;
	GeometryHeightField HeighFieldDesc{};
	HeighFieldDesc.vOffSetPosition = { 0.0f,0.0f,0.0f };
	HeighFieldDesc.vOffSetRotation = { 0.0f,0.0f,0.0f };
	HeighFieldDesc.strShapeTag = "HeightField";
	HeighFieldDesc.pVITerrain = m_pVIBufferCom;
	RigidBodyDesc.pGeometry = &HeighFieldDesc;
	RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ALL;
	RigidBodyDesc.eThisColFlag = COL_STATIC;
	RigidBodyDesc.eTargetColFlag = COL_PLAYER | COL_MONSTER | COL_NPC | COL_MONSTERPROJECTILE | COL_STATIC;
	m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

	m_pTransformCom->SetRigidBody(m_pRigidBody);

	return S_OK;
}

void CTestTerrain::PriorityTick(_float _fTimeDelta)
{
}

void CTestTerrain::Tick(_float _fTimeDelta)
{
}

void CTestTerrain::LateTick(_float _fTimeDelta)
{
	if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
}

HRESULT CTestTerrain::Render()
{
	// ¸ðµ¨
	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_pTextures[TYPE_DIFFUSE]);
	
	GAMEINSTANCE->BeginShaderBuffer(m_pVIBufferCom);

	return S_OK;
}

shared_ptr<CTestTerrain> CTestTerrain::Create(string _strTexKey, string _strFilePath)
{
	shared_ptr<CTestTerrain> pInstance = make_shared<CTestTerrain>();

	if (FAILED(pInstance->Initialize(_strTexKey, _strFilePath)))
	{
		MSG_BOX("Failed to Created : CTestTerrain");
		pInstance.reset();
	}

	return pInstance;
}
