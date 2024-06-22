#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CTexture;
END

BEGIN(Client)

class CEquipment final : public CPartObject
{
public:
	struct EQUIPMENT_DESC
	{
		CPartObject::PARTOBJ_DESC tPartObject;
		ITEM_EQUIP_TYPE				ePartsType;
	};

public:
	CEquipment();
	virtual ~CEquipment()  override;

public:
	virtual HRESULT Initialize(shared_ptr<EQUIPMENT_DESC> _pEquipmentDesc, shared_ptr<CModel> _pReferenceModel);
	virtual void	PriorityTick(_float _fTimeDelta)  override;
	virtual void	Tick(_float _fTimeDelta)  override;
	virtual void	LateTick(_float _fTimeDelta)  override;
	virtual HRESULT Render()  override;

public:
	virtual HRESULT RenderEdgeDepth() override;
	virtual HRESULT RenderEdge() override;

public:
	void	AddEquipmentModel(ITEM_EQUIP_TYPE _ePartsKey);
	virtual HRESULT RenderShadow() override;

	void	SetCurrentModel(string _strModelKey);
	void	SetNoneModel();

	ITEM_EQUIP_TYPE GetEquipType() { return m_ePartsKey; }


private:
	map<string, shared_ptr<CModel>> m_Models = {};

	shared_ptr<CModel>		m_pCurrentModel;
	
	weak_ptr<CModel>		m_pParentModelCom;


private:
	ITEM_EQUIP_TYPE		m_ePartsKey;


public:
	static shared_ptr<CEquipment> Create(shared_ptr<EQUIPMENT_DESC> _pEquipmentDesc, shared_ptr<CModel> _pReferenceModel);

};

END