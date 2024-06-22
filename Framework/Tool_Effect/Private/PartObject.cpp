#include "PartObject.h"

CPartObject::CPartObject()
{
}

CPartObject::~CPartObject()
{
}

HRESULT CPartObject::Initialize(PARTOBJ_DESC* _pPartObjDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
	__super::Initialize(_pTransformDesc);

	m_pParentTransform = _pPartObjDesc->pParentTransform;
	
	return S_OK;
}

void CPartObject::PriorityTick(_float _fTimeDelta)
{
}

void CPartObject::Tick(_float _fTimeDelta)
{
}

void CPartObject::LateTick(_float _fTimeDelta)
{
}

HRESULT CPartObject::Render()
{

	return S_OK;
}
