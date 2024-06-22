#pragma once

#include "VIInstancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIInstancePoint final : public CVIInstancing
{
	using Super = CVIInstancing;

public:
	CVIInstancePoint(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	CVIInstancePoint(const shared_ptr<CVIInstancing> _rhs);
	virtual ~CVIInstancePoint();

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT InitializeClone(_uint _iNumInstance, void* _pArg);

public:
	static shared_ptr<CVIInstancePoint> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual shared_ptr<CComponent> Clone(_uint _iNumInstance, void* _pArg);

};

END
