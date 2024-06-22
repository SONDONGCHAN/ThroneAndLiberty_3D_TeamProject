#include "PipeLine.h"
#include "GameInstance.h"

CPipeLine::CPipeLine()
{
}

void CPipeLine::Tick()
{
	for (UINT i = 0; i < D3DTS_END; i++)
	{
		m_TransformMatrix_Inverse[i] = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[i]));
	}

	memcpy(&m_vCamPosition, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[3][0], sizeof(_float4));
	memcpy(&m_vCamLook, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[2][0], sizeof(_float3));
}

void CPipeLine::CascadeTransform()
{
}

shared_ptr<CPipeLine> CPipeLine::Create()
{
    return make_shared<CPipeLine>();
}
