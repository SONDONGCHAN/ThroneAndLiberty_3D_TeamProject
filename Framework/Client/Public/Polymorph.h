#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CRigidBody;
END

BEGIN(Client)
class CCameraSpherical;

class CPolymorph : public CGameObject
{
public:
	CPolymorph();
	virtual ~CPolymorph()  override;

	virtual void EnableCollision(const char* _strShapeTag) override;
	virtual void DisableCollision(const char* _strShapeTag) override;

	virtual void SetTransStart(_float _fChangingTime);

public:
	virtual void SetChanged() { m_isChanged = false; }

public:
	shared_ptr<CRigidBody> GetRigidBody() {return  m_pRigidBody; }

public:
	void SetLightIdx(_uint _iFrontIdx, _uint _iBackIdx) {
		m_iLightIdxFront = _iFrontIdx;
		m_iLightIdxBack = _iBackIdx;
	}

	void SetIsInDungeon(_bool _IsInDungeon) {

		m_IsDungeonLevel = _IsInDungeon;

	}

public:
	void UpdateLightDesc();

protected:
	_uint m_iLightIdxFront = 0;
	_uint m_iLightIdxBack = 0;

	_bool m_IsDungeonLevel = true;

protected:
	shared_ptr<CModel>			m_pModelCom = { nullptr };
	shared_ptr<CRigidBody>		m_pRigidBody = { nullptr };
	weak_ptr<CCameraSpherical>	m_pCamera;

	_bool	m_isChanged = { false };
	_float	m_fChangingTime = { 0.f };
};

END
