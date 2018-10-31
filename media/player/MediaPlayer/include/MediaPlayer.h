#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

typedef void* HIBITMAP;
typedef void*  HWNDHANDLE;

#define VIDEOTYPE 0x10000000
#define AUDIOTYPE 0x20000000

#define MPLATER_API  extern "C" __declspec(dllexport)

struct IMediaPlayerEvent
{
	virtual void ControlPaint() = 0;//���������ײ�����꽻���ϲ��ٻ�����Ӧ����
	virtual void ShowVideo(void* ShowHandle,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH) = 0;
};

struct  PlayAddress
{
	int nMediaType;
	char szPushAddr[256];
	HWNDHANDLE hwnd;
	bool   bIsMainVideo;
	bool   bIsStudent;
	long   nUserID;
	PlayAddress()
	{
		szPushAddr[0] = '\0';
		nMediaType = 0;
		hwnd = NULL;
		bIsMainVideo = false;
		bIsStudent = true;
		nUserID = 0;
	}
};


//xiewb 2017.3.1
MPLATER_API bool   AVP_Init();
MPLATER_API void   AVP_Unit();

MPLATER_API bool   AVP_parsePalyAddrURL(const char* szPlayUrl,PlayAddress pa[4],int& nPaNum);

//����
MPLATER_API bool   AVP_Play(const char* szPlayUrl,int nPlayStreamType,PlayAddress arrAddress[4],int narrAddressNum, HIBITMAP hBGbitmap, IMediaPlayerEvent* pCallback);

//����
MPLATER_API bool   AVP_Change(const char* szPlayUrl,int nPlayStreamType,PlayAddress arrAddress[4],int narrAddressNum,HIBITMAP hBGbitmap);

//ֹͣ
MPLATER_API bool   AVP_Stop(const char* szPlayUrl,PlayAddress arrAddress[4],int narrAddressNum);


//�����ļ�
MPLATER_API bool   AVP_PlayFile(const char* szFileName,HWNDHANDLE hwnd,HIBITMAP hBGbitmap, IMediaPlayerEvent* pCallback);

//ֹͣ�����ļ�
MPLATER_API bool   AVP_StopFile(const char* szFileName,HWNDHANDLE hwnd);

//��ͣ
MPLATER_API bool   AVP_PauseFile(const char* szFileName,bool bIsPause);

//seek �ļ�
MPLATER_API bool   AVP_SeekFile(const char* szFileName,unsigned int  nPalyPos);
MPLATER_API bool   AVP_SeekFileStream(const char* szFileName,unsigned int  nPlayPos,bool bVideo);

//ȡ���ļ����ܵĲ���ʱ�䳤��
//������szFileName ��������Ƶ�ļ�
//����ֵ�����ŵ�ʱ���ܳ��ȵ�λ��
MPLATER_API unsigned int  AVP_GetFileDuration(const char* szFileName);

//ȡ���ļ��ĵ�ǰ���ŵ�ʱ��
//������szLocalFile ��������Ƶ�ļ�
//����ֵ�����ŵ�ʱ���ܳ��ȵ�λ��
MPLATER_API unsigned int  getFileCurPlayTime(const char* szFileName);
MPLATER_API unsigned int  getFileStreamCurTime(const char* szFileName,bool bVideo);


MPLATER_API bool  playFileSwitch(const char* szCurPlayLocalFileName);

MPLATER_API bool  bIsExistVideoStream(const char* szCurPlayLocalFileName);
#endif