#include "pch.h"
#include "ColliderFrame.h"

CColliderFrame::CColliderFrame()
{
}

CColliderFrame::~CColliderFrame()
{
}

shared_ptr<CColliderFrame> CColliderFrame::Create(shared_ptr<Geometry> _pGeometry, _float3 _vPosition)
{
	shared_ptr<CColliderFrame> pInstance = make_shared<CColliderFrame>();

	if (FAILED(pInstance->Initialize(_pGeometry, _vPosition)))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CColliderFrame::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CColliderFrame::Initialize(shared_ptr<Geometry> _pGeometry, _float3 _vPosition)
{
	if (!_pGeometry)
		return E_FAIL;

	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;

	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);

	m_pGeometry = _pGeometry;

	switch (m_pGeometry->GetType())
	{
	case GT_SPHERE:
		m_vColor = { 1.0f,0.0f,0.0f,1.0f };
		break;
	case GT_BOX:
		m_vColor = { 0.0f,1.0f,0.0f,1.0f };
		break;
	case GT_CAPSULE:
		m_vColor = { 1.0f,1.0f,0.0f,1.0f };
		break;
	}

    return S_OK;
}

void CColliderFrame::PriorityTick(_float _fTimeDelta)
{
}

void CColliderFrame::Tick(_float _fTimeDelta)
{
}

void CColliderFrame::LateTick(_float _fTimeDelta, UINT _iRenderType)
{
	m_iRenderType = _iRenderType;

	GAMEINSTANCE->AddRenderGroup(CRenderer::RENDER_NONLIGHT, shared_from_this());
	/*if (GAMEINSTANCE->isInWorldSpace(m_pTransformCom->GetState(CTransform::STATE_POSITION), 10.0f))
	{
	}*/
}

HRESULT CColliderFrame::Render()
{
	if (m_pGeometry)
	{
		shared_ptr<CGameInstance> pInstance = GAMEINSTANCE;

		_float3 RotVec = m_pGeometry->vOffSetRotation;
		_float4x4 ScaleMat;
		_float4x4 RotMat = XMMatrixRotationRollPitchYaw(XMConvertToRadians(RotVec.x), XMConvertToRadians(RotVec.y), XMConvertToRadians(RotVec.z));
		switch (m_pGeometry->GetType())
		{
		case GT_SPHERE:
		{
			float fRadius = static_pointer_cast<GeometrySphere>(m_pGeometry)->fRadius;
			ScaleMat = XMMatrixScaling(fRadius * 2.0f, fRadius * 2.0f, fRadius * 2.0f);
		}
		break;
		case GT_BOX:
		{
			_float3 vSize = static_pointer_cast<GeometryBox>(m_pGeometry)->vSize;
			ScaleMat = XMMatrixScaling(vSize.x, vSize.y, vSize.z);
		}
		break;
		case GT_CAPSULE:
		{
			float fRadius = static_pointer_cast<GeometryCapsule>(m_pGeometry)->fRadius;
			float fHeight = static_pointer_cast<GeometryCapsule>(m_pGeometry)->fHeight;

			ScaleMat = XMMatrixScaling(2.0f * fRadius + fHeight, 2.0f * fRadius, 2.0f * fRadius);
		}
		break;
		}
		m_WorldMat = XMMatrixMultiply(ScaleMat, RotMat);
		m_WorldMat.Translation(m_pTransformCom->GetState(CTransform::STATE_POSITION));


		pInstance->ReadyShader(ESHADER_TYPE::ST_CUBE, m_iRenderType);
		pInstance->BindWVPMatrixPerspective(m_WorldMat);

		pInstance->BindRawValue("g_RGBColor", &m_vColor, sizeof(_color));

		if (FAILED(pInstance->BeginShader()))
		{
			MessageBox(nullptr, L"Render Failed", L"CColliderFrame::Render", MB_OK);
			return E_FAIL;
		}
	}

    return S_OK;
}

void CColliderFrame::ChangeGeometry(shared_ptr<Geometry> _pGeometry)
{
	if (_pGeometry)
	{
		m_pGeometry = _pGeometry;
		switch (m_pGeometry->GetType())
		{
		case GT_SPHERE:
			m_vColor = { 1.0f,1.0f,0.0f,1.0f };
			break;
		case GT_BOX:
			m_vColor = { 0.0f,1.0f,0.0f,1.0f };
			break;
		case GT_CAPSULE:
			m_vColor = { 0.0f,1.0f,1.0f,1.0f };
			break;
		}
	}
}

void CColliderFrame::ChangePostion(_float3 _vPosition)
{
	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPosition);
}

void CColliderFrame::ChangeOffSetRotation(_float3 _vRotDegree)
{
	if (m_pGeometry)
	{
		m_pGeometry->vOffSetRotation = _vRotDegree;
	}
}

shared_ptr<Geometry> CColliderFrame::GetGeometry()
{
	return m_pGeometry;
}

_float4x4 CColliderFrame::GetWorldMat()
{
	return m_WorldMat;
}