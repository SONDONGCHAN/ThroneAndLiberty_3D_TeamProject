#pragma once

#include "Client_Defines.h"

#include "EffectGroup.h"
#include "AnimEffect.h"

BEGIN(Client)

class CEffectMgr
{
	DECLARE_SINGLETON(Client::CEffectMgr)

public:
	CEffectMgr();
	~CEffectMgr();

public:
	HRESULT Initialize();

public:
	_int	PlayEffect(string _strGroupID, shared_ptr<CGameObject> _pOwner, _bool _bForTest = false, _float _fTestStartFaster = 1.f);
	_int	PlayEffect(string _strGroupID, _float3 _vWorldPos, _bool _bLoop, _float _fDuration);
	_int	PlayAnimEffect(string _strGroupID, shared_ptr<CGameObject> _pOwner);
	HRESULT StopEffect(string _strGroupID, _int _iIdx);
	HRESULT AddEffect(string _strGroupID, _int _iCnt);
	HRESULT RemoveEffect(string _strGroupID, _int _iCnt);

public:
	HRESULT LoadEffectFiles();
	HRESULT LoadEffectGroup(string _strGroupKey);
	HRESULT LoadEffects(Json::Value& _Element, string _strEffectID, shared_ptr<CEffectGroup> pEffectGroup);

public:
	HRESULT AddAnimEffects();

private:
	unordered_map<string, vector<shared_ptr<CEffectGroup>>> m_EffectGroups;
	unordered_map<string, vector<shared_ptr<CAnimEffect>>>	m_AnimEffects;

	_bool m_IsInitDone = false;

private:
	string	m_strFilePath = "../../Tool_Effect/Bin/DataFiles/";
};

END
