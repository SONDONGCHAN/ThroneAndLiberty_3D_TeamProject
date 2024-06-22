#pragma once

#include "Tool_Defines.h"
#include "Camera.h"

namespace Tool_UI
{
	class CToolFreeCamera final : public CCamera
	{
		using Super = CCamera;

	public:
		struct TOOLCAMERAFREE_DESC {
			_float fMouseSensor = { 0.f };
		};

	public:
		CToolFreeCamera();
		virtual ~CToolFreeCamera();

	private:
		virtual HRESULT Initialize(_bool _IsEnable, TOOLCAMERAFREE_DESC* _pCamFreeDesc, CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc);
		virtual void PriorityTick(_float fTimeDelta) override;
		virtual void Tick(_float fTimeDelta) override;
		virtual void LateTick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	private:
		void KeyInput();

	private:
		_float m_fMouseSensor = 0.f;
		_bool m_IsCamLock = true;


	private:
		_bool m_bKeyDeb = false;
		_float m_fKeyDebTime = 0.f;


	public:
		static shared_ptr<CToolFreeCamera> Create(_bool _IsEnable, TOOLCAMERAFREE_DESC* _pCamFreeDesc, CAMERA_DESC* _pCamDes, CTransform::TRANSFORM_DESC* _TransDesc);

	};
}
