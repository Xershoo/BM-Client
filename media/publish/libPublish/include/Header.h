#ifndef MEDIAHEADER_H
#define MEDIAHEADER_H

#define SOURCECAMERA 0x01   //����ͷ��Ƶ
#define SOURCEDEVAUDIO 0x08 //��Ƶ

struct StreamParam
{
	int nSelectCameraID;//ѡ���id(0 <= id < GetDevCameraCount())
	int nVideoW;        //��Ƶ��
	int nVideoH;		//��Ƶ��
	int nVideoBitRate;	//��Ƶ����
	int nVideoFps;		//��Ƶ֡��

	int bAudioKaraoke; 
	int nAudioBitRate; //��Ƶ����
};

//����:��ȡ��ǰ�豸����ͷ��Ŀ
long   GetDevCameraCount();

//���ܣ���ȡ���ID��Ϣ����ͷ������
//����: nCameraID id(0 <= id < GetDevCameraCount())
//		szName ��ȡ��������ͷ�����������
bool   GetDevCameraName(int nCameraID,char szName[256]);

//����:rtmp ��ʼ��������������
//����:szRtmpPushUrl �������ĵ�ַ
//	   nStreamType �����������ͣ�
//     nRoleType ��ɫ������
//     Param ����صĲ���
//     hShowHwnd ������Ƶ����ʾ���ھ��
bool   rtmp_PushStreamToServer_begin(const char* szRtmpPushUrl,int nStreamType,StreamParam Param,HWND hShowHwnd);

//����:rtmp ������������������
//����:szRtmpPushUrl �������ĵ�ַ
bool   rtmp_PushStreamToServe_end(const char* szRtmpPushUrl);

#endif