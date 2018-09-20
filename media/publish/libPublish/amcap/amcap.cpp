//------------------------------------------------------------------------------
// File: AMCap.cpp
//
// Desc: Audio/Video Capture sample for DirectShow
//
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "amcap.h"




//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define ABS(x) (((x) > 0) ? (x) : -(x))

// An application can advertise the existence of its filter graph
// by registering the graph with a global Running Object Table (ROT).
// The GraphEdit application can detect and remotely view the running
// filter graph, allowing you to 'spy' on the graph with GraphEdit.
//
// To enable registration in this sample, define REGISTER_FILTERGRAPH.
//
#ifdef  DEBUG
#define REGISTER_FILTERGRAPH
#endif


AMCaputer::AMCaputer():
m_hWndApp(NULL),
m_iSrcVideoWidth(0),
m_iSrcVideoHeight(0),
m_iDesVideoWidth(0),
m_iDesVideoHeight(0),
m_fVideoFPS(25.0f),
m_hDevNotify(NULL),
m_dwGraphRegister(0),
m_pUnregisterDeviceNotification(NULL),
m_pRegisterDeviceNotification(NULL)
{

}

AMCaputer::~AMCaputer()
{

}


BOOL AMCaputer::Init(HWND hOwnerWnd,int iVideoWidth,int iVideoHeight)
{
	//if(hOwnerWnd == NULL || iVideoWidth < 1 || iVideoHeight < 1)
	//	return FALSE;
	if( iVideoWidth < 1 || iVideoHeight < 1)
		return FALSE;

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	m_iSrcVideoWidth = iVideoWidth;
	m_iSrcVideoHeight= iVideoHeight;

	memset(&m_capInfo,0, sizeof(m_capInfo));
	m_hWndApp = hOwnerWnd;

	ZeroMemory(m_capInfo.rgpmAudioMenu, sizeof(m_capInfo.rgpmAudioMenu));
	ZeroMemory(m_capInfo.rgpmVideoMenu, sizeof(m_capInfo.rgpmVideoMenu));
	m_capInfo.pmVideo = 0;
	m_capInfo.pmAudio = 0;

	EnumerateDevices();

	// do we want audio?
	m_capInfo.fCapAudio = FALSE;
	m_capInfo.fCapCC    = FALSE;

	// do we want preview?
	m_capInfo.fWantPreview = FALSE;

	// which stream should be the master? NONE(-1) means nothing special happens
	// AUDIO(1) means the video frame rate is changed before written out to keep
	// the movie in sync when the audio and video capture crystals are not the
	// same (and therefore drift out of sync after a few minutes).  VIDEO(0)
	// means the audio sample rate is changed before written out
	m_capInfo.iMasterStream = m_capInfo.fCapAudio ? 1 : 0;

	// get the frame rate 
	m_capInfo.fUseFrameRate  = FALSE;
	m_capInfo.FrameRate = 20.0;

	// Instantiate the capture filters we need to do the menu items.
	// This will start previewing, if desired
	//
	// Make these the official devices we're using
	SetDevices(0,-1);

	// Register for device add/remove notifications
	DEV_BROADCAST_DEVICEINTERFACE filterData;
	ZeroMemory(&filterData, sizeof(DEV_BROADCAST_DEVICEINTERFACE));

	filterData.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	filterData.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	filterData.dbcc_classguid = AM_KSCATEGORY_CAPTURE;

	m_pUnregisterDeviceNotification = NULL;
	m_pRegisterDeviceNotification = NULL;
	// dynload device removal APIs
	{
		HMODULE hmodUser = GetModuleHandle(TEXT("user32.dll"));
		ASSERT(hmodUser);       // we link to user32
		m_pUnregisterDeviceNotification = (PUnregisterDeviceNotification)
			GetProcAddress(hmodUser, "UnregisterDeviceNotification");

		// m_pRegisterDeviceNotification is prototyped differently in unicode
		m_pRegisterDeviceNotification = (PRegisterDeviceNotification)
			GetProcAddress(hmodUser,
			"RegisterDeviceNotificationW"
			);

		// failures expected on older platforms.
		ASSERT(m_pRegisterDeviceNotification && m_pUnregisterDeviceNotification ||
			!m_pRegisterDeviceNotification && !m_pUnregisterDeviceNotification);
	}

	m_hDevNotify = NULL;

	if(m_pRegisterDeviceNotification && m_hWndApp)
	{
		m_hDevNotify = m_pRegisterDeviceNotification(m_hWndApp, &filterData, DEVICE_NOTIFY_WINDOW_HANDLE);
		ASSERT(m_hDevNotify != NULL);
	}

	return TRUE;
}


void AMCaputer::Release()
{
	TCHAR szBuf[512];
	WCHAR *wszDisplayName = NULL;

	// Unregister device notifications
	if(m_hDevNotify != NULL)
	{
		ASSERT(m_pUnregisterDeviceNotification);
		m_pUnregisterDeviceNotification(m_hDevNotify);
		m_hDevNotify = NULL;
	}

	// Destroy the filter graph and cleanup
	StopPreview();
	StopCapture();
	TearDownGraph();
	FreeCapFilters();

	wszDisplayName = 0;
	szBuf[0] = NULL;

	if(m_capInfo.pmVideo)
	{
		if(SUCCEEDED(m_capInfo.pmVideo->GetDisplayName(0, 0, &wszDisplayName)))
		{
			if(wszDisplayName)
			{
				StringCchCopyN(szBuf, NUMELMS(szBuf), wszDisplayName, NUMELMS(szBuf)-1);
				CoTaskMemFree(wszDisplayName);
			}
		}
	}

	wszDisplayName = 0;
	szBuf[0] = NULL;

	if(m_capInfo.pmAudio)
	{
		if(SUCCEEDED(m_capInfo.pmAudio->GetDisplayName(0, 0, &wszDisplayName)))
		{
			if(wszDisplayName)
			{
				(void)StringCchCopy(szBuf, NUMELMS(szBuf), wszDisplayName );
				CoTaskMemFree(wszDisplayName);
			}
		}
	}


	IMonRelease(m_capInfo.pmVideo);
	IMonRelease(m_capInfo.pmAudio);
	for(int i = 0; i < NUMELMS(m_capInfo.rgpmVideoMenu); i++)
	{
		IMonRelease(m_capInfo.rgpmVideoMenu[i]);

		if(m_capInfo.szVideoDevName[i] != NULL)
		{
			delete m_capInfo.szVideoDevName[i];
			m_capInfo.szVideoDevName[i] = NULL;
		}
	}
	for(int i = 0; i < NUMELMS(m_capInfo.rgpmAudioMenu); i++)
	{
		IMonRelease(m_capInfo.rgpmAudioMenu[i]);

		if(m_capInfo.szAudioDevName[i] != NULL)
		{
			delete m_capInfo.szAudioDevName[i];
			m_capInfo.szAudioDevName[i] = NULL;
		}
	}

	//CoUninitialize();
}


void AMCaputer::IMonRelease(IMoniker *&pm)
{
	if(pm)
	{
		pm->Release();
		pm = 0;
	}
}


// Make a graph builder object we can use for capture graph building
//
BOOL AMCaputer::MakeBuilder()
{
	// we have one already
	if(m_capInfo.pBuilder)
		return TRUE;

	m_capInfo.pBuilder = new ISampleCaptureGraphBuilder( );
	if( NULL == m_capInfo.pBuilder )
	{
		return FALSE;
	}

	return TRUE;
}


// Make a graph object we can use for capture graph building
//
BOOL AMCaputer::MakeGraph()
{
	// we have one already
	if(m_capInfo.pFg)
		return TRUE;

	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, (LPVOID *)&m_capInfo.pFg);

	return (hr == NOERROR) ? TRUE : FALSE;
}


// Tear down everything downstream of a given filter
void AMCaputer::RemoveDownstream(IBaseFilter *pf)
{
	IPin *pP=0, *pTo=0;
	ULONG u;
	IEnumPins *pins = NULL;
	PIN_INFO pininfo;

	if (!pf)
		return;

	HRESULT hr = pf->EnumPins(&pins);
	pins->Reset();

	while(hr == NOERROR)
	{
		hr = pins->Next(1, &pP, &u);
		if(hr == S_OK && pP)
		{
			pP->ConnectedTo(&pTo);
			if(pTo)
			{
				hr = pTo->QueryPinInfo(&pininfo);
				if(hr == NOERROR)
				{
					if(pininfo.dir == PINDIR_INPUT)
					{
						RemoveDownstream(pininfo.pFilter);
						m_capInfo.pFg->Disconnect(pTo);
						m_capInfo.pFg->Disconnect(pP);
						m_capInfo.pFg->RemoveFilter(pininfo.pFilter);
					}
					pininfo.pFilter->Release();
				}
				pTo->Release();
			}
			pP->Release();
		}
	}

	if(pins)
		pins->Release();
}


// Tear down everything downstream of the capture filters, so we can build
// a different capture graph.  Notice that we never destroy the capture filters
// and WDM filters upstream of them, because then all the capture settings
// we've set would be lost.
//
void AMCaputer::TearDownGraph()
{
	SAFE_RELEASE(m_capInfo.pRender);
	SAFE_RELEASE(m_capInfo.pME);
	SAFE_RELEASE(m_capInfo.pDF);

	if(m_capInfo.pVW)
	{
		// stop drawing in our window, or we may get wierd repaint effects
		m_capInfo.pVW->put_Owner(NULL);
		m_capInfo.pVW->put_Visible(OAFALSE);
		m_capInfo.pVW->Release();
		m_capInfo.pVW = NULL;
	}

	// destroy the graph downstream of our capture filters
	if(m_capInfo.pVCap)
		RemoveDownstream(m_capInfo.pVCap);
	if(m_capInfo.pACap)
		RemoveDownstream(m_capInfo.pACap);

	// potential debug output - what the graph looks like
	// if (m_capInfo.pFg) DumpGraph(m_capInfo.pFg, 1);

#ifdef REGISTER_FILTERGRAPH
	// Remove filter graph from the running object table
	if(m_dwGraphRegister)
	{
		RemoveGraphFromRot(m_dwGraphRegister);
		m_dwGraphRegister = 0;
	}
#endif

	m_capInfo.fCaptureGraphBuilt = FALSE;
	m_capInfo.fPreviewGraphBuilt = FALSE;
	m_capInfo.fPreviewFaked = FALSE;
}


// create the capture filters of the graph.  We need to keep them loaded from
// the beginning, so we can set parameters on them and have them remembered
//
BOOL AMCaputer::InitCapFilters()
{
	HRESULT hr=S_OK;
	BOOL f;

	m_capInfo.fCCAvail = FALSE;  // assume no closed captioning support

	f = MakeBuilder();
	if(!f)
	{
		ErrMsg(TEXT("Cannot instantiate graph builder"));
		return FALSE;
	}

	//
	// First, we need a Video Capture filter, and some interfaces
	//
	m_capInfo.pVCap = NULL;

	if(m_capInfo.pmVideo != 0)
	{
		IPropertyBag *pBag;
		m_capInfo.wachFriendlyName[0] = 0;

		hr = m_capInfo.pmVideo->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;

			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				hr = StringCchCopyW(m_capInfo.wachFriendlyName, sizeof(m_capInfo.wachFriendlyName) / sizeof(m_capInfo.wachFriendlyName[0]), var.bstrVal);
				SysFreeString(var.bstrVal);
			}

			pBag->Release();
		}

		hr = m_capInfo.pmVideo->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_capInfo.pVCap);
	}

	if(m_capInfo.pVCap == NULL)
	{
		ErrMsg(TEXT("Error %x: Cannot create video capture filter"), hr);
		goto InitCapFiltersFail;
	}

	//
	// make a filtergraph, give it to the graph builder and put the video
	// capture filter in the graph
	//

	f = MakeGraph();
	if(!f)
	{
		ErrMsg(TEXT("Cannot instantiate filtergraph"));
		goto InitCapFiltersFail;
	}

	hr = m_capInfo.pBuilder->SetFiltergraph(m_capInfo.pFg);
	if(hr != NOERROR)
	{
		ErrMsg(TEXT("Cannot give graph to builder"));
		goto InitCapFiltersFail;
	}

	// Add the video capture filter to the graph with its friendly name
	hr = m_capInfo.pFg->AddFilter(m_capInfo.pVCap, m_capInfo.wachFriendlyName);
	if(hr != NOERROR)
	{
		ErrMsg(TEXT("Error %x: Cannot add vidcap to filtergraph"), hr);
		goto InitCapFiltersFail;
	}

	////////////////////////////////////////////////////////////////
	//bufferץȡ
	hr = CoCreateInstance(CLSID_SampleGrabber,NULL,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void**)&m_capInfo.pSG_Filter); 
	if(hr != NOERROR)
	{
		ErrMsg(TEXT("Error %x: Cannot create CLSID_SampleGrabber"), hr);
		goto InitCapFiltersFail;
	}
	hr = m_capInfo.pSG_Filter->QueryInterface(IID_ISampleGrabber,(void**)&m_capInfo.pSampleGrabber);
	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt,sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
	VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)mt.pbFormat;

	hr = m_capInfo.pSampleGrabber->SetMediaType(&mt);
	if(hr != NOERROR)
	{
		ErrMsg(TEXT("Error %x: Cannot SetMediaType to ISampleGrabber"), hr);
		goto InitCapFiltersFail;
	}
	m_capInfo.pFg->AddFilter(m_capInfo.pSG_Filter,L"SG_Filter");
	if(hr != NOERROR)
	{
		ErrMsg(TEXT("Error %x: Cannot add SG_Filter to filtergraph"), hr);
		goto InitCapFiltersFail;
	}
	///////////////////////////////////////////////////////////////////

	// Calling FindInterface below will result in building the upstream
	// section of the capture graph (any WDM TVTuners or Crossbars we might
	// need).

	// we use this interface to get the name of the driver
	// Don't worry if it doesn't work:  This interface may not be available
	// until the pin is connected, or it may not be available at all.
	// (eg: interface may not be available for some DV capture)
	hr = m_capInfo.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Interleaved, m_capInfo.pVCap,
		IID_IAMVideoCompression, (void **)&m_capInfo.pVC);
	if(hr != S_OK)
	{
		hr = m_capInfo.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, m_capInfo.pVCap,
			IID_IAMVideoCompression, (void **)&m_capInfo.pVC);
	}

	// !!! What if this interface isn't supported?
	// we use this interface to set the frame rate and get the capture size
	hr = m_capInfo.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Interleaved,
		m_capInfo.pVCap, IID_IAMStreamConfig, (void **)&m_capInfo.pVSC);

	if(hr != NOERROR)
	{
		hr = m_capInfo.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, m_capInfo.pVCap,
			IID_IAMStreamConfig, (void **)&m_capInfo.pVSC);
		if(hr != NOERROR)
		{
			// this means we can't set frame rate (non-DV only)
			ErrMsg(TEXT("Error %x: Cannot find VCapture:IAMStreamConfig"), hr);
		}
	}

	m_capInfo.fCapAudioIsRelevant = TRUE;

	AM_MEDIA_TYPE *pmt;

	// Set capture format
	if(m_capInfo.pVSC && m_capInfo.pVSC->GetFormat(&pmt) == S_OK)
	{
		// DV capture does not use a VIDEOINFOHEADER
		if(pmt->formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
			pvi->bmiHeader.biWidth  = m_iSrcVideoWidth;
			pvi->bmiHeader.biHeight = m_iSrcVideoHeight;
			hr = m_capInfo.pVSC->SetFormat(pmt);
		}
		DeleteMediaType(pmt);
	}

	// Get capture format
	if(m_capInfo.pVSC && m_capInfo.pVSC->GetFormat(&pmt) == S_OK)
	{
		// DV capture does not use a VIDEOINFOHEADER
		if(pmt->formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
			m_iDesVideoWidth   = pvi->bmiHeader.biWidth;
			m_iDesVideoHeight  = pvi->bmiHeader.biHeight;

			if(pvi->AvgTimePerFrame > 0)
				m_fVideoFPS = (float)(10000000 / pvi->AvgTimePerFrame);
		}
		if(pmt->majortype != MEDIATYPE_Video)
		{
			// This capture filter captures something other that pure video.
			// Maybe it's DV or something?  Anyway, chances are we shouldn't
			// allow capturing audio separately, since our video capture
			// filter may have audio combined in it already!
			m_capInfo.fCapAudioIsRelevant = FALSE;
			m_capInfo.fCapAudio = FALSE;
		}
		DeleteMediaType(pmt);
	}

	// we use this interface to bring up the 3 dialogs
	// NOTE:  Only the VfW capture filter supports this.  This app only brings
	// up dialogs for legacy VfW capture drivers, since only those have dialogs
	hr = m_capInfo.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video, m_capInfo.pVCap,
		IID_IAMVfwCaptureDialogs, (void **)&m_capInfo.pDlg);

	// Use the crossbar class to help us sort out all the possible video inputs
	// The class needs to be given the capture filters ANALOGVIDEO input pin
	{
		IPin        *pP = 0;
		IEnumPins   *pins=0;
		ULONG        n;
		PIN_INFO     pinInfo;
		BOOL         Found = FALSE;
		IKsPropertySet *pKs=0;
		GUID guid;
		DWORD dw;
		BOOL fMatch = FALSE;

		m_capInfo.pCrossbar = NULL;

		if(SUCCEEDED(m_capInfo.pVCap->EnumPins(&pins)))
		{
			while(!Found && (S_OK == pins->Next(1, &pP, &n)))
			{
				if(S_OK == pP->QueryPinInfo(&pinInfo))
				{
					if(pinInfo.dir == PINDIR_INPUT)
					{
						// is this pin an ANALOGVIDEOIN input pin?
						if(pP->QueryInterface(IID_IKsPropertySet,
							(void **)&pKs) == S_OK)
						{
							if(pKs->Get(AMPROPSETID_Pin,
								AMPROPERTY_PIN_CATEGORY, NULL, 0,
								&guid, sizeof(GUID), &dw) == S_OK)
							{
								if(guid == PIN_CATEGORY_ANALOGVIDEOIN)
									fMatch = TRUE;
							}
							pKs->Release();
						}

						if(fMatch)
						{
							HRESULT hrCreate=S_OK;
							m_capInfo.pCrossbar = new CCrossbar(pP, &hrCreate);
							if (!m_capInfo.pCrossbar || FAILED(hrCreate))
								break;

							hr = m_capInfo.pCrossbar->GetInputCount(&m_capInfo.NumberOfVideoInputs);
							Found = TRUE;
						}
					}
					pinInfo.pFilter->Release();
				}
				pP->Release();
			}
			pins->Release();
		}
	}

	// there's no point making an audio capture filter
	if(m_capInfo.fCapAudioIsRelevant == FALSE)
		goto SkipAudio;

	// create the audio capture filter, even if we are not capturing audio right
	// now, so we have all the filters around all the time.

	//
	// We want an audio capture filter and some interfaces
	//

	if(m_capInfo.pmAudio == 0)
	{
		// there are no audio capture devices. We'll only allow video capture
		m_capInfo.fCapAudio = FALSE;
		goto SkipAudio;
	}
	m_capInfo.pACap = NULL;


	hr = m_capInfo.pmAudio->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_capInfo.pACap);

	if(m_capInfo.pACap == NULL)
	{
		// there are no audio capture devices. We'll only allow video capture
		m_capInfo.fCapAudio = FALSE;
		ErrMsg(TEXT("Cannot create audio capture filter"));
		goto SkipAudio;
	}

	//
	// put the audio capture filter in the graph
	//
	{
		WCHAR wachAudioFriendlyName[256];
		IPropertyBag *pBag;

		wachAudioFriendlyName[0] = 0;

		// Read the friendly name of the filter to assist with remote graph
		// viewing through GraphEdit
		hr = m_capInfo.pmAudio->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;

			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				hr = StringCchCopyW(wachAudioFriendlyName, 256, var.bstrVal);
				SysFreeString(var.bstrVal);
			}

			pBag->Release();
		}

		// We'll need this in the graph to get audio property pages
		hr = m_capInfo.pFg->AddFilter(m_capInfo.pACap, wachAudioFriendlyName);
		if(hr != NOERROR)
		{
			ErrMsg(TEXT("Error %x: Cannot add audio capture filter to filtergraph"), hr);
			goto InitCapFiltersFail;
		}
	}

	// Calling FindInterface below will result in building the upstream
	// section of the capture graph (any WDM TVAudio's or Crossbars we might
	// need).

	// !!! What if this interface isn't supported?
	// we use this interface to set the captured wave format
	hr = m_capInfo.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, m_capInfo.pACap,
		IID_IAMStreamConfig, (void **)&m_capInfo.pASC);

	if(hr != NOERROR)
	{
		ErrMsg(TEXT("Cannot find ACapture:IAMStreamConfig"));
	}

SkipAudio:

	// Can this filter do closed captioning?
	IPin *pPin;
	hr = m_capInfo.pBuilder->FindPin(m_capInfo.pVCap, PINDIR_OUTPUT, &PIN_CATEGORY_VBI,
		NULL, FALSE, 0, &pPin);
	if(hr != S_OK)
		hr = m_capInfo.pBuilder->FindPin(m_capInfo.pVCap, PINDIR_OUTPUT, &PIN_CATEGORY_CC,
		NULL, FALSE, 0, &pPin);
	if(hr == S_OK)
	{
		pPin->Release();
		m_capInfo.fCCAvail = TRUE;
	}
	else
	{
		m_capInfo.fCapCC = FALSE;    // can't capture it, then
	}

	// potential debug output - what the graph looks like
	// DumpGraph(m_capInfo.pFg, 1);

	return TRUE;

InitCapFiltersFail:
	FreeCapFilters();
	return FALSE;
}


// all done with the capture filters and the graph builder
//
void AMCaputer::FreeCapFilters()
{
	SAFE_RELEASE(m_capInfo.pFg);
	if( m_capInfo.pBuilder )
	{
		delete m_capInfo.pBuilder;
		m_capInfo.pBuilder = NULL;
	}
	SAFE_RELEASE(m_capInfo.pVCap);
	SAFE_RELEASE(m_capInfo.pACap);
	SAFE_RELEASE(m_capInfo.pASC);
	SAFE_RELEASE(m_capInfo.pVSC);
	SAFE_RELEASE(m_capInfo.pVC);
	SAFE_RELEASE(m_capInfo.pDlg);

	if(m_capInfo.pCrossbar)
	{
		delete m_capInfo.pCrossbar;
		m_capInfo.pCrossbar = NULL;
	}
}


// build the capture graph
//
BOOL AMCaputer::BuildCaptureGraph()
{
	HRESULT hr;
	AM_MEDIA_TYPE *pmt=0;

	// we have one already
	if(m_capInfo.fCaptureGraphBuilt)
		return TRUE;

	// No rebuilding while we're running
	if(m_capInfo.fCapturing || m_capInfo.fPreviewing)
		return FALSE;

	// We don't have the necessary capture filters
	if(m_capInfo.pVCap == NULL)
		return FALSE;
	if(m_capInfo.pACap == NULL && m_capInfo.fCapAudio)
		return FALSE;

	// we already have another graph built... tear down the old one
	if(m_capInfo.fPreviewGraphBuilt)
		TearDownGraph();

	//////////////////////////////////////////////////////////////////////////////////////
	m_capInfo.pRender = m_capInfo.pSG_Filter;
	//////////////////////////////////////////////////////////////////////////////////////
	//
	// Render the video capture and preview pins - even if the capture filter only
	// has a capture pin (and no preview pin) this should work... because the
	// capture graph builder will use a smart tee filter to provide both capture
	// and preview.  We don't have to worry.  It will just work.
	//

	// NOTE that we try to render the interleaved pin before the video pin, because
	// if BOTH exist, it's a DV filter and the only way to get the audio is to use
	// the interleaved pin.  Using the Video pin on a DV filter is only useful if
	// you don't want the audio.

	hr = m_capInfo.pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Interleaved,
		m_capInfo.pVCap, NULL, m_capInfo.pRender);
	if(hr != NOERROR)
	{
		hr = m_capInfo.pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video,
			m_capInfo.pVCap, NULL, m_capInfo.pRender);
		if(hr != NOERROR)
		{
			ErrMsg(TEXT("Cannot render video capture stream"));
			goto SetupCaptureFail;
		}
	}

	if(m_capInfo.fWantPreview)
	{
		hr = m_capInfo.pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Interleaved,
			m_capInfo.pVCap, NULL, NULL);
		if(hr == VFW_S_NOPREVIEWPIN)
		{
			// preview was faked up for us using the (only) capture pin
			m_capInfo.fPreviewFaked = TRUE;
		}
		else if(hr != S_OK)
		{
			hr = m_capInfo.pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
				m_capInfo.pVCap, NULL, NULL);
			if(hr == VFW_S_NOPREVIEWPIN)
			{
				// preview was faked up for us using the (only) capture pin
				m_capInfo.fPreviewFaked = TRUE;
			}
			else if(hr != S_OK)
			{
				ErrMsg(TEXT("Cannot render video preview stream"));
				goto SetupCaptureFail;
			}
		}
	}

	//
	// Render the audio capture pin?
	//

	if(m_capInfo.fCapAudio)
	{
		hr = m_capInfo.pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio,
			m_capInfo.pACap, NULL, m_capInfo.pRender);
		if(hr != NOERROR)
		{
			ErrMsg(TEXT("Cannot render audio capture stream"));
			goto SetupCaptureFail;
		}
	}

	//
	// Render the closed captioning pin? It could be a CC or a VBI category pin,
	// depending on the capture driver
	//

	if(m_capInfo.fCapCC)
	{
		hr = m_capInfo.pBuilder->RenderStream(&PIN_CATEGORY_CC, NULL,
			m_capInfo.pVCap, NULL, m_capInfo.pRender);
		if(hr != NOERROR)
		{
			hr = m_capInfo.pBuilder->RenderStream(&PIN_CATEGORY_VBI, NULL,
				m_capInfo.pVCap, NULL, m_capInfo.pRender);
			if(hr != NOERROR)
			{
				ErrMsg(TEXT("Cannot render closed captioning"));
				// so what? goto SetupCaptureFail;
			}
		}
		// To preview and capture VBI at the same time, we can call this twice
		if(m_capInfo.fWantPreview)
		{
			hr = m_capInfo.pBuilder->RenderStream(&PIN_CATEGORY_VBI, NULL,
				m_capInfo.pVCap, NULL, NULL);
		}
	}

	/////////////////////////////////////////////////////
	if(m_capInfo.pSampleGrabber)
	{
		hr = m_capInfo.pSampleGrabber->SetOneShot(FALSE);
		hr = m_capInfo.pSampleGrabber->SetBufferSamples(TRUE);
	}
	///////////////////////////////////////////////////////////

	//
	// Get the preview window to be a child of our app's window
	//

	// This will find the IVideoWindow interface on the renderer.  It is
	// important to ask the filtergraph for this interface... do NOT use
	// ICaptureGraphBuilder2::FindInterface, because the filtergraph needs to
	// know we own the window so it can give us display changed messages, etc.

	if(m_capInfo.fWantPreview)
	{
		hr = m_capInfo.pFg->QueryInterface(IID_IVideoWindow, (void **)&m_capInfo.pVW);
		if(hr != NOERROR && m_capInfo.fWantPreview)
		{
			ErrMsg(TEXT("This graph cannot preview"));
		}
		else if(hr == NOERROR)
		{
			RECT rc;
			m_capInfo.pVW->put_Owner((OAHWND)m_hWndApp);    // We own the window now
			m_capInfo.pVW->put_WindowStyle(WS_CHILD);    // you are now a child

			// give the preview window all our space but where the status bar is
			GetClientRect(m_hWndApp, &rc);

			m_capInfo.pVW->SetWindowPosition(0, 0, rc.right, rc.bottom); // be this big
			m_capInfo.pVW->put_Visible(OATRUE);
		}
	}

	// now tell it what frame rate to capture at.  Just find the format it
	// is capturing with, and leave everything alone but change the frame rate
	hr = m_capInfo.fUseFrameRate ? E_FAIL : NOERROR;
	if(m_capInfo.pVSC && m_capInfo.fUseFrameRate)
	{
		hr = m_capInfo.pVSC->GetFormat(&pmt);

		// DV capture does not use a VIDEOINFOHEADER
		if(hr == NOERROR)
		{
			if(pmt->formattype == FORMAT_VideoInfo)
			{
				VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
				pvi->AvgTimePerFrame = (LONGLONG)(10000000 / m_capInfo.FrameRate);
				hr = m_capInfo.pVSC->SetFormat(pmt);
			}
			DeleteMediaType(pmt);
		}
	}
	if(hr != NOERROR)
		ErrMsg(TEXT("Cannot set frame rate for capture"));

	// now ask the filtergraph to tell us when something is completed or aborted
	// (EC_COMPLETE, EC_USERABORT, EC_ERRORABORT).  This is how we will find out
	// if the disk gets full while capturing
	hr = m_capInfo.pFg->QueryInterface(IID_IMediaEventEx, (void **)&m_capInfo.pME);
	if(hr == NOERROR)
	{
		m_capInfo.pME->SetNotifyWindow((OAHWND)m_hWndApp, WM_FGNOTIFY, 0);
	}

	// potential debug output - what the graph looks like
	// DumpGraph(m_capInfo.pFg, 1);

	// Add our graph to the running object table, which will allow
	// the GraphEdit application to "spy" on our graph
#ifdef REGISTER_FILTERGRAPH
	hr = AddGraphToRot(m_capInfo.pFg, &m_dwGraphRegister);
	if(FAILED(hr))
	{
		ErrMsg(TEXT("Failed to register filter graph with ROT!  hr=0x%x"), hr);
		m_dwGraphRegister = 0;
	}
#endif

	// All done.
	m_capInfo.fCaptureGraphBuilt = TRUE;
	return TRUE;

SetupCaptureFail:
	TearDownGraph();
	return FALSE;
}



// build the preview graph!
//
// !!! PLEASE NOTE !!!  Some new WDM devices have totally separate capture
// and preview settings.  An application that wishes to preview and then
// capture may have to set the preview pin format using IAMStreamConfig on the
// preview pin, and then again on the capture pin to capture with that format.
// In this sample app, there is a separate page to set the settings on the
// capture pin and one for the preview pin.  To avoid the user
// having to enter the same settings in 2 dialog boxes, an app can have its own
// UI for choosing a format (the possible formats can be enumerated using
// IAMStreamConfig) and then the app can programmatically call IAMStreamConfig
// to set the format on both pins.
//
BOOL AMCaputer::BuildPreviewGraph()
{
	HRESULT hr;
	AM_MEDIA_TYPE *pmt;

	// we have one already
	if(m_capInfo.fPreviewGraphBuilt)
		return TRUE;

	// No rebuilding while we're running
	if(m_capInfo.fCapturing || m_capInfo.fPreviewing)
		return FALSE;

	// We don't have the necessary capture filters
	if(m_capInfo.pVCap == NULL)
		return FALSE;
	if(m_capInfo.pACap == NULL && m_capInfo.fCapAudio)
		return FALSE;

	// we already have another graph built... tear down the old one
	if(m_capInfo.fCaptureGraphBuilt)
		TearDownGraph();

	//
	// Render the preview pin - even if there is not preview pin, the capture
	// graph builder will use a smart tee filter and provide a preview.
	//
	// !!! what about latency/buffer issues?

	// NOTE that we try to render the interleaved pin before the video pin, because
	// if BOTH exist, it's a DV filter and the only way to get the audio is to use
	// the interleaved pin.  Using the Video pin on a DV filter is only useful if
	// you don't want the audio.


	hr = m_capInfo.pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,
		&MEDIATYPE_Interleaved, m_capInfo.pVCap, NULL, NULL);
	if(hr == VFW_S_NOPREVIEWPIN)
	{
		// preview was faked up for us using the (only) capture pin
		m_capInfo.fPreviewFaked = TRUE;
	}
	else if(hr != S_OK)
	{
		// maybe it's DV?
		hr = m_capInfo.pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,
			&MEDIATYPE_Video, m_capInfo.pVCap, NULL, NULL);
		if(hr == VFW_S_NOPREVIEWPIN)
		{
			// preview was faked up for us using the (only) capture pin
			m_capInfo.fPreviewFaked = TRUE;
		}
		else if(hr != S_OK)
		{
			ErrMsg(TEXT("This graph cannot preview!"));
			m_capInfo.fPreviewGraphBuilt = FALSE;
			return FALSE;
		}
	}

	//
	// Render the closed captioning pin? It could be a CC or a VBI category pin,
	// depending on the capture driver
	//

	if(m_capInfo.fCapCC)
	{
		hr = m_capInfo.pBuilder->RenderStream(&PIN_CATEGORY_CC, NULL,
			m_capInfo.pVCap, NULL, NULL);
		if(hr != NOERROR)
		{
			hr = m_capInfo.pBuilder->RenderStream(&PIN_CATEGORY_VBI, NULL,
				m_capInfo.pVCap, NULL, NULL);
			if(hr != NOERROR)
			{
				ErrMsg(TEXT("Cannot render closed captioning"));
			}
		}
	}

	//
	// Get the preview window to be a child of our app's window
	//

	// This will find the IVideoWindow interface on the renderer.  It is
	// important to ask the filtergraph for this interface... do NOT use
	// ICaptureGraphBuilder2::FindInterface, because the filtergraph needs to
	// know we own the window so it can give us display changed messages, etc.

	hr = m_capInfo.pFg->QueryInterface(IID_IVideoWindow, (void **)&m_capInfo.pVW);
	if(hr != NOERROR)
	{
		ErrMsg(TEXT("This graph cannot preview properly"));
	}
	else
	{
		//Find out if this is a DV stream
		AM_MEDIA_TYPE * pmtDV;

		if(m_capInfo.pVSC && SUCCEEDED(m_capInfo.pVSC->GetFormat(&pmtDV)))
		{
			if(pmtDV->formattype == FORMAT_DvInfo)
			{
				// in this case we want to set the size of the parent window to that of
				// current DV resolution.
				// We get that resolution from the IVideoWindow.
				SmartPtr<IBasicVideo> pBV;

				// If we got here, m_capInfo.pVW is not NULL 
				ASSERT(m_capInfo.pVW != NULL);
				hr = m_capInfo.pVW->QueryInterface(IID_IBasicVideo, (void**)&pBV);
			}
		}

		RECT rc;
		m_capInfo.pVW->put_Owner((OAHWND)m_hWndApp);    // We own the window now
		m_capInfo.pVW->put_WindowStyle(WS_CHILD);    // you are now a child

		// give the preview window all our space but where the status bar is
		GetClientRect(m_hWndApp, &rc);

		m_capInfo.pVW->SetWindowPosition(0, 0, rc.right, rc.bottom); // be this big
		m_capInfo.pVW->put_Visible(OATRUE);
	}

	// now tell it what frame rate to capture at.  Just find the format it
	// is capturing with, and leave everything alone but change the frame rate
	// No big deal if it fails.  It's just for preview
	// !!! Should we then talk to the preview pin?
	if(m_capInfo.pVSC && m_capInfo.fUseFrameRate)
	{
		hr = m_capInfo.pVSC->GetFormat(&pmt);

		// DV capture does not use a VIDEOINFOHEADER
		if(hr == NOERROR)
		{
			if(pmt->formattype == FORMAT_VideoInfo)
			{
				VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
				pvi->AvgTimePerFrame = (LONGLONG)(10000000 / m_capInfo.FrameRate);

				hr = m_capInfo.pVSC->SetFormat(pmt);
				if(hr != NOERROR)
					ErrMsg(TEXT("%x: Cannot set frame rate for preview"), hr);
			}
			DeleteMediaType(pmt);
		}
	}

	// make sure we process events while we're previewing!
	hr = m_capInfo.pFg->QueryInterface(IID_IMediaEventEx, (void **)&m_capInfo.pME);
	if(hr == NOERROR)
	{
		m_capInfo.pME->SetNotifyWindow((OAHWND)m_hWndApp, WM_FGNOTIFY, 0);
	}

	// potential debug output - what the graph looks like
	// DumpGraph(m_capInfo.pFg, 1);

	// Add our graph to the running object table, which will allow
	// the GraphEdit application to "spy" on our graph
#ifdef REGISTER_FILTERGRAPH
	hr = AddGraphToRot(m_capInfo.pFg, &m_dwGraphRegister);
	if(FAILED(hr))
	{
		ErrMsg(TEXT("Failed to register filter graph with ROT!  hr=0x%x"), hr);
		m_dwGraphRegister = 0;
	}
#endif

	// All done.
	m_capInfo.fPreviewGraphBuilt = TRUE;
	return TRUE;
}


// Start previewing
//
BOOL AMCaputer::StartPreview()
{
	// way ahead of you
	if(m_capInfo.fPreviewing)
		return TRUE;

	m_capInfo.fWantPreview = TRUE;

	BuildPreviewGraph();

	if(!m_capInfo.fPreviewGraphBuilt)
		return FALSE;

	// run the graph
	IMediaControl *pMC = NULL;
	HRESULT hr = m_capInfo.pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if(SUCCEEDED(hr))
	{
		hr = pMC->Run();
		if(FAILED(hr))
		{
			// stop parts that ran
			pMC->Stop();
		}
		pMC->Release();
	}
	if(FAILED(hr))
	{
		ErrMsg(TEXT("Error %x: Cannot run preview graph"), hr);
		return FALSE;
	}

	m_capInfo.fPreviewing = TRUE;
	return TRUE;
}


// stop the preview graph
//
BOOL AMCaputer::StopPreview()
{
	// way ahead of you
	if(!m_capInfo.fPreviewing)
	{
		return FALSE;
	}

	// stop the graph
	IMediaControl *pMC = NULL;
	HRESULT hr = m_capInfo.pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if(SUCCEEDED(hr))
	{
		hr = pMC->Stop();
		pMC->Release();
	}
	if(FAILED(hr))
	{
		ErrMsg(TEXT("Error %x: Cannot stop preview graph"), hr);
		return FALSE;
	}
	
	if(m_capInfo.pVW)
		m_capInfo.pVW->put_Visible(OAFALSE);

	m_capInfo.fPreviewing = FALSE;

	// get rid of menu garbage
	InvalidateRect(m_hWndApp, NULL, TRUE);

	return TRUE;
}


// start the capture graph
//
BOOL AMCaputer::StartCapture(BOOL bPreview)
{
	BOOL fHasStreamControl;
	HRESULT hr;

	if(m_capInfo.fPreviewing)
		StopPreview();
	if(m_capInfo.fPreviewGraphBuilt)
		TearDownGraph();

	// way ahead of you
	if(m_capInfo.fCapturing)
		return TRUE;

	m_capInfo.fWantPreview = bPreview;

	BuildCaptureGraph();

	// or we'll crash
	if(!m_capInfo.fCaptureGraphBuilt)
		return FALSE;

	// This amount will be subtracted from the number of dropped and not
	// dropped frames reported by the filter.  Since we might be having the
	// filter running while the pin is turned off, we don't want any of the
	// frame statistics from the time the pin is off interfering with the
	// statistics we gather while the pin is on
	m_capInfo.lDroppedBase = 0;
	m_capInfo.lNotBase = 0;

	REFERENCE_TIME start = MAXLONGLONG, stop = MAXLONGLONG;

	// don't capture quite yet...
	hr = m_capInfo.pBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, NULL,
		NULL, &start, NULL, 0, 0);

	// Do we have the ability to control capture and preview separately?
	fHasStreamControl = SUCCEEDED(hr);

	// prepare to run the graph
	IMediaControl *pMC = NULL;
	hr = m_capInfo.pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if(FAILED(hr))
	{
		ErrMsg(TEXT("Error %x: Cannot get IMediaControl"), hr);
		return FALSE;
	}

	// If we were able to keep capture off, then we can
	// run the graph now for frame accurate start later yet still showing a
	// preview.   Otherwise, we can't run the graph yet without capture
	// starting too, so we'll pause it so the latency between when they
	// press a key and when capture begins is still small (but they won't have
	// a preview while they wait to press a key)

	if(fHasStreamControl)
		hr = pMC->Run();
	else
		hr = pMC->Pause();
	if(FAILED(hr))
	{
		// stop parts that started
		pMC->Stop();
		pMC->Release();
		ErrMsg(TEXT("Error %x: Cannot start graph"), hr);
		return FALSE;
	}

	// Start capture NOW!
	if(fHasStreamControl)
	{
		// we may not have this yet
		if(!m_capInfo.pDF)
		{
			hr = m_capInfo.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
				&MEDIATYPE_Interleaved, m_capInfo.pVCap,
				IID_IAMDroppedFrames, (void **)&m_capInfo.pDF);
			if(hr != NOERROR)
				hr = m_capInfo.pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
				&MEDIATYPE_Video, m_capInfo.pVCap,
				IID_IAMDroppedFrames, (void **)&m_capInfo.pDF);
		}

		// turn the capture pin on now!
		hr = m_capInfo.pBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, NULL,
			NULL, NULL, &stop, 0, 0);
		// make note of the current dropped frame counts

		if(m_capInfo.pDF)
		{
			m_capInfo.pDF->GetNumDropped(&m_capInfo.lDroppedBase);
			m_capInfo.pDF->GetNumNotDropped(&m_capInfo.lNotBase);
		}
	}
	else
	{
		hr = pMC->Run();
		if(FAILED(hr))
		{
			// stop parts that started
			pMC->Stop();
			pMC->Release();
			ErrMsg(TEXT("Error %x: Cannot run graph"), hr);
			return FALSE;
		}
	}

	pMC->Release();

	// when did we start capture?
	m_capInfo.lCapStartTime = timeGetTime();

	m_capInfo.fCapturing = TRUE;
	return TRUE;
}


// stop the capture graph
//
BOOL AMCaputer::StopCapture()
{
	// way ahead of you
	if(!m_capInfo.fCapturing)
	{
		return FALSE;
	}

	// stop the graph
	IMediaControl *pMC = NULL;
	HRESULT hr = m_capInfo.pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if(SUCCEEDED(hr))
	{
		hr = pMC->Stop();
		pMC->Release();
	}
	if(FAILED(hr))
	{
		ErrMsg(TEXT("Error %x: Cannot stop graph"), hr);
		return FALSE;
	}

	if(m_capInfo.pVW)
	   m_capInfo.pVW->put_Visible(OAFALSE);

	// when the graph was stopped
	m_capInfo.lCapStopTime = timeGetTime();

	m_capInfo.fCapturing = FALSE;

	// get rid of menu garbage
	InvalidateRect(m_hWndApp, NULL, TRUE);

	return TRUE;
}


// Check the devices we're currently using and make filters for them
//
void AMCaputer::SetDevices(int nVideoIndex,int nAudioIndex)
{
#define VERSIZE 40
#define DESCSIZE 80

	int versize = VERSIZE;
	int descsize = DESCSIZE;
	WCHAR wachVer[VERSIZE]={0}, wachDesc[DESCSIZE]={0};
	TCHAR tachStatus[VERSIZE + DESCSIZE + 5]={0};

	if(nVideoIndex >= MAX_DEVICE_NUM || nAudioIndex >= MAX_DEVICE_NUM)
		return;

	IMoniker *pmVideo = m_capInfo.rgpmVideoMenu[nVideoIndex]; 
	IMoniker *pmAudio = m_capInfo.rgpmAudioMenu[nAudioIndex];
	if(pmVideo == NULL && pmAudio == NULL)
		return;

	// they chose a new device. rebuild the graphs
	if(m_capInfo.pmVideo != pmVideo || m_capInfo.pmAudio != pmAudio)
	{
		if(pmVideo)
		{
			pmVideo->AddRef();
		}
		if(pmAudio)
		{
			pmAudio->AddRef();
		}

		IMonRelease(m_capInfo.pmVideo);
		IMonRelease(m_capInfo.pmAudio);
		m_capInfo.pmVideo = pmVideo;
		m_capInfo.pmAudio = pmAudio;

		if(m_capInfo.fPreviewing)
			StopPreview();
		if(m_capInfo.fCaptureGraphBuilt || m_capInfo.fPreviewGraphBuilt)
			TearDownGraph();

		FreeCapFilters();
		InitCapFilters();
	}
}


// put all installed video and audio devices in the menus
//
void AMCaputer::EnumerateDevices()
{
	m_capInfo.iNumVCapDevices = 0;
	m_capInfo.iNumACapDevices = 0;

	UINT    uIndex = 0;
	HRESULT hr;

	for(int i = 0; i < NUMELMS(m_capInfo.rgpmVideoMenu); i++)
	{
		IMonRelease(m_capInfo.rgpmVideoMenu[i]);

		if(m_capInfo.szVideoDevName[i] != NULL)
		{
			delete m_capInfo.szVideoDevName[i];
			m_capInfo.szVideoDevName[i] = NULL;
		}
	}
	for(int i = 0; i < NUMELMS(m_capInfo.rgpmAudioMenu); i++)
	{
		IMonRelease(m_capInfo.rgpmAudioMenu[i]);

		if(m_capInfo.szAudioDevName[i] != NULL)
		{
			delete m_capInfo.szAudioDevName[i];
			m_capInfo.szAudioDevName[i] = NULL;
		}
	}

	// enumerate all video capture devices
	ICreateDevEnum *pCreateDevEnum=0;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if(hr != NOERROR)
	{
		ErrMsg(TEXT("Error Creating Device Enumerator"));
		return;
	}

	IEnumMoniker *pEm=0;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
	if(hr != NOERROR)
	{
		ErrMsg(TEXT("Sorry, you have no video capture hardware.\r\n\r\n")
			TEXT("Video capture will not function properly."));
		goto EnumAudio;
	}

	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;

	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag=0;

		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				m_capInfo.szVideoDevName[uIndex] = new TCHAR[MAX_PATH];
				memset(m_capInfo.szVideoDevName[uIndex],0,sizeof(TCHAR));
				_tcscpy_s(m_capInfo.szVideoDevName[uIndex],MAX_PATH,var.bstrVal);

				SysFreeString(var.bstrVal);

				ASSERT(m_capInfo.rgpmVideoMenu[uIndex] == 0);
				m_capInfo.rgpmVideoMenu[uIndex] = pM;
				pM->AddRef();
			}
			pBag->Release();
		}

		pM->Release();
		uIndex++;
	}
	pEm->Release();

	m_capInfo.iNumVCapDevices = uIndex;

EnumAudio:

	// enumerate all audio capture devices
	uIndex = 0;

	/*ASSERT(pCreateDevEnum != NULL);

	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &pEm, 0);
	pCreateDevEnum->Release();
	if(hr != NOERROR)
		return;
	pEm->Reset();

	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				m_capInfo.szAudioDevName[uIndex] = new TCHAR[MAX_PATH];
				memset(m_capInfo.szAudioDevName[uIndex],0,sizeof(TCHAR));
				_tcscpy_s(m_capInfo.szAudioDevName[uIndex],MAX_PATH,var.bstrVal);

				SysFreeString(var.bstrVal);

				ASSERT(m_capInfo.rgpmAudioMenu[uIndex] == 0);
				m_capInfo.rgpmAudioMenu[uIndex] = pM;
				pM->AddRef();
			}
			pBag->Release();
		}
		pM->Release();
		uIndex++;
	}

	m_capInfo.iNumACapDevices = uIndex;

	pEm->Release();*/
}


void AMCaputer::GetVideoSize(int & nWidth,int & nHeight)
{
	if(m_iDesVideoWidth>0) {
		nWidth = m_iDesVideoWidth;
	}

	if(m_iDesVideoHeight>0) {
		nHeight = m_iDesVideoHeight;
	}
}


TCHAR*AMCaputer::GetVideoDeviceName(int index)
{
	if(index < 0 || index > m_capInfo.iNumVCapDevices)
		return NULL;

	return m_capInfo.szVideoDevName[index];
}

/*----------------------------------------------------------------------------*\
|   ErrMsg - Opens a Message box with a error message in it.  The user can     |
|            select the OK button to continue                                  |
\*----------------------------------------------------------------------------*/
void AMCaputer::ErrMsg(LPTSTR szFormat,...)
{
	static TCHAR szBuffer[2048]={0};
	const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
	const int LASTCHAR = NUMCHARS - 1;

	// Format the input string
	va_list pArgs;
	va_start(pArgs, szFormat);

	// Use a bounded buffer size to prevent buffer overruns.  Limit count to
	// character size minus one to allow for a NULL terminating character.
	HRESULT hr = StringCchVPrintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
	va_end(pArgs);

	// Ensure that the formatted string is NULL-terminated
	szBuffer[LASTCHAR] = TEXT('\0');

	//MessageBox(m_hWndApp, szBuffer, NULL,
	//MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
}















