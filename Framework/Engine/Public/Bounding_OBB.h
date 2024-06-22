#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	struct OBBDesc
	{
		_float3		vCenter;
		_float3		vExtents;
		_float3		vRadians; /* x : x축 기준 회전 각도, y : y축 기준 회전 각도, z : z축 기준 회전 각도 */
	};
public:
	CBounding_OBB(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual ~CBounding_OBB() = default;

public:
	static shared_ptr<CBounding_OBB> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, void* _pArg);

public:
	HRESULT Initialize(void* pArg);

public:
	virtual void Tick(_float4x4& _WorldMatrix) override;
	virtual HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> _pBatch) override;
	virtual bool Intersect(CCollider::ECOL_TYPE _eType, shared_ptr<CBounding> _pBounding) override;

public:
	virtual void* GetBounding() override { return m_pOBB.get(); }

	_float3				Get_Extents();
	virtual _float3		Get_Center() override;

	void				Set_Extents(_float3 vExtents);
	void				Set_Center(_float3 vCenter) override;

private:
	shared_ptr<BoundingOrientedBox> m_pOriginalOBB	= nullptr;
	shared_ptr<BoundingOrientedBox> m_pOBB			= nullptr;
};

END