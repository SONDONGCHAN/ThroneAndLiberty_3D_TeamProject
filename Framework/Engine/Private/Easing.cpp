#include "Easing.h"

_float CEasing::Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float _fStartValue, _float _fEndValue)
{
	_float fOutputVal = {};		// 0~1 값으로 출력되는 보간 값
	_float fInputTime = _eElapsedTime / _fDuration;	// 0~1로 치환한 시간 인풋값

	if (1.f < fInputTime)
		fInputTime = 1.f;

	else if (0.f > fInputTime)
		fInputTime = 0.f;

	switch (_eEase)
	{
	case EASE_LINEAR:
		fOutputVal = EaseLinear(fInputTime);
		break;
	case EASE_IN_SINE:
		fOutputVal = EaseInSine(fInputTime);
		break;
	case EASE_OUT_SINE:
		fOutputVal = EaseOutSine(fInputTime);
		break;
	case EASE_IN_OUT_SINE:
		fOutputVal = EaseInOutSine(fInputTime);
		break;
	case EASE_IN_QUAD:
		fOutputVal = EaseInQuad(fInputTime);
		break;
	case EASE_OUT_QUAD:
		fOutputVal = EaseOutQuad(fInputTime);
		break;
	case EAS_IN_OUT_QUAD:
		fOutputVal = EaseInOutQuad(fInputTime);
		break;
	case EASE_IN_CUBIC:
		fOutputVal = EaseInCubic(fInputTime);
		break;
	case EASE_OUT_CUBIC:
		fOutputVal = EaseOutCubic(fInputTime);
		break;
	case EASE_IN_OUT_CUBIC:
		fOutputVal = EaseInOutCubic(fInputTime);
		break;
	case EASE_IN_QUART:
		fOutputVal = EaseInQuart(fInputTime);
		break;
	case EASE_OUT_QUART:
		fOutputVal = EaseOutQuart(fInputTime);
		break;
	case EASE_IN_OUT_QUART:
		fOutputVal = EaseInOutQuart(fInputTime);
		break;
	case EASE_IN_QUINT:
		fOutputVal = EaseInQuint(fInputTime);
		break;
	case EASE_OUT_QUINT:
		fOutputVal = EaseOutQuint(fInputTime);
		break;
	case EASE_IN_OUT_QUINT:
		fOutputVal = EaseInOutQuint(fInputTime);
		break;
	case EASE_IN_EXPO:
		fOutputVal = EaseInExpo(fInputTime);
		break;
	case EASE_OUT_EXPO:
		fOutputVal = EaseOutExpo(fInputTime);
		break;
	case EASE_IN_OUT_EXPO:
		fOutputVal = EaseInOutExpo(fInputTime);
		break;
	case EASE_IN_CIRC:
		fOutputVal = EaseInCirc(fInputTime);
		break;
	case EASE_OUT_CIRC:
		fOutputVal = EaseOutCirc(fInputTime);
		break;
	case EASE_IN_OUT_CIRC:
		fOutputVal = EaseInOutCirc(fInputTime);
		break;
	case EASE_IN_BACK:
		fOutputVal = EaseInBack(fInputTime);
		break;
	case EASE_OUT_BACK:
		fOutputVal = EaseOutBack(fInputTime);
		break;
	case EASE_IN_OUT_BACK:
		fOutputVal = EaseInOutBack(fInputTime);
		break;
	case EASE_IN_ELASTIC:
		fOutputVal = EaseInElastic(fInputTime);
		break;
	case EASE_OUT_ELASTIC:
		fOutputVal = EaseOutElastic(fInputTime);
		break;
	case EASE_IN_OUT_ELASTIC:
		fOutputVal = EaseInOutElastic(fInputTime);
		break;
	case EASE_IN_BOUNCE:
		fOutputVal = EaseInBounce(fInputTime);
		break;
	case EASE_OUT_BOUNCE:
		fOutputVal = EaseOutBounce(fInputTime);
		break;
	case EASE_IN_OUT_BOUNCE:
		fOutputVal = EaseInOutBounce(fInputTime);
		break;
	default:
		break;
	}

	return fOutputVal * (_fEndValue - _fStartValue) + _fStartValue;
}

_float2 CEasing::Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float2 _fStartValue, _float2 _fEndValue)
{
	_float2 vOut = {};

	vOut.x = Ease(_eEase, _eElapsedTime, _fDuration, _fStartValue.x, _fEndValue.x);
	vOut.y = Ease(_eEase, _eElapsedTime, _fDuration, _fStartValue.y, _fEndValue.y);

	return vOut;
}

_float3 CEasing::Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float3 _fStartValue, _float3 _fEndValue)
{
	_float3 vOut = {};

	vOut.x = Ease(_eEase, _eElapsedTime, _fDuration, _fStartValue.x, _fEndValue.x);
	vOut.y = Ease(_eEase, _eElapsedTime, _fDuration, _fStartValue.y, _fEndValue.y);
	vOut.z = Ease(_eEase, _eElapsedTime, _fDuration, _fStartValue.z, _fEndValue.z);

	return vOut;
}

_float4 CEasing::Ease(EASE_TYPE _eEase, _float _eElapsedTime, _float _fDuration, _float4 _fStartValue, _float4 _fEndValue)
{
	_float4 vOut = {};

	vOut.x = Ease(_eEase, _eElapsedTime, _fDuration, _fStartValue.x, _fEndValue.x);
	vOut.y = Ease(_eEase, _eElapsedTime, _fDuration, _fStartValue.y, _fEndValue.y);
	vOut.z = Ease(_eEase, _eElapsedTime, _fDuration, _fStartValue.z, _fEndValue.z);
	vOut.w = Ease(_eEase, _eElapsedTime, _fDuration, _fStartValue.w, _fEndValue.w);

	return vOut;
}

CEasing::CEasing()
{
}

HRESULT CEasing::Initialize()
{
	return S_OK;
}

_float CEasing::EaseLinear(_float _fTimeRatio)
{
	return _fTimeRatio;
}

_float CEasing::EaseInSine(_float _fTimeRatio)
{
	return 1 - cosf((_fTimeRatio * PI) / 2.f);
}

_float CEasing::EaseOutSine(_float _fTimeRatio)
{
	return sinf((_fTimeRatio * PI) / 2.f);
}

_float CEasing::EaseInOutSine(_float _fTimeRatio)
{
	return -(cosf(PI * _fTimeRatio) - 1.f) / 2.f;
}

_float CEasing::EaseInQuad(_float _fTimeRatio)
{
	return _fTimeRatio * _fTimeRatio;
}

_float CEasing::EaseOutQuad(_float _fTimeRatio)
{
	return 1.f - (1.f - _fTimeRatio) * (1.f - _fTimeRatio);
}

_float CEasing::EaseInOutQuad(_float _fTimeRatio)
{
	return _fTimeRatio < 0.5f ? 2.f * _fTimeRatio * _fTimeRatio : 1.f - pow(-2.f * _fTimeRatio + 2.f, 2.f) / 2.f;
}

_float CEasing::EaseInCubic(_float _fTimeRatio)
{
	return _fTimeRatio * _fTimeRatio * _fTimeRatio;
}

_float CEasing::EaseOutCubic(_float _fTimeRatio)
{
	return 1.f - pow(1.f - _fTimeRatio, 3.f);
}

_float CEasing::EaseInOutCubic(_float _fTimeRatio)
{
	return _fTimeRatio < 0.5f ? 4.f * _fTimeRatio * _fTimeRatio * _fTimeRatio : 1.f - pow(-2.f * _fTimeRatio + 2.f, 3.f) / 2.f;
}

_float CEasing::EaseInQuart(_float _fTimeRatio)
{
	return _fTimeRatio * _fTimeRatio * _fTimeRatio * _fTimeRatio;
}

_float CEasing::EaseOutQuart(_float _fTimeRatio)
{
	return 1.f - pow(1.f - _fTimeRatio, 4.f);
}

_float CEasing::EaseInOutQuart(_float _fTimeRatio)
{
	return _fTimeRatio < 0.5f ? 8.f * _fTimeRatio * _fTimeRatio * _fTimeRatio * _fTimeRatio : 1.f - pow(-2.f * _fTimeRatio + 2.f, 4.f) / 2.f;
}

_float CEasing::EaseInQuint(_float _fTimeRatio)
{
	return _fTimeRatio * _fTimeRatio * _fTimeRatio * _fTimeRatio * _fTimeRatio;
}

_float CEasing::EaseOutQuint(_float _fTimeRatio)
{
	return 1.f - pow(1.f - _fTimeRatio, 5.f);
}

_float CEasing::EaseInOutQuint(_float _fTimeRatio)
{
	return _fTimeRatio < 0.5f ? 16.f * _fTimeRatio * _fTimeRatio * _fTimeRatio * _fTimeRatio * _fTimeRatio : 1.f - pow(-2.f * _fTimeRatio + 2.f, 5.f) / 2.f;
}

_float CEasing::EaseInExpo(_float _fTimeRatio)
{
	return _fTimeRatio == 0.f ? 0.f : pow(2.f, 10.f * _fTimeRatio - 10.f);
}

_float CEasing::EaseOutExpo(_float _fTimeRatio)
{
	return _fTimeRatio == 1.f ? 1.f : 1.f - pow(2.f, -10.f * _fTimeRatio);
}

_float CEasing::EaseInOutExpo(_float _fTimeRatio)
{
	return _fTimeRatio == 0.f
		? 0.f
		: _fTimeRatio == 1.f
		? 1.f
		: _fTimeRatio < 0.5f ? pow(2.f, 20.f * _fTimeRatio - 10.f) / 2.f
		: (2.f - pow(2.f, -20.f * _fTimeRatio + 10.f)) / 2.f;
}

_float CEasing::EaseInCirc(_float _fTimeRatio)
{
	return 1.f - sqrt(1.f - pow(_fTimeRatio, 2.f));
}

_float CEasing::EaseOutCirc(_float _fTimeRatio)
{
	return sqrt(1.f - pow(_fTimeRatio - 1.f, 2.f));
}

_float CEasing::EaseInOutCirc(_float _fTimeRatio)
{
	return _fTimeRatio < 0.5f
		? (1.f - sqrt(1.f - pow(2.f * _fTimeRatio, 2.f))) / 2.f
		: (sqrt(1.f - pow(-2.f * _fTimeRatio + 2.f, 2.f)) + 1.f) / 2.f;
}

_float CEasing::EaseInBack(_float _fTimeRatio)
{
	const _float c1 = 1.70158f;
	const _float c3 = c1 + 1.f;

	return c3 * _fTimeRatio * _fTimeRatio * _fTimeRatio - c1 * _fTimeRatio * _fTimeRatio;
}

_float CEasing::EaseOutBack(_float _fTimeRatio)
{
	const _float c1 = 1.70158f;
	const _float c3 = c1 + 1.f;

	return 1.f + c3 * pow(_fTimeRatio - 1.f, 3.f) + c1 * pow(_fTimeRatio - 1.f, 2.f);
}

_float CEasing::EaseInOutBack(_float _fTimeRatio)
{
	const _float c1 = 1.70158f;
	const _float c2 = c1 * 1.525f;

	return _fTimeRatio < 0.5f
		? (pow(2.f * _fTimeRatio, 2.f) * ((c2 + 1.f) * 2.f * _fTimeRatio - c2)) / 2.f
		: (pow(2.f * _fTimeRatio - 2.f, 2.f) * ((c2 + 1.f) * (_fTimeRatio * 2.f - 2.f) + c2) + 2.f) / 2.f;
}

_float CEasing::EaseInElastic(_float _fTimeRatio)
{
	const _float c4 = (2.f * PI) / 3.f;

	return _fTimeRatio == 0.f
		? 0.f
		: _fTimeRatio == 1.f
		? 1.f
		: -pow(2.f, 10.f * _fTimeRatio - 10.f) * sinf((_fTimeRatio * 10.f - 10.75f) * c4);
}

_float CEasing::EaseOutElastic(_float _fTimeRatio)
{
	const _float c4 = (2.f * PI) / 3.f;

	return _fTimeRatio == 0.f
		? 0.f
		: _fTimeRatio == 1.f
		? 1.f
		: pow(2.f, -10.f * _fTimeRatio) * sinf((_fTimeRatio * 10.f - 0.75f) * c4) + 1.f;
}

_float CEasing::EaseInOutElastic(_float _fTimeRatio)
{
	const _float c5 = (2.f * PI) / 4.5f;

	return _fTimeRatio == 0.f
		? 0.f
		: _fTimeRatio == 1.f
		? 1.f
		: _fTimeRatio < 0.5f
		? -(pow(2.f, 20.f * _fTimeRatio - 10.f) * sinf((20.f * _fTimeRatio - 11.125f) * c5)) / 2.f
		: (pow(2.f, -20.f * _fTimeRatio + 10.f) * sinf((20.f * _fTimeRatio - 11.125f) * c5)) / 2.f + 1.f;
}

_float CEasing::EaseInBounce(_float _fTimeRatio)
{
	return 1.f - EaseOutBounce(1.f - _fTimeRatio);
}

_float CEasing::EaseOutBounce(_float _fTimeRatio)
{
	const _float n1 = 7.5625f;
	const _float d1 = 2.75f;

	if (_fTimeRatio < 1.f / d1) {
		return n1 * _fTimeRatio * _fTimeRatio;
	}
	else if (_fTimeRatio < 2.f / d1) {
		return n1 * (_fTimeRatio -= 1.5f / d1) * _fTimeRatio + 0.75f;
	}
	else if (_fTimeRatio < 2.5f / d1) {
		return n1 * (_fTimeRatio -= 2.25f / d1) * _fTimeRatio + 0.9375f;
	}
	else {
		return n1 * (_fTimeRatio -= 2.625f / d1) * _fTimeRatio + 0.984375f;
	}
}

_float CEasing::EaseInOutBounce(_float _fTimeRatio)
{
	return _fTimeRatio < 0.5f
		? (1.f - EaseOutBounce(1.f - 2.f * _fTimeRatio)) / 2.f
		: (1.f + EaseOutBounce(2.f * _fTimeRatio - 1.f)) / 2.f;
}

shared_ptr<CEasing> CEasing::Create()
{
	shared_ptr<CEasing> pInstance = make_shared<CEasing>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CEasing");

	return pInstance;
}
