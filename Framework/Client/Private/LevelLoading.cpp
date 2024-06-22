#include "LevelLoading.h"
#include "Loader.h"
#include "UIMgr.h"

#include "UIMgr.h"
#include "UIBackGround.h"
#include "VIInstancePoint.h"

CLevelLoading::CLevelLoading()
    :CLevel()
{
}

CLevelLoading::~CLevelLoading()
{
    Free();
}

HRESULT CLevelLoading::Initialize(LEVEL _eNextLevel)
{


  //CUIBase::UIDesc uiDesc{};
  // uiDesc.strTag = "UI_Logo";
  // uiDesc.eUILayer = UI_POPUP_FULL;
  // uiDesc.eUIType = UI_BACKGROUND;
  // uiDesc.eUISlotType = SLOT_END;
  // uiDesc.eUITexType = UI_TEX_D;
  // uiDesc.fZOrder = 0.f;
  // uiDesc.iShaderPassIdx = 0;
  // uiDesc.isMasked = false;
  // uiDesc.isNoised = false;
  // uiDesc.iShaderPassIdx = 0;

  // CVIInstancing::InstanceDesc instanceDesc{};
  // instanceDesc.vSize = _float2(1280.f, 1280.f);
  // instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
  // instanceDesc.fSizeY = 720.f;

  // vector<string> texs;
  // texs.emplace_back("UI_Logo");
  // shared_ptr<CUIBackGround> pLogo = CUIBackGround::Create(texs, "", "", uiDesc, instanceDesc);
  // if (pLogo == nullptr)
  //     return E_FAIL;

  // UIMGR->AddUI("UI_Logo", pLogo);
   //if (FAILED(UIMGR->AddUI("UI_Logo", pLogo)))
   //    return E_FAIL;

   m_pLoader = CLoader::Create(_eNextLevel);

   if (!m_pLoader)
       return E_FAIL;

    return S_OK;
}

void CLevelLoading::Tick(_float _fTimeDelta)
{
    if (m_pLoader->isFinished())
    {
        UIMGR->DeactivateLoading();
        m_pLoader->StartLevel();
    }
}

void CLevelLoading::LateTick(_float _fTimeDelta)
{

}

HRESULT CLevelLoading::Render()
{
    return S_OK;
}

HRESULT CLevelLoading::ReadyNextLevel(_uint _iNextLevel)
{
    /*레벨 시작*/
  //  UIMGR->ActivateUI("UI_Logo");
    UIMGR->ActivateLoading(_iNextLevel);
    m_pLoader->StartLoading(Client::LEVEL(_iNextLevel));

    return S_OK;
}

void CLevelLoading::Free()
{
    if(m_pLoader)
        delete m_pLoader;
}

shared_ptr<CLevelLoading> CLevelLoading::Create(LEVEL _eNextLevel)
{
    shared_ptr<CLevelLoading> pInstance = make_shared<CLevelLoading>();

    if (FAILED(pInstance->Initialize(_eNextLevel)))
        MSG_BOX("Filed to Create: CLevelLoading");

    return pInstance;
}
