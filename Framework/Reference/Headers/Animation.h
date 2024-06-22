#pragma once

#include "Engine_Defines.h"

#include "Bone.h"
#include "Model.h"
#include "Channel.h"

BEGIN(Engine)


class ENGINE_DLL CAnimation : public std::enable_shared_from_this<CAnimation>
{
public:
	CAnimation();
	CAnimation(const shared_ptr<CAnimation> _pOrigin);
	~CAnimation() = default;

public:
	HRESULT Initialize(ifstream& _ifs, shared_ptr<CModel> _pModel, _float _fSpeed);
	_bool InvalidateTransformationMatrix(_float _fTimeDelta, const vector<shared_ptr<CBone>>& _Bones, _bool _isLoop);
	_bool SeparateTransformationMatrix(_float _fTimeDelta, const vector<shared_ptr<CBone>>& _Bones, _bool _isLoop, _bool _isUpper, _uint _iNumBaseBone, _uint _iNumUpperBone);

public:
	// 애니메이션이 변경 될 때, 변경된 애니매이션과의 선형 보간을 위한 정보들을 준비합니다.
	void	SetBlendingInfo(_float _fChangingDelay , vector<shared_ptr<class CBone>> _Bones, _bool _isUpper = true , _uint _iNumBaseBone = 0, _uint _iNumUpperBone = 0);

	// 애니메이션이 가진 모든 채널들이 가진 BoneIndex를 Model의 현재 BoneIndex기준으로 업데이트 합니다.
	void	UpdateChannelsBoneIndex(shared_ptr<CModel> _pModel);

public:
	_double		GetDuration()		const { return m_Duration; }
	_double		GetTrackPosition()	const { return m_TrackPosition; }
	const char* GetName()		const { return m_szName; }
	_float		GetAnimRatio()		const { return m_fAnimRatio; }
	_bool		GetCanCancel()		const { return m_bCanCancel; }

	void		SetTrackPosition(_double _fTrackPosition) { m_TrackPosition = _fTrackPosition; }
	void		SetAnimCancleRatio(_float _fAnimCancleRatio) { m_fAnimCancleRatio = _fAnimCancleRatio; }

public:
	void		SetOwnerObject(shared_ptr<class CGameObject> _pOwnerObject) { m_pOwnerObject = _pOwnerObject; }
	void		SetColStartTrackPos(_float _fStartTrackPos) { m_fColStartTrackPos = _fStartTrackPos; m_isColAnim = true; }
	void		SetColEndTrackPos(_float _fEndTrackPos) { m_fColEndTrackPos = _fEndTrackPos; }

public:
	void		DisableCollision();	// 콜라이더 트리거 Disable
	_bool		GetIsColAnim() { return m_isColAnim; }

public:
	void		SetAnimIndex(_int _iAnimIndex) { m_iMyAnimIndex = _iAnimIndex; }		// My Anim Index
	void		AddProjectileTrigger(_float _fProjectileTriggerTrackPos, _int eProjectileTriggerOwners);	//

public:
	void		SetIsSoundAnim() { m_isSoundAnim = true; }		// 
	void		AddSound(string _strSoundName, _float _fSoundTrackPos, _float _fVolume);	//
	//For Anim Tool
	void		ResetSound();	//
	void		SetSoundCursor() { m_SoundCursor = -1; }	//

private:
	_float m_fSpeed = { 1.f };					// Json에서 읽은 애니메이션 재생 속도입니다.

private:
	_char m_szName[MAX_PATH] = "";
	_uint m_iNumAnimations = { 0 };

	_double m_Duration = { 0.0 };
	_double m_TickPerSecond = { 0.0 };
	_double m_TrackPosition = { 0.0 };
	_double m_BlendingPosition = { 0.0 };

	_uint m_iNumChannels = { 0 };
	vector<shared_ptr<class CChannel>> m_Channels = {};
	vector<_uint> m_iCurrentKeyFrames;

	_bool	m_isFinished	= { false };	// 애니메이션이 끝났는지
	_float	m_fAnimCancleRatio = { 0.f };	// 애니메이션 캔슬 가능한 비율
	_bool	m_bCanCancel	= { false };	// 애니메이션 캔슬 가능한 상태인지
	_float	m_fAnimRatio = { 0.f };			// 애니메이션 진행된 비율

	
	weak_ptr<class CGameObject> m_pOwnerObject; // 이 애니메이션의 주인

	//선형보간 관련 변수입니다.
	_float	m_fChangingDelay	= { 0.0f };				// 선형 보간이 진행될 시간.
	_bool	m_isChanging		= { false };			// 선형 보간이 진행 중인지.
	map<_uint, CChannel::KEYFRAME> m_BlendKeyFrame;		// 이전 애니매이션과 겹치는 뼈들의 현재 상태 KEYFRAME정보를 보관.

	//Collier 관련
	_float	m_fColStartTrackPos = {0.f};	// ColliderTrigger를 켜줄 TrackPos
	_float	m_fColEndTrackPos = {0.f};		// ColliderTrigger를 꺼줄 TrackPos
	_bool	m_isColOn = { false };			// ColliderTrigger의 현재 ON/OFF 상태
	_bool	m_isColAnim = { false };		// ColliderTrigger가 있는 애니메이션인지.

	//Projectile 관련
	_int			m_iMyAnimIndex = { -1 };	// 내 애니메이션 인덱스
	vector<_float>	m_ProjectileTriggers;		// Projectile발사 키프레임
	vector<_int>	m_ProjectileTriggerOwners;	// Projectile발사되는 위치
	_int			m_TriggerCursor = { -1 };

	//Sound 관련
	_bool			m_isSoundAnim = { false };	// 사운드가 있는 애니메이션인지
	vector<string>	m_SoundNames;				// 재생해줄 사운드의 이름
	vector<_float>	m_SoundTrackPoses;			// 사운드 재생 TrackPos
	vector<_float>	m_SoundVolumes;				// 사운드 볼륨
	_int			m_SoundCursor = { -1 };

public:
	static shared_ptr<CAnimation> Create(ifstream& _ifs, shared_ptr<CModel> _pModel, _float _fSpeed);
	shared_ptr<CAnimation> Clone();

};

END
