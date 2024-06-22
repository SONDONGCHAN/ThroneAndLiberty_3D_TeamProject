#include "pch.h"
#include "VillageMiniGameMgr.h"
#include "CameraMgr.h"
#include "DialogueMgr.h"
#include "UIMgr.h"
#include "Player.h"
#include "QuestMgr.h"

bool CVillageMiniGameMgr::m_isCleared = false;

CVillageMiniGameMgr::CVillageMiniGameMgr()
{
}

CVillageMiniGameMgr::~CVillageMiniGameMgr()
{
}

shared_ptr<CVillageMiniGameMgr> CVillageMiniGameMgr::Create()
{
	shared_ptr<CVillageMiniGameMgr> pInstance{ nullptr };

	if (false == m_isCleared)
	{
		pInstance = make_shared<CVillageMiniGameMgr>();
		pInstance->Initialize();
	}
	
	return pInstance;
}

HRESULT CVillageMiniGameMgr::Initialize()
{
	if (FAILED(CreateAmitoy()))
		return E_FAIL;

	if (FAILED(CreateObjects()))
		return E_FAIL;

	if (FAILED(CreateQuestItems()))
		return E_FAIL;

	return S_OK;
}

void CVillageMiniGameMgr::PriorityTick(_float _fTimeDelta)
{
#ifdef _DEBUG
	if ((GAMEINSTANCE->KeyDown(DIK_NUMPAD3) && !m_isStarted) || (GAMEINSTANCE->KeyDown(DIK_HOME) && !m_isStarted))
	{
		StartMiniGame();
		m_isStarted = true;
		UIMGR->DeactivateHUD();
	}
	if ((GAMEINSTANCE->KeyDown(DIK_NUMPAD6) && !m_isCleared) || (GAMEINSTANCE->KeyDown(DIK_END) && !m_isCleared))
	{
		ClearMiniGame();
	}
#endif // _DEBUG

	if (!m_isStarted && !m_isCleared)
	{
		if (DIALOGUEMGR->GetCurDialogueKey() == L"Davinci0" && DIALOGUEMGR->GetDialogueState() == DIALOGUE_COMPLETE)
		{
			m_isStarted = true;
			StartMiniGame();
			UIMGR->DeactivateHUD();
			UIMGR->ActivateCountDown();
		}
	}
	
	if (m_isStarted && !m_isCleared)
	{
		if (!m_pAmitoy->IsEnabled())
		{
			ClearMiniGame();
			UIMGR->UpdateNumFound(-1, 3);
			UIMGR->DeactivateCountDown();
		}
	}

	if (m_pAmitoy->IsActive())
	{
		m_pAmitoy->PriorityTick(_fTimeDelta);
	}

	for (auto& pMNGObejct : m_vecObjects)
	{
		if (pMNGObejct->IsActive())
		{
			pMNGObejct->PriorityTick(_fTimeDelta);
		}
	}

	if (m_isStarted)
	{
		m_isAllAcquired = true;
	}
	int iGetCnt{ 0 };
	for (auto& pMNGQuestItem : m_vecQuestItems)
	{
		if (pMNGQuestItem->IsActive())
		{
			pMNGQuestItem->PriorityTick(_fTimeDelta);
			m_isAllAcquired = false;
		}
		else
			iGetCnt++;
	}
	if (m_isStarted)
		UIMGR->UpdateNumFound(iGetCnt, 3);

	if (m_isAllAcquired)
	{
		m_pAmitoy->AllAcquired();
	}
}

void CVillageMiniGameMgr::Tick(_float _fTimeDelta)
{
	if (m_pAmitoy->IsActive())
	{
		m_pAmitoy->Tick(_fTimeDelta);
	}

	for (auto& pMNGObejct : m_vecObjects)
	{
		if (pMNGObejct->IsActive())
		{
			pMNGObejct->Tick(_fTimeDelta);
		}
	}

	for (auto& pMNGQuestItem : m_vecQuestItems)
	{
		if (pMNGQuestItem->IsActive())
		{
			pMNGQuestItem->Tick(_fTimeDelta);
		}
	}
}

void CVillageMiniGameMgr::LateTick(_float _fTimeDelta)
{
	if (m_pAmitoy->IsActive())
	{
		m_pAmitoy->LateTick(_fTimeDelta);
	}

	for (auto& pMNGObejct : m_vecObjects)
	{
		if (pMNGObejct->IsActive())
		{
			pMNGObejct->LateTick(_fTimeDelta);
		}
	}

	for (auto& pMNGQuestItem : m_vecQuestItems)
	{
		if (pMNGQuestItem->IsActive())
		{
			pMNGQuestItem->LateTick(_fTimeDelta);
		}
	}
}

void CVillageMiniGameMgr::StartMiniGame()
{
	static_pointer_cast<CPlayer>(GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player")))->SetAllInputState(true);
	SpawnAmitoy();
	SpawnObjects();
	SpawnQuestItems();
}

void CVillageMiniGameMgr::ClearMiniGame()
{
	static_pointer_cast<CPlayer>(GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player")))->SetAllInputState(false);
	DespanwAmitoy();
	DespawnObjects();
	UIMGR->ActivateHUD();
	m_isCleared = true;
	m_isStarted = false;
}

HRESULT CVillageMiniGameMgr::CreateAmitoy()
{
	m_pAmitoy = CAmitoy::Create();
	if (!m_pAmitoy)
		return E_FAIL;

	return S_OK;
}

HRESULT CVillageMiniGameMgr::CreateObjects()
{
	m_vecObjects.reserve(118);

	GeometryBox BoxDesc{};
	BoxDesc.strShapeTag = "Box";
	BoxDesc.vSize = { 1.0f,1.0f,1.0f };
	BoxDesc.vOffSetPosition = { 0.0f,0.5f,0.0f };


	_float4x4 WorldMatrix = XMMatrixRotationY(XM_PIDIV4);

	WorldMatrix.Translation({ 38.259f,-10.641f,323.642f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_Boom", WorldMatrix, &BoxDesc, 2.0f, true));

	WorldMatrix.Translation({ 36.672f,-10.641f,325.272f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 37.474f,-10.641f,324.433f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 39.074f,-10.641f,322.842f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 39.063f,-10.641f,324.386f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 39.847f,-10.641f,323.604f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 38.269f,-10.641f,325.208f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 39.548f,-10.641f,321.837f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 39.625f,-9.441f,322.536f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 39.157f,-9.441f,323.761f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 38.206f,-9.441f,324.567f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 36.880f,-9.441f,325.171f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 38.115f,-10.641f,322.071f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 38.987f,-10.641f,320.582f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 37.215f,-10.641f,322.804f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 36.287f,-10.641f,323.651f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 38.052f,-9.441f,322.696f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_Boom", WorldMatrix, &BoxDesc, 2.0f, true));

	WorldMatrix.Translation({ 37.114f,-9.441f,323.874f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	// 18 °³

	WorldMatrix = XMMatrixTranslation(53.429f, -10.641f, 371.293f);
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 53.228f,-10.641f,372.512f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 53.115f,-10.641f,373.716f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	WorldMatrix.Translation({ 53.008f,-10.641f,374.989f });
	m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));

	// 4 °³

	WorldMatrix = XMMatrixTranslation(48.534f, -10.641f, 399.546f);
	for (int y = 0; y < 4; y++)
	{
		for (int z = 0; z < 8; z++)
		{
			for (int x = 0; x < 3; x++)
			{
				WorldMatrix.Translation({ 50.634f - (float)x * 1.05f, -10.641f + (float)y * 1.1f, 399.546f + (float)z * 1.05f });
				if (y % 2 == 0 && abs(x - z) % 3 == 0)
				{
					m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_Boom", WorldMatrix, &BoxDesc, 2.0f, true));
				}
				else
				{
					m_vecObjects.emplace_back(CMNGObject::Create("BG_Box_02_01_SM", WorldMatrix, &BoxDesc, 2.0f));
				}
			}
		}
	}
	// 96 °³
	

	return S_OK;
}

HRESULT CVillageMiniGameMgr::CreateQuestItems()
{
	_float4x4 WorldMatrix = XMMatrixTranslation(37.013f, -10.745f, 322.0f);

	GeometrySphere SphereDesc{};
	SphereDesc.strShapeTag = "QuestItem";
	SphereDesc.fRadius = 1.0f;

	shared_ptr<CMNGQuestItem> pObject2 = CMNGQuestItem::Create("DragonBook", WorldMatrix, &SphereDesc, 1.0f);
	m_vecQuestItems.emplace_back(pObject2);
	pObject2->SetAmitoyPointer(m_pAmitoy);

	WorldMatrix = XMMatrixRotationZ(XMConvertToRadians(55.0f)) * XMMatrixRotationY(XMConvertToRadians(50.0f)) * XMMatrixTranslation(44.387f, -4.255f, 370.010f);

	pObject2 = CMNGQuestItem::Create("JusinScroll", WorldMatrix, &SphereDesc, 1.0f);
	m_vecQuestItems.emplace_back(pObject2);
	pObject2->SetAmitoyPointer(m_pAmitoy);

	WorldMatrix = XMMatrixRotationY(XMConvertToRadians(-40.0f)) * XMMatrixTranslation(42.26f, -7.5f, 402.53f);

	pObject2 = CMNGQuestItem::Create("BurgerPortrait", WorldMatrix, &SphereDesc, 1.5f);
	m_vecQuestItems.emplace_back(pObject2);
	pObject2->SetAmitoyPointer(m_pAmitoy);

	
	return S_OK;
}

HRESULT CVillageMiniGameMgr::SpawnAmitoy()
{
	m_pAmitoy->SpawnAmitoy();

	return S_OK;
}

HRESULT CVillageMiniGameMgr::SpawnObjects()
{
	for (auto& pObjects : m_vecObjects)
	{
		pObjects->SpawnObject();
	}

	return S_OK;
}

HRESULT CVillageMiniGameMgr::SpawnQuestItems()
{
	for (auto& pQuestItem : m_vecQuestItems)
	{
		pQuestItem->SpawnQuestItem();
	}

	return S_OK;
}

HRESULT CVillageMiniGameMgr::DespanwAmitoy()
{
	m_pAmitoy->DespawnAmitoy();
	return S_OK;
}

HRESULT CVillageMiniGameMgr::DespawnObjects()
{
	for (auto& pObjects : m_vecObjects)
	{
		if (pObjects->IsActive())
			pObjects->DespawnObject();
	}
	return S_OK;
}
