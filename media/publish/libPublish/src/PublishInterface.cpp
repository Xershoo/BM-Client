#include "stdafx.h"
#include "publishinterface.h"
#include "rtmppublish.h"
#include "MediaConfig.h"
CRTMPPublish* g_rtmpPublish = NULL;

//��ʼ��lib���л���
PUBLISH_MODULE_API void InitLibEnv()
{
	if(g_rtmpPublish == NULL)
	{
		HWND hShowHwnd = NULL;
		g_rtmpPublish = new CRTMPPublish(hShowHwnd);
		setSpeakersMute(false);
	}
}

//����ʼ��
PUBLISH_MODULE_API void   UninitLibEnv()
{
	if(g_rtmpPublish)
	{
		delete g_rtmpPublish;
		g_rtmpPublish = NULL;
	}
}

//����:��ȡ��ǰ�豸����ͷ��Ŀ
PUBLISH_MODULE_API long   GetDevCameraCount()
{
	if(g_rtmpPublish)
	{
		return g_rtmpPublish->GetDevCameraCount();
	}
	return 0;
}

//���ܣ���ȡ���ID��Ϣ����ͷ������
//����: nCameraID id(0 <= id < GetDevCameraCount())
//		szName ��ȡ��������ͷ�����������
PUBLISH_MODULE_API bool   GetDevCameraName(int nCameraID,char szName[256])
{
	if(g_rtmpPublish)
		return g_rtmpPublish->GetDevCameraName(nCameraID,szName);
	return false;
}

//����:��ȡ��ǰ�豸��˷���Ŀ
PUBLISH_MODULE_API long   GetDevMicCount()
{
	if(g_rtmpPublish)
	{
		return g_rtmpPublish->GetDevMicCount();
	}
	return 0;
}

//���ܣ���ȡ���ID��Ϣ��˷������
//����: nCameraID id(0 <= id < GetDevCameraCount())
//		szName ��ȡ������˷������������
PUBLISH_MODULE_API bool   GetDevMicName(int nMicID,char szName[256])
{
	if(g_rtmpPublish)
		return g_rtmpPublish->GetDevMicName(nMicID,szName);
	return false;
}


//����:��ȡ��ǰ�豸��������Ŀ
PUBLISH_MODULE_API long   GetDevSpeakerCount()
{
	if(g_rtmpPublish)
	{
		return g_rtmpPublish->GetDevSpeakerCount();
	}
	return 0;
}

//���ܣ���ȡ���ID��Ϣ������������
//����: nCameraID id(0 <= id < GetDevCameraCount())
//		szName ��ȡ����������������
PUBLISH_MODULE_API bool   GetDevSpeakerName(int nSpeakerID,char szName[256])
{
	if(g_rtmpPublish)
		return g_rtmpPublish->GetDevSpeakerName(nSpeakerID,szName);
	return false;
}

PUBLISH_MODULE_API bool  SetDevSpeaker(int index)
{
	if(g_rtmpPublish)
		return g_rtmpPublish->SetDevSpeaker(index);
	return false;
}


//���ܣ�����ͷ����γ���Ϣ
//����: DeviceType,DeviceState,DeviceName, DeviceID
PUBLISH_MODULE_API bool   DeviceChangeMsg(DeviceType nDeviceType,DeviceState nDeviceState,char nDeviceName[256],int DeviceID)
{
	if(g_rtmpPublish)
		return g_rtmpPublish->DeviceChangeMsg(nDeviceType,nDeviceState,nDeviceName,DeviceID);
	return false;
}


//����
PUBLISH_MODULE_API bool   TakePhotos(const char* szUrl)
{
	if(g_rtmpPublish)
		return g_rtmpPublish->TakePhotos(szUrl);
	return false;
}

//�����ⲿ��ʾ�Ļص�����
PUBLISH_MODULE_API bool SetShowEventCallback(scEventCallBack pEC,void* dwUser)
{
	if(g_rtmpPublish == NULL)
		return false;
	return  g_rtmpPublish->SetShowEventCallback(pEC,dwUser);
}

PUBLISH_MODULE_API bool   parsePushAddrURL(const char* szRtmpPushUrl,PushAddress pa[4],int& nPaNum)
{
	if(pa == NULL || szRtmpPushUrl == NULL)
		return false;
	int i = 0;
	char szPushAddrHeader[128] = {'\0'};
	char szPushStreamID[128] = {'\0'};
	char szVideoURL[1024] = {'\0'};
	char *p = NULL;
	char *szSeparatorStream = "|&|";
	char *szSeparatorMedia = "|@|";
	p = (char*)strstr(szRtmpPushUrl,szSeparatorMedia);

	if(p != NULL)
	{
		strncpy(szVideoURL,szRtmpPushUrl,p-szRtmpPushUrl);
		szVideoURL[p-szRtmpPushUrl+1] = '\0';
		strcpy(pa[i].szPushAddr,p+strlen(szSeparatorMedia));
		pa[i++].nMediaType = AUDIOTYPE;
	}
	else
	{
		strcpy(szVideoURL,szRtmpPushUrl);
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
//����:rtmp ��ʼ��������������
//����:szRtmpPushUrl �������ĵ�ַ
//	   nStreamType �����������ͣ�
//     nRoleType ��ɫ������
//     Param ����صĲ���
//     hShowHwnd ������Ƶ����ʾ���ھ��
PUBLISH_MODULE_API bool   rtmpPushStreamToServerBegin(const char* szRtmpPushUrl,int nStreamType,PublishParam Param)
{
	OutputDebugStringA("rtmpPushStreamToServerBegin..............\n");

	if(g_rtmpPublish== NULL || szRtmpPushUrl == NULL )
		return false;
	char szVideoURL[512] = {'\0'};
	char szAudioURL[512] = {'\0'};
	char *szSplit = "|@|";
	const char *p = NULL;
	p = strstr(szRtmpPushUrl,szSplit);
	if(p == NULL)
	{
		strcpy(szVideoURL,szRtmpPushUrl);
	}
	else
	{
		strncpy(szVideoURL,szRtmpPushUrl,p-szRtmpPushUrl);
		szVideoURL[p-szRtmpPushUrl+1] = '\0';
		strcpy(szAudioURL,p+strlen(szSplit));
	}

	CMediaConfig  mc;
	StreamParam  sparam;
	mc.copyMediaParam(Param,sparam,(nStreamType&SOURCESCREEN) ==  SOURCESCREEN ? true : false);

	bool ret = false;
	if(strcmp(szAudioURL,"") == 0)
	{
		ret = g_rtmpPublish->PushStart(szRtmpPushUrl,nStreamType,sparam);
	}
	else
	{
		ret = g_rtmpPublish->PushStartEx(szVideoURL,szAudioURL,nStreamType,sparam);
	}

	if(ret)
	{
		OutputDebugStringA("rtmpPushStreamToServerBegin..............OK\n");
	}
	else
	{
		OutputDebugStringA("rtmpPushStreamToServerBegin..............failed\n");
	}

	return ret;
}


//����:rtmp ������������������
//����:szRtmpPushUrl �������ĵ�ַ
PUBLISH_MODULE_API bool   rtmpPushStreamToServerEnd(const char* szRtmpPushUrl,bool bIsColseDev)
{
	OutputDebugStringA("rtmpPushStreamToServerEnd..................");
	if(g_rtmpPublish == NULL || szRtmpPushUrl == NULL)
		return false;	

	char szVideoURL[512] = {'\0'};
	char szAudioURL[512] = {'\0'};
	char *szSplit = "|@|";
	const char *p = NULL;
	p = strstr(szRtmpPushUrl,szSplit);
	if(p == NULL)
	{
		strcpy(szVideoURL,szRtmpPushUrl);
	}
	else
	{
		strncpy(szVideoURL,szRtmpPushUrl,p-szRtmpPushUrl);
		szVideoURL[p-szRtmpPushUrl+1] = '\0';
		strcpy(szAudioURL,p+strlen(szSplit));
	}
	bool bIsAudioResult = true;
	bool bIsVideoResult = true;
	if(strcmp(szAudioURL,"") != 0)
	{
		 bIsAudioResult =  g_rtmpPublish->PushEnd(szAudioURL,bIsColseDev);
	}
	bIsVideoResult = g_rtmpPublish->PushEnd(szVideoURL,bIsColseDev);
	return (bIsAudioResult && bIsVideoResult);
}

//����: ��̬���л����������������ĸ�ʽ
//����:szRtmpPushUrl �������ĵ�ַ
//	   nStreamType �����������ͣ�
//     nRoleType ��ɫ������
//     Param ����صĲ���
//˵�����������������rtmp_PushStreamToServer_begin�������Ҫ�ı��л�����ʱ���Ե��ô˺���
PUBLISH_MODULE_API bool   rtmpPushStreamToServerChange(const char* szRtmpPushUrl,int nStreamType,PublishParam Param)
{
	if(g_rtmpPublish == NULL || szRtmpPushUrl == NULL)
		return false;	

	CMediaConfig  mc;
	StreamParam  sparam;
	mc.copyMediaParam(Param,sparam,(nStreamType&SOURCESCREEN) ==  SOURCESCREEN ? true : false);

	char szVideoURL[512] = {'\0'};
	char szAudioURL[512] = {'\0'};
	char *szSplit = "|@|";
	const char *p = NULL;
	p = strstr(szRtmpPushUrl,szSplit);
	if(p == NULL)
	{
		strcpy(szVideoURL,szRtmpPushUrl);
	}
	else
	{
		strncpy(szVideoURL,szRtmpPushUrl,p-szRtmpPushUrl);
		szVideoURL[p-szRtmpPushUrl+1] = '\0';
		strcpy(szAudioURL,p+strlen(szSplit));
	}
	if(strcmp(szAudioURL,"") == 0)
		return  g_rtmpPublish->PushChange(szRtmpPushUrl,nStreamType,sparam);
	else
		return  g_rtmpPublish->PushChangeEx(szVideoURL,szAudioURL,nStreamType,sparam);
}


//�ƶ�ͼƬ�����ڵ��ƶ�λ��λ��
//����:szRtmpPushUrl �������ĵ�ַ
//     nIndex �±�
//     nStreamType �±��Ӧ��������
//     showRect  ���ڵ�����

PUBLISH_MODULE_API bool   moveImageShowRect(const char* szRtmpPushUrl,int nIndex,int nStreamType,ScRect showRect)
{
	if(g_rtmpPublish == NULL || szRtmpPushUrl == NULL)
		return false;	

	char szVideoURL[512] = {'\0'};
	char szAudioURL[512] = {'\0'};
	char *szSplit = "|@|";
	const char *p = NULL;
	p = strstr(szRtmpPushUrl,szSplit);
	if(p == NULL)
	{
		strcpy(szVideoURL,szRtmpPushUrl);
	}
	else
	{
		strncpy(szVideoURL,szRtmpPushUrl,p-szRtmpPushUrl);
		szVideoURL[p-szRtmpPushUrl+1] = '\0';
		strcpy(szAudioURL,p+strlen(szSplit));
	}

	return g_rtmpPublish->moveImageShowRect(szVideoURL,nIndex,nStreamType,showRect);
}

PUBLISH_MODULE_API bool   rtmpPushFileToServerBegin(const char* szLocalFile,char* szRtmpServerURL,int nUseDrawToolType,HWND hShowHwnd)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL)
		return false;	
	if(szRtmpServerURL && strncmp(szRtmpServerURL,"rtmp://",strlen("rtmp://")) != 0)
		return false;

	return g_rtmpPublish->PushFileToServerBegin(szLocalFile,szRtmpServerURL,nUseDrawToolType,hShowHwnd);
}

//����:�����������ͺͲ����ļ�
//������szLocalFile ��������Ƶ�ļ�
PUBLISH_MODULE_API bool   rtmpPushFileToServerEnd(const char* szLocalFile)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL )
		return false;	
	return g_rtmpPublish->PushFileToServerEnd(szLocalFile);
}

PUBLISH_MODULE_API bool   rtmpPushFileSwitch(const char* szCurPlayLocalFileName)
{
	if(g_rtmpPublish == NULL || szCurPlayLocalFileName == NULL)
		return false;	
	return g_rtmpPublish->PushFileSwitch(szCurPlayLocalFileName);
}
//�ƶ������ļ���λ��
//������szLocalFile ��������Ƶ�ļ�
//		nSeekTime   seek�����ļ�λ��
//����ֵ�����ŵ�ʱ���ܳ��ȵ�λ��
PUBLISH_MODULE_API bool rtmpPushFileSeek(const char* szLocalFile,unsigned int nSeekTime)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL)
		return false;	
	return g_rtmpPublish->PushFileSeek(szLocalFile,nSeekTime);
}

//ȡ���ļ���d��ǰ���ŵ�ʱ��
//������szLocalFile ��������Ƶ�ļ�
//����ֵ�����ŵ�ʱ���ܳ��ȵ�λ��
PUBLISH_MODULE_API unsigned int getPushFileCurPlayTime(const char* szLocalFile)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL)
		return false;	
	return g_rtmpPublish->GetPushFileCurPlayTime(szLocalFile);
}

//ȡ���ļ����ܵĲ���ʱ�䳤��
//������szLocalFile ��������Ƶ�ļ�
//����ֵ�����ŵ�ʱ���ܳ��ȵ�λ��
PUBLISH_MODULE_API unsigned int getPushFileDuration(const char* szLocalFile)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL)
		return false;	
	return g_rtmpPublish->GetPushFileDuration(szLocalFile);
}

//��ͣ/�ָ�
//������szLocalFile ��������Ƶ�ļ�
//		bIsPause   ��ͣ(true)/�ָ�(flase)
PUBLISH_MODULE_API bool rtmpPushFilePause(const char* szLocalFile,bool bIsPause)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL)
		return false;	
	return g_rtmpPublish->PushFilePause(szLocalFile,bIsPause);
}


//��ʼ�汾�����͵����ݵ����ص�FLV�ļ�
PUBLISH_MODULE_API bool  savePushDataToLocalFlvFileBegin(const char* szLocalFile,HWND hRecordWindow)
{
	if(g_rtmpPublish == NULL || szLocalFile == NULL)
		return false;
	int nLen = strlen(szLocalFile) - 4;
	if(strcmp(szLocalFile+nLen,".flv") != 0)
		return false;

	return g_rtmpPublish->PushDataToLocalFlvFileBegin(szLocalFile,hRecordWindow);

}

//�����汾�ļ�
PUBLISH_MODULE_API bool  savePushDataToLocalFlvFileEnd()
{
	if(g_rtmpPublish == NULL)
		return false;
	return g_rtmpPublish->PushDataToLocalFlvFileEnd();
}

//���ñ�����˷�ľ���
//������ vol true��ʾ�������� false ��ʾ�رվ���
PUBLISH_MODULE_API bool setMicMute(bool vol)
{
	if(g_rtmpPublish == NULL)
		return false;
	return g_rtmpPublish->setMicMute(vol);
}

//���ñ�����˷������
//������ vol��Χ:1--100
PUBLISH_MODULE_API bool setMicVolume(long vol)
{
	if(g_rtmpPublish == NULL)
		return false;
	return g_rtmpPublish->setMicVolume(vol);
}

//��ȡ������˷������
//������ vol��Χ:1--100
PUBLISH_MODULE_API long getMicVolume()
{
	if(g_rtmpPublish == NULL)
		return 0;
	return g_rtmpPublish->getMicVolume();
}

//��ȡʵʱ��˷������
//������ vol��Χ:1--128
PUBLISH_MODULE_API int GetMicRTVolum()
{
	if(g_rtmpPublish == NULL)
		return false;
	return g_rtmpPublish->GetMicRTVolum();
}

//��ȡ�������������ŵ�����
//������vol��ʾ������С��0-100��
PUBLISH_MODULE_API long getSpeakersVolume()
{
	if(g_rtmpPublish == NULL)
		return 0;
	return g_rtmpPublish->getSpeakersVolume();
}

//���ñ������������ŵľ���
//������ vol true��ʾ�������� false ��ʾ�رվ���
PUBLISH_MODULE_API bool setSpeakersMute(bool vol)
{
	if(g_rtmpPublish == NULL)
		return false;
	return g_rtmpPublish->setSpeakersMute(vol);
}

//���ñ������������ŵ�����
//������vol��ʾ������С��0-100��
PUBLISH_MODULE_API bool setSpeakersVolume(long vol)
{
	if(g_rtmpPublish == NULL && (vol >= 0 && vol <= 100))
		return false;
	return g_rtmpPublish->setSpeakersVolume(vol);
}

//��ȡ��˷��豸��״̬
PUBLISH_MODULE_API MDevStatus getMicDevStatus()
{
	if(g_rtmpPublish == NULL)
		return NODEV;
	return g_rtmpPublish->getMicDevStatus();
}

//��ȡ��˷�״̬
PUBLISH_MODULE_API MDevStatus getCamDevStatus()
{
	if(g_rtmpPublish == NULL)
		return NODEV;
	return g_rtmpPublish->getCamDevStatus();
}

//��ȡ������״̬
PUBLISH_MODULE_API MDevStatus getSpeakersDevStatus()
{
	if(g_rtmpPublish == NULL)
		return NODEV;
	return g_rtmpPublish->getSpeakersDevStatus();
}

//���ûص�����
PUBLISH_MODULE_API bool SetEventCallback(scEventCallBack pEC,void* dwUser)
{
	if(g_rtmpPublish == NULL)
		return false;
	return  g_rtmpPublish->SetEventCallback(pEC,dwUser);
}

//��������
PUBLISH_MODULE_API  bool PreviewSound(int nMicID,bool bIsPreview)
{
	if(g_rtmpPublish == NULL)
		return false;
	return  g_rtmpPublish->PreviewSound(nMicID,bIsPreview);
}

//ת���ļ�
PUBLISH_MODULE_API bool TrancodeFile(const char* szSourFileName,const char* szDestFileName,char* szErr)
{
	if(g_rtmpPublish == NULL)
		return false;
	return  g_rtmpPublish->TrancodeFile(szSourFileName,szDestFileName,szErr);
}

//ת���ļ�
PUBLISH_MODULE_API bool StopTrancodeFile()
{
	if(g_rtmpPublish == NULL)
		return false;
	return  g_rtmpPublish->StopTrancodeFile();
}