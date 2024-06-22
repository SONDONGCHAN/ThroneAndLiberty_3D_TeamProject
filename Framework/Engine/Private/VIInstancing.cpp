#include "VIInstancing.h"

CVIInstancing::CVIInstancing(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	: Super(_pDevice, _pContext)
{

}

CVIInstancing::CVIInstancing(const shared_ptr<CVIInstancing> _rhs)
	: Super(_rhs),
	m_pVBInstance(_rhs->m_pVBInstance),
	m_iInstanceStride(_rhs->m_iInstanceStride),
	m_iNumInstance(_rhs->m_iNumInstance),
	m_iIndexCountPerInstance(_rhs->m_iIndexCountPerInstance),
	m_RandomNumber(_rhs->m_RandomNumber),
	m_speed(_rhs->m_speed),
	m_lifeTime(_rhs->m_lifeTime)
{

}

CVIInstancing::~CVIInstancing()
{

}

HRESULT CVIInstancing::Initialize()
{
	m_RandomNumber = mt19937_64(m_RandomDevice());

	return S_OK;
}

HRESULT CVIInstancing::InitializeClone(void* _pArg)
{
	m_InstanceData = *(reinterpret_cast<InstanceDesc*>(_pArg));

	m_speed.resize(m_iNumInstance);
	m_lifeTime.resize(m_iNumInstance);

	uniform_real_distribution<float> speedRange(m_InstanceData.vSpeed.x, m_InstanceData.vSpeed.y);
	uniform_real_distribution<float> timeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_speed[i] = speedRange(m_RandomNumber);
		m_lifeTime[i] = timeRange(m_RandomNumber);
	}

	return S_OK;
}

HRESULT CVIInstancing::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIInstancing::BindBuffers()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB.Get(),
		m_pVBInstance.Get()
	};

	_uint iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

void CVIInstancing::SetBufferScale(_float _fSizeX, _float _fSizeY)
{
	D3D11_MAPPED_SUBRESOURCE subResource{};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE* vertices = reinterpret_cast<VTXINSTANCE*>(subResource.pData);

		reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vRight = _float4(_fSizeX, 0.f, 0.f, 0.f);
		reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vUp = _float4(0.f, _fSizeY, 0.f, 0.f);
		m_InstanceData.vSize = _float2(_fSizeX, _fSizeX);
		m_InstanceData.fSizeY = _fSizeY;
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIInstancing::SetInstanceBufferScale(_float _fSizeX, _float _fSizeY)
{
	D3D11_MAPPED_SUBRESOURCE subResource{};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE* vertices = reinterpret_cast<VTXINSTANCE*>(subResource.pData);

		reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vRight = _float4(_fSizeX, 0.f, 0.f, 0.f);
		reinterpret_cast<VTXINSTANCE*>(subResource.pData)[i].vUp = _float4(0.f, _fSizeY, 0.f, 0.f);
		//m_InstanceUIData.vSize = _float2(_fSizeX, _fSizeY);
		m_InstanceData.vSize = _float2(_fSizeX, _fSizeX);
		m_InstanceData.fSizeY = _fSizeY;
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIInstancing::SetColor(_float4 _vColor)
{
	D3D11_MAPPED_SUBRESOURCE subResource{};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE* vertices = reinterpret_cast<VTXINSTANCE*>(subResource.pData);

		vertices[i].vColor = _vColor;

		m_InstanceData.vColor = _vColor;
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIInstancing::SetInstanceColor(_float4 _vColor)
{
	D3D11_MAPPED_SUBRESOURCE subResource{};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE* vertices = reinterpret_cast<VTXINSTANCE*>(subResource.pData);

		vertices[i].vColor = _vColor;

		//m_InstanceUIData.vColor = _vColor;
		m_InstanceData.vColor = _vColor;
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIInstancing::ResetUIAlpha()
{
	D3D11_MAPPED_SUBRESOURCE subResource{};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE* vertices = reinterpret_cast<VTXINSTANCE*>(subResource.pData);

		vertices[i].vColor = m_InstanceData.vColor;
	}

	m_isFinished = false;

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIInstancing::ResetInstanceUIAlpha()
{
	D3D11_MAPPED_SUBRESOURCE subResource{};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE* vertices = reinterpret_cast<VTXINSTANCE*>(subResource.pData);

		//vertices[i].vColor = m_InstanceUIData.vColor;
		vertices[i].vColor = m_InstanceData.vColor;
	}

	m_isFinished = false;

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIInstancing::TickUIAlpha(_float _fDeltaTime)
{

}

void CVIInstancing::TickUIScaling(_float _fDeltaTime)
{
	m_fTrackPosition += _fDeltaTime;
	
	//if (m_InstanceData.fDuration >= m_fTimeAcc)
	//	m_isFinished = true;

	D3D11_MAPPED_SUBRESOURCE subResource{};
	
	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	VTXINSTANCE* vertices = reinterpret_cast<VTXINSTANCE*>(subResource.pData);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		vertices[i].vRight = SimpleMath::Vector4::Lerp(_float4(20.f, 0.f, 0.f, 0.f), _float4(m_InstanceData.vSize.x, 0.f, 0.f, 0.f), m_fTrackPosition);
		vertices[i].vUp = SimpleMath::Vector4::Lerp(_float4(0.f, 5.f, 0.f, 0.f), _float4(0.f, m_InstanceData.fSizeY, 0.f, 0.f), m_fTrackPosition);

		if (vertices[i].vRight.x >= m_InstanceData.vSize.x && vertices[i].vUp.y >= m_InstanceData.fSizeY)
		{
			vertices[i].vRight = _float4(m_InstanceData.vSize.x, 0.f, 0.f, 0.f);
			vertices[i].vUp = _float4(0.f, m_InstanceData.fSizeY, 0.f, 0.f);

			m_isFinished = true;
			m_fTrackPosition = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIInstancing::UIQuickSlotScaling(_float _fDeltaTime, _uint _slotIndex)
{
	m_fTrackPosition += _fDeltaTime;

	D3D11_MAPPED_SUBRESOURCE subResource{};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	VTXINSTANCE* vertices = reinterpret_cast<VTXINSTANCE*>(subResource.pData);

	vertices[_slotIndex].vRight = SimpleMath::Vector4::Lerp(vertices[_slotIndex].vRight, _float4(m_InstanceData.vSize.x, 0.f, 0.f, 0.f), m_fTrackPosition);
	vertices[_slotIndex].vUp = SimpleMath::Vector4::Lerp(vertices[_slotIndex].vUp, _float4(0.f, m_InstanceData.fSizeY, 0.f, 0.f), m_fTrackPosition);

	if (vertices[_slotIndex].vRight.x >= m_InstanceData.vSize.x && vertices[_slotIndex].vUp.y >= m_InstanceData.fSizeY)
	{
		vertices[_slotIndex].vRight = _float4(m_InstanceData.vSize.x, 0.f, 0.f, 0.f);
		vertices[_slotIndex].vUp = _float4(0.f, m_InstanceData.fSizeY, 0.f, 0.f);

		m_isFinished = true;
		m_fTrackPosition = 0.f;
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}
