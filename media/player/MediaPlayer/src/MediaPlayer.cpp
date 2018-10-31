#include "stdafx.h"
#include "quit.h"


struct PlayUnit
{
	bool bsamplerate;
	char szPlayMediaUrl[1024];
	char szPlayAudioUrl[512];
	void* pPlayProcess;
	PlayUnit()
	{
		bsamplerate = false;
		szPlayMediaUrl[0] = '\0';
		szPlayAudioUrl[0] = '\0';
		pPlayProcess = NULL;
	}

	bool PUCmp(const char* szMediaUrl,const char* szAudioUrl)
	{
		bool bMResult = false;
		bool bAResult = false;
		if(szMediaUrl)
		{
			if(strcmp(szPlayMediaUrl,szMediaUrl) == 0)
			{
				bMResult =  true;
			}
		}

		if(szAudioUrl)
		{
			if(strcmp(szPlayAudioUrl,szAudioUrl) == 0)
			{
				bAResult =  true;
			}
		}
		return bMResult&&bAResult;
	}
};

CMutexLock     g_gStopUnitLock;
StopUnit       g_QuitStopUnit[MAXPLAYERNUM*2];

list<PlayUnit> m_glistPlayUnit;
CMutexLock     m_gPlayUnitLock;

bool		   g_initPlay = false;

MPLATER_API bool   AVP_Init()
{
	g_initPlay = true;
	return true;
}

MPLATER_API void   AVP_Unit()
{
	g_initPlay = false;
}

void* FindPlayUnit(const char* szVideoURL,const char*szAudioURL)
{
	m_gPlayUnitLock.Lock();
	list<PlayUnit>::iterator iter;
	for(iter = m_glistPlayUnit.begin(); iter != m_glistPlayUnit.end();iter++)
	{
		PlayUnit pu = (*iter);
		if(pu.PUCmp(szVideoURL,szAudioURL))
		{
			m_gPlayUnitLock.Unlock();
			return pu.pPlayProcess;
		}
	}
	m_gPlayUnitLock.Unlock();
	return NULL;
}

void PutPlayUnit(const char* szVideoURL,const char*szAudioURL,void* pUser,bool bs)
{
	PlayUnit pu;
	pu.bsamplerate = bs;
	pu.pPlayProcess = pUser;
	strcpy(pu.szPlayAudioUrl,szAudioURL);
	strcpy(pu.szPlayMediaUrl,szVideoURL);
	m_gPlayUnitLock.Lock();
	m_glistPlayUnit.push_back(pu);
	m_gPlayUnitLock.Unlock();
}

void* ErasePlayUnit(const char* szVideoURL,const char*szAudioURL)
{
	m_gPlayUnitLock.Lock();
	list<PlayUnit>::iterator iter;
	for(iter = m_glistPlayUnit.begin(); iter != m_glistPlayUnit.end();iter++)
	{
		PlayUnit pu = (*iter);
		if(pu.PUCmp(szVideoURL,szAudioURL))
		{
			m_glistPlayUnit.erase(iter);
			m_gPlayUnitLock.Unlock();
			return pu.pPlayProcess;
		}
	}
	m_gPlayUnitLock.Unlock();
	return NULL;
}

MPLATER_API bool   AVP_parsePalyAddrURL(const char* szPlayUrl,PlayAddress pa[4],int& nPaNum)
{
	if(pa == NULL || szPlayUrl == NULL)
		return false;
	int i = 0;
	char szPushAddrHeader[128] = {'\0'};
	char szPushStreamID[128] = {'\0'};
	char szVideoURL[1024] = {'\0'};
	char *p = NULL;
	char *szSeparatorStream = "|&|";
	char *szSeparatorMedia = "|@|";
	p = (char*)strstr(szPlayUrl,szSeparatorMedia);

	if(p != NULL)
	{
		strncpy(szVideoURL,szPlayUrl,p-szPlayUrl);
		szVideoURL[p-szPlayUrl+1] = '\0';
		strcpy(pa[i].szPushAddr,p+strlen(szSeparatorMedia));
		pa[i++].nMediaType = AUDIOTYPE;
	}
	else
	{
		strcpy(szVideoURL,szPlayUrl);
	}

	p = (char*)strstr(szVideoURL,szSeparatorStream);
	if(p)
	{
		strcpy(pa[i].szPushAddr,szVideoURL);
		strncpy(pa[i].szPushAddr,szVideoURL,p-szVideoURL);
		pa[i].szPushAddr[p-szVideoURL] = '\0';
		pa[i++].nMediaType = VIDEOTYPE;
		char *pp = p;
		p = p + strlen(szSeparatorStream);
		if(p)
		{
			
			int n = 0;
			int nLen = strlen(pa[i-1].szPushAddr);
			n = nLen;
			while(n > 0)
			{
				if(*pp == '\/')
				{
					strncpy(szPushAddrHeader,szVideoURL,pp-szVideoURL);
					szPushAddrHeader[pp-szVideoURL +1] = '\0';
					break;
				}
				pp--;
				n--;
			}
			char *q = p;
			char *Temp = p;
			p = strstr(q,szSeparatorStream);
			if(p)
			{
				strncpy(szPushStreamID,q,p-q);
				szPushStreamID[p-q+1] = '\0';
				sprintf(pa[i].szPushAddr,"%s/%s",szPushAddrHeader,szPushStreamID);
				pa[i++].nMediaType = VIDEOTYPE;
				p = p + strlen(szSeparatorStream);
				strcpy(szPushStreamID,p);
				sprintf(pa[i].szPushAddr,"%s/%s",szPushAddrHeader,szPushStreamID);
				pa[i++].nMediaType = VIDEOTYPE;
			}
			else
			{
				strcpy(szPushStreamID,q);
				sprintf(pa[i].szPushAddr,"%s/%s",szPushAddrHeader,szPushStreamID);
				pa[i++].nMediaType = VIDEOTYPE;
			}
		}
		
	}
	else
	{
		strcpy(pa[i].szPushAddr,szVideoURL);
		pa[i++].nMediaType = VIDEOTYPE;
	}
	nPaNum = i;
	return true;
}

MPLATER_API bool   AVP_Play(const char* szPlayUrl,int nPlayStreamType,PlayAddress arrAddress[4],int narrAddressNum, HIBITMAP hBGbitmap, IMediaPlayerEvent* pCallback)
{
	if(szPlayUrl == NULL)
		return false;

	if(strncmp("rtmp://",szPlayUrl,strlen("rtmp://")) != 0 
	   &&strncmp("udp://",szPlayUrl,strlen("udp://")) != 0 
	   && strncmp("http://",szPlayUrl,strlen("http://")) != 0 )
	   return false;

	CPlayProcess * rp = NULL;
	char szVideoURL[1024] = {'\0'};
	char szAudioURL[512] = {'\0'};
	char *szSplit = "|@|";
	const char *p = NULL;
	p = strstr(szPlayUrl,szSplit);
	if(p == NULL)
	{
		strcpy(szVideoURL,szPlayUrl);
	}
	else
	{
		strncpy(szVideoURL,szPlayUrl,p-szPlayUrl);
		szVideoURL[p-szPlayUrl+1] = '\0';
		strcpy(szAudioURL,p+strlen(szSplit));
	}

	if((rp = (CPlayProcess*)FindPlayUnit(szVideoURL,szAudioURL)) == NULL)
	{
		rp = new CPlayProcess();
		PutPlayUnit(szVideoURL,szAudioURL,rp,false);
	}

	if(rp)
	{
		rp->SetCallBack(szPlayUrl,pCallback);
		for(int i = 0;i<narrAddressNum;i++)
		{
			bool bIsPlay = true;
			int nTemp = (arrAddress[i].nMediaType & AUDIOTYPE);
			if(nTemp == AUDIOTYPE)
			{
				int nTempType = nPlayStreamType & AUDIOTYPE;
				if(nTempType != AUDIOTYPE)
					bIsPlay = false;
			}
			rp->PlayMedia(arrAddress[i].szPushAddr,arrAddress[i].nMediaType,arrAddress[i].hwnd,hBGbitmap,bIsPlay,arrAddress[i].nUserID,arrAddress[i].bIsMainVideo,arrAddress[i].bIsStudent,pCallback);
		}
	}
	return true;
}

//播放
MPLATER_API bool   AVP_Change(const char* szPlayUrl,int nPlayStreamType,PlayAddress arrAddress[4],int narrAddressNum,HIBITMAP hBGbitmap)
{
	char szVideoURL[1024] = {'\0'};
	char szAudioURL[512] = {'\0'};
	char *szSplit = "|@|";
	const char *p = NULL;
	p = strstr(szPlayUrl,szSplit);
	if(p == NULL)
	{
		strcpy(szVideoURL,szPlayUrl);
	}
	else
	{
		strncpy(szVideoURL,szPlayUrl,p-szPlayUrl);
		szVideoURL[p-szPlayUrl+1] = '\0';
		strcpy(szAudioURL,p+strlen(szSplit));
	}
	CPlayProcess * rp = NULL;

	if((rp = (CPlayProcess*)FindPlayUnit(szVideoURL,szAudioURL)))
	{
		for(int i = 0;i<narrAddressNum;i++)
		{
			bool bIsPlay = true;

			int nTemp = (arrAddress[i].nMediaType & AUDIOTYPE);
			if(nTemp == AUDIOTYPE)
			{
				int nTempType = nPlayStreamType & AUDIOTYPE;
				if(nTempType != AUDIOTYPE)
					bIsPlay = false;
			}
			if(!rp->ChangeMedia(arrAddress[i].szPushAddr,arrAddress[i].nMediaType,arrAddress[i].hwnd,hBGbitmap,bIsPlay,arrAddress[i].nUserID,arrAddress[i].bIsMainVideo,arrAddress[i].bIsStudent))
			{
				return false;
			}
		}
	}
	return true;
}
//ֹͣ
MPLATER_API bool   AVP_Stop(const char* szPlayUrl,PlayAddress arrAddress[4],int narrAddressNum)
{
	if(szPlayUrl == NULL)
		return false;
	char szVideoURL[512] = {'\0'};
	char szAudioURL[512] = {'\0'};
	char *szSplit = "|@|";
	const char *p = NULL;
	p = strstr(szPlayUrl,szSplit);
	if(p == NULL)
	{
		strcpy(szVideoURL,szPlayUrl);
	}
	else
	{
		strncpy(szVideoURL,szPlayUrl,p-szPlayUrl);
		szVideoURL[p-szPlayUrl+1] = '\0';
		strcpy(szAudioURL,p+strlen(szSplit));
	}
	CPlayProcess * rp = NULL;
	if((rp = (CPlayProcess*)ErasePlayUnit(szVideoURL,szAudioURL)) == NULL)
	{
		return false;
	}

	for(int n = 0;n < narrAddressNum;n++)
	{        
		rp->SetCallBack(szPlayUrl,NULL);
		rp->StopShowVideo(arrAddress[n].szPushAddr);
		if(rp->GetShowHandCount(arrAddress[n].szPushAddr)  > 1)
		{
			rp->StopMedia(arrAddress[n].szPushAddr,arrAddress[n].hwnd);
			//PutPlayUnit(szVideoURL,szAudioURL,rp,false);
		}
	}

	int i = 0;
	g_gStopUnitLock.Lock();
	for(i = 0; i< MAXPLAYERNUM*2 ; i++)
	{
		if(g_QuitStopUnit[i].nIndex == -1)
		{
			g_QuitStopUnit[i].nIndex = i;
			g_QuitStopUnit[i].hwnd = NULL;
			g_QuitStopUnit[i].rp = rp;
			break;
		}
	}
	g_gStopUnitLock.Unlock();			
	g_QuitStopUnit[i].stop();
	return true;
}

//播放文件
MPLATER_API bool   AVP_PlayFile(const char* szFileName,HWNDHANDLE hwnd,HIBITMAP hBGbitmap, IMediaPlayerEvent* pCallback)
{
	CPlayProcess * rp = NULL;

	if((rp = (CPlayProcess*)FindPlayUnit(szFileName,"")) == NULL)
	{
		rp = new CPlayProcess();
		PutPlayUnit(szFileName,"",rp,false);
	}

	if(NULL == rp)
	{
		return false;
	}
	
	rp->SetCallBack(szFileName,pCallback);
	return rp->PlayFile(szFileName,hwnd,hBGbitmap);
}

//停止播放文件
MPLATER_API bool   AVP_StopFile(const char* szFileName,HWNDHANDLE hwnd)
{
	CPlayProcess * rp = NULL;
	if((rp = (CPlayProcess*)ErasePlayUnit(szFileName,"")) == NULL)
	{
		return false;
	}
        
	rp->SetCallBack(szFileName,NULL);
	rp->stopFile(szFileName);
	
	int i = 0;
	g_gStopUnitLock.Lock();
	for(i = 0; i< MAXPLAYERNUM*2 ; i++)
	{
		if(g_QuitStopUnit[i].nIndex == -1)
		{
			g_QuitStopUnit[i].nIndex = i;
			g_QuitStopUnit[i].hwnd = (HWND)hwnd;
			g_QuitStopUnit[i].rp = rp;
			break;
		}
	}
	g_gStopUnitLock.Unlock();			
	g_QuitStopUnit[i].stop();
	return true;
}

//暂停
MPLATER_API bool   AVP_PauseFile(const char* szFileName,bool bIsPause)
{
	CPlayProcess * rp = NULL;
	rp = (CPlayProcess*)FindPlayUnit(szFileName,"");
	if(rp)
	{
		return rp->PauseFile(szFileName,bIsPause);
	}
	return false;
}

//seek 文件
MPLATER_API bool   AVP_SeekFile(const char* szFileName,unsigned int nPalyPos)
{
	CPlayProcess * rp = NULL;
	rp = (CPlayProcess*)FindPlayUnit(szFileName,"");
	if(rp)
	{
		return rp->SeekFile(szFileName,nPalyPos);
	}
	return false;
}

//seek 文件
MPLATER_API bool   AVP_SeekFileStream(const char* szFileName,unsigned int nPlayPos,bool bVideo)
{
	CPlayProcess * rp = NULL;
	rp = (CPlayProcess*)FindPlayUnit(szFileName,"");
	if(rp)
	{
		return rp->SeekFileStream(szFileName,nPlayPos,bVideo);
	}
	return false;
}

//取得文件的总的播放时间长度
//参数：szFileName 本地音视频文件
//返回值：播放的时间总长度单位秒
MPLATER_API unsigned int    AVP_GetFileDuration(const char* szFileName)
{
	CPlayProcess * rp = NULL;
	rp = (CPlayProcess*)FindPlayUnit(szFileName,"");
	if(rp)
	{
		return rp->GetFileDuration(szFileName);
	}
	return 0;
}

//取得文件的当前播放的时间
//参数：szLocalFile 本地音视频文件
//返回值：播放的时间总长度单位秒
MPLATER_API unsigned int getFileCurPlayTime(const char* szFileName)
{
	CPlayProcess * rp = NULL;
	rp = (CPlayProcess*)FindPlayUnit(szFileName,"");
	if(rp)
	{
		return rp->getFileCurPlayTime(szFileName);
	}
	return 0;
}

MPLATER_API unsigned int getFileStreamCurTime(const char* szFileName,bool bVideo)
{
	CPlayProcess * rp = NULL;
	rp = (CPlayProcess*)FindPlayUnit(szFileName,"");
	if(rp)
	{
		return rp->getFileStreamCurTime(szFileName,bVideo);
	}
	return 0;
}

MPLATER_API bool  playFileSwitch(const char* szCurPlayLocalFileName)
{
	m_gPlayUnitLock.Lock();
	CPlayProcess * rp = NULL;
	list<PlayUnit>::iterator iter;
	for(iter = m_glistPlayUnit.begin(); iter != m_glistPlayUnit.end();iter++)
	{
		PlayUnit pu = (*iter);
		CPlayProcess * rpTemp = (CPlayProcess *)pu.pPlayProcess;
		if(pu.pPlayProcess && rpTemp->getIsPlayFile())
		{
			if(pu.PUCmp(szCurPlayLocalFileName,""))
			{				
				rp = rpTemp;
			}
			else
			{
				rpTemp->PauseFile(pu.szPlayMediaUrl,true);
				rpTemp->SwitchPaly(pu.szPlayMediaUrl);
			}
		}
	}

	m_gPlayUnitLock.Unlock();
	if(rp)
	{
		bool bIsOK = rp->SwitchPaly(szCurPlayLocalFileName,true);
		if(bIsOK)
		{
			return rp->PauseFile(szCurPlayLocalFileName,false);
		}
	}
	return true;
}

MPLATER_API bool  bIsExistVideoStream(const char* szCurPlayLocalFileName)
{
	CPlayProcess * rp = NULL;
	rp = (CPlayProcess*)FindPlayUnit(szCurPlayLocalFileName,"");
	if(rp)
	{
		return rp->bIsHaveVideo(szCurPlayLocalFileName);
	}
	return false;
}