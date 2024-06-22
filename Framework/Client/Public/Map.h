#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CTerrain;
class CEnvironmentObject;

class CMap : public CGameObject
{
public:
	enum AREA { AREA_DEFAULT, AREA_IN_VILLAGE, AREA_OUT_VILLAGE, AREA_END };

public:
	CMap();
	~CMap();

public:
	void SetAreaType(AREA _eAreaType) { m_eChangetArea = _eAreaType; }

public:
	virtual HRESULT Initialize(const string& _strMapFilePath, _bool _IsField);
	virtual void PriorityTick(_float _fTimeDelta);
	virtual void Tick(_float _fTimeDelta);
	virtual void LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private:

	string m_strMapName{};

	AREA m_eChangetArea = AREA_DEFAULT;
	AREA m_eCurrentArea = AREA_DEFAULT;

	shared_ptr<CTerrain> m_pTerrain{};

	array<vector<shared_ptr<CEnvironmentObject>>, AREA_END> arrObjectGroups{};

private:
	/*For. Test*/
	_bool m_IsField = false;


public:
	static shared_ptr<CMap> Create(const string& _strMapFilePath, _bool _IsField);
};

END