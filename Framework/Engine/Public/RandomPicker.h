#pragma once

/* =============================================== */
//	[RandomPicker]
// 
//  매개변수로 던져준 지정된 범위에서 랜덤한 숫자를 골라 반환하는 기능을 포함한 클래스입니다
//  _int와 _float 두 형식을 지원하며 사용법은 Funtion 밑에 작성해둠
//
/* =============================================== */

#include "Engine_Defines.h"
#include <random>

BEGIN(Engine)

class CRandomPicker final
{
public:
	CRandomPicker();
	~CRandomPicker() = default;

public:
	HRESULT Initialize();

public:
	/*매개변수로는 숫자 제한의 시작점과 끝부분을 받습니다*/
	/*ex) 30 ~ 50사이의 랜덤숫자로 제한을 두고 싶다면 PickRandomIntNum(30, 50)을 던져주면 됨 */
	_int PickRandomInt(_int _iStartLimit, _int _iEndLimit, _bool _bSort = false);
	_float PickRandomFloat(_float _fStartLimit, _float _fEndLimit, _bool _bSort = false);

private:
	random_device m_RandomDevice;
	mt19937_64 m_RandomNumber;

public:
	static shared_ptr<CRandomPicker> Create();

};

END