#pragma once

#include "VIInstancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIInstanceUI final : public CVIInstancing
{
	using Super = CVIInstancing;

public:
	CVIInstanceUI(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	CVIInstanceUI(const shared_ptr<CVIInstancing> _rhs);
	virtual ~CVIInstanceUI();

public:
	virtual HRESULT Initialize(CVIInstancing::InstanceDesc& _instanceUI, vector<_float2>& _uiPos);
	virtual HRESULT InitializeClone(_uint _iNumInstance, void* _pArg);

public:
	//static shared_ptr<CVIInstanceUI> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, InstanceUI& _instanceUI, vector<_float2>& _uiPos);
	static shared_ptr<CVIInstanceUI> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext, CVIInstancing::InstanceDesc& _instanceUI, vector<_float2>& _uiPos);
	virtual shared_ptr<CComponent> Clone(_uint _iNumInstance, void* _pArg);

};

END
