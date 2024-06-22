#include "pch.h"
#include "ToolUIMgr.h"
#include "ToolUIIcon.h"
#include "ToolUIScroll.h"
#include "ToolUIBG.h"
#include "ToolUISlot.h"
#include "ToolUIItemBackGround.h"
#include "ToolUIPanel.h"

#include "GameInstance.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(Tool_UI::CToolUIMgr)

Tool_UI::CToolUIMgr::CToolUIMgr()
{

}

Tool_UI::CToolUIMgr::~CToolUIMgr()
{
	m_pHUD.reset();

	for (auto& ui : m_UIs)
	{
		if (ui.second != nullptr)
			ui.second.reset();
	}

	m_UIs.clear();
}

HRESULT Tool_UI::CToolUIMgr::Initialize()
{
	if (FAILED(PrepareBindFuncs()))
		return E_FAIL;

	return S_OK;
}

void Tool_UI::CToolUIMgr::Tick(_float _fDeltaTime)
{
	//KeyInput();

	if (m_pHUD != nullptr)
		m_pHUD->Tick(_fDeltaTime);

	for (auto& pUI : m_UIVec)
	{
		if (!pUI.expired())
			pUI.lock()->Tick(_fDeltaTime);
	}

	/* 활성화 된 UI들 중 겹칠 경우 ZOrder를 처리하는 함수 필요 */
	/* 필요...한가? */
	SortUIZOrder();

	if (m_pHUD != nullptr)
		m_pHUD->LateTick(_fDeltaTime);

	for (auto& pUI : m_UIVec)
	{
		if (!pUI.expired())
			pUI.lock()->LateTick(_fDeltaTime);
	}
}

void Tool_UI::CToolUIMgr::SetHUD(const string& _strUITag)
{
	shared_ptr<CToolUIBase> pUIHUD = FindUI(_strUITag);

	if (pUIHUD == nullptr)
		return;

	m_pHUD = pUIHUD;
}

void Tool_UI::CToolUIMgr::SetUISize(_float _fSizeX, _float _fSizeY)
{
	if (m_pLatestActivatedUI.lock() != nullptr && m_pLatestActivatedUI.lock() != m_pHUD)
	{
		m_pLatestActivatedUI.lock()->SetBufferSize(_fSizeX, _fSizeY);
	}
}

HRESULT Tool_UI::CToolUIMgr::AddUI(const string& _strUITag, shared_ptr<CToolUIBase> _pUI)
{
	shared_ptr<CToolUIBase> foundUI = FindUI(_strUITag);
	if (foundUI != nullptr)
		return E_FAIL;

	m_UIs.emplace(_strUITag, _pUI);

	return S_OK;
}

HRESULT Tool_UI::CToolUIMgr::AddUIAfterSwapTag(const string& _strUITag, shared_ptr<CToolUIBase> _pUI)
{
	if (_pUI == nullptr)
		return E_FAIL;

	m_UIs.emplace(_strUITag, _pUI);

	return S_OK;
}

shared_ptr<CToolUIBase> Tool_UI::CToolUIMgr::FindUI(const string& _strUITag)
{
	auto it = m_UIs.find(_strUITag);
	if (it == m_UIs.end())
		return nullptr;

	return it->second;
}

HRESULT Tool_UI::CToolUIMgr::AddUIChild(const string& _strUITag, const string& _strChildTag)
{
	shared_ptr<CToolUIBase> foundUI = FindUI(_strUITag);
	if (foundUI == nullptr)
		return E_FAIL;

	shared_ptr<CToolUIBase> foundChildUI = FindUI(_strChildTag);
	if (foundChildUI == nullptr)
		return E_FAIL;
			
	if (foundUI->GetUIType() == TOOL_UI_SCROLLBG && foundChildUI->GetUIType() == TOOL_UI_SCROLL)
	{
		shared_ptr<CToolUIScroll> foundScroll = dynamic_pointer_cast<CToolUIScroll>(foundChildUI);
		shared_ptr<CToolUIBG> pUIBG = dynamic_pointer_cast<CToolUIBG>(foundUI);
		pUIBG->SetScrollUI(foundScroll);
		foundScroll->SetUIBG(pUIBG);
		foundScroll->SetVisibleCenterAndSizeY(foundUI->GetUIPos(), foundUI->GetUISize().y);
	}

	if (foundUI->GetUIType() == TOOL_UI_PANEL && foundChildUI->GetUIType() == TOOL_UI_ITEMBG)
	{
		shared_ptr<CToolUIPanel> foundParentPanel = dynamic_pointer_cast<CToolUIPanel>(foundUI);
		shared_ptr<CToolUIItemBackGround> foundChildItemBG = dynamic_pointer_cast<CToolUIItemBackGround>(foundChildUI);
		SortChildUIs(foundParentPanel, foundChildItemBG);
	}

	if (foundChildUI->GetUIType() == TOOL_UI_SCROLLBG)
	{
		shared_ptr<CToolUIBG> pUIBG = dynamic_pointer_cast<CToolUIBG>(foundChildUI);
		pUIBG->SetVisibleRange();
	}

	foundUI->AddUIChild(foundChildUI->GetUITag(), foundChildUI);
	foundChildUI->AddUIParent(foundUI);
	
	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&foundChildUI](weak_ptr<CToolUIBase> pUI) {
		if (!pUI.expired() && (pUI.lock()->GetUITag() == foundChildUI->GetUITag()))
			return true;

		return false;
		});

	if (it != m_UIVec.end())
	{
		it->reset();
		m_UIVec.erase(it);
	}

	return S_OK;
}

HRESULT Tool_UI::CToolUIMgr::AddUIParent(const string& _strUITag, const string& _strParentTag)
{
	shared_ptr<CToolUIBase> foundUI = FindUI(_strUITag);
	if (foundUI == nullptr)
		return E_FAIL;

	shared_ptr<CToolUIBase> foundParentUI = FindUI(_strParentTag);
	if (foundParentUI == nullptr)
		return E_FAIL;

	foundUI->AddUIParent(foundParentUI); // 부모UI는 하나밖에 존재할 수 없다

	return S_OK;
}

HRESULT Tool_UI::CToolUIMgr::AddUIChild(const string& _strChildUITag)
{
	shared_ptr<CToolUIBase> foundChildUI = FindUI(_strChildUITag);
	if (foundChildUI == nullptr)
		return E_FAIL;
	
	if (m_pLatestActivatedUI.lock()->GetUIType() == TOOL_UI_SCROLLBG && foundChildUI->GetUIType() == TOOL_UI_SCROLL)
	{
		shared_ptr<CToolUIScroll> foundScroll = dynamic_pointer_cast<CToolUIScroll>(foundChildUI);
		shared_ptr<CToolUIBG> pUIBG = dynamic_pointer_cast<CToolUIBG>(m_pLatestActivatedUI.lock());
		pUIBG->SetScrollUI(foundScroll);
		foundScroll->SetUIBG(pUIBG);
		foundScroll->SetVisibleCenterAndSizeY(m_pLatestActivatedUI.lock()->GetUIPos(), m_pLatestActivatedUI.lock()->GetUISize().y);
	}

	if (m_pLatestActivatedUI.lock()->GetUIType() == TOOL_UI_PANEL && foundChildUI->GetUIType() == TOOL_UI_ITEMBG)
	{
		shared_ptr<CToolUIPanel> foundParentPanel = dynamic_pointer_cast<CToolUIPanel>(m_pLatestActivatedUI.lock());
		shared_ptr<CToolUIItemBackGround> foundChildItemBG = dynamic_pointer_cast<CToolUIItemBackGround>(foundChildUI);
		SortChildUIs(foundParentPanel, foundChildItemBG);
	}

	if (foundChildUI->GetUIType() == TOOL_UI_SCROLLBG)
	{
		shared_ptr<CToolUIBG> pUIBG = dynamic_pointer_cast<CToolUIBG>(foundChildUI);
		pUIBG->SetVisibleRange();
	}

	m_pLatestActivatedUI.lock()->AddUIChild(foundChildUI->GetUITag(), foundChildUI);
	foundChildUI->AddUIParent(m_pLatestActivatedUI.lock());

	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&foundChildUI](weak_ptr<CToolUIBase> pUI) {
		if (!pUI.expired() && (pUI.lock()->GetUITag() == foundChildUI->GetUITag()))
			return true;

		return false;
		});

	if (it != m_UIVec.end())
	{
		it->reset();
		m_UIVec.erase(it);
	}

	return S_OK;
}

HRESULT Tool_UI::CToolUIMgr::AddUIParent(const string& _strParentTag)
{
	shared_ptr<CToolUIBase> foundParentUI = FindUI(_strParentTag);
	if (foundParentUI == nullptr)
		return E_FAIL;

	m_pLatestActivatedUI.lock()->AddUIParent(foundParentUI);
	foundParentUI->AddUIChild(m_pLatestActivatedUI.lock()->GetUITag(), m_pLatestActivatedUI.lock());
	weak_ptr<CToolUIBase> tempPtr = m_pLatestActivatedUI;
	//m_pLatestActivatedUI = foundParentUI;
	ActivateUI(foundParentUI->GetUITag());

	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&tempPtr](weak_ptr<CToolUIBase> pUI) {
		if (!pUI.expired() && (pUI.lock()->GetUITag() == tempPtr.lock()->GetUITag()))
			return true;

		return false;
		});

	if (it != m_UIVec.end())
	{
		it->reset();
		m_UIVec.erase(it);
	}

	tempPtr.reset();

	return S_OK;
}

HRESULT Tool_UI::CToolUIMgr::AddUIInstanceChild(const string& _strUITag, const string& _strChildTag)
{
	shared_ptr<CToolUIBase> foundUI = FindUI(_strUITag);
	if (foundUI == nullptr)
		return E_FAIL;

	shared_ptr<CToolUIBase> foundChildUI = FindUI(_strChildTag);
	if (foundChildUI == nullptr)
		return E_FAIL;

	if (foundUI->GetUIType() == TOOL_UI_SCROLLBG && foundChildUI->GetUIType() == TOOL_UI_SCROLL)
	{
		shared_ptr<CToolUIScroll> foundScroll = dynamic_pointer_cast<CToolUIScroll>(foundChildUI);
		shared_ptr<CToolUIBG> pUIBG = dynamic_pointer_cast<CToolUIBG>(foundUI);
		pUIBG->SetScrollUI(foundScroll);
		foundScroll->SetUIBG(pUIBG);
		foundScroll->SetVisibleCenterAndSizeY(foundUI->GetUIPos(), foundUI->GetUISize().y);
	}

	if (foundUI->GetUIType() == TOOL_UI_PANEL && foundChildUI->GetUIType() == TOOL_UI_ITEMBG)
	{
		shared_ptr<CToolUIPanel> foundParentPanel = dynamic_pointer_cast<CToolUIPanel>(foundUI);
		shared_ptr<CToolUIItemBackGround> foundChildItemBG = dynamic_pointer_cast<CToolUIItemBackGround>(foundChildUI);
		SortChildUIs(foundParentPanel, foundChildItemBG);
	}

	if (foundChildUI->GetUIType() == TOOL_UI_SCROLLBG)
	{
		shared_ptr<CToolUIBG> pUIBG = dynamic_pointer_cast<CToolUIBG>(foundChildUI);
		pUIBG->SetVisibleRange();
	}

	//foundUI->AddUIChild(foundChildUI->GetUITag(), foundChildUI);
	foundChildUI->AddUIParent(foundUI);

	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&foundChildUI](weak_ptr<CToolUIBase> pUI) {
		if (!pUI.expired() && (pUI.lock()->GetUITag() == foundChildUI->GetUITag()))
			return true;

		return false;
		});

	if (it != m_UIVec.end())
	{
		it->reset();
		m_UIVec.erase(it);
	}

	return S_OK;
}

HRESULT Tool_UI::CToolUIMgr::AddUIInstanceChild(const string& _strChildTag)
{
	return E_NOTIMPL;
}

HRESULT Tool_UI::CToolUIMgr::RemoveUIChild(const string& _strUITag, const string& _strChildUITag)
{
	shared_ptr<CToolUIBase> pFoundUI = FindUI(_strUITag);
	if (pFoundUI == nullptr)
		return E_FAIL;

	shared_ptr<CToolUIBase> pFoundChildUI = FindUI(_strChildUITag);
	if (pFoundChildUI == nullptr)
		return E_FAIL;

	pFoundUI->RemoveUIChild(_strChildUITag);
	pFoundChildUI->RemoveUIParent();

	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&pFoundChildUI](weak_ptr<CToolUIBase> pUI) {
		if (!pUI.expired() && (pUI.lock()->GetUITag() == pFoundChildUI->GetUITag()))
			return true;

		return false;
		});

	if (it == m_UIVec.end())
		ActivateUI(pFoundChildUI->GetUITag());

	return S_OK;
}

void Tool_UI::CToolUIMgr::ActivateUI(const string& _strUITag)
{
	shared_ptr<CToolUIBase> foundUI = FindUI(_strUITag);
	if (foundUI == nullptr)
		return;

	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&foundUI](weak_ptr<CToolUIBase> pUI) {
		if (foundUI == pUI.lock())
			return true;

		return false;
		});

	if (foundUI->IsChild())
		return;

	if (it == m_UIVec.end())
		m_UIVec.emplace_back(foundUI);

	/* 여기에서 ZOrder Sorting을 한 번 해줄 필요가 있음 */
	SortUIZOrder();

	foundUI->ActivateUI();

	m_pLatestActivatedUI = foundUI;
}

void Tool_UI::CToolUIMgr::DeactivateUI(const string& _strUITag)
{
	if (m_UIVec.empty())
		return;

	shared_ptr<CToolUIBase> foundUI = FindUI(_strUITag);
	if (foundUI == nullptr)
		return;

	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&foundUI](weak_ptr<CToolUIBase> toolUIBase) {
		if (!toolUIBase.expired())
		{
			if (foundUI == toolUIBase.lock())
				return true;

			return false;
		}

		return false;
		});

	if (it == m_UIVec.end())
		return;

	foundUI->DeactivateUI();

	m_UIVec.erase(it);

	/* 여기에서 ZOrder Sorting을 한 번 해줄 필요가 있음 */
	SortUIZOrder();

	if (m_UIVec.size() >= 1)
		m_pLatestActivatedUI = m_UIVec.back();
	else
		m_pLatestActivatedUI.reset();
}

void Tool_UI::CToolUIMgr::SelectUI(const string& _strUITag)
{
	auto it = find_if(m_UIVec.begin(), m_UIVec.end(), [&_strUITag](weak_ptr<CToolUIBase> pUI) {
		if (!pUI.expired())
		{
			if (pUI.lock()->GetUITag() == _strUITag)
				return true;

			return false;
		}

		return false;
		});

	if (it != m_UIVec.end())
	{
		m_pLatestActivatedUI = it->lock();

		return;
	}

	ActivateUI(_strUITag);
}

void Tool_UI::CToolUIMgr::DeleteSelectedUI(const string& _strUITag)
{
	DeactivateUI(_strUITag);

	auto it = m_UIs.find(_strUITag);
	if (it == m_UIs.end())
		return;

	if (!it->second->GetChildrenList().empty())
	{
		unordered_map<string, weak_ptr<CToolUIBase>> childrenList = it->second->GetChildrenList();
		for (auto& childPair : childrenList)
		{
			DeleteSelectedUI(childPair.first);
		}
	}

	it->second.reset();

	m_UIs.erase(it);
}

void Tool_UI::CToolUIMgr::SortUIZOrder()
{
	if (static_cast<_uint>(m_UIVec.size()) > 1)
	{
		::sort(m_UIVec.begin(), m_UIVec.end(), [](weak_ptr<CToolUIBase> first, weak_ptr<CToolUIBase> second) {
			if (first.lock()->IsSelected())
			{
				if (first.lock()->GetSelectedZOrder() > second.lock()->GetZOrder())
					return true;
				else
					return false;
			}

			else if (second.lock()->IsSelected())
			{
				if (first.lock()->GetZOrder() > second.lock()->GetSelectedZOrder())
					return true;
				else
					return false;
			}

			else
			{
				if (first.lock()->GetZOrder() > second.lock()->GetZOrder())
					return true;
				else
					return false;
			}
		});
	}
}

void Tool_UI::CToolUIMgr::ResetUISelect()
{
	for (auto& ui : m_UIVec)
	{
		if (ui.lock() != nullptr)
		{
			ui.lock()->SetIsSelected(false);
		}
	}
}

void Tool_UI::CToolUIMgr::KeyInput()
{
	if (GAMEINSTANCE->KeyDown(DIK_DELETE))
	{
		//_uint activatedUISize = static_cast<_uint>(m_UIVec.size());
		//if (activatedUISize > 1)
		//{
		//	if (m_pLatestActivatedUI.lock() != nullptr)
		//	{
		//		DeactivateUI(m_pLatestActivatedUI.lock()->GetUITag());
		//	}

		//	else
		//	{
		//		DeactivateUI(m_UIVec.end()->lock()->GetUITag()); // ZOrder Sorting을 통해 가장 앞에 있는 UI가 Vector 컨테이너 내 가장 뒤에 있을 예정
		//		//_uiVec.pop_back(); // ZOrder Sorting을 통해 가장 앞에 있는 UI가 Vector 컨테이너 내 가장 뒤에 있을 예정
		//	}

		//	m_pLatestActivatedUI = m_UIVec.end()->lock();
		//}

		//else if (activatedUISize == 1)
		//{
		//	if (m_pLatestActivatedUI.lock() != nullptr)
		//	{
		//		DeactivateUI(m_pLatestActivatedUI.lock()->GetUITag());
		//	}

		//	else
		//	{
		//		DeactivateUI(m_UIVec.end()->lock()->GetUITag()); // ZOrder Sorting을 통해 가장 앞에 있는 UI가 Vector 컨테이너 내 가장 뒤에 있을 예정
		//		//_uiVec.pop_back(); // ZOrder Sorting을 통해 가장 앞에 있는 UI가 Vector 컨테이너 내 가장 뒤에 있을 예정
		//	}

		//	//m_pLatestActivatedUI = m_pHUD;
		//	m_pLatestActivatedUI.reset();
		//}

	}

	/*if (GAMEINSTANCE->KeyDown(DIK_I))
	{
		shared_ptr<CToolUIBase> foundUI = FindUI("Inventory");
		if (foundUI != nullptr)
		{
			if (foundUI->IsActivated() == false)
				ActivateUI("Inventory");
		}
	}

	if (GAMEINSTANCE->KeyDown(DIK_O))
	{
		shared_ptr<CToolUIBase> foundUI = FindUI("Inventory1");
		if (foundUI != nullptr)
		{
			if (foundUI->IsActivated() == false)
				ActivateUI("Inventory1");
		}
	}*/

	if (GAMEINSTANCE->KeyDown(DIK_LALT))
	{
		shared_ptr<CToolUIBase> foundUI = FindUI("MouseSlot");
		if (foundUI != nullptr)
		{
			if (m_isMouseSlotActivated == false)
			{
				ActivateUI("MouseSlot");
				m_isMouseSlotActivated = true;
				foundUI->SetIsSelected(true);
			}
			else
			{
				DeactivateUI("MouseSlot");
				m_isMouseSlotActivated = false;
				foundUI->SetIsSelected(false);
			}
		}
	}

	/*if (m_pLatestActivatedUI.lock() != m_pHUD && !m_pLatestActivatedUI.expired() && m_pLatestActivatedUI.lock()->GetUIType() != TOOL_UI_SLIDER)
	{
		if (GAMEINSTANCE->MouseDown(DIM_LB) && (m_pLatestActivatedUI.lock() != m_pHUD))
		{
			auto it = m_UIVec.rbegin();
			for (it; it != m_UIVec.rend(); ++it)
			{
				if (it->lock()->IsMouseOn())
				{
					ResetUISelect();
					it->lock()->SetIsSelected(true);
					m_pLatestActivatedUI = it->lock();

					POINT mousePt;
					::GetCursorPos(&mousePt);
					::ScreenToClient(g_hWnd, &mousePt);

					_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
					_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

					m_vDir = _float3(mousePosX, mousePosY, 0.f) - m_pLatestActivatedUI.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
					m_fDist = m_vDir.Length();
					m_vDir.Normalize();

					return;
				}
			}
		}*/

	if (m_isMouseSlotActivated == false)
	{
		if (GAMEINSTANCE->MouseDown(DIM_LB))
		{
			auto it = m_UIVec.rbegin();
			for (it; it != m_UIVec.rend(); ++it)
			{
				if (it->lock()->IsMouseOn() && !it->lock()->IsChild())
				{
					ResetUISelect();

					if (it->lock()->GetUIType() == TOOL_UI_SLOT && dynamic_pointer_cast<CToolUISlot>(it->lock())->GetSlotType() == TOOL_SLOT_MOUSE)
						continue;

					it->lock()->SetIsSelected(true);
					m_pLatestActivatedUI = it->lock();

					POINT mousePt;
					::GetCursorPos(&mousePt);
					::ScreenToClient(g_hWnd, &mousePt);

					_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
					_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

					m_vDir = _float3(mousePosX, mousePosY, 0.f) - m_pLatestActivatedUI.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
					m_fDist = m_vDir.Length();
					m_vDir.Normalize();

					return;
				}
			}
		}

		if (GAMEINSTANCE->MousePressing(DIM_LB) && !m_pLatestActivatedUI.expired())
		{
			if (m_pLatestActivatedUI.lock()->IsSelected() && m_pLatestActivatedUI.lock()->IsActivated())
			{
				if (m_pLatestActivatedUI.lock()->GetUIType() == TOOL_UI_SLOT && dynamic_pointer_cast<CToolUISlot>(m_pLatestActivatedUI.lock())->GetSlotType() == TOOL_SLOT_MOUSE)
				{

				}

				else
				{
					POINT mousePt;
					::GetCursorPos(&mousePt);
					::ScreenToClient(g_hWnd, &mousePt);

					_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
					_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

					_float3 dir = _float3(mousePosX, mousePosY, 0.f) - m_pLatestActivatedUI.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
					_float dist = dir.Length();
					dir.Normalize();

					if (dist != m_fDist)
					{
						_float3 moveDir = m_vDir * m_fDist;

						_float3 newPos = _float3(mousePosX, mousePosY, 0.f) - moveDir;

						m_pLatestActivatedUI.lock()->SetUIPos(newPos.x, newPos.y);
					}
				}
			}
		}

		if (GAMEINSTANCE->MouseUp(DIM_LB) && !m_pLatestActivatedUI.expired())
		{
			m_pLatestActivatedUI.lock()->SetIsSelected(false);
		}
	}
}

HRESULT Tool_UI::CToolUIMgr::PrepareBindFuncs()
{
	/*m_bindFuncs.emplace("Toggle Inventory", [&](void) -> void {
		shared_ptr<CToolUIBase> foundUI = FindUI("Inventory");
		if (foundUI == nullptr)
			return;

		if (foundUI->IsActivated())
		{
			DeactivateUI(foundUI->GetUITag());
		}
		else
		{
			ActivateUI(foundUI->GetUITag());
		}
		});*/

	m_bindFuncs.emplace("Deactivate Inventory", [&]() {
		shared_ptr<CToolUIBase> foundUI = FindUI("InventoryPanel");
		if (foundUI == nullptr)
			return;

		if (foundUI->IsActivated())
			DeactivateUI(foundUI->GetUITag());
		});

	return S_OK;
}

void Tool_UI::CToolUIMgr::SortChildUIs(shared_ptr<CToolUIBase> _parentItemBG, shared_ptr<CToolUIBase> _childItemBG)
{
	_float upperPadding = 30.f;
	_float parentSizeX = _parentItemBG->GetUISize().x;
	_float parentSizeY = _parentItemBG->GetUISize().y;
	_float parentTop = _parentItemBG->GetUIPos().y + (parentSizeY / 2.f);

	_float parentStartTop = parentTop - upperPadding;
	_float parentBottom = _parentItemBG->GetUIPos().y - (parentSizeY / 2.f);
	_float parentLeft = _parentItemBG->GetUIPos().x - (parentSizeX / 2.f);
	_float parentRight = _parentItemBG->GetUIPos().x + (parentSizeX / 2.f);
	
	_float intervalSize = (parentSizeX / 6.f) * 1.2f;

	vector<_float2> uiPos;

	_uint maxNum = 50;
	for (_uint i = 0; i < maxNum; ++i)
	{
		if (i == 0)
		{
			uiPos.emplace_back(_float2(parentLeft + (intervalSize / 2.f), parentStartTop - (intervalSize / 2.f)));
		}

		else if (i < 5 && i > 0)
		{
			uiPos.emplace_back(_float2(uiPos[i - 1].x + intervalSize, parentStartTop - (intervalSize / 2.f)));
		}

		else
		{
			uiPos.emplace_back(_float2(uiPos[i - 5].x, uiPos[i - 5].y - intervalSize));
		}
	}

	for (_uint i = 0; i < maxNum; ++i)
	{
		string childUITag = _childItemBG->GetUITag();
		stringstream ss;

		for (char c : childUITag)
		{
			if (std::isdigit(c))
				ss << c;
		}

		int foundNum{ 0 };
		ss >> foundNum;

		if (foundNum == i)
		{
			_childItemBG->SetUIPos(uiPos[i].x, uiPos[i].y);
		}
	}
}

shared_ptr<CToolUIMgr> Tool_UI::CToolUIMgr::Create()
{
	shared_ptr<CToolUIMgr> pInstance = make_shared<CToolUIMgr>();

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CToolUIMgr::Create", MB_OK);

		pInstance.reset();

		return nullptr;
	}

	return pInstance;
}
