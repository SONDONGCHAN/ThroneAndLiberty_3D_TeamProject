#pragma once

#include "VIBuffer.h"

BEGIN(Engine)


class CVICube : public CVIBuffer
{
public:
	CVICube(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual ~CVICube() = default;

public:
	virtual HRESULT Initialize() override;

public:
	static shared_ptr<CVICube> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);

};

END