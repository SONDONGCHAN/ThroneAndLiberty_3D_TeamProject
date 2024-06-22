#pragma once

#include "Client_Defines.h"
#include "InteractionObj.h"

BEGIN(Client)

class CCameraSpherical;

class CFonosAmitoy : public CInteractionObj
{
public:
	enum AMITOY_STATE
	{
		AMITOY_STATE_IDLE = 0,
		AMITOY_STATE_SPIN = 18,
		AMITOY_STATE_SHOCK = 7,
		AMITOY_STATE_SUPERJUMP = 4,
		AMITOY_STATE_RUN = 12,
		AMITOY_STATE_WALK = 23,
		AMITOY_STATE_APPEAR = 6,
		AMITOY_STATE_DISAPPEAR = 5,
		AMITOY_STATE_CEREMONY = 14,
		AMITOY_STATE_LAUGH = 15,
		AMITOY_STATE_DISAPPOINT = 17,
		AMITOY_STATE_TALK = 21
	};

public:
	CFonosAmitoy();
	virtual ~CFonosAmitoy();

public:
	static shared_ptr<CFonosAmitoy> Create();

public:
	HRESULT Initialize();
	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;

public:
	void	TestKeyInput();
	void	SetTriggerOn(_bool _bOn) { m_bTriggerOn = _bOn; }

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;

	virtual void EnableCollision(const char* _strShapeTag) override;
	virtual void DisableCollision(const char* _strShapeTag) override;

private:
	_bool		AbnormalCheck(float _fTimeDelta);
	AMITOY_STATE KeyInput(float _fTimeDelta);

public:
	virtual void InteractionOn() override;

public:
	void		MiniGameCeremony();
	void		MiniGameEnd();

private:
	HRESULT AddRigidBody(string _strModelKey);

private:
	void State_In(_float _fTimeDelta);
	void State_Tick(_float _fTimeDelta);
	void State_Out(_float _fTimeDelta);

private:
	shared_ptr<CModel> m_pModelCom{ nullptr };
	shared_ptr<CRigidBody> m_pRigidBody{ nullptr };
	weak_ptr<CCameraSpherical> m_pCamera;

	AMITOY_STATE m_eCurState = AMITOY_STATE_APPEAR;
	AMITOY_STATE m_ePrevState = AMITOY_STATE_APPEAR;
	AMITOY_STATE m_eNextState = AMITOY_STATE_APPEAR;

	_float m_fAbnormalTime = 0.f;
	_float m_fInvTime = 0.f;
	_bool  m_bInvincible = false;

	_float		m_fCeremonyTime = 3.f;

	_bool		m_bShock	= false;
	_bool		m_bJumpBack	= false;
	_bool		m_bSlow		= false;
	_bool		m_bSuperJump = false;

	_bool		m_bTriggerOn = false;
	_bool		m_bGameStart = false;
	_bool		m_bRunStart = false;
	_bool		m_bGameEnd = false;

	_bool		m_bWinGame = false;

	_float		m_fGameStartWaitTime = 11.f;
	_float		m_fRaceTime = 20.f;

	_int		m_iCountSound = 4;

	_float		m_fRunSoundTimer = 1.f;

	bool m_isInteraction{ false };
	bool m_isAttactObject{ false };
	bool m_isAttactedBoom{ false };
	float m_fIdleTime{ 0.0f };
	const char* m_szIdleSoundTag[3]{};
	float m_fSoundWeight[3]{ 0.0f };

private:
	_bool m_bIsInteractedOnce{ false };

};

END