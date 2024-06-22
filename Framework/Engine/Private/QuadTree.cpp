#include "QuadTree.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Collider.h"

#include "VIBuffer.h"

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Initialize(shared_ptr<CVIBuffer> _pVIBuffer, _uint _iLT, _uint _iRT, _uint _iRB, _uint _iLB)
{
	//m_pVIBuffer = _pVIBuffer;
	m_pVerticesPos = _pVIBuffer->GetVerticesPos();
	//vector<_float3>& vecVerticesPos = *(_pVIBuffer->GetVerticesPos());

	m_arrCorners[CORNER_LT] = _iLT;
	m_arrCorners[CORNER_RT] = _iRT;
	m_arrCorners[CORNER_RB] = _iRB;
	m_arrCorners[CORNER_LB] = _iLB;

	// 

	//_float fCenterX = (vecVerticesPos[_iLT].x + vecVerticesPos[_iRT].x) * 0.5f;
	//_float fCenterY = (vecVerticesPos[_iLT].y + vecVerticesPos[_iRB].y) * 0.5f;

	//_float fSizeX = abs(vecVerticesPos[_iLT].x - vecVerticesPos[_iRT].x) * 0.5f;
	//_float fSizeY = abs(vecVerticesPos[_iLT].y - vecVerticesPos[_iRB].y) * 0.5f;

	//CBounding_AABB::AABBDesc BoundingDesc{};
	//BoundingDesc.vCenter = _float3(fCenterX, 0.5f, fCenterY);
	//BoundingDesc.vExtents = _float3(fSizeX, 0.5f, fSizeY);

	//m_pCollider = CCollider::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), CCollider::AABB_TYPE, &BoundingDesc);

	//_uint a = m_arrCorners[CORNER_RT] - m_arrCorners[CORNER_LT]; 

	_uint a = m_arrCorners[CORNER_RT] - m_arrCorners[CORNER_LT];

	if (1 == m_arrCorners[CORNER_RT] - m_arrCorners[CORNER_LT])
	{
		m_bIsLeaf = true;
		return S_OK;
	}

	m_arrCenters[CENTER_CC] = (_iLT + _iRB) >> 1;
	//m_iCenter = (_iLT + _iRB) >> 1;

	_uint		iLC, iTC, iRC, iBC;

	iLC = m_arrCenters[CENTER_LC] = (m_arrCorners[CORNER_LT] + m_arrCorners[CORNER_LB]) >> 1;
	iTC = m_arrCenters[CENTER_TC] = (m_arrCorners[CORNER_LT] + m_arrCorners[CORNER_RT]) >> 1;
	iRC = m_arrCenters[CENTER_RC] = (m_arrCorners[CORNER_RT] + m_arrCorners[CORNER_RB]) >> 1;
	iBC = m_arrCenters[CENTER_BC] = (m_arrCorners[CORNER_LB] + m_arrCorners[CORNER_RB]) >> 1;



	//iLC = (m_arrCorners[CORNER_LT] + m_arrCorners[CORNER_LB]) >> 1;
	//iTC = (m_arrCorners[CORNER_LT] + m_arrCorners[CORNER_RT]) >> 1;
	//iRC = (m_arrCorners[CORNER_RT] + m_arrCorners[CORNER_RB]) >> 1;
	//iBC = (m_arrCorners[CORNER_LB] + m_arrCorners[CORNER_RB]) >> 1;

	// 정확하지는 않지만 일단 패스.
	m_iCenter = (_iLT + _iRB) >> 1;

	m_arrChilds[CORNER_LT] = CQuadTree::Create(_pVIBuffer, m_arrCorners[CORNER_LT], m_arrCenters[CENTER_TC], m_arrCenters[CENTER_CC], m_arrCenters[CENTER_LC]);
	m_arrChilds[CORNER_RT] = CQuadTree::Create(_pVIBuffer, m_arrCenters[CENTER_TC], m_arrCorners[CORNER_RT], m_arrCenters[CENTER_RC], m_arrCenters[CENTER_CC]);
	m_arrChilds[CORNER_RB] = CQuadTree::Create(_pVIBuffer, m_arrCenters[CENTER_CC], m_arrCenters[CENTER_RC], m_arrCorners[CORNER_RB], m_arrCenters[CENTER_BC]);
	m_arrChilds[CORNER_LB] = CQuadTree::Create(_pVIBuffer, m_arrCenters[CENTER_LC], m_arrCenters[CENTER_CC], m_arrCenters[CENTER_BC], m_arrCorners[CORNER_LB]);

	return S_OK;
}

void CQuadTree::Search(vector<_uint>& _vecIndices, _float2& _vRayPos, _float2& _vRayDir, _float& _fRaySlope, _float& _fRayIntercept)
//void CQuadTree::Search(vector<_uint>& _vecIndices, _float& _fRaySlope, _float& _fRayIntercept)
{
	if (true == m_bIsLeaf)
	{
		_vecIndices.push_back(m_arrCorners[CORNER_LT]);
		_vecIndices.push_back(m_arrCorners[CORNER_RT]);
		_vecIndices.push_back(m_arrCorners[CORNER_RB]);

		_vecIndices.push_back(m_arrCorners[CORNER_LT]);
		_vecIndices.push_back(m_arrCorners[CORNER_RB]);
		_vecIndices.push_back(m_arrCorners[CORNER_LB]);
		return;
	}


	_float2 vCenterPos{ (*m_pVerticesPos)[m_arrCenters[CENTER_CC]].x, (*m_pVerticesPos)[m_arrCenters[CENTER_CC]].z };
	_float2 vToCenterDir = vCenterPos - _vRayPos;

	if (vToCenterDir.Dot(_vRayPos) > 0)
	{
		for (auto& bIsIntersected : m_vecIsIntersected)
			bIsIntersected = true;
	}
	else
	{
		// Line LeftCenter->Center
		if (true == ComputeLineIntersect(_fRaySlope, _fRayIntercept, (*m_pVerticesPos)[m_arrCenters[CENTER_LC]], (*m_pVerticesPos)[m_arrCenters[CENTER_CC]]))
			m_vecIsIntersected[CORNER_LB] = m_vecIsIntersected[CORNER_LT] = true;

		// Line Center->TopCenter
		if (true == ComputeLineIntersect(_fRaySlope, _fRayIntercept, (*m_pVerticesPos)[m_arrCenters[CENTER_CC]], (*m_pVerticesPos)[m_arrCenters[CENTER_TC]]))
			m_vecIsIntersected[CORNER_LT] = m_vecIsIntersected[CORNER_RT] = true;

		// If. LT == flase
		if (false == m_vecIsIntersected[CORNER_LT])
			if (true == ComputeLineIntersect(_fRaySlope, _fRayIntercept, (*m_pVerticesPos)[m_arrCenters[CENTER_LC]], (*m_pVerticesPos)[m_arrCorners[CORNER_LT]]))
				m_vecIsIntersected[CORNER_LT] = true;

		// If. LT == flase
		if (false == m_vecIsIntersected[CORNER_LT])
			if (true == ComputeLineIntersect(_fRaySlope, _fRayIntercept, (*m_pVerticesPos)[m_arrCorners[CORNER_LT]], (*m_pVerticesPos)[m_arrCenters[CENTER_LC]]))
				m_vecIsIntersected[CORNER_LT] = true;





		// Line Center->RightCenter
		if (true == ComputeLineIntersect(_fRaySlope, _fRayIntercept, (*m_pVerticesPos)[m_arrCenters[CENTER_CC]], (*m_pVerticesPos)[m_arrCenters[CENTER_RC]]))
			m_vecIsIntersected[CORNER_RB] = m_vecIsIntersected[CORNER_RT] = true;

		// If. RT == flase
		if (false == m_vecIsIntersected[CORNER_RT])
			if (true == ComputeLineIntersect(_fRaySlope, _fRayIntercept, (*m_pVerticesPos)[m_arrCenters[CENTER_TC]], (*m_pVerticesPos)[m_arrCorners[CORNER_RT]]))
				m_vecIsIntersected[CORNER_RT] = true;

		// If. RT == flase
		if (false == m_vecIsIntersected[CORNER_RT])
			if (true == ComputeLineIntersect(_fRaySlope, _fRayIntercept, (*m_pVerticesPos)[m_arrCenters[CENTER_RC]], (*m_pVerticesPos)[m_arrCorners[CORNER_RT]]))
				m_vecIsIntersected[CORNER_RT] = true;





		// Line BottomCenter->Center
		if (true == ComputeLineIntersect(_fRaySlope, _fRayIntercept, (*m_pVerticesPos)[m_arrCenters[CENTER_BC]], (*m_pVerticesPos)[m_arrCenters[CENTER_CC]]))
			m_vecIsIntersected[CORNER_LB] = m_vecIsIntersected[CORNER_RB] = true;

		// If. RB == flase
		if (false == m_vecIsIntersected[CORNER_RB])
			if (true == ComputeLineIntersect(_fRaySlope, _fRayIntercept, (*m_pVerticesPos)[m_arrCenters[CENTER_BC]], (*m_pVerticesPos)[m_arrCorners[CORNER_RB]]))
				m_vecIsIntersected[CORNER_RB] = true;

		// If. RB == flase
		if (false == m_vecIsIntersected[CORNER_RB])
			if (true == ComputeLineIntersect(_fRaySlope, _fRayIntercept, (*m_pVerticesPos)[m_arrCorners[CORNER_RB]], (*m_pVerticesPos)[m_arrCenters[CENTER_BC]]))
				m_vecIsIntersected[CORNER_RB] = true;





		// If. LB == flase
		if (false == m_vecIsIntersected[CORNER_LB])
			if (true == ComputeLineIntersect(_fRaySlope, _fRayIntercept, (*m_pVerticesPos)[m_arrCorners[CORNER_LB]], (*m_pVerticesPos)[m_arrCenters[CENTER_BC]]))
				m_vecIsIntersected[CORNER_LB] = true;

		// If. LB == flase
		if (false == m_vecIsIntersected[CORNER_LB])
			if (true == ComputeLineIntersect(_fRaySlope, _fRayIntercept, (*m_pVerticesPos)[m_arrCorners[CORNER_LB]], (*m_pVerticesPos)[m_arrCenters[CENTER_LC]]))
				m_vecIsIntersected[CORNER_LB] = true;
	}

	for (_int i = 0; i < CORNER_END; i++)
	{

		if (true == m_vecIsIntersected[i])
		{
			m_arrChilds[i]->Search(_vecIndices, _vRayPos, _vRayDir, _fRaySlope, _fRayIntercept);
			m_vecIsIntersected[i] = false;
		}
	}
}

_bool CQuadTree::ComputeLineIntersect(_float& _fRaySlope, _float& _fRayIntercept, _float3& _vStartPos, _float3& _vEndPos)
{

	if (0.f < _fRaySlope) // Slope : positive
	{
		// horiz
		if (_vStartPos.z == _vEndPos.z)
		{
			// 
			if (true == (_fRaySlope * _vStartPos.x + _fRayIntercept <= _vStartPos.z
				&& _fRaySlope * _vEndPos.x + _fRayIntercept >= _vEndPos.z))
			{
				return true;
			}
		}
		// vertical
		if (_vStartPos.x == _vEndPos.x)
		{
			if (true == (_vStartPos.z / _fRaySlope) - (_fRayIntercept / _fRaySlope) <= _vStartPos.x
				&& (_vEndPos.z / _fRaySlope) - (_fRayIntercept / _fRaySlope) >= _vEndPos.x)
			{
				return true;
			}
		}
	}
	else // Slope : negative
	{
		// horiz
		if (_vStartPos.z == _vEndPos.z)
		{
			if (true == _fRaySlope * _vStartPos.x + _fRayIntercept >= _vStartPos.z
				&& _fRaySlope * _vEndPos.x + _fRayIntercept <= _vEndPos.z)
			{
				return true;
			}
		}

		// vertical
		if (_vStartPos.x == _vEndPos.x)
		{
			if (true == (_vStartPos.z / _fRaySlope) - (_fRayIntercept / _fRaySlope) >= _vStartPos.x
				&& (_vEndPos.z / _fRaySlope) - (_fRayIntercept / _fRaySlope) <= _vEndPos.x)
			{
				return true;
			}
		}

	}

	return _bool();
}

//void CQuadTree::Make_Neighbors()
//{
//	if (nullptr == m_arrChilds[CORNER_LT]->m_arrChilds[CORNER_LT])
//		return;
//
//	m_arrChilds[CORNER_LT]->m_arrNeighbors[NEIGHBOR_RIGHT] = m_arrChilds[CORNER_RT];
//	m_arrChilds[CORNER_LT]->m_arrNeighbors[NEIGHBOR_BOTTOM] = m_arrChilds[CORNER_LB];
//
//	m_arrChilds[CORNER_RT]->m_arrNeighbors[NEIGHBOR_LEFT] = m_arrChilds[CORNER_LT];
//	m_arrChilds[CORNER_RT]->m_arrNeighbors[NEIGHBOR_BOTTOM] = m_arrChilds[CORNER_RB];
//
//	m_arrChilds[CORNER_RB]->m_arrNeighbors[NEIGHBOR_LEFT] = m_arrChilds[CORNER_LB];
//	m_arrChilds[CORNER_RB]->m_arrNeighbors[NEIGHBOR_TOP] = m_arrChilds[CORNER_RT];
//
//	m_arrChilds[CORNER_LB]->m_arrNeighbors[NEIGHBOR_RIGHT] = m_arrChilds[CORNER_RB];
//	m_arrChilds[CORNER_LB]->m_arrNeighbors[NEIGHBOR_TOP] = m_arrChilds[CORNER_LT];
//
//	if (nullptr != m_arrNeighbors[NEIGHBOR_RIGHT])
//	{
//		m_arrChilds[CORNER_RT]->m_arrNeighbors[NEIGHBOR_RIGHT] = m_arrNeighbors[NEIGHBOR_RIGHT]->m_arrChilds[CORNER_LT];
//		m_arrChilds[CORNER_RB]->m_arrNeighbors[NEIGHBOR_RIGHT] = m_arrNeighbors[NEIGHBOR_RIGHT]->m_arrChilds[CORNER_LB];
//	}
//
//	if (nullptr != m_arrNeighbors[NEIGHBOR_BOTTOM])
//	{
//		m_arrChilds[CORNER_LB]->m_arrNeighbors[NEIGHBOR_BOTTOM] = m_arrNeighbors[NEIGHBOR_BOTTOM]->m_arrChilds[CORNER_LT];
//		m_arrChilds[CORNER_RB]->m_arrNeighbors[NEIGHBOR_BOTTOM] = m_arrNeighbors[NEIGHBOR_BOTTOM]->m_arrChilds[CORNER_RT];
//	}
//
//	if (nullptr != m_arrNeighbors[NEIGHBOR_LEFT])
//	{
//		m_arrChilds[CORNER_LT]->m_arrNeighbors[NEIGHBOR_LEFT] = m_arrNeighbors[NEIGHBOR_LEFT]->m_arrChilds[CORNER_RT];
//		m_arrChilds[CORNER_LB]->m_arrNeighbors[NEIGHBOR_LEFT] = m_arrNeighbors[NEIGHBOR_LEFT]->m_arrChilds[CORNER_RB];
//	}
//
//	if (nullptr != m_arrNeighbors[NEIGHBOR_TOP])
//	{
//		m_arrChilds[CORNER_LT]->m_arrNeighbors[NEIGHBOR_TOP] = m_arrNeighbors[NEIGHBOR_TOP]->m_arrChilds[CORNER_LB];
//		m_arrChilds[CORNER_RT]->m_arrNeighbors[NEIGHBOR_TOP] = m_arrNeighbors[NEIGHBOR_TOP]->m_arrChilds[CORNER_RB];
//	}
//
//	for (_uint i = 0; i < CORNER_END; i++)
//	{
//		if (nullptr != m_arrChilds[i])
//			m_arrChilds[i]->Make_Neighbors();
//	}
//}

shared_ptr<CQuadTree> CQuadTree::Create(shared_ptr<CVIBuffer> _pVIBuffer, _uint _iLT, _uint _iRT, _uint _iRB, _uint _iLB)
{
	shared_ptr<CQuadTree> pInstance = make_shared<CQuadTree>();

	if (FAILED(pInstance->Initialize(_pVIBuffer, _iLT, _iRT, _iRB, _iLB)))
	{
		MSG_BOX("Failed to Created : CQuadTree");
		pInstance.reset();
	}

	return pInstance;
}
