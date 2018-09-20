
#pragma once

#include <atlbase.h>
#include <windows.h>

#pragma include_alias( "dxtrans.h", "qedit.h" )
	#define __IDxtCompositor_INTERFACE_DEFINED__
	#define __IDxtAlphaSetter_INTERFACE_DEFINED__
	#define __IDxtJpeg_INTERFACE_DEFINED__
	#define __IDxtKey_INTERFACE_DEFINED__

#include "qedit.h"
#include "dshow.h"

#ifndef srelease
#define srelease(x) \
if ( NULL != x ) \
{ \
  x->Release( ); \
  x = NULL; \
}
#endif


class CCaptureVideo : public CWnd
{
public:
	CCaptureVideo();
	virtual ~CCaptureVideo();
	HRESULT Start(int iDeviceID,HWND hWnd);
	HRESULT Stop();
	
	void GetVideoSize(int & nWidth,int & nHeight);
	float  GetVideoFPS(){return m_fVideoFPS;}
	ISampleGrabber * GetSampleGrabber(){return m_pSG;}

	int GetVideoDeviceNum(){return m_nVideoDevNum;}
	TCHAR*GetVideoDeviceName(int index);

protected:
	HRESULT SetupVideoWindow();
	HRESULT InitCaptureGraphBuilder();
	int EnumDevices();
	void FreeMediaType(AM_MEDIA_TYPE &mt);
	bool BindFilter(int deviceID, IBaseFilter **ppBF);
private:
	HWND m_hWnd;
	ISampleGrabber *m_pSG;
	IGraphBuilder *m_pGB;
	ICaptureGraphBuilder2 *m_pCapGB;
	IMediaControl *m_pMC;
	IVideoWindow *m_pVW;

	int m_lVideoWidth;
	int m_lVideoHeight;
	float m_fVideoFPS;

	#define MAX_DEVICE_NUM 16
	TCHAR *m_szVideoDevName[MAX_DEVICE_NUM];
	int    m_nVideoDevNum;
};

