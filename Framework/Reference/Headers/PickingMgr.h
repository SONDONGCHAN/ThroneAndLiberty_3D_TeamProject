#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)
class CGameObject;

class CPickingMgr
{
public:
	enum INDEX { INDEX_A, INDEX_B, INDEX_C, INDEX_END };
	enum TARGET { TARGET_TERRAIN, TARGET_OBJECT, TARGET_ALL, TARGET_END };
	enum TYPE { TYPE_MESH, TYPE_PIXEL, TYPE_END };

public:
	CPickingMgr();
	virtual ~CPickingMgr() = default;

public:
	void GetWorldRay(_float3& _vRayPos, _float3& _vRayDir);
	_uint GetIdxPickedVertex() { return m_iIdxPickedVertex; }
	_float3 GetWorldPosPicked() { return m_vWorldPosPicked; }
	//shared_ptr<CGameObject> GetPickedObject() { return m_pPickedObject.lock(); }
	_uint GetPickedObjectID() { return m_iPickedObjectID; }

public:
	HRESULT Initialize(const GRAPHIC_DESC& _GraphicDesc); // 레이 만들어서 들고있게 만들어준다.
	void Tick(_float _fTimeDelta);

public:
	HRESULT ComputePicking(_uint _iTarget);

public:
	HRESULT IntersectTerrain();
	HRESULT IntersectPixel();
private:
	void TransformViewportToWorld();
	HRESULT IntersectRatToTriangle(vector<VTXNORTEX>& _vecVertices, vector<_uint>& _vecIndices, _float3 _vRayPos, _float3 _vRayDir, _float3& _vIntersectedPos);

private:
	HWND m_hWnd{};
	_float m_fClientWidth{};
	_float m_fClientHeight{};

	_float3 m_vWorldPosRay{};
	_float3 m_vWorldDirRay{};

	// Terrain 용 함수
	_uint m_iIdxPickedVertex{};
	_float3 m_vWorldPosPicked{};

	//weak_ptr<CGameObject> m_pPickedObject{};
	
	_uint m_iPickedObjectID{};

	ComPtr<ID3D11Texture2D> m_pTargetTexture2D{};
	ComPtr<ID3D11Texture2D> m_pTexture2D{};
	ComPtr<ID3D11RenderTargetView> m_pRTV{};
	ComPtr<ID3D11ShaderResourceView> m_pSRV{};

public:
	static shared_ptr<CPickingMgr> Create(const GRAPHIC_DESC& _GraphicDesc);
};

END