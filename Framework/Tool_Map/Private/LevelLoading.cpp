#include "LevelLoading.h"

#include "Loader.h"
#include "LevelAtelier.h"

CLevelLoading::CLevelLoading()
{
}

#ifdef CHECK_REFERENCE_COUNT
CLevelLoading::~CLevelLoading()
{
	m_pLoader.reset();
}
#endif // CHECK_REFERENCE_COUNT

HRESULT CLevelLoading::Initialize(LEVEL _eNextLevel)
{
	m_eNextLevel = _eNextLevel;

	m_pLoader = CLoader::Create(_eNextLevel);
	if (m_pLoader == nullptr)
		return E_FAIL;

	return S_OK;
}

void CLevelLoading::Tick(_float _fTimeDelta)
{
	if (m_pLoader->isFinished() == true)
	{
		if (GetKeyState(VK_SPACE) & 0x8000)
		{
			shared_ptr<CLevel> pLevel = { nullptr };

			switch (m_eNextLevel)
			{
			case LEVEL_ATELIER:
				pLevel = LevelAtelier::Create();
				break;
			}

			if (pLevel == nullptr)
				return;

			if (FAILED(CGameInstance::GetInstance()->OpenLevel(m_eNextLevel, pLevel)))
				return;
		}
	}
}

HRESULT CLevelLoading::Render()
{
	if (m_pLoader == nullptr)
		return E_FAIL;

#ifdef _DEBUG
	m_pLoader->Output_LoadingText();
#endif

	return S_OK;
}

HRESULT CLevelLoading::Ready_Layer_BackGround()
{
	return S_OK;
}

HRESULT CLevelLoading::Ready_Layer_UI()
{
	return S_OK;
}

shared_ptr<CLevelLoading> CLevelLoading::Create(LEVEL _eNextLevel)
{
	shared_ptr<CLevelLoading> pInstance = WRAPPING(CLevelLoading)();

	if (FAILED(pInstance->Initialize(_eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLevelLoading");
		pInstance.reset();
	}

	return pInstance;
}
