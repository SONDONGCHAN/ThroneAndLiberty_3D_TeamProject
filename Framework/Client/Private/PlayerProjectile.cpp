#include "PlayerProjectile.h"
#include "Model.h"

CPlayerProjectile::CPlayerProjectile()
{
}

CPlayerProjectile::~CPlayerProjectile()
{
}

HRESULT CPlayerProjectile::Initialize(shared_ptr<CTransform::TRANSFORM_DESC> pArg)
{
	__super::Initialize(pArg.get());

	return S_OK;
}

void CPlayerProjectile::PriorityTick(_float _fTimeDelta)
{
}

void CPlayerProjectile::Tick(_float _fTimeDelta)
{
}

void CPlayerProjectile::LateTick(_float _fTimeDelta)
{
}

HRESULT CPlayerProjectile::Render()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++) 
	{
		GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
	}

	return S_OK;
}

HRESULT CPlayerProjectile::RenderGlow()
{
	return S_OK;
}
