#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CInteractionObj abstract : public CGameObject
{
public:
	CInteractionObj();
	virtual  ~CInteractionObj() = default;

public:
	virtual HRESULT Initialize(CTransform::TRANSFORM_DESC* _pDesc);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render() { return S_OK; };

public:
	_bool CheckDistance();
	virtual void InteractionOn() = 0;
	virtual void InteractionOff();

protected:
	void KeyInput();

/*Player(Target)*/
protected:
	_bool	m_bInteractAble = true;			// Interaction이 가능한 상태인지
	_bool m_IsInteractionOn = false;
	/*일회성 상호작용 오브젝트에 사용 -> true일 경우 더 이상 체크 안함*/
	_bool m_IsInteractionDone = false;
	weak_ptr<CGameObject> m_pPlayer;

/*Interaction UI*/
protected:
	weak_ptr<class CUIPanel> m_pInteractionPanel;
	/*Object Position으로부터 얼마나 먼 곳에 UI를 띄울지 결정*/
	_float3 m_vUIDistance = _float3(0.f, 0.f, 0.f);

	CRenderer::ERENDER_GROUP m_eRenderGroup = CRenderer::RENDER_NONBLEND;

protected:
	_float m_fInteractDistance = 4.f;

protected:
	shared_ptr<CModel> m_pModel = nullptr;

};

END