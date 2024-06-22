#pragma once
#include "Tool_Defines.h"

BEGIN(Engine)
class CTransform;

END

BEGIN(Tool_Map)
class CMap;
class CArea;
class CEnvironmentObject;
class CGroupObject;
class CTerrainTool;
class CObjectTool;

class CMapTool
{
public:
	enum IO { IO_INPUT, IO_OUTPUT, TYPE_END };
	enum SAVE { SAVE_FULL, SAVE_PART, SAVE_END };

private:
	CMapTool();
	DESTRUCTOR(CMapTool)

public:
	HRESULT Initialize();
	void Tick(_float _fTimeDelta);

private:
	void MakeList(string _strMapPath);

	void FileMap();

	HRESULT LoadMap();
	HRESULT SaveMap(SAVE _eSaveType);

	void ResourceExtract(SAVE _eSaveType);
	
	void CopyModelFile(string _strModelFilePath, const vector<string>& _vecModelName);
	void CopyModelTextureFile(string _strModelTextureFilePath, const vector<string>& _vecModelTextureName);
	void CopyMapFile();
	void CopyTerrainTextureFile(string _strTerrainTextureFilePath, string _strTerrainTexture);

	void EditArea();

	void EditModeArea();
	void ViewModeArea();
	void ChangedModeArea();

	HRESULT ShowMapTransform();

	// 공용 기능.
private:
	void SelectedComboListIndex(string _strListName, vector<string>& _vecList, _uint& _iSelectedIndex);
	string IODialog(IO _IOType);

	
public:
	void AddObject(shared_ptr<CEnvironmentObject> _pObject);
	void AddGroupObject(shared_ptr<CGroupObject> _pGroupObject);
	shared_ptr<CArea> GetCerrentEditModeArea();


private:
	_bool m_bIsMapToolWindow{};

	string m_strOriginModelFilePath = "..\\Bin\\Resources\\Level_MapTool\\Models\\NonAnim\\Environment";
	string m_strCopyModelFilePath = "..\\Bin\\Copy\\Model\\";
	
	string m_strOriginModelTextureFilePath = "..\\Bin\\Resources\\Level_MapTool\\Models\\ModelTexture\\Environment\\";
	string m_strCopyModelTextureFilePath = "..\\Bin\\Copy\\ModelTexture\\";

	string m_strOriginMapDataFilePath = "..\\Bin\\DataFiles\\Map\\";
	string m_strCopyMapDataFilePath = "..\\Bin\\Copy\\Map\\";

	string m_strOriginTerrainTextureFilePath = "..\\Bin\\Resources\\Level_MapTool\\Textures\\Terrain";
	string m_strCopyTerrainTextureFilePath = "..\\Bin\\Copy\\TerrainTexture\\";

	_uint m_SelectedLoadMapDataFileIndex{};
	vector<string> m_vecLoadMapDataFile{};

	
	string m_strSaveMapDataFileName{};

	_float3 m_vScale{};
	_float3	m_vRotation{};
	_float3 m_vTranslation{};

	



	weak_ptr<CTerrainTool> m_pTerrainTool;
	weak_ptr<CObjectTool> m_pObjectTool;

	shared_ptr<CTransform> m_pMapTransform{};




	weak_ptr<CMap> m_pMap{};
	_bool m_bIsMap{};
	string m_strCreateAreaName{};

	_uint m_iSelectedEditModeAreaIndex{};
	_uint m_iCurEditModeAreaIndex{};

	vector<string> m_vecCurViewModeAreaNames{};

	_bool m_bIsChangedEditModeArea{};
	_bool m_bIsChangedViewModeArea{};


/////////////////
	vector<string> m_vecSaveMapDataFileMode = {"Full Area", "Part Area"};
	_uint m_iSelectedSaveMapDataFileModeIndex{};

	/////////////////
	_bool m_bIsTerrainSaveNeeded{};
	_bool m_bIsAreasSaveNeeded{};

	vector<shared_ptr<CArea>> m_vecTempArea{};

public:
	static shared_ptr<CMapTool> Create();
};
END