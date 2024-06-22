#include "EnterStatue.h"
#include "GameInstance.h"

#include "Model.h"
#include "EffectMgr.h"
#include "Component.h"

#include "DungeonDoor.h"
#include "UIMgr.h"

CEnterStatue::CEnterStatue()
{
}

HRESULT CEnterStatue::Initialize(_float3 _vPosition, _float _fAnswerDegree, _int _iStatueIdx)
{

    CTransform::TRANSFORM_DESC desc;
    desc.fRotationPerSec = 0.5f;
    desc.fSpeedPerSec = 0.f;

    m_fTurnSpeed = desc.fRotationPerSec;

    /*Create Component*/
    __super::Initialize(&desc);

    m_pModel = GAMEINSTANCE->GetModel("BG_Armillary_06_02_SM");

    m_pTransformCom->SetScaling(_float3(0.25f, 0.25f, 0.25f));
    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);

    m_pJewelModel = GAMEINSTANCE->GetModel("BG_Armillary_04_06_SM");
    m_pJewelTransform = CTransform::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo(), nullptr);

    m_pJewelTransform->SetScaling(_float3(0.2f, 0.2f, 0.2f));
    m_pJewelTransform->SetState(CTransform::STATE_POSITION, _vPosition);
    /*************************/

    m_iShaderPass = 0;
    m_eShaderType = ST_NONANIM;

    m_fAnswerDegree = XMConvertToRadians(_fAnswerDegree);
    m_iStatueIdx = _iStatueIdx;

    SetLaserEffect();

    if (0 == m_iStatueIdx) {

        m_IsEnergyOn = true;
        m_iFrontEffectIdx = EFFECTMGR->PlayEffect("Puzzle_Front", shared_from_this());

    }

    m_vOriginPos = _vPosition;

    return S_OK;
}

void CEnterStatue::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);

}

void CEnterStatue::Tick(_float _fTimeDelta)
{
    //__super::Tick(_fTimeDelta);

    if (GAMEINSTANCE->KeyPressing(DIK_F)) {

        if (m_IsInteractionOn && m_bInteractAble)
            InteractionOn();
    }

    /*Floating Height*/
    {
        _float3 vOriginPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
        _float3 vOriginJewelPos = m_pJewelTransform->GetState(CTransform::STATE_POSITION);

        _float fComputeSum = m_fFloatingDir * 0.2f * 0.016f;
        //* _fTimeDelta;

        m_fHeightSum += fComputeSum;
        _float3 vFinalPos;
        _float3 vFinalJewelPos;

        if (m_fHeightSum * m_fFloatingDir >= 0.3f) {

            vFinalPos = vOriginPos + _float3(0.f, (m_fHeightSum - 0.3f * m_fFloatingDir), 0.f);
            vFinalJewelPos = vOriginJewelPos + _float3(0.f, (m_fHeightSum - 0.3f * m_fFloatingDir), 0.f);

            m_fHeightSum = 0.f;
            m_fFloatingDir *= -1.f;

        }
        else {

            vFinalPos = vOriginPos + _float3(0.f, fComputeSum, 0.f);
            vFinalJewelPos = vOriginJewelPos + _float3(0.f, fComputeSum, 0.f);

        }

        m_pTransformCom->SetState(CTransform::STATE_POSITION, vFinalPos);
        m_pJewelTransform->SetState(CTransform::STATE_POSITION, vFinalJewelPos);
    }


    if (m_fAnswerDegree + 0.031f >= m_fCurrentDegree && m_fCurrentDegree >= m_fAnswerDegree - 0.031f) {


        if (m_IsEnergyOn) {
            _bool m_IsPrevState = m_IsConnected;
            m_IsConnected = true;

            if (m_IsPrevState != m_IsConnected) {

                m_iLaserEffectIdx = EFFECTMGR->PlayEffect(m_strEffectID, shared_from_this());

                m_pDoor.lock()->EnergyConnect(m_iStatueIdx + 1);
                GAMEINSTANCE->PlaySoundW("SFX_DungeonEnterLaser", 1.f);
            }
        }
    }
    else {
        _bool m_IsPrevState = m_IsConnected;
        m_IsConnected = false;

        if (m_IsPrevState != m_IsConnected) {
            m_pDoor.lock()->EnergyDisConnect(m_iStatueIdx + 1);
            EFFECTMGR->StopEffect(m_strEffectID, m_iLaserEffectIdx);

            GAMEINSTANCE->PlaySoundW("SFX_DungeonEnterLaserOff", 1.f);
        }
    }

}

void CEnterStatue::LateTick(_float _fTimeDelta)
{
    __super::LateTick(_fTimeDelta);
    GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
    GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());

}

HRESULT CEnterStatue::Render()
{

    /*Armillary*/
    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    _uint iNumMeshes = m_pModel->GetNumMeshes();


    for (size_t i = 0; i < iNumMeshes; i++) {

        GAMEINSTANCE->BeginNonAnimModel(m_pModel, (_uint)i);
    }

    /*Jewel*/

    GAMEINSTANCE->BindWVPMatrixPerspective(m_pJewelTransform->GetWorldMatrix());
  
    
    if(m_IsEnergyOn)
      UsingRimLight(_float3(0.2f, 0.5f, 1.f), 2.f);

    _uint iNumMeshesJewel = m_pJewelModel->GetNumMeshes();


    for (size_t i = 0; i < iNumMeshesJewel; i++) {

        GAMEINSTANCE->BeginNonAnimModel(m_pJewelModel, (_uint)i);
    }

    /*RimLight Off*/
    
    if (m_IsEnergyOn) {
        _bool bRimOff = false;
        GAMEINSTANCE->BindRawValue("g_IsUsingRim", &bRimOff, sizeof(bRimOff));
    }
   
    return S_OK;
}

HRESULT CEnterStatue::RenderShadow()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, 1);
    GAMEINSTANCE->BindLightProjMatrix();
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    _uint iNumMeshes = m_pModel->GetNumMeshes();


    for (size_t i = 0; i < iNumMeshes; i++) {

        GAMEINSTANCE->BeginNonAnimModel(m_pModel, (_uint)i);
    }

    /*Jewel*/

    GAMEINSTANCE->BindWVPMatrixPerspective(m_pJewelTransform->GetWorldMatrix());

    _uint iNumMeshesJewel = m_pJewelModel->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshesJewel; i++) {

        GAMEINSTANCE->BeginNonAnimModel(m_pJewelModel, (_uint)i);
    }

    return S_OK;
}

void CEnterStatue::InteractionOn()
{
    m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), 0.016f);
    m_fCurrentDegree += 0.016f * m_fTurnSpeed;

    // XMConvertToRadians(360.f) == 6.2831f
    if (m_fCurrentDegree >= 6.2831f) {
        m_fCurrentDegree -= 6.2831f;
    }
}

void CEnterStatue::SetEnergyOn(_bool _IsEnergyOn)
{

    m_IsEnergyOn = _IsEnergyOn;

    /*활성화*/
    if (_IsEnergyOn) {
        m_iFrontEffectIdx = EFFECTMGR->PlayEffect("Puzzle_Front", shared_from_this());
    }
    /*비활성화*/
    else {
        EFFECTMGR->StopEffect("Puzzle_Front", m_iFrontEffectIdx);
    }
}

void CEnterStatue::SetDungeonDoor(shared_ptr<CGameObject> _pDoor)
{
    m_pDoor = dynamic_pointer_cast<CDungeonDoor>(_pDoor);

}

void CEnterStatue::SetLaserEffect()
{
    if (0 == m_iStatueIdx) {
        m_strEffectID = "MiniGame_Razor_Blue0";
    }
    else if (1 == m_iStatueIdx) {
        m_strEffectID = "MiniGame_Razor_Blue1";

    }
    else if (2 == m_iStatueIdx) {
        m_strEffectID = "MiniGame_Razor_Blue2";

    }
    else if (3 == m_iStatueIdx) {
        m_strEffectID = "MiniGame_Razor_Blue3";

    }
    else if (4 == m_iStatueIdx) {
        m_strEffectID = "MiniGame_Razor_Blue4";


    }
    else if (5 == m_iStatueIdx) {
        m_strEffectID = "MiniGame_Razor_Blue5";

    }


}

shared_ptr<CEnterStatue> CEnterStatue::Create(_float3 _vPos, _float _fAnswerDegree , _int _iStatueIdx)
{
    shared_ptr<CEnterStatue> pStatue = make_shared<CEnterStatue>();

    if(FAILED(pStatue->Initialize(_vPos, _fAnswerDegree, _iStatueIdx)))
        MSG_BOX("Failed To Create : CEnterStatue");
      
    return pStatue;
}
