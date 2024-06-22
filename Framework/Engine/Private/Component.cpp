#include "Component.h"

CComponent::CComponent(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice(_pDevice),
    m_pContext(_pContext)
{
}

CComponent::CComponent(const shared_ptr<CComponent> _pOrigin)
    :m_pDevice(_pOrigin->m_pDevice), m_pContext(_pOrigin->m_pContext)
{
}
