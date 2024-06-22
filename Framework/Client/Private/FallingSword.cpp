#include "FallingSword.h"
#include "Model.h"
#include "RigidBody.h"
#include "UIBar.h"
#include "UIMgr.h"

CFallingSword::CFallingSword()
{
}

CFallingSword::~CFallingSword()
{
}

HRESULT CFallingSword::Initialize(FALLINGSWORD_DESC* _pFallingSwordDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    PROJECTILE_DESC* pProjectileDesc = (PROJECTILE_DESC*)_pFallingSwordDesc;

    __super::Initialize(pProjectileDesc, _pTransformDesc);

    /*Shader Info*/
    m_eShaderType = ESHADER_TYPE::ST_NONANIM;
    m_iShaderPass = 0;

    m_pModelCom = GAMEINSTANCE->GetModel("N_Sword2h_UndeadArmyKnight");
    m_pTransformCom->SetScaling(10.f, 10.f, 15.f);
    m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), 3.14f);
    m_pTransformCom->RotationPlus(_float3(0.f, 1.f, 0.f), 3.14f/2.f);
    
    // 발사체 종류 정해주기
    m_iProjectileType = PJ_FALL;
    
    AddRigidBody();

    shared_ptr<CTransform::TRANSFORM_DESC> pTransformDesc = make_shared<CTransform::TRANSFORM_DESC>();
    pTransformDesc->fSpeedPerSec = 10.f;
    pTransformDesc->fRotationPerSec = XMConvertToRadians(90.0f);
    
    m_IsEnabled = false;
    
    //m_pRigidBody->GravityOn();

    // For. Glow
    m_bUseGlow = false;
    m_bUseGlowColor = true;
    m_vGlowColor = { 1.f, 0.004f, 0.29f, 0.5f };
    
    SimulationOff();
    
    /* For UI HPBar */
    if (FAILED(PrepareHPBar()))
        return E_FAIL;
    
    return S_OK;
}

void CFallingSword::PriorityTick(_float _fTimeDelta)
{
    __super::PriorityTick(_fTimeDelta);
}

void CFallingSword::Tick(_float _fTimeDelta)
{
    CheckCollisionActive();
     
    __super::Tick(_fTimeDelta);
    UpdateTargetPos();

    if (m_fHp <= 0.f && !m_bTargetOut)
    {
        m_bTargetOn = false;
        m_bTargetOut = true;
        //m_bUseGlow = false;
        StopEffect("Boss_FallingSword");
        GAMEINSTANCE->PlaySoundW("Final_SFX_SwordOut", 1.f);
    }

    if (m_bSwordAttack)
    {
        GAMEINSTANCE->PlaySoundW("Final_SFX_SwordOn", 1.f);
        m_bUseGlow = true;
        PlayEffect("Boss_FallingSword_After");
        m_bUseGlow = false;
        //m_vGlowColor = { 0.98f,  0.89f,  0.48f, 0.5f };
        m_bMoveState[0] = true;
        m_bSwordAttack = false;
        m_pRigidBody->GravityOff();
    }

    if (m_bRise)
    {
        m_pRigidBody->GravityOff();
        m_bRise = false;
        m_bRising = true;
    }
    if (m_bRising)
    {
        m_fCalculateTime += _fTimeDelta;
        MovePos(m_fRiseSpeed * _fTimeDelta * _float3(0.f, 1.f, 0.f) + m_vCurrentPos);

        if (m_vCurrentPos.y > 50.f)
        {
            m_fCalculateTime = 0.f;
            m_bRising = false;
            m_IsEnabled = false;
            SimulationOff();
        }
    }

    // 떨어지기 시작할때
    if (m_bStartFall)
    {
        m_vCurrentPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
        m_pTransformCom->SetPositionParam(1, (m_vCurrentPos.y - _fTimeDelta * 100.f));
        if (m_bFallGround)
        {
            m_bFallGround = false;
            m_bStartFall = false;
            // 떨어진 이펙트
            m_pRigidBody->GravityOn();
            EFFECTMGR->PlayEffect("Sword_Fall", _float3(m_pTransformCom->GetState(CTransform::STATE_POSITION).x, -30.8f, m_pTransformCom->GetState(CTransform::STATE_POSITION).z), false, 3.f);
            GAMEINSTANCE->PlaySoundW("Final_SFX_JumpAtt1", 1.f);
        }
    }

    // 빛나는 검이였던거는 천천히 위로 올라가기
    // 아닌 검은 위로 올라가 사라지기
    if (m_bMoveState[0])
    {
        m_fCalculateTime += _fTimeDelta;
        MovePos(m_fUpSpeed * _fTimeDelta * _float3(0.f, 1.f, 0.f) + m_vCurrentPos);
        // 2초간 올라가기
        if (m_fCalculateTime > 2.f)
        {
            m_fCalculateTime = 0.f;
            m_bMoveState[0] = false;
            m_bMoveState[1] = true;
        }
    }
    // 검이 올라가서 보스방향으로 회전하기
    if (m_bMoveState[1])
    {
        m_bMoveState[1] = false;
        m_bMoveState[2] = true;
        GetTransform()->SetSpeed(60.f);
    }

    // 검이 보스방향으로 움직이도록
    if (m_bMoveState[2])
    {
        m_fCalculateTime += _fTimeDelta;
        m_vTargetPos = m_vFallPos;
        if (m_fDistanceToTarget > 0.5f)
        {
            MoveToTarget(_fTimeDelta);
        }

        if (m_fCalculateTime > 2.5f)
        {
            m_fCalculateTime = 0.f;
            m_bStartRotation = true;
            GAMEINSTANCE->PlaySoundW("Final_SFX_SwordRotation", 1.f);
        }
        if (m_bStartRotation)
        {
            m_fCalculateTime += _fTimeDelta;
            switch (m_iSwordType)
            {
                // 좌
            case 1:
                m_pTransformCom->RotationPlus(_float3(0.f, 0.f, 1.f), 20.f * _fTimeDelta / 180.f * 3.14f);
                break;
                // 우
            case 2:
                m_pTransformCom->RotationPlus(_float3(0.f, 0.f, 1.f), -20.f * _fTimeDelta / 180.f * 3.14f);
                break;
                // 중앙
            case 3:
                break;
            }
            if (m_fCalculateTime > 1.5f)
            {
                m_bStartRotation = false;
                m_fCalculateTime = 0.f;
                m_bMoveState[2] = false;
                m_bMoveState[3] = true;
                GAMEINSTANCE->PlaySoundW("Final_SFX_Quake", 0.5f);
            }
        }
    }

    // 검 흔들리기
    if (m_bMoveState[3])
    {
        m_fCalculateTime += _fTimeDelta;
        
        // 아래로 흔들리기
        if (!m_vShakeUp)
        {
            m_fCalculateTime2 += _fTimeDelta;
            MovePos(m_vCurrentPos - _float3(0.f, m_fShakePower, 0.f) * _fTimeDelta);
            if (m_fCalculateTime2 > m_fShakeIntervel)
            {
                m_fCalculateTime2 = 0.f;
                m_vShakeUp = true;
            }
        }
        // 위로 흔들리기
        if (m_vShakeUp)
        {
            m_fCalculateTime2 += _fTimeDelta;
            MovePos(m_vCurrentPos + _float3(0.f, m_fShakePower, 0.f) * _fTimeDelta);
            if (m_fCalculateTime2 > m_fShakeIntervel)
            {
                m_fCalculateTime2 = 0.f;
                m_vShakeUp = false;
            }
        }

        if (m_fCalculateTime > 5.f)
        {
            m_fCalculateTime = 0.f;
            m_fCalculateTime2 = 0.f;
            m_vShakeUp = false;
            m_bMoveState[3] = false;
            m_bMoveState[4] = true;
        }
    }

    // 떨어지기
    if (m_bMoveState[4])
    {
        m_fCalculateTime += _fTimeDelta;

        switch (m_iSwordType)
        {
            // 좌
        case 1:
            if (m_fCalculateTime > 2.f)
            {
                m_bFallMove = true;
            }
            break;
            // 우
        case 2:
            if (m_fCalculateTime > 4.f)
            {
                m_bFallMove = true;
            }
            break;
            // 중앙
        case 3:
            if (m_fCalculateTime > 6.f)
            {
                m_bFallMove = true;
            }
            break;
        }

        if (m_bFallMove)
        {
            switch (m_iSwordType)
            {
                // 좌
            case 1:
                if (m_fCalculateTime > 2.f)
                {
                    MoveToTarget(_fTimeDelta * m_fMoveSpeed);
                }
                break;
                // 우
            case 2:
                if (m_fCalculateTime > 4.f)
                {
                    MoveToTarget(_fTimeDelta* m_fMoveSpeed);
                }
                break;
                // 중앙
            case 3:
                if (m_fCalculateTime > 6.f)
                {
                    MoveToTarget(_fTimeDelta* m_fMoveSpeed);
                }
                break;
            }
        }

        // 꽂히면
        if (m_vCurrentPos.y < -7.8f && m_iSwordType == 3)
        {
            m_bFallGround = true;
            m_bFallMove = false;
            m_bUseGlow = false;
            m_pRigidBody->SimulationOff();

            m_bMoveState[4] = false;

            //StopEffect("Boss_FallingSword_After");
        }
        if (m_vCurrentPos.y < -10.8f && (m_iSwordType == 1 || m_iSwordType == 2))
        {
            m_bFallGround = true;
            m_bFallMove = false;
            m_bUseGlow = false;
            m_pRigidBody->SimulationOff();

            m_bMoveState[4] = false;

            //StopEffect("Boss_FallingSword_After");
        }
    }
    
    m_pTransformCom->Tick(_fTimeDelta);

    if (m_pHPBar != nullptr)
    {
        UpdateUIPos();
        m_pHPBar->Tick(_fTimeDelta);
    }
}

void CFallingSword::LateTick(_float _fTimeDelta)
{
    m_pTransformCom->LateTick(_fTimeDelta);

    if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 25.f))
    {
		__super::LateTick(_fTimeDelta);

        GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody)
			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
    }

    if (m_pHPBar != nullptr)
    {
        if (!UIMGR->IsCameraDirecting())
        {
            m_pHPBar->LateTick(_fTimeDelta);
        }
        UpdateHPBar();
    }
}

HRESULT CFallingSword::Render()
{
    if (!m_IsEnabled)
        return S_OK;
    
    __super::Render();

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }
    
    return S_OK;
}

HRESULT CFallingSword::RenderGlow()
{
    if (!m_IsEnabled)
        return S_OK;

    _float fScale = 10.f;
    
    GAMEINSTANCE->ReadyShader(m_eShaderType, 4);
    GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

    GAMEINSTANCE->BindRawValue("g_bUseGlowColor", &m_bUseGlowColor, sizeof(_bool));
    GAMEINSTANCE->BindRawValue("g_vGlowColor", &m_vGlowColor, sizeof(_float4));
    GAMEINSTANCE->BindRawValue("g_fScale", &fScale, sizeof(_float));

    _uint iNumMeshes = m_pModelCom->GetNumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++) {
        GAMEINSTANCE->BeginNonAnimModel(m_pModelCom, (_uint)i);
    }

    GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);

    return S_OK;
}

HRESULT CFallingSword::RenderDistortion()
{
    return E_NOTIMPL;
}

shared_ptr<CFallingSword> CFallingSword::Create(FALLINGSWORD_DESC* _pFallingSwordDesc, CTransform::TRANSFORM_DESC* _pTransformDesc)
{
    shared_ptr<CFallingSword> pInstance = make_shared<CFallingSword>();

    if (FAILED(pInstance->Initialize(_pFallingSwordDesc, _pTransformDesc)))
        MSG_BOX("Failed to Create : FallingSword");

    return pInstance;
}

void CFallingSword::GravityOff()
{
    m_pRigidBody->GravityOff();
}

HRESULT CFallingSword::AddRigidBody()
{
    CRigidBody::RIGIDBODYDESC RigidBodyDesc;

    /* Create Actor */
//bool			isStatic = false;	/* 정적인 객체는 true */
//bool			isTrigger = false;	/* 트리거 객체는 true */
//bool			isGravity = true;		/* 중력을 적용 받는 객체는 true, (static 은 중력을 받지않음)*/
//bool			isInitCol = true;		/* 생성하자마자 충돌을 끄려면 false */
//_float4x4		WorldMatrix = XMMatrixIdentity();	/* Transform 꺼 통째로 던지면 됨 */
//UINT			eLock_Flag = ELOCK_FLAG::NONE; /* Static은 필요없음,CRigidBody::ELOCK_FLAG */
//
///* Create Shape */
//weak_ptr<CGameObject> pOwnerObject;				/* 소유 객체의 포인터 */
//float			fStaticFriction = 0.5f;		/* 정지 마찰력(0.0f~) */
//float			fDynamicFriction = 0.5f;		/* 운동 마찰력(0.0f~) */
//float			fRestitution = 0.0f;		/* 탄성력(0.0f~1.0f) */
//Geometry* pGeometry = nullptr;	/* 모양(GeometrySphere, GeometryBox, etc) */
//ECOLLISIONFLAG	eThisColFlag = COL_NONE;	/* 자기 자신의 충돌 Flag */
//UINT			eTargetColFlag = COL_NONE; /* 충돌할 객체의 Flag를 추가 ex) COL_MONSTER | COL_ITEM  */

    RigidBodyDesc.isStatic = false;
    RigidBodyDesc.isTrigger = false;
    RigidBodyDesc.isGravity = true;
    RigidBodyDesc.isInitCol = false;
    RigidBodyDesc.pTransform = m_pTransformCom;
    RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;

    RigidBodyDesc.pOwnerObject = shared_from_this();
    RigidBodyDesc.fStaticFriction = 1.0f;
    RigidBodyDesc.fDynamicFriction = 0.0f;
    RigidBodyDesc.fRestitution = 0.0f;

    GeometryCapsule CapsuleDesc;
    CapsuleDesc.fHeight = 16.f;
    CapsuleDesc.fRadius = 2.f;
    _float3 vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
    CapsuleDesc.vOffSetPosition = { 0.0f, 10.0f, 0.0f };
    CapsuleDesc.vOffSetRotation = { 0.0f, 0.0f, 90.0f };
    CapsuleDesc.strShapeTag = "FallingSword_Body";
    m_strShapeTag = "FallingSword_Body";
    RigidBodyDesc.pGeometry = &CapsuleDesc;
    
    RigidBodyDesc.eThisColFlag = COL_STATIC;
    RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_PLAYER | COL_MONSTER | COL_PLAYERWEAPON | COL_PLAYERSKILL | COL_MONSTERPROJECTILE;
    m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

    m_pTransformCom->SetRigidBody(m_pRigidBody);
    m_pRigidBody->SetMass(100.0f);

    return S_OK;
}

void CFallingSword::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
    // 검끼리 서로 충돌 안되게
    if (_ColData.eColFlag & COL_STATIC && _ColData.szShapeTag == "FallingSword_Body")
    {
        return;
    }

    // 처음 검이 떨어져 꽂혔을때
    if (_ColData.eColFlag & COL_STATIC && !m_bFallGround && m_bStartFall)
    {
        m_bFallGround = true;
    }

    // 밝게 빛났던게 지형에 떨어져 꽂혔을때
    if (_ColData.eColFlag & COL_STATIC && !m_bFallGround && m_bTargetOut)
    {
        //m_bFallGround = true;
    }
    
    // 보스 참격에 피격시
    if (_ColData.eColFlag & COL_MONSTERPROJECTILE && _ColData.szShapeTag == "Slash_Body")
    {
        if(m_bTargetOn)
            m_fHp -= 20.f;
    }
    if (_ColData.eColFlag & COL_MONSTERPROJECTILE && _ColData.szShapeTag == "LSlash_Body")
    {
        if (m_bTargetOn)
            m_fHp -= 20.f;
    }

    // 플레이어 스킬이나 공격에 피격시
    if (_ColData.eColFlag & COL_PLAYERSKILL || _ColData.eColFlag & COL_PLAYERWEAPON)
    {
        m_fHp -= 1.f;
    }
}

void CFallingSword::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CFallingSword::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

HRESULT CFallingSword::PrepareHPBar()
{
    CUIBase::UIDesc uiDesc{};
    uiDesc.strTag = "HPBar";
    uiDesc.eUILayer = UI_LAYER_HUD;
    uiDesc.eUIType = UI_BAR;
    uiDesc.eUITexType = UI_TEX_D;
    uiDesc.eUISlotType = SLOT_END;
    uiDesc.isMasked = false;
    uiDesc.isNoised = false;
    uiDesc.fDiscardAlpha = 0.1f;
    uiDesc.fZOrder = 0.1f;
    uiDesc.iShaderPassIdx = 2;

    CVIInstancing::InstanceDesc instanceDesc{};
    instanceDesc.vSize = _float2(167.f, 167.f);
    instanceDesc.fSizeY = 13.f;
    instanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);

    vector<string> texture;
    texture.emplace_back("Tex_Gauge_Casting_GaugeDisturb_0");
    m_pHPBar = CUIBar::Create(texture, "", "", uiDesc, instanceDesc);
    if (m_pHPBar == nullptr)
        return E_FAIL;

    m_pHPBar->SetIsWorldUI(true);

    return S_OK;
}

void CFallingSword::UpdateUIPos()
{
    //_float4x4 monWorldMat = m_pTransformCom->GetWorldMatrix();
    /* 몬스터 마다 위치를 조금씩 조절 할 필요성 있을지도? */
    //_float3 bonePos = monWorldMat.Translation();
    _float3 bonePos = m_pTransformCom->GetState(CTransform::STATE_POSITION) - _float3(0.f, 20.f, 0.f);
    bonePos = SimpleMath::Vector3::Transform(bonePos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
    bonePos = SimpleMath::Vector3::Transform(bonePos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));

    /* w나누기 필요없겠지? */
    if (bonePos.z < 0.f || bonePos.z > 1.f)
    {
        bonePos.x = 100000.f;
        bonePos.y = 100000.f;

        m_pHPBar->SetUIPos(bonePos.x, bonePos.y);
        m_pHPBar->DeactivateUI();
    }

    else
    {
        _float posX = (bonePos.x + 1.f) * 0.5f;
        _float posY = (bonePos.y - 1.f) * -0.5f;

        //posY = 1.f - posY;

        posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
        posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

        m_pHPBar->ActivateUI();
        m_pHPBar->SetUIPos(posX, posY);
    }
}

void CFallingSword::UpdateHPBar()
{
    _float fRatio = m_fHp / m_fMaxHP;

    m_pHPBar->SetRatio(fRatio);
}

void CFallingSword::PlayEffect(const string& _strEffectID)
{
    m_iEffectIdx = EFFECTMGR->PlayEffect(_strEffectID, shared_from_this());
}

void CFallingSword::StopEffect(const string& _strEffectID)
{
    EFFECTMGR->StopEffect(_strEffectID, m_iEffectIdx);
}
