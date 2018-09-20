//------------------------------------------------------------------------------
// File: AMCap.h
//
// Desc: DirectShow sample code - audio/video capture.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#pragma  once

#include <dbt.h>
#include <mmreg.h>
#include <msacm.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <commdlg.h>
#include <strsafe.h>

#if 1 //liw
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__ 
#define __IDxtAlphaSetter_INTERFACE_DEFINED__ 

#define __IDxtJpeg_INTERFACE_DEFINED__

#define __IDxtKey_INTERFACE_DEFINED__
#endif


#include <qedit.h>
#include "dshow.h"

#include "crossbar.h"
#include "SampleCGB.h"

// Macros
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }
#endif


extern "C"
{
    typedef BOOL (/* WINUSERAPI */ WINAPI *PUnregisterDeviceNotification)(
        IN HDEVNOTIFY Handle
        );

    typedef HDEVNOTIFY (/* WINUSERAPI */ WINAPI *PRegisterDeviceNotificationA)(
        IN HANDLE hRecipient,
        IN LPVOID NotificationFilter,
        IN DWORD Flags
        );

    typedef HDEVNOTIFY (/* WINUSERAPI */ WINAPI *PRegisterDeviceNotificationW)(
        IN HANDLE hRecipient,
        IN LPVOID NotificationFilter,
        IN DWORD Flags
        );
}

#define PRegisterDeviceNotification  PRegisterDeviceNotificationW


// window messages
#define WM_FGNOTIFY WM_USER+1

class AMCaputer
{
public:
	AMCaputer();
	virtual ~AMCaputer();

	BOOL Init(HWND hOwnerWnd,int iVideoWidth,int iVideoHeight);
	void Release();
	
	void SetDevices(int nVideoIndex,int nAudioIndex = -1);
	
	BOOL StartCapture(BOOL bPreview = TRUE);
	BOOL StopCapture();

	BOOL StartPreview();
	BOOL StopPreview();

	void  GetVideoSize(int & nWidth,int & nHeight);
	float GetVideoFPS(){return m_fVideoFPS;}
	ISampleGrabber * GetSampleGrabber(){return m_capInfo.pSampleGrabber;}

	int GetVideoDeviceNum(){return m_capInfo.iNumVCapDevices;}
	TCHAR*GetVideoDeviceName(int index);

private:

	BOOL MakeBuilder();
	BOOL MakeGraph();

	void EnumerateDevices();

	BOOL InitCapFilters();
	void FreeCapFilters();

	BOOL BuildCaptureGraph();
	BOOL BuildPreviewGraph();

	void RemoveDownstream(IBaseFilter *pf);
	void IMonRelease(IMoniker *&pm);
	void TearDownGraph(void);


	void ErrMsg(LPTSTR sz,...);


private:

    #define MAX_DEVICE_NUM 16

	struct _capstuff
	{
		ISampleCaptureGraphBuilder *pBuilder;
		IVideoWindow *pVW;
		IMediaEventEx *pME;
		IAMDroppedFrames *pDF;
		IAMVideoCompression *pVC;
		IAMVfwCaptureDialogs *pDlg;
		IAMStreamConfig *pASC;      // for audio cap
		IAMStreamConfig *pVSC;      // for video cap
		IBaseFilter *pRender;
		IBaseFilter *pVCap, *pACap;
		IBaseFilter *pSG_Filter;
		ISampleGrabber *pSampleGrabber;
		IGraphBuilder *pFg;
		int  iMasterStream;
		BOOL fCaptureGraphBuilt;
		BOOL fPreviewGraphBuilt;
		BOOL fCapturing;
		BOOL fPreviewing;
		BOOL fCapAudio;
		BOOL fCapCC;
		BOOL fCCAvail;
		BOOL fCapAudioIsRelevant;
		IMoniker *rgpmVideoMenu[MAX_DEVICE_NUM];
		TCHAR *szVideoDevName[MAX_DEVICE_NUM];
		IMoniker *rgpmAudioMenu[MAX_DEVICE_NUM];
		TCHAR *szAudioDevName[MAX_DEVICE_NUM];
		IMoniker *pmVideo;
		IMoniker *pmAudio;
		double FrameRate;
		BOOL fWantPreview;
		long lCapStartTime;
		long lCapStopTime;
		WCHAR wachFriendlyName[120];
		BOOL fUseFrameRate;
		long lDroppedBase;
		long lNotBase;
		BOOL fPreviewFaked;
		CCrossbar *pCrossbar;
		int iVideoInputMenuPos;
		LONG NumberOfVideoInputs;
		int iNumVCapDevices;
		int iNumACapDevices;
	}m_capInfo;

	HWND m_hWndApp;

	int m_iSrcVideoWidth;
	int m_iSrcVideoHeight;

	int m_iDesVideoWidth;
	int m_iDesVideoHeight;
	float m_fVideoFPS;

	HDEVNOTIFY m_hDevNotify;
	DWORD m_dwGraphRegister;

	PUnregisterDeviceNotification m_pUnregisterDeviceNotification;
	PRegisterDeviceNotification m_pRegisterDeviceNotification;

};







