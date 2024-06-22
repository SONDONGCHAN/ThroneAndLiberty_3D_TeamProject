#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

#include "Easing.h"

BEGIN(Engine)
class CBone;
class CModel;
class CTexture;
END

BEGIN(Tool_Effect)

class CTestModel : public CGameObject
{
public:
    // 셰이더용
    struct EffectMeshDesc
    {
        _float      fDuration = 5.f;

        _int        bBillBoard = 1;
        _float2     Padding1 = { 0.f, 0.f };

        _int         bUseDiffuse = 0;
        _int         bUseMask = 0;
        _int         bUseNoise = 0;
        _int         bUseColor = 0;

        _int         iMaskSamplerType = 0;     // 0 : WRAP / 1 : MIRROR / 2 : CLAMP / 3 : BORDER
        _int         iMaskUVType = 0;          // 0 : FIX  / 1 : SCROLL / 2 : ATLAS

        _float      fMaskDiscardValue = 0.f;
        _int         bMaskDiscardArea = 0;     // 0 : Less Than / 1 : More Than

        _float2     fMaskUVScroll;
        _float2     iMaskUVAtlasCnt;
        _float       fMaskUVAtlasSpeed;
        _float3     Padding2 = { 0.f, 0.f, 0.f };

        _int         iNoiseSamplerType = 0;     // 0 : WRAP / 1 : MIRROR / 2 : CLAMP / 3 : BORDER
        _int         iNoiseUVType = 0;          // 0 : FIX  / 1 : SCROLL / 2 : ATLAS
        
        _float      fNoiseDiscardValue = 0.f;
        _int         bNoiseDiscardArea = 0;     // 0 : Less Than / 1 : More Than


        _float2     fNoiseUVScroll = { 0.f, 0.f };
        _float2     iNoiseUVAtlasCnt;
        _float      fNoiseUVAtlasSpeed;

        // 나누는 구역 개수도 던져야 한다.
        _int        iColorSplitNum = 0;
        _float2     Padding3 = { 0.f, 0.f };
        // 셰이더에 던질 컬러 변수는
        // 현재 Split Value와 현재 Color 4개
        // 만 있으면 될껄?
        _float4     vColorSplitter = { 0.f, 0.f, 0.f, 0.f };
        _float4     vColor1 = { _float4(0.f, 0.f, 0.f, 1.f) };
        _float4     vColor2 = { _float4(0.f, 0.f, 0.f, 1.f) };
        _float4     vColor3 = { _float4(0.f, 0.f, 0.f, 1.f) };
        _float4     vColor4 = { _float4(0.f, 0.f, 0.f, 1.f) };

        // 셰이더용 가중치?
    };

public:
    CTestModel();
    virtual ~CTestModel();

public:
    virtual HRESULT Initialize(const string& _strModelKey);
    virtual void	PriorityTick(_float _fTimeDelta);
    virtual void	Tick(_float _fTimeDelta);
    virtual void	LateTick(_float _fTimeDelta);
    virtual HRESULT Render();

public:
    // 텍스처 (Diffuse, Mask, Noise)
    HRESULT         SetUseTexture(TEX_TYPE _eTexType, _bool _bUseTexture);
    HRESULT         SetTexture(TEX_TYPE _eTexType, const string& _strTextureTag);

    // 텍스처 옵션
    HRESULT         SetTexSampler(TEX_TYPE _eTexType, SAMPLE_TYPE _eSampleType);
    HRESULT         SetTexUV(TEX_TYPE _eTexType, UV_TYPE _eUVType);
    HRESULT         SetTexScrollOpt(TEX_TYPE _eTexType, _float2 _vScrollX, _float2 _vScrollY);
    HRESULT         SetTexAtlasOpt(TEX_TYPE _eTexType, _float2 _vAtlasCnt, _float _fAtalsSpeed);
    HRESULT         SetTexDiscardOpt(TEX_TYPE _eTexType, _float _fDiscardValue, _bool _bLessThan);

    // 컬러 옵션
    HRESULT         SetColorSetNum(_bool _bIncrase);
    HRESULT         SetColorChangeTime(_int _iColorSetIdx, _float _fColorChangeTime);
    HRESULT         SetColorSetEasing(_int _iColorSetIdx, _int _iEaseType);
    HRESULT         SetColorSplitNum(_int _iColorSplitNum);

    HRESULT         SetColorSplitter(_float _vColorSplitter[4]);
    HRESULT         SetColor(_int _iColorSetIdx, _int _iColorTimeIdx, _float4 _vColor);

    HRESULT         SetLifeTime(_float _fLifeTime) { m_fLifeTime = _fLifeTime; }

    // SRT 옵션
    HRESULT         SetFollowObj(_int _iTransformType, _int _iFollowType, shared_ptr<class CAnimModel> _pAnimModel, _float4x4* _pBoneMatrix = nullptr);
    
    HRESULT         SetTransformSetNum(_int _iTransformType, _int _iSetNum);
    HRESULT         SetTransformTime(_int _iTransformType, _int iIdx, _float _fTransformTime);
    HRESULT         SetTransformEasing(_int _iTransformType, _int iIdx, _int _iTransformEasing);
    HRESULT         SetTransformValue(_int _iTransformType, _int iIdx, _float* _fTransformValue);

private:
    shared_ptr<CModel>		m_pModelCom = nullptr;
    string                  m_pTextureKey[TEX_END] = {};

    shared_ptr<CTexture>	m_pTextureCom[TEX_END] = { 0 };

private:
    _bool                   m_bUseTexture[TEX_END] = { false };

    _float2                 m_vMaskScrollRangeX = { 0.f, 0.f };
    _float2                 m_vMaskScrollRangeY = { 0.f, 0.f };

    _float2                 m_vNoiseScrollRangeX = { 0.f, 0.f };
    _float2                 m_vNoiseScrollRangeY = { 0.f, 0.f };
    _uint	                m_iNoiseUVIdx = 0;
    _float2	                m_vNoiseUVPivot = { 0.f, 0.f };

    _float2	                m_vUVScale = { 1.f, 1.f };
    _uint	                m_iUVCnt = 0;
    _float	                m_fFrameTime = 0.f;

    // 컬러용 변수
    // 컬러 시간 세트 사용 개수도 알아야돼
    _int                    m_iColorSetNum = 1;

    // 4세트 묶음
    _float                  m_fColorTime[4] = { 0.f };    // 세트가 시작하는 시간
    _int                    m_iColorSplitAreaNum = { 0 };  // split 개수
    EASE_TYPE               m_eColorEasingType[4] = { EASE_LINEAR };  // 다음 세트 값으로 바뀌는 보간 공식
    _float                  m_vColorSplitter[4] = { 0.f };

    // 한 세트 당 컬러 4개
    _float4                 m_vColor[4][4] = { _float4(0.f, 0.f, 0.f, 0.f) };

    _int                    m_iCurColorSetIdx = 0;

    // 시간 관련 변수
    _float                  m_fActiveTime = 0.f;
    _float                  m_fLifeTime = 0.f;
    _float                  m_fTimeDelta = 0.f;

    EffectMeshDesc          m_eEffectMeshDesc = { 0 };

    // SRT용 변수
    shared_ptr<CGameObject> m_pFollowObject = nullptr;
    shared_ptr<CModel>      m_pFollowModel = nullptr;

    // Scale
    _bool                   m_bScaleChange = false;
    _int                    m_iCurScaleSetIdx = 0;

    _int                    m_iScaleSetNum = 0;
    _bool                   m_bScaleFollowObj = false;
    _bool                   m_bScaleFollowBone = false;
    shared_ptr<CGameObject> m_pScaleFollowObj = nullptr;
    _float4x4*              m_pScaleFollowBoneMat = nullptr;

    _float                  m_fScaleTime[MAX_TIMETABLE] = { 0.f };
    _float3                 m_fScale[MAX_TIMETABLE] = {};
    _int                    m_iScaleEasing[MAX_TIMETABLE] = { 0 };

    // Rotation
    _bool                   m_bRotationChange = false;
    _int                    m_iRotationSetNum = 0;
    _bool                   m_bRotationFollowObj = false;
    _bool                   m_bRotationFollowBone = false;
    shared_ptr<CGameObject> m_pRotationFollowObj = nullptr;
    shared_ptr<CBone>       m_pRotationFollowBone = nullptr;
    _float4x4*              m_pRotationFollowBoneMat = nullptr;

    _float3                 m_fRotation;
    _float3                 m_fTurnVel;
    _float3                 m_fTurnAcc;

    // Translation
    _bool                   m_bTranslationChange = false;
    _int                    m_iCurTranslationSetIdx = 0;

    _int                    m_iTranslationSetNum = 0;
    _bool                   m_bTranslationFollowObj = false;
    _bool                   m_bTranslationFollowBone = false;
    shared_ptr<CGameObject> m_pTranslationFollowObj = nullptr;
    shared_ptr<CBone>       m_pTranslationFollowBone = nullptr;
    _float4x4*              m_pTranslationFollowBoneMat = nullptr;

    _float                  m_fTranslationTime[MAX_TIMETABLE] = { 0.f };
    _float3                 m_fTranslation[MAX_TIMETABLE] = {};
    _int                    m_iTranslationEasing[MAX_TIMETABLE] = { 0 };

    _float4x4               m_OriginWorldMat;

    struct EffectKeyFrame
    {
        _float  fTime;

        _bool   bChangeScale;
        _bool   bFollowScale[3];
        _float3 vScale;
        _uint   eScaleEaseType;

        _bool   bChangeTranslation;
        _bool   bFollowTranslation[3];
        _float3 vTranslation;
        _uint   eTranslationEaseType;

        _bool   bChangeColor;
        _uint   eColorEaseType;
        _float4 vColorSplitter;
        _float4 vColor1;
        _float4 vColor2;
        _float4 vColor3;
        _float4 vColor4;
    };

    vector<EffectKeyFrame>  m_pKeyFrames;

private:
	ComPtr<ID3D11Device>				m_pDevice = nullptr;
	ComPtr<ID3D11DeviceContext>			m_pContext = nullptr;

public:
	static shared_ptr<CTestModel> Create(const string& _strModelKey);
};

END