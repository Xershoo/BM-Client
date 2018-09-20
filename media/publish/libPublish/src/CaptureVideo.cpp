// CAPTURE_HAHA.cpp : implementation file
//

#include "stdafx.h"
#include "CaptureVideo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaptureVideo

CCaptureVideo::CCaptureVideo()
{
	m_hWnd = NULL;
	m_pVW = NULL;
	m_pGB = NULL;
	m_pMC = NULL;
	m_pCapGB = NULL;
	m_pSG = NULL;

	m_lVideoWidth  = 0;
	m_lVideoHeight = 0;
	m_fVideoFPS = 0.0f;

	memset(m_szVideoDevName,0,sizeof(m_szVideoDevName));
	m_nVideoDevNum = EnumDevices();
}

CCaptureVideo::~CCaptureVideo()
{
	Stop();

	for(int i=0; i<MAX_DEVICE_NUM; i++)
	{
		if(m_szVideoDevName[i] != NULL)
		{
			delete m_szVideoDevName[i];
			m_szVideoDevName[i] = NULL;
		}
	}
}

void CCaptureVideo::GetVideoSize(int & nWidth,int & nHeight)
{
	nWidth = m_lVideoWidth;
	nHeight = m_lVideoHeight;
}

HRESULT CCaptureVideo::InitCaptureGraphBuilder()
{
	HRESULT hr = CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder,(void**)&m_pGB);
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2,NULL,CLSCTX_INPROC_SERVER,
		IID_ICaptureGraphBuilder2,(void**)&m_pCapGB);
	if (FAILED(hr))
		return hr;
	m_pCapGB->SetFiltergraph(m_pGB);
	hr = m_pGB->QueryInterface(IID_IMediaControl,(void**)&m_pMC);
	if (FAILED(hr))
		return hr;
	hr = m_pGB->QueryInterface(IID_IVideoWindow,(void**)&m_pVW);  
	return hr;
}

bool CCaptureVideo::BindFilter(int deviceID, IBaseFilter **ppBF)
{
	if(deviceID<0)
		return false;
	CComPtr<ICreateDevEnum> pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum,NULL,
		CLSCTX_INPROC_SERVER,IID_ICreateDevEnum,(void**)&pCreateDevEnum);
	if(hr!=NOERROR)
		return false;
	CComPtr<IEnumMoniker> pEM;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEM,0);
	if(hr!=NOERROR)
		return false;
	pEM->Reset();
	ULONG cFetched;
	IMoniker *pM;
	int index = 0;
	while(S_OK==pEM->Next(1,&pM,&cFetched) && index<=deviceID)
	{
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0,0,IID_IPropertyBag,(void**)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName",&var,NULL);
			if(hr==NOERROR)
			{
				if(index==deviceID)
					pM->BindToObject(0,0,IID_IBaseFilter,(void**)ppBF);
				SysFreeString(var.bstrVal);
			}
			pBag->Release();  //注意释放位置
		}
		pM->Release();
		index++;
	}
	return true;
}

int CCaptureVideo::EnumDevices()
{
	int id = 0;
	CComPtr<ICreateDevEnum> pCreateDevEnum=NULL;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum,NULL,
		CLSCTX_INPROC_SERVER,IID_ICreateDevEnum,(void**)&pCreateDevEnum);
	if(hr != NOERROR)
		return 0;
	CComPtr<IEnumMoniker> pEM=NULL;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEM,0);
	if(hr != NOERROR)
		return 0;
	pEM->Reset();
	ULONG cFetched;
	IMoniker *pM=NULL;
	while(hr = pEM->Next(1, &pM, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr)) 
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR) 
			{
				m_szVideoDevName[id] = new TCHAR[MAX_PATH];
				memset(m_szVideoDevName[id],0,sizeof(TCHAR));
				_tcscpy_s(m_szVideoDevName[id],MAX_PATH,var.bstrVal);

				SysFreeString(var.bstrVal);  

				id++;
			}
			pBag->Release();
		}
		pM->Release();
	}
	return id;
}

void CCaptureVideo::FreeMediaType(AM_MEDIA_TYPE &mt)
{
	if (mt.cbFormat != 0) 
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL) 
	{
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}


HRESULT CCaptureVideo::SetupVideoWindow()
{
	HRESULT hr;
	hr = m_pVW->put_Owner((OAHWND)m_hWnd);
	if (FAILED(hr))
		return hr;
	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	if (FAILED(hr))
		return hr;

	if (m_pVW)//让图像充满整个窗口
	{
		CRect rc;
		::GetClientRect(m_hWnd,&rc);
		m_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);
	}
	hr = m_pVW->put_Visible(OATRUE);
	return hr;
}


HRESULT CCaptureVideo::Start(int iDeviceID, HWND hWnd)
{
	HRESULT hr = InitCaptureGraphBuilder();
	if(FAILED(hr))
	{
		AfxMessageBox(L"Failed to InitCaptureGraphBuilder");
		return hr;
	}

	CComPtr<IBaseFilter> pCap_Filter,pSG_Filter;
//------------------------------------------------------------------处理第一个Filter
	if(!BindFilter(iDeviceID,&pCap_Filter))	
		return S_FALSE;
//------------------------------------------------------------------处理第二个Filter
	CoCreateInstance(CLSID_SampleGrabber,NULL,CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void**)&pSG_Filter); 
	hr = pSG_Filter->QueryInterface(IID_ISampleGrabber,(void**)&m_pSG);
	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt,sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
	hr = m_pSG->SetMediaType(&mt);
	if(FAILED(hr))
	{
		AfxMessageBox(L"Failed to SetMediaType");
		return hr;
	}
//------------------------------------------------------------------将filters加入filter graph
	m_pGB->AddFilter(pCap_Filter,L"Capture_Filter");
	m_pGB->AddFilter(pSG_Filter,L"SG_Filter");
	if(FAILED(hr))
	{
		AfxMessageBox(L"Failed to  AddFilter");
		return hr;
	}
//------------------------------------------------------------------智能连接filters
	hr = m_pCapGB->RenderStream(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,pCap_Filter,pSG_Filter,NULL);
	// 如果最后的一个参数设置为NULL，这个方法就自动的为graph设置一个renderer filter。如果是视频就设置成Video Renderer，如果是音频就设置为DirectSoundRenderer
	if(FAILED(hr))
	{
		AfxMessageBox(L"Failed to RenderStream");
		m_pCapGB->RenderStream(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video,pCap_Filter,pSG_Filter,NULL);
	}

	hr = m_pSG->SetOneShot(FALSE);
	hr = m_pSG->SetBufferSamples(TRUE);

	//----------------------------------------------获取mt
	m_pSG->GetConnectedMediaType(&mt);
	if ( FAILED(hr))
	{
		AfxMessageBox(L"Failed to read the connected media type");
		return hr;
	}

	if(mt.formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER *vih = (VIDEOINFOHEADER*)mt.pbFormat;
		if(vih)
		{
			m_lVideoHeight = vih->bmiHeader.biHeight;
			m_lVideoWidth = vih->bmiHeader.biWidth;
			if(vih->AvgTimePerFrame > 0)
			   m_fVideoFPS = (float)(10000000 / vih->AvgTimePerFrame);
		}
	}


	FreeMediaType(mt);

	m_hWnd = hWnd;
	SetupVideoWindow();
	if(m_pMC)
		hr = m_pMC->Run();
	
	return S_OK;
}


HRESULT CCaptureVideo::Stop()
{
	HRESULT hr = S_FALSE;
	if(m_pMC)
	{
		hr = m_pMC->Stop();
	}

	srelease(m_pCapGB);
	srelease(m_pMC);
	srelease(m_pGB);
	srelease(m_pSG);
	srelease(m_pVW);

	return hr;
}

TCHAR*CCaptureVideo::GetVideoDeviceName(int index)
{
	if(index < 0 || index > m_nVideoDevNum)
		return NULL;

	return m_szVideoDevName[index];
}



