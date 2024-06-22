#include "AnimEffect.h"
#include "Model.h"
#include "Animation.h"

CAnimEffect::CAnimEffect()
{
}

CAnimEffect::~CAnimEffect()
{
}

HRESULT CAnimEffect::Initialize()
{
	m_eShaderType = ST_ANIM;
	m_iShaderPass = 2;

	__super::Initialize(nullptr);

	m_IsEnabled = false;

	return S_OK;
}

void CAnimEffect::PriorityTick(_float _fTimeDelta)
{
}

void CAnimEffect::Tick(_float _fTimeDelta)
{
	if (m_bPlayAnimation)
		m_pModelCom->PlayAnimation(_fTimeDelta);

	else
	{
		if (m_bDslv)
			return;

		m_IsEnabled = false;
		m_pModelCom->ChangeAnim(0, 0.f, false);
		m_bPlayAnimation = true;
		m_fDistortionTimer = 0.f;
	}
}

void CAnimEffect::LateTick(_float _fTimeDelta)
{
	if (m_bDslv)
		UpdateDslv(_fTimeDelta);


	if (m_pModelCom->GetCurAnimation()->GetAnimRatio() >= 1.f)
	{
		m_bPlayAnimation = false;

		// µðÁ¹ºê ¾²¸é µðÁ¹ºê ÇÏ°í
		if (m_eAnimEffectDesc.bUseDslv)
		{
			SetDissolve(true, true, m_eAnimEffectDesc.fDslvDuration);
			SetDissolveInfo(m_eAnimEffectDesc.fDslvThick, m_eAnimEffectDesc.vDslvColor, m_eAnimEffectDesc.strDslvTexKey.c_str());
			
			m_bDslv = true;
		}
	}

	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());

	if (m_eAnimEffectDesc.bUseDistortion)
	{
		m_fDistortionTimer += _fTimeDelta;
		GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_DISTORTION, shared_from_this());
	}
}

HRESULT CAnimEffect::Render()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
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

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		GAMEINSTANCE->BeginAnimModel(m_pModelCom, i);
	}

	_bool bDslvFalse = false;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &bDslvFalse, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimEffect::RenderDistortion()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 9);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	GAMEINSTANCE->BindSRV("g_DistortionTexture", m_eAnimEffectDesc.strDistortionKey);

	GAMEINSTANCE->BindRawValue("g_fDistortionTimer", &m_fDistortionTimer, sizeof(_float));
	GAMEINSTANCE->BindRawValue("g_fDistortionSpeed", &m_eAnimEffectDesc.fDistortionSpeed, sizeof(_float));
	GAMEINSTANCE->BindRawValue("g_fDistortionWeight", &m_eAnimEffectDesc.fDistortionWeight, sizeof(_float));

	_uint iNumMeshes = m_pModelCom->GetNumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++) {
		GAMEINSTANCE->BeginAnimModel(m_pModelCom, (_uint)i);
	}

	return S_OK;
}

void CAnimEffect::SetAnimEffectDesc(ANIMEFFECT_DESC _AnimEffectDesc)
{
	m_eAnimEffectDesc = _AnimEffectDesc;

	m_pModelCom = GAMEINSTANCE->GetModel(m_eAnimEffectDesc.strModelKey);
	m_pModelCom->ChangeAnim(0, 0.f, false);
	m_bPlayAnimation = true;
	m_fDistortionTimer = 0.f;
}

shared_ptr<CAnimEffect> CAnimEffect::Create()
{
	shared_ptr<CAnimEffect> pInstance = make_shared<CAnimEffect>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CAnimEffect");

	return pInstance;
}
