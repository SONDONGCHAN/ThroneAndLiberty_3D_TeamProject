#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer : public CComponent
{
public:
	CVIBuffer(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	CVIBuffer(const shared_ptr<CVIBuffer> _rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Render();

public:
	HRESULT BindBuffers();
	vector<_float3>* GetVerticesPos() { return &m_verticesPos; }
	_uint GetNumIndices() { return m_iNumIndices; }

protected:
	wrl::ComPtr<ID3D11Buffer> m_pVB = { nullptr };
	wrl::ComPtr<ID3D11Buffer> m_pIB = { nullptr };


protected:
	D3D11_BUFFER_DESC m_BufferDesc = {};
	D3D11_SUBRESOURCE_DATA	m_InitialData = {};
	_uint m_iVertexStride = { 0 };
	_uint m_iNumVertices = { 0 };
	_uint m_iIndexStride = { 0 };
	_uint m_iNumIndices = { 0 };
	_uint m_iNumVertexBuffers = { 0 };
	DXGI_FORMAT m_eIndexFormat = {};
	D3D11_PRIMITIVE_TOPOLOGY m_ePrimitiveTopology = {};

protected:
	/*Vertices¿« Local Position¿ª ¿˙¿Â«ÿµ– ∫§≈Õ*/
	vector<_float3> m_verticesPos;
protected:
	HRESULT CreateBuffer(_Out_ wrl::ComPtr<ID3D11Buffer>* ppBuffer);

};

END