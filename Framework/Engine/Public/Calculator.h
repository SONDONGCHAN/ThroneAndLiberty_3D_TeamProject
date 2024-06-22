
#pragma once

/* =============================================== */
//	[Calculator]
// 
//  기타 여러 계산을 해주는 함수가 포함된 클래스 입니다.
//
/* =============================================== */

#include "Engine_Defines.h"
#include <random>

BEGIN(Engine)

class CCalculator final
{
public:
	CCalculator();
	~CCalculator() = default;

public:
	HRESULT Initialize();

public:

	_float Lerp(_float _fStart, _float _fEnd, _float _fRatio);

private:

public:
	static shared_ptr<CCalculator> Create();

};

END