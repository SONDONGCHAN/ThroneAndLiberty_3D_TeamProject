#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIPoint final : public CVIBuffer
{
public:
	CVIPoint(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual ~CVIPoint() = default;

public:
	virtual HRESULT Initialize() override;

public:
	static shared_ptr<CVIPoint> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);

};

END