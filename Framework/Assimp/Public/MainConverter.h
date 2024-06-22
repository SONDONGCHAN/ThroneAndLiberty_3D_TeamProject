#pragma once

#include "Assimp_Defines.h"

BEGIN(Assimp)

class CMainConverter final
{
public:
	CMainConverter();
	virtual ~CMainConverter();

public:
	HRESULT Initialize();
	void Tick(_float _fTimeDelta);
	HRESULT Render();

public:
	void FindFBXFile(wstring _strFilePath);
	/*모델의 상위 폴더를 읽은 후 애님 모델인지 논애님 모델인지 구분해준다*/
	_uint CheckModelType(string _strPath);
	/*자동 저장 경로 생성*/
	wstring MakeSavePath(wstring _strPath, wstring _strFileName);

private:
	/*FBX를 읽는 기본 경로*/
	wstring m_strBasePath = TEXT("..\\Bin\\Resource");
	/*클라이언트 저장경로*/
	wstring strClientBase = TEXT("..\\..\\Client\\Bin\\Resources");
	/*MapTool 분류 경로*/
	wstring strMapToolBase = TEXT("..\\..\\Tool_Map\\Bin\\Resources");

public:
	void Free();

};

END