#include "NPC.h"
#include "GameInstance.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "EffectMgr.h"
#include "UIMgr.h"
#include "UIInstanceWorldNPC.h"
#include "UIText.h"

CNPC::CNPC()
{
}

HRESULT CNPC::Initialize(_float3 _vPosition)
{
    __super::Initialize(nullptr);

    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);
    m_vCurrentPos = _vPosition;
    m_vFirstCreatePos = _vPosition;

    if (m_vInitLookPos != _float3(0.f, 0.f, 0.f))
    {
        m_pTransformCom->LookAtDir(_float3(m_vInitLookPos.x, _vPosition.y, m_vInitLookPos.z) -
            _float3(_vPosition.x, _vPosition.y, _vPosition.z));
    }

    m_fInteractDistance = 3.f;
    m_bInteractAble = false;

    m_vUIDistance = _float3(0.f, 1.4f, 0.f);

    return S_OK;
}

void CNPC::PriorityTick(_float _fTimeDelta)
{
    // 동물 스포너 용도
    if (m_isSpawn) {
        m_pRigidBody->SimulationOn();
        m_pTransformCom->SetState(CTransform::STATE_POSITION, m_vSpwnPos);
        if (m_vLookPos != _float3(0.f, 0.f, 0.f))
        {
            m_pTransformCom->LookAtDir(_float3(m_vLookPos.x, m_vSpwnPos.y, m_vLookPos.z) -
                _float3(m_vSpwnPos.x, m_vSpwnPos.y, m_vSpwnPos.z));
        }
        m_isSpawn = false;
    }

    __super::PriorityTick(_fTimeDelta);

    for (auto& Pair : m_pNPCParts)
        (Pair.second)->PriorityTick(_fTimeDelta);

    m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));

    m_PlayerPos = m_pPlayer.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);

    _float2 vPlayerPosXZ = _float2(m_PlayerPos.x, m_PlayerPos.z);
    _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
    m_fDistanceXZToPlayer = (vPlayerPosXZ - vCurrentPosXZ).Length();

    m_PlayerDir = m_PlayerPos - m_vCurrentPos;
}

void CNPC::Tick(_float _fTimeDelta)
{
    __super::Tick(_fTimeDelta);

    for (auto& Pair : m_pNPCParts)
        (Pair.second)->Tick(_fTimeDelta);

    /*for (auto& Pair : m_pNPCParts)
        (Pair.second)->Tick(_fTimeDelta);*/
    if (m_pNPCBar != nullptr)
    {
        UpdateUIPos();
        m_pNPCBar->Tick(_fTimeDelta);
    }
}

void CNPC::LateTick(_float _fTimeDelta)
{
    __super::LateTick(_fTimeDelta);

    for (auto& Pair : m_pNPCParts)
        (Pair.second)->LateTick(_fTimeDelta);

    /*for (auto& Pair : m_pNPCParts)
        (Pair.second)->LateTick(_fTimeDelta);*/

    if (m_pNPCBar != nullptr)
    {
        m_pNPCBar->LateTick(_fTimeDelta);
        if (m_IsInteractionOn)
            m_pNPCBar->SetIsRender(true);
        else
            m_pNPCBar->SetIsRender(false);
    }

    GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());

}

HRESULT CNPC::Render()
{
    /*GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());*/

    return S_OK;
}

HRESULT CNPC::RenderShadow()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, 3);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
    GAMEINSTANCE->BindLightProjMatrix();

    _uint iNumMeshes = m_pModel->GetNumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        GAMEINSTANCE->BeginAnimModel(m_pModel, i);
    }

    return S_OK;
}

void CNPC::SpawnAnimal(_float3 _vPosition, _float3 _vLookPos)
{
    m_isSpawn = true;
    m_IsEnabled = true;
    m_vSpwnPos = _vPosition;
    m_vLookPos = _vLookPos;
}

void CNPC::SimulationOn()
{
    m_pRigidBody->SimulationOn();
}

void CNPC::SimulationOff()
{
    m_pRigidBody->SimulationOff();
}

void CNPC::MovePos(_float3 _vPos)
{
    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPos);
    m_vCurrentPos = _vPos;
}

void CNPC::UpdateDirection()
{
    m_pTransformCom->LookAt(_float3(m_PlayerPos.x, m_vCurrentPos.y, m_PlayerPos.z));
}

void CNPC::MoveFront(_float _fTimeDelta)
{
    m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);
}

_bool CNPC::IsInPlayerDegree(_float _fDegree)
{
    _float2 vLookDir = _float2(m_pTransformCom->GetState(CTransform::STATE_LOOK).x, m_pTransformCom->GetState(CTransform::STATE_LOOK).z);
    _float2 vPlayerDir = _float2((m_pPlayer.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) - m_vCurrentPos).x,
        (m_pPlayer.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) - m_vCurrentPos).z);
    vPlayerDir.Normalize();

    _float fDot = vLookDir.Dot(vPlayerDir);

    if (cos(3.14f / 180.f * _fDegree) < fDot)
    {
        return true;
    }
    else
    {
        return false;
    }
}

_bool CNPC::IsPlayerRight()
{
    _float2 vLookXZ = _float2(m_pTransformCom->GetState(CTransform::STATE_LOOK).x, m_pTransformCom->GetState(CTransform::STATE_LOOK).z);
    _float2 vPlayerDir = _float2((m_pPlayer.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) - m_vCurrentPos).x,
        (m_pPlayer.lock()->GetTransform()->GetState(CTransform::STATE_POSITION) - m_vCurrentPos).z);
    vPlayerDir.Normalize();
    
    // 플레이어가 오른쪽
    if ((vPlayerDir.Cross(vLookXZ)).y > 0.f)
    {
        return true;
    }
    // 플레이어가 왼쪽
    if ((vPlayerDir.Cross(vLookXZ)).y < 0.f)
    {
        return false;
    }
}

void CNPC::SetWeaponRimLight(_bool _bOn, _float3 _fRimLight, _float _fPow)
{
    if (_bOn)
    {
        dynamic_pointer_cast<CWeapon>(m_pNPCParts.find("Part_Weapon")->second)->OnRimLight(_fRimLight, _fPow);
    }
    else
        dynamic_pointer_cast<CWeapon>(m_pNPCParts.find("Part_Weapon")->second)->OffRimLight();
}

void CNPC::InteractionOn()
{
}

_float4x4& CNPC::GetCombinedBoneMat(const string& _boneName)
{
    return *m_pModel->GetCombinedBoneMatrixPtr(_boneName.c_str());
}

HRESULT CNPC::PrepareNPCBar(const wstring& _strNPCTag)
{
    // Font_AdenL10
    weak_ptr<CCustomFont> pFont = GAMEINSTANCE->FindFont("Font_AdenL10");
    if (!pFont.expired())
    {
        wstring strScript = _strNPCTag;

        RECT measuredRect = pFont.lock()->GetFont()->MeasureDrawBounds(strScript.c_str(), _float2(0.f, 0.f));
        _float fSizeX = (float)(measuredRect.right - measuredRect.left) + 5.f;
        _float fSizeY = (float)(measuredRect.bottom - measuredRect.top);

        CUIBase::UIDesc uiDesc{};
        uiDesc.strTag = "NPCName";
        uiDesc.eUILayer = UI_POPUP_MOVABLE;
        uiDesc.eUIType = UI_TEXT;
        uiDesc.eUITexType = UI_TEX_D;
        uiDesc.eUISlotType = SLOT_END;
        uiDesc.isMasked = false;
        uiDesc.isNoised = false;
        uiDesc.fDiscardAlpha = 0.1f;
        uiDesc.fZOrder = 0.1f;
        uiDesc.iShaderPassIdx = 0;

        CVIInstancing::InstanceDesc instanceDesc{};
        instanceDesc.vSize = _float2(fSizeX, fSizeX);
        instanceDesc.fSizeY = fSizeY;

        vector<string> texture;
        texture.emplace_back("Tex_UI_SpecialShop_Frame_BG_Grow");
        shared_ptr<CUIText> pUI = CUIText::Create(texture, "", "", uiDesc, instanceDesc, pFont.lock(), strScript);

        if (pUI == nullptr)
            pUI.reset();

        // MonsterHPPanel
        UIMGR->LoadCertainUI("InstanceNPCPanel");
        shared_ptr<CUIBase> npcPanel = UIMGR->FindUI("InstanceNPCPanel");
        if (npcPanel == nullptr)
            return E_FAIL;

        npcPanel->AddUIChild("NPCName", pUI);
        pUI->AddUIParent(npcPanel);

        //m_pHPBar = pUI;
        m_pNPCBar = npcPanel;
        npcPanel->SaveAllChildren(m_pNPCs);
        m_pNPCBar->SetIsWorldUI(true);
        m_pNPCBar->SetIsRender(false);

        UIMGR->DeleteSelectedUI("InstanceNPCPanel");
    }

    return S_OK;
}

void CNPC::UpdateUIPos()
{
    _float4x4 monWorldMat = m_pTransformCom->GetWorldMatrix();
    /* 몬스터 마다 위치를 조금씩 조절 할 필요성 있을지도? */
    _float3 bonePos = monWorldMat.Translation() + _float3(0.f, 2.5f, 0.f);
    bonePos = SimpleMath::Vector3::Transform(bonePos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
    bonePos = SimpleMath::Vector3::Transform(bonePos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

    /* w나누기 필요없겠지? */
    if (bonePos.z < 0.f || bonePos.z > 1.f)
    {
        m_pNPCBar->SetIsRender(false);
    }

    else
    {
        _float posX = (bonePos.x + 1.f) * 0.5f;
        _float posY = (bonePos.y - 1.f) * -0.5f;

        //posY = 1.f - posY;

        posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
        posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

        m_pNPCBar->SetUIPos(posX, posY);
    }
}
