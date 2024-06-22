#pragma once
#include "Engine_Defines.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "Tool_Animation_Defines.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"
#include "AnimObject.h"


class CAnimToolImguiManager
{
	DECLARE_SINGLETON(CAnimToolImguiManager)

public:
	struct ShapeInfo
	{
		_uint iGeometryType;

		_float3 vBoxSize = { 0.f, 0.f, 0.f };
		_float	fSphereRadius = { 0.f };
		_float	fCapsuleHeight = { 0.f };
		_float	fCapsuleRadius = { 0.f };

		_float3 vOffSetPosition;
		_float3 vOffSetRotation;
		
		_uint iMyFlag;
		_uint iTargetFlag;

		_float fColStartTrackPos;
		_float fColEndTrackPos;

		_uint iAnimationIndex;
	};
	struct ProjectileInfo
	{
		_int			 iAnimationIndex;
		vector<_float>	 fProjectileTriggerTrackPoss;
		vector<_int>	 eProjectileTriggerOwners;
	};
	struct SoundInfo
	{
		_uint	iAnimationIndex;
		string	strSoundName;
		_float	fSoundTrackPoss;
		_float	fSoundVolume;
	};

private:
	CAnimToolImguiManager();
	~CAnimToolImguiManager();

public:
	HRESULT Initialize();
	void	ImGuiSetUp();

public:
	void	ImGuiTick();
	void	ImGuiRender();
	void	ShowGui();
	void	ShutDown();

public:
	vector<string>	GetAnimModelName() { return m_vecAnimModelName; }

private:
	bool			FileExists(const wstring& filePath);
	HRESULT			LoadAllAnimModelName(const wstring& _strModelPath);

	void			SaveShapesToJSON(string _strModelKey, map<string, ShapeInfo> _shapes);
	void			SaveProjTriggersToJSON(string _strModelKey, vector<ProjectileInfo> _Triggers);
	void			SaveSoundToJSON(string _strModelKey, map<string, vector<SoundInfo>> _Sounds);

	void			LoadAllShapesJSON();
	void			LoadAllProjTriggersJSON();
	void			LoadAllSoundJSON();


private:
	void	StoreAllSoundPath(const filesystem::path& CurrentPath, const string& FileExtension);
	void	FindAllSoundPath(const string& _strBasePath, const string& _FileExtension);

private:
	// 랜더용객체
	shared_ptr<CAnimObject>	m_pAnimObject = { nullptr };

	// 모델
	vector<string>			m_vecAnimModelName;	
	_uint					m_iCurrentModelIndex = { 0 };
	shared_ptr<CModel>		m_pCurrentAnimModel = { nullptr };
	_bool					m_bObjectCreated = { false };

	// 애니매이션
	vector<shared_ptr<class CAnimation>>	m_CurrentModelAnimations;
	_int									m_iCurrentAnimationIndex = { -1 };
	//vector<const _char*>					m_vecAnimationNames;
	vector<string>							m_vecAnimationNames;

	_bool									m_isAnimationPlay = { true };

	// 데이터 저장 map<모델이름, map<애니메이션이름,정보구조체> >
	//map<string, map<string, ShapeInfo>> m_ModelDatas;
	wstring m_strSavePath				= TEXT("..\\Bin\\DataFiles\\");
	wstring m_strProjTriggersSavePath	= TEXT("..\\Bin\\DataFiles\\ProjectileTrigger\\");
	wstring m_strSoundSavePath			= TEXT("..\\Bin\\DataFiles\\SoundTrigger\\");


	// 로드한 정보
	map<string, map<string, ShapeInfo>> m_ModelLoadDatas;
	map<string, ShapeInfo> m_CurrentShapes;

	//====Projectile====
	map<string, vector<ProjectileInfo>> m_ProjTriggerLoadDatas;
	vector<ProjectileInfo>	m_CurrentProjectileDatas;

	// 로드한 모든 사운드 정보
	vector<string>	m_vecAllSound;

	// 애니메이션에 심은 사운드정보 데이터 map< str모델이름, map<str애니메이션이름, vector사운드 > >
	map<string, map<string, vector<SoundInfo>>>	m_ModelSoundDatas;
	map<string, vector<SoundInfo>>				m_CurrentModelSoundDatas;
	vector<SoundInfo>							m_CurrnetAnimSoundDatas;

private:
	wrl::ComPtr<ID3D11Device> m_pDevice = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
};

