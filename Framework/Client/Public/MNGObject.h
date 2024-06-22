#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMNGObject final : public CGameObject
{
public:
	CMNGObject();
	virtual ~CMNGObject();

public:
	static shared_ptr<CMNGObject> Create(const string& _strModelTag, const _float4x4& _WorldMatrix, Geometry* _pGeometry, float _fRadius, bool _isTrap = false);

public:
	HRESULT Initialize(const string& _strModelTag, const _float4x4& _WorldMatrix, Geometry* _pGeometry, float _fRadius, bool _isTrap);
	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	virtual void OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag) override;
	
public:
	void SpawnObject();
	void DespawnObject();

public:
	virtual HRESULT RenderEdgeDepth() override;
	virtual HRESULT RenderEdge() override;

private:
	shared_ptr<CModel> m_pModelCom{ nullptr };
	shared_ptr<CRigidBody> m_pRigidBody{ nullptr };
	float m_fRadius{ 0.0f };
	bool m_isTrap{ false };
	bool m_isDissolving{ false };
	UINT m_iAttackedCnt{ 0 };

	bool m_isAttackedbyAmitoy{ false };
	_float3 m_AttakedPos{};
	bool m_AttackedbyBomb{ false };
	_float3 m_AttackedBombPos{};
	bool m_isGrabed{ false };
	bool m_isGravityChange{ false };
	weak_ptr<CGameObject> m_pAmitoy;
};

END