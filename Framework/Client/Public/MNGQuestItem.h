#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMNGQuestItem final : public CGameObject
{
public:
	CMNGQuestItem();
	virtual ~CMNGQuestItem();

public:
	static shared_ptr<CMNGQuestItem> Create(const string& _strModelTag, const _float4x4& _WorldMatrix, Geometry* _pGeometry, float _fRadius);

public:
	HRESULT Initialize(const string& _strModelTag, const _float4x4& _WorldMatrix, Geometry* _pGeometry, float _fRadius);
	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;

public:
	void SpawnQuestItem();
	void RenderPosUI();
	void SetAmitoyPointer(shared_ptr<CGameObject> _pAmitoy);

private:
	void AcquireItem();

private:
	shared_ptr<CModel> m_pModelCom{ nullptr };
	shared_ptr<CRigidBody> m_pRigidBody{ nullptr };
	weak_ptr<CGameObject> m_pAmitoy;
	_float3 m_vItemScale{};
	_float3 m_vItemPos{};
	float m_fRadius{ 0.0f };
	bool  m_isContactAmitoy{ false };
	bool  m_isAcquired{ false };
	float m_fTimeAcc{ 0.0f };
	_float3 m_vAmitoyPos{};
	string m_strPanelKey{};
};

END