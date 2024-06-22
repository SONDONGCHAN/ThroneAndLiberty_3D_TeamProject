#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIPointTrail final : public CVIBuffer
{
public:
	typedef struct
	{
		_float3		vUpOffset;
		_float3		vDownOffset;
		_float4x4	vOwnerMatrix;
	} TRAIL_DESC;

public:
	CVIPointTrail(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
	CVIPointTrail(const CVIPointTrail& rhs);
	virtual ~CVIPointTrail() = default;

private:
	
	// Trail Points
	_int				m_iPointCntMax = 24;	// Number of Maximum Trail point 
	list<_float3>		m_TrailPoint;

	// Spline
	_uint				m_iNumVerticesSpline = 0;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	HRESULT Update(TRAIL_DESC TrailPoint);
	HRESULT Clear();

public:
	static shared_ptr<CVIPointTrail> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg = nullptr);
};

END