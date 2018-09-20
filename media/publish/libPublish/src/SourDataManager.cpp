#include <stdafx.h>
#include "SourDataManager.h"

inline bool IsXPHigher()
{
	static bool bIsXPHigher = false;
	static bool bIsInited = false;

	// 取得当前系统版本
	OSVERSIONINFO VersionInfo = {0};
	VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&VersionInfo);

	// 判断并返回
	bIsXPHigher = VersionInfo.dwMajorVersion > 5;
	bIsInited = TRUE;
	return bIsXPHigher;
}

CSourDataManager::CSourDataManager()
{
	int  m_nCamNum = 0;
	m_listSour.clear();
	CoInitialize(NULL);
	InitDevSourManager();

}

CSourDataManager::~CSourDataManager()
{
	UninitDevSourManager();
	m_nCamNum = 0;
	CoUninitialize();
	
}

void CSourDataManager::clearSour()
{
	list<SourceNode>::iterator iter;
	for(iter = m_listSour.begin(); iter != m_listSour.end();iter++)
	{
		ISourData *psd = (*iter).pSD;
		if(psd)
		{
			psd->SourClose(true);
			delete psd;
		    (*iter).pSD = NULL;
		}
	}
	m_listSour.clear();
}

bool CSourDataManager::InitDevSourManager()
{
	StreamParam param;
	//liw
	CSourCamera * pCamera = new CSourCamera(NULL,640,480,"carame");
	if(pCamera == NULL)
	{
		return false;
	}

	pCamera->SetSourType(SOURCECAMERA);
	pCamera->SourOpen(&param);

	SourceNode sN;
	sN.pSD = (ISourData *)pCamera;
	sN.nMediaType = MEDIACAMERAVIODE;
	sN.nUserType = PUSHTONET;
	sN.nIndex = 0;
	m_listSour.push_back(sN);

	long nCount  = GetDevCameraCount();
	
	if(nCount > 1)
	{
		for(int i = 1 ;i <nCount;i++ )
		{
			CSourCamera * pCamera = new CSourCamera(NULL,640,480,"carame");
			if(pCamera == NULL)
			{
				clearSour();
				return false;
			}
			pCamera->SetSourType(SOURCECAMERA);
			pCamera->SourOpen(&param);
			SourceNode sN;
			sN.pSD = (ISourData *)pCamera;
			sN.nMediaType = MEDIACAMERAVIODE;
			sN.nUserType = PUSHTONET;
			sN.nIndex = i;
			m_listSour.push_back(sN);
		}
	}
    
	CSourAudio *pAudio = new CSourAudio("audio");
	if(pAudio == NULL)
	{
		clearSour();
		return false;
	}

	pAudio->SetSourType(SOURCEDEVAUDIO);
	pAudio->SourOpen(&param);
	sN.pSD = (ISourData *)pAudio;
	sN.nMediaType = MEDIAMICAUDIO;
	sN.nUserType = PUSHTONET;
	sN.nIndex = 0;
	m_listSour.push_back(sN);
	
    nCount  = GetDevMicCount();

    if(nCount > 1)
    {
        for(int i = 1 ;i <nCount;i++ )
        {
            CSourAudio *pAudio = new CSourAudio("audio");
            if(pAudio == NULL)
            {
                clearSour();
                return false;
            }

            pAudio->SetSourType(SOURCEDEVAUDIO);            
            sN.pSD = (ISourData *)pAudio;
            sN.nMediaType = MEDIAMICAUDIO;
            sN.nUserType = PUSHTONET;
            sN.nIndex = i;
            m_listSour.push_back(sN);
        }
    }

   CSourAudio *pKAudio = new CSourAudio("audio-kalaok");
    if(pKAudio == NULL)
    {
        clearSour();
        return false;
    }

    param.bAudioKaraoke = 1;
    pKAudio->SetSourType(SOURCEDEMUSIC);
	if(IsXPHigher())
	  pKAudio->SourOpen(&param);
    sN.pSD = (ISourData *)pKAudio;
    sN.nMediaType = MEDIASPEKAUDIO;
    sN.nUserType = PUSHTONET;
    sN.nIndex = 0;
    m_listSour.push_back(sN); 

	CSourScreen * pScreen =  new CSourScreen(NULL,"fullscreen");
	if(pScreen == NULL)
	{
		clearSour();
		return false;
	}

	sN.pSD = (ISourData *)pScreen;
	sN.nMediaType = MEDIASCREENVIODE;
	sN.nUserType = PUSHTONET;
	m_listSour.push_back(sN);
	return false;
}

bool CSourDataManager::UninitDevSourManager()
{
	clearSour();
	return false;
}

//功能：获取当前设备摄像头设备信息
long CSourDataManager::GetDevCameraCount()
{
	list<SourceNode>::iterator iter;
	for(iter = m_listSour.begin();iter != m_listSour.end();iter++)
	{
		ISourData* pSour = (*iter).pSD;
		if(pSour->GetSourType() == SOURCECAMERA)
		{
			return ((CSourCamera*)pSour)->GetDevCameraCount();
		}	
	}
	return 0;
}

bool CSourDataManager::GetDevCameraName(long nIndex,char szName[256])
{
	list<SourceNode>::iterator iter;
	for(iter = m_listSour.begin();iter != m_listSour.end();iter++)
	{
		ISourData* pSour = (*iter).pSD;
		if(pSour->GetSourType() == SOURCECAMERA)
		{
			((CSourCamera*)pSour)->GetDevCameraName(nIndex,szName);
			 return true;
		}
			
	}
	return false;
}

//功能:获取当前设备麦克风数目
long   CSourDataManager::GetDevMicCount()
{
	list<SourceNode>::iterator iter;
	for(iter = m_listSour.begin();iter != m_listSour.end();iter++)
	{
		if((*iter).nMediaType == MEDIAMICAUDIO)
		{
			ISourData* pSour = (*iter).pSD;
			return ((CSourAudio*)pSour)->GetDevMicCount();
		}	
	}
	return 0;
}

bool   CSourDataManager::GetDevMicName(int nMicID,char szName[256])
{
	list<SourceNode>::iterator iter;
	for(iter = m_listSour.begin();iter != m_listSour.end();iter++)
	{
		if((*iter).nMediaType == MEDIAMICAUDIO)
		{
			ISourData* pSour = (*iter).pSD;
			return ((CSourAudio*)pSour)->GetDevMicName(nMicID,szName);
		}	
	}
	return 0;
}

	//功能:获取当前设备麦克风数目
long   CSourDataManager::GetDevSpeakerCount()
{
	list<SourceNode>::iterator iter;
	for(iter = m_listSour.begin();iter != m_listSour.end();iter++)
	{
		if((*iter).nMediaType == MEDIAMICAUDIO)
		{
			ISourData* pSour = (*iter).pSD;
			return ((CSourAudio*)pSour)->GetDevSpeakerCount();
		}	
	}
	return 0;
}

bool   CSourDataManager::GetDevSpeakerName(int nSpeakerID,char szName[256])
{
	list<SourceNode>::iterator iter;
	for(iter = m_listSour.begin();iter != m_listSour.end();iter++)
	{
		if((*iter).nMediaType == MEDIAMICAUDIO)
		{
			ISourData* pSour = (*iter).pSD;
			return ((CSourAudio*)pSour)->GetDevSpeakerName(nSpeakerID,szName);
		}	
	}
	return 0;
}

bool  CSourDataManager::SetDevSpeaker(int index)
{
	list<SourceNode>::iterator iter;
	for(iter = m_listSour.begin();iter != m_listSour.end();iter++)
	{
		if((*iter).nMediaType == MEDIAMICAUDIO)
		{
			ISourData* pSour = (*iter).pSD;
			return ((CSourAudio*)pSour)->SetDevOutPut(index);
		}	
	}
	return 0;
}


bool   CSourDataManager::DeviceChangeMsg(DeviceType nDeviceType,DeviceState nDeviceState,char nDeviceName[256],int DeviceID)
{
	return 1;
}

ISourData* CSourDataManager::getSourData(int nMediaType,int nIndex)
{
	ISourData* psd = NULL;
	list<SourceNode>::iterator iter;
	for(iter = m_listSour.begin();iter != m_listSour.end();iter++)
	{
		if(((*iter).nMediaType == nMediaType) && ((*iter).nIndex == nIndex) )
		{
			psd = (*iter).pSD;
			break;
		}	
	}
	return psd;
}