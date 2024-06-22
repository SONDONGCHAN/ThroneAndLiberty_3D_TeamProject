#pragma once

#include "InteractionObj.h"

#include "Client_Defines.h"

BEGIN(Client)

class CEnterStatue : public CInteractionObj
{
public:
	CEnterStatue();
	virtual ~CEnterStatue() = default;

public:
	virtual HRESULT Initialize(_float3 _vPosition, _float _fAnswerDegree, _int _iStatueIdx);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render();

public:
	virtual HRESULT RenderShadow();

public:
	virtual void InteractionOn();

public:
	_bool IsConnected() { return m_IsConnected; }
	void SetEnergyOn(_bool _IsEnergyOn);
	void SetDungeonDoor(shared_ptr<CGameObject> _pDoor);

private:
	void SetLaserEffect();

private:
	_float m_fCurrentDegree = 0.f;
	_float m_fAnswerDegree = 0.f;

	_float m_fTurnSpeed = 1.f;

private:
	_bool m_IsRotaion = false;

	_float m_fRotationTime = 0.f;

	//Floating State
private:
	_float m_fFloatingDir = 1.f;
	_float m_fHeightSum = 0.f;

	_float3 m_vOriginPos = _float3();

private:
	shared_ptr<CModel> m_pJewelModel = nullptr;
	shared_ptr<CTransform> m_pJewelTransform = nullptr;

private:
	/*Connect With Next Statue*/
	_bool m_IsConnected = false;
	/*Connect With Prev Statue*/
	_bool m_IsEnergyOn = false;

private:
	_int m_iLaserEffectIdx = 0;
	_int m_iFrontEffectIdx = 0;

	_int m_iStatueIdx = 0;
	
	string m_strEffectID = "";

private:
	weak_ptr<class CDungeonDoor> m_pDoor;

public:
	static shared_ptr<CEnterStatue> Create(_float3 _vPos, _float _fAnswerDegree, _int _iStatueIdx);


};

END