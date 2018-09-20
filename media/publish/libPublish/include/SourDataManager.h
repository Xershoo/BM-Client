#ifndef SOURDATAMANAGER_H
#define SOURDATAMANAGER_H
#include "SourHeader.h"

class CSourDataManager
{
public:
	CSourDataManager();
	~CSourDataManager();
public:
	//���ܣ���ȡ��ǰ�豸����ͷ�豸��Ϣ
	long GetDevCameraCount();
	bool GetDevCameraName(long nIndex,char szName[256]);
	//����:��ȡ��ǰ�豸��˷���Ŀ
	long   GetDevMicCount();
    bool   GetDevMicName(int nMicID,char szName[256]);

	//����:��ȡ��ǰ�豸��˷���Ŀ
	long   GetDevSpeakerCount();
    bool   GetDevSpeakerName(int nSpeakerID,char szName[256]);
	bool   SetDevSpeaker(int index);

	//���ܣ�����ͷ����γ���Ϣ
	bool   DeviceChangeMsg(DeviceType nDeviceType,DeviceState nDeviceState,char nDeviceName[256],int DeviceID);

	ISourData* getSourData(int nMediaType,int nIndex);

private:
	bool InitDevSourManager();
	bool UninitDevSourManager();
	void clearSour();
private:
	list<SourceNode>  m_listSour;
	int  m_nCamNum;
};

#endif