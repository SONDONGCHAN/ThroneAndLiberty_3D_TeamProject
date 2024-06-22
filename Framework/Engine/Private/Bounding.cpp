#include "Bounding.h"

CBounding::CBounding(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
}

