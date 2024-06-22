#include "Monster.h"
#include "Model.h"
#include "Weapon.h"
#include "RigidBody.h"
#include "UIText.h"
#include "UIMgr.h"
#include "UIBar.h"
#include "EffectMgr.h"
#include "UIInstanceWorldHP.h"
#include "PlayerProjectile.h"

#include "GameInstance.h"
#include "VIInstancePoint.h"

CMonster::CMonster()
{

}

CMonster::~CMonster()
{

}

HRESULT CMonster::Initialize(MONSTER_DESC* _pMonsterDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    __super::Initialize(_pTransformDesc);

    m_pTarget = _pMonsterDesc->pTarget;

    m_pTransformCom->SetState(CTransform::STATE_POSITION, m_vCurrentPos);

    return S_OK;
}

void CMonster::PriorityTick(_float _fTimeDelta)
{
    if (m_isSpawn) {
        m_pRigidBody->SimulationOn();
        m_pTransformCom->SetState(CTransform::STATE_POSITION, m_vSpwnPos);
        m_pTransformCom->LookAtDir(_float3(m_vLookPos.x, m_vSpwnPos.y, m_vLookPos.z) -
            _float3(m_vSpwnPos.x, m_vSpwnPos.y, m_vSpwnPos.z));
        m_isSpawn = false;
    }

    for (auto& Pair : m_pMonsterParts)
        (Pair.second)->PriorityTick(_fTimeDelta);

    m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
    _float3 vLookXZ = _float3(m_pTransformCom->GetState(CTransform::STATE_LOOK).x, 0.f,
        m_pTransformCom->GetState(CTransform::STATE_LOOK).z);
    vLookXZ.Normalize(m_vNormLookDirXZ);
    m_vTargetPos = m_pTarget->GetTransform()->GetState(CTransform::STATE_POSITION);
    
    _float2 vTargetPosXZ = _float2(m_vTargetPos.x, m_vTargetPos.z);
    _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);
    m_fDistanceToTarget = (vTargetPosXZ - vCurrentPosXZ).Length();

    m_vTargetDir = m_vTargetPos - m_vCurrentPos;
    _float3 vTargetDirXZ = _float3(m_vTargetDir.x, 0.f, m_vTargetDir.z);
    vTargetDirXZ.Normalize(m_vNormTargetDirXZ);
}

void CMonster::Tick(_float _fTimeDelta)
{
    for (auto& Pair : m_pMonsterParts)
        (Pair.second)->Tick(_fTimeDelta);

    if (m_pHPBar != nullptr)
    {
        UpdateUIPos();
        m_pHPBar->Tick(_fTimeDelta);
    }
}

void CMonster::LateTick(_float _fTimeDelta)
{
    for (auto& Pair : m_pMonsterParts)
        (Pair.second)->LateTick(_fTimeDelta);

    if (m_pHPBar != nullptr)
    {
        m_pHPBar->LateTick(_fTimeDelta);
        UpdateHPBar();
    }
    if(m_IsEdgeRender)
        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_EDGE, shared_from_this());
}

HRESULT CMonster::Render()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    if (m_bUsingRimLight)
        UsingRimLight(_float3(1.f, 1.f, 1.f), 10.f);

    return S_OK;
}

HRESULT CMonster::RenderShadow()
{
    GAMEINSTANCE->ReadyShader(m_eShaderType, 3);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
    GAMEINSTANCE->BindLightProjMatrix();
    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++) 
    {

        GAMEINSTANCE->BeginAnimModel(m_pModelCom, i);
    }

    return S_OK;
}

HRESULT CMonster::RenderEdgeDepth()
{
    // 외곽선을 위한 깊이 그리기
    GAMEINSTANCE->ReadyShader(m_eShaderType, 5);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        GAMEINSTANCE->BeginAnimModel(m_pModelCom, i);
    }

    return S_OK;
}

HRESULT CMonster::RenderEdge()
{
    // 외곽선을 위한 노말 늘린 포지션 그리기
    GAMEINSTANCE->ReadyShader(m_eShaderType, 8);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    if (FAILED(GAMEINSTANCE->BindRawValue("g_fLineThick", &m_fLineThick, sizeof(_float))))
        return E_FAIL;

    if (FAILED(GAMEINSTANCE->BindRawValue("g_vLineColor", &m_vLineColor, sizeof(_float4))))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        GAMEINSTANCE->BeginAnimModel(m_pModelCom, i);
    }

    return S_OK;
}

_float4x4& CMonster::GetCombinedBoneMat(const string& _boneName)
{
    return *m_pModelCom->GetCombinedBoneMatrixPtr(_boneName.c_str());
}

void CMonster::SpawnMoster(_float3 _vPosition, _float3 _vLookPos)
{
    m_isSpawn = true;
    m_IsEnabled = true;
    m_vSpwnPos = _vPosition;
    m_vLookPos = _vLookPos;
}

void CMonster::SimulationOn()
{
    m_pRigidBody->SimulationOn();
}

void CMonster::SimulationOff()
{
    m_pRigidBody->SimulationOff();
}

void CMonster::UpdateDirection()
{
    m_pTransformCom->LookAt(_float3(m_vTargetPos.x, m_vCurrentPos.y, m_vTargetPos.z));
}

void CMonster::MoveToTarget(_float _fTimeDelta)
{
    m_pTransformCom->GoStraight(m_fSpeed * _fTimeDelta);
}

_float CMonster::UpdateDistanceToCerPos(_float3 _vCerPos)
{
    m_vCurrentPos = _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION));
    
    _float2 vCerPosXZ = _float2(_vCerPos.x, _vCerPos.z);
    _float2 vCurrentPosXZ = _float2(m_vCurrentPos.x, m_vCurrentPos.z);

    return (vCerPosXZ - vCurrentPosXZ).Length();
}

void CMonster::MovePos(_float3 _vPos)
{
    m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPos);
    m_vCurrentPos = _vPos;
}

void CMonster::MovePosXZ(_float3 _vPos)
{
    m_pTransformCom->SetPositionParam(0, _vPos.x);
    m_pTransformCom->SetPositionParam(2, _vPos.z);
}

void CMonster::OnCollisionWeapon()
{
    if (dynamic_pointer_cast<CWeapon>(m_pMonsterParts.begin()->second))
        dynamic_pointer_cast<CWeapon>(m_pMonsterParts.begin()->second)->SetCollsionActive(true);
    else
        return;
}

void CMonster::OffCollisionWeapon()
{
    if (dynamic_pointer_cast<CWeapon>(m_pMonsterParts.begin()->second))
        dynamic_pointer_cast<CWeapon>(m_pMonsterParts.begin()->second)->SetCollsionActive(false);
    else
        return;
}

void CMonster::SetWeaponRimLight(_bool _bOn, _float3 _fRimLight, _float _fPow)
{
    if (_bOn)
    {
        dynamic_pointer_cast<CWeapon>(m_pMonsterParts.find("Part_Weapon")->second)->OnRimLight(_fRimLight, _fPow);
    }
    else
        dynamic_pointer_cast<CWeapon>(m_pMonsterParts.find("Part_Weapon")->second)->OffRimLight();
}

void CMonster::DamagedByPlayer(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    // 대검 공격
    // 일반 공격
    if (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_1" || _ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_2" ||
        _ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_3")
    {
        m_fHp -= 5.f;
    }
    else if (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_PowerAttack")
    {
        m_fHp -= 10.f;
    }
    // 스킬1
    else if (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack" || _ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack2")
    {
        m_fHp -= 7.f;
    }
    else if (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack3")
    {
        m_fHp -= 11.f;
    }
    // 스킬2
    else if (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_CM_SW2_DashAttack")
    {
        m_fHp -= 10.f;
    }
    // 스킬3
    else if (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_GaiaCrash_Legend")
    {
        m_fHp -= 25.f;
    }
    // 스킬4
    else if (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_UpperAttack")
    {
        m_fHp -= 20.f;
    }
    // 스킬5
    else if (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_1" || _ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_2" ||
        _ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_3")
    {
        m_fHp -= 6.f;
    }
    // 필살기
    else if (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_ActionSkill_ContextSkill")
    {
        m_fHp -= 200.f;
    }

    // 석궁공격
    // 일반공격, 1번스킬, 2번스킬
    if (_ColData.szShapeTag == "Player_Arrow_Basic")
    {
        PlayerProjAbnormal iAbnormal = dynamic_pointer_cast<CPlayerProjectile>(_ColData.pGameObject.lock())->GetAbnormal();

        if (iAbnormal == PLAYER_PJ_A_STIFF_SHORT)
        {
            m_fHp -= 10.f;
        }
        else
        {
            m_fHp -= 5.f;
        }
    }
    // 3번스킬
    else if (_ColData.szShapeTag == "Player_Arrow_Curve")
    {
        m_fHp -= 2.f;
    }
    // 4번스킬
    else if (_ColData.szShapeTag == "Player_Arrow_Electric")
    {
        m_fHp -= 25.f;
    }
    // 5번스킬
    else if (_ColData.szShapeTag == "Player_Arrow_FuriousFire")
    {
        m_fHp -= 2.f;
    }

    // 스태프 공격
    // 일반 공격
    if (_ColData.szShapeTag == "Player_Magic_Bullet")
    {
        m_fHp -= 8.f;
    }
    // 스킬 1
    else if (_ColData.szShapeTag == "Player_Spell_FireBall_Explosion")
    {
        m_fHp -= 5.f;
    }
    // 스킬 2
    else if (_ColData.szShapeTag == "Player_Spell_IceSpear")
    {
        m_fHp -= 10.f;
    }
    // 스킬 3
    else if (_ColData.szShapeTag == "Player_Spell_FireBlast")
    {
        m_fHp -= 20.f;
    }
    // 스킬 4
    else if (_ColData.szShapeTag == "Player_Spell_LightnigJudgment")
    {
        m_fHp -= 30.f;
    }
    // 스킬 5
    else if (_ColData.szShapeTag == "Player_Spell_Meteor")
    {
        m_fHp -= 200.f;
    }
}

HRESULT CMonster::PrepareMonsterHPBar(const wstring& _strMonsterTag)
{
    weak_ptr<CCustomFont> pFont = GAMEINSTANCE->FindFont("Font_AdenL");
    if (!pFont.expired())
    {
        wstring strScript = _strMonsterTag;

        RECT measuredRect = pFont.lock()->GetFont()->MeasureDrawBounds(strScript.c_str(), _float2(0.f, 0.f));
        _float fSizeX = (float)(measuredRect.right - measuredRect.left) + 5.f;
        _float fSizeY = (float)(measuredRect.bottom - measuredRect.top);

        CUIBase::UIDesc uiDesc{};
        uiDesc.strTag = "MonsterName";
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
        UIMGR->LoadCertainUI("InstanceMonsterHPPanel");
        shared_ptr<CUIBase> hpPanel = UIMGR->FindUI("MonsterHPPanel");
        if (hpPanel == nullptr)
            return E_FAIL;

        hpPanel->AddUIChild("MonsterName", pUI);
        pUI->AddUIParent(hpPanel);

        //m_pHPBar = pUI;
        m_pHPBar = hpPanel;
        hpPanel->SaveAllChildren(m_pHPUIs);
        m_pHPBar->SetIsWorldUI(true);
        m_pHPBar->SetIsRender(false);

        UIMGR->DeleteSelectedUI("MonsterHPPanel");
    }

    return S_OK;
}

void CMonster::UpdateUIPos()
{
    //_float4x4 monWorldMat = *m_pModelCom->GetCombinedBoneMatrixPtr("Bip001-Head") * m_pTransformCom->GetWorldMatrix();
    _float4x4 monWorldMat = m_pTransformCom->GetWorldMatrix();

    /* 몬스터 마다 위치를 조금씩 조절 할 필요성 있을지도? */
    _float3 bonePos = monWorldMat.Translation() + _float3(0.f, GetLockOnOffSetPosition().y + 2.f, 0.f);
    bonePos = SimpleMath::Vector3::Transform(bonePos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
    bonePos = SimpleMath::Vector3::Transform(bonePos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

    /* w나누기 필요없겠지? */
    if (bonePos.z < 0.f || bonePos.z > 1.f)
    {
       // bonePos.x = 100000.f;
       // bonePos.y = 100000.f;

        //m_pHPBar->SetUIPos(bonePos.x, bonePos.y);
        //m_pHPBar->DeactivateUI();
    }

    else
    {
        _float posX = (bonePos.x + 1.f) * 0.5f;
        _float posY = (bonePos.y - 1.f) * -0.5f;

        //posY = 1.f - posY;

        posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
        posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

        //m_pHPBar->ActivateUI();
        m_pHPBar->SetUIPos(posX, posY);
    }

    //m_fX - (g_iWinSizeX * 0.5f), -m_fY + (g_iWinSizeY * 0.5f)

}

void CMonster::UpdateHPBar()
{
    float fRatio = m_fHp / m_fMaxHp;
    string hpBarTag = "MonsterHP";
    auto it = find_if(m_pHPUIs.begin(), m_pHPUIs.end(), [&hpBarTag](shared_ptr<CUIBase> pUI) {
        if (pUI->GetUITag() == hpBarTag)
            return true;

        return false;
        });

    if (it != m_pHPUIs.end())
    {
        shared_ptr<CUIInstanceWorldHP> uiBar = dynamic_pointer_cast<CUIInstanceWorldHP>(*it);
        if (uiBar != nullptr)
            uiBar->SetRatio(fRatio);
    }
}

void CMonster::ToggleHPBar()
{
    m_pHPBar->SetIsRender(true);
}

void CMonster::ToggleOffHPBar()
{
    m_pHPBar->SetIsRender(false);
}

void CMonster::ShowEffect(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{    
    // 대검
    {
        // 일반 피격 왼쪽
        if ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_1") ||
            (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack"))
        {
            EFFECTMGR->PlayEffect("Hit_Left", shared_from_this());
            EFFECTMGR->PlayEffect("P_Hu_M_SW2_Act_Flinching_B", shared_from_this());
        }
        // 일반 피격 오른쪽
        if ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_2") ||
            (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_BasicAttack_3") ||
            (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack2"))
        {
            EFFECTMGR->PlayEffect("Hit_Right", shared_from_this());
            EFFECTMGR->PlayEffect("P_Hu_M_SW2_Act_Flinching_B", shared_from_this());
        }
        // 짧은 경직 
        if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack3") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_CM_SW2_DashAttack") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_1") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_2") ||
                (_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_SpinAttack_3")))
        {
            EFFECTMGR->PlayEffect("Hit_ShortStun", shared_from_this());

            if ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_MainAttack3"))
                EFFECTMGR->PlayEffect("Hit_GlowHaze", shared_from_this());
        }

        // 넘어뜨리기
        if ((_ColData.eColFlag & COL_PLAYERWEAPON) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_PowerAttack")))
        {
            EFFECTMGR->PlayEffect("Hit_GlowHaze", shared_from_this());
            EFFECTMGR->PlayEffect("P_Hu_M_SW2_Act_Flinching_B", shared_from_this());
        }
        if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
            ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_UpperAttack")))
        {
            EFFECTMGR->PlayEffect("Hit_Plr_Sk_Upper", shared_from_this());
        }
        //if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
        //    ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_ActionSkill_ContextSkill")))
        //{
        //    EFFECTMGR->PlayEffect("Hit_Plr_Sk_Upper", shared_from_this());
        //}
    }

    // 석궁
    {
        if (_ColData.szShapeTag == "Player_Arrow_Basic")
        {
            EFFECTMGR->PlayEffect("P_Hu_M_SW2_Act_Flinching_B", shared_from_this());
        }
        if (_ColData.szShapeTag == "Player_Arrow_Curve")
        {
            EFFECTMGR->PlayEffect("P_Hu_M_SW2_Act_Flinching_B", shared_from_this());
            EFFECTMGR->PlayEffect("Hit_M_Base_Yellow", shared_from_this());
        }
        if (_ColData.szShapeTag == "Player_Arrow_Electric")
        {
            EFFECTMGR->PlayEffect("P_Hu_M_SW2_Act_Flinching_B", shared_from_this());
            EFFECTMGR->PlayEffect("Hit_M_Base_Green", shared_from_this());
            EFFECTMGR->PlayEffect("Ab_Shock", shared_from_this());
        }
        if (_ColData.szShapeTag == "Player_Arrow_FuriousFire")
        {
            EFFECTMGR->PlayEffect("P_Hu_M_SW2_Act_Flinching_B", shared_from_this());
            EFFECTMGR->PlayEffect("Hit_ShortStun", shared_from_this());
        }
    }

    // 스태프
    {
        // 평타
        if ((_ColData.szShapeTag == "Player_Magic_Bullet"))
            EFFECTMGR->PlayEffect("Hit_Staff_Base", shared_from_this());

        // 불꽃탄
        if ((_ColData.szShapeTag == "Player_Spell_FireBall"))
            EFFECTMGR->PlayEffect("Hit_Fire", shared_from_this());

        // 얼음창
        if ((_ColData.szShapeTag == "Player_Spell_IceSpear") || (_ColData.szShapeTag == "Player_Spell_FireBlast"))
            EFFECTMGR->PlayEffect("Hit_Ice", shared_from_this());

        //// 폭염의 파장
        //if ((_ColData.eColFlag & COL_PLAYERWEAPON) &&
        //    ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_PowerAttack")))
        //{
        //    EFFECTMGR->PlayEffect("Hit_GlowHaze", shared_from_this());
        //}
        //if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
        //    ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_UpperAttack")))
        //{
        //    EFFECTMGR->PlayEffect("Hit_Plr_Sk_Upper", shared_from_this());
        //}

        // 심판의 낙뢰
        if ((_ColData.szShapeTag == "Player_Spell_LightnigJudgment"))
            EFFECTMGR->PlayEffect("Hit_Monster_Shock", shared_from_this());

        //// 지옥불 운석
        //if ((_ColData.eColFlag & COL_PLAYERWEAPON) &&
        //    ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_PowerAttack")))
        //{
        //    EFFECTMGR->PlayEffect("Hit_GlowHaze", shared_from_this());
        //}
        //if ((_ColData.eColFlag & COL_PLAYERSKILL) &&
        //    ((_ColData.szShapeTag == "Armature|P_Hu_M_SW2_Skl_S_UpperAttack")))
        //{
        //    EFFECTMGR->PlayEffect("Hit_Plr_Sk_Upper", shared_from_this());
        //}
    }
}
