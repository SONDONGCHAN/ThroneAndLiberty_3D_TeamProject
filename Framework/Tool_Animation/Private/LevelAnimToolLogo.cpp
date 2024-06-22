#include "LevelAnimToolLogo.h"

CLevelAnimToolLogo::CLevelAnimToolLogo()
{
}

HRESULT CLevelAnimToolLogo::Initialize()
{
	if (FAILED(ReadyLayerBackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(ReadyCamera(TEXT("Layer_Camera"))))
		return E_FAIL;

	return S_OK;
}

void CLevelAnimToolLogo::Tick(_float _fTimeDelta)
{
}

void CLevelAnimToolLogo::LateTick(_float _fTimeDelta)
{
}

HRESULT CLevelAnimToolLogo::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevelAnimToolLogo::ReadyLayerBackGround(const wstring& strLayerTag)
{
	//shared_ptr<CTestLogo> pLogo = CTestLogo::Create();

	//if(FAILED(GAMEINSTANCE->AddObject(LEVEL_LOGO, strLayerTag, pLogo)))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CLevelAnimToolLogo::ReadyCamera(const wstring& strLayerTag)
{
	return S_OK;
}

shared_ptr<CLevelAnimToolLogo> CLevelAnimToolLogo::Create()
{
	shared_ptr<CLevelAnimToolLogo> pInstance = make_shared<CLevelAnimToolLogo>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CLevelAnimToolLogo");

	return pInstance;
}
