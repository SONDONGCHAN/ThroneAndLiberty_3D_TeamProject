#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCameraFree final : public CCamera
{
public:
	struct CAMERAFREE_DESC {
		_float fMouseSensor = { 0.f };
	};

public:
	CCameraFree();
	virtual ~CCameraFree();

private:
	virtual HRESULT Initialize(_bool _IsEnable, CAMERAFREE_DESC* _pCamFreeDesc, CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc);
	virtual void PriorityTick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void CursorFixCenter();

private:
	void KeyInput(_float _fTimeDelta);

private:
	_float m_fMouseSensor = 0.f;
	_bool m_IsCamLock = false;


private:
	_bool m_bKeyDeb = false;
	_float m_fKeyDebTime = 0.f;


public:
	static shared_ptr<CCameraFree> Create(_bool _IsEnable, CAMERAFREE_DESC* _pCamFreeDesc, CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc);

};

END