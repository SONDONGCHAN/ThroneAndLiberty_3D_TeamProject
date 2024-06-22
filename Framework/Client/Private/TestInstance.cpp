#include "TestInstance.h"
#include "GameInstance.h"
#include "InstancingModel.h"

CTestInstance::CTestInstance()
{
}

CTestInstance::~CTestInstance()
{
}

HRESULT CTestInstance::Initialize()
{
    __super::Initialize(nullptr);




    vector<_float4x4> PosMatrices;

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_MESHINSTANCE;
    m_iShaderPass = 0;

    for (_int i = 0; i < 10; ++i) {
        _float4x4 BaseMatrix = XMMatrixIdentity();
        _float4 vPos = _float4(i * 1.f, 5.f, i * 1.f, 1.f);
        memcpy(BaseMatrix.m[3], &vPos, sizeof(_float4));

        PosMatrices.push_back(BaseMatrix);

    }
    
    shared_ptr<CModel> pOrigin = GAMEINSTANCE->GetModel("Staff_1");
    m_pModel = CInstancingModel::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), pOrigin, &PosMatrices);

    return S_OK;
}

void CTestInstance::PriorityTick(_float _fTimeDelta)
{
}

void CTestInstance::Tick(_float _fTimeDelta)
{
}

void CTestInstance::LateTick(_float _fTimeDelta)
{
    GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

}

HRESULT CTestInstance::Render()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());





	_uint iNumMeshes = m_pModel->GetNumMeshes();


	for (size_t i = 0; i < iNumMeshes; i++)
	{
        GAMEINSTANCE->BeginNonAnimInstance(m_pModel, (_uint)i);

	}





	return S_OK;
}

shared_ptr<CTestInstance> CTestInstance::Create()
{

    shared_ptr<CTestInstance> pInstance = make_shared<CTestInstance>();

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CTestInstance");

    return pInstance;
}
