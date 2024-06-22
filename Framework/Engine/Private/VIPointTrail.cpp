#include "VIPointTrail.h"

CVIPointTrail::CVIPointTrail(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIPointTrail::CVIPointTrail(const CVIPointTrail& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIPointTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIPointTrail::Initialize(void* pArg)
{
	m_iPointCntMax = 24;
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumVertices = m_iPointCntMax;
	m_iNumVerticesSpline = (m_iNumVertices - 4) * 10 + 4;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVerticesSpline;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;		//버텍스 버퍼 바인드용
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVerticesSpline];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVerticesSpline);

	for (_uint i = 0; i < m_iNumVerticesSpline; ++i)
	{
		pVertices[i].vPositon = _float3(0.f, 0.f, 0.f);
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::CreateBuffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = (m_iNumVerticesSpline - 2) * 3;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 정적버퍼를 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;				//인덱스 버퍼용
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (_uint iIndex = 0; iIndex < m_iNumVerticesSpline - 2; iIndex += 2)
	{
		pIndices[iNumIndices++] = iIndex + 3;
		pIndices[iNumIndices++] = iIndex + 1;
		pIndices[iNumIndices++] = iIndex;

		pIndices[iNumIndices++] = iIndex + 2;
		pIndices[iNumIndices++] = iIndex + 3;
		pIndices[iNumIndices++] = iIndex;
	}

	ZeroMemory(&m_InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


	return S_OK;
}

HRESULT CVIPointTrail::Update(TRAIL_DESC TrailPoint)
{
	_float3 UpperPos;
	_float3 LowerPos;

	XMStoreFloat3(&UpperPos, XMVector3TransformCoord(XMLoadFloat3(&TrailPoint.vUpOffset), XMLoadFloat4x4(&TrailPoint.vOwnerMatrix)));
	XMStoreFloat3(&LowerPos, XMVector3TransformCoord(XMLoadFloat3(&TrailPoint.vDownOffset), XMLoadFloat4x4(&TrailPoint.vOwnerMatrix)));

	if (!isnan(LowerPos.x))
	{
		m_TrailPoint.push_back(LowerPos);
		m_TrailPoint.push_back(UpperPos);
	}

	while (m_TrailPoint.size() < m_iPointCntMax)
	{
		m_TrailPoint.push_back(LowerPos);
		m_TrailPoint.push_back(UpperPos);
	}
	if (m_TrailPoint.size() > m_iPointCntMax)
	{
		m_TrailPoint.pop_front();
		m_TrailPoint.pop_front();
	}

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	auto iter = m_TrailPoint.begin();

	_float4		vPos[60];

	for (_uint iIndex = 0; iIndex < m_TrailPoint.size(); iIndex += 2)
	{
		vPos[iIndex] = XMVectorSetW(XMLoadFloat3(&(*iter)), 1.f);
		pVertices[iIndex].vPositon = *iter;
		pVertices[iIndex].vTexcoord = _float2((_float)iIndex / (m_TrailPoint.size() - 2), 1.f);

		++iter;

		vPos[iIndex + 1] = XMVectorSetW(XMLoadFloat3(&(*iter)), 1.f);
		pVertices[iIndex + 1].vPositon = (*iter);
		pVertices[iIndex + 1].vTexcoord = _float2((_float)iIndex / (m_TrailPoint.size() - 2), 0.f);

		++iter;
	}

	// Spline
	vector<VTXPOSTEX> pSplineVertices;
	pSplineVertices.reserve(m_iNumVerticesSpline);

	_uint iNumVertices = 0;

	for (_uint i = 0; i < m_iNumVertices; i += 2)
	{
		if (i >= m_iNumVertices - 4)
		{
			pSplineVertices.push_back(pVertices[i]);
			pSplineVertices.push_back(pVertices[i + 1]);
			continue;
		}

		VTXPOSTEX vUp0, vUp1, vUp2, vUp3;
		VTXPOSTEX vDown0, vDown1, vDown2, vDown3;

		if (i == 0)
		{
			vUp0 = pVertices[i];
			vDown0 = pVertices[i + 1];
		}
		else
		{
			vUp0 = pVertices[i - 2];
			vDown0 = pVertices[i - 1];
		}

		vUp1 = pVertices[i];
		vUp2 = pVertices[i + 2];
		vUp3 = pVertices[i + 4];


		vDown1 = pVertices[i + 1];
		vDown2 = pVertices[i + 3];
		vDown3 = pVertices[i + 5];

		for (_uint j = 0; j < 10; ++j)
		{
			VTXPOSTEX VertexDown = {};
			XMStoreFloat3(&VertexDown.vPositon, XMVectorCatmullRom(
				XMLoadFloat3(&vDown0.vPositon)
				, XMLoadFloat3(&vDown1.vPositon)
				, XMLoadFloat3(&vDown2.vPositon)
				, XMLoadFloat3(&vDown3.vPositon)
				, j * 0.1f));

			XMStoreFloat2(&VertexDown.vTexcoord, XMVectorCatmullRom(
				XMLoadFloat2(&vDown0.vTexcoord)
				, XMLoadFloat2(&vDown1.vTexcoord)
				, XMLoadFloat2(&vDown2.vTexcoord)
				, XMLoadFloat2(&vDown3.vTexcoord)
				, j * 0.1f));

			pSplineVertices.push_back(VertexDown);

			VTXPOSTEX VertexUp = {};
			XMStoreFloat3(&VertexUp.vPositon, XMVectorCatmullRom(
				XMLoadFloat3(&vUp0.vPositon)
				, XMLoadFloat3(&vUp1.vPositon)
				, XMLoadFloat3(&vUp2.vPositon)
				, XMLoadFloat3(&vUp3.vPositon)
				, j * 0.1f));

			XMStoreFloat2(&VertexUp.vTexcoord, XMVectorCatmullRom(
				XMLoadFloat2(&vUp0.vTexcoord)
				, XMLoadFloat2(&vUp1.vTexcoord)
				, XMLoadFloat2(&vUp2.vTexcoord)
				, XMLoadFloat2(&vUp3.vTexcoord)
				, j * 0.1f));

			pSplineVertices.push_back(VertexUp);
		}

	}

	D3D11_MAPPED_SUBRESOURCE SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pContext->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, pSplineVertices.data(), sizeof(VTXPOSTEX) * pSplineVertices.size());
	m_pContext->Unmap(m_pVB.Get(), 0);

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CVIPointTrail::Clear()
{
	m_TrailPoint.clear();

	return S_OK;
}

shared_ptr<CVIPointTrail> CVIPointTrail::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CVIPointTrail> pInstance = make_shared<CVIPointTrail>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIPointTrail");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CComponent> CVIPointTrail::Clone(void* pArg)
{
	shared_ptr<CVIPointTrail> pInstance = make_shared<CVIPointTrail>(*this);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CVIPointTrail");
		pInstance.reset();
	}

	return pInstance;
}