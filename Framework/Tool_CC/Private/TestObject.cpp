#include "pch.h"
#include "TestObject.h"

CTestObject::CTestObject()
{
}

CTestObject::~CTestObject()
{
}

shared_ptr<CTestObject> CTestObject::Create()
{
	shared_ptr<CTestObject> pInstance = make_shared<CTestObject>();

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CTestObject::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CTestObject::Initialize()
{
	CGameObject::Initialize(nullptr);

	m_pTransformCom->SetScaling(5.0f, 5.0f, 5.0f);
	m_pTransformCom->SetPositionParam(2, 5.0f);

	m_eShaderType = ESHADER_TYPE::ST_POSTEX;
	m_iShaderPass = 1;

	return S_OK;
}

void CTestObject::PriorityTick(float _fTimeDelta)
{
}

void CTestObject::Tick(float _fTimeDelta)
{
}

void CTestObject::LateTick(float _fTimeDelta)
{
	if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION)))
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONLIGHT, shared_from_this());
	}
}

HRESULT CTestObject::Render()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
	pGameInstance->ReadyShader(m_eShaderType, m_iShaderPass);
	pGameInstance->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
	pGameInstance->BindSRV("g_Texture", "PlaceHolder");
	pGameInstance->BeginShader();

	return S_OK;
}
