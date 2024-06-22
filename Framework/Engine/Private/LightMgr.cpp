#include "LightMgr.h"
#include "Light.h"

#include "GameInstance.h"
#include "GameObject.h"

CLightMgr::CLightMgr(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice(_pDevice), m_pContext(_pContext)
{
}

LIGHT_DESC* CLightMgr::GetLightDesc(_uint _iIndex)
{
    auto iter = m_Lights.begin();

    for (size_t i = 0; i < _iIndex; ++i)
        ++iter;

    return (*iter)->GetLightDesc();
}

void CLightMgr::SetLightEnable(_uint _iIndex, _bool _IsEnabled)
{
    auto iter = m_Lights.begin();

    for (size_t i = 0; i < _iIndex; ++i)
        ++iter;

    (*iter)->SetEnable(_IsEnabled);
}

HRESULT CLightMgr::Initialize()
{
    return S_OK;
}

HRESULT CLightMgr::AddLight(const LIGHT_DESC& _LightDesc, _uint* _iIndex)
{
    shared_ptr<CLight> pLight = CLight::Create(_LightDesc);
    if (!pLight)
        return E_FAIL;

    m_Lights.push_back(pLight);

    if (_iIndex) {
        *(_iIndex) = (_uint)(m_Lights.size() - 1);
    }

    if (LIGHT_DESC::TYPE::TYPE_DIRECTIONAL == _LightDesc.eType) {

        m_vLightDir = _float3(_LightDesc.vDirection.x, _LightDesc.vDirection.y, _LightDesc.vDirection.z);

    }

    return S_OK;
}

HRESULT CLightMgr::LateTick()
{
    CheckDistanceFromPlayer();

    return S_OK;
}

HRESULT CLightMgr::Render()
{
    if (m_Lights.empty())
        return S_OK;

    if (FAILED(GAMEINSTANCE->BindSRV("g_NoiseTexture", "T_GPP_Noise_Smooth_A")))
        return E_FAIL;

    m_fNoiseSpeed += (0.6f * 0.016f);

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fNoiseSpeed", &m_fNoiseSpeed, sizeof(_float))))
        return E_FAIL;

        
    for (auto& pLight : m_Lights) {
        if (!pLight->GetEnable())
            continue;

        pLight->Render();
    }

    return S_OK;
}

void CLightMgr::CheckDistanceFromPlayer()
{
    _float4 vCamPos = GAMEINSTANCE->GetCamPosition();
    _float3 vCamPosParam = _float3(vCamPos.x, vCamPos.y, vCamPos.z);

    for (auto& iter : m_Lights) {

        iter->CheckDistance(vCamPosParam);
    }

}

void CLightMgr::DeleteLight(_int _iLightIdx)
{
    if (_iLightIdx >= m_Lights.size())
        return;

    auto lightIter = m_Lights.begin();

    for (_int i = 0; i < _iLightIdx; ++i) {

        ++lightIter;
    }

    m_Lights.erase(lightIter);

}

void CLightMgr::ClearAllLight()
{
    m_Lights.clear();
}

shared_ptr<CLightMgr> CLightMgr::Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
{
    shared_ptr<CLightMgr> pInstance = make_shared<CLightMgr>(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
        MSG_BOX("Failed to Create : CLightMgr");

    return pInstance;
}
