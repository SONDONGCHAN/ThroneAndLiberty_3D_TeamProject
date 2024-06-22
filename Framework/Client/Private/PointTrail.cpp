#include "PointTrail.h"
#include "VIPointTrail.h"
#include "Weapon.h"

CPointTrail::CPointTrail()
{
}

CPointTrail::CPointTrail(const CPointTrail& rhs)
{
}

HRESULT CPointTrail::Initialize(const string& _strTextureKey)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	m_strTextureKey = _strTextureKey;

	m_pVIBufferCom = dynamic_pointer_cast<CVIPointTrail>(GAMEINSTANCE->GetBuffer("Buffer_PointTrail")->Clone(nullptr));
	if (m_pVIBufferCom == nullptr)
		return E_FAIL;

	m_eShaderType = (ESHADER_TYPE)ST_TRAIL;
	m_iShaderPass = 1;

	m_IsEnabled = false;

	return S_OK;
}

void CPointTrail::Tick(_float _fTimeDelta)
{
	CVIPointTrail::TRAIL_DESC TrailDesc;

	TrailDesc.vUpOffset = m_vUpOffset;
	TrailDesc.vDownOffset = m_vDownOffset;
	TrailDesc.vOwnerMatrix = *dynamic_pointer_cast<CWeapon>(m_pOwner)->GetWeaponWorld();

	m_pVIBufferCom->Update(TrailDesc);

	if (m_bEnding || m_bAlphaChange)
	{
		m_vDownColor.w -= m_fChangeWeight * _fTimeDelta;
		m_vUpColor.w -= m_fChangeWeight * _fTimeDelta;
	}
}

void CPointTrail::LateTick(_float _fTimeDelta)
{
	if (m_bEnding)
	{
		if (m_vDownColor.w <= 0.f && m_vUpColor.w <= 0.f)
		{
			m_IsEnabled = false;
			return;
		}
	}

	if (m_bStarting)
	{
		m_bStarting = false;
		return;
	}

	if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_BLEND, shared_from_this())))
		return;

	if (m_bGlow)
	{
		if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_GLOW, shared_from_this())))
			return;
	}

	if (m_bDistortion)
	{
		m_fDistortionTime += _fTimeDelta;

		if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_DISTORTION, shared_from_this())))
			return;
	}
}

HRESULT CPointTrail::Render()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
	GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTextureKey);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	if (FAILED(GAMEINSTANCE->BindRawValue("g_vUpColor", &m_vUpColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_vDownColor", &m_vDownColor, sizeof(_float4))))
		return E_FAIL;

	m_fAlpha = 1.f;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	GAMEINSTANCE->BeginShaderBuffer(m_pVIBufferCom);
	
	return S_OK;
}

HRESULT CPointTrail::RenderGlow()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 2);
	GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTextureKey);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	if (FAILED(GAMEINSTANCE->BindRawValue("g_vUpColor", &m_vUpColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_vDownColor", &m_vDownColor, sizeof(_float4))))
		return E_FAIL;

	m_fAlpha = 1.f;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	GAMEINSTANCE->BeginShaderBuffer(m_pVIBufferCom);

	return S_OK;
}

HRESULT CPointTrail::RenderDistortion()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 3);
	GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strTextureKey);

	GAMEINSTANCE->BindSRV("g_DistortionTexture", "Noise_2406");

	GAMEINSTANCE->BindRawValue("g_fDistortionTimer", &m_fDistortionTime, sizeof(_float));
	GAMEINSTANCE->BindRawValue("g_fDistortionSpeed", &m_fDistortionSpeed, sizeof(_float));
	GAMEINSTANCE->BindRawValue("g_fDistortionWeight", &m_fDistortionWeight, sizeof(_float));

	m_fAlpha = 1.f;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	GAMEINSTANCE->BeginShaderBuffer(m_pVIBufferCom);

	return S_OK;
}

void CPointTrail::SetColor(_float4 _vUpColor, _float4 _vDownColor)
{
	m_vUpColor = _vUpColor;
	m_vDownColor = _vDownColor;
}

void CPointTrail::ChangeWeapon(shared_ptr<CGameObject> _pOwner, _float3 _vUpOffset, _float3 _vDownOffset)
{
	m_pOwner = _pOwner;

	m_vUpOffset = _vUpOffset;
	m_vDownOffset = _vDownOffset;
}

void CPointTrail::ClearTrail()
{
	m_pVIBufferCom->Clear();
}

void CPointTrail::StartTrail(_float4 _vUpColor, _float4 _vDownColor, _bool _bGlow, _bool _bDistortion, _bool _bAlphaChange, _float _fChangeWeight)
{
	ClearTrail();

	m_vUpColor = _vUpColor;
	m_vDownColor = _vDownColor;
	m_bAlphaChange = _bAlphaChange;
	m_fChangeWeight = _fChangeWeight;

	m_fDistortionTime = 0.f;

	m_bGlow = _bGlow;
	m_bDistortion = _bDistortion;

	m_bEnding = false;
	m_bStarting = true;

	m_IsEnabled = true;
}

void CPointTrail::EndTrail()
{
	m_bEnding = true;
	m_fChangeWeight = 10.f;
	m_bAlphaChange = true;
}

shared_ptr<CPointTrail> CPointTrail::Create(const string& _strTextureKey)
{
	shared_ptr<CPointTrail> pInstance = make_shared<CPointTrail>();
	if (FAILED(pInstance->Initialize(_strTextureKey)))
	{
		MSG_BOX("Failed to Created : CPointTrail");
		pInstance.reset();
	}
	return pInstance;
}
