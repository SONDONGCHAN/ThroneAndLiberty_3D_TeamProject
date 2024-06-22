#include "EnvironmentObject.h"
#include "Model.h"
#include "InstancingModel.h"
#include "GroupObject.h"

// Test
#include "Collider.h"
#include "VIBuffer.h"
#include "Mesh.h"

_uint CEnvironmentObject::IDCount = 1;
_bool CEnvironmentObject::m_bIsReanderPixelPicking = true;

CEnvironmentObject::CEnvironmentObject()
{

}

#ifdef CHECK_REFERENCE_COUNT
CEnvironmentObject::~CEnvironmentObject()
{ 
}
#endif // CHECK_REFERENCE_COUNT

void CEnvironmentObject::SetCollider()
{
    // Shpere 테스트 콜라이더 //
    CBounding_Sphere::SphereDesc		SphereDesc{};
    SphereDesc.fRadius = m_fRadius;
    SphereDesc.vCenter = m_vCenterPoint;

    m_pSphereCollider = CCollider::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), CCollider::SPHERE_TYPE, &SphereDesc);

    if (nullptr == m_pSphereCollider)
        return;
}

HRESULT CEnvironmentObject::Initialize(string _strModelKey, _float4x4 _matWorld)
{
    __super::Initialize(nullptr);
 
    m_iObjID = IDCount++;

    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;

    m_pTransformCom->SetWorldMatrix(_matWorld);

    m_strModelKey = _strModelKey;
    m_pModelCom = GAMEINSTANCE->GetModel(m_strModelKey);

    return S_OK;
}

HRESULT CEnvironmentObject::Initialize(string _strModelKey, vector<_float4x4> _vecMatWorld)
{
    __super::Initialize(nullptr);


    if (2 > _vecMatWorld.size())
    {
        if (0 < _vecMatWorld.size())
        {
            m_eShaderType = ESHADER_TYPE::ST_NONANIM;
            m_iShaderPass = 0;

            m_pTransformCom->SetWorldMatrix(_vecMatWorld.front());

            m_strModelKey = _strModelKey;
            m_pModelCom = GAMEINSTANCE->GetModel(m_strModelKey);
        }
    }
    else
    {
        m_eShaderType = ESHADER_TYPE::ST_MESHINSTANCE;
        m_iShaderPass = 0;

        shared_ptr<CModel> pOrigin = GAMEINSTANCE->GetModel(_strModelKey);

        m_pInstancingModelCom = CInstancingModel::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), pOrigin, &_vecMatWorld);
    }

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
    if (true == m_bIsGroup)
        m_matOnerworld = m_pOnerGroupObject.lock()->GetTransform()->GetWorldMatrix();

    GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

    if (m_bIsReanderPixelPicking)
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_PIXELPICKING, shared_from_this());

    if (m_IsEdgeRender)
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_EDGE, shared_from_this());
}

HRESULT CEnvironmentObject::Render()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);

    if (true == m_bIsGroup)
    {
        _float4x4 matWorld = m_pTransformCom->GetWorldMatrix() * m_matOnerworld;
        GAMEINSTANCE->BindWVPMatrixPerspective(matWorld);
    }
    else
        GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

   // GAMEINSTANCE->BindRawValue("g_ObjectID", &m_iObjID, sizeof(_uint));

    _bool bRimLight = false;
    GAMEINSTANCE->BindRawValue("g_IsUsingRim", &bRimLight, sizeof(bRimLight));

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

HRESULT CEnvironmentObject::RenderPixelPicking()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, 9);
    
    if (true == m_bIsGroup)
    {
        _float4x4 matWorld = m_pTransformCom->GetWorldMatrix() * m_matOnerworld;
        GAMEINSTANCE->BindWVPMatrixPerspective(matWorld);
    }
    else
        GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    //_bool bRimLight = false;
//GAMEINSTANCE->BindRawValue("g_IsUsingRim", &bRimLight, sizeof(bRimLight));

    GAMEINSTANCE->BindRawValue("g_ObjectID", &m_iObjID, sizeof(_uint));


    if (ESHADER_TYPE::ST_NONANIM == m_eShaderType)
    {
        _uint iNumMeshes = m_pModelCom->GetNumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
            GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }
    //else // ESHADER_TYPE::ST_MESHINSTANCE == m_eShaderType
    //{
    //   _uint iNumMeshes = m_pInstancingModelCom->GetNumMeshes();

    //    for (size_t i = 0; i < iNumMeshes; i++)
    //        GAMEINSTANCE->BeginNonAnimInstance(m_pInstancingModelCom, (_uint)i);

    //}

    return S_OK;
}

HRESULT CEnvironmentObject::RenderEdgeDepth()
{
    
    _float4x4 matWorld = m_pTransformCom->GetWorldMatrix();

    if (nullptr != m_pOnerGroupObject.lock())
        matWorld = matWorld * m_matOnerworld;

    GAMEINSTANCE->ReadyShader(m_eShaderType, 2);
    GAMEINSTANCE->BindWVPMatrixPerspective(matWorld);

    //GAMEINSTANCE->BindRawValue("g_ObjectID", &m_iObjID, sizeof(_uint));

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }
    
    return S_OK;
}

HRESULT CEnvironmentObject::RenderEdge()
{
    
    _float4x4 matWorld = m_pTransformCom->GetWorldMatrix();

    if (nullptr != m_pOnerGroupObject.lock())
        matWorld = matWorld * m_matOnerworld;

    GAMEINSTANCE->ReadyShader(m_eShaderType, 7);
    GAMEINSTANCE->BindWVPMatrixPerspective(matWorld);

   // GAMEINSTANCE->BindRawValue("g_ObjectID", &m_iObjID, sizeof(_uint));

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }
    
    return S_OK;
}

shared_ptr<CEnvironmentObject> CEnvironmentObject::Create(string _strModelKey, _float4x4 _matWorld)
{
    shared_ptr<CEnvironmentObject> pInstance = WRAPPING(CEnvironmentObject)();

    if (FAILED(pInstance->Initialize(_strModelKey, _matWorld)))
    {
        MSG_BOX("Failed to Created : CEnvironment");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CEnvironmentObject> CEnvironmentObject::Create(const string _strModelKey, vector<_float4x4> _vecMatWorld)
{
    shared_ptr<CEnvironmentObject> pInstance = WRAPPING(CEnvironmentObject)();

    if (FAILED(pInstance->Initialize(_strModelKey, _vecMatWorld)))
    {
        MSG_BOX("Failed to Created : CEnvironment");
        pInstance.reset();
    }

    return pInstance;
}