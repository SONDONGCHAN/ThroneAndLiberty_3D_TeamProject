#include "Calculator.h"

CCalculator::CCalculator()
{
}

HRESULT CCalculator::Initialize()
{
	return S_OK;
}

_float CCalculator::Lerp(_float _fStart, _float _fEnd, _float _fRatio)
{
	_float fResult = _fStart + ((_fEnd - _fStart) * _fRatio);

	return fResult;
}


shared_ptr<CCalculator> CCalculator::Create()
{
	shared_ptr<CCalculator> pInstance = make_shared<CCalculator>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CCalculator");

	return pInstance;
}
