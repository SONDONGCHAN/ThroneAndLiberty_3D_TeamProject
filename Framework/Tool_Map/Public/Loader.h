#pragma once
#include "Tool_Defines.h"

BEGIN(Tool_Map)
class CLoader : public enable_shared_from_this<CLoader>
{
private:
	CLoader();
	DESTRUCTOR(CLoader)

public:
	HRESULT Initialize(LEVEL _eNextLevel);
	HRESULT Loading_Selector();
	_bool isFinished() const { return m_bIsFinished; }

private:
	HRESULT Loading_For_AtelierLevel();

#ifdef _DEBUG
public:
	void Output_LoadingText();
#endif

private:
	_bool m_bIsFinished{};

	LEVEL m_eNextLevel{ LEVEL_END };

	wstring	m_strLoadingText{};
	thread m_LodingThread;

public:
	static shared_ptr<CLoader> Create(LEVEL _eNextLevel);
};
END