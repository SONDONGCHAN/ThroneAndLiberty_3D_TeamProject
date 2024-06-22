#include "GameMgr.h"
#include "CameraSpherical.h"
#include "CameraMgr.h"
#include "Player.h"
#include "UIMgr.h"


IMPLEMENT_SINGLETON(CGameMgr)

CGameMgr::CGameMgr()
{
}

CGameMgr::~CGameMgr()
{
}

void CGameMgr::Tick(_float _fTimeDelta)
{
	KeyInput(_fTimeDelta);
}

void CGameMgr::KeyInput(_float _fTimeDelta)
{
	if (GAMEINSTANCE->KeyDown(DIK_O))
	{
		if (STATE_NORMAL == m_eMouseState)
		{
			CCameraMgr::GetInstance()->MouseLockOff();
			m_eMouseState = STATE_FREEMOUSE;
			//ShowCursor(true);
			UIMGR->RenderOnMouseCursor();
			m_pPlayer.lock()->SetUIInputState(true);
			GAMEINSTANCE->PlaySoundW("UI_Item_Mounting", 0.6f);
		}
		else if (STATE_FREEMOUSE == m_eMouseState)
		{
			CCameraMgr::GetInstance()->MouseLockOn();
			m_eMouseState = STATE_NORMAL;
			//ShowCursor(false);
			UIMGR->RenderOffMouseCursor();
			m_pPlayer.lock()->SetUIInputState(false);
			m_pPlayer.lock()->SetAllInputState(false);
		}
	}
}

void CGameMgr::ChangeToNormalMouse()
{
	if (STATE_NORMAL == m_eMouseState)
		return;
	
	CCameraMgr::GetInstance()->MouseLockOn();
	m_eMouseState = STATE_NORMAL;
	//ShowCursor(false);
	UIMGR->RenderOffMouseCursor();

	m_pPlayer.lock()->SetUIInputState(false);
	m_pPlayer.lock()->SetAllInputState(false);
}

void CGameMgr::ChangeToFreeMouse(_uint _iPlayerLockOption)
{
	if (STATE_FREEMOUSE == m_eMouseState)
		return;

	CCameraMgr::GetInstance()->MouseLockOff();
	m_eMouseState = STATE_FREEMOUSE;
	//ShowCursor(true);
	UIMGR->RenderOnMouseCursor();
	GAMEINSTANCE->PlaySoundW("UI_Item_Mounting", 1.f);

	if(_iPlayerLockOption == 0)
		m_pPlayer.lock()->SetUIInputState(true);
	else if (_iPlayerLockOption == 1)
		m_pPlayer.lock()->SetAllInputState(true);
}

void CGameMgr::BindPlayer(shared_ptr<CPlayer> _pPlayer)
{
	m_pPlayer = _pPlayer;
}
