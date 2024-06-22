#pragma once

#include "DebugDraw.h"
#include "Collider.h"

BEGIN(Engine)

class CBounding abstract
{
public:
	CBounding(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual ~CBounding() = default;

public:
	virtual void Tick(_float4x4& _WorldMatrix) = 0;
	virtual HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> _pBatch) = 0;
	virtual bool Intersect(CCollider::ECOL_TYPE _eType, shared_ptr<CBounding> _pBounding) = 0;

public:
	virtual void* GetBounding() = 0;
	virtual _float3	Get_Center() = 0;

	virtual void	Set_Center(_float3 vCenter) = 0;

protected:
	wrl::ComPtr<ID3D11Device>			m_pDevice	= nullptr;
	wrl::ComPtr<ID3D11DeviceContext>	m_pContext	= nullptr;
	bool								m_isColl	= false;
};

END