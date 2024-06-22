#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	struct SphereDesc
	{
		_float3		vCenter;
		_float		fRadius;
	};

public:
	CBounding_Sphere(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual ~CBounding_Sphere() = default;

public:
	static shared_ptr<CBounding_Sphere> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, void* _pArg);

public:
	HRESULT Initialize(void* _pArg);

public:
	virtual void Tick(_float4x4& _WorldMatrix) override;
	virtual HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> _pBatch) override;
	bool Intersect(CCollider::ECOL_TYPE _eType, shared_ptr<CBounding> _pBounding);

public:
	virtual void* GetBounding() override { return m_pSphere.get(); }

	_float				Get_Radius();
	virtual _float3		Get_Center() override;

	void				Set_Radius(_float fRadius);
	void				Set_Center(_float3 vCenter) override;

private:
	shared_ptr<BoundingSphere> m_pOriginalSphere	= nullptr;
	shared_ptr<BoundingSphere> m_pSphere			= nullptr;
};

END