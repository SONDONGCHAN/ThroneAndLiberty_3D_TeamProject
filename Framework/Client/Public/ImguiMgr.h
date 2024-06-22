#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CImguiMgr
{
public:
	CImguiMgr();
	~CImguiMgr();

public:
	HRESULT Initialize();
	void	Tick(_float _fTimeDelta);
	HRESULT	Render();

/*Imgui Window Setting Funtion*/
//#ifdef _DEBUG
public:
	void RenderMainWindow();
	void ShaderOptionWindow();
	void CamShakeWindow();
	HRESULT SaveShakeData();
	HRESULT LoadShakeData();
//#endif
	void KeyInput();

private:
	_float m_fDeltaTime = 0.f;

	_bool m_IsTargetVisible = false;
	/*·»´õ Å¸°Ù Å¸ÀÔ ÀÎµ¦½º*/
	_int m_iTargetNum = 0;

	_bool m_IsColliderVisible = false;


	/*Ä«¸Þ¶ó Å¸ÀÔ ÀÎµ¦½º*/
	_int m_iCamIndex = 1;

	_bool m_IsHDROn = true;


private:
	_bool m_UsingDebugWindow = false;

	_bool m_IsCurrentMouseLock = false;

	_bool m_IsToneMappingOn = false;

	_bool m_IsPBROn = true;

	_bool m_IsAOOn = true;

	_bool m_IsShadowOn = true;

	_bool m_IsDOFOn = true;

	_bool m_IsFogOn = true;

private:
	_float m_fConstrast = 0.601f;
	_float m_fMiddleGray = 0.511f;
	_float m_fSaturate = 0.58f;

	_float m_fAOPower = 6.f; 

	/* Ä«¸Þ¶ó ½¦ÀÌÅ©*/
	string CameraSaveLoadPath = "..\\Bin\\DataFiles\\CameraShakeData\\";
	string m_strFileNameShake;
	bool	m_isRot = false;
	bool	m_isLoc = false;
	bool	m_isFov = false;

	UINT	m_eShakeFlag = SHAKE_NONE;
	float	m_fShakeDuration = 0.0f;
	float	m_fShakeAccTime = 0.0f;
	ESHAKEFUNC m_eShakeFunc[3];

	ROT_DESC m_RotDesc;
	float	m_fRotAmplitude[3]{};
	float   m_fRotFrequency[3]{};
	_float2	m_vRotBlendInOut;
	_float2 m_vRotBlendWeight = { 1.0f,1.0f };
	bool	m_isRotRand = false;

	LOC_DESC m_LocDesc;
	float	m_fLocAmplitude[3]{};
	float	m_fLocFrequency[3]{};
	_float2	m_vLocBlendInOut;
	_float2 m_vLocBlendWeight = { 1.0f,1.0f };
	bool	m_isLocRand = false;

	FOV_DESC m_FovDesc;
	float	m_fFovyAmplitude = 0.0f;
	float	m_fFovyFrequency = 0.0f;
	_float2 m_vFovyBlendInOut;
	_float2 m_vFovyBlendWeight = { 1.0f,1.0f };
	float	m_fShakeFovy = 0.0f;
	/* Ä«¸Þ¶ó ½¦ÀÌÅ©*/

	_float3 m_vMovePoint = _float3();
private:
	void ShutDown();

public:
	static shared_ptr<CImguiMgr> Create();



};

END