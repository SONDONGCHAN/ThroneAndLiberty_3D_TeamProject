#pragma once

#include "Tool_Defines.h"
#define MAX_EFFECT_NUM 20

BEGIN(Tool_Effect)

class CWindowPlay
{
public:
	CWindowPlay();
	~CWindowPlay() = default;

public:
	HRESULT Initialize();
	void	Tick(_float _fTimeDelta);

public:
	//// 0. 메쉬
	//void	Properties_Mesh();
	//// 1. 디퓨즈 텍스처
	//void	Properties_Diffuse();
	//// 2. 마스크 텍스처
	//void	Properties_Mask();
	//// 3. 노이즈 텍스처
	//void	Properties_Noise();
	//// 4. 색상
	//void	Properties_Color();
	//// 5. 빌보드
	//void	Properties_BillBoard();
	//// 6. 크기
	//void	Properties_Scale();
	//// 7. 회전
	//void	Properties_Rotation();
	//// 8. 이동
	//void	Properties_Translation();
	//// 9. 셰이더
	//void	Properties_Shader();

private:
	void	KeyInput();

private:
	// 이펙트 모음 전체 재생 시간
	_float			m_fTotalDuration = 5.f;
	_float			m_fTotalSpeed = 1.f;

	_float			m_fElapsedTime = 0.f;

	// 각 이펙트 재생 시간
	vector<shared_ptr<class CEffect>> m_pEffects = {};
	vector<_float>	m_EffectPlayTimes = {};

	_bool			m_bLoop = false;
	_bool			m_bPlay = false;

private:
	ComPtr<ID3D11Device>				m_pDevice = nullptr;
	ComPtr<ID3D11DeviceContext>			m_pContext = nullptr;
};

END