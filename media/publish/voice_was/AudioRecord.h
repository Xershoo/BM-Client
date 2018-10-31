#ifndef _AUDIO_RECORD_H_
#define _AUDIO_RECORD_H_
#include "AudioImplBase.h"

class CAudioRender;
class CPlaybackRecord;

class CAudioRecord : public CAudioImplBase
{
public:
	CAudioRecord(IMMDevice* pDevice);
	virtual ~CAudioRecord();

	void setKarokRecord(CPlaybackRecord* recordPlayback);
protected:
	virtual UINT doAuidoImpl();

protected:
	virtual bool open();
	virtual void close();

	virtual void setVolume(int volume);	//��������
	virtual int  getVolume();			//��ȡ����

	virtual void setPreview(bool preview,int spkIndex);
	virtual void setAec(bool useAec);		//���û�������
protected:
	bool initAecAudioRecord();
	void doAecAudioRecord();
	void doAudioRecord();

	bool openAudioRender();

protected:
	void renderPreviewData(BYTE* pbData, int nSize);

protected:
	CAudioRender*		m_audioRender;
	CPlaybackRecord*	m_recordPlayback;
};

#endif