#pragma once

#ifdef VOICE_ENGINE_EXPORTS
#define VOIVE_ENGINE_API __declspec(dllexport)
#else
#define  VOIVE_ENGINE_API // __declspec(dllimport)
#endif


namespace voice_engine
{
	struct VData
	{
		UINT64		rid;	// �Խ�ɫid��Ϊ����
		UINT32		seq;
		UINT8		type;
		const char*	payload;
		size_t		payloadSize;
#ifdef LOG_VOICE_DELAY
		DWORD	time_begin;
		DWORD	time_send;
		DWORD	time_recv;
#endif
	};

	static const int MAX_DEVICE_NUM = 100;
	static const int MAX_DEVICE_NAME_LEN = 256;
	struct DeviceList
	{
		int size;
		wchar_t nameArray[MAX_DEVICE_NUM][MAX_DEVICE_NAME_LEN];
	};

	struct AudioInfo
	{
		char title[30];  //����
		char artist[30]; //������
		char album[30];  //ר��
		int  year;       //���
		char comment[28];//��ע

		AudioInfo()
		{
			memset(title,0,sizeof(title));
			memset(artist,0,sizeof(artist));
			memset(album,0,sizeof(album));
			year = 0;
			memset(comment,0,sizeof(comment));
		}
	};

	enum CallBackType
	{
		VOICE_DATA = 0,
		MIC_VOLUME_SHOW,
		SPK_VOLUME_SHOW
	};

	typedef void (CALLBACK * PVoiceEngineCallback)(CallBackType, const void*, int, void* arg);


	struct IVoiceEngine
	{
		virtual bool Start(UINT64 uid, PVoiceEngineCallback cb, void* cbarg) = 0;
		virtual void Stop() = 0;
		virtual void RecvData(VData&) = 0;

		virtual DeviceList GetInDevList() = 0;
		virtual DeviceList GetOutDevList() = 0;

		// ����
		virtual void SetAdjust(bool isAdjust) = 0;
		virtual bool Adjust() = 0;

		// ����OK
		virtual void SetKaraoke(bool bKaraoke) = 0;
		virtual bool Karaoke() = 0;

		// �㲥
		virtual void SetBroadcast(bool bBroadcast) = 0;
		virtual bool Broadcast() = 0;

		// ����
		virtual void SetInVolume(int vol) = 0;
		virtual void SetInMute(bool ismute) = 0;
		virtual void SetOutVolume(int vol) = 0;

		// �豸
		virtual void SetInDevice(int index) = 0;
		virtual void SetOutDevice(int index) = 0;

		// ������ֵ
		virtual void SetActiveVolume(int vol) = 0;
		virtual int ActiveVolume() = 0;

		// ˵����ʽ�����ɡ�������
		virtual void SetKeySpeak(bool isKeySpeak) = 0;
		virtual void SetActiveKey(int key) = 0;

		// ��������
		virtual void SetQuality(int qa) = 0;	// 1-10,speex
		virtual void SetNs(bool isNs) = 0;		// ����
		virtual void SetEc(bool isEc) = 0;      //��������

		virtual void SetHighQuality(bool bHigh) = 0; //�ߵ�����

		// mic��ǿ
		virtual void SetMicEnhance(bool isEnchance) = 0;

		//¼�����ļ�
		virtual bool StartRecordFile(TCHAR* szFileName,AudioInfo* info) = 0;
		virtual bool StopRecordFile() = 0;

		//�Ƿ���룬��Ƶֱ����
		virtual void SetVoiceDataEncoded(bool isEncoded) = 0;

		virtual void Release() = 0;
	};


	VOIVE_ENGINE_API IVoiceEngine* CreateVoiceEngine(int nSample);
}