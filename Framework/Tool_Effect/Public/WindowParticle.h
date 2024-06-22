#pragma once

#include "Tool_Defines.h"
#include "EffectParticle.h"

#include "ParticleSystem.h"
#include "TestParticle.h"

BEGIN(Engine)
class CTexture;
class CBone;
END


BEGIN(Tool_Effect)

class CWindowParticle
{
public:
	CWindowParticle();
	~CWindowParticle() = default;

public:
	HRESULT Initialize();
	void	Tick(_float _fTimeDelta);

public:
	// 0. 디폴트
	void	Properties_Default();
	// 1. 메인
	void	Properties_Main();
	// 2. 위치 (파티클 시스템)
	void	Properties_Position();
	// 3. 스피드
	void	Properties_Speed();
	// 4. 크기
	void	Properties_Scale();
	// 5. 회전
	void	Properties_Rotation();
	// 6. 색상
	void	Properties_Color();
	// 7. 파티클 시스템이 이루는 모양
	void	Properties_Shape();
	// 8. 셰이더
	void	Properties_Shader();

public:
	shared_ptr<CParticleSystem>		CreateEffect();
	void							CreateTestEffect(CParticleSystem::PARTICLE_SYSTEM* _pEffectDesc, shared_ptr<CGameObject> _pOwner);

private:
	void	KeyInput();

private:
	HRESULT	LoadResourceNames(const wstring& _strResourcePath, TEX_TYPE _eTexType);

private:
	vector<string>						m_TextureList[TEX_END];
	_int								m_iTextureIdx[TEX_END] = { 0 };

private:
	// 0. Default
	_int								m_iShaderType = ESHADER_TYPE::ST_PARTICLE;
	_int								m_iShaderPass = 0;
	_int								m_iRenderGroup = CRenderer::RENDER_BLEND;

	// 1. 기본 정보
	_int								m_iNumInstance = 100;
	_float								m_fStartTrackPos = 0.0;
	_float								m_fDuration = 1.f;
	_float								m_fLifeTime[2] = { 1.f, 3.f };
	_bool								m_bLoop = false;
	_int								m_iBllBoardType = 0; // 0 : 기본, 1 : 빌보드, 2 : 방향성
	string								m_strDiffuseTexTag = "";
	_int								m_iSpawnType = 0;	// 0 : Continuous, 1 : Burst
	_int								m_iSpawnNumPerSec = 10;

	// 2. Position
	_int								m_iPosType = { 0 };		// 0 : World, 1 : Object, 2 : Bone
	_bool								m_bPosFollow = { false };
	string								m_strPosFollowBoneKey = "";

	_int								m_pPosFollowBoneIdx;

	_bool								m_bUsePosKeyFrame = false;

	_int								m_iPosSetCnt = 1;
	_float								m_fPosTime[MAX_TIMETABLE] = { 0.f };
	_float								m_vPosMin[MAX_TIMETABLE][3] = { 0.f, 0.f, 0.f };
	_float								m_vPosMax[MAX_TIMETABLE][3] = { 0.f, 0.f, 0.f };
	_int								m_iPosEasing[MAX_TIMETABLE] = { 0 };
	
	_bool								m_bUsePivot;
	_float								m_vPivotMin[3] = { 0.f, 0.f, 0.f };
	_float								m_vPivotMax[3] = { 0.f, 0.f, 0.f };

	// 파싱X
	string								m_strPosTimeLabel[MAX_TIMETABLE];
	string								m_strPosMinLabel[MAX_TIMETABLE];
	string								m_strPosMaxLabel[MAX_TIMETABLE];
	string								m_strPosEaseTypeLabel[MAX_TIMETABLE];
	
	string								m_strPivotTimeLabel[MAX_TIMETABLE];
	string								m_strPivotMinLabel[MAX_TIMETABLE];
	string								m_strPivotMaxLabel[MAX_TIMETABLE];
	string								m_strPivotEaseTypeLabel[MAX_TIMETABLE];

	// 2-1. Haze
	_bool								m_bUseHazeEffect = false;
	_float								m_vHazeSpeedMin[3] = { -1.f, -1.f, -1.f };
	_float								m_vHazeSpeedMax[3] = { 1.f, 1.f, 1.f };

	// 2-2. Implosion
	_bool								m_bImplosion = false;

	// 3. Speed
	_int								m_iSpeedType = 0;		// 0 : TimeTable, 1 : Acceleration, // 2 : Implosion TimeTable, 3 : Implosion Acc

	// Speed Time Mode
	_int								m_iSpeedSetCnt = 1;
	_float								m_fSpeedTime[MAX_TIMETABLE] = { 0.f };
	_float								m_fSpeedMin[MAX_TIMETABLE][3] = { -1.f, -1.f, -1.f };
	_float								m_fSpeedMax[MAX_TIMETABLE][3] = { 1.f, 1.f, 1.f };
	_int								m_iSpeedEasing[MAX_TIMETABLE] = { 0 };

	// Speed Acc Mode
	_float								m_vStartSpeedMin[3];
	_float								m_vStartSpeedMax[3];
	_float								m_vSpeedAccMin[3];
	_float								m_vSpeedAccMax[3];

	_bool								m_bRelativeOwner = false;

	// 파싱X
	string								m_strSpeedTimeLabel[MAX_TIMETABLE];
	string								m_strSpeedMinLabel[MAX_TIMETABLE];
	string								m_strSpeedMaxLabel[MAX_TIMETABLE];
	string								m_strSpeedEaseTypeLabel[MAX_TIMETABLE];

	// 4. Scale
	_int								m_iScaleSetCnt = 1;
	_float								m_fScaleTime[MAX_TIMETABLE] = { 0.f };
	_float								m_fScaleMin[MAX_TIMETABLE][3] = { 1.f, 1.f, 1.f };
	_float								m_fScaleMax[MAX_TIMETABLE][3] = { 1.f, 1.f, 1.f };
	_int								m_iScaleEasing[MAX_TIMETABLE] = { 0 };

	// 파싱X
	string								m_strScaleTimeLabel[MAX_TIMETABLE];
	string								m_strScaleMinLabel[MAX_TIMETABLE];
	string								m_strScaleMaxLabel[MAX_TIMETABLE];
	string								m_strScaleEaseTypeLabel[MAX_TIMETABLE];

	// 5. Rotation
	_float								m_vRotationMin[3] = { 0.f, 0.f, 0.f };
	_float								m_vRotationMax[3] = { 0.f, 0.f, 0.f };

	_float								m_vTurnVelMin[3] = { 0.f, 0.f, 0.f };
	_float								m_vTurnVelMax[3] = { 0.f, 0.f, 0.f };

	_float								m_vTurnAccMin[3] = { 0.f, 0.f, 0.f };
	_float								m_vTurnAccMax[3] = { 0.f, 0.f, 0.f };

	// 6. Color
	_bool								m_bUseColor = { false };
	_int								m_iColorSetCnt = 1;
	_float								m_fColorTime[MAX_TIMETABLE] = { 0.f };
	_float								m_vColorMin[MAX_TIMETABLE][4] = { 1.f, 1.f, 1.f, 1.f };
	_float								m_vColorMax[MAX_TIMETABLE][4] = { 1.f, 1.f, 1.f, 1.f };
	_int								m_iColorEasing[MAX_TIMETABLE] = { 0 };

	// 파싱X
	string								m_strColorTimeLabel[MAX_TIMETABLE];
	string								m_strColorMinLabel[MAX_TIMETABLE];
	string								m_strColorMaxLabel[MAX_TIMETABLE];
	string								m_strColorEaseTypeLabel[MAX_TIMETABLE];

	shared_ptr<class CAnimModel>		m_pFollowObj;
	shared_ptr<CModel>					m_pFollowObjModel;
	vector<shared_ptr<CBone>>			m_pFollowObjBones;
	vector<const _char*>				m_vecFollowBoneNames;

	// 9. Shader
	_bool								m_bUseSoftEffect = false;

	_int								m_iSoftEffectType = 0;	// 0 : Glow, 1 : Blur
	_bool								m_bUseGlowColor = false;
	_float								m_fGlowColor[4] = { 1.f, 1.f, 1.f, 1.f };

	_bool								m_bUseBlur = false;


	// 새로운 이펙트 생성을 위한 변수들
private:
	vector<CParticleSystem::COLOR_KEYFRAME_PARTICLE>		m_ColorKeyFrames;
	vector<CParticleSystem::SCALE_KEYFRAME_PARTICLE>		m_ScaleKeyFrames;
	vector<CParticleSystem::SPEED_KEYFRAME_PARTICLE>		m_SpeedKeyFrames;

	CParticleSystem::KEYFRAMES_PARTICLE						m_KeyFrames;

	CParticleSystem::PARTICLE_SYSTEM						m_CreateParticle;		// 제작중인 이펙트

	shared_ptr<CTestParticle>								m_pTestParticle;

	// 들고있는? 재생중인? 이펙트 리스트들
private:
	vector<CEffect::EffectDesc>			m_Effects;

	_bool								m_isTested = false;


private:
	ComPtr<ID3D11Device>				m_pDevice = nullptr;
	ComPtr<ID3D11DeviceContext>			m_pContext = nullptr;
};

END