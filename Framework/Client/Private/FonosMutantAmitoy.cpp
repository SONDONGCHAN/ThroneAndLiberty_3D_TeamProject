//#include "pch.h"
//#include "FonosAmitoy.h"
//#include "Model.h"
//#include "Animation.h"
//#include "RigidBody.h"
//#include "CameraMgr.h"
//#include "CameraSpherical.h"
//
//#include "EffectMgr.h"
//
//CFonosMutantAmitoy::CFonosMutantAmitoy()
//	: CGameObject()
//{
//}
//
//CFonosMutantAmitoy::~CFonosMutantAmitoy()
//{
//}
//
//shared_ptr<CFonosMutantAmitoy> CFonosMutantAmitoy::Create()
//{
//	shared_ptr<CFonosMutantAmitoy> pInstance = make_shared<CFonosMutantAmitoy>();
//
//	if (FAILED(pInstance->Initialize()))
//	{
//		MessageBox(nullptr, L"Initialize Failed", L"CFonosMutantAmitoy::Create", MB_OK);
//		pInstance.reset();
//		return nullptr;
//	}
//
//	return pInstance;
//}
//
//HRESULT CFonosMutantAmitoy::Initialize()
//{
//	m_pTransformCom = CTransform::Create(GAMEINSTANCE->GetDeviceInfo(), GAMEINSTANCE->GetDeviceContextInfo());
//	m_pTransformCom->SetSpeed(4.f);
//
//	string ModelTag = "MD_Golem_Skin_001";
//	m_pModelCom = GAMEINSTANCE->GetModel(ModelTag);
//	AddComponent(L"Com_Model", m_pModelCom);
//
//	m_pModelCom->ChangeAnim(AMITOY_STATE_TEST, 0.1f, true);
//	//m_pModelCom->ChangeAnim(AMITOY_STATE_IDLE, 0.1f, true);
//
//	/*Shader Info*/
//	m_eShaderType = ESHADER_TYPE::ST_ANIM;
//	m_iShaderPass = 0;
//
//	AddRigidBody("Fonos_Amitoy");
//
//	shared_ptr<CCameraSpherical> pCamera = CCameraSpherical::Create();
//	if (pCamera)
//	{
//		pCamera->SetOwner(shared_from_this());
//		pCamera->SetOffSetPosition({ 0.0f,0.5f,0.0f });
//		pCamera->SetRadius(2.0f);
//		pCamera->SetOwnerLerpRatio(0.15f);
//		if (FAILED(GAMEINSTANCE->AddObject(LEVEL_FONOS, L"Layer_Camera", pCamera)))
//			return E_FAIL;
//		CCameraMgr::GetInstance()->AddCamera("FonosAmitoyCamera", pCamera);
//		m_pCamera = pCamera;
//	}
//
//	return S_OK;
//}
//
//void CFonosMutantAmitoy::PriorityTick(_float _fTimeDelta)
//{
//	TestKeyInput();
//
//	if (m_isInteraction && m_pModelCom->GetIsFinishedAnimation())
//	{
//		m_isInteraction = false;
//	}
//}
//
//void CFonosMutantAmitoy::Tick(_float _fTimeDelta)
//{
//	m_ePrevState = m_eCurState;
//	State_Tick(_fTimeDelta);
//
//	if (m_eCurState != m_ePrevState)
//	{
//		State_Out(_fTimeDelta);
//		State_In(_fTimeDelta);
//	}
//
//	m_pModelCom->PlayAnimation(_fTimeDelta);
//	m_pTransformCom->Tick(_fTimeDelta);
//}
//
//void CFonosMutantAmitoy::LateTick(_float _fTimeDelta)
//{
//	m_pModelCom->CheckDisableAnim();
//	m_pTransformCom->LateTick(_fTimeDelta);
//
//	// frustrum
//	if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 2.f))
//	{
//		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
//
//#ifdef _DEBUG
//		if (m_pRigidBody)
//			GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
//#endif // _DEBUG
//	}
//}
//
//HRESULT CFonosMutantAmitoy::Render()
//{
//	UINT iNumMeshes = m_pModelCom->GetNumMeshes();
//
//	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
//
//	for (UINT i = 0; i < iNumMeshes; i++)
//	{
//		GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
//		GAMEINSTANCE->BeginAnimModel(m_pModelCom, i);
//	}
//
//	return S_OK;
//}
//
//void CFonosMutantAmitoy::TestKeyInput()
//{
//
//	if (GAMEINSTANCE->KeyDown(DIK_LEFT))
//	{
//		m_eCurState = (AMITOY_STATE)(m_eCurState - 1 < 0 ? m_pModelCom->GetAnimations().size() - 1 : m_eCurState - 1);
//		m_pModelCom->ChangeAnim(m_eCurState, 0.1f, true);
//	}
//
//	if (GAMEINSTANCE->KeyDown(DIK_RIGHT))
//	{
//		m_eCurState = (AMITOY_STATE)(m_eCurState + 1 >= m_pModelCom->GetAnimations().size() ? 0 : m_eCurState + 1);
//		m_pModelCom->ChangeAnim(m_eCurState, 0.1f, true);
//	}
//
//	if (GAMEINSTANCE->KeyDown(DIK_R))
//		m_pTransformCom->SetState(CTransform::STATE_POSITION, { 55.7, 15.37, 104.62 });
//
//	if (GAMEINSTANCE->KeyDown(DIK_END))
//	{
//		shared_ptr<CGameObject> pPlr = GAMEINSTANCE->GetGameObject(LEVEL_STATIC, TEXT("Layer_Player"));
//		pPlr->SetEnable(true);
//
//		CCameraMgr::GetInstance()->CameraTurnOn("PlayerCamera");
//
//		m_IsEnabled = false;
//	}
//
//	if (GAMEINSTANCE->KeyDown(DIK_PGUP))
//	{
//		EFFECTMGR->PlayEffect("Trap_Stone_Target", m_pTransformCom->GetState(CTransform::STATE_POSITION), false, 3.f);
//	}
//}
//
//void CFonosMutantAmitoy::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
//{
//	if (_ColData.eColFlag & COL_TRAP)
//	{
//		if (m_bInvincible)
//			return;
//
//		if (_ColData.szShapeTag == "Fonos_Trap_Shock")
//			m_bShock = true;
//
//		if (_ColData.szShapeTag == "Jump_Mushroom_Head")
//			m_bSuperJump = true;
//	}
//}
//
//void CFonosMutantAmitoy::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
//{
//	if (_ColData.szShapeTag == "Fonos_Trap_Slow")
//		m_bSlow = true;
//}
//
//void CFonosMutantAmitoy::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
//{
//	if (_ColData.szShapeTag == "Fonos_Trap_Slow")
//		m_bSlow = false;
//}
//
//void CFonosMutantAmitoy::EnableCollision(const char* _strShapeTag)
//{
//	m_pRigidBody->EnableCollision(_strShapeTag);
//}
//
//void CFonosMutantAmitoy::DisableCollision(const char* _strShapeTag)
//{
//	m_pRigidBody->DisableCollision(_strShapeTag);
//}
//
//_bool CFonosMutantAmitoy::AbnormalCheck(float _fTimeDelta)
//{
//	if (m_bInvincible)
//	{
//		m_fInvTime -= _fTimeDelta;
//
//		if (0.f >= m_fInvTime)
//			m_bInvincible = false;
//
//		return false;
//	}
//
//	_bool bAbnormal = true;
//
//	if (m_bShock)
//		m_eCurState = AMITOY_STATE_SHOCK;
//	else if (m_bJumpBack)
//		m_eCurState = AMITOY_STATE_KNOCKBACK;
//	else if (m_bSuperJump)
//		m_eCurState = AMITOY_STATE_SUPERJUMP;
//	else
//		bAbnormal = false;
//
//	return bAbnormal;
//}
//
//CFonosMutantAmitoy::AMITOY_STATE CFonosMutantAmitoy::KeyInput(float _fTimeDelta)
//{
//	if (GAMEINSTANCE->KeyDown(DIK_E))
//	{
//		m_pTransformCom->Jump(1100.0f);
//	}
//
//	_uint	iMoveType = 0;
//	_bool	bValidMove = false;
//
//	if (GAMEINSTANCE->KeyPressing(DIK_W))
//		iMoveType |= 1;
//	if (GAMEINSTANCE->KeyPressing(DIK_A))
//		iMoveType |= 2;
//	if (GAMEINSTANCE->KeyPressing(DIK_S))
//		iMoveType |= 4;
//	if (GAMEINSTANCE->KeyPressing(DIK_D))
//		iMoveType |= 8;
//
//	if (0 == iMoveType)
//		return m_eCurState == AMITOY_STATE_SUPERJUMP ? AMITOY_STATE_SUPERJUMP : AMITOY_STATE_IDLE;
//
//	_float3 vLook(0.f, 0.f, 0.f);
//
//	// Move Control
//	switch (iMoveType)
//	{
//	case 1: // »ó
//		vLook = GAMEINSTANCE->GetCamLook();
//		bValidMove = true;
//		break;
//	case 4:	// ÇÏ
//		vLook = GAMEINSTANCE->GetCamLook() * -1.f;
//		bValidMove = true;
//		break;
//	case 2:	// ÁÂ
//		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(270.f)));
//		bValidMove = true;
//		break;
//	case 8: // ¿ì
//		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(90.f)));
//		bValidMove = true;
//		break;
//	case 3: // »óÁÂ
//		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(315.f)));
//		bValidMove = true;
//		break;
//	case 9: // »ó¿ì
//		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(45.f)));
//		bValidMove = true;
//		break;
//	case 6:	// ÇÏÁÂ
//		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(225.f)));
//		bValidMove = true;
//		break;
//	case 12:// ÇÏ¿ì
//		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(135.f)));
//		bValidMove = true;
//		break;
//	case 11:// ÁÂ»ó¿ì
//		vLook = GAMEINSTANCE->GetCamLook();
//		bValidMove = true;
//		break;
//	case 14:// ÁÂÇÏ¿ì
//		vLook = GAMEINSTANCE->GetCamLook() * -1.f;
//		bValidMove = true;
//		break;
//	case 7:	// »óÁÂÇÏ
//		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(270.f)));
//		bValidMove = true;
//		break;
//	case 13:// »ó¿ìÇÏ
//		vLook = XMVector3TransformNormal(GAMEINSTANCE->GetCamLook(), XMMatrixRotationY(XMConvertToRadians(90.f)));
//		bValidMove = true;
//		break;
//	}
//
//	if (!bValidMove)
//		return m_eCurState == AMITOY_STATE_SUPERJUMP ? AMITOY_STATE_SUPERJUMP : AMITOY_STATE_IDLE;
//
//	_float fRadian = XMVectorGetX(XMVector3Dot(m_pTransformCom->GetState(CTransform::STATE_LOOK), vLook));
//
//	if (-0.7 > fRadian)
//	{
//		vLook = XMVectorLerp(m_pTransformCom->GetState(CTransform::STATE_LOOK), vLook, 0.5f);
//	}
//	else if (-0.1 > fRadian)
//	{
//		vLook = XMVectorLerp(m_pTransformCom->GetState(CTransform::STATE_LOOK), vLook, 0.5f);
//	}
//	else
//	{
//		vLook = XMVectorLerp(m_pTransformCom->GetState(CTransform::STATE_LOOK), vLook, 0.5f);
//	}
//
//	m_pTransformCom->LookAtDirForLandObject(vLook);
//
//	if (AMITOY_STATE_SUPERJUMP == m_eCurState)
//	{
//		m_pTransformCom->GoStraight(_fTimeDelta);
//		return AMITOY_STATE_SUPERJUMP;
//	}
//
//	if (m_bSlow)
//		return	AMITOY_STATE_WALK;
//	else
//		return	AMITOY_STATE_RUN;
//}
//
//HRESULT CFonosMutantAmitoy::AddRigidBody(string _strModelKey)
//{
//	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
//
//	RigidBodyDesc.isStatic = false;
//	RigidBodyDesc.isTrigger = false;
//	RigidBodyDesc.isGravity = true;
//	RigidBodyDesc.isInitCol = true;
//	RigidBodyDesc.pTransform = m_pTransformCom;
//	RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;
//
//	RigidBodyDesc.pOwnerObject = shared_from_this();
//	RigidBodyDesc.fStaticFriction = 1.0f;
//	RigidBodyDesc.fDynamicFriction = 0.0f;
//	RigidBodyDesc.fRestitution = 0.0f;
//
//	GeometrySphere SphereDesc;
//	SphereDesc.fRadius = 0.25f;
//	SphereDesc.strShapeTag = _strModelKey.c_str();
//	SphereDesc.vOffSetPosition = { 0.0f, 0.1f,0.0f };
//	RigidBodyDesc.pGeometry = &SphereDesc;
//
//	RigidBodyDesc.eThisColFlag = COL_AMITOY;
//	RigidBodyDesc.eTargetColFlag = COL_STATIC | COL_TRIGGER | COL_TRAP | COL_AMITOY;
//	m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);
//	m_pRigidBody->SetMass(3.0f);
//
//	m_pTransformCom->SetRigidBody(m_pRigidBody);
//
//	return S_OK;
//}
//
//void CFonosMutantAmitoy::State_In(_float _fTimeDelta)
//{
//	switch (m_eCurState)
//	{
//	case AMITOY_STATE_IDLE:
//		m_pModelCom->ChangeAnim(AMITOY_STATE_IDLE, 0.1f, true);
//		break;
//	case AMITOY_STATE_SHOCK:
//		m_fAbnormalTime = 1.f;
//		m_bShock = false;
//		m_pModelCom->ChangeAnim(AMITOY_STATE_SHOCK, 0.1f, false, 0.7f);
//		break;
//	case AMITOY_STATE_SUPERJUMP:
//		m_bSuperJump = false;
//		m_pModelCom->ChangeAnim(AMITOY_STATE_SUPERJUMP, 0.1f, false, 0.7f);
//		m_pModelCom->GetCurAnimation()->SetTrackPosition(4.f);
//		m_pRigidBody->SetLinearVelocity({ 0.0f,0.0f,0.0f });
//		m_pTransformCom->AddForce({ 0.0f, 10.0f,0.0f }, PxForceMode::eVELOCITY_CHANGE);
//		break;
//	case AMITOY_STATE_APPEAR:
//		m_pModelCom->ChangeAnim(AMITOY_STATE_APPEAR, 0.1f, true);
//		break;
//	case AMITOY_STATE_DISAPPEAR:
//		m_pModelCom->ChangeAnim(AMITOY_STATE_DISAPPEAR, 0.1f, true);
//		break;
//	case AMITOY_STATE_CEREMONY:
//		m_pModelCom->ChangeAnim(AMITOY_STATE_CEREMONY, 0.1f, true);
//		break;
//	case AMITOY_STATE_LAUGH:
//		m_pModelCom->ChangeAnim(AMITOY_STATE_LAUGH, 0.1f, true);
//		break;
//	case AMITOY_STATE_DISAPPOINT:
//		m_pModelCom->ChangeAnim(AMITOY_STATE_DISAPPOINT, 0.1f, true);
//		break;
//	case AMITOY_STATE_KNOCKBACK:
//		m_pModelCom->ChangeAnim(AMITOY_STATE_KNOCKBACK, 0.1f, true);
//		break;
//	case AMITOY_STATE_RUN:
//		m_pModelCom->ChangeAnim(AMITOY_STATE_RUN, 0.1f, true);
//		break;
//	case AMITOY_STATE_WALK:
//		m_pModelCom->ChangeAnim(AMITOY_STATE_WALK, 0.1f, true);
//		break;
//	}
//}
//
//void CFonosMutantAmitoy::State_Tick(_float _fTimeDelta)
//{
//	switch (m_eCurState)
//	{
//	case AMITOY_STATE_IDLE:
//		if (AbnormalCheck(_fTimeDelta))
//			return;
//		m_eCurState = KeyInput(_fTimeDelta);
//		break;
//	case AMITOY_STATE_SHOCK:
//		m_fAbnormalTime -= _fTimeDelta;
//		if (0.f >= m_fAbnormalTime)
//		{
//			m_eCurState = AMITOY_STATE_IDLE;
//			m_bInvincible = true;
//			m_fInvTime = 0.5f;
//		}
//		break;
//	case AMITOY_STATE_SUPERJUMP:
//		if (m_pModelCom->GetCanCancelAnimation())
//		{
//			AbnormalCheck(_fTimeDelta);
//			m_eCurState = AMITOY_STATE_IDLE;
//			return;
//		}
//		m_eCurState = KeyInput(_fTimeDelta);
//		break;
//	case AMITOY_STATE_APPEAR:
//		break;
//	case AMITOY_STATE_DISAPPEAR:
//		break;
//	case AMITOY_STATE_CEREMONY:
//		break;
//	case AMITOY_STATE_LAUGH:
//		break;
//	case AMITOY_STATE_DISAPPOINT:
//		break;
//	case AMITOY_STATE_KNOCKBACK:
//		break;
//	case AMITOY_STATE_RUN:
//		if (AbnormalCheck(_fTimeDelta))
//			return;
//		m_eCurState = KeyInput(_fTimeDelta);
//		m_pTransformCom->GoStraight(_fTimeDelta);
//		break;
//	case AMITOY_STATE_WALK:
//		if (AbnormalCheck(_fTimeDelta))
//			return;
//		m_eCurState = KeyInput(_fTimeDelta);
//		m_pTransformCom->GoStraight(_fTimeDelta / 2.f);
//		break;
//	}
//}
//
//void CFonosMutantAmitoy::State_Out(_float _fTimeDelta)
//{
//	int a;
//	switch (m_ePrevState)
//	{
//	case AMITOY_STATE_IDLE:
//		break;
//	case AMITOY_STATE_SHOCK:
//		break;
//	case AMITOY_STATE_SUPERJUMP:
//		break;
//	case AMITOY_STATE_APPEAR:
//		break;
//	case AMITOY_STATE_DISAPPEAR:
//		break;
//	case AMITOY_STATE_CEREMONY:
//		break;
//	case AMITOY_STATE_LAUGH:
//		break;
//	case AMITOY_STATE_DISAPPOINT:
//		break;
//	case AMITOY_STATE_KNOCKBACK:
//		break;
//	case AMITOY_STATE_RUN:
//		a = 3;
//		break;
//	case AMITOY_STATE_WALK:
//		break;
//	}
//}
