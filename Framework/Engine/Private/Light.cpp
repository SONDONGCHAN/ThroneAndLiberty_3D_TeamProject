#include "Light.h"
#include "Shader.h"
#include "VIRect.h"
#include "GameInstance.h"

CLight::CLight()
{
}

void CLight::CheckDistance(_float3& _vPlrPos)
{
	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
		return;


	_float3 vLightPos = _float3(m_LightDesc.vPosition.x, m_LightDesc.vPosition.y, m_LightDesc.vPosition.z);
	_float fLength = (_vPlrPos - vLightPos).Length();

	if (fLength >= m_LightDesc.fCullDistance) {
		m_bEnable = false;
	}
	else {
		m_bEnable = true;
	}

}

HRESULT CLight::Initialize(const LIGHT_DESC& _LightDesc)
{
    m_LightDesc = _LightDesc;
    return S_OK;
}

HRESULT CLight::Render()
{
	if (!m_bEnable)
		return S_OK;

	_uint		iPassIndex = { 0 };

	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{

		GAMEINSTANCE->ChangePassIdx(1);

		if (FAILED(GAMEINSTANCE->BindRawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
	}
	else if (LIGHT_DESC::TYPE_POINT == m_LightDesc.eType)
	{

		GAMEINSTANCE->ChangePassIdx(2);


		if (FAILED(GAMEINSTANCE->BindRawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_IsUsingNoise", &m_LightDesc.bUsingNoise, sizeof(_bool))))
			return E_FAIL;

	}

	if (FAILED(GAMEINSTANCE->BindRawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BeginShader()))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CLight> CLight::Create(const LIGHT_DESC& _LightDesc)
{
	shared_ptr<CLight> pLight = make_shared<CLight>();

	if (FAILED(pLight->Initialize(_LightDesc)))
		MSG_BOX("Failed to Create : CLight");

    return pLight;
}
