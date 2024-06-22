#include "VIInstanceParticle.h"

CVIInstanceParticle::CVIInstanceParticle(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	: CVIInstancing(_pDevice, _pContext)
{

}

CVIInstanceParticle::CVIInstanceParticle(const shared_ptr<CVIInstancing> _rhs)
	: CVIInstancing(_rhs)
{

}

CVIInstanceParticle::~CVIInstanceParticle()
{

}

HRESULT CVIInstanceParticle::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;

#pragma region VERTEX_BUFFER
	/* 정점버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);
	pVertices[0].vPSize = _float2(1.f, 1.f);

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

	return S_OK;
}

HRESULT CVIInstanceParticle::InitializeClone(void* pArg)
{
	memcpy(&m_iNumInstance, (_int*)pArg, sizeof(_int));

	//ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_iIndexCountPerInstance = 1;
	m_iIndexStride = sizeof(_ushort);
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;

	//ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

#pragma region INDEX_BUFFER
	/* 인덱스버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	//ZeroMemory(&m_InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER
	//ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_iInstanceStride = sizeof(VTXPARTICLE);

	/* 정점버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPARTICLE* pInstances = new VTXPARTICLE[m_iNumInstance];
	ZeroMemory(pInstances, sizeof(VTXPARTICLE) * m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pInstances[i].vRight = _float4(0.5f, 0.f, 0.f, 0.f);
		pInstances[i].vUp = _float4(0.f, 0.5f, 0.f, 0.f);
		pInstances[i].vLook = _float4(0.f, 0.f, 0.5f, 0.f);
		pInstances[i].vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

		pInstances[i].vColor = _float4(1.f, 1.f, 1.f, 1.f);
		pInstances[i].vPivot = _float4(0.f, 0.f, 0.f, 0.f);
	}
	
	//ZeroMemory(&m_InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_InitialData.pSysMem = pInstances;

	if (FAILED(__super::CreateBuffer(&m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstances);

#pragma endregion

	return S_OK;
}

HRESULT CVIInstanceParticle::Update(PARTICLE* _pParticles, _int _iParticleCnt)
{
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;
	//ZeroMemory(&MappedSubResource, sizeof MappedSubResource);

	m_iNumInstance = _iParticleCnt;
	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	for (_uint i = 0; i < _iParticleCnt; ++i)
	{
		if (_pParticles[i].bActive)
		{
			VTXPARTICLE* pInstances = static_cast<VTXPARTICLE*>(MappedSubResource.pData);

			_float3 vScaleRight = XMVector3Normalize(_float3(0.5f, 0.f, 0.f)) * _pParticles[i].vCurScale.x;
			_float3 vScaleUp = XMVector3Normalize(_float3(0.f, 0.5f, 0.f)) * _pParticles[i].vCurScale.y;
			_float3 vScaleLook = XMVector3Normalize(_float3(0.f, 0.f, 0.5f)) * _pParticles[i].vCurScale.z;

			XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(
				_pParticles[i].vRotation.x,
				_pParticles[i].vRotation.y,
				_pParticles[i].vRotation.z
			);

			XMVECTOR vScaleRightVec = XMLoadFloat3(&vScaleRight);
			XMVECTOR vScaleUpVec = XMLoadFloat3(&vScaleUp);
			XMVECTOR vScaleLookVec = XMLoadFloat3(&vScaleLook);

			vScaleRightVec = XMVector3TransformNormal(vScaleRightVec, rotMatrix);
			vScaleUpVec = XMVector3TransformNormal(vScaleUpVec, rotMatrix);
			vScaleLookVec = XMVector3TransformNormal(vScaleLookVec, rotMatrix);

			XMStoreFloat4(&((VTXPARTICLE*)MappedSubResource.pData)[i].vRight, _float4(vScaleRightVec.m128_f32[0], vScaleRightVec.m128_f32[1], vScaleRightVec.m128_f32[2], 0.f));
			XMStoreFloat4(&((VTXPARTICLE*)MappedSubResource.pData)[i].vUp, _float4(vScaleUpVec.m128_f32[0], vScaleUpVec.m128_f32[1], vScaleUpVec.m128_f32[2], 0.f));
			XMStoreFloat4(&((VTXPARTICLE*)MappedSubResource.pData)[i].vLook, _float4(vScaleLookVec.m128_f32[0], vScaleLookVec.m128_f32[1], vScaleLookVec.m128_f32[2], 0.f));

			/*XMStoreFloat4(&((VTXPARTICLE*)MappedSubResource.pData)[i].vRight, _float4(vScaleRight, vScaleRight.y, vScaleRight.z, 0.f));
			XMStoreFloat4(&((VTXPARTICLE*)MappedSubResource.pData)[i].vUp, _float4(vScaleUp.x, vScaleUp.y, vScaleUp.z, 0.f));
			XMStoreFloat4(&((VTXPARTICLE*)MappedSubResource.pData)[i].vLook, _float4(vScaleLook.x, vScaleLook.y, vScaleLook.z, 0.f));*/

			((VTXPARTICLE*)MappedSubResource.pData)[i].vTranslation = _float4(_pParticles[i].vPosition.x, _pParticles[i].vPosition.y, _pParticles[i].vPosition.z, 1.f);
			((VTXPARTICLE*)MappedSubResource.pData)[i].vColor = _float4(_pParticles[i].vCurColor.x, _pParticles[i].vCurColor.y, _pParticles[i].vCurColor.z, _pParticles[i].vCurColor.w);

			((VTXPARTICLE*)MappedSubResource.pData)[i].vColor.w = (_pParticles[i].fLifeTime / _pParticles[i].fStartLifeTime) * 3.f;
			((VTXPARTICLE*)MappedSubResource.pData)[i].vPivot = _pParticles[i].vPivot;
		}
		else
		{
			((VTXPARTICLE*)MappedSubResource.pData)[i].vColor.w = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);

	return S_OK;
}

shared_ptr<CVIInstanceParticle> CVIInstanceParticle::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
{
	shared_ptr<CVIInstanceParticle> pInstance = make_shared<CVIInstanceParticle>(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CVIInstanceParticle::Create", MB_OK);

		pInstance.reset();

		return nullptr;
	}

	return pInstance;
}

shared_ptr<CComponent> CVIInstanceParticle::Clone(void* pArg)
{
	shared_ptr<CVIInstanceParticle> pClone = make_shared<CVIInstanceParticle>(static_pointer_cast<CVIInstanceParticle>(shared_from_this()));

	if (FAILED(pClone->InitializeClone(pArg)))
	{
		MessageBox(nullptr, L"InitializeClone Failed", L"CVIInstanceParticle::Clone", MB_OK);

		pClone.reset();

		return nullptr;
	}

	return pClone;
}
