#include "Area.h"
#include "EnvironmentObject.h"
#include "GroupObject.h"

CArea::CArea()
{
}

#ifdef CHECK_REFERENCE_COUNT
CArea::~CArea()
{
}
#endif // CHECK_REFERENCE_COUNT

HRESULT CArea::Initialize(string _strAreaName, _float4x4 _matWorld)
{
	__super::Initialize(nullptr);

	m_pTransformCom->SetWorldMatrix(_matWorld);

	m_strAreaName = _strAreaName;

	m_eType = TYPE_WAIT;

	return S_OK;
}

void CArea::PriorityTick(_float _fTimeDelta)
{
	// Objects
	for (auto& pair : m_mapObjects)
	{
		for (auto pObject = pair.second.begin(); pObject != pair.second.end();)

		{
			//Dead Check
			if (false == (*pObject)->IsActive())
			{
				pObject->reset();
				pObject = pair.second.erase(pObject);
				continue;
			}

			if ((*pObject)->IsEnabled())
			{
				(*pObject)->PriorityTick(_fTimeDelta);
			}

			pObject++;
		}
	}

	for (auto iter = m_mapObjects.begin(); iter != m_mapObjects.end();)
	{
		if (0 == static_cast<_uint>(iter->second.size()))
			iter = m_mapObjects.erase(iter);
		else
			++iter;
	}

	// GroupObjects
	for (auto pGroupObject = m_vecGroupObjects.begin(); pGroupObject != m_vecGroupObjects.end();)

	{
		//Dead Check
		if (false == (*pGroupObject)->IsActive())
		{
			pGroupObject->reset();
			pGroupObject = m_vecGroupObjects.erase(pGroupObject);
			continue;
		}

		if ((*pGroupObject)->IsEnabled())
		{
			(*pGroupObject)->PriorityTick(_fTimeDelta);
		}

		pGroupObject++;
	}
}

void CArea::Tick(_float _fTimeDelta)
{
	// Objects
	for (auto& pair : m_mapObjects)
	{
		for (auto pObject : pair.second)
		{
			if (nullptr != pObject && pObject->IsEnabled())
				pObject->Tick(_fTimeDelta);
		}
	}

	// GroupObjects
	for (auto pGroupObject : m_vecGroupObjects)
	{
		if (nullptr != pGroupObject && true == pGroupObject->IsEnabled())
			pGroupObject->Tick(_fTimeDelta);
	}
}

void CArea::LateTick(_float _fTimeDelta)
{
	// Objects
	for (auto& pair : m_mapObjects)
	{
		for (auto pObject : pair.second)
		{
			if (nullptr != pObject && pObject->IsEnabled())
				pObject->LateTick(_fTimeDelta);
		}
	}

	// GroupObjects
	for (auto pGroupObject : m_vecGroupObjects)
	{
		if (nullptr != pGroupObject && true == pGroupObject->IsEnabled())
			pGroupObject->LateTick(_fTimeDelta);
	}
}

HRESULT CArea::Render()
{
	return S_OK;
}

void CArea::AddObject(shared_ptr<CEnvironmentObject> _pObject)
{
	string strModelKey = _pObject->GetModelKey();

	if (true == m_mapObjects.count(strModelKey))
	{
		m_mapObjects.emplace(strModelKey, vector<shared_ptr<CEnvironmentObject>>());
		m_mapObjects[strModelKey].push_back(_pObject);
	}
	else 
	{
		for (const auto pObject : m_mapObjects[strModelKey])
		{
			if (pObject->GetObjID() == _pObject->GetObjID())
				return;
		}

		m_mapObjects[strModelKey].push_back(_pObject);
	}
}

void CArea::AddGroupObject(shared_ptr<CGroupObject> _pGroupObject)
{
	m_vecGroupObjects.push_back(_pGroupObject);
}

shared_ptr<CArea> CArea::Create(string _strAreaName, _float4x4 _matWorld)
{
	shared_ptr<CArea> pInstance = WRAPPING(CArea)();

	if (FAILED(pInstance->Initialize(_strAreaName, _matWorld)))
	{
		MSG_BOX("Failed to Created : CArea");
		pInstance.reset();
	}

	return pInstance;
}
