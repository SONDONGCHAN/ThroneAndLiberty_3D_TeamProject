#pragma once

#include "VIBuffer.h"

BEGIN(Engine)


class CVIPhysX : public CVIBuffer
{
public:
	struct DESC
	{
		_float3 _vSize{};
		_float3 vOffSetPosition = { 0.0f,0.0f,0.0f };
		_quaternion vOffSetQuat = { 0.0f,0.0f,0.0f,1.0f };
	};

public:
	CVIPhysX(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual ~CVIPhysX() = default;
	
private:
	HRESULT Initialize(CVIPhysX::DESC* _Desc);

public:
	static shared_ptr<CVIPhysX> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, CVIPhysX::DESC* _Desc);

};

END