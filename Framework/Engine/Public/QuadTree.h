#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer;
class CCollider;

class CQuadTree
{
public:
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
	enum CENTER { CENTER_LC, CENTER_TC, CENTER_RC, CENTER_BC, CENTER_CC, CENTER_END };
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };

public:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT Initialize(shared_ptr<CVIBuffer> _pVIBuffer, _uint _iLT, _uint _iRT, _uint _iRB, _uint _iLB);
	//void Culling(const _float3* pVerticesPos, _uint* pIndices, _uint* pNumIndices);
	//void Make_Neighbors();
	void Search(vector<_uint>& _vecIndices, _float2& _vRayPos, _float2& _vRayDir, _float& _fRaySlope, _float& _fRayIntercept);

	_bool ComputeLineIntersect(_float& _fRaySlope, _float& _fRayIntercept, _float3& _vStartPos, _float3& _vEndPos);

private:
	_bool m_bIsLeaf{};

	_uint m_iCenter{};
	array<_uint, CORNER_END> m_arrCorners{};
	array<_uint, CENTER_END> m_arrCenters{};
	array<_bool, CORNER_END> m_vecIsIntersected;

	//weak_ptr<CVIBuffer> m_pVIBuffer{};
	vector<_float3>* m_pVerticesPos{};
	//shared_ptr<CCollider> m_pCollider{};
	array<shared_ptr<CQuadTree>, CORNER_END> m_arrChilds{};
	array<shared_ptr<CQuadTree>, NEIGHBOR_END> m_arrNeighbors{};

public:
	static shared_ptr<CQuadTree> Create(shared_ptr<CVIBuffer> _pVIBuffer, _uint _iLT, _uint _iRT, _uint _iRB, _uint _iLB);
};
END