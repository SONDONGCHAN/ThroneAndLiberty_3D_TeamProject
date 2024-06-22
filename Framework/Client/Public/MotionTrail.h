#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CMotionTrail final : public CGameObject
{
public:
	struct MOTIONTRAIL_ELEMENT_DESC
	{
		_float4x4					WorldMatrix;
		_float4x4					BoneMatrix[MAX_BONE];

		_float						fLifeTime = 0.f;

		_float3						vColor = { 1.f, 1.f, 1.f };
	};

	struct MOTIONTRAIL_DESC
	{
		shared_ptr<CModel>			pModel = nullptr;
		_uint						iSize = 0;
		_float						fTrailCntPerTick = 1.f;

		_float						fLifeTime = 0.f;
		_float3						vColor = { 1.f, 1.f, 1.f };
	};

public:
	CMotionTrail();
	CMotionTrail(const CMotionTrail& _rhs);
	virtual ~CMotionTrail() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void	SettingMotionTrail(MOTIONTRAIL_DESC _MeshTrail_Desc, shared_ptr<CGameObject> _pOwner, _int _iExceptCnt = 0, _int* ExceptIdxs = nullptr);
	void	ClearMotionTrail();

public:
	void	ChangeModel(shared_ptr<CModel> _pModel);
	void	SetTimer(_bool _bUseTimer, _float _fTimer);
	void	SetColor(_float4 _vColor);

private:
	void	AddMotionTrail();

private:
	weak_ptr<CGameObject>			m_pOwner;

	MOTIONTRAIL_DESC				m_MotionTrailDesc;
	list<MOTIONTRAIL_ELEMENT_DESC>	m_MotionTrailList;

	_float							m_fRimWeight = 4.f;

	_bool							m_bUseTimer = false;
	_float							m_fTime = 0.f;

	_float							m_fGenTimer = 0.f;

	_int							m_iExceptMeshCnt = 0;
	_int							m_iExceptMeshIdx[50];

public:
	static shared_ptr<CMotionTrail> Create();
};

END