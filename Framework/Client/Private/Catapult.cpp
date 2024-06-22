#include "Catapult.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "EventMgr.h"
#include "CameraMgr.h"
#include "UIMgr.h"
#include "Player.h"
#include "TargetDirection.h"
#include "WallStone.h"

#include "QuestMgr.h"



CCatapult::CCatapult()
{
}

HRESULT CCatapult::Initialize(_float3 _vPosition)
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 0.f;
	pArg->fRotationPerSec = XMConvertToRadians(10.0f);
	
	__super::Initialize(pArg.get());

	m_pModel = GAMEINSTANCE->GetModel("Catapult");
	AddComponent(L"Catapult_Model", m_pModel);
	AddRigidBody("Catapult");

	m_eShaderType = ST_ANIM;
	m_iShaderPass = 0;

	m_vUIDistance = _float3(0.f, 1.f, 0.f);

	m_pModel->ChangeAnim(0, 0.f, true);
	m_pModel->PlayAnimation(0.f);

	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);

	_float3 vPosition = _vPosition;
	vPosition.x += 1.f;
	m_pTransformCom->LookAtForLandObject(vPosition);

	m_pTransformCom->Tick(0.f);

	m_fInteractDistance = 0.1f;

	AddCatapultRock();
	AddTargetDir();

	CEventMgr::GetInstance()->BindCatapult(static_pointer_cast<CCatapult>(shared_from_this()));

	return S_OK;
}

void CCatapult::PriorityTick(_float _fTimeDelta)
{
	__super::PriorityTick(_fTimeDelta);

	if(m_IsCatapultOn)
		m_pTargetDirection->PriorityTick(_fTimeDelta);
}

void CCatapult::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	KeyInput(_fTimeDelta);

	UIMGR->UpdateCatapultUI((m_fCurrentShootPower - 20.f) / (m_fMaxShootPower - 20.f));

	if (m_isDissolving)
	{
		m_fDissolveTime += _fTimeDelta;
		if (m_fDissolveTime >= 0.65f)
		{
			m_isDissolving = false;
			m_fDissolveTime = 0.f;
			m_pCurrentCatapultRock.lock()->SetOnCatapultBarrel();
		}
	}

	if (m_isShooted)
	{
		m_fAfterShoot += _fTimeDelta;

		if (0.25f < m_fAfterShoot && (!m_isSeparate))
		{
			m_isSeparate = true;

			if (m_pCurrentCatapultRock.lock() != nullptr)
			{
				_float3 vMyLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);
				vMyLook.y += 1.f;
				m_pCurrentCatapultRock.lock()->ShootRock(vMyLook, m_fCurrentShootPower);
			}
		}
		else if (0.7f < m_fAfterShoot && m_isSeparate)
		{
			m_isShooted = false;
			m_isSeparate = false;

			m_fAfterShoot = 0.f;

			if (m_pCurrentCatapultRock.lock() != nullptr)
			{
				m_pCurrentCatapultRock.lock()->OnCatapultRockCamera();
				GAMEINSTANCE->PlaySoundW("Catapult_Wind", 1.0f);
			}
		}
	}
	if ( (0 <= m_fCurrentShootCoolTime) && (m_eCatapultState == CATAPULT_ON) )
	{
		m_fCurrentShootCoolTime -= _fTimeDelta;
	}
	else if(m_eCatapultState == CATAPULT_ON)
	{
		m_fCurrentShootCoolTime = m_fShootCoolTime;
		m_pModel->ChangeAnim(CATAPULT_ON_TO_OFF, 0.f);
		m_eCatapultState = CATAPULT_ON_TO_OFF;
	}

	if (m_pModel->GetIsFinishedAnimation() && (m_eCatapultState == CATAPULT_OFF_TO_ON) )
	{
		m_pModel->ChangeAnim(CATAPULT_ON, 0.f, true);
		m_eCatapultState = CATAPULT_ON;
	}
	else if (m_pModel->GetIsFinishedAnimation() && (m_eCatapultState == CATAPULT_ON_TO_OFF))
	{
		m_pModel->ChangeAnim(CATAPULT_OFF, 0.f, true);
		m_eCatapultState = CATAPULT_OFF;
	}

	m_pModel->PlayAnimation(_fTimeDelta);

	if (m_IsCatapultOn)	
		m_pTargetDirection->Tick(_fTimeDelta);
}

void CCatapult::LateTick(_float _fTimeDelta)
{
	__super::LateTick(_fTimeDelta);

	if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 10.0f))
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
		GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());
	}

#ifdef _DEBUG
	if (m_pRigidBody)
		GAMEINSTANCE->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG

	if (m_IsCatapultOn)
		m_pTargetDirection->LateTick(_fTimeDelta);
}

HRESULT CCatapult::Render()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	_uint iNumMeshes = m_pModel->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++) {

		GAMEINSTANCE->BeginAnimModel(m_pModel, i);
	}

	return S_OK;
}

HRESULT CCatapult::RenderShadow()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 3);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());
	
	GAMEINSTANCE->BindLightProjMatrix();
	
	if (FAILED(GAMEINSTANCE->BindRawValue("g_iCascadeNum", &m_iCurrentCascadeNum, sizeof(m_iCurrentCascadeNum))))
		return E_FAIL;
	
	_uint iNumMeshes = m_pModel->GetNumMeshes();
	
	for (_uint i = 0; i < iNumMeshes; i++)
	{
	
		GAMEINSTANCE->BeginAnimModel(m_pModel, i);
	}

	return S_OK;
}

void CCatapult::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CCatapult::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CCatapult::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CCatapult::ShootOn()
{
	if (m_eCatapultState == CATAPULT_OFF)
	{
		m_pModel->ChangeAnim(CATAPULT_OFF_TO_ON, 0.02f);
		m_pModel->PlayAnimation(0.f);
		m_eCatapultState = CATAPULT_OFF_TO_ON;

		m_isShooted = true;
	}
}

void CCatapult::CatapultOn()
{
	m_IsCatapultOn = true;

	m_vStaticCameraEye = GetCatapultCameraEye();
	m_vStaticCameraAt = GetCatapultCameraAt();

	CCameraMgr::GetInstance()->CameraTurnOn("StaticCamera", m_vStaticCameraEye, m_vStaticCameraAt, 1.0f, 0.1f);

	UIMGR->ActivateCatapultUI();
	UIMGR->ActivateCatapultTutorial();
	UIMGR->DeactivateHUD();
	dynamic_pointer_cast<CPlayer>(m_pPlayer.lock())->SetAllInputState(true);
}

void CCatapult::CatapultOff()
{
	m_IsCatapultOn = false;
	CCameraMgr::GetInstance()->CameraTurnOn("PlayerCamera", 0.5f, 0.1f);

	UIMGR->DeactivateCatapultUI();
	UIMGR->DeactivateCatapultTutorial();
	UIMGR->ActivateHUD();
	dynamic_pointer_cast<CPlayer>(m_pPlayer.lock())->SetAllInputState(false);

	//투석기 이벤트 종료 (퀘스트)
	QUESTMGR->ActiveEventTag(QUEST_MAIN, "Event_Catapult");
}

_float3 CCatapult::GetCatapultCameraEye()
{
	_float3 vMyPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_float3 vMyLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);
	
	vMyPos.y += 18.f;
	vMyPos -= vMyLook * 24.f;

	return vMyPos;
}

_float3 CCatapult::GetCatapultCameraAt()
{
	_float3 vMyPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_float3 vMyLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);
	
	vMyPos += vMyLook * 24.f;
	
	return vMyPos;
}

_bool CCatapult::CheckClear()
{
	_bool bClear = true;

	for (auto pWallStone : m_vecWallStones)
	{
		_float3 vPos = pWallStone->GetTransform()->GetState(CTransform::STATE_POSITION);
		
		if (vPos.y <= (-55.3f))
			continue;
		else
			bClear = false;
	}
	
	return bClear;
}
void CCatapult::CheckDisableWallStone()
{
	for (auto pWallStone : m_vecWallStones)
	{
		pWallStone->StopStone();

		_float3 vPos = pWallStone->GetTransform()->GetState(CTransform::STATE_POSITION);

		if (427.1f <= vPos.x)
			pWallStone->DisableStone();
	}
}


void CCatapult::AddRigidBody(string _strModelKey)
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	//==본체==
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::LOCK_ROT_ALL;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 1.0f;
	RigidBodyDesc.fRestitution = 0.0f;

	GeometryBox BoxDesc;
	BoxDesc.vSize = _float3{ 8.f, 4.f, 14.f };
	BoxDesc.vOffSetPosition = { 0.f, 2.f, -0.5f };
	BoxDesc.vOffSetRotation = { 0.f, 0.f, 0.f };
	BoxDesc.strShapeTag = "Catapult_Body";
	RigidBodyDesc.pGeometry = &BoxDesc;

	RigidBodyDesc.eThisColFlag = COL_INTERACTIONOBJ;
	RigidBodyDesc.eTargetColFlag = COL_PLAYER | COL_INTERACTIONOBJ;
	m_pRigidBody = GAMEINSTANCE->CloneRigidBody(&RigidBodyDesc);

	m_pTransformCom->SetRigidBody(m_pRigidBody);
}

void CCatapult::AddCatapultRock()
{
	//Layer_Catapult
	shared_ptr<CCatapultRock> pCatapultRock = make_shared<CCatapultRock>();

	shared_ptr<CCatapultRock::CatapultRock_Desc> pCatapultRockDesc = make_shared<CCatapultRock::CatapultRock_Desc>();
	pCatapultRockDesc->pParentTransform = m_pTransformCom;
	pCatapultRockDesc->pSocketMatrix = m_pModel->GetCombinedBoneMatrixPtr("BN_SFX_01");
	pCatapultRockDesc->pCatapult = static_pointer_cast<CCatapult>(shared_from_this());


	_float3 vRockPos = { 282.8f, -63.9f, 330.3f };
	pCatapultRock = CCatapultRock::Create(pCatapultRockDesc, vRockPos);
	m_pCatapultRocks.push_back(pCatapultRock);
	GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_Catapult"), pCatapultRock);
	
	vRockPos = { 282.8f, -63.9f, 325.3f };
	pCatapultRock = CCatapultRock::Create(pCatapultRockDesc, vRockPos);
	m_pCatapultRocks.push_back(pCatapultRock);
	GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_Catapult"), pCatapultRock);
		
	vRockPos = { 277.8f, -63.9f, 330.3f };
	pCatapultRock = CCatapultRock::Create(pCatapultRockDesc, vRockPos);
	m_pCatapultRocks.push_back(pCatapultRock);
	GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_Catapult"), pCatapultRock);
	
	vRockPos = { 277.8f, -63.9f, 325.3f };
	pCatapultRock = CCatapultRock::Create(pCatapultRockDesc, vRockPos);
	m_pCatapultRocks.push_back(pCatapultRock);
	GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_Catapult"), pCatapultRock);

	vRockPos = { 280.3f, -59.f, 327.8f };
	pCatapultRock = CCatapultRock::Create(pCatapultRockDesc, vRockPos);
	m_pCatapultRocks.push_back(pCatapultRock);
	GAMEINSTANCE->AddObject(LEVEL_VILLAGE, TEXT("Layer_Catapult"), pCatapultRock);
}

void CCatapult::AddTargetDir()
{
	shared_ptr<CTargetDirection::TargetDirection_Desc> pTargetDirectionDesc = make_shared<CTargetDirection::TargetDirection_Desc>();
	pTargetDirectionDesc->pParentTransform = m_pTransformCom;
	
	m_pTargetDirection = CTargetDirection::Create(pTargetDirectionDesc);
}

void CCatapult::OnCatapultCamera()
{		
	if (CheckClear())
	{
		CheckDisableWallStone();
		CatapultOff();
	}
	else {
		m_fCurrentShootPower = m_fMinShootPower;
		CCameraMgr::GetInstance()->CameraTurnOn("StaticCamera", m_vStaticCameraEye, m_vStaticCameraAt);
	}
}

void CCatapult::BindWallStones(vector<shared_ptr<CWallStone>> _vecWallStones)
{
	m_vecWallStones = _vecWallStones;

	int a = 0;
}

void CCatapult::KeyInput(_float _fTimeDelta)
{
	if (!m_IsCatapultOn)
		return;

	if (GAMEINSTANCE->KeyDown(DIK_F))
	{
		for (auto pCatapultRock : m_pCatapultRocks)
		{
			if (!(pCatapultRock->GetIsUsed()))
			{
				m_isDissolving = true;
				m_pCurrentCatapultRock = pCatapultRock;
				pCatapultRock->StartDissolve();
				GAMEINSTANCE->PlaySoundW("Catapult_Reload", 0.3f);
				return;
			}	
		}

		CatapultOff();
		m_pCurrentCatapultRock.lock() = nullptr;
	}

	if (GAMEINSTANCE->KeyPressing(DIK_LEFT) && (m_fCurrentAngle > -3.f) )
	{
		m_fCurrentAngle -= _fTimeDelta;

		m_pTransformCom->Turn(_float3(0.f, -1.f, 0.f), _fTimeDelta);
		m_pTransformCom->Tick(0.f);
		m_pTargetDirection->GetTransform()->Turn(_float3(0.f, -1.f, 0.f), _fTimeDelta);

		GAMEINSTANCE->PlaySoundNonOverLap("Catapult_Move", 1.f);
	}
	else if (GAMEINSTANCE->KeyPressing(DIK_RIGHT) && (m_fCurrentAngle < 3.f))
	{
		m_fCurrentAngle += _fTimeDelta;

		m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), _fTimeDelta);
		m_pTransformCom->Tick(0.f);

		m_pTargetDirection->GetTransform()->Turn(_float3(0.f, 1.f, 0.f), _fTimeDelta);

		GAMEINSTANCE->PlaySoundNonOverLap("Catapult_Move", 1.f);
	}
	else
	{
		GAMEINSTANCE->StopSound("Catapult_Move");
	}

	if (GAMEINSTANCE->KeyDown(DIK_SPACE))
	{
		GAMEINSTANCE->PlaySoundNonOverLap("Catapult_Charging", 1.f);
		m_fCurrentShootPower = m_fMinShootPower;
	}
	if (GAMEINSTANCE->KeyPressing(DIK_SPACE))
	{
		_float fIncrement = (m_fMaxShootPower - m_fMinShootPower) * (0.5f) * (_fTimeDelta);	
		m_fCurrentShootPower += fIncrement;

		if (m_fCurrentShootPower >= m_fMaxShootPower)
			m_fCurrentShootPower = m_fMaxShootPower;

		console("현재Power : %f\n", m_fCurrentShootPower);
	}
	if (GAMEINSTANCE->KeyUp(DIK_SPACE))
	{
		GAMEINSTANCE->StopSound("Catapult_Charging");
		GAMEINSTANCE->PlaySoundW("Catapult_Shoot", 0.8f);
		ShootOn();
	}
}


void CCatapult::InteractionOn()
{

}

shared_ptr<CCatapult> CCatapult::Create(_float3 _vPosition)
{
	shared_ptr<CCatapult> pCatapult = make_shared<CCatapult>();
	
	if (FAILED(pCatapult->Initialize(_vPosition)))
		MSG_BOX("Failed to Create : CCatapult");
	
	return pCatapult;
}
