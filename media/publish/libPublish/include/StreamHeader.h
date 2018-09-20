#ifndef STREAMHEADER_H
#define STREAMHEADER_H

#include "globalconfig.h"

//
#include "ave_global.h"
//#ifdef __cplusplus



#if defined LIBPUBLISH_EXPORTS
	#define MD_EXPORTS __declspec(dllexport)
#else 
	#define MD_EXPORTS __declspec(dllimport)
#endif 

#define VIDEOTYPE 0x10000000
#define AUDIOTYPE 0x20000000

#define SOURCECAMERA (VIDEOTYPE|0x01)   //����ͷ��Ƶ
#define SOURCESCREEN (VIDEOTYPE|0x02)   //������Ƶ
#define SOURCEDEVAUDIO (AUDIOTYPE|0x04) //��Ƶ
#define SOURCEDEMUSIC (AUDIOTYPE|0x08) //����������Ƶ

//DrawToolType
#define DRAWIAGER_SDL 1// sdl ��ʾͼ��
#define DRAWIAGER_DX  2// dx��ʾ

enum SHOWSCALE
{
	SHOWSCALE4A3 = 1,
	SHOWSCALE16A9= 2
};

enum MDevStatus
{
	NORMAL = 0, //����
	MUTE = 1,   //����
	DEVERR = 2, //�豸�쳣
	NODEV = 3,   //�豸������
	CAMFORBID = 4, //����ͷ����ֹ
	MICFORBID   //��˷类��ֹ
};

enum EventInfoType
{
	EVENT_HANDUP = 0,//����
	EVENT_CANCLEHANDUP = 1, //ȡ������
	TAKE_PHOTOS = 2, //����
	TRANCODE_FILE = 3,//ת��
	EVENT_SHOW = 4 //��ʾ
};

enum MediaLevel  //ý��ļ���
{
	SMOOTHLEVEL = 0, //����
	STANDARRDLEVEL = 1, //����
	HIGHLEVEL = 2, //����
	SUPERHIGHLEVEL = 3 //����
};

enum AudioLevel //��Ƶ����
{
	AUDIOLOW = 0,
	AUDIONORMAL = 1,
	AUDIOMID = 2,
	AUDIOHIGH = 3
};

enum MediaRole //ý���ɫ
{
	ANCHORROLE = 0,//����
	LISTENERROLE = 1 //����
};

enum MediabBusiness //ý��ҵ��
{
	EDUCATION = 0,//����
	RECREATION = 1//����
};

enum MasterSlave //���ӻ�λ
{
	MAINCAMERA=0,//����λ
	SUBCAMERA=1,//cong��λ
};


enum DeviceType //�豸����
{
	Type_MICROPHONE = 1, //��˷�
	Type_CAMERA          //����ͷ
};

enum DeviceState //�豸״̬
{
	State_ARRIVAL = 1, //����
	State_REMOVAL      //�γ�
};

struct ScRect
{
	int nTop;
	int nBottom;
	int nRight;
	int nLeft;
	ScRect()
	{
		nTop = 0;
		nBottom = 0;
		nRight = 0;
		nLeft = 0;
	}
};

struct VideoUnit
{
	int nSelectCameraID;//ѡ���id(0 <= id < GetDevCameraCount())
	int nType;
	ScRect showRect;

	int  nParam[2];
	int  nVEnMainID; 
	int  nVEnSubID;
	VideoUnit()
	{
		nType = SOURCECAMERA;
		nSelectCameraID = 0;//ѡ���id(0 <= id < GetDevCameraCount())
		memset(&showRect,0,sizeof(showRect));

		nParam[0] = 0;
		nParam[1] = 0;
		nVEnMainID = 0; 
	    nVEnSubID = 0;
	}
};

struct PublishParam
{
	MediaLevel ml; //ý��ļ���
	MediaRole mr; //��ɫ
	MediabBusiness mb; //ҵ��
	MasterSlave ms; //����
	AudioLevel  al;//��Ƶ����
	SHOWSCALE   scale;//��ʾ����

	bool bIsManyCamera; //���ӽ�
	VideoUnit  VU[4]; //��Ƶ��Ԫ
	int        nVUNum;
	HWND       hShowHwnd; //��ʵ���

	bool bIsHighAudio; //�Ƿ���Ҫ��������Ƶ
	int  nSelectMicID; //mic id
	bool  bAudioKaraoke; //�Ƿ���Ҫ����
	
	bool bIsFullScreen; //�����Ƿ�ȫ��
	bool bDrawLocRect; //��ʾ����
	bool bIsPublish;   //�Ƿ���������

	PublishParam()
	{
		ml = SMOOTHLEVEL;
		mr = ANCHORROLE;
		mb = EDUCATION;
		ms = MAINCAMERA;
		al = AUDIONORMAL;
		scale = SHOWSCALE4A3;
		bIsManyCamera = false;
		nVUNum  = 0;
		hShowHwnd = NULL;
		bIsHighAudio = true;
		nSelectMicID = 0;
		bIsPublish = true;
		bAudioKaraoke = false;
		bIsFullScreen =true;
		bDrawLocRect = false;
	}
};

struct  PushAddress
{
	int nMediaType;
	char szPushAddr[256];
	PushAddress()
	{
		szPushAddr[0] = '\0';
		nMediaType = 0;
	}
};

struct TakePhptpsParam
{
	int nImageWidth;
	int nImageHeight;
	TakePhptpsParam()
	{
		nImageWidth = 0;
		nImageHeight = 0;
	}
};

struct ShowVideoParam
{
	int nImageWidth;
	int nImageHeight;
	HWND hShowHwnd;   //��ʾ���
	ShowVideoParam()
	{
		nImageWidth = 0;
		nImageHeight = 0;
		hShowHwnd = NULL;
	}
};

struct TrancodeFileParam
{
	int  nCurTrancodePos; //��ǰת���λ��
	int  nTrancodeLen; //�ļ�ת����Ҫ���ܳ���

	TrancodeFileParam()
	{
		nCurTrancodePos = 0;
		nTrancodeLen = 0;

	}
};




//�¼��ص��������ص�����
typedef void (*scEventCallBack)(EventInfoType enmuType,void* dwUser,unsigned char* pBuf,unsigned int nBufSize,void* Param);

#endif