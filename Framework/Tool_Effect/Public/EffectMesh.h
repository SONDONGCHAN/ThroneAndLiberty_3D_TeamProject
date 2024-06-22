#pragma once

#include "Tool_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Tool_Effect)

class CEffectMesh : public CEffect
{
public:
    struct COLOR_KEYFRAME
    {
        // 해당 키프레임의 시작 시간
        _float fTime;

        _int		eEaseType;
        _float4		vColor[4];
    };

    struct GLOWCOLOR_KEYFRAME
    {
        // 해당 키프레임의 시작 시간
        _float fTime;

        _float4		vGlowColor;
    };

    struct SCALE_KEYFRAME
    {
        // 해당 키프레임의 시작 시간
        _float		fTime;

        _int		eEaseType;
        _float3		vScale;
    };

    struct TRANS_KEYFRAME
    {
        // 해당 키프레임의 시작 시간
        _float		fTime;

        _int		eEaseType;
        _float3		vTranslation;
    };

    struct KEYFRAMES
    {
        vector<COLOR_KEYFRAME> ColorKeyFrames;
        vector<SCALE_KEYFRAME> ScaleKeyFrames;
        vector<TRANS_KEYFRAME> TransKeyFrames;
        vector<GLOWCOLOR_KEYFRAME> GlowColorKeyFrames;

        _float4* GetCurColor(_float _fTime, _int iSplitCnt)
        {
            _float4 vCurColors[4] = {};

            for (auto iter = ColorKeyFrames.begin(); iter != ColorKeyFrames.end(); ++iter)
            {
                // 다음 요소에 접근 가능
                if (iter + 1 != ColorKeyFrames.end())
                {
                    auto iterNext = iter + 1;

                    if (iterNext->fTime <= _fTime)
                        continue;

                    for (_int i = 0; i < iSplitCnt; ++i)
                    {
                        vCurColors[i].x = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime - iter->fTime, iterNext->fTime - iter->fTime, iter->vColor[i].x, iterNext->vColor[i].x);
                        vCurColors[i].y = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime - iter->fTime, iterNext->fTime - iter->fTime, iter->vColor[i].y, iterNext->vColor[i].y);
                        vCurColors[i].z = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime - iter->fTime, iterNext->fTime - iter->fTime, iter->vColor[i].z, iterNext->vColor[i].z);
                        vCurColors[i].w = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime - iter->fTime, iterNext->fTime - iter->fTime, iter->vColor[i].w, iterNext->vColor[i].w);
                    }

                    return vCurColors;
                }
            }

            auto iter_end = ColorKeyFrames.end();
            return (--iter_end)->vColor;
        }

        _float4		GetCurGlowColor(_float _fTime)
        {
            _float4 vCurColor = {};

            for (auto iter = GlowColorKeyFrames.begin(); iter != GlowColorKeyFrames.end(); ++iter)
            {
                // 다음 요소에 접근 가능
                if (iter + 1 != GlowColorKeyFrames.end())
                {
                    auto iterNext = iter + 1;

                    if (iterNext->fTime <= _fTime)
                        continue;

                    vCurColor.x = GAMEINSTANCE->Ease(EASE_LINEAR, _fTime, iterNext->fTime, iter->vGlowColor.x, iterNext->vGlowColor.x);
                    vCurColor.y = GAMEINSTANCE->Ease(EASE_LINEAR, _fTime, iterNext->fTime, iter->vGlowColor.y, iterNext->vGlowColor.y);
                    vCurColor.z = GAMEINSTANCE->Ease(EASE_LINEAR, _fTime, iterNext->fTime, iter->vGlowColor.z, iterNext->vGlowColor.z);
                    vCurColor.w = GAMEINSTANCE->Ease(EASE_LINEAR, _fTime, iterNext->fTime, iter->vGlowColor.w, iterNext->vGlowColor.w);

                    return vCurColor;
                }
            }

            auto iter_end = GlowColorKeyFrames.end();
            return (--iter_end)->vGlowColor;
        }

        _float3		GetCurScale(_float _fTime)
        {
            _float3 vCurScale = {};

            for (auto iter = ScaleKeyFrames.begin(); iter != ScaleKeyFrames.end(); ++iter)
            {
                // 다음 요소에 접근 가능
                if (iter + 1 != ScaleKeyFrames.end())
                {
                    auto iterNext = iter + 1;

                    if (iterNext->fTime <= _fTime)
                        continue;

                    vCurScale.x = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime, iterNext->fTime, iter->vScale.x, iterNext->vScale.x);
                    vCurScale.y = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime, iterNext->fTime, iter->vScale.y, iterNext->vScale.y);
                    vCurScale.z = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime, iterNext->fTime, iter->vScale.z, iterNext->vScale.z);

                    return vCurScale;
                }
            }

            auto iter_end = ScaleKeyFrames.end();
            return (--iter_end)->vScale;
        }

        _float3		GetCurTrans(_float _fTime)
        {
            _float3 vCurTrans = {};

            for (auto iter = TransKeyFrames.begin(); iter != TransKeyFrames.end(); ++iter)
            {
                // 다음 요소에 접근 가능
                if (iter + 1 != TransKeyFrames.end())
                {
                    auto iterNext = iter + 1;

                    if (iterNext->fTime <= _fTime)
                        continue;

                    vCurTrans.x = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime, iterNext->fTime, iter->vTranslation.x, iterNext->vTranslation.x);
                    vCurTrans.y = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime, iterNext->fTime, iter->vTranslation.y, iterNext->vTranslation.y);
                    vCurTrans.z = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime, iterNext->fTime, iter->vTranslation.z, iterNext->vTranslation.z);

                    return vCurTrans;
                }
            }

            auto iter_end = TransKeyFrames.end();
            return (--iter_end)->vTranslation;
        }
    };

    // 파싱용
    struct EffectMeshDesc : public EffectDesc
    {
        string      strModelKey = "";

        _bool       bBillBoard = 1;

        _bool       bUseMask = 0;
        string      strTexMaskKey = "";
        _bool       bUseNoise = 0;
        string      strTexNoiseKey = "";
        _bool       bUseColor = 0;

        _int        iDiffuseSamplerType = 0;     // 0 : WRAP / 1 : MIRROR / 2 : CLAMP / 3 : BORDER
        _int        iDiffuseUVType = 0;          // 0 : FIX  / 1 : SCROLL / 2 : ATLAS

        _int        iDiffuseDiscardArea = 0;
        _float      fDiffuseDiscardValue = 0.f;

        _float2     fDiffuseUVScroll = { 0.f, 0.f };
        _float2     iDiffuseUVAtlasCnt = { 1.f, 1.f };
        _float      fDiffuseUVAtlasSpeed = 0.f;

        _int        iMaskSamplerType = 0;     // 0 : WRAP / 1 : MIRROR / 2 : CLAMP / 3 : BORDER
        _int        iMaskUVType = 0;          // 0 : FIX  / 1 : SCROLL / 2 : ATLAS

        _float      fMaskDiscardValue = 0.f;
        _int        iMaskDiscardArea = 0;     // 0 : Less Than / 1 : More Than

        _float2     fMaskUVScroll = { 0.f,0.f };
        _float2     iMaskUVAtlasCnt = { 1.f, 1.f };
        _float      fMaskUVAtlasSpeed = 0.f;

        _int        iNoiseSamplerType = 0;     // 0 : WRAP / 1 : MIRROR / 2 : CLAMP / 3 : BORDER
        _int        iNoiseUVType = 0;          // 0 : FIX  / 1 : SCROLL / 2 : ATLAS

        _float      fNoiseDiscardValue = 0.f;
        _int        iNoiseDiscardArea = 0;     // 0 : Less Than / 1 : More Than

        _float2     fNoiseUVScroll = { 0.f, 0.f };
        _float2     iNoiseUVAtlasCnt = { 1.f, 1.f };
        _float      fNoiseUVAtlasSpeed = 0.f;

        _int        iColorSetCnt = 1;
        _int        iColorSplitArea = 1;
        _float4     vColorSplitter = { 0.f, 0.f, 0.f, 0.f };

        _int        iScaleFollowType = 0;       // -1 : No Follow / 0 : Obj / 1 : Bone
        string      strScaleFollowBoneKey = "";
        _int        iScaleSetCnt = 1;

        _int        iRotFollowType = 0;
        string      strRotFollowBoneKey = "";

        _float3     vRotation = { 0.f, 0.f, 0.f };
        _float3     vTurnVel = { 0.f, 0.f, 0.f };
        _float3     vTurnAcc = { 0.f, 0.f, 0.f };

        _int        iTransFollowType = 0;
        string      strTransFollowBoneKey = "";
        _int        iTransSetCnt = 1;

        _bool       bUseGlowColor = false;
        _int        iGlowColorSetCnt = 1;

        _bool       bUseDistortion = false;
        string      strDistortionKey = "";
        _float      fDistortionSpeed = 1.f;
        _float      fDistortionWeight = 1.f;

        _bool       bUseDslv = false;
        _int        iDslvType = 0;  // 0 : Dslv In, 1 : Dslv Out
        _float      fDslvStartTime = 0.f;
        _float      fDslvDuration = 0.f;
        _float      fDslvThick = 1.f;
        _float4     vDslvColor = { 1.f, 1.f, 1.f, 1.f };
        string      strDslvTexKey = "";
        _bool       bDslvGlow = false;
        _float4     vDslvGlowColor = { 1.f, 1.f, 1.f, 1.f };

        KEYFRAMES   KeyFrames;

    };

    // 셰이더 바인드용
    struct EffectMeshBindDesc
    {
        _float      fDuration = 5.f;

        _int        bBillBoard = 1;

        _float2     fPadding1;

        //=====1

        _int        bUseDiffuse = 0;
        _int        bUseMask = 0;
        _int        bUseNoise = 0;
        _int        bUseColor = 0;

        //=====2

        _int        iDiffuseSamplerType = 0;     // 0 : WRAP / 1 : MIRROR / 2 : CLAMP / 3 : BORDER
        _int        iDiffuseUVType = 0;          // 0 : FIX  / 1 : SCROLL / 2 : ATLAS

        _float      fDiffuseDiscardValue = 0.f;
        _int        iDiffuseDiscardArea = 0;     // 0 : Less Than / 1 : More Than

        //=====3

        _float2     fDiffuseUVScroll = { 0.f, 0.f };
        _float2     iDiffuseUVAtlasCnt = { 1.f, 1.f };

        //=====4

        _float      fDiffuseUVAtlasSpeed = 0.f;

        _float3     fPadding4;

        //====5

        _int        iMaskSamplerType = 0;     // 0 : WRAP / 1 : MIRROR / 2 : CLAMP / 3 : BORDER
        _int        iMaskUVType = 0;          // 0 : FIX  / 1 : SCROLL / 2 : ATLAS

        _float      fMaskDiscardValue = 0.f;
        _int        iMaskDiscardArea = 0;     // 0 : Less Than / 1 : More Than

        //====6

        _float2     fMaskUVScroll = { 0.f,0.f };
        _float2     iMaskUVAtlasCnt = { 1.f, 1.f };

        //====7

        _float      fMaskUVAtlasSpeed = 0.f;

        _int        iNoiseSamplerType = 0;     // 0 : WRAP / 1 : MIRROR / 2 : CLAMP / 3 : BORDER
        _int        iNoiseUVType = 0;          // 0 : FIX  / 1 : SCROLL / 2 : ATLAS

        _float      fNoiseDiscardValue = 0.f;

        //====8
        _int        iNoiseDiscardArea = 0;     // 0 : Less Than / 1 : More Than

        _float3     fPadding2;
        //====9

        _float2     fNoiseUVScroll = { 0.f, 0.f };
        _float2     iNoiseUVAtlasCnt = { 1.f, 1.f };
        //===10
        _float      fNoiseUVAtlasSpeed = 0.f;

        _int        iColorSplitArea = 1;
        _float2     fPadding3;
        //===11
        _float4     vColorSplitter = { 0.f, 0.f, 0.f, 0.f };

        //===12

        _float4     vColor1 = { 0.f, 0.f, 0.f, 0.f };
        _float4     vColor2 = { 0.f, 0.f, 0.f, 0.f };
        _float4     vColor3 = { 0.f, 0.f, 0.f, 0.f };
        _float4     vColor4 = { 0.f, 0.f, 0.f, 0.f };

        // 셰이더용 가중치?
    };

public:
    CEffectMesh();
    CEffectMesh(const CEffectMesh& rhs);
    virtual ~CEffectMesh();

public:
    virtual HRESULT Initialize(EffectMeshDesc* pDesc);
    virtual void	PriorityTick(_float _fTimeDelta);
    virtual void	Tick(_float _fTimeDelta);
    virtual void	LateTick(_float _fTimeDelta);
    virtual HRESULT Render();
    virtual HRESULT RenderGlow() override;
    virtual HRESULT RenderDistortion() override;

public:
    virtual HRESULT SetOwner(shared_ptr<CGameObject> _pOwner) override;
    EffectMeshDesc* GetMeshDesc() { return &m_eEffectDesc; }

private:
    HRESULT         Initialize_Desc(EffectMeshDesc* pDesc);
    void            Reset();

private:
    shared_ptr<CModel>		m_pModelCom = nullptr;
    EffectMeshBindDesc      m_eEffectBindDesc = { 0 };
    EffectMeshDesc          m_eEffectDesc = { 0 };

private:
    // 키프레임으로부터 뽑아온 정보들
    _float4                 m_vCurColor[4] = {};
    _float3                 m_vCurScale = {};
    _float3                 m_vCurTrans = {};

    _float4x4               m_vCurTransMatrix;

    _uint	                m_iDiffuseUVIdx = 0;
    _float2	                m_vDiffuseUVPivot = { 0.f, 0.f };
    _float2                 m_vDiffuseUVScroll = { 0.f, 0.f };

    _uint	                m_iMaskUVIdx = 0;
    _float2	                m_vMaskUVPivot = { 0.f, 0.f };
    _float2                 m_vMaskUVScroll = { 0.f, 0.f };

    _uint	                m_iNoiseUVIdx = 0;
    _float2	                m_vNoiseUVPivot = { 0.f, 0.f };
    _float2                 m_vNoiseUVScroll = { 0.f, 0.f };

    _float                  m_fDistortionTimer = 0.f;

    _float2	                m_vUVScale = { 1.f, 1.f };
    _uint	                m_iUVCnt = 0;

    _float4x4* m_pSFollowBoneMat = nullptr;
    _float4x4* m_pRFollowBoneMat = nullptr;
    _float4x4* m_pTFollowBoneMat = nullptr;

    _bool                   m_bFirstTrans = true;
    _bool                   m_bFirstRot = true;
    _bool                   m_bFirstScale = true;

private:
    _bool                   m_bSetStartTrans = false;
    _float3                 m_vStartTrans = {};

public:
    static shared_ptr<CEffectMesh> Create(EffectMeshDesc* pDesc);
};

END