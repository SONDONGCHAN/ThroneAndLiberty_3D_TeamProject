#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Tool_Map)
class CArea;
//class CTerrain;
class CEnvironmentObject;
class CGroupObject;

class CMap : public CGameObject
{
private:
	CMap();
	DESTRUCTOR(CMap)

public:
	string GetMapName() { return m_strMapName; }
	vector<shared_ptr<CArea>>& GetAreas() { return m_vecAreas; }
	
	//vector<shared_ptr<CEnvironmentObject>>& GetInstancingObjects() { return m_vecInstancingObjects; }

	void SetEditModeArea(_uint* _pEditModeAreaIndex) { m_pEditModeAreaIndex = _pEditModeAreaIndex; }
	void SetInstancingObjects(vector<shared_ptr<CEnvironmentObject>> _vecInstancingObjects);

public:
	virtual HRESULT Initialize(string _strAreaName, _float4x4 _matWorld = _float4x4::Identity);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT AddArea(shared_ptr<CArea> _pArea);
	void AddObject(shared_ptr<CEnvironmentObject> _pObject);
	void AddGroupObject(shared_ptr<CGroupObject> _pGroupObject);
	void ResetMap();

private:
	_uint* m_pEditModeAreaIndex{};

	string m_strMapName{};
	vector<shared_ptr<CArea>> m_vecAreas{};

	vector<shared_ptr<CEnvironmentObject>> m_vecInstancingObjects{};

public:
	static shared_ptr<CMap> Create(string _strMapName, _float4x4 _matWorld = _float4x4::Identity);
};
END