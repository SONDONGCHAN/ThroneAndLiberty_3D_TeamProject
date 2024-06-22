#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Tool_Map)
class CEnvironmentObject;
class CGroupObject;

class CArea : public CGameObject
{
public:
	enum TYPE { TYPE_EDIT, TYPE_VIEW, TYPE_WAIT, TYPE_END };

private:
	CArea();
	DESTRUCTOR(CArea)
		
public:
	TYPE GetType() { return m_eType; }
	void SetType(TYPE _eType) { m_eType = _eType; }
	void SetAreaName(string _strAreaName) { m_strAreaName = _strAreaName; }
	string GetAreaName() { return m_strAreaName; }
	map<string, vector<shared_ptr<CEnvironmentObject>>>& GetObjects() { return m_mapObjects; }
	vector<shared_ptr<CGroupObject>>& GetGroupObjects() { return m_vecGroupObjects; }

public:
	virtual HRESULT Initialize(string _strAreaName, _float4x4 _matWorld = _float4x4::Identity);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void AddObject(shared_ptr<CEnvironmentObject> _pObject);
	void AddGroupObject(shared_ptr<CGroupObject> _pGroupObject);

private:
	TYPE m_eType{};
	string m_strAreaName{};
	map<string, vector<shared_ptr<CEnvironmentObject>>> m_mapObjects{};
	vector<shared_ptr<CGroupObject>> m_vecGroupObjects{};

public:
	static shared_ptr<CArea> Create(string _strAreaName, _float4x4 _matWorld = _float4x4::Identity);
};
END