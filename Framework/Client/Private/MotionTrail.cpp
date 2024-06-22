#include "pch.h"
#include "MotionTrail.h"
#include "Model.h"

CMotionTrail::CMotionTrail()
    : CGameObject()
{
}

CMotionTrail::CMotionTrail(const CMotionTrail& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CMotionTrail::Initialize()
{
    if (FAILED(__super::Initialize(nullptr)))
        return E_FAIL;

    return S_OK;
}

void CMotionTrail::Tick(_float _fTimeDelta)
{
    m_fGenTimer += _fTimeDelta;
    
    // 1. Trail Per Sec으로 수명 관리 (최대 개수를 넘길 수는 없으므로, LifeTime이 다 돌지 않아도 소멸되는 트레일이 생긴다)
    while (m_fGenTimer * 60.f > (1.f / m_MotionTrailDesc.fTrailCntPerTick))
    {
        m_fGenTimer -= (1.f / m_MotionTrailDesc.fTrailCntPerTick) / 60.f;
        AddMotionTrail();
    }

    //// 2. LifeTime으로 수명 관리
    //for (auto iter = m_MotionTrailList.begin(); iter != m_MotionTrailList.end();)
    //{
    //    iter->fLifeTime -= _fTimeDelta;

    //    if (iter->fLifeTime <= 0.f)
    //    {
    //        iter = m_MotionTrailList.erase(iter);
    //        //AddMotionTrail();
    //    }
    //    else
    //        ++iter;
    //}

    if (m_bUseTimer)
    {
        if (m_fTime <= 0.f)
        {
            m_MotionTrailList.clear();
            m_IsEnabled = false;
        }

        else
        {
            m_fTime -= _fTimeDelta;
            //// 3. 그냥 1 Tick 당 한개씩 추가하기 (지금 쓰는 방법)
            //AddMotionTrail();
        }
    }

    // 무슨 방법으로 쓰지...?
}

void CMotionTrail::LateTick(_float _fTimeDelta)
{
    if (!m_MotionTrailDesc.pModel)
        return;

    if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_TRAIL, shared_from_this())))
        return;
}

HRESULT CMotionTrail::Render()
{
    if (FAILED(GAMEINSTANCE->ReadyShader(ST_ANIM, 6)))
        return E_FAIL;

    UsingRimLight(m_MotionTrailDesc.vColor, m_fRimWeight);
    //UsingRimLight(_float3(0.98f, 0.59f, 0.31f), 4.f);

    _uint   iNumMeshes = m_MotionTrailDesc.pModel->GetNumMeshes();

    _uint   iIndex = 0;

    _float _fRimWeight = 0;

    for (auto& iter : m_MotionTrailList)
    {
        iIndex++;

        _fRimWeight = (_float)iIndex / (_float)m_MotionTrailList.size();
        _fRimWeight = (3.7f - _fRimWeight * 3.f); // 최대 3.7, 최소 0.7 // 낮을수록 진해진다.

        //_fRimWeight = 1.5f;

        GAMEINSTANCE->BindRawValue("g_fRimPow", &_fRimWeight, sizeof(m_fRimWeight));

        if (FAILED(GAMEINSTANCE->BindWVPMatrixPerspective(iter.WorldMatrix)))
            return E_FAIL;

        _int    iExceptIdx = 0;

        for (_uint i = 0; i < iNumMeshes; i++)
        {
            if (0 < m_iExceptMeshCnt)
            {
                if (m_iExceptMeshIdx[iExceptIdx] == i)
                {
                    iExceptIdx++;
                    continue;
                }
            }

            if (FAILED(GAMEINSTANCE->BindMatrix("g_WorldMatrix", &iter.WorldMatrix)))
                return E_FAIL;

            GAMEINSTANCE->BeginAnimModelBone(m_MotionTrailDesc.pModel, iter.BoneMatrix, (_uint)i);
        }
    }

    /*RimLight Off*/
    _bool bRimOff = false;
    GAMEINSTANCE->BindRawValue("g_IsUsingRim", &bRimOff, sizeof(bRimOff));

    return S_OK;
}

void CMotionTrail::SettingMotionTrail(MOTIONTRAIL_DESC _MotionTrail_Desc, shared_ptr<CGameObject> _pOwner, _int _iExceptCnt, _int* ExceptIdxs)
{
    m_MotionTrailDesc = _MotionTrail_Desc;
    m_pOwner = _pOwner;

    if (0 != _iExceptCnt)
    {
        m_iExceptMeshCnt = _iExceptCnt;
        memcpy(m_iExceptMeshIdx, ExceptIdxs, sizeof(_int) * _iExceptCnt);
    }
}

void CMotionTrail::AddMotionTrail()
{
    if (!m_MotionTrailDesc.pModel)
        return;

    MOTIONTRAIL_ELEMENT_DESC MotionTrail_Element_Desc = {};
    MotionTrail_Element_Desc.fLifeTime = m_MotionTrailDesc.fLifeTime;
    MotionTrail_Element_Desc.vColor = m_MotionTrailDesc.vColor;
    MotionTrail_Element_Desc.WorldMatrix = m_pOwner.lock()->GetTransform()->GetWorldMatrix();
    memcpy(MotionTrail_Element_Desc.BoneMatrix, m_MotionTrailDesc.pModel->GetPrevBoneMatrix(), sizeof(_float4x4) * MAX_BONE);

    m_MotionTrailList.push_back(MotionTrail_Element_Desc);

    if (m_MotionTrailDesc.iSize < m_MotionTrailList.size())
        m_MotionTrailList.pop_front();
}

void CMotionTrail::ClearMotionTrail()
{
    m_MotionTrailList.clear();
}

void CMotionTrail::ChangeModel(shared_ptr<CModel> _pModel)
{
    m_MotionTrailDesc.pModel = _pModel;
}

void CMotionTrail::SetTimer(_bool _bUseTimer, _float _fTimer)
{
    m_bUseTimer = true;
    m_fTime = _fTimer;
}

void CMotionTrail::SetColor(_float4 _vColor)
{
    m_MotionTrailDesc.vColor = _float3(_vColor.x, _vColor.y, _vColor.z);
}

shared_ptr<CMotionTrail> CMotionTrail::Create()
{
    shared_ptr<CMotionTrail> pInstance = make_shared<CMotionTrail>();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CMotionTrail");
        pInstance.reset();
    }
    return pInstance;
}