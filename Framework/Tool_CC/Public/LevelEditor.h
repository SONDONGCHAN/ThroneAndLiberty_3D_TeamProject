#pragma once

#include "Tool_CC_Define.h"
#include "Camera_Direction.h"
#include "ColliderFrame.h"
#include "Level.h"
#include "Camera.h"
#include "GameInstance.h"

class CCameraKeyFrame;

class CLevelEditor final : public CLevel
{
	enum ECOLTYPE { COL_BOX, COL_CAPSULE, COL_SPHERE, COL_END };
	

public:
	CLevelEditor();
	~CLevelEditor();

public:
	static shared_ptr<CLevelEditor> Create();

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float _fTimeDelta);
	virtual void LateTick(_float _fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Init_Camera();
	HRESULT Init_Map();
	HRESULT Init_TestObj();


private:
	HRESULT ImgZmoUpdate();
	HRESULT Render_Collider_Window(float _fTimeDelta);
	HRESULT Render_Camera_Window(float _fTimeDelta);
	HRESULT RenderLightWindow(float _fTimeDelta);
	HRESULT RenderAnimation(float _fTimeDelta);

	bool ColliderPicking(weak_ptr<CColliderFrame>* _ColOut, _float3* _vColPosOut, int& _pIndex);
	bool CameraPicking(weak_ptr<CCameraKeyFrame>* _CameraOut, _float3* _vCameraPosOut, UINT& _pIndex);

	HRESULT SaveColliderData();
	HRESULT LoadColliderData();
	HRESULT CombineColliderData(int _iA, int _iB);

	HRESULT SaveCameraData();
	HRESULT LoadCameraData();

	HRESULT CameraDirection();

	HRESULT SaveLightData();
	HRESULT LoadLightData();

private:		// F키는 콜라이더 색변경, G는 카메라 키프레임 외곽선 on off
	string ColliderSaveLoadPath = "..\\Bin\\DataFiles\\Collider\\";
	string CameraSaveLoadPath = "..\\Bin\\DataFiles\\Camera\\";
	string m_strFileNameCol;
	string m_strFileNameCam;
	ImGuizmo::MODE m_CurrentGizmoMode = ImGuizmo::WORLD;
	ImGuizmo::OPERATION m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGuizmo::OPERATION m_CurrentGizmoOperation2 = ImGuizmo::TRANSLATE;

	bool show_demo_window = false;
	shared_ptr<CCamera_Direction> m_pFreeCamera = nullptr;
	shared_ptr<CCamera_Direction> m_pDirectionCamera = nullptr;
	shared_ptr<CCamera_Direction> m_pCurrentCamera = nullptr;
	float m_fCamVelocity = 10.0f;

	/* 카메라 연출 */
	vector<pair<UINT, vector<shared_ptr<CCameraKeyFrame>>>> m_CameraKeyFrames;
	int iSelectPath = 0;
	bool m_isRenderCamBox{ true };
	/* 카메라 연출 */

	int m_iPickType = 0;
	weak_ptr<CColliderFrame> m_pPickCol;
	weak_ptr<CCameraKeyFrame> m_pPickCamera;
	

	_float3 m_vPickingPosition;


	/* 콜라이더 */
	int m_iColType = COL_BOX;
	int m_iSelectColLine = 0;
	int m_iSelectColVector = 0;
	vector<shared_ptr<CColliderFrame>> m_Colliders[10];


	_float3 m_vBoxSize = { 1.0f,1.0f,1.0f };
	float m_fCapHeight = 1.0f;
	float m_fCapRadius = 1.0f;
	float m_fSphRadius = 1.0f;
	/* 콜라이더 */

	
	/* 카메라 쉐이크*/
	bool	m_isRot = false;
	bool	m_isLoc = false;
	bool	m_isFov = false;

	UINT	m_eShakeFlag = SHAKE_NONE;
	float	m_fShakeStartTiem{ 0.0f };
	bool	m_isShake = false;
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
	/* 카메라 쉐이크*/


/*Light Information*/
private:
	_float3 m_vLightPosition = { 0.f ,0.f, 0.f };
	_float4 m_vLightDiffuse = { 1.f, 1.f, 1.f, 1.f };
	_float4 m_vLightAmbient = { 1.f, 1.f, 1.f, 1.f };
	_float	m_fLightRange = 0.f;

	_float m_fLightCullRange = 0.f;
	_bool m_IsUsingNoise = false;

	_int m_iCurrentLightIdx = 0;

	wstring m_strLightSavePath = TEXT("..\\..\\Client\\Bin\\DataFiles\\LightData\\");
	vector<LIGHT_DESC> m_LightDescs;

};

