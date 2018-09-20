#ifndef PUBLISHINTERFACE_H
#define PUBLISHINTERFACE_H

#define PUBLISH_MODULE_API  extern "C" MD_EXPORTS

#include "StreamHeader.h"



//��ʼ��lib���л���
PUBLISH_MODULE_API void   InitLibEnv();

//����ʼ��
PUBLISH_MODULE_API void   UninitLibEnv();

//����:��ȡ��ǰ�豸����ͷ��Ŀ
PUBLISH_MODULE_API long   GetDevCameraCount();

//���ܣ���ȡ���ID��Ϣ����ͷ������
//����: nCameraID id(0 <= id < GetDevCameraCount())
//		szName ��ȡ��������ͷ�����������
PUBLISH_MODULE_API bool   GetDevCameraName(int nCameraID,char szName[256]);


//����:��ȡ��ǰ�豸��˷���Ŀ
PUBLISH_MODULE_API long   GetDevMicCount();

//���ܣ���ȡ���ID��Ϣ��˷������
//����: nCameraID id(0 <= id < GetDevCameraCount())
//		szName ��ȡ��������ͷ�����������
PUBLISH_MODULE_API bool   GetDevMicName(int nMicID,char szName[256]);

//����:��ȡ��ǰ�豸��������Ŀ
PUBLISH_MODULE_API long   GetDevSpeakerCount();

//���ܣ���ȡ���ID��Ϣ������������
//����: nCameraID id(0 <= id < GetDevCameraCount())
//		szName ��ȡ����������������
PUBLISH_MODULE_API bool   GetDevSpeakerName(int nSpeakerID,char szName[256]);


PUBLISH_MODULE_API bool   SetDevSpeaker(int index);

//���ܣ�����ͷ����γ���Ϣ
//����: DeviceType,DeviceState,DeviceName, DeviceID
PUBLISH_MODULE_API bool   DeviceChangeMsg(DeviceType nDeviceType,DeviceState nDeviceState,char nDeviceName[256],int DeviceID);

//����
PUBLISH_MODULE_API bool   TakePhotos(const char* szUrl);


//�����ⲿ��ʾ�Ļص�����
PUBLISH_MODULE_API bool SetShowEventCallback(scEventCallBack pEC,void* dwUser);


PUBLISH_MODULE_API bool   parsePushAddrURL(const char* szRtmpPushUrl,PushAddress pa[4],int& nPaNum);
//����:rtmp ��ʼ��������������
//����:szRtmpPushUrl �������ĵ�ַ
//	   nStreamType �����������ͣ�
//     nRoleType ��ɫ������
//     Param ����صĲ���
PUBLISH_MODULE_API bool   rtmpPushStreamToServerBegin(const char* szRtmpPushUrl,int nStreamType,PublishParam Param);

//����:rtmp ������������������
//����:szRtmpPushUrl �������ĵ�ַ
PUBLISH_MODULE_API bool   rtmpPushStreamToServerEnd(const char* szRtmpPushUrl,bool bIsColseDev = false);

//����: ��̬���л����������������ĸ�ʽ
//����:szRtmpPushUrl �������ĵ�ַ
//	   nStreamType �����������ͣ�
//     nRoleType ��ɫ������
//     Param ����صĲ���
//˵�����������������rtmp_PushStreamToServer_begin�������Ҫ�ı��л�����ʱ���Ե��ô˺���
PUBLISH_MODULE_API bool   rtmpPushStreamToServerChange(const char* szRtmpPushUrl,int nStreamType,PublishParam Param);

//�ƶ�ͼƬ�����ڵ��ƶ�λ��λ��
//����:szRtmpPushUrl �������ĵ�ַ
//     nIndex �±�
//     nStreamType �±��Ӧ��������
//     showRect  ���ڵ�����

PUBLISH_MODULE_API bool   moveImageShowRect(const char* szRtmpPushUrl,int nIndex,int nStreamType,ScRect showRect);

//���ܣ����ͱ����ļ���Զ�̷�������ͬʱ���ز���
//������szLocalFile ��������Ƶ�ļ�
//      szRtmpServerURL Զ�������ķ�������ַ������ΪNULL�����Ͳ�����
//      nUseDrawToolType  ��ʾͼ���������Ĺ��� DRAWIAGER_SDL ���� DRAWIAGER_DX
//      hShowHwnd   �ļ����ز��ŵĴ��ھ��������ΪNULL��ΪNULL���Զ���ʹ��InitLibEnv ���õĴ��ھ۱����ţ�
//����ֵ���ɹ�����true,ʧ�ܷ���flase
PUBLISH_MODULE_API bool   rtmpPushFileToServerBegin(const char* szLocalFile,char* szRtmpServerURL,int nUseDrawToolType,HWND hShowHwnd);


//����:�����������ͺͲ����ļ�
//������szLocalFile ��������Ƶ�ļ�
PUBLISH_MODULE_API bool   rtmpPushFileToServerEnd(const char* szLocalFile);

//���ܣ������л�
//��ǰ���ڲ��ŵ��ļ�����
PUBLISH_MODULE_API bool   rtmpPushFileSwitch(const char* szCurPlayLocalFileName);

//�ƶ������ļ���λ��
//������szLocalFile ��������Ƶ�ļ�
//		nSeekTime   seek�����ļ�λ��
//����ֵ�����ŵ�ʱ���ܳ��ȵ�λ��
PUBLISH_MODULE_API bool rtmpPushFileSeek(const char* szLocalFile,unsigned int nSeekTime);

//��ͣ/�ָ�
//������szLocalFile ��������Ƶ�ļ�
//		bIsPause   ��ͣ(true)/�ָ�(flase)
PUBLISH_MODULE_API bool rtmpPushFilePause(const char* szLocalFile,bool bIsPause);

//ȡ���ļ��ĵ�ǰ���ŵ�ʱ��
//������szLocalFile ��������Ƶ�ļ�
//����ֵ�����ŵ�ʱ���ܳ��ȵ�λ��
PUBLISH_MODULE_API unsigned int getPushFileCurPlayTime(const char* szLocalFile);

//ȡ���ļ����ܵĲ���ʱ�䳤��
//������szLocalFile ��������Ƶ�ļ�
//����ֵ�����ŵ�ʱ���ܳ��ȵ�λ��
PUBLISH_MODULE_API unsigned int getPushFileDuration(const char* szLocalFile);


//��ʼ�汾�����͵����ݵ����ص�FLV�ļ�
//������szLocalFile ������ļ�·��
//      hRecordWindow Ҫ��¼�ĵľ������
PUBLISH_MODULE_API bool  savePushDataToLocalFlvFileBegin(const char* szLocalFile,HWND hRecordWindow);

//���������ļ�
PUBLISH_MODULE_API bool  savePushDataToLocalFlvFileEnd();

//���ñ�����˷�ľ���
//������ vol true��ʾ�������� false ��ʾ�رվ���
PUBLISH_MODULE_API bool setMicMute(bool vol);

//���ñ�����˷������
//������ vol��Χ:1--100
PUBLISH_MODULE_API bool setMicVolume(long vol);

//��ȡ������˷������
//������ vol��Χ:1--100
PUBLISH_MODULE_API long getMicVolume();

//�õ�Micʵʱ����
PUBLISH_MODULE_API int GetMicRTVolum();

//��ȡ�������������ŵ�����
//������vol��ʾ������С��0-100��
PUBLISH_MODULE_API long getSpeakersVolume();

//���ñ������������ŵľ���
//������ vol true��ʾ�������� false ��ʾ�رվ���
PUBLISH_MODULE_API bool setSpeakersMute(bool vol);

//���ñ������������ŵ�����
//������vol��ʾ������С��0-100��
PUBLISH_MODULE_API bool setSpeakersVolume(long vol);

//��ȡ��˷��豸��״̬
PUBLISH_MODULE_API MDevStatus getMicDevStatus();

//��ȡ��˷�״̬
PUBLISH_MODULE_API MDevStatus getCamDevStatus();

//��ȡ������״̬
PUBLISH_MODULE_API MDevStatus getSpeakersDevStatus();

//��������
PUBLISH_MODULE_API  bool PreviewSound(int nMicID,bool bIsPreview);

//���ûص�����
PUBLISH_MODULE_API bool SetEventCallback(scEventCallBack pEC,void* dwUser);

//ת���ļ�
PUBLISH_MODULE_API bool TrancodeFile(const char* szSourFileName,const char* szDestFileName,char* szErr);

//ת���ļ�
PUBLISH_MODULE_API bool StopTrancodeFile();
#endif