#pragma once

#include "Tool_CC_Define.h"
#include "GameObject.h"
#include "VICube.h"

class CColliderFrame final : public CGameObject
{
public:
	CColliderFrame();
	~CColliderFrame();

public:
	static shared_ptr<CColliderFrame> Create(shared_ptr<Geometry> _pGeometry, _float3 _vPosition);

public:
	HRESULT Initialize(shared_ptr<Geometry> _pGeometry, _float3 _vPosition);
	virtual void PriorityTick(_float _fTimeDelta);
	virtual void Tick(_float _fTimeDelta);
	void LateTick(_float _fTimeDelta, UINT _iRenderType);
	virtual HRESULT Render();

public:
	void ChangeGeometry(shared_ptr<Geometry> _pGeometry);
	void ChangePostion(_float3 _vPosition);
	void ChangeOffSetRotation(_float3 _vRotDegree);
	shared_ptr<Geometry> GetGeometry();
	_float4x4 GetWorldMat();


private:
	shared_ptr<Geometry> m_pGeometry = nullptr;
	_color m_vColor;
	_float4x4 m_WorldMat;
	UINT m_iRenderType = 0;
};