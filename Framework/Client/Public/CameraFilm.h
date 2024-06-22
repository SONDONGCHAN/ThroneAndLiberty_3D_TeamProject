#pragma once

#include "Camera.h"
#include "Client_Defines.h"

namespace Engine
{
	class CFilm;
}

namespace Client
{
	class CCameraFilm final : public CCamera
	{
	public:
		CCameraFilm();
		virtual ~CCameraFilm() = default;

	public:
		static shared_ptr<CCameraFilm> Create(CAMERA_DESC* _pCameraDesc, const wstring& _wstrFilePath);

	public:
		HRESULT Initialize(CAMERA_DESC* _pCameraDesc, const wstring& _wstrFilePath);
		virtual void PriorityTick(_float fTimeDelta) override;
		virtual void Tick(_float fTimeDelta) override;
		virtual void LateTick(_float fTimeDelta) override;
		virtual HRESULT Render() override;

	public:
		HRESULT StartProduction();
		HRESULT StartProduction(const _float3& _vLook, const _float3& _vPosition);
		bool isFinish() { return m_isFinish; }
		HRESULT ResetCamera();

	private:
		bool m_isStart{ false };
		bool m_isFinish{ false };
		bool m_isOriginal{ true };
		shared_ptr<CFilm> m_pFilm{ nullptr };

	};
}