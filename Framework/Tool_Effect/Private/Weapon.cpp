#include "Weapon.h"
#include "Model.h"

CWeapon::CWeapon()
	: CPartObject()
{
}

CWeapon::~CWeapon()
{
}

HRESULT CWeapon::Initialize(WEAPON_DESC* _pWeaponDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
	WEAPON_DESC* pWeaponDesc = _pWeaponDesc;

	m_pSocketMatrix			= pWeaponDesc->pSocketMatrix;
	m_strModelKey			= pWeaponDesc->strModelKey;
	m_OffsetMatrixNormal	= pWeaponDesc->OffsetMatrixNormal;
	m_OffsetMatrixSituation = pWeaponDesc->OffsetMatrixSituation;

	PARTOBJ_DESC* pPartObjDesc = (PARTOBJ_DESC*)_pWeaponDesc;

	__super::Initialize(pPartObjDesc, _pTransformDesc);

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_NONANIM;
	m_iShaderPass = 0;
	m_pTransformCom->SetScaling(1.f, 1.5f, 1.f);
	//m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

	m_pModelCom = GAMEINSTANCE->GetModel(m_strModelKey);

	m_CurrentOffset = m_OffsetMatrixNormal;

	return S_OK;
}

void CWeapon::PriorityTick(_float _fTimeDelta)
{
}

void CWeapon::Tick(_float _fTimeDelta)
{
}

void CWeapon::LateTick(_float _fTimeDelta)
{
	_float4x4 BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	BoneMatrix.Right(XMVector3Normalize(BoneMatrix.Right()));
	BoneMatrix.Up(XMVector3Normalize(BoneMatrix.Up()));
	BoneMatrix.Forward(XMVector3Normalize(BoneMatrix.Forward()));

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMatrix() * m_CurrentOffset * BoneMatrix * m_pParentTransform->GetWorldMatrix());

	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
}

HRESULT CWeapon::Render()
{
	if (m_bActiveOn)
	{
		GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
		GAMEINSTANCE->BindWVPMatrixPerspective(m_WorldMatrix);

		_uint iNumMeshes = m_pModelCom->GetNumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++) {
			GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
		}
	}

	return S_OK;
}

shared_ptr<CWeapon> CWeapon::Create(WEAPON_DESC* _pWeaponDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
	shared_ptr<CWeapon> pInstance = make_shared<CWeapon>();

	if (FAILED(pInstance->Initialize(_pWeaponDesc, _pTransformDesc)))
		MSG_BOX("Failed to Create : Weapon");
	
	return pInstance;
}
