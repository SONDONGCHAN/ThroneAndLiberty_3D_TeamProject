#pragma once

#include "Component.h"
#include "GameObject.h"

BEGIN(Engine)

class CTransform;


class CVIPhysX;



class ENGINE_DLL CRigidBody final : public CComponent
{
public:
	enum ELOCK_FLAG
	{
		NONE			= 0,
		LOCK_POS_X		= (1 << 0),
		LOCK_POS_Y		= (1 << 1),
		LOCK_POS_Z		= (1 << 2),
		LOCK_ROT_X		= (1 << 3),
		LOCK_ROT_Y		= (1 << 4),
		LOCK_ROT_Z		= (1 << 5),

		LOCK_POS_ALL	= LOCK_POS_X | LOCK_POS_Y | LOCK_POS_Z,
		LOCK_ROT_ALL	= LOCK_ROT_X | LOCK_ROT_Y | LOCK_ROT_Z,
		LOCK_ALL		= LOCK_POS_ALL | LOCK_ROT_ALL
	};

	struct RIGIDBODYDESC
	{
		/* Create Actor */
		bool			isStatic			= false;	/* 정적인 객체는 true */
		bool			isTrigger			= false;	/* 트리거 객체는 true */
		bool			isGravity			= true;		/* 중력을 적용 받는 객체는 true, (static 은 중력을 받지않음)*/
		bool			isInitCol			= true;		/* 생성하자마자 충돌을 끄려면 false */
		shared_ptr<CTransform> pTransform = nullptr;	/* Transform 통째로 던지면 됨 */
		UINT			eLock_Flag			= ELOCK_FLAG::NONE; /* Static은 필요없음,CRigidBody::ELOCK_FLAG */
		
		/* Create Shape */
		weak_ptr<CGameObject> pOwnerObject;				/* 소유 객체의 포인터 */ 
		float			fStaticFriction		= 0.5f;		/* 정지 마찰력(0.0f~) */
		float			fDynamicFriction	= 0.5f;		/* 운동 마찰력(0.0f~) */
		float			fRestitution		= 0.0f;		/* 탄성력(0.0f~1.0f) */
		Geometry*		pGeometry			= nullptr;	/* 모양(GeometrySphere, GeometryBox, etc) */
		ECOLLISIONFLAG	eThisColFlag		= COL_NONE;	/* 자기 자신의 충돌 Flag */
		UINT			eTargetColFlag		= COL_NONE; /* 충돌할 객체의 Flag를 추가 ex) COL_MONSTER | COL_ITEM  */

		void*			pArg				= nullptr;	/* 무언가 담을거, 구조체를 "new"로 할당해서 넣어줄 것 */
	};

public:
	CRigidBody(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	CRigidBody(const shared_ptr<CRigidBody> _pOrigin);
	virtual ~CRigidBody();

public:
	static shared_ptr<CRigidBody> Create(wrl::ComPtr<ID3D11Device> _pDevice, wrl::ComPtr<ID3D11DeviceContext> _pContext);
	virtual shared_ptr<CComponent> Clone(void* _pArg) override;

public:
	HRESULT Initialize();;
	virtual HRESULT InitializeClone(void* _pArg) override;

	/* 추가적인 모양을 더 넣으려면 이걸로 넣으면 됨
	   단, 이미 만들어진 Actor는 바꿀 수 없음. (static 이나 eLock_Flag 같은 것들)     */
	HRESULT CreateShape(RIGIDBODYDESC* _pDesc, bool _isShared = false);

public:
	_float3 GetPosition();
	_quaternion GetQuat();

public:
	HRESULT ChangeShape(const char* _szShapeTag);	/* 기존에 만들어 놓은 shape로 변경, Actor에 붙어있는 모든 shape는 떨어진다 */
	HRESULT AttachShape(const char* _szShapeTag);	/* 기존에 만들어 놓은 shape를 덧붙인다, Actor에 붙어있는 shpae는 유지 */
	void SetPosition(const _float3& _vPosition, bool _isAutowake = true);
	void SetRotation(const _quaternion& _vQuat, bool _isAutowake = true);
	void SetGlobalPos(const _float3& _vPosition, const _quaternion& _Quat, bool _isAutowake = true);
	void EnableCollision(const char* _szShapeTag);
	void DisableCollision(const char* _szShapeTag);
	void WakeUp();

public:		/* Dynamic Function */
	_float3 GetLinearVelocity();
	_float3 GetAngularVelocity();
	void SetLinearVelocity(const PxVec3& _vLinVel, bool _isAutowake = true);
	void SetAngularVelocity(const PxVec3& _vAngVel, bool _isAutowake = true);
	void SetMaxLinearVelocity(float _fMaxLinVel);
	void SetLockFlags(UINT _eLockFlags, bool _isLock);	/* CRigidBody::ELOCK_FLAG 을 참조 */
	void AddForce(const PxVec3& _vForce, PxForceMode::Enum _eMode = PxForceMode::eFORCE, bool _isAutowake = true);
	void SetMass(float _fMass);
	void GravityOn();
	void GravityOff();
	void SimulationOn();
	void SimulationOff();

private:
	HRESULT CheckArgument(RIGIDBODYDESC* _pDesc);
	HRESULT CreateActor(RIGIDBODYDESC* _pDesc);
	HRESULT CreateGeometry(RIGIDBODYDESC* _pDesc, bool _isShared);

	PxShape* Find_Shape(const char* _szShapeTag);

	virtual HRESULT Render() override;
	HRESULT CreateBuffer(PxShape* _pShape, float _fSizeX, float _fSizeY, float _fSizeZ, const _float3& _vOffsetPosition, const _quaternion& _Quat);

private:
	PxRigidActor*	m_pActor	= nullptr;
	PxMaterial*		m_pMaterial = nullptr;
	unordered_multimap<string, pair<PxShape*, shared_ptr<COLLISIONDATA>>> m_Shapes; // 플레이어의 경우 사람, 늑대, 독수리 등
	PxScene*		m_pScene	= nullptr;
	
	bool			m_isStatic	= false;


private:
	list<pair<PxShape*, shared_ptr<CVIPhysX>>> m_lstVIBuffer;

private:
	void Free();
};

END