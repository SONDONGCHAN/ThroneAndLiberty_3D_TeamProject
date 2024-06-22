#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CModel;
class CTexture;
END

BEGIN(Tool_Map)
class CEnvironmentObject;

class CGroupObject : public CGameObject
{
public:
	struct MEMBER_DESC
	{
		string strModelKey{};
		_float4x4 matWorld{};
		_float3 vCenterPoint{};
		_float fRadius{};
	};

private:
	CGroupObject();
	DESTRUCTOR(CGroupObject)

public:
	string GetGroupObjectName() { return m_strGroupObjectName; }
	vector<shared_ptr<CEnvironmentObject>>& GetMemberObjects() { return m_vecMemberObjects; }
	vector<pair<string, _float4x4>> GetMemberInfo() { return m_vecMemberInfo; }

public:
	virtual HRESULT Initialize(string _strGroupObjectName, vector<pair<string, _float4x4>> _vecMemberInfo);
	virtual HRESULT Initialize(string _strGroupObjectName, _float4x4 _matGroupObjectWorld, vector<MEMBER_DESC> _vecMemberObjectDesc);
	virtual void	PriorityTick(_float _fTimeDelta);
	virtual void	Tick(_float _fTimeDelta);
	virtual void	LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	shared_ptr<CGroupObject> Clone();

private:
	string m_strGroupObjectName{};

	vector<pair<string, _float4x4>> m_vecMemberInfo{};

	vector<shared_ptr<CEnvironmentObject>> m_vecMemberObjects{};

public:
	static shared_ptr<CGroupObject> Create(string _strGroupObjectName, vector<pair<string, _float4x4>> _vecMemberInfo);
	static shared_ptr<CGroupObject> Create(string _strGroupObjectName, _float4x4 _matGroupObjectWorld, vector<MEMBER_DESC> _vecMemberObjectDesc);
}; 
END