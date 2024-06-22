#pragma once

#include "Component.h"

BEGIN(Engine)

class CBounding;

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum ECOL_TYPE { SPHERE_TYPE, AABB_TYPE, OBB_TYPE, TYPE_END };

public:
	CCollider(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	CCollider(const shared_ptr<CCollider> _pOrigin);
	virtual ~CCollider() = default;

public:
	static shared_ptr<CCollider> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, ECOL_TYPE _eType, void* _pArg);
	virtual shared_ptr<CComponent> Clone(void* _pArg) override;

public:
	HRESULT Initialize(ECOL_TYPE _eType, void* _pArg);
	virtual HRESULT InitializeClone(void* pArg) override;

public:
	void Tick(_float4x4 _WorldMatrix);
	bool Intersect(CCollider* pTargetCollider);

public:
	void			Set_Center(_float3 vCenter);
	void			Set_Extents(_float3 vExtents);	// For Box
	void			Set_Radius(_float fRadius);		// For Sphere
	
	_float3			Get_Center();
	_float3			Get_Extents();		// For Box			
	_float			Get_Radius();		// For Sphere

	void			Set_Rotation(_float3 vRotation);


public:
	virtual HRESULT Render() override;

private:
	ECOL_TYPE						m_eType		= TYPE_END;
	shared_ptr<CBounding>			m_pBounding = nullptr;

	shared_ptr<PrimitiveBatch<VertexPositionColor>>		m_pBatch		= nullptr;
	shared_ptr<BasicEffect>								m_pEffect		= nullptr;
	wrl::ComPtr<ID3D11InputLayout>						m_pInputLayout	= nullptr;

private:
	_float3 m_vRotation = {0.f, 0.f, 0.f};
};

END