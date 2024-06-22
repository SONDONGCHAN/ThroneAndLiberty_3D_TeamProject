#include "Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
{
}

CFrustum::~CFrustum()
{
}

shared_ptr<CFrustum> CFrustum::Create()
{
    shared_ptr<CFrustum> pInstance = make_shared<CFrustum>();

    if (FAILED(pInstance->Initialize()))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CFrustum::Create", MB_OK);
        pInstance.reset();
        return nullptr;
    }

    return pInstance;
}

HRESULT CFrustum::Initialize()
{
	/* 투영스페이스 상의 여덟개 꼭지점을 정의한다. */
	m_vOriginalPoints[0] = { -1.f,  1.f, 0.f };
	m_vOriginalPoints[1] = {  1.f,  1.f, 0.f };
	m_vOriginalPoints[2] = {  1.f, -1.f, 0.f };
	m_vOriginalPoints[3] = { -1.f, -1.f, 0.f };

	m_vOriginalPoints[4] = { -1.f,  1.f, 1.f };
	m_vOriginalPoints[5] = {  1.f,  1.f, 1.f };
	m_vOriginalPoints[6] = {  1.f, -1.f, 1.f };
	m_vOriginalPoints[7] = { -1.f, -1.f, 1.f };

	return S_OK;
}

void CFrustum::Tick()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
	XMMATRIX ProjMatInv = pGameInstance->GetTransformMatrixInverse(CPipeLine::D3DTS_PROJ);
	XMMATRIX ViewMatInv = pGameInstance->GetTransformMatrixInverse(CPipeLine::D3DTS_VIEW);

	for (UINT i = 0; i < 8; i++)
	{
		XMStoreFloat3(&m_vPoints[i], XMVector3TransformCoord(XMVector3TransformCoord(XMLoadFloat3(&m_vOriginalPoints[i]), ProjMatInv), ViewMatInv));
	}

	MakePlanes(m_vPoints, m_WorldPlanes);
}

ContainmentType CFrustum::isInWorldSpace(const BoundingBox& _BoundingBox)
{
	return _BoundingBox.ContainedBy(XMLoadFloat4(&m_WorldPlanes[5]), XMLoadFloat4(&m_WorldPlanes[4]), XMLoadFloat4(&m_WorldPlanes[0]), XMLoadFloat4(&m_WorldPlanes[1]), XMLoadFloat4(&m_WorldPlanes[2]), XMLoadFloat4(&m_WorldPlanes[3]));
}

bool CFrustum::isInWorldSpace(FXMVECTOR _vPosition, const float& _fRadius)
{
	for (UINT i = 0; i < 6; i++)
	{
		if (_fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_WorldPlanes[i]), _vPosition)))
			return false;
	}

	return true;
}

void CFrustum::MakeLocalSpace(FXMMATRIX _WorldMatrix)
{
	XMMATRIX WorldMatInv = XMMatrixInverse(nullptr, _WorldMatrix);

	XMFLOAT3 LocalPoints[8]{};

	for (UINT i = 0; i < 8; i++)
	{
		XMStoreFloat3(&LocalPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), WorldMatInv));
	}

	MakePlanes(LocalPoints, m_LocalPlanes);
}

bool CFrustum::isInLocalSpace(FXMMATRIX _WorldMatrix, FXMVECTOR _vPosition, const float& _fRadius)
{
	for (UINT i = 0; i < 6; i++)
	{
		if (_fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_LocalPlanes[i]), _vPosition)))
			return false;
	}

	return true;
}

void CFrustum::MakePlanes(const XMFLOAT3* _pPoints, XMFLOAT4* _pPlanes)
{
	XMStoreFloat4(&_pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&_pPoints[1]), XMLoadFloat3(&_pPoints[5]), XMLoadFloat3(&_pPoints[6])));
	XMStoreFloat4(&_pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&_pPoints[4]), XMLoadFloat3(&_pPoints[0]), XMLoadFloat3(&_pPoints[3])));
	XMStoreFloat4(&_pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&_pPoints[4]), XMLoadFloat3(&_pPoints[5]), XMLoadFloat3(&_pPoints[1])));
	XMStoreFloat4(&_pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&_pPoints[3]), XMLoadFloat3(&_pPoints[2]), XMLoadFloat3(&_pPoints[6])));
	XMStoreFloat4(&_pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&_pPoints[5]), XMLoadFloat3(&_pPoints[4]), XMLoadFloat3(&_pPoints[7])));
	XMStoreFloat4(&_pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&_pPoints[0]), XMLoadFloat3(&_pPoints[1]), XMLoadFloat3(&_pPoints[2])));
}
