#pragma once

#include "Tool_CC_Define.h"
#include "GameObject.h"


class CTerrain;
class CEnvironmentObject;
class COctree;

class CMap : public CGameObject
{
public:
	CMap();
	~CMap();

public:
	virtual HRESULT Initialize(const string& _strMapFilePath, _bool _IsField);
	virtual void PriorityTick(_float _fTimeDelta);
	virtual void Tick(_float _fTimeDelta);
	virtual void LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private:
	string m_strMapName{};

	shared_ptr<CTerrain> m_pTerrain{};
	vector<shared_ptr<CEnvironmentObject>> m_vecObjects{};
	//vector<shared_ptr<CEnvironmentObject>> m_vecInstancingObjects{};
	//vector<shared_ptr<CEnvironmentObject>> m_vecObjects{};


private:
	/*For. Test*/
	_bool m_IsField = false;


public:
	static shared_ptr<CMap> Create(const string& _strMapFilePath, _bool _IsField);
};

