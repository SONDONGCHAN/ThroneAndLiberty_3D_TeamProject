#pragma once

#include "PartObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CModel;
class CTexture;
END

BEGIN(Tool_Effect)

class CWeapon final : public CPartObject
{
public:
	struct WEAPON_DESC
	{
		CPartObject::PARTOBJ_DESC tPartObject;
		_float4x4* pSocketMatrix;
		string strModelKey;
		_float4x4 OffsetMatrixNormal;
		_float4x4 OffsetMatrixSituation;
	};
	
public:
	CWeapon();
	virtual ~CWeapon() override;

public:
	HRESULT Initialize(WEAPON_DESC* _pWeaponDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);
	virtual void PriorityTick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void LateTick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	SwitchOffsetToNormal() { m_CurrentOffset = m_OffsetMatrixNormal;}
	void	SwitchOffsetToSituation() { m_CurrentOffset = m_OffsetMatrixSituation;}

protected:
	shared_ptr<CModel> m_pModelCom = nullptr;

private:
	_float4x4*	m_pSocketMatrix = nullptr;
	string		m_strModelKey;
	_float4x4	m_OffsetMatrixNormal	= { XMMatrixIdentity() };
	_float4x4	m_OffsetMatrixSituation = { XMMatrixIdentity() };
	_float4x4	m_CurrentOffset			= { XMMatrixIdentity() };

public:
	static shared_ptr<CWeapon> Create(WEAPON_DESC* _pWeaponDesc, CTransform::TRANSFORM_DESC* _pTransformDesc);
};

END

