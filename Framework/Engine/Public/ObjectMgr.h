#pragma once

/* =============================================== */
//	[ObjectMgr]
// 
//  현재 진행 중인 레벨 레이어 관리
//
//
/* =============================================== */


#include "Engine_Defines.h"

BEGIN(Engine)

class CGameObject;

class CObjectMgr final
{
public:
	CObjectMgr();
	~CObjectMgr();

public:
	HRESULT Initialize(_uint _iNumLevels);
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

public:
	HRESULT AddObject(_uint iLevelIndex, const wstring& strLayerTag, shared_ptr<CGameObject> _pGameObject);
	shared_ptr<class CGameObject> GetGameObject(_uint iLevelIndex, const wstring& strLayerTag);
	list<shared_ptr<class CGameObject>>* GetObjectList(_uint iLevelIndex, const wstring& strLayerTag);

public:
	shared_ptr<class CLayer> FindLayer(_uint _iLevelIndex, const wstring& _strLayerTag);
	void ClearLayer(_uint _iLevelIndex, const wstring& _strLayerTag);

private:
	_uint m_iNumLevels = { 0 };
	map<const wstring, class shared_ptr<class CLayer>>* m_pLayers = { nullptr };
	typedef map<const wstring, shared_ptr<class CLayer>> LAYERS;

public:
	static shared_ptr<CObjectMgr> Create(_uint iNumLevels);
	void Clear(_uint _iLevelNum);
	void Clear();

public:
	void SetChangeLevelSignal(_uint _iChangeLevel) {

		m_IsChangeSignalOn = true;
		m_iChangeLevel = _iChangeLevel;
	}

private:
	_bool m_IsChangeSignalOn = false;
	_uint m_iChangeLevel = 0;

private:
	void Free();


};

END