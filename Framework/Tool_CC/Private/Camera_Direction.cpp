#include "pch.h"
#include "Camera_Direction.h"
#include "CameraKeyFrame.h"

CCamera_Direction::CCamera_Direction()
{
}

CCamera_Direction::~CCamera_Direction()
{
}

shared_ptr<CCamera_Direction> CCamera_Direction::Create()
{
    shared_ptr<CCamera_Direction> pInstance = make_shared<CCamera_Direction>();

	if (FAILED(pInstance->Initialize()))
	{
		MessageBox(nullptr, L"Initialize Failed", L"CCamera_Direction::Create", MB_OK);
		pInstance.reset();
		return nullptr;
	}

	return pInstance;
}

HRESULT CCamera_Direction::Initialize()
{
    CAMERA_DESC CamDesc{};
    CamDesc.fAspect = (float)g_iWinSizeX / (float)g_iWinSizeY;
    CamDesc.fFovy = XM_PI * 0.3333333f;

    CTransform::TRANSFORM_DESC TransDesc{};
	TransDesc.fSpeedPerSec = 10.0f;
	TransDesc.fRotationPerSec = XM_PIDIV4;

    if (FAILED(CCamera::Initialize(&CamDesc, &TransDesc)))
    {
        MessageBox(nullptr, L"Initialize Failed", L"CCamera_Direction::Initialize", MB_OK);
        return E_FAIL;
    }

    m_fMouseSensitivity = 0.2f;

    return S_OK;
}

void CCamera_Direction::PriorityTick(float _fTimeDelta)
{
	if (m_isDirectioning)
	{
		DirectionTick(_fTimeDelta);
	}
	else
	{
		KeyInput(_fTimeDelta);
		MouseMove(_fTimeDelta);
	}
	CCamera::PriorityTick(_fTimeDelta);
	if (m_isRecording)
	{
		SetUpTransformMatices();
	}
}

void CCamera_Direction::Tick(float fTimeDelta)
{
}

void CCamera_Direction::LateTick(float fTimeDelta)
{
}

HRESULT CCamera_Direction::Render()
{
    return S_OK;
}

void CCamera_Direction::StartDirection(const vector<shared_ptr<CCameraKeyFrame>>& _KeyFrames)
{
	if (!_KeyFrames.empty())
	{
		m_isRecording = true;
		m_isDirectioning = true;
		m_isFadeComplete = false;
		m_isShakeComplete = false;
		m_iCurrentKey = 0;
		m_fTimeAcc = 0.0f;
		m_DirectionKeyFrame.clear();
		m_DirectionKeyFrame.reserve(_KeyFrames.size());
		for_each(_KeyFrames.begin(), _KeyFrames.end(), [&](shared_ptr<CCameraKeyFrame> KeyFrame)->void {m_DirectionKeyFrame.emplace_back(KeyFrame); });
	}
}

void CCamera_Direction::EndDirection()
{
	m_isRecording = false;
	m_isDirectioning = false;
	m_isFadeComplete = false;
	m_isShakeComplete = false;
	m_iCurrentKey = 0;
	m_fTimeAcc = 0.0f;
	m_DirectionKeyFrame.clear();
	// FadeInOut 초기화
	GAMEINSTANCE->SetFadeEffect(false, 0.7f);
}

void CCamera_Direction::DirectionTick(float _fTimeDelta)
{
	UINT KeySize{ (UINT)m_DirectionKeyFrame.size() };
	if (m_iCurrentKey < KeySize)
	{
		if (m_DirectionKeyFrame[m_iCurrentKey]->GetFade() && !m_isFadeComplete)
		{
			UINT eFade{ 0 };
			float fFadeInOutStartTime{ 0.0f };
			float fFadeInOutDuration{ 0.0f };
			m_DirectionKeyFrame[m_iCurrentKey]->GetFadeInfo(&eFade, &fFadeInOutStartTime, &fFadeInOutDuration);
			if (m_fTimeAcc >= fFadeInOutStartTime)
			{
				GAMEINSTANCE->SetFadeEffect(eFade == FADE_IN ? false : true, fFadeInOutDuration);
				// 여기서 페이드 인아웃 처리
				m_isFadeComplete = true;
			}
		}
		if (m_DirectionKeyFrame[m_iCurrentKey]->GetisCameraShake() && !m_isShakeComplete)
		{
			UINT ShakeFlags{ 0 };
			float fStartTime{ 0.0f };
			float fDuration{ 0.0f };
			ROT_DESC Rot{};
			LOC_DESC Loc{};
			FOV_DESC Fov{};
			m_DirectionKeyFrame[m_iCurrentKey]->GetShakeInfo(&ShakeFlags, &fStartTime, &fDuration, &Rot, &Loc, &Fov);
			if (m_fTimeAcc >= fStartTime)
			{
				CamShake(ShakeFlags, fDuration, &Rot, &Loc, &Fov);
				m_isShakeComplete = true;
			}
		}
		
		float CurrentKeyLerpTime{ m_DirectionKeyFrame[m_iCurrentKey]->GetLerpTime() };	// 다음 KeyFrame까지 걸리는 시간
		UINT iLeftKeyFrames{ KeySize - m_iCurrentKey - 1 };		// 남아 있는 프레임 수
		
		if (m_fTimeAcc < CurrentKeyLerpTime)
		{
			float fRatio{ m_fTimeAcc / CurrentKeyLerpTime };

			if (iLeftKeyFrames >= 2)
			{
				if (m_iCurrentKey > 0)
				{
					CatmullRomFunc(-1, 0, 1, 2, fRatio);
				}
				else
				{
					CatmullRomFunc(0, 0, 1, 2, fRatio);
				}
			}
			else if (iLeftKeyFrames == 1)
			{
				if (m_iCurrentKey > 0)
				{
					CatmullRomFunc(-1, 0, 1, 1, fRatio);
				}
				else
				{
					LinearFunc(fRatio);
				}
			}
			else
			{
				m_pTransformCom->SetState(CTransform::STATE_POSITION, m_DirectionKeyFrame[m_iCurrentKey]->GetTransform()->GetState(CTransform::STATE_POSITION));
				m_pTransformCom->LookAt(m_DirectionKeyFrame[m_iCurrentKey]->GetTransformAt()->GetState(CTransform::STATE_POSITION));
			}
			m_fTimeAcc += _fTimeDelta;
		}
		else
		{
			m_iCurrentKey++;
			m_fTimeAcc = 0.0f;
			m_isFadeComplete = false;
			m_isShakeComplete = false;
			DirectionTick(_fTimeDelta);
		}
	}
	else
	{
		EndDirection();
	}
}

void CCamera_Direction::KeyInput(float _fTimeDelta)
{
	if (m_isRecording)
	{
		shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
		if (pGameInstance->KeyDown(DIK_GRAVE))
		{
			m_IsMouseLock = !m_IsMouseLock;
		}
		if (pGameInstance->KeyPressing(DIK_W))
		{
			m_pTransformCom->GoStraight(_fTimeDelta);
		}
		if (pGameInstance->KeyPressing(DIK_S))
		{
			m_pTransformCom->GoBackward(_fTimeDelta);
		}
		if (pGameInstance->KeyPressing(DIK_A))
		{
			m_pTransformCom->GoLeft(_fTimeDelta);
		}
		if (pGameInstance->KeyPressing(DIK_D))
		{
			m_pTransformCom->GoRight(_fTimeDelta);
		}
	}
}

void CCamera_Direction::MouseMove(float _fTimeDelta)
{
	if (m_isRecording && m_IsMouseLock)
	{
		shared_ptr<CGameInstance> pGameInstance = GAMEINSTANCE;
		long dx = pGameInstance->GetDIMouseMove(DIMS_X);
		long dy = pGameInstance->GetDIMouseMove(DIMS_Y);

		if (dy)
		{
			m_pTransformCom->Turn(m_pTransformCom->GetState(CTransform::STATE_RIGHT), dy * m_fMouseSensitivity * _fTimeDelta);
		}
		if (dx)
		{
			m_pTransformCom->Turn(_float3{ 0.0f,1.0f,0.0f }, dx * m_fMouseSensitivity * _fTimeDelta);
		}

		CursorFixCenter();
	}
}

void CCamera_Direction::CursorFixCenter()
{
	POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CCamera_Direction::CatmullRomFunc(int _i0, int _i1, int _i2, int _i3, float _fRatio)
{
	XMVECTOR PrePos = m_DirectionKeyFrame[m_iCurrentKey + _i0]->GetTransform()->GetState(CTransform::STATE_POSITION);
	XMVECTOR CurPos = m_DirectionKeyFrame[m_iCurrentKey + _i1]->GetTransform()->GetState(CTransform::STATE_POSITION);
	XMVECTOR NextPos = m_DirectionKeyFrame[m_iCurrentKey + _i2]->GetTransform()->GetState(CTransform::STATE_POSITION);
	XMVECTOR NNextPos = m_DirectionKeyFrame[m_iCurrentKey + _i3]->GetTransform()->GetState(CTransform::STATE_POSITION);

	XMVECTOR PreAtPos = m_DirectionKeyFrame[m_iCurrentKey + _i0]->GetTransformAt()->GetState(CTransform::STATE_POSITION);
	XMVECTOR CurAtPos = m_DirectionKeyFrame[m_iCurrentKey + _i1]->GetTransformAt()->GetState(CTransform::STATE_POSITION);
	XMVECTOR NextAtPos = m_DirectionKeyFrame[m_iCurrentKey + _i2]->GetTransformAt()->GetState(CTransform::STATE_POSITION);
	XMVECTOR NNextAtPos = m_DirectionKeyFrame[m_iCurrentKey + _i3]->GetTransformAt()->GetState(CTransform::STATE_POSITION);

	m_pTransformCom->SetState(CTransform::STATE_POSITION, XMVectorCatmullRom(PrePos, CurPos, NextPos, NNextPos, _fRatio));
	m_pTransformCom->LookAt(XMVectorCatmullRom(PreAtPos, CurAtPos, NextAtPos, NNextAtPos, _fRatio));
}

void CCamera_Direction::LinearFunc(float _fRatio)
{
	XMVECTOR CurPos = m_DirectionKeyFrame[m_iCurrentKey]->GetTransform()->GetState(CTransform::STATE_POSITION);
	XMVECTOR NextPos = m_DirectionKeyFrame[m_iCurrentKey + 1]->GetTransform()->GetState(CTransform::STATE_POSITION);

	XMVECTOR CurAtPos = m_DirectionKeyFrame[m_iCurrentKey]->GetTransformAt()->GetState(CTransform::STATE_POSITION);
	XMVECTOR NextAtPos = m_DirectionKeyFrame[m_iCurrentKey + 1]->GetTransformAt()->GetState(CTransform::STATE_POSITION);

	m_pTransformCom->SetState(CTransform::STATE_POSITION, XMVectorLerp(CurPos, NextPos, _fRatio));
	m_pTransformCom->LookAt(XMVectorLerp(CurAtPos, NextAtPos, _fRatio));
}

