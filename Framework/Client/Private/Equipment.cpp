#include "Equipment.h"
#include "Model.h"
#include "ItemEquip.h"
#include "ItemMgr.h"

CEquipment::CEquipment()
{
}

CEquipment::~CEquipment()
{
}

HRESULT CEquipment::Initialize(shared_ptr<EQUIPMENT_DESC> _pEquipmentDesc, shared_ptr<CModel> _pReferenceModel)
{
	shared_ptr<EQUIPMENT_DESC> pEquipmentDesc = _pEquipmentDesc;	
	m_ePartsKey = pEquipmentDesc->ePartsType;

	PARTOBJ_DESC* pPartObjDesc = (PARTOBJ_DESC*)_pEquipmentDesc.get();
	
	shared_ptr<CTransform::TRANSFORM_DESC> pArg = make_shared<CTransform::TRANSFORM_DESC>();
	pArg->fSpeedPerSec = 0.f;
	pArg->fRotationPerSec = XMConvertToRadians(0.f);
	
	__super::Initialize(pPartObjDesc, pArg.get());
	
	/*Shader Info*/
	m_eShaderType = ESHADER_TYPE::ST_ANIM;
	m_iShaderPass = 2;
	
	m_pParentModelCom = _pReferenceModel;

	AddEquipmentModel(m_ePartsKey);

	return S_OK;
}

void CEquipment::PriorityTick(_float _fTimeDelta)
{
}

void CEquipment::Tick(_float _fTimeDelta)
{
	if (nullptr == m_pCurrentModel)
		return;

	m_pCurrentModel->PlayReferenceAnimation();
}

void CEquipment::LateTick(_float _fTimeDelta)
{
	if (nullptr == m_pCurrentModel)
		return;

	if (m_bDslv)
		UpdateDslv(_fTimeDelta);

	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this());
	GAMEINSTANCE->AddCascadeGroup(0, shared_from_this());

	//if (m_IsEdgeRender)
		//GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_EDGE, shared_from_this());
}

HRESULT CEquipment::Render()
{
	if (nullptr == m_pCurrentModel)
		return S_OK;

	GAMEINSTANCE->ReadyShader(m_eShaderType, m_iShaderPass);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pParentTransform->GetWorldMatrix());

	if (m_bDslv)
	{
		if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &m_bDslv, sizeof(_bool))))
			return E_FAIL;

		_float g_fDslvValue = m_fDslvTime / m_fMaxDslvTime;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_fDslvValue", &g_fDslvValue, sizeof(_float))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_fDslvThick", &m_fDslvThick, sizeof(_float))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindRawValue("g_vDslvColor", &m_vDslvColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(GAMEINSTANCE->BindSRV("g_DslvTexture", m_strDslvTexKey)))
			return E_FAIL;
	}

	_uint iNumMeshes = m_pCurrentModel->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++) {
		GAMEINSTANCE->BeginAnimModel(m_pCurrentModel, i);
	}

	_bool bDslvFalse = false;

	if (FAILED(GAMEINSTANCE->BindRawValue("g_bDslv", &bDslvFalse, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEquipment::RenderEdgeDepth()
{
	if (nullptr == m_pCurrentModel)
		return S_OK;

	GAMEINSTANCE->ReadyShader(m_eShaderType, 5);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pParentTransform->GetWorldMatrix());

	_uint iNumMeshes = m_pCurrentModel->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++) {
		GAMEINSTANCE->BeginAnimModel(m_pCurrentModel, i);
	}

	return S_OK;
}

HRESULT CEquipment::RenderEdge()
{
	if (nullptr == m_pCurrentModel)
		return S_OK;

	GAMEINSTANCE->ReadyShader(m_eShaderType, 7);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pParentTransform->GetWorldMatrix());

	_uint iNumMeshes = m_pCurrentModel->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++) {
		GAMEINSTANCE->BeginAnimModel(m_pCurrentModel, i);
	}

	return S_OK;
}

HRESULT CEquipment::RenderShadow()
{
	GAMEINSTANCE->ReadyShader(m_eShaderType, 3);
	GAMEINSTANCE->BindWVPMatrixPerspective(m_pParentTransform->GetWorldMatrix());

	GAMEINSTANCE->BindLightProjMatrix();

	_uint iNumMeshes = m_pCurrentModel->GetNumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++) {
		GAMEINSTANCE->BeginAnimModel(m_pCurrentModel, i);
	}
	

	return S_OK;
}

void CEquipment::AddEquipmentModel(ITEM_EQUIP_TYPE _ePartsKey)
{
	if (_ePartsKey == ITEM_EQUIP_HELMET)
	{
		shared_ptr<CModel>		pHelmetPlate1 = { nullptr };
		pHelmetPlate1 = GAMEINSTANCE->GetModel("Helmet_Plate_1");
		pHelmetPlate1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Helmet_Plate_1", pHelmetPlate1);
		//m_pCurrentModel = pHelmetPlate1;
		
		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Helmet_Plate_1";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_HELMET;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_set_PL_M_HM_00019";
			itemDesc.strModelTag = "Helmet_Plate_1";
			shared_ptr<CItemEquip> Parts_Helmet = CItemEquip::Create(itemDesc);
			if (Parts_Helmet == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Parts_Helmet);
			//Parts_Helmet0->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pHelmetPlate3 = { nullptr };
		pHelmetPlate3 = GAMEINSTANCE->GetModel("Helmet_Plate_3");
		pHelmetPlate3->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Helmet_Plate_3", pHelmetPlate3);
		//m_pCurrentModel = pHelmetPlate3;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Helmet_Plate_3";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_HELMET;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_PL_M_HM_00016";
			itemDesc.strModelTag = "Helmet_Plate_3";
			shared_ptr<CItemEquip> Parts_Helmet = CItemEquip::Create(itemDesc);
			if (Parts_Helmet == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Parts_Helmet);
			//Parts_Helmet0->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pHelmetLeather2 = { nullptr };
		pHelmetLeather2 = GAMEINSTANCE->GetModel("Helmet_Leather_2");
		pHelmetLeather2->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Helmet_Leather_2", pHelmetLeather2);
		//m_pCurrentModel = pHelmetLeather2;

		{
			//CItemBase::ItemDesc itemDesc2{};
			//itemDesc2.strItemTag = "Helmet_Fabric_1";
			//itemDesc2.eItemType = ITEM_EQUIP;
			//itemDesc2.eItemEquipType = ITEM_EQUIP_HELMET;
			//itemDesc2.numItem = 1;
			//itemDesc2.strTexTag = "P_Set_FA_M_HM_00014";
			//itemDesc2.strModelTag = "Helmet_Fabric_1";
			//shared_ptr<CItemEquip> Parts_Helmet2 = CItemEquip::Create(itemDesc2);
			//if (Parts_Helmet2 == nullptr)
			//	return;

			////ITEMMGR->AddItemToInventory(Parts_Helmet2);
			////Parts_Helmet2->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pHelmetFabric1 = { nullptr };
		pHelmetFabric1 = GAMEINSTANCE->GetModel("Helmet_Fabric_1");
		pHelmetFabric1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Helmet_Fabric_1", pHelmetFabric1);
		//m_pCurrentModel = pHelmetFabric1;
		
		{
			CItemBase::ItemDesc itemDesc2{};
			itemDesc2.strItemTag = "Helmet_Fabric_1";
			itemDesc2.eItemType = ITEM_EQUIP;
			itemDesc2.eItemEquipType = ITEM_EQUIP_HELMET;
			itemDesc2.numItem = 1;
			itemDesc2.strTexTag = "P_Set_FA_M_HM_00014";
			itemDesc2.strModelTag = "Helmet_Fabric_1";
			shared_ptr<CItemEquip> Parts_Helmet2 = CItemEquip::Create(itemDesc2);
			if (Parts_Helmet2 == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Parts_Helmet2);
			//Parts_Helmet2->SetIsEquipped(true);
		}
	}

	else if (_ePartsKey == ITEM_EQUIP_UPPER)
	{
		shared_ptr<CModel>		pUpperBasic = { nullptr };
		pUpperBasic = GAMEINSTANCE->GetModel("Upper_Basic");
		pUpperBasic->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Upper_Basic", pUpperBasic);
		m_pCurrentModel = pUpperBasic;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "ÆÄ¼ö²Û ÀüÅõº¹";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_UPPER;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Part_LE_M_TS_00001";
			itemDesc.strModelTag = "Upper_Basic";
			shared_ptr<CItemEquip> Parts_Upper0 = CItemEquip::Create(itemDesc);
			if (Parts_Upper0 == nullptr)
				return;

			ITEMMGR->AddItemToInventory(Parts_Upper0);
			Parts_Upper0->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pUpperPlate1 = { nullptr };
		pUpperPlate1 = GAMEINSTANCE->GetModel("Upper_Plate_1");
		pUpperPlate1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Upper_Plate_1", pUpperPlate1);
		//m_pCurrentModel = pUpperPlate1;
		
		CItemBase::ItemDesc itemDesc{};
		itemDesc.strItemTag = "Upper_Plate_1";
		itemDesc.eItemType = ITEM_EQUIP;
		itemDesc.eItemEquipType = ITEM_EQUIP_UPPER;
		itemDesc.numItem = 1;
		itemDesc.strTexTag = "P_set_PL_M_TS_00019";
		itemDesc.strModelTag = "Upper_Plate_1";
		shared_ptr<CItemEquip> Parts_Upper0 = CItemEquip::Create(itemDesc);
		if (Parts_Upper0 == nullptr)
			return;
		
		//ITEMMGR->AddItemToInventory(Parts_Upper0);
		//Parts_Upper0->SetIsEquipped(true);
		
		shared_ptr<CModel>		pUpperPlate2 = { nullptr };
		pUpperPlate2 = GAMEINSTANCE->GetModel("Upper_Plate_2");
		pUpperPlate2->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Upper_Plate_2", pUpperPlate2);
		//m_pCurrentModel = pUpperPlate2;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Upper_Plate_2";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_UPPER;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_PL_M_TS_00017";
			itemDesc.strModelTag = "Upper_Plate_2";
			shared_ptr<CItemEquip> Parts_Upper0 = CItemEquip::Create(itemDesc);
			if (Parts_Upper0 == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Parts_Upper0);
			//Parts_Upper0->SetIsEquipped(true);
		}
		
		shared_ptr<CModel>		pUpperPlate3 = { nullptr };
		pUpperPlate3 = GAMEINSTANCE->GetModel("Upper_Plate_3");
		pUpperPlate3->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Upper_Plate_3", pUpperPlate3);
		//m_pCurrentModel = pUpperPlate3;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Upper_Plate_3";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_UPPER;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_PL_M_TS_00016";
			itemDesc.strModelTag = "Upper_Plate_3";
			shared_ptr<CItemEquip> Parts_Upper0 = CItemEquip::Create(itemDesc);
			if (Parts_Upper0 == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Parts_Upper0);
			//Parts_Upper0->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pUpperLeather1 = { nullptr };
		pUpperLeather1 = GAMEINSTANCE->GetModel("Upper_Leather_1");
		pUpperLeather1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Upper_Leather_1", pUpperLeather1);
		//m_pCurrentModel = pUpperLeather1;

		/* Temp Code For Initial Item */
		CItemBase::ItemDesc itemDesc1{};
		itemDesc1.strItemTag = "Upper_Leather_1";
		itemDesc1.eItemType = ITEM_EQUIP;
		itemDesc1.eItemEquipType = ITEM_EQUIP_UPPER;
		itemDesc1.numItem = 1;
		itemDesc1.strTexTag = "P_Set_LE_M_TS_05003";
		itemDesc1.strModelTag = "Upper_Leather_1";
		shared_ptr<CItemEquip> Parts_Upper1 = CItemEquip::Create(itemDesc1);
		if (Parts_Upper1 == nullptr)
			return;

		//ITEMMGR->AddItemToInventory(Parts_Upper1);
		//Parts_Upper1->SetIsEquipped(true);

		shared_ptr<CModel>		pUpperLeather2 = { nullptr };
		pUpperLeather2 = GAMEINSTANCE->GetModel("Upper_Leather_2");
		pUpperLeather2->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Upper_Leather_2", pUpperLeather2);
		//m_pCurrentModel = pUpperLeather2;

		///* Temp Code For Initial Item */
		//CItemBase::ItemDesc itemDesc1{};
		//itemDesc1.strItemTag = "Upper_Leather_1";
		//itemDesc1.eItemType = ITEM_EQUIP;
		//itemDesc1.eItemEquipType = ITEM_EQUIP_UPPER;
		//itemDesc1.numItem = 1;
		//itemDesc1.strTexTag = "P_Set_LE_M_TS_05003";
		//itemDesc1.strModelTag = "Upper_Leather_1";
		//shared_ptr<CItemEquip> Parts_Upper1 = CItemEquip::Create(itemDesc1);
		//if (Parts_Upper1 == nullptr)
		//	return;

		//ITEMMGR->AddItemToInventory(Parts_Upper1);
		//Parts_Upper1->SetIsEquipped(true);

		
		shared_ptr<CModel>		pUpperFabric1 = { nullptr };
		pUpperFabric1 = GAMEINSTANCE->GetModel("Upper_Fabric_1");
		pUpperFabric1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Upper_Fabric_1", pUpperFabric1);
		//m_pCurrentModel = pUpperFabric1;

		CItemBase::ItemDesc itemDesc2{};
		itemDesc2.strItemTag = "Upper_Fabric_1";
		itemDesc2.eItemType = ITEM_EQUIP;
		itemDesc2.eItemEquipType = ITEM_EQUIP_UPPER;
		itemDesc2.numItem = 1;
		itemDesc2.strTexTag = "P_Set_FA_M_TS_00014";
		itemDesc2.strModelTag = "Upper_Fabric_1";
		shared_ptr<CItemEquip> Parts_Upper2 = CItemEquip::Create(itemDesc2);
		if (Parts_Upper2 == nullptr)
			return;

		//ITEMMGR->AddItemToInventory(Parts_Upper2);
		//Parts_Upper2->SetIsEquipped(true);
	}

	else if (_ePartsKey == ITEM_EQUIP_LOWER)
	{
		shared_ptr<CModel>		pLowerBasic = { nullptr };
		pLowerBasic = GAMEINSTANCE->GetModel("Lower_Basic");
		pLowerBasic->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Lower_Basic", pLowerBasic);
		m_pCurrentModel = pLowerBasic;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "ÆÄ¼ö²Û °¡Á×¹ÙÁö";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Part_LE_M_PT_00001";
			itemDesc.strModelTag = "Lower_Basic";
			shared_ptr<CItemEquip> Parts_Lower = CItemEquip::Create(itemDesc);
			if (Parts_Lower == nullptr)
				return;

			ITEMMGR->AddItemToInventory(Parts_Lower);
			Parts_Lower->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pLowerPlate1 = { nullptr };
		pLowerPlate1 = GAMEINSTANCE->GetModel("Lower_Plate_1");
		pLowerPlate1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Lower_Plate_1", pLowerPlate1);
		//m_pCurrentModel = pLowerPlate1;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Lower_Plate_1";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_set_PL_M_PT_00019";
			itemDesc.strModelTag = "Lower_Plate_1";
			shared_ptr<CItemEquip> Parts_Lower = CItemEquip::Create(itemDesc);
			if (Parts_Lower == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Parts_Lower);
			//Parts_Lower->SetIsEquipped(true);
		}

		//m_pCurrentModel = pLowerPlate1;

		shared_ptr<CModel>		pLowerPlate2 = { nullptr };
		pLowerPlate2 = GAMEINSTANCE->GetModel("Lower_Plate_2");
		pLowerPlate2->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Lower_Plate_2", pLowerPlate2);
		//m_pCurrentModel = pLowerPlate2;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Lower_Plate_2";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_PL_M_PT_00017";
			itemDesc.strModelTag = "Lower_Plate_2";
			shared_ptr<CItemEquip> Parts_Lower = CItemEquip::Create(itemDesc);
			if (Parts_Lower == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Parts_Lower);
			//Parts_Lower->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pLowerPlate3 = { nullptr };
		pLowerPlate3 = GAMEINSTANCE->GetModel("Lower_Plate_3");
		pLowerPlate3->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Lower_Plate_3", pLowerPlate3);
		//m_pCurrentModel = pLowerPlate3;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Lower_Plate_3";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_PL_M_PT_00016";
			itemDesc.strModelTag = "Lower_Plate_3";
			shared_ptr<CItemEquip> Parts_Lower = CItemEquip::Create(itemDesc);
			if (Parts_Lower == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Parts_Lower);
			//Parts_Lower->SetIsEquipped(true);
		}
		
		shared_ptr<CModel>		pLowerLeather1 = { nullptr };
		pLowerLeather1 = GAMEINSTANCE->GetModel("Lower_Leather_1");
		pLowerLeather1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Lower_Leather_1", pLowerLeather1);
		//m_pCurrentModel = pLowerLeather1;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Lower_Leather_1";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_LE_M_PT_05003";
			itemDesc.strModelTag = "Lower_Leather_1";
			shared_ptr<CItemEquip> Parts_Lower = CItemEquip::Create(itemDesc);
			if (Parts_Lower == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Parts_Lower);
			//Parts_Lower->SetIsEquipped(true);
		}


		shared_ptr<CModel>		pLowerLeather2 = { nullptr };
		pLowerLeather2 = GAMEINSTANCE->GetModel("Lower_Leather_2");
		pLowerLeather2->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Lower_Leather_2", pLowerLeather2);
		//m_pCurrentModel = pLowerLeather2;

		{
			//CItemBase::ItemDesc itemDesc{};
			//itemDesc.strItemTag = "Lower_Leather_1";
			//itemDesc.eItemType = ITEM_EQUIP;
			//itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
			//itemDesc.numItem = 1;
			//itemDesc.strTexTag = "P_Set_LE_M_PT_05003";
			//itemDesc.strModelTag = "Lower_Leather_1";
			//shared_ptr<CItemEquip> Parts_Lower = CItemEquip::Create(itemDesc);
			//if (Parts_Lower == nullptr)
			//	return;

			////ITEMMGR->AddItemToInventory(Parts_Lower);
			////Parts_Lower->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pLowerFabric1 = { nullptr };
		pLowerFabric1 = GAMEINSTANCE->GetModel("Lower_Fabric_1");
		pLowerFabric1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Lower_Fabric_1", pLowerFabric1);
		//m_pCurrentModel = pLowerFabric1;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Lower_Fabric_1";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_LOWER;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_FA_M_PT_00014";
			itemDesc.strModelTag = "Lower_Fabric_1";
			shared_ptr<CItemEquip> Parts_Lower = CItemEquip::Create(itemDesc);
			if (Parts_Lower == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Parts_Lower);
			//Parts_Lower->SetIsEquipped(true);
		}
	}

	else if (_ePartsKey == ITEM_EQUIP_CAPE)
	{
		shared_ptr<CModel>		pCapePlate1 = { nullptr };
		pCapePlate1 = GAMEINSTANCE->GetModel("Cape_Plate_1");
		pCapePlate1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Cape_Plate_1", pCapePlate1);
		//m_pCurrentModel = pCapePlate1;
		
		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Cape_Plate_1";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_CAPE;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_M_CA_00019";
			itemDesc.strModelTag = "Cape_Plate_1";
			shared_ptr<CItemEquip> Parts_Lower = CItemEquip::Create(itemDesc);
			if (Parts_Lower == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Parts_Lower);
			//Parts_Lower->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pCapeLeather1 = { nullptr };
		pCapeLeather1 = GAMEINSTANCE->GetModel("Cape_Leather_1");
		pCapeLeather1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Cape_Leather_1", pCapeLeather1);
		//m_pCurrentModel = pCapeLeather1;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Cape_Leather_1";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_CAPE;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_CA_00001";
			itemDesc.strModelTag = "Cape_Leather_1";
			shared_ptr<CItemEquip> Parts_Lower = CItemEquip::Create(itemDesc);
			if (Parts_Lower == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Parts_Lower);
			//Parts_Lower->SetIsEquipped(true);
		}
	}

	else if (_ePartsKey == ITEM_EQUIP_GLOVE)
	{
		shared_ptr<CModel>		pGloveBasic = { nullptr };
		pGloveBasic = GAMEINSTANCE->GetModel("Glove_Basic");
		pGloveBasic->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Glove_Basic", pGloveBasic);
		m_pCurrentModel = pGloveBasic;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "ÆÄ¼ö²Û °¡Á×¹ÙÁö";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Part_LE_M_GL_00001";
			itemDesc.strModelTag = "Glove_Basic";
			shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			if (Part == nullptr)
				return;

			ITEMMGR->AddItemToInventory(Part);
			Part->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pGlovePlate1 = { nullptr };
		pGlovePlate1 = GAMEINSTANCE->GetModel("Glove_Plate_1");
		pGlovePlate1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Glove_Plate_1", pGlovePlate1);
		//m_pCurrentModel = pGlovePlate1;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Glove_Plate_1";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_set_PL_M_GL_00019";
			itemDesc.strModelTag = "Glove_Plate_1";
			shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			if (Part == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Part);
			//Part->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pGlovePlate2 = { nullptr };
		pGlovePlate2 = GAMEINSTANCE->GetModel("Glove_Plate_2");
		pGlovePlate2->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Glove_Plate_2", pGlovePlate2);
		//m_pCurrentModel = pGlovePlate2;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Glove_Plate_2";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_PL_M_GL_00017";
			itemDesc.strModelTag = "Glove_Plate_2";
			shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			if (Part == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Part);
			//Part->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pGlovePlate3 = { nullptr };
		pGlovePlate3 = GAMEINSTANCE->GetModel("Glove_Plate_3");
		pGlovePlate3->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Glove_Plate_3", pGlovePlate3);
		//m_pCurrentModel = pGlovePlate3;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Glove_Plate_3";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_PL_M_GL_00018";
			itemDesc.strModelTag = "Glove_Plate_3";
			shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			if (Part == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Part);
			//Part->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pGloveLeather1 = { nullptr };
		pGloveLeather1 = GAMEINSTANCE->GetModel("Glove_Leather_1");
		pGloveLeather1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Glove_Leather_1", pGloveLeather1);
		//m_pCurrentModel = pGloveLeather1;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Glove_Leather_1";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_LE_M_GL_05003";
			itemDesc.strModelTag = "Glove_Leather_1";
			shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			if (Part == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Part);
			//Part->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pGloveLeather2 = { nullptr };
		pGloveLeather2 = GAMEINSTANCE->GetModel("Glove_Leather_2");
		pGloveLeather2->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Glove_Leather_2", pGloveLeather2);
		//m_pCurrentModel = pGloveLeather2;

		{
			//CItemBase::ItemDesc itemDesc{};
			//itemDesc.strItemTag = "Glove_Leather_1";
			//itemDesc.eItemType = ITEM_EQUIP;
			//itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
			//itemDesc.numItem = 1;
			//itemDesc.strTexTag = "P_Set_LE_M_GL_05003";
			//itemDesc.strModelTag = "Glove_Leather_1";
			//shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			//if (Part == nullptr)
			//	return;

			////ITEMMGR->AddItemToInventory(Part);
			////Part->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pGloveFabric1= { nullptr };
		pGloveFabric1 = GAMEINSTANCE->GetModel("Glove_Fabric_1");
		pGloveFabric1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Glove_Fabric_1", pGloveFabric1);
		//m_pCurrentModel = pGloveFabric1;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Glove_Fabric_1";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_GLOVE;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_FA_M_GL_00014";
			itemDesc.strModelTag = "Glove_Fabric_1";
			shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			if (Part == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Part);
			//Part->SetIsEquipped(true);
		}
	}

	else if (_ePartsKey == ITEM_EQUIP_BOOTS)
	{
		shared_ptr<CModel>		pBootsBasic = { nullptr };
		pBootsBasic = GAMEINSTANCE->GetModel("Boots_Basic");
		pBootsBasic->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Boots_Basic", pBootsBasic);
		m_pCurrentModel = pBootsBasic;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "ÆÄ¼ö²Û °¡Á×ÀåÈ­";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Part_LE_M_BT_00001";
			itemDesc.strModelTag = "Boots_Basic";
			shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			if (Part == nullptr)
				return;

			ITEMMGR->AddItemToInventory(Part);
			Part->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pBootsPlate1 = { nullptr };
		pBootsPlate1 = GAMEINSTANCE->GetModel("Boots_Plate_1");
		pBootsPlate1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Boots_Plate_1", pBootsPlate1);
		//m_pCurrentModel = pBootsPlate1;
		
		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Boots_Plate_1";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_set_PL_M_BT_00019";
			itemDesc.strModelTag = "Boots_Plate_1";
			shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			if (Part == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Part);
			//Part->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pBootsPlate2 = { nullptr };
		pBootsPlate2 = GAMEINSTANCE->GetModel("Boots_Plate_2");
		pBootsPlate2->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Boots_Plate_2", pBootsPlate2);
		//m_pCurrentModel = pBootsPlate2;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Boots_Plate_2";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_PL_M_BT_00017";
			itemDesc.strModelTag = "Boots_Plate_2";
			shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			if (Part == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Part);
			//Part->SetIsEquipped(true);
		}
		
		shared_ptr<CModel>		pBootsPlate3 = { nullptr };
		pBootsPlate3 = GAMEINSTANCE->GetModel("Boots_Plate_3");
		pBootsPlate3->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Boots_Plate_3", pBootsPlate3);
		//m_pCurrentModel = pBootsPlate3;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Boots_Plate_3";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_PL_M_BT_00016";
			itemDesc.strModelTag = "Boots_Plate_3";
			shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			if (Part == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Part);
			//Part->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pBootsLeather1 = { nullptr };
		pBootsLeather1 = GAMEINSTANCE->GetModel("Boots_Leather_1");
		pBootsLeather1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Boots_Leather_1", pBootsLeather1);
		//m_pCurrentModel = pBootsLeather1;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Boots_Leather_1";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_LE_M_BT_05003";
			itemDesc.strModelTag = "Boots_Leather_1";
			shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			if (Part == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Part);
			//Part->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pBootsLeather2 = { nullptr };
		pBootsLeather2 = GAMEINSTANCE->GetModel("Boots_Leather_2");
		pBootsLeather2->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Boots_Leather_2", pBootsLeather2);
		//m_pCurrentModel = pBootsLeather2;

		{
			//CItemBase::ItemDesc itemDesc{};
			//itemDesc.strItemTag = "Boots_Leather_1";
			//itemDesc.eItemType = ITEM_EQUIP;
			//itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
			//itemDesc.numItem = 1;
			//itemDesc.strTexTag = "P_Set_LE_M_BT_05003";
			//itemDesc.strModelTag = "Boots_Leather_1";
			//shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			//if (Part == nullptr)
			//	return;

			////ITEMMGR->AddItemToInventory(Part);
			////Part->SetIsEquipped(true);
		}

		shared_ptr<CModel>		pBootsFabric1= { nullptr };
		pBootsFabric1 = GAMEINSTANCE->GetModel("Boots_Fabric_1");
		pBootsFabric1->ReadyReferenceBones(m_pParentModelCom.lock());
		m_Models.emplace("Boots_Fabric_1", pBootsFabric1);
		//m_pCurrentModel = pBootsFabric1;

		{
			CItemBase::ItemDesc itemDesc{};
			itemDesc.strItemTag = "Boots_Fabric_1";
			itemDesc.eItemType = ITEM_EQUIP;
			itemDesc.eItemEquipType = ITEM_EQUIP_BOOTS;
			itemDesc.numItem = 1;
			itemDesc.strTexTag = "P_Set_FA_M_BT_00014";
			itemDesc.strModelTag = "Boots_Fabric_1";
			shared_ptr<CItemEquip> Part = CItemEquip::Create(itemDesc);
			if (Part == nullptr)
				return;

			//ITEMMGR->AddItemToInventory(Part);
			//Part->SetIsEquipped(true);
		}
	}

	AddComponent(L"Com_Model", m_pCurrentModel);
}

void CEquipment::SetCurrentModel(string _strModelKey)
{
	m_pCurrentModel = m_Models.find(_strModelKey)->second;

	m_Components[L"Com_Model"] = m_pCurrentModel;
}

void CEquipment::SetNoneModel()
{
	m_pCurrentModel = nullptr;
}

shared_ptr<CEquipment> CEquipment::Create(shared_ptr<EQUIPMENT_DESC> _pEquipmentDesc, shared_ptr<CModel> _pReferenceModel)
{
	shared_ptr<CEquipment> pInstance = make_shared<CEquipment>();

	if (FAILED(pInstance->Initialize(_pEquipmentDesc, _pReferenceModel)))
		MSG_BOX("Failed to Create : CEquipment");

	return pInstance;
}
