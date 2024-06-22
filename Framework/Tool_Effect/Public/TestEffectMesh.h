#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

#include "EffectMesh.h"

BEGIN(Tool_Effect)

class CTestEffectMesh : public CGameObject
{
public:
    CTestEffectMesh();
    CTestEffectMesh(const CTestEffectMesh& rhs);
    virtual ~CTestEffectMesh();

public:
    virtual HRESULT Initialize(const string& _strModelKey);
    virtual void	PriorityTick(_float _fTimeDelta);
    virtual void	Tick(_float _fTimeDelta);
    virtual void	LateTick(_float _fTimeDelta);
    virtual HRESULT Render();
    virtual HRESULT RenderGlow() override;
    virtual HRESULT RenderDistortion() override;
    virtual HRESULT RenderEdge() override;

private:
    shared_ptr<CModel>                  m_pModelCom = nullptr;
    CEffectMesh::EffectMeshBindDesc     m_eEffectBindDesc = { 0 };
    CEffectMesh::EffectMeshDesc         m_eEffectDesc = { 0 };

private:
    _bool       m_bPlaying = true;
    
    _float      m_fSpeedTime = 0.f;
    _float      m_fSpeed = 1.f;

    _float      m_fActiveTime = 0.f;
    _int        m_iCurKeyFrameIdx = 0;

    _int        m_iDiffuseUVIdx = 0;
    _float2     m_vDiffuseUVScroll = { 0.f, 0.f };
    _float2     m_vDiffuseUVPivot = { 0.f, 0.f };
    
    _int        m_iMaskUVIdx = 0;
    _float2     m_vMaskUVScroll = { 0.f, 0.f };
    _float2     m_vMaskUVPivot = { 0.f, 0.f };

    _int        m_iNoiseUVIdx = 0;
    _float2     m_vNoiseUVScroll = { 0.f, 0.f };
    _float2     m_vNoiseUVPivot = { 0.f, 0.f };

    _float		m_fDistortionTimer = 0.f;

    _float4     m_vCurColor[4] = { _float4(1.f, 1.f, 1.f, 1.f) };
    _float3     m_vCurScale = { 1.f, 1.f, 1.f };
    _float3     m_vCurTrans = { 0.f, 0.f, 0.f };

    weak_ptr<CGameObject> m_pOwner = {};

    _float4x4*  m_pSFollowBoneMat = nullptr;
    _float4x4*  m_pRFollowBoneMat = nullptr;
    _float4x4*  m_pTFollowBoneMat = nullptr;

    string      m_strTextureKey[TEX_END];

    _bool       m_bSetScale = false, m_bSetTrans = false;

    _bool       m_bEnableRender = true;

    _bool       m_bFirstRot = true;
    _bool       m_bFirstTrans = true;
    _bool       m_bFirstScale = true;

public:
    void        SetDefaultInfo(_int _iShaderType, _int _iShaderPass, _int _iRenderGroup, _float _fDuration, _float _StartTrackPos, _bool _bBillBoard, _bool _bLoop);
    void        SetDiffuseInfo(_bool _bUseDiffuse, string _strDiffuseKey, _int _iDiffuseSamplerType,
        _int _iDiffuseDiscardArea, _float _fDiffuseDiscardValue, _int _iDiffuseUVType,
        _float2 _vDiffuseUVScroll, _float2 _vDiffuseUVAtlasCnt, _float _fDiffuseUVAtalsSpeed);
    void        SetColorInfo(_bool _bUseColor, _int _iColorSplitArea, _float4 _vColorSplitter, _int _iColorSetCnt, _float* _fColorTime, _int* _iColorEase, _float4 _vColor[5][4]);
    void        SetMaskInfo(_bool _bUseMask, string _strMaskKey, _int _iMaskSamplerType, _int _iMaskDiscardArea, _float _fMaskDiscardValue, _int _iMaskUVType, _float2 _vMaskUVScroll, _float2 _vMaskUVAtlasCnt, _float _fMaskUVAtalsSpeed);
    void        SetNoiseInfo(_bool _bUseNoise, string _strNoiseKey, _int _iNoiseSamplerType, _int _iNoiseDiscardArea, _float _fNoiseDiscardValue, _int _iNoiseUVType, _float2 _vNoiseUVScroll, _float2 _vNoiseUVAtlasCnt, _float _fNoiseUVAtalsSpeed);
    void        SetScaleInfo(_bool _bScaleFollow, _int _iScaleFollowType, string _strFollowBoneName, _int _iScaleCnt, _float* _fScaleTime, _float _fScale[5][3], _int* _iScaleEasing);
    void        SetRotInfo(_bool _bRotationFollow, _int _iRotationFollowType, string _strFollowBoneName, _float3 _fRotate, _float3 _fTurnVel, _float3 _fTurnAcc);
    void        SetTransInfo(_bool _bTransFollow, _int _iTransFollowType, string _strFollowBoneName, _int _iTransCnt, _float* _fTransTime, _float _fTrans[5][3], _int* _iTransEasing);
    void        SetOwnerInfo(shared_ptr<CGameObject> _pOwner);
    void        SetUseSoftShader(_bool _bUseSoftEffect);
    void        SetShaderGlowInfo(_bool _bUseGlowColor, _int _iGlowColorSetCnt, _float* _fGlowColorTime, _float _vGlowColor[5][4]);
    void        SetDistiortion(_bool _bUseDistortion, string _strDistortionKey, _float _fDistortionSpeed, _float _fDistortionWeight);
    void        SetDissolveT(_bool _bUseDissolve, _int _iDslvType, _float _fDslvStartTime, _float _fDslvDuration, _float _fDslvThick, _float4 _vDslvColor, string _strDslvTexKey, _bool _bDslvGlow, _float4 _vDslvGlowColor);


    void        Reset();

public:
    HRESULT     SetOwner(_bool _bSetOwner, _int _iType, shared_ptr<CGameObject> _pOwner);
    HRESULT     SetOwner(shared_ptr<CGameObject> _pOwner);

public:
    static shared_ptr<CTestEffectMesh> Create(const string& _strModelKey);
};

END