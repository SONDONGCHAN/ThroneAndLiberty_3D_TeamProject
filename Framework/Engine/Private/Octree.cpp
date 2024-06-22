#include "Octree.h"
#include "GameInstance.h"
#include "GameObject.h"

COctree::COctree()
{
}

COctree::~COctree()
{
}

shared_ptr<COctree> COctree::Create(const _float3& _vCenter, const float& _fHalfWidth, const int& _iDepthLimit, shared_ptr<COctree> _pParent)
{
	shared_ptr<COctree> pInstance = make_shared<COctree>();

	if (FAILED(pInstance->Initialize(_vCenter, _fHalfWidth, _iDepthLimit, _pParent)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"COctree::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT COctree::Initialize(const _float3& _vCenter, const float& _fHalfWidth, const int& _iDepthLimit, shared_ptr<COctree> _pParent)
{
	if (_pParent)
		m_pParent = _pParent;

	memcpy_s(&m_BoundingBox.Center, sizeof(_float3), &_vCenter, sizeof(_float3));
	memcpy_s(&m_BoundingBox.Extents.x, sizeof(float), &_fHalfWidth, sizeof(float));
	memcpy_s(&m_BoundingBox.Extents.y, sizeof(float), &_fHalfWidth, sizeof(float));
	memcpy_s(&m_BoundingBox.Extents.z, sizeof(float), &_fHalfWidth, sizeof(float));
	memcpy_s(&m_iDepth, sizeof(int), &_iDepthLimit, sizeof(int));

	int childDepth = _iDepthLimit - 1;
	if (childDepth < 0)
	{
		return S_OK;
	}

	float vHalfWidth{ _fHalfWidth * 0.5f };

	float vRight{ _vCenter.x + vHalfWidth };
	float vLeft{ _vCenter.x - vHalfWidth };
	float vTop{ _vCenter.y + vHalfWidth };
	float vBottom{ _vCenter.y - vHalfWidth };
	float vFront{ _vCenter.z + vHalfWidth };
	float vBack{ _vCenter.z - vHalfWidth };

	m_pChilds[LBB] = COctree::Create({ vLeft,vBottom,vBack }, vHalfWidth, childDepth, shared_from_this());
	m_pChilds[RBB] = COctree::Create({ vRight,vBottom,vBack }, vHalfWidth, childDepth, shared_from_this());
	m_pChilds[LTB] = COctree::Create({ vLeft,vTop,vBack }, vHalfWidth, childDepth, shared_from_this());
	m_pChilds[RTB] = COctree::Create({ vRight,vTop,vBack }, vHalfWidth, childDepth, shared_from_this());

	m_pChilds[LBF] = COctree::Create({ vLeft,vBottom,vFront }, vHalfWidth, childDepth, shared_from_this());
	m_pChilds[RBF] = COctree::Create({ vRight,vBottom,vFront }, vHalfWidth, childDepth, shared_from_this());
	m_pChilds[LTF] = COctree::Create({ vLeft,vTop,vFront }, vHalfWidth, childDepth, shared_from_this());
	m_pChilds[RTF] = COctree::Create({ vRight,vTop,vFront }, vHalfWidth, childDepth, shared_from_this());
	return S_OK;
}

void COctree::PriorityTick(const float& _fTimeDelta)
{
	if (m_isObtainStatic && m_isVisible)
	{
		if (!m_GameObjects.empty())
		{
			for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
			{
				if ((*iter)->IsActive())
				{
					if ((*iter)->GetDistanceFromCam() < 40.0f)
					{
						(*iter)->SetEnable(true);
						(*iter)->PriorityTick(_fTimeDelta);
					}
					else
						(*iter)->SetEnable(false);
				}
				else
				{
					iter = m_GameObjects.erase(iter);
					continue;
				}
				iter++;
			}
		}
		if (m_iDepth)
		{
			for (int Child = LBB; Child < CHILDEND; Child++)
			{
				m_pChilds[Child]->PriorityTick(_fTimeDelta);
			}
		}
	}
}

void COctree::Tick(const float& _fTimeDelta)
{
	if (m_isObtainStatic && m_isVisible)
	{
		if (!m_GameObjects.empty())
		{
			for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
			{
				if ((*iter)->IsActive())
				{
					if ((*iter)->IsEnabled())
						(*iter)->Tick(_fTimeDelta);
				}
				else
				{
					iter = m_GameObjects.erase(iter);
					continue;
				}
				iter++;
			}
		}
		if (m_iDepth)
		{
			for (int Child = LBB; Child < CHILDEND; Child++)
			{
				m_pChilds[Child]->Tick(_fTimeDelta);
			}
		}
	}
}

void COctree::LateTick(const float& _fTimeDelta)
{
	if (m_isObtainStatic && m_isVisible)
	{
		if (!m_GameObjects.empty())
		{
			for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
			{
				if ((*iter)->IsActive())
				{
					if ((*iter)->IsEnabled())
						(*iter)->LateTick(_fTimeDelta);
				}
				else
				{
					iter = m_GameObjects.erase(iter);
					continue;
				}
				iter++;
			}
		}
		if (!m_Instances.empty())
		{
			shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
			for (auto& pair : m_Instances)
			{
				for (auto& vecInstance : pair.second)
				{
					for (auto& Info : vecInstance.InstanceInfo)
					{
						if (vecInstance.fRadius < 5.2f)
						{
							if (40.0f > pGameInstance->DistanceFromCam(Info.first) && pGameInstance->isInWorldSpace(Info.first, vecInstance.fRadius))
							{
								vecInstance.pGameObject.lock()->AddInstancingWorldMatrix(Info.second);
							}
						}
						else
						{
							if (pGameInstance->isInWorldSpace(Info.first, vecInstance.fRadius))
							{
								vecInstance.pGameObject.lock()->AddInstancingWorldMatrix(Info.second);
							}
						}
					}
				}
			}
		}
		if (m_iDepth)
		{
			for (int Child = LBB; Child < CHILDEND; Child++)
			{
				m_pChilds[Child]->LateTick(_fTimeDelta);
			}
		}
	}
}

bool COctree::AddStaticObject(shared_ptr<CGameObject> _pGameObject, const _float3& _vPoint, const float& _fRadius)
{
	ContainmentType Containment = (_fRadius == 0.0f ? m_BoundingBox.Contains(_vPoint) : m_BoundingBox.Contains(BoundingSphere(_vPoint, _fRadius)));
	if (CONTAINS == Containment)
	{
		m_isObtainStatic = true;
		if (m_iDepth)
		{
			int ChildIndex{ 0 };
			if (m_BoundingBox.Center.x <= _vPoint.x)
				ChildIndex |= 1;
			if (m_BoundingBox.Center.y <= _vPoint.y)
				ChildIndex |= 2;
			if (m_BoundingBox.Center.z <= _vPoint.z)
				ChildIndex |= 4;

			if (m_pChilds[ChildIndex]->AddStaticObject( _pGameObject, _vPoint,  _fRadius))
				return true;

			m_GameObjects.emplace_back(_pGameObject);
			return true;
		}
		else
		{
			m_GameObjects.emplace_back(_pGameObject);
			return true;
		}
	}
	else if (!m_pParent.lock() && Containment)
	{
		m_isObtainStatic = true;
		m_GameObjects.emplace_back(_pGameObject);
		return true;
	}

	return false;
}

bool COctree::AddStaticInstance(const string& _strModelTag, shared_ptr<CGameObject> _pGameObject, const _float3& _vPoint, const _float4x4& _WorldMatrix, const float& _fRadius)
{
	ContainmentType Containment = (_fRadius == 0.0f ? m_BoundingBox.Contains(_vPoint) : m_BoundingBox.Contains(BoundingSphere(_vPoint, _fRadius)));
	if (CONTAINS == Containment)
	{
		m_isObtainStatic = true;
		if (m_iDepth)
		{
			int ChildIndex{ 0 };
			if (m_BoundingBox.Center.x <= _vPoint.x)
				ChildIndex |= 1;
			if (m_BoundingBox.Center.y <= _vPoint.y)
				ChildIndex |= 2;
			if (m_BoundingBox.Center.z <= _vPoint.z)
				ChildIndex |= 4;

			if (m_pChilds[ChildIndex]->AddStaticInstance(_strModelTag, _pGameObject, _vPoint, _WorldMatrix, _fRadius))
				return true;

			vector<tInstance>* pInstance = FindInstance(_strModelTag);
			if (pInstance)
			{
				for (auto& Instances : *pInstance)
				{
					if (Instances.pGameObject.lock() == _pGameObject)
					{
						Instances.InstanceInfo.emplace_back(_vPoint, _WorldMatrix);
						return true;
					}
				}
				tInstance Temp;
				Temp.pGameObject = _pGameObject;
				Temp.fRadius = _fRadius;
				Temp.InstanceInfo.emplace_back(_vPoint, _WorldMatrix);
				(*pInstance).emplace_back(Temp);
			}
			else
			{
				vector<tInstance> vector;
				tInstance Temp;
				Temp.pGameObject = _pGameObject;
				Temp.fRadius = _fRadius;
				Temp.InstanceInfo.emplace_back(_vPoint, _WorldMatrix);
				vector.emplace_back(Temp);
				m_Instances.emplace(_strModelTag, vector);
			}
			return true;
		}
		else
		{
			vector<tInstance>* pInstance = FindInstance(_strModelTag);
			if (pInstance)
			{
				for (auto& Instances : *pInstance)
				{
					if (Instances.pGameObject.lock() == _pGameObject)
					{
						Instances.InstanceInfo.emplace_back(_vPoint, _WorldMatrix);
						return true;
					}
				}
				tInstance Temp;
				Temp.pGameObject = _pGameObject;
				Temp.fRadius = _fRadius;
				Temp.InstanceInfo.emplace_back(_vPoint, _WorldMatrix);
				(*pInstance).emplace_back(Temp);
			}
			else
			{
				vector<tInstance> vector;
				tInstance Temp;
				Temp.pGameObject = _pGameObject;
				Temp.fRadius = _fRadius;
				Temp.InstanceInfo.emplace_back(_vPoint, _WorldMatrix);
				vector.emplace_back(Temp);
				m_Instances.emplace(_strModelTag, vector);
			}
			return true;
		}
	}
	else if (!m_pParent.lock() && Containment)
	{
		vector<tInstance>* pInstance = FindInstance(_strModelTag);
		if (pInstance)
		{
			for (auto& Instances : *pInstance)
			{
				if (Instances.pGameObject.lock() == _pGameObject)
				{
					Instances.InstanceInfo.emplace_back(_vPoint, _WorldMatrix);
					return true;
				}
			}
			tInstance Temp;
			Temp.pGameObject = _pGameObject;
			Temp.fRadius = _fRadius;
			Temp.InstanceInfo.emplace_back(_vPoint, _WorldMatrix);
			(*pInstance).emplace_back(Temp);
		}
		else
		{
			vector<tInstance> vector;
			tInstance Temp;
			Temp.pGameObject = _pGameObject;
			Temp.fRadius = _fRadius;
			Temp.InstanceInfo.emplace_back(_vPoint, _WorldMatrix);
			vector.emplace_back(Temp);
			m_Instances.emplace(_strModelTag, vector);
		}
		m_isObtainStatic = true;
		return true;
	}

	return false;
}

void COctree::Culling()
{
	if (m_isObtainStatic)
	{
		switch (isDraw())
		{
		case DISJOINT:
		{
			Invisible();
		}
		break;
		case INTERSECTS:
		{
			m_isVisible = true;
			if (m_iDepth)
			{
				for (auto& Child : m_pChilds)
				{
					Child->Culling();
				}
			}
		}
		break;
		case CONTAINS:
		{
			AllVisible();
		}
		break;
		}
	}
}

void COctree::Clear()
{
	m_GameObjects.clear();
	m_Instances.clear();
	m_isVisible = false;
	m_isObtainStatic = false;
	if (m_iDepth)
	{
		for (auto& Child : m_pChilds)
		{
			Child->Clear();
		}
	}
}

ContainmentType COctree::isDraw()
{
	return GAMEINSTANCE->isInWorldSpace(m_BoundingBox);
}

void COctree::AllVisible()
{
	m_isVisible = true;
	if (m_iDepth)
	{
		for (auto& Child : m_pChilds)
		{
			Child->AllVisible();
		}
	}
}

void COctree::Invisible()
{
	m_isVisible = false;
	/*if (m_iDepth)
	{
		for (auto& Child : m_pChilds)
		{
			Child->Invisible();
		}
	}*/
}

vector<COctree::tInstance>* COctree::FindInstance(const string& _strModelTag)
{
	auto iter = m_Instances.find(_strModelTag);
	if (iter == m_Instances.end())
		return nullptr;

	return &iter->second;
}
