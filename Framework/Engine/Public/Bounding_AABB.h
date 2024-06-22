#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	struct AABBDesc
	{
		_float3		vCenter;
		_float3		vExtents;
	};
public:
	CBounding_AABB(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual ~CBounding_AABB() = default;

public:
	static shared_ptr<CBounding_AABB> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, void* _pArg);

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(_float4x4& _WorldMatrix) override;
	virtual HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> _pBatch) override;
	virtual bool Intersect(CCollider::ECOL_TYPE _eType, shared_ptr<CBounding> _pBounding) override;

public:
	virtual void* GetBounding() override { return m_pAABB.get(); }

	virtual _float3	Get_Center() override;

	virtual void	Set_Center(_float3 vCenter)override;

private:
	shared_ptr<BoundingBox> m_pOriginalAABB = nullptr;
	shared_ptr<BoundingBox> m_pAABB			= nullptr;
};

END