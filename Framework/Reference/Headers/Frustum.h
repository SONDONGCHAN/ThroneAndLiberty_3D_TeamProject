#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CFrustum final
{
public:
	CFrustum();
	~CFrustum();

public:
	static shared_ptr<CFrustum> Create();

public:
	HRESULT Initialize();
	void Tick();
	ContainmentType isInWorldSpace(const BoundingBox& _BoundingBox);
	bool isInWorldSpace(FXMVECTOR _vPosition, const float& _fRadius);
	
	void MakeLocalSpace(FXMMATRIX _WorldMatrix);
	bool isInLocalSpace(FXMMATRIX _WorldMatrix, FXMVECTOR _vPosition, const float& _fRadius);

private:
	void MakePlanes(const XMFLOAT3* _pPoints, XMFLOAT4* _pPlanes);

private:
	XMFLOAT3 m_vOriginalPoints[8]{};
	XMFLOAT3 m_vPoints[8]{};
	XMFLOAT4 m_WorldPlanes[6]{};
	XMFLOAT4 m_LocalPlanes[6]{};
};

END