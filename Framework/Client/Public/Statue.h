#pragma once

#include "InteractionObj.h"

// 천사		파랑, 얼음창	
// 사신		노랑, 빛		낮을 싫어함
// 장님		빨강, 불		화형
// 성직자	연한초록, 얼음파편		동상

enum ST_Type
{
	ST_ENGEL = 0,
	ST_LEEPER,
	ST_BLIND,
	ST_PRIEST,
};

BEGIN(Engine)
class CModel;
class CTexture;
END

BEGIN(Client)

class CStatue : public CInteractionObj
{
public:
	CStatue();
	virtual ~CStatue() = default;

public:
	virtual HRESULT Initialize(_float3 _vPosition);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render();

public:
	void		SimulationOn();
	void		SimulationOff();

public:
	virtual void InteractionOn();
	void	SetStatueType(_int _iType) { m_iStatueType = _iType; }
	void	SetSpellCollisionAble(_bool _bAble) { m_bSpellCollisionAble = _bAble; }
	void	SetMatchStatue(weak_ptr<CStatue> _pMatchStatue) { m_pMatchStatue = _pMatchStatue; }
	_bool	GetSpellHitted() { return m_bSpellHitted; }
	_bool	GetMatchCorrect() { return m_bMatchCorrect; }

protected:
	HRESULT			AddRigidBody(string _strModelKey);		// RigidBody 추가
	virtual void	OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 닿는 순간
	virtual void	OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;	// 닿아 있을 때
	virtual void	OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;		// 끝날때

protected:
	_bool			Rotatate45Degree(_float _fTimeDelta);
	void			StatueRotateCount();

protected:
	_uint m_iLightIdx = 0;

public:
	static shared_ptr<CStatue> Create(_float3 _vPosition, _int _iStatueType, _bool _bGenType = false);

protected:
	shared_ptr<CRigidBody>	m_pRigidBody = { nullptr };				// Rigid Body
	string m_strModelName = "";										// Model 이름

private:
	_int	m_iStatueType = 0;		// 조각상 타입

	_float	m_fCalculateTime1 = 0.f;
	_bool	m_bStatueInteractOn = false;		// 상호작용 여부
	_bool	m_bSpellCollisionAble = false;		// 가능하면 던전방 배치된 석상
	_bool	m_bMatchCorrect = false;
	_bool	m_bSpellHitted = false;

	_int	m_iSpinCount = 0;		// 도는 조각상 돈 횟수
	_int	m_iTrueSpinCount = 0;	// 조각상 돌아야하는 횟수

	_float3	m_vRimLightColor = _float3(0.f, 0.f, 0.f);

	weak_ptr<CStatue>		m_pMatchStatue;		// 짝맞는 조각상
};

END