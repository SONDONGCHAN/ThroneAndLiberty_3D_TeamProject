#pragma once
#include "Client_Defines.h"

BEGIN(Engine)
class CCamera;
END


BEGIN(Client)
class CPlayer;

class CGameMgr
{
	DECLARE_SINGLETON(CGameMgr)

public:
	enum EMouse_State {
		STATE_NORMAL,
		STATE_FREEMOUSE,
		STATE_END
	};

public:
	CGameMgr();
	~CGameMgr();

public:
	void	Tick(_float _fTimeDelta);

public:
	EMouse_State GetMouseState() { return m_eMouseState; }

private:
	void	KeyInput(_float _fTimeDelta);


public:
	void ChangeToNormalMouse();
	void ChangeToFreeMouse(_uint _iPlayerLockOption); // [0==플레이어 이동만가능] [1==플레이어 모든 행동 잠굼]

public:
	void BindPlayer(shared_ptr<CPlayer> _pPlayer);

private:
	EMouse_State m_eMouseState = { STATE_NORMAL };
	weak_ptr<CPlayer> m_pPlayer;

};

END