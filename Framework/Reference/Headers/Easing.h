#pragma once

#include "Engine_Defines.h"
#define PI 3.14f

// Easing Equations : 시간 흐름에 따른 매개변수의 변화율 (보간 공식)
// https://easings.net/ko : 그래프 확인 가능 사이트

BEGIN(Engine)

class CEasing
{
public:
	CEasing();
	~CEasing() = default;

public:
	HRESULT Initialize();

	// 각 보간 공식들
	// IN : 처음은 느리고 갈수록 빠르게
	// OUT : 처음은 빠르고 갈수록 느리게
	// IN_OUT : 처음과 끝은 상대적으로 느리게, 중간은 빠르게
private:
	// Linear
	_float EaseLinear(_float _fTimeRatio);

	// Sine
	_float EaseInSine(_float _fTimeRatio);
	_float EaseOutSine(_float __fTimeRatio);
	_float EaseInOutSine(_float _fTimeRatio);

	// Quad
	_float EaseInQuad(_float _fTimeRatio);
	_float EaseOutQuad(_float _fTimeRatio);
	_float EaseInOutQuad(_float _fTimeRatio);

	// Cubic
	_float EaseInCubic(_float _fTimeRatio);
	_float EaseOutCubic(_float _fTimeRatio);
	_float EaseInOutCubic(_float _fTimeRatio);

	// Quart
	_float EaseInQuart(_float _fTimeRatio);
	_float EaseOutQuart(_float _fTimeRatio);
	_float EaseInOutQuart(_float _fTimeRatio);

	// Quint
	_float EaseInQuint(_float _fTimeRatio);
	_float EaseOutQuint(_float _fTimeRatio);
	_float EaseInOutQuint(_float _fTimeRatio);

	// Expo
	_float EaseInExpo(_float _fTimeRatio);
	_float EaseOutExpo(_float _fTimeRatio);
	_float EaseInOutExpo(_float _fTimeRatio);

	// Circ
	_float EaseInCirc(_float _fTimeRatio);
	_float EaseOutCirc(_float _fTimeRatio);
	_float EaseInOutCirc(_float _fTimeRatio);

	// Back
	_float EaseInBack(_float _fTimeRatio);
	_float EaseOutBack(_float _fTimeRatio);
	_float EaseInOutBack(_float _fTimeRatio);

	// Elastic
	_float EaseInElastic(_float _fTimeRatio);
	_float EaseOutElastic(_float _fTimeRatio);
	_float EaseInOutElastic(_float _fTimeRatio);

	// Bounce
	_float EaseInBounce(_float _fTimeRatio);
	_float EaseOutBounce(_float _fTimeRatio);
	_float EaseInOutBounce(_float _fTimeRatio);

public:
	// (보간 타입, 경과 시간, 총 시간, 시작 값, 끝 값)
	_float		Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float _fStartValue, _float _fEndValue);
	_float2		Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float2 _fStartValue, _float2 _fEndValue);
	_float3		Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float3 _fStartValue, _float3 _fEndValue);
	_float4		Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float4 _fStartValue, _float4 _fEndValue);

public:
	const char** GetEaseTypeKeys() { return m_strEasingTypes; }

public:
	const char* m_strEasingTypes[EASE_END] = 
	{
		"Linear",
		"InSine", "OutSine", "InOutSine",
		"InQuad", "OutQuad", "InOutQuad",
		"InCubic", "OutCubic", "InOutCubic",
		"InQuart", "OutQuart", "InOutQuart",
		"InQuint", "OutQuint", "InOutQuint",
		"InExpo", "OutExpo", "InOutExpo",
		"InCric", "OutCric", "InOutCric",
		"InBack", "OutBack", "InOutBack",
		"InElastic", "OutElastic", "InOutElastic",
		"InBounce", "OutBounce", "InOutBounce"
	};

public:
	static shared_ptr<CEasing> Create();
};

END