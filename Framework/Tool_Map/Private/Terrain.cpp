#include "Terrain.h"
#include "VITerrain.h"
#include "Texture.h"

#include <fstream>

CTerrain::CTerrain()
{
}

#ifdef CHECK_REFERENCE_COUNT
CTerrain::~CTerrain()
{
}
#endif // CHECK_REFERENCE_COUNT

void CTerrain::ChangeViewMode()
{
	switch (m_iShaderPass)
	{
	case 1:
		m_iShaderPass = 3;
		break;
	case 3:
		m_iShaderPass = 1;
		break;
	}
}

HRESULT CTerrain::Initialize(ifstream& _InFileStream)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;



	m_pVIBufferCom = CVITerrain::Create(DEVICE, CONTEXT, _InFileStream);
	m_Components.emplace(TEXT("Com_VIBuffer"), m_pVIBufferCom);

	char szDiffuseTextureName[MAX_PATH]{};
	_InFileStream.read(szDiffuseTextureName, sizeof(szDiffuseTextureName));
	m_strBaseDiffuseTextureName = szDiffuseTextureName;

	_float4x4 matWorld{};
	_InFileStream.read(reinterpret_cast<char*>(&matWorld), sizeof(_float4x4));
	m_pTransformCom->SetWorldMatrix(matWorld);

	m_eShaderType = ESHADER_TYPE::ST_NORTEX;
	m_iShaderPass = 3;

	return S_OK;
}

HRESULT CTerrain::Initialize(_float4x4 _matWorld, shared_ptr<CVITerrain> _pVITerrain)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	m_pTransformCom->SetWorldMatrix(_matWorld);

	m_eShaderType = ESHADER_TYPE::ST_NORTEX;
	m_iShaderPass = 3;

	m_strBrushTextureName = "Brush2";

	m_pVIBufferCom = _pVITerrain;
	m_Components.emplace(TEXT("Com_VIBuffer"), m_pVIBufferCom);

	m_pTransformCom->SetScaling(1.f, 1.f, 1.f);

	return S_OK;
}

void CTerrain::PriorityTick(_float _fTimeDelta)
{
}

void CTerrain::Tick(_float _fTimeDelta)
{
}

void CTerrain::LateTick(_float _fTimeDelta)
{
	if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_PRIORITY, shared_from_this())))
		return;

	if (FAILED(GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_PIXELPICKING, shared_from_this())))
		return;
}

HRESULT CTerrain::Render()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass); //
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix()); //

	GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_strBaseDiffuseTextureName); //
	GAMEINSTANCE->BindSRV("g_BrushTexture", m_strBrushTextureName); // ¸ÊÅø

	GAMEINSTANCE->BindRawValue("g_fBrushRadius", &m_fBrushRadius, sizeof(_float)); // ¸ÊÅø
	GAMEINSTANCE->BindRawValue("g_vBrushPos", &m_vBrushPos, sizeof(_float3)); // ¸ÊÅø
	
	_uint iNumWeightSRVs = static_cast<_uint>(m_vecWeightDescs.size());

	GAMEINSTANCE->BindRawValue("g_iNumWeightTexture", &iNumWeightSRVs, sizeof(_uint));

	for (_uint iCntWeightSRVs = 0; iCntWeightSRVs < iNumWeightSRVs; iCntWeightSRVs++)
	{
		string strConstantName = "g_WeightTexture_" + to_string(iCntWeightSRVs);
		GAMEINSTANCE->BindSRVDirect(strConstantName.c_str(), m_vecWeightDescs[iCntWeightSRVs].pWeightSRV);

		strConstantName = "g_BlendTexture_Red_" + to_string(iCntWeightSRVs);
		GAMEINSTANCE->BindSRV(strConstantName.c_str(), m_vecWeightDescs[iCntWeightSRVs].arrBlendDiffusesName[BLEND_RED]);

		strConstantName = "g_BlendTexture_Green_" + to_string(iCntWeightSRVs);
		GAMEINSTANCE->BindSRV(strConstantName.c_str(), m_vecWeightDescs[iCntWeightSRVs].arrBlendDiffusesName[BLEND_GREEN]);

		strConstantName = "g_BlendTexture_Blue_" + to_string(iCntWeightSRVs);
		GAMEINSTANCE->BindSRV(strConstantName.c_str(), m_vecWeightDescs[iCntWeightSRVs].arrBlendDiffusesName[BLEND_BLUE]);

		strConstantName = "g_BlendTexture_Alpha_" + to_string(iCntWeightSRVs);
		GAMEINSTANCE->BindSRV(strConstantName.c_str(), m_vecWeightDescs[iCntWeightSRVs].arrBlendDiffusesName[BLEND_ALPHA]);
	}

	GAMEINSTANCE->BeginShaderBuffer(m_pVIBufferCom); //

	return S_OK;
}

HRESULT CTerrain::RenderPixelPicking()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 2);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pTransformCom->GetWorldMatrix());

	//GAMEINSTANCE->BindSRV("g_DiffuseTexture", m_pTextures[TYPE_DIFFUSE]);

	GAMEINSTANCE->BeginShaderBuffer(m_pVIBufferCom);

	return S_OK;
}

shared_ptr<CTerrain> CTerrain::Create(ifstream& _InFileStream)
{
	shared_ptr<CTerrain> pInstance = WRAPPING(CTerrain)();

	if (FAILED(pInstance->Initialize(_InFileStream)))
	{
		MSG_BOX("Failed to Created : CTerrain");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CTerrain> CTerrain::Create(_float4x4 _matWorld, shared_ptr<CVITerrain> _pVITerrain)
{
	shared_ptr<CTerrain> pInstance = WRAPPING(CTerrain)();

	if (FAILED(pInstance->Initialize(_matWorld, _pVITerrain)))
	{
		MSG_BOX("Failed to Created : CTerrain");
		pInstance.reset();
	}

	return pInstance;
}