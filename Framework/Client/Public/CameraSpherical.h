#pragma once

#include "Camera.h"
#include "Client_Defines.h"

namespace Client
{
    class CCameraSpherical final : public CCamera
    {
    public:
        CCameraSpherical();
        virtual ~CCameraSpherical() = default;

    public:
        static shared_ptr<CCameraSpherical> Create();

    public:
        HRESULT Initialize();
        virtual void PriorityTick(_float _fTimeDelta) override;
        virtual void Tick(_float _fTimeDelta) override;
        virtual void LateTick(_float _fTimeDelta) override;
        virtual HRESULT Render() override;

    public:
        void SetOffSetPosition(_float3 _vPosition);
        HRESULT SetOwner(shared_ptr<CGameObject> _pOwner);      // 카메라 객체의 소유자 설정
        void LockOn(shared_ptr<CGameObject> _pTarget);          // LockOn 객체 설정
        void LockOff();                                         // LockOn 해제
        void CameraZoom(bool _isZoom);
        float GetRadius() { return m_vSphericalPos.x; }
        void SetRadius(float _fRadius) { m_vSphericalPos.x = _fRadius; }
        void SetSphericalPos(const _float3& _vSphericalPos) { memcpy_s(&m_vSphericalPos, sizeof(_float3), &_vSphericalPos, sizeof(_float3)); }
        void SetOwnerLerpRatio(float _fLerpRatio) { m_fOwnerLerpRatio = _fLerpRatio; }

    private:
        _float3 ComputeSphericalCoordinate(_float3 _vAt);
		_float3 ComputeSphericalCoordinateLockOn(_float3 _vAt, _float3 _vTargetPosition);
        void CursorFixCenter();
        void KeyInput(float _fTimeDelta);
        void MouseMove(float _fTimeDelta);
        void CameraMove(float _fTimeDelta);
        void RefineCoordinate();
        _float3 ConvertSpherical(_float3 _vPos);
        void ZoomTick();

    private:
        _float3 m_vOffSetPos;
        _float3 m_vLockOnOffSetPos;
		_float3 m_vSphericalPos = { 4.0f,XM_PIDIV2,XM_PI * 1.5f };
        float m_fProjRadius = 0.0f;
        weak_ptr<CGameObject> m_pOwnerObject;
        weak_ptr<CGameObject> m_pTargetObject;
        _float3 m_vLockOnPosition;
        float m_fMouseSensitivity = 0.0f;
        _bool m_IsCurrentMouseLock = { true };
        bool m_isLockOn{ false };
        float m_fLockOffTimeAcc{ 0.0f };

        float m_fOwnerLerpRatio{ 0.55f };
        float m_fDefaultFovy{ 0.0f };
        bool  m_isZoom{ false };
    };
}
