#include "pch.h"
#include "MNGQuestItem.h"
#include "Model.h"
#include "RigidBody.h"
#include "UIMgr.h"
#include "UIPanel.h"

static UINT m_PanelIndex{ 0 };

CMNGQuestItem::CMNGQuestItem()
{
}

CMNGQuestItem::~CMNGQuestItem()
{
}

shared_ptr<CMNGQuestItem> CMNGQuestItem::Create(const string& _strModelTag, const _float4x4& _WorldMatrix, Geometry* _pGeometry, float _fRadius)
{
	shared_ptr< CMNGQuestItem> pInstance = make_shared<CMNGQuestItem>();

	if (FAILED(pInstance->Initialize(_strModelTag, _WorldMatrix, _pGeometry, _fRadius)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CMNGQuestItem::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CMNGQuestItem::Initialize(const string& _strModelTag, const _float4x4& _WorldMatrix, Geometry* _pGeometry, float _fRadius)
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	m_pTransformCom = CTransform::Create(pGameInstance->GetDeviceInfo(), pGameInstance->GetDeviceContextInfo());
	m_pTransformCom->SetWorldMatrix(_WorldMatrix);

	m_pModelCom = pGameInstance->GetModel(_strModelTag);

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_NONANIM;
	m_iShaderPass = 0;

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;

	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.isInitCol = true;
	RigidBodyDesc.pTransform = m_pTransformCom;
	RigidBodyDesc.eLock_Flag = CRigidBody::NONE;

	RigidBodyDesc.pOwnerObject = shared_from_this();
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestitution = 0.0f;

	RigidBodyDesc.pGeometry = _pGeometry;

	RigidBodyDesc.eThisColFlag = COL_STATIC;
	RigidBodyDesc.eTargetColFlag = COL_AMITOY;
	m_pRigidBody = pGameInstance->CloneRigidBody(&RigidBodyDesc);

	m_pTransformCom->SetRigidBody(m_pRigidBody);

	m_fRadius = _fRadius;
	m_vItemScale = m_pTransformCom->GetScaled();
	m_vItemPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);

	m_IsActive = false;

	return S_OK;
}

void CMNGQuestItem::PriorityTick(_float _fTimeDelta)
{
}

void CMNGQuestItem::Tick(_float _fTimeDelta)
{
	AcquireItem();
	if (m_isAcquired)
	{
		m_fTimeAcc += _fTimeDelta;
		if (m_fTimeAcc < 1.0f)
		{
			m_pTransformCom->GoDirFreeSpeed(_fTimeDelta, { 0.0f,1.0f,0.0f }, 0.3f);
		}
		else
		{
			m_pTransformCom->SetScaling(LinearIp(m_vItemScale, { 0.0f,0.0f,0.0f }, (m_fTimeAcc - 1.0f) / 1.0f));
			m_pTransformCom->SetState(CTransform::STATE_POSITION, LinearIp(m_vItemPos, (m_vAmitoyPos + _float3{0.0f, 0.5f, 0.0f}), (m_fTimeAcc - 1.0f) / 1.0f));
			if (m_fTimeAcc > 2.0f)
				m_IsActive = false;
		}
	}
	else
	{
		RenderPosUI();
	}
}

void CMNGQuestItem::LateTick(_float _fTimeDelta)
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	// frustrum
	if (pGameInstance->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), m_fRadius))
	{
		pGameInstance->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

#ifdef _DEBUG
		if (m_pRigidBody)
			pGameInstance->AddDebugComponent(m_pRigidBody);
#endif // _DEBUG
	}
}

HRESULT CMNGQuestItem::Render()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	pGameInstance->ReadyShader(m_eShaderType, m_iShaderPass);
	pGameInstance->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	UINT iNumMeshes = m_pModelCom->GetNumMeshes();

	if(m_isAcquired)
		UsingRimLight({ 1.0f, 1.0f, 0.2f }, 3.0f);
	else
		UsingRimLight({ 1.0f, 1.0f, 0.2f }, 2.0f);

	for (UINT i = 0; i < iNumMeshes; i++) {

		pGameInstance->BeginNonAnimModel(m_pModelCom, i);
	}

	/*RimLight Off*/
	bool bRimOff = false;
	pGameInstance->BindRawValue("g_IsUsingRim", &bRimOff, sizeof(bool));


	return S_OK;
}

void CMNGQuestItem::OnContactFound(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.eColFlag & COL_AMITOY)
		m_isContactAmitoy = true;
}

void CMNGQuestItem::OnContactPersist(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
}

void CMNGQuestItem::OnContactLost(const COLLISIONDATA& _ColData, const string& _szMyShapeTag)
{
	if (_ColData.eColFlag & COL_AMITOY)
	{
		m_isContactAmitoy = false;
		m_vAmitoyPos = _ColData.pGameObject.lock()->GetTransform()->GetState(CTransform::STATE_POSITION);
	}
}

void CMNGQuestItem::SpawnQuestItem()
{
	m_IsActive = true;
	switch (m_PanelIndex)
	{
	case 0:
	{
		m_strPanelKey = "SubQuestIndicatorPanel4";
	}
	break;
	case 1:
	{
		m_strPanelKey = "SubQuestIndicatorPanel5";
	}
	break;
	case 2:
	{
		m_strPanelKey = "SubQuestIndicatorPanel6";
	}
	break;
	}
	m_PanelIndex++;
}

void CMNGQuestItem::RenderPosUI()
{
	
	shared_ptr<CUIBase> lockOnPanel = UIMGR->FindUI(m_strPanelKey);
	if (lockOnPanel == nullptr)
		return;

	_float3 followPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);

	_float dist = SimpleMath::Vector3::Distance(m_pAmitoy.lock()->GetTransform()->GetState(CTransform::STATE_POSITION), followPos);

	lockOnPanel->SetIsRender(true);

	followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_VIEW));
	followPos = SimpleMath::Vector3::Transform(followPos, GAMEINSTANCE->GetTransformMatrix(CPipeLine::D3DTS_PROJ));


	if (followPos.z <= 0.f || followPos.z > 1.f)
	{
		//followPos.x = 100000.f;
		//followPos.y = 100000.f;

		lockOnPanel->SetIsRender(false);
	}

	_float posX = (followPos.x + 1.f) * 0.5f;
	_float posY = (followPos.y - 1.f) * -0.5f;

	posX = g_iWinSizeX * posX - (g_iWinSizeX * 0.5f);
	posY = g_iWinSizeY * -posY + (g_iWinSizeY * 0.5f);

	/* LockOnUI Ã£¾Æ¼­ Pos Set */
	lockOnPanel->SetUIPos(posX, posY);
}

void CMNGQuestItem::SetAmitoyPointer(shared_ptr<CGameObject> _pAmitoy)
{
	m_pAmitoy = _pAmitoy;
}

void CMNGQuestItem::AcquireItem()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	if (m_isContactAmitoy && pGameInstance->KeyDown(DIK_F))
	{
		m_pRigidBody->SimulationOff();
		pGameInstance->PlaySoundW("GetItems", 0.4f);
		m_isContactAmitoy = false;
		m_isAcquired = true;
		shared_ptr<CUIPanel> pUI = dynamic_pointer_cast<CUIPanel>(CUIMgr::GetInstance()->FindUI(m_strPanelKey));
		if (pUI == nullptr)
			return;

		pUI->SetIsRender(false);
	}
}
