#ifndef SOURDATAMANAGER_H
#define SOURDATAMANAGER_H
#include "SourHeader.h"

class CSourDataManager
{
public:
	CSourDataManager();
	~CSourDataManager();
public:
	//功能：获取当前设备摄像头设备信息
	long GetDevCameraCount();
	bool GetDevCameraName(long nIndex,char szName[256]);
	//功能:获取当前设备麦克风数目
	long   GetDevMicCount();
    bool   GetDevMicName(int nMicID,char szName[256]);

	//功能:获取当前设备麦克风数目
	long   GetDevSpeakerCount();
    bool   GetDevSpeakerName(int nSpeakerID,char szName[256]);
	bool   SetDevSpeaker(int index);

	//功能：摄像头插入拔出消息
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