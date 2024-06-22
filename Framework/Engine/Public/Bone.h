#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CBone final : public std::enable_shared_from_this<CBone>
{
public:
	enum BONE_CONTROL_OPTION { OPTION_DEFAULT, OPTION_SUBTRACT_ROTATION_Y, OPTION_END };

public:
	CBone();
	~CBone() = default;

public:
	const _char* GetBoneName() const {
		return m_szName;
	}

	_float4x4 GetCombiendTransformationMatrix() const {
		return m_CombinedTransformationMatrix;
	}

	_float4x4* GetCombindTransformationMatrixPtr() {
		return &m_CombinedTransformationMatrix;
	}

	_float4x4* GetRefTransformationMatrixPtr() {
		return m_pRefTransformationMatrix;
	}

	_float4x4 GetTransformationMatrix() {
		return m_TransformationMatrix;
	}

	void SetTranslationMatrix(_float4x4 _TranslationMatrix) {
		m_TransformationMatrix =  _TranslationMatrix;
	}

	void SetCombindTransformationMatrixPtr(_float4x4* _pRefTransformationMatrix) {
		m_pRefTransformationMatrix = _pRefTransformationMatrix;
	}

public:
	void SetParentBoneIndex(_int _iParentBoneIndex) { m_iParentBoneIndex = _iParentBoneIndex; }

public:
	_int GetParentBoneIndex() { return  m_iParentBoneIndex; }
	_int GetOriginIndex() { return m_iOriginIndex; }

public:
	HRESULT Initialize(char* _pName, _int _iParentBoneIndex, _uint _iOriginIndex, ifstream& _ifs);
	void InvalidateCombinedTransformationMatrix(const vector<shared_ptr<CBone>>& _Bones);
	void InvalidateRefTransformationMatrix();

public:
	void	SetOption(BONE_CONTROL_OPTION eOption) { m_eMyOption = eOption; }


private:
	_char m_szName[MAX_PATH] = "";
	_float4x4 m_TransformationMatrix;
	_float4x4 m_CombinedTransformationMatrix;
	_float4x4* m_pRefTransformationMatrix;


	_int m_iParentBoneIndex = { 0 };
	_int m_iOriginIndex		= { 0 };

	_float4x4 m_PrevTransformationMatrix = XMMatrixIdentity();

private:
	BONE_CONTROL_OPTION m_eMyOption = { OPTION_DEFAULT };

public:
	static shared_ptr<CBone> Create(char* _pName, _int _iParentBoneIndex, _uint _iOriginIndex, ifstream& _ifs);
	shared_ptr<CBone> Clone();

};

END
