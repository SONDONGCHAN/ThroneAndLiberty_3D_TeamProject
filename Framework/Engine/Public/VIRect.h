#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIRect final : public CVIBuffer
{
public:
	CVIRect(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	CVIRect(const shared_ptr<CVIRect> _rhs);
	virtual ~CVIRect() = default;

public:
	virtual HRESULT Initialize() override;

public:
	static shared_ptr<CVIRect> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);

};

END
