#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "Easing.h"

BEGIN(Engine)
class CModel;
class CTexture;
END

BEGIN(Client)

class CEffect : public CGameObject
{
public:
    enum TEX_TYPE { TEX_DIFFUSE, TEX_MASK, TEX_NOISE, TEX_END };

    enum EFFECT_TYPE
    {
        EFFECT_MESH,
        //EFFECT_TEXTURE,
        EFFECT_PARTICLE,
        EFFECT_END
    };

    enum EFFECT_STATE
    {
        EFFECT_STATE_IDLE,
        EFFECT_STATE_START,
        EFFECT_STATE_FINISH,
        EFFECT_STATE_PAUSE,
        EFFECT_STATE_END
    };

public:
    // 파싱용
    struct EffectDesc
    {
        _int        iEffectType = -1;

        _int        iShaderType = 4;    // 4 : ST_EFFECTMESH
        _int        iShaderPass = 0;
        _int        iRenderGroup = 2;   // 2 : RENDER_NONLIGHT

        _float      fDuration = 5.f;
        double      StartTrackPos = 0.f;
        _bool       bLoop = 0;

        _bool       bUseDiffuse = 0;
        string      strTexDiffuseKey = "";

        // 셰이더 가중치?
    };

public:
    CEffect();
    CEffect(const CEffect& rhs);
    virtual ~CEffect();

public:
    virtual HRESULT Initialize(EffectDesc* pDesc);
    virtual void	PriorityTick(_float _fTimeDelta);
    virtual void	Tick(_float _fTimeDelta);
    virtual void	LateTick(_float _fTimeDelta);
    virtual HRESULT Render();

public:
    // Set
    void            SetPlaying(_bool _bPlaying) { m_bPlaying = _bPlaying; }
    void            SetSpeed(_float _fSpeed) { m_fSpeed = _fSpeed; }

    // Get
    EffectDesc      GetEffectDesc() { return m_eEffectDesc; }

public:
    virtual HRESULT SetOwner(shared_ptr<CGameObject> _pOwner);
    EFFECT_STATE    CheckState(_double _TrackPos);
    virtual void    Reset();

protected:
    shared_ptr<CModel>		m_pModelCom = nullptr;
    weak_ptr<CGameObject>   m_pOwner;

    string                  m_strTextureKey[TEX_END];

    _int                    m_iRenderGroup = -1;

protected:
    // 시간 관련 변수
    _float                  m_fSpeed = 1.f;
    _float                  m_fSpeedTime = 0.f;

    _float                  m_fActiveTime = 0.f;
    _float                  m_fLifeTime = 0.f;
    _float                  m_fTimeDelta = 0.f;
    _int                    m_iCurKeyFrameIdx = 0;

    _bool                   m_bPlaying = true;      // 일시정지용
    _bool                   m_bFinished = false;    // 끝났는지 체크용
    _bool                   m_bStart = false;       // 시작 트리거 받으면 true

    EFFECT_STATE            m_ePrevState = EFFECT_STATE_IDLE;
    EFFECT_STATE            m_eState = EFFECT_STATE_IDLE;

    _float	                m_fFrameTime = 0.f;

    EffectDesc              m_eEffectDesc;

protected:
    ComPtr<ID3D11Device>				m_pDevice = nullptr;
    ComPtr<ID3D11DeviceContext>			m_pContext = nullptr;

public:
    static shared_ptr<CEffect> Create(EffectDesc* pDesc);
};

END