#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CLight
{
public:
	CLight();
	~CLight() = default;

public:
	LIGHT_DESC* GetLightDesc() {
		return &m_LightDesc;
	}

	void SetLightDesc(LIGHT_DESC _LightDesc) {
		m_LightDesc = _LightDesc;
	}

public:
	void SetEnable(_bool _IsEnabled) { m_bEnable = _IsEnabled; }
	_bool GetEnable() { return m_bEnable; }
	void CheckDistance(_float3& _vPlrPos);

	void SetUsingNoiseTex(_bool _IsUsingNoise) {
		m_IsUsingNoise = _IsUsingNoise;
	}
	
public:
	HRESULT Initialize(const LIGHT_DESC& _LightDesc);
	HRESULT Render();

private:

	_bool m_bEnable = true;
	_bool m_IsUsingNoise = false;
	LIGHT_DESC m_LightDesc = {};

	_float m_fCheckDistance = 50.f;

public:
	static shared_ptr<CLight> Create(const LIGHT_DESC& _LightDesc);

};

END
