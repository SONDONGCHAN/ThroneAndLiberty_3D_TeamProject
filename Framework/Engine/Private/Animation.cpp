#include "Animation.h"
#include "Channel.h"
#include "GameObject.h"

#include <fstream>

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const shared_ptr<CAnimation> _pOrigin)
{
}

HRESULT CAnimation::Initialize(ifstream& _ifs, shared_ptr<CModel> _pModel, _float _fSpeed)
{
    _ifs.read(m_szName, sizeof(char) * MAX_PATH);
    _ifs.read((char*)&m_Duration, sizeof(_double));
    _ifs.read((char*)&m_TickPerSecond, sizeof(_double));
    _ifs.read((char*)&m_iNumChannels, sizeof(_uint));

    /*Json에 파싱해둔 Anim Speed 정보*/
    m_fSpeed = _fSpeed;

    m_iCurrentKeyFrames.resize(m_iNumChannels);

    for (_uint i = 0; i < m_iNumChannels; ++i) {

        shared_ptr<CChannel> pChannel = CChannel::Create(_ifs, _pModel);

        if (!pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }

    //// m_iBoneIndex의 크기 순서로 m_Channels을 정렬합니다.
    //sort(m_Channels.begin(), m_Channels.end(), [](const std::shared_ptr<CChannel>& a, const std::shared_ptr<CChannel>& b) 
    //{
    //    return a->GetBoneIndex() < b->GetBoneIndex();
    //});


    return S_OK;
}


_bool CAnimation::InvalidateTransformationMatrix(_float _fTimeDelta, const vector<shared_ptr<CBone>>& _Bones, _bool _isLoop)
{
    m_bCanCancel = false;

    if (!m_isChanging)
    {
        if (m_TrackPosition == 0)
        {
            m_TriggerCursor = 0;
            m_SoundCursor = 0;

            string strAnimKey = m_szName;
            size_t pos = strAnimKey.find("Armature|");
            if (pos != std::string::npos) {
                strAnimKey.erase(pos, string("Armature|").length());
            }

            if (m_pOwnerObject.lock())
                m_pOwnerObject.lock()->PlayEffect(strAnimKey.c_str());
        }
        m_TrackPosition += m_TickPerSecond * (double)_fTimeDelta * m_fSpeed;

        m_fAnimRatio = (_float)( m_TrackPosition / m_Duration);
        
        if ( m_fAnimRatio >= m_fAnimCancleRatio )
            m_bCanCancel = true;
        
        // 투사체
        if ( ( (-1) != m_iMyAnimIndex ) && ( (-1) != m_TriggerCursor ) )
        {
            if (m_ProjectileTriggers[m_TriggerCursor] <= m_TrackPosition)
            {
                m_pOwnerObject.lock()->ShootProjectile(m_ProjectileTriggerOwners[m_TriggerCursor]);
                m_TriggerCursor++;
                
                if (m_TriggerCursor == m_ProjectileTriggers.size())
                    m_TriggerCursor = -1;
            }
        }

        // 콜라이더
        if (m_isColAnim)
        {
            if ((m_fColStartTrackPos <= m_TrackPosition) && (m_TrackPosition < m_fColEndTrackPos) && (!m_isColOn))
            {
                m_pOwnerObject.lock()->EnableCollision(m_szName);
                m_isColOn = true;
            }
            else if ((m_fColEndTrackPos <= m_TrackPosition) && m_isColOn)
            {
                m_pOwnerObject.lock()->DisableCollision(m_szName);
                m_isColOn = false;
            }
        }

        // 사운드
        if (m_isSoundAnim && ((-1) != m_SoundCursor) )
        {
            if (m_SoundTrackPoses[m_SoundCursor] <= m_TrackPosition)
            {
                GAMEINSTANCE->PlaySoundW(m_SoundNames[m_SoundCursor], m_SoundVolumes[m_SoundCursor]);
                m_SoundCursor++;
            
                if (m_SoundCursor == m_SoundTrackPoses.size())
                    m_SoundCursor = -1;
            }
        }

        if (m_TrackPosition >= m_Duration)
        {
            if (!_isLoop)
            {
                m_isFinished = true;
                return m_isFinished;
            }
            else
            {
                m_TrackPosition = 0.0;
            }
        }

        for (size_t i = 0; i < m_iNumChannels; i++)
            m_Channels[i]->InvalidateTransformationMatrix(m_TrackPosition, &m_iCurrentKeyFrames[i], _Bones);
    }
    else
    {
        m_BlendingPosition += (double)_fTimeDelta;

        _double Ratio = m_BlendingPosition / m_fChangingDelay;

        if (1.0 <= Ratio)
        {
            m_isChanging = false;
            Ratio = 1.0;
            m_BlendingPosition = 0.0;
        }

        for (auto pChannel : m_Channels)
        {
            _uint iIndex = pChannel->GetBoneIndex();
            
            auto iter = m_BlendKeyFrame.find(iIndex);

            if (iter != m_BlendKeyFrame.end())
            {
                pChannel->BlendAnimation(Ratio, iter->second, _Bones);
            }
        }
    }

    return false;
}

_bool CAnimation::SeparateTransformationMatrix(_float _fTimeDelta, const vector<shared_ptr<CBone>>& _Bones, _bool _isLoop, _bool _isUpper, _uint _iNumBaseBone, _uint _iNumUpperBone)
{
    m_bCanCancel = false;

    if (!m_isChanging)
    {
        if (m_TrackPosition == 0)
        {
            m_TriggerCursor = 0;
            m_SoundCursor = 0;

            string strAnimKey = m_szName;
            size_t pos = strAnimKey.find("Armature|");
            if (pos != std::string::npos) {
                strAnimKey.erase(pos, string("Armature|").length());
            }

            if (m_pOwnerObject.lock())
                m_pOwnerObject.lock()->PlayEffect(strAnimKey.c_str());
        }

        m_TrackPosition += m_TickPerSecond * (double)_fTimeDelta * m_fSpeed;

        m_fAnimRatio = (_float)(m_TrackPosition / m_Duration);

        if (m_fAnimRatio >= m_fAnimCancleRatio)
            m_bCanCancel = true;


        // 투사체
        if (((-1) != m_iMyAnimIndex) && ((-1) != m_TriggerCursor))
        {
            if (m_ProjectileTriggers[m_TriggerCursor] <= m_TrackPosition)
            {
                m_pOwnerObject.lock()->ShootProjectile(m_ProjectileTriggerOwners[m_TriggerCursor]);
                m_TriggerCursor++;

                if (m_TriggerCursor == m_ProjectileTriggers.size())
                    m_TriggerCursor = -1;
            }
        }

        // 콜라이더
        if (m_isColAnim)
        {
            if ((m_fColStartTrackPos <= m_TrackPosition) && (!m_isColOn))
            {
                m_pOwnerObject.lock()->EnableCollision(m_szName);
                m_isColOn = true;
            }
            else if ((m_fColEndTrackPos <= m_TrackPosition) && m_isColOn)
            {
                m_pOwnerObject.lock()->DisableCollision(m_szName);
                m_isColOn = false;
            }
        }

        // 사운드
        if (m_isSoundAnim && ((-1) != m_SoundCursor))
        {
            if (m_SoundTrackPoses[m_SoundCursor] <= m_TrackPosition)
            {
                GAMEINSTANCE->PlaySoundW(m_SoundNames[m_SoundCursor], m_SoundVolumes[m_SoundCursor]);
                m_SoundCursor++;

                if (m_SoundCursor == m_SoundTrackPoses.size())
                    m_SoundCursor = -1;
            }
        }

        if (m_TrackPosition >= m_Duration) 
        {
            if (!_isLoop)
            {
                m_isFinished = true;
                return m_isFinished;
            }
            else
            {
                m_TrackPosition = 0.0;
            }
        }

        for (size_t i = 0; i < m_iNumChannels; i++)
        {
            if (_isUpper)
            {
                if (_iNumBaseBone <= m_Channels[i]->GetBoneIndex() && m_Channels[i]->GetBoneIndex() < _iNumUpperBone)
                {
                    m_Channels[i]->InvalidateTransformationMatrix(m_TrackPosition, &m_iCurrentKeyFrames[i], _Bones);
                }
            }
            else
            {       
                //if (m_Channels[i]->GetBoneIndex() < _iNumBaseBone )
                //{
                //     m_Channels[i]->IdentityMatrix(_Bones);
                //}
                //else if (_iNumUpperBone <= m_Channels[i]->GetBoneIndex())
                //{
                //    m_Channels[i]->InvalidateTransformationMatrix(m_TrackPosition, &m_iCurrentKeyFrames[i], _Bones);
                //}

                if (m_Channels[i]->GetBoneIndex() < _iNumBaseBone || _iNumUpperBone <= m_Channels[i]->GetBoneIndex())
                {
                    m_Channels[i]->InvalidateTransformationMatrix(m_TrackPosition, &m_iCurrentKeyFrames[i], _Bones);
                }
            }

        }
    }
    else
    {
        m_BlendingPosition += (double)_fTimeDelta;

        _double Ratio = m_BlendingPosition / m_fChangingDelay;

        if (1.0 <= Ratio)
        {
            m_isChanging = false;
            Ratio = 1.0;
            m_BlendingPosition = 0.0;
        }

        for (auto pChannel : m_Channels)
        {
            _uint iIndex = pChannel->GetBoneIndex();

            auto iter = m_BlendKeyFrame.find(iIndex);

            if (iter != m_BlendKeyFrame.end())
            {
                pChannel->BlendAnimation(Ratio, iter->second, _Bones);
            }
        }
    }

    return false;
}


void CAnimation:: SetBlendingInfo(_float _fChangingDelay, vector<shared_ptr<class CBone>> _Bones, _bool _isUpper, _uint _iNumBaseBone, _uint _iNumUpperBone)
{
    m_fChangingDelay = _fChangingDelay;
    m_isChanging = true;
    m_BlendKeyFrame.clear();
    m_TrackPosition = 0.f;
    m_BlendingPosition = 0.f;
    for (auto pChannel : m_Channels)
    {   
        _uint iBoneIndex = 0;

        for (auto pBone : _Bones)
        {
            if (_isUpper)
            {
                if (iBoneIndex == pChannel->GetBoneIndex())
                {
                    _float4x4 matTransformation = pBone->GetTransformationMatrix();
                    CChannel::KEYFRAME KeyFrame{};
                    XMVECTOR vScale, vRotation, vPosition;
                    XMMatrixDecompose(&vScale, &vRotation, &vPosition, matTransformation);
                    KeyFrame.vScale = vScale;
                    KeyFrame.vRotation = vRotation;
                    KeyFrame.vPosition = vPosition;
                    m_BlendKeyFrame.emplace(iBoneIndex, KeyFrame);

                    break;
                }
            }
            else
            {
                if (pChannel->GetBoneIndex() < _iNumBaseBone || _iNumUpperBone <= pChannel->GetBoneIndex())
                {
                    if (iBoneIndex == pChannel->GetBoneIndex())
                    {
                        _float4x4 matTransformation = pBone->GetTransformationMatrix();
                        CChannel::KEYFRAME KeyFrame{};
                        XMVECTOR vScale, vRotation, vPosition;
                        XMMatrixDecompose(&vScale, &vRotation, &vPosition, matTransformation);
                        KeyFrame.vScale = vScale;
                        KeyFrame.vRotation = vRotation;
                        KeyFrame.vPosition = vPosition;
                        m_BlendKeyFrame.emplace(iBoneIndex, KeyFrame);

                        break;
                    }
                }
            }
            ++iBoneIndex;
        }
    }
}

void CAnimation::UpdateChannelsBoneIndex(shared_ptr<CModel> _pModel)
{
    for (auto pChannel : m_Channels)
        pChannel->UpdateBoneIndex(_pModel);
}

void CAnimation::DisableCollision()
{
    if ((!m_isColAnim) || (!m_isColOn) )
        return;

    m_pOwnerObject.lock()->DisableCollision(m_szName);  
    m_isColOn = false;
}

void CAnimation::AddProjectileTrigger(_float _fProjectileTriggerTrackPos, _int eProjectileTriggerOwners)
{
    m_ProjectileTriggers.push_back(_fProjectileTriggerTrackPos);
    m_ProjectileTriggerOwners.push_back(eProjectileTriggerOwners);
}

void CAnimation::AddSound(string _strSoundName, _float _fSoundTrackPos, _float _fVolume)
{
    m_SoundNames.push_back(_strSoundName);
    m_SoundTrackPoses.push_back(_fSoundTrackPos);
    m_SoundVolumes.push_back(_fVolume);

}

void CAnimation::ResetSound()
{
    m_isSoundAnim = false;
    m_SoundNames.clear();
    m_SoundTrackPoses.clear();	
    m_SoundVolumes.clear();
}

shared_ptr<CAnimation> CAnimation::Create(ifstream& _ifs, shared_ptr<CModel> _pModel, _float _fSpeed)
{
    shared_ptr<CAnimation> pInstance = make_shared<CAnimation>();

    if (FAILED(pInstance->Initialize(_ifs, _pModel, _fSpeed)))
        MSG_BOX("Faile to Create : CAnimation");

    return pInstance;
}

shared_ptr<CAnimation> CAnimation::Clone()
{
    shared_ptr<CAnimation> pAnim = make_shared<CAnimation>(*(shared_from_this().get()));
    return pAnim;
}
