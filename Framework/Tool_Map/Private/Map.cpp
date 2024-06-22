#include "Map.h"
#include "Area.h"
#include "EnvironmentObject.h"
#include "GroupObject.h"

CMap::CMap()
{
}

#ifdef CHECK_REFERENCE_COUNT
CMap::~CMap()
{
}
#endif // CHECK_REFERENCE_COUNT

void CMap::SetInstancingObjects(vector<shared_ptr<CEnvironmentObject>> _vecInstancingObjects)
{
	m_vecInstancingObjects.clear();
	m_vecInstancingObjects = _vecInstancingObjects;
}

HRESULT CMap::Initialize(string _strMapName, _float4x4 _matWorld)
{
	__super::Initialize(nullptr);

	m_pTransformCom->SetWorldMatrix(_matWorld);

	m_strMapName = _strMapName;

	return S_OK;
}

void CMap::PriorityTick(_float _fTimeDelta)
{
	string a = m_vecAreas[*m_pEditModeAreaIndex]->GetAreaName();
	m_vecAreas[*m_pEditModeAreaIndex]->PriorityTick(_fTimeDelta);


	for (const auto pInstancingObject : m_vecInstancingObjects)
		pInstancingObject->PriorityTick(_fTimeDelta);
}

void CMap::Tick(_float _fTimeDelta)
{
	string a = m_vecAreas[*m_pEditModeAreaIndex]->GetAreaName();
	m_vecAreas[*m_pEditModeAreaIndex]->Tick(_fTimeDelta);
	for (const auto pInstancingObject : m_vecInstancingObjects)
		pInstancingObject->Tick(_fTimeDelta);
}

void CMap::LateTick(_float _fTimeDelta)
{
	string a = m_vecAreas[*m_pEditModeAreaIndex]->GetAreaName();
	m_vecAreas[*m_pEditModeAreaIndex]->LateTick(_fTimeDelta);
	for (const auto pInstancingObject : m_vecInstancingObjects)
		pInstancingObject->LateTick(_fTimeDelta);
}

HRESULT CMap::Render()
{
	return S_OK;
}

HRESULT CMap::AddArea(shared_ptr<CArea> _pArea)
{
	string strAreaName = _pArea->GetAreaName();
	for (const auto pArea : m_vecAreas)
	{
		if (strAreaName == pArea->GetAreaName())
		{
			MSG_BOX("해당 키값에 대한 영역이 이미 존재합니다.");
			return E_FAIL;
		}
	}

	m_vecAreas.push_back(_pArea);

	return S_OK;
}

void CMap::AddObject(shared_ptr<CEnvironmentObject> _pObject)
{
	m_vecAreas[*m_pEditModeAreaIndex]->AddObject(_pObject);
}

void CMap::AddGroupObject(shared_ptr<CGroupObject> _pGroupObject)
{
	m_vecAreas[*m_pEditModeAreaIndex]->AddGroupObject(_pGroupObject);
}

void CMap::ResetMap()
{
	for (auto pArea : m_vecAreas)
		pArea->SetActive(false);

	for (auto pInstObject : m_vecInstancingObjects)
		pInstObject->SetActive(false);

	*m_pEditModeAreaIndex = 0;
}

shared_ptr<CMap> CMap::Create(string _strMapName, _float4x4 _matWorld)
{
	shared_ptr<CMap> pInstance = WRAPPING(CMap)();

	if (FAILED(pInstance->Initialize(_strMapName, _matWorld)))
	{
		MSG_BOX("Failed to Created : CMap");
		pInstance.reset();
	}

	return pInstance;
}
