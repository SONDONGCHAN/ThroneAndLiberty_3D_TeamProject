#include <limits>

#include "PickingMgr.h"
#include "GameInstance.h"

#include "GameObject.h"
#include "VITerrain.h"


CPickingMgr::CPickingMgr()
{
}

void CPickingMgr::GetWorldRay(_float3& _vRayPos, _float3& _vRayDir)
{
	_vRayPos = m_vWorldPosRay;
	_vRayDir = m_vWorldDirRay;
}

HRESULT CPickingMgr::Initialize(const GRAPHIC_DESC& _GraphicDesc)
{
	m_hWnd = _GraphicDesc.hWnd;

	m_fClientWidth = static_cast<_float>(_GraphicDesc.iWinSizeX);
	m_fClientHeight = static_cast<_float>(_GraphicDesc.iWinSizeY);

	// 픽셀 피킹용 텍스쳐 생성. (픽셀 피킹 랜더타겟을 복사할 것!)
	D3D11_VIEWPORT ViewPortDesc{};

	_uint iNumViewPorts = 1;

	GAMEINSTANCE->GetDeviceContextInfo()->RSGetViewports(&iNumViewPorts, &ViewPortDesc);

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 1;  //ViewPortDesc.Width; // 1
	TextureDesc.Height = 1; //ViewPortDesc.Height; // 1
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_STAGING; // GPU->CPU 복사
	TextureDesc.BindFlags = 0;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	TextureDesc.MiscFlags = 0;
	
	if (FAILED(GAMEINSTANCE->GetDeviceInfo()->CreateTexture2D(&TextureDesc, nullptr, m_pTexture2D.GetAddressOf())))
		return E_FAIL;

	m_pTargetTexture2D = GAMEINSTANCE->GetTexture2DForTarget(TEXT("Target_PixelPicking"));
	if (nullptr == m_pTargetTexture2D)
		return E_FAIL;

	return S_OK;
}

void CPickingMgr::Tick(_float _fTimeDelta)
{
	TransformViewportToWorld();
}

// 전체 순회용 코드!!
HRESULT CPickingMgr::ComputePicking(_uint _iType)
{
	if (TYPE_MESH == _iType)
		return IntersectTerrain();

	if (TYPE_PIXEL == _iType)
		return IntersectPixel();

	return S_OK;
}

void CPickingMgr::TransformViewportToWorld()
{
	POINT ptMouse{};
	// 마우스 스크린 좌표를 가져오기
	::GetCursorPos(&ptMouse);
	::ScreenToClient(m_hWnd, &ptMouse);

	// 화면의 마우스 좌표 저장
	_float fScreenX = static_cast<_float>(ptMouse.x);
	_float fScreenY = static_cast<_float>(ptMouse.y);

	// 엔진 - 파이프라인에서 투영행렬 가져온 뒤 저장
	_float4x4 matProj = GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ);

	// 뷰포트의 마우스 좌표를 뷰 포트 -> 투영 스페이스 -> 뷰 스페이스로 내림.
	_float fViewX = ((+2.f * fScreenX) / m_fClientWidth - 1.f) / matProj._11;
	_float fViewY = ((-2.f * fScreenY) / m_fClientHeight + 1.f) / matProj._22;

	// View Ray의 초기 셋팅.
	_float3 vRayPos(0.f, 0.f, 0.f);
	_float3 vRayDir(fViewX, fViewY, 1.f);

	// Engine의 PipeLine이 갖고 있는 View 역행렬을 반환하여 저장.
	_float4x4 matInvView = GAMEINSTANCE->GetTransformMatrixInverse(CPipeLine::D3DTS_VIEW);

	m_vWorldPosRay = SimpleMath::Vector3::Transform(vRayPos, matInvView);
	m_vWorldDirRay = SimpleMath::Vector3::TransformNormal(vRayDir, matInvView);
	m_vWorldDirRay.Normalize();
}

HRESULT CPickingMgr::IntersectRatToTriangle(vector<VTXNORTEX>& _vecVertices, vector<_uint>& _vecIndices, _float3 _vRayPos, _float3 _vRayDir, _float3& _vIntersectedPos)
{
	_vRayDir.Normalize();

	_uint iIndicesSize = static_cast<_uint>((_vecIndices).size());

	// Check Intersect
	_bool bIsIntersected{};
	_float fDist{};
	_float fShortDist = numeric_limits<_float>::infinity();

	array<_uint, INDEX_END> arrIndicesPickedVtx{};

	for (_uint iCntIdx = 0; iCntIdx < iIndicesSize;)
	{
		_uint iIdxA = iCntIdx++;
		_uint iIdxB = iCntIdx++;
		_uint iIdxC = iCntIdx++;

		bIsIntersected = DirectX::TriangleTests::Intersects
		(
			_vRayPos,
			_vRayDir,
			_vecVertices[(_vecIndices)[iIdxA]].vPosition,
			_vecVertices[(_vecIndices)[iIdxB]].vPosition,
			_vecVertices[(_vecIndices)[iIdxC]].vPosition,
			fDist
		);

		if (true == bIsIntersected && fShortDist > fDist)
		{
			fShortDist = fDist;
			arrIndicesPickedVtx[INDEX_A] = (_vecIndices)[iIdxA];
			arrIndicesPickedVtx[INDEX_B] = (_vecIndices)[iIdxB];
			arrIndicesPickedVtx[INDEX_C] = (_vecIndices)[iIdxC];
		}
	}

	// Intersect Position
	if (numeric_limits<_float>::infinity() != fShortDist)
	{
		// Local Pos
		_vRayDir.Normalize();
		_vIntersectedPos = (_vRayDir * fShortDist) + _vRayPos;

		// 버텍스 영역에서 픽된 좌표와 가장 가까운 정점 구하기
		_float fShortDistance{ numeric_limits<_float>::infinity() };
		for (_uint i = 0; i < INDEX_END; i++)
		{
			_float fDistance = (_vIntersectedPos - _vecVertices[arrIndicesPickedVtx[i]].vPosition).Length();

			if (fShortDistance > fDistance)
			{
				fShortDistance = fDistance;
				m_iIdxPickedVertex = arrIndicesPickedVtx[i];
			}
		}

	}
	else 
		return E_FAIL;

	return S_OK;
}

HRESULT CPickingMgr::IntersectTerrain()
{
	
	// 1. 오브젝트 매니져로부터 현재 터레인 정보를 가지고온다.
	shared_ptr<CGameObject> pCurTerrain = GAMEINSTANCE->GetGameObject(GAMEINSTANCE->GetCurrentLevel(), TEXT("Layer_Terrain"));
	_float4x4 matWorld = pCurTerrain->GetTransform()->GetWorldMatrix();
	_float4x4 matInvWorld = pCurTerrain->GetTransform()->GetWorldMatrixInverse();

	shared_ptr<CVITerrain> m_pCurVIBuffer = static_pointer_cast<CVITerrain>(pCurTerrain->GetComponent(TEXT("Com_VIBuffer")));
	vector<VTXNORTEX> vecVertices = m_pCurVIBuffer->GetVertices();

	_float3 vRayPos = SimpleMath::Vector3::Transform(m_vWorldPosRay, matInvWorld);
	_float3 vRayDir = SimpleMath::Vector3::TransformNormal(m_vWorldDirRay, matInvWorld);
	//vRayDir.Normalize();

	_float2 vRay2DPos{ vRayPos.x, vRayPos.z };
	_float2 vRay2DDir{ vRayDir.x, vRayDir.z };
	//vRay2DDir.Normalize();

	_float _fRaySlope{ vRay2DDir.y / vRay2DDir.x };
	_float _fRayIntercept{ vRay2DPos.y - (_fRaySlope * vRay2DPos.x) };

	vector<_uint> vecIndices{};

	m_pCurVIBuffer->Search(vecIndices, vRay2DPos, vRay2DDir, _fRaySlope, _fRayIntercept);

	_float3 vIntersectedPos{};

	if (FAILED(IntersectRatToTriangle(vecVertices, vecIndices, vRayPos, vRayDir, vIntersectedPos)))
		return E_FAIL;

	m_vWorldPosPicked = SimpleMath::Vector3::Transform(vIntersectedPos, matWorld);
	
	return S_OK;
}

HRESULT CPickingMgr::IntersectPixel()
{
	POINT ptMouse{};
	// 마우스 스크린 좌표를 가져오기
	::GetCursorPos(&ptMouse);
	::ScreenToClient(m_hWnd, &ptMouse);

	if (!(0 <= ptMouse.x && m_fClientWidth > ptMouse.x))
		return E_FAIL;
	if (!(0 <= ptMouse.y && m_fClientHeight > ptMouse.y))
		return E_FAIL;

	_uint iLeft = static_cast<_uint>(ptMouse.x);
	_uint iTop = static_cast<_uint>(ptMouse.y);

	D3D11_BOX srcBox{ iLeft, iTop, 0, iLeft + 1, iTop + 1, 1 };

	// 1. 1픽셀만큼 복사한다.
	GAMEINSTANCE->GetDeviceContextInfo()->CopySubresourceRegion(m_pTexture2D.Get(), 0, 0, 0, 0, m_pTargetTexture2D.Get(), 0, &srcBox);
	
	// 2. m_pTexture2D이 친구한테 1픽셀만큼 복사가 된거다.
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	GAMEINSTANCE->GetDeviceContextInfo()->Map(m_pTexture2D.Get(), 0, D3D11_MAP_READ, 0, &SubResource);

	_float4* pObjInfo = reinterpret_cast<_float4*>(SubResource.pData);

	m_iPickedObjectID = static_cast<_uint>(round(pObjInfo->w));
	m_vWorldPosPicked = SimpleMath::Vector3(pObjInfo->x, pObjInfo->y, pObjInfo->z);

	GAMEINSTANCE->GetDeviceContextInfo()->Unmap(m_pTexture2D.Get(), 0);  

	return S_OK;
}

shared_ptr<CPickingMgr> CPickingMgr::Create(const GRAPHIC_DESC& _GraphicDesc)
{
	shared_ptr<CPickingMgr> pInstance = make_shared<CPickingMgr>();

	if (FAILED(pInstance->Initialize(_GraphicDesc)))
	{
		MSG_BOX("Failed to Created : CPickingMgr");
		pInstance.reset();
	}

	return pInstance;
}