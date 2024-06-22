#include "pch.h"
#include "Trap.h"

CTrap::CTrap()
{
}

HRESULT CTrap::Initialize()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	m_pTransformCom = CTransform::Create(pGameInstance->GetDeviceInfo(), pGameInstance->GetDeviceContextInfo());

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_NONANIM;
	m_iShaderPass = 0;

	return S_OK;
}
