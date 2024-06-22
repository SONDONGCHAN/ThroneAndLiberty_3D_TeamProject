#include "Channel.h"

#include <fstream>

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(ifstream& _ifs, shared_ptr<Engine::CModel> _pModel)
{
	_ifs.read(m_szName, sizeof(char) * MAX_PATH);
	m_iBoneIndex = _pModel->GetBoneIndex(m_szName);

	_ifs.read((char*)&m_iNumKeyFrames, sizeof(_uint));


	for (_uint i = 0; i < m_iNumKeyFrames; i++) {

		KEYFRAME KeyFrame = {};

		_ifs.read((char*)&KeyFrame, sizeof(KEYFRAME));
		m_KeyFrames.push_back(KeyFrame);

	}

	return S_OK;
}

void CChannel::InvalidateTransformationMatrix(_double _TrackPosition, _uint* _pCurrentKeyFrame, const vector<shared_ptr<Engine::CBone>>& Bones)
{
	if (0.0 == _TrackPosition)
		*_pCurrentKeyFrame = 0;

	KEYFRAME LastKeyFrame = m_KeyFrames.back();
	_float4x4 TransformationMatrix;

	_float3 vScale;
	_float4 vRotation, vTranslation;

	//마지막 프레임보다 큰 경우
	if (_TrackPosition >= LastKeyFrame.Time) {

		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);

	}
	else
	{
		if (_TrackPosition < m_KeyFrames[*_pCurrentKeyFrame].Time)
		{
			while (_TrackPosition < m_KeyFrames[*_pCurrentKeyFrame].Time)
				--*_pCurrentKeyFrame;
		}
		else
		{
			while (_TrackPosition >= m_KeyFrames[*_pCurrentKeyFrame + 1].Time)
				++*_pCurrentKeyFrame;
		}

		_double Ratio = (_TrackPosition - m_KeyFrames[*_pCurrentKeyFrame].Time) /
			(m_KeyFrames[*_pCurrentKeyFrame + 1].Time - m_KeyFrames[*_pCurrentKeyFrame].Time);


		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*_pCurrentKeyFrame].vScale), XMLoadFloat3(&m_KeyFrames[*_pCurrentKeyFrame + 1].vScale), (_float)Ratio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*_pCurrentKeyFrame].vPosition), XMLoadFloat3(&m_KeyFrames[*_pCurrentKeyFrame + 1].vPosition), (_float)Ratio), 1.f);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*_pCurrentKeyFrame].vRotation), XMLoadFloat4(&m_KeyFrames[*_pCurrentKeyFrame + 1].vRotation), (_float)Ratio);

	}

	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);
	Bones[m_iBoneIndex]->SetTranslationMatrix(TransformationMatrix);

}

void CChannel::IdentityMatrix(const vector<shared_ptr<Engine::CBone>>& Bones)
{
	Bones[m_iBoneIndex]->SetTranslationMatrix(XMMatrixIdentity());
}

void CChannel::BlendAnimation(_double _Ratio, KEYFRAME Frame, const vector<shared_ptr<Engine::CBone>>& Bones)
{
	_float4x4	TransformationMatrix;

	_float4		vScale, vRotation, vPosition;

	//서로다른 애니매이션 사이를 선형 보간
	vScale = XMVectorLerp(Frame.vScale, m_KeyFrames[0].vScale, (_float)_Ratio);
	vRotation = XMQuaternionSlerp(Frame.vRotation, m_KeyFrames[0].vRotation, (_float)_Ratio);
	vPosition = XMVectorSetW(XMVectorLerp(Frame.vPosition, m_KeyFrames[0].vPosition, (_float)_Ratio), 1.f);

	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Bones[m_iBoneIndex]->SetTranslationMatrix(TransformationMatrix);
}

void CChannel::UpdateBoneIndex(shared_ptr<CModel> _pModel)
{
	m_iBoneIndex = _pModel->GetBoneIndex(m_szName);

	_int a = 0;
}


shared_ptr<CChannel> CChannel::Create(ifstream& _ifs, shared_ptr<CModel> _pModel)
{
	shared_ptr<CChannel> pInstance = make_shared<CChannel>();

	if (FAILED(pInstance->Initialize(_ifs, _pModel)))
		MSG_BOX("Failed to Create : Channel");

    return pInstance;
}
