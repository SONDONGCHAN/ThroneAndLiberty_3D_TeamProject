#pragma once
#include "Effect.h"
#include "ParticleSystem.h"

BEGIN(Engine)
class CShader;
class CVIInstanceParticle;
END

BEGIN(Tool_Effect)

class CTestParticle : public CEffect
{
public:
    CTestParticle();
    CTestParticle(const CTestParticle& rhs);
    ~CTestParticle();

public:
    virtual HRESULT Initialize(_int _iNumInstance, string _strTexKey);
    virtual void	PriorityTick(_float _fTimeDelta);
    virtual void	Tick(_float _fTimeDelta);
    virtual void	LateTick(_float _fTimeDelta);
    virtual HRESULT Render();
    virtual HRESULT RenderGlow() override;

private:
    void            SpawnParticles(_float _fTimeDelta);
    void            UpdateParticles(_float _fTimeDelta);
    void            CheckDeadParticles(_float _fTimeDelta);

    void            CreateParticle(_int _iIndex);

public:
    void            SetDefaultInfo(_int _iShaderType, _int _iShaderPass, _int _iRenderGroup);
    void            SetMainInfo(_int _iMaxParticles, _float _fDuration, _float _fStartTrackPos, _float2 _fLifeTimeRange, _bool _bLoop, _int _iBillBoardType, string _strTexKey, _int _iSpawnType, _int _iSpawnNumPerSec);
    void            SetPosInfo(_int _iPosType, _bool _bPosFollow, string _strPosFollowBoneKey, _int _iPosSetCnt, _float* _fPosTime, _float _vPosMin[5][3], _float _vPosMax[5][3], _int* _iPosEasing, _bool _bUsePosKeyFrame);
    void            SetSpeedInfo(_bool _bSpeedMode, _int _iSpeedSetCnt, _float* _fSpeedTimes, _float _vSpeedMins[5][3], _float _vSpeedMax[5][3], _int* _iSpeedEasing);
    void            SetSpeedInfo(_bool _bSpeedMode, _float3 _vStartSpeedMin, _float3 _vStartSpeedMax, _float3 _vSpeedAccMin, _float3 _vSpeedAccMax);
    void            SetScaleInfo(_int _iScaleSetCnt, _float* _fScaleTimes, _float _vScaleMin[5][3], _float _vScaleMax[5][3], _int* _iScaleEasing);
    void            SetRotationInfo(_float* _vRotationMin, _float* _vRotationMax, _float* _vTurnVelMin, _float* _vTurnVelMax, _float* _fTurnAccMin, _float* _fTurnAccMax);
    void            SetColorInfo(_bool _bUseColor, _int _iColorSetCnt, _float* _fColorTimes, _float _vColorMin[5][4], _float _vColorMax[5][4], _int* _iColorEasing);
    void            SetShaderInfo(_bool _bUseGlowColor, _float _vGlowColor[4]);
    void            SetHazeInfo(_bool _bUseHazeEffect, _float _vHazeSpeedMin[3], _float _vHazeSpeedMax[3], _bool _bRelativeOwner);
    void            SetPivotInfo(_bool _bUsePivot, _float _vPivotMin[3], _float _vPivotMax[3]);
    void            SetImplosionInfo(_bool _bUseImplosion);

private:
    shared_ptr<CShader>             m_pShaderCom = { nullptr };
    shared_ptr<CVIInstanceParticle> m_pVIBufferCom = { nullptr };

private:
    CParticleSystem::PARTICLE_SYSTEM m_eParticleSystem;

    PARTICLE* m_pParticles = nullptr;

    _int            m_iCurrentParticleCnt = 0;
    _float          m_fSpawnTime = 0.f;
    _float          m_fHazeTime = 0.f;
    _float          m_fHazeTimeMax = 0.1f;
    _float3         m_vHazeSpeed = { 0.f, 0.f, 0.f };

    _float4x4*      m_pFollowBoneMat = nullptr;

    _float3         m_vStartPosMin;
    _float3         m_vStartPosMax;
    _float3         m_vStartSpeedMin;
    _float3         m_vStartSpeedMax;
    _float3         m_vStartScaleMin;
    _float3         m_vStartScaleMax;
    _float4         m_vStartColorMin;
    _float4         m_vStartColorMax;

    _bool           m_bEmitted = false;
    _bool           m_bCanEmit = true;
    _bool           m_bFinished = false;

    _float4x4       m_OwnerWorld;
    _bool           m_bEntered = true;

    _float3         m_vCurPos;
    _int            m_iPosIdx = 0;
    _float3         m_vNextPosMin;
    _float3         m_vNextPosMax;
    _float4x4       m_vCurPosMatrix;

    _float3         m_vCurPivot;
    _float4x4       m_vCurPivotMatrix;

    _float3         m_vCurSpeed;
    _float4x4       m_vCurSpeedMatrix;

    _float3         m_vCurDir;

public:
    static shared_ptr<CTestParticle> Create(_int _iNumInstance, string _strTexKey);
};

END