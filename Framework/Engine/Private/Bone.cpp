#include "Bone.h"

#include <fstream>

CBone::CBone()
{
}


HRESULT CBone::Initialize(char* _pName, _int _iParentBoneIndex, _uint _iOriginIndex, ifstream& _ifs)
{
	strcpy_s(m_szName, _pName);
	
	_float	a1{}, a2{}, a3{}, a4{},
			a5{}, a6{}, a7{}, a8{},
			a9{}, a10{}, a11{}, a12{},
			a13{}, a14{}, a15{}, a16{};


	_ifs.read((char*)&a1, sizeof(_float));
	_ifs.read((char*)&a2, sizeof(_float));
	_ifs.read((char*)&a3, sizeof(_float));
	_ifs.read((char*)&a4, sizeof(_float));
	_ifs.read((char*)&a5, sizeof(_float));
	_ifs.read((char*)&a6, sizeof(_float));
	_ifs.read((char*)&a7, sizeof(_float));
	_ifs.read((char*)&a8, sizeof(_float));
	_ifs.read((char*)&a9, sizeof(_float));
	_ifs.read((char*)&a10, sizeof(_float));
	_ifs.read((char*)&a11, sizeof(_float));
	_ifs.read((char*)&a12, sizeof(_float));
	_ifs.read((char*)&a13, sizeof(_float));
	_ifs.read((char*)&a14, sizeof(_float));
	_ifs.read((char*)&a15, sizeof(_float));
	_ifs.read((char*)&a16, sizeof(_float));



	m_TransformationMatrix._11 = a1;
	m_TransformationMatrix._12 = a2;
	m_TransformationMatrix._13 = a3;
	m_TransformationMatrix._14 = a4;
	m_TransformationMatrix._21 = a5;
	m_TransformationMatrix._22 = a6;
	m_TransformationMatrix._23 = a7;
	m_TransformationMatrix._24 = a8;
	m_TransformationMatrix._31 = a9;
	m_TransformationMatrix._32 = a10;
	m_TransformationMatrix._33 = a11;
	m_TransformationMatrix._34 = a12;
	m_TransformationMatrix._41 = a13;
	m_TransformationMatrix._42 = a14;
	m_TransformationMatrix._43 = a15;
	m_TransformationMatrix._44 = a16;


	m_CombinedTransformationMatrix = XMMatrixIdentity();
	m_iParentBoneIndex = _iParentBoneIndex;
	m_iOriginIndex = _iOriginIndex;

	return S_OK;
}

void CBone::InvalidateCombinedTransformationMatrix(const vector<shared_ptr<CBone>>& _Bones)
{
	if (-1 == m_iParentBoneIndex) {
		m_CombinedTransformationMatrix = m_TransformationMatrix;
	}
	else
	{
		if (m_eMyOption == OPTION_SUBTRACT_ROTATION_Y)
		{
			m_CombinedTransformationMatrix = m_TransformationMatrix * _Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix;

			_float3 vScaled = _float3(	m_CombinedTransformationMatrix.Right().Length(),
										m_CombinedTransformationMatrix.Up().Length(),
										m_CombinedTransformationMatrix.Forward().Length());

			_float4x4 matRotation = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(90.f))
									* SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(90.f))
									* SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(10.f));


			for (_uint i = 0; i < 3; i++)
			{
				_float3 vTemp = _float3(0.f, 0.f, 0.f);
				*((_float*)&vTemp + i) = 1.f * *((_float*)&vScaled + i);

				if (i == 0)
					m_CombinedTransformationMatrix.Right(vTemp.TransformNormal(vTemp, matRotation));
				else if (i == 1)
					m_CombinedTransformationMatrix.Up(vTemp.TransformNormal(vTemp, matRotation));
				else 
					m_CombinedTransformationMatrix.Forward(vTemp.TransformNormal(vTemp, matRotation));
			}
		}
		else
		{
			m_CombinedTransformationMatrix = m_TransformationMatrix * _Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix;
		}
	}
}

void CBone::InvalidateRefTransformationMatrix()
{
	if (nullptr == m_pRefTransformationMatrix)
		return;

	m_CombinedTransformationMatrix = *m_pRefTransformationMatrix;
}

shared_ptr<CBone> CBone::Create(char* _pName, _int _iParentBoneIndex, _uint _iOriginIndex,  ifstream& _ifs)
{
	shared_ptr<CBone> pInstance = make_shared<CBone>();

	if (FAILED(pInstance->Initialize(_pName, _iParentBoneIndex, _iOriginIndex, _ifs)))
		MSG_BOX("Failed to Create : CBone");

	return pInstance;
}

shared_ptr<CBone> CBone::Clone()
{
	shared_ptr<CBone> pAnim = make_shared<CBone>(*(shared_from_this().get()));
	return pAnim;
}
