#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class CTexture;
class CQuadTree;

class ENGINE_DLL CVITerrain final : public CVIBuffer
{
	enum AXISTYPE { AXIS_X, AXIS_Y, AXIS_Z, AXIS_END };
		

public:
	CVITerrain(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual ~CVITerrain() = default;

public:
	vector<VTXNORTEX>& GetVertices() { return m_vecVertices; }
	vector<_uint>& GetIndices() { return m_vecIndices; }
	_uint GetNumX() { return m_iNumVerticesX; }
	_uint GetNumZ() { return m_iNumVerticesZ; }

public:
	virtual HRESULT Initialize(ifstream& _In); // Client, MapTool
	virtual HRESULT Initialize(array<_int, AXIS_END> _arrSize); // MapTool
	virtual HRESULT Initialize(vector<_float3>& _vecPos, _uint _iSize); // MapTool

public:
	void UpdateHight(vector<_uint>& _vecEditableIndices, _float3 _vPickedLocalPos, _float _fRadius, _float _fPower, _float _fTimeDelta);
	void UpdateHightFlat(vector<_uint>& _vecEditableIndices, _float3 _vPickedLocalPos, _float _fRadius, _float _fPivotY);
	void Search(vector<_uint>& _vecIndices, _float2& _vRayPos, _float2& _vRayDir, _float& _fRaySlope, _float& _fRayIntercept);
	void SaveBinary(ofstream& _Out);

private:
	_uint m_iNumVerticesX{};
	_uint m_iNumVerticesZ{};

	vector<VTXNORTEX> m_vecVertices{};
	vector<_uint> m_vecIndices{};

	class shared_ptr<CQuadTree> m_pQuadTree {};

public:
	static shared_ptr<CVITerrain> Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext, ifstream& _In); // Client, MapTool
	static shared_ptr<CVITerrain> Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext, array<_int, AXIS_END> _arrSize); // MapTool
	static shared_ptr<CVITerrain> Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext, vector<_float3>& _vecPos, _uint _iSize); // MapTool
};
END