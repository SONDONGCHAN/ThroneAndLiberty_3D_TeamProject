#pragma once

#include "Tool_Defines.h"
#include "EffectMesh.h"
#include "AnimEffect.h"

BEGIN(Engine)
class CTexture;
class CBone;
END


BEGIN(Tool_Effect)

class CWindowMesh
{
public:
	enum MODEL_TYPE { MODEL_BASIC, MODEL_UNIQUE, MODEL_ANIM, MODEL_OTHER, MODEL_OTHER2, MODEL_END };

public:
	CWindowMesh();
	~CWindowMesh() = default;

public:
	HRESULT Initialize();
	void	Tick(_float _fTimeDelta);

public:
	// 0. 디폴트
	void	Properties_Default();
	// 1. 메쉬
	void	Properties_Mesh();
	// 2. 디퓨즈 텍스처
	void	Properties_Diffuse();
	// 3. 마스크 텍스처
	void	Properties_Mask();
	// 4. 노이즈 텍스처
	void	Properties_Noise();
	// 5. 색상
	void	Properties_Color();
	// 6. 크기
	void	Properties_Scale();
	// 7. 회전
	void	Properties_Rotation();
	// 8. 이동
	void	Properties_Translation();
	// 9. 셰이더
	void	Properties_Shader();

public:
	CEffect::EffectDesc CreateEffectDesc();
	shared_ptr<CEffectMesh>		CreateEffect();
	shared_ptr<CEffect::EffectDesc>	GetEffectDesc();

	void				CreateTestEffect(CEffectMesh::EffectMeshDesc* _pEffectDesc, shared_ptr<CGameObject> _pOwner);

private:
	void	KeyInput();

private:
	HRESULT	LoadResourceNames(const wstring& _strResourcePath, MODEL_TYPE _eModelType, TEX_TYPE _eTexType);

private:
	vector<string>						m_ModelList[MODEL_END];
	vector<string>						m_TextureList[TEX_END];

	_int								m_iTextureIdx[TEX_END] = { 0 };
	_int								m_iDistortionTexIdx = 0;

private:
	// 테스트 창에 띄우는 모델
	shared_ptr<class CTestEffectMesh>	m_pTestEffectMesh = nullptr;
	
	// 0. 기본 정보
	_float								m_fDuration = 1.f;
	_float								m_StartTrackPos = 0.5f;
	_int								m_iShaderType = ESHADER_TYPE::ST_EFFECTMESH;
	_int								m_iShaderPass = 0;
	_int								m_iRenderGroup = CRenderer::RENDER_BLEND;
	_bool								m_bLoop = false;
	_bool								m_bBillBoard = false;
	
	// 1. 메쉬
	shared_ptr<class CTestModel>		m_pTestModel = nullptr;
	string								m_strModelKey = "";
	_int								m_iSelectedMeshIdx = 0;

	// 2. 디퓨즈 텍스처
	_bool								m_bUseDiffuse = true;
	shared_ptr<CTexture>				m_pDiffuseTexture = nullptr;
	string								m_strDiffuseTexTag = "";

	_int								m_iDiffuseSamplerType = 0;
	_int								m_iDiffuseUVType = 0;

	_float								m_fDiffuseDiscardValue = 0.f;
	_int								m_iDiffuseDiscardArea = 0;

	_float								m_fDiffuseUVScroll[2] = { 0.f, 0.f };
	_int								m_iDiffuseUVAtlasCnt[2] = { 1, 1 };
	_float								m_fDiffuseUVAtlasSpeed = 0.f;

	// 3. 마스크 텍스처
	_bool								m_bUseMask = false;
	shared_ptr<CTexture>				m_pMaskTexture = nullptr;
	string								m_strMaskTexTag = "";

	_int								m_iMaskSamplerType = 0;
	_int								m_iMaskUVType = 0;

	_float								m_fMaskDiscardValue = 0.f;
	_int								m_iMaskDiscardArea = 0;

	_float								m_fMaskUVScroll[2] = {0.f, 0.f};
	_int								m_iMaskUVAtlasCnt[2] = { 1, 1 };
	_float								m_fMaskUVAtlasSpeed = 0.f;

	// 4. 노이즈 텍스처
	_bool								m_bUseNoise = false;
	shared_ptr<CTexture>				m_pNoiseTexture = nullptr;
	string								m_strNoiseTexTag = "";

	_int								m_iNoiseSamplerType = 0;
	_int								m_iNoiseUVType = 0;

	_float								m_fNoiseDiscardValue = 0.f;
	_int								m_iNoiseDiscardArea = 0;

	_float								m_fNoiseUVScroll[2] = { 0.f, 0.f };
	_int								m_iNoiseUVAtlasCnt[2] = { 1, 1 };
	_float								m_fNoiseUVAtlasSpeed = 0.f;

	// 5. 컬러
	_bool								m_bUseColor = false;
	_int								m_iColorSetCnt = 1;
	_int								m_iColorSplitArea = 1;
	_float								m_vColorSplitter[MAX_COLORSPLIT] = {1.f, 1.f, 1.f, 1.f};

	_float								m_fColorTime[MAX_TIMETABLE] = { 0.f, 0.f, 0.f, 0.f, 0.f };
	_int								m_iColorEase[MAX_TIMETABLE] = { 0, 0, 0, 0, 0 };

	_float4								m_vColor[MAX_TIMETABLE][MAX_COLORSPLIT] = { _float4(1.f, 1.f, 1.f, 1.f) };
	

	// 6~8. SRT 선택할 수 있는 오브젝트와 본 정보 추가하기
	shared_ptr<class CAnimModel>		m_pFollowObj;
	shared_ptr<CModel>					m_pFollowObjModel;
	vector<shared_ptr<CBone>>			m_pFollowObjBones;
	vector<const _char*>				m_vecFollowBoneNames;

	// 6. Scale
	_bool								m_bScaleFollow = { false };
	_int								m_iScaleFollowType = { -1 };	// 0 : object / 1 : bone
	_int								m_pScaleFollowBoneIdx;
	string								m_strScaleFollowBoneKey = "";

	_int								m_iScaleCnt = 1;

	_float								m_fScaleTime[MAX_TIMETABLE] = { 0.f };
	_float								m_fScale[MAX_TIMETABLE][3] = { 1.f, 1.f, 1.f };
	_int								m_iScaleEasing[MAX_TIMETABLE] = {};

	// 7. Rotation
	_bool								m_bRotationFollow = { false };
	_int								m_iRotationFollowType = { 0 };
	string								m_strRotationFollowBoneKey = "";

	_int								m_pRotationFollowBoneIdx;

	_float								m_fRotate[3] = { 0.f, 0.f, 0.f };
	_float								m_fTurnVel[3] = { 0.f, 0.f, 0.f };
	_float								m_fTurnAcc[3] = { 0.f, 0.f, 0.f };

	// 8. Translation
	_int								m_iTranslationCnt = 1;
	_bool								m_bTranslationFollow = { false };
	_int								m_iTranslationFollowType = { -1 };
	string								m_strTranslationFollowBoneKey = "";	// 0 : object / 1 : bone

	_int								m_pTranslationFollowBoneIdx;

	_float								m_fTranslationTime[MAX_TIMETABLE] = { 0.f };
	_float								m_fTranslation[MAX_TIMETABLE][3] = { 1.f, 1.f, 1.f };
	_int								m_iTranslationEasing[MAX_TIMETABLE] = {};

	// 9. Shader
	_bool								m_bUseSoftEffect = false;
	_int								m_iSoftEffectType = 0;	// 0 : Glow, 1 : Blur
	_bool								m_bGlowUseColor = false;

	_int								m_iGlowColorSetCnt = 1;

	_float								m_fGlowColorTime[MAX_TIMETABLE] = { 0.f, 0.f, 0.f, 0.f, 0.f };
	_float								m_fGlowColor[MAX_TIMETABLE][4] = {1.f, 1.f, 1.f, 1.f};

	_bool								m_bUseDistortion = false;
	string								m_strDistortionTexTag = "";
	_float								m_fDistortionSpeed = 0.f;
	_float								m_fDistortionWeight = 0.f;

	_bool								m_bUseDslv = false;
	_int								m_iDslvType = false;	// 0 : Dslv In, 1 : Dslv Out
	_bool								m_bDslvGlow = false;
	_float								m_fDslvStartTime = 0.f;
	_float								m_fDslvDuration = 0.f;
	_float								m_fDslvThick = 1.f;
	_float								m_vDslvColor[4] = { 1.f, 1.f, 1.f, 1.f };
	_float								m_vDslvGlowColor[4] = { 1.f, 1.f, 1.f, 1.f };
	string								m_strDslvTexKey = "Noise_1001";

// 새로운 이펙트 생성을 위한 변수들
private:
	vector<CEffectMesh::COLOR_KEYFRAME>		m_ColorKeyFrames;
	vector<CEffectMesh::SCALE_KEYFRAME>		m_ScaleKeyFrames;
	vector<CEffectMesh::TRANS_KEYFRAME>		m_TransKeyFrames;

	CEffectMesh::KEYFRAMES					m_KeyFrames;

	CEffectMesh::EffectMeshDesc				m_CreateEffect;		// 제작중인 이펙트

// 들고있는? 재생중인? 이펙트 리스트들
private:
	vector<CEffectMesh::EffectMeshDesc>		m_Effects;
	shared_ptr<CAnimEffect>			m_pAnimEffect;

private:
	ComPtr<ID3D11Device>				m_pDevice = nullptr;
	ComPtr<ID3D11DeviceContext>			m_pContext = nullptr;
};

END