#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferInstance abstract : public CVIBuffer
{
public:
	struct INSTANCE_DESC
	{

	};

public:
	CVIBufferInstance(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	CVIBufferInstance(const shared_ptr<CVIBufferInstance> _rhs);
	virtual ~CVIBufferInstance();
};

END