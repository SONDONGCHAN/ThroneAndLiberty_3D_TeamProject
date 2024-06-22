
#pragma once
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CVIInstanceParticle;
END

BEGIN(Tool_Effect)

class CParticleSystem : public CEffect
{
public:
    struct COLOR_KEYFRAME_PARTICLE
    {
        _float      fTime = { 0.f };

        _int        eEaseType = { 0 };

        _float4     vColorMin = { 0.f, 0.f, 0.f, 0.f };
        _float4     vColorMax = { 0.f, 0.f, 0.f, 0.f };
    };

    struct SCALE_KEYFRAME_PARTICLE
    {
        _float      fTime = { 0.f };

        _int        eEaseType = { 0 };

        _float3     vScaleMin = { 0.f, 0.f, 0.f };
        _float3     vScaleMax = { 0.f, 0.f, 0.f };
    };

    struct SPEED_KEYFRAME_PARTICLE
    {
        _float      fTime = { 0.f };

        int         eEaseType = { 0 };

        _float3     vSpeedMin = { 0.f, 0.f, 0.f };
        _float3     vSpeedMax = { 0.f, 0.f, 0.f };
    };

    struct POSITION_KEYFRAME_PARTICLE
    {
        _float      fTime = { 0.f };

        int         eEaseType = { 0 };

        _float3     vPosMin = { 0.f, 0.f, 0.f };
        _float3     vPosMax = { 0.f, 0.f, 0.f };
    };

    struct KEYFRAMES_PARTICLE
    {
        vector<COLOR_KEYFRAME_PARTICLE> ColorKeyFrames;
        vector<SCALE_KEYFRAME_PARTICLE> ScaleKeyFrames;
        vector<SPEED_KEYFRAME_PARTICLE> SpeedKeyFrames;
        vector<POSITION_KEYFRAME_PARTICLE> PosKeyFrames;
        //vector<POSITION_KEYFRAME_PARTICLE> PosKeyFrames;

        _float4 GetNextColor(_int iCurIdx, _float _fTime)
        {
            _float4 vNextColor = {};

            // 현재 인덱스가 마지막 키프레임이 아닐 때
            if (ColorKeyFrames.size() > iCurIdx + 1)
            {
                // 인덱스가 처음 넘어갔을 때 (Next Color를 랜덤으로 추출해서 들고있도록)
                if (ColorKeyFrames[iCurIdx].fTime <= _fTime)
                {
                    vNextColor.x = GAMEINSTANCE->PickRandomFloat(ColorKeyFrames[iCurIdx + 1].vColorMin.x, ColorKeyFrames[iCurIdx + 1].vColorMax.x, true);
                    vNextColor.y = GAMEINSTANCE->PickRandomFloat(ColorKeyFrames[iCurIdx + 1].vColorMin.y, ColorKeyFrames[iCurIdx + 1].vColorMax.y, true);
                    vNextColor.z = GAMEINSTANCE->PickRandomFloat(ColorKeyFrames[iCurIdx + 1].vColorMin.z, ColorKeyFrames[iCurIdx + 1].vColorMax.z, true);
                    vNextColor.w = GAMEINSTANCE->PickRandomFloat(ColorKeyFrames[iCurIdx + 1].vColorMin.w, ColorKeyFrames[iCurIdx + 1].vColorMax.w, true);

                    return vNextColor;
                }

                // 인덱스 아직 안넘어갔을 때 (들고 있던 Next Color를 사용하도록)
                return _float4(0.f, -101.f, 0.f, 0.f);
            }

            // 현재 인덱스가 마지막 키프레임일 때
            else
            {
                return _float4(-101.f, 0.f, 0.f, 0.f);
            }
        }

        _float4 GetCurColor(_int iCurIdx, _float _fTime, _float4 vColor, _float4 vNextColor)
        {
            _float4 vCurColor;

            vCurColor.x = GAMEINSTANCE->Ease((EASE_TYPE)ColorKeyFrames[iCurIdx - 1].eEaseType, _fTime, ColorKeyFrames[iCurIdx].fTime, vColor.x, vNextColor.x);
            vCurColor.y = GAMEINSTANCE->Ease((EASE_TYPE)ColorKeyFrames[iCurIdx - 1].eEaseType, _fTime, ColorKeyFrames[iCurIdx].fTime, vColor.y, vNextColor.y);
            vCurColor.z = GAMEINSTANCE->Ease((EASE_TYPE)ColorKeyFrames[iCurIdx - 1].eEaseType, _fTime, ColorKeyFrames[iCurIdx].fTime, vColor.z, vNextColor.z);
            vCurColor.w = GAMEINSTANCE->Ease((EASE_TYPE)ColorKeyFrames[iCurIdx - 1].eEaseType, _fTime, ColorKeyFrames[iCurIdx].fTime, vColor.w, vNextColor.w);

            return vCurColor;
        }

        _float3 GetNextScale(_int iCurIdx, _float _fTime)
        {
            _float3 vNextScale = {};

            // 현재 인덱스가 마지막 키프레임이 아닐 때
            if (ScaleKeyFrames.size() > iCurIdx + 1)
            {
                // 인덱스가 처음 넘어갔을 때 (Next Scale를 랜덤으로 추출해서 들고있도록)
                if (ScaleKeyFrames[iCurIdx].fTime <= _fTime)
                {
                    vNextScale.x = GAMEINSTANCE->PickRandomFloat(ScaleKeyFrames[iCurIdx + 1].vScaleMin.x, ScaleKeyFrames[iCurIdx + 1].vScaleMax.x, true);
                    vNextScale.y = GAMEINSTANCE->PickRandomFloat(ScaleKeyFrames[iCurIdx + 1].vScaleMin.y, ScaleKeyFrames[iCurIdx + 1].vScaleMax.y, true);
                    vNextScale.z = GAMEINSTANCE->PickRandomFloat(ScaleKeyFrames[iCurIdx + 1].vScaleMin.z, ScaleKeyFrames[iCurIdx + 1].vScaleMax.z, true);

                    return vNextScale;
                }

                // 인덱스 아직 안넘어갔을 때 (들고 있던 Next Scale를 사용하도록)
                return _float3(0.f, -101.f, 0.f);
            }

            // 현재 인덱스가 마지막 키프레임일 때
            else
            {
                return _float3(-101.f, 0.f, 0.f);
            }
        }

        _float3 GetCurScale(_int iCurIdx, _float _fTime, _float3 vScale, _float3 vNextScale)
        {
            _float3 vCurScale;

            vCurScale.x = GAMEINSTANCE->Ease((EASE_TYPE)ScaleKeyFrames[iCurIdx - 1].eEaseType, _fTime, ScaleKeyFrames[iCurIdx].fTime, vScale.x, vNextScale.x);
            vCurScale.y = GAMEINSTANCE->Ease((EASE_TYPE)ScaleKeyFrames[iCurIdx - 1].eEaseType, _fTime, ScaleKeyFrames[iCurIdx].fTime, vScale.y, vNextScale.y);
            vCurScale.z = GAMEINSTANCE->Ease((EASE_TYPE)ScaleKeyFrames[iCurIdx - 1].eEaseType, _fTime, ScaleKeyFrames[iCurIdx].fTime, vScale.z, vNextScale.z);

            return vCurScale;
        }

        _float3		GetCurMinPos(_float _fTime)
        {
            _float3 vCurPos = {};

            for (auto iter = PosKeyFrames.begin(); iter != PosKeyFrames.end(); ++iter)
            {
                // 다음 요소에 접근 가능
                if (iter + 1 != PosKeyFrames.end())
                {
                    auto iterNext = iter + 1;

                    if (iterNext->fTime <= _fTime)
                        continue;

                    vCurPos.x = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime, iterNext->fTime, iter->vPosMin.x, iterNext->vPosMin.x);
                    vCurPos.y = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime, iterNext->fTime, iter->vPosMin.y, iterNext->vPosMin.y);
                    vCurPos.z = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime, iterNext->fTime, iter->vPosMin.z, iterNext->vPosMin.z);

                    return vCurPos;
                }
            }

            auto iter_end = PosKeyFrames.end();
            return (--iter_end)->vPosMin;
        }

        _float3		GetCurMaxPos(_float _fTime)
        {
            _float3 vCurPos = {};

            for (auto iter = PosKeyFrames.begin(); iter != PosKeyFrames.end(); ++iter)
            {
                // 다음 요소에 접근 가능
                if (iter + 1 != PosKeyFrames.end())
                {
                    auto iterNext = iter + 1;

                    if (iterNext->fTime <= _fTime)
                        continue;

                    vCurPos.x = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime, iterNext->fTime, iter->vPosMax.x, iterNext->vPosMax.x);
                    vCurPos.y = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime, iterNext->fTime, iter->vPosMax.y, iterNext->vPosMax.y);
                    vCurPos.z = GAMEINSTANCE->Ease((EASE_TYPE)iter->eEaseType, _fTime, iterNext->fTime, iter->vPosMax.z, iterNext->vPosMax.z);

                    return vCurPos;
                }
            }

            auto iter_end = PosKeyFrames.end();
            return (--iter_end)->vPosMax;
        }

        _float3 GetNextSpeed(_int iCurIdx, _float _fTime)
        {
            _float3 vNextSpeed = {};

            // 현재 인덱스가 마지막 키프레임이 아닐 때
            if (SpeedKeyFrames.size() > iCurIdx + 1)
            {
                // 인덱스가 처음 넘어갔을 때 (Next Speed를 랜덤으로 추출해서 들고있도록)
                if (SpeedKeyFrames[iCurIdx].fTime <= _fTime)
                {
                    vNextSpeed.x = GAMEINSTANCE->PickRandomFloat(SpeedKeyFrames[iCurIdx + 1].vSpeedMin.x, SpeedKeyFrames[iCurIdx + 1].vSpeedMax.x, true);
                    vNextSpeed.y = GAMEINSTANCE->PickRandomFloat(SpeedKeyFrames[iCurIdx + 1].vSpeedMin.y, SpeedKeyFrames[iCurIdx + 1].vSpeedMax.y, true);
                    vNextSpeed.z = GAMEINSTANCE->PickRandomFloat(SpeedKeyFrames[iCurIdx + 1].vSpeedMin.z, SpeedKeyFrames[iCurIdx + 1].vSpeedMax.z, true);

                    return vNextSpeed;
                }

                // 인덱스 아직 안넘어갔을 때 (들고 있던 Next Speed를 사용하도록)
                return _float3(0.f, -101.f, 0.f);
            }

            // 현재 인덱스가 마지막 키프레임일 때
            else
            {
                return _float3(-101.f, 0.f, 0.f);
            }
        }

        _float3 GetCurSpeed(_int iCurIdx, _float _fTime, _float3 vSpeed, _float3 vNextSpeed)
        {
            _float3 vCurSpeed;

            vCurSpeed.x = GAMEINSTANCE->Ease((EASE_TYPE)SpeedKeyFrames[iCurIdx - 1].eEaseType, _fTime, SpeedKeyFrames[iCurIdx].fTime, vSpeed.x, vNextSpeed.x);
            vCurSpeed.y = GAMEINSTANCE->Ease((EASE_TYPE)SpeedKeyFrames[iCurIdx - 1].eEaseType, _fTime, SpeedKeyFrames[iCurIdx].fTime, vSpeed.y, vNextSpeed.y);
            vCurSpeed.z = GAMEINSTANCE->Ease((EASE_TYPE)SpeedKeyFrames[iCurIdx - 1].eEaseType, _fTime, SpeedKeyFrames[iCurIdx].fTime, vSpeed.z, vNextSpeed.z);

            return vCurSpeed;
        }
    };

    struct PARTICLE_SYSTEM : public EffectDesc
    {
        // Main Module
        _int        iMaxParticles = { MAX_PARTICLE };
        _float2     vLifeTime = { 3.f, 3.f };
        _int        iSpawnType = { 0 }; // 0 : Continuous, 1 : Burst, 2 : Random, 3 : OnCol
        _float      fSpawnNumPerSec = 10.f;
        _int        iRenderType = { 0 }; // 0 : None, 1 : BillBoard, 2 : Directional
        _int        iBillBoardType = { 0 }; // 0 : 기본, 1 : 빌보드, 2 : 방향성

        // Position
        _int        iPosType = 0; // 0 : World, 1 : Object, 2 : Bone
        _bool       bPosFollow = false;
        string      strPosFollowBoneKey = "";

        _bool       bUsePosKeyFrame = false;
        _float3     vPosMin = { 0.f, 0.f, 0.f };
        _float3     vPosMax = { 0.f, 0.f, 0.f };

        // Haze
        _bool       bUseHazeEffect = false;
        _float3     vHazeSpeedMin = { -1.f, -1.f, -1.f };
        _float3     vHazeSpeedMax = { 1.f, 1.f, 1.f };

        // Pivot
        _bool       bUsePivot = false;
        _float3     vPivotMin = { 0.f, 0.f, 0.f };
        _float3     vPivotMax = { 0.f, 0.f, 0.f };

        // Shape Module
        _int        iShape = { 0 };     // 0 : Sphere, 1 : Box, 2 : Cone, 3 : Mesh

        // Speed
        _bool       bSpeedMode = false; // 0 : Time Mode, 1 : Acc Mode
        _bool       bRelativeOwner = false;

        // Time Mode -> KeyFrame
        // Acc Mode
        _float3     vStartSpeedMin = { 0.0f, 0.1f, 0.0f };
        _float3     vStartSpeedMax = { 0.0f, 0.1f, 0.0f };
        _float3     vSpeedAccMin = { 5.f, 5.f, 5.f };
        _float3     vSpeedAccMax = { 5.f, 5.f, 5.f };

        // Rotation
        _float3     vRotationMin = { 0.f, 0.f, 0.f };
        _float3     vRotationMax = { 0.f, 0.f, 0.f };
        _float3     vTurnVelMin = { 0.f, 0.f, 0.f };
        _float3     vTurnVelMax = { 0.f, 0.f, 0.f };
        _float3     vTurnAccMin = { 0.f, 0.f, 0.f };
        _float3     vTurnAccMax = { 0.f, 0.f, 0.f };

        // Color
        _bool       bUseColor = false;

        // GlowColor
        _bool       bUseGlowColor = false;
        _float4     vGlowColor = { 1.f, 1.f ,1.f ,1.f };

        // Scale
        // Speed (Speed Mode가 0 : Time Mode일 때만 사용)
        KEYFRAMES_PARTICLE KeyFrames;
    };

public:
    CParticleSystem();
    CParticleSystem(const CParticleSystem& rhs);
    ~CParticleSystem();

public:
    virtual HRESULT Initialize(PARTICLE_SYSTEM* pDesc);
    virtual void	PriorityTick(_float _fTimeDelta);
    virtual void	Tick(_float _fTimeDelta);
    virtual void	LateTick(_float _fTimeDelta);
    virtual HRESULT Render();
    virtual HRESULT RenderGlow() override;

private:
    HRESULT         Initialize_Desc(PARTICLE_SYSTEM* pDesc);

    void            SpawnParticles(_float _fTimeDelta);
    void            UpdateParticles(_float _fTimeDelta);
    void            CheckDeadParticles(_float _fTimeDelta);

    void            CreateParticle(_int _iIndex);

public:
    virtual HRESULT SetOwner(shared_ptr<CGameObject> _pOwner) override;
    virtual void    Reset() override;

    PARTICLE_SYSTEM* GetParticleDesc() { return &m_eParticleSystem; }

private:
    shared_ptr<CShader>             m_pShaderCom = { nullptr };
    shared_ptr<CVIInstanceParticle> m_pVIBufferCom = { nullptr };

private:
    PARTICLE_SYSTEM m_eParticleSystem;
    PARTICLE* m_pParticles = nullptr;

    _int            m_iCurrentParticleCnt = 0;
    _float          m_fSpawnTime = 0.f;

    _float4x4* m_pFollowBoneMat = nullptr;

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

    _float4x4       m_OwnerWorld;
    _bool           m_bEntered = true;

    _float3         m_vCurPos;
    _float4x4       m_vCurPosMatrix;

    _float3         m_vCurSpeed;
    _float          m_fCurSpeedScale;
    _float4x4       m_vCurSpeedMatrix;

    _float3         m_vCurDir;

    _float3         m_vCurPivot;
    _float4x4       m_vCurPivotMatrix;

    EFFECT_TYPE     m_eEffectType = EFFECT_END;

public:
    static shared_ptr<CParticleSystem> Create(PARTICLE_SYSTEM* pDesc);
};

END