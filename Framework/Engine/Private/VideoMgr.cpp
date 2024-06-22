#include "VideoMgr.h"
#include "GameInstance.h"

#include <windows.h>
#include <d3d11.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <stdio.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "MF.lib")


CVideoMgr::CVideoMgr()
{

}

HRESULT CVideoMgr::Initialize(HWND _hVideo)
{

	m_hwndVideo = _hVideo;

	return S_OK;
}

HRESULT CVideoMgr::LoadVideoFiles()
{
	return S_OK;
}


HRESULT CVideoMgr::PlayVideo(wstring _strPath)
{

	//if (FAILED(CoInitializeEX(NULL)))
	//	return E_FAIL;

	if (FAILED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_pGraph)))
		return E_FAIL;

	m_pGraph->QueryInterface(IID_IMediaControl, (void**)&m_pControl);
	m_pGraph->QueryInterface(IID_IMediaEventEx, (void**)&m_pEvent);
	m_pGraph->QueryInterface(IID_IMediaSeeking, (void**)&m_pSeek);

	// Add the source filter to the graph
	if (FAILED(CoCreateInstance(CLSID_AsyncReader, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_pSource)))
		return E_FAIL;
	m_pGraph->AddFilter(m_pSource, L"Source");
	// Load the video file
	m_pSource->QueryInterface(IID_IFileSourceFilter, (void**)&m_pFileSourceFilter);
	m_pFileSourceFilter->Load(_strPath.c_str(), NULL);

	// Render the file
	if (FAILED(m_pGraph->RenderFile(_strPath.c_str(), NULL)))
		return E_FAIL;

	// Set the video window
	m_pGraph->QueryInterface(IID_IVideoWindow, (void**)&m_pVidWin);
	m_pVidWin->put_Owner((OAHWND)m_hwndVideo);
	m_pVidWin->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	m_pVidWin->put_MessageDrain((OAHWND)m_hwndVideo);
	m_pVidWin->put_Left(0);
	m_pVidWin->put_Top(0);
	m_pVidWin->put_Width(1280); // Adjust to your window's width
	m_pVidWin->put_Height(720); // Adjust to your window's height
	m_pVidWin->put_Visible(OATRUE);

	// Run the graph
	HRESULT hr = m_pControl->Run();

	long ev;
	/*영상이 끝날때까지 대기*/
	m_pEvent->WaitForCompletion(INFINITE, &ev);

	return S_OK;
}


shared_ptr<CVideoMgr> CVideoMgr::Create(HWND _hVideo)
{
	shared_ptr<CVideoMgr> pVideoMgr = make_shared<CVideoMgr>();

	if (FAILED(pVideoMgr->Initialize(_hVideo)))
		MSG_BOX("Failed to Create : CVideoMgr");

	return pVideoMgr;
}
