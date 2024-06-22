#include "pch.h"
#include "MeshTrail.h"
#include "Model.h"

CMeshTrail::CMeshTrail()
    : CGameObject()
{
}

CMeshTrail::CMeshTrail(const CMeshTrail& rhs)
    : CGameObject(rhs)
{
}

HRESULT CMeshTrail::Initialize()
{
    if (FAILED(__super::Initialize(nullptr)))
        return E_FAIL;

    return S_OK; 
}

void CMeshTrail::Tick(_float _fTimeDelta)
{
    for (auto iter = m_MeshTrailList.begin(); iter != m_MeshTrailList.end();)
    {
        iter->fLifeTime -= _fTimeDelta;

        if (iter->fLifeTime <= 0.f)
            iter = m_MeshTrailList.erase(iter);
        else
            ++iter;
    }
}

void CMeshTrail::LateTick(_float fTimeDelta)
{
    //if (m_MeshTraliDesc.bGlow)
    //{
    //    if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_GLOW, shared_from_this())))
    //        return;
    //}

    if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_TRAIL, shared_from_this())))
        return;
}

HRESULT CMeshTrail::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(ST_NONANIM, 5)))
        return E_FAIL;

    UsingRimLight(m_MeshTraliDesc.vColor, m_fRimWeight);
    //UsingRimLight(_float3(0.98f, 0.59f, 0.31f), 4.f);

    _uint   iNumMeshes = m_MeshTraliDesc.pModel->GetNumMeshes();

    _uint   iIndex = 0;

    _float _fRimWeight = 0;

    for (auto& iter : m_MeshTrailList)
    {
        iIndex++;

        _fRimWeight = (((_float)iIndex / (_float)m_MeshTrailList.size()));
        _fRimWeight = (1.f - _fRimWeight);

        GAMEINSTANCE->BindRawValue("g_fRimPow", &_fRimWeight, sizeof(m_fRimWeight));

        if (FAILED(GAMEINSTANCE->BindWVPMatrixPerspective(iter.WorldMatrix)))
            return E_FAIL;

        for (_uint i = 0; i < iNumMeshes; i++)
        {
            
            if (FAILED(GAMEINSTANCE->BindMatrix("g_WorldMatrix", &iter.WorldMatrix)))
                return E_FAIL;

            GAMEINSTANCE->BeginNonAnimModel(m_MeshTraliDesc.pModel, (_uint)i);
            
        }
    }

    /*RimLight Off*/
    _bool bRimOff = false;
    GAMEINSTANCE->BindRawValue("g_IsUsingRim", &bRimOff, sizeof(bRimOff));



    return S_OK;
}

void CMeshTrail::SettingMeshTrail(MESHTRAIL_DESC MeshTrail_Desc)
{
    m_MeshTraliDesc = MeshTrail_Desc;

    if (MESH_MASK_ALPHA == m_MeshTraliDesc.ePassType)
    {
        m_strMaskTexKey = m_MeshTraliDesc.strMaskTexture;
    }
}

void CMeshTrail::AddMeshTrail(MESHTRAIL_ELEMENT_DESC MeshTrail_Desc)
{
    MeshTrail_Desc.fUVSpeedX = m_MeshTraliDesc.fUVx;
    MeshTrail_Desc.fUVSpeedY = m_MeshTraliDesc.fUVy;
    MeshTrail_Desc.fLifeTime = m_MeshTraliDesc.fLifeTime;
    m_MeshTrailList.push_back(MeshTrail_Desc);

    if (m_MeshTraliDesc.iSize < m_MeshTrailList.size())
        m_MeshTrailList.pop_front();
}

void CMeshTrail::ClearMeshTrail()
{
    m_MeshTrailList.clear();
}

shared_ptr<CMeshTrail> CMeshTrail::Create()
{
    shared_ptr<CMeshTrail> pInstance = make_shared<CMeshTrail>();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CMeshTrail");
        pInstance.reset();
    }
    return pInstance;
}