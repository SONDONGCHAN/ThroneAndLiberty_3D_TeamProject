#include "RandomPicker.h"

CRandomPicker::CRandomPicker()
{
}

HRESULT CRandomPicker::Initialize()
{
	/*랜덤 시드 설정*/
	srand((unsigned)time(NULL));

	/*Random Device Setting*/
	m_RandomNumber = mt19937_64(m_RandomDevice());


	return S_OK;
}

_int CRandomPicker::PickRandomInt(_int _iStartLimit, _int _iEndLimit, _bool _bSort)
{
	if (_bSort)
	{
		if (_iStartLimit > _iEndLimit)
		{
			_int _iTemp = _iStartLimit;
			_iStartLimit = _iEndLimit;
			_iEndLimit = _iTemp;
		}
	}

	if (_iStartLimit >= _iEndLimit) {
		return _iStartLimit;
	}

	_int iDiff = _iEndLimit - _iStartLimit;

	_int iInitPickNum = rand() % iDiff;
	iInitPickNum += _iStartLimit;

	return iInitPickNum;
}

_float CRandomPicker::PickRandomFloat(_float _fStartLimit, _float _fEndLimit, _bool _bSort)
{
	if (_bSort)
	{
		if (_fStartLimit > _fEndLimit)
		{
			_float _fTemp = _fStartLimit;
			_fStartLimit = _fEndLimit;
			_fEndLimit = _fTemp;
		}
	}

	if (_fStartLimit >= _fEndLimit)
		return _fStartLimit;

	uniform_real_distribution<float>	fRandom(_fStartLimit, _fEndLimit);
	
	_float fPickNum = fRandom(m_RandomNumber);

	return fPickNum;
}

shared_ptr<CRandomPicker> CRandomPicker::Create()
{
	shared_ptr<CRandomPicker> pInstance = make_shared<CRandomPicker>();

	if (FAILED(pInstance->Initialize()))
		MSG_BOX("Failed to Create : CRandomPicker");

	return pInstance;
}
