#pragma once
#include "Client_Defines.h"
#include "Wolf.h"

BEGIN(Engine)
class CCamera;
END

BEGIN(Client)

class CCharacterMgr
{
	DECLARE_SINGLETON(CCharacterMgr)

public:
	CCharacterMgr();
	~CCharacterMgr();

public:
	void AddPolymorph(const string& _strPolymorphTag, shared_ptr<CPolymorph> _pPolymorph);
	void SetCurrentPolymorph(const string& _strPolymorphTag);
	void SetCamera(shared_ptr<CCameraSpherical> _pCameraSpherical);

public:
	HRESULT ReadyLight();
	void SetInDungeon(_bool _bInDungeon);

public:
	weak_ptr<CPolymorph> GetCurPolymorph() { return m_CurrentPolymorph; }

public:
	void	ChangePolymorph(const string& _strPolymorphTag);

/*Light*/
private:
	_uint m_iLightIdxFront = 0;
	_uint m_iLightIdxBack = 0;

private:
	map<const string, shared_ptr<CPolymorph>>	m_Polymorphs;
	weak_ptr<CPolymorph>						m_CurrentPolymorph;
	weak_ptr<CCameraSpherical>					m_pCamera;
};

END