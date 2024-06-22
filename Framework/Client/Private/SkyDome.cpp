#include "SkyDome.h"
#include "GameInstance.h"
#include "Model.h"

CSkyDome::CSkyDome()
{
}

HRESULT CSkyDome::Initialize()
{
    __super::Initialize(nullptr);

    m_eShaderType = ST_NONANIM;
    m_iShaderPass = 8;
    
    m_pSkyModel = GAMEINSTANCE->GetModel("SkyDome");

    if (!m_pSkyModel)
        return E_FAIL;


    m_pTransformCom->SetScaling(0.25f, 0.25f, 0.25f);
    m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(150.f, 0.f, 150.f));

    return S_OK;
}

void CSkyDome::PriorityTick(_float _fTimeDelta)
{
}

void CSkyDome::Tick(_float _fTimeDelta)
{
}

void CSkyDome::LateTick(_float _fTimeDelta)
{
    GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_PRIORITY, shared_from_this());
}

HRESULT CSkyDome::Render()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    //T_Sky_Clouds_M

    _uint iNumMeshes = m_pSkyModel->GetNumMeshes();


    if(FAILED(GAMEINSTANCE->BindSRV("g_CloudMaskTexture", "hemispherical-sky-domes-5000ft3")))
        return E_FAIL;

    for (_uint i = 0; i < iNumMeshes; i++)
        GAMEINSTANCE->BeginNonAnimModel(m_pSkyModel, i);

    return S_OK;

}

shared_ptr<CSkyDome> CSkyDome::Create()
{
    shared_ptr<CSkyDome> pSkyDome = make_shared<CSkyDome>();

    if (FAILED(pSkyDome->Initialize()))
        MSG_BOX("Failed to Create : SkyDome");

    return pSkyDome;
}
