#pragma once

#include "Effect.h"

BEGIN(Tool_Effect)

class CEffectParticle : public CEffect
{
public:
    // 셰이더 바인드용
    struct EffectParticleBindDesc
    {
        _float      fDuration = 5.f;

        _int        bBillBoard = 1;

        _float2     fPadding1;

        _int        bUseDiffuse = 0;
        _int        bUseMask = 0;
        _int        bUseNoise = 0;
        _int        bUseColor = 0;

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

        _float3     fPadding2;

        _float2     fNoiseUVScroll = { 0.f, 0.f };
        _float2     iNoiseUVAtlasCnt = { 1.f, 1.f };
        _float      fNoiseUVAtlasSpeed = 0.f;

        _int        iColorSplitArea = 1;
        _float2     fPadding3;
        _float4     vColorSplitter = { 0.f, 0.f, 0.f, 0.f };

        _float4     vColor1 = { 0.f, 0.f, 0.f, 0.f };
        _float4     vColor2 = { 0.f, 0.f, 0.f, 0.f };
        _float4     vColor3 = { 0.f, 0.f, 0.f, 0.f };
        _float4     vColor4 = { 0.f, 0.f, 0.f, 0.f };

        // 셰이더용 가중치?
    };
public:
    CEffectParticle();
    CEffectParticle(const CEffectParticle& rhs);
    virtual ~CEffectParticle();

public:
    virtual HRESULT Initialize(EffectDesc* pDesc);
    virtual void	PriorityTick(_float _fTimeDelta);
    virtual void	Tick(_float _fTimeDelta);
    virtual void	LateTick(_float _fTimeDelta);
    virtual HRESULT Render();

private:
    EffectParticleBindDesc  m_eEffectBindDesc = { 0 };

public:
    static shared_ptr<CEffectParticle> Create(EffectDesc* pDesc);
};

END