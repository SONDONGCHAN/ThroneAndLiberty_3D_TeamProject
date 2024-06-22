#include "TestLogo.h"
#include "GameInstance.h"
#include "Texture.h"

CTestLogo::CTestLogo()
{
}

HRESULT CTestLogo::Initialize()
{
    __super::Initialize(nullptr);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_POSTEX;
    m_iShaderPass = 0;

    m_strKeyName = "UI_Logo";
    m_pTransformCom->SetScaling(g_iWinSizeX, g_iWinSizeY, 1.f);

    return S_OK;
}

void CTestLogo::PriorityTick(_float _fTimeDelta)
{
}

void CTestLogo::Tick(_float _fTimeDelta)
{
}

void CTestLogo::LateTick(_float _fTimeDelta)
{
   CGameInstance::GetInstance()->AddRenderGroup(CRenderer::RENDER_PRIORITY, shared_from_this());
}

HRESULT CTestLogo::Render()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
    GAMEINSTANCE->BindWVPMatrixOrthoGraphic(m_pTransformCom->GetWorldMatrix());
    GAMEINSTANCE->BindSRV("g_Texture", m_strKeyName);

    GAMEINSTANCE->BeginShader();

    return S_OK;
}

shared_ptr<CTestLogo> CTestLogo::Create()
{
    shared_ptr<CTestLogo> pInstacne = make_shared<CTestLogo>();

    if (FAILED(pInstacne->Initialize()))
        MSG_BOX("Failed to Create : CTestLogo");
       
    return pInstacne;
}
