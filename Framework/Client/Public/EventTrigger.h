#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CEventTrigger : public CGameObject
{

public:
	CEventTrigger();
	virtual ~CEventTrigger() = default;

public:
	virtual HRESULT Initialize(vector<GeometrySphere>* _Geometries);
	virtual void LateTick(_float _fTimeDelta)  override;

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override; //처음 닿을 때
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag);	// 닿아 있을 때
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag);		// 끝날때

private:
	HRESULT AddRigidBody(vector<GeometrySphere>* _Geometries);

private:
	shared_ptr<CRigidBody>	m_pRigidBody = nullptr;

	_bool m_isColl = false;
	_bool m_isCollLost = false;
	string m_strCurrentCollTag = "";
	string m_strCurrentLostCollTag = "";

public:
	static shared_ptr<CEventTrigger> Create(vector<GeometrySphere>* _Geometries);


};

END