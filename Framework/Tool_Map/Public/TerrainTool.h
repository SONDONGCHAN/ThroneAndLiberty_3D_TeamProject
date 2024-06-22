#pragma once
#include "Tool_Defines.h"

BEGIN(Tool_Map)
class CTerrain;

class CTerrainTool : public std::enable_shared_from_this<CTerrainTool>
{
public:
	//enum BUFFERTYPE { BUFFER_CREATE, BUFFER_BINARYFILE, BUFFER_END };
	enum BLENDTYPE { BLEND_RED, BLEND_GREEN, BLEND_BLUE, BLEND_ALPHA, BLEND_END };
	
	struct WEIGHTDATA_DESC
	{
		string strName{};
		vector<_float4> vecPixelColor{};
		vector<_uint> vecPixel{};
		array<_uint, BLEND_END> arrBlendDiffuseIndex{};

		_int iSelectedBlendDiffuseChannelIndex{};
	};
	
private:
	CTerrainTool();
	DESTRUCTOR(CTerrainTool)

public:
	shared_ptr<CTerrain> GetTerrain() { return m_pTerrain.lock(); }
	string GetSaveTerrainDataFileName() { return m_strSaveTerrainDataFileName; }

public:
	HRESULT Initialize();
	void Tick(_float _fTimeDelta);

	

public:
	//HRESULT LoadTerrain(ifstream& _InFileStream);
	HRESULT LoadTerrain(string& _strLoadTerrainFileName);

	//HRESULT SaveTerrain(ofstream& _OutFileStream);
	HRESULT SaveFileNameCheck();
	HRESULT SaveTerrain();

	void FileTerrain();


private: // Initialize 단계
	void MakeList(string _strResourcePath);

private: // ReadyTerrain 단계
	void EditTerrain();

	void EditBuffer();
	void EditBaseDiffuse();

	void EditWeight();
	void EditWeightDesc();
	void EditBlendDiffuse();

	//void EditTerrain();
	
	//void CreateBuffer();
	//void LoadBuffer();

	void EditWeightTexture();



	//HRESULT LoadTextures();

	
	void EditGizmo();


	
	ComPtr<ID3D11ShaderResourceView> CreateWeightSRV(vector<_uint>& _vecPixel);
	HRESULT LoadWeightTexture();
	

	void UpdateWeightTextureList();

	void EditHight();
	
	
	HRESULT EditWeightTexture(_float _fTimeDelta);
	HRESULT TestTick();

private:
	void InputEvent(_float _fTimeDelta);
	void SelectedComboListIndex(string _strListName, vector<string>& _vecList, _uint& _iSelectedIndex);
	void UpdateHightPos(_float _fPower, _float _fTimeDelta);
	void UpdateHightFlatPos(_float _fPower, _float _fTimeDelta);


	void UpdateWeightDataDesc(int num);

private:
	// For. Buffer Data File
	string m_strBufferDataFilePath = "..\\Bin\\DataFiles\\Buffer";
	vector<string> m_vecBufferDataFiles{};
	_uint m_SelectedBufferDataFileIndex{};
	string m_strSaveBufferDataFileName{};
	// 추후 FBX->Buffer.dat 변경할 때 사용됨.
	//string m_strCreateBufferName{};
	
	// For. DiffuseTextureFile
	string m_strDiffuseTextureFilePath = "..\\Bin\\Resources\\Level_MapTool\\Textures\\Diffuse";
	vector<string> m_vecDiffuseTextureFiles{};
	
	// For. Buffer File
	_uint m_SelectedBaseDiffuseTextureIndex{};
	//string m_strSaveBaseDiffuseName{};

	// For. Weight File
	string m_strWeightDataFilePath = "..\\Bin\\DataFiles\\Weight";
	vector<WEIGHTDATA_DESC> m_vecWeightDataDesc{};
	_uint m_iSelectedWeightDescIndex{};
	_uint m_iSelectedClearWeightDescIndex{};

	string m_strAddWeightDescName{};

	
	// Ready Buffer Type
	//_int m_iSelectedLoadBufferType{};

	// Make Buffer Size
	//pair<_int, _int> m_iCreateSize{};
	array<_int, AXIS_END> m_arrCreateSize{};
	//_uint m_iWeightTextureSize{};

	// Selected Load List Index
	
	_float3 m_vPosPicked{};

	_float m_fRadius{ 1.f };
	_float m_fSharp{ 1.f };
	_float m_fInterval{ 1.f };
	
	// Terrain Load
	_uint m_iIdxSelectedLoadTerrainDataFile{};
	vector<string> m_vecLoadTerrainDataFile{};

	// Terrain HightPos Update
	

	weak_ptr<CTerrain> m_pTerrain{};

	_bool m_bIsBrush{};

	
	//vector<_float4> m_vecPixelColor{};
	
	//vector<_uint> m_vecPixel{};


/// //////////

	string m_strCreateWeightTextureName{};
	_uint m_iWeightTextureSize{ 2048 };

/// ////////
	vector<string> m_vecLoadWeightTextures{};
	_uint m_iIdxSelectedLoadWeightTextures{};

/// ///////
	string m_strSaveWeightTextureName{};
	/// ///////
	vector<string> m_vecEditWeightTextures{};
	
	
	//array<_uint, BLEND_END> m_iIdxSelectedBlendTextures{};


	
////////////
	_float m_fTerrainFlatPivotY{};


	////////////
	string m_strTerrainDataFilePath = "..\\Bin\\DataFiles\\Terrain";

	
	

	string m_strSaveTerrainDataFileName{};

	
	string m_strSaveWeightDataFileName{};
	
	
	//vector<Map<string, vector<string>>> m_vecWeights{};
	
	
	vector<thread> m_vecThreads;
	//array<thread, 20> m_vecThreads;

	vector<_bool> m_vecJoinableCheck;
	_bool m_ebIsUsingEditWeight{};



	//_float3 m_vWeightPos{};
	//_float m_fWeightMapRadius{};
	//_int m_iRectLT_X{};
	//_int m_iRectLT_Z{};
	//_int m_iRectRB_X{};
	//_int m_iRectRB_Z{};
	//_float3 m_vPixelPos{};
	//_float m_fDistance{};

	map<_uint, _float> m_mapDeltaWeight{};

	_uint m_iCountWeightDataDescIndex{};

public:
	static shared_ptr<CTerrainTool> Create();
};
END