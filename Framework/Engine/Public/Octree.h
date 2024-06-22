#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

class CGameObject;

class COctree final : public std::enable_shared_from_this<COctree>
{
	struct tInstance
	{
		weak_ptr<CGameObject> pGameObject;
		vector<pair<_float3, _float4x4>> InstanceInfo;		// vPoint, vWorldMatrix
		float fRadius;
	};

protected:
	enum CHILD
	{
		LBB, RBB, LTB, RTB, LBF, RBF, LTF, RTF, CHILDEND
	//x  -	  +    -    +    -    +    -    +
	//y  -    -    +    +    -    -    +    +
	//z  -    -    -    -    +    +    +    +
	};

public:
	COctree();
	~COctree();

public:
	static shared_ptr<COctree> Create(const _float3& _vCenter, const float& _fHalfWidth, const int& _iDepthLimit, shared_ptr<COctree> _pParent = nullptr);

public:
	HRESULT Initialize(const _float3& _vCenter, const float& _fHalfWidth, const int& _iDepthLimit, shared_ptr<COctree> _pParent = nullptr);
	void PriorityTick(const float& _fTimeDelta);
	void Tick(const float& _fTimeDelta);
	void LateTick(const float& _fTimeDelta);

	void Culling();

public:
	bool AddStaticObject(shared_ptr<CGameObject> _pGameObject, const _float3& _vPoint, const float& _fRadius = 0.0f);
	bool AddStaticInstance(const string& _strModelTag, shared_ptr<CGameObject> _pGameObject, const _float3& _vPoint, const _float4x4& _WorldMatrix, const float& _fRadius = 0.0f);
	void Clear();

private:
	ContainmentType isDraw();
	void AllVisible();
	void Invisible();
	vector<tInstance>* FindInstance(const string& _strModelTag);

private:
	BoundingBox m_BoundingBox{};
	shared_ptr<COctree> m_pChilds[CHILDEND]{};
	weak_ptr<COctree> m_pParent;
	int m_iDepth{ 0 };
	bool m_isVisible{ false };
	bool m_isObtainStatic{ false };

	list<shared_ptr<CGameObject>> m_GameObjects;
	
	//map<const string, tInstance> m_Instances;
	map<const string, vector<tInstance>> m_Instances;
};

END