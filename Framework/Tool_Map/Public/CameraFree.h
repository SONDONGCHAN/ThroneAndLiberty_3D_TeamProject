#pragma once

#include "Tool_Defines.h"
#include "Camera.h"

BEGIN(Tool_Map)

class CCameraFree : public CCamera
{
public:
	struct CAMERAFREE_DESC 
	{
		_float fMouseSensor{};
	};

public:
	CCameraFree();
	DESTRUCTOR(CCameraFree)

public:
	void ChangeLockState() { m_IsCamLock = !m_IsCamLock; }
	_float* GetMouseSensor() { return &m_fMouseSensor; }

private:
	virtual HRESULT Initialize(_bool _IsEnable, CAMERAFREE_DESC* _pCamFreeDesc, CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc);
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void KeyInput(_float _fTimeDelta);

private:
	_float m_fMouseSensor{};
	_bool m_IsCamLock{};

	_bool m_bKeyDeb{};
	_float m_fKeyDebTime{};

public:
	static shared_ptr<CCameraFree> Create(_bool _IsEnable, CAMERAFREE_DESC* _pCamFreeDesc, CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc);
};

END