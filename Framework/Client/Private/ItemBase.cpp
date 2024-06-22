#include "ItemBase.h"

#include "GameInstance.h"
#include "VIInstanceMesh.h"

CItemBase::CItemBase()
{

}

CItemBase::~CItemBase()
{

}

HRESULT CItemBase::Initialize()
{
	CTransform::TRANSFORM_DESC pGameObjectDesc;

	pGameObjectDesc.fSpeedPerSec = 10.f;
	pGameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(Super::Initialize(&pGameObjectDesc)))
		return E_FAIL;

	return S_OK;
}

void CItemBase::PriorityTick(_float _deltaTime)
{

}

void CItemBase::Tick(_float _deltaTime)
{

}

void CItemBase::LateTick(_float _deltaTime)
{

}

HRESULT CItemBase::Render()
{
	return S_OK;
}

_int CItemBase::IncreaseNumItem(_uint _iIncNum)
{
	_uint increasedNum = m_ItemDesc.numItem + _iIncNum;
	if (increasedNum > 999)
	{
		_uint numGap = increasedNum - 999;

		m_ItemDesc.numItem = 999;

		return numGap;
	}

	else if (increasedNum <= 999)
	{
		m_ItemDesc.numItem = increasedNum;

		return 0;
	}

	return -1;
}

_int CItemBase::DecreaseNumItem(_uint _iDecNum)
{
	_int decreasedNum = m_ItemDesc.numItem - _iDecNum;
	if (decreasedNum == 0)
	{
		m_ItemDesc.numItem = 0;

		return decreasedNum; // »èÁ¦
	}

	else if (decreasedNum < 0)
	{
		return decreasedNum;
	}

	else if (decreasedNum > 0)
	{
		m_ItemDesc.numItem = decreasedNum;

		return 1;
	}
}
