#include "EnvironmentObject.h"
#include "Model.h"
#include "InstancingModel.h"
#include "RigidBody.h"

CEnvironmentObject::CEnvironmentObject()
{
}


CEnvironmentObject::~CEnvironmentObject()
{
}


HRESULT CEnvironmentObject::Initialize(const string& _strModelKey, vector<_float4x4>& _vecMatWorld, vector<_float3>& _vCenterPoint, vector<_float>& _fRadius)
{
    __super::Initialize(nullptr);

    m_CulledInstanceWorldMatrix = _vecMatWorld;
    m_vecCenterPoint = _vCenterPoint;
    m_vecRadius = _fRadius;

	m_iShaderPass = 0;

	if (0 == _strModelKey.compare("BG_Pillar_02_01_SM")
		|| 0 == _strModelKey.compare("BG_HumanStatue_30_01_SM")
		|| 0 == _strModelKey.compare("BG_OrcTent_01_01")
		|| 0 == _strModelKey.compare("BG_OrcTent_01_02")
		|| 0 == _strModelKey.compare("BG_OrcTent_01_03")
		|| 0 == _strModelKey.compare("BG_OrcTent_01_04")
		|| 0 == _strModelKey.compare("BG_OrcTent_01_05")
		|| 0 == _strModelKey.compare("BG_OrcTentAwning_01_01")
		|| 0 == _strModelKey.compare("BG_OrcTentAwning_01_02")
		|| 0 == _strModelKey.compare("BG_OrcTentAwning_01_03")
		|| 0 == _strModelKey.compare("BG_OrcTowerAwning_01_01")
		|| 0 == _strModelKey.compare("BG_TentAwning_06_01_SM")
		|| 0 == _strModelKey.compare("BG_TentAwning_10_01_SM")
		|| 0 == _strModelKey.compare("BG_TentAwning_11_01_SM")
		|| 0 == _strModelKey.compare("BG_Wagon_04_01_SM")
		|| 0 == _strModelKey.compare("BG_TentAwning_01_03_SM")
		|| 0 == _strModelKey.compare("BG_TentAwning_01_01_SM")
		|| 0 == _strModelKey.compare("BG_TentAwning_01_02_SM")
		|| 0 == _strModelKey.compare("BG_LinenTool_01_01_SM"))
	{
		if (2 > (UINT)_vecMatWorld.size())
		{
			m_iShaderPass = 10;
		}
		else
		{
			m_iShaderPass = 2;
		}
	}


    if (2 > _vecMatWorld.size())
    {
        m_eShaderType = ESHADER_TYPE::ST_NONANIM;
        
        m_pTransformCom->SetWorldMatrix(_vecMatWorld.front());
        
        m_strModelKey = _strModelKey;
        m_pModelCom = GAMEINSTANCE->GetModel(m_strModelKey);
    }
    else
    {
        m_eShaderType = ESHADER_TYPE::ST_MESHINSTANCE;

        shared_ptr<CModel> pOrigin = GAMEINSTANCE->GetModel(_strModelKey);
        
        m_pInstancingModelCom = CInstancingModel::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), pOrigin, &_vecMatWorld);

        m_CulledInstanceWorldMatrix.reserve(m_pInstancingModelCom->GetInstanceNum());

        shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

        _uint iNumMatWorldSize = static_cast<_uint>(_vecMatWorld.size());
        for (UINT i = 0; i < iNumMatWorldSize; i++)
        {
            pGameInstance->AddStaticInstance(_strModelKey, shared_from_this(), _vCenterPoint[i], _vecMatWorld[i], _fRadius[i] * 1.5f);
        }
    }

    DistinguishShadowModel(_strModelKey);

    return S_OK;
}

void CEnvironmentObject::PriorityTick(_float _fTimeDelta)
{
}

void CEnvironmentObject::Tick(_float _fTimeDelta)
{

}

void CEnvironmentObject::LateTick(_float _fTimeDelta)
{
	if (m_pModelCom && GAMEINSTANCE->isInWorldSpace(m_vecCenterPoint.front(),m_vecRadius.front()))
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
        
        if(m_IsRenderShadow)
            GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());
		return;
	}
	if (m_pInstancingModelCom)
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
        if (m_IsRenderShadow)
            GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());
		return;
	}
}

HRESULT CEnvironmentObject::Render()
{
    if (ESHADER_TYPE::ST_MESHINSTANCE == m_eShaderType)
    {
        if (m_CulledInstanceWorldMatrix.empty())
            return S_OK;

        m_pInstancingModelCom->DiscardAndFill(m_CulledInstanceWorldMatrix.data(), (UINT)m_CulledInstanceWorldMatrix.size());
    }

    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    if (ESHADER_TYPE::ST_NONANIM == m_eShaderType)
    {
        _bool bRimLight = false;
        GAMEINSTANCE->BindRawValue("g_IsUsingRim", &bRimLight, sizeof(bRimLight));
        _uint iNumMeshes = m_pModelCom->GetNumMeshes();

        for (_uint i = 0; i < iNumMeshes; i++)
            GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, i);
    }
    else // ESHADER_TYPE::ST_MESHINSTANCE == m_eShaderType
    {
        _uint iNumMeshes = m_pInstancingModelCom->GetNumMeshes();

        for (_uint i = 0; i < iNumMeshes; i++)
        {
            GAMEINSTANCE->BeginNonAnimInstance(m_pInstancingModelCom, i);
        }
        m_CulledInstanceWorldMatrix.clear();
        m_CulledInstanceWorldMatrix.reserve(m_pInstancingModelCom->GetInstanceNum());
    }

    return S_OK;
}

void CEnvironmentObject::AddInstancingWorldMatrix(const _float4x4& _WorldMatrix)
{
    m_CulledInstanceWorldMatrix.emplace_back(_WorldMatrix);
}

void CEnvironmentObject::ClearInstancingWorldMatrix()
{
    if (ESHADER_TYPE::ST_MESHINSTANCE == m_eShaderType)
    {
        m_CulledInstanceWorldMatrix.clear();
        m_CulledInstanceWorldMatrix.reserve(m_pInstancingModelCom->GetInstanceNum());
    }
}

void CEnvironmentObject::DistinguishShadowModel(const string& _strModelKey)
{
    /*Shadow Instance*/
    if ("BG_StoneFloor_04_01_SM" == _strModelKey || "BG_StoneFloorBorder_09_01_SM" == _strModelKey
        ||"BG_StoneFloorBorder_09_02_SM" == _strModelKey || "BG_StoneFloorDeco_04_01_SM" == _strModelKey
        ||"BG_StoneStair_01_01_SM" == _strModelKey || "BG_StoneStair_01_03_SM" == _strModelKey
        ||"BG_StoneStair_01_06_SM" == _strModelKey || "BG_StoneStair_01_07_SM" == _strModelKey
        || "BG_BrickFloor_01_01_SM" == _strModelKey || "BG_Cliff_07_01_SM" == _strModelKey
        || "BG_Cliff_01_01_SM" == _strModelKey || "BG_Cliff_07_02_SM" == _strModelKey
        || "BG_Cliff_09_01_SM" == _strModelKey || "BG_Cliff_15_01_SM" == _strModelKey
        || "BG_Rock_30_01_SM" == _strModelKey || "BG_Rock_30_02_SM" == _strModelKey
        || "BG_StoneGate_02_01_SM" == _strModelKey || "BG_StoneGate_02_02_SM" == _strModelKey
        || "BG_StoneWall_07_09_SM" == _strModelKey || "BG_StoneWall_07_10_SM" == _strModelKey
        || "BG_StoneWall_07_18_SM" == _strModelKey || "BG_StonePillarCeiling_19_07_SM" == _strModelKey
        || "BG_StonePillar_03_01_SM" == _strModelKey || "BG_StonePillar_19_02_SM" == _strModelKey
        || "BG_BrickFloor_01_01_SM" == _strModelKey || "BG_Cathedral_06_05_SM" == _strModelKey
        || "BG_Cathedral_06_20_SM" == _strModelKey || "BG_Cliff_15_01_SM" == _strModelKey
        || "BG_FloorBorder_04_01_SM" == _strModelKey || "BG_Rock_30_01_SM" == _strModelKey
        || "BG_PillarDeco_01_01_SM" == _strModelKey || "BG_StonePillar_19_02_SM" == _strModelKey
        || "BG_Rock_30_02_SM" == _strModelKey || "BG_StoneFloor_04_01_SM" == _strModelKey
        || "BG_StoneFloor_05_01_SM" == _strModelKey || "BG_StoneFloorBorder_09_01_SM" == _strModelKey
        || "BG_StoneFloorBorder_09_02_SM" == _strModelKey || "BG_StoneFloorDeco_01_01_SM" == _strModelKey
        //|| "BG_Tree_01_07_SM" == _strModelKey
        )
    {
        m_IsRenderShadow = false;
    }

}

HRESULT CEnvironmentObject::RenderEdge()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, 2);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    if (ESHADER_TYPE::ST_NONANIM == m_eShaderType)
    {
        _uint iNumMeshes = m_pModelCom->GetNumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
            GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }
    else // ESHADER_TYPE::ST_MESHINSTANCE == m_eShaderType
    {
        _uint iNumMeshes = m_pInstancingModelCom->GetNumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
            GAMEINSTANCE->BeginNonAnimInstance(m_pInstancingModelCom, (_uint)i);

    }

    return S_OK;
}

HRESULT CEnvironmentObject::RenderShadow()
{


    if (ESHADER_TYPE::ST_NONANIM == m_eShaderType)
    {

        GAMEINSTANCE->ReadyShader(m_eShaderType, 1);
        GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
        GAMEINSTANCE->BindLightProjMatrix();

        _uint iNumMeshes = m_pModelCom->GetNumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
            GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }
    else // ESHADER_TYPE::ST_MESHINSTANCE == m_eShaderType
    {

        GAMEINSTANCE->ReadyShader(m_eShaderType, 1);
        GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
        GAMEINSTANCE->BindLightProjMatrix();

        _uint iNumMeshes = m_pInstancingModelCom->GetNumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
            GAMEINSTANCE->BeginNonAnimInstance(m_pInstancingModelCom, (_uint)i);

    }


    return S_OK;
}

shared_ptr<CEnvironmentObject> CEnvironmentObject::Create(const string& _strModelKey, vector<_float4x4>& _vecMatWorld, vector<_float3>& _vCenterPoint, vector<_float>& _fRadius)
{
    shared_ptr<CEnvironmentObject> pInstance = make_shared<CEnvironmentObject>();

    if (FAILED(pInstance->Initialize(_strModelKey, _vecMatWorld, _vCenterPoint, _fRadius)))
    {
        MSG_BOX("Failed to Created : CEnvironment");
        pInstance.reset();
    }

    return pInstance;
}