#include "pch.h"
#include "ToolUIBase.h"

#include "VIInstancePoint.h"

CToolUIBase::CToolUIBase()
{

}

CToolUIBase::~CToolUIBase()
{

}

HRESULT CToolUIBase::Initialize(CTransform::TRANSFORM_DESC* _pTransformDesc)
{
	if (FAILED(Super::Initialize(_pTransformDesc)))
		return E_FAIL;

	return S_OK;
}

void CToolUIBase::PriorityTick(_float _deltaTime)
{
}

void CToolUIBase::Tick(_float _deltaTime)
{
}

void CToolUIBase::LateTick(_float _deltaTime)
{
}

HRESULT CToolUIBase::Render()
{
	return S_OK;
}

void CToolUIBase::SetUIPos(_float _fPosX, _float _fPosY)
{
	m_pTransformCom->SetState(CTransform::STATE_POSITION, _float3(_fPosX, _fPosY, 0.f));
	m_fX = _fPosX;
	m_fY = _fPosY;
}

void CToolUIBase::SetBufferSize(_float _fSizeX, _float _fSizeY)
{
	m_pBuffer->SetBufferScale(_fSizeX, _fSizeY);
	m_fSizeX = _fSizeX;
	m_fSizeY = _fSizeY;
	m_InstanceDesc.vSize = _float2(m_fSizeX, m_fSizeX);
	m_InstanceDesc.fSizeY = m_fSizeY;
}

void CToolUIBase::SetColor(_float4 _vColor)
{
	m_pBuffer->SetColor(_vColor);
	m_InstanceDesc.vColor = _vColor;
}

_bool CToolUIBase::IsFinished()
{
	if (m_pBuffer != nullptr)
		return m_pBuffer->IsFinished();

	return false;
}

_bool CToolUIBase::IsMouseOn()
{
	/*if (!m_pUIParent.expired())
	{
		POINT mousePt;
		::GetCursorPos(&mousePt);
		::ScreenToClient(g_hWnd, &mousePt);

		_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
		_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

		if ((mousePosX >= (m_BindWorldMat._31 - m_fSizeX * 0.5f)) && (mousePosX <= (m_BindWorldMat._31 + m_fSizeX * 0.5f)) &&
			(mousePosY >= (m_BindWorldMat._32 - m_fSizeY * 0.5f)) && (mousePosY <= (m_BindWorldMat._32 + m_fSizeY * 0.5f)))
			return true;

		return false;
	}*/

	POINT mousePt;
	::GetCursorPos(&mousePt);
	::ScreenToClient(g_hWnd, &mousePt);

	_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
	_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

	if ((mousePosX >= (m_fX - m_fSizeX * 0.5f)) && (mousePosX <= (m_fX + m_fSizeX * 0.5f)) && (mousePosY >= (m_fY - m_fSizeY * 0.5f) &&
		mousePosY <= m_fY + m_fSizeY * 0.5f))
		return true;

	return false;
}

shared_ptr<CGameObject> CToolUIBase::GetUIOwner()
{
	if (!m_pOwner.expired())
		return m_pOwner.lock();

	return nullptr;
}

_bool CToolUIBase::IsChild()
{
	if (!m_pUIParent.expired())
		return true;

	return false;
}

shared_ptr<CToolUIBase> CToolUIBase::GetUIParent()
{
	if (m_pUIParent.expired())
		return nullptr;

	return m_pUIParent.lock();
}

void CToolUIBase::SetShaderPassIndex(_uint _passIndex)
{
	m_ToolUIDesc.iShaderPassIdx = _passIndex;
	m_iShaderPass = _passIndex;
}

void CToolUIBase::AddUIChild(const string& _strUITag, shared_ptr<CToolUIBase> _pUIChild)
{
	auto it = m_pUIChildren.find(_strUITag);
	if (it != m_pUIChildren.end())
		return;

	m_pUIChildren.emplace(_strUITag, _pUIChild);
}

void CToolUIBase::AddUIChild(shared_ptr<CToolUIBase> _pUIChild)
{
	shared_ptr<CToolUIBase> pUIChild = _pUIChild;

	auto it = find_if(m_pUIChildrenVec.begin(), m_pUIChildrenVec.end(), [&pUIChild](weak_ptr<CToolUIBase> _pUIChild) {
		if (pUIChild == _pUIChild.lock())
			return true;

		return false;
		});

	if (it != m_pUIChildrenVec.end())
		return;

	m_pUIChildrenVec.emplace_back(pUIChild);
}

void CToolUIBase::AddUIParent(shared_ptr<CToolUIBase> _uiParent)
{
	m_pUIParent = _uiParent;

	//m_vPrevParentPos = _float3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_vPrevParentPos = _float3(_uiParent->GetUIPos().x, _uiParent->GetUIPos().y, 0.f);
}

void CToolUIBase::RemoveUIChild(const string& _strUIChildTag)
{
	auto it = m_pUIChildren.find(_strUIChildTag);
	if (it == m_pUIChildren.end())
		return;

	it->second.reset();

	m_pUIChildren.erase(it);
}

void CToolUIBase::RemoveUIChildrenVec()
{

}

void CToolUIBase::RemoveAllUIChild()
{
	for (auto& uiChild : m_pUIChildren)
	{
		if (!uiChild.second.expired())
			uiChild.second.reset();
	}

	m_pUIChildren.clear();
}

void CToolUIBase::RemoveUIParent()
{
	m_pUIParent.reset();
}

void CToolUIBase::RotateUI(_float _xValue, _float _yValue, _float _zValue)
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

void CToolUIBase::ActivateUI()
{
	ResetInstanceData();
	m_isActivated = true;
	m_fSelectedZOrder = 0.f;
}

void CToolUIBase::DeactivateUI()
{
	m_isActivated = false;
	m_isSelected = false;
}

void CToolUIBase::ResetInstanceData()
{
	if (m_pBuffer != nullptr)
		m_pBuffer->ResetUIAlpha();
}

void CToolUIBase::ConvertMousePosToUV(_float _mouseX, _float _mouseY)
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

void CToolUIBase::PrepareUVs(_float _fX, _float _fY)
{
	for (_uint i = 0; i < static_cast<_uint>(_fY); ++i)
	{
		for (_uint j = 0; j < static_cast<_uint>(_fX); ++j)
		{
			m_UVs.emplace_back(_float4(static_cast<_float>(j) / _fX, static_cast<_float>(i) / _fY, static_cast<_float>(j + 1) / _fX, static_cast<_float>(i + 1) / _fY));
		}
	}
}

void CToolUIBase::KeyInput()
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

	else
	{
		/*if (GAMEINSTANCE->MouseDown(DIM_LB))
		{
			POINT mousePt;
			::GetCursorPos(&mousePt);
			::ScreenToClient(g_hWnd, &mousePt);

			_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
			_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

			m_vDir = _float3(mousePosX, mousePosY, 0.f) - m_pTransformCom->GetState(CTransform::STATE_POSITION);
			m_fDist = m_vDir.Length();
			m_vDir.Normalize();

			return;
		}

		if (GAMEINSTANCE->MousePressing(DIM_LB) && m_isSelected)
		{
			POINT mousePt;
			::GetCursorPos(&mousePt);
			::ScreenToClient(g_hWnd, &mousePt);

			_float mousePosX = mousePt.x - g_iWinSizeX * 0.5f;
			_float mousePosY = -mousePt.y + g_iWinSizeY * 0.5f;

			_float3 dir = _float3(mousePosX, mousePosY, 0.f) - m_pTransformCom->GetState(CTransform::STATE_POSITION);
			_float dist = dir.Length();
			dir.Normalize();

			if (dist != m_fDist)
			{
				_float3 moveDir = m_vDir * m_fDist;

				_float3 newPos = _float3(mousePosX, mousePosY, 0.f) - moveDir;

				SetUIPos(newPos.x, newPos.y);
			}
		}*/
	}

}
