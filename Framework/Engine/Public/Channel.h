#pragma once

#include "Engine_Defines.h"

#include "Model.h"
#include "Bone.h"

BEGIN(Engine)

class CChannel
{

public:
	struct KEYFRAME
	{
		_float3 vScale, vPosition;
		_float4 vRotation;
		_double Time;
	};

public:
	CChannel();
	~CChannel() = default;

public:
	HRESULT Initialize(ifstream& _ifs, shared_ptr<Engine::CModel> _pModel);
	void InvalidateTransformationMatrix(_double _TrackPosition, _uint* _pCurrentKeyFrame, const vector<shared_ptr<Engine::CBone>>& Bones);
	void IdentityMatrix(const vector<shared_ptr<Engine::CBone>>& Bones);

	// 변경된 애니메이션과의 선형보간을 계산하고 Bone에 정보를 전달합니다.
	void BlendAnimation(_double _Ratio, KEYFRAME Frame, const vector<shared_ptr<Engine::CBone>>& Bones); 

	// BoneIndex를 Model의 현재 BoneIndex기준으로 업데이트 합니다.
	void UpdateBoneIndex(shared_ptr<CModel> _pModel);

public:
	_uint GetBoneIndex() { return m_iBoneIndex; }

public:
	KEYFRAME GetKeyFrame(_uint _iCurrentFrame) {
		return m_KeyFrames[_iCurrentFrame];
	}

	_uint GetKeyFramesNum() { return m_iNumKeyFrames; }


private:
	_char m_szName[MAX_PATH] = "";
	_uint m_iNumKeyFrames = { 0 };
	vector<KEYFRAME> m_KeyFrames;

	_uint m_iBoneIndex = { 0 };

public:
	static shared_ptr<CChannel> Create(ifstream& _ifs, shared_ptr<CModel> _pModel);


};

END