#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
class CModel;
class CTexture;
class CRigidBody;
END

BEGIN(Tool_Effect)

class CAnimModel : public CGameObject
{
public:
	enum AnimModel_WEAPON_TYPE
	{
		WEAPON_TYPE_SWORD, WEAPON_TYPE_CROSSBOW, WEAPON_TYPE_STAFF, WEAPON_TYPE_END
	};

public:
	CAnimModel();
	virtual ~CAnimModel();

public:
	virtual HRESULT Initialize(shared_ptr<class CModel> _pModel);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void	SetModel(shared_ptr<CModel> _pModel) { m_pModelCom = _pModel; }
	void	RenderWeapon(_float _fTimeDelta);

public:
	void SetCollider(CCollider::ECOL_TYPE _ColType);
	shared_ptr<CCollider> GetCollider();

	void EnableCollision(const char* _strShapeTag) override;
	void DisableCollision(const char* _strShapeTag) override;

	void	EnableCollider() { isColliderOn = true; }
	void	DisableCollider() { isColliderOn = false; }

private:
	HRESULT AddWeapons();		// 무기 추가
	HRESULT	AddRigidBody();		// RigidBody 추가
	HRESULT	AddCollider();		// Collider 추가

private:
	AnimModel_WEAPON_TYPE		m_eCurrentWeapon = { WEAPON_TYPE_CROSSBOW };	// 현재 무기 타입 상태

private:
	_bool	isColliderOn = { false };

private:

	shared_ptr<CModel>		m_pModelCom;
	map<string, shared_ptr<CWeapon>>	m_pPlayerWeapons;

	//shared_ptr<CRigidBody>	m_pRigidBody = { nullptr };

	shared_ptr<CCollider>	m_pSphereCollider = { nullptr };
	shared_ptr<CCollider>	m_pBoxCollider = { nullptr };

	shared_ptr<CCollider>	m_pCurrentCollider = { nullptr };


public:
	static shared_ptr<CAnimModel> Create(shared_ptr<class CModel> _pModel);
};

END