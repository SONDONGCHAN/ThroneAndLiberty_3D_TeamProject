#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public std::enable_shared_from_this<CComponent>
{
public:
	CComponent(wrl::ComPtr<ID3D11Device> _pDevice , wrl::ComPtr<ID3D11DeviceContext> _pContext);
	CComponent(const shared_ptr<CComponent> _pOrigin);
	virtual ~CComponent() = default;

public:
	virtual shared_ptr<CComponent> Clone(void* _pArg) { return nullptr; }

public:
	virtual HRESULT Initialize() { return S_OK; }
	virtual HRESULT InitializeClone(void* _pArg) { return S_OK; }
	/*For Debug*/
	virtual HRESULT Render() { return S_OK; }

protected: 
	wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

};

END