#include "PlayerRopeLine.h"

CPlayerRopeLine::CPlayerRopeLine()
{
}

CPlayerRopeLine::~CPlayerRopeLine()
{
}

HRESULT CPlayerRopeLine::Initialize(shared_ptr<ROPELINE_DESC> _pRopeLineDesc)
{
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 5.f;
	pArg->fRotationPerSec = XMConvertToRadians(90.0f);
	__super::Initialize(pArg.get());

	m_pParentTransform = _pRopeLineDesc->pParentTransform;
	m_pSocketMatrix = _pRopeLineDesc->pSocketMatrix;
	m_pTargetMatrix = _pRopeLineDesc->pTargetMatrix;

	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_POINT;
	m_iShaderPass = 0;

	return S_OK;
}

void CPlayerRopeLine::PriorityTick(_float _fTimeDelta)
{
}

void CPlayerRopeLine::Tick(_float _fTimeDelta)
{
}

void CPlayerRopeLine::LateTick(_float _fTimeDelta)
{
	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_GLOW, shared_from_this());
}

HRESULT CPlayerRopeLine::Render()
{
	_float4x4 WorldMat;
	_float4x4 BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	
	BoneMatrix.Right(XMVector3Normalize(BoneMatrix.Right()));
	BoneMatrix.Up(XMVector3Normalize(BoneMatrix.Up()));
	BoneMatrix.Forward(XMVector3Normalize(BoneMatrix.Forward()));
	
	WorldMat = BoneMatrix * m_pParentTransform->GetWorldMatrix();

	_float4	vColor		= { 0.727f, 0.527f, 0.873f, 1.f };
	_float	fThickness	= { 0.02f };

	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
	GAMEINSTANCE->BindWVPMatrixPerspective(WorldMat);
	GAMEINSTANCE->BindMatrix("g_TargetWorldMatrix", m_pTargetMatrix);
	GAMEINSTANCE->BindRawValue("g_fThickness", &fThickness, sizeof(_float));
	GAMEINSTANCE->BindRawValue("g_vColor", &vColor, sizeof(_float4));

	GAMEINSTANCE->BeginShader();
	
	return S_OK;
}

HRESULT CPlayerRopeLine::RenderGlow()
{
	_float4x4 WorldMat;
	_float4x4 BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	
	BoneMatrix.Right(XMVector3Normalize(BoneMatrix.Right()));
	BoneMatrix.Up(XMVector3Normalize(BoneMatrix.Up()));
	BoneMatrix.Forward(XMVector3Normalize(BoneMatrix.Forward()));
	
	WorldMat = BoneMatrix * m_pParentTransform->GetWorldMatrix();
	
	_float	fThickness = { 0.02f };

	GAMEINSTANCE->ReadyShader(m_eShaderType, 1);
	GAMEINSTANCE->BindWVPMatrixPerspective(WorldMat);
	GAMEINSTANCE->BindMatrix("g_TargetWorldMatrix", m_pTargetMatrix);
	GAMEINSTANCE->BindRawValue("g_fThickness", &fThickness, sizeof(_float));

	_bool   bUseGlowColor = true;
	//_float4	vGlowColor = { 0.427f, 0.227f, 0.573f, 1.f };
	_float4 vGlowColor = { 0.827f, 0.627f, 0.973f, 1.f };
	
	GAMEINSTANCE->BindRawValue("g_bUseGlowColor", &bUseGlowColor, sizeof(_bool));
	GAMEINSTANCE->BindRawValue("g_vGlowColor", &vGlowColor, sizeof(_float4));
	GAMEINSTANCE->BeginShader();
	
	return S_OK;
}

shared_ptr<CPlayerRopeLine> CPlayerRopeLine::Create(shared_ptr<ROPELINE_DESC> _pRopeLineDesc)
{
	shared_ptr<CPlayerRopeLine> pInstance = make_shared<CPlayerRopeLine>();

	if (FAILED(pInstance->Initialize(_pRopeLineDesc)))
		MSG_BOX("Failed to Create : CPlayerRopeLine");

	return pInstance;
}
