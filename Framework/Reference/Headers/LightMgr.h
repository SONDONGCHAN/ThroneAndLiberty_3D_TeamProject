#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CLightMgr final
{
public:
	CLightMgr(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	~CLightMgr() = default;

public:
	/*Light 생성 시 Index를 반환받아 필요에 따라 끄고 켤 수 있도록함*/
	LIGHT_DESC* GetLightDesc(_uint _iIndex);
	void SetLightEnable(_uint _iIndex, _bool _IsEnabled);

public:
	void SetPlayer(shared_ptr<class CGameObject> _pPlayer) {
		m_pPlayer = _pPlayer;
	};

public:
	HRESULT Initialize();
	HRESULT AddLight(const LIGHT_DESC& _LightDesc, _uint* _iIndex = nullptr);
	HRESULT LateTick();
	HRESULT Render();
	
	void CheckDistanceFromPlayer();

public:
	void DeleteLight(_int _iLightIdx);
	void ClearAllLight();

public:
	_float3 GetLightDir() {
		return m_vLightDir;
	}

private:
	wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

	list<shared_ptr<class CLight>> m_Lights;

private:
	weak_ptr<class CGameObject> m_pPlayer;

private:
	_float3 m_vLightDir = _float3();
	_float4x4 m_shadowViewMat = _float4x4();

private:
	_float m_fNoiseSpeed = 1.f;

public:

	static shared_ptr<CLightMgr> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);

};

END