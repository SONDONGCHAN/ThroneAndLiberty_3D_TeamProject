#pragma once

#include "Tool_Defines.h"

BEGIN(Tool_Effect)

class CImGuiMgr
{
	DECLARE_SINGLETON(CImGuiMgr)

public:
	CImGuiMgr();
	~CImGuiMgr();

public:
	HRESULT Initialize();
	void	Tick(_float _fTimeDelta);
	HRESULT	Render();

public:
	void	TickImGui(_float _fTimeDelta);
	
	void	TickSetting(_float _fTimeDelta);
	void	TickMainWindow(_float _fTimeDelta);
	void	TickListWindow(_float _fTimeDelta);
	void	TickPlayWindow(_float _fTimeDelta);
	void	TickModelWindow(_float _fTimeDelta);

	void	TickMesh(_float _fTimeDelta);
	void	TickTexture(_float _fTimeDelta);
	void	TickParticle(_float _fTimeDelta);

public:
	void	KeyInput(_float _fTimeDelta);

	void	LoadEffectGroupList();
	void	LoadEffectList(string _strEffectGroupKey);
	void	LoadEffect();

	HRESULT	AddToEffect();
	HRESULT	AddToPlay();

public:
	void	ShutDown();

public:
	void	TextModel();

public:
	void	SetModelObject(shared_ptr<CGameObject> _pGameObject, string _strModelKey, string _strAnimKey);

public:
	HRESULT	SaveEffectJson();
	HRESULT SaveEffectListJson(Json::Value& _EffectListRoot, shared_ptr<class CEffect> _pEffect, _int _iIdx);

	HRESULT	LoadEffectJson(const string& _strGroupKey);
	HRESULT LoadEffectListJson(Json::Value& _EffectListRoot);

private:
	HRESULT ModelToClient(string _strModelPath, string _strModelKey);
	HRESULT TextureToClient(TEX_TYPE _iTexType, string _strTextureKey);

private:
	_bool	m_bShowPlayWindow = false;
	_bool	m_bShowSetting = false;

	_bool	m_bTargetVisible = false;
	_int	m_iTargetNum = 0;
	_bool	m_IsHDROn = true;

public:
	shared_ptr<class CWindowMesh>		GetWindowMesh() { return m_pWindowMesh; }
	shared_ptr<class CWindowPlay>		GetWindowPlay() { return m_pWindowPlay; }
	shared_ptr<class CWindowModel>		GetWindowModel() { return m_pWindowModel; }
	shared_ptr<class CWindowParticle>	GetWindowParticle() { return m_pWindowParticle; }

public:
	vector<shared_ptr<class CEffect>>	GetEffectList() { return m_pEffectList; }

private:
	shared_ptr<class CWindowMesh>		m_pWindowMesh = nullptr;
	shared_ptr<class CWindowPlay>		m_pWindowPlay = nullptr;
	shared_ptr<class CWindowModel>		m_pWindowModel = nullptr;
	shared_ptr<class CWindowParticle>	m_pWindowParticle = nullptr;

private:
	char									m_strEffectName[50] = "";

private:
	_int									m_iCreateEffectType = -1;

	vector<string>							m_strEffectGroupList;			// 이펙트 그룹 리스트
	_int									m_iSelectEffectGroupIdx = 0;

	vector<string>							m_strEffectList;				// 이펙트 그룹 하나를 로드했을 때 포함된 이펙트들
	vector<shared_ptr<class CEffect>>		m_pEffectList;
	_int									m_iSelectLoadEffectList = 0;

	map<string, shared_ptr<class CEffect>>	m_pPlayEffectList;				// 재생 중인 이펙트 리스트

	shared_ptr<class CEffectGroup>			m_pLoadEffectGroup;

	_int									m_iEffectAttachType = 0;

private: /* Parsing */
	string	m_strSavePath = "../Bin/DataFiles/";
	_int	m_iGroupCloneCnt = 1;
	char	m_strGroupFileName[50] = "";
	string	m_strLoadFile;
	string	m_strLoadNoiseFile;
	string	m_strLoadJsonFile;

	string	m_strOriginPath = "../Bin/Resources/Level_EffectTool/";
	string	m_strClientRscPth = "../../Client/Bin/Resources/Level_Static/Effect/";

	unordered_map<string, vector<string>> m_uiChildrenTags;
	vector<string> m_strLoadUIList;

private:
	const char* m_chTitle = "Noooooooo";
	const char* m_chContent = "Empty";

private:
	ComPtr<ID3D11Device>				m_pDevice = nullptr;
	ComPtr<ID3D11DeviceContext>			m_pContext = nullptr;
};

END