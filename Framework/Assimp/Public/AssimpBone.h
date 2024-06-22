#pragma once

#include "Assimp_Defines.h"

BEGIN(Assimp)

class CAssimpBone
{
public:
	CAssimpBone();
	~CAssimpBone();

public:
	const _char* GetBoneName() const {
		return m_szName;
	}

	_float4x4 GetCombiendTransformationMatrix() const {
		return m_CombinedTransformationMatrix;
	}

	_float4x4 GetTransformationMatrix() {
		return m_TransformationMatrix;
	}

	void SetTranslationMatrix(_float4x4 _TranslationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, _TranslationMatrix);
	}

public:
	HRESULT Initialize(const aiNode* _pAIBone, _int _iParentBoneIndex);
public:
	void ParsingBone(ofstream& out);

private:
	_char m_szName[MAX_PATH] = "";
	_float4x4 m_TransformationMatrix;
	_float4x4 m_CombinedTransformationMatrix;

	_int m_iParentBoneIndex = { 0 };

	_float4x4 m_PrevTransformationMatrix = XMMatrixIdentity();

public:
	static shared_ptr<CAssimpBone> Create(const aiNode* _pAIBone, _int _iParentBoneIndex);



};

END