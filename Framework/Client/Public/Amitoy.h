#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CCameraSpherical;

class CAmitoy final : public CGameObject
{
	enum EACTIONS
	{
		ACT_IDLE = 7,
		ACT_WALK = 13,
		ACT_RUN = 12,
		ACT_ATTACK = 4,
		ACT_STUN = 0,
		ACT_GETITEM = 14
	};

public:
	CAmitoy();
	virtual ~CAmitoy();

public:
	static shared_ptr<CAmitoy> Create();

public:
	HRESULT Initialize();
	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;

	virtual void EnableCollision(const char* _strShapeTag) override;
	virtual void DisableCollision(const char* _strShapeTag) override;

public:
	void SpawnAmitoy();
	void DespawnAmitoy();
	void AllAcquired();

private:
	void KeyInput(float _fTimeDelta);

	HRESULT ReadyJsonShape();
	HRESULT ReadyCamera();

private:
	shared_ptr<CModel> m_pModelCom{ nullptr };
	shared_ptr<CRigidBody> m_pRigidBody{ nullptr };
	weak_ptr<CCameraSpherical> m_pCamera;
	weak_ptr<CGameObject> m_pPlayer;
	bool m_isInteraction{ false };
	bool m_isAttactObject{ false };
	bool m_isAttactedBoom{ false };
	float m_fIdleTime{ 0.0f };
	const char* m_szIdleSoundTag[3]{};
	float m_fSoundWeight[3]{ 0.0f };
	bool m_isUsingGrab{ false };
	bool m_isAllAcquired{ false };
};

END