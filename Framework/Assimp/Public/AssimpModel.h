#pragma once

#include "Assimp_Defines.h"

BEGIN(Assimp)

class CAssimpModel : public enable_shared_from_this<CAssimpModel>
{
public:
	struct BoneDesc {
		string strBoneName;
		_int iParentBoneIndex;
		_uint iBoneIndex;
		_float4x4 TransformMatrix;
	};

public:
	struct KEYFRAME
	{
		_float3 vScale, vPosition;
		_float4 vRotation;
		_double Time;

	};

public:
	CAssimpModel();
	~CAssimpModel();

public:
	void ConvertModel(const string& _strFilePath, _uint _iType, const wstring& _strSaveFileName, const wstring& _strFileName);

public:
	void ReadyBones(ofstream& out);

	void ReadBoneData(aiNode* _pNode, ofstream& out, _int _iParentBoneIdx);

	void ReadyMaterial(ofstream& out);
	void ReadyMesh(ofstream& out);
	void ReadyAnimMesh(ofstream& out);
	void ReadyAnim(ofstream& out);

public:
	void SetSavePath();
	_int GetBoneIndex(const _char* _pBoneName) const;

private:
	const aiScene* pAIScene = {};

private:
	vector<shared_ptr<class CAssimpBone>> m_Bones;
	vector<shared_ptr<class CAssimpMesh>> m_Meshes;

private:
	/*.dat Parsing Base Path*/
	wstring m_strSaveBasePath = TEXT("..\\..\\Client\\Bin\\Resources");
	/*애니메이션 Json Data Parsing*/
	wstring m_strAnimSavePath = TEXT("..\\..\\Client\\Bin\\DataFiles\\AnimData\\");

	wstring m_strObjName = TEXT("");

private:
	void WriteJson(Json::Value _value, const wstring& _strSavePath);

public:
	static shared_ptr<CAssimpModel> Create(const string& _strFilePath, _uint _iType , const wstring& _strSaveFileName, const wstring& _strFileName); // 파일경로 ? + 애님논애님 타입
};

END