#include "UIBase.h"

#include "GameInstance.h"
#include "VIInstancePoint.h"
#include "VIInstanceUI.h"

CUIBase::CUIBase()
{
}

CUIBase::~CUIBase()
{
}

HRESULT CUIBase::Initialize()
{
	CTransform::TRANSFORM_DESC pGameObjectDesc;

	pGameObjectDesc.fSpeedPerSec = 10.f;
	pGameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(Super::Initialize(&pGameObjectDesc)))
		return E_FAIL;

	return S_OK;
}

void CUIBase::PriorityTick(_float _deltaTime)
{
}

void CUIBase::Tick(_float _deltaTime)
{
}

void CUIBase::LateTick(_float _deltaTime)
{
}

HRESULT CUIBase::Render()
{

	return S_OK;
}

void CUIBase::SetUIPos(_float _fPosX, _float _fPosY)
{
	m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(_fPosX, _fPosY, 0.f));
	m_fX = _fPosX;
	m_fY = _fPosY;
}

void CUIBase::SetBufferSize(_float _fSizeX, _float _fSizeY)
{
	m_pBuffer->SetBufferScale(_fSizeX, _fSizeY);
	m_fSizeX = _fSizeX;
	m_fSizeY = _fSizeY;
}

_bool CUIBase::IsFinished()
{
	if (m_pBuffer != nullptr)
		return m_pBuffer->IsFinished();

	return false;
}

_bool CUIBase::IsMouseOn()
{
	POINT mousePt;
	::GetCursorPos(&mousePt);
	::ScreenToClient(g_hWnd, &mousePt);

	/*POINT mousePoint;
	mousePoint.x = mousePt.x - g_iWinSizeX * 0.5f;
	mousePoint.y = -mousePt.y + g_iWinSizeY * 0.5f;

	RECT rect = { m_fX - m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f };

	if (m_UIDesc.strTag == "QuickMainSlot0")
	{
		m_fTimeAcc += 0.016f;
		if (m_fTimeAcc >= 3.f)
		{
			_float mousePosX = mousePoint.x;
			_float mousePosY = mousePoint.y;

			console("UIMgr Mouse X : %f, Mouse Y : %f \n", mousePosX, mousePosY);
			console("x : %f, y : %f \n", m_fX, m_fY);
			console("Rect %d, %d, %d, %d \n", rect.left, rect.top, rect.right, rect.bottom);

			m_fTimeAcc = 0.f;
		}
	}*/
	
	_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
	_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

	if ((mousePosX >= (m_fX - m_fSizeX * 0.5f)) && (mousePosX <= (m_fX + m_fSizeX * 0.5f)) && (mousePosY >= (m_fY - m_fSizeY * 0.5f) &&
		mousePosY <= m_fY + m_fSizeY * 0.5f))
		return true;

	return false;
}

_int CUIBase::IsInstanceMouseOn()
{
	POINT mousePt;
	::GetCursorPos(&mousePt);
	::ScreenToClient(g_hWnd, &mousePt);

	_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
	_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

	_uint maxTexNum = static_cast<_uint>(m_vUIPos.size());
	for (_uint i = 0; i < maxTexNum; ++i)
	{
		if ((mousePosX >= (m_vUIPos[i].x - m_vUISize[i].x * 0.5f)) && (mousePosX <= (m_vUIPos[i].x + m_vUISize[i].x * 0.5f)) &&
			(mousePosY >= (m_vUIPos[i].y - m_vUISize[i].y * 0.5f) && mousePosY <= m_vUIPos[i].y + m_vUISize[i].y * 0.5f))
			return i;
	}

	return -1;
}

_bool CUIBase::IsInstanceMouseOn(_uint _iIndex)
{
	POINT mousePt;
	::GetCursorPos(&mousePt);
	::ScreenToClient(g_hWnd, &mousePt);

	_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
	_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

	if ((mousePosX >= (m_vUIPos[_iIndex].x - m_vUISize[_iIndex].x * 0.5f)) && (mousePosX <= (m_vUIPos[_iIndex].x + m_vUISize[_iIndex].x * 0.5f)) &&
		(mousePosY >= (m_vUIPos[_iIndex].y - m_vUISize[_iIndex].y * 0.5f) && mousePosY <= m_vUIPos[_iIndex].y + m_vUISize[_iIndex].y * 0.5f))
		return true;

	return false;
}

shared_ptr<CGameObject> CUIBase::GetUIOwner()
{
	if (!m_pOwner.expired())
		return m_pOwner.lock();

	return nullptr;
}

_bool CUIBase::IsChild()
{
	if (!m_pUIParent.expired())
		return true;

	return false;
}

void CUIBase::SetNonResetRender(_bool _isRender)
{
	m_isRender = _isRender;
}

void CUIBase::SetIsRender(_bool _isRender)
{
	m_isRender = _isRender;

	m_fTimeAcc = 0.f;
}

void CUIBase::AddUIChild(const string& _strUITag, shared_ptr<CUIBase> _pUIChild)
{
	auto it = m_pUIChildren.find(_strUITag);
	if (it != m_pUIChildren.end())
		return;

	m_pUIChildren.emplace(_strUITag, _pUIChild);
}

void CUIBase::AddUIParent(shared_ptr<CUIBase> _uiParent)
{
	m_pUIParent = _uiParent;

	//m_vPrevParentPos = _float3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_vPrevParentPos = _float3(_uiParent->GetUIPos().x, _uiParent->GetUIPos().y, 0.f);
}

void CUIBase::RemoveUIChild(const string& _strUIChildTag)
{
	auto it = m_pUIChildren.find(_strUIChildTag);
	if (it == m_pUIChildren.end())
		return;

	it->second.reset();

	m_pUIChildren.erase(it);
}

void CUIBase::RemoveAllUIChild()
{
	for (auto& uiChild : m_pUIChildren)
	{
		if (!uiChild.second.expired())
			uiChild.second.reset();
	}

	m_pUIChildren.clear();
}

void CUIBase::RemoveUIParent()
{
	m_pUIParent.reset();
}

void CUIBase::SaveAllChildren(vector<shared_ptr<CUIBase>>& _outAllChildren)
{
	if (m_pUIChildren.empty())
		return;

	for (auto& pChild : m_pUIChildren)
	{
		if (!pChild.second.expired())
		{
			_outAllChildren.emplace_back(pChild.second.lock());

			pChild.second.lock()->SaveAllChildren(_outAllChildren);
		}
	}
}

void CUIBase::RotateUI(_float _xValue, _float _yValue, _float _zValue)
{
	m_pTransformCom->RotationAll(XMConvertToRadians(_xValue), XMConvertToRadians(_yValue), XMConvertToRadians(_zValue));

	//_float4x4 RotationMatrixX = SimpleMath::Matrix::CreateFromAxisAngle({ 1.f, 0.f, 0.f }, XMConvertToRadians(_xValue));
	//_float4x4 RotationMatrixY = SimpleMath::Matrix::CreateFromAxisAngle({ 0.f, 1.f, 0.f }, XMConvertToRadians(_yValue));
	//_float4x4 RotationMatrixZ = SimpleMath::Matrix::CreateFromAxisAngle({ 0.f, 0.f, 1.f }, XMConvertToRadians(_zValue));
	//
	//_float4x4 RotationAllMat = RotationMatrixX * RotationMatrixY * RotationMatrixZ;
	//
	//m_RotationMat = RotationAllMat;
}

void CUIBase::ActivateUI()
{
	ResetInstanceData();
	m_isActivated = true;
	m_fSelectedZOrder = 0.f;
}

void CUIBase::DeactivateUI()
{
	m_isActivated = false;
	m_isSelected = false;
}

void CUIBase::ResetInstanceData()
{
	//if (m_pBuffer != nullptr)
	//	m_pBuffer->ResetUIAlpha();
	
	//if (m_pInstanceBuffer != nullptr)
	//	m_pInstanceBuffer->ResetUIAlpha();
}

void CUIBase::ConvertMousePosToUV(_float _mouseX, _float _mouseY)
{
	_float mousePosX = _mouseX - g_iWinSizeX * 0.5f;
	_float mousePosY = -_mouseY + g_iWinSizeY * 0.5f;

	if ((mousePosX >= (m_fX - m_fSizeX * 0.5f)) && (mousePosX <= (m_fX + m_fSizeX * 0.5f)) && (mousePosY >= (m_fY - m_fSizeY * 0.5f) &&
		mousePosY <= m_fY + m_fSizeY * 0.5f))
	{
		float newMinX = (m_fX - (m_fSizeX * 0.5f));
		float newMaxX = (m_fX + (m_fSizeX * 0.5f));
		float newMinY = (m_fY - (m_fSizeY * 0.5f));
		float newMaxY = (m_fY + (m_fSizeY * 0.5f));

		m_uvMouse.x = (mousePosX - newMinX) / (fabs(newMinX) + fabs(newMaxX));
		m_uvMouse.y = (-mousePosY + newMaxY) / (fabs(newMinY) + fabs(newMaxY));
	}
}

void CUIBase::PrepareUVs(_float _fX, _float _fY)
{
	for (_uint i = 0; i < static_cast<_uint>(_fY); ++i)
	{
		for (_uint j = 0; j < static_cast<_uint>(_fX); ++j)
		{
			m_UVs.emplace_back(_float4(static_cast<_float>(j) / _fX, static_cast<_float>(i) / _fY, static_cast<_float>(j + 1) / _fX, static_cast<_float>(i + 1) / _fY));
		}
	}
}

void CUIBase::KeyInput()
{
	if (!m_pUIParent.expired())
	{
		if (GAMEINSTANCE->KeyPressing(DIK_LSHIFT) && GAMEINSTANCE->MouseDown(DIM_LB) && IsMouseOn())
		{
			POINT mousePt;
			::GetCursorPos(&mousePt);
			::ScreenToClient(g_hWnd, &mousePt);

			_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
			_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

			m_vDir = _float3(mousePosX, mousePosY, 0.f) - _float3(m_BindWorldMat._31, m_BindWorldMat._32, 0.f);
			m_fDist = m_vDir.Length();
			m_vDir.Normalize();
		}

		if (GAMEINSTANCE->KeyPressing(DIK_LSHIFT) && GAMEINSTANCE->MousePressing(DIM_LB))
		{
			POINT mousePt;
			::GetCursorPos(&mousePt);
			::ScreenToClient(g_hWnd, &mousePt);

			_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
			_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

			_float3 dir = _float3(mousePosX, mousePosY, 0.f) - _float3(m_BindWorldMat._31, m_BindWorldMat._32, 0.f);
			_float dist = dir.Length();
			dir.Normalize();

			if (dist != m_fDist)
			{
				_float3 moveDir = m_vDir * m_fDist;
				_float3 newPos = _float3(mousePosX, mousePosY, 0.f) - moveDir;
				newPos = newPos.Transform(newPos, m_pUIParent.lock()->GetTransform()->GetWorldMatrixInverse());

				//m_vLocalPos = newPos;
			}
		}
	}
}
