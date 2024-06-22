#include "TestEffectMesh.h"
#include "Model.h"
#include "Animation.h"

CTestEffectMesh::CTestEffectMesh()
{
}

CTestEffectMesh::CTestEffectMesh(const CTestEffectMesh& rhs)
{
}

CTestEffectMesh::~CTestEffectMesh()
{
}

HRESULT CTestEffectMesh::Initialize(const string& _strModelKey)
{
    __super::Initialize(nullptr);

    m_eEffectDesc.strModelKey = _strModelKey;

    m_pModelCom = GAMEINSTANCE->GetModel(m_eEffectDesc.strModelKey);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_EFFECTMESH;
    m_iShaderPass = 0;

    m_bSetScale = false;
    m_bSetTrans = false;

    m_bPlaying = false;

    m_eEffectDesc.bUseDiffuse = true;
    m_strTextureKey[TEX_DIFFUSE] = "PlaceHolder";

    m_pTransformCom->SetTurnSpeed(1.f);

    m_fDistortionTimer = 0.f;

	return S_OK;
}

void CTestEffectMesh::PriorityTick(_float _fTimeDelta)
{
}

void CTestEffectMesh::Tick(_float _fTimeDelta)
{
    if (!m_bPlaying)
    {
        if (m_pOwner.lock())
        {
            if (m_eEffectDesc.StartTrackPos <= dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCurAnimation()->GetTrackPosition())
                m_bPlaying = true;

            else
                m_bPlaying = false;
        }

        else
            m_bPlaying = true;
    }

    if (!m_bPlaying)
        return;

    m_bEnableRender = true;

    m_fSpeedTime = _fTimeDelta * m_fSpeed;

    // 루프인 애들은 반복해서 돌려주기? 이거 이펙트 시스템에서 관여해야하기 때문에
    // 추후 이펙트 시스템 만들고 나서 생각해보기
    if (m_eEffectDesc.fDuration <= m_fActiveTime)
    {
        m_bFirstTrans = true;

        if (m_eEffectDesc.bLoop)
        {
            m_fActiveTime = 0.f;

            m_iDiffuseUVIdx = 0;
            m_vDiffuseUVScroll = { 0.f, 0.f };
            m_vDiffuseUVPivot = { 0.f, 0.f };

            m_iMaskUVIdx = 0;
            m_vMaskUVScroll = { 0.f, 0.f };
            m_vMaskUVPivot = { 0.f, 0.f };

            m_iNoiseUVIdx = 0;
            m_vNoiseUVScroll = { 0.f, 0.f };
            m_vNoiseUVPivot = { 0.f, 0.f };

            m_iCurKeyFrameIdx = 0;

            m_bPlaying = false;
        }
        else
        {
            m_bEnableRender = false;
            return;
        }
    }

    m_fActiveTime += m_fSpeedTime;

    if (m_eEffectDesc.bUseDiffuse)
    {
        if (1 == m_eEffectDesc.iDiffuseUVType)
        {
            m_vDiffuseUVScroll += m_eEffectDesc.fDiffuseUVScroll;
        }

        else if (2 == m_eEffectDesc.iDiffuseUVType)
        {
            if (m_fActiveTime > 1.f / m_eEffectDesc.fDiffuseUVAtlasSpeed * m_fSpeedTime * m_iDiffuseUVIdx)
            {
                m_iDiffuseUVIdx++;
                m_vDiffuseUVPivot.x += (1.f / m_eEffectDesc.iDiffuseUVAtlasCnt.x);

                if (1 <= m_vDiffuseUVPivot.x)
                {
                    m_vDiffuseUVPivot.x = 0;
                    m_vDiffuseUVPivot.y += (1.f / m_eEffectDesc.iDiffuseUVAtlasCnt.y);
                }
            }
            if (m_iDiffuseUVIdx >= m_eEffectDesc.iDiffuseUVAtlasCnt.x * m_eEffectDesc.iDiffuseUVAtlasCnt.y)
            {
                if (!m_eEffectDesc.bLoop)
                {
                    m_fActiveTime = 0.f;

                    m_vDiffuseUVScroll = { 0.f, 0.f };
                    m_vDiffuseUVPivot = { 0.f, 0.f };

                    m_iMaskUVIdx = 0;
                    m_vMaskUVScroll = { 0.f, 0.f };
                    m_vMaskUVPivot = { 0.f, 0.f };

                    m_iNoiseUVIdx = 0;
                    m_vNoiseUVScroll = { 0.f, 0.f };
                    m_vNoiseUVPivot = { 0.f, 0.f };

                    m_iCurKeyFrameIdx = 0;

                    m_bPlaying = false;
                    m_bEnableRender = false;

                    return;
                }
                else
                {
                    m_fActiveTime = 0.f;

                    m_iDiffuseUVIdx = 0;
                    m_vDiffuseUVScroll = { 0.f, 0.f };
                    m_vDiffuseUVPivot = { 0.f, 0.f };

                    m_iMaskUVIdx = 0;
                    m_vMaskUVScroll = { 0.f, 0.f };
                    m_vMaskUVPivot = { 0.f, 0.f };

                    m_iNoiseUVIdx = 0;
                    m_vNoiseUVScroll = { 0.f, 0.f };
                    m_vNoiseUVPivot = { 0.f, 0.f };

                    m_iCurKeyFrameIdx = 0;

                    m_bPlaying = false;

                    return;
                }
            }
        }
    }

    if (m_eEffectDesc.bUseMask)
    {
        if (1 == m_eEffectDesc.iMaskUVType)
        {
            m_vMaskUVScroll += m_eEffectDesc.fMaskUVScroll;
        }

        else if (2 == m_eEffectDesc.iMaskUVType)
        {
            if (m_fActiveTime > 1.f / m_eEffectDesc.fMaskUVAtlasSpeed * m_fSpeedTime * m_iMaskUVIdx)
            {
                m_iMaskUVIdx++;
                m_vMaskUVPivot.x += (1.f / m_eEffectDesc.iMaskUVAtlasCnt.x);

                if (1 <= m_vMaskUVPivot.x)
                {
                    m_vMaskUVPivot.x = 0;
                    m_vMaskUVPivot.y += (1.f / m_eEffectDesc.iMaskUVAtlasCnt.y);
                }
            }
        }
    }

    if (m_eEffectDesc.bUseNoise)
    {
        if (1 == m_eEffectDesc.iNoiseUVType)
        {
            m_vNoiseUVScroll += m_eEffectDesc.fNoiseUVScroll;
        }

        else if (2 == m_eEffectDesc.iNoiseUVType)
        {
            if (m_fActiveTime > 1.f / m_eEffectDesc.fNoiseUVAtlasSpeed * m_fSpeedTime * m_iNoiseUVIdx)
            {
                m_iNoiseUVIdx++;
                m_iNoiseUVIdx = 1;
                m_vNoiseUVPivot.x += (1.f / m_eEffectDesc.iNoiseUVAtlasCnt.x);

                if (1 <= m_vNoiseUVPivot.x)
                {
                    m_vNoiseUVPivot.x = 0;
                    m_vNoiseUVPivot.y += (1.f / m_eEffectDesc.iNoiseUVAtlasCnt.y);
                }
            }
        }
    }

    if (m_eEffectDesc.bUseColor)
    {
        memcpy(m_vCurColor, m_eEffectDesc.KeyFrames.GetCurColor(m_fActiveTime, m_eEffectDesc.iColorSplitArea), sizeof(_float4[4]));
    }

    if (m_bSetScale)
        m_vCurScale = m_eEffectDesc.KeyFrames.GetCurScale(m_fActiveTime);

    if (m_bSetTrans)
        m_vCurTrans = m_eEffectDesc.KeyFrames.GetCurTrans(m_fActiveTime);

    // Scale
    switch (m_eEffectDesc.iScaleFollowType)
    {
    case -1:
        m_pTransformCom->SetScaling(m_vCurScale);
        break;
    case 0:
        if (m_pOwner.lock())
            m_pTransformCom->SetScaling(m_pOwner.lock()->GetTransform()->GetScaled() + m_vCurScale);
        break;
    case 1:
        if (m_pSFollowBoneMat)
            m_pTransformCom->SetScaling(_float3((*m_pSFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Right().Length(), (*m_pSFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Up().Length(), (*m_pSFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Forward().Length()) + m_vCurScale);
        break;
    default:
        break;
    }
    
    _float3 parentRight;
    _float3 parentUp;
    _float3 parentLook;

    switch (m_eEffectDesc.iRotFollowType)
    {
    case -1:
        // World
        if (m_bFirstRot)
        {
            m_pTransformCom->RotationAll(0.f, 0.f, 0.f);

            // 1. 독립적인 회전값 적용하기
            m_pTransformCom->RotationPlus({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vRotation.x));
            m_pTransformCom->RotationPlus({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vRotation.y));
            m_pTransformCom->RotationPlus({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vRotation.z));

            m_bFirstRot = false;
        }

        // 2. 회전 속도, 회전 가속도 적용하기
        // 회전 속도
        m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.x) * m_fSpeedTime);
        m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.y) * m_fSpeedTime);
        m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.z) * m_fSpeedTime);

        // 회전 가속도
        m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.x) * m_fSpeedTime * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.y) * m_fSpeedTime * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.z) * m_fSpeedTime * m_fActiveTime);
        break;
    case 0:
        // Object + No Follow
        if (m_bFirstRot)
        {
            // 0. 부모의 라업룩을 적용하기
            if (m_pOwner.lock())
            {
                _quaternion CombinedQuat = m_pOwner.lock()->GetTransform()->GetQuat();

                m_pTransformCom->SetQuaternion(CombinedQuat);
            }

            // 1. 독립적인 회전값 적용하기
            m_pTransformCom->RotationPlus({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vRotation.x));
            m_pTransformCom->RotationPlus({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vRotation.y));
            m_pTransformCom->RotationPlus({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vRotation.z));

            m_bFirstRot = false;
        }

        // 2. 회전 속도, 회전 가속도 적용하기
        // 회전 속도
        m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.x) * m_fSpeedTime);
        m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.y) * m_fSpeedTime);
        m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.z) * m_fSpeedTime);

        // 회전 가속도
        m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.x) * m_fSpeedTime * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.y) * m_fSpeedTime * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.z) * m_fSpeedTime * m_fActiveTime);

        break;
    case 1:
        // Bone + No Follow
        if (m_bFirstRot)
        {
            // 0. 부모의 라업룩을 적용하기
            if (m_pRFollowBoneMat)
                m_pTransformCom->SetQuaternion(XMQuaternionRotationMatrix(*m_pRFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()));

            // 1. 독립적인 회전값 적용하기
            m_pTransformCom->RotationPlus({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vRotation.x));
            m_pTransformCom->RotationPlus({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vRotation.y));
            m_pTransformCom->RotationPlus({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vRotation.z));

            m_bFirstRot = false;
        }

        // 2. 회전 속도, 회전 가속도 적용하기
        // 회전 속도
        m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.x) * m_fSpeedTime);
        m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.y) * m_fSpeedTime);
        m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.z) * m_fSpeedTime);

        // 회전 가속도
        m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.x) * m_fSpeedTime * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.y) * m_fSpeedTime * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.z) * m_fSpeedTime * m_fActiveTime);

        break;
    case 2:
        // Object + Follow
        // 이것도 같이
        // 0. 부모의 라업룩을 적용하기
        if (m_pOwner.lock())
        {
            _quaternion CombinedQuat = m_pOwner.lock()->GetTransform()->GetQuat();

            m_pTransformCom->SetQuaternion(CombinedQuat);
        }
        //m_pTransformCom->SetState(CTransform::STATE_LOOK, m_pRFollowBoneMat->Forward());
        // 2. 독립적인 회전값 적용하기
        m_pTransformCom->RotationPlus({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vRotation.x));
        m_pTransformCom->RotationPlus({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vRotation.y));
        m_pTransformCom->RotationPlus({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vRotation.z));
        // 3. 회전 속도, 회전 가속도 적용하기
        // 회전 속도
        m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.x) * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.y) * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.z) * m_fActiveTime);
        // 회전 가속도
        m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.x) * m_fActiveTime * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.y) * m_fActiveTime * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.z) * m_fActiveTime * m_fActiveTime);
        break;
    case 3:
        // Bone + Follow
        // 0. 부모의 라업룩을 적용하기
        if (m_pRFollowBoneMat)
            m_pTransformCom->SetQuaternion(XMQuaternionRotationMatrix(*m_pRFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()));

        // 1. 독립적인 회전값 적용하기
        m_pTransformCom->RotationPlus({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vRotation.x));
        m_pTransformCom->RotationPlus({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vRotation.y));
        m_pTransformCom->RotationPlus({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vRotation.z));

        // 3. 회전 속도, 회전 가속도 적용하기
        // 회전 속도
        m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.x) * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.y) * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vTurnVel.z) * m_fActiveTime);
        // 회전 가속도
        m_pTransformCom->Turn({ 1.f, 0.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.x) * m_fActiveTime * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 1.f, 0.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.y) * m_fActiveTime * m_fActiveTime);
        m_pTransformCom->Turn({ 0.f, 0.f, 1.f }, XMConvertToRadians(m_eEffectDesc.vTurnAcc.z) * m_fActiveTime * m_fActiveTime);
        break;
    default:
        break;
    }

    // Translation
    switch (m_eEffectDesc.iTransFollowType)
    {
    case -1:
        // World
        m_pTransformCom->SetState(CTransform::STATE_POSITION, m_vCurTrans);
        break;
    case 0:
        // Object && No Follow
        if (m_pOwner.lock() && m_bFirstTrans)
        {
            _float3 vTransOnOwner = (SimpleMath::Matrix::CreateTranslation(m_vCurTrans) * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Translation();
            m_pTransformCom->SetState(CTransform::STATE_POSITION, vTransOnOwner);
            m_bFirstTrans = false;
        }
        break;
    case 1:
        // Bone && No Follow
        if (m_pTFollowBoneMat && m_bFirstTrans)
        {
            _float3 vTransOnOwner = (SimpleMath::Matrix::CreateTranslation(m_vCurTrans) * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Translation();
            m_pTransformCom->SetState(CTransform::STATE_POSITION, ((*m_pTFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Translation() - m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_POSITION)) + vTransOnOwner);
            m_bFirstTrans = false;
        }
        break;
    case 2:
        // Object && Follow
        if (m_pOwner.lock())
        {
            _float3 vTransOnOwner = (SimpleMath::Matrix::CreateTranslation(m_vCurTrans) * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Translation();
            m_pTransformCom->SetState(CTransform::STATE_POSITION, vTransOnOwner);

        }
        break;
    case 3:
        // Bone && Follow
        if (m_pTFollowBoneMat)
        {
            _float3 vTransOnOwner = (SimpleMath::Matrix::CreateTranslation(m_vCurTrans) * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Translation();
            m_pTransformCom->SetState(CTransform::STATE_POSITION, ((*m_pTFollowBoneMat * m_pOwner.lock()->GetTransform()->GetWorldMatrix()).Translation() - m_pOwner.lock()->GetTransform()->GetState(CTransform::STATE_POSITION)) + vTransOnOwner);
        }
        break;
    default:
        break;
    }

    if (m_eEffectDesc.bUseDslv && !m_bDslv)
    {
        if (m_fActiveTime < m_eEffectDesc.fDslvStartTime)
            return;

        SetDissolve(true, m_eEffectDesc.iDslvType, m_eEffectDesc.fDslvDuration);
        SetDissolveInfo(m_eEffectDesc.fDslvThick, m_eEffectDesc.vDslvColor, m_eEffectDesc.strDslvTexKey.c_str());
    }
}

void CTestEffectMesh::LateTick(_float _fTimeDelta)
{
    if (m_bDslv)
        UpdateDslv(_fTimeDelta);

    if (!m_bEnableRender)
        return;

    m_fSpeedTime = _fTimeDelta * m_fSpeed;

    m_eEffectBindDesc.fDiffuseUVScroll = m_vDiffuseUVScroll;
    m_eEffectBindDesc.fMaskUVScroll = m_vMaskUVScroll;
    m_eEffectBindDesc.fNoiseUVScroll = m_vNoiseUVScroll;

    m_eEffectBindDesc.vColor1 = m_vCurColor[0];
    m_eEffectBindDesc.vColor2 = m_vCurColor[1];
    m_eEffectBindDesc.vColor3 = m_vCurColor[2];
    m_eEffectBindDesc.vColor4 = m_vCurColor[3];

    m_eEffectBindDesc.fDuration = m_eEffectDesc.fDuration;
    m_eEffectBindDesc.bBillBoard = m_eEffectDesc.bBillBoard;
    m_eEffectBindDesc.bUseDiffuse = m_eEffectDesc.bUseDiffuse;
    m_eEffectBindDesc.bUseMask = m_eEffectDesc.bUseMask;
    m_eEffectBindDesc.bUseNoise = m_eEffectDesc.bUseNoise;
    m_eEffectBindDesc.bUseColor = m_eEffectDesc.bUseColor;

    m_eEffectBindDesc.iDiffuseSamplerType = m_eEffectDesc.iDiffuseSamplerType;
    m_eEffectBindDesc.iDiffuseUVType = m_eEffectDesc.iDiffuseUVType;
    m_eEffectBindDesc.fDiffuseDiscardValue = m_eEffectDesc.fDiffuseDiscardValue;
    m_eEffectBindDesc.iDiffuseDiscardArea = m_eEffectDesc.iDiffuseDiscardArea;
    m_eEffectBindDesc.iDiffuseUVAtlasCnt = m_eEffectDesc.iDiffuseUVAtlasCnt;
    m_eEffectBindDesc.fDiffuseUVAtlasSpeed = m_eEffectDesc.fDiffuseUVAtlasSpeed;

    m_eEffectBindDesc.iMaskSamplerType = m_eEffectDesc.iMaskSamplerType;
    m_eEffectBindDesc.iMaskUVType = m_eEffectDesc.iMaskUVType;
    m_eEffectBindDesc.fMaskDiscardValue = m_eEffectDesc.fMaskDiscardValue;
    m_eEffectBindDesc.iMaskDiscardArea = m_eEffectDesc.iMaskDiscardArea;
    m_eEffectBindDesc.iMaskUVAtlasCnt = m_eEffectDesc.iMaskUVAtlasCnt;
    m_eEffectBindDesc.fMaskUVAtlasSpeed = m_eEffectDesc.fMaskUVAtlasSpeed;

    m_eEffectBindDesc.iNoiseSamplerType = m_eEffectDesc.iNoiseSamplerType;
    m_eEffectBindDesc.iNoiseUVType = m_eEffectDesc.iNoiseUVType;
    m_eEffectBindDesc.fNoiseDiscardValue = m_eEffectDesc.fNoiseDiscardValue;
    m_eEffectBindDesc.iNoiseDiscardArea = m_eEffectDesc.iNoiseDiscardArea;
    m_eEffectBindDesc.iNoiseUVAtlasCnt = m_eEffectDesc.iNoiseUVAtlasCnt;
    m_eEffectBindDesc.fNoiseUVAtlasSpeed = m_eEffectDesc.fNoiseUVAtlasSpeed;

    m_eEffectBindDesc.iColorSplitArea = m_eEffectDesc.iColorSplitArea;
    m_eEffectBindDesc.vColorSplitter = m_eEffectDesc.vColorSplitter;

    if (CRenderer::RENDER_GLOW == m_eEffectDesc.iRenderGroup)
    {
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_BLEND, shared_from_this());
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_GLOW, shared_from_this());
    }
    else if (CRenderer::RENDER_BLUR == m_eEffectDesc.iRenderGroup)
    {
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_GLOW, shared_from_this());
    }
    else
    {
        GAMEINSTANCE->AddRenderGroup((CRenderer::ERENDER_GROUP)m_eEffectDesc.iRenderGroup, shared_from_this());
    }

    if (m_eEffectDesc.bUseDistortion)
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_DISTORTION, shared_from_this());

    //GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_EDGE, shared_from_this());

}

HRESULT CTestEffectMesh::Render()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);

    if (m_eEffectDesc.bBillBoard)
    {
        _float4 vCamPosV4 = GAMEINSTANCE->GetCamPosition();
        _float3 vCamPos = _float3(vCamPosV4.x, vCamPosV4.y, vCamPosV4.z);

        _float3 vCurPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);

        _float3 vBillBoardLook = vCamPos - vCurPos;

        // 오브젝트의 크기 저장
        _float3 vOriginalScale = m_pTransformCom->GetScaled();

        // 방향을 카메라와 평행하게 설정
        m_pTransformCom->LookAtForLandObject(vCamPos);

        float angleX = XMConvertToRadians(m_eEffectDesc.vRotation.x);
        float angleY = XMConvertToRadians(m_eEffectDesc.vRotation.y);
        float angleZ = XMConvertToRadians(m_eEffectDesc.vRotation.z);

        // 회전 행렬 생성
        SimpleMath::Matrix rotationMatrix = SimpleMath::Matrix::CreateRotationX(angleX) *
            SimpleMath::Matrix::CreateRotationY(angleY) *
            SimpleMath::Matrix::CreateRotationZ(angleZ);

        // 현재 변환 행렬 얻기
        SimpleMath::Matrix transformMatrix = m_pTransformCom->GetWorldMatrix();

        // 회전 행렬 적용
        transformMatrix *= rotationMatrix;

        // 새로운 변환 행렬 설정
        m_pTransformCom->SetWorldMatrix(transformMatrix);

        // 저장된 크기를 다시 적용
        m_pTransformCom->SetScaling(vOriginalScale);
    }

    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    if (m_bDslv)
    {
        if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &m_bDslv, sizeof(_bool))))
            return E_FAIL;

        _float g_fDslvValue = m_fDslvTime / m_fMaxDslvTime;


        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDslvValue", &g_fDslvValue, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_fDslvThick", &m_fDslvThick, sizeof(_float))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindRawValue("g_vDslvColor", &m_vDslvColor, sizeof(_float4))))
            return E_FAIL;

        if (FAILED(GAMEINSTANCE->BindSRV("g_DslvTexture", m_strDslvTexKey)))
            return E_FAIL;
    }

    if (m_eEffectDesc.bUseDiffuse)
        GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTextureKey[TEX_DIFFUSE]);

    if (m_eEffectDesc.bUseMask)
        GAMEINSTANCE->BindSRV("g_MaskTexture", m_strTextureKey[TEX_MASK]);

    if (m_eEffectDesc.bUseNoise)
        GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strTextureKey[TEX_NOISE]);

    if (2 == m_eEffectDesc.iDiffuseUVType)
        GAMEINSTANCE->BindRawValue("g_vDiffuseUVPivot", &m_vDiffuseUVPivot, sizeof(_float2));

    if (2 == m_eEffectDesc.iMaskUVType)
        GAMEINSTANCE->BindRawValue("g_vMaskUVPivot", &m_vMaskUVPivot, sizeof(_float2));

    if (2 == m_eEffectDesc.iNoiseUVType)
        GAMEINSTANCE->BindRawValue("g_vNoiseUVPivot", &m_vNoiseUVPivot, sizeof(_float2));

    GAMEINSTANCE->BindRawValue("g_EffectDesc", &m_eEffectBindDesc, sizeof(CEffectMesh::EffectMeshBindDesc));

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }

    _bool _bDslvFalse = false;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &_bDslvFalse, sizeof(_bool))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestEffectMesh::RenderGlow()
{
    //// 원본이랑 합쳐질게 아니라, 그냥 블러만 먹일거면 기존 원본을 갖고 해야됨.
    //if (CRenderer::RENDER_BLUR == m_eEffectDesc.iRenderGroup)
    //{
    //    GAMEINSTANCE->ReadyShader(m_eShaderType, m_eEffectDesc.iShaderPass);

    //    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    //    if (m_eEffectDesc.bUseDiffuse)
    //        GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTextureKey[TEX_DIFFUSE]);

    //    if (m_eEffectDesc.bUseMask)
    //        GAMEINSTANCE->BindSRV("g_MaskTexture", m_strTextureKey[TEX_MASK]);

    //    if (m_eEffectDesc.bUseNoise)
    //        GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strTextureKey[TEX_NOISE]);

    //    if (2 == m_eEffectDesc.iNoiseUVType)
    //        GAMEINSTANCE->BindRawValue("g_vNoiseUVPivot", &m_vNoiseUVPivot, sizeof(_float2));

    //    GAMEINSTANCE->BindRawValue("g_EffectDesc", &m_eEffectBindDesc, sizeof(CEffectMesh::EffectMeshBindDesc));
    //}

    //// 그게 아니면 색상값만 가지고 그려도 된다. (마스크, 노이즈? 일단 그려보고 생각하자)
    //else
    //{
        _bool bUseBlurColor = m_eEffectDesc.bUseGlowColor;
        _float4 vBlurColor = { 0.f, 0.f, 0.f, 0.f };
        if (bUseBlurColor)
            vBlurColor = m_eEffectDesc.KeyFrames.GetCurGlowColor(m_fActiveTime);

        GAMEINSTANCE->BindRawValue("g_bUseBlurColor", &bUseBlurColor, sizeof(_int));
        GAMEINSTANCE->BindRawValue("g_vBlurColor", &vBlurColor, sizeof(_float4));

        GAMEINSTANCE->ReadyShader(m_eShaderType, 1);

        GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

        if (m_eEffectDesc.bUseDiffuse)
            GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTextureKey[TEX_DIFFUSE]);

        if (m_eEffectDesc.bUseMask)
            GAMEINSTANCE->BindSRV("g_MaskTexture", m_strTextureKey[TEX_MASK]);

        if (m_eEffectDesc.bUseNoise)
            GAMEINSTANCE->BindSRV("g_NoiseTexture", m_strTextureKey[TEX_NOISE]);

        if (2 == m_eEffectDesc.iDiffuseUVType)
            GAMEINSTANCE->BindRawValue("g_vDiffuseUVPivot", &m_vDiffuseUVPivot, sizeof(_float2));

        if (2 == m_eEffectDesc.iNoiseUVType)
            GAMEINSTANCE->BindRawValue("g_vNoiseUVPivot", &m_vNoiseUVPivot, sizeof(_float2));

        GAMEINSTANCE->BindRawValue("g_EffectDesc", &m_eEffectBindDesc, sizeof(CEffectMesh::EffectMeshBindDesc));
    //}

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }

    return S_OK;
}

HRESULT CTestEffectMesh::RenderDistortion()
{
    if (!m_eEffectDesc.bUseDistortion)
        return E_FAIL;

    GAMEINSTANCE->ReadyShader(m_eShaderType, 2);

    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    GAMEINSTANCE->BindSRV("g_DistortionTexture", m_eEffectDesc.strDistortionKey);

    m_fDistortionTimer += GAMEINSTANCE->ComputeTimeDelta(TEXT("Timer_60"));

    GAMEINSTANCE->BindRawValue("g_fDistortionTimer", &m_fDistortionTimer, sizeof(_float));
    GAMEINSTANCE->BindRawValue("g_fDistortionSpeed", &m_eEffectDesc.fDistortionSpeed, sizeof(_float));
    GAMEINSTANCE->BindRawValue("g_fDistortionWeight", &m_eEffectDesc.fDistortionWeight, sizeof(_float));

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }

    return S_OK;
}

HRESULT CTestEffectMesh::RenderEdge()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, 3);

    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }

    return S_OK;
}

shared_ptr<CTestEffectMesh> CTestEffectMesh::Create(const string& _strModelKey)
{
    shared_ptr<CTestEffectMesh> pInstance = make_shared<CTestEffectMesh>();

    if (FAILED(pInstance->Initialize(_strModelKey)))
        MSG_BOX("Failed to Create : CTestEffectMesh");

    return pInstance;
}


HRESULT CTestEffectMesh::SetOwner(_bool _bSetOwner, _int _iType, shared_ptr<CGameObject> _pOwner)
{
    // 어떠한 항목의 Follow를 껐을 때
    if (!_bSetOwner)
    {
        //// Obj 껐을 때
        //if (_iObjOrBone)
        //{
        //    switch (_iSRT)
        //    {
        //    case 0:
        //        break;
        //    case 1:
        //        break;
        //    case 2:
        //        break;
        //    default:
        //        break;
        //    }
        //}
        //// Bone 껐을 때
        //else
        //{
            switch (_iType)
            {
            case -1:
                m_pOwner.lock() = nullptr;
                m_eEffectDesc.strScaleFollowBoneKey = "";
                m_pSFollowBoneMat = nullptr;
            case 0:
                m_eEffectDesc.strScaleFollowBoneKey = "";
                m_pSFollowBoneMat = nullptr;
                break;
            case 1:
                m_eEffectDesc.strRotFollowBoneKey = "";
                m_pRFollowBoneMat = nullptr;
                break;
            case 2:
                m_eEffectDesc.strTransFollowBoneKey = "";
                m_pTFollowBoneMat = nullptr;
                break;
            default:
                break;
            }
        //}

        return S_OK;
    }

    if (!_pOwner)
        return E_FAIL;

    m_pOwner = _pOwner;

    if ("" != m_eEffectDesc.strScaleFollowBoneKey)
    {
        m_pSFollowBoneMat = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr(m_eEffectDesc.strScaleFollowBoneKey.c_str());
    }

    if ("" != m_eEffectDesc.strRotFollowBoneKey)
    {
        m_pRFollowBoneMat = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr(m_eEffectDesc.strRotFollowBoneKey.c_str());
    }

    if ("" != m_eEffectDesc.strTransFollowBoneKey)
    {
        m_pTFollowBoneMat = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr(m_eEffectDesc.strTransFollowBoneKey.c_str());
    }

    return S_OK;
}

HRESULT CTestEffectMesh::SetOwner(shared_ptr<CGameObject> _pOwner)
{
    if (!_pOwner)
        return E_FAIL;

    m_pOwner = _pOwner;

    if ("" != m_eEffectDesc.strScaleFollowBoneKey)
    {
        m_pSFollowBoneMat = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr(m_eEffectDesc.strScaleFollowBoneKey.c_str());
    }

    if ("" != m_eEffectDesc.strRotFollowBoneKey)
    {
        m_pRFollowBoneMat = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr(m_eEffectDesc.strRotFollowBoneKey.c_str());
    }

    if ("" != m_eEffectDesc.strTransFollowBoneKey)
    {
        m_pTFollowBoneMat = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr(m_eEffectDesc.strTransFollowBoneKey.c_str());
    }

    return S_OK;
}

void CTestEffectMesh::SetDefaultInfo(_int _iShaderType, _int _iShaderPass, _int _iRenderGroup,
                                        _float _fDuration, _float _StartTrackPos, _bool _bBillBoard, _bool _bLoop)
{
    m_eShaderType = (ESHADER_TYPE)_iShaderType;
    m_iShaderPass = _iShaderPass;

    m_eEffectDesc.iRenderGroup = _iRenderGroup;
    m_eEffectDesc.fDuration = _fDuration;

    m_eEffectDesc.StartTrackPos = _StartTrackPos;
    
    m_eEffectDesc.bBillBoard = _bBillBoard;
    m_eEffectDesc.bLoop = _bLoop;

    Reset();
}

void CTestEffectMesh::SetDiffuseInfo(_bool _bUseDiffuse, string _strDiffuseKey, _int _iDiffuseSamplerType,
    _int _iDiffuseDiscardArea, _float _fDiffuseDiscardValue, _int _iDiffuseUVType,
    _float2 _vDiffuseUVScroll, _float2 _vDiffuseUVAtlasCnt, _float _fDiffuseUVAtalsSpeed)
{
    m_eEffectDesc.bUseDiffuse = _bUseDiffuse;
    m_strTextureKey[TEX_DIFFUSE] = _strDiffuseKey;

    m_fActiveTime = 0.f;

    m_eEffectDesc.iDiffuseSamplerType = _iDiffuseSamplerType;
    m_eEffectDesc.iDiffuseDiscardArea = _iDiffuseDiscardArea;
    m_eEffectDesc.fDiffuseDiscardValue = _fDiffuseDiscardValue;
    m_eEffectDesc.iDiffuseUVType = _iDiffuseUVType;
    m_eEffectDesc.fDiffuseUVScroll = _vDiffuseUVScroll;
    m_eEffectDesc.iDiffuseUVAtlasCnt = _vDiffuseUVAtlasCnt;
    m_eEffectDesc.fDiffuseUVAtlasSpeed = _fDiffuseUVAtalsSpeed;

    Reset();
}

void CTestEffectMesh::SetColorInfo(_bool _bUseColor, _int _iColorSplitArea,
    _float4 _vColorSplitter, _int _iColorSetCnt, _float* _fColorTime,
    _int* _iColorEase, _float4 _vColor[5][4])
{
    m_eEffectDesc.bUseColor = _bUseColor;
    m_eEffectDesc.iColorSplitArea = _iColorSplitArea;
    m_eEffectDesc.vColorSplitter = _vColorSplitter;
    m_eEffectDesc.iColorSetCnt = _iColorSetCnt;

    vector<CEffectMesh::COLOR_KEYFRAME> newColoreKeyFrames;

    for (_int i = 0; i < _iColorSetCnt; ++i)
    {
        CEffectMesh::COLOR_KEYFRAME newColorKey = {};

        newColorKey.fTime = _fColorTime[i];
        newColorKey.eEaseType = _iColorEase[i];

        memcpy(newColorKey.vColor, &_vColor[i], sizeof(_float4) * 4);

        newColoreKeyFrames.push_back(newColorKey);
    }

    m_eEffectDesc.KeyFrames.ColorKeyFrames = newColoreKeyFrames;

    Reset();
}

void CTestEffectMesh::SetMaskInfo(_bool _bUseMask, string _strMaskKey, _int _iMaskSamplerType,
    _int _iMaskDiscardArea, _float _fMaskDiscardValue, _int _iMaskUVType,
    _float2 _vMaskUVScroll, _float2 _vMaskUVAtlasCnt, _float _fMaskUVAtalsSpeed)
{
    m_eEffectDesc.bUseMask = _bUseMask;
    m_strTextureKey[TEX_MASK] = _strMaskKey;
    m_eEffectDesc.iMaskSamplerType = _iMaskSamplerType;
    m_eEffectDesc.iMaskDiscardArea = _iMaskDiscardArea;
    m_eEffectDesc.fMaskDiscardValue = _fMaskDiscardValue;
    m_eEffectDesc.iMaskUVType = _iMaskUVType;
    m_eEffectDesc.fMaskUVScroll = _vMaskUVScroll;
    m_eEffectDesc.iMaskUVAtlasCnt = _vMaskUVAtlasCnt;
    m_eEffectDesc.fMaskUVAtlasSpeed = _fMaskUVAtalsSpeed;

    Reset();
}

void CTestEffectMesh::SetNoiseInfo(_bool _bUseNoise, string _strNoiseKey,
    _int _iNoiseSamplerType, _int _iNoiseDiscardArea, _float _fNoiseDiscardValue,
    _int _iNoiseUVType, _float2 _vNoiseUVScroll, _float2 _vNoiseUVAtlasCnt, _float _fNoiseUVAtalsSpeed)
{
    m_eEffectDesc.bUseNoise = _bUseNoise;
    m_strTextureKey[TEX_NOISE] = _strNoiseKey;
    m_eEffectDesc.iNoiseSamplerType = _iNoiseSamplerType;
    m_eEffectDesc.iNoiseDiscardArea = _iNoiseDiscardArea;
    m_eEffectDesc.fNoiseDiscardValue = _fNoiseDiscardValue;
    m_eEffectDesc.iNoiseUVType = _iNoiseUVType;
    m_eEffectDesc.fNoiseUVScroll = _vNoiseUVScroll;
    m_eEffectDesc.iNoiseUVAtlasCnt = _vNoiseUVAtlasCnt;
    m_eEffectDesc.fNoiseUVAtlasSpeed = _fNoiseUVAtalsSpeed;

    Reset();
}

void CTestEffectMesh::SetScaleInfo(_bool _bScaleFollow, _int _iScaleFollowType,
    string _strFollowBoneName, _int _iScaleCnt, _float* _fScaleTime, _float _fScale[5][3], _int* _iScaleEasing)
{
    m_eEffectDesc.iScaleFollowType = _bScaleFollow ? _iScaleFollowType : -1;
    
    m_eEffectDesc.strScaleFollowBoneKey = _strFollowBoneName;
    if (m_pOwner.lock() && _iScaleFollowType == 1)
        m_pSFollowBoneMat = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr(m_eEffectDesc.strScaleFollowBoneKey.c_str());

    vector<CEffectMesh::SCALE_KEYFRAME> newScaleKeyFrames;

    for (_int i = 0; i < _iScaleCnt; ++i)
    {
        CEffectMesh::SCALE_KEYFRAME newScaleKey = {};

        newScaleKey.fTime = _fScaleTime[i];
        newScaleKey.vScale = _float3(_fScale[i][0], _fScale[i][1], _fScale[i][2]);
        if (newScaleKey.vScale.x == 0.f)
            newScaleKey.vScale.x = 1.f;
        if (newScaleKey.vScale.y == 0.f)
            newScaleKey.vScale.y = 1.f;
        if (newScaleKey.vScale.z == 0.f)
            newScaleKey.vScale.z = 1.f;
        newScaleKey.eEaseType = _iScaleEasing[i];

        newScaleKeyFrames.push_back(newScaleKey);
    }

    m_eEffectDesc.KeyFrames.ScaleKeyFrames = newScaleKeyFrames;

    m_bSetScale = true;

    Reset();
}

void CTestEffectMesh::SetRotInfo(_bool _bRotationFollow, _int _iRotationFollowType, string _strFollowBoneName, _float3 _fRotate, _float3 _fTurnVel, _float3 _fTurnAcc)
{
    m_eEffectDesc.iRotFollowType = _iRotationFollowType;

    if (_iRotationFollowType != -1 && _bRotationFollow)
        m_eEffectDesc.iRotFollowType += 2;

    m_pTransformCom->RotationAll(0.f, 0.f, 0.f);

    m_eEffectDesc.strRotFollowBoneKey = _strFollowBoneName;
    if (m_pOwner.lock() && _iRotationFollowType == 1)
        m_pRFollowBoneMat = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr(m_eEffectDesc.strRotFollowBoneKey.c_str());

    if (m_eEffectDesc.vRotation != _fRotate)
    {
        m_eEffectDesc.vRotation = _fRotate;
        m_pTransformCom->RotationAll(XMConvertToRadians(m_eEffectDesc.vRotation.x), XMConvertToRadians(m_eEffectDesc.vRotation.y), XMConvertToRadians(m_eEffectDesc.vRotation.z));
    }

    m_eEffectDesc.vTurnVel = _fTurnVel;
    m_eEffectDesc.vTurnAcc = _fTurnAcc;

    Reset();
}

void CTestEffectMesh::SetTransInfo(_bool _bTransFollow, _int _iTransFollowType, string _strFollowBoneName, _int _iTransCnt, _float* _fTransTime, _float _fTrans[5][3], _int* _iTransEasing)
{
    m_eEffectDesc.iTransFollowType = _iTransFollowType;

    if (_iTransFollowType!= -1 && _bTransFollow)
        m_eEffectDesc.iTransFollowType += 2;

    m_eEffectDesc.strTransFollowBoneKey = _strFollowBoneName;
    if (m_pOwner.lock() && _iTransFollowType == 1)
        m_pTFollowBoneMat = dynamic_pointer_cast<CModel>(m_pOwner.lock()->GetComponent(L"Com_Model"))->GetCombinedBoneMatrixPtr(m_eEffectDesc.strTransFollowBoneKey.c_str());

    vector<CEffectMesh::TRANS_KEYFRAME> newTransKeyFrames;

    for (_int i = 0; i < _iTransCnt; ++i)
    {
        CEffectMesh::TRANS_KEYFRAME newTransKey = {};

        newTransKey.fTime = _fTransTime[i];
        newTransKey.vTranslation = _float3(_fTrans[i][0], _fTrans[i][1], _fTrans[i][2]);
        newTransKey.eEaseType = _iTransEasing[i];

        newTransKeyFrames.push_back(newTransKey);
    }

    m_eEffectDesc.KeyFrames.TransKeyFrames = newTransKeyFrames;

    m_bSetTrans = true;

    Reset();
}

void CTestEffectMesh::SetOwnerInfo(shared_ptr<CGameObject> _pOwner)
{

}

void CTestEffectMesh::SetUseSoftShader(_bool _bUseSoftEffect)
{
    if (_bUseSoftEffect)
        m_eEffectDesc.iRenderGroup = CRenderer::RENDER_GLOW;

    else
        m_eEffectDesc.iRenderGroup = CRenderer::RENDER_BLEND;
}

void CTestEffectMesh::SetShaderGlowInfo(_bool _bUseGlowColor, _int _iGlowColorSetCnt, _float* _fGlowColorTime, _float _vGlowColor[5][4])
{
    m_eEffectDesc.bUseGlowColor = _bUseGlowColor;
    m_eEffectDesc.iGlowColorSetCnt = _iGlowColorSetCnt;

    vector<CEffectMesh::GLOWCOLOR_KEYFRAME> newGlowColoreKeyFrames;

    for (_int i = 0; i < _iGlowColorSetCnt; ++i)
    {
        CEffectMesh::GLOWCOLOR_KEYFRAME newGlowColorKey = {};

        newGlowColorKey.fTime = _fGlowColorTime[i];

        newGlowColorKey.vGlowColor = _float4(_vGlowColor[i][0], _vGlowColor[i][1], _vGlowColor[i][2], _vGlowColor[i][3]);

        newGlowColoreKeyFrames.push_back(newGlowColorKey);
    }

    m_eEffectDesc.KeyFrames.GlowColorKeyFrames = newGlowColoreKeyFrames;

    Reset();
}

void CTestEffectMesh::SetDistiortion(_bool _bUseDistortion, string _strDistortionKey, _float _fDistortionSpeed, _float _fDistortionWeight)
{
    m_eEffectDesc.bUseDistortion = _bUseDistortion;
    m_eEffectDesc.strDistortionKey = _strDistortionKey;
    m_eEffectDesc.fDistortionSpeed = _fDistortionSpeed;
    m_eEffectDesc.fDistortionWeight = _fDistortionWeight;

    Reset();
}

void CTestEffectMesh::SetDissolveT(_bool _bUseDissolve, _int _iDslvType, _float _fDslvStartTime, _float _fDslvDuration, _float _fDslvThick, _float4 _vDslvColor, string _strDslvTexKey, _bool _bDslvGlow, _float4 _vDslvGlowColor)
{
    m_eEffectDesc.bUseDslv = _bUseDissolve;
    m_eEffectDesc.iDslvType = _iDslvType;
    m_eEffectDesc.fDslvStartTime = _fDslvStartTime;
    m_eEffectDesc.fDslvDuration = _fDslvDuration;
    m_eEffectDesc.fDslvThick = _fDslvThick;
    m_eEffectDesc.vDslvColor = _vDslvColor;
    m_eEffectDesc.bDslvGlow = _bDslvGlow;
    m_eEffectDesc.vDslvGlowColor = _vDslvGlowColor;
    m_eEffectDesc.strDslvTexKey = _strDslvTexKey;

    Reset();
}

void CTestEffectMesh::Reset()
{
    m_fActiveTime = 0.f;

    m_iDiffuseUVIdx = 0;
    m_vDiffuseUVScroll = { 0.f, 0.f };
    m_vDiffuseUVPivot = { 0.f, 0.f };

    m_iMaskUVIdx = 0;
    m_vMaskUVScroll = { 0.f, 0.f };
    m_vMaskUVPivot = { 0.f, 0.f };

    m_iNoiseUVIdx = 0;
    m_vNoiseUVScroll = { 0.f, 0.f };
    m_vNoiseUVPivot = { 0.f, 0.f };

    m_bPlaying = false;

    m_iCurKeyFrameIdx = 0;
    m_fDistortionTimer = 0.f;

    m_bFirstTrans = true;
    m_bFirstRot = true;
    m_bFirstScale = true;
    
    m_bDslv = false;

    // Dslv In으로 시작하기
    if (m_eEffectDesc.bUseDslv && m_eEffectDesc.iDslvType == 0)
    {
        SetDissolve(true, false, m_eEffectDesc.fDslvDuration);
        SetDissolveInfo(m_eEffectDesc.fDslvThick, m_eEffectDesc.vDslvColor, m_eEffectDesc.strDslvTexKey.c_str());
    }
}
