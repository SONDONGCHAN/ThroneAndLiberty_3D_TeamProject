#pragma once
#include "FonosAmitoy.h"

BEGIN(Client)

class CAIAmitoy : public CFonosAmitoy
{
public:
	enum AMITOY_MUT_STATE
	{
		AMITOY_MUT_IDLE = 18,
		AMITOY_MUT_SHOCK = 7,
		AMITOY_MUT_SUPERJUMP = 4,
		AMITOY_MUT_RUN = 12,
		AMITOY_MUT_WALK = 13,
		AMITOY_MUT_APPEAR = 6,
		AMITOY_MUT_DISAPPEAR = 5,
		AMITOY_MUT_CEREMONY = 14,
		AMITOY_MUT_LAUGH = 15,
		AMITOY_MUT_DISAPPOINT = 17,
		AMITOY_MUT_KNOCKBACK = 21
	};

	enum AMITOY_TREE_STATE
	{
		AMITOY_TREE_IDLE = 18,
		AMITOY_TREE_SHOCK = 7,
		AMITOY_TREE_SUPERJUMP = 4,
		AMITOY_TREE_RUN = 12,
		AMITOY_TREE_WALK = 13,
		AMITOY_TREE_APPEAR = 6,
		AMITOY_TREE_DISAPPEAR = 5,
		AMITOY_TREE_CEREMONY = 14,
		AMITOY_TREE_LAUGH = 15,
		AMITOY_TREE_DISAPPOINT = 17,
		AMITOY_TREE_KNOCKBACK = 21
	};

public:
	CAIAmitoy();
	virtual ~CAIAmitoy();

public:
	static shared_ptr<CFonosAmitoy> Create();

public:
	HRESULT Initialize();
	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;

	virtual void EnableCollision(const char* _strShapeTag) override;
	virtual void DisableCollision(const char* _strShapeTag) override;

private:
	_bool		AbnormalCheck(float _fTimeDelta);
	AMITOY_STATE KeyInput(float _fTimeDelta);

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

	AMITOY_STATE m_eCurState = AMITOY_STATE_IDLE;
	AMITOY_STATE m_ePrevState = AMITOY_STATE_IDLE;
	AMITOY_STATE m_eNextState = AMITOY_STATE_IDLE;

	_float m_fAbnormalTime = 0.f;

	_bool		m_bShock = false;
	_bool		m_bJumpBack = false;
	_bool		m_bSlow = false;
	_bool		m_bSuperJump = false;

	bool m_isInteraction{ false };
	bool m_isAttactObject{ false };
	bool m_isAttactedBoom{ false };
	float m_fIdleTime{ 0.0f };
	const char* m_szIdleSoundTag[3]{};
	float m_fSoundWeight[3]{ 0.0f };
};

END