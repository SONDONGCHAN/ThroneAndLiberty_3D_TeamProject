#include "pch.h"
#include "Mirror.h"
#include "Model.h"
#include "CameraMgr.h"
#include "CameraSpherical.h"

CMirror::CMirror()
	: CInteractionObj()
{
}

shared_ptr<CMirror> CMirror::Create(const _float4x4& _vWorldMatrix, const _float3& _vColor, EDIRECTION _eDir)
{
	shared_ptr<CMirror> pInstance = make_shared<CMirror>();

	if (FAILED(pInstance->Initialize(_vWorldMatrix, _vColor, _eDir)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CMirror::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CMirror::Initialize(const _float4x4& _vWorldMatrix, const _float3& _vColor, EDIRECTION _eDir)
{
	if (FAILED(CInteractionObj::Initialize(nullptr)))
		return E_FAIL;

	m_pTransformCom->SetWorldMatrix(_vWorldMatrix);
	m_pTransformCom->SetScaling(0.9f, 0.9f, 0.9f);
	m_eDirection = _eDir;

	memcpy_s(&m_vColor, sizeof(_float3), &_vColor, sizeof(_float3));

	m_eShaderType = ST_NONANIM;
	m_iShaderPass = 0;

	m_fInteractDistance = 2.0f;
	m_vUIDistance = { 0.0f,1.0f,0.0f };

	m_pModel = GAMEINSTANCE->GetModel("BG_Mirror_02_01_SM");;

	m_fTimeAcc = GAMEINSTANCE->PickRandomFloat(0.0f, 1.0f);
	m_isUp = GetRandomChance(0.5f);

	m_FovDesc.eShakeFunc = SHAKE_WAVE;
	m_FovDesc.fFovyAmplitude = XMConvertToRadians(8.0f);
	m_FovDesc.fFovyFrequency = 0.8f;
	m_FovDesc.vBlendInOut = { 0.0f,0.2f };
	m_FovDesc.vInOutWeight = { 1.0f,1.2f };

	m_IsActive = false;

	return S_OK;
}

void CMirror::PriorityTick(_float _fTimeDelta)
{
	if (GAMEINSTANCE->KeyDown(DIK_F)) {

		if (m_IsInteractionOn && m_bInteractAble)
			InteractionOn();
	}
}

void CMirror::Tick(_float _fTimeDelta)
{
	m_fTimeAcc += _fTimeDelta * 0.5f;
	if (m_isDespawn)
	{
		m_fDespawnTime += _fTimeDelta;
		if (m_fDespawnTime >= 3.7f)
		{
			m_IsActive = false;
			return;
		}
	}

	if (m_fTimeAcc >= 1.0f)
	{
		m_isUp = !m_isUp;
		memset(&m_fTimeAcc, 0, sizeof(float));
	}
}

void CMirror::LateTick(_float _fTimeDelta)
{
	if (m_bDslv)
		UpdateDslv(_fTimeDelta);

	if(m_isUp)
		m_pTransformCom->SetPositionParam(1, LinearIp(0.1f, 0.3f, m_fTimeAcc));
	else
		m_pTransformCom->SetPositionParam(1, LinearIp(0.3f, 0.1f, m_fTimeAcc));

	CInteractionObj::LateTick(_fTimeDelta);

	if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 3.0f))
	{
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
	}
}

HRESULT CMirror::Render()
{
	shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;

	pGameInstance->ReadyShader(m_eShaderType, m_iShaderPass);
	pGameInstance->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	if (m_bDslv)
	{
		if (FAILED(pGameInstance->BindRawValue("g_bDslv", &m_bDslv, sizeof(_bool))))
			return E_FAIL;

		_float g_fDslvValue = m_fDslvTime / m_fMaxDslvTime;

		if (FAILED(pGameInstance->BindRawValue("g_fDslvValue", &g_fDslvValue, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pGameInstance->BindRawValue("g_fDslvThick", &m_fDslvThick, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pGameInstance->BindRawValue("g_vDslvColor", &m_vDslvColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pGameInstance->BindSRV("g_DslvTexture", m_strDslvTexKey)))
			return E_FAIL;
	}

	_uint iNumMeshes = m_pModel->GetNumMeshes();

	UsingRimLight(m_vColor, 2.0f);

	for (UINT i = 0; i < iNumMeshes; i++) {

		pGameInstance->BeginNonAnimModel(m_pModel, (_uint)i);
	}

	/*RimLight Off*/
	_bool bRimOff = false;
	pGameInstance->BindRawValue("g_IsUsingRim", &bRimOff, sizeof(bRimOff));

	if (m_bDslv)
	{
		bool bDslvFalse = false;
		if (FAILED(pGameInstance->BindRawValue("g_bDslv", &bDslvFalse, sizeof(bool))))
			return E_FAIL;
	}

	return S_OK;
}

void CMirror::InteractionOn()
{
	shared_ptr<CTransform> pPlayerTransform = m_pPlayer.lock()->GetTransform();
	shared_ptr<CTransform> pLinkedMirrorTransform = m_pLinkedMirror.lock()->GetTransform();

	_float3 vLinkedMirrorLookDir = -pLinkedMirrorTransform->GetState(CTransform::STATE_LOOK);
	_float3 vLinkedMirrorPos = pLinkedMirrorTransform->GetState(CTransform::STATE_POSITION);

	pPlayerTransform->SetState(CTransform::STATE_POSITION, vLinkedMirrorPos + vLinkedMirrorLookDir);
	pPlayerTransform->LookAtDir(vLinkedMirrorLookDir);

	shared_ptr<CCameraSpherical> pPlayerCamera = static_pointer_cast<CCameraSpherical>(CCameraMgr::GetInstance()->FindCamera("PlayerCamera"));
	if (pPlayerCamera)
	{
		CCameraMgr::GetInstance()->CameraShake(ESHAKEFLAG::SHAKE_FOV, 0.4f, nullptr, nullptr, &m_FovDesc);
		switch (m_pLinkedMirror.lock()->m_eDirection)
		{
		case M_RIGHT:
		{
			pPlayerCamera->SetSphericalPos({ pPlayerCamera->GetRadius(),XM_PIDIV2 * 0.7f ,XM_PI });
		}
		break;
		case M_BACK:
		{
			pPlayerCamera->SetSphericalPos({ pPlayerCamera->GetRadius(),XM_PIDIV2 * 0.7f ,XM_PIDIV2 });
		}
		break;
		case M_LEFT:
		{
			pPlayerCamera->SetSphericalPos({ pPlayerCamera->GetRadius(),XM_PIDIV2 * 0.7f ,0 });
		}
		break;
		case M_FRONT:
		{
			pPlayerCamera->SetSphericalPos({ pPlayerCamera->GetRadius(),XM_PIDIV2 * 0.7f ,XM_PI * 1.5f });
		}
		break;
		}
	}
	GAMEINSTANCE->PlaySoundW("UseMirror", 0.5f);
}

void CMirror::InteractionOff()
{
}

void CMirror::LinkMirror(shared_ptr<CMirror> _pLinkedMirror)
{
	if (_pLinkedMirror)
		m_pLinkedMirror = _pLinkedMirror;
}

void CMirror::SpawnMirror()
{
	m_IsActive = true;
	m_isDespawn = false;
	memset(&m_fDespawnTime, 0, sizeof(float));
	SetDissolve(true, false, 6.0f);
	SetDissolveInfo(1.f, _float4(m_vColor.x, m_vColor.y, m_vColor.z, 1.f), "Noise_3003");
}

void CMirror::DespawnMiorror()
{
	m_isDespawn = true;
	memset(&m_fDespawnTime, 0, sizeof(float));
	SetDissolve(true, true, 6.0f);
	SetDissolveInfo(1.f, _float4(m_vColor.x, m_vColor.y, m_vColor.z, 1.f), "Noise_3003");
}
