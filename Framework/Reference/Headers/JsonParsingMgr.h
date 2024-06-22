#pragma once

#include "Engine_Defines.h"

/* =============================================== */
//	[JsonParsingMgr]
// 
//  Json Parsing용 함수를 모아둔 클래스 
//	
//
/* =============================================== */




BEGIN(Engine)

class CJsonParsingMgr final
{
public:
	CJsonParsingMgr();
	~CJsonParsingMgr() = default;

public:
	void WriteJson(Json::Value _value, const wstring& _strSavePath);
	Json::Value ReadJson(const wstring& _strReadPath);

public:
	static shared_ptr<CJsonParsingMgr> Create();

};

END