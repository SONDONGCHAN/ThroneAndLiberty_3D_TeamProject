#pragma once
#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Tool_Map)
class CToolMgr;

class LevelAtelier : public CLevel
{
private:
	LevelAtelier();
	DESTRUCTOR(LevelAtelier)


public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const wstring& _strLayerTag);
	HRESULT Ready_Layer_Stage(const wstring& _strLayerTag);
	//HRESULT Ready_LandObjects();
	//HRESULT Ready_Layer_Player(const wstring& _strLayerTag, const shared_ptr<CTransform> _pTransformStage, const shared_ptr<CNavigation> _pNavigationStage);
	//HRESULT Ready_Layer_Monster(const wstring& _strLayerTag);

#ifdef _DEBUG
private:
	_tchar m_szFPS[MAX_PATH] = TEXT("");
	_uint m_iNumRender{};
	_float m_fTimeAcc{};
#endif // _DEBUG


private:
	shared_ptr<CToolMgr> m_pToolMgr{};

public:
	static shared_ptr<LevelAtelier> Create();
};

END