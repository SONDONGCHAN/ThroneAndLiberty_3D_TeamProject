#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

#include "Effect.h"

BEGIN(Engine)
class CModel;
class CAnimation;
END

enum EFFECT_ATTACH_TYPE {
    EFFECT_ATTACH_ANIM,		// 애님 객체에 붙어서 트랙포지션을 받아서 재생되는 타입
    EFFECT_ATTACH_NONANIM,	// 논애님 객체에 붙어서 스스로 재생되는 타입
    EFFECT_ATTACH_FREE,		// 아무 객체에도 붙지 않는 타입
    EFFECT_ATTACH_END
};

BEGIN(Tool_Effect)

class CEffectGroup : public CGameObject
{
public:
    // 파싱용
    struct EffectGroupInfoDesc
    {
        string      strGroupID = "";

        string      strModelKey = "";
        string      strAnimKey = "";

        _double     StartTrackPos = 0.0;
    };

    struct EffectGroupDesc
    {
        string      strGroupID = "";
        _int        iEffectCnt = 0;
    };

public:
    CEffectGroup();
    CEffectGroup(const CEffectGroup& rhs);
    virtual ~CEffectGroup();

public:
    virtual HRESULT Initialize(EFFECT_ATTACH_TYPE _eEffectAttacjType);
    virtual void	PriorityTick(_float _fTimeDelta);
    virtual void	Tick(_float _fTimeDelta);
    virtual void	LateTick(_float _fTimeDelta);
    virtual HRESULT Render();

public:
    void            SetPlaying(_bool _bPlaying) { m_bPlaying = _bPlaying; }
    void            SetSpeed(_float _fSpeed) { m_fSpeed = _fSpeed; }

    void            AddEffect(string _strEffectID, shared_ptr<CEffect> _pEffect);
    void            DeleteEffect(string _strEffectID);

    void            StopEffect();       // 완전 종료(reset)시키고, render까지 끔
    void            PauseEffect();      // 일시정지 시키고, render는 켬

    HRESULT         Play(shared_ptr<CGameObject> _pOwner);
    void            Reset();

    HRESULT         SetOwner(shared_ptr<CGameObject> _pOwner);
    HRESULT         SetModelKey(string _strModelKey);
    HRESULT         SetAnimKey(string _strAnimKey);

    EffectGroupInfoDesc         GetGroupInfo();
    void            SetGroupInfo(string _strGroupID, string _strModelKey, string _strAnimKey);
    vector<shared_ptr<CEffect>> GetEffects() { return m_pEffects; }
    void            SetAttachType(EFFECT_ATTACH_TYPE _eAttachType) { m_eEffectAttachType = _eAttachType; }

protected:
    weak_ptr<CModel>		m_pOwnerModelCom;
    weak_ptr<CAnimation>    m_pOwnerAnimCom;
    weak_ptr<CGameObject>   m_pOwner;

private:
    // 저장할 애들
    string                  m_strGroupID = "";
    
    _bool                   m_bWithModel = true;
    _bool                   m_bWithAnim = true;

    string                  m_strModelKey = "";
    string                  m_strAnimKey = "";

    _double                 m_StartTrackPos = 0.0;

    //====================================================//

    // 저장할 애들 2
    _int                            m_iEffectCnt;

    vector<string>                  m_strEffectID;
    vector<shared_ptr<CEffect>>     m_pEffects;

    //====================================================//
     
    // 시간 관련 변수
    _float                  m_fSpeedTime = 0.f; // 속도 * 델타타임
    _float                  m_fElapsedTime = 0.f;
    _bool                   m_bPlaying = true;

    _float                  m_fSpeed = 1.f;

    _bool                   m_bAllFinish = false;

    vector<CEffect::EFFECT_STATE>   m_eEffectStates;

    EFFECT_ATTACH_TYPE      m_eEffectAttachType = EFFECT_ATTACH_END;

protected:
    ComPtr<ID3D11Device>				m_pDevice = nullptr;
    ComPtr<ID3D11DeviceContext>			m_pContext = nullptr;

public:
    static shared_ptr<CEffectGroup> Create(EFFECT_ATTACH_TYPE _eEffectAttacjType);
};

END