#include "stdafx.h"
#include "AudioEngine.h"
#include "common_macro.h"

CAudioEngine* CAudioEngine::m_instance = NULL;

VOIVE_WAS_INTERFACE_API IWasVoiceEngine* CreateWasVoiceEngine()
{
	return (IWasVoiceEngine*)CAudioEngine::createInstance();
}

CAudioEngine* CAudioEngine::createInstance()
{
	if(NULL == m_instance)
	{
		m_instance = new CAudioEngine();
	}

	return m_instance;
}

void CAudioEngine::freeInstance()
{	
	SAFE_DELETE(m_instance);
}

CAudioEngine::CAudioEngine():m_audioDeviceEnumer(NULL)
	,m_audioSpkCollect(NULL)
	,m_audioMicCollect(NULL)
	,m_refCount(1)
{
	::CoInitialize(NULL);

	initialize();
}

CAudioEngine::~CAudioEngine()
{
	release();

	::CoUninitialize();
}

bool CAudioEngine::initialize()
{
	bool done = false;
	HRESULT hr = S_OK;

	do 
	{
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&m_audioDeviceEnumer);
		if (FAILED(hr)) 
		{
			break;
		}

		hr = m_audioDeviceEnumer->EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE,&m_audioSpkCollect);
		if (FAILED(hr)) 
		{
			break;
		}

		hr = m_audioDeviceEnumer->EnumAudioEndpoints(eCapture,DEVICE_STATE_ACTIVE,&m_audioMicCollect);
		if (FAILED(hr)) 
		{
			break;
		}

		initMicrophoneList();
		initSpeakerList();

		done = true;
	} while (false);

	if(!done)
	{
		release();
	}

	return done;
}

void CAudioEngine::release()
{
	freeAudioImplList(m_listMicrophoeImpl);
	freeAudioImplList(m_listSpeakerImpl);
	freeAudioImplList(m_listPlaybackImpl);

	SAFE_RELEASE(m_audioDeviceEnumer);
	SAFE_RELEASE(m_audioSpkCollect);
	SAFE_RELEASE(m_audioMicCollect);
}

int CAudioEngine::getMicCount()
{
	if(NULL==m_audioMicCollect)
	{
		return 0;
	}

	UINT nCount = 0;
	HRESULT hr = m_audioMicCollect->GetCount(&nCount);
	if(FAILED(hr))
	{
		return 0;
	}
	
	return nCount;
}

int CAudioEngine::getSpkCount()
{
	if(NULL==m_audioSpkCollect)
	{
		return 0;
	}

	UINT nCount = 0;
	HRESULT hr = m_audioSpkCollect->GetCount(&nCount);
	if(FAILED(hr))
	{
		return 0;
	}

	return nCount;
}

void CAudioEngine::getSpkName(int index,wchar_t* name,int length)
{
	getDeviceName(m_audioSpkCollect,index,name,length);
	return;
}

void CAudioEngine::getMicName(int index,wchar_t* name,int length)
{
	getDeviceName(m_audioMicCollect,index,name,length);
	return;
}

void CAudioEngine::getDeviceName(IMMDeviceCollection* devCol,int index,wchar_t* name,int length)
{
	if(NULL == devCol || NULL == name || length <= 0)
	{
		return;
	}

	IMMDevice* device = NULL;
	IPropertyStore * propertyStore = NULL;
	PROPVARIANT varDevName;

	HRESULT hr = devCol->Item(index,&device);
	if(FAILED(hr) || NULL == device)
	{
		return;
	}
	
	PropVariantInit(&varDevName);

	do 
	{
		HRESULT hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
		if (FAILED(hr)||NULL ==propertyStore)
		{
			break;				
		}


		hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &varDevName);
		if(FAILED(hr))
		{
			break;
		}

		wcscpy_s(name,length,varDevName.pwszVal);
	} while (false);

	PropVariantClear(&varDevName);
	SAFE_RELEASE(propertyStore);
	SAFE_RELEASE(device);

	return;
}

int CAudioEngine::getDefaultMic()
{
	return getDefaultDevice(m_audioMicCollect,eCapture);
}

int CAudioEngine::getDefaultSpk()
{
	return getDefaultDevice(m_audioSpkCollect,eRender);
}

UINT CAudioEngine::getDefaultDevice(IMMDeviceCollection* devCol,EDataFlow eFlow)
{
	if(NULL == m_audioDeviceEnumer || NULL == devCol)
	{
		return 0;
	}
		
	UINT nCount = 0;
	HRESULT hr = devCol->GetCount(&nCount);
	if (FAILED(hr) || 0>=nCount)
	{
		return 0;
	}

	IMMDevice* defDevice = NULL;
	WCHAR defDevId[MAX_PATH] = { 0 };

	hr = m_audioDeviceEnumer->GetDefaultAudioEndpoint(eFlow,eConsole,&defDevice);
	if(FAILED(hr) || NULL == defDevice)
	{
		return 0;
	}

	getDeviceId(defDevice,defDevId,MAX_PATH);

	for(UINT i=0;i<nCount;i++)
	{
		WCHAR wszDevId[MAX_PATH] = {0};
		getDeviceId(devCol,i,wszDevId,MAX_PATH);
		
		if(wcscmp(defDevId,wszDevId) == 0)
		{	
			return i;
		}
	}

	return 0;
}

void CAudioEngine::getDeviceId(IMMDevice* device,LPWSTR deviceId,int lengthId)
{
	if(NULL==device||NULL==deviceId||0>=lengthId)
	{
		return;
	}

	LPWSTR pwszDeviceId = NULL;
	HRESULT hr = device->GetId(&pwszDeviceId);	
	if(SUCCEEDED(hr)&&NULL!=pwszDeviceId)
	{
		wcscpy_s(deviceId,lengthId,pwszDeviceId);
		CoTaskMemFree(pwszDeviceId);
	}

	SAFE_RELEASE(device);
}

void CAudioEngine::getDeviceId(IMMDeviceCollection* devCol,UINT deviceIndex,LPWSTR deviceId,int lengthId)
{
	if(NULL==devCol)
	{
		return;
	}

	IMMDevice* device = NULL;
	HRESULT hr = devCol->Item(deviceIndex,&device);
	if(FAILED(hr))
	{
		return;
	}

	getDeviceId(device,deviceId,lengthId);
}

void CAudioEngine::setDefaultMic(int index)
{
	WCHAR deviceId[MAX_PATH]={0};
	getDeviceId(m_audioMicCollect,index,deviceId,MAX_PATH);
	setDefaultDevice(deviceId);

	return;
}

void CAudioEngine::setDefaultSpk(int index)
{
	WCHAR deviceId[MAX_PATH]={0};
	getDeviceId(m_audioSpkCollect,index,deviceId,MAX_PATH);
	setDefaultDevice(deviceId);

	return;
}

void CAudioEngine::setDefaultDevice(LPCWSTR devID)
{
	if(NULL==devID)
	{
		return;
	}

	IPolicyConfigVista *pPolicyConfigVista = NULL;
	IPolicyConfig *pPolicyConfig = NULL;	

	HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigClient),NULL, CLSCTX_ALL, __uuidof(IPolicyConfig), (LPVOID *)&pPolicyConfig);
	if (SUCCEEDED(hr))
	{
		pPolicyConfig->SetDefaultEndpoint(devID, eConsole);
		pPolicyConfig->SetDefaultEndpoint(devID, eMultimedia);
		pPolicyConfig->SetDefaultEndpoint(devID, eCommunications);
		pPolicyConfig->Release();

		return;
	}

	hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient),NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfigVista);
	if (SUCCEEDED(hr))
	{
		pPolicyConfigVista->SetDefaultEndpoint(devID, eConsole);
		pPolicyConfigVista->SetDefaultEndpoint(devID, eMultimedia);
		pPolicyConfigVista->SetDefaultEndpoint(devID, eCommunications);
		pPolicyConfigVista->Release();

		return;
	}

	return;
}

IWasAudioImpl* CAudioEngine::getMicAudioImpl(int index)
{
	if(NULL == m_audioMicCollect || NULL == m_audioDeviceEnumer)
	{
		return NULL;
	}

	WCHAR wszCurDevId[MAX_PATH] = { 0 };	
	IMMDevice * pAudioDevice = NULL;
	CAudioImplBase* pAudioImpl = NULL;

	getDeviceId(m_audioMicCollect,index,wszCurDevId,MAX_PATH);	
	pAudioImpl = findAudioImpl(m_listMicrophoeImpl,wszCurDevId);
	if(NULL != pAudioImpl)
	{
		pAudioImpl->AddRef();
		return pAudioImpl;
	}

	HRESULT hr = m_audioMicCollect->Item(index,&pAudioDevice);
	if(FAILED(hr) || NULL == pAudioDevice)
	{
		return NULL;
	}

	pAudioImpl = new CAudioRecord(pAudioDevice);
	if(NULL!=pAudioImpl)
	{
		pAudioImpl->AddRef();
		m_audioDeviceEnumer->RegisterEndpointNotificationCallback(pAudioImpl);
		m_listMicrophoeImpl.push_back(pAudioImpl);
	}
		
	return pAudioImpl;
}

IWasAudioImpl* CAudioEngine::getSpkAudioImpl(int index)
{
	if(NULL == m_audioSpkCollect || NULL == m_audioDeviceEnumer)
	{
		return NULL;
	}
	
	/**** render create multi sample,for auto mixer......
	WCHAR wszCurDevId[MAX_PATH] = { 0 };	
	IMMDevice * pAudioDevice = NULL;
	CAudioImplBase* pAudioImpl = NULL;
	
	getDeviceId(m_audioSpkCollect,index,wszCurDevId,MAX_PATH);	
	pAudioImpl = findAudioImpl(m_listSpeakerImpl,wszCurDevId);
	if(NULL != pAudioImpl)
	{
		pAudioImpl->AddRef();
		return pAudioImpl;
	}
	*/

	IMMDevice * pAudioDevice = NULL;
	CAudioImplBase* pAudioImpl = NULL;

	HRESULT hr = m_audioSpkCollect->Item(index,&pAudioDevice);
	if(FAILED(hr) || NULL == pAudioDevice)
	{	
		return NULL;
	}

	pAudioImpl = new CAudioRender(pAudioDevice);
	if(NULL!=pAudioImpl)
	{
		pAudioImpl->AddRef();
		m_audioDeviceEnumer->RegisterEndpointNotificationCallback(pAudioImpl);
		m_listSpeakerImpl.push_back(pAudioImpl);
	}
		
	return pAudioImpl;
}

IWasAudioImpl* CAudioEngine::getPlaybackImpl(int spkIndex)
{
	if(NULL == m_audioSpkCollect || NULL == m_audioDeviceEnumer)
	{
		return NULL;
	}

	WCHAR wszCurDevId[MAX_PATH] = { 0 };	
	IMMDevice * pAudioDevice = NULL;
	CAudioImplBase* pAudioImpl = NULL;

	getDeviceId(m_audioSpkCollect,spkIndex,wszCurDevId,MAX_PATH);	
	pAudioImpl = findAudioImpl(m_listPlaybackImpl,wszCurDevId);
	if(NULL != pAudioImpl)
	{
		pAudioImpl->AddRef();
		return pAudioImpl;
	}

	HRESULT hr = m_audioSpkCollect->Item(spkIndex,&pAudioDevice);
	if(FAILED(hr) || NULL == pAudioDevice)
	{		
		return NULL;
	}

	pAudioImpl = new CPlaybackRecord(pAudioDevice);
	if(NULL!=pAudioImpl)
	{		
		pAudioImpl->AddRef();

		m_audioDeviceEnumer->RegisterEndpointNotificationCallback(pAudioImpl);
		m_listPlaybackImpl.push_back(pAudioImpl);
	}
		
	return pAudioImpl;
}

void CAudioEngine::initMicrophoneList()
{
	if(NULL==m_audioMicCollect)
	{
		return;
	}

	UINT nCount = 0;
	HRESULT hr = m_audioMicCollect->GetCount(&nCount);
	if(FAILED(hr))
	{
		return;
	}

	for(UINT i=0;i<nCount;i++)
	{
		IMMDevice * pDevice = NULL;
		hr = m_audioMicCollect->Item(i,&pDevice);
		if(FAILED(hr)||NULL == pDevice)
		{
			continue;
		}

		CAudioRecord* audioRecord = new CAudioRecord(pDevice);
		if(NULL==audioRecord)
		{
			continue;
		}

		m_audioDeviceEnumer->RegisterEndpointNotificationCallback(audioRecord);
		m_listMicrophoeImpl.push_back(audioRecord);
	}
}

void CAudioEngine::initSpeakerList()
{
	if(NULL==m_audioSpkCollect)
	{
		return;
	}

	UINT nCount = 0;
	HRESULT hr = m_audioSpkCollect->GetCount(&nCount);
	if(FAILED(hr))
	{
		return;
	}

	for(UINT i=0;i<nCount;i++)
	{
		IMMDevice * pDevice = NULL;
		hr = m_audioSpkCollect->Item(i,&pDevice);
		if(FAILED(hr)||NULL == pDevice)
		{
			continue;
		}

		CAudioRender* audioRender = new CAudioRender(pDevice);
		if(NULL==audioRender)
		{
			continue;
		}

		int channels = 0;
		int sampleBit = 0;
		int sampleRate = 0;

		audioRender->getDeviceParam(channels,sampleBit,sampleRate);
		audioRender->setParam(channels,sampleBit,sampleRate);
		audioRender->open();

		m_audioDeviceEnumer->RegisterEndpointNotificationCallback(audioRender);
		m_listSpeakerImpl.push_back(audioRender);
	}
}

void CAudioEngine::freeAudioImplList(CAudioImplList& listAudioImpl)
{
	while(!listAudioImpl.empty())
	{
		CAudioImplBase* pAudioImpl = *listAudioImpl.begin(); 
		if(NULL != pAudioImpl)
		{
			m_audioDeviceEnumer->UnregisterEndpointNotificationCallback(pAudioImpl);
		}

		SAFE_RELEASE(pAudioImpl);
		
		listAudioImpl.pop_front();		
	}
}

CAudioImplBase* CAudioEngine::findAudioImpl(CAudioImplList& listAudioImpl,LPWSTR devId)
{
	if(NULL==devId || listAudioImpl.empty())
	{
		return NULL;
	}

	CAudioImplBase * pAudioImpl = NULL;
	for(CAudioImplList::iterator itr = listAudioImpl.begin();itr != listAudioImpl.end();itr++)
	{
		IMMDevice* pDevice = NULL;
		WCHAR wszDeviceId[MAX_PATH] = {0};

		pAudioImpl = *itr;
		if(NULL == pAudioImpl)
		{
			continue;
		}

		pDevice = pAudioImpl->getDevice();
		getDeviceId(pDevice,wszDeviceId,MAX_PATH);
		if(wcscmp(devId,wszDeviceId) == 0)
		{
			return pAudioImpl;
		}
	}

	return NULL;
}

int CAudioEngine::getMicIndex(LPWSTR pwszDevId)
{
	if(NULL==pwszDevId || NULL == pwszDevId[0])
	{
		return -1;
	}

	for(int i=0;i<getMicCount();i++)
	{
		WCHAR wszId[MAX_PATH] = { 0 };
		getDeviceId(m_audioMicCollect,i,wszId,MAX_PATH);

		if(wcscmp(pwszDevId,wszId) == 0)
		{
			return i;
		}
	}

	return -1;
}
int CAudioEngine::getSpkIndex(LPWSTR pwszDevId)
{
	if(NULL==pwszDevId || NULL == pwszDevId[0])
	{
		return -1;
	}

	for(int i=0;i<getSpkCount();i++)
	{
		WCHAR wszId[MAX_PATH] = { 0 };
		getDeviceId(m_audioSpkCollect,i,wszId,MAX_PATH);

		if(wcscmp(pwszDevId,wszId) == 0)
		{
			return i;
		}
	}

	return -1;
}