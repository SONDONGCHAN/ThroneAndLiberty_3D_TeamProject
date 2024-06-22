#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CMeshTrail final : public CGameObject
{
public:
	enum PASS_TYPE { MESH_DEFAULT, MESH_COLOR_NONALPHA, MESH_COLOR_ALPHA, MESH_MASK_ALPHA, MESH_END };

public:
	struct MESHTRAIL_ELEMENT_DESC
	{
		_float4x4					WorldMatrix;
		_float						fLifeTime = 0.f;

		_float						fUVSpeedX = 0.f;
		_float						fUVSpeedY = 0.f;

		_float3						vColor = { 1.f, 1.f, 1.f };
	};

	struct MESHTRAIL_DESC
	{
		shared_ptr<CModel>			pModel;
		_uint						iSize{ 0 };

		_float						fLifeTime = 1.f;
		_float3						vColor = { 1.f, 1.f, 1.f };
		_bool						bGlow = false;

		PASS_TYPE					ePassType = MESH_COLOR_ALPHA;

		_float						fUVx = 0.f;
		_float						fUVy = 0.f;

		_float						fUVSpeedRangeX = 0.f;
		_float						fUVSpeedRangeY = 0.f;

		string						strDiffuseTexture = "";
		string						strMaskTexture = "";
	};

public:
	CMeshTrail();
	CMeshTrail(const CMeshTrail& rhs);
	virtual ~CMeshTrail() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_float fTimeDelta);
	virtual void	LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void	SettingMeshTrail(MESHTRAIL_DESC MeshTrail_Desc);
	void	AddMeshTrail(MESHTRAIL_ELEMENT_DESC MeshTrail_Desc);
	void	ClearMeshTrail();

public:
	void	ChangeColor(_float4 _vColor)
	{
		m_MeshTraliDesc.vColor = _float3(_vColor.x, _vColor.y, _vColor.z);
		m_fRimWeight = _vColor.w *= 4.f;
	}

private:
	string							m_strDiffuseTexKey = "";
	string							m_strMaskTexKey = "";

	MESHTRAIL_DESC					m_MeshTraliDesc;
	list<MESHTRAIL_ELEMENT_DESC>	m_MeshTrailList;

	_float							m_fRimWeight = 4.f;

	_bool							m_bUseTimer = false;
	_float							m_fTime = 0.f;

public:
	static shared_ptr<CMeshTrail> Create();
};

END