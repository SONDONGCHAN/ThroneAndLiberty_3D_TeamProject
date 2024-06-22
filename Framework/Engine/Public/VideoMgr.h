#pragma once



#include "Engine_Defines.h"

BEGIN(Engine)

class CVideoMgr
{
public:
	CVideoMgr();
	virtual ~CVideoMgr() = default;

public:
	HRESULT Initialize(HWND _hVideo);
	HRESULT LoadVideoFiles();
	HRESULT PlayVideo(wstring _strPath);

private:

	HWND m_hwndVideo;
	HANDLE m_hCloseEvent;



private:
	wstring m_strPath = TEXT("..\\..\\Client\\Bin\\Videos\\기억조각_오크필드.wmv");

private:
	shared_ptr<IGraphBuilder> m_pGraph = NULL;
	IMediaEventEx* m_pEvent = NULL;
	IBaseFilter* m_pSource = NULL;
	IFileSourceFilter* m_pFileSourceFilter = NULL;
	shared_ptr<IVideoWindow> m_pVidWin = NULL;
	shared_ptr<IMediaControl> m_pControl = NULL;
	shared_ptr<IMediaSeeking> m_pSeek = NULL;
	REFERENCE_TIME rtCurrent;

	shared_ptr<IGraphBuilder> m_pGraphChange = NULL;
	IMediaEventEx* m_pEventChange = NULL;
	IBaseFilter* m_pSourceChange = NULL;
	IFileSourceFilter* m_pFileSourceFilterChange = NULL;
	shared_ptr<IVideoWindow> m_pVidWinChange = NULL;
	shared_ptr<IMediaControl> m_pControlChange = NULL;
	shared_ptr<IMediaSeeking> m_pSeekChange = NULL;

public:
	static shared_ptr<CVideoMgr> Create(HWND _hVideo);

};

END