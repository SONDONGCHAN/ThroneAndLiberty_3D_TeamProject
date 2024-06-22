#pragma once
#include "Tool_Defines.h"
#include "Transform.h"

BEGIN(Tool_Map)
class CEnvironmentObject;
class CArea;
class CGroupObject;

class CObjectTool
{
private:
	enum NODETYPE { TYPE_INTERNAL, TYPE_LEAF, TYPE_END };

	struct NODE
	{
		NODETYPE eType{};
		string strName{};
		_uint iIdxParentNode{};
		_uint iIdxSiblingNode{};
		_uint iIdxMyNode{};

		_uint iNumChild{};
		vector<_uint> vecDirectoryIdxList{};
		vector<string> vecFileList{};
	};

private:
	CObjectTool();
	DESTRUCTOR(CObjectTool)

public:
	_bool IsObjectsEmpty() { return m_mapObjsID.empty(); }
	map<_uint, weak_ptr<CEnvironmentObject>>& GetObjectsID() { return m_mapObjsID; }

	void ResetEdge(shared_ptr<CArea> _pArea);

public:
	HRESULT Initialize();
	void Tick(_float _fTimeDelta);

private: // Initialize
	void MakeDirectory(string _strFilePath, _uint& _iIndex, _int _iParentIndex);
	HRESULT MakePrefabGroupObjectsList();

private: // UI
	void ModelExplorer();
	void ShowDirectory(_uint& _iIndex);
	void ShowModelList();

	void EditGroupObject();
	void ShowGroupObjectMemberList();
	HRESULT CreatePrefabGroupObjectFile();
	void ClearGroupObject();
	HRESULT DeleteGroupObjectMember();

	void CreateGroupObject();
	void UnGroupObjects();

private: // 기능
	void PickedObject();
	void EditGizmo();
	
	void EditCreatePrefabGroupObjectMember(_bool _bIsAdd);

	void ShowProtoGroupObjectList();

	void TargetAxisInversion(CTransform::STATE _eState);

	void DeleteObject();

	void TrackingObject(_float _fTimeDelta);

	void CopyObject();

	// 키 이벤트
	void KeyEvent();
	void Placing();

	



private:
	// MakeDirectory
	string m_strFilePath = "..\\Bin\\Resources\\Level_MapTool\\Models\\NonAnim\\Environment";
	vector<NODE> m_vecFileNodeTree{};
	
	// MakeGroupObjectList
	string m_strPrefabGroupObjectDataFilePath = "..\\Bin\\DataFiles\\GroupObject\\";
	

	// ShowDirectory
	map<_uint, _bool> m_mapLeafNodeList{};

	// ShowModelList
	_bool m_bIsSelectedModel{};
	_int m_iIdxModel{};
	string m_strCurSelectedModel{};

	// MakeGroupObject
	string m_strCreatePrefabGroupObjectName;
	vector<weak_ptr<CEnvironmentObject>> m_vecCreatePrefabGroupObjectMembers{};

	// ShowGroupObjectMemberList
	_int m_iIdxMakeGroupObjectMember{};
	_bool m_bIsSelectedGroupMember{};

	// ShowProtoGroupObjectList
	_bool m_bIsSelectedProtoGroupObject{};
	_int m_iIdxProtoGroupObject{};
	string m_strCurSelectProtoGroupObjectName{};
	
	// EnableObject
	//_bool m_bIsEnable{};
	//vector<weak_ptr<CGameObject>> m_vecEnableObject{};

	shared_ptr<CEnvironmentObject> m_pSelectedObject{};
	weak_ptr<CEnvironmentObject> m_pPickedObject{};

	shared_ptr<CGroupObject> m_pSelectedPrefabGroupObject{};
	weak_ptr<CGroupObject> m_pPickedGroupObject{};

	vector<shared_ptr<CGroupObject>> m_vecProtoGroupObjects{};

	map<_uint, weak_ptr<CEnvironmentObject>> m_mapObjsID{};


/// <summary>
	weak_ptr<CEnvironmentObject> TestObj{};
/// </summary>

public:
	static shared_ptr<CObjectTool> Create();
};
END
