#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIInstancing abstract : public CVIBuffer
{
	using Super = CVIBuffer;

public:
	struct InstanceDesc
	{
		_float3			vPivot{};
		_float3			vCenter{};
		_float3			vRange{};
		_float2			vSize{};
		_float2			vSpeed{};
		_float2			vLifeTime{};
		_bool			isLoop{ false };
		_float4			vColor{};
		_float			fDuration{ 0.f };

		_float			fSizeY{ 0.f };

		_bool			isDir{ false };
		_float3			vDirRange{};
		_float2			vDirX{};
		_float2			vDirY{};
		_float2			VDirZ{};
	};

public:
	CVIInstancing(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	CVIInstancing(const shared_ptr<CVIInstancing> _rhs);
	virtual ~CVIInstancing();

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT InitializeClone(void* _pArg) override;
	virtual HRESULT Render();
	virtual HRESULT BindBuffers();

public:
	void SetBufferScale(_float _fSizeX, _float _fSizeY);
	void SetInstanceBufferScale(_float _fSizeX, _float _fSizeY);
	void SetColor(_float4 _vColor);
	void SetInstanceColor(_float4 _vColor);
	_bool IsFinished() { return m_isFinished; }

	ComPtr<ID3D11Buffer> GetInstanceBuffer() { return m_pVBInstance; }

public:
	void ResetUIAlpha();
	void ResetInstanceUIAlpha();

public: /* Tick */
	void TickUIAlpha(_float _fDeltaTime);
	void TickUIScaling(_float _fDeltaTime);

public:
	void UIQuickSlotScaling(_float _fDeltaTime, _uint _slotIndex);

protected:
	ComPtr<ID3D11Buffer> m_pVBInstance{ nullptr };
	_uint m_iInstanceStride{ 0 };
	_uint m_iNumInstance{ 0 };
	_uint m_iIndexCountPerInstance{ 0 };

	InstanceDesc m_InstanceData{};
	//InstanceUI m_InstanceUIData{};

	random_device m_RandomDevice;
	mt19937_64 m_RandomNumber;

	vector<_float> m_speed;
	vector<_float> m_lifeTime;

	_float m_fTrackPosition{ 0.f };
	_bool m_isFinished{ false };

protected:
	_float m_fScaleY{ 0.f }; /* UI용 스케일Y값이 다를 경우 */

};

END